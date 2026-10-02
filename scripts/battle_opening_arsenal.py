#!/usr/bin/env python3
"""Certify concrete pre-Route103-rival preparations, never a maximal arsenal.

Random acquisition natures and first-ball captures describe possible legal
outcomes, not guaranteed outcomes or an earned save. Optional finite plans
include repeatable farming, PC boxing and ordinary random Pokérus infection.
Native acquisition receipts and maximal availability remain unproven; later
routes, Mega access and target rewards are not granted.
"""
from __future__ import annotations
import argparse
import copy
from collections import Counter
from functools import lru_cache
import json
import hashlib
from pathlib import Path
import re
import struct
import sys

from battle_arsenal import ROOT, source_evidence, source_fingerprint, validate_party, species_rules
import ec_moves
from verify_trainer_ability_legality import preprocess_species_info, resolve_species

DEFAULT_CAPTURES = ('SPECIES_PAWMI', 'SPECIES_POOCHYENA', 'SPECIES_PACHIRISU', 'SPECIES_SHELLOS_WEST')
SOURCE_PATHS = (
    'src/starter_choose.c', 'src/emerald_champions_story.c', 'src/new_game.c', 'src/caps.c',
    'src/pokemon.c', 'src/party_menu.c', 'src/wild_encounter.c', 'src/field_specials.c',
    'src/inclement_stat_services.c', 'src/move_relearner.c', 'src/data/items.h',
    'src/data/wild_encounters.json', 'data/scripts/general_mart.inc',
    'data/scripts/emerald_champions.inc', 'data/scripts/pkmn_center_nurse.inc',
    'data/maps/Route101/map.json', 'data/maps/Route101/scripts.inc',
    'data/maps/OldaleTown/map.json', 'data/maps/OldaleTown/scripts.inc',
    'data/maps/OldaleTown_PokemonCenter_1F/map.json',
    'data/maps/OldaleTown_PokemonCenter_1F/scripts.inc',
    'data/maps/Route103/map.json', 'data/maps/Route103/scripts.inc',
    'data/layouts/layouts.json', 'data/layouts/Route103/map.bin',
    'data/tilesets/primary/general/metatile_attributes.bin', 'src/battle_setup.c',
    'src/metatile_behavior.c', 'include/global.fieldmap.h', 'include/config/general.h',
    'include/config/pokemon.h', 'include/constants/pokemon.h',
    'include/constants/pokemon_stats.h', 'include/constants/field_specials.h',
    'include/constants/emerald_champions.h', 'include/pokemon.h', 'include/fieldmap.h',
    'include/constants/metatile_behaviors.h',
)

def text(path):
    return (ROOT / path).read_text()

def number(path, name):
    match = re.search(r'^#define\s+' + re.escape(name) + r'\s+(\d+)\b', text(path), re.M)
    if not match:
        raise ValueError('Source constant unresolved: ' + name)
    return int(match[1])

def demand(path, snippets):
    for snippet in snippets:
        if snippet not in text(path):
            raise ValueError('Opening source contract changed: ' + path + ': ' + snippet)

def evolution_entries(block):
    marker=re.search(r"\.evolutions\s*=",block)
    if not marker:
        return []
    opening=block.index('{',marker.end());depth=0;start=None;rows=[]
    for i in range(opening,len(block)):
        char=block[i]
        if char=='{':
            depth+=1
            if depth==2:start=i
        elif char=='}':
            if depth==2:
                raw=block[start:i+1]
                match=re.match(r"\{(EVO_\w+),\s*(\d+),\s*(SPECIES_\w+)(.*)\}",raw,re.S)
                if match:
                    conditions=[list(pair) for pair in re.findall(r"\{(IF_\w+),\s*([^{}]+)\}",match[4])]
                    rows.append(dict(method=match[1],level=int(match[2]),target=match[3],conditions=conditions))
            depth-=1
            if depth==0:break
    return rows


def rules():
    paths=set(ROOT/p for p in SOURCE_PATHS)
    paths.update((ROOT/'src/data/pokemon/species_info').glob('*.h'))
    paths.update((ROOT/'include/config').glob('*.h'))
    identity=tuple((str(p),hashlib.sha256(p.read_bytes()).digest()) for p in sorted(paths))
    return _rules(identity)

