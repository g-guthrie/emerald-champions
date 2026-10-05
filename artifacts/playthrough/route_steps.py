"""Turn a walk (map, from, to) into Studio recipe steps.

The path is only a proposal for which buttons to hold; the game decides. Every leg ends with the
game's own reported map and coordinates, so a wrong proposal shows up as a failed expectation.
Usage from the repo root:  python3 artifacts/playthrough/route_steps.py MAP sx,sy gx,gy
"""
import json, os, sys
from collections import deque

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import reach as R

DIRS = {(1, 0): 'RIGHT', (-1, 0): 'LEFT', (0, 1): 'DOWN', (0, -1): 'UP'}
FRAMES_PER_TILE = 16


def path(map_dir, start, goal, caps=frozenset(), block=None):
    """block: tiles occupied right now (the game's visible actors). Without it, every object in
    the map file is treated as standing where the file places it."""
    mid = next(k for k, m in R.MAPS.items() if m['_dir'] == map_dir)
    g = R.grid(mid)
    npcs = set(block) if block is not None else {(o['x'], o['y']) for o in R.MAPS[mid].get('object_events', [])}
    prev = {start: None}; q = deque([start])
    while q:
        p = q.popleft()
        if p == goal: break
        for d in DIRS:
            n = (p[0] + d[0], p[1] + d[1])
            if n in npcs and n != goal: continue
            r = R.can_enter(g, p, n, d, set(caps))
            if r and r not in prev:
                prev[r] = (p, d); q.append(r)
    if goal not in prev: return None
    moves = []; p = goal
    while prev[p]:
        p, d = prev[p][0], prev[p][1]; moves.append(d)
    moves.reverse()
    return moves


def is_counter(map_dir, p):
    mid = next(k for k, m in R.MAPS.items() if m['_dir'] == map_dir)
    g = R.grid(mid)
    return g.inb(*p) and g.beh(*p) == R.MB['MB_COUNTER']


def walk(moves, label=None):
    step = {'walk': [DIRS[d] for d in moves]}
    if label: step['label'] = label
    return [step]


def steps(moves, label=None):
    out = []
    for d in moves:
        if out and out[-1]['hold'] == DIRS[d]:
            out[-1]['frames'] += FRAMES_PER_TILE
        else:
            out.append({'hold': DIRS[d], 'frames': FRAMES_PER_TILE})
    if out and label: out[-1]['label'] = label
    return out


if __name__ == '__main__':
    m = sys.argv[1]; s = tuple(map(int, sys.argv[2].split(','))); g = tuple(map(int, sys.argv[3].split(',')))
    mv = path(m, s, g)
    print(json.dumps(steps(mv) if mv else None))
