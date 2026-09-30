"""Concrete campaign wild formats and input gates, without native fabrication."""
import copy
from pathlib import Path
import sys
import types
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'scripts/playthrough'))
sys.path.insert(0, str(ROOT / 'tools/agent_player'))
import battle_driver as driver
import battle_calibration as calibration
import generate_battle_suite as suite


class ScriptedWildTests(unittest.TestCase):
    def fixture(self, kind='birth_island_deoxys'):
        constants = driver.build_constants()
        flags = constants['battletype']
        mon = {'slot': 1, 'species': 'SPECIES_DEOXYS', 'level': 80,
               'item': 'ITEM_LIFE_ORB', 'ability': 'ABILITY_PRESSURE',
               'nature': 'NATURE_TIMID', 'friendship': 70,
               'evs': [4, 0, 0, 252, 0, 252], 'ivs': [31] * 6,
               'moves': ['MOVE_PSYCHIC_TERRAIN', 'MOVE_EXPANDING_FORCE', 'MOVE_FOCUS_BLAST', 'MOVE_PROTECT']}
        team = [mon]
        native_flags = flags['BATTLE_TYPE_LEGENDARY']
        if kind == 'birch_rescue':
            team = [dict(mon, species='SPECIES_POOCHYENA', level=2),
                    dict(mon, slot=2, species='SPECIES_ZIGZAGOON', level=2)]
            native_flags = flags['BATTLE_TYPE_FIRST_BATTLE'] | flags['BATTLE_TYPE_DOUBLE']
        roster = {'battle_type_native': native_flags,
                  'owners': [{'owner': 'A', 'team': team}, {'owner': 'B', 'team': []}]}
        expected = {'battle_kind': kind, 'allowed_teams': [copy.deepcopy(team)]}
        return expected, roster, constants

    def test_actual_constants_include_wild_formats(self):
        flags = driver.build_constants()['battletype']
        self.assertGreater(flags['BATTLE_TYPE_FIRST_BATTLE'], 0)
        self.assertGreater(flags['BATTLE_TYPE_LEGENDARY'], 0)
        self.assertNotEqual(flags['BATTLE_TYPE_FIRST_BATTLE'], flags['BATTLE_TYPE_LEGENDARY'])

    def test_concrete_format_and_entire_native_loadout_identify_variant(self):
        expected, roster, constants = self.fixture()
        other = copy.deepcopy(expected['allowed_teams'][0])
        other[0]['item'] = 'ITEM_FOCUS_SASH'
        expected['allowed_teams'].insert(0, other)
        self.assertEqual(driver.verify_scripted_wild_identity(expected, roster, constants),
                         {'observed_variant': 1, 'variant_count': 2})
        for key, value in [('level', 100), ('nature', 'NATURE_ADAMANT'), ('ivs', [0] * 6),
                           ('moves', ['MOVE_TACKLE'] * 4)]:
            wrong = copy.deepcopy(roster)
            wrong['owners'][0]['team'][0][key] = value
            with self.assertRaisesRegex(SystemExit, 'source-legal variant'):
                driver.verify_scripted_wild_identity(expected, wrong, constants)

    def test_rescue_is_first_battle_doubles_and_deoxys_is_single(self):
        for kind in ('birch_rescue', 'birth_island_deoxys'):
            expected, roster, constants = self.fixture(kind)
            driver.verify_scripted_wild_identity(expected, roster, constants)
            roster['battle_type_native'] |= constants['battletype']['BATTLE_TYPE_IS_MASTER']
            driver.verify_scripted_wild_identity(expected, roster, constants)
            for flag in ('BATTLE_TYPE_TRAINER', 'BATTLE_TYPE_MULTI', 'BATTLE_TYPE_DOUBLE'):
                wrong = copy.deepcopy(roster)
                wrong['battle_type_native'] ^= constants['battletype'][flag]
                with self.assertRaises(SystemExit):
                    driver.verify_scripted_wild_identity(expected, wrong, constants)

    def test_missing_or_ambiguous_source_variant_is_refused(self):
        expected, roster, constants = self.fixture()
        for teams in ([], expected['allowed_teams'] * 2):
            expected['allowed_teams'] = teams
            with self.assertRaises(SystemExit):
                driver.verify_scripted_wild_identity(expected, roster, constants)
        expected, roster, constants = self.fixture()
        del expected['allowed_teams'][0][0]['ivs']
        with self.assertRaisesRegex(SystemExit, 'missing ivs'):
            driver.verify_scripted_wild_identity(expected, roster, constants)

    def test_rescue_cannot_replace_native_starters_with_prepared_party(self):
        args = types.SimpleNamespace(battle_kind='birch_rescue', party='party.json')
        with self.assertRaisesRegex(SystemExit, 'actual native starter-pair factory'):
            driver.command_start(args)

    def test_trainer_catalogue_cannot_certify_wild_native_fixture(self):
        puzzle = {'puzzle_id': 'wild', 'scenario': {'battle_kind': 'birth_island_deoxys',
                  'legality_status': 'proven', 'expected_opponent': {'team': []}}}
        puzzle['content_sha256'] = suite.digest_bytes(suite.canonical(puzzle))
        catalogue = {'schema_version': 2, 'source_fingerprint': 'current', 'puzzles': [puzzle]}
        with patch.object(suite, 'source_fingerprint', return_value='current'):
            with self.assertRaisesRegex(ValueError, 'dedicated source stage producer'):
                calibration.validate_puzzle(catalogue, 'wild', {})


if __name__ == '__main__':
    unittest.main()
