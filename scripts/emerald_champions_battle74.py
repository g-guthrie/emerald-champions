#!/usr/bin/env python3
"""Generate/check the exact Battle 74 Route 117 dynamic native-pair cluster closure."""

from __future__ import annotations

import argparse, json, re
from pathlib import Path
import verdant_custom_teams as custom
import verdant_doubles_conversion as doubles
import verdant_team_polish as polish

ROOT=Path(__file__).resolve().parents[1]
DESIGNS=ROOT/"docs/verdant_bespoke_battle_designs.json"; LEDGER=ROOT/"docs/verdant_battle_experience_ledger.json"; SEQUENCE=ROOT/"docs/verdant_battle_sequence.json"; OS_PATH=ROOT/"docs/emerald_champions_battle_design_operating_system.json"

TEAMS={
"TRAINER_AISHA":[
{"level":1,"species":"SPECIES_MIENSHAO","item":"ITEM_FOCUS_SASH","ability_slot":1,"spread":"SPREAD_31_IV_ATK_SPEED_JOLLY","moves":["MOVE_FAKE_OUT","MOVE_FEINT","MOVE_CLOSE_COMBAT","MOVE_U_TURN"]},
{"level":3,"species":"SPECIES_HARIYAMA","item":"ITEM_SITRUS_BERRY","ability_slot":0,"spread":"SPREAD_31_IV_HP_ATK_ADAMANT","moves":["MOVE_FAKE_OUT","MOVE_CLOSE_COMBAT","MOVE_KNOCK_OFF","MOVE_WIDE_GUARD"]}],
"TRAINER_MELINA":[
{"level":1,"species":"SPECIES_RAPIDASH","item":"ITEM_LIFE_ORB","ability_slot":0,"spread":"SPREAD_31_IV_ATK_SPEED_JOLLY","moves":["MOVE_FLARE_BLITZ","MOVE_HIGH_HORSEPOWER","MOVE_WILD_CHARGE","MOVE_PROTECT"]},
{"level":3,"species":"SPECIES_BOLTUND","item":"ITEM_MAGNET","ability_slot":0,"spread":"SPREAD_31_IV_ATK_SPEED_JOLLY","moves":["MOVE_THUNDER_FANG","MOVE_CRUNCH","MOVE_PSYCHIC_FANGS","MOVE_PLAY_ROUGH"]}],
"TRAINER_BRANDI":[
{"level":1,"species":"SPECIES_MEOWSTIC","item":"ITEM_LIGHT_CLAY","ability_slot":2,"spread":"SPREAD_31_IV_HP_SPEED_TIMID","moves":["MOVE_REFLECT","MOVE_LIGHT_SCREEN","MOVE_PSYCHIC","MOVE_HELPING_HAND"]},
{"level":3,"species":"SPECIES_MUSHARNA","item":"ITEM_KASIB_BERRY","ability_slot":2,"spread":"SPREAD_31_IV_HP_SPATK_QUIET","moves":["MOVE_PSYCHIC","MOVE_MOONBLAST","MOVE_YAWN","MOVE_PROTECT"]}],
}
REFS=["showdown:gen7randomdoublesbattle:016","showdown:gen7randomdoublesbattle:010","showdown:gen8randomdoublesbattle:017","showdown:gen6randomdoublesbattle:012","showdown:gen5randomdoublesbattle:011"]

