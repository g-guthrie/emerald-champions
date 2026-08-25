#!/usr/bin/env python3
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_doubles_conversion.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_underused_balance.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_encounter_upgrade.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verify_competitive_references.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_custom_teams.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_team_polish.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_team_quality_audit.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_battle_guide.py"), "--check"],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_bespoke_battle_audit.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_evolution_stage_audit.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_ai_audit.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_trainer_dialogue_audit.py")],
    cwd=ROOT,
    check=True,
)
subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_logical_audit.py")],
    cwd=ROOT,
    check=True,
)


def read(path: str) -> str:
    return (ROOT / path).read_text()


champions_megas = {
    "MEGANIUM_MEGA": ("meganium", "meganiumite"),
    "FERALIGATR_MEGA": ("feraligatr", "feraligite"),
    "EMBOAR_MEGA": ("emboar", "emboarite"),
    "RAICHU_MEGA_X": ("raichu_x", "raichunite_x"),
    "RAICHU_MEGA_Y": ("raichu_y", "raichunite_y"),
    "DRAGONITE_MEGA": ("dragonite", "dragoninite"),
    "EXCADRILL_MEGA": ("excadrill", "excadrite"),
    "MALAMAR_MEGA": ("malamar", "malamarite"),
    "CHANDELURE_MEGA": ("chandelure", "chandelurite"),
    "HAWLUCHA_MEGA": ("hawlucha", "hawluchanite"),
    "GRENINJA_MEGA": ("greninja", "greninjite"),
}

world_mega_stones = {
    "PetalburgWoods_3_TM86_GrassKnot": "ITEM_MEGANIUMITE",
    "SeafloorCavern_Room9_EventScript_ItemTM26": "ITEM_FERALIGITE",
    "FieryPath_EventScript_ItemTM06": "ITEM_EMBOARITE",
    "NewMauville_Inside_EventScript_ItemTM91FlashCannon": "ITEM_RAICHUNITE_X",
    "Route103_EventScript_ItemTM40AerialAce": "ITEM_RAICHUNITE_Y",
    "MeteorFalls_1F_1R_EventScript_ItemTM59DragonPulse": "ITEM_DRAGONINITE",
    "Sandstrewn_Ruins_ItemLeechLife": "ITEM_EXCADRITE",
    "DewfordMeadow_EventScript_TM95Snarl": "ITEM_MALAMARITE",
    "MtPyre_Summit_EventScript_ItemTM61_WillOWisp": "ITEM_CHANDELURITE",
    "Route119_EventScript_ItemTM62_Acrobatics": "ITEM_HAWLUCHANITE",
    "Seaspray_Cave_Water_Pulse": "ITEM_GRENINJITE",
}

trainer_mega_showcases = {
    "sParty_Rose3": ("SPECIES_MEGANIUM", "ITEM_MEGANIUMITE"),
    "sParty_MattMtPyre": ("SPECIES_FERALIGATR", "ITEM_FERALIGITE"),
    "sParty_Flannery1": ("SPECIES_EMBOAR", "ITEM_EMBOARITE"),
    "sParty_Isabel3": ("SPECIES_RAICHU", "ITEM_RAICHUNITE_X"),
    "sParty_Wattson1": ("SPECIES_RAICHU", "ITEM_RAICHUNITE_Y"),
    "sParty_Lydia3": ("SPECIES_DRAGONITE", "ITEM_DRAGONINITE"),
    "sParty_TabithaMagmaHideout": ("SPECIES_EXCADRILL", "ITEM_EXCADRITE"),
    "sParty_Archie1": ("SPECIES_MALAMAR", "ITEM_MALAMARITE"),
    "sParty_Isaac3": ("SPECIES_CHANDELURE", "ITEM_CHANDELURITE"),
    "sParty_Maria3": ("SPECIES_HAWLUCHA", "ITEM_HAWLUCHANITE"),
    "sParty_Lao3": ("SPECIES_GRENINJA", "ITEM_GRENINJITE"),
}

mega_family_bases = (
    "SPECIES_CHIKORITA",
    "SPECIES_TOTODILE",
    "SPECIES_TEPIG",
    "SPECIES_PIKACHU",
    "SPECIES_DRATINI",
    "SPECIES_DRILBUR",
    "SPECIES_INKAY",
    "SPECIES_LITWICK",
    "SPECIES_HAWLUCHA",
    "SPECIES_FROAKIE",
)

starter_battle_items = (
    "ITEM_LIFE_ORB",
    "ITEM_CHOICE_BAND",
    "ITEM_CHOICE_SPECS",
    "ITEM_CHOICE_SCARF",
    "ITEM_FOCUS_SASH",
    "ITEM_ASSAULT_VEST",
    "ITEM_EVIOLITE",
    "ITEM_LEFTOVERS",
    "ITEM_ROCKY_HELMET",
    "ITEM_HEAVY_DUTY_BOOTS",
)


