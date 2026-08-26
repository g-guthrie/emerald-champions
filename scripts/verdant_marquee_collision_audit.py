#!/usr/bin/env python3
"""Cross-review Verdant marquee dossiers without assigning a synthetic quality score."""

from __future__ import annotations

import argparse
import json
import re
from collections import Counter, defaultdict
from pathlib import Path

import verdant_team_quality_audit as quality


ROOT = Path(__file__).resolve().parents[1]
DESIGNS = ROOT / "docs/verdant_marquee_battle_designs.json"
REPORT_JSON = ROOT / "docs/verdant_marquee_collision_report.json"
REPORT_MD = ROOT / "docs/verdant_marquee_collision_report.md"

COMMON_MOVES = {"MOVE_PROTECT", "MOVE_DETECT", "MOVE_RETURN"}
MODE_MARKERS = {
    "weather-rain": ("rain", "drizzle", "water spout"),
    "weather-snow": ("snow", "hail", "blizzard"),
    "trapping-clock": ("perish", "trap", "infestation", "toxic clock"),
    "hazard-switch-tax": ("stealth rock", "hazard", "switch tax"),
    "detonation": ("explosion", "detonation"),
}


def build() -> dict:
    payload = json.loads(DESIGNS.read_text())
    expected = payload.get("expected_phase_anchors", [])
    designs = payload.get("designs", {})
    hard: list[dict] = []
    advisory: list[dict] = []

    missing = sorted(set(expected) - set(designs))
    if missing:
        hard.append({"code": "MISSING_PHASE_DOSSIERS", "anchors": missing})

    species_uses: dict[str, list[str]] = defaultdict(list)
    mega_uses: dict[str, list[str]] = defaultdict(list)
    item_uses: dict[str, list[str]] = defaultdict(list)
    move_uses: dict[str, list[str]] = defaultdict(list)
    reference_uses: dict[str, list[str]] = defaultdict(list)
    question_uses: dict[str, list[str]] = defaultdict(list)
    mode_uses: dict[str, list[str]] = defaultdict(list)
    format_counts = Counter()
    resource_taxes = {}

    for anchor, dossier in designs.items():
        runtime = dossier.get("runtime", {})
        format_counts[runtime.get("canonical_format", "unknown")] += 1
        if anchor == "ELITE_FOUR_DRAKE" and runtime.get("canonical_format") != "single":
            hard.append({"code": "DRAKE_FORMAT_DRIFT", "anchor": anchor})
        if anchor != "ELITE_FOUR_DRAKE" and anchor in expected and runtime.get("canonical_format") != "double":
            hard.append({"code": "LEAGUE_FORMAT_DRIFT", "anchor": anchor, "format": runtime.get("canonical_format")})
        if dossier.get("difficulty", {}).get("target") != 10:
            hard.append({"code": "LEAGUE_TARGET_NOT_TEN", "anchor": anchor})

        question = re.sub(r"\s+", " ", dossier.get("identity", {}).get("primary_player_question", "").lower()).strip()
        if question:
            question_uses[question].append(anchor)
        blob = json.dumps({
            "identity": dossier.get("identity"),
            "difficulty": dossier.get("difficulty"),
            "team": dossier.get("team"),
            "ai": dossier.get("ai"),
        }).lower()
        for mode, markers in MODE_MARKERS.items():
            if any(marker in blob for marker in markers):
                mode_uses[mode].append(anchor)

        mega_count = 0
        exact_moves: set[str] = set()
        exact_items: set[str] = set()
        for mon in dossier.get("team", []):
            species_uses[mon["species"]].append(anchor)
            item_uses[mon["item"]].append(anchor)
            exact_items.add(mon["item"])
            for move in mon["moves"]:
                exact_moves.add(move)
                if move not in COMMON_MOVES:
                    move_uses[move].append(anchor)
            if mon.get("mega_candidate"):
                mega_count += 1
                mega_uses[mon["species"]].append(anchor)
        if mega_count != 1:
            hard.append({"code": "LEAGUE_MEGA_COUNT", "anchor": anchor, "count": mega_count})
        if exact_moves & quality.SETUP_MOVES:
            mode_uses["setup-endgame"].append(anchor)
        if exact_items & quality.CHOICE_ITEMS:
            mode_uses["choice-pressure"].append(anchor)
        if exact_moves & quality.REDIRECTION_MOVES:
            mode_uses["redirection"].append(anchor)
        if "MOVE_TRICK_ROOM" in exact_moves:
            mode_uses["slow-control"].append(anchor)
        if exact_moves & {"MOVE_TAILWIND", "MOVE_ICY_WIND", "MOVE_ELECTROWEB"}:
            mode_uses["fast-control"].append(anchor)

        for ref in dossier.get("competitive_research", {}).get("selected_reference_ids", []):
            reference_uses[ref].append(anchor)
        resource_taxes[anchor] = dossier.get("difficulty", {}).get("resource_tax")

    repeated_species = {key: sorted(set(value)) for key, value in species_uses.items() if len(set(value)) > 1}
    if repeated_species:
        hard.append({"code": "LEAGUE_SPECIES_COLLISION", "uses": repeated_species})
    repeated_megas = {key: sorted(set(value)) for key, value in mega_uses.items() if len(set(value)) > 1}
    if repeated_megas:
        hard.append({"code": "LEAGUE_MEGA_COLLISION", "uses": repeated_megas})
    repeated_questions = {key: value for key, value in question_uses.items() if len(value) > 1}
    if repeated_questions:
        hard.append({"code": "PRIMARY_QUESTION_DUPLICATE", "uses": repeated_questions})

    repeated_items = {key: sorted(set(value)) for key, value in item_uses.items() if len(set(value)) >= 3}
    if repeated_items:
        advisory.append({"code": "PREMIUM_ITEM_REPETITION", "uses": repeated_items})
    repeated_moves = {key: sorted(set(value)) for key, value in move_uses.items() if len(set(value)) >= 3}
    if repeated_moves:
        advisory.append({"code": "SIGNATURE_MOVE_REPETITION", "uses": repeated_moves})
    repeated_refs = {key: sorted(set(value)) for key, value in reference_uses.items() if len(set(value)) > 1}
    if repeated_refs:
        advisory.append({"code": "HISTORIC_REFERENCE_REUSE", "uses": repeated_refs})
    clustered_modes = {key: sorted(set(value)) for key, value in mode_uses.items() if len(set(value)) >= 3}
    if clustered_modes:
        advisory.append({"code": "MODE_CLUSTER", "uses": clustered_modes})

    return {
        "version": 1,
        "phase": payload.get("current_phase"),
        "expected_anchors": expected,
        "designed_anchors": sorted(designs),
        "format_counts": dict(sorted(format_counts.items())),
        "species_count": len(species_uses),
        "mega_species": dict(sorted(mega_uses.items())),
        "mode_uses": dict(sorted(mode_uses.items())),
        "resource_taxes": resource_taxes,
        "hard_errors": hard,
        "advisories": advisory,
        "policy": "Hard collisions block the current phase. Advisories require judgment and written disposition; they are not scores, quotas, or automatic bans.",
    }


