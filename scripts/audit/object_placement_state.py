#!/usr/bin/env python3
"""Hoenn maps whose object positions depend on state, ranked by persistence risk.

Engine model (src/overworld.c, src/event_object_movement.c):
  warp entry  templates reloaded from map.json -> ON_TRANSITION -> ON_LOAD ->
              ON_RESUME -> objects spawned from templates -> ON_WARP_INTO_MAP
  Continue    saved templates + saved live objects restored; ON_LOAD, ON_RESUME,
              ON_RETURN_TO_FIELD run; ON_TRANSITION does NOT run
  camera      an object whose live and spawn tiles both leave the view respawns
              from its (saved) template when the camera returns
So a position is deterministic only when every state's final position is what
ON_TRANSITION (setobjectxyperm) or the template produces. Positions reached by
a scene movement, setobjectxy (live only) or copyobjectxytoperm (template of
this stay only) survive Continue but are lost on re-entry.

This is a source inventory with bounded path exploration, not execution proof.
Findings name the script path; confirm each one natively before changing it.
"""
import argparse
import json
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
MAP_DIR = ROOT / 'data/maps'

MAIN_STORY = ('LittlerootTown', 'OldaleTown', 'PetalburgCity_Gym', 'RustboroCity', 'SlateportCity',
              'MauvilleCity', 'LavaridgeTown', 'FortreeCity', 'LilycoveCity', 'MossdeepCity',
              'SootopolisCity', 'EverGrandeCity')
NON_HOENN = ('BattleFrontier_', 'TrainerHill_', 'SecretBase', 'BattlePyramid', 'UnionRoom',
             'RecordCorner', 'BattleColosseum', 'TradeCenter', 'TrainerTower')
INIT_TYPES = {'MAP_SCRIPT_ON_TRANSITION': 'transition', 'MAP_SCRIPT_ON_LOAD': 'load',
              'MAP_SCRIPT_ON_RESUME': 'resume', 'MAP_SCRIPT_ON_RETURN_TO_FIELD': 'return',
              'MAP_SCRIPT_ON_DIVE_WARP': 'dive', 'MAP_SCRIPT_ON_FRAME_TABLE': 'frame',
              'MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE': 'warp_into'}
SPECIAL_IDS = {'OBJ_EVENT_ID_PLAYER', 'LOCALID_PLAYER', 'OBJ_EVENT_ID_FOLLOWER', 'OBJ_EVENT_ID_NPC_FOLLOWER',
               'OBJ_EVENT_ID_CAMERA', 'LOCALID_CAMERA', 'LOCALID_FOLLOWER', '255', '254', '253', '127'}
DIRS = {'up': (0, -1), 'down': (0, 1), 'left': (-1, 0), 'right': (1, 0),
        'northwest': (-1, -1), 'northeast': (1, -1), 'southwest': (-1, 1), 'southeast': (1, 1)}
WARP = re.compile(r'^warp\w*$')
LABEL = re.compile(r'^([A-Za-z_]\w*)::?')
COMMAND = re.compile(r'^\s+([a-z_]\w*)\s*(.*)$')


def step_delta(name):
    """Tile displacement of one movement macro (asm/macros/movement.inc)."""
    if 'in_place' in name or name.startswith(('face_', 'acro_wheelie_face', 'acro_end_wheelie_face',
                                               'acro_pop_wheelie_down', 'acro_pop_wheelie_up',
                                               'acro_pop_wheelie_left', 'acro_pop_wheelie_right')):
        return 0, 0
    for word, (dx, dy) in DIRS.items():
        if not name.endswith('_' + word):
            continue
        if name.startswith(('jump_2_', 'jump_special_', 'acro_wheelie_jump_')):
            return 2 * dx, 2 * dy
        if re.match(r'(walk|walk_slow|walk_fast|walk_faster|walk_slow_stairs|walk_diag|walk_slow_diag|'
                    r'jump|slide|player_run|ride_water_current|acro_wheelie_hop|acro_\w*move)_' + word + '$', name):
            return dx, dy
    if name in ('walk_down_affine', 'walk_down_start_affine'):
        return 0, 1
    if name == 'walk_left_affine':
        return -1, 0
    if name == 'walk_right_affine':
        return 1, 0
    return 0, 0


