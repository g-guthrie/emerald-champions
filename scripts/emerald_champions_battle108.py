#!/usr/bin/env python3
"""Generate and verify Battle 108, Mt. Chimney's opposing-sight Magma Grunt pair."""

from __future__ import annotations

import argparse
import collections
import json
import re
import struct
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

GRUNT1_TEAM = [
    {"level": 2, "species": "SPECIES_STONJOURNER", "item": "ITEM_CHOPLE_BERRY", "ability_slot": 0, "spread": "SPREAD_31_IV_HP_DEF_IMPISH", "moves": ["MOVE_STONE_EDGE", "MOVE_BODY_PRESS", "MOVE_HEAVY_SLAM", "MOVE_WIDE_GUARD"]},
    {"level": 3, "species": "SPECIES_BLACEPHALON", "item": "ITEM_WISE_GLASSES", "ability_slot": 0, "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID", "moves": ["MOVE_HEAT_WAVE", "MOVE_SHADOW_BALL", "MOVE_PSYCHIC", "MOVE_FLAMETHROWER"]},
    {"level": 4, "species": "SPECIES_PALOSSAND", "item": "ITEM_LEFTOVERS", "ability_slot": 1, "spread": "SPREAD_31_IV_HP_SPATK_MODEST", "moves": ["MOVE_EARTH_POWER", "MOVE_SHADOW_BALL", "MOVE_SHORE_UP", "MOVE_GIGA_DRAIN"]},
]

