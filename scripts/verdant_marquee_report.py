#!/usr/bin/env python3
"""Render Verdant's canonical marquee dossiers as a readable design report."""

from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DESIGNS = ROOT / "docs/verdant_marquee_battle_designs.json"
COLLISIONS = ROOT / "docs/verdant_marquee_collision_report.json"
OUTPUT = ROOT / "docs/verdant_marquee_design_report.md"


def markdown() -> str:
    payload = json.loads(DESIGNS.read_text())
    designs = payload["designs"]
    lines = [
        "# Verdant marquee battle design report",
        "",
        f"Current phase: `{payload['current_phase']}`",
        "",
        "These are design dossiers, not source implementation or observed ROM results. Target difficulty is editorial until runtime playtesting.",
        "",
        "## Current League mechanics baseline",
        "",
    ]
    baseline = payload["mechanics_baselines"]["pokemon_league_main_story"]
    for key, value in baseline.items():
        if key not in {"source_evidence"}:
            lines.append(f"- {key.replace('_', ' ').title()}: `{value}`")
    for anchor in payload["expected_phase_anchors"]:
        dossier = designs.get(anchor)
        lines.extend(["", f"## {anchor}", ""])
        if dossier is None:
            lines.append("Missing dossier.")
            continue
        status = dossier["status"]
        lines.extend([
            f"- Status: `{status['design']}` / critic `{status['fresh_critic']}` / source `{status['source']}` / runtime `{status['runtime']}`",
            f"- Format: `{dossier['runtime']['canonical_format']}`",
            f"- Strict cap: {dossier['campaign_state']['strict_cap']}",
            f"- Target / observed difficulty: **{dossier['difficulty']['target']} / {dossier['difficulty']['observed']}**",
            f"- Memory hook: {dossier['identity']['memory_hook']}",
            "",
            "### Exact proposed team",
            "",
            "| # | Pokémon | Offset | Item | Ability | Moves | Role |",
            "| ---: | --- | ---: | --- | --- | --- | --- |",
        ])
        for mon in dossier["team"]:
            lines.append(
                f"| {mon['order']} | `{mon['species']}` | {mon['level_offset']:+} | `{mon['item']}` | "
                f"`{mon['ability']}` | {', '.join(f'`{move}`' for move in mon['moves'])} | {mon['role']} |"
            )
        lines.extend([
            "",
            "### Why this battle exists",
            "",
            f"- Primary question: {dossier['identity']['primary_player_question']}",
            f"- Primary mode: {dossier['identity']['primary_mode']}",
            f"- Secondary mode: {dossier['identity']['secondary_mode']}",
            f"- Difficulty rationale: {dossier['difficulty']['rationale']}",
            f"- First-loss lesson: {dossier['counterplay']['first_loss_lesson']}",
            f"- Intentional weakness: {dossier['counterplay']['intentional_weakness']}",
            "",
            "### AI and evidence",
            "",
            f"- State machine: {dossier['ai']['state_machine']}",
            f"- Selected references: {', '.join(f'`{ref}`' for ref in dossier['competitive_research']['selected_reference_ids'])}",
            f"- Required source work: {'; '.join(dossier['ai']['custom_requirements'])}",
            "",
            "### Fresh-context verdict",
            "",
        ])
        critic = dossier.get("fresh_critic_review") or {}
        if critic:
            lines.extend([
                f"- Reviewer: `{critic.get('reviewer_agent_id')}`",
                f"- Review engine: `{critic.get('reviewer_model')}` / `{critic.get('reasoning_effort')}` / template `{critic.get('review_template_version')}`",
                f"- Verdict: **{critic.get('verdict')}**",
                f"- Is it sick and awesome? {critic.get('is_this_sick_and_awesome')}",
                f"- Signature moment: {critic.get('signature_moment')}",
                f"- Biggest problem: {critic.get('biggest_problem')}",
                f"- Honest difficulty take: {critic.get('honest_difficulty_take')}",
                f"- Single best change: {critic.get('single_best_change')}",
                f"- Ship blocker: {critic.get('ship_blocker')}",
            ])
        else:
            lines.append("- Final fresh no-history review is pending.")
        lines.extend([
            "",
            "### Campaign reservations",
            "",
            f"- Spends: {'; '.join(dossier['campaign_reservations']['spends'])}",
            f"- Preserves: {'; '.join(dossier['campaign_reservations']['preserves'])}",
            f"- Releases: {'; '.join(dossier['campaign_reservations']['releases'])}",
        ])

    lines.extend(["", "## Cross-dossier collision review", ""])
    if COLLISIONS.exists():
        report = json.loads(COLLISIONS.read_text())
        lines.append(f"- Hard errors: {len(report['hard_errors'])}")
        lines.append(f"- Unique species: {report['species_count']}")
        lines.append(f"- Formats: {report['format_counts']}")
        for row in report["advisories"]:
            lines.append(f"- Advisory `{row['code']}`: `{json.dumps(row.get('uses'), sort_keys=True)}`")
    else:
        lines.append("- Collision report has not been generated.")
    lines.extend(["", "No dossier here authorizes a game-source or campaign-mechanics change.", ""])
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.write and not args.check:
        parser.error("choose --write or --check")
    expected = markdown()
    if args.write:
        OUTPUT.write_text(expected)
    if args.check and (not OUTPUT.exists() or OUTPUT.read_text() != expected):
        raise SystemExit("FAIL: marquee design report is missing or stale")
    print(f"PASS: marquee report renders {len(json.loads(DESIGNS.read_text())['designs'])} dossier(s)")


if __name__ == "__main__":
    main()
