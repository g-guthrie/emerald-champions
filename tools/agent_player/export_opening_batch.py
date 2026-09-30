#!/usr/bin/env python3
"""Export finite opening preparations and actual regional rival identities.

A source-identical female loadout remains separate coverage; reversed starter
order remains unplayed coverage. Nothing here certifies a win or a maximal pool.
"""
from __future__ import annotations
import argparse
import copy
import itertools
import json
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import battle_opening_arsenal as opening
import generate_battle_suite as suite_tools
import ec_moves
from doubles_policy import source_metadata

CAPTURES = ('SPECIES_EEVEE','SPECIES_WURMPLE','SPECIES_SCATTERBUG','SPECIES_PACHIRISU')
EVOLUTIONS = dict(zip(CAPTURES[:3], ('SPECIES_SYLVEON','SPECIES_DUSTOX','SPECIES_VIVILLON')))
SUPPORT = {
    'SPECIES_SYLVEON': ('NATURE_MODEST','ABILITY_PIXILATE',[252,0,4,252,0,0],
                       ['MOVE_HYPER_VOICE','MOVE_PSYSHOCK','MOVE_SHADOW_BALL','MOVE_MYSTICAL_FIRE'],'ITEM_CHOICE_SPECS'),
    'SPECIES_DUSTOX': ('NATURE_BOLD','ABILITY_UNAWARE',[252,0,0,252,4,0],
                      ['MOVE_QUIVER_DANCE','MOVE_BUG_BUZZ','MOVE_SLUDGE_BOMB','MOVE_PROTECT'],'ITEM_FOCUS_SASH'),
    'SPECIES_VIVILLON': ('NATURE_TIMID','ABILITY_COMPOUND_EYES',[4,0,0,252,0,252],
                        ['MOVE_SLEEP_POWDER','MOVE_HURRICANE','MOVE_BUG_BUZZ','MOVE_PROTECT'],'ITEM_NONE'),
    'SPECIES_PACHIRISU': ('NATURE_BOLD','ABILITY_VOLT_ABSORB',[252,0,252,0,4,0],
                         ['MOVE_FOLLOW_ME','MOVE_SUPER_FANG','MOVE_NUZZLE','MOVE_PROTECT'],'ITEM_LEFTOVERS'),
}


def c_function(text, name):
    match = re.search(r'(?m)^(?:static\s+)?(?:enum\s+\w+|[\w *]+)\s+'+name+r'\([^;{}]*\)\s*\{',text)
    if not match:
        raise ValueError('Native source function missing: '+name)
    depth=0
    for end in range(text.index('{',match.start()),len(text)):
        depth += (text[end]=='{')-(text[end]=='}')
        if depth==0:
            return text[match.start():end+1]
    raise ValueError('Unterminated native source function: '+name)


def run_c_probe(program):
    compiler=shutil.which('cc')
    if not compiler:
        raise ValueError('Existing host C compiler required for native source probes')
    (ROOT/'work').mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='opening-source-probe-',dir=ROOT/'work') as scratch:
        exe=Path(scratch)/'probe'
        subprocess.run([compiler,'-std=c11','-x','c','-','-o',str(exe)],input=program,text=True,
                       capture_output=True,check=True)
        return subprocess.run([str(exe)],text=True,capture_output=True,check=True).stdout


def raw_starter_sets(starters):
    text=(ROOT/'src/data/pokemon/emerald_champions_battle_sets.h').read_text()
    body=text.split('gEmeraldChampionsBattleSets[] =',1)[1].split('gEmeraldChampionsBattleSetRanges',1)[0]
    presets=re.findall(r'\.preset\s*=\s*\{(.*?)\n\s*\}\}',body,re.S)
    result={}
    for species in sorted({s for g,row in starters.items() if g for s in row}):
        match=re.search(r'\[EC_BATTLE_FORMAT_DOUBLES\]\['+species+r'\]\s*=\s*\{\.offset\s*=\s*(\d+),\s*\.count\s*=\s*(\d+)',text)
        if not match or int(match[2])==0:
            raise ValueError('No direct native starter set: '+species)
        preset=presets[int(match[1])]
        result[species]={field:re.search(r'\.'+field+r'\s*=\s*(\w+)',preset)[1]
                         for field in ('item','requiredItem','requiredMove','nature','ability')}
        result[species]['moves']=re.findall(r'MOVE_\w+',re.search(r'\.moves\s*=\s*\{([^}]+)',preset)[1])
        result[species]['evs']=[int(n) for n in re.findall(r'\d+',re.search(r'\.evs\s*=\s*\{([^}]+)',preset)[1])]
    return result


