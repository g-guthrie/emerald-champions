#!/usr/bin/env python3
"""Generate/check Battle 89, Jaylen's Route 113 ash-glass thermal double."""

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
        "species": "SPECIES_CRYOGONAL",
        "item": "ITEM_NEVER_MELT_ICE",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_HP_SPDEF_CALM",
        "moves": ["MOVE_FREEZE_DRY", "MOVE_ICE_BEAM", "MOVE_HAZE", "MOVE_RECOVER"],
    },
    {
        "level": 2,
        "species": "SPECIES_MAGNEZONE",
        "item": "ITEM_SHUCA_BERRY",
        "ability_slot": 2,
        "spread": "SPREAD_31_IV_HP_DEF_BOLD",
        "moves": ["MOVE_THUNDERBOLT", "MOVE_FLASH_CANNON", "MOVE_BODY_PRESS", "MOVE_VOLT_SWITCH"],
    },
    {
        "level": 3,
        "species": "SPECIES_MAGMORTAR",
        "item": "ITEM_EXPERT_BELT",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
        "moves": ["MOVE_HEAT_WAVE", "MOVE_THUNDERBOLT", "MOVE_FOCUS_BLAST", "MOVE_PSYCHIC"],
    },
    {
        "level": 4,
        "species": "SPECIES_DARMANITAN_GALARIAN",
        "item": "ITEM_CHOICE_SCARF",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
        "moves": ["MOVE_ICICLE_CRASH", "MOVE_FLARE_BLITZ", "MOVE_U_TURN", "MOVE_SUPERPOWER"],
    },
]

REFERENCES = [
    "showdown:gen6randomdoublesbattle:014",
    "showdown:gen9randomdoublesbattle:020",
    "showdown:gen6randomdoublesbattle:013",
    "showdown:gen8randomdoublesbattle:011",
]

NEXT = {
    "index": 90,
    "encounter_id": "BATTLE_090_ROUTE_113_MADELINE",
    "location": "Route113",
    "category": "optional moving ash-field Parasol Lady encounter",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_MADELINE_1"],
    "access_note": (
        "Madeline is the next westbound physical trainer after Jaylen: a Parasol Lady at (51,11), rotating "
        "counterclockwise with two-tile sight. Her base record owns a Match Call rematch family, so Battle 90 "
        "must close the reachable first battle without conflating later rematch tiers."
    ),
}