@lru_cache(maxsize=1)
def _rules(_source_identity):
    starters = {int(g): re.findall(r'SPECIES_\w+', row) for g, row in re.findall(
        r'\[(\d+)\]\s*=\s*\{([^}]+)\}', text('src/starter_choose.c').split('sStarterMons[][STARTER_MON_COUNT] =')[1].split('};')[0])}
    wild = json.loads(text('src/data/wild_encounters.json'))
    rosters = {row['map']: sorted({m['species'] for m in row['land_mons']['mons']})
               for group in wild['wild_encounter_groups'] for row in group.get('encounters', [])
               if row.get('map') in {'MAP_ROUTE101', 'MAP_ROUTE103'}}
    kit_block = text('src/field_specials.c').split('void GiveEmeraldChampionsStarterBattleItems(void)')[1].split('};')[0]
    kit = dict((token, int(count)) for token, count in re.findall(r'\{(ITEM_\w+),\s*(\d+)\}', kit_block))
    cash = int(re.search(r'SetMoney\(&gSaveBlock1Ptr->money,\s*(\d+)\)', text('src/new_game.c'))[1])
    ball_price = int(re.search(r'\[ITEM_POKE_BALL\]\s*=\s*\{.*?\.price\s*=\s*(\d+)', text('src/data/items.h'), re.S)[1])
    cap = int(re.search(r'return (\d+);\s*\}\s*else if', text('src/caps.c'))[1])
    ev_block = text('src/pokemon.c').split('sPlayerBaselineEVs[NUM_STATS] =')[1].split('};')[0]
    evs = []
    for stat in ('HP','ATK','DEF','SPATK','SPDEF','SPEED'):
        value = re.search(r'\[STAT_' + stat + r'\]\s*=\s*(\w+)', ev_block)[1]
        evs.append(int(value) if value.isdigit() else number('include/constants/pokemon_stats.h', value))
    demand('src/emerald_champions_story.c', ['first == second', 'CreateRandomMonWithIVs', 'second + 1', '(GetEmeraldChampionsRivalStarterIndex() + 2) % 3'])
    demand('src/pokemon.c', ['Random32(), OTID_STRUCT_PLAYER_ID', 'MaxPlayerMonIVs(mon);', 'SetPlayerMonBaselineEVs(mon);'])
    demand('data/scripts/general_mart.inc', ['call EmeraldChampions_EventScript_TryStarterKit', 'goto_if_set FLAG_SYS_POKEMON_GET, Mart_Poke_Center_Pokedex'])
    demand('data/scripts/emerald_champions.inc', ['setmoverelearnerstate MOVE_RELEARNER_ALL_MOVES', 'special SetPlannedEVPricing', 'special ApplyEmeraldChampionsBonding'])
    demand('data/scripts/pkmn_center_nurse.inc', ['giveitem ITEM_LEVELER, 1'])
    center = json.loads(text('data/maps/OldaleTown_PokemonCenter_1F/map.json'))
    scripts = {o['script'] for o in center['object_events'] if o.get('flag','0') == '0'}
    if not {'General_Mart_Script', 'Common_EventScript_EmeraldChampionsMoveTutor'} <= scripts:
        raise ValueError('Oldale preparation services are gated or missing')
    oldale = json.loads(text('data/maps/OldaleTown/map.json'))
    if not {'MAP_ROUTE101','MAP_ROUTE103'} <= {c['map'] for c in oldale['connections']}:
        raise ValueError('Opening route adjacency changed')
    abilities, _, aliases = species_rules()
    species_text = preprocess_species_info()
    marks = list(re.finditer(r'\[(SPECIES_\w+)\]\s*=\s*\{', species_text))
    friendship = {}; initial_abilities={}; evolutions={}
    # STANDARD_FRIENDSHIP's two branches are source-owned; the configured
    # generation chain is resolved from general.h rather than assuming Gen9.
    defines = dict(re.findall(r'^#define\s+(GEN_\w+)\s+([^/\n]+)', text('include/config/general.h'), re.M))
    def generation(expr):
        expr = re.sub(r'\bGEN_\w+\b', lambda m: str(generation(defines[m[0]])), expr).strip()
        if not re.fullmatch(r'[\d\s+]+', expr):
            raise ValueError('Configured friendship generation unresolved')
        return sum(map(int, expr.split('+')))
    configured = re.search(r'^#define\s+P_UPDATED_FRIENDSHIP\s+(\w+)', text('include/config/pokemon.h'), re.M)[1]
    branches = re.search(r'#define STANDARD_FRIENDSHIP\s+\(\(P_UPDATED_FRIENDSHIP >= GEN_8\) \? (\d+) : (\d+)\)', text('include/constants/pokemon.h'))
    standard = int(branches[1] if generation(configured) >= generation('GEN_8') else branches[2])
    friendship_config = re.search(r'^#define\s+P_FRIENDSHIP_EVO_THRESHOLD\s+(\w+)', text('include/config/pokemon.h'), re.M)[1]
    friendship_branches = re.search(r'#define FRIENDSHIP_EVO_THRESHOLD\s+\(\(P_FRIENDSHIP_EVO_THRESHOLD >= GEN_8\) \? (\d+) : (\d+)\)', text('include/pokemon.h'))
    friendship_threshold = int(friendship_branches[1] if generation(friendship_config) >= generation('GEN_8') else friendship_branches[2])
    for i, mark in enumerate(marks):
        block = species_text[mark.end():marks[i+1].start() if i+1<len(marks) else len(species_text)]
        evolutions[resolve_species(mark[1],aliases)]=evolution_entries(block)
        ability_list=re.search(r'\.abilities\s*=\s*\{([^}]+)',block)
        if ability_list:
            initial_abilities[resolve_species(mark[1],aliases)]=re.findall(r'ABILITY_\w+',ability_list[1])[0]
        value = re.search(r'\.friendship\s*=\s*(\w+)', block)
        if value and (value[1].isdigit() or value[1] == 'STANDARD_FRIENDSHIP'):
            friendship[resolve_species(mark[1],aliases)] = int(value[1]) if value[1].isdigit() else standard
    return dict(starters=starters, rosters=rosters, kit=kit, cash=cash, ball_price=ball_price, cap=cap,
                evs=evs, iv=number('include/constants/pokemon_stats.h','MAX_PER_STAT_IVS'),
                ev_fee=number('include/constants/field_specials.h','EV_PLAN_FEE'),
                abilities=abilities, aliases=aliases, friendship=friendship, friendship_threshold=friendship_threshold,initial_abilities=initial_abilities,evolutions=evolutions)


