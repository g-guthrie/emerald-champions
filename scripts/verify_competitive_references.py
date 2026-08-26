#!/usr/bin/env python3
"""Validate Verdant's reproducible competitive-team reference corpus."""

from __future__ import annotations

import json
from pathlib import Path

import build_competitive_team_index as competitive_index


ROOT = Path(__file__).resolve().parents[1]


def main() -> None:
    problems = []
    showdown_files = sorted((ROOT / "docs").glob("showdown_*_30.json"))
    if len(showdown_files) != 13:
        problems.append(f"expected 13 Showdown sample files, found {len(showdown_files)}")
    showdown_teams = 0
    for path in showdown_files:
        data = json.loads(path.read_text())
        samples = data.get("samples", [])
        showdown_teams += len(samples)
        if data.get("sample_count") != 30 or len(samples) != 30:
            problems.append(f"{path.name}: expected 30 samples")
        if any(len(sample.get("team", [])) != 6 for sample in samples):
            problems.append(f"{path.name}: malformed team")
        if not data.get("showdown_commit") or not data.get("format"):
            problems.append(f"{path.name}: missing source provenance")

    vgc = json.loads((ROOT / "docs/vgc_major_champion_teams.json").read_text())
    if vgc.get("champion_team_count") != 390 or len(vgc.get("teams", [])) != 390:
        problems.append("VGC champion corpus must contain 390 teams")
    if any(not team.get("verified") or len(team.get("team", [])) != 6 for team in vgc.get("teams", [])):
        problems.append("VGC corpus contains an unverified or malformed champion team")

    smogon = json.loads((ROOT / "docs/smogon_gen4_9_ou_uu_nu_sample_teams.json").read_text())
    smogon_count = sum(len(teams) for teams in smogon.get("formats", {}).values())
    if smogon.get("sample_team_count") != 203 or smogon_count != 203 or len(smogon.get("formats", {})) != 18:
        problems.append("Smogon corpus must contain 203 teams across 18 formats")
    if any(len(team.get("data", [])) != 6 for teams in smogon.get("formats", {}).values() for team in teams):
        problems.append("Smogon corpus contains a malformed team")

    if showdown_teams != 390:
        problems.append(f"Showdown reference corpus must contain 390 teams, found {showdown_teams}")

    indexed = competitive_index.build()
    serialized = competitive_index.serialized_index(indexed)
    expected_metadata = competitive_index.metadata(indexed, serialized)
    if not competitive_index.OUTPUT.exists() or competitive_index.OUTPUT.read_text() != serialized:
        problems.append("expanded competitive index is missing or stale")
    if not competitive_index.METADATA.exists() or json.loads(competitive_index.METADATA.read_text()) != expected_metadata:
        problems.append("expanded competitive index metadata is missing or stale")
    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))
    print("PASS: 390 reproducible Showdown random-team references")
    print("PASS: 390 verified official-event champion teams from VGC History")
    print("PASS: 203 Smogon Gen 4-9 OU/UU/NU sample teams")
    print("PASS: 983 baseline competitive reference teams with provenance")
    print(
        f"PASS: {len(indexed)} searchable references including "
        f"{len(indexed) - 983} curated elite additions ({expected_metadata['sha256'][:12]})"
    )


if __name__ == "__main__":
    main()
