"""Earned battery imports reject unrelated or unverified progress evidence."""
import copy
import hashlib
import json
from pathlib import Path
import sys
import tempfile
import unittest

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'scripts'))
import run_emerald_champions_campaign as campaign


class BatteryImportIntegrityTests(unittest.TestCase):
    def setUp(self):
        self.temporary=tempfile.TemporaryDirectory(prefix='ec-earned-save-evidence-')
        self.addCleanup(self.temporary.cleanup)
        self.save=Path(self.temporary.name)/'earned.sav'
        self.save.write_bytes(b'earned cartridge bytes')
        self.metadata={'schema_version':1,'segment':'saved-parent','parent':'president',
                       'save_sha256':hashlib.sha256(self.save.read_bytes()).hexdigest(),
                       'rom_sha256':'a'*64,'artifact_evidence':{'rom':{'verified_immutable':True,'snapshot_sha256':'a'*64}},
                       'expected':{'map':'MAP_DEVON','position':[14,5],'stable_overworld':True,'items':{'ITEM_LETTER':1}},
                       'assertions':{'items':{'ITEM_LETTER':{'actual':1,'expected':1,'passed':True}}}}

    def test_native_import_accepts_finalized_earned_cartridge_contract(self):
        campaign.validate_save_import_metadata(self.save,self.metadata,'saved-parent')

    def test_wrong_parent_and_modified_battery_are_rejected(self):
        with self.assertRaisesRegex(RuntimeError,'earned parent'):
            campaign.validate_save_import_metadata(self.save,self.metadata,'other-parent')
        self.save.write_bytes(b'unrelated cartridge bytes')
        with self.assertRaisesRegex(RuntimeError,'differs'):
            campaign.validate_save_import_metadata(self.save,self.metadata,'saved-parent')

    def test_unfinalized_or_inconsistent_rom_evidence_is_rejected(self):
        for key,value in [('verified_immutable',False),('snapshot_sha256','b'*64)]:
            data=copy.deepcopy(self.metadata);data['artifact_evidence']['rom'][key]=value
            with self.subTest(key=key),self.assertRaisesRegex(RuntimeError,'provenance'):
                campaign.validate_save_import_metadata(self.save,data,'saved-parent')

    def test_missing_or_failed_required_item_assertion_is_rejected(self):
        for evidence in ({},{'actual':0,'passed':False},{'actual':0,'passed':True}):
            data=copy.deepcopy(self.metadata);data['assertions']['items']['ITEM_LETTER']=evidence
            with self.subTest(evidence=evidence),self.assertRaisesRegex(RuntimeError,'earned assertion'):
                campaign.validate_save_import_metadata(self.save,data,'saved-parent')

    def test_unlocated_or_unstable_source_contract_is_rejected(self):
        for field in ('map','position','stable_overworld'):
            data=copy.deepcopy(self.metadata);del data['expected'][field]
            with self.subTest(field=field),self.assertRaisesRegex(RuntimeError,'stable, located'):
                campaign.validate_save_import_metadata(self.save,data,'saved-parent')


if __name__=='__main__':unittest.main()
