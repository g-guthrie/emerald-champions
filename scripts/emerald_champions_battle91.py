#!/usr/bin/env python3
"""Generate/check Battle 91, Tori and Tia's mirrored Dancer double."""

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
PRESETS = ROOT / "docs/verdant_battle_set_presets.json"

TEAM = [
    {
        "level": 1,
        "species": "SPECIES_VOLCARONA",
        "item": "ITEM_CHARTI_BERRY",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
        "moves": ["MOVE_QUIVER_DANCE", "MOVE_HEAT_WAVE", "MOVE_BUG_BUZZ", "MOVE_GIGA_DRAIN"],
    },
    {
        "level": 2,
        "species": "SPECIES_ORICORIO_POM_POM",
        "item": "ITEM_SHARP_BEAK",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
        "moves": ["MOVE_REVELATION_DANCE", "MOVE_AIR_SLASH", "MOVE_ROOST", "MOVE_TAUNT"],
    },
    {
        "level": 3,
        "species": "SPECIES_FROSMOTH",
        "item": "ITEM_FOCUS_SASH",
        "ability_slot": 2,
        "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
        "moves": ["MOVE_QUIVER_DANCE", "MOVE_ICE_BEAM", "MOVE_BUG_BUZZ", "MOVE_GIGA_DRAIN"],
    },
    {
        "level": 4,
        "species": "SPECIES_ORICORIO_SENSU",
        "item": "ITEM_SPELL_TAG",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
        "moves": ["MOVE_REVELATION_DANCE", "MOVE_AIR_SLASH", "MOVE_ROOST", "MOVE_HELPING_HAND"],
    },
]

REFERENCES = ["showdown:gen5randomdoublesbattle:030"]
LOCAL_REFERENCE_IDS = [
    "preset:SPECIES_FROSMOTH:ice-scales-quiver",
    "preset:SPECIES_ORICORIO_POM_POM:dancer-support",
    "preset:SPECIES_ORICORIO_SENSU:dancer-support",
]

NEXT = {
    "index": 92,
    "encounter_id": "BATTLE_092_ROUTE_113_LAO",
    "location": "Route113",
    "category": "optional buried Ninja Boy first battle",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_LAO_1"],
    "access_note": (
        "Lao is the next westbound physical encounter after the twins: a buried Ninja Boy at (29,6) with "
        "one-tile all-direction sight. His base record owns a Match Call rematch family; Battle 92 closes the "
        "first reachable ash ambush without claiming the later tiers."
    ),
}


