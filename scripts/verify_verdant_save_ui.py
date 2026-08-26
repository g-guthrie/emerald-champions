#!/usr/bin/env python3
"""Focused, source-level regression checks for Verdant save and native UI code."""

from pathlib import Path
import sys


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


save = read("src/save.c")
crt0 = read("src/crt0.s")
item = read("src/item.c")
item_menu = read("src/item_menu.c")
party = read("src/party_menu.c")
shop = read("src/shop.c")
strings = read("src/strings.c")
tutor = read("data/scripts/pokemon_center_move_tutor.inc")
daycare_script = read("data/scripts/day_care.inc")
daycare = read("src/daycare.c")
overworld = read("src/overworld.c")


checks = {
    "serialized save structures retain their compatibility sizes": all(
        token in save
        for token in (
            "sizeof(struct SaveBlock1) == 0x380C",
            "sizeof(struct SaveBlock2) == 0x0F2C",
            "sizeof(struct Pokedex) == 0x78",
        )
    ),
    "external ROM save-layout header matches serialized structures": all(
        token in crt0
        for token in (
            ".4byte 0x000018f8 @ offsetof(struct SaveBlock1, flags)",
            ".4byte 0x00001a24 @ offsetof(struct SaveBlock1, vars)",
            ".4byte 0x00003004 @ offsetof(struct SaveBlock1, dexSeen)",
            ".4byte VERDANT_LEGACY_NATIONAL_DEX_COUNT @ contiguous dexSeen bit count",
            ".4byte VAR_NATIONAL_DEX - VARS_START",
            ".4byte FLAG_RECEIVED_POKEDEX_FROM_BIRCH",
            ".4byte FLAG_SYS_MYSTERY_EVENT_ENABLE",
            ".4byte 0x0000380c @ sizeof(struct SaveBlock1)",
            ".4byte 0x00002c33 @ offsetof(struct SaveBlock1, externalEventFlags)",
            ".4byte 0x00002c1f @ offsetof(struct SaveBlock1, externalEventData)",
            ".4byte FLAG_SYS_GAME_CLEAR",
            ".4byte FLAG_SYS_RIBBON_GET",
            ".byte BAG_ITEMS_COUNT, BAG_KEYITEMS_COUNT, BAG_POKEBALLS_COUNT, BAG_TMHM_COUNT",
            ".byte BAG_BERRIES_COUNT, PC_ITEMS_COUNT",
            ".4byte 0x00002c14 @ offsetof(struct SaveBlock1, giftRibbons)",
            ".4byte 0x00002c64 @ offsetof(struct SaveBlock1, enigmaBerry)",
        )
    ),
    "external save-layout offsets are compile-time guarded": all(
        name in save
        for name in (
            "SaveBlock1FlagsOffsetMustRemainStable",
            "SaveBlock1VarsOffsetMustRemainStable",
            "SaveBlock1DexSeenOffsetMustRemainStable",
            "SaveBlock1ExternalFlagsOffsetMustRemainStable",
            "SaveBlock1ExternalDataOffsetMustRemainStable",
            "SaveBlock1GiftRibbonsOffsetMustRemainStable",
            "SaveBlock1EnigmaBerryOffsetMustRemainStable",
        )
    ),
    "legacy item migrations run on every valid Continue path": (
        "MigrateVerdantGen9WorldItems();" in overworld
        and overworld.index("MigrateVerdantGen9WorldItems();")
        < overworld.index("LoadSaveblockMapHeader();")
        and "gSaveFileStatus != SAVE_STATUS_OK && gSaveFileStatus != SAVE_STATUS_ERROR" in save
    ),
    "recovered backup saves receive the canonical ruleset": (
        "if (status == SAVE_STATUS_OK || status == SAVE_STATUS_ERROR)" in save
        and "gSaveBlock2Ptr->gameDifficulty = DIFFICULTY_CHALLENGE;" in save
        and "gSaveBlock2Ptr->levelCaps = LEVEL_CAPS_STRICT;" in save
        and "gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SET;" in save
    ),
    "corrupt save footers cannot index outside section metadata": (
        "if (id >= SECTOR_SAVE_SLOT_LENGTH)\n            continue;" in save
        and save.count("if (gFastSaveSection->id < SECTOR_SAVE_SLOT_LENGTH)") == 2
    ),
    "a save slot is valid only when every sector counter agrees": (
        "bool8 counterMismatch = FALSE;" in save
        and save.count("else if (saveSlot1Counter != gFastSaveSection->counter)") == 1
        and save.count("else if (saveSlot2Counter != gFastSaveSection->counter)") == 1
        and save.count("slotCheckField == 0x3FFF && !counterMismatch") == 2
    ),
    "multi-item rewards are bounded atomic and retryable": all(
        token in item
        for token in (
            "if (count > ARRAY_COUNT(added))",
            "if (!AddBagItem(itemIds[i], 1))",
            "if (added[i])\n                    RemoveBagItem(itemIds[i], 1);",
            "PlayerOwnsItemAnywhere(itemIds[i])",
        )
    ),
    "item ownership scans every persistent player location": all(
        token in item
        for token in (
            "CheckBagHasItem(itemId, 1) || CheckPCHasItem(itemId, 1)",
            "gSaveBlock1Ptr->daycare.mons[i].mon",
            "GetBoxMonDataAt(boxId, boxPosition, MON_DATA_HELD_ITEM)",
        )
    ),
    "bag and PC transactions fail safely if temporary allocation fails": (
        item.count("if (newItems == NULL)") >= 2
        and item.count("if (newItems == NULL || newQuantities == NULL)") >= 2
    ),
    "regular Marts append the core stock once": (
        "BuildPokemartItemsWithCoreStock(itemsForSale)" in shop
        and "alreadyStocked" in shop
        and "ITEM_RARE_CANDY" in shop.split("sCorePokemartStock[]", 1)[1].split("};", 1)[0]
    ),
    "shop item-name storage includes the EOS byte": (
        "u8 (*sItemNames)[ITEM_NAME_LENGTH]" in shop
        and "u8 (*sItemNames)[16]" not in shop
    ),
    "party actions preserve and scroll all possible choices": all(
        token in party
        for token in (
            "#define PARTY_MENU_MAX_ACTIONS               (MAX_MON_MOVES + 7)",
            "ListMenu_ProcessInput(sPartyMenuInternal->actionListTaskId)",
            "DestroyPartyActionList();",
            "sPartyMenuInternal->actions[sPartyMenuInternal->selectedActionIndex] - MENU_FIELD_MOVES",
        )
    ),
    "Bag icon transitions never index a missing sprite": (
        "HideBagItemIconSprite(gBagMenu->itemIconSlot ^ 1);" in item_menu
        and "gSprites[gBagMenu->spriteIds[ITEMMENUSPRITE_ITEM + (gBagMenu->itemIconSlot ^ 1)]]" not in item_menu
    ),
    "Pokemon Center exposes one badge-free all-legal teacher": (
        "case 0, PKMN_Center_Move_Tutor_MoveTutorIntro" in tutor
        and "goto_if_set FLAG_TALKED_TO_MOVE_TUTOR, PKMN_Center_MoveReminder_EventScriptChooseMon" in tutor
        and "Which Pokémon should I teach?" in tutor
        and "legal moves to learn" in tutor
        and "remember a move" not in tutor
    ),
    "Day Care presentation matches its fixed-fee no-level behavior": (
        "Pokémon don't gain levels here" in daycare_script
        and "Our fixed care fee" in daycare_script
        and "level = GetLevelFromBoxMonExp" in daycare
    ),
    "native save card uses the compact Emerald Champions label": (
        'gText_SavingVersionNum[] = _("E. Champs")' in strings
        and 'gText_SavingVersionNum[] = _("Ver 1.13")' not in strings
    ),
}


failed = [name for name, passed in checks.items() if not passed]
for name, passed in checks.items():
    print(f"{'PASS' if passed else 'FAIL'}: {name}")

print(f"\n{len(checks) - len(failed)}/{len(checks)} save/UI checks passed")
sys.exit(bool(failed))
