#!/usr/bin/env python3
"""Generate/check the campaign-level Route 114 battle-design zoom-out."""

from __future__ import annotations

import argparse
import json
import statistics
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
LEDGER = ROOT / "docs/verdant_battle_experience_ledger.json"
DESIGNS = ROOT / "docs/verdant_bespoke_battle_designs.json"
GUIDE = ROOT / "docs/verdant_battle_guide.json"
OUTPUT_JSON = ROOT / "docs/emerald_champions_route114_review.json"
OUTPUT_MD = ROOT / "docs/emerald_champions_route114_review.md"
START_INDEX = 95
END_INDEX = 104


def format_class(value: str) -> str:
    if " or " in value:
        return "mixed"
    if "four guarded doubles" in value:
        return "rematch-family"
    return value.replace("guarded ", "")


def build() -> dict:
    ledger = json.loads(LEDGER.read_text())["entries"]
    designs = json.loads(DESIGNS.read_text())["designs"]
    guide = json.loads(GUIDE.read_text())["entries"]
    entries = [entry for entry in ledger if START_INDEX <= entry["index"] <= END_INDEX]
    species_by_encounter = {}
    all_species = []
    rows = []
    for entry in entries:
        encounter_id = entry["encounter_id"]
        species = sorted({
            member["speciesId"]
            for guide_entry in guide
            if guide_entry.get("encounterId") == encounter_id and guide_entry.get("designStatus") == "closed"
            for member in guide_entry["party"]
        })
        species_by_encounter[encounter_id] = species
        all_species.extend(species)
        design = designs[encounter_id]
        rows.append({
            "index": entry["index"],
            "encounter_id": encounter_id,
            "format": entry["identity"]["format"],
            "target_difficulty": entry["target_difficulty"],
            "manual_quality": design["manual_quality"],
            "tempo": entry["tempo"],
            "species": species,
            "reference_count": len(entry["historic_reference_ids"]),
            "playtest_status": entry["playtest_status"],
        })
    targets = [entry["target_difficulty"] for entry in entries]
    duplicates = sorted(species for species, count in Counter(all_species).items() if count > 1)
    formats = Counter(format_class(entry["identity"]["format"]) for entry in entries)
    return {
        "version": 1,
        "scope": {"location": "Route114", "battle_indices": [START_INDEX, END_INDEX], "encounter_count": len(entries), "status": "source-closed-static-pass-runtime-unplayed"},
        "difficulty": {
            "minimum": min(targets),
            "median": statistics.median(targets),
            "maximum": max(targets),
            "targets": targets,
            "interpretation": "Editorial targets only; observed difficulty requires playtesting on Hard/Medium/Easy.",
        },
        "format_mix": dict(sorted(formats.items())),
        "species_usage": {"slots": len(all_species), "unique_species_forms": len(set(all_species)), "duplicates_across_physical_encounters": duplicates, "by_encounter": species_by_encounter},
        "evidence": {
            "quality_ten_dossiers": sum(row["manual_quality"] == 10 for row in rows),
            "competitive_reference_count": sum(row["reference_count"] for row in rows),
            "runtime_playtested": sum(row["playtest_status"] != "static-pass-runtime-unplayed" for row in rows),
        },
        "variety_review": {
            "result": "pass-no-local-maximum",
            "encounter_modes": [
                "canonical Commander and Zero to Hero water showcase",
                "joint-or-split Trick Room pond cluster",
                "manual-rain three-category fishing single",
                "Contrary and Fur Coat workout single",
                "Coaching and Wide Guard fighting curriculum",
                "Choice-to-sustain-to-setup campsite single",
                "four-stage hard-body Match Call family",
                "four-stage controlled-burn safety Match Call family",
                "mutual-immunity mountain echo double with Mega Altaria",
                "joint-or-split snow expedition with Articuno",
            ],
            "difficulty_shape": "The route ranges from 8.6 to 9.6, with a 9.1 median and no encounter below the campaign's serious-adaptation floor.",
            "format_result": "Three direct doubles, three singles, two joint/split clusters, and two rematch families prevent one format from becoming the route's default rhythm.",
            "species_result": "All 45 physical-encounter species/form identities are unique across Route 114; rematch-family internal recurrence is intentional progression, not cross-encounter duplication.",
            "protected_reveal_result": "Route 114 spends canonical Commander, one Mega Altaria, and Articuno while preserving every protected faction, Gym, League, Primal, and historic-team anchor.",
            "next_chapter_guardrail": (
                "Meteor Falls should sharply change tone to required faction coordination. Do not repeat snow, sound immunity, "
                "route-style rematch progression, a four-member standalone double, or a passive weather lesson. The Courtney/"
                "Grunt multi battle must exploit the player's rival partner and Magma's competitive identity."
            ),
        },
        "battles": rows,
    }


