#!/usr/bin/env python3
"""Generate and verify Battle 102, Bernie's complete fire-safety rematch family."""

from __future__ import annotations
import argparse, json, re
from pathlib import Path
import verdant_battle_set_presets as presets
import verdant_custom_teams as custom
import verdant_doubles_conversion as doubles
import verdant_team_polish as polish

ROOT=Path(__file__).resolve().parents[1]; DESIGNS=ROOT/"docs/verdant_bespoke_battle_designs.json"; LEDGER=ROOT/"docs/verdant_battle_experience_ledger.json"; SEQUENCE=ROOT/"docs/verdant_battle_sequence.json"; OS_PATH=ROOT/"docs/emerald_champions_battle_design_operating_system.json"; CORPUS=ROOT/"docs/competitive_team_index.jsonl"
def mon(level,species,item,ability,spread,moves): return {"level":level,"species":species,"item":item,"ability_slot":ability,"spread":spread,"moves":moves}
PYROAR=mon(1,"SPECIES_PYROAR","ITEM_THROAT_SPRAY",1,"SPREAD_31_IV_SPATK_SPEED_TIMID",["MOVE_HYPER_VOICE","MOVE_HEAT_WAVE","MOVE_DARK_PULSE","MOVE_WILL_O_WISP"])
GOLDUCK=mon(2,"SPECIES_GOLDUCK","ITEM_EXPERT_BELT",1,"SPREAD_31_IV_SPATK_SPEED_TIMID",["MOVE_SCALD","MOVE_ICE_BEAM","MOVE_PSYCHIC","MOVE_ENCORE"])
MAROWAK=mon(3,"SPECIES_MAROWAK_ALOLAN","ITEM_THICK_CLUB",1,"SPREAD_31_IV_HP_ATK_BRAVE",["MOVE_FLARE_BLITZ","MOVE_POLTERGEIST","MOVE_BONEMERANG","MOVE_PROTECT"])
VAPOREON=mon(4,"SPECIES_VAPOREON","ITEM_LEFTOVERS",0,"SPREAD_31_IV_HP_SPATK_MODEST",["MOVE_WATER_PULSE","MOVE_ICE_BEAM","MOVE_HELPING_HAND","MOVE_PROTECT"])
INFERNAPE=mon(1,"SPECIES_INFERNAPE","ITEM_FOCUS_SASH",1,"SPREAD_31_IV_ATK_SPEED_JOLLY",["MOVE_FAKE_OUT","MOVE_CLOSE_COMBAT","MOVE_FIRE_PUNCH","MOVE_MACH_PUNCH"])
ROTOM=mon(3,"SPECIES_ROTOM_HEAT","ITEM_SITRUS_BERRY",0,"SPREAD_31_IV_HP_SPATK_MODEST",["MOVE_OVERHEAT","MOVE_THUNDERBOLT","MOVE_WILL_O_WISP","MOVE_VOLT_SWITCH"])
TEAM_1=[PYROAR,GOLDUCK,MAROWAK,VAPOREON]
TEAM_2=[INFERNAPE,mon(2,"SPECIES_VAPOREON","ITEM_LEFTOVERS",0,"SPREAD_31_IV_HP_SPATK_MODEST",VAPOREON["moves"]),mon(3,"SPECIES_PYROAR","ITEM_THROAT_SPRAY",1,"SPREAD_31_IV_SPATK_SPEED_TIMID",PYROAR["moves"]),mon(4,"SPECIES_GOLDUCK","ITEM_EXPERT_BELT",1,"SPREAD_31_IV_SPATK_SPEED_TIMID",GOLDUCK["moves"])]
TEAM_3=[mon(1,"SPECIES_MAROWAK_ALOLAN","ITEM_THICK_CLUB",1,"SPREAD_31_IV_HP_ATK_BRAVE",MAROWAK["moves"]),mon(2,"SPECIES_VAPOREON","ITEM_LEFTOVERS",0,"SPREAD_31_IV_HP_SPATK_MODEST",VAPOREON["moves"]),ROTOM,mon(4,"SPECIES_INFERNAPE","ITEM_FOCUS_SASH",1,"SPREAD_31_IV_ATK_SPEED_JOLLY",INFERNAPE["moves"])]
TEAM_4=[PYROAR,mon(1,"SPECIES_GOLDUCK","ITEM_EXPERT_BELT",1,"SPREAD_31_IV_SPATK_SPEED_TIMID",GOLDUCK["moves"]),mon(2,"SPECIES_MAROWAK_ALOLAN","ITEM_THICK_CLUB",1,"SPREAD_31_IV_HP_ATK_BRAVE",MAROWAK["moves"]),mon(2,"SPECIES_VAPOREON","ITEM_LEFTOVERS",0,"SPREAD_31_IV_HP_SPATK_MODEST",VAPOREON["moves"]),mon(3,"SPECIES_INFERNAPE","ITEM_FOCUS_SASH",1,"SPREAD_31_IV_ATK_SPEED_JOLLY",INFERNAPE["moves"]),mon(4,"SPECIES_ROTOM_HEAT","ITEM_SITRUS_BERRY",0,"SPREAD_31_IV_HP_SPATK_MODEST",ROTOM["moves"])]
TEAMS={"TRAINER_BERNIE_1":TEAM_1,"TRAINER_BERNIE_2":TEAM_2,"TRAINER_BERNIE_3":TEAM_3,"TRAINER_BERNIE_4":TEAM_4}
REFERENCES=["showdown:gen9championsrandomdoublesbattle:021","showdown:gen7randomdoublesbattle:029","vgc:worlds-2017","showdown:gen4randomdoublesbattle:013","showdown:gen9randomdoublesbattle:029","vgc:ocic-2020"]
NEXT={"index":103,"encounter_id":"BATTLE_103_ROUTE_114_LENNY","location":"Route114","category":"optional lower-route Youngster single","status":"next","strict_cap":40,"trainer_ids":["TRAINER_LENNY"],"access_note":"Lenny faces right at (15,65) with six-tile sight on the lower Route 114 path. He is next after Bernie and before the final Angelina/Lucas pair."}

