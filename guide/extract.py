#!/usr/bin/env python3
"""Pull everything the guide knows about a location out of the repository.

Nothing here is authored by hand. Pickups come from the Game Book's register,
mart stock from the map's own script, encounters from the wild tables, and
trainers from the authored teams plus the campaign index. Run it and the result
is what the ROM actually ships.

    .venv-studio/bin/python guide/extract.py PetalburgCity PetalburgWoods
    .venv-studio/bin/python guide/extract.py --all > guide/data/locations.json
"""
import argparse
import json
import re
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
BOOK = ROOT / "Game Blueprint/Emerald_Champions_Game_Book.txt"

METHOD_NAMES = {
    "land_mons": "Grass",
    "water_mons": "Surfing",
    "rock_smash_mons": "Rock Smash",
    "fishing_mons": "Fishing",
    "hidden_mons": "Rustling Grass",
}
ROD_BANDS = [("Old Rod", 0, 2), ("Good Rod", 2, 5), ("Super Rod", 5, 10)]


def title(name):
    """SPECIES_TAPU_FINI -> Tapu Fini; ITEM_MAX_REVIVE -> Max Revive."""
    bare = re.sub(r"^(SPECIES|ITEM|MOVE)_", "", name)
    return " ".join(w.capitalize() for w in bare.split("_"))


def pickups():
    pattern = re.compile(
        r"^  R\d+ (visible|hidden) pickup \S+ \((\d+),(\d+)\): (.+?) \| data/maps/(\w+)/map\.json$", re.M)
    out = defaultdict(list)
    for kind, x, y, item, map_name in pattern.findall(BOOK.read_text()):
        out[map_name].append(dict(kind=kind, x=int(x), y=int(y), item=item))
    return out


def item_prices():
    src = (ROOT / "src/data/items.h").read_text()
    prices = {}
    for block in re.split(r"\n    \[ITEM_", src)[1:]:
        ident = "ITEM_" + block.split("]", 1)[0]
        name = re.search(r'\.name\s*=\s*(?:ITEM_NAME|_)\("([^"]+)"\)', block)
        price = re.search(r"\.price\s*=\s*(?:\(I_PRICE >= GEN_7\)\s*\?\s*(\d+)\s*:\s*\d+|(\d+))", block)
        if name:
            prices[ident] = dict(name=name.group(1),
                                 price=int(price.group(1) or price.group(2)) if price else None)
    return prices


def mart_for(map_name, prices):
    # A town's shelves live in its Mart submap, not in the town script.
    candidates = [map_name, map_name + "_Mart", map_name + "_PokemonCenter_1F"]
    text = ""
    for candidate in candidates:
        path = ROOT / "data/maps" / candidate / "scripts.inc"
        if path.exists() and "Pokemart" in path.read_text():
            text = path.read_text()
            break
    if not text:
        return []
    block = re.search(r"_Pokemart_\w*:\n((?:\t\.2byte ITEM_\w+\n)+)", text)
    if not block:
        block = re.search(r"Pokemart\w*:\n((?:\t\.2byte ITEM_\w+\n)+)", text)
    if not block:
        return []
    stock = []
    for ident in re.findall(r"ITEM_\w+", block.group(1)):
        info = prices.get(ident, dict(name=title(ident), price=None))
        stock.append(dict(item=info["name"], price=info["price"]))
    return stock


def encounters_for(map_constant):
    data = json.loads((ROOT / "src/data/wild_encounters.json").read_text())
    groups = data["wild_encounter_groups"][0]["encounters"]
    out = []
    for entry in groups:
        if entry["map"] != map_constant:
            continue
        for field, table in entry.items():
            if not isinstance(table, dict) or "mons" not in table:
                continue
            method = METHOD_NAMES.get(field, field)
            seen = {}
            for slot, mon in enumerate(table["mons"]):
                label = method
                if field == "fishing_mons":
                    label = next((n for n, lo, hi in ROD_BANDS if lo <= slot < hi), method)
                key = (mon["species"], label)
                if key in seen:
                    seen[key]["min"] = min(seen[key]["min"], mon["min_level"])
                    seen[key]["max"] = max(seen[key]["max"], mon["max_level"])
                    continue
                seen[key] = dict(species=title(mon["species"]), method=label,
                                 min=mon["min_level"], max=mon["max_level"])
            out.extend(seen.values())
    return out