def design():
 return {
 "guide_order":74,"trainer_ids":["TRAINER_AISHA","TRAINER_MELINA","TRAINER_BRANDI"],"status":"closed","strict_cap":40,
 "campaign_point":"Optional upper Route 117 meadow cluster after Derek. Patrol timing can create Aisha+Melina or Brandi+Melina native doubles, leave the third as a later single, or allow three split singles; no trainer has a callback, item, or rematch.",
 "evolution_stage_fit":{"campaign_phase":"post-Wattson mature meadow cluster","effective_levels":"41 and 43 per trainer","eligible_ratio":"6/6","mega_access":True,"status":"pass","reason":"All six are naturally fully evolved by cap 40 or single-stage. No Mega or legendary is used; difficulty comes from branch-aware optimized pairs and levels."},
 "manual_quality":10,"manual_difficulty":9.0,
 "corpus_review":{"reference_pool_size":1005,"full_team_candidates":[{"reference_id":r,"decision":"selected role; full team rejected","reason":"Reproducible doubles donor supports one exact trainer role without replacing the native meadow identities."} for r in REFS],"decision":"Five full-set donors support the Fighting, Strong Jaw, screen, and Psychic roles. The two possible native pairs and three splits are transparently hand-authored around fixed map geometry."},
 "competitive_references":[{"reference_id":r,"adaptation":"One exact locally legal role is retained; unrelated donor teammates and gimmicks are rejected."} for r in REFS],
 "branch_contract":{
  "aisha_melina_joint":{"format":"two-opponent native double","trainers":["TRAINER_AISHA","TRAINER_MELINA"],"source_slots":{"aisha":[0,1],"melina":[0,1]},"target_difficulty":9.0},
  "brandi_melina_joint":{"format":"two-opponent native double","trainers":["TRAINER_BRANDI","TRAINER_MELINA"],"source_slots":{"brandi":[0,1],"melina":[0,1]},"target_difficulty":8.8},
  "splits":{"TRAINER_AISHA":8.0,"TRAINER_MELINA":7.9,"TRAINER_BRANDI":7.8},
  "one_usable_policy":"Each independently scripted single remains legal with one usable player Pokemon; native joint formation follows ordinary two-opponent requirements and map sight timing."
 },
 "ordering":{"intended_lead":["SPECIES_MIENSHAO","SPECIES_RAPIDASH"],"source_order":{"aisha":["SPECIES_MIENSHAO","SPECIES_HARIYAMA"],"melina":["SPECIES_RAPIDASH","SPECIES_BOLTUND"],"brandi":["SPECIES_MEOWSTIC","SPECIES_MUSHARNA"]},"reason":"Every two-member source record is its own complete single. In joints, source-first members expose either tactical Fighting plus sprint offense or screens plus sprint offense; no hidden reserve selector is needed."},
 "team_intent":"Aisha supplies Regenerator Mienshao tactical Fake Out/Feint/pivot pressure and Thick Fat Hariyama Fake Out/Wide Guard bulk. Melina is the shared full-tilt lane through Reckless Rapidash and Strong Jaw Boltund. Brandi supplies Prankster screens/Helping Hand and Psychic Surge Musharna with Yawn. Either native double has four optimized levels 41/43 and a coherent but different question; every split remains self-sufficient.",
 "intended_counterplay":"Ghost/Inner Focus, Protect, priority, Rocky Helmet, Fairy/Psychic/Flying, burn, and faster pressure answer Aisha. Water/Ground/Rock, Intimidate, recoil pressure, and physical bulk answer Melina. Taunt, Brick Break/Defog/screen removal, Dark/Ghost/Bug, terrain replacement, status immunity, and concentrated damage answer Brandi. Single-target moves play around Wide Guard; non-Protect turns play around Feint; open terrain lets the player choose engagement order.",
 "bespoke_ai":"All three retain native single records and ordinary native-pair combination. Smart switching, partner, HP, field, and combo flags are enabled where relevant. Fake Out, Feint, Wide Guard, screens, Helping Hand, Psychic Terrain, Yawn, recoil, Strong Jaw, Life Orb, Sash, and Protect use public existing mechanics. AI never reads hidden Protect or stacks redundant support.",
 "uniqueness":"All six exact species are new to the first 73 closed encounters. This is the only physical cluster where one moving runner can pair with either a Fighting specialist or Psychic specialist, creating two real doubles from the same six fresh species without a global allocator.",
 "story_logic":"Existing native dialogue already distinguishes Aisha's clean pressure, Melina's full-tilt sprint, and Brandi's calm screens/Yawn. All three truthfully wait for the Dynamo Badge and have no story reward or callback.",
 "reward_logic":"Each trainer grants only EXP and prize money. No item, shop, Match Call, Mega Stone, or progression reward is attached.",
 "campaign_reservations":{"spends":["Route 117 dynamic three-trainer cluster","Mienshao/Hariyama tactical Fighting pair","Rapidash/Boltund sprint pair","Meowstic/Musharna screen-terrain pair"],"preserves":["weather, room, Tailwind, hazards, legends, Megas, self-activation, and persistent traps for later battles"],"repeat_rule":"Later use of these species requires a materially different trainer or role; same-rival future blueprint species may be revised before source implementation."},
 "author_self_check":{"strongest_part":"One real map formation produces two different hard doubles and three fair singles without inventing NPCs or scripts; the shared Melina lane changes meaning beside each specialist.","weakest_link":"Each split has only two Pokemon. Levels +1/+3 and exact coverage keep them above the floor, but split difficulty is intentionally below the four-member joints."},
 "closure":"Battle 74 is source-closed at quality 10: six fresh legal mature species, six distinct items, exact 2+2/2+2/split branch contracts, native map geometry, branch-aware AI, five current references, truthful dialogue, broad counterplay, no reward debt, and no unsupported gimmick. Joint target difficulty is 9.0/8.8; split targets are 7.8-8.0; runtime remains unplayed."
 }

