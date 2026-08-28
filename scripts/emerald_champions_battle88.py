#!/usr/bin/env python3
"""Generate/check Battle 88, Route 113's Lung/Wyatt/Lawrence triple-sight cluster."""

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
    "TRAINER_LUNG": [
        {
            "level": 1,
            "species": "SPECIES_WEEZING",
            "item": "ITEM_BLACK_SLUDGE",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_DEF_BOLD",
            "moves": ["MOVE_SLUDGE_BOMB", "MOVE_HEAT_WAVE", "MOVE_WILL_O_WISP", "MOVE_CLEAR_SMOG"],
        },
        {
            "level": 2,
            "species": "SPECIES_NINJASK",
            "item": "ITEM_SHARP_BEAK",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_U_TURN", "MOVE_DUAL_WINGBEAT", "MOVE_X_SCISSOR", "MOVE_SWORDS_DANCE"],
        },
        {
            "level": 3,
            "species": "SPECIES_SHEDINJA",
            "item": "ITEM_SAFETY_GOGGLES",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_POLTERGEIST", "MOVE_SHADOW_SNEAK", "MOVE_X_SCISSOR", "MOVE_WILL_O_WISP"],
        },
    ],
    "TRAINER_WYATT": [
        {
            "level": 1,
            "species": "SPECIES_GABITE",
            "item": "ITEM_ROCKY_HELMET",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_EARTHQUAKE", "MOVE_DRAGON_CLAW", "MOVE_IRON_HEAD", "MOVE_STEALTH_ROCK"],
        },
        {
            "level": 2,
            "species": "SPECIES_PUPITAR",
            "item": "ITEM_EVIOLITE",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_STOMPING_TANTRUM", "MOVE_ROCK_SLIDE", "MOVE_CRUNCH", "MOVE_IRON_HEAD"],
        },
        {
            "level": 3,
            "species": "SPECIES_SHELGON",
            "item": "ITEM_LUM_BERRY",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_DRAGON_DANCE", "MOVE_DRAGON_CLAW", "MOVE_CRUNCH", "MOVE_BRICK_BREAK"],
        },
    ],
    "TRAINER_LAWRENCE": [
        {
            "level": 1,
            "species": "SPECIES_CLAYDOL",
            "item": "ITEM_LIGHT_CLAY",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPDEF_CALM",
            "moves": ["MOVE_REFLECT", "MOVE_LIGHT_SCREEN", "MOVE_ICE_BEAM", "MOVE_PSYCHIC"],
        },
        {
            "level": 2,
            "species": "SPECIES_HIPPOWDON",
            "item": "ITEM_SMOOTH_ROCK",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_HIGH_HORSEPOWER", "MOVE_ROCK_SLIDE", "MOVE_CRUNCH", "MOVE_BODY_PRESS"],
        },
        {
            "level": 3,
            "species": "SPECIES_CRADILY",
            "item": "ITEM_LEFTOVERS",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_GIGA_DRAIN", "MOVE_POWER_GEM", "MOVE_EARTH_POWER", "MOVE_RECOVER"],
        },
    ],
}

REFERENCES = [
    "showdown:gen4randomdoublesbattle:002",
    "showdown:gen7randomdoublesbattle:022",
    "showdown:gen5randomdoublesbattle:009",
    "showdown:gen4randombattle:018",
    "showdown:gen4randombattle:005",
    "showdown:gen5randomdoublesbattle:010",
    "showdown:gen9championsrandomdoublesbattle:022",
    "showdown:gen5randomdoublesbattle:008",
]

NEXT = {
    "index": 89,
    "encounter_id": "BATTLE_089_ROUTE_113_JAYLEN",
    "location": "Route113",
    "category": "optional east ash-field Youngster double",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_JAYLEN"],
    "access_note": (
        "After resolving the Lung/Wyatt/Lawrence east cluster, Jaylen is the next westbound trainer: "
        "a stationary Youngster at (62,8), facing down with three-tile sight. His source record forces "
        "a four-member double and precedes Madeline's moving sight lane."
    ),
}