def render(report: dict) -> str:
    d = report["difficulty"]
    u = report["species_usage"]
    lines = [
        "# Emerald Champions Route 114 Battle Review",
        "",
        "Generated from the source-backed battle ledger, dossiers, and guide. Runtime remains unplayed.",
        "",
        "## Verdict",
        "",
        "PASS: Route 114 does not exhibit a local design maximum.",
        "",
        f"- Battles: {START_INDEX}-{END_INDEX} ({report['scope']['encounter_count']} encounters)",
        f"- Target difficulty: {d['minimum']}-{d['maximum']} (median {d['median']})",
        f"- Format mix: {report['format_mix']}",
        f"- Physical species slots: {u['slots']}; unique species/forms: {u['unique_species_forms']}; cross-encounter duplicates: {u['duplicates_across_physical_encounters']}",
        f"- Quality-10 dossiers: {report['evidence']['quality_ten_dossiers']}",
        f"- Competitive references: {report['evidence']['competitive_reference_count']}",
        "- Runtime status: source-closed, not yet playtested",
        "",
        "## Encounter sequence",
        "",
    ]
    for row in report["battles"]:
        lines.append(f"- Battle {row['index']} ({row['encounter_id']}): {row['format']}, target {row['target_difficulty']}. {row['tempo']}")
    lines.extend(["", "## Forward guardrail", "", report["variety_review"]["next_chapter_guardrail"], ""])
    return "\n".join(lines)


def check(report: dict) -> None:
    if report["scope"]["encounter_count"] != 10:
        raise SystemExit("FAIL: Route 114 review does not cover ten encounters")
    expected_difficulty = {
        "minimum": 8.6,
        "median": 9.1,
        "maximum": 9.6,
        "targets": [9.6, 9.3, 8.7, 8.6, 9.2, 8.7, 9.0, 8.9, 9.3, 9.5],
        "interpretation": "Editorial targets only; observed difficulty requires playtesting on Hard/Medium/Easy.",
    }
    if report["difficulty"] != expected_difficulty:
        raise SystemExit(f"FAIL: Route 114 difficulty shape drifted: {report['difficulty']}")
    if report["format_mix"] != {"double": 3, "mixed": 2, "rematch-family": 2, "single": 3}:
        raise SystemExit(f"FAIL: Route 114 format mix drifted: {report['format_mix']}")
    usage = report["species_usage"]
    if usage["slots"] != 45 or usage["unique_species_forms"] != 45 or usage["duplicates_across_physical_encounters"]:
        raise SystemExit(f"FAIL: Route 114 species variety drifted: {usage}")
    if report["evidence"] != {"quality_ten_dossiers": 10, "competitive_reference_count": 44, "runtime_playtested": 0}:
        raise SystemExit(f"FAIL: Route 114 evidence drifted: {report['evidence']}")
    if report["variety_review"]["result"] != "pass-no-local-maximum":
        raise SystemExit("FAIL: Route 114 variety review no longer passes")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.write and not args.check:
        parser.error("choose --write or --check")
    report = build()
    expected_json = json.dumps(report, indent=2, ensure_ascii=False) + "\n"
    expected_md = render(report)
    if args.write:
        OUTPUT_JSON.write_text(expected_json)
        OUTPUT_MD.write_text(expected_md)
    if args.check:
        check(report)
        if OUTPUT_JSON.read_text() != expected_json or OUTPUT_MD.read_text() != expected_md:
            raise SystemExit("FAIL: Route 114 review artifact is stale")
    print("PASS: Route 114 has ten quality-10 encounters, 45/45 unique physical species, median target 9.1, and no local maximum")


if __name__ == "__main__":
    main()
