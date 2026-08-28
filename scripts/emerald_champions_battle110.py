#!/usr/bin/env python3
"""Generate and verify Battle 110, Maxie's Mt. Chimney ridge boss."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

import verdant_battle_set_presets as presets
import verdant_custom_teams as custom
import verdant_doubles_conversion as doubles
import verdant_team_polish as polish


ROOT = Path(__file__).resolve().parents[1]
DESIGNS = ROOT / "docs/verdant_bespoke_battle_designs.json"
LEDGER = ROOT / "docs/verdant_battle_experience_ledger.json"
SEQUENCE = ROOT / "docs/verdant_battle_sequence.json"
OS_PATH = ROOT / "docs/emerald_champions_battle_design_operating_system.json"
CORPUS = ROOT / "docs/competitive_team_index.jsonl"
FACTION = ROOT / "docs/emerald_champions_faction_anchor_designs.json"
RESERVATIONS = ROOT / "docs/verdant_historic_team_reservations.json"

TEAM = [
    {"level": 1, "species": "SPECIES_GROUDON", "item": "ITEM_HEAT_ROCK", "ability_slot": 0, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_PRECIPICE_BLADES", "MOVE_FIRE_PUNCH", "MOVE_STONE_EDGE", "MOVE_PROTECT"]},
    {"level": 1, "species": "SPECIES_CROBAT", "item": "ITEM_SITRUS_BERRY", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_SPEED_TIMID", "moves": ["MOVE_TAILWIND", "MOVE_SUPER_FANG", "MOVE_TAUNT", "MOVE_QUICK_GUARD"]},
    {"level": 2, "species": "SPECIES_SHIFTRY", "item": "ITEM_FOCUS_SASH", "ability_slot": 0, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_FAKE_OUT", "MOVE_LEAF_BLADE", "MOVE_KNOCK_OFF", "MOVE_PROTECT"]},
    {"level": 2, "species": "SPECIES_SALAZZLE", "item": "ITEM_LIFE_ORB", "ability_slot": 0, "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID", "moves": ["MOVE_HEAT_WAVE", "MOVE_SLUDGE_BOMB", "MOVE_ENCORE", "MOVE_HELPING_HAND"]},
    {"level": 3, "species": "SPECIES_ENTEI", "item": "ITEM_ASSAULT_VEST", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_SACRED_FIRE", "MOVE_EXTREME_SPEED", "MOVE_STOMPING_TANTRUM", "MOVE_SNARL"]},
    {"level": 5, "species": "SPECIES_FLYGON", "item": "ITEM_FLYGONITE", "ability_slot": 0, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_EARTHQUAKE", "MOVE_DRAGON_CLAW", "MOVE_STONE_EDGE", "MOVE_PROTECT"]},
]

REFERENCES = [
    "vgc:naic-2022",
    "showdown:gen4randomdoublesbattle:007",
    "showdown:gen9championsrandomdoublesbattle:003",
    "vgc:regional-portland-2024",
    "showdown:gen5randomdoublesbattle:002",
]

NEXT = {
    "index": 111,
    "encounter_id": "BATTLE_111_JAGGED_PASS_ERIC",
    "location": "JaggedPass",
    "category": "optional upper-pass Hiker single",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_ERIC"],
    "access_note": "Eric faces right at (10,8) with three-tile sight near the Mt. Chimney entry warps (13,5)/(14,5). He precedes Diana, the optional direct-interaction Magma guard, the Autumn/Julio corridor, and Ethan on the first descent.",
}


def design() -> dict:
    return {
        "guide_order": 110,
        "trainer_ids": ["TRAINER_MAXIE_MT_CHIMNEY"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Required summit leader boss immediately after Tabitha, with manual Bag/menu preparation available between. "
            "Maxie's locked story scene now performs an explicit two-usable-Pokemon guard before its no-intro battle."
        ),
        "runtime_branches": ["Guarded six-member double at cap 40.", "Native refusal releases the scene if fewer than two usable Pokemon are available."],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 faction-leader summit boss",
            "effective_levels": "41, 41, 42, 42, 43, and 45",
            "eligible_ratio": "6/6",
            "mega_access": True,
            "status": "pass",
            "reason": "Groudon and Entei are single-stage; Crobat evolves by friendship; Shiftry by stone; Salazzle at 33; this source evolves Vibrava into Flygon at 45, so the Mega ace is intentionally cap+5 and exactly stage-legal.",
        },
        "manual_quality": 10,
        "manual_difficulty": 10.0,
        "observed_difficulty": None,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {"reference_id": REFERENCES[0], "decision": "base Groudon history selected", "reason": "NAIC champion evidence validates elite Groudon sun; Mt. Chimney uses base Drought and reserves Primal doctrine."},
                {"reference_id": REFERENCES[1], "decision": "Shiftry role adapted", "reason": "Generated sun offense validates priority and Grass/Dark pressure; sleep/setup are removed for one Sash Fake Out role."},
                {"reference_id": REFERENCES[2], "decision": "Salazzle role adapted", "reason": "The Champions set validates fast doubles utility; Encore/Helping Hand create a special tempo role rather than a second Fake Out."},
                {"reference_id": REFERENCES[3], "decision": "Entei history selected", "reason": "Portland-winning evidence validates Entei as modern bulky direct doubles pressure."},
                {"reference_id": REFERENCES[4], "decision": "Mega Flygon role adapted", "reason": "Generated Flygon validates Ground/Dragon doubles offense; the custom Mega is used at its exact evolution level 45."},
            ],
            "decision": "All 1005 references and Battles 100-109 were reviewed. No previous-ten species collision exists; Protect/Rock Slide/Fake Out density was reduced without changing the protected ridge thesis.",
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Heat Rock base Groudon establishes ordinary Drought and direct Ground/Fire/Rock pressure without Red Orb or setup."},
            {"reference_id": REFERENCES[1], "adaptation": "Focus Sash Chlorophyll Shiftry is the sole Fake Out ambusher with Grass/Dark item pressure."},
            {"reference_id": REFERENCES[2], "adaptation": "Life Orb Salazzle uses spread Fire/Poison, Encore, and Helping Hand rather than duplicate Fake Out or Protect."},
            {"reference_id": REFERENCES[3], "adaptation": "Assault Vest Inner Focus Entei supplies Sacred Fire, priority, Ground coverage, and Snarl."},
            {"reference_id": REFERENCES[4], "adaptation": "Level-45 Mega Flygon owns the ridge through Earthquake positioning and direct Dragon/Rock alternatives."},
        ],
        "ordering": {"lead": ["SPECIES_GROUDON", "SPECIES_CROBAT"], "reserves": ["SPECIES_SHIFTRY", "SPECIES_SALAZZLE", "SPECIES_ENTEI", "SPECIES_FLYGON"], "reason": "Base Groudon and Crobat make summit control public; physical/special tempo, guardian, and Mega ridge roles remain board-state reserves."},
        "team_intent": (
            "Base Groudon plus Crobat establish ordinary sun, Tailwind, Taunt, Quick Guard, Super Fang, and Precipice pressure. "
            "Shiftry is the only Fake Out; Salazzle uses Encore/Helping Hand as distinct special tempo; Entei is the bulky priority "
            "guardian; legal level-45 Mega Flygon closes through partner-sensitive Earthquake. Groudon never holds Red Orb or Primal Reverts."
        ),
        "intended_counterplay": (
            "Contest Tailwind or focus/Taunt Crobat; use Wide Guard, Flying/Levitate, weather control, Water/Grass/Fairy/Fighting, "
            "and mixed bulk against Groudon/ambushers; use Protect, Ghost/Inner Focus, priority, or double-targeting into Shiftry; "
            "respect Encore/Helping Hand; pressure Entei with Water/Rock/Ground special damage; position immunities, Intimidate/burn, "
            "Ice/Fairy/Dragon, Wide Guard, and speed control against Mega Flygon."
        ),
        "bespoke_ai": (
            "Maxie uses smart switching, partner awareness, HP awareness, Speed Control, Field Control, and Combo Setup. Native "
            "scoring evaluates Crobat support from visible state, Shiftry Fake Out and Salazzle Helping Hand from actual tempo, base "
            "Groudon without Primal assumptions, and Flygon Earthquake through ally collateral/immunity checks. No action is forced."
        ),
        "uniqueness": (
            "Groudon, Shiftry, and Entei are new to the first 109 encounters. Crobat returns 16 battles later, Flygon 31, and "
            "Salazzle 38 in boss-specific ridge/tempo/Mega roles; none appears in the previous ten. Premature Red Orb, Sleep Powder, "
            "Zweilous/Weezing/Victreebel/Camerupt, second Fake Out, and repeated Rock Slide are removed."
        ),
        "story_logic": (
            "Maxie's land ideology and Meteorite/Orb progression remain native. The battle extension names heat, air control, tempo "
            "and ridge ownership; defeat concedes the ridge rather than the ideology. The scene guard, retreat, Archie arrival, flags, "
            "Meteorite recovery, and Jagged Pass descent remain intact."
        ),
        "reward_logic": "Required story progress, EXP, prize money, and native Meteorite recovery only; no competitive held-item reward is added.",
        "campaign_reservations": {
            "spends": ["base Groudon first Maxie reveal", "Crobat ridge control", "Shiftry/Salazzle split tempo", "Entei guardian", "legal level-45 Mega Flygon"],
            "preserves": ["final Primal Groudon/Cherrim/Oranguru/Walking Wake/Mega Camerupt", "Flannery thermal timing", "Tabitha machinery", "Archie/Matt water identities"],
            "repeat_rule": "Groudon may recur only as Maxie's Primal progression; the other five require materially different roles or same-character evolution."
        },
        "author_self_check": {
            "strongest_part": "Groudon appears without spending Primal, while the summit's air lane and safe Earthquake positioning—not raw sun alone—define the boss.",
            "weakest_link": "Sun/Tailwind/Fake Out are familiar tools. One Fake Out, Salazzle's different support, Stone Edge substitutions, no sleep, and legal Mega positioning keep the sequence distinct."
        },
        "closure": (
            "Battle 110 is source-closed at quality 10 and target difficulty 10: guarded story scene; exact +1/+1/+2/+2/+3/+5 "
            "stage-legal team; base Groudon/no Red Orb; one Mega; three fresh and three distant role-changed species; five indexed "
            "references; source-honest faction anchor and spent reservation; native-width dialogue; broad counterplay; story flags and "
            "reward flow; and final-Magma reservations. Runtime remains unplayed and observed difficulty is unset."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 110,
        "encounter_id": "BATTLE_110_MT_CHIMNEY_MAXIE",
        "identity": {"location": "MtChimney", "category": "required Magma Leader marquee boss", "format": "guarded six-member double", "strict_cap": 40, "memory_hook": "Base Groudon raises heat, Crobat controls air, two distinct ambushers steal tempo, Entei guards the crater, and Mega Flygon claims the ridge."},
        "primary_player_question": "Can the player contest Crobat's ridge control and base Groudon pressure, distinguish physical Fake Out from special Encore support, and preserve safe positioning for Mega Flygon?",
        "tempo": "Six-stage summit boss: base-sun air-control lead, physical/special tempo reserves, rare priority guardian, then partner-sensitive Mega ridge ace.",
        "pressure_sources": ["Heat Rock base Groudon", "Crobat Tailwind/Taunt/Quick Guard/Super Fang", "Sash Chlorophyll Shiftry", "Life Orb Salazzle Encore/Helping Hand", "AV Entei Sacred Fire/priority/Snarl", "level-45 Mega Flygon Earthquake positioning"],
        "intentional_opening": "Groudon+Crobat are fixed; Groudon remains base form and no support action is forced.",
        "intentional_weakness": "Low-damage Crobat, frail ambushers, Entei without Protect, no-setup base Groudon, Flygon ally-damage constraint/no recovery, no Primal/sleep/redirection/healing loop.",
        "first_loss_lesson": "Break the air lane, distinguish the two tempo roles, remember Groudon is not Primal, and reach Mega Flygon with positioning—not only type coverage—intact.",
        "revealed_information": ["cap 40", "guarded required double", "levels 41-45", "base Groudon Heat Rock", "one Tailwind", "one Fake Out", "Encore/Helping Hand", "Entei", "Mega Flygon", "no Red Orb/Primal", "native story reward"],
        "counterplay_classes": ["Tailwind contest/Crobat focus", "Wide Guard/Flying/Levitate/weather control", "Water/Grass/Fairy/Fighting", "Protect/Ghost/Inner Focus", "Encore awareness", "Water/Rock/Ground into Entei", "Ice/Fairy/Dragon and positioning into Flygon"],
        "target_difficulty": 10.0,
        "difficulty_rationale": "Six optimized levels 41-45, base Groudon sun, active air control, split tempo, Entei, and a legal +5 Mega demand boss-level adaptation while exposing every major answer publicly.",
        "tuning_knob": "Tune Mega Flygon +5 to +4 only if stage legality is separately preserved by a species change; otherwise tune Entei +3, then ambushers +2. Never restore premature Primal.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["mt-chimney", "required-boss", "magma-leader", "base-groudon", "ordinary-sun", "crobat-ridge", "shiftry", "salazzle-support", "entei", "mega-flygon", "level-45-ace", "one-fake-out", "no-sleep", "no-primal", "one-mega"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Champion Groudon/Entei evidence plus exact generated Shiftry/Salazzle/Flygon roles."},
        "author_self_check": {"strongest_part": "The story reveal and mechanical midpoint are both clear: Groudon is present, but the full Primal plan is not.", "weakest_link": "Familiar sun tools must execute as distinct ridge roles rather than generic offense."},
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_110_MT_CHIMNEY_MAXIE"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 110] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])
    sequence = json.loads(SEQUENCE.read_text())
    for row in sequence["entries"]:
        if row["index"] == 110:
            row.update({"category": "required Mt. Chimney Magma Leader marquee boss", "trainer_ids": ["TRAINER_MAXIE_MT_CHIMNEY"], "access_note": "Maxie at (13,6) owns a guarded no-intro six-member double. Defeat completes the summit scene and unlocks the Jagged Pass descent."})
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 111] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 110 else "next" if row["index"] == 111 else "queued"
    os_data = json.loads(OS_PATH.read_text())
    os_data["current_state"].update({"closed_encounters": 110, "next_index": 111, "next_encounter_id": NEXT["encounter_id"], "queued_sequence_entries": 0, "canonical_sequence_groups": 111, "physical_encounter_groups": 525, "unordered_physical_groups": 414})
    return designs, ledger, sequence, os_data


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text(); parties = (ROOT / "src/data/trainer_parties.h").read_text()
    block = doubles.trainer_blocks(trainers)["TRAINER_MAXIE_MT_CHIMNEY"].group(0)
    actual = [polish.parse_entry(entry) for entry in custom.party_entries(doubles.party_match(parties, doubles.party_name(block)).group(2))]
    if actual != TEAM: raise SystemExit("FAIL: Battle 110 Maxie source party differs")
    for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"):
        if token not in block: raise SystemExit(f"FAIL: Battle 110 Maxie missing {token}")
    if len({m['species'] for m in TEAM}) != 6 or len({m['item'] for m in TEAM}) != 6: raise SystemExit("FAIL: Battle 110 duplicate species/items")
    dex=presets.LocalDex(); slots=doubles.base_ability_slots()
    for member in TEAM:
        illegal=[move for move in member['moves'] if move not in dex.legal_moves(member['species'])]
        if illegal: raise SystemExit(f"FAIL: Battle 110 illegal {member['species']} {illegal}")
        if member['ability_slot']>=len(slots[member['species']]): raise SystemExit(f"FAIL: Battle 110 ability {member['species']}")
    evolution=(ROOT/'src/data/pokemon/evolution.h').read_text()
    if "[SPECIES_VIBRAVA]" not in evolution or "EVO_LEVEL, 45, SPECIES_FLYGON" not in evolution: raise SystemExit("FAIL: Battle 110 Flygon level proof")
    if any(m['item']=='ITEM_RED_ORB' for m in TEAM): raise SystemExit("FAIL: Battle 110 premature Red Orb")

    script=(ROOT/'data/maps/MtChimney/scripts.inc').read_text(); event=script.split('MtChimney_EventScript_Maxie::',1)[1].split('MtChimney_EventScript_ArchieApproachPlayerEast::',1)[0]
    for token in ('special HasEnoughMonsForDoubleBattle','compare VAR_RESULT, PLAYER_HAS_TWO_USABLE_MONS','goto_if_ne MtChimney_EventScript_MaxieNeedsTwoMons','trainerbattle_no_intro TRAINER_MAXIE_MT_CHIMNEY','MtChimney_EventScript_MaxieNeedsTwoMons::','releaseall','FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY'):
        if token not in event: raise SystemExit(f"FAIL: Battle 110 script missing {token}")
    section=script.split('MtChimney_Text_MaxieIntro:',1)[1].split('MtChimney_Text_TabithaIntro:',1)[0]
    for cue in ('summit belongs','Groudon raises the heat','Crobat owns','ambushers steal tempo','Flygon claims the ridge','not the land','obtain that Orb','double battle'):
        if cue not in section: raise SystemExit(f"FAIL: Battle 110 dialogue {cue}")
    for line in re.findall(r'\.string "([^"]*)"',section):
        visible=line.replace('\\n','').replace('\\l','').replace('\\p','').replace('$','')
        if len(visible)>36: raise SystemExit(f"FAIL: Battle 110 overlong {visible}")

    manifest=json.loads((ROOT/'docs/verdant_doubles_manifest.json').read_text())['formats']['TRAINER_MAXIE_MT_CHIMNEY']
    if manifest!={"format":"double","target_size":6,"archetype":"Summit ridge command","difficulty":100,"partner_interaction":True,"level_offset":3,"location":"Mt Chimney"}: raise SystemExit("FAIL: Battle 110 manifest")
    anchor=json.loads(FACTION.read_text())['designs']['MT_CHIMNEY_MAXIE']
    if anchor['status']['source']!='source-closed' or [m['species'] for m in anchor['team']]!=[m['species'] for m in TEAM] or anchor['difficulty']['observed'] is not None: raise SystemExit("FAIL: Battle 110 anchor")
    reservation=next(r for r in json.loads(RESERVATIONS.read_text())['marquee_blueprints']['entries'] if r.get('anchor')=='MT_CHIMNEY_MAXIE')
    if reservation['design_commitment']!='spent' or reservation['target_difficulty']!=10: raise SystemExit("FAIL: Battle 110 reservation")
    ids={json.loads(line)['reference_id'] for line in CORPUS.read_text().splitlines()}
    if any(ref not in ids for ref in REFERENCES): raise SystemExit("FAIL: Battle 110 reference")


def main() -> None:
    parser=argparse.ArgumentParser(); parser.add_argument('--write',action='store_true'); parser.add_argument('--check',action='store_true'); args=parser.parse_args()
    if not args.write and not args.check: parser.error('choose --write or --check')
    payloads=expected_payloads(); paths=(DESIGNS,LEDGER,SEQUENCE,OS_PATH); texts=[json.dumps(p,indent=2,ensure_ascii=False)+'\n' for p in payloads]
    if args.write:
        for path,text in zip(paths,texts): path.write_text(text)
    if args.check:
        for path,text in zip(paths,texts):
            if path.read_text()!=text: raise SystemExit(f"FAIL: Battle 110 stale {path.name}")
        verify_source(); guide=json.loads((ROOT/'docs/verdant_battle_guide.json').read_text())['entries']; entry=next(r for r in guide if r['trainerId']=='TRAINER_MAXIE_MT_CHIMNEY')
        if entry['designStatus']!='closed' or entry['format']!='double' or entry['partySize']!=6: raise SystemExit('FAIL: Battle 110 guide')
    print('PASS: Battle 110 Maxie base-Groudon ridge boss is source-closed')


if __name__=='__main__': main()
