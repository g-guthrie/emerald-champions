#!/usr/bin/env python3
"""Owned infection graphs preserve budgets and cannot manufacture preparation."""
import copy,json,unittest
import battle_opening_arsenal as opening
import battle_campaign_arsenal as campaign

class CampaignInfectionTests(unittest.TestCase):
    def scenario(self,plan,**kw):
        return campaign._campaign_scenario('TRAINER_ROXANNE_1','hard',stage='after_woods',
            opening_parameters=kw.pop('opening_parameters',{'preparation':{}}),campaign_preparation=plan,**kw)
    def test_direct_fe_after_actual_acquisition_and_evolution_defaults(self):
        plan={'mom_old_rod':True,'captures':[
          {'id':'riolu','species':'SPECIES_RIOLU','map':'Route116','xy':[4,13],'evolved_to':'SPECIES_LUCARIO'},
          {'id':'woobat','species':'SPECIES_WOOBAT','map':'Seaspray_Cave','xy':[9,30],'evolved_to':'SPECIES_SWOOBAT'},
          {'id':'fish','species':'SPECIES_CORPHISH','map':'Route102','xy':[39,2],'method':'old_rod','facing':'RIGHT'}],
          'pokerus':{'direct':['riolu','woobat','fish']}}
        s=self.scenario(plan);state=s['acquisition_states'][0]
        self.assertEqual([state['owned_monsters'][i]['species']for i in ['riolu','woobat','fish']],['SPECIES_LUCARIO','SPECIES_SWOOBAT','SPECIES_CORPHISH'])
        self.assertEqual([state['pokemon_defaults_by_id'][i]['pokerus']for i in ['riolu','woobat','fish']],[254]*3)
        no_infection=copy.deepcopy(plan);no_infection.pop('pokerus')
        before=self.scenario(no_infection)
        self.assertEqual(s['economy'],before['economy'])
        self.assertEqual(state['resources'],before['acquisition_states'][0]['resources'])
        self.assertEqual(state['pokemon_defaults_by_id']['riolu']['ivs'],[31]*6)
        self.assertEqual(state['pokemon_defaults_by_id']['riolu']['evs'],[252,52,52,52,52,50])
        campaign.certify_scenario(json.loads(json.dumps(s)))
    def test_existing_donor_budget_and_non_propagating_recipients(self):
        args={'preparation':{'pokerus':{'direct':['starter-0']}}}
        plan={'captures':[{'id':'one','species':'SPECIES_RIOLU','map':'Route116','xy':[4,13]},
                          {'id':'two','species':'SPECIES_WOOBAT','map':'Seaspray_Cave','xy':[9,30]}],
              'pokerus':{'transmissions':[{'donor':'starter-0','recipient':'one'},{'donor':'starter-0','recipient':'two'}]}}
        s=self.scenario(plan,opening_parameters=args);d=s['acquisition_states'][0]['pokemon_defaults_by_id']
        self.assertEqual([d[i]['pokerus']for i in ['starter-0','one','two']],[252]*3)
        for row in [{'donor':'starter-0','recipient':'starter-1'},{'donor':'one','recipient':'starter-1'}]:
            forged=copy.deepcopy(plan);forged['pokerus']['transmissions'].append(row)
            with self.assertRaises(ValueError):self.scenario(forged,opening_parameters=args)
        one=copy.deepcopy(plan);one['pokerus']['transmissions']=one['pokerus']['transmissions'][:1]
        self.assertEqual(self.scenario(one,opening_parameters=args)['acquisition_states'][0]['pokemon_defaults_by_id']['starter-0']['pokerus'],253)
    def test_direct_reinfection_unknown_id_and_treatment_forgery_fail(self):
        args={'preparation':{'pokerus':{'direct':['starter-0']}}}
        invalid=[{'direct':['starter-0']},{'direct':['not-owned']},{'direct':['starter-1','starter-1']},
                 {'direct':[{}]},{'hot_spring':'starter-1'},{'byte':251},
                 {'transmissions':[{'donor':'starter-0','recipient':'starter-0'}]}]
        for plan in invalid:
            with self.subTest(plan=plan),self.assertRaises(ValueError):self.scenario({'pokerus':plan},opening_parameters=args)
        for status in [252,253,254]:
            defaults={'id':{'pokerus':status}}
            with self.assertRaises(ValueError):opening.apply_owned_pokerus_plan({'id':{}},defaults,{'direct':['id']})
    def test_trade_reset_zero_can_be_infected_without_copying_donor_history(self):
        # Native NPC trade creation gives a new PID/OT and clears infection.
        owned={'fidough-trade':{'species':'SPECIES_FIDOUGH','method':'npc_trade'}}
        default={'nature':'NATURE_RELAXED','pokerus':0,'pp_bonuses':0,'ivs':[31]*6,'evs':[252,52,52,52,52,50]}
        defaults={'fidough-trade':copy.deepcopy(default)}
        opening.apply_owned_pokerus_plan(owned,defaults,{'direct':['fidough-trade']})
        self.assertEqual(defaults['fidough-trade'],{**default,'pokerus':254})
    def test_invalid_graph_is_atomic_and_has_no_new_money_or_stat_service(self):
        owned={'one':{},'two':{},'three':{},'four':{}}
        defaults={i:{'pokerus':0}for i in owned};before=copy.deepcopy(defaults)
        plan={'direct':['one'],'transmissions':[{'donor':'one','recipient':i}for i in ['two','three','four']]}
        with self.assertRaises(ValueError):opening.apply_owned_pokerus_plan(owned,defaults,plan)
        self.assertEqual(defaults,before)
        s=self.scenario({'pokerus':{'direct':['starter-1']}})
        self.assertNotIn('pokerus',s['acquisition_states'][0]['services'])
        proof=s['acquisition_states'][0]['campaign_acquisition_proofs'][-1]
        self.assertEqual(proof['minimum_favorable_battles'],1)
        self.assertIn('withdraw the next',proof['conditions'])
    def test_legacy_species_states_reject_infection_and_certificate_forgery(self):
        with self.assertRaises(ValueError):self.scenario({'pokerus':{'direct':['SPECIES_TREECKO']}},opening_parameters={})
        s=self.scenario({'pokerus':{'direct':['starter-1']}})
        for status in [0,251,248,253]:
            forged=copy.deepcopy(s);forged['acquisition_states'][0]['pokemon_defaults_by_id']['starter-1']['pokerus']=status
            with self.assertRaises(ValueError):campaign.certify_scenario(forged)
    def test_opening_transmission_retains_its_budget_and_plain_state_stays_uninfected(self):
        s=opening._opening_scenario(preparation={'pokerus':{'direct':['starter-0'],
            'transmissions':[{'donor':'starter-0','recipient':'starter-1'}]}})
        d=s['acquisition_states'][0]['pokemon_defaults_by_id']
        self.assertEqual([d[i]['pokerus']for i in ['starter-0','starter-1']],[253,252])
        plain=opening._opening_scenario(preparation={})
        self.assertTrue(all(row['pokerus']==0 for row in plain['acquisition_states'][0]['pokemon_defaults_by_id'].values()))
        self.assertEqual(s['economy'],plain['economy'])
    def test_consumed_trade_donor_is_absent_before_later_infection_graph(self):
        args={'preparation':{'captures':[{'id':'donor','species':'SPECIES_ZIGZAGOON'}],
            'selected':['starter-0','starter-1','donor'],'pokerus':{'direct':['donor']}}}
        plan={'trades':[{'trade_id':'INGAME_TRADE_FIDOUGH','outgoing_id':'donor','received_id':'fidough'}],
              'pokerus':{'direct':['fidough']}}
        s=self.scenario(plan,opening_parameters=args);state=s['acquisition_states'][0]
        self.assertNotIn('donor',state['owned_monsters'])
        self.assertEqual(state['pokemon_defaults_by_id']['fidough']['pokerus'],254)
        self.assertEqual(state['pokemon_defaults_by_id']['fidough']['personality'],132)
        self.assertEqual(state['pokemon_defaults_by_id']['fidough']['ot_id'],38726)
        forged=copy.deepcopy(plan);forged['pokerus']['direct']=['donor']
        with self.assertRaises(ValueError):self.scenario(forged,opening_parameters=args)

if __name__=='__main__':unittest.main()
