#!/usr/bin/env python3
"""Return a small, ranked Verdant competitive-reference digest."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
INDEX = ROOT / "docs/competitive_team_index.jsonl"


def terms(value: str) -> list[str]:
    return re.findall(r"[a-z0-9-]+", value.lower())


def searchable(record: dict) -> str:
    values = [
        record.get("reference_id", ""), record.get("format") or "", record.get("player") or "",
        record.get("event") or "", record.get("strategy_notes") or "", " ".join(record.get("tags", [])),
        record.get("primary_mode") or "", record.get("secondary_mode") or "",
        record.get("preview_pressure") or "", record.get("verdant_fit") or "",
        " ".join(record.get("ai_requirements", [])),
        " ".join(record.get("gimmick_dependencies", [])),
        " ".join(record.get("roster", [])),
    ]
    for mon in record.get("sets", []):
        values.extend([mon.get("species", ""), mon.get("item", ""), mon.get("ability", ""), " ".join(mon.get("moves", []))])
    return " ".join(values).lower()


def load_records() -> list[dict]:
    if not INDEX.exists():
        raise FileNotFoundError("competitive index is missing; run build_competitive_team_index.py --write")
    return [json.loads(line) for line in INDEX.read_text().splitlines() if line]


def rank_records(
    records: list[dict],
    *,
    query: str = "",
    tags: list[str] | None = None,
    pokemon: list[str] | None = None,
    player: str | None = None,
    format_name: str | None = None,
    style: str | None = None,
    limit: int = 12,
) -> list[dict]:
    query_terms = terms(query)
    required_tags = {tag.lower() for tag in (tags or [])}
    required_pokemon = {name.lower() for name in (pokemon or [])}
    ranked = []
    for record in records:
        record_tags = {tag.lower() for tag in record.get("tags", [])}
        roster = {name.lower() for name in record.get("roster", [])}
        if not required_tags <= record_tags or not required_pokemon <= roster:
            continue
        if player and player.lower() not in (record.get("player") or "").lower():
            continue
        if format_name and format_name.lower() not in (record.get("format") or "").lower():
            continue
        if style and record.get("battle_style") != style:
            continue
        haystack = searchable(record)
        if query_terms and not all(term in haystack for term in query_terms):
            continue
        score = sum(haystack.count(term) * 3 for term in query_terms)
        score += len(required_tags & record_tags) * 8 + len(required_pokemon & roster) * 12
        score += 3 if (record.get("completeness") or "").startswith("full-sets") else 0
        score += 2 if record.get("source_kind") == "curated-elite-research" else 0
        ranked.append((score, record))
    ranked.sort(key=lambda item: (
        -item[0],
        -(item[1].get("year") or 0),
        item[1]["reference_id"],
    ))
    return [record for _, record in ranked[: max(1, limit)]]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--query", default="")
    parser.add_argument("--tag", action="append", default=[])
    parser.add_argument("--pokemon", action="append", default=[])
    parser.add_argument("--player")
    parser.add_argument("--format")
    parser.add_argument("--style", choices=("singles", "doubles"))
    parser.add_argument("--limit", type=int, default=12)
    parser.add_argument("--json", action="store_true")
    args = parser.parse_args()
    try:
        records = load_records()
    except FileNotFoundError as exc:
        raise SystemExit(str(exc)) from exc
    selected = rank_records(
        records,
        query=args.query,
        tags=args.tag,
        pokemon=args.pokemon,
        player=args.player,
        format_name=args.format,
        style=args.style,
        limit=args.limit,
    )
    if args.json:
        print(json.dumps(selected, indent=2))
        return
    for record in selected:
        print(f"{record['reference_id']} | {record.get('player') or '-'} | {record.get('event') or record.get('format') or '-'}")
        print(f"  roster: {', '.join(record.get('roster', []))}")
        print(f"  tags: {', '.join(record.get('tags', [])) or '-'}")
        print(f"  evidence: {record.get('completeness')} / {record.get('confidence')}")
        if record.get("strategy_notes"):
            print(f"  notes: {record['strategy_notes']}")
        if record.get("primary_mode"):
            print(f"  primary: {record['primary_mode']}")
        if record.get("secondary_mode"):
            print(f"  secondary: {record['secondary_mode']}")
        if record.get("gimmick_dependencies"):
            print(f"  original gimmicks: {', '.join(record['gimmick_dependencies'])}")
        if record.get("urls"):
            print(f"  source: {record['urls'][0]}")


if __name__ == "__main__":
    main()
