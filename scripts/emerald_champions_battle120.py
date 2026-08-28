#!/usr/bin/env python3
"""Generate and verify Battle 120, Danielle's evolved Dancer double."""
from __future__ import annotations
import argparse,json,re
from pathlib import Path
import verdant_battle_set_presets as presets,verdant_custom_teams as custom,verdant_doubles_conversion as doubles,verdant_team_polish as polish
ROOT=Path(__file__).resolve().parents[1];DESIGNS=ROOT/'docs/verdant_bespoke_battle_designs.json';LEDGER=ROOT/'docs/verdant_battle_experience_ledger.json';SEQUENCE=ROOT/'docs/verdant_battle_sequence.json';OS=ROOT/'docs/emerald_champions_battle_design_operating_system.json';CORPUS=ROOT/'docs/competitive_team_index.jsonl'
TEAM=[
 {'level':1,'species':'SPECIES_ORICORIO','item':'ITEM_CHARTI_BERRY','ability_slot':0,'spread':'SPREAD_31_IV_SPATK_SPEED_TIMID','moves':['MOVE_REVELATION_DANCE','MOVE_AIR_SLASH','MOVE_HELPING_HAND','MOVE_PROTECT']},
 {'level':2,'species':'SPECIES_VOLCARONA','item':'ITEM_LUM_BERRY','ability_slot':0,'spread':'SPREAD_31_IV_SPATK_SPEED_TIMID','moves':['MOVE_QUIVER_DANCE','MOVE_HEAT_WAVE','MOVE_BUG_BUZZ','MOVE_GIGA_DRAIN']},
 {'level':3,'species':'SPECIES_RAPIDASH','item':'ITEM_CHOICE_BAND','ability_slot':0,'spread':'SPREAD_31_IV_ATK_SPEED_JOLLY','moves':['MOVE_FLARE_BLITZ','MOVE_WILD_CHARGE','MOVE_JUMP_KICK','MOVE_POISON_JAB']},
 {'level':4,'species':'SPECIES_FROSLASS','item':'ITEM_FOCUS_SASH','ability_slot':1,'spread':'SPREAD_31_IV_SPATK_SPEED_TIMID','moves':['MOVE_FROST_BREATH','MOVE_ICY_WIND','MOVE_TAUNT','MOVE_DESTINY_BOND']}]
