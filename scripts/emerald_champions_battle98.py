#!/usr/bin/env python3
"""Generate and verify Battle 98, Nancy's post-picnic workout single."""

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
        "level": 2,
        "species": "SPECIES_LICKILICKY",
        "item": "ITEM_ASSAULT_VEST",
        "ability_slot": 2,
        "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
        "moves": ["MOVE_SURF", "MOVE_ICE_BEAM", "MOVE_THUNDERBOLT", "MOVE_FOCUS_BLAST"],
    },
    {
        "level": 3,
        "species": "SPECIES_SPINDA",
        "item": "ITEM_LIFE_ORB",
        "ability_slot": 2,
        "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
        "moves": ["MOVE_SUPERPOWER", "MOVE_PSYCHO_CUT", "MOVE_SUCKER_PUNCH", "MOVE_RETURN"],
    },
    {
        "level": 4,
        "species": "SPECIES_FURFROU",
        "item": "ITEM_ROCKY_HELMET",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_HP_DEF_IMPISH",
        "moves": ["MOVE_COTTON_GUARD", "MOVE_RETURN", "MOVE_SUCKER_PUNCH", "MOVE_U_TURN"],
    },
]

REFERENCES = [
    "showdown:gen6randomdoublesbattle:013",
    "showdown:gen7randomdoublesbattle:018",
    "showdown:gen9championsrandomdoublesbattle:017",
]

NEXT = {
    "index": 99,
    "encounter_id": "BATTLE_099_ROUTE_114_TYRA_IVY",
    "location": "Route114",
    "category": "optional student-mentor shared guarded double",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_TYRA_AND_IVY"],
    "access_note": (
        "Tyra at (23,44) and Ivy at (24,44) each face down with one-tile sight and both invoke the same guarded "
        "TRAINER_TYRA_AND_IVY double. The two object scripts are one physical encounter, not split trainer records."
    ),
}


