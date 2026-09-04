#!/usr/bin/env python3
"""Static invariants for the Emerald Champions core-service checkpoint."""

from __future__ import annotations

import json
import re
from pathlib import Path
from item_catalog import battle_item_categories, free_vendor_items


ROOT = Path(__file__).resolve().parents[1]

# Persisted by the 81e288b51995c59c1dbc640f77907b8120788bc9 save
# contract. These ordinals are data, not merely names: the runtime migration
# copies their bits into the current append-only Sign fields.
LEGACY_81E_SIGN_PREFIX = (
    "LEGENDARY_SIGN_ARCEUS",
    "LEGENDARY_SIGN_AZELF",
    "LEGENDARY_SIGN_BLACEPHALON",
    "LEGENDARY_SIGN_BUZZWOLE",
    "LEGENDARY_SIGN_CALYREX",
    "LEGENDARY_SIGN_CELEBI",
    "LEGENDARY_SIGN_CELESTEELA",
    "LEGENDARY_SIGN_COBALION",
    "LEGENDARY_SIGN_CRESSELIA",
    "LEGENDARY_SIGN_DARKRAI",
    "LEGENDARY_SIGN_DIALGA",
    "LEGENDARY_SIGN_ENTEI",
    "LEGENDARY_SIGN_ETERNATUS",
    "LEGENDARY_SIGN_GENESECT",
    "LEGENDARY_SIGN_GIRATINA",
    "LEGENDARY_SIGN_GLASTRIER",
    "LEGENDARY_SIGN_GUZZLORD",
    "LEGENDARY_SIGN_HOOPA",
    "LEGENDARY_SIGN_KARTANA",
    "LEGENDARY_SIGN_KYUREM",
    "LEGENDARY_SIGN_LANDORUS",
    "LEGENDARY_SIGN_MARSHADOW",
    "LEGENDARY_SIGN_MESPRIT",
    "LEGENDARY_SIGN_NECROZMA",
    "LEGENDARY_SIGN_NIHILEGO",
    "LEGENDARY_SIGN_PALKIA",
    "LEGENDARY_SIGN_PHEROMOSA",
    "LEGENDARY_SIGN_PHIONE",
    "LEGENDARY_SIGN_POIPOLE",
    "LEGENDARY_SIGN_RAIKOU",
    "LEGENDARY_SIGN_REGIDRAGO",
    "LEGENDARY_SIGN_REGIELEKI",
    "LEGENDARY_SIGN_RESHIRAM",
    "LEGENDARY_SIGN_SHAYMIN",
    "LEGENDARY_SIGN_SPECTRIER",
    "LEGENDARY_SIGN_STAKATAKA",
    "LEGENDARY_SIGN_TAPU_BULU",
    "LEGENDARY_SIGN_TAPU_KOKO",
    "LEGENDARY_SIGN_TAPU_LELE",
    "LEGENDARY_SIGN_THUNDURUS",
    "LEGENDARY_SIGN_TORNADUS",
    "LEGENDARY_SIGN_UXIE",
    "LEGENDARY_SIGN_VICTINI",
    "LEGENDARY_SIGN_VIRIZION",
    "LEGENDARY_SIGN_XERNEAS",
    "LEGENDARY_SIGN_XURKITREE",
    "LEGENDARY_SIGN_YVELTAL",
    "LEGENDARY_SIGN_ZACIAN",
    "LEGENDARY_SIGN_ZAMAZENTA",
    "LEGENDARY_SIGN_ZARUDE",
    "LEGENDARY_SIGN_ZEKROM",
    "LEGENDARY_SIGN_ZERAORA",
    "LEGENDARY_SIGN_ZYGARDE",
)