def design() -> dict:
    return {
        "guide_order": 91,
        "trainer_ids": ["TRAINER_TORI_AND_TIA"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Optional fixed Route 113 twin double after Madeline. Two adjacent objects invoke the same guarded "
            "four-member record and expose distinct Tori/Tia intro, defeat, post-battle, and one-Pokemon guard paths."
        ),
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 fully evolved twin recital after a middle-stage single",
            "effective_levels": "41, 42, 43, and 44",
            "eligible_ratio": "4/4",
            "mega_access": True,
            "status": "pass",
            "reason": (
                "Volcarona, Frosmoth, and both permanent Oricorio forms are final forms. The preceding Madeline "
                "battle already uses the route's late-bloomer window; this battle deliberately demonstrates the "
                "fully evolved payoff of Quiver Dance and Dancer."
            ),
        },
        "manual_quality": 10,
        "manual_difficulty": 8.8,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {
                    "reference_id": REFERENCES[0],
                    "decision": "selected Volcarona role; full donor rejected",
                    "reason": (
                        "The generated doubles record supplies Flame Body Quiver Dance and sustain. The exact "
                        "two-pair Dancer topology is authored from local Oricorio/Frosmoth legality and the twin map."
                    ),
                }
            ],
            "local_authored_references": LOCAL_REFERENCE_IDS,
            "decision": (
                "The retained ranked corpus has exact Volcarona but no Frosmoth or Oricorio random-team records. "
                "Their independently audited Emerald Champions presets cite Smogon doubles support/Quiver roles and "
                "exact local legality; the absence of direct corpus evidence remains explicit rather than fabricated."
            ),
        },
        "competitive_references": [
            {
                "reference_id": REFERENCES[0],
                "adaptation": "Volcarona keeps Flame Body, Quiver Dance, and Giga Drain, using legal Heat Wave/Bug Buzz for immediate doubles pressure.",
            },
            {
                "reference_id": LOCAL_REFERENCE_IDS[0],
                "adaptation": "Authored Smogon-PU-derived Frosmoth Quiver role keeps Ice Scales and legal Ice/Bug/Grass coverage, dropping Protect for a public Sash.",
            },
            {
                "reference_id": LOCAL_REFERENCE_IDS[1],
                "adaptation": "Pom-Pom's audited Dancer support becomes the first pair's Electric Revelation Dance, Taunt, and Roost partner.",
            },
            {
                "reference_id": LOCAL_REFERENCE_IDS[2],
                "adaptation": "Sensu's audited Dancer support becomes the second pair's Ghost Revelation Dance and Helping Hand partner.",
            },
        ],
        "ordering": {
            "intended_lead": ["SPECIES_VOLCARONA", "SPECIES_ORICORIO_POM_POM"],
            "intended_reserve_pair": ["SPECIES_FROSMOTH", "SPECIES_ORICORIO_SENSU"],
            "source_order": [
                "SPECIES_VOLCARONA", "SPECIES_ORICORIO_POM_POM", "SPECIES_FROSMOTH", "SPECIES_ORICORIO_SENSU"
            ],
            "reason": (
                "Each moth precedes its matching Dancer partner in one battle wave. Volcarona/Pom-Pom is the "
                "Fire/Electric recital; Frosmoth/Sensu is the Ice/Ghost echo."
            ),
        },
        "team_intent": (
            "Volcarona or Frosmoth can spend one turn on Quiver Dance; the active Oricorio copies the same boosts "
            "automatically through Dancer. Charti Berry and Focus Sash prevent the two 4x-Rock moths from collapsing "
            "to the first correct hit. Pom-Pom adds Electric/Flying coverage and Taunt; Sensu adds Ghost/Flying "
            "coverage and Helping Hand. Both birds can recover, but there is no Protect or speed field."
        ),
        "intended_counterplay": (
            "Rock Slide and other Rock attacks are the clearest answer, but Charti/Sash make the first hit a tempo "
            "exchange rather than an instant wipe. Taunt, Haze, Clear Smog, phazing, Unaware, priority after Sash, "
            "physical attacks into Ice Scales Frosmoth, Electric/Ice/Water/Fire coverage by slot, and concentrated "
            "damage on the initiating moth all work. The player never needs to guess which partner will copy."
        ),
        "bespoke_ai": (
            "The record uses smart switching, partner help, HP awareness, and Combo Setup. The reusable AI explicitly "
            "adds value when an ally has Dancer and the selected move carries FLAG_DANCE, so each moth understands the "
            "double-boost turn. Taunt, Roost, Helping Hand, Sash, Charti, Flame Body, Ice Scales, and spread Heat Wave "
            "use public state. No sleep, confusion, Protect, Tailwind, Trick Room, evasion, or hidden input read remains."
        ),
        "uniqueness": (
            "All four exact species/forms are new to the first 90 encounters. Battle 14 introduced a juvenile three-"
            "dance recital 77 encounters ago; this is its fully evolved twin exam with two exact moth/bird waves, "
            "different types, no confusion, and explicit reusable Dancer AI. It is progression, not nearby repetition."
        ),
        "story_logic": (
            "Both twins now describe the ash recital, which moth leads each Oricorio, and the public Dancer copy. "
            "Separate Tori/Tia defeat and post-battle paths remain distinct, and both one-Pokemon guard lines truthfully "
            "request two able Pokemon. The old White Flute, Hypnosis, Teeter Dance, and duplicate Spinda claims are gone."
        ),
        "reward_logic": "EXP and prize money only; the twins own no item, rematch, story flag, or progression reward.",
        "campaign_reservations": {
            "spends": [
                "fully evolved mirrored Dancer recital",
                "Volcarona/Pom-Pom Fire-Electric pair",
                "Frosmoth/Sensu Ice-Ghost pair",
            ],
            "preserves": [
                "sleep/confusion dance teams",
                "weather Quiver teams",
                "Baton Pass chains",
                "other Oricorio forms",
                "Volcarona/Frosmoth boss roles",
            ],
            "repeat_rule": (
                "These exact forms should not recur soon. Later Quiver or Dancer teams must change the initiator, "
                "positioning, and answer rather than replay two moth/bird waves."
            ),
        },
        "author_self_check": {
            "strongest_part": (
                "The twin visual reads instantly and the AI actually executes it: one moth dances, one Oricorio copies, "
                "then the second typed pair repeats the lesson at a harder angle."
            ),
            "weakest_link": (
                "Every member is Rock-weak, and both moths are 4x weak. Charti/Sash force at least one interaction, but "
                "a prepared Rock plan is intentionally decisive broad counterplay, which keeps this explosive setup "
                "battle at 8.8 rather than pretending it is a boss."
            ),
        },
        "closure": (
            "Battle 91 is source-closed at quality 10 and target difficulty 8.8: four fresh legal levels 41-44, four "
            "distinct items, exact moth/Dancer wave order, both guarded map scripts, reusable combo AI proof, native "
            "width-safe dialogue, one generated and three audited local references, broad Rock/setup counterplay, no "
            "reward debt, and no sleep/confusion/Protect/speed-field luck. Runtime remains unplayed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 91,
        "encounter_id": "BATTLE_091_ROUTE_113_TORI_AND_TIA",
        "identity": {
            "location": "Route113",
            "category": "optional fixed twin double",
            "format": "double",
            "strict_cap": 40,
            "memory_hook": (
                "Volcarona dances with Pom-Pom Oricorio; Frosmoth repeats the recital with Sensu Oricorio. Dancer "
                "copies each Quiver Dance into a second boosted attacker."
            ),
        },
        "primary_player_question": (
            "Can the player stop the initiating moth before Dancer doubles the setup value, then repeat the answer "
            "against a second pair with different Fire/Electric and Ice/Ghost coverage?"
        ),
        "tempo": (
            "Two consecutive moth-and-Dancer setup waves with immediate special coverage, finite anti-Rock items, "
            "Taunt/Helping Hand, and Roost; no Protect or persistent speed field."
        ),
        "pressure_sources": [
            "level-41 Charti Flame Body Volcarona",
            "level-42 Sharp Beak Dancer Pom-Pom Oricorio",
            "level-43 Focus Sash Ice Scales Frosmoth",
            "level-44 Spell Tag Dancer Sensu Oricorio",
            "two Quiver Dance copy turns and four-way typed special coverage",
        ],
        "intentional_opening": "Volcarona/Pom-Pom is fixed; Frosmoth/Sensu is the source-ordered reserve wave.",
        "intentional_weakness": (
            "Universal Rock weakness, 4x moth weaknesses, no Protect/speed field, special-offense concentration, "
            "setup denial, item removal, and finite Charti/Sash safety."
        ),
        "first_loss_lesson": (
            "The bird is not setting up independently. Stop, Taunt, haze, or immediately focus the moth that initiates "
            "the dance; otherwise one turn creates two boosted attackers."
        ),
        "revealed_information": [
            "cap 40",
            "guarded four-member double",
            "levels 41-44",
            "two exact Dancer waves",
            "all four species/forms fresh",
            "no sleep/confusion/Protect/speed field",
            "no reward/rematch",
        ],
        "counterplay_classes": [
            "Rock Slide and concentrated Rock attacks",
            "Taunt/Haze/Clear Smog/phazing/Unaware",
            "priority after Sash",
            "physical pressure into Frosmoth",
            "item removal",
            "typed Electric/Ice/Water/Fire attacks by slot",
        ],
        "target_difficulty": 8.8,
        "difficulty_rationale": (
            "Two potential double-boost turns, four optimized fresh levels 41-44, anti-one-hit items, typed coverage, "
            "and support create a severe route double. Universal public Rock counterplay keeps it below boss difficulty."
        ),
        "tuning_knob": "Tune Sensu from +4 to +3 first; preserve both pairs, items, and the Dancer lesson.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": [
            "route-double", "twins", "mirrored-pairs", "dancer", "quiver-dance", "volcarona", "frosmoth",
            "oricorio-pom-pom", "oricorio-sensu", "charti", "focus-sash", "no-sleep", "no-confusion",
            "no-protect", "no-speed-field", "no-mega", "no-legendary",
        ],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {
            "status": "complete-current-review-plus-local-presets",
            "pool_size": 1005,
            "selection": "Exact generated Volcarona plus audited Smogon-derived Frosmoth/Oricorio local presets.",
        },
        "author_self_check": {
            "strongest_part": "One public dance command advances two visually matched Pokemon, twice.",
            "weakest_link": "Rock compresses all four; anti-one-hit items make it decisive rather than trivial.",
        },
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_091_ROUTE_113_TORI_AND_TIA"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [entry for entry in ledger["entries"] if entry["index"] != 91] + [ledger_entry()]
    ledger["entries"].sort(key=lambda entry: entry["index"])
    sequence = json.loads(SEQUENCE.read_text())
    sequence["entries"] = [entry for entry in sequence["entries"] if entry["index"] != 92] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda entry: entry["index"])
    for entry in sequence["entries"]:
        if entry["index"] <= 91:
            entry["status"] = "closed"
        elif entry["index"] == 92:
            entry["status"] = "next"
        else:
            entry["status"] = "queued"
    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update(
        {
            "closed_encounters": 91,
            "next_index": 92,
            "next_encounter_id": "BATTLE_092_ROUTE_113_LAO",
            "queued_sequence_entries": 0,
            "canonical_sequence_groups": 92,
            "physical_encounter_groups": 529,
            "unordered_physical_groups": 437,
        }
    )
    return designs, ledger, sequence, operating_system


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    block = doubles.trainer_blocks(trainers)["TRAINER_TORI_AND_TIA"].group(0)
    body = doubles.party_match(parties, doubles.party_name(block)).group(2)
    actual = [polish.parse_entry(entry) for entry in custom.party_entries(body)]
    if actual != TEAM:
        raise SystemExit("FAIL: Battle 91 source party differs")
    for token in (
        ".doubleBattle = TRUE",
        "AI_FLAG_SMART_SWITCHING",
        "AI_FLAG_HELP_PARTNER",
        "AI_FLAG_HP_AWARE",
        "AI_FLAG_COMBO_SETUP",
    ):
        if token not in block:
            raise SystemExit(f"FAIL: Battle 91 missing {token}")

    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for member in TEAM:
        illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
        if illegal:
            raise SystemExit(f"FAIL: Battle 91 illegal moves for {member['species']}: {illegal}")
        if member["ability_slot"] >= len(slots[member["species"]]):
            raise SystemExit(f"FAIL: Battle 91 invalid ability slot for {member['species']}")
    if len({member["species"] for member in TEAM}) != 4 or len({member["item"] for member in TEAM}) != 4:
        raise SystemExit("FAIL: Battle 91 species/items are not unique")
    for forbidden in ("MOVE_PROTECT", "MOVE_HYPNOSIS", "MOVE_TEETER_DANCE", "MOVE_TAILWIND", "MOVE_TRICK_ROOM"):
        if forbidden in body:
            raise SystemExit(f"FAIL: Battle 91 retained forbidden move {forbidden}")

    scripts = (ROOT / "data/maps/Route113/scripts.inc").read_text()
    for prefix in ("Tori", "Tia"):
        command = (
            f"trainerbattle_double TRAINER_TORI_AND_TIA, Route113_Text_{prefix}Intro, "
            f"Route113_Text_{prefix}Defeat, Route113_Text_{prefix}NotEnoughMons"
        )
        if command not in scripts:
            raise SystemExit(f"FAIL: Battle 91 missing guarded {prefix} script")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]["TRAINER_TORI_AND_TIA"]
    if manifest != {
        "format": "double",
        "target_size": 4,
        "archetype": "Mirrored moth-and-Dancer recital",
        "difficulty": 88,
        "partner_interaction": True,
        "level_offset": 3,
        "location": "Route 113",
    }:
        raise SystemExit("FAIL: Battle 91 format manifest stale")

    ai = (ROOT / "src/battle_ai_main.c").read_text()
    for token in (
        "partnerAbility == ABILITY_DANCER",
        "TestMoveFlags(move, FLAG_DANCE)",
        "score += 12",
    ):
        if token not in ai:
            raise SystemExit(f"FAIL: Battle 91 Dancer combo AI missing {token}")
    move_data = (ROOT / "src/data/battle_moves.h").read_text()
    quiver_block = move_data.split("[MOVE_QUIVER_DANCE]", 1)[1].split("[MOVE_", 1)[0]
    if "FLAG_DANCE" not in quiver_block:
        raise SystemExit("FAIL: Battle 91 Quiver Dance is not marked as a dance")

    dialogue_file = (ROOT / "data/text/trainers.inc").read_text()
    dialogue = dialogue_file.split("Route113_Text_ToriIntro:", 1)[1].split("Route113_Text_CobyIntro:", 1)[0]
    for cue in (
        "dance the ash in pairs",
        "Volcarona leads Pom-Pom",
        "Frosmoth leads Sensu",
        "Dancer copies each Quiver Dance",
        "No sleep, no confusion",
        "Bring two able Pokémon",
    ):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 91 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 91 overlong dialogue: {visible}")

    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in corpus_ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 91 generated reference missing from corpus")
    preset_text = PRESETS.read_text()
    for species in ("SPECIES_FROSMOTH", "SPECIES_ORICORIO_POM_POM", "SPECIES_ORICORIO_SENSU"):
        if species not in preset_text:
            raise SystemExit(f"FAIL: Battle 91 local preset evidence missing {species}")


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
                raise SystemExit(f"FAIL: Battle 91 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
        entry = next(row for row in guide["entries"] if row["trainerId"] == "TRAINER_TORI_AND_TIA")
        if entry["designStatus"] != "closed" or entry["format"] != "double":
            raise SystemExit("FAIL: Battle 91 guide status/format stale")
        if [member["speciesId"] for member in entry["party"]] != [member["species"] for member in TEAM]:
            raise SystemExit("FAIL: Battle 91 guide party stale")
    print("PASS: Battle 91 Tori and Tia mirrored Dancer double is source-closed")


if __name__ == "__main__":
    main()