GRUNT2_TEAM = [
    {"level": 3, "species": "SPECIES_GOLURK", "item": "ITEM_COLBUR_BERRY", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_DYNAMIC_PUNCH", "MOVE_HIGH_HORSEPOWER", "MOVE_POLTERGEIST", "MOVE_ICE_PUNCH"]},
    {"level": 2, "species": "SPECIES_SANDACONDA", "item": "ITEM_SITRUS_BERRY", "ability_slot": 0, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_HIGH_HORSEPOWER", "MOVE_GLARE", "MOVE_COIL", "MOVE_BODY_PRESS"]},
    {"level": 4, "species": "SPECIES_KROOKODILE", "item": "ITEM_ROSELI_BERRY", "ability_slot": 0, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_HIGH_HORSEPOWER", "MOVE_KNOCK_OFF", "MOVE_TAUNT", "MOVE_STONE_EDGE"]},
]

REFERENCES = [
    "showdown:gen8randomdoublesbattle:015",
    "showdown:gen7randomdoublesbattle:011",
    "showdown:gen7randomdoublesbattle:022",
    "showdown:gen8randomdoublesbattle:008",
    "showdown:gen8randombattle:008",
    "showdown:gen9championsrandomdoublesbattle:016",
]

NEXT = {
    "index": 109,
    "encounter_id": "BATTLE_109_MT_CHIMNEY_TABITHA",
    "location": "MtChimney",
    "category": "required Team Magma Admin summit approach",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_TABITHA_MT_CHIMNEY"],
    "access_note": "Tabitha waits at (12,11) after the opposing-sight Grunt corridor and before Maxie at (13,6). His protected faction-admin anchor is the next required physical battle.",
}


def design() -> dict:
    return {
        "guide_order": 108,
        "trainer_ids": ["TRAINER_GRUNT_MT_CHIMNEY_1", "TRAINER_GRUNT_MT_CHIMNEY_2"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "First active story-trainer corridor after entering Mt. Chimney from the southern cable-car station. Ordinary "
            "post-crisis trainers remain hidden. Two Magma Grunts face each other across the walkable ascent and can join as a "
            "six-member native-pair double or be approached as two complete three-member singles."
        ),
        "runtime_branches": [
            "Joint native-pair double: Stonjourner and Golurk lead, all six source members available.",
            "Female Grunt split single: Stonjourner, Blacephalon, Palossand.",
            "Male Grunt split single: Golurk, Sandaconda, Krookodile.",
        ],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 mature faction-ascent terrain crew",
            "effective_levels": "female 42/43/44; male 43/42/44",
            "eligible_ratio": "6/6",
            "mega_access": True,
            "status": "pass",
            "reason": "Stonjourner and Blacephalon are single-stage; Palossand evolves at 42 and appears at 44; Golurk evolves at 43 and appears at 43; Sandaconda evolves at 36 and Krookodile at 40.",
        },
        "manual_quality": 10,
        "manual_difficulty": 9.4,
        "branch_difficulty": {"joint_double": 9.4, "female_single": 8.8, "male_single": 8.9},
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {"reference_id": REFERENCES[0], "decision": "Stonjourner role selected", "reason": "The exact doubles set validates Power Spot, Body Press, Stone Edge, and Protect; Wide Guard/Heavy Slam replace setup and Protect for a less repetitive lead."},
                {"reference_id": REFERENCES[1], "decision": "Blacephalon role adapted", "reason": "The exact doubles set validates Beast Boost Fire/Ghost offense; Wise Glasses and four attacks avoid another Life Orb/Sash module."},
                {"reference_id": REFERENCES[2], "decision": "Palossand role adapted", "reason": "The exact doubles set validates Shore Up, Earth Power, and Shadow Ball; local Sand Stream/Giga Drain make the foundation autonomous without Toxic or Protect."},
                {"reference_id": REFERENCES[3], "decision": "Golurk role selected", "reason": "The exact doubles set validates No Guard Dynamic Punch and Ground/Ghost pressure; Colbur and Ice coverage remove Sitrus/Protect repetition."},
                {"reference_id": REFERENCES[4], "decision": "Sandaconda role adapted", "reason": "Generated Glare/ground pressure validates the compactor; Sand Spit, Coil, and Body Press make contact change the board."},
                {"reference_id": REFERENCES[5], "decision": "Krookodile role adapted", "reason": "The Champions generator validates Intimidate, High Horsepower, and Knock Off; Roseli/Taunt/Stone Edge avoid Choice and spread repetition."},
            ],
            "decision": "All 1005 references were available. Six exact-species records support the roles; map-derived opposing sight lines and the land-construction story are locally authored.",
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Power Spot Stonjourner is the joint lead and carries Wide Guard rather than another Protect-heavy setup shell."},
            {"reference_id": REFERENCES[1], "adaptation": "Wise Glasses Blacephalon is the rare kiln with four direct special attacks and Beast Boost."},
            {"reference_id": REFERENCES[2], "adaptation": "Sand Stream Palossand supplies the foundation and recovery without passive Toxic or Protect."},
            {"reference_id": REFERENCES[3], "adaptation": "Colbur No Guard Golurk is the joint breaker with guaranteed Dynamic Punch and three coverage attacks."},
            {"reference_id": REFERENCES[4], "adaptation": "Sand Spit Sandaconda uses Glare, Coil, and Body Press to turn contact into a compaction clock."},
            {"reference_id": REFERENCES[5], "adaptation": "Roseli Intimidate Krookodile closes through Ground, Dark, Taunt, and Rock coverage without Choice lock."},
        ],
        "ordering": {
            "joint_lead": ["SPECIES_STONJOURNER", "SPECIES_GOLURK"],
            "female_source_order": [member["species"] for member in GRUNT1_TEAM],
            "male_source_order": [member["species"] for member in GRUNT2_TEAM],
            "reason": "Power Spot immediately boosts No Guard Golurk in the joint branch. Each split keeps a complete construction sequence, while later sand supports every Rock/Ground/Ghost reserve except the intentionally urgent Blacephalon.",
        },
        "team_intent": (
            "The joint lead is Power Spot Stonjourner beside No Guard Golurk: one supports every attack while the other makes "
            "Dynamic Punch reliable. Blacephalon supplies rare special Fire/Ghost escalation; Palossand creates sand and recovery; "
            "Sandaconda answers contact with sand, then Glare/Coil/Body Press; Krookodile adds Intimidate, item removal, Taunt, and "
            "direct coverage. No member uses Protect, speed field, priority offense, sleep, trap, Mega, sun, or a protected faction mechanic."
        ),
        "intended_counterplay": (
            "Water, Grass, Ice, Fairy, Fighting, Ghost, Dark, special bulk, burn, Intimidate, Taunt, Haze, phazing, Wide Guard, "
            "weather replacement, item removal, and focused damage are broad. Remove Stonjourner to erase Power Spot; exploit "
            "Golurk's Dark/Ghost seams around one Colbur; deny Blacephalon a Beast Boost; change weather or pressure Palossand; "
            "special-attack Sandaconda before Coil/Body Press compounds; and use Water/Grass/Fighting/Fairy into Krookodile."
        ),
        "bespoke_ai": (
            "Both source records remain native singles for joint/split safety and use smart switching, partner awareness, HP "
            "awareness, Field Control, and Combo Setup. Power Spot, No Guard, Beast Boost, Sand Stream, Sand Spit, Intimidate, "
            "Wide Guard, Glare, Coil, Taunt, Shore Up, and item effects resolve through native visible-state scoring. No partner "
            "activation, target, support turn, or switch is forced."
        ),
        "uniqueness": (
            "Blacephalon, Palossand, Sandaconda, and Krookodile are new to the first 107 encounters. Stonjourner returns 96 "
            "battles after its juvenile Power Spot lesson and Golurk 26 battles after a different route role. The inherited Numel/"
            "Dugtrio/Marowak/Graveler sand filler is removed. This is the only opposing-sight land-construction crew."
        ),
        "story_logic": (
            "The female Grunt's dream of a lava house becomes foundation, kiln, and Palossand. The male Grunt's promise of land "
            "for everyone becomes digging, compaction, and guarding. Post-battle text names actual abilities. Story visibility, "
            "trainer flags, the corridor, and the progression toward Tabitha/Maxie remain unchanged."
        ),
        "reward_logic": "Required story progress, EXP, and prize money only; neither Grunt grants an item, registration, or extra progression flag.",
        "campaign_reservations": {
            "spends": ["Mt. Chimney opposing-sight land crew", "Power Spot/No Guard joint lead", "first opponent Blacephalon", "first Palossand", "first Sandaconda", "first Krookodile"],
            "preserves": ["Tabitha's protected machinery", "Maxie's Groudon/Crobat ridge control", "Flannery's Torkoal and thermal timing", "Coalossal/Excadrill/Camerupt/Entei", "all Fire and Ground Megas"],
            "repeat_rule": "These six species should not recur soon; later Magma teams must progress from laborers to machinery and command rather than repeat construction.",
        },
        "author_self_check": {
            "strongest_part": "The map geometry, Grunt motives, exact lead abilities, and reserve roles all describe one Magma construction crew while both split singles remain coherent.",
            "weakest_link": "The male split is entirely physical and the joint exposes broad Water/Grass/Dark pressure. Sand/status, Power Spot, rare special Blacephalon, Palossand sustain, Intimidate, items, and +2 to +4 levels create a serious fight without hiding those answers."
        },
        "closure": (
            "Battle 108 is source-closed at quality 10: target 9.4 joint and 8.8/8.9 splits; map-derived path and opposing-sight "
            "proof; six legal levels 42-44; six distinct items; four fresh and two distant role-changed species; six indexed "
            "references; no Protect; branch-truthful native-width dialogue; broad counterplay; protected-anchor separation; and zero reward debt. Runtime remains unplayed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 108,
        "encounter_id": "BATTLE_108_MT_CHIMNEY_GRUNT_PAIR",
        "identity": {"location": "MtChimney", "category": "required opposing-sight Magma Grunt corridor", "format": "native-pair double or two split singles", "strict_cap": 40, "memory_hook": "Power Spot Stonjourner and No Guard Golurk open a construction crew of kiln, foundation, compactor, and guard."},
        "primary_player_question": "Can the player dismantle Power Spot plus No Guard before the construction reserves establish sand, setup, Beast Boost, and Intimidate pressure?",
        "tempo": "Six-member joint construction double or two autonomous three-member singles: boosted breaker lead, rare kiln/compactor middles, then sand foundation and Intimidate guard.",
        "pressure_sources": ["Power Spot Stonjourner Wide Guard", "No Guard Golurk Dynamic Punch", "Wise Glasses Beast Boost Blacephalon", "Sand Stream Palossand Shore Up", "Sand Spit Sandaconda Coil/Body Press", "Intimidate Krookodile Knock Off/Taunt"],
        "intentional_opening": "Joint opens Stonjourner+Golurk; splits preserve each source-first member. No action is scripted and no member has Protect.",
        "intentional_weakness": "Broad Water/Grass/Dark/Ghost/Fighting/Fairy pressure, all-physical male split, weather dependence after reserves, Blacephalon urgency, no speed field/priority/Protect/Mega/sleep/trap.",
        "first_loss_lesson": "Remove the foreman, not every worker evenly: erase Power Spot or Golurk first, then stop the weather/setup reserve that matches the surviving half.",
        "revealed_information": ["cap 40", "joint and split branches", "levels 42-44", "Power Spot", "No Guard Dynamic Punch", "Blacephalon", "two sand abilities", "Coil/Body Press", "Intimidate", "four fresh species", "no Protect/reward"],
        "counterplay_classes": ["Water/Grass/Ice/Fairy/Fighting/Ghost/Dark", "special bulk and burn/Intimidate", "Taunt/Haze/phazing", "Wide Guard and weather replacement", "item removal", "Power Spot focus", "Blacephalon denial", "special pressure into Sandaconda"],
        "target_difficulty": 9.4,
        "difficulty_rationale": "The joint has six optimized levels 42-44, automatic Power Spot, guaranteed Dynamic Punch, rare Beast Boost special offense, sand, setup, sustain, and Intimidate. Public type/category/weather seams and no Protect keep all branches learnable.",
        "tuning_knob": "Tune both +4 closers to +3 first, then Blacephalon/Golurk +3 to +2; preserve geometry, species, lead, abilities, and no-Protect identity.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["mt-chimney", "required-native-pair", "split-singles", "land-construction", "power-spot", "no-guard", "stonjourner", "golurk", "blacephalon", "palossand", "sandaconda", "krookodile", "sand-stream", "sand-spit", "four-fresh-species", "no-protect", "no-speed-field", "no-mega"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Six exact-species generated references; physical path and construction identity are local."},
        "author_self_check": {"strongest_part": "The corridor itself creates a real cross-trainer lead and every reserve continues the Grunts' land-building motive.", "weakest_link": "Broad Water/Grass and physical mitigation are strong; mixed special reserves, sand, setup, support, items, and levels compensate without erasing them."},
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_108_MT_CHIMNEY_GRUNT_PAIR"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 108] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])

    sequence = json.loads(SEQUENCE.read_text())
    for row in sequence["entries"]:
        if row["index"] == 108:
            row.update({
                "category": "required opposing-sight Mt. Chimney Magma Grunt pair",
                "trainer_ids": ["TRAINER_GRUNT_MT_CHIMNEY_1", "TRAINER_GRUNT_MT_CHIMNEY_2"],
                "access_note": "Female Grunt at (13,16) faces left and male Grunt at (9,16) faces right, both with sight three; they form one native pair or split singles before Tabitha.",
            })
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 109] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 108 else "next" if row["index"] == 109 else "queued"

    os_data = json.loads(OS_PATH.read_text())
    os_data["current_state"].update({
        "closed_encounters": 108,
        "next_index": 109,
        "next_encounter_id": NEXT["encounter_id"],
        "queued_sequence_entries": 0,
        "canonical_sequence_groups": 109,
        "physical_encounter_groups": 525,
        "unordered_physical_groups": 416,
    })
    return designs, ledger, sequence, os_data


