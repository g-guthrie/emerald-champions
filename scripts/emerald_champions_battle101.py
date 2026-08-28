#!/usr/bin/env python3
"""Generate and verify Battle 101, Steve's complete Match Call family."""

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


TYRANTRUM = mon(1, "SPECIES_TYRANTRUM", "ITEM_CHOICE_SCARF", 2, "SPREAD_31_IV_ATK_SPEED_JOLLY", ["MOVE_HEAD_SMASH", "MOVE_DRAGON_CLAW", "MOVE_CLOSE_COMBAT", "MOVE_CRUNCH"])
COPPERAJAH = mon(2, "SPECIES_COPPERAJAH", "ITEM_ASSAULT_VEST", 2, "SPREAD_31_IV_HP_ATK_BRAVE", ["MOVE_HEAVY_SLAM", "MOVE_HIGH_HORSEPOWER", "MOVE_POWER_WHIP", "MOVE_ROCK_SLIDE"])
TURTONATOR = mon(3, "SPECIES_TURTONATOR", "ITEM_WHITE_HERB", 0, "SPREAD_31_IV_HP_SPATK_QUIET", ["MOVE_SHELL_SMASH", "MOVE_HEAT_WAVE", "MOVE_DRAGON_PULSE", "MOVE_PROTECT"])
GOLEM_BALLOON = mon(4, "SPECIES_GOLEM_ALOLAN", "ITEM_AIR_BALLOON", 2, "SPREAD_31_IV_HP_ATK_ADAMANT", ["MOVE_RETURN", "MOVE_ROCK_SLIDE", "MOVE_HEAVY_SLAM", "MOVE_FIRE_PUNCH"])
NIDOKING = mon(1, "SPECIES_NIDOKING", "ITEM_LIFE_ORB", 2, "SPREAD_31_IV_SPATK_SPEED_TIMID", ["MOVE_EARTH_POWER", "MOVE_SLUDGE_BOMB", "MOVE_ICE_BEAM", "MOVE_FLAMETHROWER"])
HERACROSS_GUTS = mon(4, "SPECIES_HERACROSS", "ITEM_TOXIC_ORB", 1, "SPREAD_31_IV_ATK_SPEED_JOLLY", ["MOVE_MEGAHORN", "MOVE_CLOSE_COMBAT", "MOVE_KNOCK_OFF", "MOVE_PROTECT"])