class Source:
    """Every script label in data/maps and data/scripts, with its file's .set names."""

    def __init__(self):
        self.blocks, self.kind, self.where, self.sets = {}, {}, {}, {}
        files = sorted(MAP_DIR.glob('*/scripts.inc')) + sorted((ROOT / 'data/scripts').glob('*.inc'))
        for path in files:
            sets, label, body = {}, None, []
            for raw in path.read_text(errors='replace').splitlines():
                line = raw.split('@', 1)[0].rstrip()
                m = re.match(r'^\s*\.set\s+(\w+)\s*,\s*(\w+)', line)
                if m:
                    sets[m[1]] = m[2]
                    continue
                m = LABEL.match(line)
                if m:
                    if label:
                        self.add(label, body, path)
                    label, body = m[1], []
                    continue
                m = COMMAND.match(line)
                if m and label:
                    body.append((m[1], [a.strip() for a in m[2].split(',')] if m[2].strip() else [], raw))
            if label:
                self.add(label, body, path)
            self.sets[path] = sets

    def add(self, label, body, path):
        self.blocks[label], self.where[label] = body, path
        names = {c for c, _, _ in body}
        if names & {'string', 'braille'}:
            self.kind[label] = 'text'
        elif body and all(c in ('step_end',) or step_delta(c) != (0, 0) or c.startswith(
                ('walk', 'face', 'delay', 'jump', 'emote', 'lock', 'unlock', 'set_', 'slide', 'player_run',
                 'ride', 'acro', 'fly', 'nurse', 'disable', 'enable', 'restore', 'reveal', 'levitate',
                 'stop_', 'figure', 'hide_', 'show_', 'init_', 'clear_', 'rock', 'cut', 'start_anim',
                 'destroy', 'exit_', 'enter_')) for c, _, _ in body) and 'step_end' in names:
            self.kind[label] = 'movement'
        else:
            self.kind[label] = 'script'

    def movement(self, label):
        dx = dy = 0
        for command, _, _ in self.blocks.get(label, []):
            if command == 'step_end':
                break
            ddx, ddy = step_delta(command)
            dx, dy = dx + ddx, dy + ddy
        return dx, dy


