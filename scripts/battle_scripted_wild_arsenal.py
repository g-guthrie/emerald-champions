#!/usr/bin/env python3
"""Source certificates for the native, unprepared Birch starter-pair rescue.

Deoxys remains unsupported until a finale acquisition producer exists. Random
factory fields have explicit source domains; callers cannot supply wildcards.
"""
import argparse
import copy
from functools import lru_cache
import hashlib
import json
import math
from pathlib import Path
import re
import shutil
import subprocess
import struct
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools/agent_player'))
import generate_battle_suite as suite
from verify_trainer_ability_legality import species_aliases, resolve_species

SOURCE_PATHS = ('src/pokemon.c', 'src/emerald_champions_story.c', 'src/starter_choose.c',
                'src/battle_setup.c', 'src/battle_controllers.c', 'src/battle_main.c',
                'src/caps.c', 'data/maps/Route101/scripts.inc', 'data/maps/Route101/map.json',
                'data/layouts/layouts.json', 'data/layouts/Route101/map.bin',
                'data/tilesets/primary/general/metatile_attributes.bin',
                'include/global.fieldmap.h', 'include/fieldmap.h',
                'include/constants/metatile_behaviors.h', 'src/metatile_behavior.c')

def demand(path, fragments):
    text = (ROOT / path).read_text()
    for fragment in fragments:
        if fragment not in text:
            raise ValueError('Rescue source contract changed: ' + path + ': ' + fragment)

