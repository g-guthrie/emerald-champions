#!/usr/bin/env python3
"""Check native Mega Stones against structurally linked world reward sources.

Starter Mega Stones are dynamic: src/mega_stone_rewards.c (sStarterMegaStones)
gives each one a single receipt flag shared by Norman's Mega Ring gift, its world
home (a sparkle's object flag or a Gym Leader's receipt) and, for the Hoenn
stones, Norman's shown-partner gift. Sources that close the same receipt count
as one finite source; this script proves every home really closes on that flag.

This is source association, not first-access or native transaction proof;
test/mega_stone_rewards.c runs the rule natively."""
from __future__ import annotations

import json
import re
from collections import defaultdict
from pathlib import Path

from generate_emerald_champions_mega_archive import stones

ROOT = Path(__file__).resolve().parents[1]
LABEL = re.compile(r"(?m)^([A-Za-z_][A-Za-z_0-9]*)::?\s*$")


STARTER_TABLE = re.compile(r"\{(SPECIES_\w+),\s*(ITEM_\w+),\s*(FLAG_\w+)\}")
RECEIPT = re.compile(r"\[receipt (FLAG_\w+)\]$")
NORMAN_NATIVES = ("BufferNormanMegaGiftKind", "BufferNormanStarterMegaStone", "MarkStarterMegaStoneReceived")
PARTNER_NATIVES = ("BufferNormanPartnerMegaStone", "MarkStarterMegaStoneReceived")


def starter_mega_stones(root: Path = ROOT) -> list[tuple[str, str, str]]:
    """The native table: (final species, stone, the stone's single receipt flag)."""
    code = (root / "src/mega_stone_rewards.c").read_text()
    table = re.search(r"sStarterMegaStones\[\]\s*=\s*\{(.*?)^\};", code, re.S | re.M)
    if not table:
        raise ValueError("src/mega_stone_rewards.c: sStarterMegaStones table is missing")
    return STARTER_TABLE.findall(table[1])


def expected_starter_mega_stones(root: Path = ROOT) -> set[tuple[str, str]]:
    """Every (final starter form, Mega Stone) pair the game defines."""
    starter = (root / "src/starter_choose.c").read_text()
    body = re.search(r"GetFinalEvolutionForStarter\(enum Species species\)\s*\{(.*?)^\}", starter, re.S | re.M)
    finals = set(re.findall(r"return (SPECIES_\w+);", body[1])) - {"SPECIES_NONE"}
    forms = (root / "src/data/pokemon/form_change_tables.h").read_text()
    pairs = set()
    for form, item in re.findall(r"FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM,\s*(SPECIES_\w+?)_MEGA(?:_[XYZ])?,\s*(ITEM_\w+)", forms):
        if form in finals:
            pairs.add((form, item))
    return pairs


