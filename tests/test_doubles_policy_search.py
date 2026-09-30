"""Tactical policy choices and bounded search semantics without native builds."""
import copy
import json
from pathlib import Path
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools/agent_player'))
import battle_search
from doubles_policy import TacticalPolicy, PolicyUnresolved, effectiveness, source_metadata


def metadata():
    moves = {
        'MOVE_TACKLE': {'power': 70, 'type': 'TYPE_NORMAL', 'category': 'PHYSICAL'},
        'MOVE_FAKE_OUT': {'power': 40, 'type': 'TYPE_NORMAL', 'category': 'PHYSICAL', 'priority': 3},
        'MOVE_BITE': {'power': 60, 'type': 'TYPE_DARK', 'category': 'PHYSICAL'},
        'MOVE_EARTHQUAKE': {'power': 100, 'type': 'TYPE_GROUND', 'category': 'PHYSICAL'},
        'MOVE_PROTECT': {'power': 0, 'type': 'TYPE_NORMAL', 'category': 'STATUS'},
        'MOVE_FOLLOW_ME': {'power': 0, 'type': 'TYPE_NORMAL', 'category': 'STATUS'},
        'MOVE_SWORDS_DANCE': {'power': 0, 'type': 'TYPE_NORMAL', 'category': 'STATUS'},
        'MOVE_TAILWIND': {'power': 0, 'type': 'TYPE_FLYING', 'category': 'STATUS'},
    }
    return {'moves': moves, 'speeds': {'SPECIES_PLAYER': 50, 'SPECIES_FOE': 90},
            'types': {'SPECIES_PLAYER': ('TYPE_NORMAL',), 'SPECIES_RESERVE': ('TYPE_GROUND',)},
            'chart': {'TYPE_NORMAL': {'TYPE_GHOST': 0}, 'TYPE_GROUND': {'TYPE_FLYING': 0}}}


def actor(index=0, side='player', types=None):
    return {'battler': index, 'side': side, 'alive': True, 'party_slot': index // 2,
            'species': 'SPECIES_PLAYER' if side == 'player' else 'SPECIES_FOE',
            'types': types or ['TYPE_NORMAL'], 'level': 30, 'hp': 100, 'max_hp': 100,
            'stat_stages': {}, 'moves': [{'move': 'MOVE_TACKLE'}],
            'ability': 'ABILITY_NONE', 'usable_gimmick': 'none', 'mega_already_used': False}


def observation(moves=('MOVE_FAKE_OUT', 'MOVE_TACKLE')):
    actors = [actor(0), actor(1, 'opponent'), actor(2), actor(3, 'opponent')]
    entries = [{'battler': 0, 'may_switch': False, 'switch_slots': [], 'moves': [
        {'index': index, 'move': move, 'legal': True, 'targets': [1], 'target_type': 'selected'}
        for index, move in enumerate(moves)]}]
    return {'phase': 'await_action', 'turn': 0, 'actives': actors, 'pending_decision': entries,
            'player_reserves': [], 'previous_turn': [], 'sides': {'player': []}, 'field': []}


