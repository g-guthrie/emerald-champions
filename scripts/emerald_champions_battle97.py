#!/usr/bin/env python3
"""Generate and verify Battle 97, Claude's manual-rain fishing relay."""

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
        "species": "SPECIES_HUNTAIL",
        "item": "ITEM_ASSAULT_VEST",
        "ability_slot": 1,
        "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
        "moves": ["MOVE_WATERFALL", "MOVE_CRUNCH", "MOVE_ICE_FANG", "MOVE_SUCKER_PUNCH"],
    },
    {
        "level": 2,
        "species": "SPECIES_GOREBYSS",
        "item": "ITEM_FOCUS_SASH",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
        "moves": ["MOVE_RAIN_DANCE", "MOVE_MUDDY_WATER", "MOVE_ICE_BEAM", "MOVE_PSYCHIC"],
    },
    {
        "level": 3,
        "species": "SPECIES_BARRASKEWDA",
        "item": "ITEM_CHOICE_BAND",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
        "moves": ["MOVE_LIQUIDATION", "MOVE_CLOSE_COMBAT", "MOVE_CRUNCH", "MOVE_FLIP_TURN"],
    },
]

REFERENCES = [
    "showdown:gen6randomdoublesbattle:023",
    "showdown:gen9randomdoublesbattle:017",
    "smogon:gen8ou:001",
]

NEXT = {
    "index": 98,
    "encounter_id": "BATTLE_098_ROUTE_114_NANCY",
    "location": "Route114",
    "category": "optional rotating mid-route Picnicker single",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_NANCY"],
    "access_note": (
        "Nancy moves and looks around at (19,35) with three-tile sight on the main north-to-south path below Claude. "
        "She is the next physical encounter before the Tyra/Ivy sight pair."
    ),
}


