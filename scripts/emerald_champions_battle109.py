#!/usr/bin/env python3
"""Generate and verify Battle 109, Tabitha's Mt. Chimney prototype boss."""

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
FACTION = ROOT / "docs/emerald_champions_faction_anchor_designs.json"
RESERVATIONS = ROOT / "docs/verdant_historic_team_reservations.json"

TEAM = [
    {"level": 1, "species": "SPECIES_COALOSSAL", "item": "ITEM_AIR_BALLOON", "ability_slot": 0, "spread": "SPREAD_31_IV_HP_SPATK_MODEST", "moves": ["MOVE_HEAT_WAVE", "MOVE_POWER_GEM", "MOVE_BODY_PRESS", "MOVE_PROTECT"]},
    {"level": 1, "species": "SPECIES_KLINKLANG", "item": "ITEM_WHITE_HERB", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_GEAR_GRIND", "MOVE_WILD_CHARGE", "MOVE_SHIFT_GEAR", "MOVE_PROTECT"]},
    {"level": 2, "species": "SPECIES_ELECTIVIRE", "item": "ITEM_EXPERT_BELT", "ability_slot": 0, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_WILD_CHARGE", "MOVE_ICE_PUNCH", "MOVE_FIRE_PUNCH", "MOVE_CROSS_CHOP"]},
    {"level": 2, "species": "SPECIES_XURKITREE", "item": "ITEM_CHOICE_SPECS", "ability_slot": 0, "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID", "moves": ["MOVE_THUNDERBOLT", "MOVE_VOLT_SWITCH", "MOVE_DAZZLING_GLEAM", "MOVE_ENERGY_BALL"]},
    {"level": 3, "species": "SPECIES_RHYPERIOR", "item": "ITEM_ASSAULT_VEST", "ability_slot": 1, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_HIGH_HORSEPOWER", "MOVE_STONE_EDGE", "MOVE_ICE_PUNCH", "MOVE_FIRE_PUNCH"]},
    {"level": 4, "species": "SPECIES_MACHAMP", "item": "ITEM_MACHAMPITE", "ability_slot": 0, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_CLOSE_COMBAT", "MOVE_ICE_PUNCH", "MOVE_THUNDER_PUNCH", "MOVE_BULLET_PUNCH"]},
]

REFERENCES = [
    "showdown:gen8randomdoublesbattle:005",
    "showdown:gen6randomdoublesbattle:024",
    "showdown:gen6randombattle:025",
    "showdown:gen8randombattle:014",
    "showdown:gen9championsrandomdoublesbattle:015",
    "showdown:gen7randomdoublesbattle:012",
]

NEXT = {
    "index": 110,
    "encounter_id": "BATTLE_110_MT_CHIMNEY_MAXIE",
    "location": "MtChimney",
    "category": "required Team Magma leader summit boss",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_MAXIE_MT_CHIMNEY"],
    "access_note": "Maxie stands at (13,6), eight walkable steps beyond Tabitha's (12,11) position. His protected base-Groudon/Mega-Flygon ridge anchor is the next required interaction, with field-menu control available first.",
}


def design() -> dict:
    return {
        "guide_order": 109,
        "trainer_ids": ["TRAINER_TABITHA_MT_CHIMNEY"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Required Mt. Chimney Admin boss immediately after the opposing-sight Grunt corridor and before separately "
            "interacted Maxie. The event ends after Tabitha's post-battle message, restoring field and Bag access before Maxie."
        ),
        "runtime_branches": ["Guarded six-member double at cap 40.", "Native refusal path if the player cannot field two usable Pokemon."],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 mature prototype-admin boss",
            "effective_levels": "41, 41, 42, 42, 43, and 44",
            "eligible_ratio": "6/6",
            "mega_access": True,
            "status": "pass",
            "reason": "Coalossal evolves at 34 and Klinklang at 39 in this source; Electivire evolves by item and Xurkitree is single-stage; Rhyperior evolves by item after Rhydon and Machamp by trade. Every slot is stage-legal at its source level.",
        },
        "manual_quality": 10,
        "manual_difficulty": 10.0,
        "observed_difficulty": None,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {"reference_id": REFERENCES[0], "decision": "Coalossal role selected", "reason": "The generated doubles roster proves independent Coalossal pressure; allied activation and Weakness Policy remain forbidden here."},
                {"reference_id": REFERENCES[1], "decision": "Klinklang role selected", "reason": "The exact doubles set supports Shift Gear and Gear Grind as the prototype's sole setup clock."},
                {"reference_id": REFERENCES[2], "decision": "Electivire role selected", "reason": "Generated Motor Drive Expert Belt coverage validates the circuit controller without stealing a protected Frontier species."},
                {"reference_id": REFERENCES[3], "decision": "Xurkitree role selected", "reason": "Generated Choice Specs coverage validates the power supply without importing a speed field."},
                {"reference_id": REFERENCES[4], "decision": "Rhyperior role adapted", "reason": "The Champions set validates heavy doubles offense; Assault Vest and direct single-target coverage remove Trick Room/setup dependence."},
                {"reference_id": REFERENCES[5], "decision": "Machamp role adapted", "reason": "The exact doubles set validates direct Fighting coverage; custom Mega Machamp becomes the sole transformation without confusion."},
            ],
            "decision": (
                "All 1005 records and the reached previous-ten window were reviewed. The soft anchor's Magnezone/Rotom circuit "
                "collided at Battles 104/102 and was replaced with fresh Electivire/Xurkitree, with Genesect preserved for Noland."
            ),
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Air Balloon Steam Engine Coalossal attacks independently with mixed Fire/Rock/Fighting coverage and Protect."},
            {"reference_id": REFERENCES[1], "adaptation": "White Herb Clear Body Klinklang owns the sole Shift Gear and dual physical attacks."},
            {"reference_id": REFERENCES[2], "adaptation": "Expert Belt Motor Drive Electivire supplies physical Electric/Ice/Fire/Fighting controller coverage."},
            {"reference_id": REFERENCES[3], "adaptation": "Choice Specs Beast Boost Xurkitree keeps Electric/Fairy/Grass coverage and Volt Switch as public commitment."},
            {"reference_id": REFERENCES[4], "adaptation": "Assault Vest Solid Rock Rhyperior is the independent four-attack chassis with Stone Edge rather than repeated spread Rock."},
            {"reference_id": REFERENCES[5], "adaptation": "Mega Machamp keeps direct Fighting/Ice/Electric/priority coverage and rejects Dynamic Punch confusion dependence."},
        ],
        "ordering": {
            "lead": ["SPECIES_COALOSSAL", "SPECIES_KLINKLANG"],
            "reserves": ["SPECIES_ELECTIVIRE", "SPECIES_XURKITREE", "SPECIES_RHYPERIOR", "SPECIES_MACHAMP"],
            "reason": "Boiler and gears make the unfinished prototype public. Circuit, chassis, and labor roles are reserves chosen by visible matchup; source-last Mega Machamp is the final worker when practical.",
        },
        "team_intent": (
            "Coalossal is a visible Steam Engine prototype with no allied Water activation or Weakness Policy. Klinklang owns "
            "the roster's single setup clock. Motor Drive Electivire and Choice Specs Xurkitree create physical versus committed special circuit "
            "pressure; Rhyperior is an unassisted special-bulk chassis; Mega Machamp closes through four direct attacks. No weather, "
            "speed field, sleep, redirection, confusion plan, recovery loop, second setup, second Mega, or Primal exists."
        ),
        "intended_counterplay": (
            "Taunt, Haze, Unaware, phazing, priority, Fighting/Ground/Fire, or focus deny Klinklang. Ground, immunity, resist "
            "pivots, Protect, and forced targets exploit Xurkitree's Choice lock. Pressure Electivire around recoil and coverage; use "
            "Water/Grass/Fighting/Ground special pressure through Rhyperior's defenses; preserve Psychic/Fairy/Flying/Ghost, burn, "
            "Intimidate, physical bulk, priority, or speed control for Mega Machamp. Activating Coalossal with Water is optional player risk, not a hidden trap."
        ),
        "bespoke_ai": (
            "Tabitha uses smart switching, partner awareness, and HP awareness. Native setup scoring requires Klinklang survival; "
            "Choice logic constrains Xurkitree; Motor Drive and Volt Switch use native immunity and integrated "
            "switch-in ranking; Balloon, Solid Rock, Assault Vest, Beast Boost, and Mega simulation are native. No activation, move, target, or switch is forced."
        ),
        "uniqueness": (
            "Coalossal, Klinklang, Electivire, and Xurkitree are new to the first 108 encounters. Rhyperior returns 25 battles after "
            "an ordinary route role and Machamp 29 battles after a Winstrate single; both now occupy boss-specific chassis/Mega jobs. "
            "Recent Magnezone and Rotom-Heat are explicitly removed."
        ),
        "story_logic": (
            "Tabitha's source text now names boiler, gears, data reader, circuit, chassis, and worker. Defeat identifies the weak "
            "coupling; post-battle text explains that the prototype still lacks ignition while preserving the Meteorite handoff to "
            "Maxie. The guarded double ends normally and leaves Bag/menu access before Maxie's separate object interaction."
        ),
        "reward_logic": "Required story progress, EXP, and prize money only; Tabitha grants no item or redundant competitive reward.",
        "campaign_reservations": {
            "spends": ["Mt. Chimney prototype assembly line", "unactivated Coalossal", "sole Shift Gear Klinklang", "Electivire-Xurkitree circuit", "Rhyperior chassis", "Mega Machamp laborer"],
            "preserves": ["final Tabitha Surf ignition/Gastrodon/Stakataka/Darmanitan/Mega Excadrill", "Maxie's Groudon/Crobat ridge", "Flannery's Torkoal timing", "Magnezone/Rotom recent Route 114 identities", "every Primal"],
            "repeat_rule": "Coalossal may recur only as Tabitha's evolved machine; the other five require materially different mechanics, format, or same-character progression.",
        },
        "author_self_check": {
            "strongest_part": "The incomplete Coalossal progression and fresh Electivire-Xurkitree circuit make Tabitha feel like an engineer while every machine part has a distinct battle rule.",
            "weakest_link": "Six mechanical species can become visual theming without execution. Shift Gear, Motor Drive coverage, Choice lock, Beast Boost, chassis bulk, and direct Mega coverage are therefore separately public and separately answerable."
        },
        "closure": (
            "Battle 109 is source-closed at quality 10 and target difficulty 10: exact +1/+1/+2/+2/+3/+4 team, one legal Mega, "
            "four fresh and two distant role-changed species, five indexed references plus one authored review, guarded routing, "
            "field-menu recovery before Maxie, native-width dialogue, legal moves/items/abilities/stages, broad counterplay, protected "
            "final-machine reservations, and zero reward debt. Runtime remains unplayed and observed difficulty is unset."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 109,
        "encounter_id": "BATTLE_109_MT_CHIMNEY_TABITHA",
        "identity": {"location": "MtChimney", "category": "required Magma Admin marquee boss", "format": "guarded six-member double", "strict_cap": 40, "memory_hook": "Coalossal boiler and Klinklang gears feed an Electivire-Xurkitree circuit, Rhyperior chassis, and Mega Machamp worker."},
        "primary_player_question": "Can the player stop the one gear boost, force the power supply into a bad Choice lock, contain Electivire's coverage, and preserve a clean answer for the chassis and Mega worker?",
        "tempo": "Six-stage prototype boss: unactivated boiler/gears lead, mixed rare circuit, heavy independent chassis, then priority Mega laborer.",
        "pressure_sources": ["Air Balloon Steam Engine Coalossal", "White Herb Shift Gear Klinklang", "Expert Belt Motor Drive Electivire", "Choice Specs Beast Boost Xurkitree", "Assault Vest Solid Rock Rhyperior", "Mega Machamp direct coverage"],
        "intentional_opening": "Coalossal+Klinklang are fixed source leads; Coalossal has no allied Water trigger and Klinklang's setup is not forced.",
        "intentional_weakness": "Unactivated Coalossal, one setup user, public Choice lock, Electivire recoil and ordinary bulk, slow Rhyperior, no-Protect Mega, shared Ground/Fighting pressure, no weather/speed field/sleep/redirection/healing loop.",
        "first_loss_lesson": "This is unfinished machinery. Stop the gear, exploit the lock, contain Electivire's coverage, crack the chassis specially, and save Psychic/Fairy/Flying/Ghost or speed for Machamp.",
        "revealed_information": ["cap 40", "required guarded double", "levels 41-44", "no Coalossal activation", "one Shift Gear", "Motor Drive Electivire", "Choice Xurkitree", "Rhyperior chassis", "Mega Machamp", "Bag access before Maxie", "no reward"],
        "counterplay_classes": ["Taunt/Haze/Unaware/phazing", "Ground/Fighting/Water/Grass/Fire", "Choice exploitation", "Electivire recoil and coverage", "special pressure into Rhyperior", "Psychic/Fairy/Flying/Ghost into Machamp", "burn/Intimidate/priority/speed control"],
        "target_difficulty": 10.0,
        "difficulty_rationale": "Six optimized levels 41-44, one setup clock, two rare mixed circuit bodies, Choice and Beast Boost pressure, Solid Rock/AV chassis, and a priority Mega create an admin boss. Every engine is public and broad answers remain.",
        "tuning_knob": "Tune Mega Machamp +4 to +3 first, then Rhyperior +3 and circuit +2; preserve all six roles and prototype progression.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["mt-chimney", "required-boss", "magma-admin", "prototype-machine", "coalossal", "klinklang", "electivire", "xurkitree", "rhyperior", "mega-machamp", "motor-drive", "choice-lock", "beast-boost", "four-fresh-species", "no-self-activation", "no-weather", "no-speed-field", "one-mega"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Six indexed exact-role references; soft-anchor recent and protected collisions removed."},
        "author_self_check": {"strongest_part": "The source battle now delivers the protected same-character machine progression without repeating recent circuit species.", "weakest_link": "Mechanical visuals alone are insufficient; each of six public rules must execute distinctly in runtime testing."},
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_109_MT_CHIMNEY_TABITHA"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 109] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])

    sequence = json.loads(SEQUENCE.read_text())
    for row in sequence["entries"]:
        if row["index"] == 109:
            row.update({
                "category": "required Mt. Chimney Magma Admin marquee boss",
                "trainer_ids": ["TRAINER_TABITHA_MT_CHIMNEY"],
                "access_note": "Tabitha stands at (12,11) after the Grunt corridor. His guarded six-member double returns to field/menu control before Maxie's separate (13,6) interaction.",
            })
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 110] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 109 else "next" if row["index"] == 110 else "queued"

    os_data = json.loads(OS_PATH.read_text())
    os_data["current_state"].update({
        "closed_encounters": 109,
        "next_index": 110,
        "next_encounter_id": NEXT["encounter_id"],
        "queued_sequence_entries": 0,
        "canonical_sequence_groups": 110,
        "physical_encounter_groups": 525,
        "unordered_physical_groups": 415,
    })
    return designs, ledger, sequence, os_data


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    block = doubles.trainer_blocks(trainers)["TRAINER_TABITHA_MT_CHIMNEY"].group(0)
    actual = [polish.parse_entry(entry) for entry in custom.party_entries(doubles.party_match(parties, doubles.party_name(block)).group(2))]
    if actual != TEAM:
        raise SystemExit("FAIL: Battle 109 Tabitha source party differs")
    for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE"):
        if token not in block:
            raise SystemExit(f"FAIL: Battle 109 Tabitha missing {token}")
    if len({m["species"] for m in TEAM}) != 6 or len({m["item"] for m in TEAM}) != 6:
        raise SystemExit("FAIL: Battle 109 species/items are not unique")

    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for member in TEAM:
        illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
        if illegal:
            raise SystemExit(f"FAIL: Battle 109 illegal moves for {member['species']}: {illegal}")
        if member["ability_slot"] >= len(slots[member["species"]]):
            raise SystemExit(f"FAIL: Battle 109 invalid ability slot for {member['species']}")
    evolution = (ROOT / "src/data/pokemon/evolution.h").read_text()
    if "[SPECIES_KLANG] \t = {{EVO_LEVEL, 39, SPECIES_KLINKLANG}}" not in evolution:
        raise SystemExit("FAIL: Battle 109 Klinklang stage proof drifted")

    script = (ROOT / "data/maps/MtChimney/scripts.inc").read_text()
    tabitha_block = script.split("MtChimney_EventScript_Tabitha::", 1)[1].split("MtChimney_EventScript_Grunt2::", 1)[0]
    if "trainerbattle_double TRAINER_TABITHA_MT_CHIMNEY" not in tabitha_block or "MtChimney_Text_TabithaNotEnoughMons" not in tabitha_block or "msgbox MtChimney_Text_TabithaPostBattle" not in tabitha_block or "\tend" not in tabitha_block:
        raise SystemExit("FAIL: Battle 109 guarded independent routing missing")
    if "MtChimney_EventScript_Maxie::" not in script or "trainerbattle_no_intro TRAINER_MAXIE_MT_CHIMNEY" not in script:
        raise SystemExit("FAIL: Battle 109 separate Maxie interaction drifted")
    map_data = json.loads((ROOT / "data/maps/MtChimney/map.json").read_text())["object_events"]
    positions = {row["script"]: (row["x"], row["y"]) for row in map_data if row.get("script") in {"MtChimney_EventScript_Tabitha", "MtChimney_EventScript_Maxie"}}
    if positions != {"MtChimney_EventScript_Maxie": (13, 6), "MtChimney_EventScript_Tabitha": (12, 11)}:
        raise SystemExit("FAIL: Battle 109 boss geometry drifted")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]["TRAINER_TABITHA_MT_CHIMNEY"]
    if manifest != {"format": "double", "target_size": 6, "archetype": "Prototype assembly line", "difficulty": 100, "partner_interaction": True, "level_offset": 3, "location": "Mt Chimney"}:
        raise SystemExit("FAIL: Battle 109 manifest stale")

    section = script.split("MtChimney_Text_TabithaIntro:", 1)[1].split("MtChimney_Text_Grunt2Intro:", 1)[0]
    for cue in ("prototype line", "Coalossal is the boiler", "Electivire drives", "Xurkitree powers", "Rhyperior braces", "Machamp does the work", "weak coupling", "needs an ignition", "real machine"):
        if cue not in section:
            raise SystemExit(f"FAIL: Battle 109 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', section):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 109 overlong dialogue: {visible}")

    anchor = json.loads(FACTION.read_text())["designs"]["MT_CHIMNEY_TABITHA"]
    if anchor["status"]["source"] != "source-closed" or [mon["species"] for mon in anchor["team"]] != [m["species"] for m in TEAM] or anchor["difficulty"]["observed"] is not None:
        raise SystemExit("FAIL: Battle 109 faction anchor is not source-honest")
    reservation = next(row for row in json.loads(RESERVATIONS.read_text())["marquee_blueprints"]["entries"] if row.get("anchor") == "MT_CHIMNEY_TABITHA")
    if reservation["design_commitment"] != "spent" or reservation["target_difficulty"] != 10.0:
        raise SystemExit("FAIL: Battle 109 historic reservation is not spent")
    ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 109 competitive reference missing")


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
                raise SystemExit(f"FAIL: Battle 109 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())["entries"]
        entry = next(row for row in guide if row["trainerId"] == "TRAINER_TABITHA_MT_CHIMNEY")
        if entry["designStatus"] != "closed" or entry["format"] != "double" or entry["partySize"] != 6:
            raise SystemExit("FAIL: Battle 109 guide stale")
    print("PASS: Battle 109 Tabitha prototype assembly-line boss is source-closed")


if __name__ == "__main__":
    main()