def read(path: str) -> str:
    return (ROOT / path).read_text()


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def main() -> None:
    option_menu = read("src/option_menu.c")
    battle_main = read("src/battle_main.c")
    battle_setup = read("src/battle_setup.c")
    battle_util = read("src/battle_util.c")
    battle_config = read("include/config/battle.h")
    general_config = read("include/config/general.h")
    graphics = read("src/graphics.c")
    species_config = read("include/config/species_enabled.h")
    new_game = read("src/new_game.c")
    overworld = read("src/overworld.c")
    core_migration = overworld.split("void MigrateEmeraldChampionsCoreState(void)", 1)[1].split("void CB2_ContinueSavedGame(void)", 1)[0]
    vars_header = read("include/constants/vars.h")
    sign_header = read("include/legendary_signs.h")
    sign_definitions = read("src/data/pokemon/legendary_signs.h")
    pokemon_config = read("include/config/pokemon.h")
    text_config = read("include/config/text.h")
    summary_config = read("include/config/summary_screen.h")
    tutor = read("data/scripts/emerald_champions.inc")
    wild = read("src/wild_encounter.c")
    item_use = read("src/item_use.c")
    require(
        "special SetHiddenNature" in tutor and "special BufferSelectedMonNature" in tutor,
        "Nature must be changeable standalone at the Center specialist",
    )
    require(
        "SCROLL_MULTI_EMERALD_CHAMPIONS_NATURES" in tutor
        and "task->tNumItems = NUM_NATURES;" in read("src/field_specials.c")
        and "BuildEmeraldChampionsNatureMenuText(i)" in read("src/field_specials.c"),
        "Nature service must list every Nature from gNaturesInfo in a native scrolling list",
    )
    require(
        "if (sSweetScentInverted || (LURE_STEP_COUNT != 0" in wild,
        "Sweet Scent must invert the rarity curve",
    )
    require(
        "TryEndRepelSprayForAttractant()" in item_use
        and "sText_RepelSprayEnded" in item_use,
        "Attracting items must end the Repel Spray and say so",
    )
    lure_task = item_use.split("static void Task_UseLure(u8 taskId)\n{", 1)[1].split("\n}\n", 1)[0]
    repel_task = item_use.split("static void Task_UseRepel(u8 taskId)\n{", 1)[1].split("\n}\n", 1)[0]
    require(
        "TryEndRepelSprayForAttractant()" in lure_task
        and "TryEndRepelSprayForAttractant()" not in repel_task,
        "Lures (not Repels) must end the Repel Spray",
    )
    field_specials = read("src/field_specials.c")
    require(
        "{RIGHT_ARROW}" in field_specials
        and "GetEmeraldChampionsStatPointBreakpoint" in field_specials,
        "Stat Point editor must show the resulting stat and the next breakpoint",
    )
    battle_sets = read("src/emerald_champions_battle_sets.c")
    require(
        "TryNormalizeEmeraldChampionsBellyDrumHpParity(mon);" in battle_sets
        and "TryNormalizeEmeraldChampionsBellyDrumHpParity(" in read("src/party_menu.c"),
        "Belly Drum + berry spreads must be re-landed on even HP by presets and the Leveler",
    )
    pokemon_c = read("src/pokemon.c")
    require(
        "statInvestment = min(2 * ev[i], 63);" in pokemon_c
        and "statInvestment = min(2 * ev[STAT_HP], 63);" in pokemon_c,
        "one Stat Point must be worth two investment, capped at 63",
    )
    require(
        "#define B_EC_CATCH_ODDS_PERCENT         125" in read("include/config/battle.h")
        and "odds = odds * B_EC_CATCH_ODDS_PERCENT / 100;" in read("src/battle_script_commands.c"),
        "capture odds must carry the Emerald Champions flat boost after all other modifiers",
    )
    require(
        "#define B_MISSING_BADGE_CATCH_MALUS     GEN_3" in read("include/config/battle.h"),
        "the badge catch malus must stay off: caps already clamp wild levels",
    )
    field_move = read("src/field_move.c")
    require(
        field_move.count(".hideIfLocked = TRUE,") >= 8
        and "SpeciesCanLearnFieldMove(" in field_move
        and "EventScript_NobodyCanUseFieldMove::" in read("data/scripts/field_move_scripts.inc")
        and "EventScript_NobodyCanSurf" in read("src/field_control_avatar.c")
        and all(name in read("scripts/render_emerald_champions_ui.py") for name in ("field-move-cut-fallback", "field-move-rock-smash-fallback", "field-move-strength-fallback", "flight-beacon-fly"))
        and "u32 FieldMove_GetUserSlot(enum FieldMove fieldMove, bool32 doUnlockedCheck)" in field_move
        and "FieldMove_GetUserSlot(fieldMove, doUnlockedCheck)" in read("src/scrcmd.c")
        and "FieldMove_GetUserSlot(FIELD_MOVE_SURF, TRUE)" in read("src/field_player_avatar.c")
        and "IsFieldMoveUnlocked(FIELD_MOVE_FLASH)" in read("src/overworld.c"),
        "HM field moves must hide until unlocked and then work for any party member; caves light once Flash is unlocked",
    )
    require(
        read("data/scripts/pkmn_center_nurse.inc").count("giveitem ITEM_FLIGHT_BEACON, 1") == 2
        and "ItemUseOutOfBattle_FlightBeacon" in item_use
        and "SetFlyMapCancelCallback(" in item_use,
        "the Flight Beacon must be handed out with the starter tools, back-filled, and open the fly map",
    )
    player_controller = read("src/battle_controller_player.c")
    require(
        "MoveSelectionDisplayFoeTypes(battler);" in player_controller
        and "JOY_NEW(R_BUTTON) && !gBattleStruct->zmove.viewing" in player_controller
        and "PlaySE(SE_SELECT);\n        // Reuse the native move-description window on the action-menu page."
            in player_controller
        and "SetWindowAttribute(B_WIN_MOVE_DESCRIPTION, WINDOW_TILEMAP_TOP, 27);\n        OpenFoeTypesSubmenu(battler);"
            in player_controller,
        "R from both action and move selection must show the foes' types without replacing the action menu",
    )
    summary_screen = read("src/pokemon_summary_screen.c")
    require(
        "gRelearnMode == RELEARN_MODE_SCRIPT && gMoveRelearnerState == MOVE_RELEARNER_ALL_MOVES"
        in summary_screen,
        "the all-moves preparation tutor must be able to replace an HM move",
    )
    start_menu = read("src/start_menu.c")
    overworld = read("src/overworld.c")
    # Reload lives on the Start menu only. A prompt on the whiteout screen was
    # removed on purpose: players have not always saved before that battle, so
    # offering a reload there invites throwing progress away.
    require(
        "MENU_ACTION_RELOAD_SAVE" in start_menu
        and "if (CanReloadLastSave())" in start_menu
        and "WhiteOutReload" not in overworld
        and "WhiteOutReload" not in tutor,
        "Reload must be offered from the Start menu and never from the whiteout screen",
    )
    ai_util = read("src/battle_ai_util.c")
    ai_main = read("src/battle_ai_main.c")
    require(
        "enum Move GetLockedInMove(enum BattlerId battler)" in ai_util
        and ai_util.count("locked = GetLockedInMove(opposingBattler)") == 2,
        "Every AI must treat an Encore lock as certain knowledge of the target's move",
    )
    require(
        "IsTargetCertainToBlockWithProtect(battlerAtk, battlerDef, move)" in ai_main,
        "AI must not attack into a Protect that cannot fail",
    )
    party = read("src/data/trainers.party")
    ai_lines = {line for line in party.splitlines() if line.startswith("AI: ")}
    require(ai_lines, "trainers.party declares AI flags")
    # "Assumptions" is AI_FLAG_ASSUMPTIONS: Assume Stab, Assume Status Moves and
    # Weigh Ability Prediction. Every trainer reads held items, unrevealed status
    # moves and likely abilities; tiers differ only in switching and prediction.
    floor = ("Assumptions", "Hp Aware", "Smart Mon Choices", "Try To 2HKO")
    dumb = sorted(
        line for line in ai_lines
        if "Smart Trainer" not in line and not all(flag in line for flag in floor)
    )
    require(not dumb, f"no campaign trainer may sit below the competent AI floor: {dumb[:3]}")
    bosses = sorted(line for line in ai_lines if "Smart Trainer" in line)
    require(all("Assumptions" in line and "Prediction" in line for line in bosses),
            "every boss keeps Assumptions and the full Prediction set")

    nurse = read("data/scripts/pkmn_center_nurse.inc")
    birch_lab = read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
    items = read("src/data/items.h")
    item_use = read("src/item_use.c")
    field_specials = read("src/field_specials.c")
    specials = read("data/specials.inc")
    field_moves = read("src/field_move.c")
    party_menu = read("src/party_menu.c")
    vendor_scripts = read("data/scripts/emerald_champions.inc")
    trainers = read("src/data/trainers.party")

    retired_effort_ribbon_specials = (
        "LeadMonHasEffortRibbon",
        "GiveLeadMonEffortRibbon",
        "Special_AreLeadMonEVsMaxedOut",
    )
    require(
        all(name not in field_specials and name not in specials for name in retired_effort_ribbon_specials),
        "retired EV/Effort Ribbon specials were reintroduced into the live engine",
    )

    require("#define GEN_LATEST GEN_CHAMPIONS" in general_config, "battle standard is not pinned to Champions")
    title_asset = ROOT / "graphics/title_screen/emerald_champions_version.png"
    require(
        graphics.count('"graphics/title_screen/emerald_champions_version.png"') == 2
        and "gTitleScreenEmeraldVersionGfx" in graphics
        and "gTitleScreenEmeraldVersionPal" in graphics
        and '"graphics/title_screen/emerald_version.png"' not in graphics
        and title_asset.is_file()
        and title_asset.stat().st_size > 0,
        "the live Emerald title path is not exclusively branded Emerald Champions",
    )
    require("#define P_MEGA_EVOLUTIONS                TRUE" in species_config, "Mega Evolution is disabled")
    require(
        "#define B_FLAG_DYNAMAX_BATTLE       0" in battle_config
        and "#define B_FLAG_TERA_ORB_CHARGED     0" in battle_config
        and "#define B_TERA_ORB_ALWAYS_CHARGED       FALSE" in battle_config,
        "Dynamax or Terastallization has a global campaign enable path",
    )
    require(
        "GEN_LATEST == GEN_CHAMPIONS" in battle_util
        and "gBattleTypeFlags & BATTLE_TYPE_TRAINER" in battle_util
        and "!(gBattleTypeFlags & BATTLE_TYPE_PYRAMID)" in battle_util,
        "ordinary Trainer battles no longer enforce the no-Bag puzzle rule",
    )
    require(
        "if (GEN_LATEST == GEN_CHAMPIONS)" in battle_util
        and "RestorePlayerPartyMonHeldItem(i);" in battle_util,
        "competitive held loadouts are not restored after battle",
    )
    require(
        "Dynamax:" not in trainers and "Gigantamax:" not in trainers and "Tera Type:" not in trainers,
        "an authored campaign trainer uses a non-Mega gimmick",
    )
    active_hoenn_source = "\n".join(
        path.read_text()
        for path in (ROOT / "data/maps").glob("*/**/*")
        if path.is_file() and "_Frlg" not in path.parts[-2]
    )
    for inaccessible_gimmick_item in ("ITEM_Z_POWER_RING", "ITEM_DYNAMAX_BAND", "ITEM_TERA_ORB"):
        require(
            inaccessible_gimmick_item not in active_hoenn_source,
            f"non-Mega campaign gimmick item is obtainable: {inaccessible_gimmick_item}",
        )

    require("COMPOUND_STRING(\"DIFFICULTY\")" in option_menu, "Options no longer exposes Difficulty")
    require(all(label in option_menu for label in ("DifficultyHard", "DifficultyMedium", "DifficultyEasy")), "Difficulty choices are incomplete")
    require("SetCurrentDifficultyLevel(DIFFICULTY_HARD);" in new_game, "Hard is not the new-game default")
    require(
        "MENUITEM_BATTLESTYLE" not in option_menu
        and "COMPOUND_STRING(\"BATTLE STYLE\")" not in option_menu,
        "Options still exposes the removed Shift/Set selector",
    )
    require(
        "TEXT SPEED" not in option_menu
        and "#define TEXT_SPEED_INSTANT           TRUE" in text_config
        and "optionsTextSpeed = OPTIONS_TEXT_SPEED_INSTANT;" in new_game,
        "instant text is not the fixed native default",
    )
    require(
        "gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SET;" in new_game
        and "gBattleScripting.battleStyle = OPTIONS_BATTLE_STYLE_SET;" in battle_main,
        "Emerald Champions no longer forces competitive Set-style battles",
    )
    require(
        "VarSet(VAR_EMERALD_CHAMPIONS_SAVE_VERSION, EMERALD_CHAMPIONS_SAVE_VERSION_CURRENT);" in new_game
        and "FlagSet(FLAG_EC_BESPOKE_TRAINER_FLAGS_MIGRATED);" in new_game
        and "sRepurposedRematchTrainerIds" in overworld
        and "ClearTrainerFlag(sRepurposedRematchTrainerIds[i]);" in overworld,
        "repurposed rematch trainer flags are not migrated safely",
    )
    require(
        "MigrateEmeraldChampionsCoreState();" in overworld
        and "VAR_EMERALD_CHAMPIONS_SAVE_VERSION                0x40B8" in vars_header
        and "if (version == EMERALD_CHAMPIONS_SAVE_VERSION_CURRENT)" in core_migration
        and core_migration.index("if (version == EMERALD_CHAMPIONS_SAVE_VERSION_CURRENT)")
            < core_migration.index("FlagGet(FLAG_EC_BESPOKE_TRAINER_FLAGS_MIGRATED)")
        and "legacyGymMarker" in core_migration
        and "legacyItemMarker" in core_migration
        and "completeLegacySignature" in core_migration
        and "completeLegacySignature\n     && !modernMarker" in core_migration
        and "modernMarker\n          && !legacyGymMarker\n          && !legacyItemMarker" in core_migration
        and "MigrateEmeraldChampions81eState();" in core_migration
        and "ResetAmbiguousEmeraldChampionsState();" in core_migration,
        "save version is not authoritative over the colliding 0x4C5 legacy Zygarde bit",
    )
    require(
        all(token in overworld for token in (
            "FLAG_EC_ITEM_PRISON_BOTTLE, FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE",
            "FLAG_HIDDEN_ITEMS_START, FLAG_UNUSED_0x2BB",
            "FLAG_ITEM_ROUTE_116_LUCARIONITE_Z, FLAG_ITEM_SAFARI_ZONE_SOUTH_EAST_BIG_PEARL",
            "FLAG_EC_STARTER_ARCHIVE_BULBASAUR, FLAG_RECEIVED_GAME_CORNER_POIPOLE",
            "sDirectClaimFlags",
            "Inclement's inherited static encounters are visible whenever their map",
            "SetEmeraldChampionsPhysicalSignFlags();",
        )),
        "legacy pickup, claim, or physical-object collisions are not reset and reconstructed",
    )
    enum_body = sign_header.split("enum LegendarySignId", 1)[1].split("};", 1)[0]
    current_sign_ids = tuple(re.findall(r"\bLEGENDARY_SIGN_[A-Z0-9_]+\b", enum_body))
    require(
        current_sign_ids[: len(LEGACY_81E_SIGN_PREFIX)] == LEGACY_81E_SIGN_PREFIX,
        "persisted 81e Legendary Sign ID prefix was reordered",
    )
    current_definition_ids = tuple(re.findall(
        r"(?:WILD|VISIBLE|OTHER)_SIGN\((LEGENDARY_SIGN_[A-Z0-9_]+)",
        sign_definitions,
    ))
    require(
        current_definition_ids[: len(LEGACY_81E_SIGN_PREFIX)] == LEGACY_81E_SIGN_PREFIX,
        "persisted 81e Legendary Sign definition prefix was reordered",
    )
    # Every place an enemy trainer party is built must apply the difficulty offset to it.
    # (DoTrainerBattle builds both; the in-battle Restart rebuilds both the same way.)
    built = battle_setup.count("CreateNPCTrainerParty(&gParties[B_TRAINER_OPPONENT_")
    scaled = battle_setup.count("ApplyTrainerLevelDifficulty(&gParties[B_TRAINER_OPPONENT_")
    require(built >= 2 and scaled == built,
            f"every built enemy trainer party must have difficulty applied (built={built} scaled={scaled})")
    require("P_LEVEL_UP_MOVE_LEARNING    FALSE" in pokemon_config, "Level-up prompts are not disabled")
    battle_util = read("src/battle_util.c")
    require(
        "obedienceLevel = GetCurrentLevelCap();" in battle_util,
        "Obedience must follow the strict level cap, not the vanilla badge ladder",
    )
    party_menu = read("src/party_menu.c")
    require(
        "gBattleResources->bufferB[partner][0] == CONTROLLER_CHOSENMONRETURNVALUE" in party_menu,
        "Double-faint replacement menu must reject the partner's pending pick",
    )
    require(
        "GetPartyIdFromBattleSlot(slot) == gBattleResources->bufferB[partner][1])\n            palFlags |= PARTY_PAL_TO_SWITCH;" in party_menu,
        "Double-faint replacement menu must tint the partner's pending pick with the native switch palette",
    )
    wild = read("src/wild_encounter.c")
    require(
        "if (FlagGet(FLAG_EC_REPEL_SPRAY_ACTIVE))\n        return FALSE;" in wild,
        "Repel Spray must suppress step-based wild encounters",
    )
    for deliberate in ("FishingWildEncounter", "SweetScentWildEncounter", "RockSmashWildEncounter"):
        body = wild.split(deliberate, 1)
        require(len(body) > 1, f"{deliberate} still exists")
    item_use = read("src/item_use.c")
    require(
        "FlagClear(FLAG_EC_REPEL_SPRAY_ACTIVE);" in item_use
        and "FlagSet(FLAG_EC_REPEL_SPRAY_ACTIVE);" in item_use
        and "VarSet(VAR_EC_REPEL_SPRAY_STEPS, EC_REPEL_SPRAY_STEPS);" in item_use
        and "EmeraldChampions_EventScript_RepelSprayWoreOff" in wild,
        "Repel Spray must run a 500-step counter and offer reuse when it wears off",
    )
    nurse = read("data/scripts/pkmn_center_nurse.inc")
    require(
        nurse.count("giveitem ITEM_REPEL_SPRAY, 1") == 2,
        "Repel Spray must be given with the starter tools and back-filled for existing saves",
    )
    receive_dex = birch_lab.split("LittlerootTown_ProfessorBirchsLab_EventScript_ReceivePokedex::", 1)[1].split("return", 1)[0]
    require(
        "setflag FLAG_SYS_NATIONAL_DEX" in receive_dex and "special EnableNationalPokedex" in receive_dex,
        "the initial Pokedex does not cover the Gen 1-9 campaign roster",
    )
    for flag in (
        "FLAG_RECEIVED_HM_CUT", "FLAG_RECEIVED_HM_FLASH", "FLAG_RECEIVED_HM_ROCK_SMASH",
        "FLAG_RECEIVED_HM_STRENGTH", "FLAG_RECEIVED_HM_SURF", "FLAG_RECEIVED_HM_FLY",
        "FLAG_RECEIVED_HM_DIVE", "FLAG_RECEIVED_HM_WATERFALL",
    ):
        require(flag in field_moves, f"field use no longer requires the story license {flag}")
    require(all(value in summary_config for value in (
        "P_ENABLE_MOVE_RELEARNERS         TRUE",
        "P_PRE_EVO_MOVES                  TRUE",
        "P_ENABLE_ALL_LEVEL_UP_MOVES      TRUE",
        "P_TM_MOVES_RELEARNER             TRUE",
        "P_ENABLE_ALL_TM_MOVES            TRUE",
    )), "Complete legal tutor access is not enabled")

    require("giveitem ITEM_POKE_VIAL" in nurse and "giveitem ITEM_LEVELER" in nurse, "Center does not grant both tools")
    require("copyvar VAR_POKE_VIAL_CHARGES, VAR_POKE_VIAL_MAX_CHARGES" in nurse, "Center does not refill the Vial")
    require(
        "for (u32 i = 0; i < gPartiesCount[B_TRAINER_PLAYER]; i++)" in item_use
        and "HealPokemon(&gParties[B_TRAINER_PLAYER][i]);" in item_use
        and "VarSet(VAR_POKE_VIAL_CHARGES, VarGet(VAR_POKE_VIAL_CHARGES) - 1);" in item_use,
        "Poke Vial no longer heals the complete live party and consumes exactly one charge",
    )
    # Keep the upstream item enum and handling for engine compatibility, but
    # Emerald Champions uses the Leveler exclusively.  No live producer may
    # sell, award, or place a Rare Candy.
    acquisition_paths = [ROOT / "data"]
    rare_candy_sources = []
    for base in acquisition_paths:
        for path in base.rglob("*"):
            if path.is_file():
                try:
                    source = path.read_text()
                except UnicodeDecodeError:
                    continue
                if "ITEM_RARE_CANDY" in source:
                    rare_candy_sources.append(str(path.relative_to(ROOT)))
    require(not rare_candy_sources, f"Rare Candy acquisition paths remain: {rare_candy_sources}")
    route111 = read("data/maps/Route111/scripts.inc")
    route133 = read("data/maps/Route133/scripts.inc")
    require(
        "setvar VAR_POKE_VIAL_MAX_CHARGES, 2" in route111
        and "setvar VAR_CHANSEY_NURSE_STATE, 7" in route111,
        "the one-time Chansey quest does not grant the second Vial charge",
    )
    require(
        "setvar VAR_POKE_VIAL_MAX_CHARGES, 3" in route133,
        "Route 133 does not grant the final Vial charge",
    )

    oldale = read("data/maps/OldaleTown_Mart/scripts.inc")
    expanded_oldale = oldale.split("OldaleTown_Mart_Pokemart_Expanded:", 1)[1].split("pokemartlistend", 1)[0]
    require("ITEM_POKE_BALL" in expanded_oldale, "Oldale Mart never stocks Poke Balls after the adventure starts")

    require(
        "AppendToList(sPartyMenuInternal->actions, &sPartyMenuInternal->numActions, MENU_OPEN_ABILITY)" in party_menu,
        "the normal party menu lacks on-the-fly Ability switching",
    )
    require(
        "GetEmeraldChampionsBattleSetCountForFormat(" in field_specials
        and "gSpecialVar_0x8007" in field_specials
        and "task->tMaxItemsOnScreen = min(task->tNumItems, 4);" in field_specials
        and "task->tWidth = ConvertPixelWidthToTileWidth(width);" in field_specials
        and "if (task->tLeft + task->tWidth > MAX_MULTICHOICE_WIDTH + 1)" in field_specials,
        "the battle-role chooser is no longer dynamically sized and screen-bounded",
    )

    centers = tuple((ROOT / "data" / "maps").glob("*PokemonCenter_1F/map.json"))
    target_centers = []
    for path in centers:
        data = json.loads(path.read_text())
        scripts = [obj["script"] for obj in data["object_events"]]
        if "Common_EventScript_EmeraldChampionsBattleVendor" in scripts or "Common_EventScript_EmeraldChampionsMoveTutor" in scripts:
            target_centers.append(path)
            require(scripts.count("Common_EventScript_EmeraldChampionsBattleVendor") == 1, f"Battle vendor count wrong in {path.parent.name}")
            require(scripts.count("Common_EventScript_EmeraldChampionsMoveTutor") == 1, f"Move tutor count wrong in {path.parent.name}")
            coordinates = [(obj["x"], obj["y"]) for obj in data["object_events"]]
            require(len(coordinates) == len(set(coordinates)), f"Object overlap in {path.parent.name}")
            for service_script, service_name in (
                ("Common_EventScript_EmeraldChampionsBattleVendor", "Battle vendor"),
                ("Common_EventScript_EmeraldChampionsMoveTutor", "Move tutor"),
            ):
                service = next(obj for obj in data["object_events"] if obj["script"] == service_script)
                interaction_tile = (service["x"], service["y"] + 1)
                blockers = [
                    obj for obj in data["object_events"]
                    if obj is not service and (obj["x"], obj["y"]) == interaction_tile
                ]
                require(not blockers, f"{service_name} interaction tile is blocked in {path.parent.name}")
    require(len(target_centers) == 16, f"Expected 16 serviced Hoenn Centers, found {len(target_centers)}")

    medicine = {"ITEM_POTION", "ITEM_SUPER_POTION", "ITEM_HYPER_POTION", "ITEM_MAX_POTION", "ITEM_FULL_RESTORE"}
    medicine_lists = 0
    paths = [ROOT / "data" / "scripts" / "mart_clerk.inc"]
    paths.extend(path for path in (ROOT / "data" / "maps").glob("*/scripts.inc") if "_Frlg" not in path.parent.name)
    for path in paths:
        lines = path.read_text().splitlines()
        for index, line in enumerate(lines):
            if line.strip() != "pokemartlistend":
                continue
            cursor = index - 1
            listed: set[str] = set()
            while cursor >= 0 and lines[cursor].lstrip().startswith(".2byte ITEM_"):
                listed.add(lines[cursor].strip().split()[-1])
                cursor -= 1
            if listed.intersection(medicine):
                medicine_lists += 1
                require("ITEM_RARE_CANDY" not in listed, f"Medicine mart still sells obsolete Rare Candy: {path}")
    require(medicine_lists == 21, f"Expected 21 Hoenn medicine lists, found {medicine_lists}")

    free_items = free_vendor_items(ROOT)
    require(not any("_BERRY" in item for item in free_items), "Berries leaked into the free vendor")

    mega_items = set()
    z_crystal_items = set()
    for match in re.finditer(r"\[(ITEM_[A-Z0-9_]+)\]\s*=\s*\{(.*?)\n\s*\},", items, re.S):
        if "HOLD_EFFECT_MEGA_STONE" in match.group(2):
            mega_items.add(match.group(1))
        if "HOLD_EFFECT_Z_CRYSTAL" in match.group(2):
            z_crystal_items.add(match.group(1))
    require(not free_items.intersection(mega_items), "Mega Stones leaked into the free vendor")
    evolution_items = set(re.findall(
        r"ITEM_[A-Z0-9_]+",
        read("src/data/emerald_champions_evolution_items.h"),
    ))
    require(not free_items.intersection(evolution_items), "Evolution items leaked into the free vendor")
    forbidden_parts = ("_PLATE", "_MEMORY", "_DRIVE", "_MASK", "_Z_CRYSTAL", "TERA_SHARD")
    require(not any(any(part in item for part in forbidden_parts) for item in free_items), "Progression held items leaked into the free vendor")
    require(not free_items.intersection({"ITEM_RED_ORB", "ITEM_BLUE_ORB", "ITEM_RUSTED_SWORD", "ITEM_RUSTED_SHIELD"}), "Transformation items leaked into the free vendor")

    categories = list(battle_item_categories(ROOT).values())
    require(sum(map(len, categories)) == len(set().union(*categories)), "a held item appears in multiple vendor categories")
    require(
        all(token in vendor_scripts for token in (
            "EmeraldChampions_Text_HeldItems",
            "EmeraldChampions_Text_OffenseItems",
            "EmeraldChampions_Text_DefenseItems",
            "EmeraldChampions_Text_FieldItems",
            "EmeraldChampions_Text_TypeItems",
            "EmeraldChampions_Text_GemItems",
            "EmeraldChampions_Text_SpeciesItems",
        )),
        "the Pokemon Center held-item category menu is incomplete",
    )

    presets = json.loads(read("data/emerald_champions/emerald_champions_battle_sets.json"))
    preset_items = {
        entry[field]
        for group in ("defaults", "alternatives", "singles_defaults", "singles_alternatives")
        for entry in presets[group]
        for field in ("item", "required_item")
    }
    trainer_items = set(re.findall(r"@\s*(ITEM_[A-Z0-9_]+)", trainers))
    require(
        not z_crystal_items.intersection(free_items | preset_items | trainer_items),
        "a live campaign loadout enables Z-Moves",
    )
    preset_protected = mega_items | evolution_items | {
        item for item in preset_items
        if any(part in item for part in forbidden_parts)
    } | {"ITEM_RED_ORB", "ITEM_BLUE_ORB", "ITEM_RUSTED_SWORD", "ITEM_RUSTED_SHIELD"}
    preset_berries = {item for item in preset_items if "_BERRY" in item}
    preset_entries = [
        entry
        for group in ("defaults", "alternatives", "singles_defaults", "singles_alternatives")
        for entry in presets[group]
    ]
    # Held-form items are equipped by the preset itself. Exempt only the
    # species/item pair required by the engine's form table, not the item in
    # every preset where it could accidentally appear.
    held_forms = set(re.findall(
        r"\{FORM_CHANGE_ITEM_HOLD,\s*(SPECIES_[A-Z0-9_]+),\s*(ITEM_[A-Z0-9_]+)\s*\}",
        read("src/data/pokemon/form_change_tables.h"),
    ))
    ordinary_preset_items = {
        entry[field]
        for entry in preset_entries
        for field in ("item", "required_item")
        if (entry["species"], entry[field]) not in held_forms
    } - preset_protected - preset_berries - {"ITEM_NONE"}
    require(
        ordinary_preset_items <= free_items,
        f"competitive presets use unavailable ordinary held items: {sorted(ordinary_preset_items - free_items)}",
    )
    evolution_held_roles = {
        (entry["species"], entry["item"])
        for entry in preset_entries
        if entry["item"] in evolution_items
    }
    require(
        evolution_held_roles == {
            ("SPECIES_CLAMPERL", "ITEM_DEEP_SEA_TOOTH"),
            ("SPECIES_CLAMPERL", "ITEM_DEEP_SEA_SCALE"),
        },
        f"unexpected evolution-item battle roles: {sorted(evolution_held_roles)}",
    )
    require(
        "PresetRequiresOwnedHeldItem" in read("src/emerald_champions_battle_sets.c"),
        "protected held-item presets are not gated by actual ownership",
    )

    print("core_service_static_checks=PASS")
    print(f"pokemon_centers={len(target_centers)}")
    print(f"medicine_mart_lists={medicine_lists}")
    print(f"free_battle_items={len(free_items) - 1}")


if __name__ == "__main__":
    main()