class TacticalTests(unittest.TestCase):
    def test_observed_status_moves_do_not_complete_the_enemy_scouting_report(self):
        # Actual fresh seed 2027, turn 2: only Leech Seed/Protect had been
        # revealed. The previous policy chose Nuzzle and lost Sylveon to an
        # unobserved Facade. This fixture contains only the pre-choice view.
        state = json.loads((ROOT / 'tests/data/battle_policy/partial-scouting-turn2.json').read_text())
        foes = [mon for mon in state['actives'] if mon['side'] == 'opponent']
        self.assertEqual([mon['moves'][0]['move'] for mon in foes], ['MOVE_LEECH_SEED', 'MOVE_PROTECT'])
        self.assertEqual(TacticalPolicy().choose(state, 2), ['0:move0@1', '2:move0@2'])

    def test_fully_revealed_spread_loadouts_do_not_get_redirection_cover_credit(self):
        state = json.loads((ROOT / 'tests/data/battle_policy/partial-scouting-turn2.json').read_text())
        policy = TacticalPolicy()
        def redirect_score():
            return next(row['preference_score'] for row in policy.ranked(state, 64)
                        if row['commands'] == ['0:move0@1', '2:move0@2'])
        uncertain = redirect_score()
        for mon in state['actives']:
            if mon['side'] == 'opponent':
                mon['moves'] = [{'move': move} for move in
                                ['MOVE_ELECTROWEB', 'MOVE_ROCK_SLIDE', 'MOVE_SURF', 'MOVE_PROTECT']]
        self.assertLess(redirect_score(), uncertain)

    def test_source_metadata_resolves_current_conditional_move_priority(self):
        native = source_metadata()
        self.assertEqual(native['moves']['MOVE_FAKE_OUT']['priority'], 3)
        self.assertEqual(native['moves']['MOVE_FAKE_OUT']['type'], 'TYPE_NORMAL')
        self.assertEqual(native['moves']['MOVE_PROTECT']['category'], 'STATUS')
        self.assertEqual(native['moves']['MOVE_ELECTROWEB']['target'], 'TARGET_BOTH')
        self.assertEqual(native['moves']['MOVE_WEATHER_BALL']['effect'], 'EFFECT_WEATHER_BALL')
        self.assertEqual(native['ate_multiplier'], 1.2)
        self.assertEqual(native['speeds']['SPECIES_PIKACHU'], 90)

    def test_pixilate_uses_fairy_type_stab_and_configured_boost(self):
        meta = metadata()
        meta['moves']['MOVE_HYPER_VOICE'] = {'power': 90, 'type': 'TYPE_NORMAL', 'category': 'SPECIAL',
                                           'target': 'TARGET_BOTH', 'effect': 'EFFECT_HIT'}
        meta['chart']['TYPE_FAIRY'] = {'TYPE_DARK': 2}
        player = actor(types=['TYPE_FAIRY'])
        player['ability'] = 'ABILITY_PIXILATE'
        ghost = actor(1, 'opponent', ['TYPE_GHOST'])
        policy = TacticalPolicy(metadata=meta)
        self.assertAlmostEqual(policy.attack(player, ghost, 'MOVE_HYPER_VOICE'), 90 * 1.5 * 1.2)
        dark = actor(1, 'opponent', ['TYPE_DARK'])
        self.assertAlmostEqual(policy.attack(player, dark, 'MOVE_HYPER_VOICE'), 90 * 1.5 * 1.2 * 2)
        meta['moves']['MOVE_HYPER_VOICE']['effect'] = 'EFFECT_WEATHER_BALL'
        self.assertEqual(policy.attack(player, ghost, 'MOVE_HYPER_VOICE'), 0)

    def test_low_hp_redirector_preserves_a_strong_partner_attack(self):
        meta = metadata()
        meta['moves']['MOVE_HYPER_VOICE'] = {'power': 90, 'type': 'TYPE_NORMAL', 'category': 'SPECIAL', 'target': 'TARGET_BOTH'}
        state = observation(('MOVE_HYPER_VOICE',))
        state['actives'][0]['types'] = ['TYPE_FAIRY']
        state['actives'][0]['ability'] = 'ABILITY_PIXILATE'
        state['actives'][0]['item'] = 'ITEM_CHOICE_SPECS'
        state['pending_decision'][0]['moves'][0]['target_type'] = 'both'
        state['actives'][2]['hp'] = 15
        state['pending_decision'].append({'battler': 2, 'moves': [
            {'index': 0, 'move': 'MOVE_FOLLOW_ME', 'legal': True, 'targets': [2], 'target_type': 'user'},
            {'index': 1, 'move': 'MOVE_TACKLE', 'legal': True, 'targets': [1], 'target_type': 'selected'}]})
        policy = TacticalPolicy(metadata=meta)
        self.assertEqual(policy.choose(state, 0), ['0:move0@1', '2:move0@2'])

    def test_redirection_cannot_claim_to_cover_spread_attacks(self):
        meta = metadata()
        meta['moves']['MOVE_ELECTROWEB'] = {'power': 55, 'type': 'TYPE_ELECTRIC', 'category': 'SPECIAL', 'target': 'TARGET_BOTH'}
        policy = TacticalPolicy(metadata=meta)
        foes = [actor(1, 'opponent')]
        foes[0]['moves'] = [{'move': 'MOVE_ELECTROWEB'}]
        self.assertGreater(policy.threat(actor(), foes), 0)
        self.assertEqual(policy.direct_threat(actor(), foes), 0)

    def test_wide_guard_has_value_against_observed_spread_pressure(self):
        meta = metadata()
        meta['moves']['MOVE_ELECTROWEB'] = {'power': 55, 'type': 'TYPE_ELECTRIC', 'category': 'SPECIAL', 'target': 'TARGET_BOTH'}
        meta['moves']['MOVE_WIDE_GUARD'] = {'power': 0, 'type': 'TYPE_ROCK', 'category': 'STATUS', 'target': 'TARGET_USER'}
        state = observation(('MOVE_WIDE_GUARD',))
        for foe in (state['actives'][1], state['actives'][3]):
            foe['moves'] = [{'move': 'MOVE_ELECTROWEB'}]
        policy = TacticalPolicy(metadata=meta)
        danger = policy.options(state, state['pending_decision'][0])[0]['score']
        for foe in (state['actives'][1], state['actives'][3]):
            foe['moves'] = [{'move': 'MOVE_TACKLE'}]
        harmless = policy.options(state, state['pending_decision'][0])[0]['score']
        self.assertGreater(danger, harmless)

    def test_sleep_does_not_credit_an_attack_that_cannot_execute(self):
        state = observation(('MOVE_TACKLE',))
        state['actives'][0]['status'] = ['sleep:2']
        policy = TacticalPolicy(metadata=metadata())
        option = policy.options(state, state['pending_decision'][0])[0]
        self.assertEqual(option['hits'], {})
        self.assertLessEqual(option['score'], 0)
        state['actives'][0]['status'] = ['sleep:1']
        self.assertTrue(policy.options(state, state['pending_decision'][0])[0]['hits'])

    def test_voluntary_switch_does_not_get_credit_for_attacking_this_turn(self):
        state = observation(('MOVE_TACKLE',))
        state['player_reserves'] = [dict(actor(), slot=4)]
        state['pending_decision'][0].update(may_switch=True, switch_slots=[4])
        policy = TacticalPolicy(metadata=metadata())
        chosen = policy.choose(state, 0)
        self.assertEqual(chosen, ['0:move0@1'])

    def test_fake_out_only_on_entry_turn(self):
        policy = TacticalPolicy(metadata=metadata())
        state = observation()
        self.assertEqual(policy.choose(state, 0), ['0:move0@1'])
        state['turn'] = 1
        self.assertEqual(policy.choose(state, 1), ['0:move1@1'])

    def test_immunity_never_gets_fake_out_flinch_credit(self):
        state = observation(('MOVE_FAKE_OUT', 'MOVE_BITE'))
        state['actives'][1]['types'] = ['TYPE_GHOST']
        self.assertEqual(TacticalPolicy(metadata=metadata()).choose(state, 0), ['0:move1@1'])

    def test_mega_and_ally_switch_do_not_refresh_fake_out(self):
        policy = TacticalPolicy(metadata=metadata())
        state = observation()
        policy.choose(state, 0)
        state['turn'] = 1
        state['actives'][0]['species'] = 'SPECIES_PLAYER_MEGA'
        self.assertEqual(policy.choose(state, 1), ['0:move1@1'])
        state['actives'][0]['party_slot'], state['actives'][2]['party_slot'] = 1, 0
        self.assertEqual(policy.choose(state, 2), ['0:move1@1'])

    def test_steam_engine_water_resistance_and_ability_bypass(self):
        chart = {'TYPE_WATER': {'TYPE_FIRE': 2}}
        defender = {'types': ['TYPE_FIRE'], 'ability': 'ABILITY_STEAM_ENGINE'}
        self.assertEqual(effectiveness({'ability': 'ABILITY_NONE'}, defender, 'TYPE_WATER', chart), 1)
        self.assertEqual(effectiveness({'ability': 'ABILITY_MOLD_BREAKER'}, defender, 'TYPE_WATER', chart), 2)

    def test_repeat_protect_preference_is_lower(self):
        state = observation(('MOVE_PROTECT', 'MOVE_TACKLE'))
        state['actives'][0]['hp'] = 20
        policy = TacticalPolicy(metadata=metadata())
        first = next(option['score'] for option in policy.options(state, state['pending_decision'][0]) if option['move'] == 'MOVE_PROTECT')
        state['previous_turn'] = [{'battler': 0, 'move': 'MOVE_PROTECT'}]
        second = next(option['score'] for option in policy.options(state, state['pending_decision'][0]) if option['move'] == 'MOVE_PROTECT')
        self.assertLess(second, first)

    def test_spread_friendly_fire_and_immune_partner_change_preference(self):
        state = observation(('MOVE_EARTHQUAKE', 'MOVE_TACKLE'))
        state['pending_decision'][0]['moves'][0]['target_type'] = 'foes_and_ally'
        policy = TacticalPolicy(metadata=metadata())
        score1 = next(option['score'] for option in policy.options(state, state['pending_decision'][0]) if option['move'] == 'MOVE_EARTHQUAKE')
        state['actives'][2]['types'] = ['TYPE_FLYING']
        score2 = next(option['score'] for option in policy.options(state, state['pending_decision'][0]) if option['move'] == 'MOVE_EARTHQUAKE')
        self.assertGreater(score2, score1)

    def test_joint_commands_never_mega_twice(self):
        state = observation(('MOVE_TACKLE',))
        state['actives'][0]['usable_gimmick'] = state['actives'][2]['usable_gimmick'] = 'mega'
        second = copy.deepcopy(state['pending_decision'][0])
        second['battler'] = 2
        state['pending_decision'].append(second)
        ranked = TacticalPolicy(metadata=metadata()).ranked(state, 20)
        self.assertTrue(ranked)
        self.assertTrue(all(sum(',mega' in command for command in option['commands']) <= 1 for option in ranked))

    def test_joint_replacements_never_reuse_one_reserve(self):
        state = observation()
        state['phase'] = 'await_switch'
        state['player_reserves'] = [dict(actor(), slot=2), dict(actor(), slot=3)]
        state['pending_decision'] = [dict(battler=slot, replacing=True, switch_slots=[2, 3]) for slot in (0, 2)]
        ranked = TacticalPolicy(metadata=metadata()).ranked(state, 10)
        self.assertEqual(len(ranked), 2)
        for option in ranked:
            self.assertEqual(len({command.split('switch')[1] for command in option['commands']}), 2)

    def test_single_reserve_answers_only_required_native_replacement(self):
        state = observation()
        state['phase'] = 'await_switch'
        state['player_reserves'] = [dict(actor(), slot=4)]
        state['pending_decision'] = [
            dict(battler=0, replacing=True, switch_slots=[4], awaiting_now=True),
            dict(battler=2, replacing=True, switch_slots=[4], awaiting_now=False)]
        policy = TacticalPolicy(metadata=metadata())
        self.assertEqual(policy.choose(state, 0), ['0:switch4'])
        state['pending_decision'][1]['switch_slots'] = []
        self.assertEqual(policy.choose(state, 1), ['0:switch4'])
        state['pending_decision'][1]['switch_slots'] = [4]
        state['pending_decision'][1]['awaiting_now'] = True
        with self.assertRaises(PolicyUnresolved):
            policy.choose(state, 1)

    def test_policy_clone_has_separate_turn_memory(self):
        parent = TacticalPolicy(metadata=metadata())
        parent.observe(observation())
        child = parent.clone()
        state = observation()
        state['turn'] = 4
        state['actives'][0]['party_slot'] = 3
        child.observe(state)
        self.assertEqual(parent.entered[0], 0)
        self.assertEqual(child.entered[0], 4)

    def test_frozen_override_uses_observation_not_seed(self):
        state = observation()
        data = battle_search.frozen_policy([{'observation': state, 'commands': ['0:move1@1']}])
        self.assertNotIn('seed', json.dumps(data))
        policy = TacticalPolicy(data, metadata())
        self.assertEqual(policy.choose(state, 0), ['0:move1@1'])
        state['turn'] = 1
        self.assertEqual(policy.choose(state, 1), ['0:move1@1'])