def regional_opening_sets(raw):
    """Execute the actual two native set functions, with raw-set lookup supplied.

    The stub records the requested scripted set. Slot ability validity and the
    scripted application contract are checked separately against current source.
    """
    text=(ROOT/'src/emerald_champions_story.c').read_text()
    functions='\n'.join(c_function(text,n) for n in ('GetOpeningStarterSet','ApplyEmeraldChampionsRegionalRivalSet'))
    literals=functions+'\n'+json.dumps(raw)
    groups={prefix:sorted(set(re.findall(prefix+r'\w+',literals)))
            for prefix in ('SPECIES_','MOVE_','ITEM_','ABILITY_','NATURE_')}
    enums='\n'.join('enum '+name+' {'+','.join(values)+'};' for name,values in
                    zip(('Species','Move','Item','Ability','Nature'),groups.values()))
    declarations=''
    for species,p in raw.items():
        declarations+='['+species+']={.moves={'+','.join(p['moves'])+'},.item='+p['item']+',.requiredItem='+p['requiredItem']+',.requiredMove='+p['requiredMove']+',.nature='+p['nature']+',.ability='+p['ability']+',.evs={'+','.join(map(str,p['evs']))+'}},\n'
    program='''#include <stdio.h>
#include <stdint.h>
typedef uint8_t u8; typedef uint32_t u32; typedef int bool32;
#define MAX_MON_MOVES 4
#define NUM_STATS 6
#define MON_DATA_SPECIES 0
'''+enums+'''
struct EmeraldChampionsBattleSet {enum Move moves[4];enum Item item,requiredItem;enum Move requiredMove;u8 nature;enum Ability ability;u8 evs[6];};
struct Pokemon {enum Species species;struct EmeraldChampionsBattleSet preset;};
static const struct EmeraldChampionsBattleSet raw[] = {
'''+declarations+'''};
static const struct EmeraldChampionsBattleSet *GetEmeraldChampionsRawBattleSet(enum Species s,u8 n){return &raw[s];}
static u32 GetMonData(struct Pokemon *p,int n){return p->species;}
static void ApplyEmeraldChampionsScriptedSet(struct Pokemon *p,const struct EmeraldChampionsBattleSet *s){p->preset=*s;}
'''+functions+'\nint main(void){ struct Pokemon p;\n'
    for species in raw:
        program+='p.species='+species+';ApplyEmeraldChampionsRegionalRivalSet(&p,0,1);printf("'+species+' %d %d %d %d %d %d %d %d %d %d %d %d %d\\n",p.preset.nature,p.preset.ability,p.preset.item,p.preset.moves[0],p.preset.moves[1],p.preset.moves[2],p.preset.moves[3],p.preset.evs[0],p.preset.evs[1],p.preset.evs[2],p.preset.evs[3],p.preset.evs[4],p.preset.evs[5]);\n'
    output=run_c_probe(program+'return 0;}')
    results={}
    for line in output.splitlines():
        species,*values=line.split(); v=list(map(int,values))
        results[species]={'nature':groups['NATURE_'][v[0]],'ability':groups['ABILITY_'][v[1]],'item':groups['ITEM_'][v[2]],
                          'moves':[groups['MOVE_'][i] for i in v[3:7]],'evs':v[7:]}
    return results


