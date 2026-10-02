#!/usr/bin/env python3
"""Build the requested source-backed battle/availability progression manifest."""
from __future__ import annotations

import argparse
import faulthandler
import copy
import hashlib
import json
import pickle
import re
from datetime import datetime, timezone
from pathlib import Path

import reference_pool as rp
import manifest_battle_progression as bp
import manifest_item_sources as item_sources
import manifest_pokemon_sources as pokemon_sources
from manifest_world import World

ROOT=rp.ROOT


def serial(value):
    if isinstance(value,set):return sorted(value)
    if isinstance(value,Path):return str(value)
    raise TypeError(type(value).__name__)


def name(token):
    return re.sub(r'^(SPECIES|ITEM|TRAINER)_','',token).replace('_',' ').title()


def resources(state):
    return dict(pokemon=set(state['species']),items=set(state['items']),areas=set(state['maps']))


def delta(before,after):
    return {kind:sorted(after[kind]-before[kind]) for kind in before}


def battle_items():
    """Items that can occupy a held-item slot in battle.

    Trainer battles disable Bag actions. Evolution items, medicine, field
    tools, story keys, decorations and preparation currency therefore do not
    belong in the tester-facing item ledger. Resulting evolutions/forms remain
    represented in the Pokemon ledger.
    """
    text=(ROOT/'src/data/items.h').read_text()
    starts=list(re.finditer(r'^\s*\[(ITEM_\w+)\]\s*=\s*$',text,re.M))
    held=set()
    for index,match in enumerate(starts):
        body=text[match.end():starts[index+1].start() if index+1<len(starts) else len(text)]
        hold=re.search(r'\.holdEffect\s*=\s*(HOLD_EFFECT_\w+)',body)
        if hold and hold[1]!='HOLD_EFFECT_NONE':held.add(match[1])
    # Form-change held items are battle equipment even if their ItemInfo block
    # uses a dedicated evolution marker instead of an ordinary hold effect.
    forms=(ROOT/'src/data/pokemon/form_change_tables.h').read_text()
    held.update(re.findall(r'FORM_CHANGE_BATTLE_(?:MEGA_EVOLUTION_ITEM|PRIMAL_REVERSION)\s*,\s*SPECIES_\w+\s*,\s*(ITEM_\w+)',forms))
    return held


