#!/usr/bin/env python3
"""Validate active encounter tables without freezing habitat, species or slot weights."""
from pathlib import Path
import copy
import json
from verify_trainer_ability_legality import configured_species_abilities, species_aliases, resolve_species

ROOT = Path(__file__).resolve().parents[1]
# Inclement's land curve ends 4, 4, 1, 1; the tail is lifted to 3 so nothing in
# the game sits at a rate you would never realistically see.
MIN_ORDINARY_SPECIES_PERCENT = 3


def main():
    payload = json.loads((ROOT / 'src/data/wild_encounters.json').read_text())
    group = next(g for g in payload['wild_encounter_groups'] if g['label'] == 'gWildMonHeaders')
    groups = json.loads((ROOT / 'data/maps/map_groups.json').read_text())
    active = set()
    for name in (n for g in groups['group_order'] for n in groups[g]):
        row = json.loads((ROOT / 'data/maps' / name / 'map.json').read_text())
        if row.get('region', 'REGION_HOENN') == 'REGION_HOENN':
            active.add(row['id'])
    fields = {f['type']: f for f in group['fields']}
    configured = configured_species_abilities()
    aliases = species_aliases()
    errors, checked = [], 0
    for entry in group['encounters']:
        if entry['map'] not in active:
            continue
        for name, field in fields.items():
            if name not in entry:
                continue
            table = entry[name]
            rates = field['encounter_rates']
            mons = table['mons']
            tag = f"{entry['map']}/{name}"
            encounter_rate = table.get('encounter_rate')
            if type(encounter_rate) is not int or not 0 <= encounter_rate <= 255:
                errors.append(f'{tag}: encounter rate must fit the native unsigned byte')
            if len(mons) != len(rates) or not mons:
                errors.append(f'{tag}: slots do not match encounter weights')
                continue
            if any(type(r) is not int or not 0 <= r <= 100 for r in rates):
                errors.append(f'{tag}: invalid probability weights')
                continue
            for mon in mons:
                species = mon.get('species')
                if (not isinstance(species, str) or species in {'SPECIES_NONE', 'SPECIES_EGG'}
                        or not configured.get(resolve_species(species, aliases))):
                    errors.append(f'{tag}: invalid configured encounter species {species}')
                if (type(mon.get('min_level')) is not int or type(mon.get('max_level')) is not int
                        or not 1 <= mon['min_level'] <= mon['max_level'] <= 100):
                    errors.append(f'{tag}: invalid level range for {mon["species"]}')
            methods = field.get('groups', {name: list(range(len(mons)))})
            for method, indices in methods.items():
                if not indices or len(set(indices)) != len(indices) or any(type(i) is not int or not 0 <= i < len(mons) for i in indices):
                    errors.append(f'{tag}/{method}: invalid slot indices')
                    continue
                if sum(rates[i] for i in indices) != 100:
                    errors.append(f'{tag}/{method}: weights must total100')
                if encounter_rate == 0:
                    continue
                for reverse in (False, True):
                    totals = {}
                    for offset, index in enumerate(indices):
                        species = mons[indices[-1-offset] if reverse else index]['species']
                        totals[species] = totals.get(species, 0) + rates[index]
                    for species, rate in totals.items():
                        if species != 'SPECIES_FEEBAS' and rate < MIN_ORDINARY_SPECIES_PERCENT:
                            errors.append(f'{tag}/{method}: {species} has {rate}% (reversed={reverse})')
            checked += 1
    # Compare materialized water tables to today's authored route sheet. This
    # protects edits to authoring inputs without freezing the old encounters.
    import emerald_champions_rebuild_wild_water as rebuild
    water_maps = {e['map'] for e in group['encounters']
                  if e['map'] in active and (e.get('water_mons') or e.get('fishing_mons'))}
    if set(rebuild.load_route_sheet()) != water_maps:
        errors.append('authored water rows differ from active Hoenn water maps')
    try:
        for method in rebuild.materialize_water(copy.deepcopy(payload)):
            errors.append(f'{method}: generated species differ from authored row')
    except (KeyError, ValueError) as error:
        errors.append(f'invalid water authoring: {error}')
    if not checked:
        errors.append('no active encounter tables checked')
    if errors:
        raise SystemExit('\n'.join(errors))
    print(f'PASS: {checked} active encounter tables have configured species, valid numeric fields and slot probabilities')


if __name__ == '__main__':
    main()
