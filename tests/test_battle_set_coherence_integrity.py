"""Negative controls for authored-set checks; prose is not a gameplay rule."""
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))
from verify_emerald_champions_battle_sets import coherence_failures


class BattleSetCoherenceIntegrity(unittest.TestCase):
    def entry(self, **changes):
        entry = dict(species="SPECIES_ALAKAZAM", name="Bulky attacker",
                     moves=["MOVE_PSYCHIC", "MOVE_SHADOW_BALL", "MOVE_RECOVER", "MOVE_PROTECT"],
                     ability="ABILITY_MAGIC_GUARD", item="ITEM_LEFTOVERS",
                     nature="NATURE_MODEST", stat_points=[32, 0, 2, 16, 0, 16],
                     source="Hand-audited", role="bulky wallbreaker")
        entry.update(changes)
        return entry

    def test_authored_mechanical_conflicts_fail(self):
        for changes, message in [
            ({"item": "ITEM_ASSAULT_VEST"}, "Assault Vest prevents"),
            ({"item": "ITEM_LIGHT_CLAY"}, "Light Clay has no screen"),
            ({"item": "ITEM_POWER_HERB"}, "Power Herb has no charge move"),
            ({"item": "ITEM_THROAT_SPRAY"}, "Throat Spray has no sound move"),
            ({"nature": "NATURE_ADAMANT"}, "nature lowers"),
        ]:
            with self.subTest(changes=changes):
                self.assertTrue(any(message in failure for failure in
                                    coherence_failures([self.entry(**changes)])))

    def test_valid_support_and_prose_do_not_fail(self):
        self.assertEqual(coherence_failures([self.entry()]), [])
        self.assertEqual(coherence_failures([self.entry(role="anything", source="anything")]), [])
        # A Choice holder may deliberately use Trick. Body Press uses Defense,
        # so a nature lowering Attack is not an offensive contradiction.
        self.assertEqual(coherence_failures([self.entry(item="ITEM_CHOICE_SCARF",
            moves=["MOVE_PSYCHIC", "MOVE_SHADOW_BALL", "MOVE_TRICK", "MOVE_FOCUS_BLAST"])]), [])
        self.assertEqual(coherence_failures([self.entry(nature="NATURE_BOLD",
            moves=["MOVE_BODY_PRESS", "MOVE_IRON_DEFENSE", "MOVE_REST", "MOVE_SLEEP_TALK"])]), [])


if __name__ == "__main__":
    unittest.main()