def evolution_path(base,target,nature,r):
    """Only configured cap-legal level edges with explicitly audited conditions."""
    allowed={'IF_MIN_FRIENDSHIP','IF_KNOWS_MOVE_TYPE','IF_TIME','IF_NOT_TIME',
             'IF_PID_UPPER_MODULO_10_GT','IF_PID_UPPER_MODULO_10_LT'}
    base_id=resolve_species(base,r['aliases']);target_id=resolve_species(target,r['aliases'])
    if base_id==target_id:
        raise ValueError('Evolution must change species')
    todo=[(base_id,[])];seen={base_id}
    while todo:
        current,path=todo.pop(0)
        for edge in r['evolutions'].get(current,[]):
            if edge['method']!='EVO_LEVEL' or edge['level']>r['cap']:
                continue
            if any(kind not in allowed for kind,arg in edge['conditions']):
                continue
            following=resolve_species(edge['target'],r['aliases'])
            step=dict(from_species=current,to_species=following,level=edge['level'],conditions=edge['conditions'])
            if following==target_id:
                path=path+[step];break
            if following not in seen:
                seen.add(following);todo.append((following,path+[step]))
        else:continue
        break
    else:raise ValueError('No audited cap-legal evolution path: '+base+' -> '+target)
    moves_text=text('src/data/moves_info.h')
    witness={'base_species':base,'target_species':target,'steps':path,'leveler_at_cap':r['cap']}
    for step in path:
        for kind,arg in step['conditions']:
            arg=arg.strip()
            if kind=='IF_MIN_FRIENDSHIP':
                if arg!='FRIENDSHIP_EVO_THRESHOLD':raise ValueError('Unknown friendship evolution threshold')
                witness['friendship']=255
            elif kind=='IF_KNOWS_MOVE_TYPE':
                if arg!='TYPE_FAIRY':raise ValueError('Unaudited move-type evolution')
                legal,_=ec_moves.legal_moves_with_rom_union(step['from_species'])
                if 'MOVE_BABY_DOLL_EYES' not in legal:raise ValueError('No audited Fairy move for evolution')
                move_block=moves_text.split('[MOVE_BABY_DOLL_EYES] =')[1].split('\n    [MOVE_',1)[0]
                if not re.search(r'\.type\s*=\s*TYPE_FAIRY\s*,',move_block):raise ValueError('Evolution move type changed')
                witness['teach_before_evolution']='MOVE_BABY_DOLL_EYES'
            elif kind in {'IF_TIME','IF_NOT_TIME'}:
                if arg!='TIME_NIGHT':raise ValueError('Unaudited evolution time')
                witness['time']='TIME_NIGHT' if kind=='IF_TIME' else 'TIME_DAY'
                # Sylveon is the earlier edge. Replace all moves with verified
                # Normal moves so its Fairy-move requirement cannot preempt it.
                legal,_=ec_moves.legal_moves_with_rom_union(step['from_species'])
                normal_moves=[]
                for move in sorted(legal):
                    block=moves_text.split('['+move+'] =',1)
                    if len(block)>1 and re.search(r'\.type\s*=\s*TYPE_NORMAL\s*,',block[1].split('\n    [MOVE_',1)[0]):normal_moves.append(move)
                if len(normal_moves)<4:raise ValueError('No verified non-Fairy preparation')
                witness['moves_before_evolution']=normal_moves[:4]
            elif kind.startswith('IF_PID_UPPER_MODULO_10_'):
                if not arg.isdigit():raise ValueError('Unaudited personality condition')
                nature_id=number('include/constants/pokemon.h',nature)
                count=number('include/constants/pokemon.h','NUM_NATURES')
                compare=(lambda v:v>int(arg)) if kind.endswith('_GT') else (lambda v:v<int(arg))
                upper=next(v for v in range(10) if compare(v))
                personality=(upper<<16)+(nature_id-(upper<<16))%count
                assert personality%count==nature_id and compare((personality>>16)%10)
                witness['personality_witness']=personality
    demand('src/party_menu.c',['targetSpecies = GetEvolutionTargetSpecies(mon, EVO_MODE_NORMAL',
                              'static void CB2_ContinueLevelerEvolution(void)'])
    demand('src/evolution_scene.c',['SetMonData(mon, MON_DATA_SPECIES, &after);'])
    return witness


def opening_field(trainer):
    m = json.loads(text('data/maps/Route103/map.json'))
    rival = next(o for o in m['object_events'] if o['script'] == 'Route103_EventScript_Rival')
    if rival['movement_type'] != 'MOVEMENT_TYPE_FACE_RIGHT':
        raise ValueError('Rival approach changed; battle field unresolved')
    player = [rival['x']+1,rival['y']]
    layout = next(l for l in json.loads(text('data/layouts/layouts.json'))['layouts'] if l['id'] == m['layout'])
    word = struct.unpack_from('<H',(ROOT/layout['blockdata_filepath']).read_bytes(),2*(player[1]*layout['width']+player[0]))[0]
    tile = word & int(re.search(r'MAPGRID_METATILE_ID_MASK\s+(0x[0-9A-F]+)',text('include/global.fieldmap.h'))[1],16)
    if tile >= number('include/fieldmap.h','NUM_METATILES_IN_PRIMARY'):
        raise ValueError('Rival player tile is outside audited primary tileset')
    attribute = struct.unpack_from('<H',(ROOT/'data/tilesets/primary/general/metatile_attributes.bin').read_bytes(),tile*2)[0]
    behavior = attribute & int(re.search(r'METATILE_ATTR_BEHAVIOR_MASK\s+(0x[0-9A-F]+)',text('include/global.fieldmap.h'))[1],16)
    # This factory owns one checked approach, not every possible interaction tile.
    behaviors=list(dict.fromkeys(re.findall(r'MB_\w+',text('include/constants/metatile_behaviors.h'))))
    if behavior != behaviors.index('MB_TALL_GRASS') or word & 0xC00 or m['weather'] != 'WEATHER_SUNNY':
        raise ValueError('Audited grass approach or weather changed')
    demand('src/metatile_behavior.c',['metatileBehavior == MB_TALL_GRASS'])
    demand('src/battle_setup.c',['PlayerGetDestCoords(&x, &y);','return BATTLE_ENVIRONMENT_GRASS;'])
    return dict(map='Route103',weather=m['weather'],environment='BATTLE_ENVIRONMENT_GRASS',
                trainer_xy=[rival['x'],rival['y']],player_xy=player,metatile_id=tile,metatile_behavior='MB_TALL_GRASS',
                source_evidence=[source_evidence(ROOT/p) for p in SOURCE_PATHS if 'Route103' in p or 'metatile' in p or p in {'src/battle_setup.c','include/global.fieldmap.h','data/layouts/layouts.json'}])


def build_opening_scenario(*args,**kwargs):
    return _opening_scenario(*args,**kwargs,_fingerprint=source_fingerprint())