REFS=['showdown:gen9randomdoublesbattle:010','showdown:gen5randomdoublesbattle:030','smogon:gen5ou:012','showdown:gen4randombattle:003','showdown:gen8randomdoublesbattle:005']
NEXT={'index':121,'encounter_id':'BATTLE_121_LAVARIDGE_GYM_JACE_ELI','location':'LavaridgeTown_Gym_B1F','category':'optional buried Jace/Eli native pair','status':'next','strict_cap':40,'trainer_ids':['TRAINER_JACE','TRAINER_ELI'],'access_note':'Jace and Eli are the next paired buried B1F trainers; source identifies Jace as Eli’s double partner.'}
def design():
 return {'guide_order':120,'trainer_ids':['TRAINER_DANIELLE'],'status':'closed','strict_cap':40,'campaign_point':'Standalone buried upper-room Battle Girl at (5,2), cap 40, after Keegan.','runtime_branches':['One four-member double.'],'evolution_stage_fit':{'campaign_phase':'cap-40 evolved performance','effective_levels':'41-44','eligible_ratio':'4/4','mega_access':True,'status':'pass','reason':'All four are mature; no Mega is used.'},'manual_quality':10,'manual_difficulty':9.3,'corpus_review':{'reference_pool_size':1005,'full_team_candidates':[{'reference_id':r,'decision':'role selected; donor rejected','reason':'Dance, breaker or disruption role adapted without copying the roster.'} for r in REFS],'decision':'Five indexed references validate every role; the exact performance is local.'},'competitive_references':[{'reference_id':r,'adaptation':'Local performance role adapted from source evidence.'} for r in REFS],'ordering':{'lead':['SPECIES_ORICORIO','SPECIES_VOLCARONA'],'reserves':['SPECIES_RAPIDASH','SPECIES_FROSLASS'],'reason':'Quiver Dance creates the Dancer payoff; physical commitment and disruption form the second act.'},'team_intent':'Volcarona’s Quiver Dance can be copied by Dancer Oricorio. Rapidash changes damage category through a visible Choice lock; Froslass contests resets with Taunt, speed control and Destiny Bond.','primary_player_question':'Can the player stop or reset both dancers without losing the right physical and disruption answers for the second act?','intended_counterplay':'Taunt, Haze, Clear Smog, Unaware, phazing, Rock/Electric/Water/Ice/Ghost/Dark/Steel, Wide Guard, priority, hazards, burn and Choice exploitation all work. Three members share Rock pressure and the setup is public.','bespoke_ai':'Smart switching, partner awareness, HP awareness, Combo Setup and Speed Control recognize Dancer, Quiver Dance, Choice lock, Sash and Destiny Bond. No move or target is forced.','uniqueness':'Battle 14’s young dance recital receives an evolved payoff 106 encounters later. No weather, sun, room, Mega or Flannery species is spent.','story_logic':'Danielle’s beauty/strength aspiration now names the Dancer interaction and its counterplay.','reward_logic':'Ordinary EXP and prize money only.','campaign_reservations':{'spends':['evolved Dancer payoff'],'preserves':['Flannery sun/slow mode/Mega'],'repeat_rule':'Do not repeat Oricorio plus Quiver Dance.'},'author_self_check':{'strongest_part':'An early lesson returns as a late evolved payoff without repeating its roster.','weakest_link':'Rock pressure compresses three members; Charti, Sash, physical Rapidash and direct coverage prevent autopilot.'},'closure':'Battle 120 is source-closed at quality 10 and target 9.3: exact buried route, four legal levels 41-44, five references, truthful dialogue and broad counterplay. Runtime remains unplayed.'}
def ledger_entry():
 return {'index':120,'encounter_id':'BATTLE_120_LAVARIDGE_GYM_DANIELLE','identity':{'location':'LavaridgeTown_Gym_1F','category':'optional buried Battle Girl','format':'double','strict_cap':40,'memory_hook':'Volcarona’s Quiver Dance becomes Oricorio’s Dancer performance before a physical/disruption second act.'},'primary_player_question':'Can both dancers be stopped without losing second-act answers?','tempo':'Dancer setup lead into Choice Band and Sash disruption.','pressure_sources':['Dancer Oricorio','Quiver Dance Volcarona','Band Rapidash','Sash Froslass'],'intentional_opening':'Oricorio and Volcarona lead.','intentional_weakness':'Public setup, shared Rock pressure, no field/Mega/recovery loop.','first_loss_lesson':'Reset both dancers, then preserve physical and anti-Destiny-Bond lines.','revealed_information':['cap 40','double','levels 41-44','no reward'],'counterplay_classes':['Taunt/Haze/Unaware/phazing','Rock/Electric/Water/Ice/Ghost/Dark/Steel','Wide Guard/priority/hazards/burn'],'target_difficulty':9.3,'difficulty_rationale':'One copied setup creates severe pressure, but public typing and control answers remain broad.','tuning_knob':'Lower Froslass +4 first.','playtest_status':'static-pass-runtime-unplayed','novelty_tags':['lavaridge-gym','evolved-dancer','oricorio','volcarona','rapidash','froslass','quiver-dance','choice-band','destiny-bond','no-weather','no-mega'],'historic_reference_ids':REFS,'corpus_search':{'status':'complete-current-review','pool_size':1005,'selection':'Five indexed references.'},'author_self_check':{'strongest_part':'Long-range lesson payoff.','weakest_link':'Shared Rock seam.'}}
