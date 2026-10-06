#!/usr/bin/env python3
"""Assign every wild table its levels in route order, the way Vanilla does.

Owner rule: wild levels belong to places. Each area (scripts/walkthrough_order.py AREAS) sits in
the level-cap window the player first reaches it in (FRONTIER), and its grass rises a little along
the route order inside that window, ending a few levels under the window's cap. Other methods follow
the tool that opens them:
  Surf water   - no lower than the first area reached with Surf (the Balance Badge window)
  Rock Smash   - no lower than the first area after the Dynamo Badge (the license)
  Old Rod      - a few levels under the area's grass (Mom's rod, from the start)
  Good Rod     - no lower than Route 114, where the rod is given
  Super Rod    - no lower than Mossdeep, where the rod is given
  Honey        - the area's grass band
No evolved Pokemon is met below the level it evolves at: a slot under its evolution level is raised
to it. Legendary-class slots are left alone (they meet the player at the cap).

  python3 scripts/assign_wild_levels.py --plan    print the level map and any conflicts
  python3 scripts/assign_wild_levels.py --write   rewrite src/data/wild_encounters.json
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'scripts'))
import walkthrough_order as W

WILD = ROOT / 'src/data/wild_encounters.json'
OLD_ROD_SLOTS, GOOD_ROD_SLOTS = 2, 3          # fishing slots 0-1, 2-4, 5-9
BAND = 3                                     # levels per grass band
# (window cap, the last area the player reaches in that window) in story order
WINDOWS = [(14, 'RustboroCity'), (20, 'SlateportCity'), (30, 'MauvilleCity'), (40, 'LavaridgeTown'),
           (55, 'FortreeCity'), (60, 'MagmaHideout'), (65, 'MossdeepCity'), (70, 'SootopolisCity'),
           (80, 'EverGrandeCity'), (85, 'SSTidal')]
# rooms the walkthrough order files with an earlier area but the player reaches later
LATER = {'MeteorFalls_1F_2R': 'SootopolisCity', 'MeteorFalls_B1F_1R': 'SootopolisCity',   # Waterfall
         'MeteorFalls_B1F_2R': 'SootopolisCity', 'MeteorFalls_StevensCave': 'SootopolisCity',
         'AlteringCave': 'SSTidal'}                                                        # after the League

def area_index(map_dir: str) -> int | None:
    for room, later in LATER.items():
        if map_dir.startswith(room):
            return W._index(later)
    best = None
    for i, area in enumerate(W.AREAS):
        for prefix in area:
            if map_dir.startswith(prefix) and (best is None or len(prefix) > best[1]):
                best = (i, len(prefix))
    return best[0] if best else None


def grass_tops() -> dict[int, int]:
    """Top grass level of every area index: each window climbs from the last window's top to cap-3."""
    tops, start, first = {}, 2 + BAND - 1, 0
    for cap, name in WINDOWS:
        last = W._index(name)
        idx = list(range(first, last + 1))
        end = cap - 3
        for j, i in enumerate(idx):
            tops[i] = round(start + (end - start) * (j + 1) / len(idx)) if len(idx) > 1 else end
        start, first = end + 1, last + 1
    return tops


def evolution_floors() -> dict[str, int]:
    text = (ROOT / 'src/data/wild_evolution_floors.h').read_text()
    return {s: int(l) for s, l in re.findall(r'\{(SPECIES_\w+),\s*(\d+)\}', text)}


def legendary_species() -> set[str]:
    out = set()
    for f in (ROOT / 'src/data/pokemon/species_info').glob('*.h'):
        for block in re.split(r'\n    \[SPECIES_', f.read_text())[1:]:
            name = 'SPECIES_' + block.split(']', 1)[0]
            if re.search(r'\.is(RestrictedLegendary|SubLegendary|Mythical|UltraBeast|Paradox)\s*=\s*TRUE', block):
                out.add(name)
    return out


def main():
    ap = argparse.ArgumentParser()
    g = ap.add_mutually_exclusive_group(required=True)
    g.add_argument('--plan', action='store_true'); g.add_argument('--write', action='store_true')
    a = ap.parse_args()
    dirs = {json.loads(f.read_text()).get('id'): f.parent.name for f in (ROOT / 'data/maps').glob('*/map.json')}
    tops = grass_tops()
    top_of = lambda prefix: tops[area_index(prefix)]
    surf, smash = top_of('Route118'), top_of('Route111')
    good, super_ = top_of('Route114'), top_of('MossdeepCity')
    floors, legends = evolution_floors(), legendary_species()
    cap_of = {}
    first = 0
    for cap, name in WINDOWS:
        last = W._index(name)
        for i in range(first, last + 1): cap_of[i] = cap
        first = last + 1
    raw = WILD.read_text(); data = json.loads(raw)
    rows, conflicts, skipped = [], [], []
    for group in data['wild_encounter_groups']:
        for enc in group['encounters']:
            mdir = dirs.get(enc.get('map'))
            i = area_index(mdir) if mdir else None
            if i is None:
                skipped.append(enc.get('map') or enc.get('base_label')); continue
            top = tops[i]
            bands = {'land_mons': top, 'honey_mons': top, 'water_mons': max(top, surf),
                     'rock_smash_mons': max(top, smash)}
            for kind, table in enc.items():
                if not (isinstance(table, dict) and 'mons' in table): continue
                for slot, mon in enumerate(table['mons']):
                    if mon['species'] in legends: continue
                    if kind == 'fishing_mons':
                        hi = (max(top - 4, 4) if slot < OLD_ROD_SLOTS else
                              max(top, good) if slot < OLD_ROD_SLOTS + GOOD_ROD_SLOTS else max(top, super_) + 3)
                    else:
                        hi = bands[kind]
                    lo = max(2, hi - BAND + 1)
                    evo = floors.get(mon['species'], 1)
                    if evo > lo:
                        lo, hi = evo, max(hi, evo)
                        if evo > cap_of[i] and kind == 'land_mons':
                            conflicts.append(f"{mdir} {kind}[{slot}] {mon['species'][8:]} evolves at {evo}, "
                                             f"area window cap {cap_of[i]}")
                    mon['min_level'], mon['max_level'] = lo, hi
            rows.append((i, mdir, {k: (max(2, v - BAND + 1), v) for k, v in bands.items() if k in enc}))
    if a.plan:
        for i, mdir, b in sorted(rows, key=lambda r: (r[0], r[1])):
            print(f"{i:3} cap{cap_of[i]:3} {mdir:34} " + '  '.join(f"{k.split('_')[0]} {lo}-{hi}" for k, (lo, hi) in b.items()))
        print(f"\nconflicts ({len(conflicts)}):"); print('\n'.join(conflicts))
        print(f"\nnot in the walkthrough order ({len(skipped)}): {', '.join(map(str, skipped))}")
        return
    WILD.write_text(json.dumps(data, indent=2, ensure_ascii=False) + ('\n' if raw.endswith('\n') else ''))
    print(f'wrote {WILD.relative_to(ROOT)}: {len(rows)} tables; {len(conflicts)} conflicts, {len(skipped)} skipped')


if __name__ == '__main__':
    main()
