#!/usr/bin/env python3
"""Generate and verify Battle 104, Angelina and Lucas's final Route 114 snow pair."""

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

LUCAS_TEAM = [
    {"level": 2, "species": "SPECIES_NINETALES_ALOLAN", "item": "ITEM_LIGHT_CLAY", "ability_slot": 2, "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID", "moves": ["MOVE_AURORA_VEIL", "MOVE_BLIZZARD", "MOVE_MOONBLAST", "MOVE_PROTECT"]},
    {"level": 3, "species": "SPECIES_BEARTIC", "item": "ITEM_LIFE_ORB", "ability_slot": 1, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_ICICLE_CRASH", "MOVE_CLOSE_COMBAT", "MOVE_AQUA_JET", "MOVE_PROTECT"]},
    {"level": 4, "species": "SPECIES_ARTICUNO", "item": "ITEM_CHARTI_BERRY", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_SPATK_MODEST", "moves": ["MOVE_BLIZZARD", "MOVE_HURRICANE", "MOVE_FREEZE_DRY", "MOVE_ROOST"]},
]

ANGELINA_TEAM = [
    {"level": 1, "species": "SPECIES_GLACEON", "item": "ITEM_ICY_ROCK", "ability_slot": 2, "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID", "moves": ["MOVE_HAIL", "MOVE_BLIZZARD", "MOVE_FREEZE_DRY", "MOVE_PROTECT"]},
    {"level": 3, "species": "SPECIES_SANDSLASH_ALOLAN", "item": "ITEM_WEAKNESS_POLICY", "ability_slot": 2, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_ICICLE_CRASH", "MOVE_IRON_HEAD", "MOVE_DRILL_RUN", "MOVE_PROTECT"]},
    {"level": 4, "species": "SPECIES_MAGNEZONE", "item": "ITEM_AIR_BALLOON", "ability_slot": 1, "spread": "SPREAD_31_IV_HP_SPATK_MODEST", "moves": ["MOVE_THUNDERBOLT", "MOVE_FLASH_CANNON", "MOVE_ELECTROWEB", "MOVE_PROTECT"]},
]

REFERENCES = [
    "showdown:gen9championsrandomdoublesbattle:024",
    "showdown:gen9randomdoublesbattle:001",
    "smogon:gen8uu:009",
    "showdown:gen9randomdoublesbattle:030",
    "showdown:gen8randombattle:001",
    "showdown:gen9randomdoublesbattle:020",
]

NEXT = {
    "index": 105,
    "encounter_id": "BATTLE_105_METEOR_FALLS_COURTNEY_GRUNT",
    "location": "MeteorFalls_1F_1R",
    "category": "required rival-assisted Team Magma multi battle",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_COURTNEY_METEOR_FALLS", "TRAINER_GRUNT_METEOR_FALLS"],
    "access_note": (
        "The Meteor Falls story scene branches across May or Brendan and each Hoenn starter, but every branch uses the "
        "same Courtney and Magma Grunt opponent records in a required multi_2_vs_2 battle."
    ),
}


def design() -> dict:
    return {
        "guide_order": 104,
        "trainer_ids": ["TRAINER_ANGELINA", "TRAINER_LUCAS_1"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Angelina and Lucas occupy the final Route 114 shelf at cap 40. Their overlapping physical sight lane can form "
            "a six-member native-pair double, while approaching either side separately leaves two complete three-member singles."
        ),
        "runtime_branches": [
            "Joint native-pair double: Lucas's Alolan Ninetales and Angelina's Glaceon lead, with all six source members available.",
            "Lucas split single: Alolan Ninetales, Beartic, Articuno.",
            "Angelina split single: Glaceon, Alolan Sandslash, Magnezone.",
        ],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 mature lower-mountain weather finale",
            "effective_levels": "Lucas 42/43/44; Angelina 41/43/44",
            "eligible_ratio": "6/6",
            "mega_access": True,
            "status": "pass",
            "reason": (
                "Alolan Ninetales and Sandslash use Ice Stones, Beartic evolves at 37, Glaceon uses an Ice Stone, and "
                "Magnezone evolves in New Mauville. Articuno is the deliberately rare single-stage mountain apex."
            ),
        },
        "manual_quality": 10,
        "manual_difficulty": 9.5,
        "branch_difficulty": {"joint_double": 9.5, "lucas_single": 8.9, "angelina_single": 8.8},
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {"reference_id": REFERENCES[0], "decision": "Alolan Ninetales set selected", "reason": "The Champions generator supplies the exact Snow Warning, Light Clay, Aurora Veil, Blizzard, Moonblast, Protect shell."},
                {"reference_id": REFERENCES[1], "decision": "Beartic set selected", "reason": "The exact doubles set supplies Slush Rush physical pressure, priority, Fighting coverage, Life Orb, and Protect."},
                {"reference_id": REFERENCES[2], "decision": "Articuno defensive role adapted", "reason": "Published UU evidence validates Roost and Ice pressure; Snow Cloak, Blizzard, Hurricane, and Charti are local mountain adaptations."},
                {"reference_id": REFERENCES[3], "decision": "Glaceon role selected", "reason": "The exact doubles set validates Blizzard, Freeze-Dry, Protect, and snow-speed pressure; Hail makes the split branch autonomous."},
                {"reference_id": REFERENCES[4], "decision": "Alolan Sandslash role selected", "reason": "The generated set validates Slush Rush physical Steel/Ice offense; unsupported Triple Axel/Rapid Spin are replaced with legal direct coverage."},
                {"reference_id": REFERENCES[5], "decision": "Magnezone set selected", "reason": "The exact doubles set supplies Thunderbolt, Flash Cannon, Electroweb, Protect, and a complete special closer."},
            ],
            "decision": (
                "All 1005 references were available. Six exact-species competitive references support the roles; the native "
                "joint/split topology, evolution lesson, and Articuno apex are locally authored for the physical shelf."
            ),
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Alolan Ninetales is retained nearly verbatim as Lucas's autonomous weather and screen lead."},
            {"reference_id": REFERENCES[1], "adaptation": "Beartic is retained nearly verbatim as the fast physical snow beneficiary."},
            {"reference_id": REFERENCES[2], "adaptation": "Articuno keeps Roost and special Ice identity, then becomes the Charti Snow Cloak mountain apex."},
            {"reference_id": REFERENCES[3], "adaptation": "Glaceon keeps the exact Blizzard/Freeze-Dry/Protect axis and adds Hail so Angelina works without Lucas."},
            {"reference_id": REFERENCES[4], "adaptation": "Alolan Sandslash keeps Slush Rush and Steel/Ice physical pressure, with Drill Run and Protect for native doubles safety."},
            {"reference_id": REFERENCES[5], "adaptation": "Magnezone keeps the exact Electric/Steel speed-control shell and gains Air Balloon plus Sturdy as the warm-weather fallback."},
        ],
        "ordering": {
            "joint_lead": ["SPECIES_NINETALES_ALOLAN", "SPECIES_GLACEON"],
            "lucas_source_order": [member["species"] for member in LUCAS_TEAM],
            "angelina_source_order": [member["species"] for member in ANGELINA_TEAM],
            "reason": (
                "The joint lead immediately exposes Snow Warning, Aurora Veil, and a Slush Rush Blizzard beneficiary. In the "
                "split, Glaceon can create its own extended weather. Each side then changes from special weather pressure to "
                "physical Slush Rush and ends on a non-Slush-Rush special apex."
            ),
        },
        "team_intent": (
            "Lucas opens Light Clay Snow Warning Alolan Ninetales, follows with Life Orb Slush Rush Beartic, and closes on "
            "Charti Snow Cloak Articuno. Angelina opens Icy Rock Slush Rush Glaceon with manual Hail, follows with Weakness "
            "Policy Slush Rush Alolan Sandslash, and closes on Air Balloon Sturdy Magnezone with Electroweb. Jointly, the "
            "Ninetales/Glaceon lead can choose Veil plus accurate Blizzard immediately; separately, both weather lines remain complete."
        ),
        "intended_counterplay": (
            "Fake Out, Taunt, weather replacement, Cloud Nine, Brick Break, Defog, Wide Guard, Trick Room, opposing speed "
            "control, priority, and Protect-stalling attack the shared engine. Fire, Fighting, Rock, and Steel are broad but not "
            "automatic: Water coverage, Moonblast, Hurricane, Charti, Slush Rush, Weakness Policy, Sturdy, Air Balloon, and "
            "Magnezone alter the correct target. Remove weather before racing Slush Rush; avoid gifting Sandslash its Policy; "
            "focus Articuno around Roost; break Magnezone's Balloon/Sturdy. No exact catch or turn script is required."
        ),
        "bespoke_ai": (
            "Both source records remain native singles for joint/split safety and use smart switching, partner awareness, HP "
            "awareness, Combo Setup, Speed Control, and Field Control. Native AI sets Aurora Veil only while weather permits, "
            "uses Hail when Angelina lacks snow, recognizes Slush Rush speed, chooses spread Blizzard around Wide Guard and "
            "accuracy state, protects from visible board value, and scores Electroweb, Roost, Policy, Balloon, and Sturdy normally."
        ),
        "uniqueness": (
            "Alolan Ninetales, Beartic, Articuno, Glaceon, and Alolan Sandslash are new to the first 103 physical encounters. "
            "Magnezone last appeared 15 battles earlier in a hazard-denial single; here it is a source-last Air Balloon speed "
            "control fallback. Articuno is the first legendary in eleven encounters. This is the first autonomous joint/split "
            "snow pair and does not reuse Lenny's sound, Mega, Tailwind, or Choice engine."
        ),
        "story_logic": (
            "Lucas's warning about winter mountains now names Snow Warning, Beartic, and Articuno. Angelina's evolution "
            "question now names Ice Stone species and New Mauville Magnezone. Both post-battle speeches truthfully teach their "
            "own split team; neither falsely claims the other trainer is required. Geometry, optional status, and no-reward flow remain native."
        ),
        "reward_logic": "EXP and prize money only; neither trainer grants an item, flag, Match Call registration, or progression reward.",
        "campaign_reservations": {
            "spends": ["first native joint/split snow expedition", "first Articuno opponent showcase", "first Alolan Ninetales", "first Beartic", "first Glaceon", "first Alolan Sandslash"],
            "preserves": ["every Ice Mega", "Kyurem and Calyrex identities", "Glacia's League anchor", "hail chip/stall bosses", "all primal/weather-legends"],
            "repeat_rule": "These five fresh species should not recur soon; later Articuno or snow teams must change format, setter, and central counterplay.",
        },
        "author_self_check": {
            "strongest_part": "One exact physical shelf supports a stronger joint Blizzard/Veil battle and two fully autonomous weather singles without changing map geometry.",
            "weakest_link": "Shared Ice weaknesses are intentionally broad. Veil, speed, coverage, items, the Articuno apex, and Magnezone force correct sequencing, but weather denial and Wide Guard remain honest high-value answers."
        },
        "closure": (
            "Battle 104 is source-closed at quality 10: the target 9.5 joint and 8.9/8.8 splits use six legal optimized "
            "levels 41-44, six distinct items, five fresh species, one distant role-changed repeat, one first-use legendary, "
            "exact joint/split routing, six indexed references, branch-truthful native-width dialogue, broad counterplay, and "
            "zero reward debt. Runtime remains unplayed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 104,
        "encounter_id": "BATTLE_104_ROUTE_114_ANGELINA_LUCAS",
        "identity": {"location": "Route114", "category": "optional final-shelf native-pair cluster", "format": "native-pair double or two split singles", "strict_cap": 40, "memory_hook": "Lucas's Snow Warning expedition joins Angelina's cold evolutions for Veil, Blizzard, two Slush Rush axes, Articuno, and Magnezone."},
        "primary_player_question": "Can the player remove snow or Veil before two Slush Rush lanes take over, while preserving the right answers for Articuno and Air Balloon Magnezone?",
        "tempo": "Six-member joint snow double or two autonomous three-member weather singles: field lead, physical Slush Rush middle, then special apex.",
        "pressure_sources": ["Light Clay Snow Warning Alolan Ninetales Aurora Veil", "Icy Rock Slush Rush Glaceon manual Hail/Blizzard", "Life Orb Slush Rush Beartic", "Weakness Policy Slush Rush Alolan Sandslash", "Charti Snow Cloak Articuno", "Air Balloon Sturdy Magnezone Electroweb"],
        "intentional_opening": "Joint opens Ninetales+Glaceon; splits preserve the same source-first setter/beneficiary. Glaceon's Hail makes Angelina autonomous.",
        "intentional_weakness": "Weather dependence, visible Veil, four Ice bodies, Wide Guard vulnerability, shared Fire/Fighting/Rock/Steel pressure, avoidable Policy, and breakable Balloon/Sturdy.",
        "first_loss_lesson": "Change the field before racing the snow: stop or remove Veil, deny Slush Rush, and do not spend every Rock/Steel answer before Articuno and Magnezone appear.",
        "revealed_information": ["cap 40", "joint and split branches", "levels 41-44", "Snow Warning", "manual Hail", "Aurora Veil", "two Slush Rush physical reserves", "Articuno", "Air Balloon Sturdy Magnezone", "five fresh species", "no reward"],
        "counterplay_classes": ["Fake Out/Taunt/weather replacement/Cloud Nine", "Brick Break/Defog", "Wide Guard", "Trick Room/opposing speed control/priority", "Fire/Fighting/Rock/Steel", "Policy avoidance", "Balloon/Sturdy breaking", "Roost timing and focused damage"],
        "target_difficulty": 9.5,
        "difficulty_rationale": "The joint has six optimized levels 41-44, immediate weather/Veil, accurate spread pressure, two Slush Rush physical axes, one legendary, and a durable Electric/Steel closer. Branches remain severe but smaller and independently coherent.",
        "tuning_knob": "Tune both level-44 closers to +3 first, then Angelina's Sandslash +3 to +2; preserve species, items, weather, branch topology, and source order.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["route-cluster", "native-pair-double", "split-singles", "snow", "aurora-veil", "blizzard", "slush-rush", "ninetales-alola", "beartic", "articuno", "glaceon", "sandslash-alola", "magnezone", "manual-weather-fallback", "five-fresh-species", "one-legendary", "no-mega", "no-sleep", "no-trap", "no-manual-setup"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Six exact species references; joint/split weather topology is local."},
        "author_self_check": {"strongest_part": "Joint and split formats share one transparent snow lesson without making either split incomplete.", "weakest_link": "Ice weakness compression is real and intentionally exploitable; field, speed, coverage, items, Articuno, and Magnezone force sequencing rather than one button."},
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_104_ROUTE_114_ANGELINA_LUCAS"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 104] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])

    sequence = json.loads(SEQUENCE.read_text())
    for row in sequence["entries"]:
        if row["index"] == 104:
            row.update({
                "category": "optional final-shelf native-pair snow cluster",
                "trainer_ids": ["TRAINER_ANGELINA", "TRAINER_LUCAS_1"],
                "access_note": "Angelina at (26,72) and Lucas at (30,72) can form one native-pair double or two split singles on Route 114's final shelf.",
            })
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 105] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 104 else "next" if row["index"] == 105 else "queued"

    os_data = json.loads(OS_PATH.read_text())
    os_data["current_state"].update({
        "closed_encounters": 104,
        "next_index": 105,
        "next_encounter_id": NEXT["encounter_id"],
        "queued_sequence_entries": 0,
        "canonical_sequence_groups": 105,
        "physical_encounter_groups": 526,
        "unordered_physical_groups": 421,
    })
    return designs, ledger, sequence, os_data


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    blocks = doubles.trainer_blocks(trainers)
    for trainer_id, team in (("TRAINER_LUCAS_1", LUCAS_TEAM), ("TRAINER_ANGELINA", ANGELINA_TEAM)):
        block = blocks[trainer_id].group(0)
        actual = [polish.parse_entry(entry) for entry in custom.party_entries(doubles.party_match(parties, doubles.party_name(block)).group(2))]
        if actual != team:
            raise SystemExit(f"FAIL: Battle 104 source party differs for {trainer_id}")
        for token in (".doubleBattle = FALSE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_COMBO_SETUP", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL"):
            if token not in block:
                raise SystemExit(f"FAIL: Battle 104 {trainer_id} missing {token}")

    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    combined = LUCAS_TEAM + ANGELINA_TEAM
    for member in combined:
        illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
        if illegal:
            raise SystemExit(f"FAIL: Battle 104 illegal moves for {member['species']}: {illegal}")
        if member["ability_slot"] >= len(slots[member["species"]]):
            raise SystemExit(f"FAIL: Battle 104 invalid ability slot for {member['species']}")
    if len({m["species"] for m in combined}) != 6 or len({m["item"] for m in combined}) != 6:
        raise SystemExit("FAIL: Battle 104 species/items are not unique")

    map_data = json.loads((ROOT / "data/maps/Route114/map.json").read_text())["object_events"]
    geometry = {row["script"]: (row["x"], row["y"], row["movement_type"], str(row["trainer_sight_or_berry_tree_id"])) for row in map_data if row.get("script") in {"Route114_EventScript_Angelina", "Route114_EventScript_Lucas"}}
    expected_geometry = {
        "Route114_EventScript_Angelina": (26, 72, "MOVEMENT_TYPE_FACE_DOWN_AND_RIGHT", "4"),
        "Route114_EventScript_Lucas": (30, 72, "MOVEMENT_TYPE_FACE_LEFT", "4"),
    }
    if geometry != expected_geometry:
        raise SystemExit("FAIL: Battle 104 native-pair geometry drifted")
    route = (ROOT / "data/maps/Route114/scripts.inc").read_text()
    for trainer_id in ("TRAINER_ANGELINA", "TRAINER_LUCAS_1"):
        if f"trainerbattle_single {trainer_id}" not in route:
            raise SystemExit(f"FAIL: Battle 104 split source missing {trainer_id}")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]
    expected_manifest = {
        "TRAINER_LUCAS_1": {"format": "single", "target_size": 3, "archetype": "Winter mountain trio", "difficulty": 89, "partner_interaction": True, "level_offset": 3, "location": "Route 114"},
        "TRAINER_ANGELINA": {"format": "single", "target_size": 3, "archetype": "Cold-evolution trio", "difficulty": 88, "partner_interaction": True, "level_offset": 3, "location": "Route 114"},
    }
    for trainer_id, expected in expected_manifest.items():
        if manifest[trainer_id] != expected:
            raise SystemExit(f"FAIL: Battle 104 manifest stale for {trainer_id}")

    text = (ROOT / "data/text/trainers.inc").read_text()
    sections = text.split("Route114_Text_LucasIntro:", 1)[1].split("Route114_Text_ShaneIntro:", 1)[0] + text.split("Route114_Text_AngelinaIntro:", 1)[1].split("Route115_Text_TimothyIntro:", 1)[0]
    for cue in ("Ninetales calls snow", "Beartic races", "Articuno owns the sky", "Snow Warning", "Aurora Veil", "Slush Rush", "Glaceon calls snow", "Magnezone seals", "Ice Stones", "New Mauville", "covers their weak spots"):
        if cue not in sections:
            raise SystemExit(f"FAIL: Battle 104 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', sections):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 104 overlong dialogue: {visible}")

    ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 104 competitive reference missing")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.write and not args.check:
        parser.error("choose --write or --check")
    payloads = expected_payloads()
    paths = (DESIGNS, LEDGER, SEQUENCE, OS_PATH)
    texts = [json.dumps(payload, indent=2, ensure_ascii=False) + "\n" for payload in payloads]
    if args.write:
        for path, text in zip(paths, texts):
            path.write_text(text)
    if args.check:
        for path, text in zip(paths, texts):
            if path.read_text() != text:
                raise SystemExit(f"FAIL: Battle 104 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
        entries = [row for row in guide["entries"] if row["trainerId"] in {"TRAINER_ANGELINA", "TRAINER_LUCAS_1"}]
        if len(entries) != 2 or any(row["designStatus"] != "closed" or row["format"] != "single" or row["partySize"] != 3 for row in entries):
            raise SystemExit("FAIL: Battle 104 guide stale")
    print("PASS: Battle 104 Angelina/Lucas joint-snow and split-single branches are source-closed")


if __name__ == "__main__":
    main()