class MapModel:
    def __init__(self, name, source):
        self.name, self.src = name, source
        data = json.loads((MAP_DIR / name / 'map.json').read_text())
        self.map_const = data['id']
        self.objects = {}
        for index, obj in enumerate(data.get('object_events', []), 1):
            self.objects[index] = obj
        self.path = MAP_DIR / name / 'scripts.inc'
        self.local_names = {obj['local_id']: i for i, obj in self.objects.items() if obj.get('local_id')}
        self.roots = {}
        header = f'{name}_MapScripts'
        for command, args, _ in source.blocks.get(header, []):
            if command == 'map_script' and len(args) == 2 and args[0] in INIT_TYPES:
                kind = INIT_TYPES[args[0]]
                if kind in ('frame', 'warp_into'):
                    for c, a, _ in source.blocks.get(args[1], []):
                        if c == 'map_script_2' and len(a) == 3:
                            self.roots.setdefault(a[2], kind)
                else:
                    self.roots.setdefault(args[1], kind)
        for obj in self.objects.values():
            if obj.get('script') in source.blocks:
                self.roots.setdefault(obj['script'], 'object')
        for event in data.get('coord_events', []) + data.get('bg_events', []):
            if event.get('script') in source.blocks:
                self.roots.setdefault(event['script'], 'trigger')

    def layout(self):
        data = json.loads((MAP_DIR / self.name / 'map.json').read_text())
        for row in json.loads((ROOT / 'data/layouts/layouts.json').read_text())['layouts']:
            if row.get('id') == data['layout']:
                return row['width'], row['height']
        return 0, 0

    def local(self, token, path):
        """Resolve a local id token to this map's object index, or None."""
        token = token.strip()
        if token in SPECIAL_IDS or token.startswith('VAR_'):
            return None
        seen = set()
        while token not in seen:
            seen.add(token)
            if re.fullmatch(r'\d+', token):
                value = int(token)
                return value if value in self.objects else None
            if token in self.local_names:
                return self.local_names[token]
            for sets in (self.src.sets.get(path, {}), self.src.sets.get(self.path, {})):
                if token in sets:
                    token = sets[token]
                    break
            else:
                return None
        return None

    MAP_ARG = {'applymovement': 2, 'waitmovement': 1, 'removeobject': 1, 'addobject': 1,
               'copyobjectxytoperm': 9, 'setobjectxy': 9, 'setobjectxyperm': 9}

    def on_this_map(self, command, args):
        """Commands naming another map act on that map's objects, not this one's."""
        index = self.MAP_ARG.get(command, 9)
        return len(args) <= index or args[index] in ('', self.map_const)

    # Bounded path walk: each path carries per-object position knowledge and the
    # var/flag facts its branches assumed, so exclusive cases never combine.
    def walk(self, root, limit=6000):
        results = []
        budget = limit
        # state: {obj: ('rel', dx, dy) | ('abs', x, y) | ('gone',)}; facts: {var: value | ('ne', ...)}
        stack = [(root, 0, (), {}, {}, None)]
        while stack and budget > 0:
            budget -= 1
            label, pc, calls, state, facts, cmp = stack.pop()
            body = self.src.blocks.get(label, [])
            path = self.src.where.get(label)
            if pc >= len(body):
                if calls:
                    back = calls[-1]
                    stack.append((back[0], back[1], calls[:-1], state, facts, cmp))
                else:
                    results.append((state, 'fallthrough'))
                continue
            command, args, raw = body[pc]
            nxt = (label, pc + 1, calls, state, facts, cmp)
            targets = [a for a in args if self.src.kind.get(a) == 'script']
            if command == 'end' or (command == 'return' and not calls):
                # An `end` straight after a dispatch (goto_if/case chain) is the
                # unreachable default of an exhaustive test such as player gender.
                prev = body[pc - 1][0] if pc else ''
                if not (prev.startswith('goto_if') or prev == 'case'):
                    results.append((state, 'end'))
                continue
            if command == 'return':
                back = calls[-1]
                stack.append((back[0], back[1], calls[:-1], state, facts, cmp))
                continue
            if WARP.match(command):
                results.append((state, 'warp'))
                continue
            if command == 'goto' and targets:
                stack.append((targets[0], 0, calls, state, facts, cmp))
                continue
            if command == 'compare' and len(args) == 2:
                stack.append((label, pc + 1, calls, state, facts, (args[0], args[1])))
                continue
            if command == 'switch' and args:
                stack.append((label, pc + 1, calls, state, facts, (args[0], None)))
                continue
            conditional = command.startswith(('goto_if', 'call_if')) or command == 'case'
            if conditional and targets:
                test = condition(command, args, cmp)
                taken, skipped = assume(facts, test, True), assume(facts, test, False)
                call = command.startswith('call_if')
                if skipped is not None:
                    stack.append((label, pc + 1, calls, state, skipped, cmp))
                if taken is not None and len(calls) < 12:
                    frame = calls + ((label, pc + 1),) if call else calls
                    stack.append((targets[-1], 0, frame, state, taken, cmp))
                continue
            if command == 'call' and targets and len(calls) < 12:
                stack.append((targets[-1], 0, calls + ((label, pc + 1),), state, facts, cmp))
                continue
            if (command in ('setvar', 'setflag', 'clearflag', 'addvar', 'subvar', 'copyvar') and args
                    and persistent(args[0])) or command.startswith('trainerbattle'):
                what = args[0] if args else command
                state = {**state, ('persist',): state.get(('persist',), frozenset()) | {what}}
                nxt = (label, pc + 1, calls, state, facts, cmp)
            if command in ('setvar', 'setflag', 'clearflag', 'addvar', 'subvar', 'copyvar') and args:
                facts = dict(facts)
                if command == 'setvar' and len(args) == 2:
                    facts[args[0]] = args[1]
                elif command == 'setflag':
                    facts[args[0]] = 'TRUE'
                    hidden = [i for i, o in self.objects.items() if o.get('flag') == args[0]]
                    if args[0] == 'FLAG_SYS_CTRL_OBJ_DELETE':
                        # The battle end deletes the object the player faced (VAR_LAST_TALKED).
                        hidden += [k for k, v in state.items() if not isinstance(k, tuple) and v[0] != 'gone']
                    if hidden:
                        state = {**state, **{i: ('gone',) for i in hidden}}
                elif command == 'clearflag':
                    facts[args[0]] = 'FALSE'
                else:
                    facts.pop(args[0], None)
                nxt = (label, pc + 1, calls, state, facts, cmp)
                if command == 'setflag':
                    stack.append(nxt)
                    continue
            elif 'VAR_RESULT' in facts:
                facts = {k: v for k, v in facts.items() if k != 'VAR_RESULT'}
                nxt = (label, pc + 1, calls, state, facts, cmp)
            obj = self.local(args[0], path) if args else None
            if obj is not None and self.on_this_map(command, args):
                new = dict(state)
                if command == 'applymovement' and len(args) >= 2:
                    dx, dy = self.src.movement(args[1])
                    if (dx, dy) != (0, 0):
                        cur = new.get(obj, ('rel', 0, 0))
                        if cur[0] != 'gone':
                            new[obj] = (cur[0], cur[1] + dx, cur[2] + dy)
                            new.setdefault(('moved', obj), raw.strip())
                elif command in ('setobjectxy', 'setobjectxyperm') and len(args) >= 3:
                    try:
                        new[obj] = ('abs', int(args[1], 0), int(args[2], 0))
                        new.setdefault(('moved', obj), raw.strip())
                    except ValueError:
                        pass
                elif command == 'removeobject':
                    new[obj] = ('gone',)
                elif command == 'addobject' and new.get(obj, ('x',))[0] == 'gone':
                    new[obj] = ('rel', 0, 0)
                elif command == 'copyobjectxytoperm':
                    new[('copied', obj)] = raw.strip()
                if new != state:
                    stack.append((label, pc + 1, calls, new, nxt[4], cmp))
                    continue
            stack.append(nxt)
        return results, budget <= 0

    def placements(self):
        """(kind, root, command, obj, coords) for every direct placement command reached from a root."""
        found = []
        for root, kind in self.roots.items():
            seen, todo = set(), [root]
            while todo:
                label = todo.pop()
                if label in seen:
                    continue
                seen.add(label)
                path = self.src.where.get(label)
                for command, args, raw in self.src.blocks.get(label, []):
                    for a in args:
                        if self.src.kind.get(a) == 'script':
                            todo.append(a)
                    if command in ('setobjectxy', 'setobjectxyperm', 'copyobjectxytoperm') and args:
                        obj = self.local(args[0], path)
                        if obj is None:
                            continue
                        coords = None
                        if command != 'copyobjectxytoperm' and len(args) >= 3:
                            try:
                                coords = (int(args[1], 0), int(args[2], 0))
                            except ValueError:
                                coords = None
                        found.append((kind, root, label, command, obj, coords))
        return found