def payloads():
 d=json.loads(DESIGNS.read_text());d['designs']['BATTLE_120_LAVARIDGE_GYM_DANIELLE']=design();l=json.loads(LEDGER.read_text());l['entries']=[x for x in l['entries'] if x['index']!=120]+[ledger_entry()];l['entries'].sort(key=lambda x:x['index']);s=json.loads(SEQUENCE.read_text());
 for x in s['entries']:
  if x['index']==120:x.update({'category':'optional buried evolved-Dancer double','trainer_ids':['TRAINER_DANIELLE'],'access_note':'Danielle is the standalone buried trainer at (5,2) in the upper 1F room.'})
 s['entries']=[x for x in s['entries'] if x['index']!=121]+[dict(NEXT)];s['entries'].sort(key=lambda x:x['index'])
 for x in s['entries']:x['status']='closed' if x['index']<=120 else 'next' if x['index']==121 else 'queued'
 o=json.loads(OS.read_text());o['current_state'].update({'closed_encounters':120,'next_index':121,'next_encounter_id':NEXT['encounter_id'],'canonical_sequence_groups':121,'physical_encounter_groups':522,'unordered_physical_groups':401});return d,l,s,o
def verify_source():
 tr=(ROOT/'src/data/trainers.h').read_text();pa=(ROOT/'src/data/trainer_parties.h').read_text();b=doubles.trainer_blocks(tr)['TRAINER_DANIELLE'].group(0);a=[polish.parse_entry(e) for e in custom.party_entries(doubles.party_match(pa,doubles.party_name(b)).group(2))]
 if a!=TEAM or '.doubleBattle = TRUE' not in b or 'AI_FLAG_SPEED_CONTROL' not in b:raise SystemExit('FAIL Danielle source')
 dex=presets.LocalDex();slots=doubles.base_ability_slots()
 for m in TEAM:
  if [x for x in m['moves'] if x not in dex.legal_moves(m['species'])] or m['ability_slot']>=len(slots[m['species']]):raise SystemExit('FAIL Danielle legality')
 sec=(ROOT/'data/maps/LavaridgeTown_Gym_1F/scripts.inc').read_text().split('LavaridgeTown_Gym_1F_Text_DanielleIntro:',1)[1].split('LavaridgeTown_Gym_B1F_Text_JaceIntro:',1)[0]
 for cue in ('beautiful','Volcarona dances','Dancer copies','Choice Band','Focus Sash'):
  if cue not in sec:raise SystemExit(f'FAIL Danielle dialogue {cue}')
 for z in re.findall(r'\.string "([^"]*)"',sec):
  if len(z.replace('\\n','').replace('\\l','').replace('\\p','').replace('$',''))>36:raise SystemExit(f'FAIL Danielle width {z}')
 ids={json.loads(x)['reference_id'] for x in CORPUS.read_text().splitlines()}
 if any(x not in ids for x in REFS):raise SystemExit('FAIL Danielle refs')
def main():
 p=argparse.ArgumentParser();p.add_argument('--write',action='store_true');p.add_argument('--check',action='store_true');a=p.parse_args();ps=payloads();paths=(DESIGNS,LEDGER,SEQUENCE,OS);ts=[json.dumps(x,indent=2,ensure_ascii=False)+'\n' for x in ps]
 if a.write:
  for path,t in zip(paths,ts):path.write_text(t)
 if a.check:
  for path,t in zip(paths,ts):
   if path.read_text()!=t:raise SystemExit(f'FAIL stale {path.name}')
  verify_source();e=next(x for x in json.loads((ROOT/'docs/verdant_battle_guide.json').read_text())['entries'] if x['trainerId']=='TRAINER_DANIELLE')
  if e['designStatus']!='closed' or e['partySize']!=4:raise SystemExit('FAIL Danielle guide')
 print('PASS: Battle 120 Danielle evolved-Dancer double is source-closed')
if __name__=='__main__':main()
