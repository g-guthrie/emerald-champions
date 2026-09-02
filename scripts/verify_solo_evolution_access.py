#!/usr/bin/env python3
"""Prove every trade evolution has a native single-player path."""

from __future__ import annotations

import glob
import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def main() -> None:
    trade_paths = []
    solo_items = set()
    for path_string in glob.glob(str(ROOT / "src/data/pokemon/species_info/*families.h")):
        path = Path(path_string)
        source = path.read_text()
        starts = list(re.finditer(r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*", source))
        for index, match in enumerate(starts):
            block = source[match.start(): starts[index + 1].start() if index + 1 < len(starts) else len(source)]
            source_species = match.group(1)
            for target in re.findall(r"\{EVO_TRADE,[^,}]+,\s*(SPECIES_[A-Z0-9_]+)", block):
                trade_paths.append((source_species, target))
                alternatives = re.findall(
                    r"\{(EVO_[A-Z0-9_]+),\s*([^,}]+),\s*" + re.escape(target),
                    block,
                )
                alternatives = [(method, param.strip()) for method, param in alternatives if method != "EVO_TRADE"]
                require(alternatives, f"{source_species} -> {target} remains trade-only")
                for method, param in alternatives:
                    if method == "EVO_ITEM":
                        solo_items.add(param)

    require(len(trade_paths) == 30, f"trade evolution count drifted: {len(trade_paths)}")
    require(
        ("SPECIES_KARRABLAST", "SPECIES_ESCAVALIER") in trade_paths
        and ("SPECIES_SHELMET", "SPECIES_ACCELGOR") in trade_paths,
        "paired Unova trade evolutions disappeared",
    )

    item_info = (ROOT / "src/data/items.h").read_text()
    linking_start = item_info.index("[ITEM_LINKING_CORD]")
    linking_block = item_info[linking_start: item_info.index("[ITEM_PEAT_BLOCK]", linking_start)]
    require(".pocket = POCKET_KEY_ITEMS" in linking_block, "Linking Cord is still consumable")
    require(".importance = 1" in linking_block, "Linking Cord can be purchased as a duplicate stack")
    require("reusable cord" in linking_block, "Linking Cord description does not explain reuse")
    require("giveitem ITEM_LINKING_CORD" in (ROOT / "data/maps/ShoalCave_LowTideEntranceRoom/scripts.inc").read_text(),
            "Linking Cord has no campaign acquisition")

    acquisition_text = []
    for path in (ROOT / "data/maps").glob("*/scripts.inc"):
        acquisition_text.append(path.read_text())
    for path in (ROOT / "data/scripts").glob("*.inc"):
        acquisition_text.append(path.read_text())
    for path in (ROOT / "data/maps").glob("*/map.json"):
        acquisition_text.append(path.read_text())
    acquisition = "\n".join(acquisition_text)
    missing_items = sorted(item for item in solo_items if item != "ITEM_LINKING_CORD" and item not in acquisition)
    require(not missing_items, f"solo evolution items lack campaign acquisition: {missing_items}")

    new_mauville = json.loads((ROOT / "data/maps/NewMauville_Inside/map.json").read_text())
    require(any(
        row.get("trainer_sight_or_berry_tree_id") == "ITEM_UPGRADE"
        for row in new_mauville["object_events"]
    ), "Porygon's Upgrade is not obtainable")
    require("IF_SPECIES_IN_PARTY, SPECIES_SHELMET" in (ROOT / "src/data/pokemon/species_info/gen_5_families.h").read_text(),
            "Karrablast solo evolution does not retain Shelmet's identity")
    require("IF_SPECIES_IN_PARTY, SPECIES_KARRABLAST" in (ROOT / "src/data/pokemon/species_info/gen_5_families.h").read_text(),
            "Shelmet solo evolution does not retain Karrablast's identity")

    gen8 = (ROOT / "src/data/pokemon/species_info/gen_8_families.h").read_text()
    gen9 = (ROOT / "src/data/pokemon/species_info/gen_9_families.h").read_text()
    require(
        "EVOLUTION({EVO_LEVEL, 30, SPECIES_ALCREMIE_STRAWBERRY_VANILLA_CREAM})" in gen8,
        "Milcery lost its deterministic no-item Leveler path",
    )
    require(
        gen9.count("EVOLUTION({EVO_LEVEL, 45, SPECIES_GHOLDENGO})") == 2,
        "both Gimmighoul forms must retain their deterministic no-coin Leveler path",
    )
    require(
        "IF_BAG_ITEM_COUNT, ITEM_GIMMIGHOUL_COIN" not in gen9,
        "Gimmighoul silently regained the unavailable 999-coin grind",
    )
    load = (ROOT / "src/overworld.c").read_text()
    require("CountTotalItemQuantityInBag(ITEM_LINKING_CORD)" in load,
            "older save stacks of Linking Cord are not normalized")
    require("GetBagItemId(POCKET_ITEMS" in load and "AddBagItem(ITEM_LINKING_CORD, 1)" in load,
            "legacy Linking Cords are not moved out of their former Items pocket")

    print(f"PASS: all {len(trade_paths)} trade evolutions have single-player alternatives")
    print(f"PASS: all {len(solo_items)} required evolution items are obtainable")
    print("PASS: the Linking Cord is a reusable key item and paired trades retain their partner requirement")
    print("PASS: Milcery and both Gimmighoul forms evolve through deterministic Leveler-compatible thresholds")


if __name__ == "__main__":
    main()
