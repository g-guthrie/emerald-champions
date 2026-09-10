import collections
import copy
import json
import re
import struct
from pathlib import Path

root = Path(__file__).resolve().parents[2]
old_data = json.loads((root / 'work/wild-habitat-20260908/wild_encounters.json').read_text())
new_data = json.loads((root / 'src/data/wild_encounters.json').read_text())
map_groups = json.loads((root / 'data/maps/map_groups.json').read_text())
active = set()
for name in [n for g in map_groups['group_order'] for n in map_groups[g]]:
    row = json.loads((root / 'data/maps' / name / 'map.json').read_text())
    if row.get('region', 'REGION_HOENN') == 'REGION_HOENN':
        active.add(row['id'])
def read_tables(data):
    group = next(g for g in data['wild_encounter_groups'] if g['label'] == 'gWildMonHeaders')
    rows = {}
    for row in group['encounters']:
        if row['map'] in active:
            rows.setdefault(row['map'], row)
    return group, rows
group, before = read_tables(old_data)
_, after = read_tables(new_data)
methods = [f['type'] for f in group['fields']]
def union(rows):
    return {m['species'] for row in rows.values() for f in methods for m in row.get(f, {}).get('mons', [])}
assert union(before) <= union(after)
opening = ['ROUTE101', 'ROUTE102', 'ROUTE103', 'ROUTE104', 'ROUTE116', 'PETALBURG_WOODS', 'RUSTURF_TUNNEL']
def opening_union(rows):
    return {m['species'] for name in opening for m in rows['MAP_' + name]['land_mons']['mons']}
assert opening_union(before) - opening_union(after) == {'SPECIES_PATRAT'}
assert opening_union(after) - opening_union(before) == {'SPECIES_PACHIRISU', 'SPECIES_TIMBURR'}
protected = {'MAP_PETALBURG_WOODS_3', 'MAP_DEWFORD_MEADOW', 'MAP_DEWFORD_MANOR_1F', 'MAP_RUSTURF_TUNNEL',
             'MAP_ROUTE116', 'MAP_GRANITE_CAVE_1F', 'MAP_GRANITE_CAVE_B1F', 'MAP_GRANITE_CAVE_B2F',
             'MAP_GRANITE_CAVE_STEVENS_ROOM', 'MAP_NEW_MAUVILLE_ENTRANCE', 'MAP_NEW_MAUVILLE_INSIDE',
             'MAP_MIRAGE_TOWER_1F', 'MAP_MT_PYRE_2F', 'MAP_MT_PYRE_5F'}
for mid in before:
    if mid.startswith('MAP_UNDERWATER_') or mid in protected:
        assert before[mid] == after[mid], mid
assert before['MAP_SANDSTREWN_RUINS']['land_mons'] == after['MAP_SANDSTREWN_RUINS']['land_mons']
assert before['MAP_DEWFORD_TOWN']['fishing_mons']['mons'][:2] == after['MAP_DEWFORD_TOWN']['fishing_mons']['mons'][:2]
changes = []
old_data = copy.deepcopy(old_data)
new_data = copy.deepcopy(new_data)
for old, new in zip(old_data['wild_encounter_groups'][0]['encounters'], new_data['wild_encounter_groups'][0]['encounters']):
    for method in methods:
        for i, (om, nm) in enumerate(zip(old.get(method, {}).get('mons', []), new.get(method, {}).get('mons', []))):
            if om['species'] != nm['species']:
                assert old['map'] in active
                changes.append({'map': old['map'], 'method': method, 'slot': i, 'before': om['species'], 'after': nm['species']})
            om['species'] = nm['species'] = 'SPECIES_NONE'
assert old_data == new_data, 'Non-species encounter data changed'
# Read back every changed native table from the final production ROM.
new_group, after = read_tables(json.loads((root / 'src/data/wild_encounters.json').read_text()))
rom = (root / 'pokeemerald-habitats-20260908-release.gba').read_bytes()
link_map = (root / 'pokeemerald-habitats-20260908-release.map').read_text()
species_ids = {name: int(value, 0) for name, value in re.findall(r'\b(SPECIES_[A-Z0-9_]+)\s*=\s*(0x[0-9a-fA-F]+|[0-9]+)\s*,', (root / 'include/constants/species.h').read_text())}
aliases = dict(re.findall(r'^#define\s+(SPECIES_[A-Z0-9_]+)\s+(SPECIES_[A-Z0-9_]+)\s*(?:/[^\n]*)?$', (root / 'include/constants/species.h').read_text(), re.M))
aliases.update(re.findall(r'\b(SPECIES_[A-Z0-9_]+)\s*=\s*(SPECIES_[A-Z0-9_]+)\s*,', (root / 'include/constants/species.h').read_text()))
for name, target in aliases.items():
    while target in aliases:
        target = aliases[target]
    if target in species_ids:
        species_ids[name] = species_ids[target]
suffix = {'land_mons': 'LandMons', 'water_mons': 'WaterMons', 'fishing_mons': 'FishingMons', 'rock_smash_mons': 'RockSmashMons'}
changed_tables = sorted({(c['map'], c['method']) for c in changes})
for mid, method in changed_tables:
    row = after[mid]
    symbol = row['base_label'] + '_' + suffix[method]
    address = int(re.search(r'0x([0-9a-fA-F]+)\s+' + re.escape(symbol) + r'\s*$', link_map, re.M)[1], 16)
    expected = b''.join(struct.pack('<BBH', m['min_level'], m['max_level'], species_ids[m['species']]) for m in row[method]['mons'])
    offset = address - 0x08000000
    assert rom[offset:offset + len(expected)] == expected, symbol
print(f'PASS: {len(changes)} changed slots across {len(changed_tables)} native tables / {len({c["map"] for c in changes})} maps match final ROM bytes')
print(f'PASS: {len(union(before))} previous ordinary species/forms retained; {len(union(after))} current; {len(opening_union(after))} opening species')
print('PASS: protected early, power-station, unique-form and Dive tables unchanged; levels, encounter rates, inactive groups and other fields unchanged')
(root / 'work/wild-habitat-20260908/slot_changes.json').write_text(json.dumps(changes, indent=2) + '\n')
