"""Rescue certificates bind actual factory inputs instead of prepared cap teams."""
import copy
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

ROOT=Path(__file__).resolve().parents[1]
sys.path[:0]=[str(ROOT/'scripts'),str(ROOT/'scripts/playthrough'),str(ROOT/'tools/agent_player')]
import battle_scripted_wild_arsenal as wild
import battle_calibration as calibration
import generate_battle_suite as suite
import battle_driver as driver

class RescueArsenalTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scenario=wild.rescue_scenario(internal_fingerprint='test')

    def test_actual_factory_loadout_is_natural_level_five_before_services(self):
        s=self.scenario
        self.assertEqual(s['level_cap'],14)
        members=s['expected_player_factory']['members']
        self.assertEqual([m['fixed']['level'] for m in members],[5,5])
        self.assertEqual(members[0]['fixed']['moves'],['MOVE_POUND','MOVE_LEER','MOVE_LEAFAGE','MOVE_NONE'])
        for member in members:
            fixed=member['fixed']
            self.assertEqual(fixed['evs'],[0]*6)  # no EVs before the Knuckle Badge
            self.assertEqual(fixed['ivs'],[31]*6)
            self.assertEqual(member['domains']['item'],['ITEM_NONE'])
            self.assertEqual(fixed['pokerus'],0)
        self.assertNotIn('ABILITY_UNBURDEN',members[0]['domains']['ability'])
        self.assertNotIn('ABILITY_SPEED_BOOST',members[1]['domains']['ability'])

    def test_random_domains_are_compact_and_source_specific(self):
        enemy=self.scenario['expected_scripted_wild']['source_member_domains']
        self.assertNotIn('allowed_teams',self.scenario['expected_scripted_wild'])
        self.assertEqual(enemy[0]['domains']['ability'],['ABILITY_QUICK_FEET','ABILITY_RUN_AWAY'])
        self.assertEqual(enemy[1]['domains']['item'],['ITEM_NONE','ITEM_POTION','ITEM_REVIVE'])
        self.assertEqual(len(enemy[0]['domains']['nature']),25)
        self.assertLess(len(json.dumps(self.scenario)),7000)

    def test_order_generation_gender_and_source_are_bound(self):
        for params in [dict(first=1,second=0),dict(generation=4),dict(player_gender='female')]:
            changed=wild.rescue_scenario(**params,internal_fingerprint='test')
            self.assertNotEqual(changed,self.scenario)
            wild.certify_scenario(changed,internal_fingerprint='test')
        for key,value in [('level_cap',5),('player_gender','female'),('source_fingerprint','stale')]:
            forged=copy.deepcopy(self.scenario);forged[key]=value
            with self.assertRaises(ValueError):wild.certify_scenario(forged,internal_fingerprint='test')
        field=self.scenario['battle_field']
        self.assertEqual(field['player_xy'],[6,13])
        self.assertEqual(field['metatile_id'],1)
        self.assertEqual(field['metatile_behavior'],'MB_NORMAL')
        self.assertEqual(field['environment'],'BATTLE_ENVIRONMENT_PLAIN')
        forged=copy.deepcopy(self.scenario)
        forged['battle_field']['environment']='BATTLE_ENVIRONMENT_GRASS'
        with self.assertRaises(ValueError):wild.certify_scenario(forged,internal_fingerprint='test')

    def test_caller_cannot_expand_domains_or_grant_preparation(self):
        for category,key,value in [('domains','item',['ITEM_MEGA_RING']),
                                   ('domains','ability',['ABILITY_UNBURDEN']),
                                   ('fixed','level',14),('fixed','pokerus',0xFC),
                                   ('fixed','ivs',[0]*6)]:
            forged=copy.deepcopy(self.scenario)
            forged['expected_player_factory']['members'][0][category][key]=value
            with self.assertRaises(ValueError):wild.certify_scenario(forged,internal_fingerprint='test')
        exported=wild.rescue_scenario(internal_fingerprint='test')
        exported['expected_player_factory']['members'][0]['domains']['nature'].append('NATURE_RANDOM')
        exported['expected_player_factory']['members'][0]['fixed']['evs'][0]=4
        with self.assertRaises(ValueError):wild.certify_scenario(exported,internal_fingerprint='test')
        self.assertEqual(wild.rescue_scenario(internal_fingerprint='test'),self.scenario)

    def test_native_rows_need_every_fixed_field_and_allowed_random_choice(self):
        domains=self.scenario['expected_player_factory']['members']
        actual=[{**copy.deepcopy(row['fixed']),**{k:v[-1] for k,v in row['domains'].items()}} for row in domains]
        wild.verify_members(domains,actual)
        for key,value in [('level',14),('ability','ABILITY_UNBURDEN'),('pokerus',0xFC),('moves',['MOVE_PROTECT']*4)]:
            forged=copy.deepcopy(actual);forged[0][key]=value
            with self.assertRaises(ValueError):wild.verify_members(domains,forged)

    def test_native_wild_domain_audit_refuses_caller_authored_wildcards(self):
        s=self.scenario
        expected=s['expected_scripted_wild']
        team=[{**copy.deepcopy(row['fixed']),**{k:v[-1] for k,v in row['domains'].items()}}
              for row in expected['source_member_domains']]
        constants=driver.build_constants()
        flags=constants['battletype']
        roster={'battle_type_native':flags['BATTLE_TYPE_FIRST_BATTLE']|flags['BATTLE_TYPE_DOUBLE'],
                'owners':[{'team':team},{'team':[]}]}
        with patch.object(suite,'source_fingerprint',return_value='test'):
            self.assertTrue(driver.verify_scripted_wild_identity(expected,roster,constants,s)['source_domains_verified'])
            forged=copy.deepcopy(s)
            forged['expected_scripted_wild']['source_member_domains'][0]['domains']['item'].append('ITEM_MEGA_RING')
            with self.assertRaises(SystemExit):
                driver.verify_scripted_wild_identity(forged['expected_scripted_wild'],roster,constants,forged)

    def test_factory_suite_accepts_no_manifest_and_rejects_generic_party(self):
        hashes={'native':'test'};fp=suite.digest_bytes(suite.canonical(hashes))
        scenario=wild.rescue_scenario(internal_fingerprint=fp)
        with tempfile.TemporaryDirectory() as scratch:
            index=Path(scratch)/'arsenal.json'
            index.write_text(json.dumps(dict(schema_version=2,source_generated=True,source_fingerprint=fp,scenarios=[scenario])))
            catalogue=suite.generate(index,internal_inputs=hashes)
        self.assertEqual(catalogue['puzzle_count'],1)
        puzzle=catalogue['puzzles'][0]
        with patch.object(suite,'source_fingerprint',return_value=fp):
            self.assertEqual(calibration.validate_puzzle(catalogue,puzzle['puzzle_id'],None),puzzle)
            with self.assertRaisesRegex(ValueError,'prepared-party'):
                calibration.validate_puzzle(catalogue,puzzle['puzzle_id'],{'party':[]})

    def test_private_player_decoder_has_full_combat_and_preparation_state(self):
        constants=driver.build_constants();s=self.scenario
        words=[0]*57;words[:7]=[1,2,3,0,1,0,1]
        for i,row in enumerate(s['expected_player_factory']['members']):
            f=row['fixed'];d=row['domains'];b=7+i*22
            values=[constants['species']['values'][f['species']],f['level'],constants['item']['values'][d['item'][0]],
                    constants['ability']['values'][d['ability'][0]],constants['nature']['values'][d['nature'][0]],f['friendship'],
                    *f['evs'],*f['ivs'],*[constants['move']['values'][m] for m in f['moves']]]
            words[b:b+22]=values
        decoded=driver.decode_rescue_player_factory(words,constants)
        self.assertEqual(decoded['context'],s['expected_player_factory']['context'])
        wild.verify_members(s['expected_player_factory']['members'],decoded['members'])
        wrong=words[:];wrong[1]=6
        with self.assertRaises(SystemExit):driver.decode_rescue_player_factory(wrong,constants)

    def test_seed_transport_omits_preparation_and_requires_factory_readback(self):
        from test_battle_calibration import FakeDriver
        with tempfile.TemporaryDirectory() as scratch:
            native=FakeDriver(Path(scratch)/'run')
            session=json.loads((native.directory/'session.json').read_text())
            session['player_factory']={'status':'verified','roster_sha256':'exact-input'}
            (native.directory/'session.json').write_text(json.dumps(session))
            policy=calibration.ScriptedPolicy({'schema_version':1,'kind':'scripted',
                'decisions':[{'commands':['0:move0@1']}]})
            result=calibration.run_seed(native,self.scenario,None,Path('scenario.json'),Path('build'),policy,1)
            arguments=native.calls[0][1]
            self.assertIn('birch_rescue',arguments)
            self.assertNotIn('--party',arguments)
            self.assertTrue(result['player_factory_verified'])
            self.assertEqual(result['player_factory_sha256'],'exact-input')

if __name__=='__main__':unittest.main()
