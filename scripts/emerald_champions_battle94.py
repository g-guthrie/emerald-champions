#!/usr/bin/env python3
"""Generate/check Battle 94, Sophie and Coby's Route 113 hazard-phazing lane."""

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

TEAMS = {
    "TRAINER_SOPHIE": [
        {
            "level": 1,
            "species": "SPECIES_FORRETRESS",
            "item": "ITEM_RED_CARD",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_BRAVE",
            "moves": ["MOVE_SPIKES", "MOVE_RAPID_SPIN", "MOVE_GYRO_BALL", "MOVE_DRILL_RUN"],
        },
        {
            "level": 2,
            "species": "SPECIES_RUNERIGUS",
            "item": "ITEM_LEFTOVERS",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_BRAVE",
            "moves": ["MOVE_STEALTH_ROCK", "MOVE_POLTERGEIST", "MOVE_EARTHQUAKE", "MOVE_WILL_O_WISP"],
        },
        {
            "level": 3,
            "species": "SPECIES_DELPHOX",
            "item": "ITEM_LIFE_ORB",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_FIRE_BLAST", "MOVE_PSYCHIC", "MOVE_DAZZLING_GLEAM", "MOVE_GRASS_KNOT"],
        },
    ],
    "TRAINER_COBY": [
        {
            "level": 1,
            "species": "SPECIES_CROBAT",
            "item": "ITEM_BLACK_SLUDGE",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_WHIRLWIND", "MOVE_BRAVE_BIRD", "MOVE_CROSS_POISON", "MOVE_ROOST"],
        },
        {
            "level": 2,
            "species": "SPECIES_TOUCANNON",
            "item": "ITEM_SITRUS_BERRY",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_BEAK_BLAST", "MOVE_BULLET_SEED", "MOVE_ROCK_BLAST", "MOVE_KNOCK_OFF"],
        },
        {
            "level": 3,
            "species": "SPECIES_HONCHKROW",
            "item": "ITEM_SCOPE_LENS",
            "ability_slot": 1,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_BRAVE_BIRD", "MOVE_SUCKER_PUNCH", "MOVE_SUPERPOWER", "MOVE_ROOST"],
        },
    ],
}

REFERENCES = [
    "showdown:gen4randomdoublesbattle:019",
    "showdown:gen8randombattle:023",
    "showdown:gen9randomdoublesbattle:019",
    "showdown:gen5randomdoublesbattle:026",
    "showdown:gen7randomdoublesbattle:019",
    "showdown:gen4randomdoublesbattle:028",
]

NEXT = {
    "index": 95,
    "encounter_id": "BATTLE_095_ROUTE_114_NOLAN",
    "location": "Route114",
    "category": "optional east-pond Fisherman double",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_NOLAN"],
    "access_note": (
        "After Fallarbor, Nolan is the eastern Route 114 Fisherman at (25,6). His sight range is zero, so he is "
        "a direct-interaction optional encounter near the entrance/pond rather than a forced sight battle. His "
        "source record forces a double; Battle 95 must verify the script's two-Pokemon guard before the southbound lane."
    ),
}


