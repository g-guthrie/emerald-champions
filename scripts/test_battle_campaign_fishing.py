#!/usr/bin/env python3
"""Old Rod preparation must use reachable shores and finite source acquisition."""
import copy
import json
import unittest
from unittest.mock import patch
import battle_campaign_arsenal as draft
from battle_opening_arsenal import _opening_scenario,rules

class CampaignFishingTests(unittest.TestCase):
    def make(self,preparation,stage='after_rival',**kw):
        target='TRAINER_CALVIN_1'if stage=='after_rival'else'TRAINER_BILLY'if stage=='after_wally'else'TRAINER_ROXANNE_1'
        return draft._campaign_scenario(target,'hard',stage=stage,campaign_preparation=preparation,**kw)
    def capture(self,species='SPECIES_GOLDEEN',name='Route102',xy=(39,2),facing='RIGHT',**kw):
        return dict(method='old_rod',species=species,map=name,xy=list(xy),facing=facing,**kw)
    def test_finite_mom_handoff_and_three_native_shores(self):
        cases=[('after_rival',self.capture(held_item='ITEM_MYSTIC_WATER')),
               ('after_rival',self.capture('SPECIES_REMORAID','Route103',(21,8))),
               ('after_wally',self.capture('SPECIES_SKRELP','Route104',(14,55),'DOWN'))]
        for stage,capture in cases:
            with self.subTest(stage=stage,capture=capture):
                s=self.make({'mom_old_rod':True,'captures':[capture]},stage)
                state=s['acquisition_states'][0];proof=state['campaign_acquisition_proofs'][-1]
                self.assertEqual(state['resources']['ITEM_OLD_ROD'],1)
                self.assertEqual(proof['old_rod_weights'],[60,40])
                self.assertEqual(proof['wild_level_range'],[5,10])
                self.assertEqual(s['economy']['capture_cost'],200)
                self.assertEqual(s['economy']['ev_fee_reserve'],1500)
                self.assertEqual(s['economy']['cash_remaining_minimum'],4300)
                self.assertEqual(s['progression_vars']['VAR_LITTLEROOT_TOWN_STATE'],4)
                self.assertEqual(s['progression_vars']['VAR_LITTLEROOT_RIVAL_STATE'],4) # Current later demo state is preserved.
                self.assertTrue(s['progression_flags']['FLAG_EC_RIVAL_DEXNAV_TUTORIAL_COMPLETE'])
                self.assertFalse(s['progression_flags']['FLAG_RECEIVED_BIKE'])
                self.assertFalse(s['progression_flags']['FLAG_BADGE01_GET'])
                self.assertIn('native_receipt_refs',proof)
                draft.certify_scenario(json.loads(json.dumps(s)))
        self.assertEqual(self.make({'mom_old_rod':True})['candidate_roster'],['SPECIES_TREECKO','SPECIES_TORCHIC'])
    def test_missing_rod_and_no_early_birch_prefix(self):
        for plan in [{'captures':[self.capture()]},{'mom_old_rod':False,'captures':[self.capture()]}, {'mom_old_rod':1}]:
            with self.assertRaises(ValueError):self.make(plan)
        opening=_opening_scenario();access=draft.profile('after_rival',opening)
        access['flags']['FLAG_RECEIVED_POKEDEX_FROM_BIRCH']=False
        access['maps']=sorted(set(access['maps'])|set(draft.FISHING_EXTRA_MAPS))
        geometry=draft.EarlyGeometry('after_rival',access,extra_maps=draft.FISHING_EXTRA_MAPS)
        with self.assertRaises(ValueError):draft.apply_campaign_preparation(opening,{'mom_old_rod':True},access,geometry,rules())
    def test_wrong_shore_wrong_rod_species_and_no_generic_ocean(self):
        invalid=[self.capture(xy=(1,10)),self.capture(facing='LEFT'),self.capture(facing=None),
                 self.capture('SPECIES_CHINCHOU','Route103',(21,8)),self.capture('SPECIES_CORPHISH','OldaleTown',(1,10)),
                 self.capture('SPECIES_SKRELP','Route104',(14,55),'DOWN'), # Not reached before Wally.
                 {**self.capture(),'method':'good_rod'},self.capture('SPECIES_KYOGRE'),
                 self.capture(held_item='ITEM_LIFE_ORB')]
        for capture in invalid:
            with self.subTest(capture=capture),self.assertRaises(ValueError):self.make({'mom_old_rod':True,'captures':[capture]})
    def test_restricted_fishing_gate_is_not_inferred_from_slot_membership(self):
        actual=draft._species_acquisition_metadata
        def restricted(text):
            rows=copy.deepcopy(actual(text));rows['SPECIES_GOLDEEN']['restricted']=True;return rows
        with patch.object(draft,'_species_acquisition_metadata',restricted),self.assertRaises(ValueError):
            self.make({'mom_old_rod':True,'captures':[self.capture()]})
    def test_owned_ids_and_infection_budget_remain_exact(self):
        opening={'preparation':{'captures':[{'id':'eevee','species':'SPECIES_EEVEE','nature':'NATURE_TIMID'}],
                 'selected':['starter-0','starter-1'],'pokerus':{'direct':['eevee']}}}
        plan={'mom_old_rod':True,'captures':[self.capture('SPECIES_CORPHISH',id='fish')], 'selected':['eevee','fish']}
        s=self.make(plan,opening_parameters=opening);state=s['acquisition_states'][0]
        self.assertEqual(state['pokemon_defaults_by_id']['eevee']['pokerus'],254)
        self.assertEqual(state['pokemon_defaults_by_id']['fish']['pokerus'],0)
        self.assertEqual(state['owned_monsters']['fish']['method'],'old_rod_capture')
        self.assertEqual(state['selected_owned_ids'],['eevee','fish'])
        for ident in ['eevee',None]:
            forged=copy.deepcopy(plan);forged['captures'][0]['id']=ident
            with self.assertRaises(ValueError):self.make(forged,opening_parameters=opening)
        forged=copy.deepcopy(plan);forged['captures'][0]['pokerus']=254
        with self.assertRaises(ValueError):self.make(forged,opening_parameters=opening)
    def test_budget_forgery_and_grants_are_recomputed(self):
        plan={'mom_old_rod':True,'captures':[self.capture()]}
        s=self.make(plan)
        for change in [lambda s:s['acquisition_states'][0]['resources'].update(ITEM_GOOD_ROD=1),
                       lambda s:s['acquisition_states'][0]['resources'].update(ITEM_MYSTIC_WATER=6),
                       lambda s:s['progression_flags'].update(FLAG_BADGE01_GET=True)]:
            forged=copy.deepcopy(s);change(forged)
            with self.assertRaises(ValueError):draft.certify_scenario(forged)
        with self.assertRaises(ValueError):self.make({'mom_old_rod':True,'captures':[self.capture('SPECIES_CORPHISH'),self.capture(),self.capture('SPECIES_REMORAID','Route103',(21,8)),self.capture('SPECIES_SKRELP','Route104',(14,55),'DOWN')]},
            opening_parameters={'captures':['SPECIES_EEVEE','SPECIES_WURMPLE','SPECIES_SCATTERBUG','SPECIES_PACHIRISU']},stage='after_woods')
    def test_default_land_capture_does_not_acquire_rod_or_expand_return_maps(self):
        s=self.make({'captures':[{'species':'SPECIES_RIOLU','map':'Route116','xy':[4,13]}]},stage='after_woods')
        state=s['acquisition_states'][0]
        self.assertNotIn('ITEM_OLD_ROD',state['resources'])
        self.assertEqual(state['campaign_acquisition_proofs'][0]['method'],'land_capture')
        self.assertEqual(s['level_cap'],14)
        self.assertFalse(s['progression_flags']['FLAG_BADGE01_GET'])
        self.assertFalse(s['progression_flags']['FLAG_RECEIVED_BIKE'])
        self.assertNotIn('native_receipt_refs',state['campaign_acquisition_proofs'][0])
        draft.certify_scenario(json.loads(json.dumps(s)))

if __name__=='__main__':unittest.main()
