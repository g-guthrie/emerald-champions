"""Durable trainer assertions stay inside the actual reserved save-flag range."""
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))
import run_emerald_champions_campaign as campaign


class TrainerFlagIntegrityTests(unittest.TestCase):
    def test_reserved_range_excludes_none_negative_and_overflow_ids(self):
        aliases = campaign.trainer_defeat_flag_aliases(
            {"TRAINER_NONE": 0, "TRAINER_NEGATIVE": -1, "TRAINER_FIRST": 1,
             "TRAINER_LAST": 3, "TRAINER_OUTSIDE": 4}, 0x500, 0x503, set(),
        )
        self.assertEqual(aliases, {
            "FLAG_DEFEATED_TRAINER_FIRST": 0x501,
            "FLAG_DEFEATED_TRAINER_LAST": 0x503,
        })
        with self.assertRaisesRegex(RuntimeError, "reserved trainer flag range"):
            campaign.trainer_defeat_flag_aliases({}, 0x503, 0x500, set())

    def test_alias_cannot_shadow_an_authored_declaration(self):
        with self.assertRaisesRegex(RuntimeError, "collides"):
            campaign.trainer_defeat_flag_aliases(
                {"TRAINER_FIRST": 1}, 0x500, 0x503, {"FLAG_DEFEATED_TRAINER_FIRST"},
            )

    def test_real_opponents_resolve_without_special_trainer_namespace(self):
        constants = campaign.parse_numeric_constants()
        self.assertNotIn("FLAG_DEFEATED_TRAINER_NONE", constants)
        self.assertNotIn("FLAG_DEFEATED_TRAINER_EREADER", constants)
        self.assertNotIn("FLAG_DEFEATED_TRAINER_CLASS_HIKER", constants)


if __name__ == "__main__":
    unittest.main()