TRANSIENT = re.compile(r'^(VAR_TEMP_\w+|FLAG_TEMP_\w+|VAR_0x8\w+|VAR_RESULT|VAR_FACING|VAR_SPECIAL_\w+|'
                       r'FLAG_HIDE_MAP_NAME_POPUP|FLAG_DONT_TRANSITION_MUSIC|FLAG_SAFE_FOLLOWER_MOVEMENT)$')


def persistent(name):
    """Saved state a later ON_TRANSITION or script could read; temp vars clear on warp."""
    return bool(re.match(r'^(VAR_|FLAG_)', name)) and not TRANSIENT.match(name)


def condition(command, args, cmp):
    """(subject, op, value) for a conditional command, or None when unknown."""
    label_only = len(args) == 1
    if command == 'case' and cmp and len(args) == 2:
        return (cmp[0], 'eq', args[0])
    for suffix, op in (('_set', 'set'), ('_unset', 'unset')):
        if command.endswith(suffix) and len(args) == 2:
            return (args[0], op, None)
    m = re.search(r'_(eq|ne)$', command)
    if not m:
        return None
    if label_only and cmp and cmp[1] is not None:
        return (cmp[0], m[1], cmp[1])
    if len(args) == 3:
        return (args[0], m[1], args[1])
    return None


