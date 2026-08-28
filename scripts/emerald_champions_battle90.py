#!/usr/bin/env python3
"""Generate/check Battle 90, Madeline's first Route 113 late-bloomer relay."""

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
        "species": "SPECIES_NOIBAT",
        "item": "ITEM_EVIOLITE",
        "ability_slot": 1,
        "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
        "moves": ["MOVE_U_TURN", "MOVE_AIR_SLASH", "MOVE_DRAGON_PULSE", "MOVE_SUPER_FANG"],
    },
    {
        "level": 2,
        "species": "SPECIES_MIENFOO",
        "item": "ITEM_BLACK_BELT",
        "ability_slot": 1,
        "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
        "moves": ["MOVE_FAKE_OUT", "MOVE_DRAIN_PUNCH", "MOVE_KNOCK_OFF", "MOVE_U_TURN"],
    },
    {
        "level": 3,
        "species": "SPECIES_VANILLISH",
        "item": "ITEM_LEFTOVERS",
        "ability_slot": 2,
        "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
        "moves": ["MOVE_FREEZE_DRY", "MOVE_FLASH_CANNON", "MOVE_WATER_PULSE", "MOVE_MIRROR_COAT"],
    },
]

REFERENCES = [
    "smogon:gen8uu:008",
    "showdown:gen7randomdoublesbattle:016",
    "showdown:gen5randomdoublesbattle:006",
]

NEXT = {
    "index": 91,
    "encounter_id": "BATTLE_091_ROUTE_113_TORI_AND_TIA",
    "location": "Route113",
    "category": "optional fixed twin double",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_TORI_AND_TIA"],
    "access_note": (
        "Tori and Tia are the next westbound physical encounter after Madeline: adjacent twin objects at "
        "(45,6) and (46,6), both facing down with one-tile sight and both invoking the same guarded "
        "TRAINER_TORI_AND_TIA record. One dossier must verify both object scripts and shared dialogue paths."
    ),
}


