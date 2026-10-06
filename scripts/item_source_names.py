#!/usr/bin/env python3
"""Item-source names must say what the source gives.

An item ball's script label, its pickup flag and a hidden item's flag are named after the item the
game hands over; a label or flag that names another item (a vanilla leftover) misleads every reader
and every extraction script. Flags keep their values, so renaming them never touches saves.

  python3 scripts/item_source_names.py --check   fail when a name drifts or two pickups share a flag
  python3 scripts/item_source_names.py --plan    print the renames --write would make
  python3 scripts/item_source_names.py --write   apply them across the source tree
"""
from __future__ import annotations

import argparse
import glob
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BALL_GFX = ('OBJ_EVENT_GFX_ITEM_BALL', 'OBJ_EVENT_GFX_MEGA_STONE')
GIVE = re.compile(r'\b(?:finditem|giveitem|giveuniqueitem)\s+(ITEM_\w+)')
# Scripts named after the person who gives the item, or after a condition, not after an item.
PERSON_LABELS = {
    'DewfordTown_EventScript_OldRodFisherman', 'FallarborTown_CozmosHouse_EventScript_PlayerHasMeteorite',
    'MauvilleCity_BikeShop_EventScript_TradeInMachBike', 'SlateportCity_Harbor_EventScript_AskToTradeScanner',
    'ShoalCave_LowTideLowerRoom_EventScript_BlackBelt', 'SootopolisCity_House1_EventScript_BrickBreakBlackBelt',
}


def snake(name: str) -> str:
    """PetalburgWoods_2 -> PETALBURG_WOODS_2"""
    s = re.sub(r'(?<=[a-z])(?=[A-Z0-9])', '_', name)
    return re.sub(r'_+', '_', s).upper()


def camel(item: str) -> str:
    return ''.join(w.capitalize() for w in item[5:].split('_'))


def norm(s: str) -> str:
    return re.sub(r'[^a-z0-9]', '', s.lower())


def load():
    items = [n for n in re.findall(r'^\s+(ITEM_\w+)\s*(?:=\s*[\w ]+)?,',
                                   (ROOT / 'include/constants/items.h').read_text().split('ITEMS_COUNT')[0], re.M)
             if n != 'ITEM_NONE']
    labels, label_file = {}, {}
    for f in glob.glob(str(ROOT / 'data/scripts/*.inc')) + glob.glob(str(ROOT / 'data/maps/*/scripts.inc')):
        cur = None
        for line in open(f, errors='ignore'):
            m = re.match(r'^(\w+)::', line)
            if m:
                cur = m.group(1); labels[cur] = []; label_file[cur] = f
            elif cur:
                labels[cur].append(line)
    maps = {}
    for f in sorted(glob.glob(str(ROOT / 'data/maps/*/map.json'))):
        maps[Path(f).parent.name] = json.loads(Path(f).read_text())
    return items, labels, label_file, maps


def names_other_item(text: str, given: str, items: list[str]) -> bool:
    """True when text names an item (or a TM) that is not the one given."""
    t = norm(text)
    if re.search(r'tm\d+', t) and not given.startswith('ITEM_TM'):
        return True
    hits = [i for i in items if len(norm(i[5:])) >= 4 and norm(i[5:]) in t]
    if not hits: return False
    # longest match wins (MAX_REVIVE over REVIVE)
    best = max(hits, key=lambda i: len(norm(i[5:])))
    # a longer item name that merely contains the given one (PEARL_STRING for PEARL) is still drift
    return norm(best[5:]) != norm(given[5:])


def names_other_map(flag: str, prefix: str, item: str, mp: str, maps) -> bool:
    """True when the flag's place part names another map that exists, never this one
    (FLAG_ITEM_ROUTE_133_... on Route 103)."""
    place = norm(re.sub(r'(_\d+)?$', '', flag[len(prefix):]))
    it = norm(item[5:])
    if it in place: place = place[:place.rindex(it)]
    if len(place) < 5: return False
    homes = [m for m in maps if place in norm(m)]
    return bool(homes) and mp not in homes


