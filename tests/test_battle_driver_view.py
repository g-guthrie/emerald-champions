"""The per-turn driver shows the player only what the battle has shown them.

Covers the pending-action leak (the opposing latch, AI timing, opposing
selection progress and slot-mismatched opposing PP stay out of the printed
state), the compact --brief view, --level-delta parsing, and the witness
replay's command reconstruction. No ROM is needed; boards are synthetic views.
"""
import json
import sys
import unittest
from pathlib import Path
from types import SimpleNamespace

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'scripts'))
sys.path.insert(0, str(ROOT / 'scripts' / 'playthrough'))
import battle_driver as driver


class FakeSession:
    def __init__(self, constants, meta=None):
        self.constants = constants
        self.meta = meta if meta is not None else {}

    def constants_table(self):
        return self.constants


def value(constants, table, name):
    return constants[table]['values'][name]


def board(constants, *, latch=(0, 2, 0), opposing_selection=4):
    """A doubles await_action view: Sylveon/Pachirisu against Poochyena/Zigzagoon."""
    words = [0] * driver.VIEW_WORDS
    words[0], words[1], words[3], words[13] = 3, driver.PHASES.index('await_action'), 2, 4
    words[4] = value(constants, 'battletype', 'BATTLE_TYPE_DOUBLE') \
        if 'values' in constants['battletype'] else constants['battletype']['BATTLE_TYPE_DOUBLE']
    words[8] = 1  # the engine is asking battler 0
    words[22], words[23] = 14, 2
    mons = [('SPECIES_SYLVEON', 14, 44, 63, True, ['MOVE_HYPER_VOICE', 'MOVE_PSYSHOCK'], 'ITEM_CHOICE_SPECS'),
            ('SPECIES_POOCHYENA', 21, 26, 52, False, ['MOVE_CRUNCH', 'MOVE_TAUNT'], 'ITEM_FOCUS_SASH'),
            ('SPECIES_PACHIRISU', 14, 32, 53, True, ['MOVE_FOLLOW_ME', 'MOVE_PROTECT'], 'ITEM_LEFTOVERS'),
            ('SPECIES_ZIGZAGOON', 21, 66, 66, False, ['MOVE_PROTECT'], 'ITEM_FIGY_BERRY')]
    for index, (species, level, hp, max_hp, player, moves, item) in enumerate(mons):
        base = driver.BATTLER_BASE + index * driver.BATTLER_SIZE
        words[base] = value(constants, 'species', species)
        words[base + 1], words[base + 2], words[base + 3] = level, hp, max_hp
        words[base + 5] = value(constants, 'item', item)
        words[base + 7] = index // 2
        words[base + 8] = 1 | (4 | 8 if player else 0)
        for stat in range(8):
            words[base + 9 + stat] = 6
        for slot, move in enumerate(moves):
            words[base + 17 + slot] = value(constants, 'move', move)
            words[base + 21 + slot] = 9 + slot
        words[base + 27] = value(constants, 'type', 'TYPE_NORMAL')
        if not player:
            action, move_index, target = latch
            words[base + 26] = action | (move_index << 8) | (target << 16)
        legal = driver.LEGAL_BASE + index * driver.LEGAL_SIZE
        if player:
            for slot in range(len(moves)):
                words[legal + 1 + slot] = 0b1010 | (1 << 8)  # "selected": foes 1 and 3
            words[legal + 5] = 0x100 | 0b111100
    words[31] = 2 | (opposing_selection << 8) | (1 << 16) | (1 << 24)
    words[9], words[10], words[11] = 60, 6, 0
    return words


class DriverViewTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.constants = driver.build_constants()

    def decoded(self, **kwargs):
        words = board(self.constants, **kwargs)
        return words, driver.decode_state(FakeSession(self.constants), words)

    def test_state_never_publishes_the_opposing_pending_action(self):
        words, state = self.decoded()
        foes = [a for a in state['actives'] if a['side'] == 'opponent']
        mine = [a for a in state['actives'] if a['side'] == 'player']
        self.assertTrue(foes and mine)
        for foe in foes:
            self.assertNotIn('choosing', foe)
            self.assertTrue(all('pp' not in move for move in foe['moves']))
        self.assertTrue(all('choosing' in active for active in mine))
        self.assertEqual(state['selection_state'][1], None)
        self.assertEqual(state['selection_state'][3], None)
        self.assertEqual(state['selection_state'][0], 2)
        for key in ('ai_decision_frames', 'ai_setup_frames', 'ai_delay_frames', 'seed'):
            self.assertNotIn(key, state)
        # The latch is still readable for the post-turn audit record only.
        latch = driver.opponent_latch(words)
        self.assertEqual(latch['1'], {'action': 'use_move', 'move_index': 2, 'target': 0, 'selection': 4})
        self.assertEqual(driver.ai_timing(words)['ai_decision_frames'], 60)

    def test_printed_state_is_identical_whatever_the_ai_committed(self):
        _, first = self.decoded(latch=(0, 1, 2), opposing_selection=4)
        _, second = self.decoded(latch=(2, 3, 0), opposing_selection=1)
        self.assertEqual(json.dumps(first, sort_keys=True), json.dumps(second, sort_keys=True))

    def test_private_act_fields_are_ignored_by_the_witness_comparison(self):
        words = board(self.constants)
        private = set(driver.ai_timing(words)) | {'audit_pre_commit_opponent_latch'}
        self.assertTrue(private <= driver.REPLAY_IGNORED)
        self.assertFalse({'submitted', 'chosen', 'hp_after', 'messages', 'outcome'} & driver.REPLAY_IGNORED)

    def test_brief_view_is_compact_and_player_facing(self):
        _, state = self.decoded(latch=(0, 1, 2))
        session = FakeSession(self.constants, {'trainer_a': 'TRAINER_CALVIN_1', 'battle_kind': 'trainer'})
        driver.update_tracking(session, None, state, {'messages': [], 'popups': []}, turn_resolved=0)
        text = driver.render_brief(session, state, ['The opposing Poochyena\nused~Crunch!'], heading='Resolved turn 1')
        self.assertIn('The opposing Poochyena used Crunch!', text)
        self.assertIn('[0] YOU Sylveon L14 44/63', text)
        self.assertIn('[1] FOE Poochyena L21 50%', text)       # foes as a percentage
        self.assertIn('item ?, ability ?', text)               # nothing revealed yet
        self.assertNotIn('Focus Sash', text)
        self.assertIn('move0 Hyper Voice 9pp @1/3', text)
        self.assertIn('Commands (one act call', text)
        for hidden in ('choosing', 'move_index', 'selection', 'ai_', 'seed'):
            self.assertNotIn(hidden, text)
        self.assertLess(len(text), 2600)

    def test_tracking_reads_reveals_volatiles_hazards_and_field_turns(self):
        _, state = self.decoded()
        session = FakeSession(self.constants, {'trainer_a': 'TRAINER_CALVIN_1'})
        driver.update_tracking(session, None, state, {'messages': [], 'popups': []}, turn_resolved=0)
        after = json.loads(json.dumps(state))
        after['turn'] = 3
        after['sides']['opponent'] = ['SIDE_STATUS_TAILWIND']
        log = {'messages': ['The opposing Poochyena hung\non using its Focus~Sash!',
                            'The opposing Zigzagoon\nbecame confused!',
                            'Pointed stones float in the air on the opposing side!'],
               'popups': [{'battler': 3, 'item': 'ITEM_FIGY_BERRY', 'message_index': 0}]}
        driver.update_tracking(session, state, after, log, turn_resolved=2)
        brief = session.meta['brief']
        self.assertEqual(brief['revealed']['opponent_a:0'], {'item': 'ITEM_FOCUS_SASH'})
        self.assertEqual(brief['revealed']['opponent_a:1'], {'item': 'ITEM_FIGY_BERRY'})
        self.assertEqual(brief['volatiles']['opponent_a:1'], ['confused'])
        self.assertEqual(brief['hazards']['opponent'], {'Stealth Rock': 1})
        text = driver.render_brief(session, after, [], heading='Resolved turn 2')
        self.assertIn('Tailwind[foe] since T2 (nominal 4t)', text)
        self.assertIn('Stealth Rock[foe]', text)
        self.assertIn('item Focus Sash', text)
        self.assertIn('confused', text)
        # A switch clears the leaving Pokemon's volatiles and entry turn.
        later = json.loads(json.dumps(after))
        later['actives'][3]['party_slot'] = 2
        later['actives'][3]['species'] = 'SPECIES_HOOTHOOT'
        driver.update_tracking(session, after, later, {'messages': [], 'popups': []}, turn_resolved=3)
        self.assertNotIn('opponent_a:1', session.meta['brief']['volatiles'])
        self.assertEqual(session.meta['brief']['entered']['opponent_a:2'], 3)

    def test_level_delta_spec(self):
        self.assertEqual(driver.parse_level_delta('A0=+2,A3=-1,B1=+1'),
                         [2, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, 0])
        self.assertEqual(driver.parse_level_delta(None), [0] * 12)
        # Explicit members override a wildcard whatever the order.
        for spec in ('A*=+1,A0=-2', 'A0=-2,A*=+1'):
            self.assertEqual(driver.parse_level_delta(spec), [-2, 1, 1, 1, 1, 1] + [0] * 6)
        self.assertEqual(driver.level_delta_labels(driver.parse_level_delta('B*=3,B5=0')),
                         {'B0': 3, 'B1': 3, 'B2': 3, 'B3': 3, 'B4': 3})
        for bad in ('A6=+1', 'C0=+1', 'A0=+1,A0=+2', 'A0=+120', 'A0+1'):
            with self.assertRaises(SystemExit):
                driver.parse_level_delta(bad)

    def test_ability_trial_spec_resolves_native_names(self):
        abilities = self.constants['ability']['values']
        values = driver.parse_ability_trial('A2=ABILITY_INTIMIDATE,B0=snow_warning', self.constants)
        self.assertEqual(values[2], abilities['ABILITY_INTIMIDATE'])
        self.assertEqual(values[6], abilities['ABILITY_SNOW_WARNING'])
        self.assertEqual(sum(1 for v in values if v), 2)
        self.assertEqual(driver.parse_ability_trial(None, self.constants), [0] * 12)
        self.assertEqual(driver.ability_trial_labels(values, self.constants),
                         {'A2': 'ABILITY_INTIMIDATE', 'B0': 'ABILITY_SNOW_WARNING'})
        for bad in ('A2=ABILITY_NOT_A_REAL_ABILITY', 'A2=ABILITY_NONE', 'A6=ABILITY_INTIMIDATE',
                    'A*=ABILITY_INTIMIDATE', 'A2=ABILITY_INTIMIDATE,A2=ABILITY_DROUGHT', 'A2'):
            with self.assertRaises(SystemExit):
                driver.parse_ability_trial(bad, self.constants)

    def test_rejected_trials_name_the_member(self):
        values = driver.parse_ability_trial('A2=ABILITY_INTIMIDATE,B0=ABILITY_DROUGHT', self.constants)
        self.assertEqual(driver.rejected_trials(0, values, self.constants), [])
        self.assertEqual(driver.rejected_trials((1 << 2) | (1 << 6), values, self.constants),
                         ['A2 cannot take ABILITY_INTIMIDATE', 'B0 cannot take ABILITY_DROUGHT'])

    def test_identity_audit_expects_the_runs_own_deltas_and_trials(self):
        expected = {'trainer_id': 'TRAINER_CALVIN_1', 'team': [
            {'slot': 1, 'species': 'SPECIES_POOCHYENA', 'level': 21, 'ability': 'ABILITY_RATTLED'},
            {'slot': 2, 'species': 'SPECIES_ZIGZAGOON', 'level': 99, 'ability': 'ABILITY_GLUTTONY'}]}
        roster = {'owners': [{'owner': 'A', 'trainer_id_native': 7, 'team': []},
                             {'owner': 'B', 'trainer_id_native': 0, 'team': []}]}
        meta = {'level_delta': {'spec': 'A0=+3,A1=+5'},
                'ability_trial': {'applied': {'A1': 'ABILITY_PICKUP'}}}
        adjusted = driver.expected_with_trials(expected, roster, {'TRAINER_CALVIN_1': 7}, meta)
        self.assertEqual([(m['level'], m['ability']) for m in adjusted['team']],
                         [(24, 'ABILITY_RATTLED'), (100, 'ABILITY_PICKUP')])
        self.assertEqual(expected['team'][0]['level'], 21)  # the certificate itself is untouched
        self.assertIs(driver.expected_with_trials(expected, roster, {}, {}), expected)

    def test_trial_ability_stays_hidden_until_revealed(self):
        _, state = self.decoded()
        trial = 'ABILITY_INTIMIDATE'
        state['actives'][1]['ability'] = trial
        session = FakeSession(self.constants, {'trainer_a': 'TRAINER_CALVIN_1'})
        driver.update_tracking(session, None, state, {'messages': [], 'popups': []}, turn_resolved=0)
        self.assertNotIn('Intimidate', driver.render_brief(session, state, [], heading='Start'))
        driver.update_tracking(session, state, state, {'messages': [], 'popups': [
            {'battler': 1, 'ability': trial, 'message_index': 0}]}, turn_resolved=0)
        self.assertIn('ability Intimidate', driver.render_brief(session, state, [], heading='Start'))

    def test_witness_commands_round_trip_through_the_act_parser(self):
        submitted = {'0': {'action': 'move', 'index': 1, 'move': 'MOVE_PSYSHOCK', 'target': 3, 'mega': True},
                     '2': {'action': 'switch', 'slot': 4},
                     '1': {'action': 'move', 'move': 'MOVE_STRUGGLE', 'mega': False}}
        commands = driver.commands_of(submitted)
        self.assertEqual(commands, ['0:move1@3,mega', '2:switch4', '1:struggle'])
        self.assertTrue(all(driver.COMMAND_RE.match(command) for command in commands))

    def test_replay_rebuilds_start_arguments_for_runs_before_they_were_recorded(self):
        meta = {'battle_kind': 'trainer', 'trainer_a': 'TRAINER_CALVIN_1', 'trainer_b': None,
                'partner': 'PARTNER_NONE', 'difficulty': 'hard', 'level_cap': 14,
                'script_pairing': None, 'map_field': {'weather_basis': 'map header'}}
        arguments, basis, party, scenario = driver.replay_start_args(meta, Path('/nonexistent'), Path('/nonexistent'))
        self.assertEqual((arguments['trainer'], arguments['difficulty'], arguments['cap'], arguments['weather']),
                         ('TRAINER_CALVIN_1', 'hard', 14, 'map'))
        self.assertIsNone(party)
        self.assertIn('rebuilt', basis)
        recorded = {**meta, 'start_args': {'battle_kind': 'trainer', 'trainer': 'TRAINER_CALVIN_1',
                                           'difficulty': 'hard', 'cap': 14, 'level_delta': 'A0=+3',
                                           'weather': None}}
        arguments, basis, _, _ = driver.replay_start_args(recorded, Path('/nonexistent'), Path('/nonexistent'))
        self.assertEqual((arguments['level_delta'], arguments['weather']), ('A0=+3', 'map'))
        start = SimpleNamespace(**arguments, seed=1)
        self.assertEqual(driver.parse_level_delta(start.level_delta)[0], 3)


if __name__ == '__main__':
    unittest.main()
