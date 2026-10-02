"""The per-turn driver shows the player only what the battle has shown them.

Covers the pending-action leak (the opposing latch, AI timing, opposing
selection progress and slot-mismatched opposing PP stay out of the printed
state), the compact --brief view, --level-delta parsing, and the witness
replay's command reconstruction. No ROM is needed; boards are synthetic views.
"""
import hashlib
import json
import sys
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

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


class PreparationAvailabilityTests(unittest.TestCase):
    def test_unavailable_party_is_refused_before_native_start(self):
        pool = dict(trainer='TRAINER_BRENDAN_ROUTE_103_MUDKIP', milestone='start', cap=14,
                    source_head='test', excluded_victory_flags=['FLAG_DEFEATED_RIVAL_ROUTE103'],
                    scope='Source upper bound', species=[], items=[], megas=[])
        with tempfile.TemporaryDirectory() as folder:
            party = Path(folder) / 'party.json'
            party.write_text(json.dumps({'encounter': pool['trainer'], 'availability_audit': 'test',
                'party': [{'species': 'SPECIES_NAGANADEL', 'availability': 'Seaspray Cave',
                           'evs': [252, 0, 0, 252, 0, 0]}]}))
            args = SimpleNamespace(trainer=pool['trainer'], cap=14, baseline=None, party=str(party),
                                   battle_kind='trainer', scenario=None, trainer2=None, partner=None)
            with patch('reference_pool.encounter_pool', return_value=pool), patch('battle_driver.Session') as session:
                with self.assertRaisesRegex(SystemExit, 'SPECIES_NAGANADEL is not obtainable'):
                    driver.command_start(args)
                session.assert_not_called()

    def test_experiments_and_historical_replays_do_not_claim_source_legality(self):
        with patch('reference_pool.encounter_pool') as pool:
            experimental = driver.preparation_availability(SimpleNamespace(allow_unverified_party=True))
            historical = driver.preparation_availability(SimpleNamespace(_historical_replay=True))
            self.assertEqual(experimental['status'], 'unverified_experiment')
            self.assertEqual(historical['status'], 'historical_replay')
            pool.assert_not_called()

    def test_cannot_borrow_a_later_trainer_cap_for_early_party_resources(self):
        with patch('reference_pool.encounter_pool') as pool:
            with self.assertRaisesRegex(SystemExit, 'different trainer/player caps'):
                driver.preparation_availability(SimpleNamespace(cap=14, baseline=85))
            pool.assert_not_called()


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

    def replaced(self, state):
        """The same board after battler 1 fainted and party index 2 replaced it."""
        after = json.loads(json.dumps(state))
        after['actives'][1].update(species='SPECIES_ZIGZAGOON', party_slot=2, item='ITEM_LEFTOVERS',
                                   ability='ABILITY_PICKUP', hp=66, max_hp=66, moves=[])
        return after

    def test_a_fainted_foes_reveal_stays_with_that_pokemon(self):
        _, state = self.decoded()
        state['actives'][1]['ability'] = 'ABILITY_RATTLED'
        session = FakeSession(self.constants, {'trainer_a': 'TRAINER_CALVIN_1'})
        driver.update_tracking(session, None, state, {'messages': [], 'popups': []}, turn_resolved=0)
        after = self.replaced(state)
        messages = ['The opposing Poochyena used Crunch!', 'The opposing Poochyena fainted!',
                    'Youngster Calvin sent out Zigzagoon!']
        popups = [{'battler': 1, 'item': 'ITEM_FOCUS_SASH', 'message_index': 1},       # old (value match)
                  {'battler': 1, 'ability': 'ABILITY_INTIMIDATE', 'message_index': 1},  # neither: ordering
                  {'battler': 1, 'ability': 'ABILITY_PICKUP', 'message_index': 3}]     # new (value match)
        driver.update_tracking(session, state, after, {'messages': messages, 'popups': popups}, turn_resolved=1)
        revealed = session.meta['brief']['revealed']
        self.assertEqual(revealed['opponent_a:0'], {'item': 'ITEM_FOCUS_SASH', 'ability': 'ABILITY_INTIMIDATE'})
        self.assertEqual(revealed['opponent_a:2'], {'ability': 'ABILITY_PICKUP'})
        text = driver.render_brief(session, after, [], heading='Resolved turn 1')
        self.assertIn('[1] FOE Zigzagoon L21 100% Normal | item ?, ability Pickup', text)
        self.assertNotIn('Focus Sash', text)

    def test_hp_changes_become_log_lines(self):
        _, state = self.decoded()
        after = json.loads(json.dumps(state))
        after['actives'][0]['hp'] = 28
        after['actives'][3]['hp'] = 41
        log = {'messages': ['The opposing Zigzagoon used Extreme Speed!', 'Pachirisu used Bullet Seed!',
                            'Hit 2 time(s)!'],
               'hp_changes': [
                   {'battler': 0, 'hp_before': 44, 'hp_after': 28, 'message_index': 1,
                    'attacker': 3, 'move': 'MOVE_EXTREME_SPEED', 'cause': 'move'},
                   {'battler': 3, 'hp_before': 66, 'hp_after': 54, 'message_index': 2,
                    'attacker': 2, 'move': 'MOVE_BULLET_SEED', 'cause': 'move'},
                   {'battler': 3, 'hp_before': 54, 'hp_after': 41, 'message_index': 2,
                    'attacker': 2, 'move': 'MOVE_BULLET_SEED', 'cause': 'move'}]}
        lines = driver.narrated(log, state, after)
        self.assertEqual(lines, ['The opposing Zigzagoon used Extreme Speed!',
                                 'Sylveon lost 25% (44\u219228)',
                                 'Pachirisu used Bullet Seed!',
                                 'opposing Zigzagoon lost 38% (2 hits)',
                                 'Hit 2 time(s)!'])
        # A replacement's HP is named after the Pokemon that actually took it.
        replaced = self.replaced(state)
        log = {'messages': ['The opposing Poochyena fainted!', 'Youngster Calvin sent out Zigzagoon!'],
               'hp_changes': [{'battler': 1, 'hp_before': 26, 'hp_after': 0, 'message_index': 0,
                               'cause': 'move', 'attacker': 0, 'move': 'MOVE_HYPER_VOICE'}]}
        self.assertEqual(driver.narrated(log, state, replaced)[0], 'opposing Poochyena lost 50%')

    def test_first_turn_only_moves_explain_the_native_block(self):
        _, state = self.decoded()
        session = FakeSession(self.constants, {'trainer_a': 'TRAINER_CALVIN_1'})
        entry = next(e for e in state['pending_decision'] if e['battler'] == 0)
        state['actives'][0]['moves'][0].update(move='MOVE_FAKE_OUT')
        entry['moves'][0].update(move='MOVE_FAKE_OUT', legal=True, blocked_by=[])
        self.assertIn('move0 Fake Out 9pp @1/3 [first turn out only]',
                      driver.render_brief(session, state, [], heading='Start'))
        # Choice-locked into Fake Out after the first turn: this ROM's own mask
        # blocks every slot, so Struggle (or a switch) is the real game's answer.
        entry['moves'][0].update(legal=False, blocked_by=['MOVE_LIMITATION_UNUSABLE'])
        entry['moves'][1].update(legal=False, blocked_by=['MOVE_LIMITATION_CHOICE_ITEM'])
        entry['must_struggle'] = True
        text = driver.render_brief(session, state, [], heading='Start')
        self.assertIn('0 Sylveon: 0:struggle only', text)
        entry['must_struggle'] = False
        text = driver.render_brief(session, state, [], heading='Start')
        self.assertIn('Fake Out 9pp BLOCKED(unusable: not first turn out)', text)
        self.assertIn('BLOCKED(choice_item)', text)

    def test_log_lines_name_form_changers_form_neutrally(self):
        for species, name in (('SPECIES_AEGISLASH_SHIELD', 'Aegislash'), ('SPECIES_AEGISLASH_BLADE', 'Aegislash'),
                              ('SPECIES_MIMIKYU_BUSTED', 'Mimikyu'), ('SPECIES_PALAFIN_HERO', 'Palafin'),
                              ('SPECIES_MORPEKO_HANGRY', 'Morpeko'), ('SPECIES_EISCUE_NOICE', 'Eiscue'),
                              ('SPECIES_DARMANITAN_GALAR_ZEN', 'Darmanitan'), ('SPECIES_MINIOR_CORE_RED', 'Minior'),
                              ('SPECIES_VIVILLON_ICY_SNOW', 'Vivillon'), ('SPECIES_CHARIZARD_MEGA_X', 'Charizard')):
            self.assertEqual(driver.display_species(species), name)
        # Blade Forme took the knockout hit; the view (read afterwards) shows
        # the Shield Forme it reverted to on fainting.
        _, state = self.decoded()
        state['actives'][1]['species'] = 'SPECIES_AEGISLASH_BLADE'
        after = json.loads(json.dumps(state))
        after['actives'][1].update(species='SPECIES_AEGISLASH_SHIELD', hp=0)
        log = {'messages': ['Sylveon used Shadow Ball!', 'The opposing Aegislash fainted!'],
               'hp_changes': [{'battler': 1, 'hp_before': 26, 'hp_after': 0, 'message_index': 1,
                               'attacker': 0, 'move': 'MOVE_SHADOW_BALL', 'cause': 'move'}]}
        self.assertEqual(driver.narrated(log, state, after)[1], 'opposing Aegislash lost 50%')
        session = FakeSession(self.constants, {'trainer_a': 'TRAINER_SHELBY_1'})
        after['actives'][1]['alive'] = False
        self.assertIn('[1] FOE Aegislash fainted', driver.render_brief(session, after, [], heading='T'))

    BLOCK = """## E0493 TRAINER_SIDNEY class=elite
strategy: MEGA_REVEAL
mega_slots: 2
field: GRASSY_TERRAIN
ai: Ace Pokemon
plan: A proposed draft.
crack: Its weaknesses.
tactic: ACTIVATE INCINEROAR FAKE_OUT KANGASKHAN
INCINEROAR @SITRUS_BERRY INTIMIDATE CAREFUL 252/0/4/0/252/0 -3 | FAKE_OUT, FLARE_BLITZ, KNOCK_OFF, PARTING_SHOT | ivs=31/31/31/31/31/31 | friendship=255
KANGASKHAN @KANGASKHANITE SCRAPPY JOLLY PS 15 | FAKE_OUT, DOUBLE_EDGE, SUCKER_PUNCH, PROTECT | ivs=31/0/31/31/31/31 | friendship=200
"""

    def write_block(self, folder, text=None, name='block.txt'):
        path = Path(folder) / name
        path.write_text(self.BLOCK if text is None else text)
        return path

    def test_foe_team_block_encodes_the_native_mailbox(self):
        import emerald_champions_teams as teams
        with tempfile.TemporaryDirectory() as folder:
            branch = teams.read_teams(self.write_block(folder))[0]
        words, flags = driver.encode_foe_team(branch, 1234, self.constants)
        self.assertEqual(len(words), driver.FOE_TEAM_WORDS)
        self.assertEqual(driver.FOE_TEAM_WORDS, 175)  # sizeof(struct EcAgentFoeTeam) / 4
        plans = driver.plan_constants()
        statuses = driver.starting_status_indices()
        ai = driver.resolve_wide('constants/battle_ai.h', flags)
        self.assertEqual(words[:4], [driver.FOE_TEAM_SCHEMA, 1234, 2, 0])  # elite: no easy reduction
        self.assertEqual(words[4] | words[5] << 32, sum(ai.values()))
        self.assertIn('AI_FLAG_ACE_POKEMON', flags)
        self.assertEqual(words[6], plans['EC_BATTLE_PLAN_MEGA_REVEAL'])
        self.assertEqual(words[7], 0x80 | 0b10)
        self.assertEqual(words[8] | words[9] << 32, 1 << statuses['STARTING_STATUS_GRASSY_TERRAIN'])
        self.assertEqual(words[10], 1)
        values = lambda table, name: self.constants[table]['values'][name]
        self.assertEqual(words[11:15], [values('species', 'SPECIES_INCINEROAR'), values('species', 'SPECIES_KANGASKHAN'),
                                        values('move', 'MOVE_FAKE_OUT'), plans['EC_BATTLE_TACTIC_ACTIVATE']])
        first = driver.FOE_TEAM_HEADER + 4 * driver.FOE_TEAM_TACTICS
        second = first + driver.FOE_TEAM_MEMBER_WORDS
        self.assertEqual(words[first:first + 6], [values('species', 'SPECIES_INCINEROAR'),
                                                  values('item', 'ITEM_SITRUS_BERRY'),
                                                  values('ability', 'ABILITY_INTIMIDATE'),
                                                  values('nature', 'NATURE_CAREFUL'), (-3) & 0xFFFFFFFF, 255])
        self.assertEqual(words[second + 6:second + 18], [31, 0, 31, 31, 31, 31, 4, 252, 0, 0, 0, 252])
        self.assertEqual(words[second + 5], 200)
        self.assertEqual(words[second + 18:second + 22], [values('move', 'MOVE_' + m) for m in
                                                          ('FAKE_OUT', 'DOUBLE_EDGE', 'SUCKER_PUNCH', 'PROTECT')])
        self.assertEqual(words[second + driver.FOE_TEAM_MEMBER_WORDS:], [0] * (len(words) - second - driver.FOE_TEAM_MEMBER_WORDS))

    def test_foe_team_mirrors_every_authored_trainers_compiled_plan_and_ai(self):
        # The same derivation the teams file's own --write performs, checked for
        # every authored branch: AI flags as trainers.party compiles them, plan
        # bits, Mega permission and tactic kinds all resolve.
        import emerald_champions_teams as teams
        import implement_emerald_champions_master_battles as implement
        branches = teams.read_teams()
        master = teams.compile_master(branches, teams.retain_authored_encounters(teams.MASTER.read_text(), branches))
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / 'master.txt'
            path.write_text(master)
            designs = implement.read_designs(path)
        plans = driver.plan_constants()
        names = set()
        for branch in branches:
            expected = ['AI_FLAG_' + flag.upper().replace(' ', '_')
                        for flag in implement.ai_flags(designs[branch.trainer]).split(' / ')]
            self.assertEqual(driver.foe_ai_flag_names(branch), expected, branch.trainer)
            names.update(expected)
            for strategy in branch.strategy:
                self.assertIn('EC_BATTLE_PLAN_' + strategy, plans)
            for kind, *_ in branch.tactics:
                self.assertIn('EC_BATTLE_TACTIC_' + kind, plans)
            self.assertLessEqual(len(branch.tactics), driver.FOE_TEAM_TACTICS)
        resolved = driver.resolve_wide('constants/battle_ai.h', sorted(names))
        self.assertTrue(all(value for value in resolved.values()))
        self.assertEqual(set(teams.STRATEGIES), {name[len('EC_BATTLE_PLAN_'):] for name in plans if name.startswith('EC_BATTLE_PLAN_')})
        statuses = driver.starting_status_indices()
        self.assertEqual(statuses['STARTING_STATUS_ELECTRIC_TERRAIN'], 0)
        self.assertLessEqual(max(statuses.values()), 63)
        self.assertTrue(teams.starting_statuses() <= {name[len('STARTING_STATUS_'):] for name in statuses})

    def test_foe_team_refuses_illegal_or_mismatched_blocks(self):
        with tempfile.TemporaryDirectory() as folder:
            def refused(text, trainer='TRAINER_SIDNEY'):
                path = self.write_block(folder, text, name=f'b{hash(text) & 0xFFFF}.txt')
                args = SimpleNamespace(foe_team=str(path), foe_team_b=None)
                with self.assertRaises(SystemExit) as caught:
                    driver.load_foe_blocks(args, 'trainer', trainer, None, self.constants)
                return str(caught.exception)
            self.assertIn('the block is for TRAINER_SIDNEY', refused(self.BLOCK, 'TRAINER_PHOEBE'))
            self.assertIn('exactly one', refused(self.BLOCK + self.BLOCK.replace('TRAINER_SIDNEY', 'TRAINER_PHOEBE')))
            self.assertIn('not pinned-legal', refused(self.BLOCK.replace('DOUBLE_EDGE', 'SPORE')))
            self.assertIn('mega_slots', refused(self.BLOCK.replace('mega_slots: 2', 'mega_slots: 1')))
            self.assertIn('no member knows Tailwind', refused(self.BLOCK.replace('strategy: MEGA_REVEAL', 'strategy: TAILWIND')))
            self.assertIn('Invalid EV spread', refused(self.BLOCK.replace('252/0/4/0/252/0', '252/252/252/0/0/0')))
            path = self.write_block(folder)
            with self.assertRaises(SystemExit):
                driver.load_foe_blocks(SimpleNamespace(foe_team=None, foe_team_b=str(path)),
                                       'trainer', 'TRAINER_SIDNEY', None, self.constants)
            blocks = driver.load_foe_blocks(SimpleNamespace(foe_team=str(path), foe_team_b=None),
                                            'trainer', 'TRAINER_SIDNEY', None, self.constants)
        record = blocks[0]['record']
        self.assertEqual((record['file'], record['trainer'], record['members'], record['mega_slots']),
                         ('foe-team-A.txt', 'TRAINER_SIDNEY', 2, 0b10))
        self.assertEqual(record['sha256'], hashlib.sha256(self.BLOCK.encode()).hexdigest())

    def test_replay_reuses_the_pinned_foe_team_copy(self):
        with tempfile.TemporaryDirectory() as folder:
            source = Path(folder)
            (source / 'foe-team-A.txt').write_text(self.BLOCK)
            meta = {'battle_kind': 'trainer', 'trainer_a': 'TRAINER_SIDNEY', 'difficulty': 'hard', 'level_cap': 80,
                    'start_args': {'battle_kind': 'trainer', 'trainer': 'TRAINER_SIDNEY', 'difficulty': 'hard',
                                   'cap': 80, 'weather': 'map', 'foe_team': '/elsewhere/draft.txt'},
                    'foe_team': {'A': {'file': 'foe-team-A.txt',
                                       'sha256': hashlib.sha256(self.BLOCK.encode()).hexdigest()}}}
            arguments, _, _, _ = driver.replay_start_args(meta, source, source)
            self.assertEqual((arguments['foe_team'], arguments['foe_team_b']), (str(source / 'foe-team-A.txt'), None))
            (source / 'foe-team-A.txt').write_text(self.BLOCK.replace('-3 |', '-2 |'))
            with self.assertRaises(SystemExit):
                driver.replay_start_args(meta, source, source)

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
