"""Scripted walks must respect the route's real terrain, even when the engine doesn't."""
import json
import re
import struct
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class RivalTutorialPathTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scripts = (ROOT / "data/scripts/guided_tutorials.inc").read_text()
        cls.layout = next(x for x in json.loads((ROOT / "data/layouts/layouts.json").read_text())["layouts"]
                          if x["id"] == "LAYOUT_ROUTE101")
        cls.cells = cls.words(cls.layout["blockdata_filepath"])
        headers = (ROOT / "src/data/tilesets/headers.h").read_text()
        metatiles = (ROOT / "src/data/tilesets/metatiles.h").read_text()
        cls.attributes = []
        for key in ("primary_tileset", "secondary_tileset"):
            block = headers.split(f'const struct Tileset {cls.layout[key]} =')[1].split('};')[0]
            symbol = re.search(r'\.metatileAttributes = (\w+)', block)[1]
            path = re.search(symbol + r'\[\] = INCBIN_U16\("([^"]+)"', metatiles)[1]
            cls.attributes.append(cls.words(path))
        names = re.findall(r'^\s+(MB_\w+)', (ROOT / "include/constants/metatile_behaviors.h").read_text(), re.M)
        cls.ledges = {i for i, name in enumerate(names) if name.startswith("MB_JUMP_")}

    @staticmethod
    def words(path):
        data = (ROOT / path).read_bytes()
        return struct.unpack('<' + 'H' * (len(data) // 2), data)

    def walk(self, label, start):
        block = self.scripts.split(label + ':', 1)[1].split('step_end', 1)[0]
        path = [start]
        for line in block.splitlines():
            action = line.split('@')[0].strip()
            if not action or action.startswith('face_'):
                continue
            dx, dy = {'walk_left': (-1, 0), 'walk_right': (1, 0),
                      'walk_up': (0, -1), 'walk_down': (0, 1)}[action]
            x, y = path[-1][0] + dx, path[-1][1] + dy
            self.assertTrue(0 <= x < self.layout['width'] and 0 <= y < self.layout['height'])
            cell = self.cells[y * self.layout['width'] + x]
            self.assertEqual(cell & 0xC00, 0, f'{label} walks into blocked tile {(x, y)}')
            tile = cell & 0x3FF
            behavior = self.attributes[tile >= 512][tile % 512] & 0xFF
            self.assertNotIn(behavior, self.ledges, f'{label} walks through ledge {(x, y)}')
            path.append((x, y))
        return path

    def test_both_entrance_lanes_reach_the_demo_without_obstacles_or_actor_overlap(self):
        rival = self.walk('EC_RivalDexNavTutorial_Movement_Lead', (10, 14))
        self.assertEqual(rival[-1], (11, 12))
        for x in (10, 11):
            with self.subTest(entrance=x):
                start = (x, 19)
                if x == 11:
                    start = self.walk('EC_RivalDexNavTutorial_Movement_JoinPathFromEast', start)[-1]
                    self.assertIn('call_if_eq VAR_0x8004, 11, EC_RivalDexNavTutorial_JoinPathFromEast', self.scripts)
                approach = self.walk('EC_RivalDexNavTutorial_Movement_Approach', start)
                self.assertEqual(approach[-1], (10, 16))
                self.assertNotIn((10, 14), approach)
                player = self.walk('EC_RivalDexNavTutorial_Movement_Follow', approach[-1])
                self.assertEqual(player[-1], (10, 12))
                for step in range(max(len(rival), len(player))):
                    self.assertNotEqual(rival[min(step, len(rival)-1)], player[min(step, len(player)-1)])

    def test_rival_exit_after_capture_stays_on_walkable_tiles(self):
        self.assertEqual(self.walk('EC_RivalDexNavTutorial_Movement_Leave', (14, 12))[-1], (14, 15))

    def test_rival_waits_offscreen_until_the_player_approaches(self):
        route = json.loads((ROOT / 'data/maps/Route101/map.json').read_text())
        rival = next(o for o in route['object_events'] if o.get('local_id') == 'LOCALID_ROUTE101_RIVAL_TUTORIAL')
        self.assertEqual((rival['x'], rival['y']), (10, 14))
        self.assertEqual(rival['movement_type'], 'MOVEMENT_TYPE_FACE_DOWN')
        # NPCs on a connected map are instantiated at its boundary. Keep this
        # waiting spot beyond the entering camera, then approach normally.
        self.assertGreaterEqual(19 - rival['y'], 5)
        entrance = self.scripts.split('EC_RivalDexNavTutorial_FromEntrance:', 1)[1].split('EC_RivalDexNavTutorial_AtGrass::', 1)[0]
        self.assertNotIn('setobjectxy', entrance)
        self.assertNotIn('addobject', entrance)
        self.assertLess(entrance.index('Movement_Approach'), entrance.index('Text_ComeAlong'))
