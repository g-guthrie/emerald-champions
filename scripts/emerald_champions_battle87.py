#!/usr/bin/env python3
"""Generate/check Battle 87, Bryant and Shayla's Route 112 native-pair lane."""

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

TEAMS = {
    "TRAINER_BRYANT": [
        {
            "level": 1,
            "species": "SPECIES_MAGCARGO",
            "item": "ITEM_AIR_BALLOON",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_HEAT_WAVE", "MOVE_POWER_GEM", "MOVE_EARTH_POWER", "MOVE_PROTECT"],
        },
        {
            "level": 2,
            "species": "SPECIES_TYPHLOSION",
            "item": "ITEM_CHOICE_SCARF",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_ERUPTION", "MOVE_HEAT_WAVE", "MOVE_FOCUS_BLAST", "MOVE_EXTRASENSORY"],
        },
        {
            "level": 3,
            "species": "SPECIES_HEATMOR",
            "item": "ITEM_ASSAULT_VEST",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_FIRE_LASH", "MOVE_KNOCK_OFF", "MOVE_THUNDER_PUNCH", "MOVE_SUPERPOWER"],
        },
    ],
    "TRAINER_SHAYLA": [
        {
            "level": 1,
            "species": "SPECIES_SUNFLORA",
            "item": "ITEM_FOCUS_SASH",
            "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_SOLAR_BEAM", "MOVE_EARTH_POWER", "MOVE_WEATHER_BALL", "MOVE_HELPING_HAND"],
        },
        {
            "level": 2,
            "species": "SPECIES_BELLOSSOM",
            "item": "ITEM_SITRUS_BERRY",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_QUIVER_DANCE", "MOVE_PETAL_DANCE", "MOVE_MOONBLAST", "MOVE_PROTECT"],
        },
        {
            "level": 3,
            "species": "SPECIES_FLORGES",
            "item": "ITEM_LEFTOVERS",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPDEF_CALM",
            "moves": ["MOVE_MOONBLAST", "MOVE_GIGA_DRAIN", "MOVE_HELPING_HAND", "MOVE_PROTECT"],
        },
    ],
}

REFERENCES = [
    "showdown:gen9randomdoublesbattle:024",
    "showdown:gen6randomdoublesbattle:011",
    "showdown:gen8randomdoublesbattle:012",
    "showdown:gen6randomdoublesbattle:006",
    "showdown:gen4randomdoublesbattle:024",
    "showdown:gen9championsrandomdoublesbattle:018",
]

NEXT = {
    "index": 88,
    "encounter_id": "BATTLE_088_ROUTE_113_LUNG_WYATT_LAWRENCE",
    "location": "Route113",
    "category": "optional east-ash triple-sight cluster",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_LUNG", "TRAINER_WYATT", "TRAINER_LAWRENCE"],
    "access_note": (
        "Route 112's north connection places the player near Route 113 x=90. Before Jaylen, buried Lung "
        "at (71,2), left-facing Wyatt at (75,3), and up-facing Lawrence at (71,4) can all detect the "
        "player on (71,3). Object order produces Lung+Wyatt first when all are unbeaten; prior flags or "
        "direct approaches also expose Lung+Lawrence, Wyatt+Lawrence, and all three split singles. One "
        "dossier must close every reachable pair and split before westbound Jaylen."
    ),
}


