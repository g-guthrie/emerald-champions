"""One-off materialization of the user's approved, individually authored plans."""
import argparse
import copy
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = Path(__file__).resolve().parent
PLANS = ROOT / 'work/wild-habitat-20260908'
WILD = ROOT / 'src/data/wild_encounters.json'
SHEET = ROOT / 'data/emerald_champions/wild_route_sheet.json'


def read(path):
    return json.loads(path.read_text())


def tables(data):
    group = next(g for g in data['wild_encounter_groups'] if g['label'] == 'gWildMonHeaders')
    by_map = {}
    for row in group['encounters']:
        by_map.setdefault(row['map'], row)
    return group, by_map


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    before = read(WORK / 'before_wild_encounters.json')
    after = copy.deepcopy(before)
    sheet = read(WORK / 'before_wild_route_sheet.json')
    group, old_rows = tables(before)
    _, new_rows = tables(after)
    ratings = read(PLANS / 'ratings_personal.json')
    rated = {r['id'] for r in ratings}
    expected = {r['id'] for r in ratings if r['rating'] < 10}
    methods = {f['type']: f for f in group['fields']}
    available = set(re.findall(r'\bSPECIES_[A-Z0-9_]+\b', (ROOT / 'include/constants/species.h').read_text()))
    plan = {}
    for name in ('early', 'dungeons', 'water'):
        owned = read(PLANS / f'approved_{name}_plan.json')
        assert not (plan.keys() & owned.keys()), 'overlapping ownership'
        plan.update(owned)
    assert set(plan) == expected, (sorted(expected - plan.keys()), sorted(plan.keys() - expected))
    changes = []
    for map_id, row in plan.items():
        entry = new_rows[map_id]
        count_before = len(changes)
        for name, species in row['methods'].items():
            if name in ('old_rod', 'good_rod', 'super_rod'):
                native = 'fishing_mons'
                indices = methods[native]['groups'][name]
            else:
                native = name
                assert native in methods, (map_id, name)
                indices = list(range(len(methods[native]['encounter_rates'])))
            assert native in entry, (map_id, name, 'method does not exist')
            assert len(species) == len(indices), (map_id, name, 'slot count')
            assert all('SPECIES_' + s in available and not s.startswith('SPECIES_') for s in species), (map_id, name, 'species')
            for index, s in zip(indices, species):
                mon = entry[native]['mons'][index]
                new = 'SPECIES_' + s
                if mon['species'] != new:
                    changes.append({'map': map_id, 'method': native, 'slot': index,
                                    'before': mon['species'], 'after': new})
                mon['species'] = new
            if native in ('fishing_mons', 'water_mons'):
                key = 'surf' if native == 'water_mons' else name
                sheet[map_id][key] = species
        assert len(changes) > count_before, (map_id, 'no actual change')
    for r in ratings:
        if r['rating'] == 10:
            assert new_rows[r['id']] == old_rows[r['id']], r['id']
    # Prove this operation only changes species in existing default-map slots.
    a, b = copy.deepcopy(before), copy.deepcopy(after)
    for old_group, new_group in zip(a['wild_encounter_groups'], b['wild_encounter_groups']):
        for old, new in zip(old_group['encounters'], new_group['encounters']):
            for field in methods:
                for om, nm in zip(old.get(field, {}).get('mons', []), new.get(field, {}).get('mons', [])):
                    om['species'] = nm['species'] = 'SPECIES_NONE'
    assert a == b, 'non-species data changed'
    def union(rows):
        return {m['species'] for mid, row in rows.items() if mid in rated
                for field in methods for m in row.get(field, {}).get('mons', [])}
    old_species, new_species = union(old_rows), union(new_rows)
    for mid, species in [('MAP_ROUTE101', 'MIENFOO'), ('MAP_ROUTE102', 'TIMBURR'), ('MAP_ROUTE102', 'PACHIRISU'),
                         ('MAP_PETALBURG_WOODS_3', 'KARTANA'), ('MAP_DEWFORD_MEADOW', 'PHEROMOSA'),
                         ('MAP_CAVE_OF_ORIGIN_1F', 'WALKING_WAKE')]:
        assert any(m['species'] == 'SPECIES_' + species for m in new_rows[mid]['land_mons']['mons']), (mid, species)
    assert old_rows['MAP_DEWFORD_TOWN']['fishing_mons']['mons'][:2] == new_rows['MAP_DEWFORD_TOWN']['fishing_mons']['mons'][:2]
    assert old_rows['MAP_PETALBURG_CITY']['fishing_mons']['mons'][:2] == new_rows['MAP_PETALBURG_CITY']['fishing_mons']['mons'][:2]
    summary = {'maps': len(plan), 'tables': len({(c['map'], c['method']) for c in changes}), 'slots': len(changes),
               'previous_direct_species': len(old_species), 'current_direct_species': len(new_species),
               'direct_removed': sorted(old_species - new_species), 'direct_added': sorted(new_species - old_species)}
    (WORK / 'proposed_wild_encounters.json').write_text(json.dumps(after, indent=2) + '\n')
    (WORK / 'proposed_wild_route_sheet.json').write_text(json.dumps(sheet, indent=2) + '\n')
    (WORK / 'slot_changes.json').write_text(json.dumps(changes, indent=2) + '\n')
    (WORK / 'summary.json').write_text(json.dumps(summary, indent=2) + '\n')
    print(json.dumps(summary, indent=2))
    if args.apply:
        assert read(WILD) == before, 'live wild data changed since baseline; reconcile before writing'
        assert read(SHEET) == read(WORK / 'before_wild_route_sheet.json'), 'live authoring changed since baseline'
        WILD.write_text(json.dumps(after, indent=2) + '\n')
        formatted = json.dumps(sheet, indent=2)
        formatted = re.sub(r'\[\n\s+("[A-Z0-9_]+"(?:,\n\s+"[A-Z0-9_]+")*)\n\s+\]',
                           lambda m: '[' + re.sub(r',\n\s+', ', ', m[1]) + ']', formatted)
        SHEET.write_text(formatted + '\n')
        print('Applied approved species-only plan to native tables and exact water authoring.')


if __name__ == '__main__':
    main()
