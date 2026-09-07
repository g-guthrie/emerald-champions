"""Metadata must not crash or silently disable the authored-map audit."""
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))
from verify_emerald_champions_core import read_map_sources


class CoreMapSourcesIntegrity(unittest.TestCase):
    def test_binary_and_appledouble_are_ignored_but_sources_are_checked(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            town = root / "Town"
            town.mkdir()
            (town / "scripts.inc").write_text("giveitem ITEM_TERA_ORB\n")
            (town / "map.json").write_text('{"script": "Town_Event"}')
            for name in ("._scripts.inc", "._map.json", "blockdata.bin", ".DS_Store"):
                (town / name).write_bytes(b"\xff\xfe\x00")
            frlg = root / "Town_Frlg"
            frlg.mkdir()
            (frlg / "scripts.inc").write_text("FRLG_ONLY")
            result = read_map_sources(root)
            self.assertIn("ITEM_TERA_ORB", result)
            self.assertIn("Town_Event", result)
            self.assertNotIn("FRLG_ONLY", result)

    def test_corrupt_authored_source_still_fails(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "scripts.inc").write_bytes(b"\xff")
            with self.assertRaises(UnicodeDecodeError):
                read_map_sources(root)


if __name__ == "__main__":
    unittest.main()
