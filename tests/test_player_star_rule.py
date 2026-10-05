import unittest
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))
from player_star_rule import blocked_mega_slots, restricted_exceptions

class PlayerStarTests(unittest.TestCase):
    def test_new_stars_block_a_separate_mega_but_normal_ursaluna_does_not(self):
        restricted = restricted_exceptions() | {"SPECIES_DARKRAI"}
        mega = {"species": "SPECIES_CHARIZARD", "item": "ITEM_CHARIZARDITE_X"}
        for species in ["SPECIES_GHOLDENGO", "SPECIES_URSALUNA_BLOODMOON", "SPECIES_DARKRAI"]:
            self.assertEqual(blocked_mega_slots([mega, {"species": species}], restricted), [0])
        for species in ["SPECIES_URSALUNA", "SPECIES_URSARING", "SPECIES_TEDDIURSA", "SPECIES_GIMMIGHOUL"]:
            self.assertEqual(blocked_mega_slots([mega, {"species": species}], restricted), [])

    def test_self_mega_and_explicitly_inert_stone_are_legal(self):
        restricted = {"SPECIES_DARKRAI"}
        star = {"species": "SPECIES_DARKRAI", "item": "ITEM_DARKRANITE"}
        self.assertEqual(blocked_mega_slots([star], restricted), [])
        other = {"species": "SPECIES_CHARIZARD", "item": "ITEM_CHARIZARDITE_X", "mega_disabled": True}
        self.assertEqual(blocked_mega_slots([star, other], restricted), [])
