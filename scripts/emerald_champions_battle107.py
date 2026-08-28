#!/usr/bin/env python3
"""Generate and verify Battle 107, John and Jay's three-pair anniversary family."""

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


def mon(level: int, species: str, item: str, ability: int, spread: str, moves: list[str]) -> dict:
    return {"level": level, "species": species, "item": item, "ability_slot": ability, "spread": spread, "moves": moves}


PLUSLE = mon(3, "SPECIES_PLUSLE", "ITEM_MAGNET", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", ["MOVE_THUNDERBOLT", "MOVE_GRASS_KNOT", "MOVE_ENCORE", "MOVE_PROTECT"])
MINUN = mon(3, "SPECIES_MINUN", "ITEM_SITRUS_BERRY", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", ["MOVE_FAKE_TEARS", "MOVE_HELPING_HAND", "MOVE_THUNDERBOLT", "MOVE_PROTECT"])
VOLBEAT = mon(4, "SPECIES_VOLBEAT", "ITEM_LUM_BERRY", 2, "SPREAD_31_IV_SPATK_SPEED_TIMID", ["MOVE_TAIL_GLOW", "MOVE_BUG_BUZZ", "MOVE_ENCORE", "MOVE_PROTECT"])
ILLUMISE = mon(4, "SPECIES_ILLUMISE", "ITEM_LEFTOVERS", 2, "SPREAD_31_IV_HP_SPATK_MODEST", ["MOVE_STRUGGLE_BUG", "MOVE_DAZZLING_GLEAM", "MOVE_WISH", "MOVE_HELPING_HAND"])
ACCELGOR = mon(3, "SPECIES_ACCELGOR", "ITEM_EXPERT_BELT", 1, "SPREAD_31_IV_SPATK_SPEED_TIMID", ["MOVE_BUG_BUZZ", "MOVE_ACID_SPRAY", "MOVE_ENERGY_BALL", "MOVE_PROTECT"])
ESCAVALIER = mon(3, "SPECIES_ESCAVALIER", "ITEM_OCCA_BERRY", 2, "SPREAD_31_IV_HP_ATK_BRAVE", ["MOVE_MEGAHORN", "MOVE_IRON_HEAD", "MOVE_DRILL_RUN", "MOVE_PROTECT"])


def at_level(member: dict, level: int) -> dict:
    value = dict(member)
    value["level"] = level
    value["moves"] = list(member["moves"])
    return value


TEAMS = {
    "TRAINER_JOHN_AND_JAY_1": [PLUSLE, MINUN, VOLBEAT, ILLUMISE],
    "TRAINER_JOHN_AND_JAY_2": [ACCELGOR, ESCAVALIER, at_level(PLUSLE, 4), at_level(MINUN, 4)],
    "TRAINER_JOHN_AND_JAY_3": [at_level(VOLBEAT, 3), at_level(ILLUMISE, 3), at_level(ACCELGOR, 4), at_level(ESCAVALIER, 4)],
    "TRAINER_JOHN_AND_JAY_4": [at_level(PLUSLE, 2), at_level(MINUN, 2), at_level(VOLBEAT, 3), at_level(ILLUMISE, 3), at_level(ACCELGOR, 4), at_level(ESCAVALIER, 4)],
}

REFERENCES = [
    "showdown:gen4randombattle:004",
    "showdown:gen6randomdoublesbattle:005",
    "showdown:gen4randomdoublesbattle:002",
    "smogon:gen9nu:005",
    "showdown:gen6randomdoublesbattle:009",
    "showdown:gen8randomdoublesbattle:015",
]

NEXT = {
    "index": 108,
    "encounter_id": "BATTLE_108_MT_CHIMNEY_GRUNT_PAIR",
    "location": "MtChimney",
    "category": "story-ascent opposing-sight Team Magma Grunt pair",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_GRUNT_MT_CHIMNEY_1", "TRAINER_GRUNT_MT_CHIMNEY_2"],
    "access_note": (
        "The first active story-trainer corridor pairs the female Grunt at (13,16), facing left with sight three, and the "
        "male Grunt at (9,16), facing right with sight three. Their opposing sight lines permit one native-pair double or two split singles before Tabitha and Maxie."
    ),
}


def design() -> dict:
    return {
        "guide_order": 107,
        "trainer_ids": list(TEAMS),
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Final optional family in Meteor Falls 1F 2R. John and Jay occupy adjacent tiles and either NPC launches the "
            "same guarded double, registration callback, and four-record Match Call sequence. The initial is cap 40; rematches "
            "require five badges and are earliest at cap 45."
        ),
        "runtime_branches": [
            "John initial script and Jay initial script both resolve to JOHN_AND_JAY_1.",
            "Either registration path records the same Match Call family.",
            "John and Jay rematch scripts both resolve sequentially through JOHN_AND_JAY_2, _3, and _4.",
            "Every initial/rematch branch refuses safely if the player cannot field two usable Pokemon.",
        ],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 underused-pair lesson and cap-45+ anniversary rematches",
            "effective_levels": "initial 43/43/44/44; rematches earliest 48-49; final earliest 47/47/48/48/49/49",
            "eligible_ratio": "18/18",
            "mega_access": True,
            "status": "pass",
            "reason": "Plusle, Minun, Volbeat, and Illumise are single-stage. Accelgor and Escavalier evolve by the reciprocal Karrablast/Shelmet trade and have no minimum-level conflict.",
        },
        "manual_quality": 10,
        "manual_difficulty": 9.0,
        "rematch_difficulty": {"TRAINER_JOHN_AND_JAY_2": 9.2, "TRAINER_JOHN_AND_JAY_3": 9.4, "TRAINER_JOHN_AND_JAY_4": 9.6},
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {"reference_id": REFERENCES[0], "decision": "Plusle role adapted", "reason": "Generated Plus offense validates the species; the couple uses literal Plus/Minus doubles amplification instead of Baton Pass or Sash."},
                {"reference_id": REFERENCES[1], "decision": "Minun role adapted", "reason": "The exact doubles set validates Minun's special endurance; local Fake Tears and Helping Hand make its partner identity explicit."},
                {"reference_id": REFERENCES[2], "decision": "Illumise role selected", "reason": "The exact doubles set validates Wish, Encore, and Bug offense; local Prankster support and Struggle Bug avoid another speed field."},
                {"reference_id": REFERENCES[3], "decision": "Volbeat role adapted", "reason": "Published Prankster utility validates Volbeat; Tail Glow becomes the sole setup clock without importing sun, Thunder Wave, or Tailwind."},
                {"reference_id": REFERENCES[4], "decision": "Accelgor role adapted", "reason": "The exact doubles set validates its speed and disruption; Acid Spray creates a visible special-defense handoff."},
                {"reference_id": REFERENCES[5], "decision": "Escavalier role selected", "reason": "The exact doubles set validates Overcoat physical pressure and Protect; Occa Berry preserves one public Fire answer without setup."},
            ],
            "decision": (
                "All 1005 references were available. Six exact-species records support the roles; no complete donor team "
                "expresses the physical couple, reciprocal evolution, and three-pair Match Call progression."
            ),
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Magnet Plusle attacks immediately under the shared Plus/Minus boost, with Encore and Protect as public utility."},
            {"reference_id": REFERENCES[1], "adaptation": "Sitrus Minus Minun supplies Fake Tears, Helping Hand, Thunderbolt, and Protect rather than duplicate setup."},
            {"reference_id": REFERENCES[2], "adaptation": "Prankster Illumise supports through Wish, Helping Hand, and spread Struggle Bug while retaining Dazzling Gleam pressure."},
            {"reference_id": REFERENCES[3], "adaptation": "Prankster Volbeat has the family's one Tail Glow clock, Bug Buzz, Encore, and Protect."},
            {"reference_id": REFERENCES[4], "adaptation": "Sticky Hold Accelgor uses Acid Spray to open special defense, then Bug/Grass coverage and Protect."},
            {"reference_id": REFERENCES[5], "adaptation": "Occa Overcoat Escavalier provides the physical axis through Megahorn, Iron Head, Drill Run, and Protect."},
        ],
        "ordering": {
            "TRAINER_JOHN_AND_JAY_1": {"lead": ["SPECIES_PLUSLE", "SPECIES_MINUN"], "reserves": ["SPECIES_VOLBEAT", "SPECIES_ILLUMISE"]},
            "TRAINER_JOHN_AND_JAY_2": {"lead": ["SPECIES_ACCELGOR", "SPECIES_ESCAVALIER"], "reserves": ["SPECIES_PLUSLE", "SPECIES_MINUN"]},
            "TRAINER_JOHN_AND_JAY_3": {"lead": ["SPECIES_VOLBEAT", "SPECIES_ILLUMISE"], "reserves": ["SPECIES_ACCELGOR", "SPECIES_ESCAVALIER"]},
            "TRAINER_JOHN_AND_JAY_4": {"lead": ["SPECIES_PLUSLE", "SPECIES_MINUN"], "reserves": ["SPECIES_VOLBEAT", "SPECIES_ILLUMISE", "SPECIES_ACCELGOR", "SPECIES_ESCAVALIER"]},
        },
        "team_intent": (
            "The initial double turns Plus and Minus into immediate amplified special pressure, then changes to Prankster Tail "
            "Glow and reciprocal support. Rematch one introduces the trade-linked speed/armor pair before returning to Plus/Minus. "
            "Rematch two tests Volbeat/Illumise first, then changes category through Escavalier. The final six retains all three "
            "pair identities. No record uses weather, room, Tailwind, Fake Out, priority offense, Mega, legendary, or a premium-item shell."
        ),
        "intended_counterplay": (
            "Ground pressure, Lightning Rod, Volt Absorb, special bulk, Snarl, Fake Tears reversal, item removal, Encore, Taunt, "
            "Haze, Unaware, phazing, spread damage, Rock/Fire/Flying, and focused targeting are broad. Split Plusle from Minun to "
            "remove the shared boost; deny Volbeat's one setup turn; pressure Illumise before Wish lands; exploit Accelgor's frailty; "
            "and use special Fire or remove Occa before Escavalier. Five members are special attackers, but Escavalier punishes one-category answers."
        ),
        "bespoke_ai": (
            "All four records use smart switching, partner awareness, HP awareness, and Combo Setup. Plus/Minus and Prankster "
            "resolve automatically; existing AI values Fake Tears, Helping Hand, Tail Glow, Wish, Struggle Bug, Encore, Acid Spray, "
            "and Protect only from visible state and recognizes the partner's selected Helping Hand. No target, switch, support turn, "
            "or setup action is forced."
        ),
        "uniqueness": (
            "Minun, Volbeat, Illumise, and Escavalier are new to the first 106 encounters. Plusle returns 54 battles later and "
            "Accelgor 36 battles later in entirely new paired roles. The inherited generic Fighting, Trick Room/Policy, and Beat Up "
            "Terrakion modules—and recent Infernape repeat—are removed. This is the only reciprocal-evolution anniversary family."
        ),
        "story_logic": (
            "Both NPC perspectives now describe the same fifty-year partnership through their actual lead pair. Post-battle text "
            "teaches Plus/Minus and Prankster; rematches introduce the Karrablast/Shelmet exchange, Acid Spray, Overcoat, and Occa. "
            "Both physical scripts, registration paths, refusal branches, and four-record Match Call routing remain native."
        ),
        "reward_logic": "EXP and prize money on every record; one shared Match Call registration is the sole progression reward.",
        "campaign_reservations": {
            "spends": ["John/Jay three-pair anniversary family", "Plusle/Minun amplification", "Volbeat/Illumise Prankster support", "Accelgor/Escavalier reciprocal-evolution pair"],
            "preserves": ["all protected historic tournament teams", "every Mega and legendary", "future Trick Room and Beat Up bosses", "Pachirisu/Gyarados", "Indeedee/Armarouge"],
            "repeat_rule": "These six species may repeat inside this family only; later support couples must change the pair mechanic and primary question.",
        },
        "author_self_check": {
            "strongest_part": "The battle finally feels inseparable from its two NPCs: every roster slot is one half of a visible pair, and each rematch changes which relationship leads.",
            "weakest_link": "The roster's raw BST is modest and five members are special. +2 to +4 levels, Plus/Minus, Prankster Tail Glow/support, Acid Spray, final depth, and Escavalier's physical axis create pressure without hiding broad Ground, Fire, Taunt, Haze, and special-bulk answers."
        },
        "closure": (
            "Battle 107's shared family is source-closed at quality 10: targets 9.0/9.2/9.4/9.6; two physical entry scripts, "
            "two registration paths, four guarded doubles, 18 legal cap-relative slots, four fresh and two distant role-changed "
            "species, six indexed references, native-width dialogue, broad counterplay, and zero reward debt. Runtime remains unplayed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 107,
        "encounter_id": "BATTLE_107_METEOR_FALLS_JOHN_JAY",
        "identity": {"location": "MeteorFalls_1F_2R", "category": "optional Expert couple four-record Match Call family", "format": "four guarded doubles through two physical scripts", "strict_cap": 40, "memory_hook": "Three literal pairs—Plusle/Minun, Volbeat/Illumise, and trade-linked Accelgor/Escavalier—turn fifty years of marriage into battle mechanics."},
        "primary_player_question": "Can the player separate each active pair before its amplification or support compounds, then change from special disruption to Escavalier's physical finish?",
        "tempo": "Four cap-relative doubles: Plus/Minus introduction, trade-pair rematch, Prankster-support rematch, then three-pair anniversary final.",
        "pressure_sources": ["Plus/Minus special amplification", "Fake Tears and Helping Hand Minun", "Prankster Tail Glow Volbeat", "Prankster Wish/Struggle Bug Illumise", "Acid Spray Accelgor", "Occa Overcoat physical Escavalier"],
        "intentional_opening": "Every record opens an exact thematic pair from either NPC script; no support or setup turn is forced.",
        "intentional_weakness": "Modest raw BST, five special attackers, shared Ground and Bug weaknesses, one setup user, no speed field/priority/weather/room/Mega/legend, and public support items.",
        "first_loss_lesson": "Do not damage both partners evenly. Separate or disable the active pair first, then preserve a physical answer for Escavalier after special disruption.",
        "revealed_information": ["initial cap 40", "five-badge rematches", "two physical scripts", "four guarded doubles", "levels cap+2 to +4", "three paired mechanics", "four fresh species", "no Mega/legend/reward"],
        "counterplay_classes": ["Ground/Lightning Rod/Volt Absorb", "special bulk/Snarl", "Taunt/Encore/Haze/Unaware/phazing", "spread and focused targeting", "Rock/Fire/Flying", "item removal", "Accelgor frailty", "Occa removal and special Fire into Escavalier"],
        "target_difficulty": 9.0,
        "difficulty_rationale": "The initial low-BST four are offset by levels 43-44, literal Plus/Minus amplification, Prankster setup/support, full items, and exact ordering. Rematches add category contrast, reordered pairs, and six-body depth without removing broad answers.",
        "tuning_knob": "Tune final Escavalier/Accelgor +4 to +3 first, then the Prankster pair +3 to +2; preserve all three pair identities and routing.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["meteor-falls-rematch-family", "old-couple", "two-physical-scripts", "four-guarded-doubles", "plus-minus", "plusle", "minun", "prankster-pair", "volbeat", "illumise", "trade-evolution-pair", "accelgor", "escavalier", "acid-spray", "four-fresh-species", "no-speed-field", "no-priority-offense", "no-mega", "no-legendary"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Six exact-species Showdown/Smogon references; three-pair anniversary progression is local."},
        "author_self_check": {"strongest_part": "Every mechanic and line of dialogue belongs specifically to a fifty-year battling couple.", "weakest_link": "Low BST and special concentration are real; levels, automatic amplification, disruption, setup, depth, and physical Escavalier compensate without erasing them."},
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_107_METEOR_FALLS_JOHN_JAY"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 107] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])

    sequence = json.loads(SEQUENCE.read_text())
    for row in sequence["entries"]:
        if row["index"] == 107:
            row.update({
                "category": "optional Expert couple shared four-record Match Call family",
                "trainer_ids": list(TEAMS),
                "access_note": "John at (6,12) and Jay at (7,12) own two physical scripts but one initial record, registration, and three sequential rematches.",
            })
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 108] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 107 else "next" if row["index"] == 108 else "queued"

    os_data = json.loads(OS_PATH.read_text())
    os_data["current_state"].update({
        "closed_encounters": 107,
        "next_index": 108,
        "next_encounter_id": NEXT["encounter_id"],
        "queued_sequence_entries": 0,
        "canonical_sequence_groups": 108,
        "physical_encounter_groups": 525,
        "unordered_physical_groups": 417,
    })
    return designs, ledger, sequence, os_data


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    blocks = doubles.trainer_blocks(trainers)
    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for trainer_id, team in TEAMS.items():
        block = blocks[trainer_id].group(0)
        actual = [polish.parse_entry(entry) for entry in custom.party_entries(doubles.party_match(parties, doubles.party_name(block)).group(2))]
        if actual != team:
            raise SystemExit(f"FAIL: Battle 107 source party differs for {trainer_id}")
        for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_COMBO_SETUP"):
            if token not in block:
                raise SystemExit(f"FAIL: Battle 107 {trainer_id} missing {token}")
        if len({m["species"] for m in team}) != len(team) or len({m["item"] for m in team}) != len(team):
            raise SystemExit(f"FAIL: Battle 107 duplicate species/item in {trainer_id}")
        for member in team:
            illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
            if illegal:
                raise SystemExit(f"FAIL: Battle 107 illegal moves for {member['species']}: {illegal}")
            if member["ability_slot"] >= len(slots[member["species"]]):
                raise SystemExit(f"FAIL: Battle 107 invalid ability slot for {member['species']}")

    script = (ROOT / "data/maps/MeteorFalls_1F_2R/scripts.inc").read_text()
    if script.count("trainerbattle_double TRAINER_JOHN_AND_JAY_1") != 2 or script.count("trainerbattle_rematch_double TRAINER_JOHN_AND_JAY_1") != 2 or script.count("register_matchcall TRAINER_JOHN_AND_JAY_1") != 2:
        raise SystemExit("FAIL: Battle 107 shared physical routing drifted")
    if "REMATCH(TRAINER_JOHN_AND_JAY_1, TRAINER_JOHN_AND_JAY_2, TRAINER_JOHN_AND_JAY_3, TRAINER_JOHN_AND_JAY_4, METEOR_FALLS_1F_2R)" not in (ROOT / "src/battle_setup.c").read_text():
        raise SystemExit("FAIL: Battle 107 rematch row drifted")
    geometry = {row["script"]: (row["x"], row["y"], row["movement_type"], str(row["trainer_sight_or_berry_tree_id"])) for row in json.loads((ROOT / "data/maps/MeteorFalls_1F_2R/map.json").read_text())["object_events"] if row.get("script") in {"MeteorFalls_1F_2R_EventScript_John", "MeteorFalls_1F_2R_EventScript_Jay"}}
    if geometry != {"MeteorFalls_1F_2R_EventScript_John": (6, 12, "MOVEMENT_TYPE_FACE_DOWN", "1"), "MeteorFalls_1F_2R_EventScript_Jay": (7, 12, "MOVEMENT_TYPE_FACE_DOWN", "1")}:
        raise SystemExit("FAIL: Battle 107 couple geometry drifted")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]
    expected_manifest = {
        "TRAINER_JOHN_AND_JAY_1": {"format": "double", "target_size": 4, "archetype": "Fifty-year amplification", "difficulty": 90, "partner_interaction": True, "level_offset": 4, "location": "Meteor Falls 1 F 2 R"},
        "TRAINER_JOHN_AND_JAY_2": {"format": "double", "target_size": 4, "archetype": "Trade-pair rematch", "difficulty": 92, "partner_interaction": True, "level_offset": 4, "location": "Meteor Falls 1 F 2 R"},
        "TRAINER_JOHN_AND_JAY_3": {"format": "double", "target_size": 4, "archetype": "Prankster-support rematch", "difficulty": 94, "partner_interaction": True, "level_offset": 4, "location": "Meteor Falls 1 F 2 R"},
        "TRAINER_JOHN_AND_JAY_4": {"format": "double", "target_size": 6, "archetype": "Three-pair anniversary final", "difficulty": 96, "partner_interaction": True, "level_offset": 4, "location": "Meteor Falls 1 F 2 R"},
    }
    for trainer_id, value in expected_manifest.items():
        if manifest[trainer_id] != value:
            raise SystemExit(f"FAIL: Battle 107 manifest stale for {trainer_id}")

    section = script.split("MeteorFalls_1F_2R_Text_JohnIntro:", 1)[1].split("MoveTutor_Text_DragonAscentTeach:", 1)[0]
    for cue in ("Plusle and Minun amplify", "Plus and Minus raise", "Volbeat grows, Illumise supports", "Tail Glow powers Volbeat", "Accelgor gained speed; Escavalier", "Karrablast and Shelmet traded", "Acid Spray opens", "Overcoat ignores powder", "final team keeps three pairs"):
        if cue not in section:
            raise SystemExit(f"FAIL: Battle 107 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', section):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 107 overlong dialogue: {visible}")

    ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 107 competitive reference missing")


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
                raise SystemExit(f"FAIL: Battle 107 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())["entries"]
        entries = [row for row in guide if row["trainerId"] in TEAMS]
        if len(entries) != 4 or any(row["designStatus"] != "closed" or row["format"] != "double" for row in entries):
            raise SystemExit("FAIL: Battle 107 guide stale")
        if {row["trainerId"]: row["partySize"] for row in entries} != {"TRAINER_JOHN_AND_JAY_1": 4, "TRAINER_JOHN_AND_JAY_2": 4, "TRAINER_JOHN_AND_JAY_3": 4, "TRAINER_JOHN_AND_JAY_4": 6}:
            raise SystemExit("FAIL: Battle 107 guide party sizes stale")
    print("PASS: Battle 107 John/Jay three-pair anniversary family is source-closed")


if __name__ == "__main__":
    main()
