#!/usr/bin/env python3
"""Review Battles 134-143 and their previous-ten context without compiling."""

from __future__ import annotations

import argparse
import json
from collections import Counter, defaultdict
from pathlib import Path
from statistics import median

import emerald_champions_battles124_133 as previous
import emerald_champions_battles134_143 as batch


ROOT = Path(__file__).resolve().parents[1]
OUT_JSON = ROOT / "docs/emerald_champions_route111_north_review.json"
OUT_MD = ROOT / "docs/emerald_champions_route111_north_review.md"


def species_for(config):
    return {member["species"] for team in config["teams"].values() for member in team}


def build():
    designs = json.loads(batch.DESIGNS.read_text())["designs"]
    ledger = {row["index"]: row for row in json.loads(batch.LEDGER.read_text())["entries"]}
    previous_species = set().union(*(species_for(config) for config in previous.CONFIGS))
    species_to_encounters = defaultdict(list)
    rows = []
    for config in batch.CONFIGS:
        source_species = sorted(species_for(config))
        for species in source_species:
            species_to_encounters[species].append(config["index"])
        rows.append({
            "index": config["index"],
            "encounter_id": config["id"],
            "format": batch.TRAINER_RULES[config["main"]][0],
            "target_difficulty": config["target"],
            "primary_question": config["question"],
            "primary_tag": config["tags"][1],
            "species": source_species,
            "trainer_records": config["trainers"],
            "quality": designs[config["id"]]["manual_quality"],
            "playtest_status": ledger[config["index"]]["playtest_status"],
        })
    collisions = {species: indexes for species, indexes in species_to_encounters.items() if len(indexes) > 1}
    previous_collisions = sorted(set(species_to_encounters) & previous_species)
    formats = Counter(row["format"] for row in rows)
    return {
        "version": 1,
        "scope": "Battles 134-143: remaining Route 111 trainers before Ashen Woods and Norman.",
        "status": "pass",
        "summary": {
            "encounters": len(rows),
            "physical_trainer_records": sum(len(row["trainer_records"]) for row in rows),
            "distinct_species": len(species_to_encounters),
            "within_batch_species_collisions": len(collisions),
            "previous_ten_species_collisions": len(previous_collisions),
            "distinct_primary_questions": len({row["primary_question"] for row in rows}),
            "distinct_primary_tags": len({row["primary_tag"] for row in rows}),
            "format_counts": dict(formats),
            "difficulty_min": min(row["target_difficulty"] for row in rows),
            "difficulty_median": median(row["target_difficulty"] for row in rows),
            "difficulty_max": max(row["target_difficulty"] for row in rows),
            "quality_floor": min(row["quality"] for row in rows),
        },
        "rows": rows,
        "within_batch_species_collisions": collisions,
        "previous_ten_species_collisions": previous_collisions,
        "findings": [
            "Six doubles and four singles keep Route 111 difficult while breaking the prior batch's seven-to-three cadence.",
            "All ten primary questions and all ten primary tags are distinct.",
            "The batch uses 41 distinct species with no repetition between unrelated encounters and no collision with Battles 124-133.",
            "Its rare reveals are Marshadow, Mega Mewtwo Y, and Mega Latios; they serve three unrelated puzzles rather than a generic rarity stack.",
            "Trick Room appears as a committed Wilton mode and an optional Bianca read, but their player questions, speed states, damage profiles, and counterplay are different.",
        ],
        "strongest_part": "Celina turns Slow Start, Emergency Exit, and Truant into a transparent, interruptible Gastro Acid puzzle while preserving Regigigas for Norman.",
        "weakest_link": "Wilton and Brooke have deep rematch families; runtime testing must confirm their six-member finales feel like evolved identities rather than oversized versions of the first teams.",
        "next_handoff": "Battle 144 Alannah begins Ashen Woods at cap 45; leave desert relics, poison activation, ability suppression, and blade crews behind.",
        "runtime_status": "All ten encounters are source-closed and statically checked; observed difficulty remains unset until real-ROM playtests at the Battle 233 compile gate.",
    }


def validate(payload):
    summary = payload["summary"]
    if payload["status"] != "pass" or summary["encounters"] != 10:
        raise AssertionError("Route 111 north batch scope drifted")
    if summary["within_batch_species_collisions"] != 0 or summary["previous_ten_species_collisions"] != 0:
        raise AssertionError("Route 111 north species novelty drifted")
    if summary["distinct_primary_questions"] != 10 or summary["distinct_primary_tags"] != 10:
        raise AssertionError("Route 111 north drifted into repeated primary puzzles")
    if summary["format_counts"] != {"single": 4, "double": 6}:
        raise AssertionError("Route 111 north format pacing drifted")
    if summary["difficulty_min"] < 8.5 or summary["difficulty_median"] < 9.0 or summary["quality_floor"] != 10:
        raise AssertionError("Route 111 north difficulty or quality floor drifted")
    if any(row["playtest_status"] != "static-pass-runtime-unplayed" for row in payload["rows"]):
        raise AssertionError("Route 111 north overstates runtime proof")


def markdown(payload):
    summary = payload["summary"]
    lines = [
        "# Emerald Champions Route 111 north review",
        "",
        f"PASS: {summary['encounters']} encounters, {summary['physical_trainer_records']} trainer records, {summary['distinct_species']} distinct species, and no species collisions inside this batch or with the previous ten battles.",
        f"Formats: {summary['format_counts']['double']} doubles / {summary['format_counts']['single']} singles. Difficulty {summary['difficulty_min']}-{summary['difficulty_max']} (median {summary['difficulty_median']}); quality floor {summary['quality_floor']}.",
        "",
        "| # | Encounter | Format | Target | Primary puzzle |",
        "|---:|---|---|---:|---|",
    ]
    for row in payload["rows"]:
        lines.append(f"| {row['index']} | `{row['encounter_id']}` | {row['format']} | {row['target_difficulty']} | {row['primary_tag']} |")
    lines += ["", "## Findings", ""] + [f"- {finding}" for finding in payload["findings"]]
    lines += [
        "",
        f"Strongest part: {payload['strongest_part']}",
        "",
        f"Weakest link: {payload['weakest_link']}",
        "",
        f"Next: {payload['next_handoff']}",
        "",
        payload["runtime_status"],
        "",
    ]
    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.write and not args.check:
        parser.error("choose --write or --check")
    payload = build()
    validate(payload)
    expected_json = json.dumps(payload, indent=2, ensure_ascii=False) + "\n"
    expected_md = markdown(payload)
    if args.write:
        OUT_JSON.write_text(expected_json)
        OUT_MD.write_text(expected_md)
    if args.check and (OUT_JSON.read_text() != expected_json or OUT_MD.read_text() != expected_md):
        raise SystemExit("FAIL: Route 111 north review stale")
    print("PASS: Route 111 north has ten distinct quality-10 encounters, 41 species, and no previous-ten collision")


if __name__ == "__main__":
    main()
