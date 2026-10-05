#!/usr/bin/env python3
"""Retained opening preparations at audited campaign access states.

These are lower-bound legal preparations, never maximal difficulty calibration.
Only the explicitly audited early states are supported. A trainer's
chronology or authored strict_cap cannot establish physical access.
"""
from __future__ import annotations
import argparse
from collections import deque
import copy
import hashlib
from functools import lru_cache
import json
from pathlib import Path
import re
import struct
import sys

from battle_arsenal import ROOT, source_evidence, source_fingerprint
from battle_opening_arsenal import _opening_scenario, demand, rules, evolution_path, number, apply_owned_pokerus_plan, early_farm_rules
from verify_trainer_ability_legality import preprocess_species_info, resolve_species
sys.path.insert(0,str(ROOT/'scripts/playthrough'))
from battle_driver import find_trainer_script, parse_script_file, trainer_position
from generate_battle_suite import opponent_catalogue, canonical, digest_bytes
from export_opening_batch import c_function, run_c_probe

MAPS=('OldaleTown','Route102','PetalburgCity','Route104','PetalburgWoods','RustboroCity','RustboroCity_Gym',
      'Route116','Route115','Seaspray_Cave','RusturfTunnel')
EARLY_PROFILES=('after_rival','after_wally','after_woods','after_roxanne')
LATER_PROFILES=('after_rusturf','after_devon','after_steven_letter','after_brawly_meeting','after_brawly','after_museum_grunt','after_museum',
                'after_birch_registration','after_route110','after_route110_and_wally','after_wattson','after_flannery')
DELAYED_CANDIDATES=('after_wattson_brawly_unbeaten','after_flannery_brawly_unbeaten')
PROFILE_NAMES=EARLY_PROFILES+LATER_PROFILES+DELAYED_CANDIDATES
MAPS+=tuple(['Route104_MrBrineysHouse', 'DewfordTown', 'DewfordTown_Gym', 'Route106', 'GraniteCave_1F', 'GraniteCave_B1F', 'GraniteCave_B2F', 'GraniteCave_StevensRoom', 'Route109', 'SlateportCity', 'SlateportCity_OceanicMuseum_1F', 'SlateportCity_OceanicMuseum_2F', 'SlateportCity_NameRatersHouse', 'SlateportCity_SternsShipyard_1F', 'Route110', 'MauvilleCity', 'MauvilleCity_Gym', 'MauvilleCity_BikeShop', 'MauvilleCity_House1', 'Route111', 'Route112', 'FieryPath', 'Route113', 'FallarborTown', 'Route114', 'MeteorFalls_1F_1R', 'Route112_CableCarStation', 'MtChimney', 'JaggedPass', 'LavaridgeTown', 'LavaridgeTown_Gym_1F', 'LavaridgeTown_Gym_B1F'])
SOURCE_PATHS=('data/maps/Route103/scripts.inc','data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc',
              'data/scripts/guided_tutorials.inc','data/maps/OldaleTown/scripts.inc',
              'data/maps/PetalburgCity/scripts.inc','data/maps/PetalburgCity_Gym/scripts.inc',
              'data/maps/PetalburgWoods/scripts.inc','data/scripts/new_game.inc','src/caps.c',
              'src/field_control_avatar.c','src/event_object_movement.c','src/metatile_behavior.c',
              'src/battle_setup.c','include/global.fieldmap.h','include/constants/metatile_behaviors.h',
              'data/layouts/layouts.json','data/maps/RustboroCity_Gym/scripts.inc',
              'data/maps/RustboroCity/scripts.inc','data/maps/RusturfTunnel/scripts.inc')


def read(path):return (ROOT/path).read_text()


def terrain_behaviors():
    enum=read('include/constants/metatile_behaviors.h').split('enum {',1)[1].split('};',1)[0]
    enum=re.sub(r'//[^\n]*|/\*.*?\*/','',enum,flags=re.S)
    if '=' in enum:raise ValueError('Metatile enum now needs explicit value evaluation')
    names={name:i for i,name in enumerate(re.findall(r'\bMB_\w+\b',enum))}
    rows=re.findall(r'\[(MB_\w+)\]\s*=\s*([^,\n]+)',read('src/metatile_behavior.c'))
    water={names[name]for name,flags in rows if 'TILE_FLAG_SURFABLE' in flags}
    land={names[name]for name,flags in rows if 'TILE_FLAG_HAS_ENCOUNTERS' in flags and 'TILE_FLAG_SURFABLE' not in flags}
    return water,land


def _early_profile(stage,opening):
    if stage not in EARLY_PROFILES:raise ValueError('Campaign access profile is not audited: '+stage)
    demand('data/maps/Route103/scripts.inc',['setflag FLAG_DEFEATED_RIVAL_ROUTE103'])
    demand('data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc',
           ['setflag FLAG_ADVENTURE_STARTED','setvar VAR_BIRCH_LAB_STATE, 5'])
    demand('data/scripts/guided_tutorials.inc',['setvar VAR_LITTLEROOT_RIVAL_STATE, 4','setflag FLAG_EC_RIVAL_DEXNAV_TUTORIAL_COMPLETE'])
    demand('data/maps/OldaleTown/scripts.inc',['call_if_unset FLAG_ADVENTURE_STARTED, OldaleTown_EventScript_BlockWestEntrance'])
    flags=copy.deepcopy(opening['progression_flags'])
    flags.update({'FLAG_DEFEATED_RIVAL_ROUTE103':True,'FLAG_ADVENTURE_STARTED':True,
                  'FLAG_SYS_POKEDEX_GET':True,'FLAG_SYS_NATIONAL_DEX':True,'FLAG_RECEIVED_POKEDEX_FROM_BIRCH':True,
                  'FLAG_RECEIVED_DEXNAV':True,'FLAG_EC_RIVAL_DEXNAV_TUTORIAL_COMPLETE':True,
                  **{'FLAG_BADGE%02d_GET'%i:False for i in range(1,9)},'FLAG_DEFEATED_RUSTBORO_GYM':False,
                  'FLAG_RECEIVED_POKENAV':False,'FLAG_SYS_RECEIVED_KEYSTONE':False,'FLAG_RECEIVED_BIKE':False})
    variables=copy.deepcopy(opening['progression_vars'])
    variables.update({'VAR_BIRCH_LAB_STATE':5,'VAR_LITTLEROOT_RIVAL_STATE':4,'VAR_OLDALE_TOWN_STATE':1,
                      'VAR_PETALBURG_CITY_STATE':0,'VAR_PETALBURG_GYM_STATE':0,'VAR_PETALBURG_WOODS_STATE':0,
                      'VAR_RUSTBORO_CITY_STATE':0})
    events=['First rival victory','Birch Pokédex handoff','Route101 DexNav demonstration']
    dependencies=[opening['trainer_id']]
    maps={'OldaleTown','Route102'}
    if stage in {'after_wally','after_woods','after_roxanne'}:
        demand('data/maps/PetalburgCity/scripts.inc',['setvar VAR_PETALBURG_CITY_STATE, 3'])
        demand('data/maps/PetalburgCity_Gym/scripts.inc',['setvar VAR_PETALBURG_GYM_STATE, 2'])
        variables.update(VAR_PETALBURG_CITY_STATE=3,VAR_PETALBURG_GYM_STATE=2)
        events.append('Norman/Wally capture and Leveler tutorial')
        maps|={'PetalburgCity','Route104','PetalburgWoods'}
    if stage in {'after_woods','after_roxanne'}:
        demand('data/maps/PetalburgWoods/scripts.inc',['trainerbattle_no_intro_double TRAINER_GRUNT_PETALBURG_WOODS',
                                                     'setvar VAR_PETALBURG_WOODS_STATE, 1'])
        variables['VAR_PETALBURG_WOODS_STATE']=1
        dependencies.append('TRAINER_GRUNT_PETALBURG_WOODS')
        maps|={'RustboroCity','RustboroCity_Gym','Route116','Route115','Seaspray_Cave','RusturfTunnel'}
        events.append('Woods researcher battle and departure')
    # This state owns no badges. Derive the fallback directly from cap owner.
    cap=int(re.search(r'return (\d+);\s*\}\s*else if',read('src/caps.c'))[1])
    if stage=='after_roxanne':
        demand('data/maps/RustboroCity_Gym/scripts.inc',[
            'setflag FLAG_DEFEATED_RUSTBORO_GYM','setflag FLAG_BADGE01_GET',
            'setvar VAR_RUSTBORO_CITY_STATE, 2','addvar VAR_PETALBURG_GYM_STATE, 1'])
        demand('data/maps/RustboroCity/scripts.inc',[
            'setflag FLAG_DEVON_GOODS_STOLEN','setvar VAR_RUSTBORO_CITY_STATE, 3',
            'setvar VAR_RUSTURF_TUNNEL_STATE, 2','setvar VAR_ROUTE116_STATE, 1',
            'clearflag FLAG_HIDE_RUSTURF_TUNNEL_PEEKO','clearflag FLAG_HIDE_RUSTURF_TUNNEL_AQUA_GRUNT'])
        flags.update(FLAG_BADGE01_GET=True,FLAG_DEFEATED_RUSTBORO_GYM=True,
                     FLAG_DEVON_GOODS_STOLEN=True,FLAG_RECOVERED_DEVON_GOODS=False,
                     FLAG_RETURNED_DEVON_GOODS=False,FLAG_HIDE_RUSTURF_TUNNEL_PEEKO=False,
                     FLAG_HIDE_RUSTURF_TUNNEL_AQUA_GRUNT=False,FLAG_HIDE_ROUTE_116_MR_BRINEY=False,
                     FLAG_HIDE_BRINEYS_HOUSE_MR_BRINEY=True,FLAG_HIDE_BRINEYS_HOUSE_PEEKO=True)
        variables.update(VAR_PETALBURG_GYM_STATE=3,VAR_RUSTBORO_CITY_STATE=3,
                         VAR_RUSTURF_TUNNEL_STATE=2,VAR_ROUTE116_STATE=1)
        dependencies.append('TRAINER_ROXANNE_1')
        cap=int(re.search(r'\{FLAG_BADGE01_GET,\s*(\d+)\}',read('src/caps.c'))[1])
        events+=['Roxanne Stone Badge','Rustboro theft scene; Rusturf rescue remains unbeaten']
    return {'flags':flags,'vars':variables,'maps':sorted(maps),'cap':cap,
            'preceding_battle_ids':dependencies,'overworld_dependencies':events,
            'scope':'Source transition prerequisites; victory dependencies are required, not an earned-save claim.'}


