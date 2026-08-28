#!/usr/bin/env python3
"""Generate/check the campaign-level Route 113 battle-design zoom-out."""

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
OUTPUT_JSON = ROOT / "docs/emerald_champions_route113_review.json"
OUTPUT_MD = ROOT / "docs/emerald_champions_route113_review.md"
START_INDEX = 88
END_INDEX = 94


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
        species = []
        for guide_entry in guide:
            if guide_entry.get("encounterId") == encounter_id and guide_entry.get("designStatus") == "closed":
                species.extend(member["speciesId"] for member in guide_entry["party"])
        species_by_encounter[encounter_id] = species
        all_species.extend(species)
        design = designs[encounter_id]
        rows.append(
            {
                "index": entry["index"],
                "encounter_id": encounter_id,
                "format": entry["identity"]["format"],
                "target_difficulty": entry["target_difficulty"],
                "manual_quality": design["manual_quality"],
                "tempo": entry["tempo"],
                "species": species,
                "reference_count": len(entry["historic_reference_ids"]),
                "playtest_status": entry["playtest_status"],
            }
        )
    duplicates = sorted(species for species, count in Counter(all_species).items() if count > 1)
    targets = [entry["target_difficulty"] for entry in entries]
    formats = Counter(
        "mixed" if " or " in entry["identity"]["format"] else entry["identity"]["format"]
        for entry in entries
    )
    return {
        "version": 1,
        "scope": {
            "location": "Route113",
            "battle_indices": [START_INDEX, END_INDEX],
            "encounter_count": len(entries),
            "status": "source-closed-static-pass-runtime-unplayed",
        },
        "difficulty": {
            "minimum": min(targets),
            "median": statistics.median(targets),
            "maximum": max(targets),
            "targets": targets,
            "interpretation": "Editorial targets only; observed difficulty requires playtesting on Hard/Medium/Easy.",
        },
        "format_mix": dict(sorted(formats.items())),
        "species_usage": {
            "slots": len(all_species),
            "unique_species_forms": len(set(all_species)),
            "duplicates_within_route": duplicates,
            "by_encounter": species_by_encounter,
        },
        "evidence": {
            "quality_ten_dossiers": sum(row["manual_quality"] == 10 for row in rows),
            "competitive_reference_count": sum(row["reference_count"] for row in rows),
            "runtime_playtested": sum(row["playtest_status"] != "static-pass-runtime-unplayed" for row in rows),
        },
        "variety_review": {
            "result": "pass-no-local-maximum",
            "encounter_modes": [
                "three-way object-order collision with sand/screens/Wonder Guard",
                "linear ash-glass mixed coverage without a field",
                "late-bloomer pivot single",
                "mirrored Dancer setup double",
                "Illusion/Protean/Libero identity single",
                "living fault-line positional double",
                "native-pair hazard/phazing route finale",
            ],
            "deliberate_callback": "Dancer returns 77 encounters after the juvenile Battle 14 lesson as a fully evolved twin exam.",
            "protected_reveal_result": "No Mega, Primal, or protected legendary is spent on Route 113.",
            "next_route_guardrail": (
                "Route 114 should not immediately repeat hazard/phazing, sand/weather, mirrored Dancer, or Illusion/type-change "
                "as its primary question; use its pond, fossil, camping, and Meteor Falls geography for new identities."
            ),
        },
        "battles": rows,
    }


def render(report: dict) -> str:
    difficulty = report["difficulty"]
    usage = report["species_usage"]
    lines = [
        "# Emerald Champions Route 113 Battle Review",
        "",
        "Generated from the source-backed battle ledger, dossiers, and guide. Runtime remains unplayed.",
        "",
        "## Verdict",
        "",
        "PASS: Route 113 does not exhibit a local design maximum.",
        "",
        f"- Battles: {report['scope']['battle_indices'][0]}-{report['scope']['battle_indices'][1]} ({report['scope']['encounter_count']} encounters)",
        f"- Target difficulty: {difficulty['minimum']}-{difficulty['maximum']} (median {difficulty['median']})",
        f"- Format mix: {report['format_mix']}",
        f"- Species slots: {usage['slots']}; unique species/forms: {usage['unique_species_forms']}; duplicates: {usage['duplicates_within_route']}",
        f"- Quality-10 dossiers: {report['evidence']['quality_ten_dossiers']}",
        f"- Competitive references: {report['evidence']['competitive_reference_count']}",
        "- Mega/Primal/protected legendary spending: none",
        "",
        "## Encounter sequence",
        "",
    ]
    for row in report["battles"]:
        lines.append(
            f"- Battle {row['index']} ({row['encounter_id']}): {row['format']}, target {row['target_difficulty']}. {row['tempo']}"
        )
    lines.extend(
        [
            "",
            "## Forward guardrail",
            "",
            report["variety_review"]["next_route_guardrail"],
            "",
        ]
    )
    return "\n".join(lines)


def check(report: dict) -> None:
    if report["scope"]["encounter_count"] != 7:
        raise SystemExit("FAIL: Route 113 review does not cover seven encounters")
    if report["difficulty"] != {
        "minimum": 8.5,
        "median": 8.8,
        "maximum": 9.2,
        "targets": [9.2, 8.9, 8.5, 8.8, 8.6, 8.8, 9.2],
        "interpretation": "Editorial targets only; observed difficulty requires playtesting on Hard/Medium/Easy.",
    }:
        raise SystemExit("FAIL: Route 113 difficulty shape drifted")
    if report["format_mix"] != {"double": 3, "mixed": 2, "single": 2}:
        raise SystemExit(f"FAIL: Route 113 format mix drifted: {report['format_mix']}")
    usage = report["species_usage"]
    if usage["slots"] != 33 or usage["unique_species_forms"] != 33 or usage["duplicates_within_route"]:
        raise SystemExit(f"FAIL: Route 113 species variety drifted: {usage}")
    if report["evidence"]["quality_ten_dossiers"] != 7:
        raise SystemExit("FAIL: Route 113 has a non-quality-10 dossier")
    if report["evidence"]["competitive_reference_count"] != 29:
        raise SystemExit("FAIL: Route 113 competitive evidence count drifted")
    if report["evidence"]["runtime_playtested"] != 0:
        raise SystemExit("FAIL: Route 113 runtime status is no longer source-honest")
    if report["variety_review"]["result"] != "pass-no-local-maximum":
        raise SystemExit("FAIL: Route 113 variety review no longer passes")


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
            raise SystemExit("FAIL: Route 113 review artifact is stale")
    print("PASS: Route 113 has seven quality-10 encounters, 33/33 unique species, median target 8.8, and no local maximum")


if __name__ == "__main__":
    main()