def _opening_scenario(difficulty='hard', *, generation=3, first=0, second=1, captures=(), natures=None, player_gender='male',evolutions=None,preparation=None,_fingerprint=None,_context=None):
    """Return a concrete, finite source-certified subset scenario.

    ``natures`` describes the actual naturally possible rolls for this roster.
    A solver may change moves/abilities/EVs/items within its certificate, but
    changing those acquisition rolls requires generating another scenario.
    """
    if preparation is not None:
        return _expanded_opening_scenario(difficulty,generation=generation,first=first,second=second,
            captures=captures,natures=natures,player_gender=player_gender,evolutions=evolutions,
            preparation=preparation,_fingerprint=_fingerprint,_context=_context)
    r=_context["rules"] if _context is not None else rules()
    if difficulty not in {'easy','medium','hard'} or generation not in range(1,10) or first not in range(3) or second not in range(3) or first==second:
        raise ValueError('Invalid difficulty or paired starter selection')
    if player_gender not in {'male','female'} or len(captures)>4:
        raise ValueError('Invalid opening roster or player gender')
    roster=[r['starters'][generation][first],r['starters'][generation][second],*captures]
    if len(set(roster))!=len(roster):
        raise ValueError('Opening subset factory requires distinct species')
    for species in captures:
        if not any(species in rows for rows in r['rosters'].values()):
            raise ValueError('Capture absent from audited opening land encounters: '+species)
    natures={species:(natures or {}).get(species,'NATURE_HARDY') for species in roster}
    real_natures=set(re.findall(r'^#define\s+(NATURE_\w+)\s+\d+',text('include/constants/pokemon.h'),re.M))
    real_natures-={'NATURE_RANDOM','NATURE_MAY_SYNCHRONIZE'}
    if not set(natures.values())<=real_natures:
        raise ValueError('Nature is not a naturally possible personality roll')
    captured_cost=len(captures)*r['ball_price']
    reserve=len(roster)*r['ev_fee']
    if captured_cost+reserve>r['cash']:
        raise ValueError('Preparation exceeds opening cash')
    remaining=3-first-second
    branch=(remaining+2)%3
    rival='MAY' if player_gender=='male' else 'BRENDAN'
    script=text('data/maps/Route103/scripts.inc')
    branch_label=re.search(r'case '+str(branch)+r', (Route103_EventScript_Start'+rival.title()+r'Battle\w+)',script)[1]
    trainer=re.search(re.escape(branch_label)+r'::[^\n]*\n\s*trainerbattle_no_intro_double\s+(TRAINER_\w+)',script)[1]
    defaults={species:dict(nature=natures[species],ability=r['initial_abilities'][resolve_species(species,r['aliases'])],
                           evs=list(r['evs']),ivs=[r['iv']]*6,friendship=r['friendship'][resolve_species(species,r['aliases'])]) for species in roster}
    evolution_proofs=[];base_roster=list(roster)
    unknown=set(evolutions or {})-set(base_roster)
    if unknown:
        raise ValueError('Evolution has no acquired base: '+sorted(unknown)[0])
    for base in base_roster:
        if base not in (evolutions or {}):
            continue
        target=evolutions[base]
        proof=evolution_path(base,target,natures[base],r);evolution_proofs.append(proof)
        inherited=defaults.pop(base)
        inherited['ability']=r['initial_abilities'][resolve_species(target,r['aliases'])]
        if 'friendship' in proof:inherited['friendship']=proof['friendship']
        defaults[target]=inherited;roster[base_roster.index(base)]=target
    if len(set(roster))!=len(roster):raise ValueError('Evolution targets collide; explicit slot ownership required')
    resources=dict(Counter(roster));resources.update(r['kit']);resources['ITEM_LEVELER']=1
    if _context is None:
        evidence=[source_evidence(ROOT/p) for p in SOURCE_PATHS]
        evidence.extend(source_evidence(p) for p in sorted((ROOT/'src/data/pokemon/species_info').glob('*.h')))
    else:
        evidence=copy.deepcopy(_context['source_evidence'])
    scenario=dict(scenario_id=f'opening-gen{generation}-{first}-{second}-'+','.join(s.removeprefix('SPECIES_').lower() for s in captures or ['none']),
                  trainer_id=trainer,difficulty=difficulty,source_fingerprint=_fingerprint,legality_status='proven',
                  maximal_arsenal_status='unresolved',level_cap=r['cap'],party_size_range=[2,len(roster)],
                  opening_parameters=dict(generation=generation,first=first,second=second,captures=list(captures),natures=natures,player_gender=player_gender),
                  producer=dict(kind='opening',arguments=dict(difficulty=difficulty,generation=generation,first=first,second=second,captures=list(captures),natures=natures,player_gender=player_gender)),
                  progression_vars={'VAR_STARTER_GEN':generation,'VAR_STARTER_MON':first,'VAR_EC_SECOND_STARTER':second+1,
                                    'VAR_EC_OPENING_STATE':number('include/constants/emerald_champions.h','EC_OPENING_RESCUE_WON')},
                  progression_flags={'FLAG_SYS_POKEMON_GET':True,'FLAG_RESCUED_BIRCH':True,'FLAG_VISITED_OLDALE_TOWN':True,
                                     'FLAG_EC_PLAYER_IVS_MAXED':True,'FLAG_EC_RECEIVED_STARTER_BATTLE_ITEMS':True},
                  battle_field=opening_field(trainer),candidate_roster=roster,
                  acquisition_states=[dict(resources=resources,services=['legal_move_tutor','ability','evs','friendship'],
                                           pokemon_defaults=defaults,pokerus_values=[0],pp_bonuses_values=[0],
                                           friendship_values=[0,r['friendship_threshold'],255],
                                           source_evidence=evidence)],
                  economy=dict(starting_cash=r['cash'],purchased_poke_balls=len(captures),capture_cost=captured_cost,
                               capture_condition='each selected encounter caught with one purchased ball; possible, not guaranteed',
                               ev_fee_per_changed_mon=r['ev_fee'],ev_fee_reserve=reserve,cash_remaining_minimum=r['cash']-captured_cost-reserve),
                  unresolved=['maximal reachable pool','other routes and trade prerequisites','all evolution conditions','Pokérus hunting','alternate approach tiles'],
                  scope='Source-audited possible preparation subset; neither maximal arsenal nor earned campaign progress.')
    if evolutions:
        scenario['opening_parameters']['evolutions']=dict(evolutions)
        scenario['producer']['arguments']['evolutions']=dict(evolutions)
        scenario['acquisition_states'][0]['evolution_proofs']=evolution_proofs
        scenario['acquisition_states'][0]['source_evidence'] += [source_evidence(ROOT/p) for p in ('src/evolution_scene.c','src/data/moves_info.h')]
        # JSON object ordering is not acquisition ordering. Use the captured
        # roster so sorted JSON round trips preserve the certificate identity.
        scenario['scenario_id']+='-evo-'+','.join(evolutions[base].removeprefix('SPECIES_').lower()
                                               for base in captures if base in evolutions)
    return scenario