LATER_SOURCE_PATHS=('data/maps/RusturfTunnel/scripts.inc','data/maps/RustboroCity/scripts.inc',
 'data/maps/RustboroCity_DevonCorp_3F/scripts.inc','data/maps/Route104_MrBrineysHouse/scripts.inc',
 'data/maps/Route104/scripts.inc','data/maps/DewfordTown/scripts.inc','data/maps/GraniteCave_StevensRoom/scripts.inc',
 'data/maps/SlateportCity_OceanicMuseum_1F/scripts.inc','data/maps/SlateportCity_OceanicMuseum_2F/scripts.inc',
 'data/maps/Route110/scripts.inc','data/maps/MauvilleCity/scripts.inc','data/maps/MauvilleCity_Gym/scripts.inc',
 'data/maps/MauvilleCity_House1/scripts.inc','data/maps/MeteorFalls_1F_1R/scripts.inc','data/maps/MtChimney/scripts.inc',
 'data/maps/LavaridgeTown_Gym_1F/scripts.inc','data/maps/PetalburgCity_Gym/scripts.inc','src/field_move.c',
 'src/event_object_movement.c','src/caps.c','src/field_specials.c','src/scrcmd.c','src/fieldmap.c','include/constants/metatile_labels.h','data/maps/MauvilleCity_BikeShop/scripts.inc')


def _later_profile(stage,opening):
 if stage in DELAYED_CANDIDATES:
  parent='after_wattson'if stage==DELAYED_CANDIDATES[0]else 'after_flannery'
  candidate=_later_profile(parent,opening)
  candidate['flags'].update(FLAG_BADGE02_GET=False,FLAG_DEFEATED_DEWFORD_GYM=False)
  candidate['vars']['VAR_PETALBURG_GYM_STATE']-=1
  candidate['preceding_battle_ids'].remove('TRAINER_BRAWLY_1')
  candidate.update(stage=stage,physical_access_status='unresolved',source_legality_status='unresolved',
    source_blocker='Native Museum queue blocks both doors until Brawly victory; the Museum ending cannot unlock its own entrance.')
  return candidate
 if stage not in LATER_PROFILES:raise ValueError('Unsupported campaign stage')
 rank=LATER_PROFILES.index(stage);s=copy.deepcopy(_early_profile('after_roxanne',opening));f=s['flags'];v=s['vars'];items={}
 demand('src/event_object_movement.c',['FlagSet(GetObjectEventFlagIdByObjectEventId(objectEventId));'])
 demand('data/maps/RusturfTunnel/scripts.inc',['giveitem ITEM_DEVON_GOODS','setflag FLAG_RECOVERED_DEVON_GOODS',
  'setvar VAR_RUSTBORO_CITY_STATE, 5','setvar VAR_BRINEY_HOUSE_STATE, 1'])
 f.update(FLAG_DEVON_GOODS_STOLEN=False,FLAG_RECOVERED_DEVON_GOODS=True,
          FLAG_HIDE_RUSTURF_TUNNEL_AQUA_GRUNT=True,FLAG_HIDE_RUSTURF_TUNNEL_PEEKO=True,
          FLAG_HIDE_ROUTE_116_MR_BRINEY=True)
 v.update(VAR_RUSTBORO_CITY_STATE=5,VAR_BRINEY_HOUSE_STATE=1)
 s['preceding_battle_ids'].append('TRAINER_GRUNT_RUSTURF_TUNNEL');items['ITEM_DEVON_GOODS']=1
 if rank>=LATER_PROFILES.index('after_devon'):
  demand('data/maps/RustboroCity/scripts.inc',['giveitem ITEM_DREAM_BALL, 2','setflag FLAG_RETURNED_DEVON_GOODS'])
  demand('data/maps/RustboroCity_DevonCorp_3F/scripts.inc',['giveitem ITEM_LETTER','setflag FLAG_RECEIVED_POKENAV',
   'clearflag FLAG_HIDE_BRINEYS_HOUSE_MR_BRINEY','setvar VAR_DEVON_CORP_3F_STATE, 1','setvar VAR_RUSTBORO_CITY_STATE, 7'])
  f.update(FLAG_RETURNED_DEVON_GOODS=True,FLAG_SYS_POKENAV_GET=True,FLAG_RECEIVED_POKENAV=True,
           FLAG_HIDE_BRINEYS_HOUSE_MR_BRINEY=False,FLAG_HIDE_BRINEYS_HOUSE_PEEKO=False,
           FLAG_HIDE_ROUTE_116_WANDAS_BOYFRIEND=True,FLAG_HIDE_RUSTURF_TUNNEL_WANDAS_BOYFRIEND=False,
           FLAG_HIDE_RUSTURF_TUNNEL_WANDA=False,FLAG_HIDE_RUSTBORO_CITY_RIVAL=False)
  v.update(VAR_DEVON_CORP_3F_STATE=1,VAR_RUSTBORO_CITY_STATE=7,VAR_BRINEY_LOCATION=1)
  items.update(ITEM_LETTER=1,ITEM_DREAM_BALL=2)
  s['maps']+=['Route104_MrBrineysHouse','DewfordTown','DewfordTown_Gym','Route106','GraniteCave_1F','GraniteCave_B1F','GraniteCave_B2F','GraniteCave_StevensRoom']
 if rank>=LATER_PROFILES.index('after_steven_letter'):
  demand('data/maps/DewfordTown/scripts.inc',['goto_if_unset FLAG_DELIVERED_STEVEN_LETTER, DewfordTown_EventScript_ReturnToPetalburgPrompt'])
  demand('data/maps/GraniteCave_StevensRoom/scripts.inc',['setflag FLAG_DELIVERED_STEVEN_LETTER','giveitem ITEM_EXPERT_BELT',
   'setflag FLAG_REGISTERED_STEVEN_POKENAV','removeobject LOCALID_STEVEN'])
  f.update(FLAG_DELIVERED_STEVEN_LETTER=True,FLAG_REGISTERED_STEVEN_POKENAV=True,FLAG_HIDE_GRANITE_CAVE_STEVEN=True)
  items.pop('ITEM_LETTER');items['ITEM_EXPERT_BELT']=1
  f.update(FLAG_HIDE_BRINEYS_HOUSE_MR_BRINEY=True,FLAG_HIDE_BRINEYS_HOUSE_PEEKO=True,
           FLAG_HIDE_MR_BRINEY_DEWFORD_TOWN=False,FLAG_HIDE_MR_BRINEY_BOAT_DEWFORD_TOWN=False)
  s['maps']+=['Route109','SlateportCity','SlateportCity_OceanicMuseum_1F','SlateportCity_OceanicMuseum_2F',
              'SlateportCity_NameRatersHouse','SlateportCity_SternsShipyard_1F']
 if rank>=LATER_PROFILES.index('after_brawly_meeting'):
  demand('data/maps/SlateportCity/scripts.inc',['SlateportCity_EventScript_Brawly::','removeobject LOCALID_BRAWLY'])
  f['FLAG_HIDE_SLATEPORT_CITY_BRAWLY']=True
 if rank>=LATER_PROFILES.index('after_brawly'):
  demand('data/maps/DewfordTown_Gym/scripts.inc',['setflag FLAG_BADGE02_GET','setflag FLAG_HIDE_SLATEPORT_CITY_TEAM_AQUA','addvar VAR_PETALBURG_GYM_STATE, 1'])
  f.update(FLAG_BADGE02_GET=True,FLAG_DEFEATED_DEWFORD_GYM=True,FLAG_HIDE_SLATEPORT_CITY_TEAM_AQUA=True)
  v['VAR_PETALBURG_GYM_STATE']=4
  s['cap']=int(re.search(r'\{FLAG_BADGE02_GET,\s*(\d+)\}',read('src/caps.c'))[1])
  s['preceding_battle_ids'].append('TRAINER_BRAWLY_1')
 if rank>=LATER_PROFILES.index('after_museum_grunt'):
  demand('data/maps/SlateportCity_OceanicMuseum_2F/scripts.inc',['goto_if_defeated TRAINER_GRUNT_MUSEUM_1','special HealPlayerParty'])
  s['preceding_battle_ids'].append('TRAINER_GRUNT_MUSEUM_1')
 if rank>=LATER_PROFILES.index('after_museum'):
  demand('data/maps/SlateportCity_OceanicMuseum_2F/scripts.inc',['trainerbattle_no_intro_double TRAINER_GRUNT_MUSEUM_1',
   'special HealPlayerParty','trainerbattle_no_intro_double TRAINER_ARCHIE_SLATEPORT',
   'setflag FLAG_DELIVERED_DEVON_GOODS','setflag FLAG_HIDE_ROUTE_110_TEAM_AQUA'])
  f.update(FLAG_DELIVERED_DEVON_GOODS=True,FLAG_HIDE_SLATEPORT_CITY_TEAM_AQUA=True,
           FLAG_HIDE_SLATEPORT_CITY_OCEANIC_MUSEUM_AQUA_GRUNTS=True,FLAG_HIDE_SLATEPORT_CITY_OCEANIC_MUSEUM_2F_CAPTAIN_STERN=True,
           FLAG_HIDE_ROUTE_110_TEAM_AQUA=True)
  v.update(VAR_SLATEPORT_OUTSIDE_MUSEUM_STATE=2,VAR_SLATEPORT_MUSEUM_1F_STATE=1,VAR_REGISTER_BIRCH_STATE=1,VAR_ROUTE110_STATE=0)
  f.update(FLAG_DEFEATED_WALLY_MAUVILLE=False,FLAG_HIDE_MAUVILLE_CITY_WALLY=False,FLAG_HIDE_MAUVILLE_CITY_WALLYS_UNCLE=False,FLAG_DEFEATED_MAUVILLE_GYM=False,FLAG_BADGE03_GET=False)
  items.pop('ITEM_DEVON_GOODS');s['preceding_battle_ids'].append('TRAINER_ARCHIE_SLATEPORT')
  s['maps']+=['Route110','MauvilleCity','MauvilleCity_Gym','MauvilleCity_BikeShop','MauvilleCity_House1']
 if rank>=LATER_PROFILES.index('after_birch_registration'):
  demand('data/maps/Route110/scripts.inc',['setvar VAR_REGISTER_BIRCH_STATE, 2','setflag FLAG_ENABLE_PROF_BIRCH_MATCH_CALL'])
  v['VAR_REGISTER_BIRCH_STATE']=2
  f['FLAG_ENABLE_PROF_BIRCH_MATCH_CALL']=True
 if rank>=LATER_PROFILES.index('after_route110'):
  branch=(3-opening['opening_parameters']['first']-opening['opening_parameters']['second']+2)%3
  gender='MAY' if opening['opening_parameters']['player_gender']=='male' else 'BRENDAN'
  script=read('data/maps/Route110/scripts.inc')
  label=re.search(r'case '+str(branch)+r', (Route110_EventScript_'+gender.title()+r'Battle\w+)',script)[1]
  rival=re.search(re.escape(label)+r'::[^\n]*\n\s*trainerbattle_no_intro_double\s+(TRAINER_\w+)',script)[1]
  demand('data/maps/Route110/scripts.inc',['setvar VAR_ROUTE110_STATE, 1','removeobject LOCALID_RIVAL_ON_BIKE'])
  v['VAR_ROUTE110_STATE']=1
  f.update(FLAG_HIDE_ROUTE_110_RIVAL=True,FLAG_HIDE_ROUTE_110_RIVAL_ON_BIKE=True)
  # Conservative native corridor prerequisites; not minimum unavoidable fights.
  s['preceding_battle_ids'] += ['TRAINER_ISABEL_1','TRAINER_KALEB',rival]
 if rank>=LATER_PROFILES.index('after_route110_and_wally'):
  demand('data/maps/MauvilleCity/scripts.inc',['trainerbattle_no_intro_double TRAINER_WALLY_MAUVILLE','setflag FLAG_DEFEATED_WALLY_MAUVILLE'])
  f.update(FLAG_DEFEATED_WALLY_MAUVILLE=True,FLAG_HIDE_MAUVILLE_CITY_WALLY=True,FLAG_HIDE_MAUVILLE_CITY_WALLYS_UNCLE=True)
  s['preceding_battle_ids'].append('TRAINER_WALLY_MAUVILLE')
 if rank>=LATER_PROFILES.index('after_wattson'):
  demand('data/maps/MauvilleCity_Gym/scripts.inc',['setflag FLAG_BADGE03_GET','addvar VAR_PETALBURG_GYM_STATE, 1'])
  demand('data/maps/MauvilleCity_House1/scripts.inc',['setflag FLAG_RECEIVED_HM06'])
  f.update(FLAG_BADGE03_GET=True,FLAG_DEFEATED_MAUVILLE_GYM=True,FLAG_RECEIVED_HM06=True)
  v['VAR_PETALBURG_GYM_STATE']=5
  s['cap']=int(re.search(r'\{FLAG_BADGE03_GET,\s*(\d+)\}',read('src/caps.c'))[1])
  s['preceding_battle_ids'].append('TRAINER_WATTSON_1')
  s['maps']+=['Route111','Route112','FieryPath','Route113','FallarborTown','Route114','MeteorFalls_1F_1R','Route112_CableCarStation','MtChimney','JaggedPass','LavaridgeTown','LavaridgeTown_Gym_1F','LavaridgeTown_Gym_B1F']
 if rank>=LATER_PROFILES.index('after_flannery'):
  demand('data/maps/MeteorFalls_1F_1R/scripts.inc',['setflag FLAG_HIDE_ROUTE_112_TEAM_MAGMA'])
  demand('data/maps/MtChimney/scripts.inc',['trainerbattle_no_intro_double TRAINER_MAXIE_MT_CHIMNEY','setflag FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY'])
  demand('data/maps/LavaridgeTown_Gym_1F/scripts.inc',['setflag FLAG_BADGE04_GET','addvar VAR_PETALBURG_GYM_STATE, 1'])
  f.update(FLAG_HIDE_ROUTE_112_TEAM_MAGMA=True,FLAG_HIDE_MT_CHIMNEY_TEAM_MAGMA=True,
           FLAG_HIDE_MT_CHIMNEY_TEAM_AQUA=True,FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY=True,
           FLAG_BADGE04_GET=True,FLAG_DEFEATED_LAVARIDGE_GYM=True)
  v.update(VAR_METEOR_FALLS_STATE=1,VAR_PETALBURG_GYM_STATE=6)
  s['cap']=int(re.search(r'\{FLAG_BADGE04_GET,\s*(\d+)\}',read('src/caps.c'))[1])
  s['preceding_battle_ids'] += ['TRAINER_COURTNEY_METEOR_FALLS','TRAINER_GRUNT_METEOR_FALLS',
                              'TRAINER_TABITHA_MT_CHIMNEY','TRAINER_GRUNT_MT_CHIMNEY_1',
                              'TRAINER_MAXIE_MT_CHIMNEY','TRAINER_FLANNERY_1']
 # Story milestones do not automatically execute Norman's Ring gift.
 f.setdefault('FLAG_HIDE_SLATEPORT_CITY_BRAWLY',False)
 f.setdefault('FLAG_DEFEATED_DEWFORD_GYM',False)
 f['FLAG_SYS_RECEIVED_KEYSTONE']=False
 s.update(stage=stage,story_items=items,minimum_museum_fee=50 if rank>=5 else 0,
   source_evidence=[source_evidence(ROOT/p)for p in LATER_SOURCE_PATHS],
   physical_access_status='unresolved'if rank>=LATER_PROFILES.index('after_wattson') else 'native_corridor_validated',
   maximal_arsenal_status='unresolved',scope='Source transition prerequisites; conditional native ferry/Granite/Slateport/Gym-door corridor validated, not an earned prefix.',
   unresolved=['Other physical routes beyond the native validated ferry corridor','Target-specific interaction fields','Optional quest/gym battle dependencies and rewards; extra Chimney trainer wins are conservative prerequisites',
               'Paid nature requests need source-matched harvested Berry counts; no nature service granted','No Mega Ring until Norman state6'])
 return s