def native_branch_rows(starters):
    """Execute the native paired-starter index/branch and generation lookup."""
    story=(ROOT/'src/emerald_champions_story.c').read_text()
    chooser=(ROOT/'src/starter_choose.c').read_text()
    table=re.search(r'static const enum Species sStarterMons\[\]\[STARTER_MON_COUNT\]\s*=\s*\{.*?\n\};',chooser,re.S)[0]
    names=sorted({s for row in starters.values() for s in row})
    program='''#include <stdio.h>
#include <stdint.h>
typedef uint16_t u16;typedef int bool32;
#define TRUE 1
#define FALSE 0
#define STARTER_MON_COUNT 3
#define ARRAY_COUNT(a) (sizeof(a)/sizeof((a)[0]))
#define VAR_STARTER_MON 0
#define VAR_EC_SECOND_STARTER 1
#define VAR_EC_OPENING_STATE 2
'''+f'#define EC_OPENING_PAIR_GRANTED {opening.number("include/constants/emerald_champions.h","EC_OPENING_PAIR_GRANTED")}\n#define EC_OPENING_RESCUE_WON {opening.number("include/constants/emerald_champions.h","EC_OPENING_RESCUE_WON")}\n'+'enum Species {'+','.join(names)+'};\nstatic u16 vars[3],gSpecialVar_Result;\nstatic u16 VarGet(u16 n){return vars[n];}\n'+table+'\n'+c_function(chooser,'GetStarterPokemonForGeneration')+'\n'
    program+='\n'.join(c_function(story,n) for n in ('GetEmeraldChampionsSecondStarterIndex','HasEmeraldChampionsSecondStarter','GetEmeraldChampionsRivalStarterIndex','BufferEmeraldChampionsRivalBranch'))
    program+='\nint main(void){for(int g=1;g<=9;g++)for(int a=0;a<3;a++)for(int b=0;b<3;b++)if(a!=b){vars[0]=a;vars[1]=b+1;vars[2]=EC_OPENING_RESCUE_WON;BufferEmeraldChampionsRivalBranch();printf("%d %d %d %d %d %d\\n",g,a,b,GetEmeraldChampionsRivalStarterIndex(),gSpecialVar_Result,GetStarterPokemonForGeneration(GetEmeraldChampionsRivalStarterIndex(),g));}return 0;}'
    return [dict(generation=v[0],first=v[1],second=v[2],unchosen_index=v[3],branch=v[4],unchosen_species=names[v[5]])
            for line in run_c_probe(program).splitlines() if (v:=list(map(int,line.split())))]


def expected_opponent(scenario, opponents, regional_sets, *, internal_rules=None):
    r=internal_rules if internal_rules is not None else opening.rules()
    params=scenario['opening_parameters']; g=params['generation']; a=params['first']; b=params['second']
    unchosen=3-a-b; species=r['starters'][g][unchosen]
    dossier=copy.deepcopy(opponents[scenario['trainer_id']]); team=dossier['team']
    hoenn={'SPECIES_TREECKO','SPECIES_TORCHIC','SPECIES_MUDKIP'}
    slots=[m for m in team if m['species'] in hoenn]
    if len(slots)!=1 or dossier['class']!='rival':
        raise ValueError('Opening replacement owner or starter stage changed')
    member=slots[0];old=member['species'];applied=old!=species
    if applied:
        member.update(copy.deepcopy(regional_sets[species]));member['species']=species;member['ivs']=[31]*6
    difficulty=scenario['difficulty']; label={'easy':'EASY','medium':'NORMAL','hard':'HARD'}[difficulty]
    source=(ROOT/'src/difficulty.c').read_text()
    # Execute the native level function itself rather than mirroring it.
    from battle_campaign_arsenal import _level_rows
    levels=_level_rows(source,{'EASY':0,'NORMAL':1,'HARD':2}[label],scenario['level_cap'],
                       tuple(mon['level_offset'] for mon in team),0)
    for mon,level in zip(team,levels):
        mon['level']=level
    combat={k:dossier[k] for k in ('class','ai_profile','ai_extra','strategy','tactics','field','mega_slots')}
    combat['difficulty']=difficulty;combat['team']=team
    return dict(trainer_id=scenario['trainer_id'],generation=g,unchosen_index=unchosen,unchosen_species=species,
                branch=(unchosen+2)%3,regional_replacement=dict(slot=member['slot'],from_species=old,to_species=species,applied=applied),
                team=team,combat_sha256=suite_tools.digest_bytes(suite_tools.canonical(combat)),
                source_evidence=[opening.source_evidence(ROOT/p) for p in ('src/starter_choose.c','src/battle_setup.c','src/emerald_champions_story.c','src/emerald_champions_battle_sets.c','src/data/pokemon/emerald_champions_battle_sets.h','src/difficulty.c')])