def ledger_entry():
 return {"index":74,"encounter_id":"BATTLE_074_ROUTE_117_AISHA_MELINA_BRANDI","identity":{"location":"Route117","category":"optional dynamic native-pair cluster","format":"native-pair doubles or split singles","strict_cap":40,"memory_hook":"Melina's sprint can be joined by Aisha's Fighting tactics or Brandi's Psychic study circle; all three two-member teams also stand alone."},"primary_player_question":"Can the player recognize which specialist paired with Melina, break the shared sprint lane, and adapt support counterplay without overpreparing for a branch that map timing did not create?","tempo":"Aisha-Melina tactical pressure double, Brandi-Melina screen/terrain double, or three concise level-advantaged singles; no persistent weather, room, or custom subsystem.","pressure_sources":["level-41 Sash Regenerator Mienshao","level-43 Sitrus Thick Fat Hariyama","level-41 Life Orb Reckless Rapidash","level-43 Magnet Strong Jaw Boltund","level-41 Light Clay Prankster Meowstic","level-43 Kasib Psychic Surge Musharna"],"intentional_opening":"Source-first members define the actual engaged trainers. Native sight timing—not a script selector—chooses Aisha+Melina, Brandi+Melina, or a split.","intentional_weakness":"Every record has only two Pokemon, no recovery engine, and clear typed seams. Support is finite, Rapidash pays recoil, Mienshao/Sash and Meowstic/Light Clay are item-reliant, and no Mega/legendary inflates the branch.","first_loss_lesson":"Read who joined the runner. Against Aisha, vary Protect and spread around Feint/Wide Guard; against Brandi, remove screens or terrain before racing Melina; in splits, pressure the exposed two-member seam.","revealed_information":["Badge 3 required","three independent source records","two real native pair combinations","each split has two Pokemon","all six are fresh","no rewards or rematches"],"counterplay_classes":["Ghost/Inner Focus/Fairy/Psychic/Flying into Aisha","Water/Ground/Rock/Intimidate/recoil into Melina","Taunt/screen removal/Dark/Ghost/Bug into Brandi","single/spread variation around Feint and Wide Guard","terrain replacement and status immunity","priority and concentrated damage"],"target_difficulty":9.0,"difficulty_rationale":"Each joint fields four optimized legal levels 41/43 with full distinct items and complementary public support; splits retain two level-advantaged optimized members. Broad typed seams and finite support keep all branches learnable.","tuning_knob":"Tune each level-43 closer to +2 first; preserve the six species, two native pair geometries, and split ownership.","playtest_status":"static-pass-runtime-unplayed","novelty_tags":["route-cluster","native-pair-double","split-singles","dynamic-patrol-pairing","fake-out","feint","wide-guard","screens","psychic-terrain","strong-jaw","reckless","no-mega","no-legendary","no-weather","no-room","no-tailwind"],"historic_reference_ids":REFS,"corpus_search":{"status":"complete-current-review","pool_size":1005,"selection":"Five generated doubles records support the exact role pieces; the physical three-trainer branch topology is hand-authored from source geometry."},"author_self_check":{"strongest_part":"The map itself changes the team composition without a menu or global allocator.","weakest_link":"Two-Pokemon splits are necessarily lighter; their +1/+3 levels and optimized sets are the explicit compensation."}}

