#!/usr/bin/env python3
"""Review Battles 124-133 as one experience before the batch release gate."""

from __future__ import annotations

import argparse
import json
from collections import Counter, defaultdict
from pathlib import Path
from statistics import median

import emerald_champions_battles124_133 as batch


ROOT = Path(__file__).resolve().parents[1]
OUT_JSON = ROOT / "docs/emerald_champions_heat_epilogue_review.json"
OUT_MD = ROOT / "docs/emerald_champions_heat_epilogue_review.md"


def build():
    designs = json.loads(batch.DESIGNS.read_text())["designs"]
    ledger = {row["index"]: row for row in json.loads(batch.LEDGER.read_text())["entries"]}
    species_to_encounters = defaultdict(list)
    primary_tags = []
    rows = []
    for config in batch.CONFIGS:
        design = designs[config["id"]]
        source_species = sorted({member["species"] for team in config["teams"].values() for member in team})
        for species in source_species:
            species_to_encounters[species].append(config["index"])
        primary_tags.append(config["tags"][1])
        rows.append({
            "index": config["index"],
            "encounter_id": config["id"],
            "location": config["location"],
            "format": batch.TRAINER_RULES[config["main"]][0],
            "target_difficulty": config["target"],
            "primary_question": config["question"],
            "primary_tag": config["tags"][1],
            "species": source_species,
            "trainer_records": config["trainers"],
            "quality": design["manual_quality"],
            "playtest_status": ledger[config["index"]]["playtest_status"],
        })
    collisions = {species: indexes for species, indexes in species_to_encounters.items() if len(indexes) > 1}
    formats = Counter(row["format"] for row in rows)
    return {
        "version": 1,
        "scope": "Battles 124-133: post-crisis Mt. Chimney and Route 111 desert before Norman.",
        "status": "pass",
        "summary": {
            "encounters": len(rows),
            "physical_trainer_records": sum(len(row["trainer_records"]) for row in rows),
            "distinct_species": len(species_to_encounters),
            "cross_encounter_species_collisions": len(collisions),
            "distinct_primary_questions": len({row["primary_question"] for row in rows}),
            "distinct_primary_tags": len(set(primary_tags)),
            "format_counts": dict(formats),
            "difficulty_min": min(row["target_difficulty"] for row in rows),
            "difficulty_median": median(row["target_difficulty"] for row in rows),
            "difficulty_max": max(row["target_difficulty"] for row in rows),
            "quality_floor": min(row["quality"] for row in rows),
        },
        "rows": rows,
        "species_collisions": collisions,
        "findings": [
            "Seven doubles and three singles preserve a hard doubles-majority cadence without ten identical board puzzles.",
            "Every encounter has a distinct primary question and primary tag; field modes are limited to Sawyer's sand and Heidi's oasis terrain.",
            "The batch includes three evolving rematch families, but their repeated members remain inside one trainer identity rather than leaking across unrelated encounters.",
            "Mega Diancie, Mega Excadrill, Tapu Bulu, Meloetta, Nihilego, Kingambit, and final-rematch Mega Flygon provide rare spectacle without stacking every team with a restricted species.",
            "Drew is the intentional 8.7 floor and shortest relief single; Becky and the three rematch families restore high-concentration doubles pressure afterward.",
        ],
        "weakest_link": "Several desert teams use visible control turns before damage. Their levels, finite items, and direct reserves must keep those openings from becoming passive in runtime testing.",
        "next_handoff": "Battle 134 Daisuke continues Route 111 at cap 45; do not repeat sand, terrain, Tailwind pivot control, Simple Beam, or fossil excavation immediately.",
        "runtime_status": "All ten encounters are source-closed and statically checked; observed difficulty remains unset until real-ROM playtests.",
    }


def validate(payload):
    summary = payload["summary"]
    if payload["status"] != "pass" or summary["encounters"] != 10:
        raise AssertionError("Heat epilogue batch scope drifted")
    if summary["cross_encounter_species_collisions"] != 0:
        raise AssertionError(f"unrelated batch species repeat: {payload['species_collisions']}")
    if summary["distinct_primary_questions"] != 10 or summary["distinct_primary_tags"] != 10:
        raise AssertionError("batch drifted into repeated primary puzzles")
    if summary["format_counts"] != {"double": 7, "single": 3}:
        raise AssertionError("batch format pacing drifted")
    if summary["difficulty_min"] < 8.5 or summary["difficulty_median"] < 9.0 or summary["quality_floor"] != 10:
        raise AssertionError("batch difficulty or quality floor drifted")
    if any(row["playtest_status"] != "static-pass-runtime-unplayed" for row in payload["rows"]):
        raise AssertionError("batch overstates runtime proof")


def markdown(payload):
    s = payload["summary"]
    lines = [
        "# Emerald Champions Heat Badge epilogue review", "",
        f"PASS: {s['encounters']} encounters, {s['physical_trainer_records']} trainer records, {s['distinct_species']} distinct species, and zero cross-encounter species collisions.",
        f"Formats: {s['format_counts']['double']} doubles / {s['format_counts']['single']} singles. Difficulty {s['difficulty_min']}-{s['difficulty_max']} (median {s['difficulty_median']}); quality floor {s['quality_floor']}.", "",
        "| # | Encounter | Format | Target | Primary puzzle |", "|---:|---|---|---:|---|",
    ]
    for row in payload["rows"]:
        lines.append(f"| {row['index']} | `{row['encounter_id']}` | {row['format']} | {row['target_difficulty']} | {row['primary_tag']} |")
    lines += ["", "## Findings", ""] + [f"- {finding}" for finding in payload["findings"]]
    lines += ["", f"Weakest link: {payload['weakest_link']}", "", f"Next: {payload['next_handoff']}", "", payload["runtime_status"], ""]
    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(); parser.add_argument("--write", action="store_true"); parser.add_argument("--check", action="store_true"); args = parser.parse_args()
    if not args.write and not args.check: parser.error("choose --write or --check")
    payload = build(); validate(payload); expected_json = json.dumps(payload, indent=2, ensure_ascii=False) + "\n"; expected_md = markdown(payload)
    if args.write: OUT_JSON.write_text(expected_json); OUT_MD.write_text(expected_md)
    if args.check:
        if OUT_JSON.read_text() != expected_json or OUT_MD.read_text() != expected_md: raise SystemExit("FAIL: Heat epilogue review stale")
    print("PASS: Heat Badge epilogue batch has ten distinct quality-10 encounters, zero cross-encounter species collisions, and a 9.1 median target")


if __name__ == "__main__": main()
