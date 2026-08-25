#!/usr/bin/env python3
"""Snapshot tournament-winning and Smogon sample teams for Verdant design."""

from __future__ import annotations

import json
import re
import urllib.request
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
VGC_HISTORY_URL = "https://vgchistory.com/data.js"
SMOGON_TEAMS_BASE = "https://pkmn.github.io/smogon/data/teams/"
USER_AGENT = "Verdant-Inclement-Emerald-Team-Research/1.0"


def fetch(url: str, referer: str | None = None) -> bytes:
    headers = {"User-Agent": USER_AGENT}
    if referer:
        headers["Referer"] = referer
    request = urllib.request.Request(url, headers=headers)
    with urllib.request.urlopen(request, timeout=60) as response:
        return response.read()


def import_vgc_history() -> dict:
    source = fetch(VGC_HISTORY_URL, "https://vgchistory.com/data").decode()
    marker = re.search(r"\bconst T = ", source)
    if not marker:
        raise ValueError("VGC History tournament registry was not found")
    tournaments = json.JSONDecoder().raw_decode(source, marker.end())[0]
    champions = []
    for tournament in tournaments:
        champion = next(
            (
                standing
                for standing in tournament.get("standings", [])
                if standing.get("po") == 1 and len(standing.get("team", [])) == 6
            ),
            None,
        )
        if champion is None:
            continue
        champions.append(
            {
                "tournament_id": tournament["id"],
                "tournament": tournament.get("full") or tournament.get("name"),
                "year": tournament.get("year"),
                "game": tournament.get("game"),
                "tier": tournament.get("tier"),
                "regulation": tournament.get("reg"),
                "players": tournament.get("players"),
                "verified": bool(tournament.get("verified")),
                "champion": champion.get("p"),
                "record": champion.get("rec", ""),
                "team": champion["team"],
                "source": tournament.get("src", {}),
            }
        )
    if len(champions) < 350 or not all(entry["verified"] for entry in champions):
        raise ValueError("VGC champion coverage unexpectedly regressed")
    return {
        "source": VGC_HISTORY_URL,
        "credit": "VGC History — The Record Book (vgchistory.com)",
        "scope": "Every verified event champion with a complete six-Pokémon team in the archive",
        "champion_team_count": len(champions),
        "years": [min(entry["year"] for entry in champions), max(entry["year"] for entry in champions)],
        "tier_counts": dict(sorted(Counter(entry["tier"] for entry in champions).items())),
        "teams": champions,
    }


def import_smogon_samples() -> dict:
    formats = [f"gen{generation}{tier}" for generation in range(4, 10) for tier in ("ou", "uu", "nu")]
    teams_by_format = {}
    for format_id in formats:
        url = f"{SMOGON_TEAMS_BASE}{format_id}.json"
        teams = json.loads(fetch(url).decode())
        if not isinstance(teams, list) or not all(len(team.get("data", [])) == 6 for team in teams):
            raise ValueError(f"invalid Smogon sample-team payload: {format_id}")
        teams_by_format[format_id] = teams
    return {
        "source": f"{SMOGON_TEAMS_BASE}index.json",
        "credit": "pkmn/smogon teams API, aggregated from Smogon and Pokémon Showdown sample-team sources",
        "scope": "Complete sample teams for OU, UU, and NU in Generations 4 through 9",
        "sample_team_count": sum(len(teams) for teams in teams_by_format.values()),
        "format_counts": {format_id: len(teams) for format_id, teams in teams_by_format.items()},
        "formats": teams_by_format,
    }


def main() -> None:
    vgc = import_vgc_history()
    smogon = import_smogon_samples()
    vgc_path = ROOT / "docs/vgc_major_champion_teams.json"
    smogon_path = ROOT / "docs/smogon_gen4_9_ou_uu_nu_sample_teams.json"
    vgc_path.write_text(json.dumps(vgc, indent=2) + "\n")
    smogon_path.write_text(json.dumps(smogon, indent=2) + "\n")
    print(f"wrote {vgc['champion_team_count']} VGC champion teams")
    print(f"wrote {smogon['sample_team_count']} Smogon sample teams")


if __name__ == "__main__":
    main()