TEAM_1 = [TYRANTRUM, COPPERAJAH, TURTONATOR, GOLEM_BALLOON]
TEAM_2 = [NIDOKING, COPPERAJAH, TURTONATOR, HERACROSS_GUTS]
TEAM_3 = [
    mon(1, "SPECIES_TURTONATOR", "ITEM_WHITE_HERB", 0, "SPREAD_31_IV_HP_SPATK_QUIET", ["MOVE_SHELL_SMASH", "MOVE_HEAT_WAVE", "MOVE_DRAGON_PULSE", "MOVE_PROTECT"]),
    mon(2, "SPECIES_GOLEM_ALOLAN", "ITEM_CHOICE_BAND", 2, "SPREAD_31_IV_HP_ATK_ADAMANT", ["MOVE_RETURN", "MOVE_ROCK_SLIDE", "MOVE_HEAVY_SLAM", "MOVE_FIRE_PUNCH"]),
    mon(3, "SPECIES_TYRANTRUM", "ITEM_CHOICE_SCARF", 2, "SPREAD_31_IV_ATK_SPEED_JOLLY", ["MOVE_HEAD_SMASH", "MOVE_DRAGON_CLAW", "MOVE_CLOSE_COMBAT", "MOVE_CRUNCH"]),
    mon(4, "SPECIES_NIDOKING", "ITEM_LIFE_ORB", 2, "SPREAD_31_IV_SPATK_SPEED_TIMID", ["MOVE_EARTH_POWER", "MOVE_SLUDGE_BOMB", "MOVE_ICE_BEAM", "MOVE_FLAMETHROWER"]),
]
TEAM_4 = [
    mon(1, "SPECIES_COPPERAJAH", "ITEM_ASSAULT_VEST", 2, "SPREAD_31_IV_HP_ATK_BRAVE", ["MOVE_HEAVY_SLAM", "MOVE_HIGH_HORSEPOWER", "MOVE_POWER_WHIP", "MOVE_ROCK_SLIDE"]),
    mon(1, "SPECIES_NIDOKING", "ITEM_LIFE_ORB", 2, "SPREAD_31_IV_SPATK_SPEED_TIMID", ["MOVE_EARTH_POWER", "MOVE_SLUDGE_BOMB", "MOVE_ICE_BEAM", "MOVE_FLAMETHROWER"]),
    mon(2, "SPECIES_TURTONATOR", "ITEM_WHITE_HERB", 0, "SPREAD_31_IV_HP_SPATK_QUIET", ["MOVE_SHELL_SMASH", "MOVE_HEAT_WAVE", "MOVE_DRAGON_PULSE", "MOVE_PROTECT"]),
    mon(2, "SPECIES_GOLEM_ALOLAN", "ITEM_CHOICE_BAND", 2, "SPREAD_31_IV_HP_ATK_ADAMANT", ["MOVE_RETURN", "MOVE_ROCK_SLIDE", "MOVE_HEAVY_SLAM", "MOVE_FIRE_PUNCH"]),
    mon(3, "SPECIES_TYRANTRUM", "ITEM_CHOICE_SCARF", 2, "SPREAD_31_IV_ATK_SPEED_JOLLY", ["MOVE_HEAD_SMASH", "MOVE_DRAGON_CLAW", "MOVE_CLOSE_COMBAT", "MOVE_CRUNCH"]),
    mon(4, "SPECIES_HERACROSS", "ITEM_HERACRONITE", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", ["MOVE_PIN_MISSILE", "MOVE_ROCK_BLAST", "MOVE_ARM_THRUST", "MOVE_BULLET_SEED"]),
]

TEAMS = {
    "TRAINER_STEVE_1": TEAM_1,
    "TRAINER_STEVE_2": TEAM_2,
    "TRAINER_STEVE_3": TEAM_3,
    "TRAINER_STEVE_4": TEAM_4,
}

REFERENCES = [
    "showdown:gen6randomdoublesbattle:008",
    "smogon:gen8nu:002",
    "showdown:gen7randombattle:002",
    "showdown:gen7randomdoublesbattle:005",
    "showdown:gen7randomdoublesbattle:001",
    "showdown:gen9championsrandomdoublesbattle:017",
]

NEXT = {
    "index": 102,
    "encounter_id": "BATTLE_102_ROUTE_114_BERNIE",
    "location": "Route114",
    "category": "optional rotating Kindler rematch family",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_BERNIE_1"],
    "access_note": (
        "Bernie rotates counterclockwise at (30,58) with three-tile sight just east of Steve. His first physical encounter "
        "feeds its own four-record Match Call family and is next before Lenny."
    ),
}


def design() -> dict:
    return {
        "guide_order": 101,
        "trainer_ids": list(TEAMS),
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Steve's first optional Route 114 battle is reached at cap 40 from his upward-facing three-tile sight line. Match "
            "Call rematches unlock only after five badges and 255 qualifying steps, so records 2-4 are earliest at cap 45 and "
            "remain cap-relative if fought later."
        ),
        "runtime_branches": [
            "TRAINER_STEVE_1: guarded four-member double at cap 40, levels 41-44.",
            "TRAINER_STEVE_2: first guarded four-member rematch double, earliest cap 45, levels cap+1 through cap+4.",
            "TRAINER_STEVE_3: second guarded four-member rematch double, cap-relative +1 through +4.",
            "TRAINER_STEVE_4: repeatable six-member final rematch double, cap-relative +1/+1/+2/+2/+3/+4 with one Mega.",
        ],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 initial collection and five-badge cap-45+ rematch family",
            "effective_levels": "initial 41-44; rematches earliest 46-49, final 46/46/47/47/48/49",
            "eligible_ratio": "18/18 source slots",
            "mega_access": True,
            "status": "pass",
            "reason": (
                "Tyrantrum evolves at 39; Copperajah at 34; Turtonator and Heracross are single-stage; Alolan Golem and "
                "Nidoking use item/trade-style evolutions available before these caps. Heracrossite is reserved for the final "
                "record, after the player already has the Mega Bracelet."
            ),
        },
        "manual_quality": 10,
        "manual_difficulty": 9.0,
        "rematch_difficulty": {"TRAINER_STEVE_2": 9.2, "TRAINER_STEVE_3": 9.4, "TRAINER_STEVE_4": 9.6},
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {"reference_id": REFERENCES[0], "decision": "Tyrantrum commitment selected; full donor rejected", "reason": "Generated doubles evidence validates immediate Head Smash pressure; Steve uses Rock Head and a public Scarf rather than the unrelated legendary shell."},
                {"reference_id": REFERENCES[1], "decision": "Copperajah heavy breaker selected; full donor rejected", "reason": "Published NU balance validates Heavy Metal/Assault Vest physical weight without importing hazards and pivots."},
                {"reference_id": REFERENCES[2], "decision": "Turtonator setup role selected; full donor rejected", "reason": "Generated singles evidence validates Shell Smash as a real win condition; every Steve roster retains immediate attacks and one White Herb."},
                {"reference_id": REFERENCES[3], "decision": "Alolan Golem Galvanize pressure selected; full donor rejected", "reason": "Generated doubles evidence validates Galvanize and Rock spread without importing legends."},
                {"reference_id": REFERENCES[4], "decision": "Nidoking mixed-axis pressure selected; full donor rejected", "reason": "Generated doubles supplies exact Sheer Force special coverage and breaks the family's physical compression."},
                {"reference_id": REFERENCES[5], "decision": "Heracross final Mega selected; full donor rejected", "reason": "The Champions generator validates Heracross at Mega-era doubles stakes; Steve spends its unreserved Mega only in the final rematch."},
            ],
            "decision": (
                "All 1005 indexed references and the complete species reviews were examined. Six exact-species references "
                "support every engine; no full donor matches four escalating cap-relative records, so the collection/rematch "
                "progression is transparently authored by hand."
            ),
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Rock Head Tyrantrum becomes the public Scarf Head Smash commitment across the family."},
            {"reference_id": REFERENCES[1], "adaptation": "Heavy Metal Copperajah uses Assault Vest Heavy Slam/High Horsepower/Power Whip/Rock Slide as immediate physical mass."},
            {"reference_id": REFERENCES[2], "adaptation": "White Herb Shell Smash Turtonator owns the sole stat-setup clock and three direct fallbacks."},
            {"reference_id": REFERENCES[3], "adaptation": "Galvanize Alolan Golem translates Return into physical Electric pressure and later becomes one public Band lock."},
            {"reference_id": REFERENCES[4], "adaptation": "Life Orb Sheer Force Nidoking supplies Earth/Poison/Ice/Fire special coverage."},
            {"reference_id": REFERENCES[5], "adaptation": "Final Mega Heracross converts four legal multihit moves into a visible Skill Link climax."},
        ],
        "ordering": {
            "TRAINER_STEVE_1": {"lead": ["SPECIES_TYRANTRUM", "SPECIES_COPPERAJAH"], "reserves": ["SPECIES_TURTONATOR", "SPECIES_GOLEM_ALOLAN"], "reason": "Raw head/weight pressure opens before one setup and one transformed-Normal reserve."},
            "TRAINER_STEVE_2": {"lead": ["SPECIES_NIDOKING", "SPECIES_COPPERAJAH"], "reserves": ["SPECIES_TURTONATOR", "SPECIES_HERACROSS"], "reason": "The first rematch adds a mixed lead, then White Herb setup and finite Guts pressure."},
            "TRAINER_STEVE_3": {"lead": ["SPECIES_TURTONATOR", "SPECIES_GOLEM_ALOLAN"], "reserves": ["SPECIES_TYRANTRUM", "SPECIES_NIDOKING"], "reason": "Setup and a Band commitment open; Scarf physical and Life Orb special coverage punish the preserved answer."},
            "TRAINER_STEVE_4": {"lead": ["SPECIES_COPPERAJAH", "SPECIES_NIDOKING"], "reserves": ["SPECIES_TURTONATOR", "SPECIES_GOLEM_ALOLAN", "SPECIES_TYRANTRUM", "SPECIES_HERACROSS"], "reason": "Mixed mass opens, finite setup and two locks form the middle, and Mega Heracross is source-last as the only transformation."},
        },
        "team_intent": (
            "Steve's first double teaches four public engines: Scarf Rock Head Head Smash, Heavy Metal Heavy Slam, White Herb "
            "Shell Smash, and Galvanize Return. Rematch one adds Life Orb Sheer Force special coverage and Toxic Orb Guts. "
            "Rematch two rearranges the engines so setup and a Band lock arrive first. The final six-member double combines "
            "all five ordinary engines with one Mega Heracross whose Pin Missile, Rock Blast, Arm Thrust, and Bullet Seed become "
            "five-hit Skill Link attacks. Species repeat only inside this earned Match Call progression; each record changes the "
            "opening question and every member attacks independently."
        ),
        "intended_counterplay": (
            "Water, Ground, Fighting, Grass, Ice, Fairy, special bulk, burn, Intimidate, Wide Guard, item removal, priority, "
            "and focused damage are broad. Scout Tyrantrum/Golem Choice locks; exploit Head Smash type/recoil immunity limits; "
            "attack Copperajah specially and avoid feeding ideal Heavy Slam targets; Taunt, Haze, Unaware, phazing, priority, or "
            "immediate pressure deny Turtonator; pop Air Balloon before Ground in the first fight; exploit Nidoking's Life Orb "
            "and ordinary physical bulk; do not donate a safe Toxic Orb turn; and answer Mega Heracross with Flying/Fairy/Fire/"
            "Psychic, burn, Intimidate, special pressure, or damage before its multihit coverage selects the right target."
        ),
        "bespoke_ai": (
            "All four records are guarded doubles with smart switching, partner awareness, HP awareness, and Combo Setup. "
            "Existing AI evaluates Shell Smash only with survival/follow-up value, Mega Evolves Heracross normally, recognizes "
            "Choice locks, Sheer Force, Heavy Metal weight, Galvanize Return, Rock Head recoil removal, Guts status, Skill Link, "
            "spread Rock Slide/Heat Wave, and Protect. No action, target, setup, switch, Mega timing, or item activation is forced."
        ),
        "uniqueness": (
            "Tyrantrum, Copperajah, Turtonator, Alolan Golem, and Nidoking are new to the first 100 encounters and absent from "
            "protected anchors. Heracross last appeared 89 battles earlier and gains its first Mega/Skill Link role only in the "
            "final rematch. This is the first four-stage route rematch family authored as one evolving puzzle and uses no weather, "
            "room, terrain, sleep, redirection, trap, hazards, screens, healing loop, legendary, or second Mega."
        ),
        "story_logic": (
            "Steve's hard-body obsession now names the exact initial collection and explains Rock Head, Heavy Metal, Shell Smash, "
            "and Galvanize. Shared rematch text truthfully describes engines accumulating without claiming the final Mega is "
            "present early. The initial and dynamic rematch commands are both double-safe, Match Call registration and four-record "
            "table remain native, and no reward or story flag is added."
        ),
        "reward_logic": "Every record grants ordinary EXP and prize money only; registration is the sole progression. No item, shop, legendary, or Mega Stone is awarded.",
        "campaign_reservations": {
            "spends": ["Steve four-record engine collection", "Scarf Rock Head Tyrantrum", "Heavy Metal Copperajah", "Shell Smash Turtonator", "Galvanize Alolan Golem", "Sheer Force Nidoking", "final Mega Heracross Skill Link"],
            "preserves": ["all marquee Megas and legends", "protected Rhyperior/Aggron/Haxorus/Charizard roles removed from the old family", "weather and speed fields", "other fossil and monster collections"],
            "repeat_rule": "The six species may repeat inside Steve's family only. Outside this rematch progression, they require a materially different format or postgame role."
        },
        "author_self_check": {
            "strongest_part": "The family escalates through mechanics, ordering, mixed axes, roster size, and finally one Mega—not merely larger numbers—while every repeat is narratively earned.",
            "weakest_link": "Several bodies share Water/Ground/Fighting pressure and Steve has only one setup clock. Coverage, mixed Nidoking, locks, Air Balloon, Policy-free public items, six-body final depth, and cap-relative +1 to +4 levels keep those broad answers necessary without erasing them."
        },
        "closure": (
            "Battle 101's full family is source-closed at quality 10: initial target 9.0, rematches 9.2/9.4/9.6; all four "
            "records are guarded doubles; 18 legal cap-relative slots use six unreserved species, distinct per-party items, one "
            "final Mega, exact Match Call routing, six indexed references, native-width shared dialogue, broad counterplay, and "
            "zero reward debt. Runtime playtesting remains required before difficulty is observed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 101,
        "encounter_id": "BATTLE_101_ROUTE_114_STEVE",
        "identity": {"location": "Route114", "category": "optional Poké Maniac four-record Match Call family", "format": "four guarded doubles", "strict_cap": 40, "memory_hook": "Steve adds hard-body engines across four doubles until a six-member final collection ends in Mega Heracross."},
        "primary_player_question": "Can the player identify which weight, ability, setup, or Choice engine leads this rematch and preserve the correct mixed-axis answer for final Mega Heracross?",
        "tempo": "Four cap-relative doubles: four-engine introduction, mixed rematch, setup/lock rematch, then six-engine one-Mega final.",
        "pressure_sources": ["Rock Head Scarf Tyrantrum", "Heavy Metal Assault Vest Copperajah", "White Herb Shell Smash Turtonator", "Galvanize Alolan Golem Air Balloon/Band variants", "Life Orb Sheer Force Nidoking", "Guts Heracross before final Skill Link Mega Heracross"],
        "intentional_opening": "Every record has an authored lead and direct fallbacks; the first is cap 40, all rematches require five badges and are cap-relative from earliest cap 45.",
        "intentional_weakness": "Shared Water/Ground/Fighting pressure, one setup clock, two public Choice locks, Nidoking physical frailty, Heracross typed seams, no field/recovery loop, and only one final Mega.",
        "first_loss_lesson": "Read the engine, not just the body. Scout locks, deny Turtonator, change category for Nidoking/Copperajah, and never let final Heracross choose five free hits into the right weakness.",
        "revealed_information": ["initial cap 40", "rematches five-badge cap 45+", "four guarded double records", "levels cap+1 to +4", "Rock Head", "Heavy Metal", "Shell Smash", "Galvanize", "Sheer Force", "Guts", "Choice locks", "final Mega Skill Link", "no item reward"],
        "counterplay_classes": ["Water/Ground/Fighting/Grass/Ice/Fairy", "special bulk and mixed damage", "Choice scouting/item removal", "Taunt/Haze/Unaware/phazing/priority", "burn/Intimidate/Reflect", "Wide Guard", "Air Balloon removal", "Flying/Fairy/Fire/Psychic into Heracross"],
        "target_difficulty": 9.0,
        "difficulty_rationale": "The initial four optimized levels 41-44 already form a serious double; rematches increase mixed axes, ordering pressure, roster depth, and one final Mega while retaining public seams. Final target is 9.6.",
        "tuning_knob": "Tune final Mega Heracross +4 to +3 first, then final Tyrantrum +3 to +2; preserve family species, items, formats, progression, and engine order.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["route-rematch-family", "four-guarded-doubles", "hard-body-collection", "tyrantrum", "copperajah", "turtonator", "golem-alola", "nidoking", "heracross", "rock-head", "heavy-metal", "shell-smash", "galvanize", "sheer-force", "guts", "choice-locks", "mega-heracross", "skill-link", "five-fresh-species", "no-weather", "no-sleep", "no-legendary"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Six exact indexed species references plus all-species reviews; four-stage rematch progression is local."},
        "author_self_check": {"strongest_part": "Every rematch changes the actual puzzle and the final Mega pays off a 90-battle Heracross absence.", "weakest_link": "Shared broad weaknesses remain; mixed axes, locks, setup, roster depth, items, and final Mega make them necessary rather than automatic."},
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_101_ROUTE_114_STEVE"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 101] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])
    sequence = json.loads(SEQUENCE.read_text())
    for row in sequence["entries"]:
        if row["index"] == 101:
            row.update(
                {
                    "category": "optional south-route Poké Maniac four-record Match Call family",
                    "trainer_ids": list(TEAMS),
                    "access_note": (
                        "Steve faces up at (20,56) with three-tile sight below Shane. One physical position owns his "
                        "initial record and all three sequential Match Call rematches."
                    ),
                }
            )
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 102] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        if row["index"] <= 101:
            row["status"] = "closed"
        elif row["index"] == 102:
            row["status"] = "next"
        else:
            row["status"] = "queued"
    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update({"closed_encounters": 101, "next_index": 102, "next_encounter_id": NEXT["encounter_id"], "queued_sequence_entries": 0, "canonical_sequence_groups": 102, "physical_encounter_groups": 527, "unordered_physical_groups": 425})
    return designs, ledger, sequence, operating_system


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    blocks = doubles.trainer_blocks(trainers)
    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for trainer_id, expected_team in TEAMS.items():
        block_text = blocks[trainer_id].group(0)
        body = doubles.party_match(parties, doubles.party_name(block_text)).group(2)
        actual = [polish.parse_entry(entry) for entry in custom.party_entries(body)]
        if actual != expected_team:
            raise SystemExit(f"FAIL: Battle 101 source party differs for {trainer_id}")
        for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_COMBO_SETUP"):
            if token not in block_text:
                raise SystemExit(f"FAIL: Battle 101 {trainer_id} missing {token}")
        if len({member["species"] for member in expected_team}) != len(expected_team) or len({member["item"] for member in expected_team}) != len(expected_team):
            raise SystemExit(f"FAIL: Battle 101 {trainer_id} duplicates species/items")
        for member in expected_team:
            illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
            if illegal:
                raise SystemExit(f"FAIL: Battle 101 illegal moves for {member['species']}: {illegal}")
            if member["ability_slot"] >= len(slots[member["species"]]):
                raise SystemExit(f"FAIL: Battle 101 invalid ability slot for {member['species']}")
    if sum(member["item"] == "ITEM_HERACRONITE" for member in TEAM_4) != 1 or any("ITEM_HERACRONITE" == member["item"] for team in (TEAM_1, TEAM_2, TEAM_3) for member in team):
        raise SystemExit("FAIL: Battle 101 Mega progression is not final-only")

    route = (ROOT / "data/maps/Route114/scripts.inc").read_text()
    if "trainerbattle_double TRAINER_STEVE_1" not in route or "trainerbattle_rematch_double TRAINER_STEVE_1" not in route:
        raise SystemExit("FAIL: Battle 101 initial/rematch scripts are not double-safe")
    if route.count("Route114_Text_SteveNotEnoughMons") < 2:
        raise SystemExit("FAIL: Battle 101 double guards missing")
    if "REMATCH(TRAINER_STEVE_1, TRAINER_STEVE_2, TRAINER_STEVE_3, TRAINER_STEVE_4, ROUTE114)" not in (ROOT / "src/battle_setup.c").read_text():
        raise SystemExit("FAIL: Battle 101 rematch table drifted")
    if "if (HasAtLeastFiveBadges())" not in (ROOT / "src/battle_setup.c").read_text():
        raise SystemExit("FAIL: Battle 101 five-badge rematch gate missing")

    object_event = next(row for row in json.loads((ROOT / "data/maps/Route114/map.json").read_text())["object_events"] if row.get("script") == "Route114_EventScript_Steve")
    if (object_event["x"], object_event["y"], object_event["movement_type"], str(object_event["trainer_sight_or_berry_tree_id"])) != (20, 56, "MOVEMENT_TYPE_FACE_UP", "3"):
        raise SystemExit("FAIL: Battle 101 Steve geometry drifted")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]
    expected_manifest = {
        "TRAINER_STEVE_1": {"format": "double", "target_size": 4, "archetype": "Hard-body engine opening", "difficulty": 90, "partner_interaction": True, "level_offset": 3, "location": "Route 114"},
        "TRAINER_STEVE_2": {"format": "double", "target_size": 4, "archetype": "Mixed engine rematch", "difficulty": 92, "partner_interaction": True, "level_offset": 3, "location": "Route 114"},
        "TRAINER_STEVE_3": {"format": "double", "target_size": 4, "archetype": "Setup and commitment rematch", "difficulty": 94, "partner_interaction": True, "level_offset": 3, "location": "Route 114"},
        "TRAINER_STEVE_4": {"format": "double", "target_size": 6, "archetype": "Six-engine Mega collection", "difficulty": 96, "partner_interaction": True, "level_offset": 2, "location": "Route 114"},
    }
    for trainer_id, expected in expected_manifest.items():
        if manifest[trainer_id] != expected:
            raise SystemExit(f"FAIL: Battle 101 manifest stale for {trainer_id}")

    dialogue = (ROOT / "data/text/trainers.inc").read_text().split("Route114_Text_SteveIntro:", 1)[1].split("Route114_Text_BernieIntro:", 1)[0]
    for cue in ("hard bodies", "Tyrantrum and Copperajah", "Rock Head", "Heavy Metal", "Shell Smash", "Galvanize", "new engines", "Sheer Force", "Choice locks", "Skill Link", "battles in pairs"):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 101 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 101 overlong dialogue: {visible}")

    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in corpus_ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 101 competitive reference missing")
    protected = "\n".join(path.read_text() for path in list((ROOT / "docs").glob("emerald_champions_*anchor_designs.json")) + list((ROOT / "docs/dossier_packets").glob("*.json")))
    for species in ("Tyrantrum", "Copperajah", "Turtonator", "Golem-Alola", "Nidoking", "Heracross"):
        if re.search(rf'"{re.escape(species)}"', protected):
            raise SystemExit(f"FAIL: Battle 101 spends protected anchor species {species}")


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
                raise SystemExit(f"FAIL: Battle 101 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
        entries = [row for row in guide["entries"] if row["trainerId"] in set(TEAMS)]
        if len(entries) != 4 or any(row["designStatus"] != "closed" or row["format"] != "double" for row in entries):
            raise SystemExit("FAIL: Battle 101 guide family stale")
        if {row["trainerId"]: row["partySize"] for row in entries} != {"TRAINER_STEVE_1": 4, "TRAINER_STEVE_2": 4, "TRAINER_STEVE_3": 4, "TRAINER_STEVE_4": 6}:
            raise SystemExit("FAIL: Battle 101 guide party sizes stale")
    print("PASS: Battle 101 Steve four-record hard-body family is source-closed")


if __name__ == "__main__":
    main()
