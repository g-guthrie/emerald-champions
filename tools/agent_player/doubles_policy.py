#!/usr/bin/env python3
"""Deterministic, observable doubles baseline; native search evaluates its guesses.

Move power/type/category and species base Speed come from current source. Scores
are tactical preferences, not a damage calculator or a world-expert skill grade.
No policy reads a seed, future RNG, or either side's current native commands.
"""
from __future__ import annotations

from functools import lru_cache
import itertools
import copy
import shutil
import subprocess
import tempfile
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ec_moves

PROTECT = {'MOVE_PROTECT', 'MOVE_DETECT', 'MOVE_SPIKY_SHIELD', 'MOVE_KINGS_SHIELD',
           'MOVE_BANEFUL_BUNKER', 'MOVE_OBSTRUCT', 'MOVE_SILK_TRAP', 'MOVE_BURNING_BULWARK'}
REDIRECT = {'MOVE_FOLLOW_ME', 'MOVE_RAGE_POWDER'}
SETUP = {'MOVE_SWORDS_DANCE', 'MOVE_NASTY_PLOT', 'MOVE_DRAGON_DANCE', 'MOVE_QUIVER_DANCE',
         'MOVE_CALM_MIND', 'MOVE_BULK_UP', 'MOVE_SHELL_SMASH', 'MOVE_BELLY_DRUM'}
ABSORB = {
    'TYPE_WATER': {'ABILITY_WATER_ABSORB', 'ABILITY_STORM_DRAIN', 'ABILITY_DRY_SKIN'},
    'TYPE_ELECTRIC': {'ABILITY_VOLT_ABSORB', 'ABILITY_LIGHTNING_ROD', 'ABILITY_MOTOR_DRIVE'},
    'TYPE_FIRE': {'ABILITY_FLASH_FIRE', 'ABILITY_WELL_BAKED_BODY'},
    'TYPE_GRASS': {'ABILITY_SAP_SIPPER'},
    'TYPE_GROUND': {'ABILITY_LEVITATE', 'ABILITY_EARTH_EATER'},
}
ATE_TYPES = {'ABILITY_PIXILATE': 'TYPE_FAIRY', 'ABILITY_AERILATE': 'TYPE_FLYING',
             'ABILITY_REFRIGERATE': 'TYPE_ICE', 'ABILITY_GALVANIZE': 'TYPE_ELECTRIC',
             'ABILITY_DRAGONIZE': 'TYPE_DRAGON'}
ATE_EXEMPT_EFFECTS = {'EFFECT_HIDDEN_POWER', 'EFFECT_WEATHER_BALL', 'EFFECT_NATURAL_GIFT',
                      'EFFECT_CHANGE_TYPE_ON_ITEM', 'EFFECT_REVELATION_DANCE', 'EFFECT_TERRAIN_PULSE'}
SPREAD_TARGETS = {'TARGET_BOTH', 'TARGET_FOES_AND_ALLY', 'TARGET_ALL_BATTLERS'}


class PolicyUnresolved(Exception):
    """A player policy lacks a valid decision; this is not a native battle loss."""


def matches(expected, actual):
    """Explicit subset predicates; no code execution or seed-aware callbacks."""
    if isinstance(expected, dict):
        return isinstance(actual, dict) and all(key in actual and matches(value, actual[key])
                                               for key, value in expected.items())
    if isinstance(expected, list):
        return isinstance(actual, list) and len(expected) == len(actual) and all(
            matches(left, right) for left, right in zip(expected, actual))
    return type(expected) is type(actual) and expected == actual


