#!/usr/bin/env python3
"""Validate Verdant's durable route/Gym comparative chapter reviews."""

from __future__ import annotations

import json
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
REVIEWS_PATH = ROOT / "docs/verdant_chapter_reviews.json"
SEQUENCE_PATH = ROOT / "docs/verdant_battle_sequence.json"
LEDGER_PATH = ROOT / "docs/verdant_battle_experience_ledger.json"
SPECIES_PATH = ROOT / "docs/verdant_species_usage_ledger.json"


def load(path: Path) -> dict:
    return json.loads(path.read_text())


def main() -> None:
    payload = load(REVIEWS_PATH)
    sequence = {row["index"]: row for row in load(SEQUENCE_PATH)["entries"]}
    ledger = {row["index"]: row for row in load(LEDGER_PATH)["entries"]}
    species = load(SPECIES_PATH)["species"]
    problems: list[str] = []

    if payload.get("version") != 1 or not payload.get("reviews"):
        problems.append("chapter review payload is empty or has an unsupported version")

    seen_indexes: set[int] = set()
    for review in payload.get("reviews", []):
        chapter = review.get("chapter_id", "unknown")
        indexes = review.get("encounter_indexes", [])
        if not indexes or indexes != list(range(indexes[0], indexes[-1] + 1)):
            problems.append(f"{chapter}: encounter indexes are not one contiguous range")
            continue
        if seen_indexes & set(indexes):
            problems.append(f"{chapter}: encounter range overlaps another chapter review")
        seen_indexes.update(indexes)
        if any(index not in sequence or index not in ledger for index in indexes):
            problems.append(f"{chapter}: sequence or experience-ledger row is missing")
            continue

        sequence_rows = [sequence[index] for index in indexes]
        ledger_rows = [ledger[index] for index in indexes]
        expected_ids = [row["encounter_id"] for row in sequence_rows]
        if review.get("encounter_ids") != expected_ids:
            problems.append(f"{chapter}: encounter IDs do not match canonical sequence")
        if any(row.get("status") != "closed" for row in sequence_rows):
            problems.append(f"{chapter}: review includes an encounter that is not source-closed")
        strict_caps = {row.get("strict_cap") for row in sequence_rows}
        if strict_caps != {review.get("campaign_state", {}).get("strict_cap")}:
            problems.append(f"{chapter}: strict-cap summary differs from sequence {strict_caps}")

        formats = Counter(row.get("identity", {}).get("format") for row in ledger_rows)
        difficulties = [row.get("target_difficulty") for row in ledger_rows]
        if any(not isinstance(value, (int, float)) for value in difficulties):
            problems.append(f"{chapter}: target difficulty is missing")
            continue

        species_uses: dict[str, list[int]] = {}
        index_set = set(indexes)
        for species_row in species:
            uses = sorted(
                appearance["battle_index"]
                for appearance in species_row.get("appearances", [])
                if appearance["battle_index"] in index_set
            )
            if uses:
                species_uses[species_row["species"]] = uses
        repeats = sorted(name for name, uses in species_uses.items() if len(uses) > 1)

        expected_summary = {
            "source_closed_encounters": len(indexes),
            "format_counts": dict(sorted(formats.items())),
            "difficulty_min": min(difficulties),
            "difficulty_max": max(difficulties),
            "difficulty_average": round(sum(difficulties) / len(difficulties), 2),
            "exact_species_used": len(species_uses),
            "within_chapter_species_repeats": repeats,
        }
        if review.get("derived_summary") != expected_summary:
            problems.append(
                f"{chapter}: derived summary is stale; expected {expected_summary}, found {review.get('derived_summary')}"
            )

        findings = review.get("comparative_findings", {})
        required_findings = {
            "strongest_part", "weakest_link", "difficulty_disposition",
            "species_disposition", "rare_showcase_disposition",
            "mechanic_collision_disposition", "reward_and_story_disposition",
            "next_chapter_handoff",
        }
        if set(findings) != required_findings or not all(
            isinstance(findings[key], str) and findings[key].strip()
            for key in required_findings
        ):
            problems.append(f"{chapter}: comparative findings are incomplete")
        if review.get("runtime_status") != "static-review-runtime-unplayed":
            problems.append(f"{chapter}: runtime status must remain explicit and unplayed")
        if not review.get("required_runtime_samples") or not review.get("disposition"):
            problems.append(f"{chapter}: runtime samples or disposition are missing")

    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))
    print(f"PASS: {len(payload['reviews'])} chapter review(s) match canonical source-closed encounters and ledgers")
    print("PASS: chapter findings, weakest links, next handoffs, and unplayed runtime status are explicit")


if __name__ == "__main__":
    main()