def profile(stage,opening):
    return _early_profile(stage,opening)if stage in EARLY_PROFILES else _later_profile(stage,opening)


class EarlyGeometry:
    """Conservative ordinary-walking graph for the audited initial map set."""
    def __init__(self,stage,access=None,*,extra_maps=()):
        layouts={l['id']:l for l in json.loads(read('data/layouts/layouts.json'))['layouts']}
        self.maps={name:json.loads(read('data/maps/'+name+'/map.json')) for name in (*MAPS,*extra_maps)}
        self.by_id={m['id']:name for name,m in self.maps.items()}
        self.layouts={name:layouts[m['layout']] for name,m in self.maps.items()}
        self.words={};self.attrs={};self.evidence=[]
        self.water_behaviors,_=terrain_behaviors()
        default_hidden=set(re.findall(r'\bsetflag\s+(FLAG_\w+)',read('data/scripts/new_game.inc')))
        self.blocked={}
        for name,m in self.maps.items():
            l=self.layouts[name];data=(ROOT/l['blockdata_filepath']).read_bytes()
            self.words[name]=struct.unpack('<'+'H'*(l['width']*l['height']),data)
            files=['data/maps/'+name+'/map.json','data/maps/'+name+'/scripts.inc',l['blockdata_filepath']]
            for kind in ['primary','secondary']:
                folder=l[kind+'_tileset'].removeprefix('gTileset_')
                folder=re.sub(r'(?<!^)(?=[A-Z])','_',folder).lower()
                path='data/tilesets/'+kind+'/'+folder+'/metatile_attributes.bin'
                self.attrs[name,kind]=(ROOT/path).read_bytes();files.append(path)
            self.evidence += [source_evidence(ROOT/p) for p in files]
            self.blocked[name]={(o['x'],o['y']) for o in m['object_events']
                                if o.get('flag','0')=='0' or not (access or {}).get('flags',{}).get(o.get('flag'),o.get('flag') in default_hidden)}
            if name=='RusturfTunnel' and stage=='after_roxanne':
                # OnTransition moves the visible rescue actors one tile west.
                demand('data/maps/RusturfTunnel/scripts.inc',['setobjectxyperm LOCALID_PEEKO, 13, 4','setobjectxyperm LOCALID_GRUNT, 13, 5'])
                self.blocked[name]-={(14,4),(14,5)};self.blocked[name]|={(13,4),(13,5)}
            if name=='PetalburgWoods' and stage in {'after_rival','after_wally'}:
                self.blocked[name]|={(e['x'],e['y']) for e in m['coord_events']
                                    if e.get('var')=='VAR_PETALBURG_WOODS_STATE' and e.get('var_value')=='0'}
            if name=='Route110' and (access or {}).get('vars',{}).get('VAR_ROUTE110_STATE',0)==0:
                # A stopping trigger can start the rival; it cannot be crossed
                # as a free walking edge before that battle.
                self.blocked[name]|={(e['x'],e['y'])for e in m['coord_events']
                    if e.get('var')=='VAR_ROUTE110_STATE'and e.get('var_value')=='0'}
        if (access or {}).get('vars',{}).get('VAR_MAUVILLE_GYM_STATE',0)==1:
            if not access['flags'].get('FLAG_MAUVILLE_GYM_BARRIERS_STATE'):
                raise ValueError('First-switch alternate barrier flag is missing')
            self.apply_gym_switch_one()
        self.reached=self.walk(('OldaleTown',1,10))
        if stage in LATER_PROFILES+DELAYED_CANDIDATES and stage!='after_rusturf':
            # Native-validated scripted ferry corridor, not an ordinary map
            # adjacency edge. The source scripts own the actual sailing gates.
            demand('data/maps/Route104/scripts.inc',['showobjectat OBJ_EVENT_ID_PLAYER, MAP_DEWFORD_TOWN'])
            self.reached |= self.walk(('DewfordTown',11,9))
            if access['flags'].get('FLAG_DELIVERED_STEVEN_LETTER'):
                demand('data/maps/DewfordTown/scripts.inc',['goto_if_unset FLAG_DELIVERED_STEVEN_LETTER, DewfordTown_EventScript_ReturnToPetalburgPrompt',
                    'showobjectat OBJ_EVENT_ID_PLAYER, MAP_ROUTE109'])
                self.reached |= self.walk(('Route109',21,23))
        if access is not None:
            self.reached={node for node in self.reached if node[0] in access['maps']}

    def apply_gym_switch_one(self):
        """Actual first-switch alternate barriers; never defeated-Gym removal."""
        demand('data/maps/MauvilleCity_Gym/scripts.inc',[
            'setvar VAR_MAUVILLE_GYM_STATE, 1','special MauvilleGymSetDefaultBarriers',
            'special MauvilleGymPressSwitch','goto_if_set FLAG_MAUVILLE_GYM_BARRIERS_STATE, MauvilleCity_Gym_EventScript_SetAltBarriers'])
        demand('src/field_specials.c',['{ 0 + MAP_OFFSET, 15 + MAP_OFFSET}',
            'METATILE_MauvilleGym_PressedSwitch','METATILE_MauvilleGym_RaisedSwitch'])
        demand('src/scrcmd.c',['metatileId | MAPGRID_IMPASSABLE'])
        labels={n:int(v,0)for n,v in re.findall(r'^#define (METATILE_MauvilleGym_\w+)\s+(0x[0-9A-Fa-f]+)',read('include/constants/metatile_labels.h'),re.M)}
        script=read('data/maps/MauvilleCity_Gym/scripts.inc').split('MauvilleCity_Gym_EventScript_SetAltBarriers::',1)[1].split('\n\tend',1)[0]
        rows=re.findall(r'setmetatile (\d+), (\d+), (METATILE_MauvilleGym_\w+), ([01])',script)
        if len(rows)!=26:raise ValueError('First-switch barrier overlay changed')
        words=list(self.words['MauvilleCity_Gym']);width=self.layouts['MauvilleCity_Gym']['width']
        for x,y,token,collision in rows:
            i=int(y)*width+int(x)
            words[i]=(words[i]&0xf000)|labels[token]|(0xc00 if collision=='1'else 0)
        for i,(x,y)in enumerate([(0,15),(4,12),(3,9),(8,9)]):
            index=y*width+x
            words[index]=(words[index]&0xf000)|labels['METATILE_MauvilleGym_PressedSwitch'if i==0 else'METATILE_MauvilleGym_RaisedSwitch']
        self.words['MauvilleCity_Gym']=tuple(words)
        self.evidence += [source_evidence(ROOT/p)for p in ('src/field_specials.c','src/scrcmd.c','include/constants/metatile_labels.h')]

    def word(self,node):
        name,x,y=node;l=self.layouts[name]
        return self.words[name][y*l['width']+x] if 0<=x<l['width'] and 0<=y<l['height'] else None

    def behavior(self,node):
        tile=self.word(node)&0x3ff;kind='primary' if tile<512 else 'secondary'
        return struct.unpack_from('<H',self.attrs[node[0],kind],2*(tile if tile<512 else tile-512))[0]&255

    def floor(self,node):
        word=self.word(node)
        # Elevation1 also occurs on ordinary indoor floors (native Dewford
        # Gym receipt). The game's behavior flags, not elevation, own water.
        return (word is not None and not word&0xc00 and self.behavior(node)not in self.water_behaviors
                and (node[1],node[2])not in self.blocked[node[0]])

    def walk(self,start):
        demand('src/event_object_movement.c',['if (elevation == ELEVATION_TRANSITION)',
            'mapElevation == ELEVATION_MULTI_LEVEL','curElevation == ELEVATION_MULTI_LEVEL || prevElevation == ELEVATION_MULTI_LEVEL',
            'objEvent->currentElevation = curElevation;','ShiftStillObjectEventCoords(objectEvent);'])
        seen=set();states=set();q=deque()
        def push(node,elevation=None):
            if elevation is None:
                elevation=self.word(node)>>12
                if elevation==15:return # Warp/start on a bridge has no known layer.
            state=(node,elevation)
            if state not in states:states.add(state);seen.add(node);q.append(state)
        push(start)
        while q:
            node,z=q.popleft();name,x,y=node;l=self.layouts[name]
            for dx,dy in [(-1,0),(1,0),(0,-1),(0,1)]:
                target=(name,x+dx,y+dy)
                if self.floor(target):
                    nz=self.word(target)>>12
                    if z and nz not in {0,15,z}:continue
                    # Only ordinary terrain; unknown mobility stays unresolved.
                    if self.behavior(target) in {0x20,*range(0x38,0x60),*range(0xd0,0xd7)}:continue
                    push(target,z if nz==15 else nz)
                elif self.word(target) is None:
                    direction='left' if dx<0 else 'right' if dx>0 else 'up' if dy<0 else 'down'
                    for connection in self.maps[name].get('connections') or []:
                        other=self.by_id.get(connection['map'])
                        if connection['direction']!=direction or other is None:continue
                        destination=self.layouts[other];offset=connection['offset']
                        tx=destination['width']-1 if direction=='left' else 0 if direction=='right' else x-offset
                        ty=destination['height']-1 if direction=='up' else 0 if direction=='down' else y-offset
                        candidate=(other,tx,ty)
                        if self.floor(candidate):
                            nz=self.word(candidate)>>12
                            if not z or nz in {0,15,z}:push(candidate,z if nz==15 else nz)
            # Standard doors are activated from the tile immediately below;
            # ordinary staircase/cave warps can be stepped onto directly.
            for e in self.maps[name]['warp_events']:
                warp=(e['x'],e['y']);other=self.by_id.get(e['dest_map'])
                if other is None:continue
                if (x,y)!=warp and (x,y)!=(warp[0],warp[1]+1):continue
                door=self.behavior((name,*warp))
                if (x,y)!=warp and door not in {0x60,0x69}:continue
                targets=self.maps[other]['warp_events'];index=e['dest_warp_id']
                if not isinstance(index,int) or not 0<=index<len(targets):continue
                dest=targets[index];candidate=(other,dest['x'],dest['y'])
                # Arrival warps may be collision-marked stairs/doors. Enter the
                # interior tile immediately above when the warp isn't a floor.
                if self.floor(candidate):push(candidate)
                elif self.floor((other,dest['x'],dest['y']-1)):
                    push((other,dest['x'],dest['y']-1))
        return seen



