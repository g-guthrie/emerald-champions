"""Native bag quantity assertions preserve existing campaign evidence contracts."""
import copy
import json
from pathlib import Path
import re
import sys
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'scripts'))
sys.path.insert(0, str(ROOT / 'tests'))
import run_emerald_champions_campaign as campaign
import verify_emerald_champions_campaign_run as verifier
from test_campaign_evidence_integrity import valid_run


class CampaignItemEvidence(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.constants = campaign.parse_numeric_constants()

    def test_actual_enum_includes_generated_members_and_aliases(self):
        self.assertGreater(self.constants['ITEM_LETTER'], 0)
        self.assertEqual(self.constants['ITEM_DEVON_GOODS'], self.constants['ITEM_DEVON_PARTS'])
        self.assertIn('ITEM_TM01', self.constants)
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

    def test_query_kind_is_appended_and_native_bag_read_is_used(self):
        header = (ROOT / 'include/emerald_champions_headless.h').read_text()
        names = re.findall(r'EC_HEADLESS_CAMPAIGN_QUERY_\w+', header)
        self.assertEqual(names.index('EC_HEADLESS_CAMPAIGN_QUERY_ITEM'), 4)
        source = (ROOT / 'src/emerald_champions_headless.c').read_text()
        self.assertRegex(source, r'EC_HEADLESS_CAMPAIGN_QUERY_ITEM\)\s+gEcHeadlessCampaignQueryValue = CountTotalItemQuantityInBag\(gEcHeadlessCampaignQueryId\);')
        addresses = {'gEcHeadlessCampaignQueryId': 100, 'gEcHeadlessCampaignQueryKind': 104, 'gEcHeadlessCampaignQueryValue': 108}
        with patch.object(campaign, 'run_state_chunk', return_value=({'gEcHeadlessCampaignQueryValue': 1}, '')) as native:
            value, _ = campaign.query_campaign_value(kind=4, identifier=self.constants['ITEM_LETTER'],
                runner=Path('runner'), rom=Path('rom'), state=Path('state'), addresses=addresses)
        self.assertEqual(value, 1)
        self.assertEqual(native.call_args.kwargs['writes'], [(0, 4, 100, self.constants['ITEM_LETTER']), (0, 4, 104, 4), (0, 4, 108, 0xFFFFFFFF)])
        with patch.object(campaign, 'run_state_chunk', return_value=({'gEcHeadlessCampaignQueryValue': 0xFFFFFFFF}, '')):
            with self.assertRaisesRegex(RuntimeError, 'did not answer'):
                campaign.query_campaign_value(kind=4, identifier=self.constants['ITEM_LETTER'],
                    runner=Path('runner'), rom=Path('rom'), state=Path('state'), addresses=addresses)

    def item_run(self):
        run = valid_run()
        row = run['segments'][0]
        row['expected'] = {'items': {'ITEM_LETTER': 1}}
        row['assertions']['items'] = {'ITEM_LETTER': {'id': self.constants['ITEM_LETTER'], 'expected': 1, 'actual': 1, 'passed': True}}
        return run

    def test_old_reports_remain_compatible_and_items_are_preserved(self):
        self.assertNotIn('items', verifier.normalize_run(valid_run())['segments']['root']['assertions'])
        self.assertEqual(verifier.normalize_run(self.item_run())['segments']['root']['assertions']['items']['ITEM_LETTER']['actual'], 1)

    def test_item_assertions_cannot_be_missing_false_or_retargeted(self):
        for mode in ('missing', 'failed', 'wrong_quantity', 'wrong_expected', 'unrequested'):
            run = self.item_run()
            row = run['segments'][0]
            item = row['assertions']['items']['ITEM_LETTER']
            if mode == 'missing': del row['assertions']['items']
            elif mode == 'failed': item['passed'] = False
            elif mode == 'wrong_quantity': item['actual'] = 0
            elif mode == 'wrong_expected': item.update(expected=2, actual=2)
            else: row['assertions']['items']['ITEM_DEVON_PARTS'] = copy.deepcopy(item)
            with self.subTest(mode=mode), self.assertRaises(RuntimeError):
                verifier.normalize_run(run)


if __name__ == '__main__':
    unittest.main()
