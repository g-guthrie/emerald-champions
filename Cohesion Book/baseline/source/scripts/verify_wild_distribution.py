#!/usr/bin/env python3
"""Validate active encounter tables without freezing habitat, species or slot weights."""
from pathlib import Path
import json

ROOT = Path(__file__).resolve().parents[1]
# Current no-grind policy. This is deliberately explicit and revisable.
MIN_ORDINARY_SPECIES_PERCENT = 5


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
            if len(mons) != len(rates) or not mons:
                errors.append(f'{tag}: slots do not match encounter weights')
                continue
            if any(type(r) is not int or not 0 <= r <= 100 for r in rates):
                errors.append(f'{tag}: invalid probability weights')
                continue
            for mon in mons:
                if not 1 <= mon['min_level'] <= mon['max_level'] <= 100:
                    errors.append(f'{tag}: invalid level range for {mon["species"]}')
            methods = field.get('groups', {name: list(range(len(mons)))})
            for method, indices in methods.items():
                if not indices or len(set(indices)) != len(indices) or any(type(i) is not int or not 0 <= i < len(mons) for i in indices):
                    errors.append(f'{tag}/{method}: invalid slot indices')
                    continue
                if sum(rates[i] for i in indices) != 100:
                    errors.append(f'{tag}/{method}: weights must total100')
                if table.get('encounter_rate', 0) <= 0:
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
    sheet, caps = rebuild.load_route_sheet(), rebuild.caps_by_map()
    species, evolutions, _ = rebuild.load_species_data()
    seen = set()
    for entry in group['encounters']:
        map_id = entry['map']
        if rebuild.KANTO.search(map_id) or map_id in seen or not (entry.get('water_mons') or entry.get('fishing_mons')):
            continue
        seen.add(map_id)
        if map_id not in active:
            continue
        if map_id not in sheet:
            errors.append(f'{map_id}: missing authored water row')
            continue
        row = sheet[map_id]
        region = rebuild.REGIONS[row.get('region') or rebuild.REGION_OF.get(map_id, 'inland')]
        offset = sum(map(ord, map_id)) % 7 + len(seen)
        for field, builder in [('water_mons', rebuild.build_water), ('fishing_mons', rebuild.build_fishing)]:
            if entry.get(field) and entry[field] != builder(region, offset, caps.get(map_id, 55), evolutions, species, row):
                errors.append(f'{map_id}/{field}: generated table differs from authored row')
    if set(sheet) != seen:
        errors.append('authored water rows differ from generator input maps')
    if not checked:
        errors.append('no active encounter tables checked')
    if errors:
        raise SystemExit('\n'.join(errors))
    print(f'PASS: {checked} active encounter tables have valid slots, levels and species probabilities')


if __name__ == '__main__':
    main()