def path_distances() -> dict[tuple[int, int], int]:
    layouts = json.loads((ROOT / "data/layouts/layouts.json").read_text())["layouts"]
    layout = next(row for row in layouts if row["id"] == "LAYOUT_MT_CHIMNEY")
    width, height = layout["width"], layout["height"]
    values = struct.unpack("<" + "H" * (width * height), (ROOT / layout["blockdata_filepath"]).read_bytes())
    start = (17, 36)
    queue = collections.deque([start])
    distances = {start: 0}
    while queue:
        x, y = queue.popleft()
        for point in ((x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)):
            nx, ny = point
            if 0 <= nx < width and 0 <= ny < height and point not in distances and ((values[ny * width + nx] >> 10) & 3) == 0:
                distances[point] = distances[(x, y)] + 1
                queue.append(point)
    return distances


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    blocks = doubles.trainer_blocks(trainers)
    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for trainer_id, team in (("TRAINER_GRUNT_MT_CHIMNEY_1", GRUNT1_TEAM), ("TRAINER_GRUNT_MT_CHIMNEY_2", GRUNT2_TEAM)):
        block = blocks[trainer_id].group(0)
        actual = [polish.parse_entry(entry) for entry in custom.party_entries(doubles.party_match(parties, doubles.party_name(block)).group(2))]
        if actual != team:
            raise SystemExit(f"FAIL: Battle 108 source party differs for {trainer_id}")
        for token in (".doubleBattle = FALSE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"):
            if token not in block:
                raise SystemExit(f"FAIL: Battle 108 {trainer_id} missing {token}")
        for member in team:
            illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
            if illegal:
                raise SystemExit(f"FAIL: Battle 108 illegal moves for {member['species']}: {illegal}")
            if member["ability_slot"] >= len(slots[member["species"]]):
                raise SystemExit(f"FAIL: Battle 108 invalid ability slot for {member['species']}")
    combined = GRUNT1_TEAM + GRUNT2_TEAM
    if len({m["species"] for m in combined}) != 6 or len({m["item"] for m in combined}) != 6 or any("MOVE_PROTECT" in m["moves"] for m in combined):
        raise SystemExit("FAIL: Battle 108 uniqueness/no-Protect contract drifted")

    map_data = json.loads((ROOT / "data/maps/MtChimney/map.json").read_text())["object_events"]
    geometry = {row["script"]: (row["x"], row["y"], row["movement_type"], str(row["trainer_sight_or_berry_tree_id"])) for row in map_data if row.get("script") in {"MtChimney_EventScript_Grunt1", "MtChimney_EventScript_Grunt2"}}
    if geometry != {"MtChimney_EventScript_Grunt1": (13, 16, "MOVEMENT_TYPE_FACE_LEFT", "3"), "MtChimney_EventScript_Grunt2": (9, 16, "MOVEMENT_TYPE_FACE_RIGHT", "3")}:
        raise SystemExit("FAIL: Battle 108 opposing-sight geometry drifted")
    distances = path_distances()
    if {point: distances.get(point) for point in ((13, 16), (9, 16), (12, 11), (13, 6))} != {(13, 16): 54, (9, 16): 58, (12, 11): 62, (13, 6): 70}:
        raise SystemExit("FAIL: Battle 108 Mt. Chimney path-order proof drifted")
    script = (ROOT / "data/maps/MtChimney/scripts.inc").read_text()
    for trainer_id in ("TRAINER_GRUNT_MT_CHIMNEY_1", "TRAINER_GRUNT_MT_CHIMNEY_2"):
        if f"trainerbattle_single {trainer_id}" not in script:
            raise SystemExit(f"FAIL: Battle 108 split source missing {trainer_id}")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]
    expected_manifest = {
        "TRAINER_GRUNT_MT_CHIMNEY_1": {"format": "single", "target_size": 3, "archetype": "Lava-house foundation", "difficulty": 88, "partner_interaction": True, "level_offset": 3, "location": "Mt Chimney"},
        "TRAINER_GRUNT_MT_CHIMNEY_2": {"format": "single", "target_size": 3, "archetype": "Land-reclamation crew", "difficulty": 89, "partner_interaction": True, "level_offset": 3, "location": "Mt Chimney"},
    }
    for trainer_id, value in expected_manifest.items():
        if manifest[trainer_id] != value:
            raise SystemExit(f"FAIL: Battle 108 manifest stale for {trainer_id}")

    section = script.split("MtChimney_Text_Grunt2Intro:", 1)[1].split("MtChimney_Text_TeamAquaAlwaysMessingWithPlans:", 1)[0]
    for cue in ("breaking ground for everyone", "Golurk digs", "No Guard makes Dynamic Punch", "Sand Spit", "lava house needs a foundation", "Stonjourner marks safe ground", "Blacephalon fires the kiln", "Power Spot strengthens"):
        if cue not in section:
            raise SystemExit(f"FAIL: Battle 108 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', section):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 108 overlong dialogue: {visible}")

    ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 108 competitive reference missing")


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
                raise SystemExit(f"FAIL: Battle 108 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())["entries"]
        entries = [row for row in guide if row["trainerId"] in {"TRAINER_GRUNT_MT_CHIMNEY_1", "TRAINER_GRUNT_MT_CHIMNEY_2"}]
        if len(entries) != 2 or any(row["designStatus"] != "closed" or row["format"] != "single" or row["partySize"] != 3 for row in entries):
            raise SystemExit("FAIL: Battle 108 guide stale")
    print("PASS: Battle 108 Mt. Chimney opposing-sight construction pair is source-closed")


if __name__ == "__main__":
    main()
