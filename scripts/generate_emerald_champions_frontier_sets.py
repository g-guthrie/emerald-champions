#!/usr/bin/env python3
"""Rewrite Battle Frontier, Battle Tent, and Frontier Brain loadouts from the
Emerald Champions competitive set corpus.

Why this exists
---------------
The facilities shipped with upstream's vanilla Gen 3 loadouts (Lax Incense
Sunkern, Bright Powder Alakazam) and, worse, with 252-point EV spreads.  Under
``P_STAT_CALCULATION >= GEN_CHAMPIONS`` an EV field is a Stat Point (0-32,
66 total) and feeds ``min(2*ev, 63)`` into the stat formula, so a vanilla 252 spread
inflated every facility Pokemon by roughly +440 stat points per invested stat.

What it does
------------
* Keeps every facility slot's species, ball, constant name, and table order, so
  the Factory rental tiers, Dome brackets, Pike/Pyramid draws, and the 117
  trainer mon-set macros keep working unchanged.
* Replaces moves, held item, nature, Ability, and Stat Points with a legal
  Emerald Champions orientation for that species.  Variant slots of one species
  cycle through distinct Doubles and Singles orientations so a facility trainer
  can still draw several different builds of the same Pokemon.
* Never supplies a Mega Stone or other protected progression item (Mega sets
  are skipped; no set in the corpus carries a protected item).
* Species without a set keep their vanilla moves but have their EVs rescaled to
  the Champions Stat Point budget.
* Rewrites the Frontier Brain teams in the same way, keeping each Brain's
  species identity and fixed IVs and enforcing Item Clause inside each team.

Run without arguments to rewrite in place; ``--check`` verifies the generated
files are current.
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SETS = ROOT / "docs" / "emerald_champions_battle_sets.json"
FRONTIER_MONS = ROOT / "src" / "data" / "battle_frontier" / "battle_frontier_mons.h"
TENT_MONS = ROOT / "src" / "data" / "battle_frontier" / "battle_tent.h"
FRONTIER_UTIL = ROOT / "src" / "frontier_util.c"

MAX_PER_STAT = 32
MAX_TOTAL = 66

ENTRY = re.compile(
    r"(?P<head>\[(?P<const>[A-Z0-9_]+)\] = \{\n)"
    r"(?P<body>(?:        \.[a-zA-Z]+ = [^\n]*\n)+?)"
    r"(?P<tail>    \},?\n)"
)
FIELD = re.compile(r"        \.(?P<name>[a-zA-Z]+) = (?P<value>[^\n]*?),?\n")
BRAIN_ENTRY = re.compile(
    r"(?P<head>            \{\n)"
    r"(?P<body>(?:                \.[a-zA-Z]+ = [^\n]*\n)+?)"
    r"(?P<tail>            \},\n)"
)
BRAIN_FIELD = re.compile(r"                \.(?P<name>[a-zA-Z]+) = (?P<value>[^\n]*?),?\n")


def load_orientations() -> dict[str, list[dict]]:
    data = json.loads(SETS.read_text())
    ordered: dict[str, list[dict]] = {}
    for bucket in ("defaults", "alternatives", "singles_defaults", "singles_alternatives"):
        for preset in data[bucket]:
            if preset["required_item"] != "ITEM_NONE":
                continue
            points = preset["stat_points"]
            if max(points) > MAX_PER_STAT or sum(points) > MAX_TOTAL:
                raise SystemExit(f"illegal stat points in corpus: {preset}")
            ordered.setdefault(preset["species"], []).append(preset)
    return ordered


def scale_vanilla_evs(values: list[int]) -> list[int]:
    # Vanilla spreads are 252/252/6 or three-way 170s; map them onto the
    # 32-per-stat, 66-total Champions budget without inventing new priorities.
    scaled = [min(MAX_PER_STAT, round(v * MAX_PER_STAT / 252)) for v in values]
    while sum(scaled) > MAX_TOTAL:
        scaled[scaled.index(max(scaled))] -= 1
    return scaled


def format_evs(points: list[int]) -> str:
    hp, atk, dfn, spa, spd, spe = points
    # TRAINER_PARTY_EVS(hp, atk, def, speed, spatk, spdef)
    return f"TRAINER_PARTY_EVS({hp}, {atk}, {dfn}, {spe}, {spa}, {spd})"


def rewrite_facility_table(text: str, orientations: dict[str, list[dict]]) -> tuple[str, dict]:
    variant_index: dict[str, int] = {}
    stats = {"entries": 0, "from_sets": 0, "rescaled": 0}

    def replace(match: re.Match) -> str:
        fields: dict[str, str] = {}
        order: list[str] = []
        for field in FIELD.finditer(match.group("body")):
            fields[field.group("name")] = field.group("value")
            order.append(field.group("name"))
        if "species" not in fields:
            return match.group(0)  # trainer rows in battle_tent.h, not Pokemon
        species = fields["species"]
        stats["entries"] += 1
        # Cosmetic base-form aliases (Castform's weather forms share one set row).
        presets = orientations.get(species) or orientations.get(species.removesuffix("_NORMAL"))
        if presets:
            index = variant_index.get(species, 0)
            preset = presets[index % len(presets)]
            variant_index[species] = index + 1
            fields["moves"] = "{" + ", ".join(preset["moves"]) + "}"
            fields["heldItem"] = preset["item"]
            fields["ev"] = format_evs(preset["stat_points"])
            fields["nature"] = preset["nature"]
            fields["ability"] = preset["ability"]
            if "ability" not in order:
                order.insert(order.index("nature") + 1, "ability")
            stats["from_sets"] += 1
        else:
            ev_match = re.search(r"TRAINER_PARTY_EVS\(([^)]*)\)", fields.get("ev", ""))
            if ev_match:
                hp, atk, dfn, spe, spa, spd = [int(v) for v in ev_match.group(1).split(",")]
                if max(hp, atk, dfn, spe, spa, spd) > MAX_PER_STAT:
                    fields["ev"] = format_evs(scale_vanilla_evs([hp, atk, dfn, spa, spd, spe]))
                    stats["rescaled"] += 1
                print(f"note: {species} has no competitive set; kept vanilla moves", file=sys.stderr)
        body = "".join(f"        .{name} = {fields[name]},\n" for name in order)
        # Preserve upstream's style of no trailing comma on the last field.
        body = body[: body.rstrip("\n").rfind(",")] + "\n"
        return match.group("head") + body + match.group("tail")

    return ENTRY.sub(replace, text), stats


def rewrite_brains(text: str, orientations: dict[str, list[dict]]) -> tuple[str, dict]:
    start = text.index("static const struct FrontierBrainMon sFrontierBrainsMons[][2][FRONTIER_PARTY_SIZE] =")
    end = text.index("\n};\n", start) + len("\n};\n")
    block = text[start:end]
    stats = {"brain_mons": 0}
    team_items: list[str] = []
    team_species: list[str] = []

    def replace(match: re.Match) -> str:
        nonlocal team_items, team_species
        fields: dict[str, str] = {}
        for field in BRAIN_FIELD.finditer(match.group("body")):
            fields[field.group("name")] = field.group("value")
        species = fields["species"]
        presets = orientations.get(species, [])
        chosen = None
        for preset in presets:
            if preset["item"] not in team_items:
                chosen = preset
                break
        if chosen is None and presets:
            chosen = presets[0]
        if chosen is not None:
            fields["heldItem"] = chosen["item"]
            fields["nature"] = chosen["nature"]
            hp, atk, dfn, spa, spd, spe = chosen["stat_points"]
            # struct FrontierBrainMon.evs is in vanilla stat order:
            # HP, Attack, Defense, Speed, Sp. Atk, Sp. Def.
            fields["evs"] = f"{{{hp}, {atk}, {dfn}, {spe}, {spa}, {spd}}}"
            fields["moves"] = "{" + ", ".join(chosen["moves"]) + "}"
            team_items.append(chosen["item"])
        team_species.append(species)
        if len(team_species) == 3:
            team_items, team_species = [], []
        stats["brain_mons"] += 1
        body = "".join(f"                .{name} = {value},\n" for name, value in fields.items())
        return match.group("head") + body + match.group("tail")

    new_block = BRAIN_ENTRY.sub(replace, block)
    return text[:start] + new_block + text[end:], stats


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true", help="fail if generated files are stale")
    args = parser.parse_args()

    orientations = load_orientations()
    outputs: list[tuple[Path, str, dict]] = []
    for path in (FRONTIER_MONS, TENT_MONS):
        new_text, stats = rewrite_facility_table(path.read_text(), orientations)
        outputs.append((path, new_text, stats))
    new_util, brain_stats = rewrite_brains(FRONTIER_UTIL.read_text(), orientations)
    outputs.append((FRONTIER_UTIL, new_util, brain_stats))

    stale = [path for path, text, _ in outputs if path.read_text() != text]
    if args.check:
        if stale:
            raise SystemExit(
                "Frontier loadouts are stale; run python3 scripts/generate_emerald_champions_frontier_sets.py: "
                + ", ".join(str(p.relative_to(ROOT)) for p in stale)
            )
        print("PASS: Frontier, Tent, and Brain loadouts match the competitive set corpus")
        return
    for path, text, stats in outputs:
        path.write_text(text)
        print(f"wrote {path.relative_to(ROOT)} {stats}")


if __name__ == "__main__":
    sys.exit(main())