@lru_cache(maxsize=1)
def source_metadata():
    expressions = {}
    for block in ec_moves._move_blocks():
        name = 'MOVE_' + block.split(']')[0].strip()
        expressions[name] = {}
        for field in ('power', 'accuracy', 'priority', 'type', 'category', 'target', 'effect'):
            value = re.search(r'\.' + field + r'\s*=\s*([^,\n]+)', block)
            if value:
                expressions[name][field] = value[1]
    # Conditional move data is common (including Fake Out's priority). Resolve
    # expressions with the source configuration rather than select a regex arm.
    probe = '#include <stdio.h>\n#include "constants/global.h"\n#include "constants/pokemon.h"\n#include "constants/battle.h"\n#include "constants/battle_move_effects.h"\n#include "config/battle.h"\nint main(void) {\n'
    for name, fields in expressions.items():
        for field, expression in fields.items():
            probe += f'printf("{name} {field} %d\\n", (int)({expression}));\n'
    tokens = set(ec_moves.TYPE_CHART_COLUMNS) | {'DAMAGE_CATEGORY_PHYSICAL', 'DAMAGE_CATEGORY_SPECIAL', 'DAMAGE_CATEGORY_STATUS'}
    for fields in expressions.values():
        for field in ('target', 'effect'):
            tokens.update(re.findall(r'\b(?:TARGET|EFFECT)_[A-Z0-9_]+\b', fields.get(field, '')))
    for token in sorted(tokens):
        probe += f'printf("TOKEN {token} %d\\n", (int)({token}));\n'
    probe += 'printf("CONFIG ate_multiplier %d\\n", B_ATE_MULTIPLIER >= GEN_7 ? 1200 : 1300);\nreturn 0; }\n'
    compiler = shutil.which('cc')
    if not compiler:
        raise ValueError('source move metadata requires the existing host C compiler')
    with tempfile.TemporaryDirectory(prefix='ec-policy-metadata-') as scratch:
        executable = Path(scratch) / 'metadata'
        subprocess.run([compiler, '-Iinclude', '-x', 'c', '-', '-o', str(executable)],
                       cwd=ROOT, input=probe, text=True, capture_output=True, check=True)
        output = subprocess.run([str(executable)], capture_output=True, text=True, check=True).stdout
    moves, type_tokens, category_tokens, target_tokens, effect_tokens, config = {}, {}, {}, {}, {}, {}
    for row in output.splitlines():
        name, field, value = row.split()
        if name == 'TOKEN':
            target = type_tokens if field.startswith('TYPE_') else target_tokens if field.startswith('TARGET_') \
                else effect_tokens if field.startswith('EFFECT_') else category_tokens
            target[int(value)] = field if field.startswith('TYPE_') else field.removeprefix('DAMAGE_CATEGORY_')
        elif name == 'CONFIG':
            config[field] = int(value)
        else:
            moves.setdefault(name, {})[field] = int(value)
    for move in moves.values():
        move['type'] = type_tokens[move['type']]
        move['category'] = category_tokens[move['category']]
        move['target'] = target_tokens[move['target']]
        move['effect'] = effect_tokens[move['effect']]
    from verify_trainer_ability_legality import preprocess_species_info
    text = preprocess_species_info()
    marks = list(re.finditer(r'\[(SPECIES_\w+)\]\s*=\s*\{', text))
    speeds = {}
    for index, mark in enumerate(marks):
        block = text[mark.end():marks[index + 1].start() if index + 1 < len(marks) else len(text)]
        speed = re.search(r'\.baseSpeed\s*=\s*(\d+)\s*,', block)
        if speed:
            speeds[mark[1]] = int(speed[1])
    return {'moves': moves, 'speeds': speeds, 'types': ec_moves.TYPES, 'chart': ec_moves.type_chart(),
            'ate_multiplier': config['ate_multiplier'] / 1000}


def effectiveness(actor, target, move_type, chart):
    multiplier = 1.0
    for kind in set(target.get('types', [])) - {'TYPE_NONE', 'TYPE_MYSTERY'}:
        multiplier *= chart.get(move_type, {}).get(kind, 1.0)
    if actor.get('ability') not in {'ABILITY_MOLD_BREAKER', 'ABILITY_TURBOBLAZE', 'ABILITY_TERAVOLT'}:
        if target.get('ability') in ABSORB.get(move_type, set()):
            return 0.0
        if target.get('ability') == 'ABILITY_WONDER_GUARD' and multiplier <= 1:
            return 0.0
        if target.get('ability') == 'ABILITY_STEAM_ENGINE' and move_type == 'TYPE_WATER':
            multiplier = min(multiplier, 1.0)
    return multiplier


