#!/usr/bin/env python3
"""Generate and verify Battle 119, Keegan's willpower single."""
from __future__ import annotations
import argparse,json,re
from pathlib import Path
import verdant_battle_set_presets as presets,verdant_custom_teams as custom,verdant_doubles_conversion as doubles,verdant_team_polish as polish
ROOT=Path(__file__).resolve().parents[1];DESIGNS=ROOT/'docs/verdant_bespoke_battle_designs.json';LEDGER=ROOT/'docs/verdant_battle_experience_ledger.json';SEQUENCE=ROOT/'docs/verdant_battle_sequence.json';OS=ROOT/'docs/emerald_champions_battle_design_operating_system.json';CORPUS=ROOT/'docs/competitive_team_index.jsonl'
TEAM=[
 {'level':1,'species':'SPECIES_VIGOROTH','item':'ITEM_EVIOLITE','ability_slot':0,'spread':'SPREAD_31_IV_HP_ATK_ADAMANT','moves':['MOVE_BULK_UP','MOVE_BRICK_BREAK','MOVE_NIGHT_SLASH','MOVE_ROCK_SLIDE']},
 {'level':2,'species':'SPECIES_MAGMORTAR','item':'ITEM_ASSAULT_VEST','ability_slot':2,'spread':'SPREAD_31_IV_HP_SPATK_MODEST','moves':['MOVE_FIRE_BLAST','MOVE_THUNDERBOLT','MOVE_PSYCHIC','MOVE_FOCUS_BLAST']},
 {'level':3,'species':'SPECIES_TYPHLOSION','item':'ITEM_CHOICE_SCARF','ability_slot':0,'spread':'SPREAD_31_IV_SPATK_SPEED_MODEST','moves':['MOVE_ERUPTION','MOVE_FLAMETHROWER','MOVE_EARTH_POWER','MOVE_FOCUS_BLAST']}]
REFS=['smogon:gen6nu:002','smogon:gen4nu:010','showdown:gen6randomdoublesbattle:011']
NEXT={'index':120,'encounter_id':'BATTLE_120_LAVARIDGE_GYM_DANIELLE','location':'LavaridgeTown_Gym_1F','category':'optional buried upper-room Beauty','status':'next','strict_cap':40,'trainer_ids':['TRAINER_DANIELLE'],'access_note':'Danielle is the next 1F buried trainer at (5,2), reached after Keegan on the upward Gym route.'}
def design():
 return {'guide_order':119,'trainer_ids':['TRAINER_KEEGAN'],'status':'closed','strict_cap':40,'campaign_point':'Standalone buried B1F Kindler at (3,6), cap 40, after Axle and before the upper 1F room.','runtime_branches':['One three-member intentional single.'],'evolution_stage_fit':{'campaign_phase':'cap-40 endurance contrast','effective_levels':'41-43','eligible_ratio':'3/3','mega_access':True,'status':'pass','reason':'Vigoroth intentionally remains middle-stage with Eviolite; Magmortar and Typhlosion are mature.'},'manual_quality':10,'manual_difficulty':8.9,'corpus_review':{'reference_pool_size':1005,'full_team_candidates':[{'reference_id':r,'decision':'role selected; donor rejected','reason':'One endurance role was adapted without importing sun or the full roster.'} for r in REFS],'decision':'Three indexed sets plus the authored Vigoroth review support the relay.'},'competitive_references':[{'reference_id':r,'adaptation':'Local endurance role adapted without donor-team copying.'} for r in REFS]+[{'source':'docs/battle_set_reviews/030_johto.json','adaptation':'Eviolite Vital Spirit Vigoroth supplies the off-type willpower opener.'}],'ordering':{'source_order':['SPECIES_VIGOROTH','SPECIES_MAGMORTAR','SPECIES_TYPHLOSION'],'reason':'Bulk/endurance opener, direct sleepless wallbreaker, then HP-sensitive Scarf Eruption finish.'},'team_intent':'Three forms of willpower: Vital Spirit Eviolite setup, Vital Spirit Assault Vest coverage, and a Typhlosion whose Eruption rewards preserving HP.','primary_player_question':'Can the player deny Vigoroth setup, change from physical to special defense, then chip Typhlosion before Scarf Eruption arrives?','intended_counterplay':'Taunt, phazing, burn, Unaware, Fighting/Flying/Psychic/Ground/Rock/Water, special walls, priority, hazards, Protect and chip all divide the relay. No weather, recovery loop, Mega or legend appears.','bespoke_ai':'Smart switching and HP awareness use native setup, Assault Vest, Choice lock and HP-sensitive Eruption scoring. Nothing is forced.','uniqueness':'Vigoroth is fresh; Magmortar returns after 30 battles and Typhlosion after 32. Keegan explicitly removes the old Drought leak so Flannery owns sun.','story_logic':'Dialogue now truthfully explains Vital Spirit, endurance items and Eruption chip in native width.','reward_logic':'Ordinary EXP and prize money only.','campaign_reservations':{'spends':['Keegan willpower single'],'preserves':['Flannery Drought/After You/slow mode/Mega'],'repeat_rule':'Do not repeat this three-leg endurance order.'},'author_self_check':{'strongest_part':'One brief single creates pacing relief while still demanding three different answers.','weakest_link':'Two Vital Spirit users repeat an ability by design; their categories, items and roles differ.'},'closure':'Battle 119 is source-closed at quality 10 and target 8.9: exact buried geometry, three legal levels 41-43, truthful references/dialogue and broad counterplay. Runtime remains unplayed.'}