def exact_field(name,xy,geometry):
    m=geometry.maps[name];behavior=geometry.behavior((name,*xy))
    if behavior==2:environment='BATTLE_ENVIRONMENT_GRASS'
    elif behavior==3:environment='BATTLE_ENVIRONMENT_LONG_GRASS'
    elif behavior in {6,33}:environment='BATTLE_ENVIRONMENT_SAND'
    elif m['map_type']=='MAP_TYPE_UNDERGROUND':
        demand('src/battle_setup.c',['case MAP_TYPE_UNDERGROUND:',
               'if (MetatileBehavior_IsIndoorEncounter(tileBehavior))','return BATTLE_ENVIRONMENT_CAVE;'])
        environment='BATTLE_ENVIRONMENT_BUILDING'if behavior==0x0b else 'BATTLE_ENVIRONMENT_CAVE'
    elif m['map_type']in {'MAP_TYPE_INDOOR','MAP_TYPE_SECRET_BASE'}:environment='BATTLE_ENVIRONMENT_BUILDING'
    else:environment='BATTLE_ENVIRONMENT_PLAIN'
    return {'map':name,'player_xy':list(xy),'weather':m['weather'],'environment':environment,
            'metatile_id':geometry.word((name,*xy))&0x3ff,'source_evidence':geometry.evidence+[source_evidence(ROOT/'src/battle_setup.c')]}


@lru_cache(maxsize=1)
def _species_acquisition_metadata(species_text):
    marks=list(re.finditer(r'\[(SPECIES_\w+)\]\s*=\s*\{',species_text));result={}
    for i,mark in enumerate(marks):
        block=species_text[mark.end():marks[i+1].start() if i+1<len(marks) else len(species_text)]
        items={kind:(re.search(r'\.'+kind+r'\s*=\s*(ITEM_\w+)',block)[1]
                     if re.search(r'\.'+kind+r'\s*=\s*(ITEM_\w+)',block) else 'ITEM_NONE')
               for kind in ('itemCommon','itemRare')}
        items['restricted']=bool(re.search(r'\.(?:isRestrictedLegendary|isMythical|isUltraBeast|isParadox|isSubLegendary)\s*=\s*(?:1|TRUE)\b',block))
        from player_star_rule import restricted_exceptions
        items['restricted'] |= mark[1] in restricted_exceptions()
        result[mark[1]]=items
    return result


def campaign_evolution(base,target,nature,r,geometry,access):
    if resolve_species(target,r['aliases'])!=resolve_species('SPECIES_LEAFEON',r['aliases']):
        return evolution_path(base,target,nature,{**r,'cap':access['cap']})
    if resolve_species(base,r['aliases'])!=resolve_species('SPECIES_EEVEE',r['aliases']):
        raise ValueError('Leafeon requires an owned Eevee, not an earlier evolved form')
    edge=next((e for e in r['evolutions'][resolve_species(base,r['aliases'])]
               if e['target']=='SPECIES_LEAFEON' and e['method']=='EVO_LEVEL'
               and e['conditions']==[['IF_IN_MAP','MAP_PETALBURG_WOODS']]),None)
    if edge is None or edge['level']>access['cap']:raise ValueError('Configured Leafeon map evolution changed')
    points=sorted(n for n in geometry.reached if n[0]=='PetalburgWoods')
    if not points:raise ValueError('Main Petalburg Woods evolution map is not reached')
    demand('src/pokemon.c',['case IF_IN_MAP:',
           '(gSaveBlock1Ptr->location.mapGroup) << 8 | gSaveBlock1Ptr->location.mapNum'])
    demand('src/party_menu.c',['targetSpecies = GetEvolutionTargetSpecies(mon, EVO_MODE_NORMAL'])
    return {'base_species':base,'target_species':target,'leveler_at_cap':access['cap'],
            'friendship':0,'scope':'Bonding to zero avoids earlier friendship evolutions; use Leveler on main Woods map.',
            'evolution_map':points[0][0],'evolution_xy':list(points[0][1:]),
            'steps':[{'from_species':resolve_species(base,r['aliases']),'to_species':resolve_species(target,r['aliases']),
                      'level':edge['level'],'conditions':edge['conditions']}]}