def design() -> dict:
    return {
        "guide_order": 97,
        "trainer_ids": ["TRAINER_CLAUDE"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Optional direct-interaction Fisherman at (19,26), immediately south-west of Kai and Charlotte. Sight range "
            "zero makes this a deliberate compact single before the denser southbound Route 114 sequence."
        ),
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 mature fishing category relay",
            "effective_levels": "41, 42, and 43",
            "eligible_ratio": "3/3",
            "mega_access": True,
            "status": "pass",
            "reason": (
                "Huntail and Gorebyss are the two held-item Clamperl evolutions and are naturally available before cap 40; "
                "Barraskewda evolves from Arrokuda at level 26. No premature form, Mega, or legendary appears."
            ),
        },
        "manual_quality": 10,
        "manual_difficulty": 8.7,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {
                    "reference_id": REFERENCES[0],
                    "decision": "Huntail role selected; full donor rejected",
                    "reason": "The reproducible doubles set validates Huntail's physical Water/Ice identity, but Claude needs immediate Strong Jaw pressure rather than a second setup clock or Baton Pass.",
                },
                {
                    "reference_id": REFERENCES[1],
                    "decision": "Barraskewda immediate attacker selected; full donor rejected",
                    "reason": "The generated set validates fast physical Water/Fighting pressure without importing unrelated doubles support.",
                },
                {
                    "reference_id": REFERENCES[2],
                    "decision": "Choice Band Barraskewda rain closer selected; full donor rejected",
                    "reason": "Published OU rain offense supplies the exact commitment and speed language; Claude uses one visible manual setter instead of protected Pelipper rain architecture.",
                },
            ],
            "decision": (
                "All 1005 references were reviewed. Indexed Huntail and Barraskewda sets plus the complete all-species "
                "Huntail/Gorebyss handbook evidence support every role. The three-fish category handoff is locally authored "
                "for this direct-interaction single and does not spend any protected rain anchor."
            ),
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "White Herb Smash/Baton Pass Huntail becomes an Assault Vest Strong Jaw opener with Waterfall, Crunch, Ice Fang, and Sucker Punch, preventing duplicate setup."},
            {"reference_id": REFERENCES[1], "adaptation": "Life Orb Barraskewda's Water/Fighting coverage validates the immediate closer role; local Choice Band makes its commitment public."},
            {"reference_id": REFERENCES[2], "adaptation": "The published rain breaker supplies Choice Band Liquidation/Close Combat/Crunch/Flip Turn and the central move-lock counterplay."},
            {"source": "docs/battle_set_reviews/040_hoenn.json", "adaptation": "Huntail and Gorebyss source-backed physical/special split is retained; Gorebyss trades a second Shell Smash for the one manual Rain Dance handoff."},
        ],
        "ordering": {
            "intended_lead": ["SPECIES_HUNTAIL"],
            "source_order": [member["species"] for member in TEAM],
            "reason": (
                "Assault Vest Huntail reveals physical jaw pressure without setup. Focus Sash Gorebyss changes the damage "
                "category and gets one fair chance to establish rain. Source-last Choice Band Barraskewda consumes that "
                "finite weather with immediate speed and damage, while native smart switching remains matchup-dependent."
            ),
        },
        "team_intent": (
            "Level-41 Assault Vest Strong Jaw Huntail attacks immediately through Waterfall, Crunch, Ice Fang, or Sucker "
            "Punch. Level-42 Sash Gorebyss is the visible hinge: Rain Dance empowers its Muddy Water and activates source-last "
            "Barraskewda, but it can also attack directly with Ice Beam or Psychic. Level-43 Choice Band Swift Swim "
            "Barraskewda closes through Liquidation, Close Combat, Crunch, or Flip Turn. The player must change from physical "
            "bulk to special pressure and back rather than solve three fish with one defensive axis."
        ),
        "intended_counterplay": (
            "Electric, Grass, Water immunity, Freeze-Dry, screens, and strong neutral special attacks are broad across the "
            "team. Knock Off or special pressure breaks Huntail's Vest plan; priority, multihit, weather overwrite, Taunt, "
            "or immediate focus can deny Gorebyss's Sash-to-rain handoff; Protect, resistance/immunity pivots, burn, Intimidate, "
            "Rocky Helmet, paralysis, Trick Room, and scouting exploit Barraskewda's Choice lock. Rain can also empower the "
            "player, and nothing requires one catch or exact turn sequence."
        ),
        "bespoke_ai": (
            "Claude remains a native smart single with HP, Speed Control, and Field Control scoring. Huntail attacks or "
            "switches by visible matchup; Gorebyss uses Rain Dance only when weather and board value justify it rather than on "
            "a forced turn; Barraskewda chooses its strongest legal Choice move and can Flip Turn through the ordinary switch "
            "selector. Sash, rain, Swift Swim, Strong Jaw, priority, Choice lock, and accuracy remain public native mechanics."
        ),
        "uniqueness": (
            "Huntail, Gorebyss, and Barraskewda are all new to the first 96 encounters and absent from protected marquee "
            "anchors. The physical/special/physical fishing relay is a concise single after a six-body Trick Room branch. It "
            "uses one manual weather handoff, no sleep, trap, stat setup, hazards, screens, Mega, or legendary, and preserves "
            "Pelipper, Wailord, Araquanid, Gyarados, and every reserved rain architecture."
        ),
        "story_logic": (
            "Claude's fishing boast now names the exact three-part handoff. His defeat acknowledges the broken relay, and the "
            "post-battle text openly teaches Sash Rain Dance and Choice Band commitment. He remains a sight-zero optional "
            "Fisherman with no reward, callback, story flag, or rematch."
        ),
        "reward_logic": "EXP and prize money only; Claude owns no item, shop, legendary, Mega Stone, rematch, or progression reward.",
        "campaign_reservations": {
            "spends": ["Huntail/Gorebyss category split", "manual Sash rain handoff", "first Barraskewda Choice closer"],
            "preserves": ["all protected rain setters and rain bosses", "Mega Gyarados", "Wailord faction role", "Araquanid Juan role", "weather-plus-Tailwind and weather-plus-Trick-Room teams"],
            "repeat_rule": "These three species should not recur soon; later rain must use a materially different setter, board format, and payoff."
        },
        "author_self_check": {
            "strongest_part": "The relay changes damage category and tempo in three readable steps, and the single manual weather turn gives the player a fair hinge to attack.",
            "weakest_link": "All three are mono-Water and share Electric/Grass/Water-immunity pressure. Sash, Vest, Ice coverage, priority, rain, Choice Band, and +1/+2/+3 levels make that broad answer necessary but deliberately remain the fairness seam."
        },
        "closure": (
            "Battle 97 is source-closed at quality 10 and target difficulty 8.7: three fresh, unreserved, legal mature fish "
            "appear at levels 41-43 with distinct items, categories, and tempo; exact source ordering, AI, direct-interaction "
            "geometry, three indexed competitive references, handbook evidence, native-width dialogue, broad weather/Choice/"
            "type counterplay, and zero reward debt are proven. Runtime playtesting remains required before difficulty is observed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 97,
        "encounter_id": "BATTLE_097_ROUTE_114_CLAUDE",
        "identity": {
            "location": "Route114",
            "category": "optional mid-route direct-interaction Fisherman single",
            "format": "single",
            "strict_cap": 40,
            "memory_hook": "Strong Jaw Huntail opens, Sash Gorebyss calls one manual rain, and Choice Band Barraskewda cashes the handoff."
        },
        "primary_player_question": "Can the player deny Gorebyss's manual rain handoff while changing defensive category twice and preserving a Choice-lock answer for Barraskewda?",
        "tempo": "Three-fish singles relay: bulky physical jaw opener, special Sash weather hinge, then very fast Choice-locked physical closer.",
        "pressure_sources": [
            "level-41 Assault Vest Strong Jaw Huntail with Sucker Punch",
            "level-42 Focus Sash Gorebyss with Rain Dance/Muddy Water",
            "level-43 Choice Band Swift Swim Barraskewda with Liquidation/Close Combat/Flip Turn"
        ],
        "intentional_opening": "Huntail is fixed first, Gorebyss is the middle weather hinge, and Barraskewda is source-last; native singles switching can still react to visible matchups.",
        "intentional_weakness": "Three mono-Water bodies, one fragile weather setter, rain that can help the player, public Choice lock, no recovery, and broad Electric/Grass/Freeze-Dry/Water-immunity pressure.",
        "first_loss_lesson": "Gorebyss, not the closer, was the hinge. Break or overwrite its rain, then scout Barraskewda's Choice move instead of racing boosted Liquidation.",
        "revealed_information": ["cap 40", "direct-interaction single", "levels 41-43", "physical/special/physical relay", "Strong Jaw", "Focus Sash Rain Dance", "Swift Swim", "Choice Band and Flip Turn", "three fresh species", "no reward/rematch"],
        "counterplay_classes": ["Electric/Grass/Freeze-Dry", "Water immunity and special bulk", "item removal and priority", "weather overwrite/Taunt/multihit", "burn/Intimidate/Rocky Helmet", "Protect and Choice scouting", "paralysis/Trick Room"],
        "target_difficulty": 8.7,
        "difficulty_rationale": "Three optimized fresh levels 41-43, distinct categories/items, one Sash weather hinge, and a Choice Band Swift Swim closer create a serious optional single. Common typed seams and one exposed setter keep it below the surrounding doubles.",
        "tuning_knob": "Tune Barraskewda +3 to +2 first, then Huntail +1 to cap; preserve species, order, items, categories, and manual-rain handoff.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["route-single", "fisherman", "manual-rain", "category-relay", "huntail", "gorebyss", "barraskewda", "strong-jaw", "focus-sash", "swift-swim", "choice-band", "flip-turn", "three-fresh-species", "no-sleep", "no-trap", "no-stat-setup", "no-mega", "no-legendary"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Reproducible Huntail, reproducible Barraskewda, published OU Barraskewda, and complete all-species Huntail/Gorebyss reviews."},
        "author_self_check": {"strongest_part": "A readable three-step handoff turns three obscure fish into a coherent competitive single.", "weakest_link": "Mono-Water compression is real and intentional; item/tempo/coverage pressure prevents it from becoming filler."},
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_097_ROUTE_114_CLAUDE"] = design()

    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 97] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])

    sequence = json.loads(SEQUENCE.read_text())
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 98] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        if row["index"] <= 97:
            row["status"] = "closed"
        elif row["index"] == 98:
            row["status"] = "next"
        else:
            row["status"] = "queued"

    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update(
        {
            "closed_encounters": 97,
            "next_index": 98,
            "next_encounter_id": NEXT["encounter_id"],
            "queued_sequence_entries": 0,
            "canonical_sequence_groups": 98,
            "physical_encounter_groups": 527,
            "unordered_physical_groups": 429,
        }
    )
    return designs, ledger, sequence, operating_system


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    block_text = doubles.trainer_blocks(trainers)["TRAINER_CLAUDE"].group(0)
    body = doubles.party_match(parties, doubles.party_name(block_text)).group(2)
    actual = [polish.parse_entry(entry) for entry in custom.party_entries(body)]
    if actual != TEAM:
        raise SystemExit("FAIL: Battle 97 Claude source party differs")
    for token in (".doubleBattle = FALSE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL"):
        if token not in block_text:
            raise SystemExit(f"FAIL: Battle 97 Claude missing {token}")

    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for member in TEAM:
        illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
        if illegal:
            raise SystemExit(f"FAIL: Battle 97 illegal moves for {member['species']}: {illegal}")
        if member["ability_slot"] >= len(slots[member["species"]]):
            raise SystemExit(f"FAIL: Battle 97 invalid ability slot for {member['species']}")
    if len({member["species"] for member in TEAM}) != 3 or len({member["item"] for member in TEAM}) != 3:
        raise SystemExit("FAIL: Battle 97 species/items are not unique")

    object_event = next(
        row for row in json.loads((ROOT / "data/maps/Route114/map.json").read_text())["object_events"]
        if row.get("script") == "Route114_EventScript_Claude"
    )
    if (object_event["x"], object_event["y"], object_event["movement_type"], str(object_event["trainer_sight_or_berry_tree_id"])) != (19, 26, "MOVEMENT_TYPE_FACE_LEFT", "0"):
        raise SystemExit("FAIL: Battle 97 direct-interaction geometry drifted")
    if "trainerbattle_single TRAINER_CLAUDE" not in (ROOT / "data/maps/Route114/scripts.inc").read_text():
        raise SystemExit("FAIL: Battle 97 Claude is not a single")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]["TRAINER_CLAUDE"]
    expected_manifest = {"format": "single", "target_size": 3, "archetype": "Manual-rain category relay", "difficulty": 87, "partner_interaction": False, "level_offset": 2, "location": "Route 114"}
    if manifest != expected_manifest:
        raise SystemExit("FAIL: Battle 97 manifest stale")

    dialogue = (ROOT / "data/text/trainers.inc").read_text().split("Route114_Text_ClaudeIntro:", 1)[1].split("Route114_Text_NolanIntro:", 1)[0]
    for cue in ("fishing were battling", "Huntail bites", "Gorebyss calls rain", "Barraskewda ends", "perfect handoff", "hinge", "Sash", "Choice Band", "one move"):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 97 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 97 overlong dialogue: {visible}")

    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in corpus_ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 97 competitive reference missing")
    review = (ROOT / "docs/battle_set_reviews/040_hoenn.json").read_text()
    for cue in ("Huntail uses Water Veil with Shell Smash", "Gorebyss uses Swift Swim with Muddy Water"):
        if cue not in review:
            raise SystemExit(f"FAIL: Battle 97 handbook evidence missing {cue}")

    protected = "\n".join(
        path.read_text()
        for path in list((ROOT / "docs").glob("emerald_champions_*anchor_designs.json"))
        + list((ROOT / "docs/dossier_packets").glob("*.json"))
    )
    for species in ("Huntail", "Gorebyss", "Barraskewda"):
        if re.search(rf'"{species}"', protected):
            raise SystemExit(f"FAIL: Battle 97 spends protected anchor species {species}")


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
                raise SystemExit(f"FAIL: Battle 97 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
        entry = next(row for row in guide["entries"] if row["trainerId"] == "TRAINER_CLAUDE")
        if entry["designStatus"] != "closed" or entry["format"] != "single" or entry["partySize"] != 3:
            raise SystemExit("FAIL: Battle 97 guide stale")
    print("PASS: Battle 97 Claude manual-rain category relay is source-closed")


if __name__ == "__main__":
    main()