class TacticalPolicy:
    def __init__(self, data=None, metadata=None):
        self.data = data or {'schema_version': 1, 'kind': 'tactical'}
        if self.data.get('schema_version') != 1 or self.data.get('kind') != 'tactical':
            raise ValueError('tactical policy requires schema 1 and kind=tactical')
        self.metadata = metadata if metadata is not None else source_metadata()
        self.occupants = {}
        self.entered = {}
        self.entry_by_mon = {}
        self.overrides = self.data.get('overrides', [])
        if not isinstance(self.overrides, list) or any(not isinstance(rule.get('when'), dict) or
            not isinstance(rule.get('commands'), list) or not rule['commands'] for rule in self.overrides):
            raise ValueError('frozen tactical overrides need observation predicates and commands')

    def clone(self):
        result = TacticalPolicy(self.data, self.metadata)
        result.occupants = copy.deepcopy(self.occupants)
        result.entered = dict(self.entered)
        result.entry_by_mon = dict(self.entry_by_mon)
        return result

    def observe(self, observation):
        previous = set(self.occupants.values())
        current = {}
        for actor in observation.get('actives', []):
            if actor.get('side') != 'player' or not actor.get('alive'):
                continue
            # Mega/form changes and Ally Switch keep the same Pokemon on the
            # field; neither refreshes Fake Out's entry-turn condition.
            key = (actor.get('agent_controlled', True), actor.get('party_slot'))
            index = actor['battler']
            if key not in previous:
                self.entry_by_mon[key] = observation.get('turn', 0)
            current[index] = key
            self.entered[index] = self.entry_by_mon[key]
        self.occupants = current

    def types_for(self, actor):
        return actor.get('types') or list(self.metadata['types'].get(actor.get('species'), ()))

    def attack(self, actor, target, move_name):
        meta = self.metadata['moves'].get(move_name, {})
        if meta.get('category') == 'STATUS':
            return 0.0
        power = max(meta.get('power', 0), 25)  # variable-power moves remain an approximate preference
        move_type = meta.get('type', '')
        ate_boost = 1.0
        if move_type == 'TYPE_NORMAL' and actor.get('ability') in ATE_TYPES \
         and meta.get('effect') not in ATE_EXEMPT_EFFECTS:
            move_type = ATE_TYPES[actor['ability']]
            ate_boost = self.metadata.get('ate_multiplier', 1.2)
        target = {**target, 'types': self.types_for(target)}
        multiplier = effectiveness(actor, target, move_type, self.metadata['chart'])
        stab = 1.5 if move_type in self.types_for(actor) else 1.0
        stat = 'atk' if meta.get('category') == 'PHYSICAL' else 'spa'
        stage = actor.get('stat_stages', {}).get(stat, 0)
        stage_factor = (2 + stage) / 2 if stage >= 0 else 2 / (2 - stage)
        accuracy = min(max(meta.get('accuracy', 100), 1), 100) / 100
        if meta.get('accuracy') == 0:
            accuracy = 1.0
        level_ratio = actor.get('level', 50) / max(target.get('level', 50), 1)
        wounded = 1 + (1 - target.get('hp', 1) / max(target.get('max_hp', 1), 1)) * .45
        item_boost = 1.5 if actor.get('item') == ('ITEM_CHOICE_BAND' if stat == 'atk' else 'ITEM_CHOICE_SPECS') \
            else 1.3 if actor.get('item') == 'ITEM_LIFE_ORB' else 1.0
        return power * multiplier * stab * ate_boost * item_boost * stage_factor * accuracy * level_ratio * wounded

    def can_act(self, actor):
        wake_tick = 2 if actor.get('ability') == 'ABILITY_EARLY_BIRD' else 1
        for status in actor.get('status', []):
            if status.startswith('sleep:') and int(status.split(':')[1]) > wake_tick:
                return False
        return True

    def threat(self, actor, opponents):
        return sum(max((self.attack(foe, actor, move['move']) for move in foe.get('moves', [])), default=0)
                   for foe in opponents)

    def direct_threat(self, actor, opponents):
        def danger(foe, move):
            if move in {'MOVE_SPORE', 'MOVE_SLEEP_POWDER', 'MOVE_HYPNOSIS', 'MOVE_SING'} and not actor.get('status'):
                powder = move in {'MOVE_SPORE', 'MOVE_SLEEP_POWDER'}
                immune = actor.get('ability') in {'ABILITY_INSOMNIA', 'ABILITY_VITAL_SPIRIT', 'ABILITY_SWEET_VEIL',
                                                  'ABILITY_PURIFYING_SALT', 'ABILITY_COMATOSE', 'ABILITY_GOOD_AS_GOLD'}
                immune |= powder and ('TYPE_GRASS' in self.types_for(actor) or actor.get('ability') == 'ABILITY_OVERCOAT'
                                      or actor.get('item') == 'ITEM_SAFETY_GOGGLES')
                return 0 if immune else 70
            return self.attack(foe, actor, move)
        return sum(max((danger(foe, move['move']) for move in foe.get('moves', [])
                        if self.metadata['moves'].get(move['move'], {}).get('target') not in SPREAD_TARGETS), default=0)
                   for foe in opponents)

    def speed(self, actor):
        return self.metadata['speeds'].get(actor.get('species'), 70) * actor.get('level', 50)

    def options(self, observation, entry):
        actives = {actor['battler']: actor for actor in observation['actives']}
        actor = actives[entry['battler']]
        opponents = [foe for foe in actives.values() if foe.get('side') == 'opponent' and foe.get('alive')]
        allies = [ally for ally in actives.values() if ally.get('side') == 'player' and ally.get('alive') and ally != actor]
        prior = next((turn for turn in observation.get('previous_turn', []) if turn.get('battler') == actor['battler']), {})
        options = []
        if not entry.get('replacing') and observation['phase'] != 'await_switch':
            for move in entry.get('moves', []):
                if not move.get('legal'):
                    continue
                name = move['move']
                meta = self.metadata['moves'].get(name, {})
                for target_index in move.get('targets', []):
                    target = actives.get(target_index, actor)
                    support = ''
                    hits = {}
                    if meta.get('category') == 'STATUS':
                        score = 4.0
                        if name in PROTECT:
                            pressure = self.threat(actor, opponents)
                            health = actor.get('hp', 1) / max(actor.get('max_hp', 1), 1)
                            score = 45 if health < .4 and pressure > 60 and allies else 9
                            spread = sum(max((self.attack(foe, actor, attack['move']) for attack in foe.get('moves', [])
                                              if self.metadata['moves'].get(attack['move'], {}).get('target') in SPREAD_TARGETS), default=0)
                                         for foe in opponents)
                            if health < .75 and allies:
                                score += min(spread * .3, 35)
                            if prior.get('move') in PROTECT:
                                score -= 60
                            support = 'protect'
                        elif name == 'MOVE_WIDE_GUARD':
                            spread = sum(max((self.attack(foe, ally, attack['move']) for attack in foe.get('moves', [])
                                              if self.metadata['moves'].get(attack['move'], {}).get('target') in SPREAD_TARGETS), default=0)
                                         for foe in opponents for ally in [actor, *allies])
                            score = min(spread * .7, 140) if spread and allies else 3
                            support = 'guard'
                        elif name in REDIRECT:
                            # A low-HP redirector can still secure its partner's
                            # decisive attack before fainting. Joint value is
                            # scored below; health alone must not discard it.
                            score = 9 if allies else -25
                            support = 'redirect'
                        elif name == 'MOVE_TAILWIND':
                            score = 65 if 'SIDE_STATUS_TAILWIND' not in observation.get('sides', {}).get('player', []) and allies else -30
                            support = 'speed'
                        elif name == 'MOVE_TRICK_ROOM':
                            slow = max((self.speed(mon) for mon in [actor, *allies]), default=0) < \
                                min((self.speed(foe) for foe in opponents), default=1)
                            active = 'STATUS_FIELD_TRICK_ROOM' in observation.get('field', [])
                            score = 65 if slow != active else -30
                            support = 'speed'
                        elif name in SETUP:
                            relevant = 'spa' if name in {'MOVE_NASTY_PLOT', 'MOVE_QUIVER_DANCE', 'MOVE_CALM_MIND'} else 'atk'
                            score = 35 if actor.get('stat_stages', {}).get(relevant, 0) < 2 else -25
                            support = 'setup'
                        elif name == 'MOVE_HELPING_HAND':
                            score, support = (35 if allies else -50), 'help'
                        elif name in {'MOVE_RECOVER', 'MOVE_ROOST', 'MOVE_SLACK_OFF', 'MOVE_SOFT_BOILED', 'MOVE_SHORE_UP', 'MOVE_SYNTHESIS'}:
                            health = actor.get('hp', 1) / max(actor.get('max_hp', 1), 1)
                            score = 75 if health < .5 else -35
                        elif name in {'MOVE_SPORE', 'MOVE_SLEEP_POWDER', 'MOVE_THUNDER_WAVE', 'MOVE_WILL_O_WISP'}:
                            score = 60 if target.get('side') == 'opponent' and not target.get('status') else -25
                        elif name in {'MOVE_RAIN_DANCE', 'MOVE_SUNNY_DAY', 'MOVE_SNOWSCAPE', 'MOVE_SANDSTORM'}:
                            score = 25
                        else:
                            score = 6
                    else:
                        victims = opponents if move.get('target_type') in {'both', 'foes_and_ally', 'all_battlers'} else [target]
                        if move.get('target_type') in {'foes_and_ally', 'all_battlers'}:
                            victims = [*victims, *allies]
                        score = 0.0
                        for victim in victims:
                            damage = self.attack(actor, victim, name)
                            if victim.get('side') == 'player':
                                score -= damage * 1.25
                            else:
                                score += damage
                                hits[victim['battler']] = damage
                        if name == 'MOVE_FAKE_OUT':
                            usable = self.entered.get(actor['battler']) == observation.get('turn', 0)
                            if not usable:
                                score = -1000
                            elif target.get('ability') != 'ABILITY_INNER_FOCUS' and sum(hits.values()) > 0:
                                score += 65
                        score += meta.get('priority', 0) * 2
                    base = {'score': score, 'move': name, 'actor': actor['battler'], 'hits': hits, 'support': support,
                            'switch_slot': None, 'mega': False,
                            'command': f"{actor['battler']}:move{move['index']}@{target_index}"}
                    if not self.can_act(actor) and name not in {'MOVE_SNORE', 'MOVE_SLEEP_TALK'}:
                        base['score'] = min(base['score'], 0)
                        base['hits'] = {}
                    options.append(base)
                    if actor.get('usable_gimmick') == 'mega' and not actor.get('mega_already_used'):
                        options.append({**base, 'command': base['command'] + ',mega', 'score': score + 8, 'mega': True})
            if entry.get('must_struggle'):
                options.append({'score': 2, 'move': 'MOVE_STRUGGLE', 'actor': actor['battler'], 'hits': {},
                                'support': '', 'switch_slot': None, 'mega': False,
                                'command': f"{actor['battler']}:struggle"})
        forced = entry.get('replacing') or observation['phase'] == 'await_switch'
        if forced or entry.get('may_switch') and not entry.get('switch_blocked_by'):
            reserves = {mon['slot']: mon for mon in observation.get('player_reserves', [])}
            current_threat = self.threat(actor, opponents)
            for slot in entry.get('switch_slots', []):
                reserve = reserves.get(slot)
                if not reserve:
                    continue
                offensive = max((sum(self.attack(reserve, foe, move['move']) for foe in opponents)
                                 for move in reserve.get('moves', [])), default=0)
                defensive = current_threat - self.threat(reserve, opponents)
                current_offensive = max((sum(self.attack(actor, foe, move['move']) for foe in opponents)
                                         for move in actor.get('moves', [])), default=0)
                score = offensive * .55 if forced else (offensive - current_offensive) * .25 - 60
                score += defensive * .4
                options.append({'score': score, 'move': '', 'actor': actor['battler'], 'hits': {},
                                'support': 'switch', 'switch_slot': slot, 'mega': False,
                                'command': f"{actor['battler']}:switch{slot}"})
        return sorted(options, key=lambda option: (-option['score'], option['command']))

    def ranked(self, observation, limit=4):
        self.observe(observation)
        entries = observation.get('pending_decision', [])
        if not entries:
            return []
        def required_only():
            required = [entry for entry in entries if entry.get('awaiting_now', True)]
            if required and len(required) < len(entries):
                # The engine asks for faint replacements sequentially. When
                # only one reserve remains, answering both dead slots would
                # spend it twice. Optional precommits can wait for native flow.
                return self.ranked({**observation, 'pending_decision': required}, limit)
            return []
        choices = [self.options(observation, entry)[:8] for entry in entries]
        if any(not values for values in choices):
            return required_only()
        ranked = []
        for joint in itertools.product(*choices):
            switches = [option['switch_slot'] for option in joint if option['switch_slot'] is not None]
            if len(switches) != len(set(switches)) or sum(option['mega'] for option in joint) > 1:
                continue
            score = sum(option['score'] for option in joint)
            support = {option['support'] for option in joint}
            if 'redirect' in support and 'setup' in support:
                score += 50
            if 'help' in support and any(option['hits'] for option in joint):
                score += 25
            actives = {actor['battler']: actor for actor in observation['actives']}
            opponents = [actor for actor in actives.values() if actor.get('side') == 'opponent' and actor.get('alive')]
            for redirect in (option for option in joint if option['support'] == 'redirect'):
                director = actives[redirect['actor']]
                if not self.can_act(director):
                    continue
                for attack in (option for option in joint if option['actor'] != redirect['actor'] and option['hits']):
                    partner = actives[attack['actor']]
                    if not self.can_act(partner):
                        continue
                    # Spread pressure bypasses Follow Me. Known single-target
                    # moves and a strong partner attack justify the sacrifice;
                    # unseen moves remain uncertain, never assumed harmless.
                    direct = self.direct_threat(partner, opponents)
                    cover = direct - self.direct_threat(director, opponents)
                    has_direct = any(len(foe.get('moves', [])) < 4 for foe in opponents) or direct > 0
                    if has_direct:
                        score += min(sum(attack['hits'].values()), 240) * .65 + max(cover, 0) * .3
            voluntary_switches = sum(option['switch_slot'] is not None and not entry.get('replacing')
                                     and observation['phase'] != 'await_switch' for option, entry in zip(joint, entries))
            if voluntary_switches > 1:
                score -= 90  # Both sides of our board concede this turn's attack.
            if len(joint) > 1 and all(option['support'] in {'protect', 'redirect', 'help', 'speed'} for option in joint):
                score -= 50
            commands = [option['command'] for option in joint]
            ranked.append({'commands': commands, 'preference_score': round(score, 4)})
        ranked.sort(key=lambda item: (-item['preference_score'], item['commands']))
        if not ranked:
            return required_only()
        return ranked[:limit]

    def choose(self, observation, decision):
        ranked = self.ranked(observation, 1)
        for rule in self.overrides:
            if matches(rule['when'], observation):
                return rule['commands']
        if not ranked:
            raise PolicyUnresolved('tactical baseline has no legal joint action')
        return ranked[0]['commands']
