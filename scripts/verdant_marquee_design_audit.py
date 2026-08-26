#!/usr/bin/env python3
"""Validate Verdant's design-complete, source-unimplemented marquee dossiers."""

from __future__ import annotations

import json
import re
from pathlib import Path

import build_competitive_team_index as competitive
import verdant_bespoke_battle_audit as bespoke
import verdant_doubles_conversion as doubles


ROOT = Path(__file__).resolve().parents[1]
DESIGNS = ROOT / "docs/verdant_marquee_battle_designs.json"
META = ROOT / "docs/competitive_team_index.meta.json"
RESERVATIONS = ROOT / "docs/verdant_historic_team_reservations.json"

REQUIRED_DOSSIER_FIELDS = {
    "anchor_id", "planning_tier", "status", "campaign_state", "runtime",
    "rolling_context", "identity", "difficulty", "team", "ordering", "ai",
    "counterplay", "competitive_research", "campaign_reservations",
    "presentation", "author_self_check", "verification", "mechanics_proposal",
}

REQUIRED_MON_FIELDS = {
    "order", "species", "level_offset", "item", "ability", "ability_slot",
    "spread", "moves", "role", "lead_group", "mega_candidate",
}

FORBIDDEN_GIMMICK_WORDS = {
    "tera", "terastall", "z-move", "dynamax", "gigantamax",
}

def read(path: Path) -> str:
    return path.read_text()


def load(path: Path) -> dict:
    return json.loads(read(path))


def text_list(value: object) -> list[str]:
    if not isinstance(value, list):
        return []
    return [item.strip() for item in value if isinstance(item, str) and item.strip()]