def assume(facts, test, taken):
    """Facts after this branch outcome, or None when the outcome contradicts them."""
    if test is None:
        return facts
    subject, op, value = test
    if op in ('set', 'unset'):
        want = 'TRUE' if (op == 'set') == taken else 'FALSE'
        known = facts.get(subject)
        if known in ('TRUE', 'FALSE') and known != want:
            return None
        return {**facts, subject: want}
    equal = (op == 'eq') == taken
    known = facts.get(subject)
    if equal:
        if isinstance(known, str) and known != value:
            return None
        if isinstance(known, tuple) and value in known[1]:
            return None
        return {**facts, subject: value}
    if isinstance(known, str):
        return None if known == value else facts
    excluded = known[1] if isinstance(known, tuple) else frozenset()
    return {**facts, subject: ('ne', excluded | {value})}


def audit_map(name, source):
    model = MapModel(name, source)
    placements = model.placements()
    issues = []
    reentry = {i: {(o['x'], o['y'])} for i, o in model.objects.items()}
    init_kinds = {}
    for kind, root, label, command, obj, coords in placements:
        if kind in ('transition', 'load', 'resume') and command == 'setobjectxyperm' and coords:
            reentry[obj].add(coords)
        if kind in ('transition', 'load', 'resume', 'return', 'dive') and command != 'copyobjectxytoperm':
            init_kinds.setdefault(obj, set()).add(kind)
    def who(obj):
        o = model.objects[obj]
        return f"#{obj} {o.get('local_id') or o['graphics_id'].replace('OBJ_EVENT_GFX_', '')}"
    for kind, root, label, command, obj, coords in placements:
        if kind == 'transition' and command == 'setobjectxyperm':
            continue
        if command == 'setobjectxyperm' and kind in ('load', 'resume'):
            issues.append((3, 'perm-outside-transition', f'{who(obj)} {command} in ON_{kind.upper()} ({label})'))
        elif command == 'setobjectxy' and kind == 'transition':
            issues.append((2, 'dead-xy-in-transition', f'{who(obj)} setobjectxy in ON_TRANSITION ({label}); objects are not spawned yet, so only a setobjectxyperm there counts'))
        elif command == 'setobjectxy' and kind in ('load', 'resume', 'return'):
            issues.append((4, 'live-xy-in-init', f'{who(obj)} setobjectxy in ON_{kind.upper()} ({label}); moves saved objects on Continue/return but nothing on warp entry'))
        elif command == 'setobjectxy' and kind == 'warp_into':
            if not can_revert(model, obj, coords):
                issues.append((1, 'live-only-harmless', f'{who(obj)} setobjectxy in ON_WARP_INTO_MAP ({label}); its spawn tile keeps it on screen, so it never respawns from the template'))
            elif root in frame_keys_for(model, root):
                issues.append((1, 'warp-into-staging', f'{who(obj)} setobjectxy in ON_WARP_INTO_MAP ({label}); staging for an ON_FRAME scene of the same state'))
            else:
                issues.append((4, 'live-only-standing', f'{who(obj)} setobjectxy in ON_WARP_INTO_MAP ({label}) with no scene to follow: '
                               'the template still says otherwise, so the object reverts when it respawns after scrolling off screen'))
    managed = {obj for kind, root, label, command, obj, coords in placements if command != 'setobjectxy'}
    for obj, kinds in init_kinds.items():
        if len(kinds - {'warp_into'}) > 1:
            issues.append((3, 'split-init', f'{who(obj)} placed from ' + ', '.join(sorted('ON_' + k.upper() for k in kinds))))
    scene_roots = [(r, k) for r, k in model.roots.items() if k in ('object', 'trigger', 'frame')]
    for root, kind in scene_roots:
        results, truncated = model.walk(root)
        reported = set()
        for state, how in results:
            if how == 'warp':
                continue
            for key, value in state.items():
                if isinstance(key, tuple):
                    continue
                obj = key
                if value[0] == 'gone' or (obj, how) in reported:
                    continue
                if value[0] == 'rel':
                    if (value[1], value[2]) == (0, 0):
                        continue
                    ends = {(x + value[1], y + value[2]) for x, y in reentry[obj]}
                else:
                    ends = {(value[1], value[2])}
                copied = ('copied', obj) in state
                if ends & reentry[obj]:
                    continue
                reported.add((obj, how))
                writes = sorted(state.get(('persist',), ()))
                if not writes:
                    # Nothing persistent changed: the scene replays on every visit and
                    # Continue simply resumes the visit it was saved in.
                    weight, tag = 1, 'per-visit'
                elif copied:
                    weight, tag = 5, 'copy-only'
                elif obj in managed:
                    # The map already places this object by state, so a scene end the
                    # placement code does not reproduce is a real divergence.
                    weight, tag = (4 if kind == 'frame' else 3), 'managed-scene-end'
                else:
                    # Template-only NPC: it walks home on the next visit, while a
                    # Continue saved in the same visit keeps the scene's tile.
                    weight, tag = 2, 'returns-home'
                issues.append((weight, tag, f"{who(obj)} ends at {sorted(ends)} after {root} "
                               f"(re-entry positions {sorted(reentry[obj])}; via {state.get(('moved', obj))}"
                               + (f"; persists {', '.join(writes[:4])}" if writes else '') + ')'))
        if truncated:
            issues.append((0, 'truncated', f'{root}: path budget exhausted'))
    # Objects removed without a hide flag come back on re-entry and camera respawn.
    for root, kind in scene_roots:
        for label in reachable(source, root):
            path = source.where.get(label)
            for command, args, raw in source.blocks.get(label, []):
                if command == 'removeobject' and args and model.on_this_map(command, args):
                    obj = model.local(args[0], path)
                    if obj and model.objects[obj].get('flag') in ('0', '', None, 0):
                        issues.append((2, 'remove-no-flag', f'{who(obj)} removeobject in {label} but has no hide flag'))
    # Duplicate NPC swaps: same sprite, different hide flags on one map.
    groups = {}
    for i, o in model.objects.items():
        if o.get('flag') not in ('0', '', None):
            groups.setdefault(o['graphics_id'], []).append(i)
    for gfx, ids in groups.items():
        flags = {model.objects[i]['flag'] for i in ids}
        if len(ids) > 1 and len(flags) > 1 and 'VAR_' not in gfx:
            issues.append((1, 'flag-swap', f"{gfx.replace('OBJ_EVENT_GFX_', '')} duplicates " +
                           ', '.join(f"#{i}@{model.objects[i]['x']},{model.objects[i]['y']} {model.objects[i]['flag']}" for i in ids)))
    uniq = []
    for issue in issues:
        if issue not in uniq:
            uniq.append(issue)
    placed = sorted({obj for *_, obj, _ in placements})
    single = all(init_kinds.get(o, set()) <= {'transition'} for o in placed) and not any(
        w >= 3 for w, *_ in uniq)
    return dict(map=name, main_story=name.startswith(MAIN_STORY), score=sum(w for w, *_ in uniq),
                one_path='yes' if single else 'no', placed_objects=[f'#{o}' for o in placed],
                issues=[dict(weight=w, kind=k, detail=d) for w, k, d in sorted(uniq, key=lambda t: -t[0])])


