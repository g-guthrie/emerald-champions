#!/usr/bin/env python3
"""Generate and verify Battle 112, the Jagged Pass Magma guard."""
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

TEAM = [
    {"level": 1, "species": "SPECIES_STUNFISK_GALARIAN", "item": "ITEM_BINDING_BAND", "ability_slot": 0, "spread": "SPREAD_31_IV_HP_SPDEF_CAREFUL", "moves": ["MOVE_STOMPING_TANTRUM", "MOVE_SNAP_TRAP", "MOVE_YAWN", "MOVE_STEALTH_ROCK"]},
    {"level": 2, "species": "SPECIES_THIEVUL", "item": "ITEM_THROAT_SPRAY", "ability_slot": 2, "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID", "moves": ["MOVE_DARK_PULSE", "MOVE_PSYCHIC", "MOVE_SNARL", "MOVE_PARTING_SHOT"]},
    {"level": 2, "species": "SPECIES_GOLISOPOD", "item": "ITEM_MUSCLE_BAND", "ability_slot": 0, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_FIRST_IMPRESSION", "MOVE_LIQUIDATION", "MOVE_KNOCK_OFF", "MOVE_WIDE_GUARD"]},
    {"level": 3, "species": "SPECIES_DUGTRIO", "item": "ITEM_CHOICE_BAND", "ability_slot": 1, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_HIGH_HORSEPOWER", "MOVE_STONE_EDGE", "MOVE_SUCKER_PUNCH", "MOVE_NIGHT_SLASH"]},
]

REFERENCES = [
    "smogon:gen8nu:004",
    "showdown:gen8randombattle:030",
    "smogon:gen8uu:014",
    "showdown:gen8randomdoublesbattle:019",
]

NEXT = {
    "index": 113,
    "encounter_id": "BATTLE_113_JAGGED_PASS_DIANA",
    "location": "JaggedPass",
    "category": "optional lower-pass Picnicker rematch family",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_DIANA_1", "TRAINER_DIANA_2", "TRAINER_DIANA_3", "TRAINER_DIANA_4"],
    "access_note": "Diana faces up/right at (10,21) with sight three, 37 collision-walk steps from the upper entry. She follows Eric and the optional direct-interaction Magma guard in the proven ordinary descent geometry.",
}


def design() -> dict:
    return {
        "guide_order": 112,
        "trainer_ids": ["TRAINER_GRUNT_JAGGED_PASS"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": "Optional direct-interaction Magma guard at (16,19), after Eric and before Diana on the first Jagged Pass descent. His tile is not required by the shortest exit path, and the later Magma Emblem story state removes him.",
        "runtime_branches": ["Optional guarded double requiring two usable party members.", "After victory, the persistent defeat flag changes the interaction to post-battle strategy dialogue."],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 early-middle fully evolved control team",
            "effective_levels": "41, 42, 42, and 43",
            "eligible_ratio": "4/4",
            "mega_access": True,
            "status": "pass",
            "reason": "Galarian Stunfisk is single-stage; Thievul, Golisopod, and Dugtrio evolve well before their exact levels. The prior battle deliberately owned the final middle-stage showcase, so this Magma specialist returns to final forms without spending a Mega.",
        },
        "manual_quality": 10,
        "manual_difficulty": 9.1,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {"reference_id": "smogon:gen8nu:004", "decision": "Stunfisk role adapted", "reason": "Published Yawn and hazard control becomes the guard's Binding Band Snap Trap lead."},
                {"reference_id": "showdown:gen8randombattle:030", "decision": "Thievul role adapted", "reason": "Stakeout and Parting Shot establish the punish-and-retreat identity."},
                {"reference_id": "smogon:gen8uu:014", "decision": "Golisopod role adapted", "reason": "First Impression, Knock Off, and Emergency Exit support one sharp reserve wave."},
                {"reference_id": "showdown:gen8randomdoublesbattle:019", "decision": "Dugtrio role adapted", "reason": "Arena Trap and focused Ground pressure become the final exit seal."},
            ],
            "decision": "All 1005 references and the four authored species reviews were checked. No verbatim six-member team fit an optional Magma guard; four proven positional roles were recomposed around his concealment dialogue and exact four-slot battle.",
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Galarian Stunfisk keeps Yawn and entry punishment, replacing passive singles coverage with source-legal Snap Trap and Binding Band."},
            {"reference_id": REFERENCES[1], "adaptation": "Thievul keeps Stakeout and Parting Shot; Snarl activates Throat Spray before Dark/Psychic pressure."},
            {"reference_id": REFERENCES[2], "adaptation": "Golisopod keeps First Impression, Knock Off, and Emergency Exit; Wide Guard is the doubles-native fourth slot."},
            {"reference_id": REFERENCES[3], "adaptation": "Dugtrio keeps Arena Trap and priority, but uses focused High Horsepower instead of partner-damaging Earthquake."},
        ],
        "ordering": {
            "source_order": ["SPECIES_STUNFISK_GALARIAN", "SPECIES_THIEVUL", "SPECIES_GOLISOPOD", "SPECIES_DUGTRIO"],
            "reason": "The false-ground controller and switch punisher lead. Emergency Exit supplies the visible retreat, then the fastest Arena Trap user closes the exits. Smart switching may adapt reserves to the live board without erasing that opening lesson.",
        },
        "team_intent": "A concealment, trapping, and retreat guard rather than another Magma sun team: Snap Trap, Yawn, hazards, Stakeout, Parting Shot, Emergency Exit, Wide Guard, and Arena Trap make staying and switching different risks.",
        "primary_player_question": "Can the player decide which slot may safely stay or switch while Stunfisk and Thievul create conflicting escape pressure, then prevent Golisopod and Dugtrio from converting retreat into a trapped finish?",
        "intended_counterplay": "Ghosts ignore trapping moves; Flying and Levitate users evade Arena Trap; Taunt, Safeguard, sleep immunity, Rapid Spin, Magic Bounce, item removal, spread pressure, Fake Out, priority, Water, Grass, Fire, Fighting, Electric, Fairy, and focused damage all attack different seams. The player need not solve one exact order.",
        "bespoke_ai": "Smart switching and HP awareness are sufficient. Native AI values legal damage, Yawn, trapping, hazards, Parting Shot, Wide Guard, Choice lock, and low-HP switching; native abilities execute Stakeout, Throat Spray, Emergency Exit, and Arena Trap. No move, target, switch, or turn is forced.",
        "uniqueness": "Galarian Stunfisk, Thievul, and Golisopod are new to the first 111 physical encounters. Dugtrio returns 61 battles after Trick House Battle 51, now as a Choice Band focused closer rather than an Air Balloon Earthquake lead.",
        "story_logic": "The guard's rewritten dialogue now describes hidden footing, pinning, punished escape, and retreat. After defeat he truthfully names what each team member did and still hints at Team Magma's concealed Hideout.",
        "reward_logic": "EXP and prize money only. The encounter is optional, removes no progression reward, and sets its existing persistent defeat flag only after victory.",
        "campaign_reservations": {
            "spends": ["Jagged Pass hidden-ground trap-and-retreat lesson", "ordinary Galarian Stunfisk, Thievul, and Golisopod debuts"],
            "preserves": ["all Megas and legendary families", "Magma sun, eruption, and Primal identities", "full Shadow Tag and Perish structures"],
            "repeat_rule": "Do not repeat the exact Snap Trap/Yawn plus Stakeout opening or Emergency Exit into Arena Trap handoff; individual species may return only after a long gap in a materially different role.",
        },
        "author_self_check": {
            "strongest_part": "Every mechanic translates the guard's behavior—hide, pin, punish escape, retreat, seal the exit—without borrowing weather or another boss resource.",
            "weakest_link": "Stakeout cannot perfectly predict every voluntary switch at decision time, so the battle does not rely on that multiplier alone; Throat Spray Snarl, direct coverage, trap residual, Wide Guard, and Choice Band pressure remain independently real.",
        },
        "closure": "Battle 112 is source-closed at quality 10 and target 9.1: exact optional geometry and lifecycle, guarded two-mon deployment, four legal optimized levels 41-43, four distinct items, three debuts and one remote justified reuse, four indexed references, truthful native-width dialogue, broad counterplay, and no reward debt. Runtime remains unplayed.",
    }


