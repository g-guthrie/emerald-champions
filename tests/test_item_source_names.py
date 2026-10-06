import subprocess
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class ItemSourceNamesTest(unittest.TestCase):
    def test_labels_and_flags_name_the_item_they_give(self):
        sys.path.insert(0, str(ROOT / "scripts"))
        import item_source_names
        renames, problems = item_source_names.plan()
        self.assertEqual(problems, [])
        self.assertEqual(renames, {}, "run: python3 scripts/item_source_names.py --write")


if __name__ == "__main__":
    unittest.main()
