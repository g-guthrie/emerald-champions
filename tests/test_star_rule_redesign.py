"""Native-source pool boundaries for the restricted-slot redesign."""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import reference_pool as rp


class StarRuleAvailability(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.builder = rp.Builder()
        cls.pools = rp.Pools(cls.builder, rp.Encounters(cls.builder)).result

    def first(self, token, resource="items"):
        return next(row["cap"] for row in self.pools.values() if token in row[resource])

    def test_pseudo_mega_stones_wait_for_later_stage(self):
        for item in ("GARCHOMPITE_Z", "TYRANITARITE", "DRAGONINITE", "BAXCALIBRITE"):
            with self.subTest(item=item):
                self.assertGreaterEqual(self.first("ITEM_" + item), 70)
        self.assertIn(self.first("ITEM_KANGASKHANITE"), (60, 65))

    def test_legend_mega_stones_wait_for_the_league(self):
        # A restricted Pokemon may Mega Evolve itself, so its stone is the
        # strongest single pick available; keep it at the final gym or later.
        self.assertEqual(self.first("ITEM_DARKRANITE"), 80)
        self.assertEqual(self.first("ITEM_ZYGARDITE"), 85)

    def test_eastern_ocean_matches_post_hideout_manifest_boundary(self):
        self.assertEqual(self.first("SPECIES_IRON_BUNDLE", "species"), 65)
        self.assertEqual(self.first("SPECIES_SUICUNE", "species"), 65)
        for item in ("MILOTICITE", "MAWILITE", "MASTER_BALL"):
            self.assertEqual(self.first("ITEM_" + item), 65)



if __name__ == "__main__":
    unittest.main()