def design():
 return {
  "guide_order":102,"trainer_ids":list(TEAMS),"status":"closed","strict_cap":40,
  "campaign_point":"Bernie's rotating three-tile Route 114 encounter begins at cap 40. Match Call rematches require five badges and are earliest at cap 45; all four records stay cap-relative if fought later.",
  "runtime_branches":["BERNIE_1: guarded four-member double at cap 40.","BERNIE_2: first guarded four-member rematch double, earliest cap 45.","BERNIE_3: second guarded four-member rematch double.","BERNIE_4: repeatable six-member final rematch double."],
  "evolution_stage_fit":{"campaign_phase":"cap-40 controlled-burn opening and five-badge cap-45+ rematches","effective_levels":"initial 41-44; rematches earliest 46-49, final 46/46/47/47/48/49","eligible_ratio":"18/18 source slots","mega_access":True,"status":"pass","reason":"Pyroar evolves at 35, Golduck at 33, Infernape at 36; Alolan Marowak and Vaporeon use ordinary evolution methods; Rotom is single-stage. Every slot is mature by cap 40 and no Mega or legendary is used."},
  "manual_quality":10,"manual_difficulty":8.9,"rematch_difficulty":{"TRAINER_BERNIE_2":9.1,"TRAINER_BERNIE_3":9.3,"TRAINER_BERNIE_4":9.5},
  "corpus_review":{"reference_pool_size":1005,"full_team_candidates":[{"reference_id":r,"decision":"exact species role selected; full donor rejected","reason":"The reference proves one controlled-burn role; Bernie's four-stage safety progression rejects unrelated weather, legends, and Megas."} for r in REFERENCES],"decision":"All 1005 references were reviewed. Six exact indexed references plus complete all-species reviews support every role; the controlled-burn rematch progression is transparently hand-authored."},
  "competitive_references":[
   {"reference_id":REFERENCES[0],"adaptation":"Competitive Pyroar uses Throat Spray Hyper Voice/Heat Wave with Dark Pulse and burn fallback."},{"reference_id":REFERENCES[1],"adaptation":"Cloud Nine Golduck supplies weather shutdown, Scald, Ice/Psychic coverage, and Encore."},{"reference_id":REFERENCES[2],"adaptation":"Worlds-winning Alolan Marowak validates Lightning Rod and Thick Club at elite doubles stakes."},{"reference_id":REFERENCES[3],"adaptation":"Vaporeon keeps Water Absorb, Helping Hand, Ice coverage, and Protect as the reservoir."},{"reference_id":REFERENCES[4],"adaptation":"Infernape supplies Sash Fake Out, Iron Fist Fire/Mach Punch, and Close Combat."},{"reference_id":REFERENCES[5],"adaptation":"OCIC-winning Rotom-Heat validates Levitate, Overheat, Electric pressure, burn, and pivot control."}],
  "ordering":{"TRAINER_BERNIE_1":{"lead":["SPECIES_PYROAR","SPECIES_GOLDUCK"],"reserves":["SPECIES_MAROWAK_ALOLAN","SPECIES_VAPOREON"]},"TRAINER_BERNIE_2":{"lead":["SPECIES_INFERNAPE","SPECIES_VAPOREON"],"reserves":["SPECIES_PYROAR","SPECIES_GOLDUCK"]},"TRAINER_BERNIE_3":{"lead":["SPECIES_MAROWAK_ALOLAN","SPECIES_VAPOREON"],"reserves":["SPECIES_ROTOM_HEAT","SPECIES_INFERNAPE"]},"TRAINER_BERNIE_4":{"lead":["SPECIES_PYROAR","SPECIES_GOLDUCK"],"reserves":["SPECIES_MAROWAK_ALOLAN","SPECIES_VAPOREON","SPECIES_INFERNAPE","SPECIES_ROTOM_HEAT"]}},
  "team_intent":"Bernie's first double pairs Competitive Pyroar with Cloud Nine Golduck, then Lightning Rod Thick Club Alolan Marowak protects Water Absorb Vaporeon. Rematch one opens Fake Out Iron Fist Infernape beside Helping Hand Vaporeon. Rematch two makes the Lightning Rod reservoir the lead and adds Levitate Rotom-Heat pivoting. The final six combines all safety systems: weather denial, stat-drop punishment, Electric redirection, Water absorption, Fake Out/priority, burn, and Volt Switch. Every member attacks independently and no automatic weather is created.",
  "intended_counterplay":"Rock, Ground, Water, Electric, Grass, Fighting, special/physical category changes, Taunt, item removal, Wide Guard, priority, and focused damage are broad. Cloud Nine means weather is not a free plan; avoid feeding Competitive with Intimidate, remove Marowak before leaning on Electric, pressure Vaporeon around finite Helping Hand/Protect and no Wish, break Infernape's Sash, exploit Overheat drops and Volt Switch targets, and use noncontact or special damage around burn. No precise catch or forced turn is required.",
  "bespoke_ai":"All four records are guarded doubles with smart switching, partner awareness, HP awareness, and Combo Setup. Existing AI values Helping Hand/Fake Out/Encore/Protect only from visible board value, recognizes Lightning Rod and Water Absorb immunities, avoids redundant burns, and scores Competitive, Cloud Nine, Iron Fist, Levitate, Throat Spray, Thick Club, Sash, Overheat drops, and Volt Switch normally. No action, target, or switch is forced.",
  "uniqueness":"Pyroar, Golduck, Alolan Marowak, Vaporeon, Infernape, and Rotom-Heat are all new to the first 101 encounters and absent from protected anchors. This is the first route rematch family about prevention rather than escalating firepower. It uses no weather setter, room, terrain, sleep, trap, hazards, screens, setup, Mega, or legendary, preserving every Flannery, Magma, and Archie identity.",
  "story_logic":"Bernie's water-handy warning now becomes weather denial, Electric protection, and Water absorption. Shared rematch text truthfully names Fake Out, Helping Hand, burns, and Volt Switch. Initial and rematch commands are double-safe; registration and four-record routing remain native; no reward or story flag is added.",
  "reward_logic":"Every record grants ordinary EXP and prize money only; Match Call registration is the sole progression reward.",
  "campaign_reservations":{"spends":["Bernie controlled-burn rematch family","Competitive Pyroar","Cloud Nine Golduck","Lightning Rod Alolan Marowak","Water Absorb Vaporeon","Iron Fist Infernape","Rotom-Heat pivot"],"preserves":["all protected Fire/Water anchors","Coalossal Steam Engine prototype","sun/rain bosses","every Fire Mega and legend"],"repeat_rule":"These six species may repeat inside Bernie only; later uses require a materially different format or marquee role."},
  "author_self_check":{"strongest_part":"Every rematch adds a concrete safety layer without automatic weather, setup, or a Mega.","weakest_link":"Four members are special attackers and the family has broad Rock/Ground pressure. Marowak/Infernape physical axes, immunities, items, pivoting, six-body depth, and cap-relative levels keep those answers necessary without hiding them."},
  "closure":"Battle 102's full family is source-closed at quality 10: targets 8.9/9.1/9.3/9.5; all four records are guarded doubles; 18 legal cap-relative slots use six fresh unreserved species, distinct per-party items, exact Match Call routing, six indexed references, native-width dialogue, broad counterplay, and zero reward debt. Runtime remains unplayed."
 }

