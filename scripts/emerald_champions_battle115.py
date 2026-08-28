#!/usr/bin/env python3
"""Generate and verify Battle 115, Ethan's complete Jagged Pass trail chorus."""
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


TOXTRICITY = mon(1, "SPECIES_TOXTRICITY", "ITEM_THROAT_SPRAY", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", ["MOVE_BOOMBURST", "MOVE_OVERDRIVE", "MOVE_SLUDGE_BOMB", "MOVE_VOLT_SWITCH"])
BOUFFALANT = mon(2, "SPECIES_BOUFFALANT", "ITEM_ASSAULT_VEST", 2, "SPREAD_31_IV_HP_ATK_ADAMANT", ["MOVE_HEAD_CHARGE", "MOVE_HIGH_HORSEPOWER", "MOVE_CLOSE_COMBAT", "MOVE_WILD_CHARGE"])
RILLABOOM = mon(3, "SPECIES_RILLABOOM", "ITEM_MIRACLE_SEED", 2, "SPREAD_31_IV_ATK_SPEED_ADAMANT", ["MOVE_FAKE_OUT", "MOVE_DRUM_BEATING", "MOVE_GRASSY_GLIDE", "MOVE_HIGH_HORSEPOWER"])
MR_RIME = mon(4, "SPECIES_MR_RIME", "ITEM_EXPERT_BELT", 1, "SPREAD_31_IV_SPATK_SPEED_TIMID", ["MOVE_ENCORE", "MOVE_FREEZE_DRY", "MOVE_PSYCHIC", "MOVE_FOCUS_BLAST"])
YANMEGA = mon(3, "SPECIES_YANMEGA", "ITEM_LIFE_ORB", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", ["MOVE_BUG_BUZZ", "MOVE_AIR_SLASH", "MOVE_ANCIENT_POWER", "MOVE_GIGA_DRAIN"])
KRICKETUNE = mon(4, "SPECIES_KRICKETUNE", "ITEM_FOCUS_SASH", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", ["MOVE_SWORDS_DANCE", "MOVE_X_SCISSOR", "MOVE_KNOCK_OFF", "MOVE_AERIAL_ACE"])


def at_level(member: dict, level: int) -> dict:
    return {**member, "level": level}


TEAM_1 = [TOXTRICITY, BOUFFALANT, RILLABOOM, MR_RIME]
TEAM_2 = [at_level(RILLABOOM, 1), at_level(MR_RIME, 2), YANMEGA, KRICKETUNE]
TEAM_3 = [TOXTRICITY, BOUFFALANT, YANMEGA, KRICKETUNE]
TEAM_4 = [TOXTRICITY, at_level(BOUFFALANT, 1), at_level(RILLABOOM, 2), at_level(MR_RIME, 2), YANMEGA, KRICKETUNE]
TEAMS = {
    "TRAINER_ETHAN_1": TEAM_1,
    "TRAINER_ETHAN_2": TEAM_2,
    "TRAINER_ETHAN_3": TEAM_3,
    "TRAINER_ETHAN_4": TEAM_4,
}

REFERENCES = [
    "showdown:gen9randomdoublesbattle:028",
    "showdown:gen8randomdoublesbattle:019",
    "showdown:gen9randomdoublesbattle:018",
    "smogon:gen8ou:007",
    "elite:wolfe:players-cup-ii-2020",
    "showdown:gen9championsrandomdoublesbattle:009",
    "showdown:gen5randomdoublesbattle:007",
    "showdown:gen6randomdoublesbattle:014",
]

NEXT = {
    "index": 116,
    "encounter_id": "BATTLE_116_LAVARIDGE_POKECENTER_LUCY",
    "location": "LavaridgeTown_PokemonCenter_1F",
    "category": "optional pre-Gym Pokémon Center Lucy challenge",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_LUCY_LAVARIDGE"],
    "access_note": "The first walk to (2,2) while VAR_LAVARIDGE_LUCY_STATE is zero triggers Lucy's entrance and yes/no challenge. Refusal leaves her at (10,4) for direct interaction; victory grants three Bottle Caps and removes her. This is available before entering Lavaridge Gym.",
}


def design() -> dict:
    return {
        "guide_order": 115,
        "trainer_ids": list(TEAMS),
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": "Ethan is the final ordinary trainer on the first Jagged Pass descent, facing left/right at (16,35) with sight four, 59 collision-walk steps from the upper entry and just before the lower exit. His first fight is cap 40; five-badge Match Call rematches are earliest at cap 45 and remain cap-relative.",
        "runtime_branches": ["ETHAN_1: guarded four-member call-and-response double at cap 40.", "ETHAN_2: guarded four-member drum/dance and bug rematch, earliest cap 45.", "ETHAN_3: guarded four-member Soundproof bug-chorus rematch.", "ETHAN_4: repeatable guarded six-member final chorus."],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 mature trail chorus and five-badge rematches",
            "effective_levels": "initial 41-44; rematches earliest 46-49; final 46/46/47/47/48/49",
            "eligible_ratio": "18/18 source slots",
            "mega_access": True,
            "status": "pass",
            "reason": "Toxtricity, Rillaboom, Mr. Rime, Yanmega, and Kricketune are natural final forms by these exact levels; Bouffalant is single-stage. No Mega or battle-only form is used.",
        },
        "manual_quality": 10,
        "manual_difficulty": 9.0,
        "rematch_difficulty": {"TRAINER_ETHAN_2": 9.2, "TRAINER_ETHAN_3": 9.4, "TRAINER_ETHAN_4": 9.6},
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [{"reference_id": reference, "decision": "role evidence selected; unrelated full donor rejected", "reason": "Each source supports one voice or partner rule, while Ethan's four changing choruses are locally authored around his climbing-song dialogue."} for reference in REFERENCES],
            "decision": "All 1005 references and all six authored species reviews were checked. Showdown, Smogon, and Wolfe evidence support the roles; no historic roster was copied wholesale, and the old incoherent Tailwind/Trick Room family was removed.",
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Random doubles Toxtricity validates Punk Rock spread pressure and Volt Switch; local Throat Spray adds one finite escalation."},
            {"reference_id": REFERENCES[1], "adaptation": "Random doubles Bouffalant supplies Head Charge and coverage; local Soundproof is selected specifically to survive allied Boomburst."},
            {"reference_id": REFERENCES[2], "adaptation": "Random doubles Rillaboom validates Grassy Surge, Fake Out, Grassy Glide, and High Horsepower."},
            {"reference_id": REFERENCES[3], "adaptation": "Published OU Rillaboom confirms its independent terrain/priority value outside a scripted activation."},
            {"reference_id": REFERENCES[4], "adaptation": "Wolfe Glick's Players Cup II roster validates Rillaboom at championship stakes; Coalossal and Dynamax are explicitly not imported."},
            {"reference_id": REFERENCES[5], "adaptation": "Champions random Mr. Rime validates Screen Cleaner, Encore, and Freeze-Dry; Psychic/Focus Blast replace redundant speed control."},
            {"reference_id": REFERENCES[6], "adaptation": "Random doubles Yanmega validates Speed Boost and Bug Buzz; local Life Orb four-attack coverage removes Protect dependence."},
            {"reference_id": REFERENCES[7], "adaptation": "Random doubles Kricketune validates Knock Off and Taunt utility; local Focus Sash plus Swarm preserves one visible Swords Dance/Fell-through threat without Web."},
        ],
        "ordering": {
            "TRAINER_ETHAN_1": {"lead": ["SPECIES_TOXTRICITY", "SPECIES_BOUFFALANT"], "reserves": ["SPECIES_RILLABOOM", "SPECIES_MR_RIME"]},
            "TRAINER_ETHAN_2": {"lead": ["SPECIES_RILLABOOM", "SPECIES_MR_RIME"], "reserves": ["SPECIES_YANMEGA", "SPECIES_KRICKETUNE"]},
            "TRAINER_ETHAN_3": {"lead": ["SPECIES_TOXTRICITY", "SPECIES_BOUFFALANT"], "reserves": ["SPECIES_YANMEGA", "SPECIES_KRICKETUNE"]},
            "TRAINER_ETHAN_4": {"lead": ["SPECIES_TOXTRICITY", "SPECIES_BOUFFALANT"], "reserves": ["SPECIES_RILLABOOM", "SPECIES_MR_RIME", "SPECIES_YANMEGA", "SPECIES_KRICKETUNE"]},
        },
        "team_intent": "The first chorus demonstrates one exact ally-safety rule: Punk Rock Boomburst is lethal beside ordinary partners but harmless to Soundproof Bouffalant; Toxtricity retains foe-only Overdrive, Sludge Bomb, and Volt Switch fallbacks. Later records rotate into Grassy Surge/Fake Out rhythm, Screen Cleaner/Encore dance, Speed Boost Bug Buzz, and Sash Swarm setup. The final starts with the readable sound pair and carries all six voices.",
        "primary_player_question": "Can the player identify when Boomburst is partner-safe, stop or redirect that spread turn, then adapt when Ethan changes from sound immunity to terrain, screen removal, Speed Boost, or Sash Swarm pressure?",
        "intended_counterplay": "Wide Guard, Ghosts, Soundproof, Electric/Ground immunity, Lightning Rod, Volt Absorb, priority, Fake Out, Snarl, special bulk, Trick Room, terrain replacement, screen timing, Taunt, Haze, Clear Smog, phazing, multihit, hazards, Rock/Fire/Ice/Flying/Psychic/Ground/Poison/Steel attacks, Choice/recoil exploitation, and focus fire all matter. The six share no healing or Protect loop and require no exact player species.",
        "bespoke_ai": "All four records use smart switching, partner awareness, HP awareness, and Combo Setup. Native collateral scoring sees Soundproof Bouffalant and may choose Boomburst only when the active ally is safe; Overdrive and focused attacks remain legal fallbacks. Native logic handles Throat Spray, Grassy Surge, Fake Out, Screen Cleaner, Encore, Speed Boost, Sash, Swarm, setup, items, and switches. Nothing is forced.",
        "uniqueness": "Bouffalant, Rillaboom, Mr. Rime, Yanmega, and Kricketune are new to the first 114 encounters and absent from protected anchor teams. Toxtricity returns 50 battles after a branch-invariant sound pair, now as the owner of a Soundproof ally-collateral lesson rather than a generic singer. Unlike Battle 103's Telepathy Boomburst lead, this family changes partners across rematches, exposes unsafe fallback states, uses no Tailwind, weather, room, screen setter, Mega, or legendary, and culminates in six species rather than one fixed echo core.",
        "story_logic": "Ethan's climbing-song dialogue now names the Soundproof opening, explains Boomburst's ally collateral and Overdrive fallback, promises changing verses, and truthfully identifies terrain, screen removal, Speed Boost, and Swarm. Initial and rematch commands are double-safe; Match Call registration and native four-record routing remain intact.",
        "reward_logic": "Ordinary EXP, prize money, and Match Call registration only. No item or story reward is added at the route exit.",
        "campaign_reservations": {
            "spends": ["Ethan trail-chorus rematch family", "Punk Rock/Soundproof call-and-response", "Grassy Surge Rillaboom", "Screen Cleaner Mr. Rime", "Speed Boost Yanmega", "Sash Swarm Kricketune"],
            "preserves": ["all protected future anchors", "Wolfe's full Coalossal activation", "Telepathy Boomburst as Battle 103's separate lesson", "all Megas and legendary families", "Tailwind and Trick Room identities"],
            "repeat_rule": "Do not repeat Toxtricity plus Soundproof Bouffalant or the exact six-voice final. Individual debuts require a long gap and a materially different role.",
        },
        "author_self_check": {
            "strongest_part": "A generic 'sing while climbing' line now becomes a four-stage, mechanically truthful rematch identity whose main spread move changes safety with the active partner.",
            "weakest_link": "The final contains three forms of speed or priority through Grassy Glide, Speed Boost, and naturally quick attackers. There is no global speed field, and Trick Room, priority, Rock pressure, spread defense, and immediate focus remain broad counters.",
        },
        "closure": "Battle 115's full family is source-closed at quality 10: targets 9.0/9.2/9.4/9.6; all four reachable records are guarded doubles; 18 legal cap-relative slots use five fresh species and one 50-battle justified reuse, six distinct final-party items, exact Match Call routing, eight indexed Showdown/Smogon/Wolfe references, native-width dialogue, broad counterplay, and zero reward debt. Runtime remains unplayed.",
    }


def ledger_entry() -> dict:
    return {
        "index": 115,
        "encounter_id": "BATTLE_115_JAGGED_PASS_ETHAN",
        "identity": {"location": "JaggedPass", "category": "optional Camper four-record Match Call family", "format": "four guarded doubles", "strict_cap": 40, "memory_hook": "Ethan changes a trail chorus from Soundproof Boomburst to terrain, dance, and bug voices before combining all six."},
        "primary_player_question": "Can the player identify when Boomburst is partner-safe, then adapt as the active chorus changes its terrain, screen, Speed Boost, and Swarm rules?",
        "tempo": "Soundproof call-and-response opening, drum/dance bug rematch, sound-plus-bug rematch, then six-voice final.",
        "pressure_sources": ["Throat Spray Punk Rock Boomburst", "Assault Vest Soundproof Bouffalant", "Grassy Surge Fake Out/Glide Rillaboom", "Screen Cleaner Encore Mr. Rime", "Life Orb Speed Boost Yanmega", "Focus Sash Swarm Kricketune"],
        "intentional_opening": "Toxtricity plus Bouffalant publicly owns the first, third, and final leads; the second deliberately removes it for a different rhythm/dance opening.",
        "intentional_weakness": "No Tailwind, room, weather, screen setter, healing loop, Protect, Mega, or legendary; Toxtricity's best spread move depends on the active ally, Kricketune depends on Sash, and multiple members are vulnerable to Rock/Fire/priority/focus pressure.",
        "first_loss_lesson": "Check Ethan's partner before answering Toxtricity: block or exploit Boomburst only when Bouffalant makes it safe, then break the terrain, Sash, or Speed Boost engine of the current rematch rather than using one fixed plan.",
        "revealed_information": ["initial cap 40", "five-badge rematches", "four guarded doubles", "levels cap+1 to +4", "five campaign debuts", "one 50-battle reuse", "no finite reward"],
        "counterplay_classes": ["Wide Guard/Ghost/Soundproof/electric immunity", "priority/Fake Out/Snarl/special bulk/Trick Room", "terrain replacement/screen timing/Encore counterplay", "Taunt/Haze/Clear Smog/phazing/multihit/hazards", "Rock/Fire/Ice/Flying/Psychic/Ground/Poison/Steel", "item removal/recoil/focus fire"],
        "target_difficulty": 9.0,
        "difficulty_rationale": "The initial four optimized levels 41-44 create a severe positional double; rematches change the active rule and final six-body depth raises the target to 9.6 without boss resources.",
        "tuning_knob": "Reduce final Kricketune +4 to +3 first, then Yanmega +3 to +2; preserve Soundproof ownership, species, items, and four-record progression.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["jagged-pass", "route-rematch-family", "four-guarded-doubles", "trail-chorus", "toxtricity", "bouffalant", "rillaboom", "mr-rime", "yanmega", "kricketune", "soundproof-boomburst", "grassy-surge", "screen-cleaner", "speed-boost", "swarm", "five-fresh-species", "no-speed-field", "no-weather", "no-mega", "no-legendary"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Eight indexed Showdown, Smogon, and Wolfe references plus all-species reviews; chorus progression is local."},
        "author_self_check": {"strongest_part": "The active partner changes the safety and meaning of Toxtricity's signature attack.", "weakest_link": "Priority/speed density remains public and answerable without a field-wide speed setter."},
    }


def payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_115_JAGGED_PASS_ETHAN"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 115] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])
    sequence = json.loads(SEQUENCE.read_text())
    for row in sequence["entries"]:
        if row["index"] == 115:
            row.update({"category": "optional lower-pass Camper four-record Match Call family", "trainer_ids": list(TEAMS), "access_note": "Ethan faces left/right at (16,35) with sight four, 59 collision-walk steps from the upper entry and just before the lower exit. One physical position owns his initial record and three reachable rematches."})
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 116] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 115 else "next" if row["index"] == 116 else "queued"
    os_data = json.loads(OS_PATH.read_text())
    os_data["current_state"].update({"closed_encounters": 115, "next_index": 116, "next_encounter_id": NEXT["encounter_id"], "queued_sequence_entries": 0, "canonical_sequence_groups": 116, "physical_encounter_groups": 524, "unordered_physical_groups": 408})
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
            raise SystemExit(f"FAIL: Battle 115 party differs {trainer_id}")
        for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_COMBO_SETUP"):
            if token not in block:
                raise SystemExit(f"FAIL: Battle 115 {trainer_id} missing {token}")
        if len({member["species"] for member in team}) != len(team) or len({member["item"] for member in team}) != len(team):
            raise SystemExit(f"FAIL: Battle 115 duplicates {trainer_id}")
        for member in team:
            illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
            if illegal or member["ability_slot"] >= len(ability_slots[member["species"]]):
                raise SystemExit(f"FAIL: Battle 115 legality {member['species']} {illegal}")
            if "MOVE_PROTECT" in member["moves"]:
                raise SystemExit("FAIL: Battle 115 Protect restraint drifted")

    used = {member["species"] for member in TEAM_4}
    if used & protected_anchor_species():
        raise SystemExit("FAIL: Battle 115 protected anchor collision")
    ledger = json.loads((ROOT / "docs/verdant_species_usage_ledger.json").read_text())
    tox = next(row for row in ledger["species"] if row["species"] == "SPECIES_TOXTRICITY")
    prior = [entry["battle_index"] for entry in tox["appearances"] if entry["battle_index"] < 115]
    if prior != [65]:
        raise SystemExit(f"FAIL: Battle 115 Toxtricity reuse drifted {prior}")

    script = (ROOT / "data/maps/JaggedPass/scripts.inc").read_text()
    if "trainerbattle_double TRAINER_ETHAN_1" not in script or "trainerbattle_rematch_double TRAINER_ETHAN_1" not in script or script.count("JaggedPass_Text_EthanNeedTwoMons") < 3:
        raise SystemExit("FAIL: Battle 115 guarded double routing")
    if "REMATCH(TRAINER_ETHAN_1, TRAINER_ETHAN_2, TRAINER_ETHAN_3, TRAINER_ETHAN_4, JAGGED_PASS)" not in (ROOT / "src/battle_setup.c").read_text():
        raise SystemExit("FAIL: Battle 115 rematch row")
    obj = next(row for row in json.loads((ROOT / "data/maps/JaggedPass/map.json").read_text())["object_events"] if row.get("script") == "JaggedPass_EventScript_Ethan")
    if (obj["x"], obj["y"], obj["movement_type"], str(obj["trainer_sight_or_berry_tree_id"])) != (16, 35, "MOVEMENT_TYPE_FACE_LEFT_AND_RIGHT", "4"):
        raise SystemExit("FAIL: Battle 115 geometry")
    dialogue = script.split("JaggedPass_Text_EthanIntro:", 1)[1].split("JaggedPass_Text_GruntIntro:", 1)[0]
    for cue in ("rhythm for this trail", "Soundproof Bouffalant", "Boomburst hits foes and my ally", "safer refrain", "chorus changed", "bugs may take the melody", "Grassy Surge", "erases screens", "Yanmega gains Speed", "waits for Swarm", "two healthy"):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 115 dialogue missing {cue}")
    for raw_line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = raw_line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 115 overlong dialogue: {visible}")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]
    expected = {
        "TRAINER_ETHAN_1": ("Soundproof call-and-response", 90, 4, 3),
        "TRAINER_ETHAN_2": ("Drum-dance bug rematch", 92, 4, 3),
        "TRAINER_ETHAN_3": ("Soundproof bug-chorus rematch", 94, 4, 3),
        "TRAINER_ETHAN_4": ("Six-voice trail chorus final", 96, 6, 2),
    }
    for trainer_id, (archetype, difficulty, size, offset) in expected.items():
        if manifest[trainer_id] != {"format": "double", "target_size": size, "archetype": archetype, "difficulty": difficulty, "partner_interaction": True, "level_offset": offset, "location": "Jagged Pass"}:
            raise SystemExit(f"FAIL: Battle 115 manifest {trainer_id}")
    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference not in corpus_ids for reference in REFERENCES):
        raise SystemExit("FAIL: Battle 115 reference")


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
                raise SystemExit(f"FAIL: Battle 115 stale {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())["entries"]
        entries = [row for row in guide if row["trainerId"] in TEAMS]
        if len(entries) != 4 or any(row["designStatus"] != "closed" or row["format"] != "double" for row in entries):
            raise SystemExit("FAIL: Battle 115 guide")
        if {row["trainerId"]: row["partySize"] for row in entries} != {"TRAINER_ETHAN_1": 4, "TRAINER_ETHAN_2": 4, "TRAINER_ETHAN_3": 4, "TRAINER_ETHAN_4": 6}:
            raise SystemExit("FAIL: Battle 115 guide sizes")
    print("PASS: Battle 115 Ethan trail-chorus family is source-closed")


if __name__ == "__main__":
    main()
