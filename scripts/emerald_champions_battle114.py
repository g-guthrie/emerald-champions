#!/usr/bin/env python3
"""Generate and verify Battle 114, Autumn and Julio's Jagged Pass native pair."""
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


TEAM_AUTUMN = [
    {"level": 1, "species": "SPECIES_CHESNAUGHT", "item": "ITEM_ROCKY_HELMET", "ability_slot": 1, "spread": "SPREAD_31_IV_HP_DEF_IMPISH", "moves": ["MOVE_SPIKY_SHIELD", "MOVE_BODY_PRESS", "MOVE_SEED_BOMB", "MOVE_ROCK_SLIDE"]},
    {"level": 2, "species": "SPECIES_CONKELDURR", "item": "ITEM_FLAME_ORB", "ability_slot": 0, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_DRAIN_PUNCH", "MOVE_MACH_PUNCH", "MOVE_KNOCK_OFF", "MOVE_ICE_PUNCH"]},
    {"level": 3, "species": "SPECIES_AVALUGG", "item": "ITEM_WEAKNESS_POLICY", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_DEF_IMPISH", "moves": ["MOVE_AVALANCHE", "MOVE_BODY_PRESS", "MOVE_HIGH_HORSEPOWER", "MOVE_RECOVER"]},
]
TEAM_JULIO = [
    {"level": 1, "species": "SPECIES_DRAGAPULT", "item": "ITEM_CHOICE_SPECS", "ability_slot": 1, "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID", "moves": ["MOVE_DRAGON_PULSE", "MOVE_SHADOW_BALL", "MOVE_FLAMETHROWER", "MOVE_THUNDERBOLT"]},
    {"level": 2, "species": "SPECIES_JOLTEON", "item": "ITEM_LIFE_ORB", "ability_slot": 0, "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID", "moves": ["MOVE_THUNDERBOLT", "MOVE_SHADOW_BALL", "MOVE_HYPER_VOICE", "MOVE_VOLT_SWITCH"]},
    {"level": 3, "species": "SPECIES_DODRIO", "item": "ITEM_CHOICE_BAND", "ability_slot": 0, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_BRAVE_BIRD", "MOVE_JUMP_KICK", "MOVE_KNOCK_OFF", "MOVE_QUICK_ATTACK"]},
]
TEAMS = {"TRAINER_AUTUMN": TEAM_AUTUMN, "TRAINER_JULIO": TEAM_JULIO}

REFERENCES = [
    "showdown:gen9championsrandomdoublesbattle:018",
    "showdown:gen9championsrandomdoublesbattle:001",
    "vgc:worlds-2013",
    "showdown:gen8randomdoublesbattle:006",
    "smogon:gen8ou:009",
    "elite:wolfe:players-cup-ii-2020",
    "showdown:gen9championsrandomdoublesbattle:003",
    "showdown:gen7randomdoublesbattle:014",
]

NEXT = {
    "index": 115,
    "encounter_id": "BATTLE_115_JAGGED_PASS_ETHAN",
    "location": "JaggedPass",
    "category": "optional lower-pass Camper four-record Match Call family",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_ETHAN_1", "TRAINER_ETHAN_2", "TRAINER_ETHAN_3", "TRAINER_ETHAN_4"],
    "access_note": "Ethan faces left/right at (16,35) with sight four, 59 collision-walk steps from the upper entry and three steps before the lower exit. His physical position owns the initial fight and three reachable native rematches.",
}


def design() -> dict:
    return {
        "guide_order": 114,
        "trainer_ids": ["TRAINER_AUTUMN", "TRAINER_JULIO"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": "Optional central Jagged Pass ledge after Diana. Autumn stands at (14,25) facing right and Julio at (18,25) facing left, both with sight three; one approach can trigger both, while side approaches or a previously defeated partner produce either autonomous single.",
        "runtime_branches": ["Joint: Emerald's native two-opponent rules load the first three members from each source record into one six-enemy double.", "Split Autumn: three-member cap-relative intentional single.", "Split Julio: three-member cap-relative intentional single."],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 mature traction-versus-momentum showcase",
            "effective_levels": "41, 42, and 43 in each source half",
            "eligible_ratio": "6/6 source slots",
            "mega_access": True,
            "status": "pass",
            "reason": "Chesnaught, Conkeldurr, Avalugg, Dragapult, Jolteon, and Dodrio are all naturally final-stage or stone-evolved by their exact levels. The pair uses no Mega, and all six are new to the closed campaign.",
        },
        "manual_quality": 10,
        "manual_difficulty": 9.4,
        "split_difficulty": {"TRAINER_AUTUMN": 8.8, "TRAINER_JULIO": 8.9},
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [{"reference_id": reference, "decision": "role evidence selected; full donor rejected", "reason": "The indexed role informs one source half, while the ledge's traction-versus-momentum geometry requires two autonomous threes."} for reference in REFERENCES],
            "decision": "All 1005 references and the six authored species reviews were checked. Showdown, Smogon, Wolfe, and Worlds evidence support the exact roles, but the two-half composition is original and map-led rather than copied.",
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Champions random Chesnaught validates Spiky Shield and Body Press; local Sturdy plus Rocky Helmet makes the first brace public and finite."},
            {"reference_id": REFERENCES[1], "adaptation": "Champions random Conkeldurr supplies direct Fighting priority and coverage; local Flame Orb exposes the Guts clock."},
            {"reference_id": REFERENCES[2], "adaptation": "Arash Ommati's 2013 Worlds roster validates Conkeldurr at championship doubles stakes without importing the whole historic team."},
            {"reference_id": REFERENCES[3], "adaptation": "Random-doubles Sturdy Avalugg contributes Avalanche, Ground coverage, and Recover; Weakness Policy replaces passive sustain."},
            {"reference_id": REFERENCES[4], "adaptation": "Published Specs Infiltrator Dragapult supplies Julio's immediate special line and visible Choice lock."},
            {"reference_id": REFERENCES[5], "adaptation": "Wolfe Glick's Players Cup II roster validates Dragapult as elite fast pressure; its unavailable Coalossal activation is deliberately not copied."},
            {"reference_id": REFERENCES[6], "adaptation": "Champions random Jolteon supplies Volt Absorb and immediate special speed; local four-attack Life Orb keeps it autonomous."},
            {"reference_id": REFERENCES[7], "adaptation": "Random-doubles Dodrio validates Brave Bird and Knock Off; local Moxie plus Choice Band creates the final downhill commitment."},
        ],
        "ordering": {
            "joint_lead": ["SPECIES_CHESNAUGHT", "SPECIES_DRAGAPULT"],
            "autumn_order": ["SPECIES_CHESNAUGHT", "SPECIES_CONKELDURR", "SPECIES_AVALUGG"],
            "julio_order": ["SPECIES_DRAGAPULT", "SPECIES_JOLTEON", "SPECIES_DODRIO"],
            "reason": "Autumn progresses from one Sturdy brace through Guts pressure to a second Sturdy anchor. Julio progresses from a Specs special line through a free-moving Life Orb pivot to a Choice Band Moxie commitment. Native replacement selection may respond to the board without changing either readable source half.",
        },
        "team_intent": "Autumn is traction: two Sturdy checkpoints around Flame Orb Guts strength. Julio is momentum: three naturally fast attackers, two public Choice locks, Volt Switch, and a Moxie closer. Jointly, Autumn's durability buys Julio attacking turns; separately, each three-member source asks one complete and different singles question.",
        "primary_player_question": "Can the player punish Julio's fragile locks and pivots without carelessly activating Autumn's Sturdy Weakness Policy or letting Guts turn the slower half into permanent pressure?",
        "intended_counterplay": "Autumn is all physical and slow: burn, Intimidate, Reflect, Ghosts, special attacks, Taunt, Encore, item removal, multihit, Mold Breaker, phazing, Fighting, Fire, Flying, Fairy, Psychic, Water, Grass, and Steel all divide the half. Julio has no recovery or Protect: priority, Trick Room, Rock/Ground/Ice/Fairy/Dark/Ghost attacks, Sucker Punch, Choice exploitation, recoil, and focused damage punish it. Jointly, Wide Guard, Snarl, spread pressure, speed reversal, and deleting either axis all work.",
        "bespoke_ai": "Both records remain intentional singles with smart switching and HP awareness. Under native two-opponent rules, ordinary foe-aware scoring evaluates both active opponents and each owner can choose only its own three slots. Sturdy, Spiky Shield, Guts, Flame Orb, Weakness Policy, Choice locks, Life Orb, Volt Switch, Infiltrator, Volt Absorb, Moxie, recoil, and coverage are native; no move, target, replacement, or turn is forced.",
        "uniqueness": "All six species are new to the first 113 closed encounters and absent from protected future anchor teams. This is the first traction-versus-momentum pair and the first deliberate use of two different Sturdy checkpoints against three naturally fast, field-free attackers. It uses no weather, terrain, room, Tailwind, Icy Wind, Electroweb, sleep, hazard, Mega, or legendary.",
        "story_logic": "Autumn's daily-climb dialogue now describes sure footing and her three strength roles. Julio's bike dialogue now describes three no-brakes attackers and their item commitments. Both post-battle speeches remain true whether fought alone; each also explains what the other contributes if the sight intersection creates the joint double.",
        "reward_logic": "Ordinary EXP and prize money only. No flags beyond the two native trainer-defeat flags and no item/story rewards are added.",
        "campaign_reservations": {
            "spends": ["Jagged Pass traction-versus-momentum native pair", "Sturdy Chesnaught", "Guts Conkeldurr", "Weakness Policy Avalugg", "Specs Dragapult", "Life Orb Jolteon", "Moxie Dodrio"],
            "preserves": ["every protected anchor team", "Wolfe's Coalossal activation", "all Megas and legendary families", "weather and speed-field identities", "full Weakness Policy activation cores"],
            "repeat_rule": "Do not repeat the exact slow Sturdy/Guts half beside a three-stage natural-speed Choice relay. Individual species require a long gap and materially different role.",
        },
        "author_self_check": {
            "strongest_part": "The physical geometry, dialogue, items, and battle tempo all express the same contrast: Autumn holds the mountain while Julio throws himself down it.",
            "weakest_link": "The joint team has complementarity rather than a single explosive ally activation. That is intentional after several mechanic-dense fights; cap-plus levels, six distinct items, two Sturdy checkpoints, Guts, two Choice locks, pivoting, and mixed categories keep the 9.4 pressure real without another module.",
        },
        "closure": "Battle 114 is source-closed at quality 10 and target 9.4 jointly, 8.8/8.9 split: exact opposing-sight geometry, native first-three slicing, six legal levels 41-43, six fresh unreserved species and distinct items, eight indexed references spanning Showdown/Smogon/Wolfe/Worlds, truthful native-width dialogue, broad counterplay, and zero reward debt. Runtime remains unplayed.",
    }


def ledger_entry() -> dict:
    return {
        "index": 114,
        "encounter_id": "BATTLE_114_JAGGED_PASS_AUTUMN_JULIO",
        "identity": {"location": "JaggedPass", "category": "optional opposing-sight Picnicker/Triathlete pair", "format": "joint native double or either split single", "strict_cap": 40, "memory_hook": "Autumn's two Sturdy anchors hold the ledge while Julio's three no-brakes attackers spend safety for momentum."},
        "primary_player_question": "Can the player punish Julio's locks and pivots without activating Autumn's Weakness Policy anchor or letting Guts become permanent pressure?",
        "tempo": "Three-member traction single plus three-member momentum single; jointly a six-enemy mixed-axis native double.",
        "pressure_sources": ["Rocky Helmet Sturdy Chesnaught", "Flame Orb Guts Conkeldurr", "Sturdy Weakness Policy Avalugg", "Choice Specs Infiltrator Dragapult", "Life Orb Volt Switch Jolteon", "Choice Band Moxie Dodrio"],
        "intentional_opening": "Opposing sight lines can produce Chesnaught plus Dragapult jointly; side approaches preserve either exact three-member single.",
        "intentional_weakness": "Autumn is slow and all physical; Julio is fragile with no recovery/Protect and two Choice locks; the pair has no field, automatic weather, speed move, sleep, hazard, Mega, legendary, or scripted activation.",
        "first_loss_lesson": "Separate traction from momentum: use special/status control and careful damage into Autumn, while reversing speed or exploiting Choice locks against Julio. Do not casually trigger Avalugg's Weakness Policy.",
        "revealed_information": ["cap 40", "joint/split geometry", "levels 41-43", "six fresh species", "six distinct items", "no finite reward"],
        "counterplay_classes": ["burn/Intimidate/Reflect/Ghosts/special pressure", "multihit/Mold Breaker/Taunt/Encore/item removal", "priority/Trick Room/Choice exploitation/recoil", "common Fighting/Fire/Flying/Fairy/Psychic/Water/Grass/Rock/Ground/Ice/Dark/Ghost attacks", "spread pressure/Wide Guard/Snarl/focus either axis"],
        "target_difficulty": 9.4,
        "difficulty_rationale": "Six optimized cap-plus-one-to-three members with mixed categories and complementary tempo create a severe joint double; each autonomous three remains a serious but narrower 8.8/8.9 single.",
        "tuning_knob": "Reduce joint Avalugg and Dodrio from +3 to +2 first; preserve source ownership, species, items, and geometry.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["jagged-pass", "native-pair-double", "split-singles", "traction-versus-momentum", "chesnaught", "conkeldurr", "avalugg", "dragapult", "jolteon", "dodrio", "sturdy-checkpoints", "guts", "choice-locks", "six-fresh-species", "no-speed-field", "no-weather", "no-mega", "no-legendary"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Eight indexed Showdown, Smogon, Wolfe, and tournament references; two source halves are locally authored."},
        "author_self_check": {"strongest_part": "Map geometry and opposing trainer personalities become one readable six-member battle.", "weakest_link": "No bespoke ally activation; mixed tempo and source autonomy are the intended novelty."},
    }


def payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_114_JAGGED_PASS_AUTUMN_JULIO"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 114] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])
    sequence = json.loads(SEQUENCE.read_text())
    for row in sequence["entries"]:
        if row["index"] == 114:
            row.update({"category": "optional central-ledge opposing-sight Picnicker/Triathlete pair", "trainer_ids": ["TRAINER_AUTUMN", "TRAINER_JULIO"], "access_note": "Autumn at (14,25) faces right and Julio at (18,25) faces left, each with sight three. Their first-three source halves form one native joint double or two independent singles."})
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 115] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 114 else "next" if row["index"] == 115 else "queued"
    os_data = json.loads(OS_PATH.read_text())
    os_data["current_state"].update({"closed_encounters": 114, "next_index": 115, "next_encounter_id": NEXT["encounter_id"], "queued_sequence_entries": 0, "canonical_sequence_groups": 115, "physical_encounter_groups": 524, "unordered_physical_groups": 409})
    return designs, ledger, sequence, os_data


