#!/usr/bin/env python3
"""Generate and verify Battle 123, Flannery's main fight and rematch family."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
from pathlib import Path

import verdant_battle_set_presets as presets
import verdant_custom_teams as custom
import verdant_doubles_conversion as doubles
import verdant_team_polish as polish
import verdant_team_quality_audit as quality


ROOT = Path(__file__).resolve().parents[1]
DESIGNS = ROOT / "docs/verdant_bespoke_battle_designs.json"
LEDGER = ROOT / "docs/verdant_battle_experience_ledger.json"
SEQUENCE = ROOT / "docs/verdant_battle_sequence.json"
OS_PATH = ROOT / "docs/emerald_champions_battle_design_operating_system.json"
CORPUS = ROOT / "docs/competitive_team_index.jsonl"
GYM_ANCHORS = ROOT / "docs/emerald_champions_gym_anchor_designs.json"
REMATCH_AUDIT = ROOT / "docs/emerald_champions_rematch_family_audit.json"

MAIN_TEAM = [
    {"level": 1, "species": "SPECIES_TORKOAL", "item": "ITEM_EJECT_BUTTON", "ability_slot": 1, "spread": "SPREAD_31_IV_HP_DEF_BOLD", "moves": ["MOVE_ERUPTION", "MOVE_BODY_PRESS", "MOVE_YAWN", "MOVE_PROTECT"]},
    {"level": 1, "species": "SPECIES_LILLIGANT", "item": "ITEM_FOCUS_SASH", "ability_slot": 0, "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID", "moves": ["MOVE_AFTER_YOU", "MOVE_HELPING_HAND", "MOVE_SOLAR_BEAM", "MOVE_PROTECT"]},
    {"level": 2, "species": "SPECIES_DELPHOX", "item": "ITEM_MENTAL_HERB", "ability_slot": 1, "spread": "SPREAD_31_IV_HP_SPATK_QUIET", "moves": ["MOVE_TRICK_ROOM", "MOVE_HEAT_WAVE", "MOVE_SHADOW_BALL", "MOVE_WILL_O_WISP"]},
    {"level": 2, "species": "SPECIES_SKELEDIRGE", "item": "ITEM_THROAT_SPRAY", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_SPATK_QUIET", "moves": ["MOVE_TORCH_SONG", "MOVE_SHADOW_BALL", "MOVE_HYPER_VOICE", "MOVE_PROTECT"]},
    {"level": 3, "species": "SPECIES_HEATRAN", "item": "ITEM_AIR_BALLOON", "ability_slot": 0, "spread": "SPREAD_31_IV_HP_SPATK_MODEST", "moves": ["MOVE_MAGMA_STORM", "MOVE_FLASH_CANNON", "MOVE_EARTH_POWER", "MOVE_PROTECT"]},
    {"level": 4, "species": "SPECIES_EMBOAR", "item": "ITEM_EMBOARITE", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_HEAT_CRASH", "MOVE_CLOSE_COMBAT", "MOVE_HIGH_HORSEPOWER", "MOVE_WILD_CHARGE"]},
]

TEAMS = {"TRAINER_FLANNERY_1": MAIN_TEAM}

REFERENCES = [
    "vgc:regional-vancouver-bc-2023",
    "vgc:laic-2017",
    "showdown:gen9randomdoublesbattle:019",
    "smogon:gen9ou:004",
    "showdown:gen6randomdoublesbattle:010",
    "showdown:gen9championsrandomdoublesbattle:027",
]

REMATCH_IDS = [
    "TRAINER_FLANNERY_2",
    "TRAINER_FLANNERY_3",
    "TRAINER_FLANNERY_4",
    "TRAINER_FLANNERY_5",
]

REMATCH_DIGESTS = {
    "TRAINER_FLANNERY_2": "c9ad2845ef8a493ab2e98b837300d91f6deeee3870b50ae2229271956c611bbb",
    "TRAINER_FLANNERY_3": "d6cfec19b75ea1b7771eaeefcddf4f834fa570e48d1397c9bcc3644666034f67",
    "TRAINER_FLANNERY_4": "42e176529e818002336731a3ebe675c994443992119b85a87922e2fa2e701837",
    "TRAINER_FLANNERY_5": "7e96025b24d2f4439cd346daeb076d841d032101bc23744859c910e47b31e879",
}

NEXT = {
    "index": 124,
    "encounter_id": "BATTLE_124_MT_CHIMNEY_SHELBY",
    "location": "MtChimney",
    "category": "optional post-crisis Mt. Chimney Picnicker rematch family",
    "status": "next",
    "strict_cap": 45,
    "trainer_ids": ["TRAINER_SHELBY_1"],
    "access_note": "Shelby is the first unclosed post-crisis Mt. Chimney trainer at (16,18), available after the summit story and before Norman.",
}


def rematch_family() -> dict:
    return json.loads(REMATCH_AUDIT.read_text())["families"]["FLANNERY"]


def anchor() -> dict:
    return json.loads(GYM_ANCHORS.read_text())["designs"]["LAVARIDGE_GYM_FLANNERY"]


def design() -> dict:
    gym = anchor()
    rematches = rematch_family()
    return {
        "guide_order": 123,
        "trainer_ids": ["TRAINER_FLANNERY_1", *REMATCH_IDS],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": "Required fourth-Badge boss after Mt. Chimney, Jagged Pass, every Lavaridge Gym trainer, and full preparation access.",
        "runtime_branches": [
            "Required main-story double: TRAINER_FLANNERY_1 at cap 40, authored levels 41-44.",
            "Postgame singles without Legendaries: TRAINER_FLANNERY_2.",
            "Postgame singles with Legendaries: TRAINER_FLANNERY_3.",
            "Postgame doubles with Legendaries: TRAINER_FLANNERY_4.",
            "Postgame doubles without Legendaries: TRAINER_FLANNERY_5.",
            "The native rematch menu reaches all four choices and the daily flag is set only after the selected battle.",
        ],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 fully evolved badge boss",
            "effective_levels": "main 41/41/42/42/43/44; rematches use their postgame cap-relative source levels",
            "eligible_ratio": "6/6 main and 24/24 rematch slots",
            "mega_access": True,
            "status": "pass",
            "reason": "Every main and rematch member is fully evolved or single-stage; main-story Mega Emboar is the one protected transformation.",
        },
        "manual_quality": 10,
        "manual_difficulty": 10.0,
        "observed_difficulty": None,
        "rematch_targets": {
            record["trainer_id"]: {
                "format": record["format"],
                "source_quality": record["quality_score"],
                "target_difficulty": 9.0 if record["trainer_id"].endswith("_2") else 9.6 if record["trainer_id"].endswith("_3") else 9.8 if record["trainer_id"].endswith("_4") else 9.6,
                "observed_difficulty": None,
            }
            for record in rematches["records"]
        },
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": gym["competitive_research"]["candidates"],
            "decision": "Two tournament-winning Torkoal-Lilligant teams and four exact-role references establish the main fight. The four existing rematch records are retained as distinct source-authored formats and protected by exact digests.",
        },
        "competitive_references": gym["competitive_research"]["candidates"],
        "ordering": {
            "main_lead": ["SPECIES_TORKOAL", "SPECIES_LILLIGANT"],
            "main_source_order": [member["species"] for member in MAIN_TEAM],
            "main_formations": [
                ["SPECIES_TORKOAL", "SPECIES_LILLIGANT"],
                ["SPECIES_DELPHOX", "SPECIES_SKELEDIRGE"],
                ["SPECIES_HEATRAN"],
                ["SPECIES_EMBOAR"],
            ],
            "reason": "The protected lead manipulates Torkoal's move order; the reserve selector keeps fast heat intact while a lead survives, pairs the slow Fire users after a double KO, bridges through Heatran, and exposes Mega Emboar last when practical.",
        },
        "team_intent": "Drought Torkoal and Chlorophyll Lilligant threaten After You or Helping Hand Eruption without sleep variance. Delphox may reverse visible speed for Skeledirge's finite Torch Song/Throat Spray snowball. Air Balloon Heatran seals one defensive exit before Mega Emboar changes the final damage category and carries explicit Water/Ground coverage.",
        "primary_player_question": gym["identity"]["primary_player_question"],
        "intended_counterplay": "Weather replacement or Cloud Nine, immediate Torkoal chip, Fake Out, Taunt, Encore, priority, double targeting, Wide Guard, Protect, Flash Fire, Water Absorb, Thick Fat, Water/Ground/Rock, Balloon removal, Haze, Unaware, phazing, speed reversal, and mixed bulk all work. Lilligant has no sleep move, Trick Room can be denied or exploited, Heatran's Balloon is public, and Mega Emboar has no Protect or setup.",
        "first_loss_lesson": "The boss is about temperature timing, not simply Water damage: weaken Eruption before After You, decide whether Trick Room helps or hurts, break Heatran's Balloon before committing Ground pressure, and save a physical answer for Mega Emboar.",
        "bespoke_ai": "Main-story Flannery uses HP-aware Eruption, partner After You/Helping Hand scoring, conditional Trick Room, field control, smart switching, and a source-specific formation selector with simultaneous-replacement fallbacks. Eruption now reads attacker HP rather than target HP. Mega, Balloon, Magma Storm, Throat Spray, Unaware, and ordinary move targeting remain native. Rematch singles and doubles have truthful format-specific AI; the legendary doubles branch coordinates Torkoal's Explosion only when the partner state is safe.",
        "uniqueness": "This is the first full weather boss after nine weatherless recent encounters and the first team to make the same slow Fire identity move first through After You or last through Trick Room. Five of six main members are Fire type. No main species collides with another protected Gym or League anchor, and the rematches deliberately evolve Flannery's known signatures rather than globally deduplicating them.",
        "story_logic": "Flannery stops imitating a generic stern Leader and frames inherited strength through her own timing discipline. The Gym guide names the actual lead, reversal, Balloon, and exposed Mega. The Heat Badge, Strength access, Arcanine registration, Magmarizer reward, Petalburg handoff, and all native story flags remain intact.",
        "reward_logic": "The required win grants the Heat Badge, Strength field access, Flannery's Arcanine registration, Match Call progression, and a retry-safe Magmarizer. Rematches grant only ordinary battle rewards and set the daily rematch flag after victory.",
        "campaign_reservations": gym["campaign_reservations"],
        "rematch_family": rematches,
        "author_self_check": gym["author_self_check"],
        "closure": "Battle 123 is source-closed at quality 10 and target difficulty 10: the protected main team, levels, lead, formation selector, current-HP Eruption AI, conditional Trick Room, one Mega, six real references, exact dialogue and guide hints, story rewards, and four genuinely reachable format/legend rematches are proven. Runtime remains unplayed.",
    }


def ledger_entry() -> dict:
    return {
        "index": 123,
        "encounter_id": "BATTLE_123_LAVARIDGE_GYM_FLANNERY",
        "identity": {"location": "LavaridgeTown_Gym_1F", "category": "required Heat Badge boss and rematch family", "format": "main double plus mixed rematches", "strict_cap": 40, "memory_hook": "The slowest flame moves first, the furnace reverses, one exit seals, and Mega Emboar ends the timing exam physically."},
        "primary_player_question": "Can HP-sensitive fast sun and one slow reversal be solved without spending the Water/Ground and mixed-bulk answers needed for Heatran and Mega Emboar?",
        "tempo": "After You/Helping Hand Eruption lead, conditional Fire-native Trick Room, Balloon trap bridge, physical Mega finale.",
        "pressure_sources": ["Drought high-HP Eruption", "Chlorophyll After You", "Helping Hand/Solar Beam", "Pyromancy Trick Room hinge", "Unaware Torch Song/Throat Spray", "Air Balloon Magma Storm Heatran", "Mega Emboar mixed coverage"],
        "intentional_opening": "Torkoal and Lilligant lead; After You and Eruption are strongly valued but never scripted.",
        "intentional_weakness": "Public weather and Balloon, no sleep/redirection, one denyable room, shared Water/Ground/Rock seams, finite sound boost, no Mega setup or Protect.",
        "first_loss_lesson": "Chip Torkoal before After You, control whether Trick Room is favorable, pop Heatran's Balloon, and preserve a physical answer for Emboar.",
        "revealed_information": ["cap 40", "required double", "levels 41-44", "Drought", "After You", "Eruption", "conditional Trick Room", "Pyromancy", "Throat Spray", "Air Balloon", "Magma Storm", "Mega Emboar", "Heat Badge/Magmarizer"],
        "counterplay_classes": ["weather replacement/Cloud Nine/HP chip", "Fake Out/Taunt/Encore/priority/focus", "Wide Guard/Protect/Flash Fire/Water Absorb/Thick Fat", "Water/Ground/Rock/Balloon removal", "Haze/Unaware/phazing/speed reversal/mixed bulk"],
        "target_difficulty": 10.0,
        "difficulty_rationale": "Six optimized cap+1 to +4 sets, two visible speed modes, high-HP spread pressure, trap positioning, mixed damage, and one Mega create the required badge apex while broad public counterplay remains.",
        "tuning_knob": "Lower Emboar +4 first, then Heatran +3, then Skeledirge +2; preserve all species, formations, items, moves, and AI logic.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["lavaridge-gym", "badge-boss", "thermal-timing", "torkoal-lilligant", "after-you-eruption", "drought", "conditional-trick-room", "delphox", "skeledirge", "heatran", "magma-storm", "mega-emboar", "five-of-six-fire", "one-mega", "one-legendary", "mixed-rematch-family"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Two tournament winners plus four exact-role Showdown/Smogon/Champions references."},
        "author_self_check": anchor()["author_self_check"],
    }


def payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_123_LAVARIDGE_GYM_FLANNERY"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 123] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])
    sequence = json.loads(SEQUENCE.read_text())
    for row in sequence["entries"]:
        if row["index"] == 123:
            row.update({
                "category": "required thermal-timing Heat Badge boss and rematch family",
                "trainer_ids": ["TRAINER_FLANNERY_1", *REMATCH_IDS],
                "access_note": "Flannery at (13,9) owns the required main double and a postgame menu with singles/doubles and Legendary/no-Legendary branches.",
            })
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 124] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 123 else "next" if row["index"] == 124 else "queued"
    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update({
        "closed_encounters": 123,
        "next_index": 124,
        "next_encounter_id": NEXT["encounter_id"],
        "canonical_sequence_groups": 124,
        "physical_encounter_groups": 522,
        "unordered_physical_groups": 398,
    })
    return designs, ledger, sequence, operating_system


def rematch_digest(record: dict) -> str:
    payload = {
        "trainer_id": record["trainer_id"],
        "format": record["format"],
        "level_offsets": record["level_offsets"],
        "team": record["team"],
    }
    return hashlib.sha256(json.dumps(payload, sort_keys=True, separators=(",", ":")).encode()).hexdigest()


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    blocks = doubles.trainer_blocks(trainers)
    main_block = blocks["TRAINER_FLANNERY_1"].group(0)
    actual = [
        polish.parse_entry(entry)
        for entry in custom.party_entries(doubles.party_match(parties, doubles.party_name(main_block)).group(2))
    ]
    if actual != MAIN_TEAM:
        raise SystemExit("FAIL Battle 123 main source party")
    for fragment in (
        ".doubleBattle = TRUE",
        "AI_FLAG_HP_AWARE",
        "AI_FLAG_HELP_PARTNER",
        "AI_FLAG_SMART_SWITCHING",
        "AI_FLAG_COMBO_SETUP",
        "AI_FLAG_SPEED_CONTROL",
        "AI_FLAG_FIELD_CONTROL",
    ):
        if fragment not in main_block:
            raise SystemExit(f"FAIL Battle 123 main flag {fragment}")

    local_dex = presets.LocalDex()
    ability_slots = doubles.base_ability_slots()
    for member in MAIN_TEAM:
        illegal = [move for move in member["moves"] if move not in local_dex.legal_moves(member["species"])]
        if illegal or member["ability_slot"] >= len(ability_slots[member["species"]]):
            raise SystemExit(f"FAIL Battle 123 legality {member['species']}: {illegal}")

    team_report = {team["trainer_id"]: team for team in quality.audit()["teams"]}
    audit_records = {record["trainer_id"]: record for record in rematch_family()["records"]}
    expected_formats = ["single", "single", "double", "double"]
    for trainer_id, expected_format in zip(REMATCH_IDS, expected_formats):
        source_team = team_report[trainer_id]
        record = audit_records[trainer_id]
        if source_team["format"] != expected_format or record["format"] != expected_format:
            raise SystemExit(f"FAIL Battle 123 rematch format {trainer_id}")
        if rematch_digest(record) != REMATCH_DIGESTS[trainer_id]:
            raise SystemExit(f"FAIL Battle 123 rematch source digest {trainer_id}")

    ai_main = (ROOT / "src/battle_ai_main.c").read_text()
    for fragment in (
        "GetHealthPercentage(battlerAtk) < 50",
        "effect == EFFECT_AFTER_YOU",
        "AI_FLAG_SPEED_CONTROL",
    ):
        if fragment not in ai_main:
            raise SystemExit(f"FAIL Battle 123 AI {fragment}")
    ai_switch = (ROOT / "src/battle_ai_switch_items.c").read_text()
    for fragment in (
        "GetFlanneryFormationSwitch",
        "gTrainerBattleOpponent_A != TRAINER_FLANNERY_1",
        "SPECIES_TORKOAL, SPECIES_LILLIGANT",
        "SPECIES_DELPHOX, SPECIES_SKELEDIRGE",
        "SPECIES_HEATRAN",
        "SPECIES_EMBOAR",
    ):
        if fragment not in ai_switch:
            raise SystemExit(f"FAIL Battle 123 formation selector {fragment}")

    scripts = (ROOT / "data/maps/LavaridgeTown_Gym_1F/scripts.inc").read_text()
    for fragment in (
        "multichoice 17, 6, MULTI_REMATCH_BATTLE_MODE, 0",
        "case 0, LavaridgeTown_Gym_1F_EventScript_SetSingles",
        "case 1, LavaridgeTown_Gym_1F_EventScript_SetDoubles",
        "TRAINER_FLANNERY_2",
        "TRAINER_FLANNERY_3",
        "TRAINER_FLANNERY_4",
        "TRAINER_FLANNERY_5",
    ):
        if fragment not in scripts:
            raise SystemExit(f"FAIL Battle 123 rematch routing {fragment}")
    guide_dialogue = scripts.split("LavaridgeTown_Gym_1F_Text_GymGuideAdvice2:", 1)[1].split("LavaridgeTown_Gym_1F_Text_GymGuidePostVictory:", 1)[0]
    leader_dialogue = scripts.split("LavaridgeTown_Gym_1F_Text_FlanneryIntro:", 1)[1].split("LavaridgeTown_Gym_1F_Text_ReceivedHeatBadge:", 1)[0]
    post_dialogue = scripts.split("LavaridgeTown_Gym_1F_Text_FlanneryPostBattle:", 1)[1].split("LavaridgeTown_Gym_1F_Text_GymStatue:", 1)[0]
    dialogue = guide_dialogue + leader_dialogue + post_dialogue
    for cue in ("After You", "Eruption", "Trick Room", "Heatran", "Mega Emboar", "timing", "Petalburg"):
        if cue not in dialogue:
            raise SystemExit(f"FAIL Battle 123 dialogue {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL Battle 123 dialogue width {visible!r}")

    gym = anchor()
    if gym["status"]["source"] != "source-closed" or gym["verification"]["source_implementation"] != "pass":
        raise SystemExit("FAIL Battle 123 Gym anchor status")
    expected_anchor_team = [
        {
            "level": member["level_offset"],
            "species": member["species"],
            "item": member["item"],
            "ability_slot": member["ability_slot"],
            "spread": member["spread"],
            "moves": member["moves"],
        }
        for member in gym["team"]
    ]
    if expected_anchor_team != MAIN_TEAM:
        raise SystemExit("FAIL Battle 123 protected anchor mismatch")
    reference_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference not in reference_ids for reference in REFERENCES):
        raise SystemExit("FAIL Battle 123 corpus references")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    expected = payloads()
    paths = (DESIGNS, LEDGER, SEQUENCE, OS_PATH)
    serialized = [json.dumps(payload, indent=2, ensure_ascii=False) + "\n" for payload in expected]
    if args.write:
        for path, text in zip(paths, serialized):
            path.write_text(text)
    if args.check:
        for path, text in zip(paths, serialized):
            if path.read_text() != text:
                raise SystemExit(f"FAIL Battle 123 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())["entries"]
        rows = [row for row in guide if row["trainerId"] in {"TRAINER_FLANNERY_1", *REMATCH_IDS}]
        if len(rows) != 5 or any(row["designStatus"] != "closed" or row["partySize"] != 6 for row in rows):
            raise SystemExit("FAIL Battle 123 guide rows")
    print("PASS: Battle 123 Flannery main fight and four rematch branches are source-closed")


if __name__ == "__main__":
    main()
