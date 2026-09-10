"""Explain any lost direct catches using the configured evolution relations."""
import collections
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT / 'scripts'))
from verify_trainer_ability_legality import preprocess_species_info, species_aliases, resolve_species

aliases = species_aliases()
def canon(s):
    return resolve_species(s, aliases)

text = preprocess_species_info()
markers = list(re.finditer(r'(?m)^\s*\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{', text))
forward, relatives = collections.defaultdict(set), collections.defaultdict(set)
for i, marker in enumerate(markers):
    block = text[marker.end():markers[i+1].start() if i+1 < len(markers) else len(text)]
    match = re.search(r'\.evolutions\s*=\s*', block)
    if not match:
        continue
    start = block.index('{', match.end())
    level = 1
    end = start + 1
    while level:
        level += (block[end] == '{') - (block[end] == '}')
        end += 1
    # Third field of an Evolution is the target; a species in an evolution
    # condition (e.g. Remoraid in Mantyke's party condition) is not a relative.
    for target in re.findall(r'\{\s*\w+\s*,\s*[^,{}]+,\s*(SPECIES_[A-Z0-9_]+)', block[start:end]):
        a, b = canon(marker[1]), canon(target)
        if b == 'SPECIES_NONE':
            continue
        forward[a].add(b)
        relatives[a].add(b)
        relatives[b].add(a)

rated = {r['id'] for r in json.loads((ROOT / 'work/wild-habitat-20260908/ratings_personal.json').read_text())}
def direct(path):
    data = json.loads(path.read_text())
    group = next(g for g in data['wild_encounter_groups'] if g['label'] == 'gWildMonHeaders')
    result, seen = set(), set()
    for row in group['encounters']:
        if row['map'] not in rated or row['map'] in seen:
            continue
        seen.add(row['map'])
        for field in ('land_mons', 'water_mons', 'fishing_mons', 'rock_smash_mons'):
            result.update(canon(m['species']) for m in row.get(field, {}).get('mons', []))
    return result

before = direct(WORK / 'before_wild_encounters.json')
after = direct(WORK / 'proposed_wild_encounters.json')
def closure(seeds, edges):
    reached = set(seeds)
    queue = list(seeds)
    while queue:
        for target in edges[queue.pop()]:
            if target not in reached:
                reached.add(target)
                queue.append(target)
    return reached

evolvable = closure(after, forward)
missing = []
records = []
for species in sorted(before - after):
    family = closure({species}, relatives)
    available = sorted(family & after)
    mode = 'evolution from retained catches' if species in evolvable else 'retained evolved relative; breeding requires separate review'
    record = {'species': species, 'mode': mode, 'retained_direct_relatives': available}
    records.append(record)
    print(json.dumps(record))
    if not available:
        missing.append(species)
(WORK / 'family_availability.json').write_text(json.dumps(records, indent=2) + '\n')
assert not missing, ('Evolution families lost entirely:', missing)
print(f'PASS: all {len(before)} former direct identifiers retain an ordinary catch in their configured evolution family; {len(records)} become indirect.')
