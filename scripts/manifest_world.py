#!/usr/bin/env python3
"""Encounter-level field closure. Winning a battle is an explicit transition."""
from __future__ import annotations

import ast
import copy
import json
import re
from collections import defaultdict, deque
from functools import lru_cache
from pathlib import Path

import mega_register as mr
import manifest_native_predicates as native
from manifest_geometry import ManifestGeometry


def constants():
    result = {'TRUE': 1, 'FALSE': 0, 'MALE': 0, 'FEMALE': 1}
    for name, expr in re.findall(r'^\s*(\w+)\s*=\s*([^\n@]+)',(mr.ROOT/'asm/macros/event.inc').read_text(),re.M):
        result[name]=expr.strip()
    for path in sorted((mr.ROOT / 'include').rglob('*.h')):
        text = re.sub(r'/\*.*?\*/|//[^\n]*', '', path.read_text(), flags=re.S)
        text = re.sub(r'__attribute__\s*\(\([^)]*\)\)', '', text)
        for name, value in re.findall(r'^#define\s+(\w+)\s+([^\n]+)', text, re.M):
            if value.strip(): result[name] = value.strip()
        for block in re.findall(r'\benum(?:\s+\w+)?\s*\{(.*?)\}', text, re.S):
            previous = '-1'
            for row in block.split(','):
                match = re.fullmatch(r'\s*(\w+)\s*(?:=\s*(.+))?\s*', row, re.S)
                if not match: continue
                name, value = match.groups()
                result[name] = value or f'({previous}+1)'
                previous = name
    # Script-local numeric `.set` constants (STARTER_COINS in the Game Corner).
    # A name set to different values in different files stays unresolved.
    local={}
    for path in sorted((mr.ROOT/'data').rglob('*.inc')):
        for name, expr in re.findall(r'^\s*\.set\s+(\w+)\s*,\s*(-?\d+)\s*$', path.read_text(), re.M):
            local.setdefault(name,set()).add(int(expr))
    for name, values in local.items():
        if len(values)==1 and name not in result: result[name]=next(iter(values))
    resolved={}
    def number(name,seen=frozenset()):
        if name in resolved:return resolved[name]
        if name in seen:return None
        expression=result.get(name,name)
        if isinstance(expression,int):return expression
        expression=re.sub(r'\b(0x[0-9A-Fa-f]+|\d+)[uUlL]+\b',r'\1',str(expression))
        try:tree=ast.parse(expression,mode='eval').body
        except SyntaxError:return None
        def calc(n):
            if isinstance(n,ast.Constant) and isinstance(n.value,int):return n.value
            if isinstance(n,ast.Name):return number(n.id,seen|{name})
            if isinstance(n,ast.UnaryOp):
                a=calc(n.operand)
                return None if a is None else -a if isinstance(n.op,ast.USub) else ~a if isinstance(n.op,ast.Invert) else a
            if isinstance(n,ast.BinOp):
                a,b=calc(n.left),calc(n.right)
                if a is None or b is None:return None
                if isinstance(n.op,ast.Add):return a+b
                if isinstance(n.op,ast.Sub):return a-b
                if isinstance(n.op,ast.Mult):return a*b
                if isinstance(n.op,(ast.Div,ast.FloorDiv)):return a//b if b else None
                if isinstance(n.op,ast.Mod):return a%b if b else None
                if isinstance(n.op,ast.LShift):return a<<b
                if isinstance(n.op,ast.RShift):return a>>b
                if isinstance(n.op,ast.BitOr):return a|b
                if isinstance(n.op,ast.BitAnd):return a&b
            return None
        try:v=calc(tree)
        except (ValueError,OverflowError,RecursionError):v=None
        if v is not None:resolved[name]=v
        return v
    for name in list(result):number(name)
    result.update(resolved)
    return result


def value(expression, state, names, depth=0):
    if depth > 30: return None
    if isinstance(expression, (int, bool)): return int(expression)
    expression = str(expression)
    if expression.startswith('FLAG_'): return int(mr.canon(expression) in state['flags'])
    if expression.startswith('DEFEATED:'): return int(expression[9:] in state['defeated'])
    if expression == 'PLAYER_GENDER': return state['gender']
    if expression.startswith('VAR_'): return state['vars'].get(expression, 0)
    if expression in names: return value(names[expression], state, names, depth + 1)
    try: return int(expression, 0)
    except ValueError: pass
    try: tree = ast.parse(expression, mode='eval').body
    except SyntaxError: return None
    def evaluate(node):
        if isinstance(node, ast.Constant) and isinstance(node.value, int): return node.value
        if isinstance(node, ast.Name): return value(node.id, state, names, depth + 1)
        if isinstance(node, ast.UnaryOp):
            a = evaluate(node.operand)
            if a is None: return None
            return -a if isinstance(node.op, ast.USub) else ~a if isinstance(node.op, ast.Invert) else a
        if isinstance(node, ast.BinOp):
            a, b = evaluate(node.left), evaluate(node.right)
            if a is None or b is None: return None
            if isinstance(node.op, ast.Add): return a + b
            if isinstance(node.op, ast.Sub): return a - b
            if isinstance(node.op, ast.Mult): return a * b
            if isinstance(node.op, (ast.Div, ast.FloorDiv)): return a // b if b else None
            if isinstance(node.op, ast.Mod): return a % b if b else None
            if isinstance(node.op, ast.LShift): return a << b
            if isinstance(node.op, ast.RShift): return a >> b
            if isinstance(node.op, ast.BitOr): return a | b
            if isinstance(node.op, ast.BitAnd): return a & b
        return None
    try: return evaluate(tree)
    except (ValueError, OverflowError): return None