def pointer_map() -> dict[str, str]:
    source = read(ROOT / "src/data/pokemon/level_up_learnset_pointers.h") + "\n" + read(
        ROOT / "src/data/pokemon/verdant_gen9_level_up_pointers.h"
    )
    return dict(re.findall(r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*(s[A-Za-z0-9_]+LevelUpLearnset)", source))


def level_move_map() -> dict[str, set[str]]:
    source = read(ROOT / "src/data/pokemon/level_up_learnsets.h") + "\n" + read(
        ROOT / "src/data/pokemon/verdant_gen9_level_up_learnsets.h"
    )
    result = {}
    for name, body in re.findall(
        r"static const struct LevelUpMove\s+(s[A-Za-z0-9_]+LevelUpLearnset)\[\]\s*=\s*\{(.*?)\};",
        source,
        re.S,
    ):
        result[name] = set(re.findall(r"MOVE_[A-Z0-9_]+", body))
    return result


def preevolution_map() -> dict[str, set[str]]:
    source = read(ROOT / "src/data/pokemon/evolution.h") + "\n" + read(
        ROOT / "src/data/pokemon/verdant_gen9_evolutions.h"
    )
    result: dict[str, set[str]] = {}
    for species, body in re.findall(
        r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*(.*?)(?=^\s*\[SPECIES_|\Z)",
        source,
        re.M | re.S,
    ):
        for method, target in re.findall(
            r"\{\s*(EVO_[A-Z0-9_]+)\s*,\s*[^,{}]+,\s*(SPECIES_[A-Z0-9_]+)\s*\}",
            body,
        ):
            if method not in {"EVO_MEGA_EVOLUTION", "EVO_MOVE_MEGA_EVOLUTION", "EVO_PRIMAL_REVERSION"}:
                result.setdefault(target, set()).add(species)
    return result


def mega_evolution_requirements() -> tuple[set[tuple[str, str]], set[tuple[str, str]]]:
    source = read(ROOT / "src/data/pokemon/evolution.h") + "\n" + read(
        ROOT / "src/data/pokemon/verdant_gen9_evolutions.h"
    )
    item_pairs: set[tuple[str, str]] = set()
    move_pairs: set[tuple[str, str]] = set()
    for species, body in re.findall(
        r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*(.*?)(?=^\s*\[SPECIES_|\Z)",
        source,
        re.M | re.S,
    ):
        for method, parameter in re.findall(
            r"\{\s*(EVO_(?:MOVE_)?MEGA_EVOLUTION)\s*,\s*([^,{}]+),\s*SPECIES_[A-Z0-9_]+\s*\}",
            body,
        ):
            parameter = parameter.strip()
            if method == "EVO_MEGA_EVOLUTION" and parameter.startswith("ITEM_"):
                item_pairs.add((species, parameter))
            elif method == "EVO_MOVE_MEGA_EVOLUTION" and parameter.startswith("MOVE_"):
                move_pairs.add((species, parameter))
    return item_pairs, move_pairs


def species_can_learn(
    species: str,
    move: str,
    pointers: dict[str, str],
    level_moves: dict[str, set[str]],
    tm_indices: dict[str, int],
    tutor_indices: dict[str, int],
    base_tm: str,
    gen9_tm: str,
    base_tutor: str,
    gen9_tutor: str,
    egg_source: str,
    predecessors: dict[str, set[str]],
) -> bool:
    candidates = []
    stack = [species]
    while stack:
        candidate = stack.pop()
        if candidate in candidates:
            continue
        candidates.append(candidate)
        stack.extend(sorted(predecessors.get(candidate, set())))
    for candidate in candidates:
        learnset = pointers.get(candidate)
        if learnset and move in level_moves.get(learnset, set()):
            return True
        short = candidate.removeprefix("SPECIES_")
        if move in bespoke.species_egg_body(egg_source, short):
            return True
        if bespoke.species_has_tmhm_move(base_tm, tm_indices, short, move):
            return True
        if bespoke.species_has_gen9_tm_move(gen9_tm, tm_indices, short, move):
            return True
        if move in tutor_indices:
            for source in (base_tutor, gen9_tutor):
                if bespoke.species_has_tutor_move(source, tutor_indices, short, move):
                    return True
    return False


def validate_mechanics(payload: dict, problems: list[str]) -> None:
    baselines = payload.get("mechanics_baselines", {})
    league = baselines.get("pokemon_league_main_story", {})
    expected = {
        "strict_cap": 80,
        "allowed_battle_transformations": ["Mega Evolution", "Primal Reversion"],
        "forbidden_battle_transformations": ["Terastallization", "Z-Moves", "Dynamax", "Gigantamax"],
        "automatic_healing_between_members": False,
        "manual_overworld_bag_between_members": True,
        "pokemon_menu_between_members": True,
        "save_menu_between_members": True,
        "items_during_trainer_battles": False,
        "party_composition_locked_after_entry": True,
        "drake_releases_player_control_after_victory": True,
        "hall4_allows_normal_overworld_menu": True,
        "champion_room_forces_approach_after_entry": True,
        "mechanics_proposal": None,
    }
    for key, value in expected.items():
        if league.get(key) != value:
            problems.append(f"League mechanics baseline drifted {key}: expected {value!r}, found {league.get(key)!r}")

    rooms = [
        ROOT / "data/maps/EverGrandeCity_SidneysRoom/scripts.inc",
        ROOT / "data/maps/EverGrandeCity_PhoebesRoom/scripts.inc",
        ROOT / "data/maps/EverGrandeCity_GlaciasRoom/scripts.inc",
        ROOT / "data/maps/EverGrandeCity_DrakesRoom/scripts.inc",
    ]
    if any("HealPlayerParty" in read(path) for path in rooms):
        problems.append("League mechanics claim no automatic healing, but a member room heals the party")
    drake = read(rooms[-1])
    if not all(token in drake for token in (
        "EverGrandeCity_DrakesRoom_EventScript_Defeated::",
        "msgbox EverGrandeCity_DrakesRoom_Text_PostBattleSpeech",
        "release",
    )):
        problems.append("Drake no longer returns player control after victory")
    hall4 = read(ROOT / "data/maps/EverGrandeCity_Hall4/scripts.inc")
    if "lockall" in hall4 or "trainerbattle" in hall4:
        problems.append("Hall 4 no longer provides a normal between-battle control window")
    champion = read(ROOT / "data/maps/EverGrandeCity_ChampionsRoom/scripts.inc")
    if not all(token in champion for token in (
        "MAP_SCRIPT_ON_FRAME_TABLE", "lockall", "goto EverGrandeCity_ChampionsRoom_EventScript_Wallace",
    )):
        problems.append("Champion room no longer forces the Wallace approach after entry")
    start_menu = read(ROOT / "src/start_menu.c").rsplit("static void BuildNormalStartMenu(void)", 1)[1].split(
        "static void BuildSafariZoneStartMenu(void)", 1
    )[0]
    for action in ("MENU_ACTION_POKEMON", "MENU_ACTION_BAG", "MENU_ACTION_SAVE"):
        if f"AddStartMenuAction({action})" not in start_menu:
            problems.append(f"normal between-battle menu lost {action}")
    battle_main = read(ROOT / "src/battle_main.c")
    if not all(token in battle_main for token in (
        "gSaveBlock2Ptr->gameDifficulty == DIFFICULTY_CHALLENGE",
        "gBattleTypeFlags & BATTLE_TYPE_TRAINER",
        "BattleScript_ActionSelectionItemsCantBeUsed",
    )):
        problems.append("Challenge trainer battles no longer enforce the item ban")


def validate() -> list[str]:
    payload = load(DESIGNS)
    meta = load(META)
    problems: list[str] = []

    expected_corpus = {key: meta.get(key) for key in ("version", "record_count", "sha256")}
    actual_corpus = {key: payload.get("corpus_identity", {}).get(key) for key in expected_corpus}
    if actual_corpus != expected_corpus:
        problems.append(f"marquee corpus identity is stale: expected {expected_corpus}, found {actual_corpus}")

    validate_mechanics(payload, problems)
    index = competitive.build()
    refs = {record["reference_id"]: record for record in index}
    blueprints = {
        row["anchor"]: row
        for row in load(RESERVATIONS).get("marquee_blueprints", {}).get("entries", [])
    }
    expected_anchors = payload.get("expected_phase_anchors", [])
    designs = payload.get("designs", {})
    if set(designs) != set(expected_anchors):
        missing = sorted(set(expected_anchors) - set(designs))
        extra = sorted(set(designs) - set(expected_anchors))
        problems.append(f"current marquee phase mismatch: missing={missing}, extra={extra}")

    trainers_text = read(ROOT / "src/data/trainers.h")
    trainer_blocks = doubles.trainer_blocks(trainers_text)
    ability_slots = doubles.base_ability_slots()
    item_tokens = set(re.findall(r"#define\s+(ITEM_[A-Z0-9_]+)", read(ROOT / "include/constants/items.h")))
    move_tokens = set(re.findall(r"#define\s+(MOVE_[A-Z0-9_]+)", read(ROOT / "include/constants/moves.h")))
    spread_tokens = set(re.findall(r"\[(SPREAD_[A-Z0-9_]+)\]", read(ROOT / "src/data/trainer_spreads.h")))
    pointers = pointer_map()
    level_moves = level_move_map()
    predecessors = preevolution_map()
    mega_item_pairs, mega_move_pairs = mega_evolution_requirements()
    base_tm = read(ROOT / "src/data/pokemon/tmhm_learnsets.h")
    gen9_tm = read(ROOT / "src/data/pokemon/verdant_gen9_tmhm_learnsets.h")
    base_tutor = read(ROOT / "src/data/pokemon/tutor_learnsets.h")
    gen9_tutor = read(ROOT / "src/data/pokemon/verdant_gen9_tutor_learnsets.h")
    egg_source = read(ROOT / "src/data/pokemon/egg_moves.h") + "\n" + read(
        ROOT / "src/data/pokemon/verdant_gen9_egg_moves.h"
    )
    tm_indices = bespoke.tm_move_indices(read(ROOT / "src/data/party_menu.h"))
    tutor_indices = bespoke.tutor_move_indices(base_tutor)

    for anchor_id, dossier in designs.items():
        prefix = anchor_id
        missing_fields = sorted(REQUIRED_DOSSIER_FIELDS - set(dossier))
        if missing_fields:
            problems.append(f"{prefix}: missing dossier fields {missing_fields}")
            continue
        if dossier.get("anchor_id") != anchor_id:
            problems.append(f"{prefix}: embedded anchor_id differs")
        if anchor_id not in blueprints:
            problems.append(f"{prefix}: no campaign blueprint exists")
        if dossier.get("status") != {
            "design": "design-complete",
            "source": "unimplemented",
            "static": "design-validated",
            "runtime": "unplayed",
        }:
            problems.append(f"{prefix}: status must remain design-complete/source-unimplemented/design-validated/runtime-unplayed")
        if dossier.get("mechanics_proposal") is not None:
            problems.append(f"{prefix}: mechanics proposal requires explicit approval and cannot coexist with design-complete")

        campaign = dossier.get("campaign_state", {})
        if campaign.get("strict_cap") != 80 or campaign.get("mechanics_baseline_id") != "pokemon_league_main_story":
            problems.append(f"{prefix}: campaign state does not bind the cap-80 League baseline")
        for key in ("player_tools", "mega_access", "evolution_phase", "preparation_access"):
            if not campaign.get(key):
                problems.append(f"{prefix}: campaign_state.{key} is empty")

        runtime = dossier.get("runtime", {})
        trainer_ids = text_list(runtime.get("trainer_ids"))
        if not trainer_ids or any(trainer_id not in trainer_blocks for trainer_id in trainer_ids):
            problems.append(f"{prefix}: runtime trainer IDs are empty or unknown")
        if runtime.get("canonical_format") not in {"single", "double", "native-pair double"}:
            problems.append(f"{prefix}: invalid canonical format")
        if not isinstance(runtime.get("variants"), list) or not runtime["variants"]:
            problems.append(f"{prefix}: runtime variants are not recorded")

        rolling = dossier.get("rolling_context", {})
        if not all(key in rolling for key in ("available", "reason", "protected_neighbor_anchors")):
            problems.append(f"{prefix}: rolling context is incomplete")

        identity = dossier.get("identity", {})
        for key in ("memory_hook", "story_fit", "primary_player_question", "primary_mode", "secondary_mode", "preview_pressure"):
            if not identity.get(key):
                problems.append(f"{prefix}: identity.{key} is empty")

        difficulty = dossier.get("difficulty", {})
        if difficulty.get("target") != 10 or difficulty.get("observed") is not None:
            problems.append(f"{prefix}: League target must be 10 and observed difficulty must remain null")
        for key in ("rationale", "pressure_sources", "resource_tax", "tuning_order"):
            if not difficulty.get(key):
                problems.append(f"{prefix}: difficulty.{key} is empty")

        team = dossier.get("team")
        if not isinstance(team, list) or len(team) != 6:
            problems.append(f"{prefix}: exact party must contain six Pokémon")
            continue
        orders = [mon.get("order") for mon in team]
        if orders != list(range(1, 7)):
            problems.append(f"{prefix}: party order must be exactly 1-6")
        species_seen: set[str] = set()
        mega_count = 0
        for mon in team:
            mon_prefix = f"{prefix}/{mon.get('species', 'unknown')}"
            missing_mon = sorted(REQUIRED_MON_FIELDS - set(mon))
            if missing_mon:
                problems.append(f"{mon_prefix}: missing fields {missing_mon}")
                continue
            species = mon["species"]
            if species in species_seen:
                problems.append(f"{mon_prefix}: duplicate species")
            species_seen.add(species)
            if species not in ability_slots:
                problems.append(f"{mon_prefix}: species is unavailable in current engine stats")
                continue
            slot = mon["ability_slot"]
            slots = ability_slots[species]
            if not isinstance(slot, int) or slot < 0 or slot >= len(slots) or slots[slot] != mon["ability"]:
                problems.append(f"{mon_prefix}: ability slot {slot} does not expose {mon['ability']}; found {slots}")
            if mon["item"] not in item_tokens:
                problems.append(f"{mon_prefix}: unknown item {mon['item']}")
            if mon["item"].endswith("_Z"):
                problems.append(f"{mon_prefix}: active Z item is forbidden")
            if mon["spread"] not in spread_tokens:
                problems.append(f"{mon_prefix}: unknown spread {mon['spread']}")
            if not isinstance(mon["level_offset"], int):
                problems.append(f"{mon_prefix}: level_offset must be an integer")
            moves = mon["moves"]
            if not isinstance(moves, list) or len(moves) != 4 or len(set(moves)) != 4:
                problems.append(f"{mon_prefix}: moves must contain four distinct entries")
            else:
                for move in moves:
                    if move not in move_tokens:
                        problems.append(f"{mon_prefix}: unknown move {move}")
                    elif not species_can_learn(
                        species, move, pointers, level_moves, tm_indices, tutor_indices,
                        base_tm, gen9_tm, base_tutor, gen9_tutor, egg_source, predecessors,
                    ):
                        problems.append(f"{mon_prefix}: cannot legally learn {move}")
            if mon.get("mega_candidate"):
                mega_count += 1
                item_match = (species, mon["item"]) in mega_item_pairs
                move_match = any((species, move) in mega_move_pairs for move in moves)
                if not item_match and not move_match:
                    problems.append(f"{mon_prefix}: nominated Mega has no current item or move Mega mapping")
            elif (species, mon["item"]) in mega_item_pairs:
                problems.append(f"{mon_prefix}: carries a Mega item without being the nominated Mega")
        if mega_count != 1:
            problems.append(f"{prefix}: League dossier must nominate exactly one Mega candidate, found {mega_count}")

        ordering = dossier.get("ordering", {})
        for key in ("intended_lead", "reserve_sequence", "mandatory_order_reason"):
            if not ordering.get(key):
                problems.append(f"{prefix}: ordering.{key} is empty")

        ai = dossier.get("ai", {})
        for key in ("existing_flags", "required_flags", "custom_requirements", "forbidden_behaviors", "state_machine"):
            if key not in ai or not ai[key]:
                problems.append(f"{prefix}: ai.{key} is empty")

        counterplay = dossier.get("counterplay", {})
        if not counterplay.get("intentional_weakness") or not counterplay.get("first_loss_lesson"):
            problems.append(f"{prefix}: intentional weakness or first-loss lesson is empty")
        if len(text_list(counterplay.get("classes"))) < 3:
            problems.append(f"{prefix}: fewer than three counterplay classes")
        if not text_list(counterplay.get("revealed_information")) or not text_list(counterplay.get("unacceptable_failure_modes")):
            problems.append(f"{prefix}: revealed information or unacceptable failure modes are empty")

        research = dossier.get("competitive_research", {})
        if {key: research.get("index", {}).get(key) for key in expected_corpus} != expected_corpus:
            problems.append(f"{prefix}: competitive research index identity is stale")
        if not research.get("queries") or not research.get("candidates") or not research.get("selected_reference_ids"):
            problems.append(f"{prefix}: competitive research is incomplete")
        for candidate in research.get("candidates", []):
            ref = candidate.get("reference_id")
            if ref not in refs:
                problems.append(f"{prefix}: unknown candidate reference {ref}")
            if candidate.get("decision") not in {"selected", "adapted", "rejected", "reserved"} or not candidate.get("reason"):
                problems.append(f"{prefix}: candidate {ref} lacks decision/reason")
        for ref in research.get("selected_reference_ids", []):
            if ref not in refs:
                problems.append(f"{prefix}: selected unknown reference {ref}")

        reservations = dossier.get("campaign_reservations", {})
        for key in ("spends", "preserves", "releases", "collision_notes"):
            if key not in reservations or not reservations[key]:
                problems.append(f"{prefix}: campaign_reservations.{key} is empty")

        presentation = dossier.get("presentation", {})
        for key in ("intro_concept", "defeat_concept", "post_battle_concept", "hint_concept", "guide_summary", "native_width_status"):
            if not presentation.get(key):
                problems.append(f"{prefix}: presentation.{key} is empty")

        self_check = dossier.get("author_self_check") or {}
        if set(self_check) != {"strongest_part", "weakest_link"}:
            problems.append(f"{prefix}: author_self_check must contain exactly strongest_part and weakest_link")
        elif not all(isinstance(self_check[key], str) and self_check[key].strip() for key in self_check):
            problems.append(f"{prefix}: author self-check contains an empty judgment")

        verification = dossier.get("verification", {})
        expected_verification = {
            "design_schema": "pass",
            "species_items_moves_abilities": "pass",
            "source_implementation": "not-started",
            "script_and_format": "not-started",
            "dialogue_width": "concept-only",
            "guide": "concept-only",
            "runtime": "unplayed",
            "observed_difficulty": None,
        }
        for key, value in expected_verification.items():
            if verification.get(key) != value:
                problems.append(f"{prefix}: verification.{key} expected {value!r}, found {verification.get(key)!r}")
        if not verification.get("evidence"):
            problems.append(f"{prefix}: verification evidence is empty")

        serialized = json.dumps(dossier).lower()
        for word in FORBIDDEN_GIMMICK_WORDS:
            if word in serialized and word not in json.dumps(research.get("rejected_gimmicks", [])).lower():
                # The terms may appear in documented rejection fields, but nowhere as an active mechanic.
                active = json.dumps({
                    "team": team,
                    "identity": identity,
                    "difficulty": difficulty,
                    "ai": ai,
                }).lower()
                if word in active:
                    problems.append(f"{prefix}: active design contains forbidden gimmick term {word}")

    return problems


def main() -> None:
    problems = validate()
    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))
    payload = load(DESIGNS)
    print(f"PASS: {len(payload['designs'])} marquee dossiers are design-complete and source-unimplemented")
    print("PASS: agreed mechanics, legal exact teams, competitive references, author self-checks, and unplayed runtime status are explicit")


if __name__ == "__main__":
    main()
