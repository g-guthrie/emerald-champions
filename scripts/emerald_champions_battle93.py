#!/usr/bin/env python3
"""Generate/check Battle 93, Dillon's living fault-line double."""

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
    {
        "level": 1,
        "species": "SPECIES_MAMOSWINE",
        "item": "ITEM_LIFE_ORB",
        "ability_slot": 2,
        "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
        "moves": ["MOVE_EARTHQUAKE", "MOVE_ICICLE_CRASH", "MOVE_KNOCK_OFF", "MOVE_SUPERPOWER"],
    },
    {
        "level": 2,
        "species": "SPECIES_MANDIBUZZ",
        "item": "ITEM_SITRUS_BERRY",
        "ability_slot": 1,
        "spread": "SPREAD_31_IV_HP_DEF_BOLD",
        "moves": ["MOVE_FOUL_PLAY", "MOVE_SNARL", "MOVE_ROOST", "MOVE_TAUNT"],
    },
    {
        "level": 3,
        "species": "SPECIES_GARGANACL",
        "item": "ITEM_LEFTOVERS",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_HP_DEF_BOLD",
        "moves": ["MOVE_SALT_CURE", "MOVE_HEAVY_SLAM", "MOVE_HAMMER_ARM", "MOVE_RECOVER"],
    },
    {
        "level": 4,
        "species": "SPECIES_TORTERRA",
        "item": "ITEM_YACHE_BERRY",
        "ability_slot": 1,
        "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
        "moves": ["MOVE_HIGH_HORSEPOWER", "MOVE_WOOD_HAMMER", "MOVE_ROCK_SLIDE", "MOVE_WIDE_GUARD"],
    },
]

REFERENCES = [
    "showdown:gen6randomdoublesbattle:021",
    "showdown:gen7randomdoublesbattle:002",
    "showdown:gen9championsrandomdoublesbattle:003",
    "showdown:gen6randomdoublesbattle:012",
]

NEXT = {
    "index": 94,
    "encounter_id": "BATTLE_094_ROUTE_113_SOPHIE_COBY",
    "location": "Route113",
    "category": "optional west-edge moving native-pair cluster",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_SOPHIE", "TRAINER_COBY"],
    "access_note": (
        "Sophie at (7,6) walks down/up and Coby at (7,13) walks up/down along the same west-edge vertical "
        "lane; both have six-tile sight and three-tile movement ranges. Timing exposes a native two-opponent "
        "double or either split single. One dossier must close every branch before Fallarbor Town."
    ),
}