def design() -> dict:
    return {
        "guide_order": 90,
        "trainer_ids": ["TRAINER_MADELINE_1"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Optional rotating Parasol Lady single at (51,11) after Jaylen and before the Route 113 twins. This "
            "dossier closes only the first reachable battle; Match Call rematch tiers remain separately reserved."
        ),
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 final honest middle-stage window",
            "effective_levels": "41, 42, and 43",
            "eligible_ratio": "3/3",
            "mega_access": True,
            "status": "pass",
            "reason": (
                "Noibat evolves at 48, Mienfoo at 50, and Vanillish at 47. Every member is below its natural "
                "evolution threshold at its exact opponent level, so Madeline shelters real late bloomers rather "
                "than artificially suppressing a form the player should already expect."
            ),
        },
        "manual_quality": 10,
        "manual_difficulty": 8.5,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {
                    "reference_id": reference_id,
                    "decision": "selected evolutionary-family role; full donor rejected",
                    "reason": (
                        "Noibat and Mienfoo have no exact generated entry in the retained corpus, so the corresponding "
                        "Noivern/Mienshao public role is reduced to local legal stage power. Vanillish is exact."
                    ),
                }
                for reference_id in REFERENCES
            ],
            "decision": (
                "Two mature-family roles and one exact Vanillish record support a hand-authored stage-legal single. "
                "No unsupported move, ability, or evolved-stat assumption is imported."
            ),
        },
        "competitive_references": [
            {
                "reference_id": REFERENCES[0],
                "adaptation": "Infiltrator Noivern's pivot role is reduced to Noibat's legal U-turn, Dragon Pulse, Air Slash, and Super Fang.",
            },
            {
                "reference_id": REFERENCES[1],
                "adaptation": "Regenerator Mienshao's Fake Out/U-turn pressure is reduced to legal Black Belt Mienfoo with Drain Punch and Knock Off.",
            },
            {
                "reference_id": REFERENCES[2],
                "adaptation": "Vanillish keeps immediate Ice offense but drops Explosion, setup, weather dependency, and Life Orb for coverage and Mirror Coat.",
            },
        ],
        "ordering": {
            "intended_lead": ["SPECIES_NOIBAT"],
            "source_order": ["SPECIES_NOIBAT", "SPECIES_MIENFOO", "SPECIES_VANILLISH"],
            "reason": (
                "Fast Eviolite Noibat reveals Super Fang or pivots first; Regenerator Mienfoo controls the middle; "
                "Leftovers Weak Armor Vanillish is the non-pivot anchor and special counter threat."
            ),
        },
        "team_intent": (
            "Noibat uses Super Fang to make raw bulk finite, attacks through substitutes with Infiltrator, or U-turns "
            "into the correct axis. Mienfoo uses one Fake Out, Black Belt Drain Punch, Knock Off, and Regenerator U-turn "
            "to punish passive answers. Vanillish breaks the relay with Freeze-Dry/Flash Cannon/Water Pulse coverage "
            "and threatens Mirror Coat against careless special attacks; Weak Armor creates a visible physical tradeoff."
        ),
        "intended_counterplay": (
            "Rock/Ice/Electric/Fairy/Dragon pressure answers Noibat; Psychic/Flying/Fairy, burn, and noncontact burst "
            "answer Mienfoo; Fire/Fighting/Rock/Steel and physical pressure answer Vanillish, but special attackers must "
            "respect Mirror Coat. Stealth Rock and other hazards tax both U-turns, Taunt limits Mirror Coat reads, and "
            "Knock Off removes the three public sustain items."
        ),
        "bespoke_ai": (
            "Madeline uses smart switching and HP awareness. Noibat and Mienfoo U-turn only when a visible reserve "
            "improves the matchup; Super Fang loses value on sufficiently low HP; Fake Out is not repeated into immunity; "
            "Regenerator, Drain Punch, Weak Armor, Leftovers, and Mirror Coat use public state and damage history. No "
            "weather, Protect, setup, sleep, Toxic, evasion, or hidden input read is present."
        ),
        "uniqueness": (
            "Noibat and Vanillish are new to the first 89 encounters. Mienfoo last appeared 59 battles ago as one member "
            "of Brawly Gym's four-Pokemon double; here it owns the central Regenerator pivot in a three-member single. "
            "The encounter is a concise maturation relay after three consecutive complex/double source closures."
        ),
        "story_logic": (
            "Madeline's parasol no longer protects an underleveled Numel that should already be Camerupt. It shelters "
            "three exact late bloomers, and the rewritten intro/defeat/post-battle text truthfully explains retreat, "
            "Regenerator pressure, Vanillish's anchor role, and natural evolution timing. Registration remains native."
        ),
        "reward_logic": (
            "EXP, prize money, and native Match Call registration only. No item is granted. Later rematch records are "
            "not silently claimed by this first-battle closure."
        ),
        "campaign_reservations": {
            "spends": [
                "Parasol late-bloomer shelter",
                "Super Fang/U-turn Noibat",
                "Regenerator Mienfoo relay",
                "Mirror Coat Vanillish anchor",
            ],
            "preserves": [
                "Noivern/Mienshao/Vanilluxe final-form showcases",
                "Madeline rematch escalation",
                "weather parasol teams",
                "future full pivot cores",
            ],
            "repeat_rule": (
                "Later members of these families should appear evolved or in different roles; Madeline rematches must "
                "escalate from this identity rather than revert to the old sun/hail grab bag."
            ),
        },
        "author_self_check": {
            "strongest_part": (
                "The encounter uses the disappearing early-game evolution window exactly as intended: every young "
                "Pokemon is mechanically useful and demonstrably below its own evolution threshold."
            ),
            "weakest_link": (
                "Hazards can punish both pivots heavily and Rock pressure overlaps Noibat and Vanillish. That is "
                "intentional broad counterplay; +1/+2/+3 levels, Eviolite, Regenerator, and Mirror Coat compensate."
            ),
        },
        "closure": (
            "Battle 90 is source-closed at quality 10 and target difficulty 8.5: three legal levels 41-43, three distinct "
            "items, exact evolution-stage proof, two fresh species, one distant role-changed repeat, native Match Call "
            "routing, rewritten width-safe dialogue, smart singles AI, three honest references, broad counterplay, and "
            "no weather/Protect/setup/sleep/Toxic debt. Runtime remains unplayed; rematch tiers remain separately reserved."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 90,
        "encounter_id": "BATTLE_090_ROUTE_113_MADELINE",
        "identity": {
            "location": "Route113",
            "category": "optional moving Parasol Lady first battle",
            "format": "single",
            "strict_cap": 40,
            "memory_hook": (
                "Madeline shelters three honest late bloomers: Noibat cuts bulk and pivots, Regenerator Mienfoo rotates, "
                "and Mirror Coat Vanillish stays under the parasol to catch the answer."
            ),
        },
        "primary_player_question": (
            "Can the player tax the pivot relay without donating item value or a Mirror Coat knockout, then switch "
            "from Noibat's special pressure to Mienfoo's physical pressure and back?"
        ),
        "tempo": (
            "Three-member singles relay with two U-turn users, finite Super Fang, one Fake Out, Regenerator/Drain Punch, "
            "and a non-pivot special anchor; no setup or field."
        ),
        "pressure_sources": [
            "level-41 Eviolite Infiltrator Noibat",
            "level-42 Black Belt Regenerator Mienfoo",
            "level-43 Leftovers Weak Armor Vanillish",
            "Super Fang, U-turn, Knock Off, Freeze-Dry, and Mirror Coat",
        ],
        "intentional_opening": "Noibat is fixed; visible matchup and HP determine U-turn rather than a scripted escape.",
        "intentional_weakness": (
            "Rock overlap, hazard-sensitive pivots, broad Fairy pressure on the first two, no setup/field/Protect, and "
            "Vanillish's physical Fire/Fighting/Rock/Steel seams."
        ),
        "first_loss_lesson": (
            "Do not chase every U-turn. Put a hazard or item-removal tax on the relay, preserve the right damage axis, "
            "and hit Vanillish physically instead of feeding Mirror Coat."
        ),
        "revealed_information": [
            "cap 40",
            "levels 41-43",
            "intentional single",
            "Noibat evolves at 48",
            "Mienfoo evolves at 50",
            "Vanillish evolves at 47",
            "native Match Call registration",
            "later rematches reserved",
        ],
        "counterplay_classes": [
            "hazards and Knock Off",
            "Rock/Ice/Electric/Fairy/Dragon into Noibat",
            "Psychic/Flying/Fairy/burn into Mienfoo",
            "physical Fire/Fighting/Rock/Steel into Vanillish",
            "Taunt and Mirror Coat awareness",
        ],
        "target_difficulty": 8.5,
        "difficulty_rationale": (
            "Three optimized levels 41-43 with pivoting, Regenerator, item removal, broad coverage, and a counter-move "
            "anchor meet the serious-contender floor. Shared seams, hazard sensitivity, and no setup/field keep it concise."
        ),
        "tuning_knob": "Tune Vanillish from +3 to +2 first; preserve all three stages, items, order, and pivot identity.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": [
            "route-single", "parasol", "late-bloomers", "honest-middle-stages", "pivot-relay", "super-fang",
            "regenerator", "mirror-coat", "weak-armor", "no-weather", "no-protect", "no-setup", "no-sleep",
            "no-toxic", "no-mega", "no-legendary", "match-call-first-battle",
        ],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {
            "status": "complete-current-review",
            "pool_size": 1005,
            "selection": "Two reduced mature-family roles plus one exact Vanillish record; all local stage legality rechecked.",
        },
        "author_self_check": {
            "strongest_part": "Every unevolved species is useful now and naturally below its exact evolution threshold.",
            "weakest_link": "Hazards/Rock compress the relay; level and item advantages compensate without hiding that answer.",
        },
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_090_ROUTE_113_MADELINE"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [entry for entry in ledger["entries"] if entry["index"] != 90] + [ledger_entry()]
    ledger["entries"].sort(key=lambda entry: entry["index"])
    sequence = json.loads(SEQUENCE.read_text())
    sequence["entries"] = [entry for entry in sequence["entries"] if entry["index"] != 91] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda entry: entry["index"])
    for entry in sequence["entries"]:
        if entry["index"] <= 90:
            entry["status"] = "closed"
        elif entry["index"] == 91:
            entry["status"] = "next"
        else:
            entry["status"] = "queued"
    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update(
        {
            "closed_encounters": 90,
            "next_index": 91,
            "next_encounter_id": "BATTLE_091_ROUTE_113_TORI_AND_TIA",
            "queued_sequence_entries": 0,
            "canonical_sequence_groups": 91,
            "physical_encounter_groups": 529,
            "unordered_physical_groups": 438,
        }
    )
    return designs, ledger, sequence, operating_system


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    block = doubles.trainer_blocks(trainers)["TRAINER_MADELINE_1"].group(0)
    body = doubles.party_match(parties, doubles.party_name(block)).group(2)
    actual = [polish.parse_entry(entry) for entry in custom.party_entries(body)]
    if actual != TEAM:
        raise SystemExit("FAIL: Battle 90 source party differs")
    for token in (".doubleBattle = FALSE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HP_AWARE"):
        if token not in block:
            raise SystemExit(f"FAIL: Battle 90 missing {token}")

    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for member in TEAM:
        illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
        if illegal:
            raise SystemExit(f"FAIL: Battle 90 illegal moves for {member['species']}: {illegal}")
        if member["ability_slot"] >= len(slots[member["species"]]):
            raise SystemExit(f"FAIL: Battle 90 invalid ability slot for {member['species']}")
    if [(member["species"], 40 + member["level"]) for member in TEAM] != [
        ("SPECIES_NOIBAT", 41), ("SPECIES_MIENFOO", 42), ("SPECIES_VANILLISH", 43)
    ]:
        raise SystemExit("FAIL: Battle 90 stage levels drifted")

    scripts = (ROOT / "data/maps/Route113/scripts.inc").read_text()
    if "trainerbattle_single TRAINER_MADELINE_1" not in scripts:
        raise SystemExit("FAIL: Battle 90 first battle is not an intentional single")
    for token in ("ShouldTryRematchBattle", "Route113_EventScript_RematchMadeline", "register_matchcall TRAINER_MADELINE_1"):
        if token not in scripts:
            raise SystemExit(f"FAIL: Battle 90 Match Call routing missing {token}")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]["TRAINER_MADELINE_1"]
    if manifest != {
        "format": "single",
        "target_size": 3,
        "archetype": "Sheltered late-bloomer pivot relay",
        "difficulty": 85,
        "partner_interaction": False,
        "level_offset": 2,
        "location": "Route 113",
    }:
        raise SystemExit("FAIL: Battle 90 format manifest stale")

    dialogue_file = (ROOT / "data/text/trainers.inc").read_text()
    dialogue = dialogue_file.split("Route113_Text_MadelineIntro:", 1)[1].split("Route113_Text_LaoIntro:", 1)[0]
    for cue in (
        "shelters three young",
        "retreat, recover, and return",
        "Noibat and Mienfoo pivot",
        "Vanillish stays",
        "None is being held back",
    ):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 90 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 90 overlong dialogue: {visible}")

    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in corpus_ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 90 competitive reference missing from corpus")


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
                raise SystemExit(f"FAIL: Battle 90 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
        entry = next(row for row in guide["entries"] if row["trainerId"] == "TRAINER_MADELINE_1")
        if entry["designStatus"] != "closed" or entry["format"] != "single":
            raise SystemExit("FAIL: Battle 90 guide status/format stale")
        if [member["speciesId"] for member in entry["party"]] != [member["species"] for member in TEAM]:
            raise SystemExit("FAIL: Battle 90 guide party stale")
    print("PASS: Battle 90 Madeline first-battle late-bloomer relay is source-closed")


if __name__ == "__main__":
    main()