def certify_scenario(scenario, *, internal_fingerprint=None, internal_context=None):
    producer=scenario.get('producer',{})
    if producer.get('kind')!='opening':
        raise ValueError('Unknown opening scenario producer')
    arguments=producer['arguments']
    allowed={'difficulty','generation','first','second','captures','natures','player_gender','evolutions','preparation'}
    if set(arguments)-allowed:
        raise ValueError('Opening producer arguments contain unsupported internal parameters')
    expected=_opening_scenario(**arguments,_fingerprint=internal_fingerprint,_context=internal_context)
    # Source fingerprint is verified by the generic validator; this check owns
    # the actual grants, mutually compatible acquisition rolls and world state.
    for key,value in expected.items():
        if key!='source_fingerprint' and scenario.get(key)!=value:
            raise ValueError('Opening certificate differs from source factory: '+key)


def certify_opening_manifest(scenario, manifest, *, internal_fingerprint=None, internal_context=None):
    certify_scenario(scenario,internal_fingerprint=internal_fingerprint,internal_context=internal_context)
    if not 2<=len(manifest.get('party',[]))<=6:
        raise ValueError('Opening doubles party needs two to six Pokemon')
    state=scenario['acquisition_states'][0]
    for mon in manifest['party']:
        if 'owned_monsters' in state:
            ident=mon.get('availability',{}).get('owned_mon_id')
            default=state['pokemon_defaults_by_id'].get(ident,{})
            if not default or mon.get('pokerus',0)!=default['pokerus'] or mon.get('pp_bonuses',0)!=default['pp_bonuses']:
                raise ValueError('Opening infection/PP bytes differ from owned preparation')
        else:
            default=state['pokemon_defaults'].get(mon['species'],{})
            if mon.get('pp_bonuses',0)!=0 or mon.get('pokerus',0)!=0:
                raise ValueError('Opening PP upgrades/Pokerus are unproven')
        if mon.get('friendship',default.get('friendship')) not in {*state['friendship_values'],default.get('friendship')}:
            raise ValueError('Friendship not reachable through audited bonding')
    validate_party(scenario,manifest,internal_fingerprint=internal_fingerprint,internal_context=internal_context)


def candidate_party(scenario, *, internal_fingerprint=None, internal_context=None):
    """Simple strategic seeds; legality comes from configured move/ability data."""
    if 'owned_monsters' in scenario['acquisition_states'][0]:
        return expanded_candidate_party(scenario,internal_fingerprint=internal_fingerprint,internal_context=internal_context)
    defaults=scenario['acquisition_states'][0]['pokemon_defaults'];r=internal_context['rules'] if internal_context is not None else rules();party=[]
    preferred=('MOVE_FAKE_OUT','MOVE_FOLLOW_ME','MOVE_NUZZLE','MOVE_SNARL','MOVE_GIGA_DRAIN','MOVE_FLAMETHROWER','MOVE_EARTH_POWER','MOVE_THUNDERBOLT','MOVE_CRUNCH','MOVE_BUG_BUZZ','MOVE_ICE_BEAM','MOVE_SURF','MOVE_HELPING_HAND','MOVE_PROTECT')
    items=iter(r['kit'])
    for species in scenario['candidate_roster']:
        legal,_=ec_moves.legal_moves_with_rom_union(species)
        moves=list(dict.fromkeys([m for m in preferred if m in legal]+sorted(legal)))[:4]
        base=next((base for base,target in scenario['opening_parameters'].get('evolutions',{}).items() if target==species),species)
        acquisition=dict(method='starter_pair' if base not in scenario['opening_parameters']['captures'] else 'land_capture',
                         acquired_species=base,
                         generation=scenario['opening_parameters']['generation'],
                         naturally_rolled_nature=defaults[species]['nature'],
                         source_evidence=[source_evidence(ROOT/p) for p in ('src/emerald_champions_story.c','src/starter_choose.c')] if base not in scenario['opening_parameters']['captures'] else
                         [source_evidence(ROOT/'src/data/wild_encounters.json')],
                         maps=[m for m,roster in r['rosters'].items() if base in roster])
        if base!=species:
            acquisition['evolution']=next(proof for proof in scenario['acquisition_states'][0]['evolution_proofs'] if proof['base_species']==base)
        mon_defaults={**defaults[species],'evs':list(defaults[species]['evs']),'ivs':list(defaults[species]['ivs'])}
        party.append(dict(species=species,item=next(items,'ITEM_NONE'),level=scenario['level_cap'],moves=moves,
                          availability=acquisition,role='Opening doubles attack/support candidate',
                          **mon_defaults,pokerus=0,pp_bonuses=0))
    manifest={'encounter':scenario['trainer_id'],'availability_audit':{'scenario_id':scenario['scenario_id'],'legality_status':'proven','maximal_arsenal_status':'unresolved','source_evidence':scenario['acquisition_states'][0]['source_evidence']},'party':party};certify_opening_manifest(scenario,manifest,internal_fingerprint=internal_fingerprint,internal_context=internal_context);return manifest



