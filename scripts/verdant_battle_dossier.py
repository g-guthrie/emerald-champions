#!/usr/bin/env python3
"""Build a compact, source-backed authoring packet for one Verdant battle anchor."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

import search_competitive_teams as search
import verdant_doubles_conversion as doubles
import verdant_team_quality_audit as quality


ROOT = Path(__file__).resolve().parents[1]
BLUEPRINTS = ROOT / "docs/verdant_historic_team_reservations.json"
MARQUEE = ROOT / "docs/verdant_marquee_battle_designs.json"
SEQUENCE = ROOT / "docs/verdant_battle_sequence.json"
LEDGER = ROOT / "docs/verdant_battle_experience_ledger.json"
OUTPUT_DIR = ROOT / "docs/dossier_packets"


def load(path: Path) -> dict:
    return json.loads(path.read_text())


def slug(value: str) -> str:
    return re.sub(r"[^a-z0-9]+", "_", value.lower()).strip("_")


def blueprint(anchor: str) -> tuple[dict, list[dict]]:
    entries = load(BLUEPRINTS).get("marquee_blueprints", {}).get("entries", [])
    row = next((entry for entry in entries if entry.get("anchor") == anchor), None)
    if row is None:
        raise ValueError(f"unknown marquee anchor {anchor}")
    return row, entries


def compact_team(team: dict) -> dict:
    return {
        "trainer_id": team["trainer_id"],
        "format": team["format"],
        "party_size": team["party_size"],
        "average_level_offset": team["avg_level_offset"],
        "synergy_tags": team["synergy_tags"],
        "current_party": [
            {
                "species": mon["species"],
                "item": mon["item"],
                "ability": mon["ability"],
                "moves": mon["moves"],
            }
            for mon in team["mons"]
        ],
    }


def source_facts(trainer_ids: list[str]) -> list[dict]:
    trainers_text = (ROOT / "src/data/trainers.h").read_text()
    blocks = doubles.trainer_blocks(trainers_text)
    teams = {team["trainer_id"]: team for team in quality.audit()["teams"]}
    result = []
    for trainer_id in trainer_ids:
        if trainer_id not in blocks or trainer_id not in teams:
            result.append({"trainer_id": trainer_id, "error": "trainer is not active in current source"})
            continue
        block = blocks[trainer_id].group(0)
        result.append({
            **compact_team(teams[trainer_id]),
            "current_ai_flags": re.search(r"\.aiFlags\s*=\s*(.*?),\s*$", block, re.M).group(1).split(" | "),
            "source_party": doubles.party_name(block),
            "note": "Current source is baseline evidence only; a design-complete dossier does not mutate it.",
        })
    return result


def chronological_context(anchor: str, all_blueprints: list[dict]) -> dict:
    current = next(row for row in all_blueprints if row["anchor"] == anchor)
    ordered = sorted(all_blueprints, key=lambda row: row.get("campaign_order", 10**9))
    position = ordered.index(current)
    neighbors = ordered[max(0, position - 2):position] + ordered[position + 1:position + 3]
    sequence_entries = load(SEQUENCE).get("entries", [])
    sequence_match = next(
        (
            row for row in sequence_entries
            if row.get("encounter_id") == anchor
            or set(row.get("trainer_ids", [])) & set(current.get("trainer_ids", []))
        ),
        None,
    )
    ledger_entries = load(LEDGER).get("entries", [])
    if sequence_match and sequence_match.get("index", 0) <= len(ledger_entries) + 1:
        prior = [row for row in ledger_entries if row["index"] < sequence_match["index"]][-10:]
        return {
            "available": True,
            "reason": "Canonical previous-ten context exists at this implemented frontier.",
            "previous_encounters": prior,
            "protected_neighbor_anchors": [row["anchor"] for row in neighbors],
        }
    return {
        "available": False,
        "reason": "This future marquee anchor has campaign neighbors but no trustworthy previous-ten chronological window yet; refresh before implementation.",
        "previous_encounters": [],
        "protected_neighbor_anchors": [row["anchor"] for row in neighbors],
    }


def digest(record: dict, rank: int, query: str) -> dict:
    return {
        "rank": rank,
        "query": query,
        "reference_id": record["reference_id"],
        "source_kind": record.get("source_kind"),
        "player": record.get("player"),
        "event": record.get("event"),
        "year": record.get("year"),
        "completeness": record.get("completeness"),
        "confidence": record.get("confidence"),
        "roster": record.get("roster", []),
        "tags": record.get("tags", []),
        "strategy_notes": record.get("strategy_notes", ""),
        "primary_mode": record.get("primary_mode", ""),
        "secondary_mode": record.get("secondary_mode", ""),
        "preview_pressure": record.get("preview_pressure", ""),
        "ai_requirements": record.get("ai_requirements", []),
        "gimmick_dependencies": record.get("gimmick_dependencies", []),
        "urls": record.get("urls", []),
    }


def build(anchor: str, queries: list[str], tags: list[str], pokemon: list[str], limit: int) -> dict:
    row, all_blueprints = blueprint(anchor)
    marquee = load(MARQUEE)
    records = search.load_records()
    effective_queries = queries or [
        *row.get("primary_candidate_modes", []),
        *row.get("secondary_candidate_modes", []),
    ]
    candidates = []
    seen = set()
    for query in effective_queries:
        for record in search.rank_records(
            records,
            query=query,
            tags=tags,
            pokemon=pokemon,
            style="singles" if "singles" in " ".join(row.get("primary_candidate_modes", [])).lower() else "doubles",
            limit=limit,
        ):
            if record["reference_id"] in seen:
                continue
            seen.add(record["reference_id"])
            candidates.append(digest(record, len(candidates) + 1, query))
            if len(candidates) >= limit:
                break
        if len(candidates) >= limit:
            break
    reserved_ids = list(dict.fromkeys(row.get("candidate_reference_ids", [])))
    for ref in reserved_ids:
        if ref in seen:
            continue
        record = next((record for record in records if record["reference_id"] == ref), None)
        if record:
            seen.add(ref)
            candidates.append(digest(record, len(candidates) + 1, "campaign reservation"))

    mechanics_id = "pokemon_league_main_story" if row.get("planning_tier") in {"league_gauntlet", "champion"} else None
    return {
        "version": 1,
        "packet_type": "authoring-context-not-a-closed-design",
        "anchor": row,
        "mechanics_baseline_id": mechanics_id,
        "mechanics_baseline": marquee.get("mechanics_baselines", {}).get(mechanics_id) if mechanics_id else None,
        "current_source_facts": source_facts(row.get("trainer_ids", [])),
        "rolling_context": chronological_context(anchor, all_blueprints),
        "competitive_index": marquee["corpus_identity"],
        "competitive_queries": effective_queries,
        "competitive_filters": {"tags": tags, "pokemon": pokemon, "limit": limit},
        "ranked_candidate_digest": candidates,
        "dossier_template": {
            "anchor_id": anchor,
            "planning_tier": row.get("planning_tier"),
            "status": {"design": None, "author_self_check": None, "source": "unimplemented", "static": None, "runtime": "unplayed"},
            "campaign_state": None,
            "runtime": None,
            "rolling_context": None,
            "identity": None,
            "difficulty": {"target": row.get("target_difficulty"), "observed": None},
            "team": None,
            "ordering": None,
            "ai": None,
            "counterplay": None,
            "competitive_research": None,
            "campaign_reservations": None,
            "presentation": None,
            "author_self_check": {"strongest_part": None, "weakest_link": None},
            "verification": None,
            "mechanics_proposal": None,
        },
    }


def markdown(packet: dict) -> str:
    anchor = packet["anchor"]
    lines = [
        f"# Verdant dossier packet — {anchor['anchor']}",
        "",
        "This packet is authoring context, not an implemented or closed design.",
        "",
        "## Campaign anchor",
        "",
        f"- Tier: {anchor.get('planning_tier')}",
        f"- Commitment: {anchor.get('design_commitment')}",
        f"- Target difficulty: {anchor.get('target_difficulty')}",
        f"- Protected identity: {anchor.get('protected_identity')}",
        f"- Signature reveal: {anchor.get('signature_reveal')}",
        f"- Trainer IDs: {', '.join(f'`{value}`' for value in anchor.get('trainer_ids', []))}",
        "",
        "## Current observed mechanics baseline",
        "",
    ]
    mechanics = packet.get("mechanics_baseline")
    if mechanics:
        for key, value in mechanics.items():
            if key not in {"source_evidence"}:
                lines.append(f"- {key.replace('_', ' ').title()}: `{value}`")
    else:
        lines.append("- No phase snapshot is assigned yet; source inspection is required before design.")
    lines.extend(["", "## Current source baseline", ""])
    for source in packet["current_source_facts"]:
        lines.append(
            f"- `{source['trainer_id']}`: {source.get('format', 'unknown')}, "
            f"{source.get('party_size', '?')} Pokémon, source party `{source.get('source_party', 'unknown')}`"
        )
    context = packet["rolling_context"]
    lines.extend([
        "",
        "## Rolling context",
        "",
        f"- Available: {context['available']}",
        f"- Reason: {context['reason']}",
        f"- Protected neighbors: {', '.join(context['protected_neighbor_anchors']) or 'none'}",
        "",
        "## Ranked competitive candidates",
        "",
    ])
    for candidate in packet["ranked_candidate_digest"]:
        lines.extend([
            f"### {candidate['rank']}. `{candidate['reference_id']}`",
            "",
            f"- Query: {candidate['query']}",
            f"- Evidence: {candidate['completeness']} / {candidate['confidence']}",
            f"- Roster: {', '.join(candidate['roster'])}",
            f"- Tags: {', '.join(candidate['tags']) or 'none'}",
            f"- Strategy: {candidate['strategy_notes'] or 'not documented'}",
            f"- Original gimmicks: {', '.join(candidate['gimmick_dependencies']) or 'none recorded'}",
            f"- Source: {candidate['urls'][0] if candidate['urls'] else 'local corpus source'}",
            "",
        ])
    lines.extend([
        "## Required next action",
        "",
        "Inspect the strongest candidates in full, author every dossier field, run `verdant_marquee_design_audit.py`, report the design, and leave game source untouched until chronological implementation.",
        "",
    ])
    return "\n".join(lines)


def paths(anchor: str) -> tuple[Path, Path]:
    base = OUTPUT_DIR / slug(anchor)
    return base.with_suffix(".json"), base.with_suffix(".md")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--anchor")
    parser.add_argument("--query", action="append", default=[])
    parser.add_argument("--tag", action="append", default=[])
    parser.add_argument("--pokemon", action="append", default=[])
    parser.add_argument("--limit", type=int, default=12)
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--check-all", action="store_true")
    parser.add_argument("--refresh-all", action="store_true")
    args = parser.parse_args()
    if args.check_all or args.refresh_all:
        if args.anchor or args.write or args.check:
            parser.error("--check-all/--refresh-all cannot be combined with an anchor operation")
        paths_to_check = sorted(OUTPUT_DIR.glob("*.json"))
        if not paths_to_check:
            raise SystemExit("FAIL: no dossier packets exist")
        for json_path in paths_to_check:
            saved = json.loads(json_path.read_text())
            anchor = saved["anchor"]["anchor"]
            filters = saved.get("competitive_filters", {})
            expected = build(
                anchor,
                saved.get("competitive_queries", []),
                filters.get("tags", []),
                filters.get("pokemon", []),
                filters.get("limit", 12),
            )
            md_path = json_path.with_suffix(".md")
            if args.refresh_all:
                json_path.write_text(json.dumps(expected, indent=2, sort_keys=True) + "\n")
                md_path.write_text(markdown(expected))
            elif saved != expected or not md_path.exists() or md_path.read_text() != markdown(expected):
                raise SystemExit(f"FAIL: dossier packet is stale for {anchor}")
        verb = "refreshed from" if args.refresh_all else "match"
        print(f"PASS: {len(paths_to_check)} dossier packets {verb} current source, blueprints, mechanics, and corpus")
        return
    if not args.anchor:
        parser.error("--anchor is required unless --check-all is used")
    if not args.write and not args.check:
        parser.error("choose --write, --check, or --check-all")
    packet = build(args.anchor, args.query, args.tag, args.pokemon, args.limit)
    json_path, md_path = paths(args.anchor)
    expected_json = json.dumps(packet, indent=2, sort_keys=True) + "\n"
    expected_md = markdown(packet)
    if args.write:
        OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
        json_path.write_text(expected_json)
        md_path.write_text(expected_md)
    if args.check:
        if not json_path.exists() or not md_path.exists():
            raise SystemExit(f"FAIL: dossier packet is missing for {args.anchor}")
        if json_path.read_text() != expected_json or md_path.read_text() != expected_md:
            raise SystemExit(f"FAIL: dossier packet is stale for {args.anchor}")
    print(f"PASS: {args.anchor} packet has {len(packet['ranked_candidate_digest'])} compact candidates and current observed source context")


if __name__ == "__main__":
    main()
