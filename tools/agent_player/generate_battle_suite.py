#!/usr/bin/env python3
"""Export exact authored opponents and explicitly certified preparation scenarios.

Encounter chronology and historical strict_cap are descriptive only. Availability
comes from the separate source-backed arsenal producer; missing certificates stay
unresolved and cannot be used as legal-win evidence.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import emerald_champions_teams as teams
import stamp_release_inputs as build_inputs

MASTER = teams.MASTER
ENCOUNTER_RE = teams.ENCOUNTER_RE
BRANCH_RE = teams.BRANCH_RE
# Include the authority for extraction, move legality, preparation and playback,
# in addition to every native build input (AI/mechanics/species/caps/progression).
HARNESS_INPUTS = (
    "scripts/emerald_champions_teams.py", "scripts/ec_moves.py",
    "scripts/emerald_champions_evs.py", "scripts/build_provenance.py",
    "scripts/playthrough/battle_driver.py", "scripts/playthrough/prepare_party.py",
    "scripts/native_tools.py", "scripts/render_emerald_champions_ui.py",
    "scripts/rom_artifacts.py", "scripts/verify_trainer_ability_legality.py",
    "tests/headless/emerald_champions_mgba_runner.c",
    "tools/agent_player/generate_battle_suite.py",
    "tools/agent_player/battle_calibration.py", "scripts/battle_arsenal.py",
    "tools/agent_player/battle_batch.py",
    "tools/agent_player/doubles_policy.py", "tools/agent_player/battle_search.py",
    "scripts/battle_opening_arsenal.py", "tools/agent_player/export_opening_batch.py",
    "scripts/battle_campaign_arsenal.py",
    "scripts/battle_scripted_wild_arsenal.py",
    "scripts/economy_reference.py", "scripts/item_catalog.py",
    "scripts/audit/map_dynamic_inventory.py",
)


def digest_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def digest_file(path: Path) -> str:
    return digest_bytes(path.read_bytes())


def canonical(value: Any) -> bytes:
    return json.dumps(value, sort_keys=True, separators=(",", ":")).encode()


def field(text: str, key: str) -> str:
    match = re.search(rf"(?m)^{re.escape(key)}: (.*)$", text)
    return match.group(1) if match else ""


def split_blocks(text: str, pattern: re.Pattern[str]) -> list[tuple[str, str]]:
    marks = list(pattern.finditer(text))
    return [(mark.group(1), text[mark.start():marks[index + 1].start() if index + 1 < len(marks) else len(text)]) for index, mark in enumerate(marks)]


def input_hashes() -> dict[str, str]:
    # Reuse the build's input authority instead of maintaining an incomplete
    # second list. New files and deleted files both change this tree digest.
    digest, _ = build_inputs.digest_tree()
    result = {"native_build_inputs": digest}
    for relative in HARNESS_INPUTS:
        path = ROOT / relative
        result[relative] = digest_file(path) if path.is_file() else "missing"
    return result


def source_fingerprint() -> str:
    return digest_bytes(canonical(input_hashes()))


def load_arsenals(path: Path, fingerprint: str) -> list[dict[str, Any]]:
    data = json.loads(path.read_text())
    if data.get("schema_version") != 2 or data.get("source_generated") is not True:
        raise ValueError("arsenal index must be schema 2 and source_generated=true")
    if data.get("source_fingerprint") != fingerprint:
        raise ValueError("arsenal index is stale for current native and harness sources")
    scenarios = data.get("scenarios")
    if not isinstance(scenarios, list):
        raise ValueError("arsenal index needs explicit scenarios")
    seen = set()
    for scenario in scenarios:
        key = (scenario.get("trainer_id"), scenario.get("scenario_id"), scenario.get("difficulty"))
        if any(not value for value in key) or key in seen:
            raise ValueError(f"missing or duplicate scenario identity: {key}")
        if scenario["difficulty"] not in {"easy", "medium", "hard"}:
            raise ValueError(f"unknown difficulty: {key}")
        if scenario.get("legality_status") not in {"proven", "unresolved"}:
            raise ValueError(f"scenario requires an explicit legality_status: {key}")
        seen.add(key)
    return scenarios


def opponent_catalogue() -> list[dict[str, Any]]:
    metadata = {int(number): block for number, block in split_blocks(MASTER.read_text(), ENCOUNTER_RE)}
    opponents = []
    for branch in teams.read_teams():
        encounter = metadata.get(branch.encounter, "")
        branch_metadata = dict(split_blocks(encounter, BRANCH_RE)).get(branch.trainer, "")
        members = [{
            "slot": index, "species": "SPECIES_" + mon.species,
            "item": "ITEM_" + mon.item, "level_offset": mon.offset,
            "ability": "ABILITY_" + mon.ability, "nature": "NATURE_" + mon.nature,
            "evs": [int(value) for value in mon.evs.split("/")],
            "ivs": [int(value) for value in mon.ivs.split("/")],
            "friendship": mon.friendship, "moves": ["MOVE_" + move for move in mon.moves],
        } for index, mon in enumerate(branch.mons, 1)]
        opponents.append({
            "opponent_id": f"E{branch.encounter:04d}-{branch.trainer}",
            "encounter": branch.encounter, "trainer_id": branch.trainer,
            "class": branch.cls, "format": field(branch_metadata, "format"),
            "ai_profile": teams.CLASSES[branch.cls], "ai_extra": branch.ai,
            "strategy": branch.strategy, "tactics": branch.tactics,
            "field": branch.field, "mega_slots": branch.mega_slots,
            "plan": branch.plan, "counterplay": branch.crack, "team": members,
            "descriptive_metadata": {key: field(encounter, key) for key in
                ("campaign_order", "strict_cap", "chapter", "location")},
            "source": str(teams.TEAMS.relative_to(ROOT)),
        })
    return opponents


def generate(arsenal_index: Path | None = None, *, internal_inputs=None) -> dict[str, Any]:
    # Trusted in-process export only; public CLI supplies no frozen inputs.
    hashes = internal_inputs if internal_inputs is not None else input_hashes()
    fingerprint = digest_bytes(canonical(hashes))
    opponents = opponent_catalogue()
    by_trainer = {opponent["trainer_id"]: opponent for opponent in opponents}
    scenarios = load_arsenals(arsenal_index, fingerprint) if arsenal_index else []
    puzzles = []
    for scenario in scenarios:
        trainer = scenario["trainer_id"]
        if scenario.get('battle_kind') == 'birch_rescue':
            import battle_scripted_wild_arsenal as wild
            wild.certify_scenario(scenario, internal_fingerprint=fingerprint)
            dossier = {'opponent_id': 'BIRCH_RESCUE', 'battle_kind': 'birch_rescue',
                       'format': 'native_first_battle_doubles',
                       'source_member_domains': scenario['expected_scripted_wild']['source_member_domains']}
        elif trainer not in by_trainer:
            raise ValueError(f"scenario references unauthored trainer: {trainer}")
        else:
            dossier = by_trainer[trainer]
        expected = scenario.get("expected_opponent")
        if expected is not None:
            if not isinstance(expected, dict) or expected.get("trainer_id") != trainer:
                raise ValueError("scenario opponent identity does not match its trainer")
            dossier = {**dossier, "authored_team": dossier["team"],
                       "team": expected["team"], "regional_replacement": expected["regional_replacement"],
                       "expected_combat_sha256": expected["combat_sha256"]}
        puzzle = {
            "puzzle_id": f"{dossier['opponent_id']}-{scenario['scenario_id']}-{scenario['difficulty']}",
            "opponent_dossier": dossier, "scenario": scenario,
            "provenance": {"arsenal_index": str(arsenal_index)},
        }
        puzzle["content_sha256"] = digest_bytes(canonical(puzzle))
        puzzles.append(puzzle)
    return {
        "schema_version": 2, "kind": "emerald_champions_independent_native_battle_suite",
        "source_fingerprint": fingerprint, "inputs": hashes,
        "opponent_count": len(opponents), "opponents": opponents,
        "puzzle_count": len(puzzles), "puzzles": puzzles,
        "unbound_trainers": sorted(set(by_trainer) - {scenario["trainer_id"] for scenario in scenarios}),
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--arsenal-index", type=Path)
    parser.add_argument("--out", type=Path, required=True)
    args = parser.parse_args()
    suite = generate(args.arsenal_index)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(suite, indent=2, sort_keys=True) + "\n")
    print(f"generated {suite['opponent_count']} opponents and {suite['puzzle_count']} scenario puzzles: {args.out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