def design() -> dict:
    return {
        "guide_order": 93,
        "trainer_ids": ["TRAINER_DILLON"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": "Optional west-ash Youngster double after Lao and before the final Sophie/Coby lane.",
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 mature geological layers",
            "effective_levels": "41, 42, 43, and 44",
            "eligible_ratio": "4/4",
            "mega_access": True,
            "status": "pass",
            "reason": "Mamoswine, Mandibuzz, Garganacl, and Torterra are all natural final forms by this phase.",
        },
        "manual_quality": 10,
        "manual_difficulty": 8.8,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {
                    "reference_id": reference_id,
                    "decision": "selected exact role; full donor rejected",
                    "reason": (
                        "Each generated record supplies one geological layer. Dillon's exact Earthquake/Flying lead "
                        "and mineral-to-living-soil reserve wave are source-local composition."
                    ),
                }
                for reference_id in REFERENCES
            ],
            "decision": (
                "Four exact doubles records support the roles. Garganacl's published Body Press is unavailable in "
                "the local move pool and is explicitly replaced by legal Heavy Slam/Hammer Arm rather than smuggled in."
            ),
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Thick Fat Mamoswine keeps Earthquake, Ice, Knock Off, and Fighting coverage without priority or Protect."},
            {"reference_id": REFERENCES[1], "adaptation": "Overcoat Mandibuzz keeps Taunt/Roost support and adds Foul Play/Snarl while remaining Earthquake-immune."},
            {"reference_id": REFERENCES[2], "adaptation": "Purifying Salt Garganacl keeps Recover and the defensive identity; unavailable Body Press becomes legal Salt Cure/Heavy Slam/Hammer Arm."},
            {"reference_id": REFERENCES[3], "adaptation": "Torterra keeps Yache, Wood Hammer, Ground/Rock pressure, and exact locally legal Wide Guard."},
        ],
        "ordering": {
            "intended_lead": ["SPECIES_MAMOSWINE", "SPECIES_MANDIBUZZ"],
            "intended_reserve_pair": ["SPECIES_GARGANACL", "SPECIES_TORTERRA"],
            "source_order": ["SPECIES_MAMOSWINE", "SPECIES_MANDIBUZZ", "SPECIES_GARGANACL", "SPECIES_TORTERRA"],
            "reason": (
                "Mandibuzz is naturally immune to Mamoswine's Earthquake. The reserve wave uses only single-target "
                "Ground pressure, so Garganacl and Torterra never damage each other."
            ),
        },
        "team_intent": (
            "Life Orb Thick Fat Mamoswine cracks the frozen crust with partner-safe Earthquake and mixed physical "
            "coverage. Overcoat Mandibuzz circles above, suppressing special damage with Snarl, punishing Attack with "
            "Foul Play, and denying setup with Taunt. Leftovers Purifying Salt Garganacl applies finite Salt Cure and "
            "heavy coverage; Yache Solid Rock Torterra supplies direct Ground/Grass/Rock damage and Wide Guard."
        ),
        "intended_counterplay": (
            "Water/Grass/Fighting/Steel and burn answer Mamoswine; Electric/Ice/Rock/Fairy and Taunt answer Mandibuzz; "
            "Water/Grass/Ground/Fighting and item removal answer Garganacl; special Ice remains Torterra's clearest answer "
            "despite Yache/Solid Rock. Use single-target attacks around Wide Guard, avoid feeding Foul Play with boosted "
            "Attack, and do not let Salt Cure turn switching into free damage."
        ),
        "bespoke_ai": (
            "Dillon uses smart switching, partner help, and HP awareness. Native target checks keep Earthquake beside "
            "Flying Mandibuzz and avoid ally damage. Snarl, Foul Play, Taunt, Recover, Salt Cure, Wide Guard, recoil, "
            "Yache, and Solid Rock use public state. No weather, sleep, Protect, speed field, setup, evasion ability, "
            "legendary, or hidden input read is present."
        ),
        "uniqueness": (
            "All four species are new to the first 92 closed encounters. This is the route's first geological-layer "
            "formation: permafrost, airborne scavenger, living salt, and living soil. It follows Lao's fast deception "
            "single with a slower positional double and no repeated identity mechanic."
        ),
        "story_logic": (
            "Dillon's generic claim that the earth is alive now names four living geological layers. Intro, defeat, "
            "post-battle, and new two-Pokemon guard text describe the actual order and native double. The old ash-in-"
            "eyelashes joke, invalid Pignite stage, itemless slots, and unguarded double are removed."
        ),
        "reward_logic": "EXP and prize money only; Dillon owns no item, rematch, story flag, or progression reward.",
        "campaign_reservations": {
            "spends": ["living fault-line layers", "Mamoswine/Mandibuzz Earthquake lead", "first Garganacl trainer showcase"],
            "preserves": ["protected Hydreigon", "Volcanion acquisition identity", "weather geology", "Ground legendary teams"],
            "repeat_rule": "These four should not recur soon; later geology must change the layer order and positioning question.",
        },
        "author_self_check": {
            "strongest_part": "Every visual layer has a distinct tactical job, and the only spread Ground move is provably partner-safe.",
            "weakest_link": (
                "Water/Grass/Ice/Fighting coverage can compress multiple layers. Full items, +1 through +4 levels, "
                "Mandibuzz control, Yache/Solid Rock, Salt Cure, and Wide Guard compensate without hiding those answers."
            ),
        },
        "closure": (
            "Battle 93 is source-closed at quality 10 and target difficulty 8.8: four fresh legal levels 41-44, four "
            "distinct items, partner-safe ordering, exact guarded double script, four competitive references with one "
            "explicit legality adaptation, native-width dialogue, smart AI, broad counterplay, no reward debt, and no "
            "weather/sleep/Protect/speed/setup/legendary inflation. Runtime remains unplayed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 93,
        "encounter_id": "BATTLE_093_ROUTE_113_DILLON",
        "identity": {
            "location": "Route113",
            "category": "optional west-ash Youngster double",
            "format": "double",
            "strict_cap": 40,
            "memory_hook": "Mamoswine cracks permafrost beneath Mandibuzz; Garganacl and Torterra answer as living salt and soil.",
        },
        "primary_player_question": (
            "Can the player solve partner-safe Earthquake and Mandibuzz control, then switch from immediate offense to "
            "Salt Cure/Wide Guard positioning without losing the right Ice answer for Torterra?"
        ),
        "tempo": "Four-member positional double: fast Ground/Ice plus control lead into slower mineral/soil reserve with one recovery and one guard.",
        "pressure_sources": [
            "level-41 Life Orb Thick Fat Mamoswine",
            "level-42 Sitrus Overcoat Mandibuzz",
            "level-43 Leftovers Purifying Salt Garganacl",
            "level-44 Yache Solid Rock Torterra",
        ],
        "intentional_opening": "Mamoswine/Mandibuzz is fixed; Earthquake is partner-safe. Garganacl/Torterra never use ally-damaging Ground moves.",
        "intentional_weakness": "Shared Water/Grass/Fighting/Ice seams, no setup/weather/speed field, recoil, item dependence, Taunt, and single-target play around Wide Guard.",
        "first_loss_lesson": "Do not attack the formation as one Ground team. Remove Mandibuzz's control, then preserve distinct Water/Grass/Ice/Fighting answers for each layer.",
        "revealed_information": ["cap 40", "guarded double", "levels 41-44", "all four species fresh", "partner-safe Earthquake", "Wide Guard", "no reward/rematch"],
        "counterplay_classes": ["Water/Grass/Fighting/Steel and burn", "Electric/Ice/Rock/Fairy", "Taunt/item removal", "single-target moves around Wide Guard", "special Ice into Torterra"],
        "target_difficulty": 8.8,
        "difficulty_rationale": (
            "Four optimized fresh levels 41-44, positional spread pressure, control, residual damage, recovery, and "
            "guarding create a hard route double. Broad typed seams and no setup/field keep it below a boss."
        ),
        "tuning_knob": "Tune Torterra from +4 to +3 first; preserve all four layers, items, and exact source order.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": [
            "route-double", "fault-line", "geological-layers", "partner-safe-earthquake", "mamoswine", "mandibuzz",
            "garganacl", "torterra", "salt-cure", "wide-guard", "no-weather", "no-sleep", "no-protect",
            "no-speed-field", "no-setup", "no-mega", "no-legendary",
        ],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Four exact generated doubles roles with local legality rechecked."},
        "author_self_check": {"strongest_part": "Every geological layer is visually and tactically distinct.", "weakest_link": "Typed coverage compresses layers; level/item/position advantages compensate honestly."},
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_093_ROUTE_113_DILLON"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [entry for entry in ledger["entries"] if entry["index"] != 93] + [ledger_entry()]
    ledger["entries"].sort(key=lambda entry: entry["index"])
    sequence = json.loads(SEQUENCE.read_text())
    sequence["entries"] = [entry for entry in sequence["entries"] if entry["index"] != 94] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda entry: entry["index"])
    for entry in sequence["entries"]:
        if entry["index"] <= 93:
            entry["status"] = "closed"
        elif entry["index"] == 94:
            entry["status"] = "next"
        else:
            entry["status"] = "queued"
    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update(
        {
            "closed_encounters": 93,
            "next_index": 94,
            "next_encounter_id": "BATTLE_094_ROUTE_113_SOPHIE_COBY",
            "queued_sequence_entries": 0,
            "canonical_sequence_groups": 94,
            "physical_encounter_groups": 528,
            "unordered_physical_groups": 434,
        }
    )
    return designs, ledger, sequence, operating_system


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    block = doubles.trainer_blocks(trainers)["TRAINER_DILLON"].group(0)
    body = doubles.party_match(parties, doubles.party_name(block)).group(2)
    actual = [polish.parse_entry(entry) for entry in custom.party_entries(body)]
    if actual != TEAM:
        raise SystemExit("FAIL: Battle 93 source party differs")
    for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE"):
        if token not in block:
            raise SystemExit(f"FAIL: Battle 93 missing {token}")

    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for member in TEAM:
        illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
        if illegal:
            raise SystemExit(f"FAIL: Battle 93 illegal moves for {member['species']}: {illegal}")
        if member["ability_slot"] >= len(slots[member["species"]]):
            raise SystemExit(f"FAIL: Battle 93 invalid ability slot for {member['species']}")
    if len({member["species"] for member in TEAM}) != 4 or len({member["item"] for member in TEAM}) != 4:
        raise SystemExit("FAIL: Battle 93 species/items are not unique")
    if TEAM[0]["moves"][0] != "MOVE_EARTHQUAKE" or TEAM[1]["species"] != "SPECIES_MANDIBUZZ":
        raise SystemExit("FAIL: Battle 93 partner-safe lead drifted")

    scripts = (ROOT / "data/maps/Route113/scripts.inc").read_text()
    command = (
        "trainerbattle_double TRAINER_DILLON, Route113_Text_DillonIntro, "
        "Route113_Text_DillonDefeat, Route113_Text_DillonNotEnoughMons"
    )
    if command not in scripts:
        raise SystemExit("FAIL: Battle 93 is not a guarded double")
    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]["TRAINER_DILLON"]
    if manifest != {
        "format": "double", "target_size": 4, "archetype": "Living fault-line layers", "difficulty": 88,
        "partner_interaction": True, "level_offset": 3, "location": "Route 113",
    }:
        raise SystemExit("FAIL: Battle 93 format manifest stale")

    dialogue_file = (ROOT / "data/text/trainers.inc").read_text()
    dialogue = dialogue_file.split("Route113_Text_DillonIntro:", 1)[1].split("Route113_Text_MadelineIntro:", 1)[0]
    for cue in ("volcano proves earth is alive", "Mamoswine cracks", "Mandibuzz circles", "Garganacl is living salt", "Torterra carries", "Bring two able Pokémon"):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 93 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 93 overlong dialogue: {visible}")

    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in corpus_ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 93 competitive reference missing from corpus")


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
                raise SystemExit(f"FAIL: Battle 93 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
        entry = next(row for row in guide["entries"] if row["trainerId"] == "TRAINER_DILLON")
        if entry["designStatus"] != "closed" or entry["format"] != "double":
            raise SystemExit("FAIL: Battle 93 guide status/format stale")
        if [member["speciesId"] for member in entry["party"]] != [member["species"] for member in TEAM]:
            raise SystemExit("FAIL: Battle 93 guide party stale")
    print("PASS: Battle 93 Dillon living fault-line double is source-closed")


if __name__ == "__main__":
    main()
