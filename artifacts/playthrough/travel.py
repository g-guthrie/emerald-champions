"""Walk from wherever a save stands to a neighbouring map, in the real game.

usage (repo root): .venv-studio/bin/python artifacts/playthrough/travel.py SAVE TARGET_MAP NAME [--via x,y]
The hop goes through a warp (door, mat, stairs) or a map edge of the current map that leads to
TARGET_MAP. The game reports the arrival; a wrong plan fails visibly.
"""
import argparse, json, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import reach as R
import route_steps as RS
import explore as X

ARROW = {'MB_SOUTH_ARROW_WARP': 'DOWN', 'MB_NORTH_ARROW_WARP': 'UP', 'MB_EAST_ARROW_WARP': 'RIGHT',
         'MB_WEST_ARROW_WARP': 'LEFT', 'MB_WATER_SOUTH_ARROW_WARP': 'DOWN', 'MB_DEEP_SOUTH_WARP': 'DOWN'}


def hop_steps(map_dir, here, target_map, via=None):
    mid = next(k for k, m in R.MAPS.items() if m['_dir'] == map_dir)
    m = R.MAPS[mid]; g = R.grid(mid)
    tid = next(k for k, mm in R.MAPS.items() if mm['_dir'] == target_map)
    inv = {v: k for k, v in R.MB.items()}
    options = []
    for w in m.get('warp_events', []):
        if w['dest_map'] != tid: continue
        x, y = w['x'], w['y']
        if via and (x, y) != via: continue
        b = inv.get(g.beh(x, y), '')
        if g.coll(x, y):            # door in a wall: stand below it and walk up into it
            mv = RS.path(map_dir, here, (x, y + 1), block=set())
            if mv is not None:
                options.append((len(mv), [{'walk_to': [x, y + 1]}, {'walk': ['UP']}, {'hold': 'UP', 'frames': 30}]))
        else:
            mv = RS.path(map_dir, here, (x, y), block=set()) if (x, y) != here else []
            if mv is not None:
                push = ARROW.get(b)
                steps = [{'walk_to': [x, y]}] if (x, y) != here else []
                if push: steps.append({'hold': push, 'frames': 40})
                options.append((len(mv), steps))
    for c in m.get('connections') or []:
        if c['map'] != tid: continue
        d = c['direction']
        edge = {'up': [(x, 0) for x in range(g.w)], 'down': [(x, g.h - 1) for x in range(g.w)],
                'left': [(0, y) for y in range(g.h)], 'right': [(g.w - 1, y) for y in range(g.h)]}.get(d, [])
        step = {'up': 'UP', 'down': 'DOWN', 'left': 'LEFT', 'right': 'RIGHT'}[d]
        # the tile across the edge must be walkable in the target map (connection offset)
        tg = R.grid(tid); off = int(c.get('offset', 0))
        def across(e):
            x, y = e
            return {'up': (x - off, tg.h - 1), 'down': (x - off, 0), 'left': (tg.w - 1, y - off),
                    'right': (0, y - off)}[d]
        for e in edge:
            if via and e != via: continue
            t = across(e)
            if not tg.inb(*t) or tg.coll(*t): continue
            mv = RS.path(map_dir, here, e, block=set()) if e != here else []
            if mv is not None:
                options.append((len(mv), [{'walk_to': list(e)}, {'walk': [step]}]))
    if not options: return None
    options.sort(key=lambda o: o[0])
    return options[0][1]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('save'); ap.add_argument('target'); ap.add_argument('name')
    ap.add_argument('--via', default='')
    a = ap.parse_args()
    _, pr = X.run_chunk({'name': 'probe ' + a.name, 'start': {'save': a.save}, 'steps': [{'frames': 2}]}, a.name + '-probe')
    f = pr['outcome']['final']
    if f['map'] == a.target and not a.via:   # already there
        print(json.dumps(dict(passed=True, failures=[], map=f['map'], x=f['x'], y=f['y'], end_save=a.save))); return
    via = tuple(map(int, a.via.split(','))) if a.via else None
    steps = hop_steps(f['map'], (f['x'], f['y']), a.target, via)
    if steps is None: raise SystemExit(f"no way from {f['map']} {f['x']},{f['y']} to {a.target}")
    steps = steps + [{'frames': 120}, {'tap': 'B', 'every': 40, 'frames': 6000, 'min_frames': 30, 'until': 'idle', 'label': 'Arrived'}]
    recipe = {'name': f"travel {f['map']} -> {a.target}", 'start': {'save': a.save}, 'battle_resolution': 'fixture_win',
              'steps': steps, 'expect': {'ready': True, 'map': a.target}}
    dest, res = X.run_chunk(recipe, a.name)
    o = res['outcome']; fin = o['final']
    print(json.dumps(dict(passed=o['passed'], failures=o['failures'], map=fin['map'], x=fin['x'], y=fin['y'],
                          end_save=fin.get('end_save'))))
    if fin.get('end_save'):
        import os; os.chmod(fin['end_save'], 0o444)


if __name__ == '__main__':
    main()