FARM_SOURCE_PATHS = (
    'src/battle_set_effect.c','src/battle_script_commands.c','src/battle_setup.c','src/battle_main.c',
    'src/pokerus.c','src/player_pc.c','src/item.c','src/money.c','data/scripts/pc.inc',
    'src/data/pokemon/emerald_champions_preparation_learnsets.h','include/config/item.h',
    'include/config/pokerus.h','include/config/wild_encounter.h','include/constants/item.h','include/money.h',
    'include/config/general.h','include/constants/global.h','include/constants/items.h',
    'include/constants/pokemon.h','include/item.h','src/data/items.h','src/data/decoration/header.h',
    'scripts/economy_reference.py','scripts/item_catalog.py','scripts/audit/map_dynamic_inventory.py',
)


def early_farm_rules():
    identity=tuple((path,hashlib.sha256(text(path).encode()).digest()) for path in FARM_SOURCE_PATHS)
    return _early_farm_rules(identity)


@lru_cache(maxsize=1)
def _early_farm_rules(identity):
    # Evaluate configured prices through the existing source extractor. This
    # cache is keyed by every farming authority; source edits invalidate it.
    from economy_reference import item_prices
    prices=item_prices(ROOT)
    demand('src/battle_set_effect.c',['gPaydayMoney += (gBattleMons[cv->battlerAtk].level * 5)'])
    demand('src/battle_setup.c',['if (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER))','return TRUE;'])
    demand('src/battle_script_commands.c',['RandomChance(RNG_PICKUP_AFTER_BATTLE, 1, 10)',
        'heldItem == ITEM_NONE','IsPartyMonHandFreeAfterBattle(i)'])
    table=text('src/battle_script_commands.c').split('sPickupTable[] =',1)[1].split('};',1)[0]
    pickup={}
    for item,columns in re.findall(r'\{\s*(ITEM_\w+),\s*\{([^}]+)',table):
        values=[0 if token.strip()=='_' else int(token.strip()) for token in columns.split(',') if token.strip()]
        if values[1]:pickup[item]=values[1]
    demand('include/config/pokerus.h',['P_POKERUS_ENABLED                TRUE','P_POKERUS_FLAG_INFECTION         0'])
    demand('src/pokerus.c',['#define LIMITED_POKERUS 0xFC','RandomlyGivePartyPokerus','IsPokerusInParty()',
        'void GiveMonPokerus(struct Pokemon *mon','canSpread ? 2 : 0','recipients never spread'])
    for name in ('P_POKERUS_INFECTION_ODDS','P_POKERUS_SPREAD_ODDS'):
        if not 0<number('include/config/pokerus.h',name)<=65536:
            raise ValueError('Opening infection plan has no supported positive-probability roll')
    if number('include/config/wild_encounter.h','WE_DOUBLE_WILD_CHANCE') or number('include/config/wild_encounter.h','WE_FLAG_FORCE_DOUBLE_WILD'):
        raise ValueError('Isolated single-member wild infection path changed')
    demand('src/item.c',['return GetItemPrice(itemId) / ITEM_SELL_FACTOR;'])
    demand('include/config/item.h',['#define I_SELL_VALUE_FRACTION           GEN_LATEST'])
    demand('include/config/general.h',['#define GEN_LATEST GEN_CHAMPIONS','#define GEN_CHAMPIONS GEN_9 + 1'])
    demand('include/constants/item.h',['#define ITEM_SELL_FACTOR ((I_SELL_VALUE_FRACTION >= GEN_9) ? 4 : 2)'])
    demand('src/money.c',['if (toSet + toAdd > MAX_MONEY)','toSet = MAX_MONEY;'])
    demand('src/battle_main.c',['RandomlyGivePartyPokerus();','PartySpreadPokerus();'])
    demand('data/scripts/pc.inc',['case 0, EventScript_AccessPokemonStorage'])
    return {'prices':prices,'pickup':pickup,'money_cap':number('include/money.h','MAX_MONEY'),
            'evidence':[source_evidence(ROOT/path) for path in FARM_SOURCE_PATHS]}


def _finite_quantity(value, label, maximum=None):
    if type(value) is not int or value<0 or (maximum is not None and value>maximum):
        raise ValueError('Invalid finite preparation quantity: '+label)
    return value


def apply_owned_pokerus_plan(owned, defaults, plan):
    """Apply one finite graph atomically to explicit owned IDs, never species."""
    if not isinstance(plan,dict) or set(plan)-{'direct','transmissions'}:
        raise ValueError('Unsupported Pokerus treatment or arbitrary byte')
    if not isinstance(owned,dict)or not isinstance(defaults,dict)or set(owned)-set(defaults):
        raise ValueError('Pokerus preparation requires complete owned-ID defaults')
    values={ident:defaults[ident].get('pokerus',0)for ident in owned}
    if any(type(value)is not int or value not in {0,0xFC,0xFD,0xFE}for value in values.values()):
        raise ValueError('Recorded Pokerus state has no supported acquisition')
    direct=plan.get('direct',[])
    if not isinstance(direct,list)or any(not isinstance(ident,str)for ident in direct)or len(set(direct))!=len(direct):
        raise ValueError('Direct infection must identify distinct owned acquisitions')
    for ident in direct:
        if ident not in owned or values[ident]!=0:
            raise ValueError('Direct infection requires an owned never-infected acquisition')
        values[ident]=0xFE
    transmissions=plan.get('transmissions',[])
    if not isinstance(transmissions,list):raise ValueError('Transmission plan must be a finite list')
    for row in transmissions:
        if not isinstance(row,dict)or set(row)!={'donor','recipient'}:
            raise ValueError('Transmission needs donor and recipient IDs')
        donor=row['donor'];recipient=row['recipient']
        if not isinstance(donor,str)or not isinstance(recipient,str)or donor not in owned or recipient not in owned:
            raise ValueError('Transmission requires acquired owned IDs')
        if values[donor]not in {0xFE,0xFD}or values[recipient]!=0:
            raise ValueError('Transmission exceeds donor budget or reinfects a recipient')
        values[donor]-=1;values[recipient]=0xFC
    for ident,value in values.items():defaults[ident]['pokerus']=value