def load_model(quick=False):
    print('Loading source index',flush=True)
    b=rp.Builder();parser=bp.ProgressionParser(b.scripts)
    print('Extracting battle entries',flush=True)
    battles=bp.extract_battle_nodes(b.scripts)
    keys=set(f for f,*_ in rp.mr.STORY_EVENTS)
    keys.update({'VAR_PETALBURG_GYM_STATE','VAR_PETALBURG_WOODS_STATE','VAR_ROUTE110_STATE',
                 'VAR_ROUTE119_STATE','VAR_MT_PYRE_STATE','VAR_SOOTOPOLIS_CITY_STATE',
                 'FLAG_DEFEATED_ELITE_4_SIDNEY','FLAG_DEFEATED_ELITE_4_PHOEBE',
                 'FLAG_DEFEATED_ELITE_4_GLACIA','FLAG_DEFEATED_ELITE_4_DRAKE',
                 'FLAG_EC_FINALE_DEOXYS_RESOLVED','FLAG_RECEIVED_SS_TICKET','FLAG_RECEIVED_AURORA_TICKET'})
    keys.add('FLAG_KECLEON_FLED_FORTREE') # FortreeCity/scripts.inc:84 opens the Gym approach.
    # Story NPCs can open the next scene without a battle of their own (Stern's
    # interview opens the submarine scene). Their visibility writes are part
    # of access, even when no trainer script reads those flags directly.
    keys.update(str(o.get('flag')) for map_ in b.scripts.geo.maps.values()
                for o in map_.get('object_events',[]) if str(o.get('flag')).startswith('FLAG_HIDE_'))
    for node in battles['battles']:
        for path in node['paths']+node['victory_paths']:
            for row in path['conditions']:
                keys.update(k for k in re.findall(r'\b(?:FLAG|VAR)_\w+',row['key']+' '+str(row['value'])) if not k.startswith('VAR_0x8'))
    keys.update({'VAR_BIRCH_LAB_STATE','VAR_ROUTE101_STATE','VAR_LITTLEROOT_HOUSES_STATE_BRENDAN',
                 'VAR_LITTLEROOT_HOUSES_STATE_MAY','VAR_LITTLEROOT_TOWN_STATE','VAR_LITTLEROOT_RIVAL_STATE'})
    for path in (ROOT/'data/maps').glob('*/scripts.inc'):
        text=path.read_text()
        keys.update(re.findall(r'map_script_2\s+(VAR_\w+)',text))
    keys.add('FLAG_DOCK_REJECTED_DEVON_GOODS')
    keys={k for k in keys if k not in ('VAR_RESULT','VAR_FACING')
          and not k.startswith(('VAR_0x8','VAR_CONTEST','VAR_BATTLE_FRONTIER','VAR_CABLE_CLUB','VAR_UNION_ROOM'))}
    print('Extracting completed story events',len(keys),flush=True)
    # Rental, contest and multiplayer activity requirements are retained by
    # their acquisition sources. Their private UI scratch variables do not
    # advance the campaign and must not be combined into its story closure.
    story_maps={m for m in b.scripts.geo.maps
                if not m.startswith(('BattleFrontier','TrainerHill'))
                and not any(t in m for t in ('BattleTent','ContestHall','ContestLobby',
                                             'PokemonCenter_2F','CableClub','UnionRoom'))}
    event_cache=ROOT/'work/progression-manifest/cache/story.completed.pickle'
    signature=hashlib.sha256()
    cache_sources={x['file'] for x in b.scripts.labels.values()}|{Path(bp.__file__)}
    cache_sources.update((ROOT/'data/maps').glob('*/map.json'))
    cache_sources.update((ROOT/'src').glob('*.c'))
    cache_sources.update((ROOT/'include').rglob('*.h'))
    cache_sources.add(ROOT/'scripts/manifest_native_predicates.py')
    for source in sorted(cache_sources):
        signature.update(str(source).encode());signature.update(source.read_bytes())
    signature.update(json.dumps([sorted(keys),sorted(story_maps)]).encode())
    digest=signature.hexdigest()
    cached=pickle.loads(event_cache.read_bytes()) if event_cache.exists() else None
    if cached and cached['signature']==digest:
        events=cached['events']
    else:
        def progress(root,event,phase):
            if phase=='start':print('Story event',root,event[0],flush=True)
        events=bp.extract_completed_event_transitions(b.scripts,keys,maps=story_maps,progress=progress)
        event_cache.write_bytes(pickle.dumps({'signature':digest,'events':events}))
    print('Loading Pokemon sources',flush=True)
    pokemon=pokemon_sources.PokemonSources(b)
    pcache=ROOT/'work/progression-manifest/cache/pokemon.enriched.json'
    if pcache.exists():
        pdata=json.loads(pcache.read_text());pokemon.rows=pdata['sources']
    elif not quick:pokemon.enrich_eligibility(parser)
    raw=ROOT/'work/progression-manifest/cache/items.raw.json'
    final_items=ROOT/'work/progression-manifest/cache/items.enriched.json'
    if quick and raw.exists():items=json.loads(raw.read_text())
    elif not quick and final_items.exists():
        items=json.loads(final_items.read_text())
        if not items.get('fully_certified'):raise RuntimeError('Item source cache is not certified')
    else:items=item_sources.build_item_sources(b,parser=parser,enrich=not quick)
    print('Compacting item eligibility',flush=True)
    for row in items['sources']:
        row.pop('controls',None)
        if row.get('eligibility_paths'):
            paths={json.dumps({'conditions':p['conditions'],'prior_battles':p.get('prior_battles',[])},sort_keys=True):
                   {'conditions':p['conditions'],'prior_battles':p.get('prior_battles',[])} for p in row['eligibility_paths']}
            row['eligibility_paths']=list(paths.values())
    print('Building collision graph',flush=True)
    return World(b,pokemon,items,battles,events),battles,events,items


def snapshot(world,state):
    return dict(cap=state['cap'],flags=sorted(state['flags']),vars=state['vars'],
                defeated=sorted(state['defeated']),maps=sorted(state['maps']),
                completed_sites=sorted(state['completed_sites']),
                captured_battle_sites=sorted(state['captured_battle_sites']),
                money=state['money'],coins=state['coins'],funding_possible=state['funding_possible'])


