#!/usr/bin/env python3
"""Inventory single-player prerequisites that can stop campaign automation."""

from __future__ import annotations

import argparse
import json
import re
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
EXCLUDED_SCRIPT_NAMES = {"debug.inc", "cable_club.inc", "cable_club_frlg.inc"}
LABEL = re.compile(r"^([A-Za-z_]\w*)::?\s*$")

SPECIAL_PARTY_GATES = {
    "CalculatePlayerPartyCount",
    "CountPartyNonEggMons",
    "CountPartyAliveNonEggMons",
    "CountPartyAliveNonEggMons_IgnoreVar0x8004Slot",
    "DoesPlayerPartyHaveSelectedSpeciesFamily",
    "DoesPlayerPartyContainSpecies",
    "CheckPartyHasSpecies",
    "PlayerPartyContainsSpeciesWithPlayerID",
    "IsPokemonJumpSpeciesInParty",
    "IsDodrioInParty",
    "IsStarterInParty",
    "IsGrassTypeInParty",
}
FACILITY_GATES = {
    "ChoosePartyForBattleFrontier",
    "ChooseHalfPartyForBattle",
    "ReducePlayerPartyToSelectedMons",
}
PARTY_SELECTORS = {
    "ChoosePartyMon",
}


def sources() -> list[Path]:
    result = sorted((ROOT / "data/maps").glob("*/scripts.inc"))
    result += sorted((ROOT / "data/scripts").glob("*.inc"))
    return [
        path for path in result
        if "_Frlg" not in str(path)
        and path.name not in EXCLUDED_SCRIPT_NAMES
        and "frlg" not in path.name.lower()
    ]


def classify(line: str) -> tuple[str, str, str] | None:
    stripped = line.split("@", 1)[0].strip()
    command = stripped.split(None, 1)[0] if stripped else ""
    if re.match(r"(?:trainerbattle_[a-z0-9_]*double|multi_2_vs_2)\b", stripped):
        return "party_usable_count", "route_setup", "enter with at least two usable Pokemon"
    if command == "getpartysize":
        return "party_size", "route_setup", "preserve the expected free/occupied party slot"
    if command == "checkfieldmove":
        return "field_move", "native_coverage", "obtain the campaign license and execute the native field move"
    if command == "checkitemspace":
        return "bag_capacity", "route_setup", "keep the destination pocket open; retry behavior remains a native test"
    if command == "giveitem":
        return "bag_capacity", "route_setup", "keep the destination pocket open for the scripted reward"
    if command in {"givemon", "giveegg"}:
        return "pokemon_storage_capacity", "route_setup", "keep a party or PC destination available for the gift"
    if command == "checkmoney":
        return "money", "route_setup", "earn or retain the requested amount before taking this route"
    if command in {"gettime", "gettimeofday", "dotimebasedevents"}:
        return "time_or_tide", "route_setup", "run the segment under an explicit deterministic RTC state"
    if command == "frontier_checkineligible":
        return "facility_eligibility", "route_setup", "prepare and select a legal facility party natively"
    if command == "choosecontestmon":
        return "facility_eligibility", "native_coverage", "exercise the native eligible-Pokemon selector"
    special = re.match(r"special(?:var)?(?:\s+\w+,)?\s+([A-Za-z_]\w*)", stripped)
    if special:
        name = special.group(1)
        if name in SPECIAL_PARTY_GATES:
            return "special_party_composition", "route_setup", "prepare the species, Egg, health, or party-count prerequisite"
        if name in FACILITY_GATES:
            return "facility_eligibility", "native_coverage", "exercise the native party-selection contract"
        if name in PARTY_SELECTORS:
            return "party_selector", "native_coverage", "exercise the native party-selection and cancel contract"
        if name == "UpdateShoalTideFlag":
            return "time_or_tide", "route_setup", "run separate deterministic high- and low-tide coverage"
    return None


def audit() -> dict[str, object]:
    rows = []
    failures = []
    candidate_special = re.compile(
        r"^\s*special(?:var)?[^\n]*(?:Party|Usable|Healthy|Species|FieldMove|Eligible|Eligibility|Tide|Time)",
        re.I,
    )
    for path in sources():
        current_label = "<top-level>"
        for number, raw in enumerate(path.read_text(errors="ignore").splitlines(), 1):
            match = LABEL.match(raw.strip())
            if match:
                current_label = match.group(1)
            result = classify(raw)
            if result is not None:
                category, handling, reason = result
                rows.append({
                    "path": path.relative_to(ROOT).as_posix(),
                    "line": number,
                    "label": current_label,
                    "category": category,
                    "handling": handling,
                    "reason": reason,
                    "source": raw.strip(),
                })
            elif candidate_special.search(raw) and not any(
                harmless in raw for harmless in (
                    "HealPlayerParty", "SavePlayerParty", "LoadPlayerParty",
                    "GetSelectedMonNicknameAndSpecies", "GetContestLadyMonSpecies",
                    "OpenEmeraldChampionsBattleItemMart", "TryLoseFansFromPlayTime",
                    "ScriptGetPartyMonSpecies", "ChangeSelectedMonSpecies",
                    "TryDiscoverEligibleLegendarySign",
                    "IsPokerusInParty",
                )
            ):
                failures.append(f"{path.relative_to(ROOT)}:{number}: unclassified prerequisite-like special: {raw.strip()}")

    counts = Counter(row["category"] for row in rows)
    handling = Counter(row["handling"] for row in rows)
    return {
        "schema_version": 1,
        "scope": "live non-FRLG single-player map and common event scripts",
        "prerequisites": rows,
        "summary": {
            "total": len(rows),
            "by_category": dict(sorted(counts.items())),
            "by_handling": dict(sorted(handling.items())),
            "failures": len(failures),
        },
        "failures": failures,
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--json", action="store_true")
    args = parser.parse_args()
    result = audit()
    if args.json:
        print(json.dumps(result, indent=2, sort_keys=True))
    elif result["failures"]:
        print("\n".join(result["failures"]))
    else:
        print(f"PASS: prerequisite inventory {result['summary']}")
    return bool(result["failures"])


if __name__ == "__main__":
    raise SystemExit(main())
