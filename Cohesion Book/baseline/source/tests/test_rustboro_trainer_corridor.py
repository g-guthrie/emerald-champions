"""Map-data regression for Marc's choke point, including Tommy's approach.

This checks collision and cardinal sight on this static corridor. It does not
simulate the battle engine, trainer defeat flags, or every dynamic gym layout.
"""
import collections
import copy
import json
import struct
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def can_reach_roxanne(marc_direction, defeated=False, tommy_approached=False):
    data = json.loads((ROOT / "data/maps/RustboroCity_Gym/map.json").read_text())
    layout = next(item for item in json.loads((ROOT / "data/layouts/layouts.json").read_text())["layouts"]
                  if item["id"] == data["layout"])
    width, height = layout["width"], layout["height"]
    blocks = struct.unpack(f"<{width * height}H", (ROOT / layout["blockdata_filepath"]).read_bytes())
    walkable = {(x, y) for y in range(height) for x in range(width)
                if not (blocks[y * width + x] >> 10) & 3}
    objects = copy.deepcopy(data["object_events"])
    marc = next(obj for obj in objects if obj["script"].endswith("_Marc"))
    if tommy_approached:
        next(obj for obj in objects if obj["script"].endswith("_Tommy"))["x"] = 2
    walkable -= {(obj["x"], obj["y"]) for obj in objects}
    if not defeated:
        dx, dy = {"MOVEMENT_TYPE_FACE_RIGHT": (1, 0), "MOVEMENT_TYPE_FACE_DOWN": (0, 1)}[marc_direction]
        for distance in range(1, int(marc["trainer_sight_or_berry_tree_id"]) + 1):
            tile = marc["x"] + dx * distance, marc["y"] + dy * distance
            if tile not in walkable:
                break
            walkable.remove(tile)
    pending = collections.deque([(5, 18)])
    visited = {(5, 18)}
    while pending:
        x, y = pending.popleft()
        if (x, y) == (5, 3):
            return True
        for dx, dy in ((0, 1), (0, -1), (1, 0), (-1, 0)):
            tile = x + dx, y + dy
            if tile in walkable and tile not in visited:
                visited.add(tile)
                pending.append(tile)
    return False


class RustboroTrainerCorridor(unittest.TestCase):
    def test_marc_requires_victory_before_the_upper_room(self):
        data = json.loads((ROOT / "data/maps/RustboroCity_Gym/map.json").read_text())
        direction = next(obj["movement_type"] for obj in data["object_events"]
                         if obj["script"].endswith("_Marc"))
        for approached in (False, True):
            with self.subTest(tommy_approached=approached):
                self.assertFalse(can_reach_roxanne(direction, tommy_approached=approached))
                self.assertTrue(can_reach_roxanne(direction, defeated=True, tommy_approached=approached))
                # The reported old south-facing placement must fail this gate.
                self.assertTrue(can_reach_roxanne("MOVEMENT_TYPE_FACE_DOWN", tommy_approached=approached))