def preparation(scenario, raw, context, fingerprint):
    manifest=opening.candidate_party(scenario,internal_fingerprint=fingerprint,internal_context=context)
    facts=source_metadata();used={p[4] for p in SUPPORT.values() if p[4]!='ITEM_NONE'}
    for mon in manifest['party']:
        species=mon['species']
        if species in SUPPORT:
            nature,ability,evs,moves,item=SUPPORT[species]
            mon.update(nature=nature,ability=ability,evs=evs[:],moves=moves[:],item=item)
        else:
            preset=raw[species]; mode='PHYSICAL' if preset['evs'][1]>preset['evs'][3] else 'SPECIAL'
            legal,_=ec_moves.legal_moves_with_rom_union(species)
            preferred=[m for m in preset['moves'] if m in legal]
            attacks=sorted((m for m in legal if facts['moves'].get(m,{}).get('category')==mode
                            and facts['moves'][m].get('power',0)>0),
                           key=lambda m:(facts['moves'][m]['type'] in ec_moves.TYPES[species],facts['moves'][m]['power']),reverse=True)
            moves=list(dict.fromkeys(preferred+attacks+(['MOVE_PROTECT'] if 'MOVE_PROTECT' in legal else [])))[:4]
            physical_item='ITEM_CHOICE_BAND' if mode=='PHYSICAL' and all(facts['moves'][m]['category']=='PHYSICAL' for m in moves) else 'ITEM_EVIOLITE'
            item=physical_item if physical_item not in used else 'ITEM_NONE'
            if item!='ITEM_NONE':used.add(item)
            ability=preset['ability']
            if ability in {'ABILITY_CHLOROPHYLL','ABILITY_SOLAR_POWER'}:
                fallback='ABILITY_OVERGROW' if species in context['rules']['starters'][scenario['opening_parameters']['generation']][:1] else 'ABILITY_BLAZE'
                if fallback in context['rules']['abilities'].get(species,()):ability=fallback
            mon.update(ability=ability,moves=moves,item=item,evs=preset['evs'][:])
        mon['role']='Legal preparation hypothesis; native play and strategy search still required'
    opening.certify_opening_manifest(scenario,manifest,internal_fingerprint=fingerprint,internal_context=context)
    return manifest


def source_stat_identity():
    paths=set(suite_tools.build_inputs.build_inputs())
    paths.update(ROOT/p for p in suite_tools.HARNESS_INPUTS)
    return {str(p):(stat.st_dev,stat.st_ino,stat.st_size,stat.st_mtime_ns,stat.st_ctime_ns)
            for p in sorted(paths) if p.is_file() and (stat:=p.stat())}