def ledger_entry() -> dict:
    return {
        "index": 112,
        "encounter_id": "BATTLE_112_JAGGED_PASS_MAGMA_GUARD",
        "identity": {"location": "JaggedPass", "category": "optional direct-interaction Magma guard", "format": "double", "strict_cap": 40, "memory_hook": "The ground itself pins escape until an Emergency Exit hands the chase to Arena Trap."},
        "primary_player_question": "Can the player choose which slot may safely stay or switch under Snap Trap, Yawn, Stakeout, and hazards, then stop the retreat-to-Arena-Trap finish?",
        "tempo": "Hidden-ground control lead, Snarl/Parting Shot switch punishment, Emergency Exit priority reserve, then Choice Band Arena Trap closure.",
        "pressure_sources": ["Binding Band Snap Trap and Yawn", "Throat Spray Snarl plus Stakeout", "First Impression and Wide Guard", "Choice Band Arena Trap focused coverage"],
        "intentional_opening": "Galarian Stunfisk and Thievul are fixed as the first visible pair; reserves adapt only after that escape question is established.",
        "intentional_weakness": "No Protect, weather, speed field, Mega, legendary, redirection, sleep move, or setup sweeper; each member has common type and disruption answers.",
        "first_loss_lesson": "Do not reflexively switch both slots. Disable Stunfisk or Thievul, use trap-immune positioning, and preserve Water or Grass pressure for Dugtrio after Golisopod retreats.",
        "revealed_information": ["cap 40", "guarded double", "levels 41-43", "trap and retreat abilities", "no finite reward"],
        "counterplay_classes": ["Ghost/Flying/Levitate trap immunity", "Taunt/Safeguard/sleep immunity", "Rapid Spin/Magic Bounce", "Fake Out/priority/spread pressure", "Water/Grass/Fire/Fighting/Electric/Fairy", "item removal and focused damage"],
        "target_difficulty": 9.1,
        "difficulty_rationale": "Four optimized cap-plus-one-to-three Pokémon and overlapping positional mechanics make the optional guard severe, while no defensive Protect loop, speed field, Mega, or legendary leaves many public answers.",
        "tuning_knob": "Reduce Dugtrio from +3 to +2 first, then remove Choice Band; preserve the species, opening, and retreat/trap identity.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["jagged-pass", "optional-magma-guard", "hidden-ground", "snap-trap", "yawn", "stakeout", "parting-shot", "emergency-exit", "wide-guard", "arena-trap", "no-protect", "no-speed-field", "no-mega", "no-legendary"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Four indexed species-role references recomposed for one story-specific positional double."},
        "author_self_check": {"strongest_part": "The battle mechanics and guard dialogue are the same idea.", "weakest_link": "Stakeout prediction is supplemental, not required for the team to function."},
    }


def payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_112_JAGGED_PASS_MAGMA_GUARD"] = design()

    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 112] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])

    sequence = json.loads(SEQUENCE.read_text())
    for row in sequence["entries"]:
        if row["index"] == 112:
            row.update({
                "category": "optional direct-interaction Magma trap-and-retreat guard",
                "trainer_ids": ["TRAINER_GRUNT_JAGGED_PASS"],
                "access_note": "The visible direct-interaction guard at (16,19) is distance 29 from the upper entry, after Eric at distance 6 and before Diana at distance 37. The shortest exit path does not require his tile; the later Magma Emblem story state removes him.",
            })
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 113] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 112 else "next" if row["index"] == 113 else "queued"

    os_data = json.loads(OS_PATH.read_text())
    os_data["current_state"].update({
        "closed_encounters": 112,
        "next_index": 113,
        "next_encounter_id": NEXT["encounter_id"],
        "queued_sequence_entries": 0,
        "canonical_sequence_groups": 113,
        "physical_encounter_groups": 525,
        "unordered_physical_groups": 412,
    })
    return designs, ledger, sequence, os_data


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    trainer_block = doubles.trainer_blocks(trainers)["TRAINER_GRUNT_JAGGED_PASS"].group(0)
    party = [polish.parse_entry(entry) for entry in custom.party_entries(doubles.party_match(parties, doubles.party_name(trainer_block)).group(2))]
    if party != TEAM:
        raise SystemExit("FAIL: Battle 112 party drifted")
    for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HP_AWARE"):
        if token not in trainer_block:
            raise SystemExit(f"FAIL: Battle 112 trainer missing {token}")

    dex = presets.LocalDex()
    ability_slots = doubles.base_ability_slots()
    for member in TEAM:
        illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
        if illegal or member["ability_slot"] >= len(ability_slots[member["species"]]):
            raise SystemExit(f"FAIL: Battle 112 legality {member['species']} {illegal}")
    if len({member["item"] for member in TEAM}) != 4 or any("MOVE_PROTECT" in member["moves"] for member in TEAM):
        raise SystemExit("FAIL: Battle 112 item or Protect restraint drifted")

    script = (ROOT / "data/maps/JaggedPass/scripts.inc").read_text()
    event = script.split("JaggedPass_EventScript_MagmaHideoutGuard::", 1)[1].split("JaggedPass_EventScript_GuardDefeated::", 1)[0]
    for token in ("HasEnoughMonsForDoubleBattle", "PLAYER_HAS_TWO_USABLE_MONS", "JaggedPass_EventScript_GuardNeedsTwoMons", "trainerbattle_no_intro TRAINER_GRUNT_JAGGED_PASS", "setflag FLAG_BEAT_MAGMA_GRUNT_JAGGED_PASS"):
        if token not in event:
            raise SystemExit(f"FAIL: Battle 112 guarded script missing {token}")
    section = script.split("JaggedPass_Text_GruntIntro:", 1)[1].split("JaggedPass_Text_BoulderShakingInResponseToEmblem:", 1)[0]
    for cue in ("wrong footing", "pin you down", "punish escape", "You read every retreat", "Stunfisk held", "Thievul punished", "Golisopod retreated", "Dugtrio sealed"):
        if cue not in section:
            raise SystemExit(f"FAIL: Battle 112 dialogue missing {cue}")
    for raw_line in re.findall(r'\.string "([^"]*)"', section):
        visible = raw_line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 112 overlong dialogue: {visible}")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]["TRAINER_GRUNT_JAGGED_PASS"]
    expected_manifest = {"format": "double", "target_size": 4, "archetype": "Hidden-ground trap and retreat", "difficulty": 91, "partner_interaction": True, "level_offset": 2, "location": "Jagged Pass"}
    if manifest != expected_manifest:
        raise SystemExit("FAIL: Battle 112 manifest stale")

    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference not in corpus_ids for reference in REFERENCES):
        raise SystemExit("FAIL: Battle 112 competitive reference missing")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.write and not args.check:
        parser.error("choose --write or --check")
    generated = payloads()
    paths = (DESIGNS, LEDGER, SEQUENCE, OS_PATH)
    texts = [json.dumps(payload, indent=2, ensure_ascii=False) + "\n" for payload in generated]
    if args.write:
        for path, text in zip(paths, texts):
            path.write_text(text)
    if args.check:
        for path, text in zip(paths, texts):
            if path.read_text() != text:
                raise SystemExit(f"FAIL: Battle 112 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())["entries"]
        entry = next(row for row in guide if row["trainerId"] == "TRAINER_GRUNT_JAGGED_PASS")
        if entry["designStatus"] != "closed" or entry["format"] != "double" or entry["partySize"] != 4:
            raise SystemExit("FAIL: Battle 112 guide stale")
    print("PASS: Battle 112 Jagged Pass Magma trap-and-retreat guard is source-closed")


if __name__ == "__main__":
    main()
