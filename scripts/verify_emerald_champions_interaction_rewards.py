#!/usr/bin/env python3
"""Interaction-reward gate for Emerald Champions.

Checks that every player-facing reward path still agrees with the campaign's
economy rules:

* no NPC gift or world pickup duplicates a held item the Pokémon Center vendor
  already gives away for free;
* every in-game trade delivers a competitive preset and a species the player
  cannot simply catch in the wild;
* the Champions Circuit funds the Battle Point exchange (the only repeatable
  BP source now that every Frontier desk leads to the Circuit);
* Rare Candy is never acquirable (Trainer Hill, Resort Gorgeous, marts, scripts);
* obsolete rewards (Exp. Share with no battle exp, medicine as facility prizes)
  are gone;
* the Catching and Oval Charms are awarded when the activity they support
  begins (Safari Zone / Day Care), not as postgame capstones.

Like every other gate this proves the data is self-consistent, not that the
game is fun.
"""
from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
FAILURES: list[str] = []


def read(relative: str) -> str:
    return (ROOT / relative).read_text()


def require(condition: bool, message: str) -> None:
    if condition:
        print(f"PASS: {message}")
    else:
        print(f"FAIL: {message}")
        FAILURES.append(message)


def c_array(source: str, label: str) -> list[str]:
    match = re.search(r"\b" + re.escape(label) + r"\[\]\s*=\s*\{(.*?)\};", source, re.S)
    if match is None:
        return []
    return re.findall(r"ITEM_[A-Z0-9_]+", match.group(1))


def free_vendor_items() -> set[str]:
    source = read("src/field_specials.c")
    items: set[str] = set()
    for label in (
        "sEmeraldChampionsFreeBattleItems",
        "sEmeraldChampionsOffenseItems",
        "sEmeraldChampionsDefenseItems",
        "sEmeraldChampionsFieldItems",
        "sEmeraldChampionsTypeItems",
        "sEmeraldChampionsGemItems",
        "sEmeraldChampionsSpeciesItems",
    ):
        items.update(c_array(source, label))
    items.discard("ITEM_NONE")
    return items


def map_scripts() -> dict[str, str]:
    return {
        str(path.relative_to(ROOT)): path.read_text()
        for path in sorted((ROOT / "data/maps").glob("*/scripts.inc"))
        if "_Frlg" not in path.parent.name
    }


def verify_no_free_vendor_duplicates() -> None:
    free = free_vendor_items()
    require(len(free) >= 100, f"free vendor lists parsed ({len(free)} items)")
    offenders = []
    for relative, text in map_scripts().items():
        for item in re.findall(r"^\s*giveitem\s+(ITEM_[A-Z0-9_]+)", text, re.M):
            if item in free:
                offenders.append(f"{relative}: {item}")
    for path in sorted((ROOT / "data/maps").glob("*/map.json")):
        if "_Frlg" in path.parent.name:
            continue
        data = json.loads(path.read_text())
        for event in data.get("object_events", []) + data.get("bg_events", []):
            item = event.get("trainer_sight_or_berry_tree_id") or event.get("item")
            if isinstance(item, str) and item in free:
                offenders.append(f"{path.relative_to(ROOT)}: {item}")
    require(not offenders, f"no NPC gift or pickup duplicates a free vendor held item {offenders[:8]}")


def verify_trades() -> None:
    trade_c = read("src/trade.c")
    trade_body = trade_c[trade_c.index("static void CreateInGameTradePokemonInternal"):]
    preset_pos = trade_body.index("ApplyEmeraldChampionsBattleSetChoice(pokemon, 0);")
    require(
        preset_pos > trade_body.rindex("SetMonData(pokemon, MON_DATA_ABILITY_NUM")
        and preset_pos > trade_body.rindex("SetMonData(pokemon, MON_DATA_HELD_ITEM"),
        "in-game trades apply the competitive preset after legacy Ability/item fields",
    )
    trades = read("src/data/trade.h")
    wild = set()
    for group in json.loads(read("src/data/wild_encounters.json"))["wild_encounter_groups"]:
        for encounter in group["encounters"]:
            for value in encounter.values():
                if isinstance(value, dict) and "mons" in value:
                    wild.update(mon["species"] for mon in value["mons"])
    hoenn_trades = ("FIDOUGH", "BOMBIRDIER", "CYCLIZAR", "TYPE_NULL", "TROPIUS", "HAPPINY")
    for key in hoenn_trades:
        block = re.search(r"\[INGAME_TRADE_" + key + r"\] =\s*\n    \{\n(.*?)\n    \}", trades, re.S)
        require(block is not None, f"trade table defines INGAME_TRADE_{key}")
        if block is None:
            continue
        body = block.group(1)
        species = re.search(r"\.species = (SPECIES_[A-Z0-9_]+)", body).group(1)
        requested = re.search(r"\.requestedSpecies = (SPECIES_[A-Z0-9_]+)", body).group(1)
        ivs = re.search(r"\.ivs = \{([^}]*)\}", body).group(1)
        require(all(int(v.strip()) == 31 for v in ivs.split(",")), f"{key}: perfect IVs")
        require(requested in wild, f"{key}: requested {requested} is catchable")
        if key != "TROPIUS":
            require(species not in wild, f"{key}: reward {species} is trade-exclusive")


