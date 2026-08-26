#!/usr/bin/env python3
"""Verify Emerald Champions' causal story and progression contracts."""

from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def label_block(path: str, label: str) -> str:
    text = read(path)
    match = re.search(
        rf"^{re.escape(label)}(?::|::).*?(?=^[A-Za-z0-9_]+(?::|::)|\Z)",
        text,
        re.M | re.S,
    )
    if not match:
        raise ValueError(f"missing {path}:{label}")
    return match.group(0)


def occurs_before(text: str, first: str, second: str) -> bool:
    return first in text and second in text and text.index(first) < text.index(second)


def gift_is_retry_safe(path: str, label: str, item: str, completion: str) -> bool:
    block = label_block(path, label)
    give = f"giveitem {item}"
    if give not in block or completion not in block:
        return False
    give_at = block.index(give)
    completion_at = block.index(completion, give_at)
    before_give = block[:give_at]
    before_completion = block[give_at:completion_at]
    preflight = (
        f"checkitemspace {item}" in before_give
        and "compare VAR_RESULT, FALSE" in before_give
        and "goto_if_eq" in before_give
    )
    result_guard = bool(re.search(
        r"compare\s+VAR_RESULT,\s*(?:FALSE|0).*?goto_if_eq\s+[^\n]+",
        before_completion,
        re.S,
    ))
    return preflight or result_guard


