#!/usr/bin/env python3
"""Generate and verify Battle 113, Diana's complete Jagged Pass picnic family."""
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


def mon(level: int, species: str, item: str, ability: int, spread: str, moves: list[str]) -> dict:
    return {"level": level, "species": species, "item": item, "ability_slot": ability, "spread": spread, "moves": moves}


DELIBIRD = mon(1, "SPECIES_DELIBIRD", "ITEM_FOCUS_SASH", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", ["MOVE_FAKE_OUT", "MOVE_ICY_WIND", "MOVE_FREEZE_DRY", "MOVE_WATER_PULSE"])
VESPIQUEN = mon(2, "SPECIES_VESPIQUEN", "ITEM_ROCKY_HELMET", 1, "SPREAD_31_IV_HP_DEF_IMPISH", ["MOVE_ATTACK_ORDER", "MOVE_TAILWIND", "MOVE_HEAL_ORDER", "MOVE_DEFEND_ORDER"])
GOURGEIST = mon(3, "SPECIES_GOURGEIST", "ITEM_FLAME_ORB", 1, "SPREAD_31_IV_HP_SPATK_MODEST", ["MOVE_SHADOW_BALL", "MOVE_GIGA_DRAIN", "MOVE_WILL_O_WISP", "MOVE_LEECH_SEED"])
EELEKTROSS = mon(4, "SPECIES_EELEKTROSS", "ITEM_CHOICE_SPECS", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", ["MOVE_THUNDERBOLT", "MOVE_FLAMETHROWER", "MOVE_GIGA_DRAIN", "MOVE_VOLT_SWITCH"])
POLTEAGEIST = mon(2, "SPECIES_POLTEAGEIST", "ITEM_WHITE_HERB", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", ["MOVE_SHELL_SMASH", "MOVE_STORED_POWER", "MOVE_SHADOW_BALL", "MOVE_GIGA_DRAIN"])
VANILLUXE = mon(3, "SPECIES_VANILLUXE", "ITEM_WISE_GLASSES", 2, "SPREAD_31_IV_SPATK_SPEED_TIMID", ["MOVE_FREEZE_DRY", "MOVE_FLASH_CANNON", "MOVE_WATER_PULSE", "MOVE_ICY_WIND"])


def at_level(member: dict, level: int) -> dict:
    return {**member, "level": level}


TEAM_1 = [DELIBIRD, VESPIQUEN, GOURGEIST, EELEKTROSS]
TEAM_2 = [DELIBIRD, POLTEAGEIST, VANILLUXE, EELEKTROSS]
TEAM_3 = [at_level(VESPIQUEN, 1), at_level(VANILLUXE, 2), GOURGEIST, EELEKTROSS]
TEAM_4 = [DELIBIRD, at_level(POLTEAGEIST, 1), VESPIQUEN, at_level(GOURGEIST, 2), at_level(EELEKTROSS, 3), at_level(VANILLUXE, 4)]
TEAMS = {
    "TRAINER_DIANA_1": TEAM_1,
    "TRAINER_DIANA_2": TEAM_2,
    "TRAINER_DIANA_3": TEAM_3,
    "TRAINER_DIANA_4": TEAM_4,
}

REFERENCES = [
    "showdown:gen8randomdoublesbattle:017",
    "showdown:gen6randomdoublesbattle:005",
    "showdown:gen9championsrandomdoublesbattle:020",
    "smogon:gen5nu:004",
    "showdown:gen9championsrandomdoublesbattle:017",
    "showdown:gen9championsrandomdoublesbattle:006",
]

NEXT = {
    "index": 114,
    "encounter_id": "BATTLE_114_JAGGED_PASS_AUTUMN_JULIO",
    "location": "JaggedPass",
    "category": "optional central-ledge Picnicker and Triathlete native pair",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_AUTUMN", "TRAINER_JULIO"],
    "access_note": "Autumn at (14,25) faces right and Julio at (18,25) faces left, each with sight three. Their opposing central-ledge sight lines form one native joint double or two independent singles after Diana and before lower-pass Ethan.",
}


def design() -> dict:
    return {
        "guide_order": 113,
        "trainer_ids": list(TEAMS),
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": "Diana is the lower-pass Picnicker at (10,21), after Eric and the direct-interaction Magma guard. Her initial fight is cap 40; native Match Call rematches require five badges and are earliest at cap 45, remaining cap-relative if delayed.",
        "runtime_branches": ["DIANA_1: guarded four-member picnic double at cap 40.", "DIANA_2: first guarded four-member tea-service rematch, earliest cap 45.", "DIANA_3: second guarded four-member control-course rematch.", "DIANA_4: repeatable guarded six-member final picnic double."],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 mature outdoor specialists and five-badge rematches",
            "effective_levels": "initial 41-44; rematches earliest 46-49; final 46/46/47/47/48/49",
            "eligible_ratio": "18/18 source slots",
            "mega_access": True,
            "status": "pass",
            "reason": "Delibird is single-stage; Vespiquen, Gourgeist, Eelektross, Polteageist, and Vanilluxe are all naturally obtainable final forms by these exact levels. Battle 111 already owned the late-middle-stage beat, so Diana can serve a mature six-species menu without a Mega.",
        },
        "manual_quality": 10,
        "manual_difficulty": 8.9,
        "rematch_difficulty": {"TRAINER_DIANA_2": 9.2, "TRAINER_DIANA_3": 9.3, "TRAINER_DIANA_4": 9.6},
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [{"reference_id": reference, "decision": "species role selected; unrelated full donor rejected", "reason": "The reference proves one competitive ingredient, while Diana's four-course progression is locally authored around her picnic identity."} for reference in REFERENCES],
            "decision": "All 1005 references and all six authored species reviews were checked. Six exact role references were recomposed into an original four-stage picnic family rather than copying an unrelated tournament or random roster.",
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Delibird retains competitive Fake Out utility, replacing unavailable local Tailwind with Icy Wind, Freeze-Dry, and Water Pulse under Refrigerate."},
            {"reference_id": REFERENCES[1], "adaptation": "Vespiquen retains Attack Order and Tailwind; Emerald Champions' Intimidate and Rocky Helmet make it the basket guard."},
            {"reference_id": REFERENCES[2], "adaptation": "Gourgeist keeps Ghost/Grass pressure and Will-O-Wisp; local Flare Boost plus Flame Orb turns the picnic lantern into a special threat."},
            {"reference_id": REFERENCES[3], "adaptation": "Published Choice Specs Eelektross supplies Levitate, four-type special coverage, and Volt Switch without spending reserved Rotom-Mow."},
            {"reference_id": REFERENCES[4], "adaptation": "Champions random Polteageist validates White Herb Shell Smash and Stored Power at doubles stakes."},
            {"reference_id": REFERENCES[5], "adaptation": "Vanilluxe keeps Freeze-Dry and coverage but deliberately uses Weak Armor, not Snow Warning, to avoid another weather module."},
        ],
        "ordering": {
            "TRAINER_DIANA_1": {"lead": ["SPECIES_DELIBIRD", "SPECIES_VESPIQUEN"], "reserves": ["SPECIES_GOURGEIST", "SPECIES_EELEKTROSS"]},
            "TRAINER_DIANA_2": {"lead": ["SPECIES_DELIBIRD", "SPECIES_POLTEAGEIST"], "reserves": ["SPECIES_VANILLUXE", "SPECIES_EELEKTROSS"]},
            "TRAINER_DIANA_3": {"lead": ["SPECIES_VESPIQUEN", "SPECIES_VANILLUXE"], "reserves": ["SPECIES_GOURGEIST", "SPECIES_EELEKTROSS"]},
            "TRAINER_DIANA_4": {"lead": ["SPECIES_DELIBIRD", "SPECIES_POLTEAGEIST"], "reserves": ["SPECIES_VESPIQUEN", "SPECIES_GOURGEIST", "SPECIES_EELEKTROSS", "SPECIES_VANILLUXE"]},
        },
        "team_intent": "The initial picnic controls tempo with Refrigerate Fake Out/Icy Wind and Intimidate Tailwind, then pivots through Flare Boost burn/drain pressure and Choice Specs Eelektross. Rematch one converts Fake Out into a public Shell Smash opportunity; rematch two shifts to Intimidate, Tailwind, Weak Armor, and residual control. The final combines all six courses with one setup threat and adaptive reserves.",
        "primary_player_question": "Which course is Diana serving now—tempo, tea-service setup, or control—and can the player remove its enabling partner before the picnic pivots into its next ingredient?",
        "intended_counterplay": "Rock and Electric pressure punish the opening flyers; Taunt, Haze, phazing, priority, Unaware, Clear Smog, and double targeting answer Polteageist; Fire, Ghost, Dark, Flying, Poison, Steel, item removal, weather, Trick Room, Wide Guard, and special walls attack different courses. Focus Sash, White Herb, Flame Orb, Choice lock, Weak Armor, recovery, and Volt Switch are all visible/exploitable. No exact catch or forced turn is required.",
        "bespoke_ai": "All four reachable records use smart switching, HP awareness, Combo Setup, and contextual Speed Control. Native AI evaluates Fake Out, Icy Wind, Tailwind, healing, Shell Smash, status, Leech Seed, Choice lock, Volt Switch, Intimidate, Flare Boost, Weak Armor, and coverage from live board state. Fake Out may protect a setup opportunity but neither move is forced and every member can act independently.",
        "uniqueness": "All six species are new to the first 112 closed encounters and absent from protected future anchor teams. This is the first rematch family structured as a changing menu, and the first weatherless Vanilluxe, Flare Boost Gourgeist, Intimidate Vespiquen, and Refrigerate special Delibird showcase.",
        "story_logic": "Diana's picnic dialogue now truthfully names her initial four roles, promises a changed rematch menu, explains the Fake Out/Shell Smash possibility, and describes the final six-course mixture. Initial and rematch macros now enforce the double-battle two-mon requirement while preserving registration and native rematch routing.",
        "reward_logic": "Ordinary EXP, prize money, and Match Call registration only. No item or story reward is added.",
        "campaign_reservations": {
            "spends": ["Diana six-course picnic rematch family", "Delibird", "Vespiquen", "Gourgeist", "Eelektross", "Polteageist", "weatherless Vanilluxe"],
            "preserves": ["every protected future team", "Rotom-Mow rival reveal", "snow bosses", "all Megas and legendary families", "dedicated redirection and full setup offenses"],
            "repeat_rule": "These six may repeat inside Diana's own menu. Outside her family, avoid the Fake Out/Shell Smash lead and require a long gap plus a materially different role.",
        },
        "author_self_check": {
            "strongest_part": "A forgettable Picnicker now has a native, funny, strategically legible identity that changes across every rematch and showcases six underused species.",
            "weakest_link": "Five members deal primarily special damage. That is intentional public counterplay; Attack Order, status/residual pressure, Shell Smash Stored Power, Choice coverage, item seams, cap-plus levels, and six-body depth keep a special wall helpful rather than sufficient by itself.",
        },
        "closure": "Battle 113's full family is source-closed at quality 10: targets 8.9/9.2/9.3/9.6; all four reachable records are guarded doubles; 18 legal cap-relative slots use six fresh unreserved species and six distinct final-party items; Match Call routing, six indexed references, native-width dialogue, broad counterplay, and zero reward debt all agree. Runtime remains unplayed.",
    }


def ledger_entry() -> dict:
    return {
        "index": 113,
        "encounter_id": "BATTLE_113_JAGGED_PASS_DIANA",
        "identity": {"location": "JaggedPass", "category": "optional Picnicker four-record Match Call family", "format": "four guarded doubles", "strict_cap": 40, "memory_hook": "Diana changes a six-species picnic menu from tempo to tea-service setup to a full six-course final."},
        "primary_player_question": "Which course is active now, and can the player remove its enabling partner before Diana pivots to the next ingredient?",
        "tempo": "Four-course control opening, Fake Out tea-service rematch, Intimidate control rematch, then six-course final.",
        "pressure_sources": ["Refrigerate Fake Out and Icy Wind", "Intimidate Tailwind Vespiquen", "Flare Boost burn/drain Gourgeist", "Choice Specs Eelektross pivot", "White Herb Shell Smash Polteageist", "Weak Armor weatherless Vanilluxe"],
        "intentional_opening": "Each record has a distinct authored lead; Diana's final explicitly reprises Delibird plus Polteageist before all six ingredients become available.",
        "intentional_weakness": "Five special attackers, exposed flyer weaknesses, one setup threat, no weather/terrain/room/Mega/legendary, visible Sash/White Herb/Choice/Flame Orb seams, and no Protect on any member.",
        "first_loss_lesson": "Identify whether tempo, Shell Smash, or control is the current engine; disable the enabler, exploit the visible item, then preserve the right category/type answer for the reserves.",
        "revealed_information": ["initial cap 40", "five-badge rematches", "four guarded doubles", "levels cap+1 to +4", "six fresh species", "one setup threat", "no weather/Mega/reward"],
        "counterplay_classes": ["Rock/Electric flyer pressure", "Taunt/Haze/phazing/priority/Unaware/Clear Smog", "Fire/Ghost/Dark/Flying/Poison/Steel", "special walls and mixed damage", "item removal and Choice exploitation", "Trick Room/Wide Guard/focused damage"],
        "target_difficulty": 8.9,
        "difficulty_rationale": "Initial optimized levels 41-44 create a serious optional double; rematches add one public setup engine, changing leads, and six-body depth. The final target is 9.6 without weather, Mega, or legendary inflation.",
        "tuning_knob": "Reduce final Vanilluxe from +4 to +3 first, then Eelektross +3 to +2; preserve all six species, items, leads, and menu progression.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["jagged-pass", "route-rematch-family", "four-guarded-doubles", "six-course-picnic", "delibird", "vespiquen", "gourgeist", "eelektross", "polteageist", "vanilluxe", "fake-out-shell-smash", "weatherless-ice", "six-fresh-species", "no-protect", "no-weather", "no-mega", "no-legendary"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Six indexed species-role references plus authored reviews; menu progression is local."},
        "author_self_check": {"strongest_part": "Six underused species make one memorable character and four genuinely changing fights.", "weakest_link": "Special compression is intentional and remains exploitable."},
    }


def payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_113_JAGGED_PASS_DIANA"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 113] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])
    sequence = json.loads(SEQUENCE.read_text())
    for row in sequence["entries"]:
        if row["index"] == 113:
            row.update({"category": "optional lower-pass Picnicker four-record Match Call family", "trainer_ids": list(TEAMS), "access_note": "Diana faces up/right at (10,21) with sight three, 37 collision-walk steps from the upper entry. One physical position owns her initial record and all three native rematches."})
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 114] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 113 else "next" if row["index"] == 114 else "queued"
    os_data = json.loads(OS_PATH.read_text())
    os_data["current_state"].update({"closed_encounters": 113, "next_index": 114, "next_encounter_id": NEXT["encounter_id"], "queued_sequence_entries": 0, "canonical_sequence_groups": 114, "physical_encounter_groups": 524, "unordered_physical_groups": 410})
    return designs, ledger, sequence, os_data


def protected_anchor_species() -> set[str]:
    protected: set[str] = set()
    for path in ROOT.glob("docs/emerald_champions_*anchor_designs.json"):
        data = json.loads(path.read_text())
        for anchor in data.get("designs", {}).values():
            for member in anchor.get("team", []):
                if isinstance(member, dict) and isinstance(member.get("species"), str):
                    protected.add(member["species"])
    return protected


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    blocks = doubles.trainer_blocks(trainers)
    dex = presets.LocalDex()
    ability_slots = doubles.base_ability_slots()
    for trainer_id, team in TEAMS.items():
        block = blocks[trainer_id].group(0)
        actual = [polish.parse_entry(entry) for entry in custom.party_entries(doubles.party_match(parties, doubles.party_name(block)).group(2))]
        if actual != team:
            raise SystemExit(f"FAIL: Battle 113 party differs {trainer_id}")
        for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HP_AWARE", "AI_FLAG_COMBO_SETUP", "AI_FLAG_SPEED_CONTROL"):
            if token not in block:
                raise SystemExit(f"FAIL: Battle 113 {trainer_id} missing {token}")
        if len({member["species"] for member in team}) != len(team) or len({member["item"] for member in team}) != len(team):
            raise SystemExit(f"FAIL: Battle 113 duplicates {trainer_id}")
        for member in team:
            illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
            if illegal or member["ability_slot"] >= len(ability_slots[member["species"]]):
                raise SystemExit(f"FAIL: Battle 113 legality {member['species']} {illegal}")
            if "MOVE_PROTECT" in member["moves"]:
                raise SystemExit("FAIL: Battle 113 Protect restraint drifted")

    script = (ROOT / "data/maps/JaggedPass/scripts.inc").read_text()
    if "trainerbattle_double TRAINER_DIANA_1" not in script or "trainerbattle_rematch_double TRAINER_DIANA_1" not in script or script.count("JaggedPass_Text_DianaNeedTwoMons") < 3:
        raise SystemExit("FAIL: Battle 113 guarded double routing")
    if "REMATCH(TRAINER_DIANA_1, TRAINER_DIANA_2, TRAINER_DIANA_3, TRAINER_DIANA_4, JAGGED_PASS)" not in (ROOT / "src/battle_setup.c").read_text():
        raise SystemExit("FAIL: Battle 113 rematch row")
    obj = next(row for row in json.loads((ROOT / "data/maps/JaggedPass/map.json").read_text())["object_events"] if row.get("script") == "JaggedPass_EventScript_Diana")
    if (obj["x"], obj["y"], obj["movement_type"], str(obj["trainer_sight_or_berry_tree_id"])) != (10, 21, "MOVEMENT_TYPE_FACE_UP_AND_RIGHT", "3"):
        raise SystemExit("FAIL: Battle 113 geometry")

    dialogue = script.split("JaggedPass_Text_DianaIntro:", 1)[1].split("JaggedPass_Text_EthanIntro:", 1)[0]
    for cue in ("jagged picnic", "Delibird chills", "Vespiquen guards", "Gourgeist lights", "Eelektross powers", "Shell Smash", "control course", "final menu mixes all six", "two healthy"):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 113 dialogue missing {cue}")
    for raw_line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = raw_line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 113 overlong dialogue: {visible}")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]
    expected = {
        "TRAINER_DIANA_1": ("Four-course trail picnic", 89, 4, 3),
        "TRAINER_DIANA_2": ("Fake Out tea-service rematch", 92, 4, 3),
        "TRAINER_DIANA_3": ("Intimidate control-course rematch", 93, 4, 3),
        "TRAINER_DIANA_4": ("Six-course picnic final", 96, 6, 2),
    }
    for trainer_id, (archetype, difficulty, size, offset) in expected.items():
        if manifest[trainer_id] != {"format": "double", "target_size": size, "archetype": archetype, "difficulty": difficulty, "partner_interaction": True, "level_offset": offset, "location": "Jagged Pass"}:
            raise SystemExit(f"FAIL: Battle 113 manifest {trainer_id}")

    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference not in corpus_ids for reference in REFERENCES):
        raise SystemExit("FAIL: Battle 113 reference")
    used = {member["species"] for member in TEAM_4}
    collisions = sorted(used & protected_anchor_species())
    if collisions:
        raise SystemExit(f"FAIL: Battle 113 protected anchor collision {collisions}")


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
                raise SystemExit(f"FAIL: Battle 113 stale {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())["entries"]
        entries = [row for row in guide if row["trainerId"] in TEAMS]
        if len(entries) != 4 or any(row["designStatus"] != "closed" or row["format"] != "double" for row in entries):
            raise SystemExit("FAIL: Battle 113 guide")
        if {row["trainerId"]: row["partySize"] for row in entries} != {"TRAINER_DIANA_1": 4, "TRAINER_DIANA_2": 4, "TRAINER_DIANA_3": 4, "TRAINER_DIANA_4": 6}:
            raise SystemExit("FAIL: Battle 113 guide sizes")
    print("PASS: Battle 113 Diana six-course picnic family is source-closed")


if __name__ == "__main__":
    main()
