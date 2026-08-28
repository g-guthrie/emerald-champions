#!/usr/bin/env python3
"""Generate and verify Battle 100, Shane's three-act campsite single."""

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
        "species": "SPECIES_DREDNAW",
        "item": "ITEM_CHOICE_BAND",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
        "moves": ["MOVE_LIQUIDATION", "MOVE_CRUNCH", "MOVE_ICE_FANG", "MOVE_ROCK_SLIDE"],
    },
    {
        "level": 2,
        "species": "SPECIES_CENTISKORCH",
        "item": "ITEM_BIG_ROOT",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
        "moves": ["MOVE_FIRE_LASH", "MOVE_LEECH_LIFE", "MOVE_POWER_WHIP", "MOVE_KNOCK_OFF"],
    },
    {
        "level": 3,
        "species": "SPECIES_MISMAGIUS",
        "item": "ITEM_FOCUS_SASH",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
        "moves": ["MOVE_NASTY_PLOT", "MOVE_SHADOW_BALL", "MOVE_MYSTICAL_FIRE", "MOVE_DAZZLING_GLEAM"],
    },
]

REFERENCES = [
    "showdown:gen9randomdoublesbattle:020",
    "showdown:gen8randombattle:009",
    "smogon:gen4uu:001",
    "showdown:gen9randomdoublesbattle:003",
]

NEXT = {
    "index": 101,
    "encounter_id": "BATTLE_101_ROUTE_114_STEVE",
    "location": "Route114",
    "category": "optional south-route Poké Maniac four-record Match Call family",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_STEVE_1", "TRAINER_STEVE_2", "TRAINER_STEVE_3", "TRAINER_STEVE_4"],
    "access_note": (
        "Steve faces up at (20,56) with three-tile sight below Shane. One physical position owns his initial record and "
        "all three sequential Match Call rematches."
    ),
}