# Every entry advances a persistent flag/state, removes its only object, or
# returns to a caller that advances an irreversible story scene. Consumable Gym
# Guide supplies are deliberately excluded: they do not unlock progression and
# their NPC remains available when no completion flag is written.
PROGRESSION_GIFTS = (
    ("data/maps/RustboroCity_DevonCorp_3F/scripts.inc", "RustboroCity_DevonCorp_3F_EventScript_MeetPresident", "ITEM_LETTER", "setflag FLAG_SYS_POKENAV_GET"),
    ("data/maps/RusturfTunnel/scripts.inc", "RusturfTunnel_EventScript_Grunt", "ITEM_DEVON_GOODS", "clearflag FLAG_DEVON_GOODS_STOLEN"),
    ("data/maps/RustboroCity_CuttersHouse/scripts.inc", "RustboroCity_CuttersHouse_EventScript_Cutter", "ITEM_HM01_CUT", "setflag FLAG_RECEIVED_HM01"),
    ("data/maps/GraniteCave_1F/scripts.inc", "GraniteCave_1F_EventScript_Hiker", "ITEM_HM05_FLASH", "setflag FLAG_RECEIVED_HM05"),
    ("data/maps/MauvilleCity_House1/scripts.inc", "MauvilleCity_House1_EventScript_RockSmashDude", "ITEM_HM06_ROCK_SMASH", "setflag FLAG_RECEIVED_HM06"),
    ("data/maps/RusturfTunnel/scripts.inc", "RusturfTunnel_EventScript_ClearTunnelScene", "ITEM_HM04_STRENGTH", "setflag FLAG_RECEIVED_HM04"),
    ("data/maps/PetalburgCity_WallysHouse/scripts.inc", "PetalburgCity_WallysHouse_EventScript_GiveHM03Surf", "ITEM_HM03_SURF", "setflag FLAG_RECEIVED_HM03"),
    ("data/maps/Route119/scripts.inc", "Route119_EventScript_GiveFlyHM", "ITEM_HM02_FLY", "setflag FLAG_RECEIVED_HM02"),
    ("data/maps/MossdeepCity_StevensHouse/scripts.inc", "MossdeepCity_StevensHouse_EventScript_StevenGivesDive", "ITEM_HM08_DIVE", "setflag FLAG_RECEIVED_HM08"),
    ("data/maps/SootopolisCity/scripts.inc", "SootopolisCity_EventScript_GiveWaterfall", "ITEM_HM07_WATERFALL", "setflag FLAG_RECEIVED_HM07"),
    ("data/maps/LittlerootTown/scripts.inc", "LittlerootTown_EventScript_GiveRunningShoes", "ITEM_OLD_ROD", "setflag FLAG_RECEIVED_OLD_ROD"),
    ("data/maps/Route118/scripts.inc", "Route118_EventScript_ReceiveGoodRod", "ITEM_GOOD_ROD", "setflag FLAG_RECEIVED_GOOD_ROD"),
    ("data/maps/MossdeepCity_House3/scripts.inc", "MossdeepCity_House3_EventScript_SuperRodFisherman", "ITEM_SUPER_ROD", "setflag FLAG_RECEIVED_SUPER_ROD"),
    ("data/maps/Route110/scripts.inc", "Route110_EventScript_GiveItemfinder", "ITEM_ITEMFINDER", "return"),
    ("data/maps/Route104_PrettyPetalFlowerShop/scripts.inc", "Route104_PrettyPetalFlowerShop_EventScript_GiveWailmerPail", "ITEM_WAILMER_PAIL", "setflag FLAG_RECEIVED_WAILMER_PAIL"),
    ("data/maps/Route113_GlassWorkshop/scripts.inc", "Route113_GlassWorkshop_EventScript_GlassWorker", "ITEM_SOOT_SACK", "setvar VAR_GLASS_WORKSHOP_STATE, 1"),
    ("data/scripts/contest_hall.inc", "LilycoveCity_ContestLobby_EventScript_GivePokeblockCase", "ITEM_POKEBLOCK_CASE", "setflag FLAG_RECEIVED_POKEBLOCK_CASE"),
    ("data/maps/SlateportCity/scripts.inc", "SlateportCity_EventScript_BerryPowderClerk", "ITEM_POWDER_JAR", "setflag FLAG_RECEIVED_POWDER_JAR"),
    ("data/scripts/pkmn_center_nurse.inc", "Common_EventScript_PkmnCenterNurse", "ITEM_POKE_VIAL", "setvar VAR_POKE_VIAL_MAX_CHARGES, 1"),
    ("data/maps/MauvilleCity/scripts.inc", "MauvilleCity_EventScript_Wattson", "ITEM_BASEMENT_KEY", "setflag FLAG_GOT_BASEMENT_KEY_FROM_WATTSON"),
    ("data/maps/MtPyre_Summit/scripts.inc", "MtPyre_Summit_EventScript_TeamAquaExits", "ITEM_MAGMA_EMBLEM", "setflag FLAG_RECEIVED_RED_OR_BLUE_ORB"),
    ("data/maps/Route120/scripts.inc", "Route120_EventScript_StevenGiveDeconScope", "ITEM_DEVON_SCOPE", "setflag FLAG_RECEIVED_DEVON_SCOPE"),
    ("data/maps/MtChimney/scripts.inc", "MtChimney_EventScript_MeteoriteMachine", "ITEM_METEORITE", "setflag FLAG_RECEIVED_METEORITE"),
    ("data/maps/DesertUnderpass/scripts.inc", "DesertUnderpass_EventScript_GiveClawFossil", "ITEM_CLAW_FOSSIL", "removeobject LOCALID_FOSSIL"),
    ("data/maps/DesertUnderpass/scripts.inc", "DesertUnderpass_EventScript_GiveRootFossil", "ITEM_ROOT_FOSSIL", "removeobject LOCALID_FOSSIL"),
    ("data/scripts/players_house.inc", "PlayersHouse_1F_EventScript_GetSSTicketAndSeeLatiTV", "ITEM_SS_TICKET", "setflag FLAG_RECEIVED_SS_TICKET"),
    ("data/maps/Route103/scripts.inc", "Route103_EventScript_MayGiveEonTicket", "ITEM_EON_TICKET", "setflag FLAG_ENABLE_SHIP_SOUTHERN_ISLAND"),
    ("data/maps/Route103/scripts.inc", "Route103_EventScript_BrendanGiveEonTicket", "ITEM_EON_TICKET", "setflag FLAG_ENABLE_SHIP_SOUTHERN_ISLAND"),
    ("data/maps/MossdeepCity_House1/scripts.inc", "MossdeepCity_House1_EventScript_GiveMysticTicket", "ITEM_MYSTIC_TICKET", "setflag FLAG_ENABLE_SHIP_NAVEL_ROCK"),
    ("data/maps/MeteorFalls_StevensCave/scripts.inc", "MeteorFalls_StevensCave_EventScript_GiveAuroraTicket", "ITEM_AURORA_TICKET", "setflag FLAG_RECEIVED_AURORA_TICKET"),
    ("data/maps/AbandonedShip_HiddenFloorRooms/scripts.inc", "AbandonedShip_HiddenFloorRooms_EventScript_OldSeaMap", "ITEM_OLD_SEA_MAP", "removeobject LOCALID_OLD_SEA_MAP"),
)


