#!/usr/bin/env python3
"""Check native Mega Stones against structurally linked world reward sources.

This is source association, not first-access or native transaction proof."""
from __future__ import annotations

import json
import re
from collections import defaultdict
from pathlib import Path

from generate_emerald_champions_mega_archive import stones

ROOT = Path(__file__).resolve().parents[1]
LABEL = re.compile(r"(?m)^([A-Za-z_][A-Za-z_0-9]*)::?\s*$")


def world_reward_sources(root: Path = ROOT) -> dict[str, list[str]]:
    groups = json.loads((root / "data/maps/map_groups.json").read_text())
    maps = {name for group, names in groups.items() if group != "group_order" and "_Frlg" not in group for name in names}
    required = set(stones())
    rewards: dict[str, list[str]] = defaultdict(list)
    roots = set()
    for name in sorted(maps):
        data = json.loads((root / "data/maps" / name / "map.json").read_text())
        roots.add(name + "_MapScripts")
        for kind in ("coord_events", "bg_events"):
            roots.update(event["script"] for event in data.get(kind, []) if "script" in event)
        for obj in data["object_events"]:
            roots.add(obj["script"])
            item = str(obj["trainer_sight_or_berry_tree_id"])
            if item in required and obj["script"] == "Common_EventScript_FindItem":
                if obj["graphics_id"] != "OBJ_EVENT_GFX_MEGA_STONE" or str(obj["flag"]) == "0":
                    raise ValueError(f"{name}/{item}: stone needs its sparkle and persistent pickup flag")
                rewards[item].append(f"{name}: sparkle at ({obj['x']}, {obj['y']})")
    nodes, aliases = {}, {}
    paths = list((root / "data/scripts").glob("*.inc"))
    paths += [root / "data/maps" / name / "scripts.inc" for name in sorted(maps)]
    for path in paths:
        if not path.exists():
            continue
        source = re.sub(r"@[^\n]*", "", path.read_text())
        aliases.update(re.findall(r"(?m)^\s*\.set\s+(\w+),\s*(\w+)\s*$", source))
        labels = list(LABEL.finditer(source))
        for i, match in enumerate(labels):
            end = labels[i + 1].start() if i + 1 < len(labels) else len(source)
            body = source[match.end():end]
            next_label = labels[i + 1][1] if i + 1 < len(labels) else None
            lines = [line.strip() for line in body.splitlines() if line.strip()]
            # Script branches may intentionally fall through into the next label.
            falls_through = bool(lines) and not re.match(r"(?:end|return|goto\s|\.byte\s+0|\.string)\b", lines[-1])
            nodes[match[1]] = (body, path.parent.name, next_label if falls_through else None)
    visited, pending = set(), list(roots)
    trade_reachable = False
    special_sources = defaultdict(list)
    while pending:
        name = pending.pop()
        if name in visited:
            continue
        visited.add(name)
        if name in aliases:
            pending.append(aliases[name])
        if name not in nodes:
            continue
        body, location, fallthrough = nodes[name]
        for item in set(re.findall(r"\b(?:give(?:unique)?item|add(?:pc)?item)\s+(ITEM_\w+)", body)) & required:
            rewards[item].append(f"{location}: NPC/event gift ({name})")
        for special in re.findall(r"\bspecial\s+(\w+)", body):
            special_sources[special].append(location)
        trade_reachable |= bool(re.search(r"\bspecial\s+TradeEmeraldChampionsGardenBerries\b", body))
        pending.extend(token for token in re.findall(r"\b\w+\b", body) if token in nodes or token in aliases)
        if fallthrough:
            pending.append(fallthrough)
    trade_code = (root / "src/mega_stone_rewards.c").read_text()
    trades = re.findall(r"\{(ITEM_\w+),\s*(FLAG_EC_BERRY_TRADE_\w+),", trade_code)
    if len(trades) != 3 or not trade_reachable:
        raise ValueError("three berry exchanges must be reachable from a world NPC")
    for item, flag in trades:
        if rewards[item]:
            raise ValueError(f"{item}: another reward bypasses its berry exchange")
        rewards[item].append("Route123_BerryMastersHouse: one-time garden berry trade")
    # Follow reachable native transaction entrypoints as well as item macros.
    native_rewards = {
        "ClaimEmeraldChampionsSootMilestone": ("ITEM_HOUNDOOMINITE", "FLAG_ITEM_FIERY_PATH_HOUNDOOMINITE"),
        "TradeEmeraldChampionsShoalMaterials": ("ITEM_GLALITITE", "FLAG_ITEM_ABANDONED_SHIP_ROOMS_B1F_GLALITITE"),
    }
    native = (root / "src/field_specials.c").read_text()
    for special, (item, receipt) in native_rewards.items():
        body = re.search(r"void " + special + r"\(void\)\s*\{(.*?)^\}", native, re.S | re.M)
        if not body or receipt not in body[1] or "AddBagItem" not in body[1]:
            raise ValueError(f"{special}: native finite reward/receipt contract is missing")
        if item not in native:
            raise ValueError(f"{special}: native stone selection is missing")
        for location in special_sources[special]:
            rewards[item].append(f"{location}: native finite exchange ({special})")
    return dict(rewards)


def main() -> None:
    rewards = world_reward_sources()
    missing = set(stones()) - rewards.keys()
    if missing:
        raise SystemExit(f"Mega Stones missing world rewards: {sorted(missing)}")
    source = (ROOT / "src/field_specials.c").read_text()
    vendor = (ROOT / "data/scripts/emerald_champions.inc").read_text()
    all_scripts = "\n".join(path.read_text() for path in (ROOT / "data").rglob("*.inc"))
    if "void OpenEmeraldChampionsMegaStoneArchive(" in source or re.search(r"\bspecial\s+OpenEmeraldChampionsMegaStoneArchive\b", all_scripts):
        raise SystemExit("the free Mega Stone archive bypasses world rewards")
    if "special OpenEmeraldChampionsEvolutionItemArchive" not in vendor:
        raise SystemExit("the evolution-item archive must remain available")
    code = (ROOT / "src/mega_stone_rewards.c").read_text()
    from audit.map_dynamic_inventory import functions
    start, end = functions(code)["TradeEmeraldChampionsGardenBerries"]
    trade_body = code[start:end]
    if "GetHarvestedBerryCount" not in trade_body or "RemoveBagItem" in trade_body:
        raise SystemExit("harvest trades must spend per-type harvest, never ordinary bag berries")
    from item_catalog import free_vendor_items
    berries = set(re.findall(r"F\((\w+)\)", (ROOT / "include/constants/berries.h").read_text()))
    missing = {"ITEM_" + name + "_BERRY" for name in berries} - free_vendor_items(ROOT)
    if missing:
        raise SystemExit(f"berries missing from free catalogue: {sorted(missing)}")
    print(f"PASS: all {len(rewards)} Mega Stones have world rewards; three are exclusive berry trades")
    print("PASS: no free Mega archive or preset berry-currency bypass; evolution archive preserved")


if __name__ == "__main__":
    main()
