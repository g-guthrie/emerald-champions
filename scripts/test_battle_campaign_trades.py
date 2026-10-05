#!/usr/bin/env python3
"""Finite trades consume donors and preserve native fixed identity constraints."""
import copy,json,unittest
import battle_campaign_arsenal as draft
class TradeDraftTests(unittest.TestCase):
    def make(self,plan=None,stage='after_woods'):
        opening={'preparation':{'captures':[{'id':'donor','species':'SPECIES_ZIGZAGOON','nature':'NATURE_JOLLY'}],
            'selected':['starter-0','starter-1','donor'],'pokerus':{'direct':['donor']}}}
        if plan is None:plan={'trades':[{'trade_id':'INGAME_TRADE_FIDOUGH','outgoing_id':'donor','received_id':'fidough'}]}
        return draft._campaign_scenario('TRAINER_ROXANNE_1','hard',stage=stage,opening_parameters=opening,campaign_preparation=plan)
    def test_actual_trade_consumes_donor_and_resets_native_recipient(self):
        s=self.make();state=s['acquisition_states'][0]
        self.assertNotIn('donor',state['owned_monsters']);self.assertNotIn('donor',state['pokemon_defaults_by_id'])
        self.assertNotIn('SPECIES_ZIGZAGOON',state['resources'])
        self.assertEqual(state['resources']['SPECIES_FIDOUGH'],1)
        self.assertEqual(state['selected_owned_ids'],['starter-0','starter-1','fidough'])
        p=state['pokemon_defaults_by_id']['fidough']
        self.assertEqual(p['personality'],132);self.assertEqual(p['ot_id'],38726)
        self.assertEqual(p['nature'],'NATURE_RELAXED');self.assertEqual(p['pokerus'],0)
        self.assertEqual(p['friendship'],70);self.assertEqual(p['pp_bonuses'],0)
        self.assertEqual(p['evs'],[0]*6);self.assertEqual(p['ivs'],[31]*6)
        self.assertTrue(s['progression_flags']['FLAG_RUSTBORO_NPC_TRADE_COMPLETED'])
        self.assertEqual(s['economy']['npc_trade_ev_preparation_cost'],500)
        self.assertEqual(s['economy']['cash_remaining_minimum'],3800)
        draft.certify_scenario(json.loads(json.dumps(s)))
    def test_trade_requires_reached_rustboro_and_distinct_actual_donor(self):
        base={'trade_id':'INGAME_TRADE_FIDOUGH','outgoing_id':'donor','received_id':'fidough'}
        for change in [{'outgoing_id':'starter-0'},{'outgoing_id':'missing'},{'received_id':'donor'},{'received_id':None},
                       {'trade_id':'INGAME_TRADE_BOMBIRDIER'},{'nature':'NATURE_MODEST'}]:
            with self.subTest(change=change),self.assertRaises(ValueError):self.make({'trades':[{**base,**change}]})
        with self.assertRaises(ValueError):self.make(stage='after_wally')
        with self.assertRaises(ValueError):self.make({'trades':[base,{**base,'outgoing_id':'fidough','received_id':'second'}]})
    def test_trade_cannot_retain_or_select_consumed_donor(self):
        base={'trade_id':'INGAME_TRADE_FIDOUGH','outgoing_id':'donor','received_id':'fidough'}
        with self.assertRaises(ValueError):self.make({'trades':[base],'selected':['donor','fidough']})
        s=self.make()
        for mutate in [lambda s:s['acquisition_states'][0]['pokemon_defaults_by_id']['fidough'].update(pokerus=254),
                       lambda s:s['acquisition_states'][0]['pokemon_defaults_by_id']['fidough'].update(nature='NATURE_JOLLY'),
                       lambda s:s['progression_flags'].update(FLAG_RUSTBORO_NPC_TRADE_COMPLETED=False),
                       lambda s:s['acquisition_states'][0]['resources'].update(ITEM_LIFE_ORB=1)]:
            forged=copy.deepcopy(s);mutate(forged)
            with self.assertRaises(ValueError):draft.certify_scenario(forged)
    def test_traded_fidough_evolution_needs_real_cap(self):
        plan={'trades':[{'trade_id':'INGAME_TRADE_FIDOUGH','outgoing_id':'donor','received_id':'fidough'}],
              'evolutions':{'fidough':'SPECIES_DACHSBUN'}}
        with self.assertRaises(ValueError):self.make(plan)
        s=self.make(plan,stage='after_brawly');self.assertEqual(s['level_cap'],30)
        self.assertEqual(s['acquisition_states'][0]['owned_monsters']['fidough']['species'],'SPECIES_DACHSBUN')
        self.assertEqual(s['acquisition_states'][0]['pokemon_defaults_by_id']['fidough']['nature'],'NATURE_RELAXED')
if __name__=='__main__':unittest.main()
