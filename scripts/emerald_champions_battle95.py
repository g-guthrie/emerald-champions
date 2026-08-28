#!/usr/bin/env python3
"""Generate/check Battle 95, Nolan's champion pond mechanics double."""

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
        "species": "SPECIES_DONDOZO",
        "item": "ITEM_LEFTOVERS",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
        "moves": ["MOVE_ORDER_UP", "MOVE_WAVE_CRASH", "MOVE_HEAVY_SLAM", "MOVE_ICE_PUNCH"],
    },
    {
        "level": 2,
        "species": "SPECIES_TATSUGIRI",
        "item": "ITEM_FOCUS_SASH",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
        "moves": ["MOVE_DRAGON_PULSE", "MOVE_MUDDY_WATER", "MOVE_ICY_WIND", "MOVE_HELPING_HAND"],
    },
    {
        "level": 3,
        "species": "SPECIES_PALAFIN",
        "item": "ITEM_MYSTIC_WATER",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
        "moves": ["MOVE_FLIP_TURN", "MOVE_WAVE_CRASH", "MOVE_ICE_PUNCH", "MOVE_JET_PUNCH"],
    },
    {
        "level": 4,
        "species": "SPECIES_CLAWITZER",
        "item": "ITEM_ASSAULT_VEST",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
        "moves": ["MOVE_WATER_PULSE", "MOVE_AURA_SPHERE", "MOVE_DARK_PULSE", "MOVE_DRAGON_PULSE"],
    },
]

REFERENCES = [
    "vgc:regional-merida-2025",
    "smogon:gen9nu:001",
    "elite:wolfe:orlando-2023",
    "showdown:gen9randombattle:002",
]

NEXT = {
    "index": 96,
    "encounter_id": "BATTLE_096_ROUTE_114_KAI_CHARLOTTE",
    "location": "Route114",
    "category": "optional east-pond native-pair cluster",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_KAI", "TRAINER_CHARLOTTE"],
    "access_note": (
        "Kai at (28,16) faces down/left with three-tile sight and Charlotte at (28,20) faces up with three-tile "
        "sight across the same east-pond vertical lane. Facing/timing and prior flags expose a native pair or either "
        "split single. One dossier must close every branch before the southbound Route 114 trainers."
    ),
}


