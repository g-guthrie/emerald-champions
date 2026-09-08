"""Negative controls for world-reward reachability and unique berry rewards."""
import json
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
from verify_mega_stone_rewards import world_reward_sources
from generate_emerald_champions_mega_archive import stones


class MegaStoneRewardRoutes(unittest.TestCase):
    def modified_map(self, name, transform):
        target = ROOT / "data/maps" / name / "map.json"
        payload = json.loads(target.read_text())
        transform(payload)
        replacement = json.dumps(payload)
        original = Path.read_text

        def read(path, *args, **kwargs):
            return replacement if path == target else original(path, *args, **kwargs)

        return patch.object(Path, "read_text", read)

    def test_every_native_stone_has_a_world_reward(self):
        self.assertEqual(set(stones()), set(world_reward_sources()))

    def test_berry_exchange_needs_a_live_npc_entry(self):
        def detach_menu(payload):
            payload["object_events"][0]["script"] = "Route123_BerryMastersHouse_EventScript_DailyBerries"

        with self.modified_map("Route123_BerryMastersHouse", detach_menu):
            with self.assertRaisesRegex(ValueError, "reachable from a world NPC"):
                world_reward_sources()

    def test_another_pickup_cannot_bypass_a_berry_trade(self):
        def add_bypass(payload):
            obj = next(obj for obj in payload["object_events"]
                       if obj["trainer_sight_or_berry_tree_id"] == "ITEM_LOPUNNITE")
            obj["trainer_sight_or_berry_tree_id"] = "ITEM_TYRANITARITE"

        with self.modified_map("PetalburgWoods", add_bypass):
            with self.assertRaisesRegex(ValueError, "bypasses its berry exchange"):
                world_reward_sources()