@lru_cache(maxsize=1)
def rules(fingerprint):
    # This is the same configured source as the game, including its selected
    # generation of learnsets, species defaults and Inclement normal slots.
    demand('src/emerald_champions_story.c', (
        'CreateRandomMonWithIVs(&pair[0], GetStarterPokemon(first), 5, MAX_PER_STAT_IVS)',
        'CreateRandomMonWithIVs(&pair[1], GetStarterPokemon(second), 5, MAX_PER_STAT_IVS)',
        'GiveScriptedMonToPlayer(&pair[0], 0)', 'GiveScriptedMonToPlayer(&pair[1], 1)',
        'SPECIES_POOCHYENA, SPECIES_ZIGZAGOON',
        'CreateRandomMonWithIVs(&gParties[B_TRAINER_OPPONENT_A][i], sSpecies[i], 2, MAX_PER_STAT_IVS)'))
    demand('src/pokemon.c', ('SetPlayerMonBaselineEVs(mon);', 'MaxPlayerMonIVs(mon);',
        'CreateMonWithIVs(mon, species, level, Random32()', 'GiveMonInitialMoveset(mon);',
        'value = RollNormalAbilitySlot(species, boxMon->personality);',
        'if (learnset[i].level == 0 || IsMoveRemovedFromGame(learnset[i].move))', 'moves[j] = moves[j + 1];',
        'slots[count++] = 0;', 'slots[count++] = 1;',
        'for (u32 slot = ABILITY_SLOT_INCLEMENT; slot < NUM_OWNER_ABILITY_SLOTS; slot++)',
        'return slots[personality % count];'))
    demand('src/caps.c', ('return 14;',))
    demand('src/battle_setup.c', ('gBattleTypeFlags = BATTLE_TYPE_FIRST_BATTLE | BATTLE_TYPE_DOUBLE;',))
    demand('src/battle_controllers.c', ('if (IsEmeraldChampionsBirchRescueBattle())\n        CreateEmeraldChampionsBirchRescueParty();',))
    demand('src/battle_main.c', ('SetWildMonHeldItem();',))
    demand('data/maps/Route101/scripts.inc', ('special ChooseStarter', 'special StartEmeraldChampionsBirchRescue',
        'goto_if_ne VAR_RESULT, B_OUTCOME_WON, Route101_EventScript_RescueLost'))
    cc = shutil.which('cc')
    if not cc: raise ValueError('Host C preprocessor required for factory certificate')
    native = subprocess.run([cc, '-E', '-P', '-Iinclude', '-Isrc', '-I.', 'src/pokemon.c'],
                            cwd=ROOT, text=True, capture_output=True, check=True).stdout
    start = native.index('const struct SpeciesInfo gSpeciesInfo[] =')
    marks = list(re.finditer(r'\[(SPECIES_\w+)\]\s*=\s*\{', native[start:]))
    species = {}
    aliases = species_aliases()
    for i, mark in enumerate(marks):
        end = marks[i + 1].start() if i + 1 < len(marks) else len(native) - start
        block = native[start + mark.end():start + end]
        species[resolve_species(mark[1], aliases)] = block
    raw = (ROOT / 'src/starter_choose.c').read_text().split('sStarterMons[][STARTER_MON_COUNT] =')[1].split('};')[0]
    starters = {int(g): [resolve_species(s, aliases) for s in re.findall(r'SPECIES_\w+', row)]
                for g, row in re.findall(r'\[(\d+)\]\s*=\s*\{([^}]+)', raw)}
    layer = native.split('sInclementLayer[NUM_SPECIES] =', 1)[1].split('\n};', 1)[0]
    added = {resolve_species(s, aliases): re.findall(r'ABILITY_\w+', body)
             for s, body in re.findall(r'\[(SPECIES_\w+)\]\s*=\s*\{([^\n]+)', layer)}
    ev_block = native.split('sPlayerBaselineEVs[NUM_STATS] =', 1)[1].split('};', 1)[0]
    stats = ('HP', 'ATK', 'DEF', 'SPATK', 'SPDEF', 'SPEED')
    evs = [int(re.search(r'\[STAT_' + stat + r'\]\s*=\s*(\d+)', ev_block)[1]) for stat in stats]
    # The Birch rescue is always before the Knuckle Badge: no Pokemon has EVs yet.
    if 'AreEVsUnlocked() ? sPlayerBaselineEVs : NULL' not in (ROOT / 'src/pokemon.c').read_text() or 'return FlagGet(FLAG_BADGE02_GET);' not in (ROOT / 'src/caps.c').read_text():
        raise ValueError('EV unlock gate unresolved')
    evs = [0] * 6
    natures = re.findall(r'\bNATURE_[A-Z]+\b', (ROOT / 'include/constants/pokemon.h').read_text().split('NATURE_HARDY')[1].split('NATURE_RANDOM')[0])
    natures = ['NATURE_HARDY'] + list(dict.fromkeys(natures))
    if len(natures) != 25: raise ValueError('Native nature domain unresolved')
    learnsets = {name: re.findall(r'\.move\s*=\s*(MOVE_\w+),\s*\.level\s*=\s*(\d+)', body)
                for name, body in re.findall(r'static const struct LevelUpMove (\w+)\[\]\s*=\s*\{(.*?)\};', native, re.S)}
    # Moves the owner removed from the game never enter a starting moveset.
    removed_body = (ROOT / 'src/pokemon.c').read_text().split('bool32 IsMoveRemovedFromGame(enum Move move)', 1)[1].split('\n}', 1)[0]
    removed = set(re.findall(r'case (MOVE_\w+):', removed_body))
    if not removed: raise ValueError('Removed-move list unresolved')
    return dict(learnsets=learnsets, species=species, starters=starters, added=added, evs=evs,
                natures=natures, aliases=aliases, removed=removed)

def factory_member(r, species, slot, level, player=False):
    block = r['species'][species]
    natural = re.search(r'\.levelUpLearnset\s*=\s*(\w+)', block)[1]
    moves = []
    for move, learned in r['learnsets'][natural]:
        if int(learned) > level: break
        if int(learned) and move not in r['removed'] and move not in moves: moves = (moves + [move])[-4:]
    abilities = re.findall(r'ABILITY_\w+', re.search(r'\.abilities\s*=\s*\{([^}]+)', block)[1])[:2]
    abilities += r['added'].get(species, [])
    # Hidden slot 2 is excluded by RollNormalAbilitySlot. All allowed normal
    # slot counts are coprime to 25, so every nature/slot pairing is possible.
    slots = [ability for ability in abilities if ability != 'ABILITY_NONE']
    if not slots or math.gcd(len(slots), 25) != 1:
        raise ValueError('Factory nature/normal-slot correlation needs a supported joint domain')
    abilities = sorted(set(slots))
    friendship = re.search(r'\.friendship\s*=\s*([^,]+)', block)[1].strip()
    if friendship.isdigit(): friendship = int(friendship)
    else:
        match = re.fullmatch(r'\(\(([\d +]+) >= ([\d +]+)\) \? (\d+) : (\d+)\)', friendship)
        if not match: raise ValueError('Configured factory friendship unresolved: ' + friendship)
        value = lambda expression: sum(int(n.strip()) for n in expression.split('+'))
        friendship = int(match[3] if value(match[1]) >= value(match[2]) else match[4])
    items = ['ITEM_NONE'] if player else sorted({'ITEM_NONE', *re.findall(r'\.item(?:Common|Rare)\s*=\s*(ITEM_\w+)', block)})
    fixed = dict(slot=slot, species=species, level=level, friendship=friendship,
                 evs=r['evs'] if player else [0]*6, ivs=[31]*6,
                 moves=moves+['MOVE_NONE']*(4-len(moves)))
    if player: fixed.update(pokerus=0, pp_bonuses=0, status=0)
    # Exported certificates must not share mutable lists with cached source
    # rules: modifying a caller's domain cannot poison later reconstruction.
    return copy.deepcopy({'fixed': fixed, 'domains': {'nature': r['natures'], 'ability': abilities, 'item': items}})