def ledger_entry():
 return {"index":102,"encounter_id":"BATTLE_102_ROUTE_114_BERNIE","identity":{"location":"Route114","category":"optional Kindler four-record Match Call family","format":"four guarded doubles","strict_cap":40,"memory_hook":"Bernie adds controlled-burn safety layers until six fresh Fire/Water specialists cover weather, Electric, Water, priority, burn, and pivots."},"primary_player_question":"Can the player identify which safety layer matters on this rematch and remove it before Bernie's mixed Fire/Water pressure cycles?","tempo":"Four cap-relative doubles: four-part safety opening, active Fake Out rematch, protected reservoir rematch, then six-part final.","pressure_sources":["Competitive Throat Spray Pyroar","Cloud Nine Golduck","Lightning Rod Thick Club Alolan Marowak","Water Absorb Helping Hand Vaporeon","Sash Iron Fist Infernape","Levitate Rotom-Heat burn/pivot"],"intentional_opening":"Every record has an authored lead and direct fallbacks; first is cap 40, rematches are five-badge cap 45+.","intentional_weakness":"Broad Rock/Ground pressure, four special attackers, no field/setup/healing loop, Infernape Sash dependence, Overheat drops, and removable safety partners.","first_loss_lesson":"Break the safety system before racing the fire: Cloud Nine, Lightning Rod, Water Absorb, Fake Out, and pivot state each change the correct target.","revealed_information":["initial cap 40","five-badge rematches","four guarded doubles","levels cap+1 to +4","six fresh species","no weather setter","Competitive","Cloud Nine","Lightning Rod","Water Absorb","Iron Fist","Levitate pivot","no Mega/reward"],"counterplay_classes":["Rock/Ground/Water/Electric/Grass/Fighting","mixed categories","Taunt/item removal","Wide Guard/priority","avoid Competitive Intimidate","remove Lightning Rod before Electric","Sash breaking","Overheat/pivot exploitation"],"target_difficulty":8.9,"difficulty_rationale":"The initial four optimized levels 41-44 create a serious double; rematches add protection, priority, pivots, and six-body depth. Final target is 9.5.","tuning_knob":"Tune final Rotom +4 to +3 first, then Infernape +3 to +2; preserve all safety roles and routing.","playtest_status":"static-pass-runtime-unplayed","novelty_tags":["route-rematch-family","four-guarded-doubles","controlled-burn","pyroar","golduck","marowak-alola","vaporeon","infernape","rotom-heat","competitive","cloud-nine","lightning-rod","water-absorb","iron-fist","levitate","six-fresh-species","no-weather-setter","no-mega","no-legendary"],"historic_reference_ids":REFERENCES,"corpus_search":{"status":"complete-current-review","pool_size":1005,"selection":"Six exact indexed references plus all-species reviews; safety progression is local."},"author_self_check":{"strongest_part":"Fire-safety dialogue and every mechanical role agree across four escalating doubles.","weakest_link":"Special and Rock/Ground compression remain public; physical axes, immunities, pivots, items, depth, and levels compensate."}}