def can_revert(model, obj, live):
    """Can a live-only position be lost to a respawn? RemoveObjectEventIfOutsideView keeps an
    object while either its live tile or its spawn tile (initialCoords) is within the
    view window x in [px-9, px+10], y in [py-7, py+9]; only then does it respawn from the template."""
    if live is None:
        return True
    layout = model.layout()
    spawn = (model.objects[obj]['x'], model.objects[obj]['y'])
    if tuple(live) == spawn:
        return False
    def out(p, q):
        return q[0] < p[0] - 9 or q[0] > p[0] + 10 or q[1] < p[1] - 7 or q[1] > p[1] + 9
    return any(out((x, y), live) and out((x, y), spawn)
               for x in range(layout[0]) for y in range(layout[1]))


def frame_keys_for(model, root):
    """{root} when an ON_FRAME entry shares an ON_WARP_INTO entry's var/value for this root."""
    header = model.src.blocks.get(f'{model.name}_MapScripts', [])
    tables = {}
    for command, args, _ in header:
        if command == 'map_script' and len(args) == 2 and args[0] in ('MAP_SCRIPT_ON_FRAME_TABLE', 'MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE'):
            tables[args[0]] = [tuple(a[:2]) + (a[2],) for c, a, _ in model.src.blocks.get(args[1], [])
                               if c == 'map_script_2' and len(a) == 3]
    frame = {(v, n) for v, n, _ in tables.get('MAP_SCRIPT_ON_FRAME_TABLE', [])}
    keys = {(v, n) for v, n, r in tables.get('MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE', []) if r == root}
    return {root} if keys and keys <= frame else set()