def design() -> dict:
    return {
        "guide_order": 94,
        "trainer_ids": ["TRAINER_SOPHIE", "TRAINER_COBY"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Final Route 113 west-edge trainer lane before Fallarbor. Sophie and Coby patrol opposite directions on "
            "one vertical corridor and may form a native two-opponent double or either independent split single."
        ),
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 fully evolved route-finale formation",
            "effective_levels": "41, 42, and 43 on each half",
            "eligible_ratio": "6/6",
            "mega_access": True,
            "status": "pass",
            "reason": "Forretress, Runerigus, Delphox, Crobat, Toucannon, and Honchkrow are all natural final forms.",
        },
        "manual_quality": 10,
        "manual_difficulty": 9.2,
        "branch_contract": {
            "joint": {
                "format": "two-opponent native double",
                "trainers": ["TRAINER_SOPHIE", "TRAINER_COBY"],
                "members": [member["species"] for team in TEAMS.values() for member in team],
                "target_difficulty": 9.2,
                "contract": (
                    "Forretress/Crobat opens. Spikes or Red Card movement combines with Whirlwind; every Coby reserve "
                    "is Flying, so Runerigus's later Earthquake remains partner-safe."
                ),
            },
            "splits": {"TRAINER_SOPHIE": 8.6, "TRAINER_COBY": 8.6},
            "one_usable_policy": (
                "Both scripts remain singles, so either three-member split is legal with one usable player Pokemon; "
                "the ordinary approach engine forms the pair only when the player can field a double."
            ),
        },
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {
                    "reference_id": reference_id,
                    "decision": "selected exact role; full donor rejected",
                    "reason": (
                        "Each record supplies one hazard, phazing, contact, special-closer, or bird-cleaner role. The "
                        "physical joint/split topology and hazard-to-Whirlwind sequence are hand-authored."
                    ),
                }
                for reference_id in REFERENCES
            ],
            "decision": "Six exact competitive records support six locally legal, fresh members in one source-native formation.",
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Sturdy Forretress keeps Spikes/Rapid Spin and gains legal Gyro Ball/Drill Run with a public Red Card."},
            {"reference_id": REFERENCES[1], "adaptation": "Wandering Spirit Runerigus keeps Poltergeist, burn, and Ground pressure while Stealth Rock replaces Toxic Spikes."},
            {"reference_id": REFERENCES[2], "adaptation": "Delphox keeps immediate Fire/Psychic offense, dropping Nasty Plot/Protect for four-way legal coverage and Magic Guard Life Orb."},
            {"reference_id": REFERENCES[3], "adaptation": "Infiltrator Crobat retains exact Whirlwind pressure, trading Toxic for physical STAB and Roost."},
            {"reference_id": REFERENCES[4], "adaptation": "Skill Link Toucannon keeps Beak Blast/Bullet Seed/Rock Blast and replaces Protect with Knock Off."},
            {"reference_id": REFERENCES[5], "adaptation": "Honchkrow keeps Super Luck offensive coverage, using Scope Lens and Roost rather than an itemless filler role."},
        ],
        "ordering": {
            "intended_lead": ["SPECIES_FORRETRESS", "SPECIES_CROBAT"],
            "source_order": {
                "TRAINER_SOPHIE": ["SPECIES_FORRETRESS", "SPECIES_RUNERIGUS", "SPECIES_DELPHOX"],
                "TRAINER_COBY": ["SPECIES_CROBAT", "SPECIES_TOUCANNON", "SPECIES_HONCHKROW"],
            },
            "reason": (
                "The joint opens hazard setter beside phazer. Sophie alone progresses trap to spinblocker to special "
                "closer; Coby alone progresses phazer to contact/multi-hit pressure to critical-hit cleaner."
            ),
        },
        "team_intent": (
            "Red Card Sturdy Forretress establishes Spikes and controls hazards. Black Sludge Infiltrator Crobat "
            "Whirlwinds targets through them. Runerigus adds Stealth Rock, burn, item-based Ghost damage, and a joint-safe "
            "Earthquake; Delphox supplies Magic Guard Life Orb special coverage. Toucannon punishes contact and uses Skill "
            "Link coverage/Knock Off; Scope Lens Super Luck Honchkrow closes through high-crit physical attacks."
        ),
        "intended_counterplay": (
            "Taunt, Magic Bounce, Defog, Rapid Spin, Court Change, Heavy-Duty Boots, or immediate Fire pressure deny "
            "Sophie's layers. Electric/Ice/Rock pressure challenges Coby's entire Flying half. Strong special Fire attacks "
            "bypass Forretress, Water/Grass/Ice/Ghost/Dark answer Runerigus by slot, Rock/Water/Ground/Dark answer Delphox, "
            "and priority/recoil/item removal constrain the birds. Focus the active phazer before switching voluntarily."
        ),
        "bespoke_ai": (
            "Both records use smart switching, partner help, HP awareness, and field control. The AI values unplaced "
            "hazards, Whirlwind only when a reserve exists and hazards add value, Rapid Spin when removal matters, Red Card, "
            "Wandering Spirit, Magic Guard Life Orb, Skill Link, Beak Blast contact state, and Super Luck. Coby's Flying "
            "half keeps Runerigus Earthquake ally-safe. No sleep, Protect, speed field, setup, evasion, or hidden read exists."
        ),
        "uniqueness": (
            "All six species are new to the first 93 closed encounters. This is Route 113's capstone formation: the "
            "shared moving corridor becomes a hazard conveyor belt jointly, while each three-member split retains a "
            "complete and different battle. It does not reuse Ben's Skarmory hazard maze."
        ),
        "story_logic": (
            "Sophie now scatters traps to stay awake without using sleep; Coby's wings literally force movement through "
            "them. Both intro/defeat/post-battle paths explain joint and split viability. The old drowsy surrender, generic "
            "wing boast, itemless slots, Attract, and uncoordinated five-member pair are gone."
        ),
        "reward_logic": "EXP and prize money only; neither trainer owns a rematch, item, story flag, or progression reward.",
        "campaign_reservations": {
            "spends": ["Route 113 hazard-phazing finale", "Red Card Forretress/Crobat lead", "Magic Guard Delphox closer", "Super Luck Honchkrow cleaner"],
            "preserves": ["protected Mega Chandelure", "Skarmory hazard repetition", "sleep teams", "future six-member boss hazard engines"],
            "repeat_rule": "These six should not recur soon; later hazard teams must change setter, forced-movement method, and removal counterplay.",
        },
        "author_self_check": {
            "strongest_part": "The two NPC movement lines become the battle: one lays the path and the other repeatedly pushes the player through it.",
            "weakest_link": (
                "A prepared removal or Magic Bounce plan can dismantle much of the joint engine, while Rock/Electric "
                "coverage compresses Coby's half. Six fresh optimized levels 41-43 and independent closers compensate, "
                "but those are intentionally broad answers."
            ),
        },
        "closure": (
            "Battle 94 is source-closed at quality 10 across the native joint and both splits: six fresh legal levels "
            "41-43, six distinct items, exact patrol geometry, hazard/phazing and Earthquake safety, six competitive "
            "references, smart field AI, native-width dialogue, broad removal/Rock counterplay, and no sleep/Protect/"
            "speed/setup/reward debt. Joint target is 9.2, splits are 8.6, runtime remains unplayed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 94,
        "encounter_id": "BATTLE_094_ROUTE_113_SOPHIE_COBY",
        "identity": {
            "location": "Route113",
            "category": "optional west-edge moving native-pair cluster",
            "format": "native-pair double or two split singles",
            "strict_cap": 40,
            "memory_hook": "Sophie builds an ash trap; Coby's flock Whirlwinds the player through it. Each half still stands alone.",
        },
        "primary_player_question": "Can the player deny or remove the hazard engine before forced movement compounds it, while preserving answers for two independent three-member closers?",
        "tempo": "Six-member joint hazard/phazing conveyor or two three-member splits; finite layers, burn/contact pressure, one special closer, and one crit cleaner.",
        "pressure_sources": [
            "Sophie: level-41 Red Card Forretress, level-42 Leftovers Runerigus, level-43 Magic Guard Life Orb Delphox",
            "Coby: level-41 Black Sludge Crobat, level-42 Sitrus Skill Link Toucannon, level-43 Scope Lens Super Luck Honchkrow",
        ],
        "intentional_opening": "Joint opens Forretress/Crobat; splits preserve the exact same source-first lead.",
        "intentional_weakness": "Hazard removal/Magic Bounce/Taunt, shared Coby Rock/Electric/Ice seams, no speed field/setup/Protect, recoil, and item dependence.",
        "first_loss_lesson": "The damage came from movement through layers. Stop the phazer or remove the layers before switching into the two reserve closers.",
        "revealed_information": ["cap 40", "levels 41-43", "joint plus two splits", "all six fresh", "hazards plus Whirlwind", "joint-safe Earthquake", "no reward/rematch"],
        "counterplay_classes": ["Taunt/Magic Bounce/Defog/Rapid Spin/Court Change/Boots", "Rock/Electric/Ice into Coby", "Fire into Forretress", "Water/Grass/Ice/Ghost/Dark by Sophie slot", "priority/recoil/item removal"],
        "target_difficulty": 9.2,
        "difficulty_rationale": "Six optimized fresh members and a compounding public hazard engine make the joint severe; splits retain complete +1/+2/+3 teams. Broad removal and typed seams preserve learnability.",
        "tuning_knob": "Tune both level-43 closers to +2 first; preserve six species, items, geometry, and hazard/phazing identity.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": [
            "route-cluster", "native-pair-double", "split-singles", "hazards", "phazing", "red-card", "wandering-spirit",
            "magic-guard-life-orb", "skill-link", "beak-blast", "super-luck", "no-sleep", "no-protect", "no-speed-field",
            "no-setup", "no-mega", "no-legendary",
        ],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Six exact competitive roles recomposed around proven patrol geometry."},
        "author_self_check": {"strongest_part": "Map movement and battle movement express the same trap.", "weakest_link": "Removal and Rock coverage compress the engine; full level/item/closer strength compensates."},
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_094_ROUTE_113_SOPHIE_COBY"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [entry for entry in ledger["entries"] if entry["index"] != 94] + [ledger_entry()]
    ledger["entries"].sort(key=lambda entry: entry["index"])
    sequence = json.loads(SEQUENCE.read_text())
    sequence["entries"] = [entry for entry in sequence["entries"] if entry["index"] != 95] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda entry: entry["index"])
    for entry in sequence["entries"]:
        if entry["index"] <= 94:
            entry["status"] = "closed"
        elif entry["index"] == 95:
            entry["status"] = "next"
        else:
            entry["status"] = "queued"
    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update(
        {
            "closed_encounters": 94,
            "next_index": 95,
            "next_encounter_id": "BATTLE_095_ROUTE_114_NOLAN",
            "queued_sequence_entries": 0,
            "canonical_sequence_groups": 95,
            "physical_encounter_groups": 528,
            "unordered_physical_groups": 433,
        }
    )
    return designs, ledger, sequence, operating_system


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    blocks = doubles.trainer_blocks(trainers)
    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for trainer_id, expected in TEAMS.items():
        block = blocks[trainer_id].group(0)
        body = doubles.party_match(parties, doubles.party_name(block)).group(2)
        actual = [polish.parse_entry(entry) for entry in custom.party_entries(body)]
        if actual != expected:
            raise SystemExit(f"FAIL: Battle 94 source differs for {trainer_id}")
        for token in (".doubleBattle = FALSE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_FIELD_CONTROL"):
            if token not in block:
                raise SystemExit(f"FAIL: Battle 94 {trainer_id} missing {token}")
        for member in expected:
            illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
            if illegal:
                raise SystemExit(f"FAIL: Battle 94 illegal moves for {member['species']}: {illegal}")
            if member["ability_slot"] >= len(slots[member["species"]]):
                raise SystemExit(f"FAIL: Battle 94 invalid ability slot for {member['species']}")
    all_members = [member for team in TEAMS.values() for member in team]
    if len({member["species"] for member in all_members}) != 6 or len({member["item"] for member in all_members}) != 6:
        raise SystemExit("FAIL: Battle 94 species/items are not unique")
    if "MOVE_EARTHQUAKE" not in TEAMS["TRAINER_SOPHIE"][1]["moves"]:
        raise SystemExit("FAIL: Battle 94 joint-safe Earthquake missing")
    if any("TYPE_FLYING" not in (dex.stats[m["species"]].type1, dex.stats[m["species"]].type2) for m in TEAMS["TRAINER_COBY"]):
        raise SystemExit("FAIL: Battle 94 Coby half is no longer fully Ground-immune")

    scripts = (ROOT / "data/maps/Route113/scripts.inc").read_text()
    for trainer_id in TEAMS:
        if f"trainerbattle_single {trainer_id}" not in scripts:
            raise SystemExit(f"FAIL: Battle 94 split-capable script missing for {trainer_id}")
    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]
    for trainer_id in TEAMS:
        rule = manifest[trainer_id]
        if rule["format"] != "single" or rule["target_size"] != 3 or not rule["partner_interaction"]:
            raise SystemExit(f"FAIL: Battle 94 manifest stale for {trainer_id}")
        if rule["difficulty"] != 85 or rule["level_offset"] != 2:
            raise SystemExit(f"FAIL: Battle 94 tuning stale for {trainer_id}")

    map_data = json.loads((ROOT / "data/maps/Route113/map.json").read_text())["object_events"]
    geometry = {
        event["script"]: (event["x"], event["y"], event["movement_type"], str(event["trainer_sight_or_berry_tree_id"]))
        for event in map_data if event["script"] in {"Route113_EventScript_Sophie", "Route113_EventScript_Coby"}
    }
    if geometry != {
        "Route113_EventScript_Sophie": (7, 6, "MOVEMENT_TYPE_WALK_DOWN_AND_UP", "6"),
        "Route113_EventScript_Coby": (7, 13, "MOVEMENT_TYPE_WALK_UP_AND_DOWN", "6"),
    }:
        raise SystemExit(f"FAIL: Battle 94 patrol geometry drifted: {geometry}")

    dialogue_file = (ROOT / "data/text/trainers.inc").read_text()
    dialogue = dialogue_file.split("Route113_Text_CobyIntro:", 1)[1].split("Route113_Text_LawrenceIntro:", 1)[0]
    for cue in (
        "Crobat throws you through Sophie's",
        "flock fights alone",
        "Whirlwind",
        "critical hits",
        "scatter traps",
        "Delphox guards my traps alone",
        "No sleeping needed",
    ):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 94 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 94 overlong dialogue: {visible}")

    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in corpus_ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 94 competitive reference missing from corpus")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.write and not args.check:
        parser.error("choose --write or --check")
    payloads = expected_payloads()
    paths = (DESIGNS, LEDGER, SEQUENCE, OS_PATH)
    expected_text = [json.dumps(payload, indent=2, ensure_ascii=False) + "\n" for payload in payloads]
    if args.write:
        for path, text in zip(paths, expected_text):
            path.write_text(text)
    if args.check:
        for path, text in zip(paths, expected_text):
            if path.read_text() != text:
                raise SystemExit(f"FAIL: Battle 94 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
        for trainer_id, team in TEAMS.items():
            entry = next(row for row in guide["entries"] if row["trainerId"] == trainer_id)
            if entry["designStatus"] != "closed" or [m["speciesId"] for m in entry["party"]] != [m["species"] for m in team]:
                raise SystemExit(f"FAIL: Battle 94 guide stale for {trainer_id}")
    print("PASS: Battle 94 Sophie/Coby hazard-phazing lane is source-closed across joint and split branches")


if __name__ == "__main__":
    main()