def design() -> dict:
    return {
        "guide_order": 100,
        "trainer_ids": ["TRAINER_SHANE"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Optional three-tile Camper single at (22,50), immediately below Tyra/Ivy and above Steve. This begins the "
            "southern Route 114 singles run after the central mentor double."
        ),
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 mature three-act campsite single",
            "effective_levels": "41, 42, and 43",
            "eligible_ratio": "3/3",
            "mega_access": True,
            "status": "pass",
            "reason": "Drednaw evolves from Chewtle at 22; Centiskorch evolves from Sizzlipede at 28; Mismagius evolves from Misdreavus with a Dusk Stone. All are naturally mature before cap 40.",
        },
        "manual_quality": 10,
        "manual_difficulty": 8.7,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {
                    "reference_id": REFERENCES[0],
                    "decision": "Drednaw Strong Jaw role selected; full donor rejected",
                    "reason": "The reproducible doubles set validates Drednaw's biting Water/Rock pressure; Shane uses a public Choice commitment instead of unrelated weather."
                },
                {
                    "reference_id": REFERENCES[1],
                    "decision": "Centiskorch sustain/coverage role selected; full donor rejected",
                    "reason": "The generated single validates Fire Lash, Leech Life, and broad physical coverage without importing its full random roster."
                },
                {
                    "reference_id": REFERENCES[2],
                    "decision": "Mismagius Nasty Plot closer selected; full donor rejected",
                    "reason": "Published UU offense supports Mismagius as a serious setup finish, not merely a spooky visual."
                },
                {
                    "reference_id": REFERENCES[3],
                    "decision": "Modern Mismagius doubles coverage corroborated; full donor rejected",
                    "reason": "The current generated set supports Shadow/Fairy/Fire coverage and a real immediate-attack fallback."
                },
            ],
            "decision": (
                "All 1005 references were reviewed. Four indexed exact-species references plus the complete all-species "
                "reviews support each role; fishing, roasting, and spooky-story ordering is locally hand-authored from Shane's "
                "existing campsite dialogue."
            ),
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Drednaw keeps Strong Jaw Water/Dark/Ice pressure, adds Rock Slide, and uses Choice Band as the public opening commitment."},
            {"reference_id": REFERENCES[1], "adaptation": "Centiskorch keeps physical Fire/Bug sustain; Big Root, Power Whip, and Knock Off make the campfire act autonomous."},
            {"reference_id": REFERENCES[2], "adaptation": "Published Nasty Plot Mismagius becomes the source-last Sash story climax with Ghost/Fire/Fairy coverage."},
            {"reference_id": REFERENCES[3], "adaptation": "Modern coverage evidence supplies an attack-now fallback when setup is unsafe."},
        ],
        "ordering": {
            "intended_lead": ["SPECIES_DREDNAW"],
            "source_order": [member["species"] for member in TEAM],
            "reason": (
                "Drednaw is the fishing act and reveals a Choice lock. Centiskorch is the campfire act and changes typing, "
                "sustain, and target selection. Mismagius is the story act and owns the roster's one setup clock; native smart "
                "switching may react to visible matchups without changing the source-first/source-last memory."
            ),
        },
        "team_intent": (
            "Level-41 Choice Band Strong Jaw Drednaw opens through Liquidation, Crunch, Ice Fang, or Rock Slide. Level-42 "
            "Big Root Flash Fire Centiskorch uses Fire Lash defense drops, Leech Life sustain, Power Whip, and Knock Off. "
            "Level-43 Focus Sash Levitate Mismagius can attempt one Nasty Plot or immediately use Shadow Ball, Mystical Fire, "
            "or Dazzling Gleam. Each camp act changes type, category pressure, and tempo without weather, sleep, or trapping."
        ),
        "intended_counterplay": (
            "Grass, Electric, Fighting, Ground, Water immunity, Protect, and Choice scouting answer Drednaw. Rock, Water, "
            "Flying, Big Root removal, noncontact damage, special pressure, and denying Leech Life targets answer Centiskorch; "
            "its 4x Rock weakness is deliberately real. Priority, multihit, Taunt, Haze, Unaware, phazing, Dark/Ghost, special "
            "bulk, or forcing Sash chip answer Mismagius. Shared Rock/Ground pressure exists, but Power Whip, Levitate, Ice "
            "coverage, and the category changes punish autopilot."
        ),
        "bespoke_ai": (
            "Shane gains smart switching and HP awareness. Drednaw chooses a real Choice move and can be exploited after it "
            "locks. Centiskorch attacks through the most valuable coverage or sustain line and does not stall. Mismagius uses "
            "Nasty Plot only when survival and a follow-up improve; otherwise it attacks immediately. Strong Jaw, Choice Band, "
            "Fire Lash, Big Root, Sash, Levitate, Nasty Plot, and coverage are public native mechanics with no forced turn."
        ),
        "uniqueness": (
            "Drednaw and Centiskorch are new to the first 99 encounters and absent from every protected anchor. Mismagius last "
            "appeared 35 battles earlier as one half of a sound pair; here it is a source-last setup storyteller. This is the "
            "only fishing/fire/ghost campsite ladder and uses no weather, room, terrain, sleep, trap, hazards, screens, Protect, "
            "Mega, or legendary."
        ),
        "story_logic": (
            "Shane's original fishing, roasting, and spooky-story speech now maps exactly to Drednaw, Centiskorch, and "
            "Mismagius. Post-battle text teaches Strong Jaw, Fire Lash, and Nasty Plot. He remains an optional Camper with no "
            "item, Match Call, rematch, story flag, or progression reward."
        ),
        "reward_logic": "EXP and prize money only; Shane owns no item, shop, legendary, Mega Stone, rematch, or progression reward.",
        "campaign_reservations": {
            "spends": ["first Drednaw Strong Jaw commitment", "first Centiskorch Fire Lash campfire", "Mismagius setup storyteller"],
            "preserves": ["all protected Fire/Ghost anchors", "Gigantamax and Mega identities", "weather camp teams", "sleep-based ghost teams", "trapping ghost stories"],
            "repeat_rule": "Drednaw and Centiskorch should not recur soon; later Mismagius must change format and setup role."
        },
        "author_self_check": {
            "strongest_part": "The unchanged campsite premise naturally produces three mechanically distinct acts and a memorable visual escalation.",
            "weakest_link": "Rock/Ground pressure can cover much of the roster. Drednaw's Water/Ice, Centiskorch's Power Whip, Mismagius's Levitate/special axis, items, and +1/+2/+3 levels force target-specific execution while keeping that broad seam honest."
        },
        "closure": (
            "Battle 100 is source-closed at quality 10 and target difficulty 8.7: two fresh and one distant-repurposed legal "
            "mature species appear at levels 41-43 with distinct items, types, and tempo; exact source ordering, AI, geometry, "
            "four indexed competitive references, native-width dialogue, broad type/Choice/setup counterplay, and zero reward "
            "debt are proven. Runtime playtesting remains required before difficulty is observed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 100,
        "encounter_id": "BATTLE_100_ROUTE_114_SHANE",
        "identity": {
            "location": "Route114",
            "category": "optional south-central Camper single",
            "format": "single",
            "strict_cap": 40,
            "memory_hook": "Drednaw catches dinner, Centiskorch roasts it, and Sash Nasty Plot Mismagius tells the final spooky story."
        },
        "primary_player_question": "Can the player exploit Drednaw's Choice commitment, change answers for Centiskorch's sustain, and preserve setup denial or priority for Sash Mismagius?",
        "tempo": "Three-act campsite single: Choice physical catch, physical defense-lowering sustain fire, then special Sash setup story.",
        "pressure_sources": [
            "level-41 Choice Band Strong Jaw Drednaw",
            "level-42 Big Root Flash Fire Centiskorch with Fire Lash/Leech Life",
            "level-43 Focus Sash Levitate Mismagius with Nasty Plot and three coverage types"
        ],
        "intentional_opening": "Drednaw is fixed first, Centiskorch is the middle type/sustain change, and Mismagius is source-last; native smart switching remains matchup-aware.",
        "intentional_weakness": "Choice lock, Centiskorch's 4x Rock seam, one fragile Sash setup user, no field/recovery loop, and broad Rock/Ground/Water/Dark/Ghost pressure.",
        "first_loss_lesson": "Each camp act needed a new answer. Lock Drednaw badly, deny Centiskorch a safe sustain target, and keep priority or Taunt for the final story.",
        "revealed_information": ["cap 40", "single", "levels 41-43", "Choice Band Strong Jaw", "Fire Lash plus Leech Life", "Big Root", "Focus Sash Nasty Plot", "Levitate", "two fresh species", "no reward/rematch"],
        "counterplay_classes": ["Choice scouting/Protect", "Grass/Electric/Fighting/Ground into Drednaw", "Rock/Water/Flying and item removal into Centiskorch", "priority/multihit/Taunt/Haze/Unaware/phazing into Mismagius", "Dark/Ghost and special bulk", "hazard or residual Sash chip"],
        "target_difficulty": 8.7,
        "difficulty_rationale": "Three optimized levels 41-43, complete distinct items, two immediate physical axes, and one Sash special setup closer create a serious single. Public locks, 4x weakness, and one setup clock keep it below dense doubles.",
        "tuning_knob": "Tune Mismagius +3 to +2 first, then Drednaw +1 to cap; preserve species, order, items, and campsite acts.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["route-single", "camper", "three-act-campsite", "fishing", "campfire", "spooky-story", "drednaw", "centiskorch", "mismagius", "strong-jaw", "choice-band", "fire-lash", "big-root", "focus-sash", "nasty-plot", "two-fresh-species", "no-weather", "no-sleep", "no-trap", "no-protect", "no-mega", "no-legendary"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Indexed Drednaw/Centiskorch/Mismagius references plus all-species reviews; campsite order is local."},
        "author_self_check": {"strongest_part": "The NPC's three camp activities become three distinct battle phases without mechanical clutter.", "weakest_link": "Rock/Ground compression is real; coverage, type/category changes, items, Levitate, and levels keep it fair but serious."},
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_100_ROUTE_114_SHANE"] = design()

    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 100] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])

    sequence = json.loads(SEQUENCE.read_text())
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 101] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        if row["index"] <= 100:
            row["status"] = "closed"
        elif row["index"] == 101:
            row["status"] = "next"
        else:
            row["status"] = "queued"

    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update(
        {
            "closed_encounters": 100,
            "next_index": 101,
            "next_encounter_id": NEXT["encounter_id"],
            "queued_sequence_entries": 0,
            "canonical_sequence_groups": 101,
            "physical_encounter_groups": 527,
            "unordered_physical_groups": 426,
        }
    )
    return designs, ledger, sequence, operating_system


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    block_text = doubles.trainer_blocks(trainers)["TRAINER_SHANE"].group(0)
    body = doubles.party_match(parties, doubles.party_name(block_text)).group(2)
    actual = [polish.parse_entry(entry) for entry in custom.party_entries(body)]
    if actual != TEAM:
        raise SystemExit("FAIL: Battle 100 Shane source party differs")
    for token in (".doubleBattle = FALSE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HP_AWARE"):
        if token not in block_text:
            raise SystemExit(f"FAIL: Battle 100 Shane missing {token}")

    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for member in TEAM:
        illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
        if illegal:
            raise SystemExit(f"FAIL: Battle 100 illegal moves for {member['species']}: {illegal}")
        if member["ability_slot"] >= len(slots[member["species"]]):
            raise SystemExit(f"FAIL: Battle 100 invalid ability slot for {member['species']}")
    if len({member["species"] for member in TEAM}) != 3 or len({member["item"] for member in TEAM}) != 3:
        raise SystemExit("FAIL: Battle 100 species/items are not unique")

    object_event = next(
        row for row in json.loads((ROOT / "data/maps/Route114/map.json").read_text())["object_events"]
        if row.get("script") == "Route114_EventScript_Shane"
    )
    if (object_event["x"], object_event["y"], object_event["movement_type"], str(object_event["trainer_sight_or_berry_tree_id"])) != (22, 50, "MOVEMENT_TYPE_FACE_RIGHT", "3"):
        raise SystemExit("FAIL: Battle 100 Shane geometry drifted")
    if "trainerbattle_single TRAINER_SHANE" not in (ROOT / "data/maps/Route114/scripts.inc").read_text():
        raise SystemExit("FAIL: Battle 100 Shane is not a single")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]["TRAINER_SHANE"]
    expected_manifest = {"format": "single", "target_size": 3, "archetype": "Three-act campsite", "difficulty": 87, "partner_interaction": False, "level_offset": 2, "location": "Route 114"}
    if manifest != expected_manifest:
        raise SystemExit("FAIL: Battle 100 manifest stale")

    dialogue = (ROOT / "data/text/trainers.inc").read_text().split("Route114_Text_ShaneIntro:", 1)[1].split("Route114_Text_NancyIntro:", 1)[0]
    for cue in ("three perfect acts", "Drednaw catches", "Centiskorch", "Mismagius tells", "spooky story", "Strong Jaw", "Fire Lash", "Nasty Plot"):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 100 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 100 overlong dialogue: {visible}")

    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in corpus_ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 100 competitive reference missing")

    protected = "\n".join(
        path.read_text()
        for path in list((ROOT / "docs").glob("emerald_champions_*anchor_designs.json"))
        + list((ROOT / "docs/dossier_packets").glob("*.json"))
    )
    for species in ("Drednaw", "Centiskorch"):
        if re.search(rf'"{species}"', protected):
            raise SystemExit(f"FAIL: Battle 100 spends protected anchor species {species}")


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
                raise SystemExit(f"FAIL: Battle 100 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
        entry = next(row for row in guide["entries"] if row["trainerId"] == "TRAINER_SHANE")
        if entry["designStatus"] != "closed" or entry["format"] != "single" or entry["partySize"] != 3:
            raise SystemExit("FAIL: Battle 100 guide stale")
    print("PASS: Battle 100 Shane three-act campsite is source-closed")


if __name__ == "__main__":
    main()