# Draft helper for insertion into battle_campaign_arsenal.py after native waves.
def apply_fidough_trades(plan,state,access,geometry,r):
    """Consume explicit donor ownership; fixed native recipient fields do not inherit."""
    if not isinstance(plan,list):raise ValueError('Trade preparation must be a finite list')
    if not plan:return [],[]
    owned=state.get('owned_monsters');defaults=state.get('pokemon_defaults_by_id')
    if owned is None:raise ValueError('NPC trade preparation requires explicit owned IDs')
    proofs=[];received=[]
    for row in plan:
        if not isinstance(row,dict) or set(row)!={'trade_id','outgoing_id','received_id'}:
            raise ValueError('Trade requires exactly a native trade ID and two ownership IDs')
        if row['trade_id']!='INGAME_TRADE_FIDOUGH':
            raise ValueError('Other NPC trades require proven donor and physical access paths')
        donor=row['outgoing_id'];ident=row['received_id']
        if donor not in owned or not isinstance(ident,str) or not re.fullmatch(r'[a-zA-Z0-9_-]+',ident) or ident in owned:
            raise ValueError('Missing donor or invalid/duplicate recipient ownership ID')
        name='RustboroCity_House1';label=name+'_EventScript_Trader'
        if name not in access['maps'] or name not in geometry.maps:
            raise ValueError('NPC trade map is outside source access profile')
        script=read('data/maps/'+name+'/scripts.inc')
        block=script.split(label+'::',1)[1].split('\n\n',1)[0]
        flag=re.search(r'goto_if_set (FLAG_\w+)',block)[1]
        if access['flags'].get(flag):raise ValueError('This finite NPC trade was already completed')
        demand('data/maps/'+name+'/scripts.inc',['setvar VAR_0x8008, INGAME_TRADE_FIDOUGH',
            'call Common_EventScript_ReturnNPCTradeItem','special CreateInGameTradePokemon',
            'special DoInGameTradeScene','setflag '+flag])
        actor=next(o for o in geometry.maps[name]['object_events'] if o['script']==label)
        point=(actor['x'],actor['y']+1)
        if actor.get('flag','0')!='0' or (name,*point)not in geometry.reached:
            raise ValueError('NPC trade lacks its proven ordinary walking approach')
        fields=re.search(r'\[INGAME_TRADE_FIDOUGH\]\s*=\s*\{(.*?)\n    \}',read('src/data/trade.h'),re.S)[1]
        def field(key):return re.search(r'\.'+key+r'\s*=\s*([^,\n]+)',fields)[1].strip()
        requested=field('requestedSpecies');species=field('species')
        if resolve_species(owned[donor]['species'],r['aliases'])!=resolve_species(requested,r['aliases']):
            raise ValueError('Outgoing owned Pokemon is not the native requested species')
        if field('heldItem')!='ITEM_NONE' or field('abilityNum')!='0':
            raise ValueError('NPC recipient loadout needs renewed source/native proof')
        personality=int(field('personality'),0)
        nature=next(token for token,value in re.findall(r'#define\s+(NATURE_\w+)\s+(\d+)\b',read('include/constants/pokemon.h'))
                    if int(value)==personality%number('include/constants/pokemon.h','NUM_NATURES'))
        demand('src/trade.c',['GetLevelFromBoxMonExp(boxmon)','MaxPlayerMonIVs(pokemon);',
            'SetPlayerMonBaselineEVs(pokemon);','friendship = 70;'])
        prior=copy.deepcopy(owned.pop(donor));defaults.pop(donor)
        owned[ident]={'species':species,'acquired_species':species,'method':'npc_trade','maps':[geometry.maps[name]['id']],
                      'trade_id':row['trade_id'],'consumed_owned_id':donor}
        defaults[ident]={'nature':nature,'ability':r['initial_abilities'][resolve_species(species,r['aliases'])],
                        'evs':list(r['evs']),'ivs':[r['iv']]*6,'friendship':70,'pokerus':0,'pp_bonuses':0,
                        'personality':personality,'ot_id':int(field('otId'),0)}
        state['selected_owned_ids']=[i for i in state['selected_owned_ids'] if i!=donor]
        state['resources'][prior['species']]-=1
        if not state['resources'][prior['species']]:del state['resources'][prior['species']]
        state['resources'][species]=state['resources'].get(species,0)+1
        access['flags'][flag]=True;received.append(ident)
        proofs.append({'method':'npc_trade','trade_id':row['trade_id'],'owned_mon_id':ident,
            'consumed_owned_id':donor,'requested_species':requested,'received_species':species,
            'personality':personality,'natural_nature':nature,'map':name,'xy':list(point),
            'held_item_acquired':'ITEM_NONE','outgoing_item':'Already-owned item remains in total resources after the native return-to-Bag-or-PC handoff.',
            'level_rule':'Native donor level, followed by the free Leveler up to the current cap',
            'native_receipt_refs':['work/battle-calibration/native-rustboro-fidough-trade-receipt-01/evidence.json']})
    state['source_evidence'] += [source_evidence(ROOT/p)for p in ('src/data/trade.h','src/trade.c','include/constants/trade.h',
        'data/maps/RustboroCity_House1/scripts.inc','data/maps/RustboroCity_House1/map.json')]
    return received,proofs


FISHING_EXTRA_MAPS=('Route101','Route103','LittlerootTown')
FISHING_ROUTES=('Route102','Route103','Route104')
FISHING_SOURCE_PATHS=('data/maps/LittlerootTown/scripts.inc','data/maps/LittlerootTown/map.json',
    'data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc','src/item_use.c','src/fishing.c',
    'src/wild_encounter.c','src/field_player_avatar.c','src/event_object_movement.c',
    'src/metatile_behavior.c','include/config/fishing.h','include/constants/wild_encounter.h',
    'include/global.fieldmap.h','include/constants/metatile_behaviors.h','src/data/items.h',
    'include/constants/items.h','include/constants/pokemon.h','include/config/battle.h')
FISHING_NATIVE_RECEIPTS={
    'mom':'work/battle-calibration/native-mom-old-rod-receipt-02/evidence.json',
    'Route102':'work/battle-calibration/native-route102-old-rod-receipt-01/evidence.json',
    'Route103':'work/battle-calibration/native-route103-old-rod-probe-02/evidence.json',
    'Route104':'work/battle-calibration/native-route104-old-rod-receipt-01/evidence.json'}


def acquire_mom_old_rod(state,access,geometry,gender):
    """Finite optional return after Birch; preserve the current later state."""
    if not all(access['flags'].get(flag) for flag in ('FLAG_DEFEATED_RIVAL_ROUTE103',
            'FLAG_RECEIVED_POKEDEX_FROM_BIRCH','FLAG_SYS_POKEDEX_GET')) or access['vars'].get('VAR_BIRCH_LAB_STATE')!=5:
        raise ValueError('Mom Old Rod requires first rival victory and completed Birch return')
    demand('data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc',[
        'setvar VAR_LITTLEROOT_TOWN_STATE, 3','setvar VAR_BIRCH_LAB_STATE, 5'])
    demand('data/maps/LittlerootTown/scripts.inc',['checkitemspace ITEM_OLD_ROD, 1',
        'giveitem ITEM_OLD_ROD','setflag FLAG_RECEIVED_OLD_ROD','setflag FLAG_RECEIVED_RUNNING_SHOES',
        'setflag FLAG_SYS_B_DASH','setvar VAR_LITTLEROOT_TOWN_STATE, 4',
        'setobjectxyperm LOCALID_MOM, 5, 9','setobjectxyperm LOCALID_MOM, 14, 9'])
    point=(5,10)if gender=='male'else(14,10)
    if ('LittlerootTown',*point)not in geometry.reached:
        raise ValueError('Mom return requires a reached dry interaction tile')
    state['resources']['ITEM_OLD_ROD']=1
    access['flags'].update(FLAG_RECEIVED_OLD_ROD=True,FLAG_RECEIVED_RUNNING_SHOES=True,
        FLAG_SYS_B_DASH=True,FLAG_HIDE_LITTLEROOT_TOWN_MOM_OUTSIDE=True)
    access['vars']['VAR_LITTLEROOT_TOWN_STATE']=4
    access['overworld_dependencies'].append('Optional return to Mom after Birch: Old Rod and Running Shoes')
    return {'method':'mom_old_rod','item':'ITEM_OLD_ROD','quantity':1,'cost':0,
        'map':'LittlerootTown','xy':list(point),'minimum_prefix_vars':{'VAR_LITTLEROOT_TOWN_STATE':3,'VAR_BIRCH_LAB_STATE':5},
        'conditions':'One free source handoff with key-pocket space, after first rival/Birch; no badge/time/demo condition.',
        'native_receipt_refs':[FISHING_NATIVE_RECEIPTS['mom']],
        'scope':'Source-possible optional return; native handoff from conditional prerequisites, not an earned whole trip.'}