def verify_circuit_battle_points() -> None:
    circuit = read("src/champions_circuit.c")
    require(
        "gSaveBlock2Ptr->frontier.battlePoints += points;" in circuit
        and "AwardCircuitBattlePoints(" in circuit,
        "Champions Circuit victories award Battle Points",
    )
    room = read("data/maps/BattleFrontier_BattleTowerBattleRoom/scripts.inc")
    require(
        "Text_CircuitBattlePoints" in room
        and room.index("Text_CircuitBattlePoints") > room.index("special ChampionsCircuitHandleBattleResult"),
        "Circuit room announces the BP award after each win",
    )
    exchange = read("data/maps/BattleFrontier_ExchangeServiceCorner/scripts.inc")
    require("setitemandprice" in exchange, "BP exchange still sells items")


def verify_no_rare_candy() -> None:
    for relative in ("src/trainer_hill.c", "src/battle_tent.c", "src/lottery_corner.c", "src/field_specials.c"):
        require("ITEM_RARE_CANDY" not in read(relative), f"{relative} awards no Rare Candy")
    offenders = [rel for rel, text in map_scripts().items() if "ITEM_RARE_CANDY" in text]
    require(not offenders, f"no map script gives or sells Rare Candy {offenders}")
    book = read("docs/EMERALD_CHAMPIONS_CAMPAIGN_BOOK.md")
    require("sell Rare Candies" not in book, "campaign book no longer claims marts sell Rare Candy")


def verify_obsolete_rewards() -> None:
    battle_config = read("include/config/battle.h")
    require(
        re.search(r"^#define\s+B_EC_BATTLE_EXP\s+FALSE\b", battle_config, re.M) is not None,
        "battle experience is disabled",
    )
    lottery = read("src/lottery_corner.c")
    require("ITEM_EXP_SHARE" not in lottery, "lottery no longer pays an Exp. Share")
    require("ITEM_MAX_REVIVE" not in lottery, "lottery no longer pays medicine")
    offenders = [rel for rel, text in map_scripts().items() if "giveitem ITEM_EXP_SHARE" in text]
    require(not offenders, f"no NPC gives an Exp. Share {offenders}")
    hill = read("src/trainer_hill.c")
    for medicine in ("ITEM_ETHER", "ITEM_MAX_POTION", "ITEM_REVIVE", "ITEM_FLUFFY_TAIL"):
        require(medicine not in hill, f"Trainer Hill prize tables contain no {medicine}")
    trick = read("data/maps/Route110_TrickHouseEnd/scripts.inc")
    require("ITEM_SMOKE_BALL" not in trick and "giveitem ITEM_ALAKAZITE" in trick,
            "Trick House rewards were upgraded (no Smoke Ball, Mega Stone finale)")
    flat = read("data/maps/RustboroCity_Flat2_2F/scripts.inc")
    require("ITEM_FLOAT_STONE" not in flat and "giveitem ITEM_METAL_COAT" in flat,
            "Rustboro flat NPC gives an evolution item instead of a free vendor item")


def verify_charm_timing() -> None:
    safari = read("data/maps/Route121_SafariZoneEntrance/scripts.inc")
    daycare = read("data/maps/Route117_PokemonDayCare/scripts.inc")
    require("giveitem ITEM_CATCHING_CHARM" in safari, "Catching Charm is awarded at the Safari Zone")
    require("giveitem ITEM_OVAL_CHARM" in daycare, "Oval Charm is awarded at the Day Care")
    daycare_after = daycare.split("Route117_PokemonDayCare_EventScript_TogepiEggAfter::", 1)[1].split("Route117_PokemonDayCare_EventScript_TogepiEggDeclined::", 1)[0]
    require("goto Route117_PokemonDayCare_EventScript_TryGiveOvalCharm" in daycare_after,
            "a full Bag cannot permanently lose the Day Care Oval Charm")
    for relative in ("data/maps/AlteringCave_B1F/scripts.inc", "data/maps/CaveOfOrigin_DianciesRoom/scripts.inc"):
        text = read(relative)
        require("_CHARM" not in text, f"{relative} no longer holds a charm hostage until the postgame")
    flags = read("include/constants/flags.h")
    for flag in ("FLAG_EC_RECEIVED_CATCHING_CHARM", "FLAG_EC_RECEIVED_OVAL_CHARM"):
        require(flag in flags, f"{flag} defined")


def main() -> int:
    verify_no_free_vendor_duplicates()
    verify_trades()
    verify_circuit_battle_points()
    verify_no_rare_candy()
    verify_obsolete_rewards()
    verify_charm_timing()
    if FAILURES:
        print(f"\n{len(FAILURES)} interaction-reward check(s) failed")
        return 1
    print("\nAll interaction-reward checks passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