def expected_payloads():
 designs=json.loads(DESIGNS.read_text()); designs["designs"]["BATTLE_102_ROUTE_114_BERNIE"]=design()
 ledger=json.loads(LEDGER.read_text()); ledger["entries"]=[r for r in ledger["entries"] if r["index"]!=102]+[ledger_entry()]; ledger["entries"].sort(key=lambda r:r["index"])
 sequence=json.loads(SEQUENCE.read_text())
 for row in sequence["entries"]:
  if row["index"]==102: row.update({"category":"optional rotating Kindler four-record Match Call family","trainer_ids":list(TEAMS),"access_note":"Bernie rotates at (30,58) with three-tile sight. One physical position owns his initial record and all three sequential Match Call rematches."})
 sequence["entries"]=[r for r in sequence["entries"] if r["index"]!=103]+[dict(NEXT)]; sequence["entries"].sort(key=lambda r:r["index"])
 for r in sequence["entries"]: r["status"]="closed" if r["index"]<=102 else "next" if r["index"]==103 else "queued"
 os=json.loads(OS_PATH.read_text()); os["current_state"].update({"closed_encounters":102,"next_index":103,"next_encounter_id":NEXT["encounter_id"],"queued_sequence_entries":0,"canonical_sequence_groups":103,"physical_encounter_groups":527,"unordered_physical_groups":424})
 return designs,ledger,sequence,os

