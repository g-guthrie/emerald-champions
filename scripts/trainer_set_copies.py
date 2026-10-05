#!/usr/bin/env python3
"""Report nearby trainers sharing species, ability and the same four moves."""
from __future__ import annotations
import argparse
import json
import re
from collections import defaultdict
from itertools import combinations
from emerald_champions_teams import read_teams


def trainer_identity(trainer):
    # Numbered rematches and the starter branches of the same rival encounter
    # are alternative appearances of one trainer, not independent copies.
    if trainer.startswith('TRAINER_GRUNT_'):
        return trainer  # Numbered grunts are different trainers, not rematches.
    if trainer == 'TRAINER_MATT_MT_PYRE':
        return 'TRAINER_MATT'
    return re.sub(r'_\d+$', '', re.sub(r'_(TREECKO|TORCHIC|MUDKIP)$', '', trainer))


def find_copies(branches, distance=60):
    by_set = defaultdict(list)
    for branch in branches:
        for slot, mon in enumerate(branch.mons, 1):
            key = (mon.species, mon.ability, tuple(sorted(mon.moves)))
            by_set[key].append((branch, slot))
    copies = []
    for (species, ability, moves), entries in by_set.items():
        for (left, ls), (right, rs) in combinations(entries, 2):
            if (left.encounter == right.encounter and left.cls == right.cls == 'rival') or trainer_identity(left.trainer) == trainer_identity(right.trainer):
                continue
            if abs(left.encounter - right.encounter) > distance:
                continue
            copies.append(dict(species=species, ability=ability, moves=moves,
                left=dict(trainer=left.trainer, encounter=left.encounter, slot=ls),
                right=dict(trainer=right.trainer, encounter=right.encounter, slot=rs)))
    return sorted(copies, key=lambda row: (row['left']['encounter'], row['right']['encounter'], row['species']))


def late_copies(branches):
    start = next(b.encounter for b in branches if b.trainer == 'TRAINER_GRUNT_AQUA_HIDEOUT_1')
    # Buffel is the finale but retains E0304, before the Hideout in the
    # authoring IDs. Its late-game status must not exempt it from this check.
    return [row for row in find_copies(branches)
            if any(side['encounter'] >= start or side['trainer'] == 'TRAINER_BUFFEL'
                   for side in (row['left'], row['right']))]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--late', action='store_true', help='only pairs touching the Aqua Hideout or later encounter IDs')
    parser.add_argument('--json', action='store_true')
    args = parser.parse_args()
    branches = read_teams()
    rows = late_copies(branches) if args.late else find_copies(branches)
    if args.json:
        print(json.dumps(rows, indent=2))
    else:
        for r in rows:
            print(f"{r['species']} / {r['ability']}: E{r['left']['encounter']:04} {r['left']['trainer']} #{r['left']['slot']} = E{r['right']['encounter']:04} {r['right']['trainer']} #{r['right']['slot']}")
        print(f'{len(rows)} nearby copied trainer sets')
    return bool(rows)


if __name__ == '__main__':
    raise SystemExit(main())
