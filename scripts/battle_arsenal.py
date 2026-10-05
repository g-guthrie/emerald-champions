#!/usr/bin/env python3
"""Acquisition candidates and strict per-scenario party legality.

The economy atlas discovers source paths, not physical access. Its output stays
unresolved until a scenario supplies checked acquisition states. Alternative
states remain separate: a union of their resources is not a legal party.
"""
from __future__ import annotations

import argparse
from collections import Counter
from functools import lru_cache
import hashlib
import json
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools/agent_player'))
from generate_battle_suite import source_fingerprint
import ec_moves
from verify_trainer_ability_legality import (
    configured_species_abilities, preprocess_species_info, resolve_species, species_aliases,
)


@lru_cache(maxsize=1)
def species_rules():
    text = preprocess_species_info()
    marks = list(re.finditer(r'\[(SPECIES_\w+)\]\s*=\s*\{', text))
    restricted = set()
    for index, mark in enumerate(marks):
        block = text[mark.end():marks[index + 1].start() if index + 1 < len(marks) else len(text)]
        if re.search(r'\.(?:isRestrictedLegendary|isSubLegendary|isMythical|isUltraBeast|isParadox)\s*=\s*1\b', block):
            restricted.add(mark[1])
    from player_star_rule import restricted_exceptions
    restricted |= restricted_exceptions()
    aliases = species_aliases()
    return configured_species_abilities(), restricted, aliases


def source_evidence(path: Path, root: Path = ROOT):
    return {'path': str(path.relative_to(root)),
            'sha256': hashlib.sha256(path.read_bytes()).hexdigest()}


def check_evidence(rows, root: Path = ROOT):
    if not isinstance(rows, list) or not rows:
        raise ValueError('acquisition state needs checked source evidence')
    for row in rows:
        path = (root / row['path']).resolve()
        if not path.is_relative_to(root.resolve()) or not path.is_file():
            raise ValueError('invalid acquisition source path')
        if hashlib.sha256(path.read_bytes()).hexdigest() != row.get('sha256'):
            raise ValueError('stale acquisition source evidence: ' + row['path'])


def acquisition_states(initial: dict, transitions: list[dict], *, excluded=(), max_states=10000):
    """Explore finite, checked acquisition alternatives without merging them.

    Inputs are source-reviewed transitions, not inferred map adjacency. Each
    transition fires once per path; repeatable suppliers should declare their
    useful party-sized quantity. Conditions unknown to the producer remain
    unresolved and are never silently treated as satisfied.
    """
    excluded = set(excluded)
    identifiers = [row['id'] for row in transitions]
    if len(set(identifiers)) != len(identifiers):
        raise ValueError('duplicate acquisition transition')
    def key(state):
        return (tuple(sorted(state['facts'])), tuple(sorted(state['resources'].items())),
                tuple(sorted(state['used'])))
    start = {'facts': set(initial.get('facts', [])),
             'resources': Counter(initial.get('resources', {})), 'used': set(), 'path': []}
    if any(type(n) is not int or n < 0 for n in start['resources'].values()):
        raise ValueError('invalid initial resource quantities')
    todo = [start]
    seen = {key(start)}
    states = []
    unresolved = set()
    while todo:
        state = todo.pop()
        states.append(state)
        for row in transitions:
            ident = row['id']
            if ident in excluded or ident in state['used']:
                continue
            if row.get('status') != 'verified':
                unresolved.add(ident)
                continue
            check_evidence(row.get('source_evidence'))
            if not set(row.get('requires', [])) <= state['facts']:
                continue
            if set(row.get('forbids', [])) & state['facts']:
                continue
            consumed = row.get('consumes', {})
            granted = row.get('grants', {})
            if any(type(n) is not int or n < 0 for n in [*consumed.values(), *granted.values()]):
                raise ValueError('invalid transition quantity: ' + ident)
            if any(state['resources'][token] < amount for token, amount in consumed.items()):
                continue
            resources = state['resources'].copy()
            resources.subtract(consumed)
            resources.update(granted)
            resources = Counter({k: v for k, v in resources.items() if v})
            following = {'facts': state['facts'] | set(row.get('facts', [])),
                         'resources': resources, 'used': state['used'] | {ident},
                         'path': state['path'] + [ident]}
            signature = key(following)
            if signature not in seen:
                if len(seen) >= max_states:
                    raise ValueError('acquisition search limit reached; arsenal unresolved')
                seen.add(signature)
                todo.append(following)
    return {'states': [{'facts': sorted(s['facts']), 'resources': dict(s['resources']),
                        'path': s['path']} for s in states],
            'unresolved': sorted(unresolved)}


