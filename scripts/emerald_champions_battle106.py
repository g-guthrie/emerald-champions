#!/usr/bin/env python3
"""Generate and verify Battle 106, Nicolas's complete dragon-discipline rematch family."""

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


EXEGGUTOR = mon(1, "SPECIES_EXEGGUTOR_ALOLAN", "ITEM_SITRUS_BERRY", 2, "SPREAD_31_IV_HP_SPATK_MODEST", ["MOVE_LEAF_STORM", "MOVE_DRAGON_PULSE", "MOVE_FLAMETHROWER", "MOVE_PROTECT"])
DRACOZOLT = mon(2, "SPECIES_DRACOZOLT", "ITEM_WIDE_LENS", 1, "SPREAD_31_IV_ATK_SPEED_JOLLY", ["MOVE_BOLT_BEAK", "MOVE_DRAGON_CLAW", "MOVE_HIGH_HORSEPOWER", "MOVE_PROTECT"])
NAGANADEL = mon(3, "SPECIES_NAGANADEL", "ITEM_WHITE_HERB", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", ["MOVE_SLUDGE_BOMB", "MOVE_DRACO_METEOR", "MOVE_FLAMETHROWER", "MOVE_PROTECT"])
ROARING_MOON = mon(4, "SPECIES_ROARING_MOON", "ITEM_BOOSTER_ENERGY", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", ["MOVE_DRAGON_DANCE", "MOVE_THROAT_CHOP", "MOVE_DRAGON_CLAW", "MOVE_PROTECT"])
DRAGALGE = mon(3, "SPECIES_DRAGALGE", "ITEM_BLACK_SLUDGE", 2, "SPREAD_31_IV_HP_SPATK_MODEST", ["MOVE_HAZE", "MOVE_DRACO_METEOR", "MOVE_SLUDGE_BOMB", "MOVE_PROTECT"])
REGIDRAGO = mon(4, "SPECIES_REGIDRAGO", "ITEM_DRAGON_FANG", 0, "SPREAD_31_IV_HP_SPATK_MODEST", ["MOVE_DRAGON_ENERGY", "MOVE_DRACO_METEOR", "MOVE_ANCIENT_POWER", "MOVE_PROTECT"])


def at_level(member: dict, level: int) -> dict:
    value = dict(member)
    value["level"] = level
    value["moves"] = list(member["moves"])
    return value


TEAMS = {
    "TRAINER_NICOLAS_1": [EXEGGUTOR, DRACOZOLT, NAGANADEL, ROARING_MOON],
    "TRAINER_NICOLAS_2": [DRAGALGE, EXEGGUTOR, DRACOZOLT, at_level(NAGANADEL, 4)],
    "TRAINER_NICOLAS_3": [at_level(NAGANADEL, 1), DRAGALGE, at_level(ROARING_MOON, 2), REGIDRAGO],
    "TRAINER_NICOLAS_4": [EXEGGUTOR, DRACOZOLT, at_level(NAGANADEL, 2), at_level(ROARING_MOON, 3), DRAGALGE, REGIDRAGO],
}

REFERENCES = [
    "smogon:gen8nu:001",
    "showdown:gen8randombattle:025",
    "showdown:gen8randomdoublesbattle:016",
    "showdown:gen9randomdoublesbattle:028",
]

NEXT = {
    "index": 107,
    "encounter_id": "BATTLE_107_METEOR_FALLS_JOHN_JAY",
    "location": "MeteorFalls_1F_2R",
    "category": "optional Expert couple four-record Match Call family",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_JOHN_AND_JAY_1", "TRAINER_JOHN_AND_JAY_2", "TRAINER_JOHN_AND_JAY_3", "TRAINER_JOHN_AND_JAY_4"],
    "access_note": (
        "John and Jay stand together at (6,12) and (7,12). Either physical script owns the same initial couple record, "
        "registration, and three sequential Match Call rematches."
    ),
}


def design() -> dict:
    return {
        "guide_order": 106,
        "trainer_ids": list(TEAMS),
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Optional one-tile Dragon Tamer encounter in Meteor Falls 1F 2R immediately after Courtney's required impact "
            "multi. The initial record is available at cap 40; all three Match Call rematches require five badges and are "
            "earliest at cap 45, while remaining cap-relative if fought later."
        ),
        "runtime_branches": [
            "NICOLAS_1: guarded four-member double at cap 40.",
            "NICOLAS_2: guarded four-member rematch, earliest cap 45.",
            "NICOLAS_3: guarded four-member rare-dragon rematch.",
            "NICOLAS_4: repeatable six-member final rematch.",
        ],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 single-stage dragon lesson and cap-45+ mature rematches",
            "effective_levels": "initial 41-44; rematches earliest 46-49; final earliest 46/47/47/48/48/49",
            "eligible_ratio": "18/18",
            "mega_access": True,
            "status": "pass",
            "reason": (
                "Alolan Exeggutor evolves by stone; Dracozolt, Roaring Moon, and Regidrago are single-stage; Naganadel evolves "
                "from Poipole after Dragon Pulse. Dragalge evolves at 48 and appears only in cap-45 rematches at level 48."
            ),
        },
        "manual_quality": 10,
        "manual_difficulty": 9.3,
        "rematch_difficulty": {"TRAINER_NICOLAS_2": 9.4, "TRAINER_NICOLAS_3": 9.6, "TRAINER_NICOLAS_4": 9.8},
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {"reference_id": REFERENCES[0], "decision": "Alolan Exeggutor role selected", "reason": "Published NU Harvest endurance validates the slow root discipline without importing Teleport."},
                {"reference_id": REFERENCES[1], "decision": "Dracozolt role adapted", "reason": "Generated Hustle Bolt Beak pressure is retained, but Wide Lens replaces Choice speed so the family adds no new speed-control module."},
                {"reference_id": REFERENCES[2], "decision": "Dragalge role selected", "reason": "The exact doubles set validates Adaptability special pressure and Protect; Haze replaces generic Life Orb coverage to create state discipline."},
                {"reference_id": REFERENCES[3], "decision": "Roaring Moon role adapted", "reason": "Generated Protosynthesis offense validates the rare physical dragon; Booster Energy and one Dragon Dance replace Tailwind."},
            ],
            "decision": (
                "All 1005 indexed references were available. Four exact-species records and the authored Alola/Galar set "
                "reviews support all six disciplines; no complete donor team matches the rematch progression."
            ),
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Harvest Sitrus Alolan Exeggutor keeps Leaf/Dragon/Fire special coverage and Protect."},
            {"reference_id": REFERENCES[1], "adaptation": "Wide Lens makes Hustle Dracozolt accurate enough to use Bolt Beak, Dragon Claw, and High Horsepower without a Choice lock."},
            {"reference_id": REFERENCES[2], "adaptation": "Black Sludge Adaptability Dragalge keeps Draco/Poison pressure and adds Haze as a public two-sided reset."},
            {"reference_id": REFERENCES[3], "adaptation": "Booster Energy Roaring Moon becomes the family's only manual setup clock with Dragon Dance and direct Dark/Dragon coverage."},
            {"source": "docs/battle_set_reviews/080_alola.json", "adaptation": "Naganadel keeps Beast Boost, White Herb Draco Meteor, Sludge Bomb, Fire coverage, and Protect."},
            {"source": "docs/battle_set_reviews/085_galar.json", "adaptation": "Regidrago keeps Dragon's Maw, Dragon Fang, full-health Dragon Energy, Draco Meteor, and Protect with Ancient Power as legal coverage."},
        ],
        "ordering": {
            "TRAINER_NICOLAS_1": {"lead": ["SPECIES_EXEGGUTOR_ALOLAN", "SPECIES_DRACOZOLT"], "reserves": ["SPECIES_NAGANADEL", "SPECIES_ROARING_MOON"]},
            "TRAINER_NICOLAS_2": {"lead": ["SPECIES_DRAGALGE", "SPECIES_EXEGGUTOR_ALOLAN"], "reserves": ["SPECIES_DRACOZOLT", "SPECIES_NAGANADEL"]},
            "TRAINER_NICOLAS_3": {"lead": ["SPECIES_NAGANADEL", "SPECIES_DRAGALGE"], "reserves": ["SPECIES_ROARING_MOON", "SPECIES_REGIDRAGO"]},
            "TRAINER_NICOLAS_4": {"lead": ["SPECIES_EXEGGUTOR_ALOLAN", "SPECIES_DRACOZOLT"], "reserves": ["SPECIES_NAGANADEL", "SPECIES_ROARING_MOON", "SPECIES_DRAGALGE", "SPECIES_REGIDRAGO"]},
        },
        "team_intent": (
            "The initial four teach Harvest endurance, Wide Lens Hustle accuracy, White Herb Beast Boost special pressure, "
            "and Booster Energy physical setup. Rematch one introduces level-48 Adaptability Dragalge and Haze without adding "
            "speed control. Rematch two pairs the two Poison/Dragon specialists before Roaring Moon and full-health Regidrago. "
            "The final six combines all disciplines. Haze is intentionally two-sided and can erase Roaring Moon or Beast Boost, "
            "so native AI must time it rather than treat it as unconditional upside."
        ),
        "intended_counterplay": (
            "Fairy types and Fairy coverage are excellent but face Naganadel/Dragalge Poison STAB. Ice, Dragon, Ground, "
            "Psychic, Steel, Fighting, and focused neutral damage provide broad alternatives. Remove or suppress items to weaken "
            "Harvest, Hustle accuracy, White Herb, Booster Energy, Black Sludge, and Dragon Fang; pressure Dracozolt before Bolt "
            "Beak, deny Roaring Moon's one setup turn, exploit Haze timing, and chip Regidrago before Dragon Energy. There is no "
            "weather, room, Tailwind, priority package, trap, sleep, screen, hazard stack, Mega, or forced turn."
        ),
        "bespoke_ai": (
            "Every record uses smart switching, partner awareness, and HP awareness; Haze records add Field Control. Existing "
            "doubles AI rejects Haze when its partner has positive stages, uses Dragon Dance only with visible survival value, "
            "scores Protect and HP-sensitive Dragon Energy normally, respects White Herb and Booster Energy, and chooses typed "
            "coverage from the board. No action, target, switch, or lead wave is forced."
        ),
        "uniqueness": (
            "All six species are new to the first 105 physical encounters and absent from the protected exact anchor teams. "
            "The family immediately removes inherited Noivern, Turtonator, Altaria, Hydreigon, and Dragonite repetitions. It is "
            "the first Dragon rematch family based on six independent training disciplines rather than Tailwind or generic Dragon spam."
        ),
        "story_logic": (
            "Nicolas remains a Dragon Tamer training where the Champion visits. Intro and post-battle text now name the exact "
            "initial mechanics; rematch text truthfully introduces Haze and Regidrago and warns that Haze can erase Nicolas's "
            "own boosts. Both initial and rematch scripts are double-safe; registration and four-record routing remain native."
        ),
        "reward_logic": "EXP and prize money on every record; Match Call registration is the only progression reward.",
        "campaign_reservations": {
            "spends": ["Nicolas six-discipline dragon family", "first Alolan Exeggutor", "first Dracozolt", "first Naganadel", "first Roaring Moon", "first Dragalge", "first Regidrago"],
            "preserves": ["Drake's protected League dragons", "Wattson's Ampharos", "Shelly's Goodra", "Maxie's Mega Flygon", "all Dragon Megas", "Zekrom/Zygarde/Kyurem/Rayquaza"],
            "repeat_rule": "These six species may repeat inside Nicolas only; later Dragon trainers must use a different central question and should not recreate the six-discipline final.",
        },
        "author_self_check": {
            "strongest_part": "Every rematch changes which discipline leads while avoiding the speed-control and premium-item density of the previous ten encounters.",
            "weakest_link": "Four of six species primarily use Dragon special or physical damage, so Fairy pressure is intentionally excellent. Dual Poison coverage, Grass/Electric/Ground/Fire/Dark coverage, Haze, mixed categories, six-body depth, and +1 to +4 levels stop one Fairy from solving every board."
        },
        "closure": (
            "Battle 106's complete family is source-closed at quality 10: targets 9.3/9.4/9.6/9.8; four guarded doubles; "
            "18 legal cap-relative slots; six fresh unreserved species; distinct per-party items; exact Match Call routing; four "
            "indexed references plus two authored reviews; native-width dialogue; broad counterplay; and zero reward debt. Runtime remains unplayed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 106,
        "encounter_id": "BATTLE_106_METEOR_FALLS_NICOLAS",
        "identity": {"location": "MeteorFalls_1F_2R", "category": "optional Dragon Tamer four-record Match Call family", "format": "four guarded doubles", "strict_cap": 40, "memory_hook": "Nicolas rotates six dragon disciplines—Harvest, Hustle, Beast Boost, Protosynthesis, Adaptability/Haze, and Dragon Energy—without a speed field."},
        "primary_player_question": "Can the player identify which dragon discipline is active, exploit its item or timing seam, and preserve Fairy or mixed-type answers for the later rare dragons?",
        "tempo": "Four cap-relative doubles: four-discipline introduction, Adaptability rematch, rare-dragon rematch, then six-discipline final.",
        "pressure_sources": ["Harvest Sitrus Alolan Exeggutor", "Wide Lens Hustle Dracozolt", "White Herb Beast Boost Naganadel", "Booster Energy Roaring Moon", "Black Sludge Adaptability Dragalge with two-sided Haze", "Dragon Fang Regidrago Dragon Energy"],
        "intentional_opening": "Every record has an exact lead and independent reserves; no field or first action is forced.",
        "intentional_weakness": "Strong Fairy seam, no speed field or priority, item dependence, one setup user, two-sided Haze, HP-sensitive Dragon Energy, and no sleep/trap/screens/hazard stack/Mega.",
        "first_loss_lesson": "Do not treat every Dragon alike: attack the active item and timing rule, save Poison-resistant Fairy pressure, and chip Regidrago before its full-health cannon lands.",
        "revealed_information": ["initial cap 40", "five-badge rematches", "four guarded doubles", "levels cap+1 to +4", "six fresh species", "six ability/item disciplines", "two-sided Haze", "no speed field", "no Mega/reward"],
        "counterplay_classes": ["Fairy/Ice/Dragon/Ground/Psychic/Steel/Fighting", "mixed categories", "item removal/suppression", "setup denial", "Haze timing", "Dracozolt speed pressure", "Regidrago chip", "focus and Protect sequencing"],
        "target_difficulty": 9.3,
        "difficulty_rationale": "The initial four optimized levels 41-44 are rare and complete but expose a major Fairy and speed seam. Rematches add mature Dragalge, Regidrago, reordered leads, and final six-body depth without hiding the answers.",
        "tuning_knob": "Tune final Regidrago +4 to +3 first, then Roaring Moon/Dragalge +3 to +2; preserve all six disciplines and record routing.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["meteor-falls-rematch-family", "four-guarded-doubles", "dragon-disciplines", "exeggutor-alola", "dracozolt", "naganadel", "roaring-moon", "dragalge", "regidrago", "harvest", "hustle-wide-lens", "beast-boost-white-herb", "booster-energy", "adaptability-haze", "dragon-energy", "six-fresh-species", "no-speed-field", "no-priority", "no-mega"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Four indexed exact-species references plus authored Alola/Galar set reviews."},
        "author_self_check": {"strongest_part": "The source's generic repeated dragons become six memorable disciplines with materially different public state.", "weakest_link": "Fairy pressure is broad by design; Poison, coverage, mixed axes, ordering, depth, and levels make sequencing matter."},
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_106_METEOR_FALLS_NICOLAS"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 106] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])

    sequence = json.loads(SEQUENCE.read_text())
    for row in sequence["entries"]:
        if row["index"] == 106:
            row.update({
                "category": "optional Dragon Tamer four-record Match Call family",
                "trainer_ids": list(TEAMS),
                "access_note": "Nicolas faces down at (13,2) with one-tile sight. One physical record owns his initial double and all three Match Call rematches.",
            })
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 107] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 106 else "next" if row["index"] == 107 else "queued"

    os_data = json.loads(OS_PATH.read_text())
    os_data["current_state"].update({
        "closed_encounters": 106,
        "next_index": 107,
        "next_encounter_id": NEXT["encounter_id"],
        "queued_sequence_entries": 0,
        "canonical_sequence_groups": 107,
        "physical_encounter_groups": 526,
        "unordered_physical_groups": 419,
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
            raise SystemExit(f"FAIL: Battle 106 source party differs for {trainer_id}")
        for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE"):
            if token not in block:
                raise SystemExit(f"FAIL: Battle 106 {trainer_id} missing {token}")
        if trainer_id != "TRAINER_NICOLAS_1" and "AI_FLAG_FIELD_CONTROL" not in block:
            raise SystemExit(f"FAIL: Battle 106 {trainer_id} missing Haze-aware field profile")
        if len({m["species"] for m in team}) != len(team) or len({m["item"] for m in team}) != len(team):
            raise SystemExit(f"FAIL: Battle 106 duplicate species/item in {trainer_id}")
        for member in team:
            illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
            if illegal:
                raise SystemExit(f"FAIL: Battle 106 illegal moves for {member['species']}: {illegal}")
            if member["ability_slot"] >= len(slots[member["species"]]):
                raise SystemExit(f"FAIL: Battle 106 invalid ability slot for {member['species']}")

    script = (ROOT / "data/maps/MeteorFalls_1F_2R/scripts.inc").read_text()
    if ("trainerbattle_double TRAINER_NICOLAS_1" not in script
      or "trainerbattle_rematch_double TRAINER_NICOLAS_1" not in script
      or "MeteorFalls_1F_2R_Text_NicolasNotEnoughMons:" not in script
      or "MeteorFalls_1F_2R_Text_NicolasRematchNotEnoughMons:" not in script):
        raise SystemExit("FAIL: Battle 106 double-safe routing missing")
    if "REMATCH(TRAINER_NICOLAS_1, TRAINER_NICOLAS_2, TRAINER_NICOLAS_3, TRAINER_NICOLAS_4, METEOR_FALLS_1F_2R)" not in (ROOT / "src/battle_setup.c").read_text():
        raise SystemExit("FAIL: Battle 106 rematch row drifted")
    obj = next(row for row in json.loads((ROOT / "data/maps/MeteorFalls_1F_2R/map.json").read_text())["object_events"] if row.get("script") == "MeteorFalls_1F_2R_EventScript_Nicolas")
    if (obj["x"], obj["y"], obj["movement_type"], str(obj["trainer_sight_or_berry_tree_id"])) != (13, 2, "MOVEMENT_TYPE_FACE_DOWN", "1"):
        raise SystemExit("FAIL: Battle 106 Nicolas geometry drifted")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]
    expected_manifest = {
        "TRAINER_NICOLAS_1": {"format": "double", "target_size": 4, "archetype": "Four dragon disciplines", "difficulty": 93, "partner_interaction": True, "level_offset": 3, "location": "Meteor Falls 1 F 2 R"},
        "TRAINER_NICOLAS_2": {"format": "double", "target_size": 4, "archetype": "Adaptability rematch", "difficulty": 94, "partner_interaction": True, "level_offset": 3, "location": "Meteor Falls 1 F 2 R"},
        "TRAINER_NICOLAS_3": {"format": "double", "target_size": 4, "archetype": "Rare dragon rematch", "difficulty": 96, "partner_interaction": True, "level_offset": 3, "location": "Meteor Falls 1 F 2 R"},
        "TRAINER_NICOLAS_4": {"format": "double", "target_size": 6, "archetype": "Six-discipline dragon final", "difficulty": 98, "partner_interaction": True, "level_offset": 3, "location": "Meteor Falls 1 F 2 R"},
    }
    for trainer_id, value in expected_manifest.items():
        if manifest[trainer_id] != value:
            raise SystemExit(f"FAIL: Battle 106 manifest stale for {trainer_id}")

    section = script.split("MeteorFalls_1F_2R_Text_NicolasIntro:", 1)[1].split("MeteorFalls_1F_2R_Text_JohnIntro:", 1)[0]
    for cue in ("Each dragon here trains its own way", "Harvest restores", "Wide Lens steadies", "Naganadel grows", "Roaring Moon", "Dragalge clears boosts with Haze", "Regidrago rewards full health", "Haze can erase our own boosts", "Dragon Energy weakens"):
        if cue not in section:
            raise SystemExit(f"FAIL: Battle 106 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', section):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 106 overlong dialogue: {visible}")

    ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 106 competitive reference missing")


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
                raise SystemExit(f"FAIL: Battle 106 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())["entries"]
        entries = [row for row in guide if row["trainerId"] in TEAMS]
        if len(entries) != 4 or any(row["designStatus"] != "closed" or row["format"] != "double" for row in entries):
            raise SystemExit("FAIL: Battle 106 guide stale")
        if {row["trainerId"]: row["partySize"] for row in entries} != {"TRAINER_NICOLAS_1": 4, "TRAINER_NICOLAS_2": 4, "TRAINER_NICOLAS_3": 4, "TRAINER_NICOLAS_4": 6}:
            raise SystemExit("FAIL: Battle 106 guide party sizes stale")
    print("PASS: Battle 106 Nicolas six-discipline rematch family is source-closed")


if __name__ == "__main__":
    main()
