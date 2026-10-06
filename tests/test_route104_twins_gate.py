"""Twins Gina & Mia are a mandatory battle on the Route 104 bridge.

Owner rule: crossing the bridge north to Rustboro must put the player in the twins' sight. They
stand on the bridge's upper row facing down, so the only way across runs beneath them. This test
walks the map's tiles (artifacts/playthrough/reach.py, the same movement model the playthrough
uses) from the south end to the north end without Surf, with the twins' tiles and the tiles they
watch blocked: no path may exist. Verified in play: crossing triggers the double battle at (28,16).
"""
import json
import os
import sys
import unittest
from collections import deque
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'artifacts/playthrough'))

SOUTH, NORTH = (31, 22), (24, 8)
FACING = {'MOVEMENT_TYPE_FACE_DOWN': (0, 1), 'MOVEMENT_TYPE_FACE_UP': (0, -1),
          'MOVEMENT_TYPE_FACE_LEFT': (-1, 0), 'MOVEMENT_TYPE_FACE_RIGHT': (1, 0)}


class TwinsBridgeGate(unittest.TestCase):
    def setUp(self):
        self.cwd = os.getcwd()
        os.chdir(ROOT)   # reach.py reads the source tree relative to the repo root

    def tearDown(self):
        os.chdir(self.cwd)

    def test_bridge_crossing_passes_through_the_twins_sight(self):
        import reach as R
        m = json.loads((ROOT / 'data/maps/Route104/map.json').read_text())
        twins = [o for o in m['object_events'] if o['script'] in ('Route104_EventScript_Gina', 'Route104_EventScript_Mia')]
        self.assertEqual(len(twins), 2)
        watched = set()
        for o in twins:
            self.assertEqual(o['trainer_type'], 'TRAINER_TYPE_NORMAL')
            self.assertIn(o['movement_type'], FACING, 'the twins must hold a fixed facing')
            sight = int(o['trainer_sight_or_berry_tree_id'])
            self.assertGreaterEqual(sight, 1)
            dx, dy = FACING[o['movement_type']]
            watched.add((o['x'], o['y']))
            watched |= {(o['x'] + dx * k, o['y'] + dy * k) for k in range(1, sight + 1)}
        mid = next(k for k, mm in R.MAPS.items() if mm['_dir'] == 'Route104')
        g = R.grid(mid)
        seen, q = {SOUTH}, deque([SOUTH])
        while q:
            p = q.popleft()
            for d in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                r = R.can_enter(g, p, (p[0] + d[0], p[1] + d[1]), d, set())
                if r and r not in seen and r not in watched:
                    seen.add(r); q.append(r)
        self.assertNotIn(NORTH, seen, 'a path crosses the bridge without entering the twins\' sight')
        # and without that block the bridge does connect, so the check above is meaningful
        seen, q = {SOUTH}, deque([SOUTH])
        while q:
            p = q.popleft()
            for d in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                r = R.can_enter(g, p, (p[0] + d[0], p[1] + d[1]), d, set())
                if r and r not in seen and r not in {(o['x'], o['y']) for o in twins}:
                    seen.add(r); q.append(r)
        self.assertIn(NORTH, seen)


if __name__ == '__main__':
    unittest.main()