def old_rod_capture_row(capture,state,geometry,tables):
    name=capture['map'];point=tuple(capture['xy']);facing=capture.get('facing')
    if state['resources'].get('ITEM_OLD_ROD',0)!=1:raise ValueError('Fishing capture has no source-acquired Old Rod')
    if name not in FISHING_ROUTES:raise ValueError('Fishing outside positively audited early routes remains unresolved')
    directions={'UP':(0,-1),'DOWN':(0,1),'LEFT':(-1,0),'RIGHT':(1,0)}
    if facing not in directions:raise ValueError('Old Rod capture requires an explicit native facing direction')
    dx,dy=directions[facing];water=(name,point[0]+dx,point[1]+dy)
    word=geometry.word(water)
    enum=read('include/constants/metatile_behaviors.h').split('enum {',1)[1].split('};',1)[0]
    enum=re.sub(r'//[^\n]*|/\*.*?\*/','',enum,flags=re.S)
    if '=' in enum:raise ValueError('Fishing metatile enum needs explicit value evaluation')
    names={name:i for i,name in enumerate(re.findall(r'\bMB_\w+\b',enum))}
    predicate=read('src/metatile_behavior.c').split('bool8 MetatileBehavior_IsSurfableFishableWater(',1)[1].split('\n}',1)[0]
    fishable={names[name]for name in re.findall(r'\bMB_\w+\b',predicate)}
    demand('src/item_use.c',['if (IsPlayerFacingSurfableFishableWater())','StartFishing(GetItemSecondaryId(gSpecialVar_ItemId));'])
    demand('src/field_player_avatar.c',['GetCollisionAtCoords(playerObjEvent, x, y, playerObjEvent->facingDirection) == COLLISION_ELEVATION_MISMATCH',
        'PlayerGetElevation() == ELEVATION_DEFAULT'])
    # Shore path was independently checked as ordinary dry walking; neither
    # Surf nor a Bike is inferred from the map or the adjacent water.
    if not geometry.floor((name,*point)) or geometry.word((name,*point))>>12!=3 or word is None or word&0xc00 or word>>12 in {0,3} or geometry.behavior(water)not in fishable:
        raise ValueError('Old Rod capture is not a dry native-fishable shore')
    rod=read('src/data/items.h').split('[ITEM_OLD_ROD] =',1)[1].split('\n    [ITEM_',1)[0]
    if not re.search(r'\.secondaryId\s*=\s*OLD_ROD',rod):raise ValueError('Old Rod native item selects a different fishing method')
    demand('src/wild_encounter.c',['[OLD_ROD] = 0','[OLD_ROD] = 2'])
    table=tables.get(geometry.maps[name]['id'])
    slots=table.get('mons',[])[:2]if table else []
    row=next((row for row in slots if row['species']==capture['species']),None)
    if row is None:raise ValueError('Species absent from this exact map Old Rod slots')
    weights=table.get('encounter_rates',[])[:2]
    if len(weights)!=2 or any(type(value)is not int or value<=0 for value in weights)or sum(weights)!=100:
        raise ValueError('Old Rod slot weights are not explicitly source-resolved')
    return row,{'rod':'ITEM_OLD_ROD','facing':facing,'water_xy':list(water[1:]),
        'old_rod_slots':copy.deepcopy(slots),'old_rod_weights':weights,
        'wild_level_range':[row['min_level'],row['max_level']],
        'native_receipt_refs':[FISHING_NATIVE_RECEIPTS['mom'],FISHING_NATIVE_RECEIPTS[name]],
        'native_receipt_scope':'Pinned native handoff/exact-shore cast; other source reached shores and this requested capture remain source-positive possibilities.'}


def apply_campaign_preparation(opening,preparation,access,geometry,r):
    """One concrete ownership state; encounters and later rewards aren't unioned."""
    if set(preparation)-{'captures','evolutions','selected','mom_old_rod','pokerus','trades'}:raise ValueError('Unknown campaign preparation argument')
    roster=copy.deepcopy(opening['candidate_roster']);state=copy.deepcopy(opening['acquisition_states'][0])
    economy=copy.deepcopy(opening['economy']);defaults=state['pokemon_defaults'];proofs=[]
    owned=state.get('owned_monsters');by_id=state.get('pokemon_defaults_by_id');new_ids=[]
    if 'mom_old_rod' in preparation and type(preparation['mom_old_rod'])is not bool:raise ValueError('Mom handoff must be an explicit Boolean transition')
    if preparation.get('mom_old_rod'):
        proofs.append(acquire_mom_old_rod(state,access,geometry,opening['opening_parameters']['player_gender']))
    metadata=_species_acquisition_metadata(preprocess_species_info())
    wild=json.loads(read('src/data/wild_encounters.json'))
    tables={row.get('map'):row.get('land_mons',{}).get('mons',[])
            for group in wild['wild_encounter_groups'] for row in group.get('encounters',[])}
    fishing_tables={row.get('map'):row.get('fishing_mons')
            for group in wild['wild_encounter_groups'] for row in group.get('encounters',[])}
    # Derive land encounter behaviors rather than treating every reachable
    # map cell as grass. Water and unknown mobility never grant a capture.
    _,land=terrain_behaviors()
    demand('src/wild_encounter.c',['level = min(ChooseWildMonLevel(wildMonInfo->wildPokemon, wildMonIndex, area), GetCurrentLevelCap());'])
    demand('include/config/battle.h',['#define B_EC_WILD_HELD_ITEMS        TRUE'])
    demand('src/pokemon.c',['void SetWildMonHeldItem(void)','gSpeciesInfo[species].itemCommon','gSpeciesInfo[species].itemRare'])
    captures=preparation.get('captures',[])
    if not isinstance(captures,list):raise ValueError('Stage captures must be a concrete list')
    charged=len(captures)*(r['ball_price']+r['ev_fee'])
    if charged>economy['cash_remaining_minimum']:raise ValueError('Stage captures and EV fees exceed retained cash')
    for index,capture in enumerate(captures):
        if set(capture)-{'id','species','map','xy','nature','held_item','evolved_to','method','facing'}:raise ValueError('Unknown stage capture argument')
        species=capture['species'];name=capture['map'];point=tuple(capture['xy'])
        if len(point)!=2 or (name,*point) not in geometry.reached or name not in access['maps']:
            raise ValueError('Capture requires an audited reached land tile')
        method=capture.get('method','land');fishing_proof={}
        if method=='old_rod':
            row,fishing_proof=old_rod_capture_row(capture,state,geometry,fishing_tables)
        elif method=='land':
            if 'facing'in capture:raise ValueError('Land captures do not accept fishing facing')
            if geometry.behavior((name,*point)) not in land:raise ValueError('Capture tile has no land encounters')
            row=next((m for m in tables.get(geometry.maps[name]['id'],[])if m['species']==species),None)
            if row is None:raise ValueError('Species absent from this exact map land table')
        else:raise ValueError('Unsupported capture method')
        sid=resolve_species(species,r['aliases']);meta=metadata[sid]
        if meta['restricted']:raise ValueError('Legend capture gate, authored set and caught state remain unresolved')
        if owned is None and species in defaults:raise ValueError('Stage extension requires distinct ownership species; use opening owned-slot provider for duplicates')
        ident=capture.get('id','stage-capture-'+str(index))
        if owned is not None and (not isinstance(ident,str) or not re.fullmatch(r'[a-zA-Z0-9_-]+',ident) or ident in owned):
            raise ValueError('Missing or duplicate stage ownership ID')
        nature=capture.get('nature','NATURE_HARDY')
        if not re.fullmatch(r'NATURE_\w+',nature)or not 0<=number('include/constants/pokemon.h',nature)<number('include/constants/pokemon.h','NUM_NATURES'):
            raise ValueError('Invalid natural capture nature')
        item=capture.get('held_item','ITEM_NONE')
        if meta['itemCommon']==meta['itemRare']!='ITEM_NONE':
            if item not in {'ITEM_NONE',meta['itemCommon']}:raise ValueError('Wild held item not configured')
            item=meta['itemCommon']
        elif item not in {meta['itemCommon'],meta['itemRare'],'ITEM_NONE'}:
            raise ValueError('Wild held item not configured')
        default={'nature':nature,'ability':r['initial_abilities'][sid],
                 'evs':list(r['evs']),'ivs':[r['iv']]*6,'friendship':r['friendship'][sid]}
        if owned is None:defaults[species]=default
        else:
            owned[ident]={'species':species,'acquired_species':species,'method':method+'_capture',
                          'maps':[geometry.maps[name]['id']],'xy':list(point)}
            by_id[ident]={**default,'pokerus':0,'pp_bonuses':0};new_ids.append(ident)
        roster.append(species);state['resources'][species]=state['resources'].get(species,0)+1
        if item!='ITEM_NONE':state['resources'][item]=state['resources'].get(item,0)+1
        proof={'method':method+'_capture','base_species':species,'map':name,'xy':list(point),
               'captured_level':min(row['min_level'],access['cap']),'natural_nature':nature,
               'held_item_acquired':item,'condition':'One successful purchased-ball capture; positive-probability outcome, not guaranteed.',**fishing_proof}
        if owned is not None:proof['owned_mon_id']=ident
        if 'evolved_to' in capture:
            proof['evolution']=campaign_evolution(species,capture['evolved_to'],nature,r,geometry,access)
            _replace_owned_species(species,capture['evolved_to'],proof['evolution'],roster,state,r,ident if owned is not None else None)
        proofs.append(proof)
    trade_ids,trade_proofs=apply_fidough_trades(preparation.get('trades',[]),state,access,geometry,r)
    trade_fee=len(trade_ids)*r['ev_fee'];charged+=trade_fee
    if charged>economy['cash_remaining_minimum']:raise ValueError('NPC recipient EV preparation exceeds retained cash')
    new_ids=[ident for ident in new_ids if ident in (owned or {})]+trade_ids
    proofs+=trade_proofs
    evolutions=preparation.get('evolutions',{})
    if not isinstance(evolutions,dict):raise ValueError('Retained evolutions must map owned base species to targets')
    for key,target in evolutions.items():
        ident=key if owned is not None else None
        if owned is None:
            if key not in defaults:raise ValueError('Retained evolution has no owned base')
            base=key;default=defaults[key]
        else:
            if ident not in owned:raise ValueError('Retained evolution requires an explicit owned ID')
            base=owned[ident]['species'];default=by_id[ident]
        proof=campaign_evolution(base,target,default['nature'],r,geometry,access)
        if ident is not None:proof['owned_mon_id']=ident
        _replace_owned_species(base,target,proof,roster,state,r,ident);proofs.append({'method':'retained_evolution','evolution':proof})
    if 'pokerus' in preparation:
        if owned is None:raise ValueError('Campaign Pokerus preparation requires explicit owned IDs')
        farm=early_farm_rules() # Reuse the existing positive-probability native source contract.
        apply_owned_pokerus_plan(owned,by_id,preparation['pokerus'])
        plan=preparation['pokerus'];direct=plan.get('direct',[]);transmissions=plan.get('transmissions',[])
        state['pokerus_values']=[0,0xFC,0xFD,0xFE]
        state['source_evidence']+=farm['evidence']
        proofs.append({'method':'owned_pokerus_plan','direct':copy.deepcopy(direct),
            'transmissions':copy.deepcopy(transmissions),'minimum_favorable_battles':len(direct)+len(transmissions),
            'conditions':'Positive-probability ordinary wild retries with free healing. Isolate each direct target; withdraw the next uninfected member before depositing the current FE donor, never deposit the last usable mon. No battle with both present unless transmitting deliberately. Arrange donor/recipient adjacent for one successful spread per later battle.',
            'native_receipt_refs':['test/pokerus_chains.c: boxed donors allow six isolated direct infections before Pokedex',
                'work/battle-calibration/native-oldale-pc-receipt-01/evidence.json'],
            'scope':'Finite source-possible favorable rolls after owned capture/evolution/trade; no guaranteed infection, extra item, cash, EV bonus, PP upgrade or early hot-spring treatment.'})
    if owned is not None:
        selected=preparation.get('selected',state['selected_owned_ids']+new_ids)
        if not isinstance(selected,list)or len(selected)<2 or len(set(selected))!=len(selected)or not set(selected)<=set(owned):
            raise ValueError('Funded stage pool requires distinct acquired ownership IDs')
        # Opening farm mode funds only its prepared selection. Boxing keeps
        # ownership, but adding a boxed mon to the editable pool needs its fee.
        additional_fee=len(set(selected)-set(state['selected_owned_ids'])-set(new_ids))*r['ev_fee']
        charged+=additional_fee
        if charged>economy['cash_remaining_minimum']:raise ValueError('Added boxed-mon EV fees exceed retained cash')
        state['selected_owned_ids']=selected;roster=[owned[ident]['species']for ident in selected]
    elif 'selected' in preparation:raise ValueError('Stage party selection needs an owned-slot opening preparation')
    else:additional_fee=0
    economy['purchased_poke_balls']+=len(captures);economy['capture_cost']+=len(captures)*r['ball_price']
    economy['ev_fee_reserve']+=len(captures)*r['ev_fee']+additional_fee+trade_fee;economy['cash_remaining_minimum']-=charged
    if trade_ids:economy['npc_trade_ev_preparation_cost']=trade_fee
    state['campaign_acquisition_proofs']=proofs
    state['source_evidence'] += [source_evidence(ROOT/p)for p in ('src/data/wild_encounters.json','src/pokemon.c','src/wild_encounter.c',
       'src/party_menu.c','src/evolution_scene.c','src/metatile_behavior.c','include/config/battle.h')]
    state['source_evidence'] += [source_evidence(p)for p in sorted((ROOT/'src/data/pokemon/species_info').glob('*.h'))]
    if preparation.get('mom_old_rod') or any(c.get('method')=='old_rod'for c in captures):
        state['source_evidence'] += [source_evidence(ROOT/p)for p in FISHING_SOURCE_PATHS]
    return roster,state,economy


