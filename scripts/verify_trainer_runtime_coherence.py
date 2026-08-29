#!/usr/bin/env python3
"""Reject trainer sets whose authored data contradicts their executable plan."""

from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PARTIES = ROOT / "src/data/trainers.party"

# These five teams were individually reviewed as phased speed-control teams.
# Any new Trick Room + Tailwind party must be reviewed before joining this set.
APPROVED_DUAL_SPEED = {
    "TRAINER_BETHANY",
    "TRAINER_CAMERON_1",
    "TRAINER_CHIP",
    "TRAINER_EDMOND",
    "TRAINER_LEROY",
}

LOWERS_ATTACK = {
    "NATURE_BOLD",
    "NATURE_MODEST",
    "NATURE_CALM",
    "NATURE_TIMID",
}
LOWERS_SP_ATTACK = {
    "NATURE_ADAMANT",
    "NATURE_IMPISH",
    "NATURE_CAREFUL",
    "NATURE_JOLLY",
}
SUN_SOURCES = {
    "ABILITY_DROUGHT",
    "ABILITY_ORICHALCUM_PULSE",
    "MOVE_SUNNY_DAY",
}
CHARGED_BY_SUN = {"MOVE_SOLAR_BEAM", "MOVE_SOLAR_BLADE"}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def move_categories() -> dict[str, str]:
    text = (ROOT / "src/data/moves_info.h").read_text()
    markers = list(re.finditer(r"(?m)^\s*\[(MOVE_[A-Z0-9_]+)\]\s*=\s*\{", text))
    result: dict[str, str] = {}
    for index, marker in enumerate(markers):
        end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
        body = text[marker.end():end]
        category = re.search(r"\.category\s*=\s*(DAMAGE_CATEGORY_[A-Z]+)", body)
        result[marker.group(1)] = category.group(1) if category else "DAMAGE_CATEGORY_STATUS"
    return result


def party_blocks() -> dict[str, str]:
    text = PARTIES.read_text()
    markers = list(re.finditer(r"(?m)^=== (TRAINER_[A-Z0-9_]+) ===$", text))
    return {
        marker.group(1): text[marker.end():markers[index + 1].start() if index + 1 < len(markers) else len(text)]
        for index, marker in enumerate(markers)
    }


def mon_blocks(party: str) -> list[str]:
    markers = list(re.finditer(r"(?m)^SPECIES_[A-Z0-9_]+(?: @ ITEM_[A-Z0-9_]+)?$", party))
    return [
        party[marker.start():markers[index + 1].start() if index + 1 < len(markers) else len(party)]
        for index, marker in enumerate(markers)
    ]


def field(block: str, name: str) -> str:
    match = re.search(rf"(?m)^{re.escape(name)}: (.+)$", block)
    return match.group(1).strip() if match else ""


def main() -> None:
    categories = move_categories()
    parties = party_blocks()
    failures: list[str] = []
    mon_count = 0

    dual_speed = {
        trainer for trainer, party in parties.items()
        if "MOVE_TRICK_ROOM" in party and "MOVE_TAILWIND" in party
    }
    if dual_speed != APPROVED_DUAL_SPEED:
        failures.append(
            "unreviewed Trick Room + Tailwind parties: "
            f"added={sorted(dual_speed - APPROVED_DUAL_SPEED)} "
            f"removed={sorted(APPROVED_DUAL_SPEED - dual_speed)}"
        )

    for trainer, party in parties.items():
        has_sun = any(source in party for source in SUN_SOURCES)
        for mon in mon_blocks(party):
            mon_count += 1
            species = re.match(r"(SPECIES_[A-Z0-9_]+)", mon).group(1)
            nature = field(mon, "Nature")
            evs = field(mon, "EVs")
            points = {
                stat: int(value)
                for value, stat in re.findall(r"(\d+) (HP|Atk|Def|SpA|SpD|Spe)", evs)
            }
            moves = re.findall(r"(?m)^- (MOVE_[A-Z0-9_]+)$", mon)
            physical = [move for move in moves if categories.get(move) == "DAMAGE_CATEGORY_PHYSICAL"]
            special = [move for move in moves if categories.get(move) == "DAMAGE_CATEGORY_SPECIAL"]

            if points.get("Atk") == 32 and physical and not special and nature in LOWERS_ATTACK:
                failures.append(f"{trainer}/{species}: {nature} lowers its only invested attack category")
            if points.get("SpA") == 32 and special and not physical and nature in LOWERS_SP_ATTACK:
                failures.append(f"{trainer}/{species}: {nature} lowers its only invested attack category")

            item_match = re.search(r"@ (ITEM_[A-Z0-9_]+)", mon.splitlines()[0])
            item = item_match.group(1) if item_match else "ITEM_NONE"
            unsupported_charge = CHARGED_BY_SUN.intersection(moves)
            if unsupported_charge and not has_sun and item != "ITEM_POWER_HERB":
                failures.append(
                    f"{trainer}/{species}: {sorted(unsupported_charge)} has no Sun or Power Herb"
                )

    require(not failures, f"{len(failures)} trainer runtime-coherence failures:\n" + "\n".join(failures))
    print(
        "PASS: "
        f"{mon_count} trainer Pokemon have coherent attack natures and charge support; "
        f"{len(dual_speed)} reviewed dual-speed parties remain"
    )


if __name__ == "__main__":
    main()