def reachable(source, root):
    seen, todo = [], [root]
    while todo:
        label = todo.pop()
        if label in seen or label not in source.blocks:
            continue
        seen.append(label)
        for _, args, _ in source.blocks[label]:
            todo.extend(a for a in args if source.kind.get(a) == 'script')
    return seen


def hoenn_maps():
    groups = json.loads((MAP_DIR / 'map_groups.json').read_text())
    for group in groups['group_order']:
        if group.endswith('_Frlg'):
            continue
        for name in groups[group]:
            if not name.startswith(NON_HOENN) and (MAP_DIR / name / 'map.json').exists():
                yield name


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--map', action='append', help='only these maps (prefix match)')
    parser.add_argument('--json', action='store_true')
    parser.add_argument('--all', action='store_true', help='include maps with no findings')
    parser.add_argument('--min-weight', type=int, default=2, help='hide findings below this weight in text output')
    args = parser.parse_args()
    source = Source()
    rows = []
    for name in hoenn_maps():
        if args.map and not name.startswith(tuple(args.map)):
            continue
        row = audit_map(name, source)
        if args.all or row['issues'] or row['placed_objects']:
            rows.append(row)
    rows.sort(key=lambda r: (-r['score'], r['map']))
    if args.json:
        print(json.dumps(rows, indent=2))
        return
    for row in rows:
        shown = [i for i in row['issues'] if i['weight'] >= args.min_weight]
        if not shown and not args.all:
            continue
        tag = ' [main story]' if row['main_story'] else ''
        print(f"{row['score']:3d}  {row['map']}{tag}  one ON_TRANSITION path: {row['one_path']}")
        for issue in shown:
            print(f"       w{issue['weight']} {issue['kind']}: {issue['detail']}")


if __name__ == '__main__':
    sys.exit(main())