def plan():
    items, labels, label_file, maps = load()
    moves = {norm(m[5:]) for m in re.findall(r'\b(MOVE_\w+)\s*=', (ROOT / 'include/constants/moves.h').read_text())}
    moves |= {norm(m[5:]) for m in re.findall(r'^\s+(MOVE_\w+),', (ROOT / 'include/constants/moves.h').read_text(), re.M)}
    moves -= {'none', 'return'}   # 'Return' also names events (WingullReturned, ReturnGoods)
    renames, problems = {}, []
    used_labels = set(labels)
    flag_users = {}
    for mp, m in maps.items():
        for o in m.get('object_events', []) or []:
            if o.get('graphics_id') not in BALL_GFX: continue
            label = o.get('script') or ''
            body = ''.join(labels.get(label, []))
            given = GIVE.findall(body)
            if len(given) != 1: continue
            item = given[0]
            flag = o.get('flag') or ''
            flag_users.setdefault(flag, []).append((mp, o['x'], o['y'], item))
            if label not in PERSON_LABELS and names_other_item(label, item, items) and label not in renames:
                prefix = label.split('_EventScript')[0] if '_EventScript' in label else mp
                new = f'{prefix}_EventScript_Item{camel(item)}'
                n = 2
                while new in used_labels or new in renames.values():
                    new = f'{prefix}_EventScript_Item{camel(item)}{n}'; n += 1
                renames[label] = new; used_labels.add(new)
            if flag.startswith('FLAG_') and (names_other_item(flag, item, items) or (
                    flag.startswith('FLAG_ITEM_') and names_other_map(flag, 'FLAG_ITEM_', item, mp, maps))):
                renames.setdefault(flag, f'FLAG_ITEM_{snake(mp)}_{item[5:]}')
        hidden_seen = {}
        for b in m.get('bg_events', []) or []:
            if b.get('type') != 'hidden_item': continue
            flag, item = b.get('flag') or '', b['item']
            flag_users.setdefault(flag, []).append((mp, b['x'], b['y'], item))
            if flag.startswith('FLAG_') and (names_other_item(flag.replace('FLAG_HIDDEN_ITEM_', ''), item, items) or (
                    flag.startswith('FLAG_HIDDEN_ITEM_') and names_other_map(flag, 'FLAG_HIDDEN_ITEM_', item, mp, maps))):
                k = (mp, item); hidden_seen[k] = hidden_seen.get(k, 0) + 1
                suffix = '' if hidden_seen[k] == 1 else f'_{hidden_seen[k]}'
                renames.setdefault(flag, f'FLAG_HIDDEN_ITEM_{snake(mp)}_{item[5:]}{suffix}')
    # NPC gifts: giveitem X ... setflag FLAG_RECEIVED_Y where Y names another item or a TM
    for label, body in labels.items():
        text = ''.join(body)
        given = GIVE.findall(text)
        if len(set(given)) != 1: continue
        for flag in re.findall(r'setflag (FLAG_RECEIVED_\w+)', text):
            if names_other_item(flag.replace('FLAG_RECEIVED_', ''), given[0], items):
                mp = Path(label_file[label]).parent.name
                renames.setdefault(flag, f'FLAG_RECEIVED_{given[0][5:]}_{snake(mp)}')
        suffix = label.split('_EventScript_')[-1]
        names_move = any(len(mv) >= 5 and mv in norm(suffix) for mv in moves)
        old_name = norm(given[0][5:]) not in norm(suffix) and (
            names_other_item(suffix, given[0], items) or re.search(r'TM\d*', suffix) or names_move)
        if label not in PERSON_LABELS and old_name and label not in renames and '_EventScript_' in label:
            speaker = suffix.split('Give')[0] if 'Give' in suffix else ''
            new = label.split('_EventScript_')[0] + '_EventScript_' + speaker + 'Give' + camel(given[0])
            if new != label and new not in used_labels:
                renames[label] = new; used_labels.add(new)
    for flag, users in flag_users.items():
        if flag.startswith('FLAG_') and len(users) > 1:
            problems.append(f'{flag} is shared by {users}')
    # new names must not collide with anything already defined
    defined = set(re.findall(r'#define (FLAG_\w+)', (ROOT / 'include/constants/flags.h').read_text()))
    taken = set(defined)
    for old in sorted(renames):
        new = renames[old]
        if not new.startswith('FLAG_') or new == old: continue
        base, n = new, 2
        while new in taken and new != old:
            new = f'{base}_{n}'; n += 1
        renames[old] = new; taken.add(new)
    return renames, problems


def write(renames):
    files = subprocess.run(['git', 'ls-files', 'data', 'src', 'include', 'test', 'scripts', 'tools', 'tests'],
                           cwd=ROOT, capture_output=True, text=True).stdout.split()
    pats = [(re.compile(r'\b' + re.escape(o) + r'\b'), n) for o, n in renames.items()]
    changed = 0
    for f in files:
        p = ROOT / f
        if p.suffix not in ('.inc', '.json', '.h', '.c', '.py', '.s', '.txt', '.party', '.md'): continue
        try: s = p.read_text()
        except (UnicodeDecodeError, FileNotFoundError): continue
        t = s
        for pat, n in pats:
            t = pat.sub(n, t)
        if t != s:
            p.write_text(t); changed += 1
    return changed


def main():
    ap = argparse.ArgumentParser()
    g = ap.add_mutually_exclusive_group(required=True)
    g.add_argument('--check', action='store_true'); g.add_argument('--plan', action='store_true')
    g.add_argument('--write', action='store_true')
    a = ap.parse_args()
    renames, problems = plan()
    if a.plan:
        for o, n in sorted(renames.items()): print(f'{o} -> {n}')
        for p in problems: print('PROBLEM', p)
        print(f'{len(renames)} renames, {len(problems)} problems'); return
    if a.write:
        if problems:
            for p in problems: print('PROBLEM', p)
            sys.exit('fix the problems first')
        print(f'{write(renames)} files changed, {len(renames)} names'); return
    if renames or problems:
        for o, n in sorted(renames.items()): print(f'drift: {o} should be {n}')
        for p in problems: print('PROBLEM', p)
        sys.exit(1)
    print('item source names match what they give')


if __name__ == '__main__':
    main()
