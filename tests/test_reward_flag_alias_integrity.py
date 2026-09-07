"""The shared bike receipt is valid; similarly named unrelated flags are not."""
import contextlib
import io
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import verify_emerald_champions_reward_economy as audit


class RewardFlagAliasIntegrity(unittest.TestCase):
    def test_real_bike_gifts_accept_only_the_reviewed_shared_receipt(self):
        source = (ROOT / "data/maps/MauvilleCity_BikeShop/scripts.inc").read_text()
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            path = root / "data/maps/MauvilleCity_BikeShop/scripts.inc"
            path.parent.mkdir(parents=True)
            with patch.object(audit, "ROOT", root), contextlib.redirect_stdout(io.StringIO()):
                path.write_text(source)
                audit.verify_direct_reward_flag_names()
                path.write_text(source.replace("FLAG_RECEIVED_BIKE", "FLAG_RECEIVED_BIKER_REWARD"))
                with self.assertRaisesRegex(SystemExit, "gives ITEM_MACH_BIKE"):
                    audit.verify_direct_reward_flag_names()


if __name__ == "__main__":
    unittest.main()