def markdown(report: dict) -> str:
    lines = [
        "# Verdant marquee collision review",
        "",
        f"Phase: `{report['phase']}`",
        "",
        f"Designed: {len(report['designed_anchors'])}/{len(report['expected_anchors'])}",
        f"Unique species: {report['species_count']}",
        f"Formats: {report['format_counts']}",
        "",
        "## Hard errors",
        "",
    ]
    lines.extend(
        [f"- **{row['code']}** — `{json.dumps(row, sort_keys=True)}`" for row in report["hard_errors"]]
        or ["- None."]
    )
    lines.extend(["", "## Advisories", ""])
    lines.extend(
        [f"- **{row['code']}** — `{json.dumps(row, sort_keys=True)}`" for row in report["advisories"]]
        or ["- None."]
    )
    lines.extend(["", "## Resource tax by battle", ""])
    for anchor, tax in report["resource_taxes"].items():
        lines.append(f"- `{anchor}` — {tax}")
    lines.extend(["", report["policy"], ""])
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.write and not args.check:
        parser.error("choose --write or --check")
    report = build()
    expected_json = json.dumps(report, indent=2, sort_keys=True) + "\n"
    expected_md = markdown(report)
    if args.write:
        REPORT_JSON.write_text(expected_json)
        REPORT_MD.write_text(expected_md)
    if args.check:
        if not REPORT_JSON.exists() or not REPORT_MD.exists() or REPORT_JSON.read_text() != expected_json or REPORT_MD.read_text() != expected_md:
            raise SystemExit("FAIL: marquee collision report is missing or stale")
        if report["hard_errors"]:
            raise SystemExit(f"FAIL: {len(report['hard_errors'])} hard marquee collision(s)")
    print(
        f"PASS: marquee collision report covers {len(report['designed_anchors'])}/{len(report['expected_anchors'])} "
        f"anchors with {len(report['hard_errors'])} hard error(s) and {len(report['advisories'])} advisory group(s)"
    )


if __name__ == "__main__":
    main()
