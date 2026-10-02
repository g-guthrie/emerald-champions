#!/usr/bin/env python3
"""Retained parties cannot manufacture later access or consume target rewards."""
import copy
import json
import unittest
from battle_campaign_arsenal import _campaign_scenario, certify_scenario, profile, EarlyGeometry, campaign_evolution
from battle_opening_arsenal import _opening_scenario, number, rules

def nature_id(nature):return number('include/constants/pokemon.h',nature)

class CampaignArsenalTests(unittest.TestCase):
    def test_source_profiles_preserve_earlier_resources_and_real_early_caps(self):
        args={'generation':3,'first':0,'second':1,'captures':['SPECIES_WURMPLE'],
              'evolutions':{'SPECIES_WURMPLE':'SPECIES_BEAUTIFLY'}}
        opening=_opening_scenario('hard',**args)
        for stage,trainer in [('after_rival','TRAINER_CALVIN_1'),('after_wally','TRAINER_BILLY'),
                              ('after_wally','TRAINER_GRUNT_PETALBURG_WOODS'),('after_woods','TRAINER_ROXANNE_1')]:
            s=_campaign_scenario(trainer,'hard',stage=stage,opening_parameters=args)
            self.assertEqual(s['battle_access_status'],'proven')
            self.assertEqual(s['level_cap'],14)
            self.assertEqual(s['acquisition_states'][0]['resources'],opening['acquisition_states'][0]['resources'])
            self.assertEqual(s['economy'],opening['economy'])
            self.assertEqual(s['maximal_arsenal_status'],'unresolved')
            self.assertFalse(s['progression_flags']['FLAG_BADGE01_GET'])
            self.assertFalse(s['progression_flags']['FLAG_RECEIVED_BIKE'])
            if stage!='after_rival':self.assertEqual(s['progression_vars']['VAR_PETALBURG_CITY_STATE'],3)
            certify_scenario(json.loads(json.dumps(s)))
        self.assertEqual(s['battle_field']['environment'],'BATTLE_ENVIRONMENT_BUILDING')

    def test_unproven_and_self_unlocking_targets_stay_unresolved(self):
        for stage,trainer in [('after_wally','TRAINER_ROXANNE_1'),
                              ('after_woods','TRAINER_GRUNT_PETALBURG_WOODS'),
                              ('after_woods','TRAINER_MAY_RUSTBORO_TREECKO'),
                              ('after_rival','TRAINER_WATTSON_1')]:
            s=_campaign_scenario(trainer,'medium',stage=stage)
            self.assertEqual(s['battle_access_status'],'unresolved')
            self.assertTrue(s['access_unresolved'])
        s=_campaign_scenario('TRAINER_CALVIN_1','easy',stage='after_rival',player_xy=[0,0])
        self.assertEqual(s['battle_access_status'],'unresolved')
        with self.assertRaises(ValueError):_campaign_scenario('TRAINER_WATTSON_1','hard',stage='late_game')

    def test_forged_progression_or_equipment_fails_recertification(self):
        original=_campaign_scenario('TRAINER_ROXANNE_1','hard',stage='after_woods')
        for mutate in (lambda s:s['progression_flags'].update(FLAG_BADGE01_GET=True),
                       lambda s:s['progression_vars'].update(VAR_PETALBURG_CITY_STATE=5),
                       lambda s:s['acquisition_states'][0]['resources'].update(ITEM_MEGA_RING=1),
                       lambda s:s.update(level_cap=40)):
            s=copy.deepcopy(original);mutate(s)
            with self.assertRaises(ValueError):certify_scenario(s)

    def test_reached_counter_captures_and_map_evolution_have_finite_joint_ownership(self):
        # Eevee is a Route 117 encounter, outside these early maps. The retained
        # branching evolver is an opening Wurmple: its personality branch must
        # agree with the retained nature roll.
        preparation={'evolutions':{'SPECIES_WURMPLE':'SPECIES_BEAUTIFLY'},'captures':[
            {'species':'SPECIES_RIOLU','map':'Route116','xy':[4,13],'nature':'NATURE_MODEST','evolved_to':'SPECIES_LUCARIO'},
            {'species':'SPECIES_WOOBAT','map':'Seaspray_Cave','xy':[9,30],'evolved_to':'SPECIES_SWOOBAT'}]}
        opening={'captures':['SPECIES_WURMPLE'],'natures':{'SPECIES_WURMPLE':'NATURE_CALM'}}
        s=_campaign_scenario('TRAINER_ROXANNE_1','hard',stage='after_woods',
                            opening_parameters=opening,campaign_preparation=preparation)
        state=s['acquisition_states'][0]
        self.assertTrue({'SPECIES_BEAUTIFLY','SPECIES_LUCARIO','SPECIES_SWOOBAT'}<=set(s['candidate_roster']))
        self.assertFalse({'SPECIES_WURMPLE','SPECIES_SILCOON','SPECIES_RIOLU','SPECIES_WOOBAT'}&set(state['resources']))
        retained=[p['evolution'] for p in state['campaign_acquisition_proofs'] if p['method']=='retained_evolution']
        self.assertEqual([p['target_species'] for p in retained],['SPECIES_BEAUTIFLY'])
        witness=retained[0]['personality_witness']
        self.assertEqual(witness%nature_id('NUM_NATURES'),nature_id('NATURE_CALM'))
        self.assertGreater((witness>>16)%10,4) # Silcoon, never Cascoon.
        self.assertEqual(state['pokemon_defaults']['SPECIES_BEAUTIFLY']['nature'],'NATURE_CALM')
        self.assertEqual(state['pokemon_defaults']['SPECIES_LUCARIO']['friendship'],255)
        self.assertEqual(state['pokemon_defaults']['SPECIES_SWOOBAT']['friendship'],255)
        self.assertEqual(state['pokemon_defaults']['SPECIES_LUCARIO']['nature'],'NATURE_MODEST')
        self.assertEqual(s['economy']['purchased_poke_balls'],3)
        self.assertEqual(s['economy']['capture_cost'],600)
        self.assertEqual(s['economy']['ev_fee_reserve'],2500)
        self.assertEqual(s['economy']['cash_remaining_minimum'],2900)
        self.assertEqual(s['maximal_arsenal_status'],'unresolved')
        self.assertEqual(s['preparation_native_status'],'pending_capture_and_evolution_receipts')
        certify_scenario(json.loads(json.dumps(s)))
        altered=copy.deepcopy(s);altered['acquisition_states'][0]['resources']['ITEM_ICE_STONE']=1
        with self.assertRaises(ValueError):certify_scenario(altered)
        # Woods Leafeon still needs an owned Eevee, which these stages cannot hold.
        with self.assertRaisesRegex(ValueError,'owned Eevee'):
            _campaign_scenario('TRAINER_ROXANNE_1','hard',stage='after_woods',opening_parameters=opening,
                               campaign_preparation={'evolutions':{'SPECIES_WURMPLE':'SPECIES_LEAFEON'}})
        # Once Mauville (Route 117's gateway) is reached at the Knuckle Badge
        # cap, Eevee's map evolution is a Leveler step on a reached Woods tile.
        access=profile('after_route110',_opening_scenario('hard'));geometry=EarlyGeometry('after_route110',access)
        self.assertTrue(any(node[0]=='MauvilleCity' for node in geometry.reached))
        leafeon=campaign_evolution('SPECIES_EEVEE','SPECIES_LEAFEON','NATURE_HARDY',rules(),geometry,access)
        self.assertEqual(leafeon['leveler_at_cap'],30)
        self.assertEqual(leafeon['friendship'],0)
        self.assertEqual(leafeon['evolution_map'],'PetalburgWoods')
        self.assertIn(('PetalburgWoods',*leafeon['evolution_xy']),geometry.reached)
        self.assertEqual(leafeon['steps'][0]['conditions'],[['IF_IN_MAP','MAP_PETALBURG_WOODS']])
        with self.assertRaisesRegex(ValueError,'No audited cap-legal evolution path'):
            campaign_evolution('SPECIES_EEVEE','SPECIES_GLACEON','NATURE_HARDY',rules(),geometry,access)
        item_capture=_campaign_scenario('TRAINER_ROXANNE_1','hard',stage='after_woods',campaign_preparation={'captures':[
            {'species':'SPECIES_PARAS','map':'PetalburgWoods','xy':[4,10],'held_item':'ITEM_BIG_MUSHROOM'}]})
        self.assertEqual(item_capture['acquisition_states'][0]['resources']['ITEM_BIG_MUSHROOM'],1)

    def test_inaccessible_terrain_future_stones_legends_and_unfunded_preparation_are_rejected(self):
        captures=[{'species':'SPECIES_TAILLOW','map':'Route115','xy':[12,5]},
                  {'species':'SPECIES_POIPOLE','map':'Seaspray_Cave_B1F','xy':[7,7]},
                  {'species':'SPECIES_RIOLU','map':'Route116','xy':[4,13],'held_item':'ITEM_LUCARIONITE'},
                  {'species':'SPECIES_SHAYMIN','map':'Route102','xy':[2,4]}]
        for capture in captures:
            with self.subTest(capture=capture),self.assertRaises(ValueError):
                _campaign_scenario('TRAINER_ROXANNE_1','hard',stage='after_woods',campaign_preparation={'captures':[capture]})
        # No early stage owns an Eevee (Route 117), so its map/stone rules are
        # checked directly: no Woods before Wally, no audited Glaceon route.
        r=rules();opening=_opening_scenario('hard')
        for stage,target,reason in [('after_rival','SPECIES_LEAFEON','Petalburg Woods evolution map is not reached'),
                                    ('after_woods','SPECIES_GLACEON','No audited cap-legal evolution path')]:
            access=profile(stage,opening);geometry=EarlyGeometry(stage,access)
            with self.subTest(stage=stage,target=target),self.assertRaisesRegex(ValueError,reason):
                campaign_evolution('SPECIES_EEVEE',target,'NATURE_HARDY',r,geometry,access)
        with self.assertRaisesRegex(ValueError,'exceed retained cash'):
            _campaign_scenario('TRAINER_ROXANNE_1','hard',stage='after_woods',
                opening_parameters={'captures':['SPECIES_KRICKETOT','SPECIES_WURMPLE','SPECIES_SCATTERBUG','SPECIES_PACHIRISU']},
                campaign_preparation={'captures':[{'species':s,'map':'Route116','xy':[4,13]}
                    for s in ['SPECIES_RIOLU','SPECIES_MAREEP','SPECIES_ROOKIDEE','SPECIES_JOLTIK']]})

    def test_after_roxanne_owns_badge_but_excludes_rusturf_rewards_and_target_win(self):
        s=_campaign_scenario('TRAINER_GRUNT_RUSTURF_TUNNEL','hard',stage='after_roxanne')
        self.assertEqual(s['level_cap'],20)
        self.assertEqual(s['battle_access_status'],'proven')
        self.assertTrue(s['progression_flags']['FLAG_BADGE01_GET'])
        self.assertTrue(s['progression_flags']['FLAG_DEVON_GOODS_STOLEN'])
        self.assertFalse(s['progression_flags']['FLAG_RECOVERED_DEVON_GOODS'])
        self.assertFalse(s['progression_flags']['FLAG_RETURNED_DEVON_GOODS'])
        self.assertFalse(s['progression_flags']['FLAG_RECEIVED_POKENAV'])
        self.assertEqual(s['progression_vars']['VAR_RUSTURF_TUNNEL_STATE'],2)
        self.assertIn('TRAINER_ROXANNE_1',s['preceding_battle_ids'])
        self.assertNotIn('TRAINER_GRUNT_RUSTURF_TUNNEL',s['preceding_battle_ids'])
        self.assertNotIn('ITEM_DEVON_GOODS',s['acquisition_states'][0]['resources'])
        self.assertNotIn('ITEM_LETTER',s['acquisition_states'][0]['resources'])
        self.assertEqual(s['battle_field']['environment'],'BATTLE_ENVIRONMENT_CAVE')
        certify_scenario(json.loads(json.dumps(s)))
        own_win=_campaign_scenario('TRAINER_ROXANNE_1','hard',stage='after_roxanne')
        self.assertEqual(own_win['battle_access_status'],'unresolved')

    def test_farm_ownership_ids_survive_evolution_and_stage_fees_are_funded(self):
        # Litten (Gen 7 pair) is the only opening Pay Day learner; it farms,
        # is boxed, then joins the stage pool. Two Wurmple share a species but
        # keep distinct IDs when one evolves at the stage.
        opening={'generation':7,'first':0,'second':1,'preparation':{'captures':[
            {'id':'moth','species':'SPECIES_WURMPLE','nature':'NATURE_ADAMANT'},
            {'id':'twin','species':'SPECIES_WURMPLE','nature':'NATURE_MODEST'}],
            'selected':['starter-0','moth','twin'],'pay_day_wins':1,'pay_day_farmer':'starter-1',
            'pokerus':{'direct':['moth']}}}
        prep={'evolutions':{'moth':'SPECIES_DUSTOX'},'captures':[
            {'id':'steel','species':'SPECIES_RIOLU','map':'Route116','xy':[4,13],'evolved_to':'SPECIES_LUCARIO'}],
            'selected':['moth','steel','starter-1']}
        s=_campaign_scenario('TRAINER_ROXANNE_1','hard',stage='after_woods',
                            opening_parameters=opening,campaign_preparation=prep)
        state=s['acquisition_states'][0]
        self.assertEqual(state['owned_monsters']['moth']['species'],'SPECIES_DUSTOX')
        self.assertEqual(state['owned_monsters']['twin']['species'],'SPECIES_WURMPLE')
        self.assertEqual(state['owned_monsters']['starter-1']['species'],'SPECIES_LITTEN')
        self.assertEqual(state['resources']['SPECIES_WURMPLE'],1)
        self.assertEqual(state['pokemon_defaults_by_id']['moth']['nature'],'NATURE_ADAMANT')
        self.assertEqual(state['pokemon_defaults_by_id']['twin']['nature'],'NATURE_MODEST')
        self.assertEqual(state['pokemon_defaults_by_id']['moth']['pokerus'],0xFE)
        self.assertEqual(state['owned_monsters']['steel']['species'],'SPECIES_LUCARIO')
        self.assertEqual(state['selected_owned_ids'],['moth','steel','starter-1'])
        self.assertEqual(s['economy']['ev_fee_reserve'],2500) # old three + capture + boxed farmer
        self.assertEqual(s['economy']['cash_remaining_minimum'],2970)
        self.assertEqual(s['candidate_roster'],['SPECIES_DUSTOX','SPECIES_LUCARIO','SPECIES_LITTEN'])
        certify_scenario(json.loads(json.dumps(s)))

    def test_native_validated_letter_ferry_and_brawly_blocker_have_distinct_profiles(self):
        before=_campaign_scenario('TRAINER_BRAWLY_1','hard',stage='after_steven_letter')
        self.assertEqual(before['battle_access_status'],'unresolved')
        self.assertFalse(before['progression_flags']['FLAG_HIDE_SLATEPORT_CITY_BRAWLY'])
        after=_campaign_scenario('TRAINER_BRAWLY_1','hard',stage='after_brawly_meeting')
        self.assertEqual(after['battle_access_status'],'proven')
        self.assertEqual(after['battle_field']['map'],'DewfordTown_Gym')
        self.assertEqual(after['battle_field']['player_xy'],[4,4])
        self.assertEqual(after['level_cap'],20)
        self.assertTrue(after['progression_flags']['FLAG_HIDE_SLATEPORT_CITY_BRAWLY'])
        self.assertFalse(after['progression_flags']['FLAG_BADGE02_GET'])
        self.assertFalse(after['progression_flags']['FLAG_DEFEATED_DEWFORD_GYM'])
        self.assertFalse(after['progression_flags'].get('FLAG_DELIVERED_DEVON_GOODS',False))
        self.assertFalse(after['progression_flags']['FLAG_SYS_RECEIVED_KEYSTONE'])
        resources=after['acquisition_states'][0]['resources']
        self.assertEqual(resources['ITEM_EXPERT_BELT'],1)
        self.assertEqual(resources['ITEM_DEVON_GOODS'],1)
        self.assertEqual(resources['ITEM_DREAM_BALL'],2)
        self.assertNotIn('ITEM_LETTER',resources)
        self.assertNotIn('ITEM_EMBOARITE',resources)
        self.assertNotIn('ITEM_MEGA_RING',resources)
        self.assertNotIn('museum_entry_cost',after['economy'])
        certify_scenario(json.loads(json.dumps(after)))
        forged=copy.deepcopy(before);forged['progression_flags']['FLAG_HIDE_SLATEPORT_CITY_BRAWLY']=True
        with self.assertRaises(ValueError):certify_scenario(forged)

    def test_source_delayed_brawly_caps_do_not_claim_physical_access_or_ring(self):
        for stage,cap,norman in [('after_wattson_brawly_unbeaten',40,4),('after_flannery_brawly_unbeaten',45,5)]:
            s=_campaign_scenario('TRAINER_BRAWLY_1','hard',stage=stage)
            self.assertEqual(s['level_cap'],cap)
            self.assertEqual(s['progression_vars']['VAR_PETALBURG_GYM_STATE'],norman)
            self.assertEqual(s['battle_access_status'],'unresolved')
            self.assertFalse(s['progression_flags']['FLAG_BADGE02_GET'])
            self.assertFalse(s['progression_flags']['FLAG_SYS_RECEIVED_KEYSTONE'])
            self.assertNotIn('ITEM_MEGA_RING',s['acquisition_states'][0]['resources'])
            self.assertEqual(s['economy']['museum_entry_cost'],50)
        own_win=_campaign_scenario('TRAINER_GRUNT_RUSTURF_TUNNEL','hard',stage='after_rusturf')
        self.assertEqual(own_win['battle_access_status'],'unresolved')
        self.assertIn('Target is a required preceding battle; its own win cannot unlock itself',own_win['access_unresolved'])

    def test_indoor_elevation_one_is_ground_while_surf_behavior_stays_blocked(self):
        p=profile('after_brawly_meeting',_opening_scenario('hard'));g=EarlyGeometry('after_brawly_meeting',p)
        ground=('DewfordTown_Gym',2,22);water=('DewfordTown',12,8)
        self.assertEqual(g.word(ground)>>12,1)
        self.assertEqual(g.behavior(ground),0)
        self.assertTrue(g.floor(ground))
        self.assertIn(ground,g.reached)
        self.assertFalse(g.floor(water))
        self.assertNotIn(water,g.reached)

    def test_museum_requires_prior_brawly_and_archie_requires_only_prior_grunt(self):
        blocked=_campaign_scenario('TRAINER_GRUNT_MUSEUM_1','hard',stage='after_brawly_meeting')
        self.assertEqual(blocked['battle_access_status'],'unresolved')
        self.assertIn('Brawly victory must clear the native Museum entrance queue',blocked['access_unresolved'])
        grunt=_campaign_scenario('TRAINER_GRUNT_MUSEUM_1','hard',stage='after_brawly')
        self.assertEqual(grunt['level_cap'],30)
        self.assertEqual(grunt['battle_access_status'],'proven')
        self.assertEqual(grunt['battle_field']['player_xy'],[12,6])
        self.assertEqual(grunt['battle_field']['environment'],'BATTLE_ENVIRONMENT_BUILDING')
        self.assertIn('TRAINER_BRAWLY_1',grunt['preceding_battle_ids'])
        self.assertNotIn('TRAINER_GRUNT_MUSEUM_1',grunt['preceding_battle_ids'])
        self.assertNotIn('TRAINER_ARCHIE_SLATEPORT',grunt['preceding_battle_ids'])
        self.assertFalse(grunt['progression_flags'].get('FLAG_DELIVERED_DEVON_GOODS',False))
        self.assertEqual(grunt['acquisition_states'][0]['resources']['ITEM_DEVON_GOODS'],1)
        self.assertEqual(grunt['economy']['museum_entry_cost'],50)
        missing_grunt=_campaign_scenario('TRAINER_ARCHIE_SLATEPORT','hard',stage='after_brawly')
        self.assertEqual(missing_grunt['battle_access_status'],'unresolved')
        archie=_campaign_scenario('TRAINER_ARCHIE_SLATEPORT','hard',stage='after_museum_grunt')
        self.assertEqual(archie['battle_access_status'],'proven')
        self.assertIn('TRAINER_GRUNT_MUSEUM_1',archie['preceding_battle_ids'])
        self.assertNotIn('TRAINER_ARCHIE_SLATEPORT',archie['preceding_battle_ids'])
        self.assertFalse(archie['progression_flags'].get('FLAG_DELIVERED_DEVON_GOODS',False))
        self.assertNotIn('ITEM_MEGA_RING',archie['acquisition_states'][0]['resources'])
        certify_scenario(json.loads(json.dumps(archie)))
        late=_campaign_scenario('TRAINER_BRAWLY_1','hard',stage='after_wattson_brawly_unbeaten')
        self.assertEqual(late['legality_status'],'unresolved')

    def test_bridge_keeps_the_actor_layer_and_rival_stops_free_northward_access(self):
        access=profile('after_birch_registration',_opening_scenario('hard'))
        g=EarlyGeometry('after_birch_registration',access)
        self.assertIn(('Route110',18,69),g.reached)
        self.assertIn(('Route110',33,57),g.reached)
        self.assertNotIn(('Route110',33,55),g.reached)
        self.assertNotIn(('MauvilleCity',12,19),g.reached)
        # Isolate the actual source bridge and its cycling deck. Treating15
        # as a wildcard would incorrectly permit changing layers here.
        ground={('Route110',x,69)for x in range(15,20)}
        deck=('Route110',16,68)
        self.assertEqual(g.word(deck)>>12,4)
        self.assertEqual(g.word(('Route110',16,69))>>12,15)
        native_floor=g.floor
        g.floor=lambda node:node in ground|{deck}and native_floor(node)
        below=g.walk(('Route110',15,69))
        self.assertTrue(ground<=below)
        self.assertNotIn(deck,below)
        above=g.walk(deck)
        self.assertIn(('Route110',17,69),above)
        self.assertNotIn(('Route110',15,69),above)
        self.assertNotIn(('Route110',19,69),above)

    def test_route110_and_wally_are_separate_target_excluding_cap30_states(self):
        rival=_campaign_scenario('TRAINER_MAY_ROUTE_110_TORCHIC','hard',stage='after_birch_registration')
        self.assertEqual(rival['battle_access_status'],'proven')
        self.assertEqual(rival['battle_field']['player_xy'],[33,56])
        self.assertEqual(rival['battle_field']['environment'],'BATTLE_ENVIRONMENT_PLAIN')
        self.assertEqual(rival['progression_vars']['VAR_REGISTER_BIRCH_STATE'],2)
        self.assertEqual(rival['progression_vars']['VAR_ROUTE110_STATE'],0)
        self.assertIn('TRAINER_ISABEL_1',rival['preceding_battle_ids'])
        self.assertNotIn(rival['trainer_id'],rival['preceding_battle_ids'])
        wrong=_campaign_scenario('TRAINER_MAY_ROUTE_110_MUDKIP','hard',stage='after_birch_registration')
        self.assertEqual(wrong['battle_access_status'],'unresolved')
        before=_campaign_scenario('TRAINER_WALLY_MAUVILLE','hard',stage='after_museum')
        self.assertEqual(before['battle_access_status'],'unresolved')
        wally=_campaign_scenario('TRAINER_WALLY_MAUVILLE','hard',stage='after_route110')
        self.assertEqual(wally['battle_access_status'],'proven')
        self.assertEqual(wally['battle_field']['player_xy'],[8,7])
        self.assertEqual(wally['level_cap'],30)
        self.assertFalse(wally['progression_flags']['FLAG_DEFEATED_WALLY_MAUVILLE'])
        self.assertFalse(wally['progression_flags']['FLAG_BADGE03_GET'])
        self.assertNotIn('TRAINER_WALLY_MAUVILLE',wally['preceding_battle_ids'])
        self.assertNotIn('ITEM_ACRO_BIKE',wally['acquisition_states'][0]['resources'])
        certify_scenario(json.loads(json.dumps(wally)))
        self.assertEqual(_campaign_scenario('TRAINER_WALLY_MAUVILLE','hard',stage='after_route110_and_wally')['battle_access_status'],'unresolved')

    def test_wattson_uses_real_switch_overlay_and_conservative_ben_predecessor(self):
        s=_campaign_scenario('TRAINER_WATTSON_1','hard',stage='after_route110_and_wally')
        self.assertEqual(s['battle_access_status'],'proven')
        self.assertEqual(s['level_cap'],30)
        self.assertEqual(s['battle_field']['player_xy'],[5,3])
        self.assertEqual(s['battle_field']['environment'],'BATTLE_ENVIRONMENT_BUILDING')
        self.assertEqual(s['progression_vars']['VAR_MAUVILLE_GYM_STATE'],1)
        self.assertIn('TRAINER_BEN',s['preceding_battle_ids'])
        self.assertNotIn('TRAINER_VIVIAN',s['preceding_battle_ids'])
        self.assertNotIn('TRAINER_WATTSON_1',s['preceding_battle_ids'])
        self.assertFalse(s['progression_flags']['FLAG_BADGE03_GET'])
        self.assertFalse(s['progression_flags']['FLAG_DEFEATED_MAUVILLE_GYM'])
        self.assertNotIn('ITEM_RAICHUNITE_X',s['acquisition_states'][0]['resources'])
        a=profile('after_route110_and_wally',_opening_scenario('hard'))
        initial=EarlyGeometry('after_route110_and_wally',a)
        self.assertFalse(initial.floor(('MauvilleCity_Gym',4,7)))
        a['vars']['VAR_MAUVILLE_GYM_STATE']=1
        with self.assertRaises(ValueError):EarlyGeometry('after_route110_and_wally',a)
        a['flags']['FLAG_MAUVILLE_GYM_BARRIERS_STATE']=True
        switched=EarlyGeometry('after_route110_and_wally',a)
        self.assertTrue(switched.floor(('MauvilleCity_Gym',4,7)))
        self.assertFalse(switched.floor(('MauvilleCity_Gym',4,11)))
        self.assertIn(('MauvilleCity_Gym',5,3),switched.reached)
        certify_scenario(json.loads(json.dumps(s)))

    def test_bicycle_is_only_an_explicit_free_reached_rydel_transition(self):
        baseline=_campaign_scenario('TRAINER_WALLY_MAUVILLE','hard',stage='after_route110')
        s=_campaign_scenario('TRAINER_WALLY_MAUVILLE','hard',stage='after_route110',campaign_preparation={'rydel_bicycle':True})
        self.assertEqual(s['acquisition_states'][0]['resources']['ITEM_ACRO_BIKE'],1)
        self.assertTrue(s['progression_flags']['FLAG_RECEIVED_BIKE'])
        self.assertFalse(s['progression_flags']['FLAG_DEFEATED_WALLY_MAUVILLE'])
        self.assertEqual(s['economy']['cash_remaining_minimum'],baseline['economy']['cash_remaining_minimum'])
        self.assertNotIn('ITEM_MEGA_RING',s['acquisition_states'][0]['resources'])
        certify_scenario(json.loads(json.dumps(s)))
        for stage in ['after_woods','after_birch_registration']:
            with self.assertRaises(ValueError):
                _campaign_scenario('TRAINER_CALVIN_1','hard',stage=stage,campaign_preparation={'rydel_bicycle':True})
        with self.assertRaises(ValueError):
            _campaign_scenario('TRAINER_WALLY_MAUVILLE','hard',stage='after_route110',campaign_preparation={'rydel_bicycle':'yes'})
        forged=copy.deepcopy(baseline);forged['acquisition_states'][0]['resources']['ITEM_ACRO_BIKE']=1
        with self.assertRaises(ValueError):certify_scenario(forged)

if __name__=='__main__':unittest.main()