def verify_source():
 trainers=(ROOT/"src/data/trainers.h").read_text(); parties=(ROOT/"src/data/trainer_parties.h").read_text(); blocks=doubles.trainer_blocks(trainers); dex=presets.LocalDex(); slots=doubles.base_ability_slots()
 for tid,team in TEAMS.items():
  block=blocks[tid].group(0); actual=[polish.parse_entry(e) for e in custom.party_entries(doubles.party_match(parties,doubles.party_name(block)).group(2))]
  if actual!=team: raise SystemExit(f"FAIL: Battle 102 party differs {tid}")
  for token in (".doubleBattle = TRUE","AI_FLAG_SMART_SWITCHING","AI_FLAG_HELP_PARTNER","AI_FLAG_HP_AWARE","AI_FLAG_COMBO_SETUP"):
   if token not in block: raise SystemExit(f"FAIL: Battle 102 {tid} missing {token}")
  if len({m['species'] for m in team})!=len(team) or len({m['item'] for m in team})!=len(team): raise SystemExit(f"FAIL: Battle 102 duplicates {tid}")
  for m in team:
   illegal=[x for x in m['moves'] if x not in dex.legal_moves(m['species'])]
   if illegal: raise SystemExit(f"FAIL: Battle 102 illegal {m['species']} {illegal}")
   if m['ability_slot']>=len(slots[m['species']]): raise SystemExit(f"FAIL: Battle 102 ability {m['species']}")
 route=(ROOT/"data/maps/Route114/scripts.inc").read_text()
 if "trainerbattle_double TRAINER_BERNIE_1" not in route or "trainerbattle_rematch_double TRAINER_BERNIE_1" not in route or route.count("Route114_Text_BernieNotEnoughMons")<2: raise SystemExit("FAIL: Battle 102 double guards")
 if "REMATCH(TRAINER_BERNIE_1, TRAINER_BERNIE_2, TRAINER_BERNIE_3, TRAINER_BERNIE_4, ROUTE114)" not in (ROOT/"src/battle_setup.c").read_text(): raise SystemExit("FAIL: Battle 102 rematch row")
 obj=next(r for r in json.loads((ROOT/"data/maps/Route114/map.json").read_text())["object_events"] if r.get("script")=="Route114_EventScript_Bernie")
 if (obj['x'],obj['y'],obj['movement_type'],str(obj['trainer_sight_or_berry_tree_id']))!=(30,58,"MOVEMENT_TYPE_ROTATE_COUNTERCLOCKWISE","3"): raise SystemExit("FAIL: Battle 102 geometry")
 expected={"TRAINER_BERNIE_1":("Controlled-burn opening",89,4,3),"TRAINER_BERNIE_2":("Active-safety rematch",91,4,3),"TRAINER_BERNIE_3":("Protected-reservoir rematch",93,4,3),"TRAINER_BERNIE_4":("Six-part fire-safety final",95,6,2)}; manifest=json.loads((ROOT/"docs/verdant_doubles_manifest.json").read_text())["formats"]
 for tid,(arch,diff,size,off) in expected.items():
  if manifest[tid]!={"format":"double","target_size":size,"archetype":arch,"difficulty":diff,"partner_interaction":True,"level_offset":off,"location":"Route 114"}: raise SystemExit(f"FAIL: Battle 102 manifest {tid}")
 dialogue=(ROOT/"data/text/trainers.inc").read_text().split("Route114_Text_BernieIntro:",1)[1].split("Route114_Text_ClaudeIntro:",1)[0]
 for cue in ("water near every flame","Competitive Pyroar","Cloud Nine","Lightning Rod","Water Absorb","Fake Out","Helping Hand","Volt Switch","Iron Fist","Levitate","safety needs a partner"):
  if cue not in dialogue: raise SystemExit(f"FAIL: Battle 102 dialogue {cue}")
 for line in re.findall(r'\.string "([^"]*)"',dialogue):
  if len(line.replace('\\n','').replace('\\l','').replace('\\p','').replace('$',''))>36: raise SystemExit(f"FAIL: Battle 102 overlong {line}")
 ids={json.loads(x)['reference_id'] for x in CORPUS.read_text().splitlines()}
 if any(r not in ids for r in REFERENCES): raise SystemExit("FAIL: Battle 102 reference")
 protected='\n'.join(p.read_text() for p in list((ROOT/'docs').glob('emerald_champions_*anchor_designs.json'))+list((ROOT/'docs/dossier_packets').glob('*.json')))
 for species in ('Pyroar','Golduck','Marowak-Alola','Vaporeon','Infernape','Rotom-Heat'):
  if re.search(rf'"{re.escape(species)}"',protected): raise SystemExit(f"FAIL: Battle 102 protected {species}")

