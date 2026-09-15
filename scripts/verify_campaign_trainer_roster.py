#!/usr/bin/env python3
"""Require exact authoring/native-party/Hoenn battle-call identity agreement.

Retired numeric IDs and empty metadata may remain for saves and Match Call;
retired loadouts may not. Alternate FRLG maps/scripts are outside this build.
"""
from pathlib import Path
import re
import emerald_champions_teams as teams
ROOT = Path(__file__).resolve().parents[1]

def strip_comments(text):
    return re.sub(r'//[^\n]*|/\*.*?\*/', '', text, flags=re.S)

def main():
    branches = teams.read_teams()
    expected = {b.trainer for b in branches}
    source = strip_comments((ROOT/'src/data/trainers.party').read_text())
    parties = {block.split(' ===',1)[0] for block in re.split(r'^=== ',source,flags=re.M)[1:]
               if re.search(r'^SPECIES_',block,re.M)}
    calls = set()
    gym_calls = set()
    for path in [*(ROOT/'data/maps').rglob('scripts.inc'),*(ROOT/'data/scripts').rglob('*.inc')]:
        if 'frlg' in str(path.relative_to(ROOT)).lower():
            continue
        for line in strip_comments(path.read_text()).splitlines():
            if re.match(r'\s*(trainerbattle\w*|multi_\w*)\s',line):
                ids = set(re.findall(r'\bTRAINER_\w+\b',line))
                calls.update(ids)
                if re.search(r'(?:City|Town)_Gym(?:_|/)', path.as_posix()):
                    gym_calls.update(ids)
    errors = []
    # Use actual Gym entrypoints: class=leader also includes the three-member
    # Space Center multi owners, who are not Gym trainers.
    gym_parties = {b.trainer for b in branches if b.cls == 'gym'} | gym_calls
    for branch in branches:
        if branch.trainer in gym_parties and len(branch.mons) != 6:
            errors.append(f'Gym party {branch.trainer}: requires 6 Pokemon, found {len(branch.mons)}')
    for label,actual in [('nonempty native parties',parties),('Hoenn script battle IDs',calls)]:
        if actual != expected:
            errors.append(f'{label}: missing={sorted(expected-actual)} extra={sorted(actual-expected)}')
    if errors:
        raise SystemExit('\n'.join(errors))
    print(f'PASS: exactly {len(expected)} authored variants = native nonempty parties = Hoenn script battle IDs; no retired battle loadouts')
    print(f'PASS: all {len(gym_parties)} Gym trainer/leader parties contain six Pokemon')

if __name__ == '__main__':
    main()