def export_batch(output, *, all_genders=False):
    output=Path(output);output.mkdir(parents=True,exist_ok=False)
    # One fresh native/harness tree hash for this immutable export. Source
    # grants/field/defaults are recomputed for every certificate via producer.
    source_state=source_stat_identity()
    inputs=suite_tools.input_hashes();fp=suite_tools.digest_bytes(suite_tools.canonical(inputs))
    r=opening.rules();context={'rules':r,'source_evidence':[opening.source_evidence(ROOT/p) for p in opening.SOURCE_PATHS]}
    context['source_evidence'].extend(opening.source_evidence(p) for p in sorted((ROOT/'src/data/pokemon/species_info').glob('*.h')))
    raw=raw_starter_sets(r['starters']);regional=regional_opening_sets(raw)
    branch_rows=native_branch_rows(r['starters']);native={(p['generation'],p['first'],p['second']):p for p in branch_rows}
    opponents={p['trainer_id']:p for p in suite_tools.opponent_catalogue()}
    scenarios=[];catalogue=[];pending=[];configs=[]
    def save(name,data):
        (output/name).write_text(json.dumps(data,indent=2,sort_keys=True)+'\n')
    for generation in range(1,10):
        for a,b in itertools.combinations(range(3),2):
            for difficulty in ('easy','medium','hard'):
                starters=[r['starters'][generation][i] for i in (a,b)]
                natures={s:raw[s]['nature'] for s in starters}
                natures.update({base:SUPPORT[target][0] for base,target in EVOLUTIONS.items()})
                natures['SPECIES_PACHIRISU']=SUPPORT['SPECIES_PACHIRISU'][0]
                candidates=[]
                for gender in ('male','female'):
                    scenario=opening._opening_scenario(difficulty,generation=generation,first=a,second=b,captures=CAPTURES,natures=natures,
                        player_gender=gender,evolutions=EVOLUTIONS,_fingerprint=fp,_context=context)
                    identity=expected_opponent(scenario,opponents,regional,internal_rules=r)
                    if identity['unchosen_species']!=native[generation,a,b]['unchosen_species'] or identity['branch']!=native[generation,a,b]['branch']:
                        raise ValueError('Producer differs from executed native paired-starter branch')
                    scenario["expected_opponent"]=identity
                    candidates.append((gender,scenario,identity))
                same=candidates[0][2]['combat_sha256']==candidates[1][2]['combat_sha256']
                for gender,scenario,identity in candidates:
                    ident=f'gen{generation}-pair{a}{b}-{difficulty}-{gender}'
                    included=gender=='male' or all_genders or not same
                    row={'context_id':ident,'generation':generation,'first':a,'second':b,'difficulty':difficulty,'player_gender':gender,
                         'expected_opponent':identity,'source_gender_loadout_equivalent':same,'native_execution':'pending','included':included}
                    if included:
                        party=preparation(scenario,raw,context,fp)
                        row['party_file']=ident+'-party.json';save(row['party_file'],party)
                        scenarios.append(scenario);row['scenario_id']=scenario['scenario_id']
                        row['config_file']=ident+'-evaluate.json';configs.append(row)
                    catalogue.append(row)
                    pending.append({'generation':generation,'first':b,'second':a,'difficulty':difficulty,'player_gender':gender,
                                    'unchosen_species':identity['unchosen_species'],'coverage':'reversed player party order; native validation pending'})
    save('index.json',{'schema_version':2,'source_generated':True,'source_fingerprint':fp,'scenarios':scenarios})
    suite=suite_tools.generate(output/'index.json',internal_inputs=inputs);save('suite.json',suite)
    puzzle_ids={(p['scenario']['trainer_id'],p['scenario']['scenario_id'],p['scenario']['difficulty']):p['puzzle_id'] for p in suite['puzzles']}
    save('policy.json',{'schema_version':1,'kind':'tactical'})
    for row in configs:
        save(row['config_file'],{'schema_version':1,'suite':'suite.json','puzzle_id':puzzle_ids[row['expected_opponent']['trainer_id'],row['scenario_id'],row['difficulty']],
            'party':row['party_file'],'policy':'policy.json','build_dir':str(ROOT),'discovery_seeds':[7],
            'evaluation_seeds':[17,71,313],'budgets':{'max_decisions':120,'timeout_seconds':900,'step_timeout_seconds':120}})
    report={'schema_version':1,'kind':'opening_variant_export','source_fingerprint':fp,'config_count':len(configs),
            'primary_male_contexts':81,'canonical_gender_contexts':len(catalogue),'reversed_order_pending':pending,
            'contexts':catalogue,'native_branch_probe_rows':branch_rows,
            'unknowns':['Native execution of exported configurations','Player reverse-order validation','Gender-dependent native timing/RNG',
                        'Maximal opening arsenal','Natural-roll and first-ball acquisition outcomes are possible rather than guaranteed']}
    if source_stat_identity()!=source_state:
        raise ValueError('Source changed during batch export; regenerate after source freeze')
    save('catalogue.json',report)
    return report


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--all-genders',action='store_true',help='Export same-source-loadout female contexts as additional native checks')
    args=parser.parse_args();report=export_batch(args.output,all_genders=args.all_genders)
    print(json.dumps({k:report[k] for k in ('config_count','primary_male_contexts','canonical_gender_contexts','source_fingerprint')},indent=2))

if __name__=='__main__':main()