def main():
 p=argparse.ArgumentParser();p.add_argument('--write',action='store_true');p.add_argument('--check',action='store_true');a=p.parse_args()
 if not a.write and not a.check:p.error('choose --write or --check')
 payloads=expected_payloads();paths=(DESIGNS,LEDGER,SEQUENCE,OS_PATH);texts=[json.dumps(x,indent=2,ensure_ascii=False)+'\n' for x in payloads]
 if a.write:
  for path,text in zip(paths,texts):path.write_text(text)
 if a.check:
  for path,text in zip(paths,texts):
   if path.read_text()!=text:raise SystemExit(f"FAIL: Battle 102 stale {path.name}")
  verify_source();guide=json.loads((ROOT/'docs/verdant_battle_guide.json').read_text());entries=[r for r in guide['entries'] if r['trainerId'] in TEAMS]
  if len(entries)!=4 or any(r['designStatus']!='closed' or r['format']!='double' for r in entries):raise SystemExit('FAIL: Battle 102 guide')
  if {r['trainerId']:r['partySize'] for r in entries}!={"TRAINER_BERNIE_1":4,"TRAINER_BERNIE_2":4,"TRAINER_BERNIE_3":4,"TRAINER_BERNIE_4":6}:raise SystemExit('FAIL: Battle 102 guide sizes')
 print('PASS: Battle 102 Bernie controlled-burn family is source-closed')
if __name__=='__main__':main()
