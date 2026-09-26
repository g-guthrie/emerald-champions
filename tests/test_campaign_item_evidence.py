"""Native bag quantity assertions preserve existing campaign evidence contracts."""
import json
from pathlib import Path
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'scripts'))
sys.path.insert(0, str(ROOT / 'tests'))
import run_emerald_champions_campaign as campaign


class CampaignItemEvidence(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.constants = campaign.parse_numeric_constants()

    def test_actual_enum_includes_generated_members_and_aliases(self):
        self.assertGreater(self.constants['ITEM_LETTER'], 0)
        self.assertEqual(self.constants['ITEM_DEVON_GOODS'], self.constants['ITEM_DEVON_PARTS'])
        self.assertIn('ITEM_LEVELER', self.constants)
        self.assertNotIn('ITEM_TM01', self.constants)
        self.assertIn('FLAG_BADGE01_GET', self.constants)

    def test_item_manifest_validation(self):
        for value in (0, 1, 999):
            campaign.validate_manifest_symbols([{'id': 'test', 'expected': {'items': {'ITEM_LETTER': value}}}], {}, self.constants)
        for value in (-1, True, 1.5, '1', 65536):
            with self.subTest(value=value), self.assertRaisesRegex(RuntimeError, 'item assertion'):
                campaign.validate_manifest_symbols([{'id': 'test', 'expected': {'items': {'ITEM_LETTER': value}}}], {}, self.constants)
        with self.assertRaisesRegex(RuntimeError, 'unknown item'):
            campaign.validate_manifest_symbols([{'id': 'test', 'expected': {'items': {'ITEM_FAKE': 1}}}], {}, self.constants)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'manifest.json'
            path.write_text(json.dumps({'schema_version': 1, 'segments': [{'id': 'test', 'frames': 1, 'expected': {'items': []}}]}))
            with self.assertRaisesRegex(RuntimeError, 'expected.items'):
                campaign.load_manifest(path)


if __name__ == '__main__':
    unittest.main()