def audit() -> list[str]:
    problems: list[str] = []

    # The first three badge bosses are authored as a fixed escalating sequence,
    # and Brawly owns the first opposing Mega.
    brawly = label_block("data/maps/DewfordTown_Gym/scripts.inc", "DewfordTown_Gym_EventScript_Brawly")
    if not occurs_before(brawly, "goto_if_unset FLAG_SYS_RECEIVED_KEYSTONE", "trainerbattle_double TRAINER_BRAWLY_1"):
        problems.append("Brawly is not gated behind the player's Mega Bracelet")
    wattson = label_block("data/maps/MauvilleCity_Gym/scripts.inc", "MauvilleCity_Gym_EventScript_Wattson")
    if not occurs_before(wattson, "goto_if_unset FLAG_BADGE02_GET", "trainerbattle_double TRAINER_WATTSON_1"):
        problems.append("Wattson is not gated behind the Knuckle Badge")
    tate_liza = label_block("data/maps/MossdeepCity_Gym/scripts.inc", "MossdeepCity_Gym_EventScript_TateAndLiza")
    if not occurs_before(tate_liza, "goto_if_unset FLAG_BADGE06_GET", "trainerbattle_double TRAINER_TATE_AND_LIZA_1"):
        problems.append("Tate and Liza are not gated behind the Feather Badge")
    if not occurs_before(tate_liza, "goto_if_unset FLAG_BADGE06_GET", "setflag FLAG_SYS_INVERSE_BATTLE"):
        problems.append("the blocked Tate and Liza path leaks inverse-battle state")

    lilycove = label_block("data/maps/LilycoveCity_Harbor/scripts.inc", "LilycoveCity_Harbor_EventScript_FerryRegularLocationSelect")
    slateport = label_block("data/maps/SlateportCity_Harbor/scripts.inc", "SlateportCity_Harbor_EventScript_ChooseDestinationWithBattleFrontier")
    if "case 1, LilycoveCity_Harbor_EventScript_GoToBattleFrontier" not in lilycove:
        problems.append("Lilycove still routes the Battle Frontier choice to a placeholder")
    if "case 1, SlateportCity_Harbor_EventScript_BattleFrontier" not in slateport:
        problems.append("Slateport still routes the Battle Frontier choice to a placeholder")

    rival_end = label_block("data/maps/Route103/scripts.inc", "Route103_EventScript_RivalEnd")
    if "setflag FLAG_HIDE_ROUTE_103_RIVAL" not in rival_end:
        problems.append("the first Route 103 rival is not persistently hidden")
    hall_of_fame = label_block("data/scripts/hall_of_fame.inc", "EverGrandeCity_HallOfFame_EventScript_SetGameClearFlags")
    if "clearflag FLAG_HIDE_ROUTE_103_RIVAL" not in hall_of_fame:
        problems.append("the Hall of Fame does not restore the Route 103 postgame rival")

    for path, label, item, completion in PROGRESSION_GIFTS:
        try:
            safe = gift_is_retry_safe(path, label, item, completion)
        except ValueError as error:
            problems.append(str(error))
            continue
        if not safe:
            problems.append(f"{path}:{label} advances after an unguarded {item} gift")

    # The Coin Case trade must preflight before consuming its Ice Stone payment.
    coin_case = label_block("data/maps/MauvilleCity_House2/scripts.inc", "MauvilleCity_House2_EventScript_AcceptTrade")
    if not occurs_before(coin_case, "checkitemspace ITEM_COIN_CASE", "removeitem ITEM_ICE_STONE"):
        problems.append("Coin Case trade can consume the Ice Stone before confirming Bag space")

    guide_source = read("scripts/verdant_battle_guide.py")
    for token in (
        '("MeteorFalls_StevensCave", "TRAINER_STEVEN")',
        '("MossdeepCity_House1", "TRAINER_CYNTHIA_1")',
        'if map_name == "AshenWoods"',
    ):
        if token not in guide_source:
            problems.append(f"battle-guide chronology override missing: {token}")

    guide = json.loads(read("docs/verdant_battle_guide.json"))
    by_trainer = {entry["trainerId"]: entry for entry in guide["entries"]}
    for trainer in ("TRAINER_STEVEN", "TRAINER_CYNTHIA_1"):
        entry = by_trainer.get(trainer)
        if not entry or entry["chapter"] != "Postgame" or entry["levelCap"] != 100:
            problems.append(f"generated guide still misorders {trainer}")
    ashen = [entry for entry in guide["entries"] if entry["sourceMap"] == "AshenWoods"]
    if not ashen or any(entry["chapter"] != "Balance Badge" or entry["levelCap"] != 45 for entry in ashen):
        problems.append("generated guide still misorders Ashen Woods")

    return problems


def main() -> None:
    problems = audit()
    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))
    print(
        f"PASS: story gates, Frontier access, Route 103 lifecycle, guide chronology, "
        f"and {len(PROGRESSION_GIFTS)} progression gifts are source-closed"
    )


if __name__ == "__main__":
    main()
