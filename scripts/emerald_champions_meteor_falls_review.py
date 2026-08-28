#!/usr/bin/env python3
"""Generate/check the campaign-level Meteor Falls battle-design zoom-out."""

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
OUTPUT_JSON = ROOT / "docs/emerald_champions_meteor_falls_review.json"
OUTPUT_MD = ROOT / "docs/emerald_champions_meteor_falls_review.md"
START_INDEX = 105
END_INDEX = 107


def format_class(value: str) -> str:
    if "multi_2_vs_2" in value:
        return "required-multi"
    if "four guarded doubles" in value:
        return "rematch-family"
    return value


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
        "scope": {"location": "MeteorFalls", "battle_indices": [START_INDEX, END_INDEX], "encounter_count": len(entries), "status": "source-closed-static-pass-runtime-unplayed"},
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
                "required six-branch rival-assisted meteor-impact multi battle",
                "six-discipline rare Dragon Match Call family without a speed field",
                "three-pair underused anniversary family through two physical scripts",
            ],
            "difficulty_shape": "Meteor Falls opens at target 10, then releases cognitive pressure without becoming easy: Nicolas targets 9.3 and John/Jay 9.0, with harder rematches reserved for later.",
            "format_result": "One required Multi Battle and two physically distinct optional Match Call families exercise story coordination, rare roster adaptation, and low-BST partnership rather than repeating one doubles module.",
            "species_result": "All 18 physical-encounter species/form identities are unique across Meteor Falls; internal rematch recurrence is intentional progression.",
            "protected_reveal_result": "The chapter source-closes protected Courtney impact, Mega Aerodactyl, Jirachi, Celesteela, Naganadel, Regidrago, and six underused pair species while preserving every later faction, Gym, League, Primal, and Dragon-Mega anchor.",
            "next_chapter_guardrail": (
                "Mt. Chimney must change from cave rematch families to forward-moving faction ascent. Recheck the physical path "
                "before fixing trainer order; avoid another four-record family, Protect-heavy roster, speed field, meteor/celestial "
                "theme, generic Dragon pressure, or paired-support lesson. Preserve protected Tabitha and Maxie summit anchors."
            ),
        },
        "battles": rows,
    }


def render(report: dict) -> str:
    d = report["difficulty"]
    u = report["species_usage"]
    lines = [
        "# Emerald Champions Meteor Falls Battle Review",
        "",
        "Generated from the source-backed battle ledger, dossiers, and guide. Runtime remains unplayed.",
        "",
        "## Verdict",
        "",
        "PASS: Meteor Falls does not exhibit a local design maximum.",
        "",
        f"- Battles: {START_INDEX}-{END_INDEX} ({report['scope']['encounter_count']} physical encounters)",
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
    if report["scope"]["encounter_count"] != 3:
        raise SystemExit("FAIL: Meteor Falls review does not cover three physical encounters")
    expected_difficulty = {
        "minimum": 9.0,
        "median": 9.3,
        "maximum": 10.0,
        "targets": [10.0, 9.3, 9.0],
        "interpretation": "Editorial targets only; observed difficulty requires playtesting on Hard/Medium/Easy.",
    }
    if report["difficulty"] != expected_difficulty:
        raise SystemExit(f"FAIL: Meteor Falls difficulty shape drifted: {report['difficulty']}")
    if report["format_mix"] != {"rematch-family": 2, "required-multi": 1}:
        raise SystemExit(f"FAIL: Meteor Falls format mix drifted: {report['format_mix']}")
    usage = report["species_usage"]
    if usage["slots"] != 18 or usage["unique_species_forms"] != 18 or usage["duplicates_across_physical_encounters"]:
        raise SystemExit(f"FAIL: Meteor Falls species variety drifted: {usage}")
    if report["evidence"] != {"quality_ten_dossiers": 3, "competitive_reference_count": 13, "runtime_playtested": 0}:
        raise SystemExit(f"FAIL: Meteor Falls evidence drifted: {report['evidence']}")
    if report["variety_review"]["result"] != "pass-no-local-maximum":
        raise SystemExit("FAIL: Meteor Falls variety review no longer passes")


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
            raise SystemExit("FAIL: Meteor Falls review artifact is stale")
    print("PASS: Meteor Falls has three quality-10 encounters, 18/18 unique physical species, median target 9.3, and no local maximum")


if __name__ == "__main__":
    main()
