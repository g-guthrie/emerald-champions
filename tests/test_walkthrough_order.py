import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))
import walkthrough_order as w

MAPS = ["Route101", "Route102", "PetalburgCity", "PetalburgCity_Gym", "Route104", "Route116",
        "Seaspray_Cave", "RustboroCity_Gym", "Route110", "Route110_TrickHousePuzzle4", "LavaridgeTown"]


class WalkthroughOrderTest(unittest.TestCase):
    def test_route_trainer_sees_only_the_areas_before_its_route(self):
        allowed = w.allowed_maps("Route102", "start", MAPS)
        self.assertIn("Route101", allowed)
        self.assertNotIn("Route102", allowed)  # the route's own Pokemon never count
        self.assertNotIn("PetalburgCity", allowed)
        self.assertNotIn("Route104", allowed)

    def test_side_cave_belongs_to_its_route_chapter(self):
        self.assertNotIn("Seaspray_Cave", w.allowed_maps("Route104", "start", MAPS))
        self.assertNotIn("Seaspray_Cave", w.allowed_maps("Route115", "start", MAPS + ["Route115"]))
        self.assertIn("Seaspray_Cave", w.allowed_maps("DewfordTown", "start", MAPS + ["DewfordTown"]))

    def test_first_rival_battle_counts_its_own_route(self):
        maps = MAPS + ["Route103"]
        self.assertIn("Route103", w.allowed_maps("Route103", "start", maps, "TRAINER_MAY_ROUTE_103_TREECKO"))
        self.assertNotIn("Route103", w.allowed_maps("Route103", "start", maps, "TRAINER_SOMEONE_ELSE"))

    def test_gyms_use_the_full_pool(self):
        self.assertIsNone(w.allowed_maps("RustboroCity_Gym", "start", MAPS))
        self.assertIsNone(w.allowed_maps("PetalburgCity_Gym", "badge4", MAPS))

    def test_revisited_area_follows_the_story_frontier(self):
        allowed = w.allowed_maps("Route110_TrickHousePuzzle4", "badge5", MAPS)
        self.assertIn("LavaridgeTown", allowed)
        self.assertEqual(w.chapter_of("Route110_TrickHousePuzzle4"), w.chapter_of("Route110"))


if __name__ == "__main__":
    unittest.main()
