#!/usr/bin/env python3
"""Generate the opponent half of every independent trainer-battle puzzle.

The source-derived arsenal is intentionally a separate required input: this
tool will not invent reachability from the opponent chronology.  Its producer
must bind each arsenal to the same source fingerprint and campaign order.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[2]
MASTER = ROOT / "data/emerald_champions/emerald_champions_master_battle_design.txt"
ENCOUNTER_RE = re.compile(r"(?m)^=== ENCOUNTER (\d{4}) ===$")
BRANCH_RE = re.compile(r"(?m)^--- BRANCH ([A-Z0-9_]+) ---$")
MON_RE = re.compile(
    r"(?m)^  (\d+)\. (SPECIES_[A-Z0-9_]+) @ (ITEM_[A-Z0-9_]+) \| "
    r"level_offset=(-?\d+) \| ability=(ABILITY_[A-Z0-9_]+) \| "
    r"nature=(NATURE_[A-Z0-9_]+) \| stat_points=([0-9/]+) \| "
    r"moves=(MOVE_[A-Z0-9_]+(?:,MOVE_[A-Z0-9_]+){0,3})$"
)
INPUTS = (
    "data/emerald_champions/emerald_champions_master_battle_design.txt",
    "data/emerald_champions/emerald_champions_battle_teams.txt",
    "src/data/trainers.party",
    "data/emerald_champions/emerald_champions_hand_audited_battle_sets.json",
    "data/emerald_champions/emerald_champions_move_access_review.json",
    "data/emerald_champions/showdown_champions_learnsets.json",
    "src/data/wild_encounters.json",
    "include/constants/event_objects.h",
    "include/constants/items.h",
    "include/constants/opponents.h",
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
    result = {}
    for relative in INPUTS:
        path = ROOT / relative
        if not path.is_file():
            raise SystemExit(f"missing suite input: {relative}")
        result[relative] = digest_file(path)
    # Map scripts own trainer reachability and gift/static/item gates. Aggregate
    # them canonically so any map-side change invalidates all derived arsenals.
    map_rows = [(str(path.relative_to(ROOT)), digest_file(path)) for path in sorted((ROOT / "data/maps").glob("*/scripts.inc"))]
    result["data/maps/*/scripts.inc"] = digest_bytes(canonical(map_rows))
    return result


def load_arsenals(path: Path, fingerprint: str) -> dict[str, Any]:
    data = json.loads(path.read_text())
    if data.get("schema_version") != 1 or data.get("source_generated") is not True:
        raise SystemExit("arsenal index must be schema 1 and source_generated=true")
    if data.get("source_fingerprint") != fingerprint:
        raise SystemExit("arsenal index is stale for current trainer/reachability sources")
    entries = data.get("by_campaign_order")
    if not isinstance(entries, dict):
        raise SystemExit("arsenal index needs by_campaign_order")
    return entries


def generate(arsenal_index: Path) -> dict[str, Any]:
    hashes = input_hashes()
    fingerprint = digest_bytes(canonical(hashes))
    arsenals = load_arsenals(arsenal_index, fingerprint)
    text = MASTER.read_text()
    puzzles = []
    missing = set()
    for encounter_number, encounter in split_blocks(text, ENCOUNTER_RE):
        order = int(field(encounter, "campaign_order"))
        arsenal = arsenals.get(str(order))
        if arsenal is None:
            missing.add(order)
            continue
        branch_marks = list(BRANCH_RE.finditer(encounter))
        for index, mark in enumerate(branch_marks):
            branch = encounter[mark.start():branch_marks[index + 1].start() if index + 1 < len(branch_marks) else len(encounter)]
            team = []
            for row in MON_RE.finditer(branch):
                team.append({
                    "slot": int(row.group(1)), "species": row.group(2), "item": row.group(3),
                    "level": int(field(encounter, "strict_cap")) + int(row.group(4)),
                    "ability": row.group(5), "nature": row.group(6),
                    "stat_points": [int(value) for value in row.group(7).split("/")],
                    "moves": row.group(8).split(","),
                })
            dossier = {
                "trainer_id": field(branch, "trainer_id") or mark.group(1),
                "format": field(branch, "format"),
                "ai_profile": field(encounter, "ai_profile"),
                "ai_extra": field(branch, "ai_extra"),
                "team": team,
            }
            puzzle = {
                "puzzle_id": f"E{encounter_number}-{dossier['trainer_id']}",
                "encounter": int(encounter_number), "campaign_order": order,
                "chapter": field(encounter, "chapter"), "location": field(encounter, "location"),
                "strict_cap": int(field(encounter, "strict_cap")),
                "opponent_dossier": dossier, "legal_arsenal": arsenal,
                "provenance": {"opponent": "data/emerald_champions/emerald_champions_master_battle_design.txt", "arsenal_index": str(arsenal_index)},
            }
            puzzle["content_sha256"] = digest_bytes(canonical(puzzle))
            puzzles.append(puzzle)
    if missing:
        sample = ", ".join(map(str, sorted(missing)[:12]))
        raise SystemExit(f"arsenal index incomplete; missing campaign orders: {sample} ({len(missing)} total)")
    return {
        "schema_version": 1,
        "kind": "emerald_champions_independent_trainer_battle_suite",
        "source_fingerprint": fingerprint,
        "inputs": hashes,
        "puzzle_count": len(puzzles),
        "puzzles": puzzles,
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--arsenal-index", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    args = parser.parse_args()
    suite = generate(args.arsenal_index)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(suite, indent=2, sort_keys=True) + "\n")
    print(f"generated {suite['puzzle_count']} independent trainer battle puzzles: {args.out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
