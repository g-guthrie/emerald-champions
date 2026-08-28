#!/usr/bin/env python3
"""Generate and verify Battle 121, Jace and Eli's volcanic-gravity pair."""

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

JACE_TEAM = [
    {"level": 1, "species": "SPECIES_LAMPENT", "item": "ITEM_BLUNDER_POLICY", "ability_slot": 2, "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID", "moves": ["MOVE_INFERNO", "MOVE_SHADOW_BALL", "MOVE_ENERGY_BALL", "MOVE_PROTECT"]},
    {"level": 3, "species": "SPECIES_FLAREON", "item": "ITEM_TOXIC_ORB", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_FACADE", "MOVE_FLARE_BLITZ", "MOVE_SUPERPOWER", "MOVE_QUICK_ATTACK"]},
    {"level": 5, "species": "SPECIES_GOUGING_FIRE", "item": "ITEM_BOOSTER_ENERGY", "ability_slot": 0, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_BURNING_BULWARK", "MOVE_RAGING_FURY", "MOVE_DRAGON_CLAW", "MOVE_MORNING_SUN"]},
    {"level": 6, "species": "SPECIES_MAROWAK_ALOLAN", "item": "ITEM_THICK_CLUB", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_FLARE_BLITZ", "MOVE_POLTERGEIST", "MOVE_BONEMERANG", "MOVE_PROTECT"]},
]

ELI_TEAM = [
    {"level": 2, "species": "SPECIES_CLAYDOL", "item": "ITEM_LIGHT_CLAY", "ability_slot": 0, "spread": "SPREAD_31_IV_HP_SPDEF_CALM", "moves": ["MOVE_GRAVITY", "MOVE_EARTH_POWER", "MOVE_ROCK_SLIDE", "MOVE_LIGHT_SCREEN"]},
    {"level": 4, "species": "SPECIES_COALOSSAL", "item": "ITEM_PASSHO_BERRY", "ability_slot": 0, "spread": "SPREAD_31_IV_HP_SPATK_MODEST", "moves": ["MOVE_TAR_SHOT", "MOVE_HEAT_WAVE", "MOVE_POWER_GEM", "MOVE_PROTECT"]},
    {"level": 6, "species": "SPECIES_TURTONATOR", "item": "ITEM_ROCKY_HELMET", "ability_slot": 1, "spread": "SPREAD_31_IV_HP_DEF_BOLD", "moves": ["MOVE_SHELL_TRAP", "MOVE_DRACO_METEOR", "MOVE_HEAT_WAVE", "MOVE_WIDE_GUARD"]},
    {"level": 7, "species": "SPECIES_GARGANACL", "item": "ITEM_LEFTOVERS", "ability_slot": 0, "spread": "SPREAD_31_IV_HP_DEF_SPDEF_SASSY", "moves": ["MOVE_SALT_CURE", "MOVE_RECOVER", "MOVE_WIDE_GUARD", "MOVE_HEAVY_SLAM"]},
]

TEAMS = {"TRAINER_JACE": JACE_TEAM, "TRAINER_ELI": ELI_TEAM}

REFERENCES = [
    "showdown:gen5randomdoublesbattle:008",
    "showdown:gen9championsrandomdoublesbattle:009",
    "showdown:gen7randomdoublesbattle:021",
    "elite:wolfe:players-cup-ii-2020",
    "showdown:gen7randombattle:002",
    "vgc:worlds-2017",
    "showdown:gen9championsrandomdoublesbattle:003",
]

NEXT = {
    "index": 122,
    "encounter_id": "BATTLE_122_LAVARIDGE_GYM_JEFF",
    "location": "LavaridgeTown_Gym_B1F",
    "category": "optional buried final B1F Kindler double",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_JEFF"],
    "access_note": "Jeff is the standalone buried trainer at (13,17) on B1F and the remaining physical trainer before Flannery.",
}


def design() -> dict:
    return {
        "guide_order": 121,
        "trainer_ids": ["TRAINER_JACE", "TRAINER_ELI"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": "Optional aligned buried Kindler/Hiker pair at x=4, y=18/16 on Lavaridge Gym B1F after Danielle.",
        "runtime_branches": [
            "Joint two-opponent double: Lampent and Claydol lead; the engine loads the first three members from each owner, for six total enemies.",
            "Jace-only single-trainer double: Lampent, Flareon, Gouging Fire, then split-only Alolan Marowak.",
            "Eli-only single-trainer double: Claydol, Coalossal, Turtonator, then split-only Garganacl.",
            "If one owner was defeated alone, the remaining owner retains the corresponding complete four-member double.",
        ],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 mature volcanic chamber with one intentional stone-gated middle form",
            "effective_levels": "joint 41/42/43/44/45/46; split closers 46/47",
            "eligible_ratio": "8/8",
            "mega_access": True,
            "status": "pass",
            "reason": "Lampent is a legitimate Dusk-Stone middle form; Flareon and Alolan Marowak are item/form evolutions; Claydol, Coalossal, and Garganacl have reached their ordinary thresholds; Gouging Fire and Turtonator are single-stage.",
        },
        "manual_quality": 10,
        "manual_difficulty": 9.7,
        "branch_difficulty": {"joint_double": 9.7, "jace_only_double": 9.1, "eli_only_double": 9.2},
        "observed_difficulty": None,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {"reference_id": REFERENCES[0], "decision": "Lampent role adapted", "reason": "The exact doubles record validates fast special Ghost/Fire pressure and Trick-style disruption; the local set converts that risk into Gravity-enabled Inferno and Blunder Policy."},
                {"reference_id": REFERENCES[1], "decision": "Flareon role adapted", "reason": "The Champions random doubles record validates direct physical Fire pressure and defensive utility; Guts plus Toxic Orb makes Jace's autonomous reserve more demanding."},
                {"reference_id": REFERENCES[2], "decision": "Claydol role adapted", "reason": "The exact doubles record validates bulky Levitate support with Earth Power and Protect-family tempo; local Gravity and Light Screen turn the mountain pull into the encounter's opening question."},
                {"reference_id": REFERENCES[3], "decision": "Coalossal role adapted; historic team preserved elsewhere", "reason": "Wolfe Glick's winning Steam Engine Coalossal proves the species' competitive ceiling. This ordinary trainer uses Passho, Tar Shot, and no self-activation or Weakness Policy, so the historic team is not copied or spent."},
                {"reference_id": REFERENCES[4], "decision": "Turtonator role adapted", "reason": "The generated set validates immediate special Fire/Dragon pressure; Shell Trap, Iron Barbs, Rocky Helmet, and Wide Guard create a visible contact deterrent without Shell Smash repetition."},
                {"reference_id": REFERENCES[5], "decision": "Alolan Marowak split closer adapted", "reason": "The 2017 Worlds corpus validates Thick Club Alolan Marowak as a serious competitive threat; it appears only in Jace's avoidable split branch."},
                {"reference_id": REFERENCES[6], "decision": "Garganacl split closer adapted", "reason": "The Champions doubles corpus validates Salt Cure, recovery, and guard utility; it appears only in Eli's avoidable split branch."},
            ],
            "decision": "Seven indexed references cover every inherited role. Gouging Fire has no indexed historic donor, so its legal curated Gen 9 identity is authored directly rather than assigned fake evidence.",
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Lampent exchanges old Choice trapping for a field-dependent Inferno branch whose miss activates Blunder Policy."},
            {"reference_id": REFERENCES[1], "adaptation": "Flareon becomes Jace's self-contained Guts reserve with priority and broad physical coverage."},
            {"reference_id": REFERENCES[2], "adaptation": "Claydol leads with Gravity and Light Screen while retaining mixed Ground/Rock pressure."},
            {"reference_id": REFERENCES[3], "adaptation": "Coalossal retains Steam Engine, Heat Wave, and Protect but rejects the winning team's self-activation, Dynamax, and Weakness Policy package."},
            {"reference_id": REFERENCES[4], "adaptation": "Turtonator rejects another Shell Smash and instead makes physical contact itself the puzzle."},
            {"reference_id": REFERENCES[5], "adaptation": "Thick Club Alolan Marowak supplies Jace's split-only fourth wave."},
            {"reference_id": REFERENCES[6], "adaptation": "Purifying Salt Garganacl supplies Eli's split-only fourth wave."},
            {"source": "curated Gen 9 legal data", "adaptation": "Booster Energy Gouging Fire uses Burning Bulwark, Raging Fury, Dragon Claw, and Morning Sun as a new rare ancient-fire reveal."},
        ],
        "ordering": {
            "joint_lead": ["SPECIES_LAMPENT", "SPECIES_CLAYDOL"],
            "jace_source_order": [member["species"] for member in JACE_TEAM],
            "eli_source_order": [member["species"] for member in ELI_TEAM],
            "joint_omissions": ["SPECIES_MAROWAK_ALOLAN", "SPECIES_GARGANACL"],
            "reason": "Claydol moves before Lampent at ordinary comparable investment and establishes Gravity for Inferno. The two-opponent engine owns three slots per trainer, so each fourth member is deliberately a split-branch closer rather than dead undocumented data.",
        },
        "team_intent": "The joint opening asks whether Claydol may spend a turn bending accuracy and grounding itself while Lampent threatens Inferno or a Blunder Policy recovery branch. Flareon and Coalossal change the pressure to Guts and Tar Shot; Gouging Fire and Turtonator then punish careless physical turns with Burning Bulwark, Iron Barbs, Rocky Helmet, and Shell Trap. Neither half depends on the other to function, and the split-only fourth waves are complete closers.",
        "primary_player_question": "Can the player deny or endure Gravity-enabled Inferno, then distinguish safe special pressure from physical contact while Tar Shot changes Fire matchups?",
        "intended_counterplay": "Taunt, fast focus into Claydol, Fire/burn immunity, Safeguard, Misty Terrain, Lum-style status protection, Protect, Wide Guard, special attacks, Water/Ground/Rock/Ghost/Dark/Grass/Ice/Fairy, Knock Off, Haze, phazing, hazards, and weather-neutral burst all work. Gravity grounds Claydol and removes its own Levitate safety; Lampent is still a middle form; Passho and Light Screen are finite; the team has no speed field, weather, room, redirection, Mega, or automatic recovery loop in the joint branch.",
        "bespoke_ai": "Both owners use smart switching, partner awareness, HP awareness, Combo Setup, and Field Control. Reusable AI now rewards Gravity when a partner exposes a legal low-accuracy attack, rewards that attack when Gravity is selected, and rejects reusing Gravity while the field is active. Ordinary scoring still chooses every move, target, switch, guard, and recovery turn; nothing is scripted.",
        "uniqueness": "This is the campaign's first Gravity-to-Inferno accuracy puzzle and first opponent Gouging Fire. It follows Dancer, Eruption, Flash Fire, and consumable-item battles without repeating their engines. Five of six joint enemies are Fire type, so the Gym identity remains explicit; Claydol is the geological support exception. The two extra split closers make branch topology meaningful rather than invisible filler.",
        "story_logic": "Jace's cooling emotions become combustion under Eli's pull. Eli's mountain/volcano admiration becomes Gravity, Tar Shot, and Shell Trap. Intro and post-battle dialogue name the actual opening, transition, physical deterrent, and broad special counterplay in native-width text.",
        "reward_logic": "Optional EXP and prize money only. Both trainer flags remain independent and a joint win sets both through the native two-opponent flow.",
        "campaign_reservations": {
            "spends": ["first Gravity-to-Inferno chamber", "first opponent Gouging Fire", "one evolved volcanic contact-deterrent pair"],
            "preserves": ["Flannery's Torkoal sun", "Flannery's slow mode", "Flannery's Mega Emboar", "historic Wolfe Coalossal self-activation team", "all Fire Megas"],
            "repeat_rule": "Do not repeat Gravity plus Inferno, Tar Shot plus Fire pressure, or the Gouging Fire/Turtonator deterrent finish soon.",
        },
        "author_self_check": {
            "strongest_part": "One map alignment, one competitive field interaction, and one visible volcano metaphor govern every joint and split branch.",
            "weakest_link": "Gravity has no immediate damage and the two physical deterrents can slow the finish. Six cap-plus joint enemies, Inferno pressure, Tar Shot, broad coverage, finite guard tools, and the absence of passive stall keep that weakness intentional rather than soft.",
        },
        "closure": "Battle 121 is source-closed at quality 10 and target difficulty 9.7: exact two-owner topology, all joint/split branches, eight legal cap+1 to +7 sets, seven real references, mostly Fire identity, reusable Gravity AI, truthful dialogue, broad counterplay, and no reward debt are proven. Runtime remains unplayed.",
    }


def ledger_entry() -> dict:
    return {
        "index": 121,
        "encounter_id": "BATTLE_121_LAVARIDGE_GYM_JACE_ELI",
        "identity": {"location": "LavaridgeTown_Gym_B1F", "category": "optional aligned buried native pair", "format": "joint two-opponent double or two four-member doubles", "strict_cap": 40, "memory_hook": "Claydol bends accuracy for Lampent before Tar Shot and two physical deterrents turn the volcanic chamber against instinct."},
        "primary_player_question": "Can Gravity-enabled Inferno be denied or endured before the player changes from physical instinct to safe special pressure?",
        "tempo": "Gravity/Inferno lead, Guts/Tar Shot middle, then rare ancient-fire and contact-deterrent joint finish; split branches add one closer each.",
        "pressure_sources": ["Gravity-enabled Inferno", "Blunder Policy branch", "Guts Flareon", "Tar Shot Coalossal", "Booster Energy Gouging Fire", "Iron Barbs Rocky Helmet Shell Trap Turtonator", "split-only Thick Club Marowak", "split-only Salt Cure Garganacl"],
        "intentional_opening": "Lampent and Claydol lead jointly; no Gravity, Inferno, target, switch, or guard is forced.",
        "intentional_weakness": "Claydol spends a field turn and becomes grounded; shared Water/Ground/Rock seams; no speed field, weather, room, redirection, Mega, or joint sustain loop.",
        "first_loss_lesson": "Stop Claydol before Gravity or carry a burn-safe line, then use special attacks and deliberate targeting rather than feeding Bulwark, Iron Barbs, Rocky Helmet, or Shell Trap.",
        "revealed_information": ["cap 40", "joint or split branches", "joint levels 41-46", "split closers 46-47", "Gravity", "Inferno", "Blunder Policy", "Tar Shot", "Gouging Fire", "Burning Bulwark", "Shell Trap", "no Mega", "no reward"],
        "counterplay_classes": ["Taunt/focus Claydol", "burn immunity/Safeguard/Misty Terrain/status curing", "Protect/Wide Guard/special attacks", "Water/Ground/Rock/Ghost/Dark/Grass/Ice/Fairy", "item removal/Haze/phazing/hazards"],
        "target_difficulty": 9.7,
        "difficulty_rationale": "The joint branch has six optimized cap+1 to +6 enemies and a coherent accuracy/contact sequence, but it exposes one setup turn, shared type seams, and no boss transformation. Split doubles add a fourth closer at +6/+7 while reducing simultaneous ownership complexity.",
        "tuning_knob": "Lower Turtonator +6 to +5 first, then Gouging Fire +5 to +4; for split-only tuning lower Garganacl +7 before Marowak +6. Preserve all species and mechanics.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["lavaridge-gym", "native-pair-double", "split-double-branches", "volcanic-gravity", "gravity-inferno", "lampent", "claydol", "flareon", "coalossal", "gouging-fire", "turtonator", "marowak-alolan", "garganacl", "tar-shot", "burning-bulwark", "shell-trap", "five-of-six-fire", "no-weather", "no-room", "no-mega"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Seven indexed species, Champions, VGC, and Wolfe references plus explicit disclosure of the Gouging Fire corpus gap."},
        "author_self_check": {"strongest_part": "Topology and strategy are the same idea.", "weakest_link": "A field turn and two deterrents can reduce tempo if the player answers correctly."},
    }


def payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_121_LAVARIDGE_GYM_JACE_ELI"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 121] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])
    sequence = json.loads(SEQUENCE.read_text())
    for row in sequence["entries"]:
        if row["index"] == 121:
            row.update({
                "category": "optional aligned volcanic-gravity native pair",
                "trainer_ids": ["TRAINER_JACE", "TRAINER_ELI"],
                "access_note": "Jace (4,18) and Eli (4,16) can jointly approach the player between them or be resolved as independent four-member doubles.",
            })
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 122] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 121 else "next" if row["index"] == 122 else "queued"
    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update({
        "closed_encounters": 121,
        "next_index": 122,
        "next_encounter_id": NEXT["encounter_id"],
        "canonical_sequence_groups": 122,
        "physical_encounter_groups": 522,
        "unordered_physical_groups": 400,
    })
    return designs, ledger, sequence, operating_system


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    blocks = doubles.trainer_blocks(trainers)
    local_dex = presets.LocalDex()
    ability_slots = doubles.base_ability_slots()
    for trainer_id, expected in TEAMS.items():
        block = blocks[trainer_id].group(0)
        actual = [
            polish.parse_entry(entry)
            for entry in custom.party_entries(doubles.party_match(parties, doubles.party_name(block)).group(2))
        ]
        if actual != expected:
            raise SystemExit(f"FAIL Battle 121 source party {trainer_id}")
        for fragment in (
            ".doubleBattle = TRUE",
            "AI_FLAG_HP_AWARE",
            "AI_FLAG_HELP_PARTNER",
            "AI_FLAG_SMART_SWITCHING",
            "AI_FLAG_COMBO_SETUP",
            "AI_FLAG_FIELD_CONTROL",
        ):
            if fragment not in block:
                raise SystemExit(f"FAIL Battle 121 {trainer_id} missing {fragment}")
        for member in expected:
            illegal = [move for move in member["moves"] if move not in local_dex.legal_moves(member["species"])]
            if illegal or member["ability_slot"] >= len(ability_slots[member["species"]]):
                raise SystemExit(f"FAIL Battle 121 legality {trainer_id}/{member['species']}: {illegal}")

    map_data = json.loads((ROOT / "data/maps/LavaridgeTown_Gym_B1F/map.json").read_text())
    sites = {event["script"]: event for event in map_data["object_events"]}
    expected_sites = {
        "LavaridgeTown_Gym_B1F_EventScript_Jace": (4, 18),
        "LavaridgeTown_Gym_B1F_EventScript_Eli": (4, 16),
    }
    for script, coordinates in expected_sites.items():
        event = sites.get(script)
        if event is None or (event["x"], event["y"]) != coordinates or event["trainer_type"] != "TRAINER_TYPE_BURIED":
            raise SystemExit(f"FAIL Battle 121 trigger topology {script}")

    battle_main = (ROOT / "src/battle_main.c").read_text()
    for fragment in (
        "if (gTrainers[trainerNum].partySize > 3)",
        "monsCount = 3;",
        "CreateNPCTrainerParty(&gEnemyParty[3], gTrainerBattleOpponent_B, FALSE);",
    ):
        if fragment not in battle_main:
            raise SystemExit(f"FAIL Battle 121 two-owner branch invariant {fragment}")

    ai = (ROOT / "src/battle_ai_main.c").read_text()
    for fragment in (
        "effect == EFFECT_GRAVITY",
        "HasMoveWithLowAccuracy(BATTLE_PARTNER(battlerAtk)",
        "HasMove(BATTLE_PARTNER(battlerAtk), MOVE_GRAVITY)",
        "score -= 10;",
    ):
        if fragment not in ai:
            raise SystemExit(f"FAIL Battle 121 Gravity AI {fragment}")

    scripts = (ROOT / "data/maps/LavaridgeTown_Gym_1F/scripts.inc").read_text()
    jace_dialogue = scripts.split("LavaridgeTown_Gym_B1F_Text_JaceIntro:", 1)[1].split("LavaridgeTown_Gym_B1F_Text_JeffIntro:", 1)[0]
    eli_dialogue = scripts.split("LavaridgeTown_Gym_B1F_Text_EliIntro:", 1)[1].split("LavaridgeTown_Gym_1F_Text_FlanneryIntro:", 1)[0]
    dialogue = jace_dialogue + eli_dialogue
    for cue in ("gravity", "Inferno", "Gouging Fire", "Claydol", "Coalossal", "Turtonator", "Tar Shot", "Shell Trap"):
        if cue not in dialogue:
            raise SystemExit(f"FAIL Battle 121 dialogue {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL Battle 121 dialogue width {visible!r}")

    reference_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    missing = [reference for reference in REFERENCES if reference not in reference_ids]
    if missing:
        raise SystemExit(f"FAIL Battle 121 corpus references {missing}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    expected = payloads()
    paths = (DESIGNS, LEDGER, SEQUENCE, OS_PATH)
    serialized = [json.dumps(payload, indent=2, ensure_ascii=False) + "\n" for payload in expected]
    if args.write:
        for path, text in zip(paths, serialized):
            path.write_text(text)
    if args.check:
        for path, text in zip(paths, serialized):
            if path.read_text() != text:
                raise SystemExit(f"FAIL Battle 121 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())["entries"]
        rows = [row for row in guide if row["trainerId"] in TEAMS]
        if len(rows) != 2 or any(row["designStatus"] != "closed" or row["partySize"] != 4 for row in rows):
            raise SystemExit("FAIL Battle 121 guide rows")
    print("PASS: Battle 121 Jace/Eli volcanic-gravity pair is source-closed")


if __name__ == "__main__":
    main()
