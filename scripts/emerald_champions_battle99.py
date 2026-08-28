#!/usr/bin/env python3
"""Generate and verify Battle 99, Tyra and Ivy's fighting curriculum."""

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
        "species": "SPECIES_THROH",
        "item": "ITEM_MENTAL_HERB",
        "ability_slot": 1,
        "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
        "moves": ["MOVE_COACHING", "MOVE_STORM_THROW", "MOVE_KNOCK_OFF", "MOVE_WIDE_GUARD"],
    },
    {
        "level": 2,
        "species": "SPECIES_PANGORO",
        "item": "ITEM_WEAKNESS_POLICY",
        "ability_slot": 2,
        "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
        "moves": ["MOVE_DRAIN_PUNCH", "MOVE_KNOCK_OFF", "MOVE_POISON_JAB", "MOVE_PROTECT"],
    },
    {
        "level": 3,
        "species": "SPECIES_HITMONCHAN",
        "item": "ITEM_ASSAULT_VEST",
        "ability_slot": 2,
        "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
        "moves": ["MOVE_DRAIN_PUNCH", "MOVE_ICE_PUNCH", "MOVE_THUNDER_PUNCH", "MOVE_MACH_PUNCH"],
    },
    {
        "level": 4,
        "species": "SPECIES_SIRFETCHD",
        "item": "ITEM_LEEK",
        "ability_slot": 2,
        "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
        "moves": ["MOVE_CLOSE_COMBAT", "MOVE_BRAVE_BIRD", "MOVE_LEAF_BLADE", "MOVE_PROTECT"],
    },
]

REFERENCES = [
    "showdown:gen8randomdoublesbattle:024",
    "showdown:gen6randomdoublesbattle:021",
    "showdown:gen7randomdoublesbattle:023",
    "smogon:gen4nu:002",
]

NEXT = {
    "index": 100,
    "encounter_id": "BATTLE_100_ROUTE_114_SHANE",
    "location": "Route114",
    "category": "optional south-central Poké Maniac single",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_SHANE"],
    "access_note": (
        "Shane faces right at (22,50) with three-tile sight on the main path immediately below Tyra and Ivy. "
        "He is the next north-to-south physical encounter before Steve and Bernie."
    ),
}