def _replace_owned_species(base,target,proof,roster,state,r,owned_id=None):
    defaults=state['pokemon_defaults']
    if owned_id is None and target in defaults:raise ValueError('Evolution targets collide; explicit slot certificate required')
    inherited=defaults.pop(base)if owned_id is None else state['pokemon_defaults_by_id'][owned_id]
    inherited['ability']=r['initial_abilities'][resolve_species(target,r['aliases'])]
    if 'friendship' in proof:inherited['friendship']=proof['friendship']
    if owned_id is None:defaults[target]=inherited;roster[roster.index(base)]=target
    else:state['owned_monsters'][owned_id]['species']=target
    state['resources'][base]-=1
    if not state['resources'][base]:del state['resources'][base]
    state['resources'][target]=state['resources'].get(target,0)+1


@lru_cache(maxsize=64)
def _level_rows(source,difficulty,cap,offsets,reduction):
    functions='\n'.join(c_function(source,name) for name in ('GetTrainerLevelLeadPercentFor','GetTrainerLevelCapDropPercentFor','GetCampaignTrainerLevelFor','GetCampaignTrainerLevel'))
    program='''#include <stdio.h>
#include <stdint.h>
typedef uint8_t u8;typedef int16_t s16;typedef int32_t s32;typedef uint32_t u32;
enum DifficultyLevel {DIFFICULTY_EASY,DIFFICULTY_NORMAL,DIFFICULTY_HARD};
#define MAX_LEVEL 100
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
'''+f'enum DifficultyLevel GetCurrentDifficultyLevel(void){{return {difficulty};}}\nu32 GetCurrentLevelCap(void){{return {cap};}}\n'+functions+'\nint main(void){\n'
    for offset in offsets:
        program+=f'printf("%u\\n",max(1,GetCampaignTrainerLevel({offset})-{reduction}));\n'
    return tuple(map(int,run_c_probe(program+'return 0;}').split()))


def expected_opponent(scenario):
    dossiers={d['trainer_id']:d for d in opponent_catalogue()}
    dossier=copy.deepcopy(dossiers.get(scenario['trainer_id']))
    if dossier is None:return None
    if dossier['class']=='rival':
        # Different evolution stages use different native regional presets.
        # Opening-only substitutions cannot stand in for later rival factories.
        return None
    header=read('src/data/trainers.h');trainer=scenario['trainer_id']
    for mode in ['EASY','HARD']:
        if '[DIFFICULTY_'+mode+']['+trainer+']' in header:
            raise ValueError('Difficulty-specific native trainer party is outside shared-roster certificate')
    marker='[DIFFICULTY_NORMAL]['+trainer+']'
    if marker not in header:raise ValueError('Native trainer initializer is missing')
    block=header.split(marker,1)[1].split('[DIFFICULTY_NORMAL]',1)[0]
    reduction=int(re.search(r'\.easyLevelReduction\s*=\s*([01])',block)[1])
    source=read('src/difficulty.c')
    demand('src/trainer_util.c',['if (trainer->easyLevelReduction)','battleLevel = max(1, battleLevel - 1);'])
    difficulty={'easy':0,'medium':1,'hard':2}[scenario['difficulty']]
    team=dossier['team'];levels=_level_rows(source,difficulty,scenario['level_cap'],tuple(m['level_offset'] for m in team),reduction)
    for member,level in zip(team,levels):member['level']=level
    combat={k:dossier[k]for k in ('class','ai_profile','ai_extra','strategy','tactics','field','mega_slots')}
    combat.update(difficulty=scenario['difficulty'],team=team)
    return {'trainer_id':trainer,'team':team,'regional_replacement':{'applied':False,'reason':'Native target is not a regional rival'},
            'combat_sha256':digest_bytes(canonical(combat)),
            'source_evidence':[source_evidence(ROOT/p)for p in ('src/data/trainers.h','src/data/trainers.party','src/trainer_util.c',
                                                            'src/difficulty.c','src/battle_setup.c','data/emerald_champions/emerald_champions_battle_teams.txt')]}


def build_campaign_scenario(trainer_id,difficulty,*,stage,opening_parameters=None,player_xy=None,campaign_preparation=None):
    return _campaign_scenario(trainer_id,difficulty,stage=stage,opening_parameters=opening_parameters,player_xy=player_xy,
                              campaign_preparation=campaign_preparation,_fingerprint=source_fingerprint())