def validate_party(scenario: dict, manifest: dict, *, internal_fingerprint=None, internal_context=None) -> None:
    """Refuse unproven, stale or incompatible preparation before native play."""
    if scenario.get('legality_status') != 'proven':
        raise ValueError('scenario acquisition legality is unresolved')
    # Only an in-process exporter that freshly hashed an immutable tree may
    # pass this snapshot. JSON/CLI callers always use live source verification.
    fingerprint = internal_fingerprint if internal_fingerprint is not None else source_fingerprint()
    if scenario.get('source_fingerprint') != fingerprint:
        raise ValueError('scenario does not match current source')
    producer = scenario.get('producer', {}).get('kind')
    if producer == 'opening':
        from battle_opening_arsenal import certify_scenario
        certify_scenario(scenario,internal_fingerprint=internal_fingerprint,internal_context=internal_context)
    elif producer == 'campaign':
        from battle_campaign_arsenal import certify_scenario
        certify_scenario(scenario,internal_fingerprint=internal_fingerprint,internal_context=internal_context)
        if scenario.get('battle_access_status') != 'proven':
            raise ValueError('campaign battle access is unresolved')
        if scenario.get('expected_opponent_status') != 'proven' or not scenario.get('expected_opponent'):
            raise ValueError('campaign opponent identity is unresolved')
    else:
        raise ValueError('scenario has no supported source acquisition certificate')
    if scenario.get('difficulty') not in {'easy', 'medium', 'hard'}:
        raise ValueError('invalid scenario difficulty')
    cap = scenario.get('level_cap')
    if type(cap) is not int or not 1 <= cap <= 100:
        raise ValueError('invalid player level cap')
    party = manifest.get('party')
    if not isinstance(party, list) or not 1 <= len(party) <= 6:
        raise ValueError('party must contain one to six Pokemon')
    states = scenario.get('acquisition_states')
    if not isinstance(states, list) or not states:
        raise ValueError('missing mutually compatible acquisition states')
    if scenario.get('battle_field'):
        check_evidence(scenario['battle_field'].get('source_evidence'))
    abilities, restricted, aliases = species_rules()
    restricted_count = 0
    needed = Counter()
    for mon in party:
        species = mon['species']
        configured = resolve_species(species, aliases)
        restricted_count += configured in restricted
        if mon.get('ability') not in abilities.get(configured, set()):
            raise ValueError('unavailable species ability: ' + species)
        natures = set(re.findall(r'\bNATURE_[A-Z]+\b', (ROOT / 'include/constants/pokemon.h').read_text()))
        if mon.get('nature') not in natures:
            raise ValueError('invalid nature')
        moves = mon.get('moves', [])
        legal, _ = ec_moves.legal_moves_with_rom_union(species)
        if len(moves) != 4 or len(set(moves)) != 4 or not set(moves) <= legal:
            raise ValueError('illegal moves: ' + species)
        evs = mon.get('evs', [])
        ivs = mon.get('ivs', [31] * 6)
        if len(evs) != 6 or any(type(n) is not int or not 0 <= n <= 252 for n in evs) or sum(evs) > 510:
            raise ValueError('illegal EV spread')
        if len(ivs) != 6 or any(type(n) is not int or not 0 <= n <= 31 for n in ivs):
            raise ValueError('illegal IV spread')
        if mon.get('level', cap) != cap:
            raise ValueError('preparation level differs from player cap')
        if 'friendship' in mon and (type(mon['friendship']) is not int or not 0 <= mon['friendship'] <= 255):
            raise ValueError('illegal friendship')
        if type(mon.get('pp_bonuses', 0)) is not int or not 0 <= mon.get('pp_bonuses', 0) <= 255:
            raise ValueError('illegal PP bonuses')
        needed[species] += 1
        item = mon.get('item', 'ITEM_NONE')
        if item != 'ITEM_NONE':
            needed[item] += 1
    if restricted_count > 1:
        raise ValueError('party contains multiple restricted Pokemon')
    from player_star_rule import blocked_mega_slots
    blocked = blocked_mega_slots(party, restricted, aliases)
    if blocked:
        raise ValueError('one star: a restricted party member blocks separate Mega loadouts in slots ' + ', '.join(str(i + 1) for i in blocked) + '; declare mega_disabled=true for an intentionally inert stone')
    mega_stones = set(re.findall(r'ITEM_\w+', (ROOT / 'src/data/emerald_champions_mega_stones.h').read_text()))
    for state in states:
        check_evidence(state.get('source_evidence'))
        resources = state.get('resources', {})
        if any(resources.get(token, 0) < count for token, count in needed.items()):
            continue
        services = set(state.get('services', []))
        if 'legal_move_tutor' not in services:
            continue
        if 'owned_monsters' in state:
            owned = state['owned_monsters']
            ids = [m.get('availability', {}).get('owned_mon_id') for m in party]
            if any(not isinstance(ident, str) or not ident for ident in ids) or len(set(ids)) != len(ids):
                continue
            selected = set(state.get('selected_owned_ids', []))
            by_id = state.get('pokemon_defaults_by_id', {})
            if any(ident not in owned or ident not in selected or ident not in by_id for ident in ids):
                continue
            if any(owned[ident]['species'] != m['species'] for m, ident in zip(party, ids)):
                continue
            defaults = [by_id[ident] for ident in ids]
            if any(m.get('pokerus', 0) != d.get('pokerus', 0)
                   or m.get('pp_bonuses', 0) != d.get('pp_bonuses', 0) for m, d in zip(party, defaults)):
                continue
        else:
            defaults = [state.get('pokemon_defaults', {}).get(m['species'], {}) for m in party]
        if 'nature' not in services and any(m['nature'] != d.get('nature') for m, d in zip(party, defaults)):
            continue
        if 'ability' not in services and any(m['ability'] != d.get('ability') for m, d in zip(party, defaults)):
            continue
        if 'evs' not in services and any(m['evs'] != d.get('evs') for m, d in zip(party, defaults)):
            continue
        if any(m.get('pokerus', 0) not in state.get('pokerus_values', [0]) for m in party):
            continue
        if any(m.get('pp_bonuses', 0) not in state.get('pp_bonuses_values', [0]) for m in party):
            continue
        if any(m.get('ivs', [31] * 6) != d.get('ivs') for m, d in zip(party, defaults)) and 'ivs' not in services:
            continue
        if any('friendship' in m and m['friendship'] != d.get('friendship') for m, d in zip(party, defaults)) and 'friendship' not in services:
            continue
        if any('friendship' in m and m['friendship'] not in {
            *state.get('friendship_values', []),
            d.get('friendship'),
        } for m, d in zip(party, defaults)):
            continue
        if any(m.get('item') in mega_stones and m.get('mega_disabled') is not True for m in party) and resources.get('ITEM_MEGA_RING', 0) < 1:
            continue
        return
    raise ValueError('no single legal acquisition state can prepare this party')


def candidates():
    import economy_reference
    atlas = economy_reference.build_catalog(ROOT)
    atlas['source_evidence'] = [source_evidence(path) for path in sorted(atlas.pop('source_paths'))]
    # Keep native-provider boundaries and unresolved conditions intact. This is
    # a discovery input for route/access validation, never an arsenal proof.
    wild = json.loads((ROOT / 'src/data/wild_encounters.json').read_text())
    return {'schema_version': 2, 'source_generated': True,
            'source_fingerprint': source_fingerprint(), 'legality_status': 'unresolved',
            'economy': atlas, 'wild': wild,
            'scenarios': [], 'scope': 'Source acquisition candidates; physical access and compatible progression remain to be verified.'}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    data = candidates()
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(data, indent=2, sort_keys=True) + '\n')
    print('Acquisition candidates exported; no scenario promoted to proven.')


if __name__ == '__main__':
    main()
