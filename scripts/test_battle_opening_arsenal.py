#!/usr/bin/env python3
"""Opening certificates must reject impossible and externally forged grants."""
import copy
import unittest
from battle_opening_arsenal import _opening_scenario, evolution_path, rules, build_opening_scenario, candidate_party, certify_scenario, certify_opening_manifest

class OpeningArsenalTests(unittest.TestCase):
    def test_boxing_duplicate_captures_keep_independent_ownership_and_rolls(self):
        scenario=build_opening_scenario(preparation={'captures':[
            {'id':'eevee-timid','species':'SPECIES_EEVEE','nature':'NATURE_TIMID','evolved_to':'SPECIES_SYLVEON'},
            {'id':'eevee-bold','species':'SPECIES_EEVEE','nature':'NATURE_BOLD'}],
            'selected':['eevee-timid','eevee-bold']})
        state=scenario['acquisition_states'][0]
        self.assertEqual(scenario['candidate_roster'],['SPECIES_SYLVEON','SPECIES_EEVEE'])
        self.assertEqual(state['pokemon_defaults_by_id']['eevee-timid']['nature'],'NATURE_TIMID')
        self.assertEqual(state['pokemon_defaults_by_id']['eevee-bold']['nature'],'NATURE_BOLD')
        self.assertEqual(state['resources']['SPECIES_EEVEE'],1)
        manifest=candidate_party(scenario)
        self.assertEqual([m['availability']['owned_mon_id'] for m in manifest['party']],['eevee-timid','eevee-bold'])
        for change in (lambda p:p[0]['availability'].pop('owned_mon_id'),
                       lambda p:p[1]['availability'].update(owned_mon_id='eevee-timid'),
                       lambda p:p[0]['availability'].update(owned_mon_id='starter-0')):
            forged=copy.deepcopy(manifest);change(forged['party'])
            with self.assertRaises(ValueError):certify_opening_manifest(scenario,forged)

    def test_finite_pay_day_funds_paid_copies_and_native_money_ceiling(self):
        preparation={'captures':[{'id':'cash','species':'SPECIES_EEVEE'}],
                     'selected':['starter-0','starter-1','cash'],
                     'purchases':{'ITEM_CHOICE_SPECS':5},'pay_day_farmer':'cash','pay_day_wins':100}
        scenario=build_opening_scenario(preparation=preparation)
        self.assertEqual(scenario['acquisition_states'][0]['resources']['ITEM_CHOICE_SPECS'],6)
        self.assertEqual(scenario['economy']['cash_remaining_minimum'],3800)
        unfunded=copy.deepcopy(preparation);unfunded['pay_day_wins']=0
        with self.assertRaises(ValueError):build_opening_scenario(preparation=unfunded)
        for item,count in [('ITEM_CHOICE_SPECS',6),('ITEM_LIFE_ORB',1),('ITEM_MEGA_RING',1)]:
            forged=copy.deepcopy(preparation);forged['purchases']={item:count}
            with self.assertRaises(ValueError):build_opening_scenario(preparation=forged)
        preparation['pay_day_wins']=1000000
        self.assertEqual(build_opening_scenario(preparation=preparation)['economy']['cash_remaining_minimum'],999999-7500-1500)

    def test_pickup_has_actual_level_table_and_consumed_sales(self):
        preparation={'captures':[{'id':'loot','species':'SPECIES_ZIGZAGOON'}],
                     'pickup_farmer':'loot','pickup_items':{'ITEM_MOON_STONE':1,'ITEM_SUN_STONE':1,'ITEM_BIG_MUSHROOM':6},
                     'sell_pickup':{'ITEM_BIG_MUSHROOM':6},'purchases':{'ITEM_LEFTOVERS':5}}
        scenario=build_opening_scenario(preparation=preparation)
        resources=scenario['acquisition_states'][0]['resources']
        self.assertEqual(resources['ITEM_MOON_STONE'],1)
        self.assertEqual(resources['ITEM_SUN_STONE'],1)
        self.assertEqual(resources['ITEM_BIG_MUSHROOM'],0)
        self.assertEqual(resources['ITEM_LEFTOVERS'],6)
        self.assertEqual(scenario['economy']['pickup_sales'],7500)
        for mutate in (lambda p:p['pickup_items'].update(ITEM_RARE_CANDY=1),
                       lambda p:p.update(pickup_farmer='starter-0'),
                       lambda p:p['sell_pickup'].update(ITEM_BIG_MUSHROOM=7),
                       lambda p:p['sell_pickup'].update(ITEM_MOON_STONE=1)):
            forged=copy.deepcopy(preparation);mutate(forged)
            with self.assertRaises(ValueError):build_opening_scenario(preparation=forged)

    def test_direct_infection_boxing_and_two_transmission_budget(self):
        preparation={'captures':[{'id':'one','species':'SPECIES_PACHIRISU'},{'id':'two','species':'SPECIES_PACHIRISU'}],
                     'pokerus':{'direct':['starter-0','starter-1'],'transmissions':[
                         {'donor':'starter-0','recipient':'one'}, {'donor':'starter-0','recipient':'two'}]}}
        scenario=build_opening_scenario(preparation=preparation)
        defaults=scenario['acquisition_states'][0]['pokemon_defaults_by_id']
        self.assertEqual([defaults[k]['pokerus'] for k in ['starter-0','starter-1','one','two']],[0xFC,0xFE,0xFC,0xFC])
        manifest=candidate_party(scenario)
        for value in [0,0xFB,0xF8,0xFD]:
            forged=copy.deepcopy(manifest);forged['party'][1]['pokerus']=value
            with self.assertRaises(ValueError):certify_opening_manifest(scenario,forged)
        for mutate in (lambda p:p['pokerus']['transmissions'].append({'donor':'starter-0','recipient':'starter-1'}),
                       lambda p:p['pokerus'].update(treatment='hot_spring'),
                       lambda p:p['pokerus']['direct'].append('starter-0')):
            forged=copy.deepcopy(preparation);mutate(forged)
            with self.assertRaises(ValueError):build_opening_scenario(preparation=forged)

    def test_plan_certificate_recomputes_owned_rolls_inventory_and_infection(self):
        scenario=build_opening_scenario(preparation={'pokerus':{'direct':['starter-0']}})
        for mutate in (lambda s:s['acquisition_states'][0]['pokemon_defaults_by_id']['starter-0'].update(pokerus=0xFB),
                       lambda s:s['acquisition_states'][0]['resources'].update(ITEM_LEFTOVERS=6),
                       lambda s:s['acquisition_states'][0]['selected_owned_ids'].append('not-owned')):
            forged=copy.deepcopy(scenario);mutate(forged)
            with self.assertRaises(ValueError):certify_scenario(forged)
        with self.assertRaises(ValueError):build_opening_scenario(preparation={'cash':10000})

    def test_alias_is_script_branch_not_unchosen_species_name(self):
        # Pair grass/fire leaves water; BufferRivalBranch maps water2 to branch1.
        scenario=build_opening_scenario('hard',generation=3,first=0,second=1)
        self.assertEqual(scenario['trainer_id'],'TRAINER_MAY_ROUTE_103_TORCHIC')
        self.assertEqual(scenario['candidate_roster'],['SPECIES_TREECKO','SPECIES_TORCHIC'])
        self.assertEqual(scenario['level_cap'],14)
        self.assertEqual(scenario['acquisition_states'][0]['pokemon_defaults']['SPECIES_TREECKO']['evs'],[252,52,52,52,52,50])
        self.assertEqual(scenario['battle_field']['player_xy'],[11,3])
        self.assertEqual(scenario['battle_field']['environment'],'BATTLE_ENVIRONMENT_GRASS')
        self.assertEqual(scenario['maximal_arsenal_status'],'unresolved')
        self.assertNotIn('FLAG_DEFEATED_RIVAL_ROUTE103',scenario['progression_flags'])

    def test_source_factory_refuses_external_grants_and_flags(self):
        scenario=build_opening_scenario('medium')
        for mutate in (
            lambda s:s['acquisition_states'][0]['resources'].update(ITEM_MEGA_RING=1),
            lambda s:s['acquisition_states'][0]['services'].append('nature'),
            lambda s:s['progression_flags'].update(FLAG_BADGE01_GET=True),
            lambda s:s['battle_field'].update(environment='BATTLE_ENVIRONMENT_PLAIN'),
            lambda s:s['economy'].update(starting_cash=600000),
        ):
            forged=copy.deepcopy(scenario);mutate(forged)
            with self.assertRaises(ValueError):certify_scenario(forged)
        for captures in (['SPECIES_KYOGRE'],['SPECIES_POOCHYENA']*2):
            with self.assertRaises(ValueError):build_opening_scenario(captures=captures)
        with self.assertRaises(ValueError):build_opening_scenario(first=1,second=1)

    def test_natural_rolls_preserved_and_later_services_not_assumed(self):
        scenario=build_opening_scenario(natures={'SPECIES_TORCHIC':'NATURE_TIMID'})
        manifest=candidate_party(scenario)
        certify_opening_manifest(scenario,manifest)
        forged=copy.deepcopy(manifest);forged['party'][1]['nature']='NATURE_MODEST'
        with self.assertRaises(ValueError):certify_opening_manifest(scenario,forged)
        for key,value in (('pp_bonuses',255),('pokerus',1),('friendship',123)):
            forged=copy.deepcopy(manifest);forged['party'][0][key]=value
            with self.assertRaises(ValueError):certify_opening_manifest(scenario,forged)
        state=scenario['acquisition_states'][0]
        self.assertEqual(state['pp_bonuses_values'],[0])
        self.assertNotIn('nature',state['services'])
        self.assertNotIn('ivs',state['services'])
        self.assertGreaterEqual(scenario['economy']['cash_remaining_minimum'],0)

    def test_player_ev_edits_do_not_mutate_acquisition_defaults(self):
        scenario=build_opening_scenario()
        manifest=candidate_party(scenario)
        manifest['party'][0]['evs'][0]=0
        self.assertEqual(scenario['acquisition_states'][0]['pokemon_defaults']['SPECIES_TREECKO']['evs'][0],252)
        self.assertEqual(rules()['evs'][0],252)
        certify_opening_manifest(scenario,manifest)  # Funded EV editing is legal.
        manifest['party'][0]['ivs'][0]=0
        with self.assertRaises(ValueError):certify_opening_manifest(scenario,manifest)

    def test_evolution_paths_consume_one_base_and_preserve_nature(self):
        for base,target in [('SPECIES_EEVEE','SPECIES_SYLVEON'),('SPECIES_EEVEE','SPECIES_ESPEON'),
                            ('SPECIES_EEVEE','SPECIES_UMBREON'),('SPECIES_WURMPLE','SPECIES_BEAUTIFLY'),
                            ('SPECIES_WURMPLE','SPECIES_DUSTOX'),('SPECIES_SCATTERBUG','SPECIES_VIVILLON')]:
            scenario=_opening_scenario(captures=[base],natures={base:'NATURE_TIMID'},evolutions={base:target})
            # Certificates must survive serialization; tuples in native parsed
            # conditions cannot differ from their JSON-list representation.
            import json
            certify_scenario(json.loads(json.dumps(scenario)))
            state=scenario['acquisition_states'][0]
            self.assertNotIn(base,state['resources'])
            self.assertEqual(state['resources'][target],1)
            self.assertEqual(state['pokemon_defaults'][target]['nature'],'NATURE_TIMID')
            self.assertEqual(sum(state['resources'].get(s,0) for s in scenario['candidate_roster']),3)
            if base=='SPECIES_EEVEE':self.assertEqual(state['pokemon_defaults'][target]['friendship'],255)
        for base,target in [('SPECIES_EEVEE','SPECIES_LEAFEON'),('SPECIES_EEVEE','SPECIES_GLACEON'),
                            ('SPECIES_PAWMI','SPECIES_PAWMO'),('SPECIES_TREECKO','SPECIES_GROVYLE')]:
            with self.assertRaises(ValueError):evolution_path(base,target,'NATURE_HARDY',rules())

if __name__=='__main__':unittest.main()