def world_reward_sources(root: Path = ROOT) -> dict[str, list[str]]:
    groups = json.loads((root / "data/maps/map_groups.json").read_text())
    maps = {name for group, names in groups.items() if group != "group_order" and "_Frlg" not in group for name in names}
    required = set(stones())
    rewards: dict[str, list[str]] = defaultdict(list)
    object_flags: dict[str, set[str]] = defaultdict(set)
    give_labels: dict[str, list[tuple[str, str]]] = defaultdict(list)
    roots = set()
    for name in sorted(maps):
        data = json.loads((root / "data/maps" / name / "map.json").read_text())
        roots.add(data.get("shared_scripts_map", name) + "_MapScripts")
        for kind in ("coord_events", "bg_events"):
            roots.update(event["script"] for event in data.get(kind, []) if "script" in event)
        for obj in data.get("object_events", []):
            roots.add(obj["script"])
            object_flags[obj["script"]].add(str(obj["flag"]))
            item = str(obj["trainer_sight_or_berry_tree_id"])
            if item in required and obj["script"] == "Common_EventScript_FindItem":
                if obj["graphics_id"] != "OBJ_EVENT_GFX_MEGA_STONE" or str(obj["flag"]) == "0":
                    raise ValueError(f"{name}/{item}: stone needs its sparkle and persistent pickup flag")
                rewards[item].append(f"{name}: sparkle at ({obj['x']}, {obj['y']})")
    nodes, aliases = {}, {}
    paths = [root / "data/event_scripts.s", *sorted((root / "data/scripts").glob("*.inc"))]
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
            terminals = {"end", "return", "goto", "gotostd", "gotoram", "endram", "returnram", "step_end", "pokemartlistend"}
            falls_through = not lines or (not lines[-1].startswith(".") and lines[-1].split()[0] not in terminals)
            nodes[match[1]] = (body, path.parent.name, next_label if falls_through else None)
    visited, pending = set(), list(roots)
    receipts = set()
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
        for shop in re.findall(r"\bpokemart(?:buy)?\s+(\w+)", body):
            stock = nodes.get(shop, ("", "", None))[0]
            sold_stones = set(re.findall(r"\bITEM_\w+", stock)) & required
            if sold_stones:
                raise ValueError(f"{location}/{shop}: Mega Stones cannot be shop stock: {sorted(sold_stones)}")
        # Bag/PC delivery alternatives within one transaction are one source.
        for item in set(re.findall(r"\b(?:give(?:unique)?item|finditem|additem|addpcitem)\s+(ITEM_\w+)", body)) & required:
            give_labels[item].append((location, name))
            # First delivery and a Bag-full retry are one reward when both
            # close the same persistent receipt (the Gym reward scripts).
            flags = re.findall(r"\bsetflag\s+(FLAG_RECEIVED_\w+)", body)
            receipt = (location, item, tuple(flags)) if flags else (location, item, name)
            if receipt not in receipts:
                receipts.add(receipt)
                rewards[item].append(f"{location}: NPC/event gift ({name})")
        for special in re.findall(r"\b(?:special|callnative)\s+(\w+)", body):
            special_sources[special].append(location)
        trade_reachable |= bool(re.search(r"\bspecial\s+TradeEmeraldChampionsGardenBerries\b", body))
        pending.extend(token for token in re.findall(r"\b\w+\b", body) if token in nodes or token in aliases)
        if fallthrough:
            pending.append(fallthrough)
    trade_code = (root / "src/mega_stone_rewards.c").read_text()
    trades = re.findall(r"\{(ITEM_\w+),\s*(FLAG_EC_BERRY_TRADE_\w+),", trade_code)
    if trade_reachable:
        for item, flag in trades:
            rewards[item].append("Route123_BerryMastersHouse: one-time garden berry trade")
    # Follow reachable native transaction entrypoints as well as item macros.
    native_rewards = {
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
    link_starter_mega_stones(root, rewards, nodes, object_flags, give_labels, special_sources)
    return dict(rewards)


def link_starter_mega_stones(root, rewards, nodes, object_flags, give_labels, special_sources) -> None:
    """Tag each starter stone's home with its receipt and add Norman's dynamic gifts."""
    table = starter_mega_stones(root)
    listed = {(species, item) for species, item, _ in table}
    expected = expected_starter_mega_stones(root)
    if listed != expected:
        raise ValueError(f"sStarterMegaStones must list exactly the starter Mega Stones: "
                         f"missing {sorted(expected - listed)}, extra {sorted(listed - expected)}")
    for native in set(NORMAN_NATIVES + PARTNER_NATIVES):
        locations = special_sources.get(native)
        if not locations or set(locations) != {"PetalburgCity_Gym"}:
            raise ValueError(f"{native}: Norman's starter stone gift must be reachable only from PetalburgCity_Gym")
    code = (root / "src/mega_stone_rewards.c").read_text()
    mark = re.search(r"void MarkStarterMegaStoneReceived\(void\)\s*\{(.*?)^\}", code, re.S | re.M)
    if not mark or "FlagSet(sStarterMegaStones[i].flag)" not in mark[1]:
        raise ValueError("MarkStarterMegaStoneReceived must set the stone's table receipt")
    norman = nodes.get("PetalburgCity_Gym_EventScript_NormanNextStarterStone", ("",))[0]
    partner = nodes.get("PetalburgCity_Gym_EventScript_NormanPartnerStones", ("",))[0]
    for body, label in ((norman, "NormanNextStarterStone"), (partner, "NormanPartnerStones")):
        # The receipt is written only after the item popup succeeds.
        if not re.search(r"giveitem VAR_0x8004\s+goto_if_eq VAR_RESULT, FALSE, \w+\s+callnative MarkStarterMegaStoneReceived", body):
            raise ValueError(f"{label}: stone must be delivered before its receipt is set")
    for species, item, flag in table:
        homes = [source for source in rewards.get(item, []) if not RECEIPT.search(source)]
        hoenn = species in ("SPECIES_SCEPTILE", "SPECIES_BLAZIKEN", "SPECIES_SWAMPERT")
        if hoenn != (not homes):
            raise ValueError(f"{item}: Hoenn stones have no world home; every other starter stone has one ({homes})")
        for location, label in give_labels.get(item, []):
            if label in object_flags:
                if object_flags[label] != {flag}:
                    raise ValueError(f"{item}: sparkle {label} uses {sorted(object_flags[label])}, not receipt {flag}")
                continue
            body = nodes[label][0]
            if not re.search(rf"\bsetflag {flag}\b", body):
                raise ValueError(f"{location}/{label}: gives {item} without setting its receipt {flag}")
            lines = [line.strip() for line in body.splitlines() if line.strip()]
            guarded_first = lines and re.fullmatch(rf"goto_if_set {flag}, \w+", lines[0])
            source = (root / "data/maps" / location / "scripts.inc")
            text = re.sub(r"@[^\n]*", "", source.read_text()) if source.exists() else ""
            uses = [m for m in re.findall(rf"^\s*(\w+)\s+(?:(FLAG_\w+),\s*)?{label}\s*$", text, re.M)]
            guarded_uses = uses and all(op in ("goto_if_unset", "call_if_unset") and used == flag for op, used in uses)
            if not (guarded_first or guarded_uses):
                raise ValueError(f"{location}/{label}: {item} must be guarded by its receipt {flag}")
        rewards[item] = [f"{source} [receipt {flag}]" for source in homes]
        rewards[item].append(f"PetalburgCity_Gym: Norman's Mega Ring gift for the starter pair [receipt {flag}]")
        if hoenn:
            rewards[item].append(f"PetalburgCity_Gym: Norman, shown a partner of this line [receipt {flag}]")


def duplicate_reward_sources(rewards: dict[str, list[str]]) -> dict[str, list[str]]:
    # A one-time Berry Master exchange may give an extra copy of a stone found
    # at an existing Inclement pickup. Keep the pickup unique and the exchange
    # one-time; two placed gifts or two exchange grants remain errors.
    # Sources closing one shared receipt (starter stones) are one finite source.
    duplicates = {}
    for item, sources in rewards.items():
        trades = [source for source in sources if source.endswith("one-time garden berry trade")]
        pickups = [source for source in sources if source not in trades]
        receipts = {RECEIPT.search(source)[1] for source in pickups if RECEIPT.search(source)}
        if len(receipts) > 1:
            duplicates[item] = sources
            continue
        pickups = [source for source in pickups if not RECEIPT.search(source)] + sorted(receipts)
        if len(pickups) != 1 or len(trades) > 1:
            duplicates[item] = sources
    return duplicates


def main() -> None:
    rewards = world_reward_sources()
    missing = set(stones()) - rewards.keys()
    if missing:
        raise SystemExit(f"Mega Stones missing world rewards: {sorted(missing)}")
    duplicates = duplicate_reward_sources(rewards)
    if duplicates:
        raise SystemExit(f"Mega Stones need one pickup and at most one Berry Master trade: {duplicates}")
    source = (ROOT / "src/field_specials.c").read_text()
    vendor = (ROOT / "data/scripts/emerald_champions.inc").read_text()
    all_scripts = "\n".join(path.read_text() for path in (ROOT / "data").rglob("*.inc"))
    if "void OpenEmeraldChampionsMegaStoneArchive(" in source or re.search(r"\bspecial\s+OpenEmeraldChampionsMegaStoneArchive\b", all_scripts):
        raise SystemExit("the free Mega Stone archive bypasses world rewards")
    if "GiveEmeraldChampionsStarterMegaStoneAtIndex" in source or "GiveEmeraldChampionsStarterMegaStoneAtIndex" in all_scripts:
        raise SystemExit("starter choices must not create extra Mega Stone grants")
    if "special OpenEmeraldChampionsEvolutionItemArchive" not in vendor:
        raise SystemExit("the evolution-item archive must remain available")
    from item_catalog import free_vendor_items
    catalogue_stones = set(stones()) & free_vendor_items(ROOT)
    catalogue_stones |= set(stones()) & set(re.findall(r"\bITEM_\w+", (ROOT / "src/data/emerald_champions_form_items.h").read_text()))
    if catalogue_stones:
        raise SystemExit(f"Mega Stones cannot be free catalogue stock: {sorted(catalogue_stones)}")
    print(f"PASS: all {len(rewards)} Mega Stones have one finite pickup and at most one one-time Berry Master trade")
    print(f"PASS: {len(starter_mega_stones())} starter Mega Stones share one receipt between Norman and their home")
    print("PASS: no free Mega archive or catalogue stock")



if __name__ == "__main__":
    main()