def expected_payloads():
 designs=json.loads(DESIGNS.read_text()); designs["designs"]["BATTLE_074_ROUTE_117_AISHA_MELINA_BRANDI"]=design()
 ledger=json.loads(LEDGER.read_text()); ledger["entries"]=[e for e in ledger["entries"] if e["index"]!=74]+[ledger_entry()]; ledger["entries"].sort(key=lambda e:e["index"])
 sequence=json.loads(SEQUENCE.read_text())
 for e in sequence["entries"]:
  if e["index"]==74:e["status"]="closed"
  elif e["index"]==75:e["status"]="next"
  elif e["index"]>75:e["status"]="queued"
 os=json.loads(OS_PATH.read_text()); os["current_state"].update({"closed_encounters":74,"next_index":75,"next_encounter_id":"BATTLE_075_ROUTE_117_LYDIA","queued_sequence_entries":9})
 return designs,ledger,sequence,os

def verify_source():
 trainers=(ROOT/"src/data/trainers.h").read_text(); parties=(ROOT/"src/data/trainer_parties.h").read_text(); blocks=doubles.trainer_blocks(trainers)
 for trainer_id,expected in TEAMS.items():
  block=blocks[trainer_id].group(0); body=doubles.party_match(parties,doubles.party_name(block)).group(2); actual=[polish.parse_entry(e) for e in custom.party_entries(body)]
  if actual!=expected:raise SystemExit(f"FAIL: Battle 74 source differs for {trainer_id}")
  if ".doubleBattle = FALSE" not in block or "AI_FLAG_HELP_PARTNER" not in block or "AI_FLAG_HP_AWARE" not in block:raise SystemExit(f"FAIL: Battle 74 flags differ for {trainer_id}")
 scripts=(ROOT/"data/maps/Route117/scripts.inc").read_text()
 for trainer_id,label in (("TRAINER_AISHA","Aisha"),("TRAINER_MELINA","Melina"),("TRAINER_BRANDI","Brandi")):
  if f"trainerbattle_single {trainer_id}" not in scripts:raise SystemExit(f"FAIL: Battle 74 missing {label} single script")
 dialogue=(ROOT/"data/text/trainers.inc").read_text().split("Route117_Text_MelinaIntro:",1)[1].split("Route118_Text_RoseIntro:",1)[0]
 for cue in ("full tilt","Twin screens","Mienshao heals","Dynamo Badge"):
  if cue not in dialogue:raise SystemExit(f"FAIL: Battle 74 dialogue missing {cue}")
 for line in re.findall(r'\.string "([^"]*)"',dialogue):
  visible=line.replace("\\n","").replace("\\l","").replace("\\p","").replace("$","")
  if len(visible)>36:raise SystemExit(f"FAIL: Battle 74 overlong dialogue: {visible}")

def main():
 parser=argparse.ArgumentParser();parser.add_argument("--write",action="store_true");parser.add_argument("--check",action="store_true");args=parser.parse_args()
 if not args.write and not args.check:parser.error("choose --write or --check")
 payloads=expected_payloads();paths=(DESIGNS,LEDGER,SEQUENCE,OS_PATH);expected=[json.dumps(p,indent=2,ensure_ascii=False)+"\n" for p in payloads]
 if args.write:
  for path,text in zip(paths,expected):path.write_text(text)
 if args.check:
  for path,text in zip(paths,expected):
   if path.read_text()!=text:raise SystemExit(f"FAIL: Battle 74 artifact stale: {path.name}")
  verify_source();guide=json.loads((ROOT/"docs/verdant_battle_guide.json").read_text())
  for trainer_id,team in TEAMS.items():
   entry=next(row for row in guide["entries"] if row["trainerId"]==trainer_id)
   if entry["designStatus"]!="closed" or [m["speciesId"] for m in entry["party"]]!=[m["species"] for m in team]:raise SystemExit(f"FAIL: Battle 74 guide stale for {trainer_id}")
 print("PASS: Battle 74 dynamic Route 117 cluster is source-closed across every joint and split branch")

if __name__=="__main__":main()