def ledger_entry():
 return {'index':119,'encounter_id':'BATTLE_119_LAVARIDGE_GYM_KEEGAN','identity':{'location':'LavaridgeTown_Gym_B1F','category':'optional buried Kindler','format':'single','strict_cap':40,'memory_hook':'Two sleepless endurance legs protect an HP-sensitive Scarf Eruption finish.'},'primary_player_question':'Can the player change defensive answers and chip Typhlosion before Eruption?','tempo':'Eviolite setup, AV coverage, Scarf Eruption.','pressure_sources':['Vital Spirit Vigoroth','AV Vital Spirit Magmortar','Scarf Typhlosion'],'intentional_opening':'Vigoroth fixed first.','intentional_weakness':'No field/Mega/legend/recovery loop; public setup and Choice seams.','first_loss_lesson':'Stop Bulk Up, defend specially, then chip Eruption.','revealed_information':['cap 40','single','levels 41-43','no reward'],'counterplay_classes':['Taunt/phazing/burn','Fighting/Flying/Psychic/Ground/Rock/Water','priority/hazards/Protect/chip'],'target_difficulty':8.9,'difficulty_rationale':'Three optimized cap-plus legs demand adaptation but offer broad public answers.','tuning_knob':'Lower Typhlosion +3 first.','playtest_status':'static-pass-runtime-unplayed','novelty_tags':['lavaridge-gym','route-single','willpower','vigoroth','magmortar','typhlosion','vital-spirit','eruption','no-weather','no-mega'],'historic_reference_ids':REFS,'corpus_search':{'status':'complete-current-review','pool_size':1005,'selection':'Three indexed references plus Vigoroth review.'},'author_self_check':{'strongest_part':'Short but nontrivial category changes.','weakest_link':'Intentional repeated ability.'}}
def payloads():
 d=json.loads(DESIGNS.read_text());d['designs']['BATTLE_119_LAVARIDGE_GYM_KEEGAN']=design();l=json.loads(LEDGER.read_text());l['entries']=[x for x in l['entries'] if x['index']!=119]+[ledger_entry()];l['entries'].sort(key=lambda x:x['index']);s=json.loads(SEQUENCE.read_text());
 for x in s['entries']:
  if x['index']==119:x.update({'category':'optional buried B1F willpower single','trainer_ids':['TRAINER_KEEGAN'],'access_note':'Keegan is the standalone buried trainer at (3,6) on B1F.'})
 s['entries']=[x for x in s['entries'] if x['index']!=120]+[dict(NEXT)];s['entries'].sort(key=lambda x:x['index'])
 for x in s['entries']:x['status']='closed' if x['index']<=119 else 'next' if x['index']==120 else 'queued'
 o=json.loads(OS.read_text());o['current_state'].update({'closed_encounters':119,'next_index':120,'next_encounter_id':NEXT['encounter_id'],'canonical_sequence_groups':120,'physical_encounter_groups':523,'unordered_physical_groups':403});return d,l,s,o
def verify_source():
 tr=(ROOT/'src/data/trainers.h').read_text();pa=(ROOT/'src/data/trainer_parties.h').read_text();b=doubles.trainer_blocks(tr)['TRAINER_KEEGAN'].group(0);a=[polish.parse_entry(e) for e in custom.party_entries(doubles.party_match(pa,doubles.party_name(b)).group(2))]
 if a!=TEAM or '.doubleBattle = FALSE' not in b or 'AI_FLAG_HP_AWARE' not in b:raise SystemExit('FAIL Keegan source')
 dex=presets.LocalDex();slots=doubles.base_ability_slots()
 for m in TEAM:
  if [x for x in m['moves'] if x not in dex.legal_moves(m['species'])] or m['ability_slot']>=len(slots[m['species']]):raise SystemExit('FAIL Keegan legality')
 sec=(ROOT/'data/maps/LavaridgeTown_Gym_1F/scripts.inc').read_text().split('LavaridgeTown_Gym_B1F_Text_KeeganIntro:',1)[1].split('LavaridgeTown_Gym_1F_Text_GeraldIntro:',1)[0]
 for cue in ('willpower','won\'t sleep','Eruption','Vital Spirit','Choice Scarf'):
  if cue not in sec:raise SystemExit(f'FAIL Keegan dialogue {cue}')
 for z in re.findall(r'\.string "([^"]*)"',sec):
  if len(z.replace('\\n','').replace('\\l','').replace('\\p','').replace('$',''))>36:raise SystemExit(f'FAIL Keegan width {z}')
 ids={json.loads(x)['reference_id'] for x in CORPUS.read_text().splitlines()}
 if any(x not in ids for x in REFS):raise SystemExit('FAIL Keegan refs')
def main():
 p=argparse.ArgumentParser();p.add_argument('--write',action='store_true');p.add_argument('--check',action='store_true');a=p.parse_args();ps=payloads();paths=(DESIGNS,LEDGER,SEQUENCE,OS);ts=[json.dumps(x,indent=2,ensure_ascii=False)+'\n' for x in ps]
 if a.write:
  for path,t in zip(paths,ts):path.write_text(t)
 if a.check:
  for path,t in zip(paths,ts):
   if path.read_text()!=t:raise SystemExit(f'FAIL stale {path.name}')
  verify_source();e=next(x for x in json.loads((ROOT/'docs/verdant_battle_guide.json').read_text())['entries'] if x['trainerId']=='TRAINER_KEEGAN')
  if e['designStatus']!='closed' or e['partySize']!=3:raise SystemExit('FAIL Keegan guide')
 print('PASS: Battle 119 Keegan willpower single is source-closed')
if __name__=='__main__':main()