def design() -> dict:
    return {
        "guide_order": 99,
        "trainer_ids": ["TRAINER_TYRA_AND_IVY"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Optional shared student-mentor double at Route 114's central bend. Tyra and Ivy occupy adjacent objects at "
            "(23,44)/(24,44); either one-tile sight script invokes the same guarded four-member trainer record."
        ),
        "runtime_branches": [
            "Tyra object initiates the same guarded double with Tyra-specific intro/defeat/post-battle text.",
            "Ivy object initiates the same guarded double with Ivy-specific intro/defeat/post-battle text.",
            "There is no split single or second party definition; both objects share one defeat state and exact source roster."
        ],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 mature student-mentor fighting curriculum",
            "effective_levels": "41, 42, 43, and 44",
            "eligible_ratio": "4/4",
            "mega_access": True,
            "status": "pass",
            "reason": (
                "Throh is single-stage; Pangoro evolves from Pancham from level 32 with a Dark ally; Hitmonchan evolves from "
                "Tyrogue at level 20; Sirfetch'd uses its native critical-hit evolution path. All are mature before cap 40."
            ),
        },
        "manual_quality": 10,
        "manual_difficulty": 9.2,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {
                    "reference_id": REFERENCES[0],
                    "decision": "Throh bulk selected; full donor rejected",
                    "reason": "The generated doubles team validates Throh at serious stakes; local Coaching and Wide Guard turn that bulk into the exact senior-teacher role."
                },
                {
                    "reference_id": REFERENCES[1],
                    "decision": "Pangoro physical breaker selected; full donor rejected",
                    "reason": "The generated team validates Pangoro as immediate doubles pressure without importing unrelated weather or speed structures."
                },
                {
                    "reference_id": REFERENCES[2],
                    "decision": "Hitmonchan priority coverage selected; full donor rejected",
                    "reason": "The reproducible set supports four-punch coverage; Emerald Champions' Blitz Boxer makes the specialist lesson native and distinct."
                },
                {
                    "reference_id": REFERENCES[3],
                    "decision": "Hitmonchan competitive legitimacy corroborated; full donor rejected",
                    "reason": "Published NU offense confirms Hitmonchan is a real priority and coverage threat rather than novelty filler."
                },
            ],
            "decision": (
                "All 1005 references were reviewed. Indexed Throh, Pangoro, and Hitmonchan evidence plus the all-species "
                "Sirfetch'd review support every role; the four-style curriculum and guarded shared-object structure are "
                "transparently hand-authored for Tyra and Ivy."
            ),
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Throh's source bulk becomes a Mental Herb Inner Focus teacher with Coaching, guaranteed-crit Storm Throw, Knock Off, and Wide Guard."},
            {"reference_id": REFERENCES[1], "adaptation": "Pangoro becomes the coached Weakness Policy Scrappy pupil, with immediate Drain Punch/Knock Off/Poison Jab and Protect."},
            {"reference_id": REFERENCES[2], "adaptation": "Hitmonchan retains the four-punch coverage language and uses local Blitz Boxer to make its priority lesson mechanically real."},
            {"reference_id": REFERENCES[3], "adaptation": "Published Hitmonchan priority legitimizes the Assault Vest specialist without importing the donor's full hazard offense."},
            {"source": "docs/battle_set_reviews/085_galar.json", "adaptation": "Sirfetch'd keeps Scrappy Close Combat/Brave Bird wallbreaking; Leek plus Leaf Blade creates the visible critical-hit final lesson."},
        ],
        "ordering": {
            "intended_lead": ["SPECIES_THROH", "SPECIES_PANGORO"],
            "intended_reserves": ["SPECIES_HITMONCHAN", "SPECIES_SIRFETCHD"],
            "source_order": [member["species"] for member in TEAM],
            "reason": (
                "The senior Throh can Coach or Wide Guard while Scrappy Pangoro attacks through Ghosts and threatens Policy. "
                "Assault Vest Blitz Boxer Hitmonchan changes the board to priority coverage; Leek Sirfetch'd finishes with "
                "high-critical-hit physical commitments. Every member can attack if its nominal lesson is denied."
            ),
        },
        "team_intent": (
            "Level-41 Mental Herb Inner Focus Throh teaches through Coaching and Wide Guard but retains Storm Throw and Knock "
            "Off. Level-42 Weakness Policy Scrappy Pangoro turns the boost into Drain Punch/Knock Off/Poison Jab pressure and "
            "can Protect. Level-43 Assault Vest Blitz Boxer Hitmonchan delivers priority Drain, Ice, Thunder, or Mach Punches. "
            "Level-44 Scrappy Leek Sirfetch'd closes through Close Combat, Brave Bird, high-crit Leaf Blade, or Protect. The "
            "four styles are public, sequential, and independently functional."
        ),
        "intended_counterplay": (
            "Fairy, Flying, Psychic, strong special attacks, burn, Reflect, Intimidate, Unaware, Haze, redirection, and focused "
            "damage are broad. Remove or Taunt Throh after Mental Herb, use single-target attacks around Wide Guard, avoid "
            "carelessly triggering Pangoro's Policy, exploit its 4x Fairy weakness, and remember Scrappy defeats Ghost-only "
            "plans. Special bulk handles Hitmonchan but its priority Ice/Thunder coverage punishes frail Flying answers. "
            "Sirfetch'd pays Close Combat defenses and Brave Bird recoil; item removal, speed control, Rocky Helmet, or forcing "
            "Protect can contain it. Nothing requires one catch or exact turn."
        ),
        "bespoke_ai": (
            "The shared record gains smart switching, partner awareness, HP awareness, and Combo Setup. Existing AI values "
            "Coaching only when its ally can benefit and survive, Wide Guard only against actual spread pressure, and direct "
            "Storm Throw/Knock Off otherwise. It scores Weakness Policy state, Scrappy targeting, Blitz Boxer priority punches, "
            "coverage, recoil, Leek critical pressure, and Protect normally. No action, target, setup, item trigger, or reserve "
            "order is forced."
        ),
        "uniqueness": (
            "Pangoro, Hitmonchan, and Sirfetch'd are new to the first 98 encounters and absent from protected anchors. Throh's "
            "only prior appearance was 65 battles earlier as an immediate three-discipline single; here it is a true doubles "
            "mentor. This is the first Coaching curriculum and Blitz Boxer showcase, follows two singles with an active partner "
            "puzzle, and uses no weather, room, terrain, sleep, hazards, screens, trap, Mega, or legendary."
        ),
        "story_logic": (
            "Both object-specific intros now describe the same four public lessons from Tyra's senior and Ivy's junior point "
            "of view. Their post-battle text explains Coaching/Wide Guard or Scrappy/Blitz Boxer/Leek. Both scripts preserve "
            "the two-healthy guard and one shared trainer record, with no reward, rematch, or story flag."
        ),
        "reward_logic": "EXP and prize money only; the shared optional record owns no item, shop, legendary, Mega Stone, rematch, or progression reward.",
        "campaign_reservations": {
            "spends": ["first Coaching curriculum", "first Blitz Boxer Hitmonchan", "Scrappy Pangoro pupil", "Leek Sirfetch'd critical finish"],
            "preserves": ["every protected Fighting anchor and Mega", "historic Hitmontop support", "Beat Up/Justified", "Anger Point", "No Retreat", "Receiver and Costar mechanics"],
            "repeat_rule": "These three fresh species should not recur soon; Throh should not recur again without a materially different non-Coaching format."
        },
        "author_self_check": {
            "strongest_part": "The NPC relationship, four move/ability lessons, AI flags, and exact doubles board all say the same thing without a custom script or forced turn.",
            "weakest_link": "The roster is entirely physical and Fighting-majority. Wide Guard, Scrappy, Poison/Ice/Thunder/Flying coverage, priority, Policy, Leek, and +1 to +4 levels punish lazy counters, while special Fairy/Flying/Psychic pressure remains the intended broad answer."
        },
        "closure": (
            "Battle 99 is source-closed at quality 10 and target difficulty 9.2: both object scripts map to one guarded double; "
            "three fresh and one distant-repurposed legal mature fighters appear at levels 41-44 with four distinct items and "
            "styles; exact AI, source order, geometry, four indexed competitive references, handbook evidence, native-width "
            "dialogue, broad type/category/support counterplay, and zero reward debt are proven. Runtime remains unplayed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 99,
        "encounter_id": "BATTLE_099_ROUTE_114_TYRA_IVY",
        "identity": {
            "location": "Route114",
            "category": "optional student-mentor shared guarded double",
            "format": "double",
            "strict_cap": 40,
            "memory_hook": "Throh Coaches Scrappy Pangoro, Blitz Boxer Hitmonchan teaches priority punches, and Leek Sirfetch'd closes the four-style lesson."
        },
        "primary_player_question": "Can the player interrupt the Coaching lesson, attack around real Wide Guard, and preserve special Fairy/Flying/Psychic pressure through Scrappy, priority coverage, and the Leek closer?",
        "tempo": "Four-member fighting curriculum: support-plus-pupil lead, priority coverage specialist, then recoil/critical wallbreaker.",
        "pressure_sources": [
            "level-41 Mental Herb Inner Focus Throh with Coaching/Wide Guard/Storm Throw",
            "level-42 Weakness Policy Scrappy Pangoro with Drain Punch/Poison Jab",
            "level-43 Assault Vest Blitz Boxer Hitmonchan with four priority punches",
            "level-44 Leek Scrappy Sirfetch'd with Close Combat/Brave Bird/Leaf Blade"
        ],
        "intentional_opening": "Throh+Pangoro is fixed; Throh supports only when the visible board warrants it. Hitmonchan and Sirfetch'd are source reserves with direct attacks.",
        "intentional_weakness": "All physical, Fighting-majority, no speed field or recovery loop, one support body, Policy can be avoided, and Sirfetch'd pays defense drops/recoil.",
        "first_loss_lesson": "The mentor was an active battler, not a script. Remove or Taunt Throh, use single-target attacks around Wide Guard, and do not rely on Ghost immunity against Scrappy.",
        "revealed_information": ["cap 40", "two object scripts/one guarded double", "levels 41-44", "Coaching", "Wide Guard", "Scrappy", "Weakness Policy", "Blitz Boxer priority", "Leek critical pressure", "three fresh species", "no reward/rematch"],
        "counterplay_classes": ["special Fairy/Flying/Psychic", "Taunt/focus into Throh", "single-target attacks around Wide Guard", "burn/Reflect/Unaware/Haze/Intimidate", "Policy avoidance and item removal", "special bulk into Hitmonchan", "recoil/defense-drop/priority pressure into Sirfetch'd"],
        "target_difficulty": 9.2,
        "difficulty_rationale": "Four optimized levels 41-44, one real support lead, Policy, Scrappy, strong priority coverage, and a Leek closer make a severe optional double. Physical/type compression and lack of field/recovery preserve broad answers.",
        "tuning_knob": "Tune Sirfetch'd +4 to +3 first, then Hitmonchan +3 to +2; preserve species, lead, items, four lessons, and both object scripts.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["route-double", "student-mentor", "shared-trainer-record", "guarded-double", "coaching", "wide-guard", "throh", "pangoro", "hitmonchan", "sirfetchd", "scrappy", "weakness-policy", "blitz-boxer", "priority-punches", "leek", "critical-hits", "three-fresh-species", "no-weather", "no-room", "no-sleep", "no-mega", "no-legendary"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Indexed Throh/Pangoro/Hitmonchan evidence, published Hitmonchan, and complete Sirfetch'd review; curriculum is local."},
        "author_self_check": {"strongest_part": "Trainer relationship and battle mechanics form one readable lesson without forced AI.", "weakest_link": "Physical Fighting compression is intentional; wide coverage, support, priority, items, and levels make the public special/type answer necessary rather than free."},
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_099_ROUTE_114_TYRA_IVY"] = design()

    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 99] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])

    sequence = json.loads(SEQUENCE.read_text())
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 100] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        if row["index"] <= 99:
            row["status"] = "closed"
        elif row["index"] == 100:
            row["status"] = "next"
        else:
            row["status"] = "queued"

    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update(
        {
            "closed_encounters": 99,
            "next_index": 100,
            "next_encounter_id": NEXT["encounter_id"],
            "queued_sequence_entries": 0,
            "canonical_sequence_groups": 100,
            "physical_encounter_groups": 527,
            "unordered_physical_groups": 427,
        }
    )
    return designs, ledger, sequence, operating_system


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    block_text = doubles.trainer_blocks(trainers)["TRAINER_TYRA_AND_IVY"].group(0)
    body = doubles.party_match(parties, doubles.party_name(block_text)).group(2)
    actual = [polish.parse_entry(entry) for entry in custom.party_entries(body)]
    if actual != TEAM:
        raise SystemExit("FAIL: Battle 99 Tyra/Ivy source party differs")
    for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_COMBO_SETUP"):
        if token not in block_text:
            raise SystemExit(f"FAIL: Battle 99 Tyra/Ivy missing {token}")

    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for member in TEAM:
        illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
        if illegal:
            raise SystemExit(f"FAIL: Battle 99 illegal moves for {member['species']}: {illegal}")
        if member["ability_slot"] >= len(slots[member["species"]]):
            raise SystemExit(f"FAIL: Battle 99 invalid ability slot for {member['species']}")
    if len({member["species"] for member in TEAM}) != 4 or len({member["item"] for member in TEAM}) != 4:
        raise SystemExit("FAIL: Battle 99 species/items are not unique")

    object_events = {
        row["script"]: (row["x"], row["y"], row["movement_type"], str(row["trainer_sight_or_berry_tree_id"]))
        for row in json.loads((ROOT / "data/maps/Route114/map.json").read_text())["object_events"]
        if row.get("script") in {"Route114_EventScript_Tyra", "Route114_EventScript_Ivy"}
    }
    expected_geometry = {
        "Route114_EventScript_Tyra": (23, 44, "MOVEMENT_TYPE_FACE_DOWN", "1"),
        "Route114_EventScript_Ivy": (24, 44, "MOVEMENT_TYPE_FACE_DOWN", "1"),
    }
    if object_events != expected_geometry:
        raise SystemExit("FAIL: Battle 99 shared-object geometry drifted")
    route = (ROOT / "data/maps/Route114/scripts.inc").read_text()
    if route.count("trainerbattle_double TRAINER_TYRA_AND_IVY") != 2:
        raise SystemExit("FAIL: Battle 99 does not have two shared guarded invocations")
    for cue in ("Route114_Text_TyraNotEnoughMons", "Route114_Text_IvyNotEnoughMons"):
        if cue not in route:
            raise SystemExit(f"FAIL: Battle 99 missing guard {cue}")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]["TRAINER_TYRA_AND_IVY"]
    expected_manifest = {"format": "double", "target_size": 4, "archetype": "Four-style fighting curriculum", "difficulty": 92, "partner_interaction": True, "level_offset": 3, "location": "Route 114"}
    if manifest != expected_manifest:
        raise SystemExit("FAIL: Battle 99 manifest stale")

    dialogue = (ROOT / "data/text/trainers.inc").read_text().split("Route114_Text_TyraIntro:", 1)[1].split("Route114_Text_KaiIntro:", 1)[0]
    for cue in ("lesson one: Coach", "Throh guards", "Pangoro learns", "Coaching raises", "Wide Guard", "four fighting styles", "Scrappy", "priority punches", "critical leek", "Blitz Boxer"):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 99 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 99 overlong dialogue: {visible}")

    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in corpus_ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 99 competitive reference missing")
    for path, cue in (
        ("docs/battle_set_reviews/060_unova.json", "Throh combines Guts with Storm Throw"),
        ("docs/battle_set_reviews/020_kanto.json", "Hitmonchan uses Blitz Boxer"),
        ("docs/battle_set_reviews/085_galar.json", "Sirfetch'd uses Scrappy"),
    ):
        if cue not in (ROOT / path).read_text():
            raise SystemExit(f"FAIL: Battle 99 handbook evidence missing from {path}")

    protected = "\n".join(
        path.read_text()
        for path in list((ROOT / "docs").glob("emerald_champions_*anchor_designs.json"))
        + list((ROOT / "docs/dossier_packets").glob("*.json"))
    )
    for species in ("Pangoro", "Hitmonchan", "Sirfetch'd"):
        if re.search(rf'"{re.escape(species)}"', protected):
            raise SystemExit(f"FAIL: Battle 99 spends protected anchor species {species}")


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
                raise SystemExit(f"FAIL: Battle 99 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
        entry = next(row for row in guide["entries"] if row["trainerId"] == "TRAINER_TYRA_AND_IVY")
        if entry["designStatus"] != "closed" or entry["format"] != "double" or entry["partySize"] != 4:
            raise SystemExit("FAIL: Battle 99 guide stale")
    print("PASS: Battle 99 Tyra/Ivy four-style fighting curriculum is source-closed")


if __name__ == "__main__":
    main()