def rescue_field():
    script=(ROOT/'data/maps/Route101/scripts.inc').read_text()
    opening=script.split('Route101_EventScript_BattleRescue::',1)[1].split('special StartEmeraldChampionsBirchRescue',1)[0]
    position=re.findall(r'setobjectxy LOCALID_PLAYER,\s*(\d+),\s*(\d+)',opening)
    if len(position)!=1 or re.search(r'(?:applymovement LOCALID_PLAYER|warp)',opening):
        raise ValueError('Rescue battle approach is unresolved')
    x,y=map(int,position[0])
    m=json.loads((ROOT/'data/maps/Route101/map.json').read_text())
    layout=next(row for row in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts'] if row['id']==m['layout'])
    if layout['primary_tileset']!='gTileset_General' or not 0<=x<layout['width'] or not 0<=y<layout['height']:
        raise ValueError('Rescue player tile is outside the audited primary tileset')
    constants=(ROOT/'include/global.fieldmap.h').read_text()
    mask=int(re.search(r'MAPGRID_METATILE_ID_MASK\s+(0x[0-9A-F]+)',constants)[1],16)
    behavior_mask=int(re.search(r'METATILE_ATTR_BEHAVIOR_MASK\s+(0x[0-9A-F]+)',constants)[1],16)
    word=struct.unpack_from('<H',(ROOT/layout['blockdata_filepath']).read_bytes(),2*(y*layout['width']+x))[0]
    tile=word&mask
    primary_count=int(re.search(r'#define NUM_METATILES_IN_PRIMARY\s+(\d+)',(ROOT/'include/fieldmap.h').read_text())[1])
    if tile>=primary_count:raise ValueError('Rescue player tile is not primary')
    attribute=struct.unpack_from('<H',(ROOT/'data/tilesets/primary/general/metatile_attributes.bin').read_bytes(),tile*2)[0]
    behavior=attribute&behavior_mask
    behaviors=list(dict.fromkeys(re.findall(r'MB_\w+',(ROOT/'include/constants/metatile_behaviors.h').read_text())))
    if behavior!=behaviors.index('MB_NORMAL') or m['map_type']!='MAP_TYPE_ROUTE' or m['weather']!='WEATHER_SUNNY':
        raise ValueError('Rescue normal route tile/weather changed; environment unresolved')
    demand('src/battle_setup.c',('PlayerGetDestCoords(&x, &y);','case MAP_TYPE_ROUTE:','return BATTLE_ENVIRONMENT_PLAIN;'))
    return dict(map='Route101',weather=m['weather'],environment='BATTLE_ENVIRONMENT_PLAIN',
                player_xy=[x,y],metatile_id=tile,metatile_behavior='MB_NORMAL')

def rescue_scenario(difficulty='hard', *, generation=3, first=0, second=1,
                    player_gender='male', internal_fingerprint=None):
    fingerprint = internal_fingerprint or suite.source_fingerprint()
    r = rules(fingerprint)
    if difficulty not in {'easy','medium','hard'} or type(generation) is not int or not 1<=generation<=9:
        raise ValueError('Invalid rescue difficulty/generation')
    if any(type(i) is not int or not 0<=i<3 for i in (first,second)) or first==second or player_gender not in {'male','female'}:
        raise ValueError('Invalid ordered rescue starter pair/gender')
    args=dict(difficulty=difficulty,generation=generation,first=first,second=second,player_gender=player_gender)
    player=[factory_member(r,r['starters'][generation][i],slot,5,True) for slot,i in enumerate((first,second),1)]
    enemy=[factory_member(r,species,slot,2) for slot,species in enumerate(('SPECIES_POOCHYENA','SPECIES_ZIGZAGOON'),1)]
    return dict(scenario_id=f'birch-gen{generation}-{first}-{second}-{player_gender}',
        battle_id='BIRCH_RESCUE',battle_kind='birch_rescue',trainer_id='TRAINER_NONE',
        difficulty=difficulty,level_cap=14,opponent_level_baseline=14,legality_status='proven',
        source_fingerprint=fingerprint,producer=dict(kind='birch_rescue',arguments=args),
        opening_parameters=args,player_gender=player_gender,
        progression_flags={'FLAG_RESCUED_BIRCH':False,'FLAG_SYS_POKEMON_GET':False},
        progression_vars={'VAR_STARTER_GEN':generation,'VAR_EC_OPENING_STATE':0},
        excluded_target_rewards=['FLAG_RESCUED_BIRCH','EC_OPENING_RESCUE_WON'],
        battle_field=rescue_field(),
        expected_scripted_wild=dict(battle_kind='birch_rescue',source_member_domains=enemy),
        expected_player_factory=dict(factory='GiveEmeraldChampionsStarterPair',members=player,
            context=dict(generation=generation,first=first,second=second,
                         player_gender=0 if player_gender=='male' else 1,
                         opening_state=int(re.search(r'#define EC_OPENING_PAIR_GRANTED\s+(\d+)',
                             (ROOT/'include/constants/emerald_champions.h').read_text())[1]))),
        source_evidence=[dict(path=p,sha256=hashlib.sha256((ROOT/p).read_bytes()).hexdigest()) for p in SOURCE_PATHS],
        scope='Actual unprepared level-five factory pair; no tutor, held item, infection, evolution or target rewards. Independent rescue fight with retries.')

def certify_scenario(scenario, *, internal_fingerprint=None):
    producer=scenario.get('producer',{})
    if producer.get('kind')!='birch_rescue':raise ValueError('No supported scripted-wild source producer')
    try: rebuilt=rescue_scenario(**producer['arguments'],internal_fingerprint=internal_fingerprint)
    except (KeyError,TypeError) as error:raise ValueError('Invalid rescue producer arguments') from error
    if scenario!=rebuilt:raise ValueError('Rescue source certificate/domain mismatch or stale source')

def verify_members(domains, actual):
    if len(domains)!=len(actual):raise ValueError('Factory member count mismatch')
    for source,mon in zip(domains,actual):
        if set(source)!= {'fixed','domains'}:raise ValueError('Unsupported source member domain')
        if set(mon)!=set(source['fixed'])|set(source['domains']):raise ValueError('Incomplete native factory audit')
        for key,value in source['fixed'].items():
            if mon[key]!=value:raise ValueError(f'Factory slot {mon["slot"]} {key} mismatch')
        for key,allowed in source['domains'].items():
            if key not in {'nature','ability','item'} or mon[key] not in allowed:raise ValueError(f'Factory slot {mon["slot"]} {key} outside source domain')

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True)
    p.add_argument('--generation',type=int,default=3);p.add_argument('--first',type=int,default=0);p.add_argument('--second',type=int,default=1)
    p.add_argument('--gender',choices=['male','female'],default='male');a=p.parse_args()
    fp=suite.source_fingerprint();scenarios=[rescue_scenario(d,generation=a.generation,first=a.first,second=a.second,player_gender=a.gender,internal_fingerprint=fp) for d in ['easy','medium','hard']]
    if suite.source_fingerprint()!=fp:raise ValueError('Source changed during rescue export')
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(dict(schema_version=2,source_generated=True,source_fingerprint=fp,scenarios=scenarios),indent=2)+'\n')
if __name__=='__main__':main()
