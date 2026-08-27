#!/usr/bin/env python3
"""Source-backed checks for Emerald Champions' reusable party Leveler."""

from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def check(name: str, condition: bool) -> None:
    if not condition:
        raise AssertionError(name)
    print(f"PASS: {name}")


items_h = read("include/constants/items.h")
items = read("src/data/items.h")
item_use = read("src/item_use.c")
party = read("src/party_menu.c")
nurse_script = read("data/scripts/pkmn_center_nurse.inc")
nurse_text = read("data/text/pkmn_center_nurse.inc")

check(
    "the Leveler occupies an existing reserved Key Item ID without shifting saves",
    "#define ITEM_LEVELER (LAST_TMHM_INDEX + 51)" in items_h,
)
check(
    "the Leveler is a reusable native Key Item",
    all(token in items for token in (
        "[ITEM_LEVELER] =",
        '.name = _("Leveler")',
        ".importance = 1",
        ".pocket = POCKET_KEY_ITEMS",
        ".type = ITEM_USE_FIELD",
        ".fieldUseFunc = ItemUseOutOfBattle_Leveler",
    )),
)
check(
    "new and existing Poké Vial owners are both granted the Leveler",
    nurse_script.count("giveitem ITEM_LEVELER, 1") == 2
    and "checkitem ITEM_LEVELER, 1" in nurse_script
    and nurse_script.count("goto_if_eq Common_EventScript_ShowBagIsFull") >= 3,
)
check(
    "the nurse explains the cap, whole-party behavior, and unlimited reuse",
    all(token in nurse_text for token in (
        "your party to the current level cap",
        "your whole party to the current cap",
        "It never runs out",
    )),
)
check(
    "Bag and registered-item entry paths are both supported",
    all(token in item_use for token in (
        "CB2_OpenLevelerFromBag",
        "Task_OpenRegisteredLeveler",
        "FieldCB_ReturnToFieldNoScript",
        "StartLevelerPartySequence(CB2_ReturnToBagMenuPocket)",
        "StartLevelerPartySequence(CB2_ReturnToField)",
    )),
)
check(
    "the Leveler selects party slots in order and skips Eggs and capped Pokémon",
    all(token in party for token in (
        "for (slot = sLevelerNextSlot; slot < gPlayerPartyCount; slot++)",
        "!GetMonData(&gPlayerParty[slot], MON_DATA_IS_EGG)",
        "GetMonData(&gPlayerParty[slot], MON_DATA_LEVEL) < levelCap",
        "sLevelerNextSlot = newSlot + 1",
    )),
)
check(
    "the Leveler reuses Rare Candy effects but targets the exact current cap",
    "isLeveler ? ITEM_RARE_CANDY : *itemPtr" in party
    and "if (isLeveler)\n            targetLevel = GetLevelCap();" in party,
)
check(
    "the Leveler is not consumed",
    "if (!isLeveler)\n            RemoveBagItem(gSpecialVar_ItemId, 1);" in party,
)
check(
    "crossed level-up moves and chained evolutions remain interactive",
    all(token in party for token in (
        "MonTryLearningNewMoveInRange",
        "gCB2_AfterEvolution = CB2_ContinueLevelerEvolution",
        "BeginEvolutionScene(mon, targetSpecies, TRUE, gPartyMenu.slotId)",
        "SetMainCallback2(CB2_ShowPartyMenuForLeveler)",
    )),
)

dialogue_lines = re.findall(r'\.string "([^"\\]*(?:\\.[^"\\]*)*)"', nurse_text)
visible_lines = [
    part
    for literal in dialogue_lines
    for part in re.split(r"\\[npl]", literal)
    if part and "{" not in part
]
check(
    "all Pokémon Center Leveler dialogue fits the conservative native line budget",
    max(map(len, visible_lines), default=0) <= 38,
)

print("Leveler verification: all checks passed")