def design() -> dict:
    return {
        "guide_order": 95,
        "trainer_ids": ["TRAINER_NOLAN"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Direct-interaction optional Fisherman double at Route 114's east pond after Fallarbor and before the "
            "Kai/Charlotte sight lane. Sight range zero means the player deliberately accepts this severe battle."
        ),
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 mature competitive fishing lesson",
            "effective_levels": "41, 42, 43, and 44",
            "eligible_ratio": "4/4",
            "mega_access": True,
            "status": "pass",
            "reason": (
                "Dondozo and Tatsugiri do not evolve; Palafin is the level-38 evolution of Finizen and is correctly "
                "evolved at level 43; Clawitzer is a final form."
            ),
        },
        "manual_quality": 10,
        "manual_difficulty": 9.6,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {
                    "reference_id": reference_id,
                    "decision": "selected champion/exact role; full donor rejected",
                    "reason": (
                        "The reference supports Commander, Tatsugiri, Zero to Hero, or Mega Launcher. Nolan's exact "
                        "four-member Route 114 order is locally authored around canonical Commander mechanics."
                    ),
                }
                for reference_id in REFERENCES
            ],
            "decision": (
                "A 2025 regional-winning Commander roster, an exact Smogon Tatsugiri set, Wolfe Glick's Orlando "
                "Palafin, and a generated Assault Vest Clawitzer set support all four roles."
            ),
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "The Mérida champion core uses canonical Commander: Tatsugiri enters Dondozo, grants +2 to five stats, becomes untargetable, and preserves its form for Order Up."},
            {"reference_id": REFERENCES[1], "adaptation": "Exact Tatsugiri special pressure becomes the Focus Sash contingency that emerges if Dondozo falls; its Curly form makes Order Up raise Attack while swallowed."},
            {"reference_id": REFERENCES[2], "adaptation": "Wolfe's Orlando Zero to Hero Palafin supplies the native switch-and-return lesson, translated to legal Flip Turn/Wave Crash/Ice Punch/Jet Punch."},
            {"reference_id": REFERENCES[3], "adaptation": "Assault Vest Mega Launcher Clawitzer keeps the exact four pulse/aura attacks available locally."},
        ],
        "ordering": {
            "intended_lead": ["SPECIES_DONDOZO", "SPECIES_TATSUGIRI"],
            "intended_reserves": ["SPECIES_PALAFIN", "SPECIES_CLAWITZER"],
            "source_order": ["SPECIES_DONDOZO", "SPECIES_TATSUGIRI", "SPECIES_PALAFIN", "SPECIES_CLAWITZER"],
            "reason": (
                "Canonical Commander is active from turn one: Curly Tatsugiri enters Dondozo, which gains +2 in five "
                "stats and an Attack-raising Order Up. If Dondozo falls, Sash Tatsugiri emerges before Palafin and Clawitzer."
            ),
        },
        "team_intent": (
            "Commander swallows Focus Sash Curly Tatsugiri, removes its action and target slot, and sharply raises all five "
            "of allied Dondozo's combat stats. Unaware Leftovers Dondozo can compound Attack with Order Up while using recoil "
            "Water, Steel, and Ice coverage. If Dondozo falls, Tatsugiri reappears with its full special-support set. Mystic "
            "Water Palafin then uses native switching to reach Hero form, and Assault Vest Mega Launcher Clawitzer closes."
        ),
        "intended_counterplay": (
            "Haze and Clear Smog erase the five boosts; burn, Intimidate, Reflect, Electric/Grass pressure, Water immunity, "
            "recoil exploitation, and concentrated damage contain Dondozo. Tatsugiri cannot be targeted or forced out while "
            "swallowed, but Dondozo is the only enemy taking actions and dropping it exposes the Sash contingency. Fairy/Dragon "
            "then answers Tatsugiri, while priority, Palafin state tracking, item removal, and special bulk cover the reserves."
        ),
        "bespoke_ai": (
            "Nolan uses smart switching, partner help, HP awareness, Combo Setup, and Speed Control. Commander removes "
            "Tatsugiri's action, blocks both partners from switching, and makes Order Up read the stored Curly form even if "
            "Tatsugiri later faints. Native AI values that Attack increase, then uses the healthy Zero to Hero switch path. "
            "All boosts, items, recoil, priority, pulse coverage, and the lack of weather/Protect/sleep/hazards are public."
        ),
        "uniqueness": (
            "Tatsugiri base form, Palafin, and Clawitzer are new to the first 94 encounters. Dondozo last appeared 68 "
            "battles ago in the juvenile Rusturf lesson beside Stretchy Tatsugiri; Nolan is the mature Commander payoff "
            "with a different form, exact champion evidence, Order Up, Zero to Hero, and a pulse closer."
        ),
        "story_logic": (
            "Nolan's generic fishing monologue now describes the exact boss/commander catch and Palafin's return. "
            "Post-battle text explains canonical Commander, its five boosts, Curly Order Up, Zero to Hero, and Clawitzer. "
            "The source's unguarded double, itemless fish, unrelated Rhyhorn/Cubone, and invalid Cubone stage are removed."
        ),
        "reward_logic": "EXP and prize money only; Nolan owns no item, rematch, story flag, or progression reward.",
        "campaign_reservations": {
            "spends": ["mature canonical Commander lesson", "base-form Tatsugiri Order Up Attack", "first Zero to Hero Palafin", "Mega Launcher Clawitzer pulse closer"],
            "preserves": ["Tatsugiri alternate-form Order Up stats", "Tatsugiri Mega", "rain fishing teams", "Dondozo boss teams", "other Palafin tournament adaptations"],
            "repeat_rule": "Dondozo/Tatsugiri and Palafin should not recur soon; later fishing teams must change the central doubles mechanic.",
        },
        "author_self_check": {
            "strongest_part": "A fisherman directly teaches two of modern doubles' most recognizable legal mechanics using a regional champion and Wolfe evidence.",
            "weakest_link": (
                "A fast Haze or Clear Smog can erase Commander's entire opening advantage, leaving one active enemy slot. "
                "+1 through +4 levels, Dondozo's bulk and Order Up, the re-emerging Sash Tatsugiri, Palafin transformation, "
                "and Assault Vest coverage make that broad answer necessary rather than sufficient."
            ),
        },
        "closure": (
            "Battle 95 is reclosed at quality 10 and target difficulty 9.6: four legal levels 41-44, four distinct "
            "items, exact guarded double, canonical Commander/Order Up/Zero to Hero/Mega Launcher mechanics, one "
            "regional champion plus Wolfe/Smogon/Showdown evidence, native-width dialogue, broad direct counterplay, "
            "no reward debt, and no weather/Protect/sleep/hazard/Mega dependency. Runtime remains unplayed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 95,
        "encounter_id": "BATTLE_095_ROUTE_114_NOLAN",
        "identity": {
            "location": "Route114",
            "category": "optional east-pond Fisherman double",
            "format": "double",
            "strict_cap": 40,
            "memory_hook": "Tatsugiri vanishes into a five-stat-boosted Dondozo; Palafin leaves and returns as a hero; Assault Vest Clawitzer closes with four boosted pulses.",
        },
        "primary_player_question": "Can the player neutralize a five-stat-boosted Dondozo without wasting attacks on the swallowed slot, then track Palafin and preserve special bulk for Clawitzer?",
        "tempo": "Four-member champion-mechanics double: one-slot Commander juggernaut, exposed Sash contingency, native switch transformation, and four-type special closer.",
        "pressure_sources": [
            "level-41 Leftovers Unaware Dondozo at +2 in five stats",
            "swallowed level-42 Focus Sash Curly Tatsugiri that reappears after Dondozo",
            "level-43 Mystic Water Zero to Hero Palafin",
            "level-44 Assault Vest Mega Launcher Clawitzer",
        ],
        "intentional_opening": "Dondozo/Tatsugiri is fixed; canonical Commander swallows Tatsugiri and gives Dondozo +2 in Attack, Defense, Sp. Atk, Sp. Def, and Speed.",
        "intentional_weakness": "Only Dondozo acts while Commander is active; Haze/Clear Smog, Electric/Grass, Water immunity, burn/Intimidate, recoil, and concentrated damage are broad answers.",
        "first_loss_lesson": "Do not attack the empty commander slot. Reset or contain Dondozo's five boosts, then expect Focus Sash Tatsugiri to reappear when Dondozo falls.",
        "revealed_information": ["cap 40", "guarded double", "levels 41-44", "canonical Commander", "+2 to five stats", "Curly Order Up raises Attack", "Zero to Hero", "three fresh species", "no reward/rematch"],
        "counterplay_classes": ["Haze/Clear Smog", "Electric/Grass and Water immunity", "burn/Intimidate/Reflect/recoil", "concentrated Dondozo damage", "priority and Palafin state tracking", "special bulk into Clawitzer"],
        "target_difficulty": 9.6,
        "difficulty_rationale": "Four optimized levels 41-44 and three compounding native mechanics create a severe optional double. Commander gives terrifying stats but also sacrifices an enemy action and has several broadly distributed reset and type answers.",
        "tuning_knob": "Tune Clawitzer from +4 to +3 first; preserve species, order, items, and all three public mechanic lessons.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": [
            "route-double", "fisherman", "commander", "dondozo", "tatsugiri", "order-up", "zero-to-hero", "palafin",
            "mega-launcher", "clawitzer", "champion-reference", "wolfe-reference", "no-weather", "no-protect",
            "no-sleep", "no-hazards", "no-mega", "no-legendary",
        ],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Regional champion Commander, exact Tatsugiri, Wolfe Palafin, and generated Clawitzer evidence."},
        "author_self_check": {"strongest_part": "The pond encounter now teaches the real championship Commander puzzle with native visual state.", "weakest_link": "A fast stat reset sharply lowers the opener; later levels and distinct reserve mechanics preserve the second half."},
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_095_ROUTE_114_NOLAN"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [entry for entry in ledger["entries"] if entry["index"] != 95] + [ledger_entry()]
    ledger["entries"].sort(key=lambda entry: entry["index"])
    sequence = json.loads(SEQUENCE.read_text())
    sequence["entries"] = [entry for entry in sequence["entries"] if entry["index"] != 96] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda entry: entry["index"])
    for entry in sequence["entries"]:
        if entry["index"] <= 95:
            entry["status"] = "closed"
        elif entry["index"] == 96:
            entry["status"] = "next"
        else:
            entry["status"] = "queued"
    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update(
        {
            "closed_encounters": 95,
            "next_index": 96,
            "next_encounter_id": "BATTLE_096_ROUTE_114_KAI_CHARLOTTE",
            "queued_sequence_entries": 0,
            "canonical_sequence_groups": 96,
            "physical_encounter_groups": 527,
            "unordered_physical_groups": 431,
        }
    )
    return designs, ledger, sequence, operating_system


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    block = doubles.trainer_blocks(trainers)["TRAINER_NOLAN"].group(0)
    body = doubles.party_match(parties, doubles.party_name(block)).group(2)
    actual = [polish.parse_entry(entry) for entry in custom.party_entries(body)]
    if actual != TEAM:
        raise SystemExit("FAIL: Battle 95 source party differs")
    for token in (
        ".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE",
        "AI_FLAG_COMBO_SETUP", "AI_FLAG_SPEED_CONTROL",
    ):
        if token not in block:
            raise SystemExit(f"FAIL: Battle 95 missing {token}")

    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for member in TEAM:
        illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
        if illegal:
            raise SystemExit(f"FAIL: Battle 95 illegal moves for {member['species']}: {illegal}")
        if member["ability_slot"] >= len(slots[member["species"]]):
            raise SystemExit(f"FAIL: Battle 95 invalid ability slot for {member['species']}")
    if len({member["species"] for member in TEAM}) != 4 or len({member["item"] for member in TEAM}) != 4:
        raise SystemExit("FAIL: Battle 95 species/items are not unique")

    scripts = (ROOT / "data/maps/Route114/scripts.inc").read_text()
    command = (
        "trainerbattle_double TRAINER_NOLAN, Route114_Text_NolanIntro, "
        "Route114_Text_NolanDefeat, Route114_Text_NolanNotEnoughMons"
    )
    if command not in scripts:
        raise SystemExit("FAIL: Battle 95 is not a guarded double")
    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]["TRAINER_NOLAN"]
    if manifest != {
        "format": "double", "target_size": 4, "archetype": "Champion pond mechanics", "difficulty": 96,
        "partner_interaction": True, "level_offset": 3, "location": "Route 114",
    }:
        raise SystemExit("FAIL: Battle 95 format manifest stale")

    map_data = json.loads((ROOT / "data/maps/Route114/map.json").read_text())["object_events"][14]
    if (map_data["script"], map_data["x"], map_data["y"], str(map_data["trainer_sight_or_berry_tree_id"])) != (
        "Route114_EventScript_Nolan", 25, 6, "0"
    ):
        raise SystemExit("FAIL: Battle 95 direct-interaction geometry drifted")

    util = (ROOT / "src/battle_util.c").read_text()
    for token in ("TryActivateCommander", "gBattleStruct->commanderActive[dondozo]", "STATUS4_COMMANDER", "ReleaseFaintedCommanders"):
        if token not in util:
            raise SystemExit(f"FAIL: Battle 95 canonical Commander hook missing {token}")
    commands = (ROOT / "src/battle_script_commands.c").read_text()
    for token in ("gCurrentMove == MOVE_ORDER_UP", "SPECIES_TATSUGIRI_DROOPY", "SPECIES_TATSUGIRI_STRETCHY"):
        if token not in commands:
            raise SystemExit(f"FAIL: Battle 95 Order Up form hook missing {token}")
    switch_ai = (ROOT / "src/battle_ai_switch_items.c").read_text()
    if "ShouldSwitchIfZeroToHero" not in switch_ai or "ABILITY_ZERO_TO_HERO" not in switch_ai:
        raise SystemExit("FAIL: Battle 95 Zero to Hero switch AI missing")
    combo_ai = (ROOT / "src/battle_ai_main.c").read_text()
    if "move == MOVE_ORDER_UP" not in combo_ai or "gBattleStruct->commanderActive[battlerAtk]" not in combo_ai:
        raise SystemExit("FAIL: Battle 95 Commander combo AI missing")

    dialogue_file = (ROOT / "data/text/trainers.inc").read_text()
    dialogue = dialogue_file.split("Route114_Text_NolanIntro:", 1)[1].split("Route114_Text_TyraIntro:", 1)[0]
    for cue in ("boss and its commander", "Tatsugiri orders Dondozo", "Palafin dives out", "sends Tatsugiri inside", "raises all five stats", "Order Up", "expose Tatsugiri", "become a hero", "commander needs its partner"):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 95 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 95 overlong dialogue: {visible}")

    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in corpus_ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 95 competitive reference missing from corpus")


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
                raise SystemExit(f"FAIL: Battle 95 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
        entry = next(row for row in guide["entries"] if row["trainerId"] == "TRAINER_NOLAN")
        if entry["designStatus"] != "closed" or entry["format"] != "double":
            raise SystemExit("FAIL: Battle 95 guide status/format stale")
        if [member["speciesId"] for member in entry["party"]] != [member["species"] for member in TEAM]:
            raise SystemExit("FAIL: Battle 95 guide party stale")
    print("PASS: Battle 95 Nolan champion pond mechanics double is source-closed")


if __name__ == "__main__":
    main()