class FakePolicy:
    def ranked(self, observation, limit):
        return [{'commands': ['bad'], 'preference_score': 100},
                {'commands': ['win'], 'preference_score': 5}][:limit]
    def clone(self):
        return self


class FakeNative:
    def __init__(self, directory):
        self.directory = Path(directory)
    def fork(self, directory):
        return FakeNative(directory)
    def call(self, command, *arguments):
        if command == 'act':
            self.state = {'phase': 'ended', 'outcome': 'won' if arguments == ('win',) else 'lost'}
            return {'phase': 'ended', 'outcome': self.state['outcome']}
        if command == 'state':
            return self.state
        raise AssertionError(command)


class SearchTests(unittest.TestCase):
    def test_native_outcome_can_select_lower_preference_action(self):
        with tempfile.TemporaryDirectory() as temp:
            result = battle_search.beam_search(FakeNative(temp), observation(), FakePolicy(), Path(temp) / 'search',
                                               max_nodes=2, actions_per_node=2)
            self.assertEqual(result['category'], 'candidate_win')
            self.assertFalse(result['verified_legal_winning_witness'])
            policy = json.loads((Path(temp) / 'search/discovered-policy.json').read_text())
            self.assertEqual(policy['overrides'][0]['commands'], ['win'])

    def test_budget_exhaustion_is_unresolved_not_impossible(self):
        with tempfile.TemporaryDirectory() as temp:
            result = battle_search.beam_search(FakeNative(temp), observation(), FakePolicy(), Path(temp) / 'search',
                                               max_nodes=1, actions_per_node=2)
            self.assertEqual(result['category'], 'unresolved')
            self.assertEqual(result['expanded_nodes'], 1)

    def test_fork_copies_mutable_state_and_links_same_immutable_rom(self):
        with tempfile.TemporaryDirectory() as temp:
            parent = battle_search.InlineNativeDriver(Path(temp) / 'parent')
            for name in ('scene.gba', 'scene.elf', 'current.ss1', 'events.jsonl'):
                (parent.directory / name).write_bytes(name.encode())
            (parent.directory / 'session.json').write_text('{}')
            child = parent.fork(Path(temp) / 'child')
            (child.directory / 'current.ss1').write_bytes(b'new state')
            self.assertEqual((parent.directory / 'current.ss1').read_bytes(), b'current.ss1')
            self.assertEqual((parent.directory / 'scene.gba').stat().st_ino, (child.directory / 'scene.gba').stat().st_ino)


if __name__ == '__main__':
    unittest.main()