def _expanded_opening_scenario(difficulty, *, generation,first,second,captures,natures,
                               player_gender,evolutions,preparation,_fingerprint,_context):
    if not isinstance(preparation,dict):raise ValueError('Preparation must be an explicit plan')
    keys={'captures','selected','pay_day_wins','pay_day_farmer','pickup_items','pickup_farmer',
          'sell_pickup','purchases','pokerus'}
    if set(preparation)-keys:raise ValueError('Unsupported preparation grant or service')
    r=_context['rules'] if _context is not None else rules();farm=early_farm_rules()
    scenario=_opening_scenario(difficulty,generation=generation,first=first,second=second,
        natures=natures,player_gender=player_gender,_fingerprint=_fingerprint,_context=_context)
    owned={};defaults={};proofs=[]
    def acquire(ident,base,nature,target=None,method='land_capture'):
        if not isinstance(ident,str) or not re.fullmatch(r'[a-zA-Z0-9_-]+',ident) or ident in owned:
            raise ValueError('Missing or duplicate owned Pokemon ID')
        if not isinstance(base,str) or base not in r['aliases'] and base not in r['initial_abilities']:
            raise ValueError('Unknown acquired species')
        if method=='land_capture' and not any(base in roster for roster in r['rosters'].values()):
            raise ValueError('Capture absent from opening land encounters: '+base)
        if nature not in set(re.findall(r'^#define\s+(NATURE_\w+)\s+\d+',text('include/constants/pokemon.h'),re.M))-{'NATURE_RANDOM','NATURE_MAY_SYNCHRONIZE'}:
            raise ValueError('Nature is not a possible acquisition roll')
        final=target or base
        if not isinstance(final,str) or resolve_species(final,r['aliases']) not in r['initial_abilities']:
            raise ValueError('Unknown evolution target')
        default={'nature':nature,'ability':r['initial_abilities'][resolve_species(final,r['aliases'])],
                 'evs':list(r['evs']),'ivs':[r['iv']]*6,'friendship':r['friendship'][resolve_species(base,r['aliases'])],
                 'pokerus':0,'pp_bonuses':0}
        if final!=base:
            proof=evolution_path(base,final,nature,r);proof['owned_mon_id']=ident;proofs.append(proof)
            if 'friendship' in proof:default['friendship']=proof['friendship']
        owned[ident]={'species':final,'acquired_species':base,'method':method,
                      'maps':[name for name,roster in r['rosters'].items() if base in roster]}
        defaults[ident]=default
    for slot,base in enumerate(scenario['candidate_roster']):
        acquire('starter-'+str(slot),base,scenario['opening_parameters']['natures'][base],
                (evolutions or {}).get(base),method='starter_pair')
    rows=[{'id':'capture-'+str(i),'species':base,'nature':(natures or {}).get(base,'NATURE_HARDY'),
           'evolved_to':(evolutions or {}).get(base)} for i,base in enumerate(captures)]
    extra=preparation.get('captures',[])
    if not isinstance(extra,list):raise ValueError('Captures must be a finite list')
    rows+=extra
    for row in rows:
        if not isinstance(row,dict) or set(row)-{'id','species','nature','evolved_to'}:
            raise ValueError('Capture contains an unsupported grant')
        acquire(row.get('id'),row.get('species'),row.get('nature','NATURE_HARDY'),row.get('evolved_to'))
    selected=preparation.get('selected',list(owned)[:6])
    if not isinstance(selected,list) or not 2<=len(selected)<=6 or len(set(selected))!=len(selected) or not set(selected)<=set(owned):
        raise ValueError('Select two to six distinct owned acquisitions')
    pay_wins=_finite_quantity(preparation.get('pay_day_wins',0),'Pay Day wins')
    if pay_wins:
        farmer=preparation.get('pay_day_farmer')
        if farmer not in owned:
            raise ValueError('Pay Day farming requires an owned Pay Day learner')
        from ec_moves import legal_moves_with_rom_union
        if 'MOVE_PAY_DAY' not in legal_moves_with_rom_union(owned[farmer]['species'])[0]:
            raise ValueError('Pay Day farming requires an owned Pay Day learner')
    pickups=preparation.get('pickup_items',{});sales=preparation.get('sell_pickup',{})
    purchases=preparation.get('purchases',{})
    for group in (pickups,sales,purchases):
        if not isinstance(group,dict):raise ValueError('Item plan must name finite quantities')
    if pickups:
        farmer=preparation.get('pickup_farmer')
        if farmer not in owned or 'ABILITY_PICKUP' not in r['abilities'].get(resolve_species(owned[farmer]['species'],r['aliases']),()):
            raise ValueError('Pickup farming needs an owned source-legal Pickup species')
    for item,count in pickups.items():
        _finite_quantity(count,item,6)
        if item not in farm['pickup']:raise ValueError('Item absent from level11-20 Pickup: '+item)
    sold_value=0
    for item,count in sales.items():
        _finite_quantity(count,item,6)
        if item not in {'ITEM_TINY_MUSHROOM','ITEM_BIG_MUSHROOM'} or count>pickups.get(item,0):
            raise ValueError('Sale lacks an acquired, sellable farming item')
        sold_value+=count*(farm['prices'][item]['base_price']//4)
    purchase_cost=0
    for item,count in purchases.items():
        _finite_quantity(count,item,max(0,6-r['kit'].get(item,0)))
        if item not in r['kit']:raise ValueError('Opening held stock has no first source: '+item)
        purchase_cost+=count*farm['prices'][item]['base_price']
    capture_cost=len(rows)*r['ball_price'];ev_reserve=len(selected)*r['ev_fee']
    # Captures in this finite plan precede farming; native cash can be capped,
    # but the useful six-member purchases cannot exceed the money cap.
    if capture_cost>r['cash']:raise ValueError('Capture prefix exceeds opening cash')
    cash=min(farm['money_cap'],r['cash']-capture_cost+pay_wins*r['cap']*5+sold_value)-purchase_cost-ev_reserve
    if cash<0:raise ValueError('Preparation purchases/EV edits lack finite funding')
    apply_owned_pokerus_plan(owned,defaults,preparation.get('pokerus',{}))
    resources=dict(Counter(mon['species'] for mon in owned.values()))
    resources.update(r['kit']);resources['ITEM_LEVELER']=1
    for item,count in pickups.items():resources[item]=resources.get(item,0)+count-sales.get(item,0)
    for item,count in purchases.items():resources[item]=resources.get(item,0)+count
    state=scenario['acquisition_states'][0]
    state.update(resources=resources,owned_monsters=owned,pokemon_defaults_by_id=defaults,
                 pokemon_defaults={},selected_owned_ids=selected,evolution_proofs=proofs,
                 pokerus_values=[0,0xFC,0xFD,0xFE],source_evidence=state['source_evidence']+farm['evidence'])
    scenario['candidate_roster']=[owned[ident]['species'] for ident in selected]
    scenario['party_size_range']=[2,len(selected)]
    scenario['opening_parameters'].update(captures=[row['species'] for row in rows],preparation=copy.deepcopy(preparation))
    scenario['producer']['arguments'].update(captures=list(captures),natures=copy.deepcopy(natures or {}),
                                           evolutions=copy.deepcopy(evolutions or {}),preparation=copy.deepcopy(preparation))
    scenario['scenario_id']+='-plan-'+hashlib.sha256(json.dumps(scenario['producer']['arguments'],sort_keys=True).encode()).hexdigest()[:12]
    scenario['economy'].update(purchased_poke_balls=len(rows),capture_cost=capture_cost,ev_fee_reserve=ev_reserve,
        capture_condition='every declared owned capture caught with one purchased ball; possible, not guaranteed',
        pay_day_wins=pay_wins,pay_day_money=pay_wins*r['cap']*5,pickup_sales=sold_value,
        held_item_purchase_cost=purchase_cost,cash_remaining_minimum=cash)
    scenario['preparation_scope']='Finite source-possible farming/boxing/infection plan; native acquisition receipts pending, maximal arsenal unresolved.'
    return scenario


def expanded_candidate_party(scenario, *, internal_fingerprint=None,internal_context=None):
    state=scenario['acquisition_states'][0];party=[];items=Counter(state['resources'])
    for ident in state['selected_owned_ids']:
        acquired=state['owned_monsters'][ident];species=acquired['species'];default=copy.deepcopy(state['pokemon_defaults_by_id'][ident])
        legal,_=ec_moves.legal_moves_with_rom_union(species)
        preferred=('MOVE_FAKE_OUT','MOVE_FOLLOW_ME','MOVE_NUZZLE','MOVE_SNARL','MOVE_GIGA_DRAIN','MOVE_FLAMETHROWER',
                   'MOVE_EARTH_POWER','MOVE_THUNDERBOLT','MOVE_CRUNCH','MOVE_BUG_BUZZ','MOVE_ICE_BEAM','MOVE_HELPING_HAND','MOVE_PROTECT')
        moves=list(dict.fromkeys([m for m in preferred if m in legal]+sorted(legal)))[:4]
        item=next((token for token in ('ITEM_LEFTOVERS','ITEM_FOCUS_SASH') if items[token]>0),'ITEM_NONE')
        if item!='ITEM_NONE':items[item]-=1
        party.append({'species':species,'item':item,'level':scenario['level_cap'],'moves':moves,**default,
            'availability':{'owned_mon_id':ident,**acquired,'naturally_rolled_nature':default['nature'],
                            'source_evidence':state['source_evidence']},
            'role':'Source-possible owned preparation hypothesis; no expert difficulty grade'})
    manifest={'encounter':scenario['trainer_id'],'availability_audit':{'scenario_id':scenario['scenario_id'],
        'legality_status':'proven','maximal_arsenal_status':'unresolved','source_evidence':state['source_evidence']},'party':party}
    certify_opening_manifest(scenario,manifest,internal_fingerprint=internal_fingerprint,internal_context=internal_context)
    return manifest

def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--out',type=Path,required=True)
    parser.add_argument('--generation',type=int,default=3);parser.add_argument('--first',type=int,default=0);parser.add_argument('--second',type=int,default=1)
    parser.add_argument('--captures',nargs='*',default=list(DEFAULT_CAPTURES))
    parser.add_argument('--evolve',action='append',default=[],metavar='BASE=TARGET')
    parser.add_argument('--preparation',type=Path,help='JSON finite farming/boxing/infection plan; use --captures alone to clear default captures')
    args=parser.parse_args()
    evolutions={}
    for entry in args.evolve:
        if entry.count('=')!=1:parser.error('--evolve requires BASE=TARGET')
        base,target=entry.split('=')
        if base in evolutions:parser.error('One acquired base cannot evolve into two targets')
        evolutions[base]=target
    preparation=json.loads(args.preparation.read_text()) if args.preparation else None
    scenarios=[build_opening_scenario(mode,generation=args.generation,first=args.first,second=args.second,captures=args.captures,evolutions=evolutions or None,preparation=preparation) for mode in ('easy','medium','hard')]
    data=dict(schema_version=2,source_generated=True,source_fingerprint=source_fingerprint(),maximal_arsenal_status='unresolved',
              opening_land_candidates=rules()['rosters'],scenarios=scenarios,candidate_parties={s['difficulty']:candidate_party(s) for s in scenarios})
    args.out.parent.mkdir(parents=True,exist_ok=True);args.out.write_text(json.dumps(data,indent=2,sort_keys=True)+'\n')
    print('Opening subsets exported; maximal arsenal remains unresolved.')
if __name__=='__main__':main()
