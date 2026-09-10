"""One-off readback of this approved revision's production encounter arrays."""
import json
import re
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = Path(__file__).resolve().parent
ROM = ROOT / 'pokeemerald-wild-review-20260908-release.gba'
payload = json.loads((ROOT / 'src/data/wild_encounters.json').read_text())
ratings = json.loads((ROOT / 'work/wild-habitat-20260908/ratings_personal.json').read_text())
rated = {r['id'] for r in ratings}
group = next(g for g in payload['wild_encounter_groups'] if g['label'] == 'gWildMonHeaders')
rows = {}
for row in group['encounters']:
    if row['map'] in rated:
        rows.setdefault(row['map'], row)
assert len(rows) == 138
rom = ROM.read_bytes()
link_map = ROM.with_suffix('.map').read_text()
constants = (ROOT / 'include/constants/species.h').read_text()
species_ids = {name: int(value, 0) for name, value in re.findall(r'\b(SPECIES_[A-Z0-9_]+)\s*=\s*(0x[0-9a-fA-F]+|[0-9]+)\s*,', constants)}
aliases = dict(re.findall(r'^#define\s+(SPECIES_[A-Z0-9_]+)\s+(SPECIES_[A-Z0-9_]+)\s*(?:/[^\n]*)?$', constants, re.M))
aliases.update(re.findall(r'\b(SPECIES_[A-Z0-9_]+)\s*=\s*(SPECIES_[A-Z0-9_]+)\s*,', constants))
for name, target in aliases.items():
    while target in aliases:
        target = aliases[target]
    if target in species_ids:
        species_ids[name] = species_ids[target]
suffix = {'land_mons': 'LandMons', 'water_mons': 'WaterMons', 'fishing_mons': 'FishingMons', 'rock_smash_mons': 'RockSmashMons'}
checked, slots = 0, 0
for mid, row in rows.items():
    for method, ending in suffix.items():
        if method not in row:
            continue
        symbol = row['base_label'] + '_' + ending
        found = re.search(r'0x([0-9a-fA-F]+)\s+' + re.escape(symbol) + r'\s*$', link_map, re.M)
        assert found, symbol
        address = int(found[1], 16)
        expected = b''.join(struct.pack('<BBH', m['min_level'], m['max_level'], species_ids[m['species']]) for m in row[method]['mons'])
        offset = address - 0x08000000
        assert rom[offset:offset + len(expected)] == expected, symbol
        checked += 1
        slots += len(row[method]['mons'])
print(f'PASS: all {checked} native method tables / {slots} slots across all 138 rated areas match production ROM bytes.')
(WORK / 'compiled_readback.json').write_text(json.dumps({'maps': 138, 'tables': checked, 'slots': slots, 'rom': ROM.name}, indent=2) + '\n')