def protected_anchor_species() -> set[str]:
    protected: set[str] = set()
    for path in ROOT.glob("docs/emerald_champions_*anchor_designs.json"):
        for anchor in json.loads(path.read_text()).get("designs", {}).values():
            for member in anchor.get("team", []):
                if isinstance(member, dict) and isinstance(member.get("species"), str):
                    protected.add(member["species"])
    return protected


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    blocks = doubles.trainer_blocks(trainers)
    dex = presets.LocalDex()
    ability_slots = doubles.base_ability_slots()
    for trainer_id, team in TEAMS.items():
        block = blocks[trainer_id].group(0)
        actual = [polish.parse_entry(entry) for entry in custom.party_entries(doubles.party_match(parties, doubles.party_name(block)).group(2))]
        if actual != team:
            raise SystemExit(f"FAIL: Battle 114 party differs {trainer_id}")
        for token in (".doubleBattle = FALSE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HP_AWARE"):
            if token not in block:
                raise SystemExit(f"FAIL: Battle 114 {trainer_id} missing {token}")
        if len(team) != 3 or len({member["species"] for member in team}) != 3 or len({member["item"] for member in team}) != 3:
            raise SystemExit(f"FAIL: Battle 114 source-half shape {trainer_id}")
        for member in team:
            illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
            if illegal or member["ability_slot"] >= len(ability_slots[member["species"]]):
                raise SystemExit(f"FAIL: Battle 114 legality {member['species']} {illegal}")

    if len({member["species"] for team in TEAMS.values() for member in team}) != 6 or len({member["item"] for team in TEAMS.values() for member in team}) != 6:
        raise SystemExit("FAIL: Battle 114 joint uniqueness")
    if {member["species"] for team in TEAMS.values() for member in team} & protected_anchor_species():
        raise SystemExit("FAIL: Battle 114 protected anchor collision")

    map_data = json.loads((ROOT / "data/maps/JaggedPass/map.json").read_text())["object_events"]
    objects = {row["script"]: row for row in map_data if row.get("script") in {"JaggedPass_EventScript_Autumn", "JaggedPass_EventScript_Julio"}}
    autumn = objects["JaggedPass_EventScript_Autumn"]
    julio = objects["JaggedPass_EventScript_Julio"]
    if (autumn["x"], autumn["y"], autumn["movement_type"], str(autumn["trainer_sight_or_berry_tree_id"])) != (14, 25, "MOVEMENT_TYPE_FACE_RIGHT", "3"):
        raise SystemExit("FAIL: Battle 114 Autumn geometry")
    if (julio["x"], julio["y"], julio["movement_type"], str(julio["trainer_sight_or_berry_tree_id"])) != (18, 25, "MOVEMENT_TYPE_FACE_LEFT", "3"):
        raise SystemExit("FAIL: Battle 114 Julio geometry")
    script = (ROOT / "data/maps/JaggedPass/scripts.inc").read_text()
    for token in ("trainerbattle_single TRAINER_AUTUMN", "trainerbattle_single TRAINER_JULIO"):
        if token not in script:
            raise SystemExit(f"FAIL: Battle 114 script missing {token}")
    dialogue = script.split("JaggedPass_Text_JulioIntro:", 1)[1].split("JaggedPass_Text_BoulderShakingInResponseToEmblem:", 1)[0]
    for cue in ("all momentum", "Dragapult, Jolteon, and Dodrio", "Choice Specs, Life Orb, Choice Band", "sure footing", "Chesnaught braces", "Conkeldurr drives", "Avalugg refuses", "anchor Julio's speed"):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 114 dialogue missing {cue}")
    for raw_line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = raw_line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 114 overlong dialogue: {visible}")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]
    expected = {
        "TRAINER_AUTUMN": {"format": "single", "target_size": 3, "archetype": "Sure-footed strength half", "difficulty": 88, "partner_interaction": True, "level_offset": 2, "location": "Jagged Pass"},
        "TRAINER_JULIO": {"format": "single", "target_size": 3, "archetype": "No-brakes momentum half", "difficulty": 89, "partner_interaction": True, "level_offset": 2, "location": "Jagged Pass"},
    }
    if any(manifest[trainer_id] != payload for trainer_id, payload in expected.items()):
        raise SystemExit("FAIL: Battle 114 manifest")
    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference not in corpus_ids for reference in REFERENCES):
        raise SystemExit("FAIL: Battle 114 reference")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.write and not args.check:
        parser.error("choose --write or --check")
    generated = payloads()
    paths = (DESIGNS, LEDGER, SEQUENCE, OS_PATH)
    texts = [json.dumps(payload, indent=2, ensure_ascii=False) + "\n" for payload in generated]
    if args.write:
        for path, text in zip(paths, texts):
            path.write_text(text)
    if args.check:
        for path, text in zip(paths, texts):
            if path.read_text() != text:
                raise SystemExit(f"FAIL: Battle 114 stale {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())["entries"]
        entries = [row for row in guide if row["trainerId"] in TEAMS]
        if len(entries) != 2 or any(row["designStatus"] != "closed" or row["format"] != "single" or row["partySize"] != 3 for row in entries):
            raise SystemExit("FAIL: Battle 114 guide")
    print("PASS: Battle 114 Autumn/Julio traction-versus-momentum pair is source-closed")


if __name__ == "__main__":
    main()
