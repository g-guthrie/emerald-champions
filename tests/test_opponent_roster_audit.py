"""Exact native opponent inputs must match the source regional scenario."""
import copy
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'scripts/playthrough'))
import battle_driver


class OpponentAuditTests(unittest.TestCase):
    def test_real_constants_builder_decodes_native_nature_and_roster(self):
        constants = battle_driver.build_constants()
        self.assertEqual(constants['schema'], battle_driver.CONSTANTS_SCHEMA)
        self.assertEqual(constants['nature']['values']['NATURE_TIMID'], 10)
        self.assertEqual(constants['nature']['names']['0'], 'NATURE_HARDY')
        self.assertEqual(len(constants['nature']['names']), 25)
        self.assertNotIn('NATURE_RANDOM', constants['nature']['values'])
        words = [0] * battle_driver.ROSTER_WORDS
        species = constants['species']['values']['SPECIES_PIPLUP']
        words[:9] = [1, 1, 0, 4, 2, species, 5,
                     constants['trainers']['TRAINER_MAY_ROUTE_103_TORCHIC'], 0]
        words[9:31] = [species, 14, constants['item']['values']['ITEM_SITRUS_BERRY'],
                       constants['ability']['values']['ABILITY_TORRENT'],
                       constants['nature']['values']['NATURE_TIMID'], 255,
                       252, 4, 20, 120, 40, 64, 31, 0, 31, 31, 31, 0,
                       constants['move']['values']['MOVE_SCALD'],
                       constants['move']['values']['MOVE_ICY_WIND'],
                       constants['move']['values']['MOVE_HELPING_HAND'], 0]
        roster = battle_driver.decode_opponent_roster(words, constants)
        self.assertEqual(roster['owners'][0]['team'][0]['nature'], 'NATURE_TIMID')
        self.assertEqual(roster['owners'][0]['team'][0]['species'], 'SPECIES_PIPLUP')

    def fixture(self):
        def table(names):
            return {'names': {str(index): name for index, name in names.items()}}
        constants = {
            'species': table({0: 'SPECIES_NONE', 1: 'SPECIES_PIPLUP', 2: 'SPECIES_PIKACHU', 3: 'SPECIES_MUDKIP'}),
            'item': table({1: 'ITEM_SITRUS_BERRY'}),
            'ability': table({1: 'ABILITY_TORRENT'}),
            'nature': table({3: 'NATURE_TIMID'}),
            'move': table({0: 'MOVE_NONE', 1: 'MOVE_SCALD', 2: 'MOVE_ICY_WIND', 3: 'MOVE_HELPING_HAND'}),
        }
        words = [0] * battle_driver.ROSTER_WORDS
        words[:9] = [1, 1, 0, 4, 2, 1, 5, 17, 0]
        words[9:31] = [1, 14, 1, 1, 3, 255, 252, 4, 20, 120, 40, 64,
                        31, 0, 31, 31, 31, 0, 1, 2, 3, 0]
        roster = battle_driver.decode_opponent_roster(words, constants)
        expected = {'trainer_id': 'TRAINER_RIVAL', 'generation': 4, 'unchosen_index': 2,
                    'unchosen_species': 'SPECIES_PIPLUP', 'team': copy.deepcopy(roster['owners'][0]['team'])}
        expected['team'][0]['moves'].pop()  # Authored rows may omit trailing MOVE_NONE.
        return words, constants, roster, expected

    def test_native_full_loadout_matches_export_without_exposing_it_as_player_state(self):
        _, _, roster, expected = self.fixture()
        battle_driver.verify_opponent_identity(expected, roster, {'TRAINER_RIVAL': 17})
        self.assertEqual(roster['owners'][0]['team'][0]['evs'], [252, 4, 20, 120, 40, 64])
        self.assertEqual(roster['owners'][0]['team'][0]['ivs'], [31, 0, 31, 31, 31, 0])

    def test_hoenn_opponent_cannot_masquerade_as_sinnoh_scenario(self):
        _, _, roster, expected = self.fixture()
        roster['owners'][0]['team'][0]['species'] = 'SPECIES_MUDKIP'
        with self.assertRaisesRegex(SystemExit, 'species mismatch'):
            battle_driver.verify_opponent_identity(expected, roster, {'TRAINER_RIVAL': 17})

    def test_native_loadout_differences_are_not_reduced_to_species_only(self):
        _, _, roster, expected = self.fixture()
        for key, changed in [('level', 15), ('item', 'ITEM_EVIOLITE'), ('ability', 'ABILITY_DEFIANT'),
                             ('nature', 'NATURE_JOLLY'), ('friendship', 0), ('evs', [0] * 6),
                             ('ivs', [0] * 6), ('moves', ['MOVE_PROTECT'] * 4)]:
            wrong = copy.deepcopy(roster)
            wrong['owners'][0]['team'][0][key] = changed
            with self.assertRaisesRegex(SystemExit, key + ' mismatch'):
                battle_driver.verify_opponent_identity(expected, wrong, {'TRAINER_RIVAL': 17})

    def test_unversioned_or_wrong_count_mailbox_is_refused(self):
        words, constants, _, _ = self.fixture()
        wrong = list(words); wrong[0] = 0
        with self.assertRaises(SystemExit):
            battle_driver.decode_opponent_roster(wrong, constants)
        wrong = list(words); wrong[1] = 2
        with self.assertRaisesRegex(SystemExit, 'count disagrees'):
            battle_driver.decode_opponent_roster(wrong, constants)

    def test_default_generation_matches_native_hoenn_fallback(self):
        words, constants, _, _ = self.fixture()
        words[3], words[5], words[9] = 0, 3, 3
        roster = battle_driver.decode_opponent_roster(words, constants)
        self.assertEqual(roster['starter_generation_raw'], 0)
        self.assertEqual(roster['generation'], 3)
        self.assertEqual(roster['unchosen_species'], 'SPECIES_MUDKIP')

    def test_every_native_opposing_owner_requires_a_certificate(self):
        _, _, roster, expected = self.fixture()
        roster['owners'][1] = {'owner': 'B', 'trainer_id_native': 18, 'team': copy.deepcopy(roster['owners'][0]['team'])}
        with self.assertRaisesRegex(SystemExit, 'every native opposing owner'):
            battle_driver.verify_opponent_identity(expected, roster, {'TRAINER_RIVAL': 17})
        second = {**copy.deepcopy(expected), 'trainer_id': 'TRAINER_SECOND'}
        battle_driver.verify_opponent_identity([expected, second], roster, {'TRAINER_RIVAL': 17, 'TRAINER_SECOND': 18})


if __name__ == '__main__':
    unittest.main()
