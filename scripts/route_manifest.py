#!/usr/bin/env python3
"""Route-by-route availability manifest, generated from the tuning pools.

The single source of truth is reference_pool.encounter_pool: the same pool the
tuning check and the battle driver use, with the walkthrough-order rule (a
route trainer sees only the areas before its route; gyms see everything
reachable). This walks the campaign battles in story order and, at every
point where the pool changes, prints what it added - Pokemon by source,
evolutions, and battle items by source, wild held items included.

    python3 scripts/route_manifest.py [--out artifacts/progression-manifest/route-manifest.txt]
"""
from __future__ import annotations

import argparse
from collections import defaultdict
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parent))
import reference_pool as rp  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUT = ROOT / "artifacts/progression-manifest/route-manifest.txt"

# Key items, balls, medicine and other non-battle goods are left out.
SKIP_ITEM_WORDS = ("BALL", "POTION", "TICKET", "REVIVE", "REPEL", "ETHER", "ELIXIR", "HEAL", "LETTER",
                   "PAIL", "POUCH", "ROD", "BEACON", "VIAL", "LEVELER", "REGENERATOR", "ITEMFINDER",
                   "SEA_MAP", "BIKE", "SCANNER", "GOGGLES", "CARD", "FLUTE", "CANDY", "BOTTLE_CAP",
                   "NUGGET", "MUSHROOM", "PEARL", "STARDUST", "STAR_PIECE", "FEATHER", "SHARD", "HONEY",
                   "SCALE", "MAIL", "FOSSIL", "PLUME", "SKULL", "CLAW_FOSSIL", "OLD_AMBER", "ESCAPE_ROPE",
                   "DEVON_GOODS", "DEVON_PARTS", "BASEMENT_KEY", "STORAGE_KEY", "METEORITE", "SCOPE", "TM", "HM",
                   "KEY", "PASS", "COIN", "SOOT_SACK", "WAILMER_PAIL", "MAGMA_EMBLEM", "GO_GOGGLES")
KEEP_ITEMS = {"ITEM_DEEP_SEA_SCALE", "ITEM_DRAGON_SCALE", "ITEM_PRISM_SCALE", "ITEM_HONEY",
              "ITEM_LUCKY_PUNCH", "ITEM_MAX_REVIVE"}


def battle_item(item: str) -> bool:
    if item in KEEP_ITEMS:
        return True
    return not any(word in item for word in SKIP_ITEM_WORDS)


def pretty(name: str) -> str:
    return name.replace("ITEM_", "").replace("_", " ").title()


def species_source(src: dict) -> tuple[str, str]:
    kind = src.get("kind") or "?"
    where = (src.get("where") or "").split(" (")[0]
    detail = src.get("detail") or ""
    if kind == "wild":
        method = detail.split(" ")[1] if " " in detail else ""
        method = method.replace("_mons", "").replace("land", "grass").replace("water", "surf")
        return f"wild {method}", detail.split(" ")[0] or where
    if kind == "evolution":
        return "evolution", detail
    return kind, where or detail


def item_source(src: dict) -> str:
    kind = src.get("kind") or "?"
    detail = src.get("detail") or ""
    where = (src.get("where") or "").split(" (")[0]
    if kind == "wild_held":
        return f"wild held ({detail.replace('held by wild ', '').split(' (')[0]})"
    return f"{kind}{' ' + where if where else ''}"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--out", type=Path, default=DEFAULT_OUT)
    args = parser.parse_args()

    builder = rp.Builder()
    encounters = rp.Encounters(builder)
    rows = sorted(encounters.build(), key=lambda r: r["story_order"])

    lines = ["INCLEMENT EMERALD 2 - WHAT EACH BATTLE'S CHALLENGER CAN HAVE, ROUTE BY ROUTE",
             f"Generated from source at {rp.git_head()} by scripts/route_manifest.py from the tuning pools",
             "(scripts/reference_pool.py encounter_pool + scripts/walkthrough_order.py).",
             "",
             "Rule: a route trainer's challenger has every area walked before that route, never the",
             "route's own new Pokemon or items (the first rival battle on Route 103 counts Route 103).",
             "Gyms and the League count everything reachable. Each step lists only what is new since",
             "the previous step. Starters: one region, two of its three, until the Game Corner.",
             "No Pokemon has EVs before the Knuckle Badge.", ""]
    seen_species: set[str] = set()
    seen_items: set[str] = set()
    last_key = None
    group: list[str] = []
    step = 0

    def flush(pool, battles):
        nonlocal step
        species = {s["name"]: s["source"] for s in pool["species"]}
        items = {i["item"]: i["source"] for i in pool["items"]}
        new_species = {k: v for k, v in species.items() if k not in seen_species}
        new_items = {k: v for k, v in items.items() if k not in seen_items and battle_item(k)}
        step += 1
        lines.append("=" * 100)
        lines.append(f"STEP {step} - cap {pool['cap']} - {pool['battle_map'] or '?'}"
                     f" ({'full reachable pool' if 'upper bound' in pool['scope'] else 'walkthrough order'})")
        lines.append("  Battles: " + "; ".join(battles))
        by_source = defaultdict(list)
        for name, src in sorted(new_species.items()):
            kind, where = species_source(src)
            if kind == "evolution":
                by_source["Evolution"].append(f"{name} <- {where}")
            else:
                by_source[f"{kind.capitalize()} - {where}"].append(name)
        if by_source:
            lines.append(f"  New Pokemon ({len(new_species)}):")
            for key in sorted(by_source, key=lambda k: (k == "Evolution", k)):
                lines.append(f"    {key}: " + ", ".join(by_source[key]))
        else:
            lines.append("  New Pokemon: none")
        if new_items:
            lines.append(f"  New battle items ({len(new_items)}):")
            for item, src in sorted(new_items.items()):
                lines.append(f"    {pretty(item)} - {item_source(src)}")
        lines.append("")
        seen_species.update(species)
        seen_items.update(items)

    pending_pool = None
    for row in rows:
        milestone = row["first_milestone"]
        if not milestone:
            continue
        try:
            pool = rp.encounter_pool(row["trainer"], milestone, builder=builder, encounters=encounters)
        except ValueError:
            continue
        key = (frozenset(s["name"] for s in pool["species"]), frozenset(i["item"] for i in pool["items"]))
        label = f"#{row['story_order']} {row['trainer'].removeprefix('TRAINER_')}"
        if key == last_key:
            group.append(label)
            continue
        if pending_pool is not None:
            flush(pending_pool, group)
        pending_pool, group, last_key = pool, [label], key
    if pending_pool is not None:
        flush(pending_pool, group)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text("\n".join(lines) + "\n")
    print(f"wrote {args.out.relative_to(ROOT)}: {step} steps")


if __name__ == "__main__":
    main()