def design() -> dict:
    return {
        "guide_order": 88,
        "trainer_ids": ["TRAINER_LUNG", "TRAINER_WYATT", "TRAINER_LAWRENCE"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "First Route 113 physical trainer cluster after entering near x=90: three trainers converge on tile "
            "(71,3), with object order and prior flags selecting one of three native pairs or a split single."
        ),
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 ash-route transition before Flannery",
            "effective_levels": "41, 42, and 43 on every trainer half",
            "eligible_ratio": "9/9",
            "mega_access": True,
            "status": "pass",
            "reason": (
                "Weezing, Ninjask, Shedinja, Claydol, Hippowdon, and Cradily are final forms. Gabite does not "
                "become Garchomp until 48, Pupitar does not become Tyranitar until 55, and Shelgon does not become "
                "Salamence until 50, so all three late bloomers remain honest middle stages at levels 41-43."
            ),
        },
        "manual_quality": 10,
        "manual_difficulty": 9.2,
        "branch_contract": {
            "all_unbeaten_default": {
                "format": "two-opponent native double",
                "trainers": ["TRAINER_LUNG", "TRAINER_WYATT"],
                "target_difficulty": 9.2,
                "contract": (
                    "Object-event order finds buried Lung before Wyatt and stops after two trainers. Levitate Weezing "
                    "opens beside Rough Skin Gabite, making Gabite's Earthquake partner-safe."
                ),
            },
            "lung_lawrence_joint": {
                "format": "two-opponent native double",
                "trainers": ["TRAINER_LUNG", "TRAINER_LAWRENCE"],
                "target_difficulty": 9.1,
                "contract": (
                    "When Wyatt is already defeated or bypassed, Levitate Weezing and Levitate Claydol open together. "
                    "Lung's fliers and Safety Goggles Shedinja tolerate Lawrence's later sand."
                ),
            },
            "wyatt_lawrence_joint": {
                "format": "two-opponent native double",
                "trainers": ["TRAINER_WYATT", "TRAINER_LAWRENCE"],
                "target_difficulty": 9.2,
                "contract": (
                    "When Lung is already defeated, Rough Skin Gabite opens beside Levitate Claydol. Hippowdon's "
                    "later sand strengthens Eviolite Pupitar's Rock bulk while both Wyatt reserves use Overcoat."
                ),
            },
            "splits": {
                "TRAINER_LUNG": 8.5,
                "TRAINER_WYATT": 8.6,
                "TRAINER_LAWRENCE": 8.5,
            },
            "runtime_rule": (
                "At tile (71,3), the engine scans active object events in template order and stores at most two "
                "approaching trainers. Lung is event 8, Wyatt event 12, and Lawrence event 13. Defeat flags remove "
                "earlier candidates; direct interaction and alternate approach tiles expose every split."
            ),
            "one_usable_policy": (
                "Each source script remains a single so a split is legal with one usable player Pokemon. Native pairs "
                "form only when the ordinary engine confirms the player can field a double."
            ),
        },
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {
                    "reference_id": reference_id,
                    "decision": "selected role evidence; full donor rejected",
                    "reason": (
                        "The reference supports one exact member's public role. The nine-member triple-collision, "
                        "stage progression, and three cross-compatible pairings are authored from map and engine source."
                    ),
                }
                for reference_id in REFERENCES
            ],
            "decision": (
                "Eight exact-species competitive records supplied roles for every member except Pupitar, whose legal "
                "Eviolite Overcoat coverage is locally authored. No whole donor can represent this physical branch graph."
            ),
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Levitate Black Sludge Weezing keeps burn and Clear Smog control without Toxic or recovery looping."},
            {"reference_id": REFERENCES[1], "adaptation": "Speed Boost Ninjask keeps one setup option but trades Protect/Baton Pass loops for direct attacks and U-turn."},
            {"reference_id": REFERENCES[2], "adaptation": "Wonder Guard Shedinja keeps burn and Bug pressure; Safety Goggles replaces Sash to make sand compatibility public."},
            {"reference_id": REFERENCES[3], "adaptation": "Gabite keeps Stealth Rock and Earthquake, with Rough Skin/Rocky Helmet contact tax instead of Sand Veil luck."},
            {"reference_id": REFERENCES[4], "adaptation": "Shelgon keeps Dragon Dance and coverage while Lum Berry and Overcoat make the ash-route role self-contained."},
            {"reference_id": REFERENCES[5], "adaptation": "Claydol keeps its exact Light Clay dual-screen doubles role and opens beside every Earthquake user safely."},
            {"reference_id": REFERENCES[6], "adaptation": "Hippowdon retains Sand Stream and direct Ground pressure without Toxic, Protect, Slack Off, or phazing loops."},
            {"reference_id": REFERENCES[7], "adaptation": "Storm Drain Cradily keeps sand bulk and recovery but uses immediate special coverage rather than Stockpile/Curse."},
        ],
        "ordering": {
            "intended_leads": {
                "Lung+Wyatt": ["SPECIES_WEEZING", "SPECIES_GABITE"],
                "Lung+Lawrence": ["SPECIES_WEEZING", "SPECIES_CLAYDOL"],
                "Wyatt+Lawrence": ["SPECIES_GABITE", "SPECIES_CLAYDOL"],
            },
            "source_order": {
                "TRAINER_LUNG": ["SPECIES_WEEZING", "SPECIES_NINJASK", "SPECIES_SHEDINJA"],
                "TRAINER_WYATT": ["SPECIES_GABITE", "SPECIES_PUPITAR", "SPECIES_SHELGON"],
                "TRAINER_LAWRENCE": ["SPECIES_CLAYDOL", "SPECIES_HIPPOWDON", "SPECIES_CRADILY"],
            },
            "reason": (
                "Every reachable pair opens with at least one Levitate partner beside Gabite or two Ground-immune "
                "controllers. Reserve weather never invalidates Lung's or Wyatt's own roster."
            ),
        },
        "team_intent": (
            "Lung supplies burn/Clear Smog, fast U-turn pressure, one optional Swords Dance, and a Goggles Wonder Guard "
            "endgame test. Wyatt supplies partner-safe Earthquake/contact tax, one Eviolite middle-stage tank, and one "
            "Lum Overcoat Dragon Dance closer. Lawrence supplies finite dual screens, direct Sand Stream pressure, and "
            "Storm Drain Cradily. Any two halves create a different six-member puzzle; each three-member split retains "
            "its own field, damage, and closer logic."
        ),
        "intended_counterplay": (
            "Ice and Fairy pressure threaten Wyatt's young Dragons; special Ground/Psychic and Taunt answer Weezing; "
            "Rock, Fire, Electric, Flying, Ghost, Dark, and residual damage expose Ninjask/Shedinja; screen removal or "
            "stalling answers Claydol; Water/Grass/Ice and special pressure answer Hippowdon; Fighting/Bug/Steel/Ice "
            "answer Cradily. Avoid unnecessary contact into Gabite, keep one Wonder Guard answer, use single-target moves "
            "around partner-safe Earthquake, and do not feed Storm Drain."
        ),
        "bespoke_ai": (
            "All three records use smart switching, partner help, HP awareness, and field control. Native ability and "
            "target checks prevent Gabite from Earthquaking a vulnerable ally. The AI recognizes finite screens, current "
            "weather, Storm Drain, Wonder Guard, Goggles sand immunity, Clear Smog versus boosts, and one public setup "
            "opportunity. It has no Protect loop, Tailwind, Trick Room, sleep, Toxic, evasion ability, hidden input read, "
            "or custom script."
        ),
        "uniqueness": (
            "All nine exact species are fresh in the first 87 closed encounters. This is the first three-way collision "
            "where object-event ordering changes which two complete teams merge. It deliberately showcases three honest "
            "middle evolutions before their campaign window closes, while ash concealment, sand immunity, Ground "
            "positioning, screens, and Wonder Guard make every pairing tactically distinct."
        ),
        "story_logic": (
            "Lung's dialogue now acknowledges the hidden three-trainer trap and explains why Shedinja survives Lawrence's "
            "sand. Wyatt identifies his three late bloomers without claiming they should already be evolved. Lawrence "
            "explains screens, sand, and Storm Drain through native ash-gathering language."
        ),
        "reward_logic": "EXP and prize money only; the cluster owns no item, story flag, rematch, or campaign reward.",
        "campaign_reservations": {
            "spends": [
                "three-way object-order collision",
                "Safety Goggles Shedinja in sand",
                "late-bloomer Gabite/Pupitar/Shelgon trio",
                "screen-to-sand-to-Storm Drain geology",
            ],
            "preserves": [
                "fully evolved Garchomp/Tyranitar/Salamence showcases",
                "Sand Veil/evasion teams",
                "Baton Pass chains",
                "Stockpile sand stall",
                "future ash-route trainer identities",
            ],
            "repeat_rule": (
                "Later appearances of these families must use final forms or different mechanics; no later route cluster "
                "should repeat three intersecting sight lines plus Ground-immunity composition."
            ),
        },
        "author_self_check": {
            "strongest_part": (
                "All seven reachable states are real compositions, and the default pair arises from proven engine scan "
                "order rather than a guessed map narrative. The middle-stage trio is mechanically and evolutionally honest."
            ),
            "weakest_link": (
                "Shedinja can produce a hard team-preview tax. Its item and Wonder Guard are public, every common "
                "super-effective/status/weather route remains valid, and it has no Protect or Sash, so preserving one "
                "answer—not guessing an invisible trick—is the intended lesson."
            ),
        },
        "closure": (
            "Battle 88 is source-closed at quality 10 across three native doubles and three split singles: nine fresh "
            "legal species at levels 41-43, nine distinct items, exact object-order proof, stage-correct late bloomers, "
            "partner-safe Ground positioning, sand-safe Shedinja, no Protect/speed-field/sleep/Toxic loop, eight current "
            "references, honest dialogue, broad counterplay, and no reward debt. Pair targets are 9.1-9.2, split targets "
            "are 8.5-8.6, and runtime remains unplayed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 88,
        "encounter_id": "BATTLE_088_ROUTE_113_LUNG_WYATT_LAWRENCE",
        "identity": {
            "location": "Route113",
            "category": "optional east-ash triple-sight cluster",
            "format": "three possible native-pair doubles or three split singles",
            "strict_cap": 40,
            "memory_hook": (
                "Three trainers surround one ash tile: ninjas, honest late-bloomers, and geology recombine according "
                "to object order, while Goggles, Levitate, Overcoat, sand, and Earthquake make every pair legal."
            ),
        },
        "primary_player_question": (
            "Which two trainers actually saw you, and can you preserve the correct answer for their shared reserve—"
            "Wonder Guard, a Dragon Dance middle-stage closer, or screens into sand and Storm Drain?"
        ),
        "tempo": (
            "Branch-dependent six-member double or three-member split: finite screens and hazards, one setup move per "
            "relevant half, direct mixed pressure, one sand phase, one recovery anchor, and no Protect/speed-field loop."
        ),
        "pressure_sources": [
            "Lung: level-41 Levitate Weezing, level-42 Speed Boost Ninjask, level-43 Goggles Shedinja",
            "Wyatt: level-41 Rough Skin Gabite, level-42 Eviolite Pupitar, level-43 Overcoat Shelgon",
            "Lawrence: level-41 screen Claydol, level-42 Sand Stream Hippowdon, level-43 Storm Drain Cradily",
        ],
        "intentional_opening": (
            "All-unbeaten object order chooses Lung+Wyatt. Defeat flags expose either alternate pair; direct approaches "
            "expose splits. Every pair opens with partner-safe Ground positioning."
        ),
        "intentional_weakness": (
            "No Protect, Tailwind, Trick Room, sleep, Toxic, phazing, or evasion ability; finite screens; one setup per "
            "offensive half; broad Ice/Fairy/Rock/Psychic/Ghost/Dark/Water/Grass/Fighting/Steel seams; Shedinja has no Sash."
        ),
        "first_loss_lesson": (
            "The ash tile is a team-preview check. Identify which reserve package is coming, keep one explicit Wonder "
            "Guard answer when Lung appears, and exploit the visible seams instead of attacking through screens or sand blindly."
        ),
        "revealed_information": [
            "cap 40",
            "levels 41-43",
            "object-order default Lung+Wyatt",
            "three pair and three split branches",
            "all nine species fresh",
            "three honest middle stages",
            "no Mega or legendary",
            "no reward/rematch",
        ],
        "counterplay_classes": [
            "Ice/Fairy and special pressure into Wyatt",
            "Rock/Fire/Electric/Flying/Ghost/Dark and residual damage into Lung",
            "screen removal/Taunt and Water/Grass/Ice/Fighting/Steel into Lawrence",
            "noncontact attacks into Rough Skin/Rocky Helmet",
            "weather replacement and Storm Drain avoidance",
            "single-target positioning around Earthquake",
        ],
        "target_difficulty": 9.2,
        "difficulty_rationale": (
            "Each pair fields six optimized fresh level-advantaged members with a distinct public composition; each "
            "split keeps three optimized levels 41-43. The many typed seams, no Protect/speed loop, finite setup, and "
            "explicit reserve tells keep a severe branch test learnable."
        ),
        "tuning_knob": (
            "Tune each level-43 closer from +3 to +2 first; preserve all nine species, object order, item identities, "
            "stage progression, and cross-pair safety."
        ),
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": [
            "route-cluster", "triple-sight", "object-order", "three-native-pairs", "split-singles", "ash-ninja",
            "late-bloomers", "honest-middle-stages", "levitate-earthquake", "safety-goggles-shedinja", "sand",
            "overcoat", "screens", "storm-drain", "wonder-guard", "no-protect", "no-speed-field", "no-sleep",
            "no-toxic", "no-mega", "no-legendary",
        ],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {
            "status": "complete-current-review",
            "pool_size": 1005,
            "selection": "Eight exact-species references plus one locally authored Pupitar; physical graph is source-authored.",
        },
        "author_self_check": {
            "strongest_part": "The engine's two-slot approach array turns one physical tile into three different legal six-member teams.",
            "weakest_link": "Shedinja taxes preview; its public Goggles/no-Sash state and many common answers keep it fair.",
        },
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_088_ROUTE_113_LUNG_WYATT_LAWRENCE"] = design()

    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [entry for entry in ledger["entries"] if entry["index"] != 88] + [ledger_entry()]
    ledger["entries"].sort(key=lambda entry: entry["index"])

    sequence = json.loads(SEQUENCE.read_text())
    sequence["entries"] = [entry for entry in sequence["entries"] if entry["index"] != 89] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda entry: entry["index"])
    for entry in sequence["entries"]:
        if entry["index"] <= 88:
            entry["status"] = "closed"
        elif entry["index"] == 89:
            entry["status"] = "next"
        else:
            entry["status"] = "queued"

    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update(
        {
            "closed_encounters": 88,
            "next_index": 89,
            "next_encounter_id": "BATTLE_089_ROUTE_113_JAYLEN",
            "queued_sequence_entries": 0,
            "canonical_sequence_groups": 89,
            "physical_encounter_groups": 529,
            "unordered_physical_groups": 440,
        }
    )
    return designs, ledger, sequence, operating_system


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    blocks = doubles.trainer_blocks(trainers)
    dex = presets.LocalDex()
    ability_slots = doubles.base_ability_slots()

    for trainer_id, expected in TEAMS.items():
        block = blocks[trainer_id].group(0)
        body = doubles.party_match(parties, doubles.party_name(block)).group(2)
        actual = [polish.parse_entry(entry) for entry in custom.party_entries(body)]
        if actual != expected:
            raise SystemExit(f"FAIL: Battle 88 source differs for {trainer_id}")
        for token in (
            ".doubleBattle = FALSE",
            "AI_FLAG_SMART_SWITCHING",
            "AI_FLAG_HELP_PARTNER",
            "AI_FLAG_HP_AWARE",
            "AI_FLAG_FIELD_CONTROL",
        ):
            if token not in block:
                raise SystemExit(f"FAIL: Battle 88 {trainer_id} missing {token}")
        for member in expected:
            illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
            if illegal:
                raise SystemExit(f"FAIL: Battle 88 illegal moves for {member['species']}: {illegal}")
            if member["ability_slot"] >= len(ability_slots[member["species"]]):
                raise SystemExit(f"FAIL: Battle 88 invalid ability slot for {member['species']}")

    all_members = [member for team in TEAMS.values() for member in team]
    if len({member["species"] for member in all_members}) != 9:
        raise SystemExit("FAIL: Battle 88 species are not unique")
    if len({member["item"] for member in all_members}) != 9:
        raise SystemExit("FAIL: Battle 88 items are not unique")
    for forbidden in ("MOVE_PROTECT", "MOVE_SLEEP_POWDER", "MOVE_HYPNOSIS", "MOVE_TOXIC"):
        if any(forbidden in member["moves"] for member in all_members):
            raise SystemExit(f"FAIL: Battle 88 retained forbidden move {forbidden}")

    scripts = (ROOT / "data/maps/Route113/scripts.inc").read_text()
    for trainer_id in TEAMS:
        if f"trainerbattle_single {trainer_id}" not in scripts:
            raise SystemExit(f"FAIL: Battle 88 missing split-capable script for {trainer_id}")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]
    for trainer_id in TEAMS:
        rule = manifest[trainer_id]
        if rule["format"] != "single" or rule["target_size"] != 3 or not rule["partner_interaction"]:
            raise SystemExit(f"FAIL: Battle 88 format manifest stale for {trainer_id}")
        if rule["level_offset"] != 2 or rule["difficulty"] < 85:
            raise SystemExit(f"FAIL: Battle 88 tuning manifest stale for {trainer_id}")

    map_data = json.loads((ROOT / "data/maps/Route113/map.json").read_text())["object_events"]
    geometry = {
        event["script"]: (index, event["x"], event["y"], event["movement_type"], str(event["trainer_sight_or_berry_tree_id"]))
        for index, event in enumerate(map_data)
        if event["script"] in {"Route113_EventScript_Lung", "Route113_EventScript_Wyatt", "Route113_EventScript_Lawrence"}
    }
    expected_geometry = {
        "Route113_EventScript_Lung": (8, 71, 2, "MOVEMENT_TYPE_BURIED", "1"),
        "Route113_EventScript_Wyatt": (12, 75, 3, "MOVEMENT_TYPE_FACE_LEFT", "4"),
        "Route113_EventScript_Lawrence": (13, 71, 4, "MOVEMENT_TYPE_FACE_UP", "1"),
    }
    if geometry != expected_geometry:
        raise SystemExit(f"FAIL: Battle 88 sight geometry drifted: {geometry}")
    trainer_see = (ROOT / "src/trainer_see.c").read_text()
    for token in (
        "struct ApproachingTrainer gApproachingTrainers[2]",
        "for (i = 0; i < OBJECT_EVENTS_COUNT; i++)",
        "if (gNoOfApproachingTrainers > 1)",
    ):
        if token not in trainer_see:
            raise SystemExit(f"FAIL: Battle 88 object-order proof missing {token}")

    dialogue_file = (ROOT / "data/text/trainers.inc").read_text()
    dialogue_start = dialogue_file.index("Route113_Text_LungIntro:")
    dialogue_end = dialogue_file.index("Route114_Text_LennyIntro:", dialogue_start)
    dialogue = dialogue_file[dialogue_start:dialogue_end]
    for cue in (
        "other two",
        "Shedinja's Goggles",
        "Claydol screens",
        "Hippowdon",
        "three late bloomers",
        "final forms ahead",
        "artificially held back",
    ):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 88 dialogue missing {cue}")
    for label in ("Lung", "Lawrence", "Wyatt"):
        start = dialogue.index(f"Route113_Text_{label}Intro:")
        post_start = dialogue.index(f"Route113_Text_{label}PostBattle:", start)
        end = dialogue.find("\nRoute113_Text_", post_start + 1)
        section = dialogue[start:] if end == -1 else dialogue[start:end]
        for line in re.findall(r'\.string "([^"]*)"', section):
            visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
            if len(visible) > 36:
                raise SystemExit(f"FAIL: Battle 88 overlong {label} dialogue: {visible}")

    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    missing_refs = [reference_id for reference_id in REFERENCES if reference_id not in corpus_ids]
    if missing_refs:
        raise SystemExit(f"FAIL: Battle 88 missing corpus references: {missing_refs}")


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
                raise SystemExit(f"FAIL: Battle 88 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
        for trainer_id, team in TEAMS.items():
            entry = next(row for row in guide["entries"] if row["trainerId"] == trainer_id)
            if entry["designStatus"] != "closed":
                raise SystemExit(f"FAIL: Battle 88 guide status stale for {trainer_id}")
            if [member["speciesId"] for member in entry["party"]] != [member["species"] for member in team]:
                raise SystemExit(f"FAIL: Battle 88 guide party stale for {trainer_id}")
    print("PASS: Battle 88 Route 113 triple-sight cluster is source-closed across all pair and split branches")


if __name__ == "__main__":
    main()
