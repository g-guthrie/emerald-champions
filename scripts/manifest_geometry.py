#!/usr/bin/env python3
"""Manifest-only collision graph for live objects and mandatory battle triggers.

Object and coordinate-event tiles are cut before tile components are built.
Exact campaign state supplies clearance tokens; ordinary NPC talk approaches
remain adjacent tiles. This does not alter game data or the reference pool.
"""
from __future__ import annotations
import json
import re
from collections import defaultdict
from pathlib import Path
import mega_register as mr


class ManifestGeometry(mr.Geometry):
    def __init__(self, battles=None):
        self.manifest_battles = battles.get('battles',[]) if isinstance(battles,dict) else (battles or [])
        self.object_records=[]
        self.coordinate_records=[]
        self.object_diagnostics=[]
        self.puzzle_transitions=[]
        self.native_rotating={}
        self._rotating_cache={}
        self.native_moving={}
        self._moving_cache={}
        self._moving_approaches={}
        self.native_beams={}
        self._beam_cache={}
        super().__init__()

    @staticmethod
    def token(map_, object_id, x, y):
        return f'OBJECT_CLEAR:{map_}:{object_id}:{x}:{y}'

    def _build(self):
        # Called by Geometry.__init__ after maps/grid/generic gates exist.
        import reference_pool as rp
        from manifest_battle_progression import ProgressionParser, EFFECTS
        EFFECTS.update({'setobjectmovementtype','applymovement'})
        scripts=rp.Scripts(self)
        parser=ProgressionParser(scripts,max_steps=500,max_paths=64)
        objects={}
        aliases={}
        for d,m in self.maps.items():
            local={}
            source_path=mr.MAPS_DIR/d/'map.json'
            source_text=source_path.read_text()
            object_marks=list(re.finditer(r'(?m)^    \{\n      "(?:local_id|graphics_id)"',source_text))
            for index,o in enumerate(m.get('object_events') or [],1):
                if not isinstance(o.get('x'),int): continue
                record=dict(map=d,id=index,script=o.get('script'),flag=o.get('flag','0'),
                    variants=[],default=dict(x=o['x'],y=o['y'],movement=o.get('movement_type','MOVEMENT_TYPE_NONE'),
                    range_x=o.get('movement_range_x',0),range_y=o.get('movement_range_y',0)),
                    trainer_type=o.get('trainer_type','TRAINER_TYPE_NONE'),sight=o.get('trainer_sight_or_berry_tree_id','0'),
                    citations=[f'data/maps/{d}/map.json:{source_text[:object_marks[index-1].start()].count(chr(10))+1}' if index<=len(object_marks) else f'data/maps/{d}/map.json'])
                objects[d,index]=record
                local[str(index)]=index
                if o.get('local_id'): local[o['local_id']]=index
            path=mr.MAPS_DIR/d/'scripts.inc'
            if path.exists():
                for name,value in re.findall(r'(?m)^\.(?:set|equ)\s+(\w+),\s*(\d+)',path.read_text()): local[name]=int(value)
            aliases[d]=local
        def object_id(d,arg):
            return aliases.get(d,{}).get(arg)
        roots=defaultdict(list)
        for d in self.maps:
            path=mr.MAPS_DIR/d/'scripts.inc'
            if path.exists():
                for kind,label in re.findall(r'map_script\s+MAP_SCRIPT_ON_(LOAD|TRANSITION|RESUME)\s*,\s*(\w+)',path.read_text()):
                    roots[d].append((kind,label))
        for d,entries in roots.items():
            for kind,label in entries:
                paths=parser.walk(label)
                affected=set()
                for p in paths:
                    for effect in p.get('effects',[]):
                        if effect['op'] in ('setobjectxyperm','setobjectmovementtype','removeobject','addobject'):
                            oid=object_id(d,effect['args'][0])
                            if oid and (d,oid) in objects: affected.add(oid)
                # Include the branch on which an override does NOT run, so a
                # default/wandering position clears a former guard tile later.
                for oid in affected:
                    record=objects[d,oid]
                    for p in paths:
                        variant=dict(record['default'],conditions=p['conditions'],entry=label,entry_kind=kind,present=True,citations=[])
                        for effect in p.get('effects',[]):
                            if not effect.get('args') or object_id(d,effect['args'][0])!=oid: continue
                            op,args=effect['op'],effect['args']
                            if op=='setobjectxyperm' and len(args)>=3:
                                try: variant.update(x=int(args[1]),y=int(args[2]))
                                except ValueError:
                                    self.object_diagnostics.append(dict(map=d,object=oid,reason='nonliteral object position',source=effect['source']))
                            if op=='setobjectmovementtype' and len(args)>=2: variant['movement']=args[1]
                            if op=='removeobject': variant['present']=False
                            if op=='addobject': variant['present']=True
                            if op in ('setobjectxyperm','setobjectmovementtype','removeobject','addobject'): variant['citations'].append(effect['source'])
                        record['variants'].append(variant)
        self.object_diagnostics.extend({json.dumps(d,sort_keys=True):d for d in parser.diagnostics}.values())
        bounded={d['entry'] for d in parser.diagnostics}
        for record in objects.values():
            record['bounded_callbacks']=sorted({v.get('entry') for v in record['variants']} & bounded)
        self._script_index=scripts
        self._aliases=aliases
        for record in objects.values():
            body=scripts.labels.get(record['script'],{}).get('body',[])
            first=body[0][1] if body else ''
            record['sight_trainers']=re.findall(r'\bTRAINER_(?!NONE\b)\w+',first)[:1] if first.startswith('trainerbattle') else []
            record['positions']=sorted({(record['default']['x'],record['default']['y'])}|{(v['x'],v['y']) for v in record['variants']})
        for label,info in scripts.labels.items():
            d=info.get('map')
            for _n,line in info['body']:
                match=re.match(r'setobjectxyperm\s+(\w+),\s*(\d+),\s*(\d+)',line)
                if match and (d,object_id(d,match[1])) in objects:
                    record=objects[d,object_id(d,match[1])]
                    pos=(int(match[2]),int(match[3]))
                    if pos not in record['positions']: record['positions'].append(pos)
        self.object_records=list(objects.values())
        self._objects_by_script=defaultdict(list)
        for record in self.object_records:self._objects_by_script[record['map'],record['script']].append(record)
        self._prepare_moving_objects()
        for record in self.object_records:
            for x,y in record['positions']:
                self._gate(record['map'],record['id'],x,y,record['citations'])
            variants=record['variants'] or [dict(record['default'],conditions=[],present=True)]
            if record['bounded_callbacks']: variants=variants+[dict(record['default'],conditions=[],present=True)]
            for v in variants:
                if not v['present']: continue
                if self._wanders(v,record['map']):
                    # A random walker need not occupy its spawn forever. Its
                    # initial tile is free in the theoretical waiting model.
                    continue
                self._gate(record['map'],record['id'],v['x'],v['y'],record['citations']+v.get('citations',[]))
                direction={'MOVEMENT_TYPE_FACE_DOWN':(0,1),'MOVEMENT_TYPE_FACE_UP':(0,-1),
                    'MOVEMENT_TYPE_FACE_LEFT':(-1,0),'MOVEMENT_TYPE_FACE_RIGHT':(1,0)}.get(v['movement'])
                directions=self.DIRS if record['trainer_type'] in ('TRAINER_TYPE_SEE_ALL_DIRECTIONS','TRAINER_TYPE_BURIED') else [direction] if direction else []
                if record['trainer_type']!='TRAINER_TYPE_NONE' and directions:
                    try: sight=int(record['sight'])
                    except (ValueError,TypeError): sight=0
                    for direction in directions:
                        for distance in range(1,sight+1):
                            x,y=v['x']+direction[0]*distance,v['y']+direction[1]*distance
                            if record['map'] not in self.grid or self.kind(record['map'],x,y) is None: break
                            self._gate(record['map'],f"sight{record['id']}",x,y,record['citations'])
                            self.coordinate_records.append(dict(map=record['map'],id=f"sight{record['id']}",x=x,y=y,object_id=record['id'],origin=[v['x'],v['y']],script=record['script'],kind='trainer_sight'))
        for battle in self.manifest_battles:
            if not battle.get('forced_trigger'): continue
            for loc in battle.get('locations',[]):
                if loc.get('trigger')!='coord' or not isinstance(loc.get('x'),int): continue
                record=dict(map=loc['map'],id='coord-'+battle['site_id'],x=loc['x'],y=loc['y'],
                    entry=loc.get('entry'),battle=battle,kind='coordinate_battle')
                self.coordinate_records.append(record)
                self._gate(record['map'],record['id'],record['x'],record['y'],[battle['source']])
        self._prepare_script_geometry(parser,scripts)
        self._prepare_native_puzzles()
        self._triggers_by_tile=defaultdict(list)
        for trigger in self.coordinate_records:self._triggers_by_tile[trigger['map'],trigger['x'],trigger['y']].append(trigger)
        super()._build()
        # Puzzle-map blanket component links must not bypass living NPCs or
        # mandatory coordinate battles. Physical switch walls remain modelled.
        for a,edges in self.edges.items():
            self.edges[a]=[e for e in edges if e[2]!='puzzle' and not e[2].startswith('gate@')]
        self._add_individual_gate_tiles()
        self._add_script_geometry_edges()

    def _add_individual_gate_tiles(self):
        # Native collision is tested per stepped-on tile. The base Geometry's
        # cluster conjunction incorrectly ties a trainer's entire sight ray to
        # its permanently occupied body; winning would never clear that ray.
        for (d,x,y),req in self.tile_gates.items():
            if d not in self.grid: continue
            mode='W' if self.kind(d,x,y)=='W' else 'L'
            n=len(self.nodes);self.nodes.append((d,mode));self.by_map[d].append(n)
            self.tile_nodes[d,x,y].append(n)
        for (d,x,y),req in self.tile_gates.items():
            if d not in self.grid: continue
            for a in self.tile_nodes[d,x,y]:
                for dx,dy in self.DIRS:
                    nx,ny=x+dx,y+dy
                    if self.step_z(self.elevation(d,x,y),self.elevation(d,nx,ny)) is None: continue
                    combined=mr.req_and(req,self.tile_gates.get((d,nx,ny),((),)))
                    for b in self.tile_nodes.get((d,nx,ny),[]):
                        if self.nodes[a][1]!=self.nodes[b][1]: continue
                        self.edges[a].append((b,combined,f'tile gate {d}:{x},{y}'))
                        self.edges[b].append((a,combined,f'tile gate {d}:{x},{y}'))

    def _prepare_script_geometry(self,parser,scripts):
        """Compile explicit script doors and removable wall tiles only.

        Unlike the old whole-map puzzle links, every transition keeps its
        activation position, source branch conditions and defeated trainers.
        """
        from manifest_battle_progression import EFFECTS
        warp_ops={'warpdoor','warp','warpsilent','warpteleport'}
        prior_effects=set(EFFECTS)
        EFFECTS.update(warp_ops)
        try:
            # Story maps whose only entrance is a scripted walk-in: Steven leads
            # the player into the Cave of Origin (SootopolisCity/scripts.inc:917-946),
            # and the Safari Zone counter warps a paying player inside
            # (Route121_SafariZoneEntrance/scripts.inc:59-80).
            for d in sorted(mr.PUZZLE_MAPS|{'PetalburgCity_Gym','SootopolisCity','Route121_SafariZoneEntrance'}):
                if d not in self.maps: continue
                entries=[]
                for category in ('bg_events','coord_events','object_events'):
                    for ev in self.maps[d].get(category,[]) or []:
                        if isinstance(ev.get('x'),int) and ev.get('script'):
                            entries.append((ev['script'],(d,ev['x'],ev['y']),category,
                                            ev.get('flag') if category=='object_events' and ev.get('flag') not in (None,'0','FLAG_NONE') else None))
                script=mr.MAPS_DIR/d/'scripts.inc'
                if script.exists():
                    entries.extend((label,(d,None,None),'callback',None) for label in re.findall(r'map_script\s+MAP_SCRIPT_ON_(?:LOAD|TRANSITION|RESUME),\s*(\w+)',script.read_text()))
                for label,origin,kind,visible in entries:
                    for path in parser.walk(label):
                        if not path.get('completed'): continue
                        env=path.get('assignments',{})
                        # A first-time guard the scene itself writes (Steven's
                        # FLAG_STEVEN_GUIDES_TO_CAVE_OF_ORIGIN) only says the walk-in
                        # has not played yet; afterwards the same place stays open.
                        written={e['args'][0] for e in path.get('effects',[]) if e['op'] in ('setflag','clearflag','setvar') and e.get('args')}
                        conditions=[c for c in path['conditions'] if c[0] not in written]
                        # An NPC's script runs only while that NPC is present.
                        if visible:conditions=[[visible,'eq','FALSE']]+conditions
                        for effect in path.get('effects',[]):
                            op,args=effect['op'],effect.get('args',[])
                            record=dict(origin=origin,kind=kind,entry=label,conditions=conditions,prior_battles=path.get('prior_battles',[]),source=effect['source'])
                            if op=='setmetatile' and len(args)>=4 and args[3] in ('0','FALSE'):
                                try: x,y=int(args[0]),int(args[1])
                                except ValueError: continue
                                # Base-open tiles already participate in the
                                # physical graph; decorative updates add no edge.
                                if self.kind(d,x,y) is not None: continue
                                token=f'SCRIPT_OPEN:{d}:{x}:{y}'
                                self.tile_gates.setdefault((d,x,y),((token,),))
                                self.tile_gate_cites.setdefault((d,x,y),effect['source'])
                                record.update(token=token,target=(d,x,y),action='open')
                            elif op in warp_ops and len(args)>=4:
                                dest=self.id_to_dir.get(args[0])
                                try: x,y=int(env.get(args[2],args[2])),int(env.get(args[3],args[3]))
                                except ValueError: continue
                                if not dest: continue
                                record.update(token=f'SCRIPT_WARP:{len(self.puzzle_transitions)}',target=(dest,x,y),action='warp')
                            else: continue
                            self.puzzle_transitions.append(record)
        finally:
            EFFECTS.clear();EFFECTS.update(prior_effects)

    def _add_script_geometry_edges(self):
        for transition in self.puzzle_transitions:
            if transition['action']!='warp': continue
            d,x,y=transition['origin'];dest,tx,ty=transition['target']
            if x is None: continue
            source=self.near(d,x,y) if transition['kind']=='object_events' else self.tile_nodes.get((d,x,y),[])
            # Door signs stand on their adjacent approach tile. Never extend
            # over an occupied approach or solid wall.
            if transition['kind']=='bg_events': source=self.near(d,x,y)
            target=self.tile_nodes.get((dest,tx,ty),[])
            for a in source:
                for b in target:
                    edge=(b,((transition['token'],),),'script warp '+transition['source'])
                    if edge not in self.edges[a]: self.edges[a].append(edge)

    def _prepare_native_puzzles(self):
        self._prepare_rotating_gates()
        for d,model in self.native_moving.items():
            for x,y in model['tiles']:
                self.tile_gates[d,x,y]=mr.req_and(self.tile_gates.get((d,x,y),((),)),((f'SCRIPT_MOVING:{d}:{x}:{y}',),))
        # Native stair ice forces the player south until every fresh thin-ice
        # tile of that room has been stepped on once. Prove a route instead of
        # granting access to the entire Gym merely on entry.
        d='SootopolisCity_Gym_1F'
        if d in self.grid:
            for low,high,bottom,stairs,threshold in [(17,19,(8,20),[(8,15),(8,16)],8),(12,14,(8,15),[(8,10),(8,11)],28),(6,9,(8,10),[(8,4),(8,5)],67)]:
                w,h,_words,_beh=self.grid[d]
                ice={(x,y) for y in range(low,high+1) for x in range(w) if self.behavior(d,x,y)==self.mb['MB_THIN_ICE']}
                ends={p for p in ice if any(abs(p[0]-x)+abs(p[1]-y)==1 for x,y in stairs)}
                starts={p for p in ice if abs(p[0]-bottom[0])+abs(p[1]-bottom[1])==1}
                route=self._ice_route(ice,starts,ends)
                token=f'SCRIPT_ICE:{threshold}'
                for x,y in stairs:
                    self.tile_gates[d,x,y]=mr.req_and(self.tile_gates.get((d,x,y),((),)),((token,),))
                if not route:
                    self.object_diagnostics.append(dict(map=d,reason='ice route proof failed',threshold=threshold));continue
                self.puzzle_transitions.append(dict(origin=(d,*bottom),target=(d,*stairs[0]),kind='coord_events',action='ice',token=token,conditions=[],prior_battles=[],route=route,ice_tiles=len(ice),source='data/maps/SootopolisCity_Gym_1F/scripts.inc:17-53; src/field_tasks.c:660-735'))
        # Each switch toggles the physical beams in C. Only beam tiles that
        # the source switch routine can open acquire a clearance condition.
        d='MauvilleCity_Gym'
        if d in self.grid:
            text=(mr.ROOT/'include/constants/metatile_labels.h').read_text()
            ids={name:int(num,0) for name,num in re.findall(r'#define\s+(METATILE_MauvilleGym_\w+)\s+(0x[0-9A-Fa-f]+)',text)}
            openable={n for name,n in ids.items() if ('Beam' in name and name.endswith('_On')) or name.endswith('PoleTop_On')}
            w,h,words,_beh=self.grid[d]
            for y in range(5,17):
                for x in range(9):
                    if words[y*w+x]&1023 not in openable or self.kind(d,x,y) is not None: continue
                    token=f'SCRIPT_BEAM:{d}:{x}:{y}'
                    # A script's alt configuration may open the same beam.
                    old=self.tile_gates.get((d,x,y),())
                    objects=tuple(tuple(t for t in alt if t.startswith('OBJECT_CLEAR:')) for alt in old) or ((),)
                    alternatives=tuple(tuple(t for t in alt if not t.startswith('OBJECT_CLEAR:')) for alt in old)+((token,),)
                    self.tile_gates[d,x,y]=mr.req_and(objects,alternatives)
                    for sx,sy in [(0,15),(4,12),(3,9),(8,9)]:
                        self.puzzle_transitions.append(dict(origin=(d,sx,sy),target=(d,x,y),kind='coord_events',action='beam',token=token,conditions=[],prior_battles=[],source='src/field_specials.c:1242-1355; data/maps/MauvilleCity_Gym/scripts.inc:172-220'))
            toggles={}
            for y in range(5,17):
                for x in range(9):
                    tile=words[y*w+x]&1023
                    name=next((n for n,v in ids.items() if v==tile),'')
                    if re.search(r'BeamH[34]_(?:On|Off)$',name) or name.endswith(('BeamV2_On','PoleTop_On','PoleTop_Off','FloorTile')):
                        toggles[x,y]=bool(words[y*w+x]&0xC00)
                        token=f'SCRIPT_BEAM:{d}:{x}:{y}'
                        old=self.tile_gates.get((d,x,y),((),))
                        objects=tuple(tuple(t for t in alt if t.startswith('OBJECT_CLEAR:')) for alt in old)
                        self.tile_gates[d,x,y]=mr.req_and(objects,((token,),))
            self.native_beams[d]=dict(tiles=toggles,switches={(0,15),(4,12),(3,9),(8,9)},source='src/field_specials.c:1242-1436; data/maps/MauvilleCity_Gym/scripts.inc:172-220')

    def _beam_clearance(self,state,flags):
        from collections import deque
        out=set()
        for d,model in self.native_beams.items():
            if not any(n in state.get('reached',set()) for n in self.by_map[d]):continue
            w,h,_words,_beh=self.grid[d];available=set()
            for y in range(h):
                for x in range(w):
                    if self.kind(d,x,y) not in ('L','B') and (x,y) not in model['tiles']:continue
                    req=tuple(tuple(t for t in alt if not t.startswith('SCRIPT_BEAM:')) for alt in self.tile_gates.get((d,x,y),((),)))
                    if mr.holds(req,flags):available.add((x,y))
            deactivated=mr.canon('FLAG_DEFEATED_MAUVILLE_GYM') in state['flags']
            key=(frozenset(available),deactivated)
            starts={(warp['x'],warp['y']) for warp in self.maps[d].get('warp_events',[]) if self.id_to_dir.get(warp['dest_map'])!=d}&available
            starts={p for p in starts if any(n in state.get('reached',set()) for n in self.tile_nodes.get((d,*p),[]))}
            key=(key,frozenset(starts))
            if key in self._beam_cache:out.update(self._beam_cache[key]);continue
            queue=deque((x,y,0) for x,y in starts);seen=set(queue);visited=set()
            while queue:
                x,y,phase=queue.popleft();visited.add((x,y))
                for dx,dy in self.DIRS:
                    p=(x+dx,y+dy)
                    if p not in available:continue
                    solid=model['tiles'].get(p)
                    if solid is not None and not deactivated and (solid if phase==0 else not solid):continue
                    if self.step_z(self.elevation(d,x,y),self.elevation(d,*p)) is None:continue
                    nextphase=1-phase if p in model['switches'] and not deactivated else phase
                    nxt=(*p,nextphase)
                    if nxt not in seen:seen.add(nxt);queue.append(nxt)
            tokens={f'SCRIPT_BEAM:{d}:{x}:{y}' for x,y in visited&model['tiles'].keys()}
            self._beam_cache[key]=tokens;out.update(tokens)
        return out

    def _ice_route(self,tiles,starts,ends):
        # Hamiltonian path is the actual no-repeat native ice requirement.
        adjacency={p:{(p[0]+dx,p[1]+dy) for dx,dy in self.DIRS}&tiles for p in tiles}
        attempts=0
        def visit(route,left):
            nonlocal attempts
            attempts+=1
            if attempts>300000:return None
            if not left:return route if route[-1] in ends else None
            # Disconnected unvisited ice can never be completed without
            # revisiting a cracked tile (which drops the player downstairs).
            seed=next(iter(left));seen={seed};todo=[seed]
            while todo:
                for p in adjacency[todo.pop()]&left-seen:seen.add(p);todo.append(p)
            if seen!=left:return None
            for p in sorted(adjacency[route[-1]]&left,key=lambda p:len(adjacency[p]&left)):
                if p in ends and len(left)>1:continue
                result=visit(route+[p],left-{p})
                if result:return result
            return None
        for start in sorted(starts):
            route=visit([start],tiles-{start})
            if route:return [list(p) for p in route]
        return None

    def _prepare_rotating_gates(self):
        text=(mr.ROOT/'src/rotating_gate.c').read_text()
        shapes=['L1','L2','L3','L4','T1','T2','T3','T4','UNUSED_T1','UNUSED_T2','UNUSED_T3','UNUSED_T4']
        layout=re.search(r'sRotatingGate_ArmLayout\[\]\[4 \* 2\]\s*=\s*\{(.*?)\n\};',text,re.S)[1]
        layout=re.sub(r'//[^\n]*','',layout)
        arms=[[int(n) for n in re.findall(r'\b[01]\b',row)] for row in re.findall(r'\{([^{}]*)\}',layout)]
        directions={}
        for suffix,delta in [('North',(0,-1)),('South',(0,1)),('West',(-1,0)),('East',(1,0))]:
            body=re.search(r'sRotatingGate_RotationInfo'+suffix+r'\[4 \* 4\]\s*=\s*\{(.*?)\};',text,re.S)[1]
            directions[delta]=[]
            for token in re.findall(r'GATE_ROT_NONE|GATE_ROT_(?:ACW|CW)\(GATE_ARM_\w+,\s*[01]\)',body):
                if token=='GATE_ROT_NONE': directions[delta].append(None);continue
                turn,arm,long=re.search(r'GATE_ROT_(ACW|CW)\(GATE_ARM_(\w+),\s*([01])\)',token).groups()
                directions[delta].append((1 if turn=='CW' else -1,['NORTH','EAST','SOUTH','WEST'].index(arm),int(long)))
        sweeps={}
        for suffix,turn in [('Clockwise',1),('AntiClockwise',-1)]:
            body=re.search(r'sRotatingGate_ArmPositions'+suffix+r'Rotation\[\]\s*=\s*\{(.*?)\};',text,re.S)[1]
            sweeps[turn]=[(int(x),int(y)) for x,y in re.findall(r'\{\s*(-?\d+),\s*(-?\d+)\s*\}',body)]
        for d,config in [('FortreeCity_Gym','Fortree'),('Route110_TrickHousePuzzle6','TrickHouse')]:
            body=re.search(r'sRotatingGate_'+config+r'PuzzleConfig\[\]\s*=\s*\{(.*?)\};',text,re.S)[1]
            gates=[(int(x),int(y),arms[shapes.index(shape)],int(orientation)//90) for x,y,shape,orientation in re.findall(r'\{\s*(\d+),\s*(\d+),\s*GATE_SHAPE_(\w+),\s*GATE_ORIENTATION_(\d+)\s*\}',body)]
            tiles=set()
            for x,y,_shape,_orientation in gates:
                for tx in range(x-2,x+2):
                    for ty in range(y-2,y+2):
                        if self.kind(d,tx,ty) not in ('L','B'):continue
                        tiles.add((tx,ty))
                        self.tile_gates[d,tx,ty]=mr.req_and(self.tile_gates.get((d,tx,ty),((),)),((f'SCRIPT_ROT:{d}:{tx}:{ty}',),))
            self.native_rotating[d]=dict(gates=gates,directions=directions,sweeps=sweeps,tiles=tiles,source='src/rotating_gate.c:191-217,480-615,850-997')

    def _rotating_clearance(self,state,flags):
        """Exact bounded native gate-state exploration, with actor collisions."""
        from collections import deque
        out=set()
        for d,model in self.native_rotating.items():
            if not any(n in state.get('reached',set()) for n in self.by_map.get(d,[])):continue
            w,h,words,_beh=self.grid[d]
            passable=set()
            for y in range(h):
                for x in range(w):
                    if self.kind(d,x,y) not in ('L','B'):continue
                    req=tuple(tuple(t for t in alt if not t.startswith('SCRIPT_ROT:')) for alt in self.tile_gates.get((d,x,y),((),)))
                    if mr.holds(req,flags):passable.add((x,y))
            key=(d,frozenset(passable))
            terminals=defaultdict(list)
            for trigger in self.coordinate_records:
                if trigger['kind']!='trainer_sight' or trigger['map']!=d:continue
                p=(trigger['x'],trigger['y'])
                residual=self._sight_entry_requirement(trigger,prefixes=('SCRIPT_ROT:',))
                if mr.holds(residual,flags):terminals[p].append(trigger['script'])
            # Entering from the actual external warp resets this native puzzle
            # to its authored initial orientations. No interior seeds are added.
            starts={(warp['x'],warp['y']) for warp in self.maps[d].get('warp_events',[]) if self.id_to_dir.get(warp['dest_map'])!=d}&passable
            starts={p for p in starts if any(n in state.get('reached',set()) for n in self.tile_nodes.get((d,*p),[]))}
            key=(key,frozenset(starts))
            if key in self._rotating_cache:
                tokens,approaches=self._rotating_cache[key];out.update(tokens);self._moving_approaches.update(approaches);continue
            orientation=tuple(g[3] for g in model['gates'])
            queue=deque((x,y,orientation) for x,y in starts);seen=set(queue);visited=set(starts)
            approaches=defaultdict(set)
            while queue and len(seen)<=300000:
                x,y,rot=queue.popleft()
                for dx,dy in self.DIRS:
                    nx,ny=x+dx,y+dy
                    if (nx,ny) not in passable and (nx,ny) not in terminals:continue
                    if self.step_z(self.elevation(d,x,y),self.elevation(d,nx,ny)) is None:continue
                    newrot=rot;blocked=False
                    for i,(gx,gy,arms,_initial) in enumerate(model['gates']):
                        if not (gx-2<=nx<=gx+1 and gy-2<=ny<=gy+1):continue
                        info=model['directions'][dx,dy][(ny-gy+2)*4+nx-gx+2]
                        if info is None:continue
                        turn,arm,long=info
                        if not arms[((arm-rot[i]+4)%4)*2+long]:continue
                        for a in range(4):
                            for length in range(2):
                                if not arms[a*2+length]:continue
                                sx,sy=model['sweeps'][turn][2*((rot[i]+a)%4)+length]
                                tx,ty=gx+sx,gy+sy
                                # Native CanRotate checks physical map walls,
                                # then normal walking checks the live actor.
                                if not (0<=tx<w and 0<=ty<h) or words[ty*w+tx]&0xC00:blocked=True
                        if not blocked:
                            rr=list(rot);rr[i]=(rr[i]+turn)%4;newrot=tuple(rr)
                        break
                    if blocked:continue
                    for script in terminals.get((nx,ny),[]):approaches[d,script].add((x,y))
                    if (nx,ny) not in passable:continue
                    candidate=(nx,ny,newrot)
                    if candidate not in seen:seen.add(candidate);queue.append(candidate);visited.add((nx,ny))
            if queue:
                self.object_diagnostics.append(dict(map=d,reason='native rotating gate exploration bound exceeded',states=len(seen)))
            tokens={f'SCRIPT_ROT:{d}:{x}:{y}' for x,y in visited&model['tiles']}
            self._rotating_cache[key]=(tokens,dict(approaches));out.update(tokens);self._moving_approaches.update(approaches)
        return out

    def _prepare_moving_objects(self):
        text=(mr.ROOT/'include/constants/metatile_labels.h').read_text()
        for d,label,trick in [('MossdeepCity_Gym','METATILE_MossdeepGym_YellowArrow_Right',False),('Route110_TrickHousePuzzle7','METATILE_TrickHousePuzzle_Arrow_YellowOnWhite_Right',True)]:
            start=int(re.search(r'#define\s+'+label+r'\s+(0x[0-9a-fA-F]+)',text)[1],0)
            w,h,words,_beh=self.grid[d];actors=[];tiles=set()
            for record in self.object_records:
                if record['map']!=d:continue
                x,y=record['default']['x'],record['default']['y'];delta=(words[y*w+x]&1023)-start
                if not (0<=delta<40 and delta%8<4):continue
                ring=[];cx,cy=x,y
                for _ in range(4):
                    ring.append((cx,cy));num=((words[cy*w+cx]&1023)-start)%8
                    if num>=4:break
                    dx,dy=[(1,0),(0,1),(-1,0),(0,-1)][num];cx+=dx;cy+=dy
                if (cx,cy)!=(x,y) or len(ring)!=4:
                    self.object_diagnostics.append(dict(map=d,object=record['id'],reason='native arrow object is not a four-step ring'));continue
                direction={'MOVEMENT_TYPE_FACE_RIGHT':0,'MOVEMENT_TYPE_FACE_DOWN':1,'MOVEMENT_TYPE_FACE_LEFT':2,'MOVEMENT_TYPE_FACE_UP':3}.get(record['default']['movement'],0)
                actors.append(dict(record=record,color=delta//8,ring=ring,direction=direction))
                for pos in ring:
                    if pos not in record['positions']:record['positions'].append(pos)
                    tiles.add(pos)
                try:sight=int(record['sight'])
                except ValueError:sight=0
                for phase,(px,py) in enumerate(ring):
                    dx,dy=[(1,0),(0,1),(-1,0),(0,-1)][(direction-phase)%4]
                    for distance in range(1,sight+1):
                        p=(px+distance*dx,py+distance*dy)
                        if self.kind(d,*p) not in ('L','B'):break
                        tiles.add(p)
            switches={}
            for ev in self.maps[d].get('coord_events',[]) or []:
                info=self._script_index.labels.get(ev.get('script'),{})
                for _line,body in info.get('body',[]):
                    match=re.match(r'moverotatingtileobjects\s+(\d+)',body)
                    if match:switches[ev['x'],ev['y']]=int(match[1])
            self.native_moving[d]=dict(actors=actors,tiles=tiles,switches=switches,source='src/rotating_tile_puzzle.c:108-286; '+f'data/maps/{d}/scripts.inc')

    def _moving_clearance(self,state,flags):
        from collections import deque
        out=set()
        for d,model in self.native_moving.items():
            if not any(n in state.get('reached',set()) for n in self.by_map.get(d,[])):continue
            actors=model['actors'];ids={a['record']['id'] for a in actors};w,h,words,_beh=self.grid[d]
            stationary_triggers=defaultdict(list)
            for trigger in self.coordinate_records:
                if trigger['kind']=='trainer_sight' and trigger['map']==d and trigger['object_id'] not in ids:stationary_triggers[trigger['x'],trigger['y']].append(trigger)
            ignored={self.token(d,oid,x,y) for oid in ids for x,y in next(a['record']['positions'] for a in actors if a['record']['id']==oid)}
            ignored.update(self.token(t['map'],t['id'],t['x'],t['y']) for t in self.coordinate_records if t['kind']=='trainer_sight' and t['map']==d and t['object_id'] in ids)
            floor=set()
            for y in range(h):
                for x in range(w):
                    if self.kind(d,x,y) not in ('L','B'):continue
                    req=tuple(tuple(t for t in alt if t not in ignored and not t.startswith('SCRIPT_MOVING:')) for alt in self.tile_gates.get((d,x,y),((),)))
                    if mr.holds(req,flags):floor.add((x,y))
            key=(d,frozenset(floor),frozenset(state['defeated']))
            warps=self.maps[d].get('warp_events',[]) or [];links={}
            starts=set()
            for warp in warps:
                if self.id_to_dir.get(warp['dest_map'])!=d:starts.add((warp['x'],warp['y']));continue
                target=warps[int(warp['dest_warp_id'])]
                if self.behavior(d,warp['x'],warp['y']) not in self.inert_warp:links[warp['x'],warp['y']]=(target['x'],target['y'])
            starts={p for p in starts&floor if any(n in state.get('reached',set()) for n in self.tile_nodes.get((d,*p),[]))}
            key=(key,frozenset(starts))
            if key in self._moving_cache:
                tokens,approaches=self._moving_cache[key];out.update(tokens);self._moving_approaches.update(approaches);continue
            startphase=(0,)*5;queue=deque((p,startphase) for p in starts&floor);seen=set(queue);visited=set();approaches=defaultdict(set)
            while queue and len(seen)<=100000:
                origin,phase=queue.popleft()
                occupied=set();triggers=defaultdict(set)
                for actor in actors:
                    r=actor['record'];hidden=r['flag'] not in ('0','FLAG_NONE',None) and mr.canon(r['flag']) in state['flags']
                    if hidden:continue
                    x,y=actor['ring'][phase[actor['color']]];occupied.add((x,y))
                    undefeated=not r['sight_trainers'] or not all(t in state['defeated'] for t in r['sight_trainers'])
                    if not undefeated:continue
                    try:sight=int(r['sight'])
                    except ValueError:sight=0
                    dx,dy=[(1,0),(0,1),(-1,0),(0,-1)][(actor['direction']-phase[actor['color']])%4]
                    for distance in range(1,sight+1):
                        p=(x+dx*distance,y+dy*distance)
                        if p not in floor or p in occupied:break
                        triggers[p].add(r['script'])
                    for dx,dy in self.DIRS:
                        p=(x+dx,y+dy)
                        if p==origin:approaches[d,r['script']].add(p)
                blocked=occupied|set(triggers);todo=[origin];reachable={origin} if origin in floor and origin not in blocked else set()
                if not reachable:continue
                while todo:
                    x,y=todo.pop();visited.add((x,y))
                    for actor in actors:
                        r=actor['record'];ax,ay=actor['ring'][phase[actor['color']]]
                        if abs(ax-x)+abs(ay-y)==1:approaches[d,r['script']].add((x,y))
                    for dx,dy in self.DIRS:
                        p=(x+dx,y+dy)
                        # A stationary trainer's first sight tile may also be
                        # an arrow-ring tile. It is a legal battle boundary
                        # while still blocked for ordinary traversal.
                        for trigger in stationary_triggers.get(p,[]):
                            residual=self._sight_entry_requirement(trigger,ignored=ignored,prefixes=('SCRIPT_MOVING:',))
                            if p not in occupied and p not in triggers and mr.holds(residual,flags):approaches[d,trigger['script']].add((x,y))
                        for script in triggers.get(p,[]):approaches[d,script].add((x,y))
                        if p not in floor or p in blocked or p in reachable:continue
                        if self.step_z(self.elevation(d,x,y),self.elevation(d,*p)) is None:continue
                        if p in model['switches']:
                            visited.add(p);color=model['switches'][p];new=list(phase);new[color]=(new[color]+1)%4;candidate=(p,tuple(new))
                            if candidate not in seen:seen.add(candidate);queue.append(candidate)
                            continue
                        reachable.add(p);todo.append(p)
                        dest=links.get(p)
                        if dest in floor and dest not in blocked and dest not in reachable:reachable.add(dest);todo.append(dest)
                for p in reachable&model['switches'].keys():
                    color=model['switches'][p];new=list(phase);new[color]=(new[color]+1)%4;candidate=(p,tuple(new))
                    if candidate not in seen:seen.add(candidate);queue.append(candidate)
            if queue:self.object_diagnostics.append(dict(map=d,reason='native moving-object exploration bound exceeded',states=len(seen)))
            tokens={f'SCRIPT_MOVING:{d}:{x}:{y}' for x,y in visited&model['tiles']}
            tokens.update(t for t in ignored if tuple(map(int,t.rsplit(':',2)[1:])) in visited)
            self._moving_cache[key]=(tokens,dict(approaches));out.update(tokens);self._moving_approaches.update(approaches)
        return out

    def _wanders(self,v,map_=None):
        movement=v.get('movement','')
        if 'IN_PLACE' in movement: return False
        if not ('WANDER' in movement or 'WALK_' in movement) or not (v.get('range_x') or v.get('range_y')): return False
        if map_ is None: return True
        if map_ not in self.grid: return False
        directions=self.DIRS
        if any(s in movement for s in ('UP_AND_DOWN','DOWN_AND_UP')): directions=((0,1),(0,-1))
        if any(s in movement for s in ('LEFT_AND_RIGHT','RIGHT_AND_LEFT')): directions=((1,0),(-1,0))
        for dx,dy in directions:
            if dx and not v.get('range_x'): continue
            if dy and not v.get('range_y'): continue
            if self.kind(map_,v['x']+dx,v['y']+dy) in ('L','B') and self.step_z(self.elevation(map_,v['x'],v['y']),self.elevation(map_,v['x']+dx,v['y']+dy)) is not None: return True
        return False

    def _gate(self,d,oid,x,y,citations):
        key=(d,x,y)
        req=((self.token(d,oid,x,y),),)
        self.tile_gates[key]=mr.req_and(self.tile_gates.get(key,((),)),req)
        self.tile_gate_cites[key]='; '.join(citations)

    def _sight_entry_requirement(self,trigger,ignored=(),prefixes=()):
        d,x,y=trigger['map'],trigger['x'],trigger['y']
        waived=set(ignored)|{self.token(d,trigger['id'],x,y)}
        # CheckTrainer sorts live objects by local ID. At overlapping rays,
        # the earlier trainer can start its battle before the later trainer;
        # ordinary traversal still requires both victories.
        for other in self._triggers_by_tile.get((d,x,y),[]):
            if other['kind']=='trainer_sight' and other['object_id']>trigger['object_id']:
                waived.add(self.token(d,other['id'],x,y))
        return tuple(tuple(t for t in alt if t not in waived and not t.startswith(tuple(prefixes))) for alt in self.tile_gates.get((d,x,y),((),)))

    @staticmethod
    def _conditions(conditions,state,constants):
        # World.value resolves flags, persistent vars, gender and C constants.
        from manifest_world import value
        import manifest_native_predicates as native
        uncertain=False;domains={}
        for condition in conditions:
            if isinstance(condition,dict): key,op,want=condition['key'],condition['op'],condition['value']
            else: key,op,want=condition
            a,b=value(key,state,constants),value(want,state,constants)
            if b is None:
                uncertain=True;continue
            domain={a} if a is not None else domains.get(key,native.evaluate(key,state,constants))
            if domain is None:uncertain=True;continue
            domain={a for a in domain if {'eq':a==b,'ne':a!=b,'lt':a<b,'le':a<=b,'gt':a>b,'ge':a>=b}[op]}
            if not domain:return False
            domains[key]=domain
        return None if uncertain else True

    def _geometry_conditions(self,conditions,state,constants):
        from manifest_world import value
        persistent=[];choices=defaultdict(list)
        for condition in conditions:
            key,op,want=(condition['key'],condition['op'],condition['value']) if isinstance(condition,dict) else condition
            if key.startswith(('COMMAND:msgbox@','COMMAND:multichoice@','RANDOM:')) or key=='VAR_FACING': choices[key].append((op,want))
            else: persistent.append(condition)
        if self._conditions(persistent,state,constants) is not True: return False
        for key,rows in choices.items():
            if key.startswith('RANDOM:'):
                try: domain=set(range(int(key.split(':',1)[1].split('@',1)[0])))
                except ValueError: return False
            elif key.startswith('COMMAND:msgbox@'): domain={0,1}
            elif key=='VAR_FACING': domain={1,2,3,4}
            else: domain=set(range(32)) # source menu branch indices, including cancel
            for op,want in rows:
                b=value(want,state,constants)
                if b is None: return False
                domain={a for a in domain if {'eq':a==b,'ne':a!=b,'lt':a<b,'le':a<=b,'gt':a>b,'ge':a>=b}[op]}
            if not domain: return False
        return True

    def flags_for_state(self,state,constants):
        clear=set()
        self._moving_approaches={}
        state['_geometry_object_positions']={}
        live={}
        for record in self.object_records:
            d,oid=record['map'],record['id']
            variants=record['variants'] or [dict(record['default'],conditions=[],present=True)]
            possible=[v for v in variants if self._conditions(v['conditions'],state,constants) is not False]
            # Hidden object's flag removes its collision everywhere.
            hidden=record['flag'] not in ('0','FLAG_NONE',None) and mr.canon(record['flag']) in state['flags']
            field_move=self.OBJECT_GATES.get(record['script'])
            if field_move and field_move in state['flags']: hidden=True
            occupied=set()
            if not possible and record['bounded_callbacks']: possible=[dict(record['default'],present=True)]
            override=state.get('object_positions',{}).get(f'{d}:{oid}')
            if override is not None:
                possible=[dict(record['default'],x=override[0],y=override[1],present=True)]
            state['_geometry_object_positions'][f'{d}:{oid}']=sorted({(v['x'],v['y']) for v in possible if not hidden and v['present']})
            for v in possible:
                if not hidden and v['present'] and not self._wanders(v,d): occupied.add((v['x'],v['y']))
            live[d,oid]=(record,occupied,hidden)
            for x,y in record['positions']:
                if (x,y) not in occupied: clear.add(self.token(d,oid,x,y))
        for trigger in self.coordinate_records:
            clear_trigger=False
            if trigger['kind']=='trainer_sight':
                record,occupied,hidden=live[trigger['map'],trigger['object_id']]
                defeated=bool(record['sight_trainers']) and all(t in state['defeated'] for t in record['sight_trainers'])
                clear_trigger=hidden or tuple(trigger.get('origin',[])) not in occupied or defeated
            else:
                battle=trigger['battle']
                defeated=bool(battle.get('trainers')) and all(t in state['defeated'] for t in battle['trainers'])
                conditions=[p.get('conditions',[]) for p in battle.get('paths',[]) if not p.get('event') or p['event'][0]==trigger['map']]
                inactive=bool(conditions) and all(self._conditions(c,state,constants) is False for c in conditions)
                clear_trigger=defeated or inactive or battle['site_id'] in state.get('resolved_battle_sites',set())
            if clear_trigger: clear.add(self.token(trigger['map'],trigger['id'],trigger['x'],trigger['y']))
        for transition in self.puzzle_transitions:
            if transition['action']=='beam':continue # native two-state solver below
            if any(t not in state['defeated'] for t in transition['prior_battles']): continue
            # Choosing YES at a door is an available player action; transient
            # menu-result conditions are not persistent campaign prerequisites.
            if not self._geometry_conditions(transition['conditions'],state,constants): continue
            d,x,y=transition['origin']
            reached=state.get('reached',set())
            if x is None:
                activated=any(n in reached for n in self.by_map.get(d,[]))
            elif transition['kind']=='coord_events':
                activated=any(n in reached for n in self.tile_nodes.get((d,x,y),[]))
            else:
                activated=any(n in reached for n in self.near(d,x,y))
            if activated: clear.add(transition['token'])
        clear.update(self._rotating_clearance(state,state['flags']|clear))
        clear.update(self._moving_clearance(state,state['flags']|clear))
        clear.update(self._beam_clearance(state,state['flags']|clear))
        state['_geometry_battle_approaches']={f'{d}:{entry}':sorted(points) for (d,entry),points in self._moving_approaches.items()}
        return clear

    def battle_approach(self,node,state,event=None,constants=None,clearance=None):
        """Standing components from which this battle can actually begin.

        A forced trigger is entered from its boundary, not talked to through
        its collision gate. Only this battle's trigger tokens are waived;
        objects, story walls and other undefeated trainers remain blocking.
        This method never adds traversal clearance to the campaign state.
        """
        if constants is None:
            from manifest_world import constants as read_constants
            if not hasattr(self,'_manifest_constants'): self._manifest_constants=read_constants()
            constants=self._manifest_constants
        if clearance is None:clearance=state.get('_geometry_clearance')
        if clearance is None:
            key=(frozenset(state['flags']),tuple(sorted(state['vars'].items())),frozenset(state['defeated']),frozenset(state.get('reached',set())),tuple(sorted((k,tuple(v)) for k,v in state.get('object_positions',{}).items())))
            if getattr(self,'_battle_clearance_key',None)!=key:
                self._battle_clearance_key=key;self._battle_clearance=self.flags_for_state(state,constants)
            clearance=self._battle_clearance
        flags=state['flags']|clearance
        locations=node.get('locations',[])
        entries={(l.get('map'),l.get('entry')) for l in locations}
        moving_entries={(d,a['record']['script']) for d,model in self.native_moving.items() for a in model['actors']}
        maps={l.get('map') for l in locations}
        if event is not None: maps &= {event[0]}
        triggers=[]
        for trigger in self.coordinate_records:
            if trigger['map'] not in maps: continue
            if trigger['kind']=='coordinate_battle':
                matches=trigger['battle']['site_id']==node['site_id']
            else:
                matches=(trigger['map'],trigger['script']) in entries and (trigger['map'],trigger['script']) not in moving_entries
            if matches: triggers.append(trigger)
        own={self.token(t['map'],t['id'],t['x'],t['y']) for t in triggers}
        # A single coordinate event may branch to several gender/starter
        # battle variants. Entering it starts the selected script branch;
        # unselected battle nodes must not prevent initiating that event.
        roots={(t['map'],t['x'],t['y'],t.get('entry')) for t in triggers if t['kind']=='coordinate_battle'}
        for trigger in self.coordinate_records:
            if trigger['kind']=='coordinate_battle' and (trigger['map'],trigger['x'],trigger['y'],trigger.get('entry')) in roots:
                own.add(self.token(trigger['map'],trigger['id'],trigger['x'],trigger['y']))
        trigger_tiles={(t['map'],t['x'],t['y']) for t in triggers}
        out=set()
        for d,x,y in trigger_tiles:
            candidates=[t for t in triggers if t['map']==d and t['x']==x and t['y']==y]
            residuals=[self._sight_entry_requirement(t,ignored=own) if t['kind']=='trainer_sight' else tuple(tuple(t for t in alt if t not in own) for alt in self.tile_gates.get((d,x,y),((),))) for t in candidates]
            if not any(mr.holds(req,flags) for req in residuals): continue
            for dx,dy in self.DIRS:
                nx,ny=x+dx,y+dy
                if (d,nx,ny) in trigger_tiles: continue
                if self.kind(d,nx,ny) not in ('L','B','W'): continue
                if self.step_z(self.elevation(d,nx,ny),self.elevation(d,x,y)) is None: continue
                out.update(self.tile_nodes.get((d,nx,ny),[]))
        for location in locations:
            d,x,y=location.get('map'),location.get('x'),location.get('y')
            if d in maps and location.get('trigger')=='map':
                # On-frame encounters start when this map is entered; they
                # have no NPC or coordinate-event position of their own.
                out.update(self.by_map.get(d,[]))
                continue
            if d not in maps or not isinstance(x,int): continue
            # Coordinate battles use their boundary exclusively. For an NPC
            # preserve the usual adjacent/counter interaction approach.
            if location.get('trigger')!='coord' and (d,location.get('entry')) not in moving_entries:
                actors=self._objects_by_script.get((d,location.get('entry')),[])
                if actors:
                    for actor in actors:
                        for px,py in state.get('_geometry_object_positions',{}).get(f"{d}:{actor['id']}",[]):out.update(self.near(d,px,py))
                else:out.update(self.near(d,x,y))
            approaches=state.get('_geometry_battle_approaches',{}).get(f"{d}:{location.get('entry')}",[])
            for px,py in approaches:out.update(self.tile_nodes.get((d,px,py),[]))
        return out

    def apply_effects(self,state,effects,map_=None,object_id=None,constants=None):
        """Record exact permanent coordinates from already accepted source paths.

        Call from the World's event/battle aftermath handler. Motion labels with
        cardinal walking commands are translated; unsupported native motion is
        retained as an unresolved geometry condition instead of guessed.
        """
        if constants is None:
            from manifest_world import constants as read_constants
            constants=read_constants()
        positions=state.setdefault('object_positions',{})
        for effect in effects:
            op,args=effect.get('op'),effect.get('args',[])
            if op not in ('setobjectxyperm','applymovement') or not args: continue
            match=re.match(r'data/maps/([^/]+)/',effect.get('source',''))
            d=match[1] if match else map_
            if not d: continue
            oid=object_id if args[0] in ('VAR_LAST_TALKED','LOCALID_LAST_TALKED') else self._aliases.get(d,{}).get(args[0])
            record=next((r for r in self.object_records if r['map']==d and r['id']==oid),None)
            if not record: continue # Includes the player, which owns traversal.
            key=f'{d}:{oid}'
            if op=='setobjectxyperm':
                try: positions[key]=[int(args[1]),int(args[2])]
                except (ValueError,IndexError): state.setdefault('geometry_unresolved',[]).append(effect)
                continue
            if len(args)<2 or args[1] not in self._script_index.labels: continue
            origin=positions.get(key)
            if origin is None:
                variants=record['variants'] or [dict(record['default'],conditions=[])]
                candidates={(v['x'],v['y']) for v in variants if self._conditions(v['conditions'],state,constants) is not False}
                if len(candidates)!=1:
                    state.setdefault('geometry_unresolved',[]).append(effect);continue
                origin=list(next(iter(candidates)))
            x,y=origin; unresolved=False
            for _n,line in self._script_index.labels[args[1]]['body']:
                if line.startswith('step_end'): break
                move=re.match(r'(?:walk|walk_fast|walk_slow|walk_faster|run|slide)_(up|down|left|right)$',line)
                if move:
                    dx,dy={'up':(0,-1),'down':(0,1),'left':(-1,0),'right':(1,0)}[move[1]];x+=dx;y+=dy
                elif not line.startswith(('face_','delay_','step_','pause','invisible','visible','emote')): unresolved=True
            if unresolved: state.setdefault('geometry_unresolved',[]).append(effect)
            else: positions[key]=[x,y]

    def source_model(self):
        return dict(objects=self.object_records,coordinate_triggers=self.coordinate_records,puzzle_transitions=self.puzzle_transitions,
            native_rotating_gates=[dict(map=d,gates=model['gates'],tiles=sorted(model['tiles']),source=model['source']) for d,model in self.native_rotating.items()],diagnostics=self.object_diagnostics,
            native_moving_objects=[dict(map=d,actors=[dict(id=a['record']['id'],color=a['color'],ring=a['ring'],direction=a['direction']) for a in model['actors']],switches=[dict(x=x,y=y,color=color) for (x,y),color in model['switches'].items()],source=model['source']) for d,model in self.native_moving.items()],
            native_beam_switches=[dict(map=d,tiles=[dict(x=x,y=y,initially_solid=solid) for (x,y),solid in model['tiles'].items()],switches=sorted(model['switches']),source=model['source']) for d,model in self.native_beams.items()],
            citations=['src/event_object_movement.c GetCollisionAtCoords; native object collision and fixed movement',
                'src/trainer_see.c:438-482 GetSortedTrainerObjects; local ID battle-trigger priority',
                'src/battle_setup.c:1297-1301 GetTrainerFlagFromScriptPointer; actual opponentA sight flag',
                'src/trainer_see.c CheckTrainer; fixed facing and all-direction sight checks',
                'data/maps/*/map.json; actual object positions, flags, movement and sight',
                'data/maps/*/scripts.inc; actual map callback position/movement variants'])
