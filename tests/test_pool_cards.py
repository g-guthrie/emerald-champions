import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))

import pool_cards  # noqa: E402


class PoolCardsTests(unittest.TestCase):
    def test_legal_moves_come_from_native_table_without_untuned_moves(self):
        moves = pool_cards.legal_moves("SPECIES_MIMIKYU_DISGUISED")
        self.assertIn("MOVE_PLAY_ROUGH", moves)
        self.assertIn("MOVE_SHADOW_SNEAK", moves)
        self.assertFalse(set(moves) & pool_cards.UNTUNED_MOVES)

    def test_card_marks_spread_and_priority(self):
        row = dict(species="SPECIES_MIMIKYU_DISGUISED", stats=[55, 90, 80, 50, 105, 96], bst=476,
                   abilities=["ABILITY_DISGUISE"])
        card = pool_cards.card(row, all_moves=True, per_type=4)
        self.assertIn("Dazzling Gleam(80S,spread)", card["attacks"]["Fairy"])
        self.assertIn("Shadow Sneak(40P,+1)", card["priority"])
        self.assertEqual(card["types"], ["Ghost", "Fairy"])


if __name__ == "__main__":
    unittest.main()