def trainers_for(location):
    index = json.loads((ROOT / "work/playtest/battle_index.json").read_text())["battles"]
    teams = (ROOT / "data/emerald_champions/emerald_champions_battle_teams.txt").read_text()
    member = re.compile(
        r"^([A-Z0-9_]+) @(\w+) (\w+) (\w+) (\S+) (-?\d+) \| ([^|]+)\|", re.M)
    blocks = {}
    for block in re.split(r"\n(?=## )", teams):
        head = re.match(r"## (E\d+) (\S+)(?: class=(\w+))?", block)
        if not head:
            continue
        plan = re.search(r"^plan: (.+)$", block, re.M)
        crack = re.search(r"^crack: (.+)$", block, re.M)
        mega = re.search(r"^mega_slots: (.+)$", block, re.M)
        party = []
        for m in member.finditer(block):
            party.append(dict(
                species=title(m.group(1)), item=title(m.group(2)),
                ability=title(m.group(3)), nature=m.group(4).capitalize(),
                evs=m.group(5), offset=int(m.group(6)),
                moves=[title(x.strip()) for x in m.group(7).split(",") if x.strip()]))
        blocks[head.group(2)] = dict(
            e_group=head.group(1), cls=head.group(3) or "", party=party,
            plan=plan.group(1) if plan else "",
            crack=crack.group(1) if crack else "",
            mega=mega.group(1).strip() if mega else "NONE")
    out = []
    for row in index:
        if row["location"] != location:
            continue
        for tid in row["trainer_ids"]:
            info = blocks.get(tid, {})
            cap = row["cap_window_max"]
            party = []
            for mon in info.get("party", []):
                # Medium difficulty is the cap plus the member's offset, less two.
                party.append(dict(mon, level=max(2, cap + mon["offset"] - 2)))
            out.append(dict(trainer=tid, play_order=row["play_order"],
                            cls=row["class"], cap=cap, e_group=info.get("e_group"),
                            plan=info.get("plan", ""), crack=info.get("crack", ""),
                            mega=info.get("mega", "NONE"), party=party))
    return sorted(out, key=lambda r: r["play_order"])


def map_constant(map_name):
    """PetalburgCity -> MAP_PETALBURG_CITY, Route102 -> MAP_ROUTE102.

    Route numbers stay welded to the word, which is how the wild tables and the
    map headers spell them.
    """
    parts = re.findall(r"[A-Z0-9]+(?![a-z])|[A-Z][a-z]*\d*|\d+", map_name)
    return "MAP_" + "_".join(p.upper() for p in parts)


def describe(map_name, cache):
    layouts = {x["id"]: x for x in
               json.loads((ROOT / "data/layouts/layouts.json").read_text())["layouts"]}
    m = json.loads((ROOT / "data/maps" / map_name / "map.json").read_text())
    layout = layouts[m["layout"]]
    warps = [dict(x=w["x"], y=w["y"], dest=w.get("dest_map", ""))
             for w in m.get("warp_events", [])]
    return dict(
        map=map_name,
        constant=map_constant(map_name),
        width=layout["width"], height=layout["height"],
        region_section=m.get("region_map_section", ""),
        items=cache["pickups"].get(map_name, []),
        mart=mart_for(map_name, cache["prices"]),
        encounters=encounters_for(map_constant(map_name)),
        trainers=trainers_for(map_name),
        warps=warps,
    )


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("maps", nargs="*", help="map directory names")
    ap.add_argument("--all", action="store_true", help="every Hoenn map with content")
    args = ap.parse_args()

    cache = dict(pickups=pickups(), prices=item_prices())

    names = args.maps
    if args.all:
        groups = json.loads((ROOT / "data/maps/map_groups.json").read_text())
        names = [n for g in groups["group_order"] for n in groups[g]]

    out = {}
    for name in names:
        try:
            out[name] = describe(name, cache)
        except Exception as exc:                       # noqa: BLE001
            print(f"  FAIL {name}: {type(exc).__name__}: {exc}", file=sys.stderr)
    json.dump(out, sys.stdout, indent=2)
    print()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
