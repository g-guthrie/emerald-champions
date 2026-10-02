#!/usr/bin/env python3
"""Focused regression checks for native story guards before tile grouping."""
import unittest
import re
from manifest_geometry import ManifestGeometry
from manifest_world import constants
import mega_register as mr


class ObjectCollision(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.battles=[dict(site_id=name,trainers=['TRAINER_CALVIN_1' if name=='Calvin' else 'TRAINER_'+name.upper()],locations=[dict(map='Route102',entry='Route102_EventScript_'+name,x=x,y=y,trigger='object')])
                     for name,x,y in [('Calvin',33,14),('Rick',25,15),('Tiana',8,7)]]
        cls.rival_variants=[]
        for gender,prefix in [('MALE','MAY'),('FEMALE','BRENDAN')]:
            for branch,starter in enumerate(('TREECKO','TORCHIC','MUDKIP')):
                cls.rival_variants.append(dict(site_id=f'rival110-{prefix}-{starter}',source='data/maps/Route110/scripts.inc:335-377',forced_trigger=True,
                    trainers=[f'TRAINER_{prefix}_ROUTE_110_{starter}'],
                    locations=[dict(map='Route110',x=x,y=56,entry=f'Route110_EventScript_RivalTrigger{x-32}',trigger='coord') for x in (33,34,35)],
                    paths=[dict(event=['Route110',x,56,f'Route110_EventScript_RivalTrigger{x-32}'],conditions=[dict(key='VAR_ROUTE110_STATE',op='eq',value='0'),dict(key='PLAYER_GENDER',op='eq',value=gender),dict(key='SPECIAL:BufferEmeraldChampionsRivalBranch()',op='eq',value=str(branch))]) for x in (33,34,35)]))
        cls.battles.extend(cls.rival_variants)
        cls.geo=ManifestGeometry(cls.battles)
        cls.gym_battles=[]
        for record in cls.geo.object_records:
            d=record['map']
            if d not in ('MossdeepCity_Gym','FortreeCity_Gym','MauvilleCity_Gym'):continue
            text=(mr.MAPS_DIR/d/'scripts.inc').read_text()
            if record['script']+'::' not in text:continue
            body=text.split(record['script']+'::',1)[1].split('\n\n',1)[0]
            trainers=re.findall(r'trainerbattle_\w+\s+(TRAINER_\w+)',body)
            if not trainers:continue
            cls.gym_battles.append(dict(site_id=record['script'],trainers=trainers,locations=[dict(map=d,entry=record['script'],x=record['default']['x'],y=record['default']['y'],trigger='object')]))
        cls.geo.manifest_battles.extend(cls.gym_battles)
        cls.names=constants()

    def state(self,flags=()):
        return dict(flags=set(flags),vars={},defeated=set(),gender=0)

    def walk(self,map_,x,y,state,allowed):
        return {self.geo.nodes[n][0] for n in self.components(map_,x,y,state,allowed)}

    def components(self,map_,x,y,state,allowed):
        geo=self.geo
        flags=state['flags']|geo.flags_for_state(state,self.names)
        todo=list(geo.tile_nodes.get((map_,x,y),[])); reached=set(todo)
        while todo:
            a=todo.pop()
            for b,req,_via in geo.edges.get(a,[]):
                if b not in reached and geo.nodes[b][0] in allowed and mr.holds(req,flags):
                    reached.add(b);todo.append(b)
        return reached

    def test_calvin_sight_victory_unlocks_rick_without_clearing_body(self):
        state=self.state({'FLAG_ADVENTURE_STARTED'})
        allowed={'Route102','OldaleTown','PetalburgCity'}
        state['reached']=self.components('Route102',39,15,state,allowed)
        self.assertTrue(self.geo.battle_approach(self.battles[0],state)&state['reached'])
        self.assertFalse(self.geo.battle_approach(self.battles[1],state)&state['reached'])
        state['defeated'].add('TRAINER_CALVIN_1')
        state['reached']=self.components('Route102',39,15,state,allowed)
        self.assertTrue(self.geo.battle_approach(self.battles[1],state)&state['reached'])
        clear=self.geo.flags_for_state(state,self.names)
        self.assertNotIn(self.geo.token('Route102',2,33,14),clear)
        self.assertIn(self.geo.token('Route102','sight2',33,15),clear)
        state['defeated'].add('TRAINER_RICK')
        state['reached']=self.components('Route102',39,15,state,allowed)
        self.assertIn('PetalburgCity',{self.geo.nodes[n][0] for n in state['reached']})

    def test_petalburg_doors_require_their_room_trainers(self):
        state=self.state();state['vars']['VAR_PETALBURG_GYM_STATE']=6
        allowed={'PetalburgCity_Gym'}
        def close():
            state['reached']=set(self.geo.tile_nodes['PetalburgCity_Gym',4,107])
            for _ in range(12):
                before=set(state['reached'])
                state['reached'] |= self.components('PetalburgCity_Gym',4,107,state,allowed)
                if before==state['reached']: break
        close()
        self.assertTrue(set(self.geo.near('PetalburgCity_Gym',4,81))&state['reached']) # Randall
        self.assertFalse(set(self.geo.near('PetalburgCity_Gym',4,42))&state['reached']) # Parker
        state['defeated'].add('TRAINER_RANDALL');close()
        self.assertTrue(set(self.geo.near('PetalburgCity_Gym',4,42))&state['reached'])
        self.assertFalse(set(self.geo.near('PetalburgCity_Gym',4,16))&state['reached']) # Jody
        state['defeated'].add('TRAINER_PARKER');close()
        self.assertTrue(set(self.geo.near('PetalburgCity_Gym',4,16))&state['reached'])

    def test_sootopolis_ice_has_complete_no_repeat_route_certificates(self):
        proofs=[p for p in self.geo.puzzle_transitions if p['action']=='ice']
        self.assertEqual([p['ice_tiles'] for p in proofs],[7,19,38])
        for proof in proofs:
            route=proof['route']
            self.assertEqual(len(route),len({tuple(p) for p in route}))
            self.assertEqual(len(route),proof['ice_tiles'])
            for a,b in zip(route,route[1:]):self.assertEqual(abs(a[0]-b[0])+abs(a[1]-b[1]),1)
            for x,y in route:self.assertEqual(self.geo.behavior('SootopolisCity_Gym_1F',x,y),self.geo.mb['MB_THIN_ICE'])

    def test_rusturf_grunt_approach_uses_live_story_position(self):
        state=self.state();state['vars']['VAR_RUSTURF_TUNNEL_STATE']=2
        state['reached']=self.components('RusturfTunnel',4,10,state,{'RusturfTunnel'})
        node=dict(site_id='rusturf-grunt',trainers=['TRAINER_AQUA_GRUNT_2'],locations=[dict(map='RusturfTunnel',entry='RusturfTunnel_EventScript_Grunt',x=14,y=5,trigger='object')])
        approaches=self.geo.battle_approach(node,state,constants=self.names)
        self.assertEqual(state['_geometry_object_positions']['RusturfTunnel:6'],[(13,5)])
        self.assertTrue(approaches&state['reached'])

    def test_route110_selector_variants_do_not_block_selected_rival_entry(self):
        state=self.state({'FLAG_HIDE_ROUTE_110_TEAM_AQUA'})
        state['vars'].update(VAR_STARTER_MON=0,VAR_EC_SECOND_STARTER=3,VAR_EC_OPENING_STATE=2)
        state['reached']=self.components('Route110',33,57,state,{'Route110'})
        selected=self.rival_variants[0] # Legacy player-Treecko branch retains Torchic.
        self.assertTrue(self.geo.battle_approach(selected,state,constants=self.names)&state['reached'])
        self.assertFalse(set(self.geo.tile_nodes['Route110',33,56])&state['reached'])
        self.assertTrue(self.geo._conditions(selected['paths'][0]['conditions'],state,self.names))
        self.assertFalse(self.geo._conditions(self.rival_variants[1]['paths'][0]['conditions'],state,self.names))
        state['defeated'].update(selected['trainers']);state['vars']['VAR_ROUTE110_STATE']=1
        state['reached']=self.components('Route110',33,57,state,{'Route110'})
        self.assertTrue(set(self.geo.tile_nodes['Route110',33,56])&state['reached'])

    def test_native_gym_puzzles_reach_every_authored_trainer(self):
        for d,x,y in [('MossdeepCity_Gym',6,35),('FortreeCity_Gym',15,24),('MauvilleCity_Gym',4,20)]:
            with self.subTest(map=d):
                state=self.state();state['reached']=set(self.geo.tile_nodes[d,x,y])
                battles=[b for b in self.gym_battles if b['locations'][0]['map']==d]
                for _ in range(40):
                    before=set(state['reached'])
                    state['reached'] |= self.components(d,x,y,state,{d})
                    eligible=[b for b in battles if not all(t in state['defeated'] for t in b['trainers']) and self.geo.battle_approach(b,state,constants=self.names)&state['reached']]
                    if eligible:state['defeated'].update(eligible[0]['trainers']);continue
                    if before==state['reached']:break
                expected={t for b in battles for t in b['trainers']}
                self.assertTrue(expected <= state['defeated'],expected-state['defeated'])

    def test_devon_guard_blocks_stairs_until_returned_goods(self):
        maps={'RustboroCity_DevonCorp_1F','RustboroCity_DevonCorp_2F','RustboroCity_DevonCorp_3F'}
        initial=self.walk('RustboroCity_DevonCorp_1F',5,8,self.state(),maps)
        self.assertNotIn('RustboroCity_DevonCorp_2F',initial)
        # Badge/merely recovering the goods does not move the guard. The actual
        # OnTransition branch checks RETURNED_DEVON_GOODS.
        recovered=self.walk('RustboroCity_DevonCorp_1F',5,8,self.state({'FLAG_BADGE01_GET','FLAG_RECOVERED_DEVON_GOODS'}),maps)
        self.assertNotIn('RustboroCity_DevonCorp_2F',recovered)
        returned=self.walk('RustboroCity_DevonCorp_1F',5,8,self.state({'FLAG_RETURNED_DEVON_GOODS'}),maps)
        self.assertIn('RustboroCity_DevonCorp_2F',returned)

    def test_oldale_west_exit_uses_adventure_flag(self):
        maps={'OldaleTown','Route101','Route102','Route103','LittlerootTown'}
        initial=self.walk('OldaleTown',7,19,self.state(),maps)
        self.assertNotIn('Route102',initial)
        self.assertIn('Route101',initial)
        self.assertIn('Route103',initial)
        started=self.walk('OldaleTown',7,19,self.state({'FLAG_ADVENTURE_STARTED'}),maps)
        self.assertIn('Route102',started)


if __name__=='__main__': unittest.main()