def _campaign_scenario(trainer_id,difficulty,*,stage,opening_parameters=None,player_xy=None,campaign_preparation=None,_fingerprint=None,_context=None):
    args=dict(opening_parameters or {})
    if set(args)-{'generation','first','second','captures','natures','player_gender','evolutions','preparation'}:
        raise ValueError('Unsupported retained opening acquisition arguments')
    opening=_opening_scenario(difficulty,**args,_fingerprint=_fingerprint,_context=_context)
    access=profile(stage,opening)
    if (trainer_id=='TRAINER_WATTSON_1'and 'TRAINER_WALLY_MAUVILLE'in access['preceding_battle_ids']
            and not access['flags'].get('FLAG_DEFEATED_MAUVILLE_GYM')):
        demand('data/maps/MauvilleCity_Gym/scripts.inc',['trainerbattle_double TRAINER_BEN','setvar VAR_MAUVILLE_GYM_STATE, 1'])
        access['preceding_battle_ids'].append('TRAINER_BEN')
        access['vars']['VAR_MAUVILLE_GYM_STATE']=1
        access['flags']['FLAG_MAUVILLE_GYM_BARRIERS_STATE']=True
    if re.match(r'TRAINER_(MAY|BRENDAN)_ROUTE_110_',trainer_id):
        for predecessor in ['TRAINER_ISABEL_1','TRAINER_KALEB']:
            if predecessor not in access['preceding_battle_ids']:access['preceding_battle_ids'].append(predecessor)
    fishing=campaign_preparation is not None and (campaign_preparation.get('mom_old_rod') or any(c.get('method')=='old_rod'for c in campaign_preparation.get('captures',[])))
    if fishing:access['maps']=sorted(set(access['maps'])|set(FISHING_EXTRA_MAPS))
    extra_maps=FISHING_EXTRA_MAPS if fishing else ()
    if campaign_preparation is not None and campaign_preparation.get('trades'):
        extra_maps+=('RustboroCity_House1',)
        access['maps']=sorted(set(access['maps'])|{'RustboroCity_House1'})
    geometry=EarlyGeometry(stage,access,extra_maps=extra_maps)
    name,label=find_trainer_script(trainer_id)
    unresolved=[];field=None;target_source=[]
    if access.get('physical_access_status')=='unresolved':unresolved.append('Physical access beyond the validated ferry corridor remains unresolved')
    if access.get('source_blocker'):unresolved.append(access['source_blocker'])
    if trainer_id in access['preceding_battle_ids']:unresolved.append('Target is a required preceding battle; its own win cannot unlock itself')
    if name not in access['maps'] or name not in geometry.maps:
        unresolved.append('Target map is outside this audited access profile')
    else:
        code,labels=parse_script_file(ROOT/'data/maps'/name/'scripts.inc')
        actor=next((o for o in geometry.maps[name]['object_events'] if o['script']==label),None)
        # The common extractor supplies the script's owning event, not trainer
        # chronology. Two coord-trigger entries own the Woods research battle.
        if trainer_id in {'TRAINER_GRUNT_MUSEUM_1','TRAINER_ARCHIE_SLATEPORT'}:
            # Both opponents spawn in Stern's script rather than owning an
            # interactable trainer event. This canonical west-side approach
            # was walked natively; the actual opponent ends at (11,6).
            demand('data/maps/SlateportCity_OceanicMuseum_2F/scripts.inc',[
                'addobject LOCALID_GRUNT_2','addobject LOCALID_ARCHIE',
                'trainerbattle_no_intro_double TRAINER_GRUNT_MUSEUM_1',
                'trainerbattle_no_intro_double TRAINER_ARCHIE_SLATEPORT'])
            choices=[(12,6)]
            if not access['flags'].get('FLAG_BADGE02_GET'):
                unresolved.append('Brawly victory must clear the native Museum entrance queue')
            if access['flags'].get('FLAG_DELIVERED_DEVON_GOODS'):
                unresolved.append('Museum scene is already completed')
            if trainer_id=='TRAINER_ARCHIE_SLATEPORT'and 'TRAINER_GRUNT_MUSEUM_1'not in access['preceding_battle_ids']:
                unresolved.append('Archie requires the first Museum Grunt victory and scripted party heal')
            captain=next(o for o in geometry.maps[name]['object_events']if o['script']=='SlateportCity_OceanicMuseum_2F_EventScript_CaptStern')
            if captain.get('flag')in set(re.findall(r'\bsetflag\s+(FLAG_\w+)',read('data/scripts/new_game.inc'))):
                raise ValueError('Initial Stern visibility changed')
        elif re.match(r'TRAINER_(MAY|BRENDAN)_ROUTE_110_',trainer_id):
            script=read('data/maps/Route110/scripts.inc')
            branch=(3-opening['opening_parameters']['first']-opening['opening_parameters']['second']+2)%3
            gender='May'if opening['opening_parameters']['player_gender']=='male'else'Brendan'
            label=re.search(r'case '+str(branch)+r', (Route110_EventScript_'+gender+r'Battle\w+)',script)[1]
            expected=re.search(re.escape(label)+r'::[^\n]*\n\s*trainerbattle_no_intro_double\s+(TRAINER_\w+)',script)[1]
            if trainer_id!=expected:unresolved.append('Target is not the selected native rival branch')
            if access['vars'].get('VAR_ROUTE110_STATE',0)!=0:unresolved.append('Route110 rival already completed')
            choices=[(33,56)] # Native player approach; rival moves to (33,55).
            if ('Route110',33,57)in geometry.reached:geometry.reached.add(('Route110',33,56))
        elif trainer_id=='TRAINER_GRUNT_PETALBURG_WOODS':
            triggers=[e for e in geometry.maps[name]['coord_events'] if e.get('var')=='VAR_PETALBURG_WOODS_STATE' and e.get('var_value')=='0']
            choices=[(e['x'],e['y']) for e in triggers]
            if access['vars']['VAR_PETALBURG_WOODS_STATE']!=0:unresolved.append('Woods scene already completed')
            for point in choices:
                # Triggers are stopping points; a reachable adjacent floor can
                # enter them to begin the authored scene.
                if any((name,point[0]+dx,point[1]+dy)in geometry.reached for dx,dy in [(-1,0),(1,0),(0,-1),(0,1)]):
                    geometry.reached.add((name,*point))
        else:
            position=trainer_position(geometry.maps[name],code,labels,label)
            actor=next((o for o in geometry.maps[name]['object_events'] if position==(o['x'],o['y'])),None)
            choices=[(position[0]+dx,position[1]+dy)for dx,dy in [(0,1),(1,0),(-1,0),(0,-1)]] if position else []
            if trainer_id=='TRAINER_WALLY_MAUVILLE':
                choices=[(8,7)]
                if access['flags'].get('FLAG_DEFEATED_WALLY_MAUVILLE'):unresolved.append('Wally already completed')
                if not access['vars'].get('VAR_ROUTE110_STATE'):unresolved.append('The native Route110 rival corridor is not completed')
            elif trainer_id=='TRAINER_WATTSON_1':
                choices=[(5,3)]
                if 'TRAINER_BEN'not in access['preceding_battle_ids']:unresolved.append('Validated Wattson corridor conservatively requires prior Ben')
            elif trainer_id=='TRAINER_GRUNT_RUSTURF_TUNNEL' and stage=='after_roxanne':
                demand('data/maps/RusturfTunnel/scripts.inc',['setobjectxyperm LOCALID_GRUNT, 13, 5'])
                choices=[(13,6),(12,5),(14,5)]
            elif actor is None or actor.get('flag','0')!='0':unresolved.append('Trainer event visibility is not audited')
            if trainer_id.startswith(('TRAINER_MAY_RUSTBORO_','TRAINER_BRENDAN_RUSTBORO_')):
                unresolved.append('Route104 rival requires post-Devon prerequisites outside these profiles')
        possible=[point for point in choices if (name,*point)in geometry.reached]
        chosen=tuple(player_xy)if player_xy is not None else possible[0]if possible else None
        if chosen is None or chosen not in possible:unresolved.append('No audited reachable interaction/trigger tile')
        else:field=exact_field(name,chosen,geometry)
        target_source=[source_evidence(ROOT/'data/maps'/name/'scripts.inc'),source_evidence(ROOT/'data/maps'/name/'map.json')]
    evidence=[source_evidence(ROOT/p)for p in SOURCE_PATHS]+geometry.evidence+access.get('source_evidence',[])
    state=copy.deepcopy(opening['acquisition_states'][0]);state['source_evidence']+=evidence
    arguments=dict(trainer_id=trainer_id,difficulty=difficulty,stage=stage,opening_parameters=args)
    if player_xy is not None:arguments['player_xy']=list(player_xy)
    roster=copy.deepcopy(opening['candidate_roster']);economy=copy.deepcopy(opening['economy'])
    for token,count in access.get('story_items',{}).items():
        state['resources'][token]=state['resources'].get(token,0)+count
    fee=access.get('minimum_museum_fee',0)
    if trainer_id in {'TRAINER_GRUNT_MUSEUM_1','TRAINER_ARCHIE_SLATEPORT'}:
        fee=max(50,fee) # This finite subset chooses the ordinary paid entry.
    if fee>economy['cash_remaining_minimum']:raise ValueError('Retained subset cannot fund the modeled Museum entry')
    if fee:
        economy['museum_entry_cost']=fee;economy['cash_remaining_minimum']-=fee
    if campaign_preparation is not None:
        arguments['campaign_preparation']=copy.deepcopy(campaign_preparation)
        prepared=copy.deepcopy(opening);prepared['acquisition_states'][0]=state;prepared['economy']=economy
        r=_context['rules']if _context is not None else rules()
        preparation=copy.deepcopy(campaign_preparation)
        bicycle=preparation.pop('rydel_bicycle',False)
        if not isinstance(bicycle,bool):raise ValueError('Rydel Bicycle transition must be boolean')
        if bicycle:
            if not access['vars'].get('VAR_ROUTE110_STATE')or ('MauvilleCity',35,6)not in geometry.reached:
                raise ValueError('Rydel Bicycle requires the validated post-rival shop approach')
            demand('data/maps/MauvilleCity_BikeShop/scripts.inc',['giveitem ITEM_ACRO_BIKE','setflag FLAG_RECEIVED_BIKE'])
            access['flags']['FLAG_RECEIVED_BIKE']=True
            state['resources']['ITEM_ACRO_BIKE']=1
        prepared['acquisition_states'][0]=state
        roster,state,economy=apply_campaign_preparation(prepared,preparation,access,geometry,r)
    scenario={'scenario_id':'retained-'+stage+'-'+opening['scenario_id'],'trainer_id':trainer_id,'difficulty':difficulty,
              'source_fingerprint':_fingerprint,'legality_status':access.get('source_legality_status','proven'),'maximal_arsenal_status':'unresolved',
              'battle_access_status':'unresolved'if unresolved else 'proven','access_unresolved':unresolved,
              'producer':{'kind':'campaign','arguments':arguments},'level_cap':access['cap'],
              'progression_flags':access['flags'],'progression_vars':access['vars'],
              'preceding_battle_ids':access['preceding_battle_ids'],'overworld_dependencies':access['overworld_dependencies'],
              'acquisition_states':[state],'candidate_roster':roster,'economy':economy,
              'retained_acquisition':{'producer':opening['producer'],'source_evidence':opening['acquisition_states'][0]['source_evidence'],
                                      'scope':'Concrete opening ownership retained; only explicit preceding source transitions grant later story items, never target rewards.'},
              'target_source_evidence':target_source,'scope':'Synthetic source-audited access state and retained legal subset; no earned-prefix or maximal arsenal claim.'}
    if field:scenario['battle_field']=field
    if campaign_preparation is not None:
        scenario['preparation_native_status']='pending_capture_and_evolution_receipts'
        scenario['scenario_id']+='-stage-'+digest_bytes(canonical(campaign_preparation))[:12]
    identity=expected_opponent(scenario)
    scenario['expected_opponent_status']='proven' if identity else 'unresolved'
    if identity:scenario['expected_opponent']=identity
    return scenario


def certify_scenario(scenario,*,internal_fingerprint=None,internal_context=None):
    producer=scenario.get('producer',{})
    if producer.get('kind')!='campaign':raise ValueError('Unknown campaign producer')
    arguments=producer['arguments']
    if set(arguments)-{'trainer_id','difficulty','stage','opening_parameters','player_xy','campaign_preparation'}:
        raise ValueError('Unsupported campaign producer arguments')
    expected=_campaign_scenario(**arguments,_fingerprint=internal_fingerprint,_context=internal_context)
    for key,value in expected.items():
        if key!='source_fingerprint' and scenario.get(key)!=value:
            raise ValueError('Campaign certificate differs from source factory: '+key)


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--trainer',required=True);p.add_argument('--difficulty',default='hard',choices=['easy','medium','hard'])
    p.add_argument('--stage',required=True,choices=PROFILE_NAMES);p.add_argument('--opening-parameters',type=Path)
    p.add_argument('--campaign-preparation',type=Path);p.add_argument('--out',required=True,type=Path)
    a=p.parse_args();parameters=json.loads(a.opening_parameters.read_text())if a.opening_parameters else {}
    preparation=json.loads(a.campaign_preparation.read_text())if a.campaign_preparation else None
    scenario=build_campaign_scenario(a.trainer,a.difficulty,stage=a.stage,opening_parameters=parameters,campaign_preparation=preparation)
    a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(scenario,indent=2,sort_keys=True)+'\n')
    print(scenario['trainer_id'],scenario['battle_access_status'],scenario['access_unresolved'])
if __name__=='__main__':main()