def build(world,battles,events,items,limit=80):
    state=world.initial();previous=dict(pokemon=set(),items=set(),areas=set())
    periods=[];placed=set();finished=False
    for i in range(limit):
        details=world.close(state);current=resources(state)
        actual=[n for n in world.battles if world.eligible(n,state)]
        alternatives=[n for n in world.battles if world.eligible(n,state,selectors=True) and n['site_id'] not in placed]
        periods.append(dict(index=i+1,label='Before the Route 103 rival' if i==0 else f'Progression frontier {i+1}',
            new=delta(previous,current),available={k:sorted(v) for k,v in current.items()},
            battles=[n['site_id'] for n in alternatives],state=snapshot(world,state),
            source_rows=details,after_victory=[]))
        print(f'frontier {i+1}: cap{state["cap"]}, {len(state["maps"])} maps, '
              f'{len(state["species"])} Pokemon, {len(state["items"])} items, {len(actual)} actual battles',flush=True)
        for node in alternatives:placed.add(node['site_id'])
        previous=current
        if not actual:break
        # Ordinary fights within an unchanged frontier can be taken in any
        # order. Each gate-bearing victory is recorded separately below.
        changed=False
        for node in actual:
            if not world.eligible(node,state):continue
            before=resources(state);candidate=world.win(node,state)
            if candidate['completed_sites']==state['completed_sites']:continue
            baseline=snapshot(world,state);after_details=world.close(candidate)
            after=resources(candidate)
            periods[-1]['after_victory'].append(dict(battle=node['site_id'],new=delta(before,after),
                available={k:sorted(v) for k,v in after.items()},state=snapshot(world,candidate),
                source_rows=after_details,
                next_battles=[n['site_id'] for n in world.battles if world.eligible(n,candidate)],
                flags_added=sorted(candidate['flags']-state['flags']),vars_changed={k:v for k,v in candidate['vars'].items() if state['vars'].get(k,0)!=v},
                reference_previous_victories=sorted(state['defeated'])))
            state=candidate;changed=True
            if any('BUFFEL' in t for t in node['trainers']):
                finished=True
                break
        if not changed or finished:break
    world.last_state=state
    authored={b.trainer for b in world.builder.scripts and rp.teams.read_teams()}
    included={t for n in world.battles if n['site_id'] in placed for t in n['trainers']}
    diagnostics=dict(parser=battles['diagnostics']+events['diagnostics'],
                     conditions=[dict(key=k,sources=sorted(v)) for k,v in sorted(world.unresolved.items())],
                     unplaced_battles=[n['site_id'] for n in world.battles if n['site_id'] not in placed],
                     unplaced_trainers=sorted(authored-included))
    branch_data={b.trainer:b for b in rp.teams.read_teams()}
    encounter_order=[]
    earliest={n:i for i,p in enumerate(periods) for n in p['battles']}
    for node in sorted(world.battles,key=lambda n:(earliest.get(n['site_id'],9999),min((branch_data[t].encounter for t in n['trainers'] if t in branch_data),default=9999),n['site_id'])):
        if node['site_id'] not in earliest:continue
        branches=[branch_data[t] for t in node['trainers'] if t in branch_data]
        group=tuple(sorted({b.encounter for b in branches})) or (node['site_id'],)
        if group not in encounter_order:encounter_order.append(group)
        node['battle_number']=encounter_order.index(group)+1
        cap=periods[earliest[node['site_id']]]['state']['cap']
        node['team']=[]
        for branch in branches:
            for mon in branch.mons:
                def level(percent,drop):
                    lead=mon.offset+2
                    if lead>0:lead=(lead*percent+50)//100
                    return max(1,min(100,cap+lead-(cap*drop+50)//100)-(branch.cls in ('regular','casual')))
                node['team'].append(dict(species='SPECIES_'+mon.species,item='ITEM_'+mon.item,
                    ability='ABILITY_'+mon.ability,nature='NATURE_'+mon.nature,
                    moves=['MOVE_'+m for m in mon.moves],levels=[level(25,15),level(60,0),level(100,0)],offset=mon.offset))
        node['level_note']='Levels shown for the earliest available cap; delayed fights scale with the current cap.'
        if any('_ROUTE_103_' in t or '_ROUTE_110_' in t or '_ROUTE_119_' in t for t in node['trainers']):
            node['regional_note']='The rival retains the regional starter left unchosen by your pair; non-Hoenn choices use the native regional starter set.'
    return dict(schema_version=1,generated_at=datetime.now(timezone.utc).isoformat(),
                scope={'campaign_end':'Buffel','item_exclusions':['wild held items','stolen items','Pickup ability rewards'],
                       'availability_rule':'After each victory, exhaust all legally reachable areas and nonbattle acquisitions to a fixed point before listing any next possible fight. Optional detours, Surf areas and caves count immediately; another required battle blocks only the resources behind it.',
                       'initial_reference_choices':{'generation':3,'first':'Treecko','second':'Mudkip','gender':'male'},
                       'branches':'Other starter/partner branches retain their source conditions; optional battles can be reordered.'},
                periods=periods,battles=world.battles,pokemon_sources=world.pokemon.rows,
                evolution_edges=world.pokemon.evolutions,form_transitions=world.pokemon.forms,
                item_sources=world.items,services=items['services'],
                story_transitions=world.events,optional_activities=items.get('activities',[]),
                diagnostics=diagnostics,finale_reached=finished)


def running_ledger(manifest):
    """One append-only availability list; never store a pool per battle."""
    allowed_items=battle_items()
    ledger=[];known_battles=set();known_sources={'items':set(),'pokemon':set()}
    known_resources={key:set() for key in ('pokemon','items','areas')}
    previous_state={}
    def append(new,battle_ids,state,sources):
        nonlocal previous_state
        new={key:sorted(set(values)-known_resources[key]) for key,values in new.items()}
        new['items']=[item for item in new['items'] if item in allowed_items]
        for key,values in new.items():known_resources[key].update(values)
        introduced=[b for b in battle_ids if b not in known_battles]
        known_battles.update(introduced)
        source_additions={}
        for kind,key in [('items','item_sources'),('pokemon','pokemon_sources')]:
            now=set(sources.get(key,[]));source_additions[kind]=sorted(now-known_sources[kind]);known_sources[kind].update(now)
        changes={}
        for key,value in state.items():
            if key in ('flags','defeated','maps','completed_sites','captured_battle_sites'):
                old=set(previous_state.get(key,[]));now=set(value)
                if now!=old:changes[key]={'add':sorted(now-old),'remove':sorted(old-now)}
            elif key=='vars':
                changed={k:v for k,v in value.items() if previous_state.get(key,{}).get(k)!=v}
                if changed:changes[key]=changed
            elif value!=previous_state.get(key):changes[key]=value
        if not ledger or introduced or any(new.values()) or any(source_additions.values()) or changes:
            ledger.append(dict(index=len(ledger)+1,new_pokemon=new['pokemon'],new_items=new['items'],
                new_areas=new['areas'],new_battles=introduced,new_source_indices=source_additions,
                level_cap=state['cap'],state_changes=changes))
        previous_state=copy.deepcopy(state)
    for period in manifest['periods']:
        append(period['new'],period['battles'],period['state'],period['source_rows'])
        for boundary in period['after_victory']:
            append(boundary['new'],boundary['next_battles'],boundary['state'],boundary['source_rows'])
    battle_keys={'id','site_id','trainers','kind','command','label','source','locations','wild_species',
                 'partner','team','level_note','regional_note','display_name','dynamic_opponent_pool','activity_requirement'}
    battle_rows=[]
    for node in manifest['battles']:
        if node['site_id'] not in known_battles:continue
        row={k:v for k,v in node.items() if k in battle_keys}
        row['first_available_entry']=next(e['index'] for e in ledger if node['site_id'] in e['new_battles'])
        row['requirements']=[dict(conditions=p['conditions'],prior_battles=p.get('prior_battles',[])) for p in node['paths']]
        battle_rows.append(row)
    # Numbers identify first availability; simultaneous alternatives retain a
    # shared position in the running record rather than imply a forced order.
    for number,row in enumerate(sorted(battle_rows,key=lambda r:(r['first_available_entry'],r['site_id'])),1):row['battle_number']=number
    source_keys={'id','species','key','kind','where','detail','quantity','price','cost','cost_rule',
                 'selection','requires','needs_species','needs_items','activity_requirement','campaign_scope',
                 'ownership_condition','capture_sites','cite','citations','conditions','native_conditions'}
    sources={}
    for key in ('pokemon_sources','item_sources'):
        rows=manifest[key]
        if key=='item_sources':rows=[row for row in rows if row.get('key') in allowed_items]
        sources[key]=[{k:v for k,v in row.items() if k in source_keys} for row in rows]
    result={k:manifest[k] for k in ('generated_at','scope','services','optional_activities','finale_reached','diagnostics')}
    result.update(schema_version=2,ledger=ledger,battles=battle_rows,**sources,
                  evolution_edges=manifest['evolution_edges'],form_transitions=manifest['form_transitions'],
                  instructions='Before a battle, accumulate additions through its first_available_entry, inclusive. Never include entries after that boundary. Items are held battle equipment only. Evolutions and forms are represented as Pokemon; their field evolution items are intentionally omitted. New battles are alternatives, not a required order.')
    return result


def main():
    p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);p.add_argument('--quick',action='store_true');p.add_argument('--limit',type=int,default=80)
    args=p.parse_args();faulthandler.dump_traceback_later(60,repeat=True);args.out.mkdir(parents=True,exist_ok=True)
    world,battles,events,items=load_model(args.quick)
    manifest=build(world,battles,events,items,args.limit)
    manifest['qualification']='development probe' if args.quick else 'source progression analysis'
    ledger=running_ledger(manifest)
    (args.out/'manifest.json').write_text(json.dumps(ledger,separators=(',',':'),default=serial))
    (args.out/'source-state.pickle').write_bytes(pickle.dumps(world.last_state))
    if not args.quick:
        import manifest_render
        manifest_render.render(json.loads(json.dumps(manifest,default=serial)),args.out)
    print('wrote',args.out/'manifest.json',flush=True)


if __name__=='__main__':main()