def array_body(path: str, name: str) -> str:
    return read(path).split(f"{name}[] = {{", 1)[1].split("};", 1)[0]


wild_encounters = json.loads(read("src/data/wild_encounters.json"))["wild_encounter_groups"][0]
wild_slot_counts = {
    field["type"]: len(field["encounter_rates"])
    for field in wild_encounters["fields"]
}
wild_table_lengths_match = all(
    len(encounter[field_name]["mons"]) == slot_count
    for encounter in wild_encounters["encounters"]
    for field_name, slot_count in wild_slot_counts.items()
    if field_name in encounter
)
start_menu_source = read("src/start_menu.c")
normal_start_menu = start_menu_source.split("static void BuildNormalStartMenu(void)", 2)[2].split(
    "static void BuildSafariZoneStartMenu(void)", 1
)[0]


checks = {
    "AI scores imminent Mega Evolutions as their transformed forms": (
        "TrySimulateMegaEvolutionForAI(&savedBattleMon)" in read("src/battle_controller_opponent.c")
        and "SetMonData(&simulatedMon, MON_DATA_SPECIES, &megaSpecies)" in read("src/battle_controller_opponent.c")
        and "CalculateMonStats(&simulatedMon)" in read("src/battle_controller_opponent.c")
        and "gBattleMons[gActiveBattler].ability = GetMonAbility(&simulatedMon)" in read("src/battle_controller_opponent.c")
        and "gBattleMons[gActiveBattler] = savedBattleMon" in read("src/battle_controller_opponent.c")
    ),
    "AI predicts move types with initialized dual-type matchups": (
        "bestTypeDmg = GetTypeMatchup" in read("src/battle_ai_switch_items.c")
        and "typeDmg1 *= GetTypeModifier" not in read("src/battle_ai_switch_items.c")
    ),
    "AI uses integrated switch-in ranking": (
        "GetBestMonForSwitch" in read("src/battle_ai_switch_items.c")
        and "GetBestMonDefensive" not in read("src/battle_ai_switch_items.c")
        and "GetBestMonOffensive" not in read("src/battle_ai_switch_items.c")
    ),
    "AI rejects lethal switch-in hazards": (
        "AI_CalcPartyMonHazardDamage" in read("src/battle_ai_switch_items.c")
        and "if (switchBattler >= PARTY_SIZE)" in read("src/battle_ai_util.c")
    ),
    "AI move scoring predicates repaired": (
        "!gBattleMons[battlerDef].status2 &" not in read("src/battle_ai_main.c")
        and "gLastMoves[battlerDef] != 0xFFFF" in read("src/battle_ai_main.c")
        and "AI_DATA->atkAbility == ABILITY_COMATOSE" in read("src/battle_ai_main.c")
    ),
    "AI forced tactical switches are deterministic": (
        "Random() % 3 < 2" not in read("src/battle_ai_switch_items.c")
        and "absorbingTypeAbilities[j] == monAbility && Random()" not in read("src/battle_ai_switch_items.c")
    ),
    "daycare nature parent guard": "if (parent < 0)" in read("src/daycare.c"),
    "daycare ability counter initialized": "u8 femaleCount = 0;" in read("src/daycare.c"),
    "egg moves no longer require shared egg groups": "DoMonsShareEggGroup" not in read("src/daycare.c"),
    "cap EV gains recalculate stats": read("src/battle_script_commands.c").count("CalculateMonStats(&gPlayerParty[gBattleStruct->expGetterMonId])") >= 2,
    "EV service charges actual gain": "gSpecialVar_0x8009 = actualIncrement * 100" in read("src/field_specials.c"),
    "rare candy handles chained evolutions": "CB2_ContinueRareCandyEvolution" in read("src/party_menu.c"),
    "Battle Style selector removed": "MENUITEM_BATTLE_STYLE" not in read("src/option_menu.c"),
    "normal Birch intro has no hack-author interstitial": (
        "gText_Pie_" not in read("src/main_menu.c")
        and "Buffel Saft" not in read("data/text/birch_speech.inc")
        and "gen eight will be added soon" not in read("data/text/birch_speech.inc")
        and "gSaveBlock2Ptr->gameDifficulty = DIFFICULTY_CHALLENGE;" in read("src/main_menu.c")
        and "gSaveBlock2Ptr->levelCaps = LEVEL_CAPS_STRICT;" in read("src/main_menu.c")
    ),
    "Pokécenter teacher offers every legal move without badge gates": (
        "goto PKMN_Center_MoveReminder_EventScriptChooseMon" in read("data/scripts/pokemon_center_move_tutor.inc")
        and "special GiveAllTMs" not in read("data/scripts/pokemon_center_move_tutor.inc").split("PKMN_Center_Move_Tutor_MoveTutorIntro::", 1)[1].split("PKMN_Center_Move_Tutor_NoBadges::", 1)[0]
        and "AddAllLegalMovesForSpecies" in read("src/pokemon.c")
        and "GetEggMovesSpecies" in read("src/pokemon.c")
        and "NUM_TECHNICAL_MACHINES + NUM_HIDDEN_MACHINES" in read("src/pokemon.c")
        and "TUTOR_MOVE_COUNT" in read("src/pokemon.c")
        and "moveLevel <= level" not in read("src/pokemon.c").split("GetMoveRelearnerMoves", 1)[1].split("GetLevelUpMovesBySpecies", 1)[0]
    ),
    "world TM pickups replaced": "finditem ITEM_TM" not in read("data/scripts/item_ball_scripts.inc"),
    "gifted TMs replaced": "giveitem ITEM_TM" not in "\n".join(p.read_text() for p in (ROOT / "data").rglob("*.inc")),
    "dead ability items removed from marts": "ITEM_ABILITY_CAPSULE" not in read("data/scripts/general_mart.inc") and "ITEM_ABILITY_PATCH" not in read("data/scripts/general_mart.inc"),
    "battle items permanently unlock": "BuildUnlockedBattleItemList" in read("src/item.c"),
    "Rare Candy and ten core battle items sold at every regular Mart": (
        "BuildPokemartItemsWithCoreStock(itemsForSale)" in read("src/shop.c")
        and all(item in read("src/shop.c").split("sCorePokemartStock[]", 1)[1].split("};", 1)[0] for item in starter_battle_items)
        and '[ITEM_RARE_CANDY]' in read("src/data/items.h")
        and '.price = 1000' in read("src/data/items.h").split('[ITEM_RARE_CANDY]', 1)[1].split('},', 1)[0]
        and all(
            ".price = 1000" in read("src/data/items.h").split(f"[{item}]", 1)[1].split("},", 1)[0]
            for item in starter_battle_items
        )
    ),
    "Poké Mart item names cannot overrun the Cancel row": (
        "u8 (*sItemNames)[ITEM_NAME_LENGTH]" in read("src/shop.c")
        and "u8 (*sItemNames)[16]" not in read("src/shop.c")
    ),
    "core battle items no longer duplicate campaign gifts or pickups": all(
        f"giveitem {item}" not in "\n".join(p.read_text() for p in (ROOT / "data" / "maps").rglob("scripts.inc"))
        and f'"item": "{item}"' not in "\n".join(p.read_text() for p in (ROOT / "data" / "maps").rglob("map.json"))
        and f"finditem {item}" not in read("data/scripts/item_ball_scripts.inc")
        for item in starter_battle_items
    ),
    "ordinary held items restore": "RestorePlayerHeldItemsAfterBattle" in read("src/battle_main.c"),
    "Champions Mega species registered": all(
        f"SPECIES_{name}" in read("include/constants/species.h")
        for name in champions_megas
    ),
    "Champions Mega Stones registered": all(
        f"ITEM_{item.upper()}" in read("include/constants/items.h")
        for _, item in champions_megas.values()
    ),
    "Champions Mega evolutions registered": all(
        f"SPECIES_{name}" in read("src/data/pokemon/evolution.h")
        for name in champions_megas
    ),
    "Champions Mega graphics complete": all(
        all(
            (ROOT / "graphics" / "pokemon" / f"mega_{asset}" / filename).is_file()
            for filename in ("front.png", "back.png", "icon.png", "normal.pal", "shiny.pal")
        )
        for asset, _ in champions_megas.values()
    ),
    "Champions Mega Stone graphics complete": all(
        (ROOT / "graphics" / "items" / "icons" / f"{item}.png").is_file()
        and (ROOT / "graphics" / "items" / "icon_palettes" / f"{item}.pal").is_file()
        for _, item in champions_megas.values()
    ),
    "Champions Mega abilities implemented": all(
        ability in read("src/battle_util.c") + read("src/battle_main.c")
        for ability in ("ABILITY_PIERCING_DRILL", "ABILITY_DRAGONIZE", "ABILITY_MEGA_SOL")
    ),
    "Mega Sol drives Weather Ball visuals": (
        "GetBattlerAbility(gBattleAnimAttacker) == ABILITY_MEGA_SOL"
        in read("src/battle_anim_effects_3.c")
    ),
    "Mega Bracelet introduced by Steven with Norman fallback": (
        "giveitem ITEM_MEGA_BRACELET" in read("data/maps/GraniteCave_StevensRoom/scripts.inc")
        and "setflag FLAG_SYS_RECEIVED_KEYSTONE" in read("data/maps/GraniteCave_StevensRoom/scripts.inc")
        and all(stone in read("data/maps/GraniteCave_StevensRoom/scripts.inc")
                for stone in ("ITEM_SCEPTILITE", "ITEM_BLAZIKENITE", "ITEM_SWAMPERTITE"))
        and "goto_if_set FLAG_SYS_RECEIVED_KEYSTONE" in read("data/maps/PetalburgCity_Gym/scripts.inc")
        and "giveitem ITEM_MEGA_BRACELET" in read("data/maps/PetalburgCity_Gym/scripts.inc")
    ),
    "all new Mega Stones reward exploration": (
        "GrantChampionsMegaStones" not in read("src/field_specials.c")
        and "GrantChampionsMegaStones" not in read("data/specials.inc")
        and all(
            f"{script}::" in read("data/scripts/item_ball_scripts.inc")
            and stone in read("data/scripts/item_ball_scripts.inc").split(f"{script}::", 1)[1].split("end", 1)[0]
            for script, stone in world_mega_stones.items()
        )
    ),
    "all new Megas appear during campaign progression": all(
        species in array_body("src/data/trainer_parties.h", party)
        and stone in array_body("src/data/trainer_parties.h", party)
        for party, (species, stone) in trainer_mega_showcases.items()
    ) and all(
        sum(
            f"ITEM_{item.upper()}" in array_body("src/data/trainer_parties.h", party)
            for _, item in champions_megas.values()
        ) == 1
        for party in trainer_mega_showcases
    ),
    "all new Mega families remain obtainable": all(
        species in read("src/data/wild_encounters.json")
        or species in "\n".join(p.read_text() for p in (ROOT / "data" / "maps").rglob("scripts.inc"))
        for species in mega_family_bases
    ),
    "wild encounter table lengths match configured slots": (
        wild_table_lengths_match
        and wild_slot_counts == {
            "land_mons": 12,
            "water_mons": 4,
            "rock_smash_mons": 4,
            "fishing_mons": 10,
            "honey_mons": 6,
        }
        and "#define WATER_WILD_COUNT    4" in read("include/wild_encounter.h")
        and "#define ROCK_WILD_COUNT     4" in read("include/wild_encounter.h")
    ),
    "disabled trainer entries leave no stray active commas": (
        re.search(r"^\s*},?\s+\*/,\s*$", read("src/data/trainer_parties.h"), re.M) is None
    ),
    "Area Dex is registered in the legacy linker": (
        "src/area_dex.o(.text);" in read("ld_script.txt")
        and "src/area_dex.o(.rodata);" in read("ld_script.txt")
        and '.include "src/area_dex.o"' in read("sym_ewram.txt")
    ),
    "Area Dex replaces only the redundant normal-menu Exit": (
        "MENU_ACTION_AREA_DEX" in normal_start_menu
        and "if (FlagGet(FLAG_SYS_POKEDEX_GET) == TRUE)" in normal_start_menu
        and "AddStartMenuAction(MENU_ACTION_EXIT);" in normal_start_menu
        and start_menu_source.count("AddStartMenuAction(MENU_ACTION_EXIT);") >= 6
        and "{sText_MenuAreaDex, {.u8_void = StartMenuAreaDexCallback}}" in start_menu_source
        and "SetMainCallback2(CB2_InitAreaDex);" in start_menu_source
        and "gMain.savedCallback = CB2_ReturnToFieldWithOpenMenu;" in start_menu_source
    ),
    "Area Dex covers native encounter methods safely": (
        all(
            token in read("src/area_dex.c")
            for token in (
                "WILD_SLOT_LAND",
                "WILD_SLOT_WATER",
                "WILD_SLOT_OLD_ROD",
                "WILD_SLOT_GOOD_ROD",
                "WILD_SLOT_SUPER_ROD",
                "WILD_SLOT_ROCK_SMASH",
                "WILD_SLOT_HONEY",
                "SPECIES_FEEBAS",
                "GetCurrentMapWildMonHeaderId()",
                "headerId != 0xFFFF",
            )
        )
        and "TryGetAbilityInfluencedWildMonIndex(wildMonInfo->wildPokemon, WATER_WILD_COUNT" in read("src/wild_encounter.c")
        and "TryGetRandomWildMonIndexByType(wildMon, type, numMon, monIndex)" in read("src/wild_encounter.c")
    ),
}

failed = [name for name, passed in checks.items() if not passed]
for name, passed in checks.items():
    print(f"{'PASS' if passed else 'FAIL'}: {name}")
if failed:
    raise SystemExit(f"{len(failed)} Verdant regression check(s) failed")
print(f"All {len(checks)} Verdant regression checks passed")