def design() -> dict:
    return {
        "guide_order": 98,
        "trainer_ids": ["TRAINER_NANCY"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Optional rotating Picnicker at (19,35) on Route 114's main southbound path after Claude and before the "
            "Tyra/Ivy double. Her three-tile look-around geometry makes the single likely but avoidable."
        ),
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 mature post-picnic exercise single",
            "effective_levels": "42, 43, and 44",
            "eligible_ratio": "3/3",
            "mega_access": True,
            "status": "pass",
            "reason": "Lickilicky evolves from Lickitung after learning Rollout; Spinda and Furfrou are single-stage. All three are naturally mature by cap 40 and use no Mega or legendary.",
        },
        "manual_quality": 10,
        "manual_difficulty": 8.6,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {
                    "reference_id": REFERENCES[0],
                    "decision": "Cloud Nine Lickilicky role selected; full donor rejected",
                    "reason": "The reproducible set proves Lickilicky's weather-denial bulk; Nancy changes its physical utility into four-type special coverage so the team is not one physical wall check.",
                },
                {
                    "reference_id": REFERENCES[1],
                    "decision": "Contrary Spinda offense selected; full donor rejected",
                    "reason": "The generated set supplies Superpower, Sucker Punch, Return, and the exact self-amplifying quirk Nancy's dialogue teaches.",
                },
                {
                    "reference_id": REFERENCES[2],
                    "decision": "Fur Coat Furfrou identity selected; full donor rejected",
                    "reason": "The Champions generator validates Furfrou's physical durability and immediate Normal/Dark pressure without importing its rain and Mega shell.",
                },
            ],
            "decision": (
                "All 1005 references were reviewed. Three exact-species generated doubles records and the complete "
                "all-species reviews support the roles; the post-meal exercise order and special/Contrary/defensive category "
                "ladder are transparently hand-authored for Nancy."
            ),
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Cloud Nine is retained; Sitrus physical utility becomes Assault Vest Surf/Ice Beam/Thunderbolt/Focus Blast to establish the special opening axis."},
            {"reference_id": REFERENCES[1], "adaptation": "Spinda keeps Contrary Superpower, Sucker Punch, and Return; Psycho Cut and Life Orb make the cap-40 single immediately threatening without another field mode."},
            {"reference_id": REFERENCES[2], "adaptation": "Fur Coat remains the closer's public defensive identity; Cotton Guard, Rocky Helmet, Return, Sucker Punch, and U-turn form a finite physical tax rather than paralysis support."},
            {"source": "docs/battle_set_reviews/050_sinnoh.json", "adaptation": "Cloud Nine Lickilicky evidence supports broad weather-independent coverage and the explicit handoff from Claude's rain."},
            {"source": "docs/battle_set_reviews/040_hoenn.json", "adaptation": "Contrary Superpower Spinda is retained as the exact exercise-growth quirk."},
            {"source": "docs/battle_set_reviews/070_kalos.json", "adaptation": "Fur Coat Furfrou's source-backed physical tax becomes a singles Cotton Guard and pivot finish."},
        ],
        "ordering": {
            "intended_lead": ["SPECIES_LICKILICKY"],
            "source_order": [member["species"] for member in TEAM],
            "reason": (
                "Assault Vest Lickilicky opens with special coverage and erases weather from Claude's preceding lesson. "
                "Spinda flips to fast physical Contrary pressure. Source-last Furfrou changes the question from racing "
                "damage to using special attacks, Taunt, or pivot control around Fur Coat and Cotton Guard."
            ),
        },
        "team_intent": (
            "Level-42 Cloud Nine Assault Vest Lickilicky is a weather-independent four-type special attacker. Level-43 "
            "Life Orb Contrary Spinda turns Superpower's normal Attack/Defense drops into growth while retaining Psycho Cut, "
            "Sucker Punch, and Return. Level-44 Rocky Helmet Fur Coat Furfrou can add Cotton Guard, attack directly, use "
            "priority, or U-turn. The player must move from special bulk to anti-snowball physical play and finally to special "
            "wallbreaking rather than solve three Normal types with one category."
        ),
        "intended_counterplay": (
            "Fighting pressure is broad but Focus Blast and Psycho Cut punish careless commitments. Special walls, Assault "
            "Vest removal, and accurate neutral damage answer Lickilicky; burn, Haze, Unaware, Fairy/Flying/Psychic, faster "
            "damage, or priority answer Spinda without donating Intimidate to Contrary; special attacks, Taunt, Encore, "
            "phazing, critical hits, Ghost immunity, Rocky Helmet avoidance, and U-turn punishment answer Furfrou. No weather, "
            "sleep, field, recovery loop, hidden read, precise catch, or exact turn is required."
        ),
        "bespoke_ai": (
            "Nancy remains a native smart single with HP-aware matchup switching. Lickilicky selects real super-effective "
            "coverage; Spinda uses Superpower when Contrary growth and damage are valuable rather than on a forced first turn; "
            "Furfrou uses Cotton Guard only against meaningful physical pressure and can attack or pivot otherwise. Cloud Nine, "
            "Assault Vest, Contrary, Superpower, Life Orb, Fur Coat, Cotton Guard, Rocky Helmet, priority, and U-turn are public "
            "native mechanics."
        ),
        "uniqueness": (
            "Lickilicky, Spinda, and Furfrou are all new to the first 97 encounters and absent from every protected marquee "
            "anchor. The all-Normal roster changes category and defensive logic twice, follows Claude's rain by immediately "
            "turning weather off, and uses no weather, room, terrain, sleep, trap, hazards, screens, Protect, Mega, or legendary."
        ),
        "story_logic": (
            "Nancy's meal/exercise dialogue now describes the exact three-part workout. Her post-battle text teaches Cloud "
            "Nine, Contrary, Fur Coat, and Cotton Guard without claiming a hidden rule. She remains an optional rotating "
            "Picnicker with no item, rematch, story flag, or progression reward."
        ),
        "reward_logic": "EXP and prize money only; Nancy owns no item, shop, legendary, Mega Stone, rematch, or progression reward.",
        "campaign_reservations": {
            "spends": ["first Cloud Nine special Lickilicky", "first Contrary Superpower Spinda", "first Fur Coat Cotton Guard Furfrou"],
            "preserves": ["all protected Normal-type anchors", "Snorlax finite setup", "Mega Normal species", "weather/room/terrain teams", "sleep and berry economies"],
            "repeat_rule": "These three species should not recur soon; later Contrary, Fur Coat, or Cloud Nine teams must change format and primary question."
        },
        "author_self_check": {
            "strongest_part": "One simple exercise story produces three mechanically different Normal-type questions and directly clears the preceding weather lesson instead of extending it.",
            "weakest_link": "All three share Fighting pressure and two are physically biased. Lickilicky's special axis, Contrary's Intimidate reversal, Furfrou's physical reduction, coverage, and +2/+3/+4 levels force category changes while preserving the honest type seam."
        },
        "closure": (
            "Battle 98 is source-closed at quality 10 and target difficulty 8.6: three fresh, unreserved, legal mature Normal "
            "types appear at levels 42-44 with distinct items and category questions; exact source order, AI, rotating geometry, "
            "three indexed competitive references, handbook evidence, native-width dialogue, broad type/category/setup "
            "counterplay, and zero reward debt are proven. Runtime playtesting remains required before difficulty is observed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 98,
        "encounter_id": "BATTLE_098_ROUTE_114_NANCY",
        "identity": {
            "location": "Route114",
            "category": "optional rotating mid-route Picnicker single",
            "format": "single",
            "strict_cap": 40,
            "memory_hook": "Post-picnic Lickilicky stretches special coverage, Contrary Spinda grows through Superpower, and Fur Coat Furfrou finishes the workout."
        },
        "primary_player_question": "Can the player change damage category twice—special bulk, anti-Contrary offense, then special wallbreaking—without treating three Normal types as one matchup?",
        "tempo": "Three-stage singles workout: bulky special coverage, fast Contrary physical snowball, then Fur Coat/Cotton Guard physical tax and pivot.",
        "pressure_sources": [
            "level-42 Assault Vest Cloud Nine Lickilicky with four special coverage types",
            "level-43 Life Orb Contrary Spinda with Superpower/Sucker Punch",
            "level-44 Rocky Helmet Fur Coat Furfrou with Cotton Guard/U-turn"
        ],
        "intentional_opening": "Lickilicky is fixed first, Spinda changes category second, and Furfrou is source-last; native smart switching can respond to visible matchups.",
        "intentional_weakness": "Shared Fighting weakness, no field or recovery, Lickilicky's move coverage without STAB, Spinda's modest bulk, and Furfrou's special-defense seam.",
        "first_loss_lesson": "The type stayed Normal but the answer changed. Do not Intimidate Contrary Spinda, and save special pressure or Taunt for Fur Coat Furfrou.",
        "revealed_information": ["cap 40", "rotating optional single", "levels 42-44", "Cloud Nine", "special Assault Vest Lickilicky", "Contrary Superpower", "Life Orb", "Fur Coat plus Cotton Guard", "Rocky Helmet/U-turn", "three fresh species", "no reward/rematch"],
        "counterplay_classes": ["Fighting with coverage awareness", "special walls and item removal", "burn/Haze/Unaware/Fairy/Flying/Psychic into Spinda", "special attacks and Taunt/Encore/phazing into Furfrou", "Ghost immunity and noncontact attacks", "priority and pivot punishment"],
        "target_difficulty": 8.6,
        "difficulty_rationale": "Three optimized fresh levels 42-44, complete distinct items, category reversal, one Contrary snowball, and one physical tax create a serious route single. Shared type pressure and no persistent field keep it below route doubles.",
        "tuning_knob": "Tune Furfrou +4 to +3 first, then Spinda +3 to +2; preserve species, order, items, and category ladder.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["route-single", "picnicker", "post-picnic-workout", "normal-type-ladder", "lickilicky", "spinda", "furfrou", "cloud-nine", "assault-vest", "contrary", "superpower", "fur-coat", "cotton-guard", "rocky-helmet", "category-reversal", "three-fresh-species", "no-weather", "no-sleep", "no-field", "no-protect", "no-mega", "no-legendary"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Reproducible Lickilicky/Spinda/Furfrou sets plus complete all-species reviews; category workout is local."},
        "author_self_check": {"strongest_part": "Three same-type Pokémon ask three different defensive questions in a story the NPC can explain naturally.", "weakest_link": "Fighting compresses the roster; coverage, Contrary, Fur Coat, special opening, and levels keep that honest seam from becoming free."},
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_098_ROUTE_114_NANCY"] = design()

    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 98] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])

    sequence = json.loads(SEQUENCE.read_text())
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 99] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        if row["index"] <= 98:
            row["status"] = "closed"
        elif row["index"] == 99:
            row["status"] = "next"
        else:
            row["status"] = "queued"

    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update(
        {
            "closed_encounters": 98,
            "next_index": 99,
            "next_encounter_id": NEXT["encounter_id"],
            "queued_sequence_entries": 0,
            "canonical_sequence_groups": 99,
            "physical_encounter_groups": 527,
            "unordered_physical_groups": 428,
        }
    )
    return designs, ledger, sequence, operating_system


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    block_text = doubles.trainer_blocks(trainers)["TRAINER_NANCY"].group(0)
    body = doubles.party_match(parties, doubles.party_name(block_text)).group(2)
    actual = [polish.parse_entry(entry) for entry in custom.party_entries(body)]
    if actual != TEAM:
        raise SystemExit("FAIL: Battle 98 Nancy source party differs")
    for token in (".doubleBattle = FALSE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HP_AWARE"):
        if token not in block_text:
            raise SystemExit(f"FAIL: Battle 98 Nancy missing {token}")

    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for member in TEAM:
        illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
        if illegal:
            raise SystemExit(f"FAIL: Battle 98 illegal moves for {member['species']}: {illegal}")
        if member["ability_slot"] >= len(slots[member["species"]]):
            raise SystemExit(f"FAIL: Battle 98 invalid ability slot for {member['species']}")
    if len({member["species"] for member in TEAM}) != 3 or len({member["item"] for member in TEAM}) != 3:
        raise SystemExit("FAIL: Battle 98 species/items are not unique")

    object_event = next(
        row for row in json.loads((ROOT / "data/maps/Route114/map.json").read_text())["object_events"]
        if row.get("script") == "Route114_EventScript_Nancy"
    )
    if (object_event["x"], object_event["y"], object_event["movement_type"], str(object_event["trainer_sight_or_berry_tree_id"])) != (19, 35, "MOVEMENT_TYPE_LOOK_AROUND", "3"):
        raise SystemExit("FAIL: Battle 98 rotating geometry drifted")
    if "trainerbattle_single TRAINER_NANCY" not in (ROOT / "data/maps/Route114/scripts.inc").read_text():
        raise SystemExit("FAIL: Battle 98 Nancy is not a single")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]["TRAINER_NANCY"]
    expected_manifest = {"format": "single", "target_size": 3, "archetype": "Post-picnic category workout", "difficulty": 86, "partner_interaction": False, "level_offset": 3, "location": "Route 114"}
    if manifest != expected_manifest:
        raise SystemExit("FAIL: Battle 98 manifest stale")

    dialogue = (ROOT / "data/text/trainers.inc").read_text().split("Route114_Text_NancyIntro:", 1)[1].split("Route114_Text_SteveIntro:", 1)[0]
    for cue in ("exercise after", "Lickilicky stretches", "Spinda grows", "Furfrou toughens", "Cloud Nine", "Contrary flips", "Fur Coat", "Cotton Guard", "physical attacks"):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 98 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 98 overlong dialogue: {visible}")

    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in corpus_ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 98 competitive reference missing")
    for path, cue in (
        ("docs/battle_set_reviews/050_sinnoh.json", "Lickilicky uses Cloud Nine"),
        ("docs/battle_set_reviews/040_hoenn.json", "Spinda uses Contrary with Superpower"),
        ("docs/battle_set_reviews/070_kalos.json", "Fur Coat makes Furfrou"),
    ):
        if cue not in (ROOT / path).read_text():
            raise SystemExit(f"FAIL: Battle 98 handbook evidence missing from {path}")

    protected = "\n".join(
        path.read_text()
        for path in list((ROOT / "docs").glob("emerald_champions_*anchor_designs.json"))
        + list((ROOT / "docs/dossier_packets").glob("*.json"))
    )
    for species in ("Lickilicky", "Spinda", "Furfrou"):
        if re.search(rf'"{species}"', protected):
            raise SystemExit(f"FAIL: Battle 98 spends protected anchor species {species}")


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
                raise SystemExit(f"FAIL: Battle 98 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
        entry = next(row for row in guide["entries"] if row["trainerId"] == "TRAINER_NANCY")
        if entry["designStatus"] != "closed" or entry["format"] != "single" or entry["partySize"] != 3:
            raise SystemExit("FAIL: Battle 98 guide stale")
    print("PASS: Battle 98 Nancy post-picnic category workout is source-closed")


if __name__ == "__main__":
    main()
