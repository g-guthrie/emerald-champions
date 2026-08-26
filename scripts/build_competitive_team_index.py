#!/usr/bin/env python3
"""Build Verdant's compact, searchable competitive-team reference index."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DOCS = ROOT / "docs"
OUTPUT = DOCS / "competitive_team_index.jsonl"
METADATA = DOCS / "competitive_team_index.meta.json"
ELITE = DOCS / "verdant_historic_team_research.json"
RESERVATIONS = DOCS / "verdant_historic_team_reservations.json"
INDEX_VERSION = 1


WEATHER = {
    "rain": {"raindance", "drizzle", "primordialsea", "pelipper", "politoed", "kyogre"},
    "sun": {"sunnyday", "drought", "orichalcumpulse", "desolateland", "torkoal", "groudon", "koraidon"},
    "sand": {"sandstorm", "sandstream", "hippowdon", "hippopotas", "tyranitar", "gigalith"},
    "snow": {"hail", "snowscape", "snowwarning", "ninetalesalola", "abomasnow", "vanilluxe"},
}
TERRAIN = {
    "electric-terrain": {"electricterrain", "electricsurge", "miraidon", "pincurchin", "tapu-koko"},
    "psychic-terrain": {"psychicterrain", "psychicsurge", "indeedee", "tapu-lele"},
    "grassy-terrain": {"grassyterrain", "grassysurge", "rillaboom", "tapu-bulu"},
    "misty-terrain": {"mistyterrain", "mistysurge", "tapu-fini", "weezing-galar"},
}
MOVE_TAGS = {
    "trick-room": {"trickroom"},
    "tailwind": {"tailwind"},
    "active-speed-control": {"icywind", "electroweb", "bulldoze", "stringshot", "thunderwave"},
    "redirection": {"followme", "ragepowder", "spotlight"},
    "fake-out": {"fakeout"},
    "screens": {"reflect", "lightscreen", "auroraveil"},
    "hazards": {"stealthrock", "spikes", "toxicspikes", "stickyweb"},
    "perish": {"perishsong"},
    "choice-disruption": {"trick", "switcheroo"},
    "pivoting": {"uturn", "voltswitch", "flipturn", "partingshot", "chillyreception"},
    "healing": {"recover", "roost", "softboiled", "slackoff", "strengthsap", "wish", "lifedew", "pollenpuff"},
    "sleep": {"spore", "sleeppowder", "hypnosis", "lovelykiss", "darkvoid"},
    "priority": {"extremespeed", "suckerpunch", "aquajet", "machpunch", "bulletpunch", "iceshard", "grassyglide", "firstimpression"},
    "wide-guard": {"wideguard"},
    "setup": {"swordsdance", "nastyplot", "dragondance", "quiverdance", "shellsmash", "bellydrum", "calmmind", "bulkup", "noretreat", "shiftgear", "geomancy"},
}


def compact(value: str) -> str:
    return re.sub(r"[^a-z0-9]+", "", value.lower())


def normalize_mon(mon: dict) -> dict:
    return {
        "species": mon.get("species") or mon.get("name") or "Unknown",
        "item": mon.get("item", ""),
        "ability": mon.get("ability", ""),
        "moves": list(mon.get("moves", [])),
    }


def infer_tags(roster: list[str], sets: list[dict], supplied: list[str] | None = None) -> list[str]:
    tokens = {compact(value) for value in roster}
    moves = {compact(move) for mon in sets for move in mon.get("moves", [])}
    abilities = {compact(mon.get("ability", "")) for mon in sets}
    items = {compact(mon.get("item", "")) for mon in sets}
    all_tokens = tokens | moves | abilities | items
    tags = set(supplied or [])
    for tag, markers in WEATHER.items():
        if markers & all_tokens:
            tags.add(tag)
    for tag, markers in TERRAIN.items():
        if markers & all_tokens:
            tags.add(tag)
    for tag, markers in MOVE_TAGS.items():
        if markers & moves:
            tags.add(tag)
    if "perish" in tags and ({"shadowtag", "arenatrap"} & abilities or "gothitelle" in tokens):
        tags.add("perish-trap")
    if "beatup" in moves and "justified" in abilities:
        tags.add("beat-up-justified")
    if "frostbreath" in moves and "angerpoint" in abilities:
        tags.add("anger-point-activation")
    if "surf" in moves and ({"waterabsorb", "stormdrain", "steamengine", "watercompaction"} & abilities):
        tags.add("surf-ally-activation")
    if "commander" in abilities or {"dondozo", "tatsugiri"} <= tokens:
        tags.add("commander")
    if any("choice" in item for item in items):
        tags.add("choice-item")
    if len(tags & {"setup", "tailwind", "rain", "sun", "sand", "snow"}) >= 2:
        tags.add("offense")
    if "perish-trap" in tags or len(tags & {"healing", "screens", "fake-out", "redirection", "pivoting"}) >= 3:
        tags.add("positioning-control")
    if "trick-room" in tags and "tailwind" in tags:
        tags.add("dual-speed-mode")
    return sorted(tags)


def showdown_records() -> list[dict]:
    records = []
    for path in sorted(DOCS.glob("showdown_*_30.json")):
        payload = json.loads(path.read_text())
        format_id = payload["format"]
        generation_match = re.search(r"gen(\d+)", format_id)
        style = "doubles" if "double" in format_id else "singles"
        for index, sample in enumerate(payload["samples"], 1):
            sets = [normalize_mon(mon) for mon in sample["team"]]
            roster = [mon["species"] for mon in sets]
            records.append({
                "reference_id": f"showdown:{format_id}:{index:03d}",
                "source_kind": "showdown-random",
                "source_file": str(path.relative_to(ROOT)),
                "source_locator": {"sample": index, "seed": sample["seed"]},
                "format": format_id,
                "generation": int(generation_match.group(1)) if generation_match else None,
                "battle_style": style,
                "player": None,
                "event": None,
                "year": None,
                "placement": None,
                "roster": roster,
                "sets": sets,
                "tags": infer_tags(roster, sets),
                "completeness": "full-sets",
                "confidence": "reproducible",
                "urls": [payload["source"]],
                "strategy_notes": "",
            })
    return records


def smogon_records() -> list[dict]:
    path = DOCS / "smogon_gen4_9_ou_uu_nu_sample_teams.json"
    payload = json.loads(path.read_text())
    records = []
    for format_id, teams in payload["formats"].items():
        generation = int(re.search(r"gen(\d+)", format_id).group(1))
        for index, team in enumerate(teams, 1):
            sets = [normalize_mon(mon) for mon in team["data"]]
            roster = [mon["species"] for mon in sets]
            records.append({
                "reference_id": f"smogon:{format_id}:{index:03d}",
                "source_kind": "smogon-sample",
                "source_file": str(path.relative_to(ROOT)),
                "source_locator": {"format": format_id, "sample": index, "name": team.get("name")},
                "format": format_id,
                "generation": generation,
                "battle_style": "singles",
                "player": team.get("author"),
                "event": None,
                "year": None,
                "placement": None,
                "roster": roster,
                "sets": sets,
                "tags": infer_tags(roster, sets),
                "completeness": "full-sets",
                "confidence": "published-sample",
                "urls": [payload["source"]],
                "strategy_notes": team.get("name", ""),
            })
    return records


def vgc_records() -> list[dict]:
    path = DOCS / "vgc_major_champion_teams.json"
    payload = json.loads(path.read_text())
    records = []
    for team in payload["teams"]:
        roster = list(team["team"])
        urls = [url for url in team.get("source", {}).values() if url]
        records.append({
            "reference_id": f"vgc:{team['tournament_id']}",
            "source_kind": "vgc-event-champion",
            "source_file": str(path.relative_to(ROOT)),
            "source_locator": {"tournament_id": team["tournament_id"]},
            "format": team.get("regulation"),
            "generation": None,
            "battle_style": "doubles",
            "player": team.get("champion"),
            "event": team.get("tournament"),
            "year": team.get("year"),
            "placement": 1,
            "roster": roster,
            "sets": [],
            "tags": infer_tags(roster, []),
            "completeness": "roster-only",
            "confidence": "verified-event",
            "urls": urls or [payload["source"]],
            "strategy_notes": "",
        })
    return records


def elite_records() -> list[dict]:
    if not ELITE.exists():
        return []
    payload = json.loads(ELITE.read_text())
    records = []
    for team in payload.get("teams", []):
        sets = [normalize_mon(mon) for mon in team.get("sets", [])]
        roster = list(team.get("roster") or [mon["species"] for mon in sets])
        record = {
            "reference_id": team["reference_id"],
            "source_kind": "curated-elite-research",
            "source_file": str(ELITE.relative_to(ROOT)),
            "source_locator": {"research_id": team["reference_id"]},
            "format": team.get("format"),
            "generation": team.get("generation"),
            "battle_style": team.get("battle_style", "doubles"),
            "player": team.get("player"),
            "event": team.get("event"),
            "year": team.get("year"),
            "placement": team.get("placement"),
            "roster": roster,
            "sets": sets,
            "tags": infer_tags(roster, sets, team.get("tags")),
            "completeness": team.get("completeness", "roster-only"),
            "confidence": team.get("confidence", "source-backed"),
            "urls": team.get("urls", []),
            "strategy_notes": team.get("strategy_notes", ""),
            "primary_mode": team.get("primary_mode", ""),
            "secondary_mode": team.get("secondary_mode", ""),
            "preview_pressure": team.get("preview_pressure", ""),
            "import_scope": team.get("import_scope", team.get("verdant_fit", "")),
            "verdant_fit": team.get("verdant_fit", ""),
            "ai_requirements": list(team.get("ai_requirements", [])),
            "gimmick_dependencies": list(team.get("gimmick_dependencies", [])),
        }
        records.append(record)
    return records


def build() -> list[dict]:
    records = showdown_records() + smogon_records() + vgc_records() + elite_records()
    ids = [record["reference_id"] for record in records]
    if len(ids) != len(set(ids)):
        raise ValueError("competitive index contains duplicate reference IDs")
    if len(records) < 983:
        raise ValueError(f"competitive index lost base coverage: {len(records)}")
    validate_references(records)
    return records


def validate_references(records: list[dict]) -> None:
    ids = {record["reference_id"] for record in records}
    for record in records:
        if record["source_kind"] != "curated-elite-research":
            continue
        missing = [
            field for field in (
                "player", "event", "year", "battle_style", "roster", "completeness",
                "confidence", "urls", "strategy_notes",
            )
            if not record.get(field)
        ]
        if missing:
            raise ValueError(f"{record['reference_id']} missing curated evidence fields: {missing}")
        if not all(url.startswith("https://") for url in record["urls"]):
            raise ValueError(f"{record['reference_id']} contains a non-public-source URL")
        if record["completeness"].startswith("full-sets") and len(record["sets"]) != len(record["roster"]):
            raise ValueError(f"{record['reference_id']} claims full sets without six complete records")
    if RESERVATIONS.exists():
        payload = json.loads(RESERVATIONS.read_text())
        for reservation in payload.get("reservations", []):
            missing_ids = sorted(set(reservation.get("reference_ids", [])) - ids)
            if missing_ids:
                raise ValueError(
                    f"{reservation.get('encounter')} reserves unknown competitive references: {missing_ids}"
                )
        blueprints = payload.get("marquee_blueprints", {})
        for blueprint in blueprints.get("entries", []) if isinstance(blueprints, dict) else []:
            missing_ids = sorted(set(blueprint.get("candidate_reference_ids", [])) - ids)
            if missing_ids:
                raise ValueError(
                    f"{blueprint.get('anchor')} proposes unknown competitive references: {missing_ids}"
                )


def serialized_index(records: list[dict]) -> str:
    return "".join(json.dumps(record, sort_keys=True) + "\n" for record in records)


def metadata(records: list[dict], serialized: str) -> dict:
    return {
        "version": INDEX_VERSION,
        "record_count": len(records),
        "base_snapshot_count": 983,
        "curated_addition_count": len(records) - 983,
        "source_counts": dict(sorted(Counter(record["source_kind"] for record in records).items())),
        "sha256": hashlib.sha256(serialized.encode()).hexdigest(),
    }


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.write and not args.check:
        parser.error("choose --write or --check")
    records = build()
    serialized = serialized_index(records)
    expected_metadata = metadata(records, serialized)
    if args.write:
        OUTPUT.write_text(serialized)
        METADATA.write_text(json.dumps(expected_metadata, indent=2, sort_keys=True) + "\n")
    if args.check:
        if not OUTPUT.exists() or not METADATA.exists():
            raise SystemExit("FAIL: competitive team index or metadata is missing")
        on_disk_text = OUTPUT.read_text()
        on_disk = [json.loads(line) for line in on_disk_text.splitlines() if line]
        if on_disk != records:
            raise SystemExit("FAIL: competitive team index is stale")
        if json.loads(METADATA.read_text()) != expected_metadata:
            raise SystemExit("FAIL: competitive team index metadata is stale")
    print(
        f"PASS: {len(records)} indexed competitive references "
        f"({len(records) - 983} curated elite additions; sha256 {expected_metadata['sha256'][:12]})"
    )


if __name__ == "__main__":
    main()