def design() -> dict:
    return {
        "guide_order": 89,
        "trainer_ids": ["TRAINER_JAYLEN"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Optional first independent westbound Route 113 trainer after the east triple-sight cluster; stationary "
            "Youngster double in the ash field before Madeline's moving lane."
        ),
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 ash-route thermal change of pace",
            "effective_levels": "41, 42, 43, and 44",
            "eligible_ratio": "4/4",
            "mega_access": True,
            "status": "pass",
            "reason": (
                "Cryogonal has no evolution; Magnezone and Magmortar are final forms; Galarian Darmanitan is a "
                "stone-evolved regional final form. Battle 88 already spent the route's honest middle-stage showcase, "
                "so this encounter intentionally pivots to four mature thermal specialists."
            ),
        },
        "manual_quality": 10,
        "manual_difficulty": 8.9,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {
                    "reference_id": reference_id,
                    "decision": "selected role evidence; full donor rejected",
                    "reason": (
                        "The exact generated set supplies one public role. Jaylen's ash-glass order and thermal "
                        "offense are composed locally to answer the previous-ten repetition advisories."
                    ),
                }
                for reference_id in REFERENCES
            ],
            "decision": (
                "Three exact-species doubles records and one Darmanitan doubles chassis were reviewed. The latter "
                "is legally translated to Galarian Darmanitan's Gorilla Tactics Ice/Fire set."
            ),
        },
        "competitive_references": [
            {
                "reference_id": REFERENCES[0],
                "adaptation": "Levitate Cryogonal keeps Freeze-Dry and recovery, replacing Protect and speed control with Haze and direct Ice damage.",
            },
            {
                "reference_id": REFERENCES[1],
                "adaptation": "Magnezone keeps Thunderbolt/Flash Cannon doubles pressure, trading Protect/Electroweb for Body Press and Volt Switch.",
            },
            {
                "reference_id": REFERENCES[2],
                "adaptation": "Magmortar keeps Flame Body, Heat Wave, and Thunderbolt, then adds legal immediate coverage instead of Taunt/status repetition.",
            },
            {
                "reference_id": REFERENCES[3],
                "adaptation": "Darmanitan's generated physical pivot chassis is translated to the legal Galarian Choice Scarf Gorilla Tactics form.",
            },
        ],
        "ordering": {
            "intended_lead": ["SPECIES_CRYOGONAL", "SPECIES_MAGNEZONE"],
            "source_order": [
                "SPECIES_CRYOGONAL", "SPECIES_MAGNEZONE", "SPECIES_MAGMORTAR", "SPECIES_DARMANITAN_GALARIAN"
            ],
            "reason": (
                "Levitate Cryogonal makes Ground targeting asymmetric while Shuca Magnezone cannot be erased casually. "
                "The reserve changes from special spread coverage to one revealed physical Choice closer."
            ),
        },
        "team_intent": (
            "Cryogonal cools ash into glass with Freeze-Dry/Ice pressure, Haze, and one recovery line. Analytic Shuca "
            "Magnezone shapes the seam through mixed special/Body Press coverage and Volt Switch. Expert Belt Flame Body "
            "Magmortar reheats both opposing slots with Heat Wave and broad coverage. Choice Scarf Gorilla Tactics "
            "Galarian Darmanitan closes physically with Ice/Fire/Fighting or pivots through U-turn."
        ),
        "intended_counterplay": (
            "Fighting and Fire pressure threaten the lead, but Ground must respect Cryogonal's Levitate and Magnezone's "
            "Shuca Berry. Rock/Steel/priority and physical attacks answer Cryogonal; Ground/Water/Rock answer Magmortar; "
            "revealed Choice lock, Intimidate, burn, recoil, hazards, and defensive pivots answer Darmanitan. Haze can be "
            "baited or denied, and three special attackers reward a preserved special wall."
        ),
        "bespoke_ai": (
            "Jaylen uses smart switching, partner help, HP awareness, and field control. The AI respects Levitate and "
            "Shuca targeting, uses Haze only against meaningful boosts, values Recover by HP, scores Heat Wave as public "
            "spread damage, honors Analytic and Flame Body mechanics, and obeys Choice Scarf/Gorilla Tactics lock. No "
            "weather, screens, Protect, sleep, Tailwind, Trick Room, evasion, or hidden input read is present."
        ),
        "uniqueness": (
            "Magnezone, Magmortar, and Galarian Darmanitan are new to the first 88 encounters. Cryogonal's only prior "
            "appearance was 47 battles ago in a three-member rain single; here it is a Ground-immune Haze lead in a "
            "four-member thermal double. This intentionally follows two branch-heavy weather clusters with one linear, "
            "immediate-coverage battle and no persistent field."
        ),
        "story_logic": (
            "Jaylen's old generic observation about cool ash now describes a complete native ash-glass workshop: "
            "Cryogonal cools, Magnezone shapes, Magmortar reheats, and Galarian Darmanitan shatters. Defeat and two-Pokemon "
            "guard text match the actual double."
        ),
        "reward_logic": "EXP and prize money only; Jaylen has no rematch, item, story flag, or campaign reward.",
        "campaign_reservations": {
            "spends": [
                "ash-glass thermal workshop",
                "first Galarian Darmanitan trainer showcase",
                "Analytic Shuca Magnezone",
            ],
            "preserves": [
                "Articuno for Glacia",
                "legendary ice reveals",
                "snow teams",
                "thermal weather wars",
                "future Cryogonal support identities",
            ],
            "repeat_rule": (
                "Galarian Darmanitan should not recur soon; later Magnezone/Magmortar roles must change positioning and "
                "items; Articuno remains protected for the League."
            ),
        },
        "author_self_check": {
            "strongest_part": (
                "The route's throwaway 'cool ash' line becomes one coherent visual/mechanical workshop without stealing "
                "Glacia's protected Articuno or repeating the preceding weather/branch machinery."
            ),
            "weakest_link": (
                "Three members attack specially, so a strong special wall compresses much of the battle. Magnezone's "
                "Body Press and the level-44 physical Choice closer are explicit answers, but preserving that wall is "
                "intended broad counterplay rather than a flaw to hide."
            ),
        },
        "closure": (
            "Battle 89 is source-closed at quality 10 and target difficulty 8.9: four legal levels 41-44, four distinct "
            "items, one distant role-changed repeat, three fresh species, protected Articuno, exact double guard, native "
            "dialogue, smart AI, four current references, broad counterplay, no reward debt, and no weather/screen/Protect/"
            "speed-field/sleep repetition. Runtime remains unplayed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 89,
        "encounter_id": "BATTLE_089_ROUTE_113_JAYLEN",
        "identity": {
            "location": "Route113",
            "category": "optional east ash-field Youngster double",
            "format": "double",
            "strict_cap": 40,
            "memory_hook": (
                "Cryogonal cools ash into glass, Magnezone shapes it, Magmortar reheats it, and Choice Scarf "
                "Galarian Darmanitan shatters it—four temperatures without weather."
            ),
        },
        "primary_player_question": (
            "Can the player attack the Ground-asymmetric lead correctly, preserve special bulk through three coverage "
            "attackers, and still answer the final physical Gorilla Tactics Choice lock?"
        ),
        "tempo": (
            "Linear four-member mixed-coverage double: Ground-immune Haze/recovery lead, Analytic pivot, special spread "
            "reserve, and physical Choice closer; no persistent field or Protect."
        ),
        "pressure_sources": [
            "level-41 Never-Melt Ice Levitate Cryogonal",
            "level-42 Shuca Analytic Magnezone",
            "level-43 Expert Belt Flame Body Magmortar",
            "level-44 Choice Scarf Gorilla Tactics Galarian Darmanitan",
        ],
        "intentional_opening": "Cryogonal and Magnezone are fixed; Ground is safe into only one slot and Haze punishes careless setup.",
        "intentional_weakness": (
            "Three special attackers, shared Fighting pressure on the lead, no Protect/speed field/screens/weather, "
            "Cryogonal physical frailty, Magmortar's common weaknesses, and a public Choice lock/recoil closer."
        ),
        "first_loss_lesson": (
            "This is not another weather puzzle. Preserve a special wall, target the lead asymmetrically, then force "
            "Darmanitan into the wrong Choice move or punish its recoil."
        ),
        "revealed_information": [
            "cap 40",
            "guarded four-member double",
            "levels 41-44",
            "one distant Cryogonal repeat",
            "first Galarian Darmanitan showcase",
            "Articuno remains reserved for Glacia",
            "no reward/rematch",
        ],
        "counterplay_classes": [
            "special walls",
            "Fighting/Fire into the lead",
            "Rock/Steel/priority physical Cryogonal pressure",
            "Ground/Water/Rock into Magmortar",
            "Intimidate/burn/recoil/hazards and Choice-lock exploitation",
            "Taunt or immediate damage around Haze/Recover",
        ],
        "target_difficulty": 8.9,
        "difficulty_rationale": (
            "Four optimized levels 41-44, mixed offensive axes, broad immediate coverage, one sturdy asymmetric lead, "
            "and a very strong physical closer create a serious route double. Three-special compression and no field/"
            "Protect loop keep it below the preceding six-member branches."
        ),
        "tuning_knob": "Tune Darmanitan from +4 to +3 first; preserve the four species, items, order, and no-field identity.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": [
            "route-double", "ash-glass", "thermal-shock", "cryogonal", "analytic", "body-press", "flame-body",
            "gorilla-tactics", "choice-lock", "galarian-form", "haze", "no-weather", "no-screens", "no-protect",
            "no-speed-field", "no-sleep", "no-mega", "no-legendary",
        ],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {
            "status": "complete-current-review",
            "pool_size": 1005,
            "selection": "Three exact doubles records plus one Darmanitan chassis adapted legally to the Galarian form.",
        },
        "author_self_check": {
            "strongest_part": "One simple NPC line now describes all four mechanical roles and a visually native route identity.",
            "weakest_link": "Three-special compression is real; the physical closer and Body Press prevent one-wall autopilot.",
        },
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_089_ROUTE_113_JAYLEN"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [entry for entry in ledger["entries"] if entry["index"] != 89] + [ledger_entry()]
    ledger["entries"].sort(key=lambda entry: entry["index"])
    sequence = json.loads(SEQUENCE.read_text())
    sequence["entries"] = [entry for entry in sequence["entries"] if entry["index"] != 90] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda entry: entry["index"])
    for entry in sequence["entries"]:
        if entry["index"] <= 89:
            entry["status"] = "closed"
        elif entry["index"] == 90:
            entry["status"] = "next"
        else:
            entry["status"] = "queued"
    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update(
        {
            "closed_encounters": 89,
            "next_index": 90,
            "next_encounter_id": "BATTLE_090_ROUTE_113_MADELINE",
            "queued_sequence_entries": 0,
            "canonical_sequence_groups": 90,
            "physical_encounter_groups": 529,
            "unordered_physical_groups": 439,
        }
    )
    return designs, ledger, sequence, operating_system


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    block = doubles.trainer_blocks(trainers)["TRAINER_JAYLEN"].group(0)
    body = doubles.party_match(parties, doubles.party_name(block)).group(2)
    actual = [polish.parse_entry(entry) for entry in custom.party_entries(body)]
    if actual != TEAM:
        raise SystemExit("FAIL: Battle 89 source party differs")
    for token in (
        ".doubleBattle = TRUE",
        "AI_FLAG_SMART_SWITCHING",
        "AI_FLAG_HELP_PARTNER",
        "AI_FLAG_HP_AWARE",
        "AI_FLAG_FIELD_CONTROL",
    ):
        if token not in block:
            raise SystemExit(f"FAIL: Battle 89 missing {token}")

    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for member in TEAM:
        illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
        if illegal:
            raise SystemExit(f"FAIL: Battle 89 illegal moves for {member['species']}: {illegal}")
        if member["ability_slot"] >= len(slots[member["species"]]):
            raise SystemExit(f"FAIL: Battle 89 invalid ability slot for {member['species']}")
    if len({member["species"] for member in TEAM}) != 4 or len({member["item"] for member in TEAM}) != 4:
        raise SystemExit("FAIL: Battle 89 species/items are not unique")
    forbidden = {
        "MOVE_PROTECT", "MOVE_HYPNOSIS", "MOVE_SLEEP_POWDER", "MOVE_TAILWIND", "MOVE_TRICK_ROOM",
        "MOVE_REFLECT", "MOVE_LIGHT_SCREEN", "SPECIES_ARTICUNO",
    }
    if any(token in body for token in forbidden):
        raise SystemExit("FAIL: Battle 89 retained a forbidden field/reveal token")

    scripts = (ROOT / "data/maps/Route113/scripts.inc").read_text()
    command = (
        "trainerbattle_double TRAINER_JAYLEN, Route113_Text_JaylenIntro, "
        "Route113_Text_JaylenDefeat, Route113_Text_JaylenNotEnoughMons"
    )
    if command not in scripts:
        raise SystemExit("FAIL: Battle 89 is not a guarded double")
    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]["TRAINER_JAYLEN"]
    if manifest != {
        "format": "double",
        "target_size": 4,
        "archetype": "Ash-glass thermal shock",
        "difficulty": 89,
        "partner_interaction": True,
        "level_offset": 3,
        "location": "Route 113",
    }:
        raise SystemExit("FAIL: Battle 89 format manifest stale")

    dialogue_file = (ROOT / "data/text/trainers.inc").read_text()
    dialogue = dialogue_file.split("Route113_Text_JaylenIntro:", 1)[1].split("Route113_Text_DillonIntro:", 1)[0]
    for cue in (
        "Cryogonal chills",
        "Magnezone shapes",
        "Magmortar reheats",
        "Galarian Darmanitan shatters",
        "no weather trick",
        "Bring two able Pokémon",
    ):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 89 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 89 overlong dialogue: {visible}")

    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in corpus_ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 89 competitive reference missing from corpus")


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
                raise SystemExit(f"FAIL: Battle 89 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
        entry = next(row for row in guide["entries"] if row["trainerId"] == "TRAINER_JAYLEN")
        if entry["designStatus"] != "closed" or entry["format"] != "double":
            raise SystemExit("FAIL: Battle 89 guide status/format stale")
        if [member["speciesId"] for member in entry["party"]] != [member["species"] for member in TEAM]:
            raise SystemExit("FAIL: Battle 89 guide party stale")
    print("PASS: Battle 89 Jaylen ash-glass thermal double is source-closed")


if __name__ == "__main__":
    main()