def design() -> dict:
    return {
        "guide_order": 87,
        "trainer_ids": ["TRAINER_BRYANT", "TRAINER_SHAYLA"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Optional north Route 112 sight-line cluster immediately after Fiery Path and before Route 113; "
            "either trainer may engage alone or both may combine natively."
        ),
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 late pre-Flannery mountain route",
            "effective_levels": "41, 42, and 43 on each half",
            "eligible_ratio": "6/6",
            "mega_access": True,
            "status": "pass",
            "reason": (
                "All six species are naturally fully evolved by this phase. The encounter spends no Mega or "
                "legendary reveal; it showcases overlooked final forms through exact competitive roles."
            ),
        },
        "manual_quality": 10,
        "manual_difficulty": 9.3,
        "branch_contract": {
            "bryant_shayla_joint": {
                "format": "two-opponent double",
                "trainers": ["TRAINER_BRYANT", "TRAINER_SHAYLA"],
                "members": [
                    "SPECIES_MAGCARGO", "SPECIES_TYPHLOSION", "SPECIES_HEATMOR",
                    "SPECIES_SUNFLORA", "SPECIES_BELLOSSOM", "SPECIES_FLORGES",
                ],
                "target_difficulty": 9.3,
                "contract": (
                    "Sunflora's automatic Drought turns Bryant's spread Fire pressure and Shayla's Solar Beam, "
                    "Weather Ball, and Chlorophyll into one six-member relay."
                ),
            },
            "splits": {
                "TRAINER_BRYANT": {
                    "target_difficulty": 8.5,
                    "contract": (
                        "Air Balloon coverage, Choice Scarf Eruption, and Assault Vest Fire Lash/Knock Off remain a "
                        "complete three-member single without borrowed sun."
                    ),
                },
                "TRAINER_SHAYLA": {
                    "target_difficulty": 8.5,
                    "contract": (
                        "Drought is owned by Shayla herself, so Solar Beam, Weather Ball, Chlorophyll, setup, and "
                        "bulky Fairy support remain self-contained."
                    ),
                },
            },
            "one_usable_policy": (
                "Each independently scripted single remains legal with one usable player Pokemon; native joint "
                "formation follows ordinary two-opponent requirements and sight timing."
            ),
        },
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {
                    "reference_id": reference_id,
                    "decision": "selected exact species role; full donor rejected",
                    "reason": (
                        "The reference supplies a legal public role for one member; Route 112's physical two-trainer "
                        "topology and the no-sleep, no-Primal branch contract are hand-authored."
                    ),
                }
                for reference_id in REFERENCES
            ],
            "decision": (
                "Six current generated competitive records were reviewed member by member. Their reliable pieces "
                "were recomposed into one source-native sun relay rather than copied as an unrelated global team."
            ),
        },
        "competitive_references": [
            {
                "reference_id": "showdown:gen9randomdoublesbattle:024",
                "adaptation": "Weak Armor Magcargo keeps Heat Wave, Power Gem, and Protect but drops repeated Shell Smash for immediate coverage.",
            },
            {
                "reference_id": "showdown:gen6randomdoublesbattle:011",
                "adaptation": "Typhlosion retains the public Eruption/Heat Wave doubles role and receives locally legal coverage.",
            },
            {
                "reference_id": "showdown:gen8randomdoublesbattle:012",
                "adaptation": "Heatmor retains Fire Lash and item pressure, rebuilt around local Tough Claws and Assault Vest legality.",
            },
            {
                "reference_id": "showdown:gen6randomdoublesbattle:006",
                "adaptation": "Sunflora's legal doubles coverage is upgraded by Emerald Champions' authored Drought slot.",
            },
            {
                "reference_id": "showdown:gen4randomdoublesbattle:024",
                "adaptation": "Bellossom keeps Chlorophyll but replaces sleep with a public Quiver Dance/Protect damage line.",
            },
            {
                "reference_id": "showdown:gen9championsrandomdoublesbattle:018",
                "adaptation": "Florges keeps Flower Veil, Leftovers, Moonblast, and Protect while serving the Grass partners directly.",
            },
        ],
        "ordering": {
            "intended_lead": ["SPECIES_MAGCARGO", "SPECIES_SUNFLORA"],
            "source_order": {
                "TRAINER_BRYANT": ["SPECIES_MAGCARGO", "SPECIES_TYPHLOSION", "SPECIES_HEATMOR"],
                "TRAINER_SHAYLA": ["SPECIES_SUNFLORA", "SPECIES_BELLOSSOM", "SPECIES_FLORGES"],
            },
            "reason": (
                "The native pair always opens Magcargo beside Drought Sunflora. Splits preserve each trainer's exact "
                "source-first lead and do not depend on a hidden branch selector."
            ),
        },
        "team_intent": (
            "Joint play opens public Air Balloon Magcargo beside Focus Sash Drought Sunflora. Sunflora may attack or "
            "Helping Hand while Heat Wave pressures both slots. Choice Scarf Typhlosion punishes preserved HP with "
            "Eruption; Assault Vest Tough Claws Heatmor removes items and Defense; Chlorophyll Bellossom can choose "
            "immediate Petal Dance or Quiver Dance; Flower Veil Florges supplies Moonblast, Helping Hand, and Protect."
        ),
        "intended_counterplay": (
            "Change or stall sun, break Sunflora's Sash with priority or spread damage, exploit Typhlosion's revealed "
            "Choice lock and declining Eruption, pop Magcargo's Balloon before Ground pressure, use Wide Guard against "
            "Heat Wave, target Bellossom before setup, remove items, and attack Florges physically. Rock, Ground, Water, "
            "Dragon, Poison, Steel, weather control, Taunt, Protect, and speed control create many viable plans."
        ),
        "bespoke_ai": (
            "Both source records use smart switching, partner help, HP awareness, and field control. Drought is automatic. "
            "The AI values Eruption by current HP, respects Choice Scarf lock, uses Helping Hand only with a live partner, "
            "does not Solar Beam outside sun when a better move exists, and treats Protect/setup through public state. "
            "No sleep, ally-damage combo, hidden input read, Primal timing, or custom battle script is required."
        ),
        "uniqueness": (
            "All six exact species are fresh in the first 86 closed encounters. This is the campaign's first map-timing "
            "sun fusion: two individually complete NPC identities become a materially different six-member puzzle when "
            "their sight lines overlap. It follows four short Route 112 battles without repeating Shell Smash or sleep."
        ),
        "story_logic": (
            "Bryant truthfully describes three Fiery Path catches and says Shayla amplifies rather than enables them. "
            "Shayla truthfully identifies Sunflora as the weather setter and explains the garden roles. Defeat and "
            "post-battle text expose the relevant public mechanics in native-width language."
        ),
        "reward_logic": "EXP and prize money only; neither trainer owns a rematch, item, story flag, or progression reward.",
        "campaign_reservations": {
            "spends": [
                "first native sight-line sun fusion",
                "Drought Sunflora",
                "Choice Scarf Eruption Typhlosion",
                "Tough Claws Fire Lash/Knock Off Heatmor",
                "Flower Veil garden support",
            ],
            "preserves": [
                "all Primals",
                "all Mega sun anchors",
                "legendary weather setters",
                "sleep sun teams",
                "historic full-team sun imports",
            ],
            "repeat_rule": (
                "These six species should not recur soon. Later sun teams must change setter, damage cadence, and "
                "counterplay rather than reproducing this sight-line relay."
            ),
        },
        "author_self_check": {
            "strongest_part": (
                "The map geometry itself determines whether two readable three-member teams fuse into a brutal but fair "
                "six-member sun relay; no menu, quota, or forced global allocator is involved."
            ),
            "weakest_link": (
                "The joint branch front-loads a large Fire multiplier. Drought Sunflora is fragile, weather can be replaced, "
                "Magcargo exposes two 4x weaknesses after Balloon, Eruption loses power with HP, and both splits remain "
                "independently solvable, so the pressure is high without becoming one-answer-only."
            ),
        },
        "closure": (
            "Battle 87 is source-closed at quality 10: every joint and split branch has a complete legal team; all six "
            "fresh species sit at levels 41-43 with distinct items and exact roles; the premature Red Orb Groudon, forced "
            "Shayla double, Sleep Powder, itemless filler, and repeated Shell Smash are gone; dialogue, AI, geometry, "
            "counterplay, reservations, and six current references are explicit. Joint target difficulty is 9.3, each "
            "split target is 8.5, and runtime remains unplayed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 87,
        "encounter_id": "BATTLE_087_ROUTE_112_BRYANT_SHAYLA",
        "identity": {
            "location": "Route112",
            "category": "optional dynamic native-pair north-lane cluster",
            "format": "native-pair double or two split singles",
            "strict_cap": 40,
            "memory_hook": (
                "Fiery Path catches meet a mountain garden: Drought Sunflora turns Magcargo, Typhlosion, Heatmor, "
                "Bellossom, and Florges into a six-member sun relay only when the two sight lines join."
            ),
        },
        "primary_player_question": (
            "Can the player decide whether to break the weather setter, blunt immediate spread Fire, or preserve the "
            "right answer for the Choice Eruption and Chlorophyll reserve—and adapt when only one trainer engaged?"
        ),
        "tempo": (
            "Automatic-sun joint double with fragile setter, spread pressure, Choice HP scaling, physical item removal, "
            "one optional setup flower, and bulky support; or two self-contained three-member singles."
        ),
        "pressure_sources": [
            "level-41 Air Balloon Weak Armor Magcargo",
            "level-42 Choice Scarf Eruption Typhlosion",
            "level-43 Assault Vest Tough Claws Heatmor",
            "level-41 Focus Sash Drought Sunflora",
            "level-42 Sitrus Chlorophyll Bellossom",
            "level-43 Leftovers Flower Veil Florges",
        ],
        "intentional_opening": (
            "Joint branch opens Magcargo plus Sunflora; splits open the same source-first member. Sun is automatic and "
            "public, and neither party requires the other to function."
        ),
        "intentional_weakness": (
            "Sunflora is frail, Sash is breakable, Magcargo has extreme typed seams, Eruption decays with HP and Choice "
            "locks, Heatmor is slow, Bellossom must choose setup or damage, and Florges is physically vulnerable."
        ),
        "first_loss_lesson": (
            "The two trainers amplify rather than merely concatenate. Remove or outlast Drought, deny full-HP Eruption, "
            "then pivot from spread defense into physical pressure without donating a Bellossom setup turn."
        ),
        "revealed_information": [
            "cap 40",
            "levels 41-43",
            "one joint and two split branches",
            "Drought lead",
            "Choice Scarf Eruption",
            "Flower Veil and Helping Hand",
            "no Mega, Primal, legendary, or sleep",
            "no reward or rematch",
        ],
        "counterplay_classes": [
            "weather replacement or sun stalling",
            "Wide Guard and Protect",
            "priority or spread Sash break",
            "Ground/Rock/Water after Balloon",
            "Choice-lock and current-HP exploitation",
            "Taunt and setup denial",
            "Poison/Steel and physical Florges pressure",
            "item removal and speed control",
        ],
        "target_difficulty": 9.3,
        "difficulty_rationale": (
            "The joint is six optimized fresh level-advantaged members with an automatic field and complementary damage "
            "modes; each split is three optimized members at +1/+2/+3. Public items, fragile links, broad typed seams, and "
            "multiple weather/spread/priority/lock plans preserve learnability."
        ),
        "tuning_knob": (
            "If playtesting overshoots, reduce both level-43 closers to +2 first; preserve Drought, source ordering, six "
            "species, distinct items, and the joint/split identity."
        ),
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": [
            "route-cluster", "native-pair-double", "split-singles", "sun", "drought-sunflora",
            "choice-eruption", "weak-armor", "tough-claws", "fire-lash", "knock-off", "chlorophyll",
            "flower-veil", "helping-hand", "no-sleep", "no-mega", "no-legendary", "no-primal",
        ],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {
            "status": "complete-current-review",
            "pool_size": 1005,
            "selection": "Six exact-species generated doubles records; source-native sun composition hand-authored.",
        },
        "author_self_check": {
            "strongest_part": "The joint is more than the sum of two independently valid teams because Drought crosses the sight-line boundary.",
            "weakest_link": "The sun opening is explosive; its fragile setter, weather counterplay, 4x seams, and HP/Choice constraints are the fairness valves.",
        },
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_087_ROUTE_112_BRYANT_SHAYLA"] = design()

    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [entry for entry in ledger["entries"] if entry["index"] != 87] + [ledger_entry()]
    ledger["entries"].sort(key=lambda entry: entry["index"])

    sequence = json.loads(SEQUENCE.read_text())
    sequence["entries"] = [entry for entry in sequence["entries"] if entry["index"] != 88] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda entry: entry["index"])
    for entry in sequence["entries"]:
        if entry["index"] <= 87:
            entry["status"] = "closed"
        elif entry["index"] == 88:
            entry["status"] = "next"
        else:
            entry["status"] = "queued"

    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update(
        {
            "closed_encounters": 87,
            "next_index": 88,
            "next_encounter_id": "BATTLE_088_ROUTE_113_LUNG_WYATT_LAWRENCE",
            "queued_sequence_entries": 0,
            "canonical_sequence_groups": 88,
            "physical_encounter_groups": 529,
            "unordered_physical_groups": 441,
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
            raise SystemExit(f"FAIL: Battle 87 source differs for {trainer_id}")
        for token in (
            ".doubleBattle = FALSE",
            "AI_FLAG_SMART_SWITCHING",
            "AI_FLAG_HELP_PARTNER",
            "AI_FLAG_HP_AWARE",
            "AI_FLAG_FIELD_CONTROL",
        ):
            if token not in block:
                raise SystemExit(f"FAIL: Battle 87 {trainer_id} missing {token}")
        for member in expected:
            legal = dex.legal_moves(member["species"])
            illegal = [move for move in member["moves"] if move not in legal]
            if illegal:
                raise SystemExit(f"FAIL: Battle 87 illegal moves for {member['species']}: {illegal}")
            slots = ability_slots[member["species"]]
            if member["ability_slot"] >= len(slots):
                raise SystemExit(f"FAIL: Battle 87 invalid ability slot for {member['species']}")

    combined_source = "\n".join(
        doubles.party_match(parties, doubles.party_name(blocks[trainer_id].group(0))).group(2)
        for trainer_id in TEAMS
    )
    for forbidden in ("SPECIES_GROUDON", "ITEM_RED_ORB", "MOVE_SLEEP_POWDER", "MOVE_SHELL_SMASH"):
        if forbidden in combined_source:
            raise SystemExit(f"FAIL: Battle 87 retained forbidden source token {forbidden}")

    scripts = (ROOT / "data/maps/Route112/scripts.inc").read_text()
    for trainer_id in TEAMS:
        if f"trainerbattle_single {trainer_id}" not in scripts:
            raise SystemExit(f"FAIL: Battle 87 missing split-capable script for {trainer_id}")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]
    for trainer_id in TEAMS:
        rule = manifest[trainer_id]
        if rule["format"] != "single" or rule["target_size"] != 3 or not rule["partner_interaction"]:
            raise SystemExit(f"FAIL: Battle 87 format manifest stale for {trainer_id}")
        if rule["level_offset"] != 2 or rule["difficulty"] != 85:
            raise SystemExit(f"FAIL: Battle 87 manifest tuning stale for {trainer_id}")

    dialogue_file = (ROOT / "data/text/trainers.inc").read_text()
    dialogue = dialogue_file.split("Route112_Text_BryantIntro:", 1)[1].split("Route113_Text_JaylenIntro:", 1)[0]
    for cue in (
        "three hot Pokémon",
        "fight alone",
        "Magcargo's Balloon",
        "Sunflora starts the weather",
        "Drought",
        "Chlorophyll",
        "Florges",
    ):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 87 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 87 overlong dialogue: {visible}")


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
                raise SystemExit(f"FAIL: Battle 87 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
        for trainer_id, team in TEAMS.items():
            entry = next(row for row in guide["entries"] if row["trainerId"] == trainer_id)
            if entry["designStatus"] != "closed":
                raise SystemExit(f"FAIL: Battle 87 guide status stale for {trainer_id}")
            if [member["speciesId"] for member in entry["party"]] != [member["species"] for member in team]:
                raise SystemExit(f"FAIL: Battle 87 guide party stale for {trainer_id}")
    print("PASS: Battle 87 Bryant/Shayla is source-closed across its native-pair and both split branches")


if __name__ == "__main__":
    main()
