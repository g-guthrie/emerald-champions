#!/usr/bin/env python3
"""Prove the existing Gym and Wally rematch families are coherent source-authored anchors."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

import verdant_team_quality_audit as quality  # noqa: E402

OUTPUT_JSON = ROOT / "docs/emerald_champions_rematch_family_audit.json"
OUTPUT_MD = ROOT / "docs/emerald_champions_rematch_family_audit.md"

GYM_FAMILIES = {
    "ROXANNE": ["TRAINER_ROXANNE_2", "TRAINER_ROXANNE_3", "TRAINER_ROXANNE_4", "TRAINER_ROXANNE_5"],
    "BRAWLY": ["TRAINER_BRAWLY_2", "TRAINER_BRAWLY_3", "TRAINER_BRAWLY_4", "TRAINER_BRAWLY_5"],
    "WATTSON": ["TRAINER_WATTSON_2", "TRAINER_WATTSON_3", "TRAINER_WATTSON_4", "TRAINER_WATTSON_5"],
    "FLANNERY": ["TRAINER_FLANNERY_2", "TRAINER_FLANNERY_3", "TRAINER_FLANNERY_4", "TRAINER_FLANNERY_5"],
    "NORMAN": ["TRAINER_NORMAN_2", "TRAINER_NORMAN_3", "TRAINER_NORMAN_4", "TRAINER_NORMAN_5"],
    "WINONA": ["TRAINER_WINONA_2", "TRAINER_WINONA_3", "TRAINER_WINONA_4", "TRAINER_WINONA_5"],
    "TATE_AND_LIZA": ["TRAINER_TATE_AND_LIZA_2", "TRAINER_TATE_AND_LIZA_3", "TRAINER_TATE_AND_LIZA_4"],
    "JUAN": ["TRAINER_JUAN_2", "TRAINER_JUAN_3", "TRAINER_JUAN_4", "TRAINER_JUAN_5"],
    "WALLY": ["TRAINER_WALLY_VR_2", "TRAINER_WALLY_VR_3", "TRAINER_WALLY_VR_4", "TRAINER_WALLY_VR_5"],
}

FINAL_TIER = {
    "ROXANNE": "TRAINER_ROXANNE_5", "BRAWLY": "TRAINER_BRAWLY_5", "WATTSON": "TRAINER_WATTSON_5",
    "FLANNERY": "TRAINER_FLANNERY_5", "NORMAN": "TRAINER_NORMAN_5", "WINONA": "TRAINER_WINONA_5",
    "TATE_AND_LIZA": "TRAINER_TATE_AND_LIZA_4", "JUAN": "TRAINER_JUAN_5", "WALLY": "TRAINER_WALLY_VR_5",
}

MIXED_FORMAT_CONTRACTS = {
    "FLANNERY": ["single", "single", "double", "double"],
}


def build():
    audit = quality.audit()
    by_id = {team["trainer_id"]: team for team in audit["teams"]}
    battle_setup = (ROOT / "src/battle_setup.c").read_text()
    map_scripts = "\n".join(path.read_text() for path in (ROOT / "data/maps").glob("*/scripts.inc"))
    families = {}
    all_scores = []
    for family, trainer_ids in GYM_FAMILIES.items():
        records = []
        for tier, trainer_id in enumerate(trainer_ids, 2):
            team = by_id[trainer_id]
            all_scores.append(team["quality_score"])
            records.append({
                "tier": tier,
                "trainer_id": trainer_id,
                "format": team["format"],
                "party_size": team["party_size"],
                "quality_score": team["quality_score"],
                "issues": team["issues"],
                "mega_count": team["mega_count"],
                "item_coverage": team["item_coverage"],
                "complete_moveset_coverage": team["complete_moveset_coverage"],
                "level_offsets": [mon["level_offset"] for mon in team["mons"]],
                "team": [{
                    "species": mon["species"], "item": mon["item"], "ability": mon["ability"],
                    "spread": mon["spread"], "moves": mon["moves"], "role": mon.get("role"),
                } for mon in team["mons"]],
                "source_reachability": "daily-postgame-script" if trainer_id.endswith("_5") and trainer_id != "TRAINER_WALLY_VR_5" else "rematch-table-or-Wally-chain",
            })
        families[family] = {
            "status": {"design": "source-reviewed", "source": "implemented", "static": "validated", "runtime": "unplayed"},
            "records": records,
            "final_tier": FINAL_TIER[family],
            "final_target_difficulty": 10,
            "final_observed_difficulty": None,
            "format_contract": "mixed singles/doubles" if family in MIXED_FORMAT_CONTRACTS else "single" if family == "NORMAN" else "double",
            "progression_rule": "Earlier tiers remain distinct source-authored progression teams. The final tier is the marquee target-10 rematch; deliberate leader signatures and famous species may recur here.",
        }
    return {
        "version": 1,
        "title": "Emerald Champions Gym and Wally rematch family audit",
        "scope": "All 35 reachable Gym/Wally rematch records. TRAINER_TATE_AND_LIZA_5 is deliberately reported as an unreachable internal definition, not a physical battle.",
        "families": families,
        "totals": {
            "families": len(families), "reachable_records": sum(len(family["records"]) for family in families.values()),
            "pokemon_sets": sum(len(record["team"]) for family in families.values() for record in family["records"]),
            "minimum_quality_score": min(all_scores), "maximum_quality_score": max(all_scores),
            "records_with_issues": sum(bool(record["issues"]) for family in families.values() for record in family["records"]),
            "records_with_full_items": sum(record["item_coverage"] == 1 for family in families.values() for record in family["records"]),
            "records_with_full_moves": sum(record["complete_moveset_coverage"] == 1 for family in families.values() for record in family["records"]),
        },
        "reachability_evidence": {
            "rematch_table_source": "src/battle_setup.c",
            "tier5_script_source": "data/maps/*/scripts.inc",
            "rematch_macros_present": {family: f"REMATCH_{family}" in battle_setup for family in GYM_FAMILIES if family != "WALLY"},
            "wally_macro_present": "REMATCH_WALLY_3" in battle_setup,
            "reachable_tier5": sorted(set(re.findall(r"TRAINER_(?:ROXANNE|BRAWLY|WATTSON|FLANNERY|NORMAN|WINONA|JUAN)_5", map_scripts))),
            "tate_liza_5_referenced": "TRAINER_TATE_AND_LIZA_5" in map_scripts or "TRAINER_TATE_AND_LIZA_5" in battle_setup,
        },
        "policy": {
            "do_not_global_dedupe_rematches": True,
            "reason": "A rematch is the intentional return and evolution of a known leader. Reusing one signature or iconic species is progression, not encounter inflation.",
            "runtime_gate": "Target 10 is a design target only. Observed difficulty remains null until real-ROM final-tier tests on Hard, Medium, and Easy.",
        },
    }


def validate(payload):
    totals = payload["totals"]
    if totals["families"] != 9 or totals["reachable_records"] != 35 or totals["pokemon_sets"] != 210:
        raise AssertionError("rematch family scope drifted")
    if totals["records_with_issues"] != 0 or totals["records_with_full_items"] != 35 or totals["records_with_full_moves"] != 35:
        raise AssertionError("rematch source quality gate failed")
    if totals["minimum_quality_score"] < 79:
        raise AssertionError("rematch quality floor drifted")
    evidence = payload["reachability_evidence"]
    if not all(evidence["rematch_macros_present"].values()) or not evidence["wally_macro_present"]:
        raise AssertionError("rematch table wiring drifted")
    if len(evidence["reachable_tier5"]) != 7 or evidence["tate_liza_5_referenced"]:
        raise AssertionError("tier-5 reachability classification drifted")
    for family_name, family in payload["families"].items():
        if family["final_target_difficulty"] != 10 or family["final_observed_difficulty"] is not None:
            raise AssertionError(f"{family_name} overstates runtime difficulty")
        expected_formats = MIXED_FORMAT_CONTRACTS.get(
            family_name,
            [family["format_contract"]] * len(family["records"]),
        )
        if [record["format"] for record in family["records"]] != expected_formats:
            raise AssertionError(f"{family_name} format drifted")
        if any(record["party_size"] != 6 or len(record["team"]) != 6 for record in family["records"]):
            raise AssertionError(f"{family_name} party size drifted")
        if family["final_tier"] not in {record["trainer_id"] for record in family["records"]}:
            raise AssertionError(f"{family_name} final tier missing")


def markdown(payload):
    t = payload["totals"]
    lines = [
        "# Emerald Champions Gym and Wally rematch family audit", "",
        f"PASS: {t['families']} families, {t['reachable_records']} reachable records, {t['pokemon_sets']} exact Pokemon sets.",
        f"Quality range {t['minimum_quality_score']}-{t['maximum_quality_score']}; every record has full items, full moves, and zero detected issues.", "",
    ]
    for name, family in payload["families"].items():
        lines += [f"## {name}", "", f"- Format: {family['format_contract']}; final target: 10; observed: unplayed.", f"- Final record: `{family['final_tier']}`.", "- Tiers:"]
        for record in family["records"]:
            lines.append(f"  - `{record['trainer_id']}` — quality {record['quality_score']}, Mega count {record['mega_count']}: " + ", ".join(mon["species"] for mon in record["team"]))
        lines.append("")
    lines += ["## Reachability note", "", "`TRAINER_TATE_AND_LIZA_5` exists internally but is not referenced by a map script or the rematch table. It is not counted as a physical encounter.", ""]
    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(); parser.add_argument("--write", action="store_true"); parser.add_argument("--check", action="store_true"); args = parser.parse_args()
    if not args.write and not args.check: parser.error("choose --write or --check")
    payload = build(); validate(payload)
    expected_json = json.dumps(payload, indent=2, ensure_ascii=False) + "\n"; expected_md = markdown(payload)
    if args.write: OUTPUT_JSON.write_text(expected_json); OUTPUT_MD.write_text(expected_md)
    if args.check:
        if not OUTPUT_JSON.exists() or OUTPUT_JSON.read_text() != expected_json: raise SystemExit("FAIL: rematch JSON stale")
        if not OUTPUT_MD.exists() or OUTPUT_MD.read_text() != expected_md: raise SystemExit("FAIL: rematch Markdown stale")
    print("PASS: 9 rematch families and all 35 reachable records are source-complete and statically coherent")
    print("PASS: 210 exact Pokemon sets, full item/move coverage, zero issues, quality floor 79")
    print("NEXT: campaign-wide anchor collision and reveal review")


if __name__ == "__main__": main()