class World:
    def __init__(self, builder, pokemon, item_data, battle_data, event_data):
        self.builder = builder
        self.geo = ManifestGeometry(battle_data)
        self.story = mr.Story(self.geo)
        self.pokemon, self.items = pokemon, item_data['sources']
        self.battles, self.events = battle_data['battles'], event_data['transitions']
        # Branches of one scene exclude each other once one sets its receipt.
        # The player chooses facing and answers, so try the branch that
        # changes the most state first (Wallace's Waterfall gift moves him off
        # Juan's door only on the branches that also set his position).
        first={}
        for i,event in enumerate(self.events):first.setdefault((event['entry'],tuple(event['event'])),i)
        self.events=sorted(self.events,key=lambda e:(first[(e['entry'],tuple(e['event']))],-len(e.get('final_writes',{}))))
        self.activities={}
        for activity in item_data.get('activities',[]):
            if activity.get('kind')!='three_battle_challenge':continue
            self.activities[activity['id']]=activity
            for fight in activity['fights']:
                where=activity['where']
                self.battles.append(dict(id=f"{activity['id']}:{fight['index']}",
                    site_id=f"{activity['id']}:{fight['index']}",kind='optional_activity_battle',
                    activity_id=activity['id'],activity_index=fight['index'],
                    trainers=[],wild_species=[],paths=[],victory_paths=[],
                    locations=[{'map':where[0],'x':where[1],'y':where[2],'trigger':'activity','entry':activity['id']}],
                    source=fight['source'],label=activity['id'],command='facility_battle',
                    display_name=f"{where[0].split('City')[0].split('Town')[0]} Battle Tent — fight {fight['index']} of 3",
                    dynamic_opponent_pool=activity['opponent_pool'],level_note=activity['level_rule'],
                    activity_requirement=activity['conditions']))
        self.names = native.FlagConstants(constants())
        self.unresolved = defaultdict(set)
        self.approaches = {}
        self.extra_gates = {}
        self.ash_nodes=set()
        if 'Route113' in self.geo.grid:
            width,height,words,_=self.geo.grid['Route113']
            for y in range(height):
                for x in range(width):
                    if words[y*width+x]&1023==0x20A:
                        self.ash_nodes.update(self.geo.tile_nodes.get(('Route113',x,y),()))
        # League room exits are opened by that room's actual defeated flag.
        for map_, flag in [('EverGrandeCity_SidneysRoom', 'FLAG_DEFEATED_ELITE_4_SIDNEY'),
                          ('EverGrandeCity_PhoebesRoom', 'FLAG_DEFEATED_ELITE_4_PHOEBE'),
                          ('EverGrandeCity_GlaciasRoom', 'FLAG_DEFEATED_ELITE_4_GLACIA'),
                          ('EverGrandeCity_DrakesRoom', 'FLAG_DEFEATED_ELITE_4_DRAKE')]:
            self.extra_gates[map_] = flag

    def initial(self):
        state = dict(flags=set(), vars={'VAR_STARTER_GEN':3, 'VAR_STARTER_MON':0,
                    'VAR_EC_SECOND_STARTER':3, 'VAR_EC_OPENING_STATE':2}, defeated=set(),
                    items=set(), species={'SPECIES_TREECKO','SPECIES_MUDKIP'}, caught=set(),
                    maps=set(), reached=set(self.story.seeds), cap=14, gender=0,
                    preparation_possible=True, starter_generation=3, first=0, second=2,
                    legend_unlocked=set(), legend_lost=set(), legend_resting=set(),
                    harvestable_berries=set(), event_history=set(),completed_sites=set(),captured_battle_sites=set(),resolved_battle_sites=set(),zero_unset_vars=True)
        state.update(money=6000,coins=0,pc_items={'ITEM_POTION'},pc_item_counts={'ITEM_POTION':1},funding_possible=False)
        labels = self.builder.scripts.labels
        stack, visited = ['EventScript_ResetAllMapFlags'], set()
        while stack:
            label = stack.pop()
            if label in visited or label not in labels: continue
            visited.add(label)
            for _, line in labels[label]['body']:
                match = re.match(r'(setflag|clearflag|call|goto)\s+(\w+)', line)
                if not match: continue
                op, arg = match.groups()
                if op in ('call','goto'): stack.append(arg)
                elif op == 'setflag': state['flags'].add(mr.canon(arg))
                else: state['flags'].discard(mr.canon(arg))
        # This guide begins after the mandatory level-five Birch rescue.
        # Story state is not advanced past the first rival.
        for flag in ('FLAG_SYS_POKEMON_GET','FLAG_RESCUED_BIRCH','FLAG_EC_PLAYER_IVS_MAXED'):
            state['flags'].add(mr.canon(flag))
        state['caught'].update(state['species'])
        # Route101_EventScript_FinishRescue sets these before the automatic
        # warp back to Birch. The lab's idle dialogue then releases the rival.
        state['vars'].update(VAR_BIRCH_LAB_STATE=2,VAR_ROUTE101_STATE=3,
                             VAR_LITTLEROOT_INTRO_STATE=1,VAR_LITTLEROOT_HOUSES_STATE_BRENDAN=1)
        state['flags'].discard(mr.canon('FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_BIRCH'))
        state['flags'].update(mr.canon(f) for f in ('FLAG_HIDE_ROUTE_101_BIRCH_ZIGZAGOON_BATTLE',
                              'FLAG_HIDE_ROUTE_101_ZIGZAGOON','FLAG_HIDE_ROUTE_101_BIRCH_STARTERS_BAG'))
        return state

    def token(self, token, state):
        if token.startswith('DEFEATED:'): return token[9:] in state['defeated']
        if token.startswith('BADGES>='): return mr.badge_count(state['flags']) >= int(token[8:])
        if token.startswith('NOT:'): return not self.token(token[4:], state)
        if token.startswith('VAR_NE:'):
            _, key, wanted = token.split(':',2)
            return state['vars'].get(key,0) != value(wanted,state,self.names)
        return mr.canon(token) in state['flags']

    def requirements(self, alternatives, state):
        return any(all(self.token(t,state) for t in alt) for alt in (alternatives or [[]]))

    def conditions(self, rows, state, source=''):
        location=re.search(r'data/maps/([^/]+)/',source)
        if location:state=dict(state,current_map=location[1])
        domains={}
        for row in rows:
            key, op, rhs = row['key'], row['op'], row['value']
            right = value(rhs,state,self.names)
            right_domain={right} if right is not None else domains.get(str(rhs),native.evaluate(str(rhs),state,self.names))
            left = value(key,state,self.names)
            if key in domains:
                domain=domains[key]
            elif key == 'VAR_FACING':
                domain=set(state.get('facing_domain') or {1,2,3,4})
            else:
                domain = {left} if left is not None else native.evaluate(key,state,self.names)
            if domain is None or right_domain is None:
                self.unresolved[key].add(source)
                return False
            compare = {'eq':lambda a,b:a==b,'ne':lambda a,b:a!=b,'lt':lambda a,b:a<b,
                       'le':lambda a,b:a<=b,'gt':lambda a,b:a>b,'ge':lambda a,b:a>=b}[op]
            domain={a for a in domain if any(compare(a,b) for b in right_domain if key!=str(rhs) or a==b)}
            if not domain:return False
            domains[key]=domain
        return True

    def reachable(self, where, state):
        if where is None: return True
        if isinstance(where,list): where=tuple(where)
        if where not in self.approaches:
            self.approaches[where] = self.story.locate(where)
        return any(n in state['reached'] for n in self.approaches[where])

    def event_reachable(self,event,entry,state):
        if event[3]=='object':
            actors=self.geo._objects_by_script.get((event[0],entry),[])
            if actors:
                return any(set(self.geo.near(event[0],x,y))&state['reached']
                           for actor in actors
                           for x,y in state.get('_geometry_object_positions',{}).get(f"{event[0]}:{actor['id']}",[]))
        where=(event[0],event[1],event[2]) if event[1] is not None else event[0]
        return self.reachable(where,state)

    def facing_domain(self,event,entry,state):
        """Directions the player can face an NPC from (DIR_SOUTH=1, NORTH=2,
        WEST=3, EAST=4; include/constants/global.h). Only reached neighbour
        tiles count: nobody talks to Wallace from Juan's Gym door."""
        if event[3]!='object':return None
        actors=self.geo._objects_by_script.get((event[0],entry),[])
        result=set()
        for actor in actors:
            for x,y in state.get('_geometry_object_positions',{}).get(f"{event[0]}:{actor['id']}",[]):
                for dx,dy,facing in ((0,1,2),(0,-1,1),(1,0,3),(-1,0,4)):
                    if set(self.geo.tile_nodes.get((event[0],x+dx,y+dy),[]))&state['reached']:result.add(facing)
        return result or None

    @staticmethod
    def battles_completed(prerequisites,state):
        done=state['defeated']|state.get('completed_sites',set())|state.get('resolved_battle_sites',set())
        return all(battle in done or all(part in done for part in battle.split('|')) for battle in prerequisites)

    @staticmethod
    def effect_key(effect):
        return json.dumps(effect,sort_keys=True)

    def walk(self, state):
        state['reached']=set(self.story.seeds)
        queue=deque(state['reached'])
        while queue:
            a=queue.popleft()
            for b, req, via in self.geo.edges[a] + self.story.extra_edges.get(a,[]):
                if b in state['reached'] or not self.requirements(req,state): continue
                amap,bmap=self.geo.nodes[a][0],self.geo.nodes[b][0]
                if amap!=bmap:
                    if not self.requirements(self.story.map_gates.get(bmap),state): continue
                    if self.extra_gates.get(amap) and self.geo.nodes[b][0] != amap:
                        # The return doorway remains usable; the advancing north
                        # exit is the story gate. Its tile source is checked below.
                        if bmap not in ('EverGrandeCity_EliteFourRoom1','EverGrandeCity_PokemonLeague_1F') and mr.canon(self.extra_gates[amap]) not in state['flags']:
                            continue
                state['reached'].add(b);queue.append(b)
        state['maps']={self.geo.nodes[n][0] for n in state['reached']}

    def source_live(self, row, state):
        scope=str(row.get('campaign_scope',''))
        if any(t in scope for t in ('appendix','multiplayer','inactive')) or row.get('eligibility_status') in ('inactive source appendix','unresolved'):return False
        if row.get('ownership_condition')=='successful_capture' and not any(s in state['captured_battle_sites'] for s in row.get('capture_sites',[])):return False
        if row.get('kind') == 'starter':
            if row.get('starter_region') != state['starter_generation'] or row.get('species',row.get('key')) not in state.get('starter_species',{'SPECIES_TREECKO','SPECIES_MUDKIP'}):return False
        stone=row.get('starter_stone_species')
        if stone:
            # Norman's Ring gift holds only the selected pair's stones; a Hoenn
            # partner's stone waits until after his battle for that line to be
            # shown (src/mega_stone_rewards.c, PetalburgCity_Gym NormanPostBattle).
            sd=self.pokemon.sd
            pair=state.get('starter_species',{'SPECIES_TREECKO','SPECIES_MUDKIP'})
            if not any(stone in sd.family(s) for s in pair):
                if stone not in ('SPECIES_SCEPTILE','SPECIES_BLAZIKEN','SPECIES_SWAMPERT'):return False
                if 'TRAINER_NORMAN_1' not in state['defeated'] or not any(stone in sd.family(s) for s in state['species']):return False
        if not self.reachable(row.get('where'),state) or not self.requirements(row.get('requires'),state): return False
        if any(x not in state['species'] for x in row.get('needs_species',[])): return False
        if any(x not in state['items'] for x in row.get('needs_items',[])): return False
        # The manifest covers conditional save variants. Evaluate each lady
        # concretely; never combine their incompatible saved selections.
        if row.get('kind') in ('quiz_prize','favor_prize') and 'lilycove_lady' not in state:
            return any(self.source_live(row,dict(state,lilycove_lady=variant))
                       for variant in native.initial_lady_variants())
        if row.get('kind')=='decoration_trade' and not any(k in state for k in ('mauville_man_id','trainer_id')):
            alternate=dict(state,mauville_man_id=2)
            alternate['decoration_trade_possible']=native.decoration_trade_possible(alternate)
            return self.source_live(row,alternate)
        price=row.get('price')
        if price is not None:
            amounts=native.numeric_domain(price,state,self.names)
            if amounts is None:
                self.unresolved['PRICE:'+str(price)].add(row.get('cite',''));return False
            if not state.get('funding_possible') and not any(state.get('money',0)>=amount for amount in amounts):return False
        cost=row.get('cost',{})
        if cost.get('currency')=='coins':
            coins=native.evaluate('COINS',state,self.names);amount=value(cost.get('amount'),state,self.names)
            if coins is None or amount is None:
                self.unresolved['ACTIVITY:coin purchase funding'].add(row.get('cite',''));return False
            if not any(balance>=amount for balance in coins):return False
        activity=row.get('activity_requirement',{})
        if activity:
            kind=activity['kind'];proof=False
            if kind=='lottery_match':
                # A random daily draw can match the saved OT ID. These offers
                # remain explicitly labelled with their required digit match.
                proof=bool(state.get('caught'))
            elif kind=='tent_battle_run':
                proof=state.get('tent_wins',{}).get(activity.get('town'),0)>=activity.get('wins',3)
            elif kind=='harvest_credit_trade':
                pouch=state.get('harvested_berries',{});renewable=state.get('harvestable_berries',set())
                proof=all(pouch.get(r['item'],pouch.get('BERRY_ID_'+r['item'].removeprefix('ITEM_').removesuffix('_BERRY'),0))>=r['count']
                          or (state.get('preparation_possible') and r['item'] in renewable) for r in activity['recipe'])
            if not proof:
                self.unresolved['ACTIVITY:'+kind].add(row.get('cite',''));return False
        kind=row.get('kind')
        if kind in ('quiz_prize','favor_prize'):
            lady=state.get('lilycove_lady',{});selected=lady.get('quiz' if kind=='quiz_prize' else 'favor',{})
            rows=native._lady_rows()[0 if kind=='quiz_prize' else 1]
            choice=selected.get('question_id' if kind=='quiz_prize' else 'favor_id')
            proof=False
            expected_id=0 if kind=='quiz_prize' else 1
            if lady.get('id')==expected_id and choice in range(len(rows)) and rows[choice]['prize']==row['key']:
                if selected.get('state')==2:proof=True
                elif kind=='quiz_prize':
                    answer=rows[choice]['answer']
                    proof=selected.get('state')==0 and state.get('preparation_possible') and native._easy_word_available(answer,state,self.names)
                else:
                    proof=selected.get('num_items_given',0)>=5
                    if not proof and selected.get('state')==0:
                        best=selected.get('best_item');proof=best in state['items'] if best else False
            if not proof:
                self.unresolved['ACTIVITY:'+kind].add(row.get('cite',''));return False
        if kind in ('decoration_trade','soot_exchange'):
            proof=bool(state.get('decoration_trade_possible')) if kind=='decoration_trade' else bool(state.get('collectable_ash'))
            if not proof:
                self.unresolved['ACTIVITY:'+kind].add(row.get('cite',''));return False
        for condition in row.get('conditions',[]):
            if isinstance(condition,str) and condition.startswith('NOT:') and self.token(condition[4:],state): return False
        paths=row.get('eligibility_paths')
        if paths:
            where=row.get('where');map_name=where[0] if isinstance(where,(tuple,list)) else where
            context=dict(state,current_map=map_name) if map_name else state
            return any(self.battles_completed(p.get('prior_battles',[]),state)
                       and self.conditions(p.get('conditions',[]),context,row.get('cite','')) for p in paths)
        if row.get('eligibility_status') in ('unresolved','no matching live event path found','native_or_script_path_unresolved'): return False
        return True

    def derived_flags(self,state):
        state['flags']={f for f in state['flags'] if not f.startswith(('OBJECT_CLEAR:','SCRIPT_'))}
        state['_geometry_clearance']=self.geo.flags_for_state(state,self.names)
        state['flags'].update(state['_geometry_clearance'])
        # Pay Day farming: the Oldale Center tutor teaches it to any owned
        # learner (Meowth on Route 102, Eevee from Route 117).
        state['funding_possible']='OldaleTown_PokemonCenter_1F' in state['maps'] and any(self.pokemon.can_learn(s,'MOVE_PAY_DAY') for s in state['species'])
        state['collectable_ash']='ITEM_SOOT_SACK' in state['items'] and bool(self.ash_nodes & state['reached'])
        state['decoration_trade_possible']=native.decoration_trade_possible(state)
        for key, needed in mr.FIELD_MOVES.items():
            move='MOVE_'+key.removeprefix('CAN_')
            allowed=all(self.token(t,state) for t in needed) and any(self.pokemon.can_learn_field(s,move) for s in state['species'])
            if allowed: state['flags'].add(key)
            else: state['flags'].discard(key)
        self.rusturf_rock_smash(state)
        # Admission walks the door guards aside and saves that spot
        # (EverGrandeCity_PokemonLeague_1F/scripts.inc:56-71, copyobjectxytoperm);
        # later visits step them aside again. The tile model cannot follow
        # scripted walking, so the admitted door is clear.
        if mr.canon('FLAG_ENTERED_ELITE_FOUR') in state['flags']:
            for record in self.geo.object_records:
                if record['map']=='EverGrandeCity_PokemonLeague_1F' and record['script']=='EverGrandeCity_PokemonLeague_1F_EventScript_DoorGuard':
                    for x,y in record['positions']:state['flags'].add(self.geo.token(record['map'],record['id'],x,y))
        for item, token in [('ITEM_OLD_ROD','HAS_OLD_ROD'),('ITEM_GOOD_ROD','HAS_GOOD_ROD'),('ITEM_SUPER_ROD','HAS_SUPER_ROD')]:
            if item in state['items']: state['flags'].add(token)
        state['cap']=mr.cap_of_flags(state['flags'])

    def rusturf_rock_smash(self,state):
        # Smashing a tunnel rock runs the native hook TryUpdateRusturfTunnelState
        # (data/scripts/field_move_scripts.inc:177, src/field_specials.c:2022).
        # The script graph cannot see it, yet it is what raises the tunnel state
        # for RusturfTunnel_OnFrame's ClearTunnelScene: the Strength license.
        if ('CAN_ROCK_SMASH' not in state['flags'] or mr.canon('FLAG_RUSTURF_TUNNEL_OPENED') in state['flags']
                or state['vars'].get('VAR_RUSTURF_TUNNEL_STATE',0) in (4,5)):return
        for rock in self.geo.maps['RusturfTunnel'].get('object_events',[]):
            flag=str(rock.get('flag'))
            if rock.get('script')!='EventScript_RockSmash' or not flag.startswith('FLAG_HIDE_RUSTURF_TUNNEL_ROCK_'):continue
            if mr.canon(flag) in state['flags'] or not set(self.geo.near('RusturfTunnel',rock['x'],rock['y']))&state['reached']:continue
            state['flags'].add(mr.canon(flag))
            state['vars']['VAR_RUSTURF_TUNNEL_STATE']=4 if flag.endswith('_1') else 5
            return

    def apply(self, effects, state, assignments=None, conditions=None, map_=None,entry=None):
        actors=self.geo._objects_by_script.get((map_,entry),[])
        object_id=actors[0]['id'] if len(actors)==1 else None
        before=(frozenset(state['flags']),tuple(sorted(state['vars'].items())),frozenset(state['items']),frozenset(state['legend_unlocked']))
        for effect in effects:
            op,args=effect['op'],effect['args']
            if op=='setflag':state['flags'].add(mr.canon(args[0]))
            elif op=='clearflag':state['flags'].discard(mr.canon(args[0]))
            # Temp vars reset on every map change, so one scene's guard (the
            # Champion's room VAR_TEMP_1) never blocks the next map's arrival.
            elif op in ('setvar','copyvar','addvar','subvar') and not args[0].startswith(('VAR_0x8','VAR_TEMP_')):
                v=value(args[1],state,self.names)
                if v is not None:
                    if op=='addvar':v+=state['vars'].get(args[0],0)
                    if op=='subvar':v=state['vars'].get(args[0],0)-v
                    state['vars'][args[0]]=v
            elif op in ('giveitem','giveuniqueitem','additem','addpcitem') and args and args[0].startswith('ITEM_'):
                state['items'].add(args[0])
            elif op=='settrainerflag':state['defeated'].add(args[0])
            elif op in ('addmoney','removemoney','addcoins','removecoins') and args:
                context=dict(state,vars=dict(state['vars'],**(assignments or {})))
                amounts=native.numeric_domain(args[0],context,self.names)
                if amounts is None or len(amounts)!=1:
                    self.unresolved['PAYMENT:'+args[0]].add(effect.get('source',''));continue
                amount=next(iter(amounts));currency='money' if op.endswith('money') else 'coins'
                balance=state.get(currency,0);maximum=999999 if currency=='money' else 9999
                if op.startswith('add'):state[currency]=min(maximum,balance+amount)
                elif currency=='money' or balance>=amount:state[currency]=max(0,balance-amount)
                state.setdefault('currency_transactions',[]).append({'op':op,'amount':amount,'source':effect.get('source')})
            elif op in ('removeobject','removeobjectat') and args:
                matched=re.match(r'data/maps/([^/]+)/',effect.get('source',''))
                d=matched[1] if matched else map_
                oid=object_id if args[0]=='VAR_LAST_TALKED' else self.geo._aliases.get(d,{}).get(args[0])
                record=next((r for r in self.geo.object_records if r['map']==d and r['id']==oid),None)
                if record and record['flag'] not in ('0','FLAG_NONE',None):state['flags'].add(mr.canon(record['flag']))
            elif op=='native_call':
                inputs=effect.get('inputs',{})
                key='SPECIAL:'+args[0]+'('+','.join(f'{k}={v}' for k,v in inputs.items())+')'
                context=dict(state,current_map=map_) if map_ else state
                domain=native.evaluate(key,context,self.names)
                if domain:
                    for c in conditions or []:
                        if c['key']!=key:continue
                        v=value(c['value'],state,self.names)
                        if v is None:continue
                        compare={'eq':lambda a:a==v,'ne':lambda a:a!=v,'lt':lambda a:a<v,'le':lambda a:a<=v,'gt':lambda a:a>v,'ge':lambda a:a>=v}[c['op']]
                        domain={x for x in domain if compare(x)}
                effect_values=native.native_effects(key,next(iter(sorted(domain))) if domain else None,context,self.names)
                state['flags'].update(mr.canon(f) for f in effect_values.get('flags_set',[]))
                state['vars'].update(effect_values.get('vars',{}))
                state['items'].update(effect_values.get('items_add',{}))
                state['species'].update(effect_values.get('species_add',[]))
                state['caught'].update(effect_values.get('caught_add',[]))
                state['legend_unlocked'].update(effect_values.get('legend_unlocked',[]))
                state.setdefault('decorations',set()).update(effect_values.get('decorations_add',()))
                state['decorations'].difference_update(effect_values.get('decorations_remove',()))
                state.setdefault('trader',{}).update(effect_values.get('trader_update',{}))
                if 'garden_celebi' in effect_values:state['garden_celebi']=effect_values['garden_celebi']
                if effect_values.get('lilycove_lady_update'):
                    lady=state.setdefault('lilycove_lady',{})
                    for k,v in effect_values['lilycove_lady_update'].items():
                        if isinstance(v,dict):lady.setdefault(k,{}).update(v)
                        else:lady[k]=v
                for currency in ('money','coins'):
                    maximum=999999 if currency=='money' else 9999
                    state[currency]=min(maximum,max(0,state.get(currency,0)+effect_values.get(currency+'_add',0)-effect_values.get(currency+'_remove',0)))
                for item,qty in effect_values.get('pc_items_add',{}).items():
                    state['pc_items'].add(item);state['pc_item_counts'][item]=state['pc_item_counts'].get(item,0)+qty
        self.geo.apply_effects(state,effects,map_=map_,object_id=object_id,constants=self.names)
        after=(frozenset(state['flags']),tuple(sorted(state['vars'].items())),frozenset(state['items']),frozenset(state['legend_unlocked']))
        return before!=after

    def close(self,state):
        live_items=state.setdefault('available_item_sources',set())
        live_pokemon=state.setdefault('available_pokemon_sources',set())
        for iteration in range(80):
            before=(frozenset(state['flags']),tuple(sorted(state['vars'].items())),frozenset(state['items']),frozenset(state['species']),len(state['reached']))
            self.derived_flags(state);self.walk(state)
            # Wally's catching tutorial is an automatic demonstration, not a
            # player-team test battle. Completing it is what initializes the
            # Gym counter to 2 before Roxanne increments it.
            if ('PetalburgCity_Gym' in state['maps']
                    and state['vars'].get('VAR_PETALBURG_GYM_STATE',0)<2):
                state['vars']['VAR_PETALBURG_GYM_STATE']=2
                state['flags'].update(mr.canon(f) for f in (
                    'FLAG_HIDE_PETALBURG_GYM_WALLY','FLAG_HIDE_PETALBURG_GYM_WALLYS_DAD'))
            for i,row in enumerate(self.items):
                if i in live_items:continue
                if self.source_live(row,state):
                    live_items.add(i);state['items'].add(row['key'])
                    if row.get('kind')=='berry_tree':state['harvestable_berries'].add(row['key'])
            for i,row in enumerate(self.pokemon.rows):
                if i in live_pokemon:continue
                if self.source_live(row,state):
                    live_pokemon.add(i);state['species'].add(row['species']);state['caught'].add(row['species'])
            # Breeding asks whether each owned species can be caught twice.
            # Index wild sources once and answer each species once per pass.
            if not hasattr(self,'_wild_by_species'):
                self._wild_by_species={}
                for r in self.pokemon.sources:
                    if r.kind=='wild':self._wild_by_species.setdefault(r.key,[]).append(r)
            repeatable={}
            def can_repeat(s):
                if s not in repeatable:
                    repeatable[s]=any(self.reachable(r.where,state) and self.requirements(r.requires,state)
                                      for r in self._wild_by_species.get(s,()))
                return repeatable[s]
            deriv=self.pokemon.derive(state['species'],state['items'],state['cap'],lambda w:self.reachable(w,state),state['flags'],
                {'repeatable':can_repeat,
                 'knows_move':self.pokemon.can_learn,'friendship_max':255 if 'OldaleTown_PokemonCenter_1F' in state['maps'] and mr.canon('FLAG_BADGE01_GET') in state['flags'] else 0})
            state['species'].update(deriv['species']);state['caught'].update(deriv['species'])
            for event in self.events:
                if event['id'] in state['event_history']:continue
                if not self.event_reachable(event['event'],event['entry'],state):continue
                if not self.battles_completed(event.get('prior_battles',[]),state):continue
                facing=self.facing_domain(event['event'],event['entry'],state)
                if not self.conditions(event['conditions'],dict(state,facing_domain=facing) if facing else state,event['source']):continue
                effects=event['effects']
                if event.get('prior_battles'):
                    prior=set(part for battle in event['prior_battles'] for part in battle.split('|'))
                    already=set()
                    for node in self.battles:
                        if node['site_id'] in prior or prior.intersection(node['trainers']):
                            already.update(state.get('battle_applied_effects',{}).get(node['site_id'],set()))
                    invocation=(event['entry'],tuple(event['event']),tuple(event['prior_battles']))
                    ledger=state.setdefault('continuation_applied_effects',{}).setdefault(invocation,set())
                    effects=[e for e in effects if self.effect_key(e) not in already|ledger]
                    ledger.update(self.effect_key(e) for e in effects)
                self.apply(effects,state,event.get('assignments'),event['conditions'],event['event'][0],event['entry']);state['event_history'].add(event['id'])
            after=(frozenset(state['flags']),tuple(sorted(state['vars'].items())),frozenset(state['items']),frozenset(state['species']),len(state['reached']))
            if after==before:break
        else:raise RuntimeError('Field closure did not converge')
        return dict(item_sources=sorted(live_items),pokemon_sources=sorted(live_pokemon),derivations=deriv)

    def eligible(self,node,state,selectors=False):
        if node['site_id'] in state['completed_sites']:return False
        if node.get('kind')=='optional_activity_battle':
            activity=self.activities[node['activity_id']]
            town=activity['where'][0].split('City')[0].split('Town')[0]
            wins=state.get('tent_wins',{}).get(town,0)
            return (wins==node['activity_index']-1 and self.reachable(activity['where'],state)
                    and self.requirements(activity.get('requires'),state)
                    and (activity['facility']=='FRONTIER_FACILITY_FACTORY'
                         or len([s for s in state['species'] if self.pokemon.sd.restricted_class(s) is None])>=3))
        if node.get('trainers') and all(t in state['defeated'] for t in node['trainers']):return False
        for path in node['paths']:
            event=path['event'];where=(event[0],event[1],event[2]) if event[1] is not None else event[0]
            approach=self.geo.battle_approach(node,state,event=event,constants=self.names)
            if not (approach & state['reached']) or not self.battles_completed(path.get('prior_battles',[]),state):continue
            rows=path['conditions']
            if selectors:
                for gender in (0,1):
                    for first,second in ((0,1),(0,2),(1,0),(1,2),(2,0),(2,1)):
                        alternate=dict(state,gender=gender,first=first,second=second,vars=dict(state['vars'],VAR_STARTER_MON=first,VAR_EC_SECOND_STARTER=second+1))
                        if self.conditions(rows,alternate,node['source']):return True
            elif self.conditions(rows,state,node['source']):return True
        return False

    def win(self,node,state):
        out=copy.deepcopy(state);out['defeated'].update(node['trainers']);out['completed_sites'].add(node['site_id']);out['resolved_battle_sites'].add(node['site_id'])
        if node.get('kind')=='optional_activity_battle':
            activity=self.activities[node['activity_id']]
            town=activity['where'][0].split('City')[0].split('Town')[0]
            out.setdefault('tent_wins',{})[town]=node['activity_index']
            return out
        capture=not node['trainers'] and bool(node.get('wild_species'))
        out['battle_outcome']=7 if capture else 1
        # Read the complete caller's aftermath before changing its entry
        # variables. A callback may return to a caller that removes the NPC.
        # Re-evaluating that caller after its callback changed the entry state
        # loses the removal (for example the Route 104 rival at Briney's door).
        prefixes=[p for p in node['paths'] if self.conditions(p['conditions'],state,node['source'])]
        callers=[]
        for event in self.events:
            if not event.get('prior_battles') or not self.battles_completed(event['prior_battles'],out):continue
            prior=set(part for b in event['prior_battles'] for part in b.split('|'))
            if node['site_id'] not in prior and not prior.intersection(node['trainers']):continue
            matching=[p for p in prefixes if p['entry']==event['entry'] and tuple(p['event'])==tuple(event['event'])]
            if not matching:continue
            incoming={json.dumps(c,sort_keys=True) for p in matching for c in p['conditions']}
            remaining=[c for c in event['conditions'] if json.dumps(c,sort_keys=True) not in incoming]
            if self.conditions(remaining,out,event['source']):callers.append(event)
        paths=sorted(callers,key=lambda p:len(p['effects']),reverse=True) or node['victory_paths']
        for path in paths:
            if callers or self.conditions(path['conditions'],out,node['source']):
                effects=path['effects']
                already=set()
                for ledger in out.get('battle_applied_effects',{}).values():already.update(ledger)
                if callers:effects=[e for e in effects if self.effect_key(e) not in already]
                self.apply(effects,out,path.get('assignments'),path['conditions'],node['locations'][0]['map'] if node['locations'] else None,
                           path.get('entry') or (node['locations'][0]['entry'] if node['locations'] else None))
                out.setdefault('battle_applied_effects',{})[node['site_id']]={self.effect_key(e) for e in effects}
                if callers:out['event_history'].add(path['id'])
                break
        if capture:
            out['captured_battle_sites'].add(node['site_id'])
            out['species'].update(s for s in node['wild_species'] if s.startswith('SPECIES_'))
            out['caught'].update(s for s in node['wild_species'] if s.startswith('SPECIES_'))
        return out
