#!/usr/bin/env python3
"""Focused source regressions for Battle Frontier engine and UI fixes."""

from pathlib import Path
import sys


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


def section(text: str, start: str, end: str) -> str:
    return text.split(start, 1)[1].split(end, 1)[0]


def main() -> int:
    tower_script = read("data/maps/BattleFrontier_BattleTowerLobby/scripts.inc")
    link_save = section(
        tower_script,
        "BattleFrontier_BattleTowerLobby_EventScript_SaveBeforeLinkMultisChallenge::",
        "BattleFrontier_BattleTowerLobby_EventScript_FeelingsMan::",
    )
    frontier_pass = read("src/frontier_pass.c")
    frontier_util = read("src/frontier_util.c")
    dome = read("src/battle_dome.c")
    factory = read("src/battle_factory.c")
    factory_script = read("data/maps/BattleFrontier_BattleFactoryPreBattleRoom/scripts.inc")
    palace = read("src/battle_palace.c")
    pyramid = read("src/battle_pyramid.c")
    pike = read("src/battle_pike.c")
    tower = read("src/battle_tower.c")
    ally_anim = read("src/battle_anim_effects_1.c")
    ally_anim_script = read("data/battle_anim_scripts.s")
    ally_battle_script = read("data/battle_scripts_1.s")
    ally_commands = read("src/battle_script_commands.c")
    battle_util = read("src/battle_util.c")
    battle_interface = read("src/battle_interface.c")
    field_specials = read("src/field_specials.c")

    save_calls = {
        "src/battle_tower.c": "static void SaveTowerChallenge(void)\n{",
        "src/battle_dome.c": "static void SaveDomeChallenge(void)\n{",
        "src/battle_factory.c": "static void SaveFactoryChallenge(void)\n{",
        "src/battle_palace.c": "static void SavePalaceChallenge(void)\n{",
        "src/battle_arena.c": "static void SaveArenaChallenge(void)\n{",
        "src/battle_pike.c": "static void SavePikeChallenge(void)\n{",
        "src/battle_pyramid.c": "static void SavePyramidChallenge(void)\n{",
    }

    checks: dict[str, bool] = {
        "Link Multi does not use the partial SAVE_LINK path": "tower_save 0" not in link_save,
        "Link Multi entry stat waits for connection success": "incrementgamestat GAME_STAT_ENTERED_BATTLE_TOWER" not in link_save,
        "Frontier Pass loads only its eight real palettes": (
            "NUM_FRONTIER_PASS_BG_PALETTES 8" in frontier_pass
            and frontier_pass.count("NUM_FRONTIER_PASS_BG_PALETTES * sizeof(gFrontierPassBg_Pal[0])") == 2
            and "0x1A0" not in "\n".join(
                line for line in frontier_pass.splitlines() if "LoadPalette(gFrontierPassBg_Pal" in line
            )
        ),
        "Frontier Pass handles allocation failure": (
            "if (AllocateFrontierPassData(callback) != SUCCESS)" in frontier_pass
            and "if (AllocateFrontierPassGfx() != SUCCESS)" in frontier_pass
            and "sMapData == NULL" in frontier_pass
        ),
        "Frontier Pass reallocates graphics after child screens": (
            "sPassGfx == NULL && AllocateFrontierPassGfx() != SUCCESS" in section(
                frontier_pass, "void CB2_ReshowFrontierPass(void)\n{", "static void CB2_ReturnFromRecord"
            )
            and "if (AllocateFrontierPassData(callback) != SUCCESS)" in section(
                frontier_pass, "static void CB2_ReturnFromRecord(void)\n{", "static void CB2_ShowFrontierPassFeature"
            )
        ),
        "Ended facility challenges clear stale enemy rentals": all(
            "ClearEnemyPartyAfterChallenge();" in section(read(path), marker, "SaveGameFrontier")
            if "Pike" not in marker and "Pyramid" not in marker
            else "ClearEnemyPartyAfterChallenge();" in read(path).split(marker, 1)[1][:500]
            for path, marker in save_calls.items()
        ),
        "Tent challenges clear stale enemy rentals": read("src/battle_tent.c").count("ClearEnemyPartyAfterChallenge();") == 3,
        "Enemy rental cleanup is end-only": "if (gSpecialVar_0x8005 == 0)\n        ZeroEnemyPartyMons();" in frontier_util,
        "Dome final trainer re-add yields a frame": (
            "removeobject LOCALID_OPPONENT\n\tdelay 1\n\taddobject LOCALID_OPPONENT"
            in read("data/maps/BattleFrontier_BattleDomeBattleRoom/scripts.inc")
        ),
        "Slateport Tent trainer re-add yields a frame": (
            "removeobject LOCALID_OPPONENT\n\tdelay 1\n\taddobject LOCALID_OPPONENT"
            in read("data/maps/SlateportCity_BattleTentBattleRoom/scripts.inc")
        ),
        "All Pike no-return messages release object locks": (
            read("data/scripts/battle_pike.inc").count(
                "msgbox BattleFrontier_BattlePike_Text_PathBlockedNoTurningBack, MSGBOX_DEFAULT\n"
                "\tclosemessage\n\treleaseall\n\tend"
            )
            == 3
        ),
        "Pike hints retain hard and double rooms when healing is disabled": (
            "u8 roomCandidates[NUM_PIKE_ROOM_TYPES - 1];" in pike
            and "for (i = 0; i < PIKE_ROOM_BRAIN; i++)" in pike
            and "i == PIKE_ROOM_HEAL_FULL || i == PIKE_ROOM_HEAL_PART" in pike
            and "roomCandidates = AllocZeroed" not in section(pike, "static void SetHintedRoom(void)\n{", "static void GetHintedRoomIndex")
        ),
        "Pike status rooms honor Sweet Veil and Meteor Minior": (
            "ability == ABILITY_SWEET_VEIL" in pike
            and "ability == ABILITY_SHIELDS_DOWN" in pike
            and "GetFormIdFromFormSpeciesId(species) < GetFormIdFromFormSpeciesId(SPECIES_MINIOR_CORE_RED)" in pike
        ),
        "Pike first-room doubles cannot underflow trainer history": (
            "gSaveBlock2Ptr->frontier.curChallengeBattleNum > 1" in section(
                pike, "static void PrepareTwoTrainers(void)\n{", "static void ClearPikeTrainerIds"
            )
            and "? gSaveBlock2Ptr->frontier.curChallengeBattleNum - 1\n                     : 0;" in pike
        ),
        "Pike room count has one canonical constant": (
            "#define NUM_PIKE_ROOMS 14" in read("include/constants/battle_pike.h")
            and "curChallengeBattleNum > NUM_PIKE_ROOMS" in pike
            and "for (i = 0; i < NUM_PIKE_ROOMS; i++)" in pike
        ),
        "Pike maniac previews both Lucy teams one run early": (
            "[FRONTIER_MANIAC_BATTLE_PIKE]          = { 13, 112 }," in field_specials
            and "These reveal Lucy's teams at 14 and" in field_specials
            and "[FRONTIER_FACILITY_PIKE]    = {28, 140, 56, 1}" in read("src/frontier_util.c")
            and "#define NUM_PIKE_ROOMS 14" in read("include/constants/battle_pike.h")
        ),
        "Pike and shared Frontier use one Brain schedule": (
            "GetFrontierBrainStreakAppearances(FRONTIER_FACILITY_PIKE)" in pike
            and "static const u8 sFrontierBrainStreakAppearances" not in pike
            and "return sFrontierBrainStreakAppearances[facility];" in frontier_util
        ),
        "Factory highlight sheet uses its real asset size": (
            "{sActionHighlightLeft_Gfx,   sizeof(sActionHighlightLeft_Gfx),"
            in read("src/battle_factory_screen.c")
        ),
        "Factory selection releases every temporary graphics buffer": (
            read("src/battle_factory_screen.c").count("FREE_AND_SET_NULL(sSelectMonPicBgTilesetBuffer);") == 2
            and "AllocZeroed(sizeof(sMonPicBg_Gfx))" in read("src/battle_factory_screen.c")
        ),
        "Dome eliminated opponent is guarded before indexing": (
            "else if (tournamentId2 != 0xFF && DOME_TRAINERS[tournamentId2].trainerId == TRAINER_FRONTIER_BRAIN"
            in dome
        ),
        "Dome opponent lookup rejects invalid tournament state": (
            "if (i == DOME_TOURNAMENT_TRAINERS_COUNT)" in dome
            and dome.count("if (tournamentId >= DOME_TOURNAMENT_TRAINERS_COUNT)") >= 2
            and "return TRAINER_NONE;" in dome
        ),
        "Dome evaluates specific styles before broad subsets": (
            dome.index("DOME_BATTLE_STYLE_ENFEEBLE_HIGH,", dome.index("sBattleStyleEvaluationOrder"))
            < dome.index("DOME_BATTLE_STYLE_ENFEEBLE_LOW,", dome.index("sBattleStyleEvaluationOrder"))
            and dome.index("DOME_BATTLE_STYLE_DEF_OVER_ATK,", dome.index("sBattleStyleEvaluationOrder"))
            < dome.index("DOME_BATTLE_STYLE_DEF,", dome.index("sBattleStyleEvaluationOrder"))
        ),
        "Dome classifies post-Gen-3 moves from move data": "GetModernDomeMovePoints" in dome and "AddDomeMovePoints" in dome,
        "Dome scratch scoring cannot fail heap allocation": (
            "s16 allocatedArray[ALLOC_ARRAY_SIZE] = {0};" in dome
            and "u16 rankingScores[DOME_TOURNAMENT_TRAINERS_COUNT] = {0};" in dome
            and "u16 statSums[DOME_TOURNAMENT_TRAINERS_COUNT] = {0};" in dome
        ),
        "Dome info cards handle zero-EV parties": (
            dome.count("if (j != 0)\n        {\n            for (i = 0; i < NUM_STATS; i++)") >= 2
            and "if (k != 0)\n                k = MAX_TOTAL_EVS / k;" in dome
        ),
        "Dome stat scoring applies EV spread bits": (
            "resultingEvs = count == 0 ? 0 : MAX_TOTAL_EVS / count;\n    bits = 1;" in dome
            and "if (evBits & bits)\n            evs[i] = resultingEvs;" in dome
            and "(u8) ModifyStatByNature" not in dome
        ),
        "Dome simulation honors Levitate and combined Wonder Guard matchups": (
            "defAbility == ABILITY_LEVITATE && moveType == TYPE_GROUND" in dome
            and "typePower = TYPE_x0;" in section(
                dome, "static int GetTypeEffectivenessPoints", "static u8 GetDomeTrainerMonIvs"
            )
            and "defAbility == ABILITY_WONDER_GUARD && typePower <= TYPE_x1" in dome
            and "typeEffectiveness1 != 20 && typeEffectiveness2 != 20" not in dome
        ),
        "Dome winning-move text handles Brain forms and prior rounds": (
            "targetSpecies = DOME_MONS[loserTournamentId][k];" in dome
            and "targetNature = GetFrontierBrainMonNature(k);" in dome
            and "GetAbilityBySpecies(targetSpecies, personality & 1)" in dome
            and "for (i = 0; i < roundId; i++)" in dome
            and "roundId - 1" not in section(dome, "static u16 GetWinningMove", "static void Task_ShowTourneyTree")
        ),
        "Factory classifies post-Gen-3 moves from executable move data": all(
            token in section(factory, "static u8 GetMoveBattleStyle", "bool8 InBattleFactory")
            for token in (
                "effect = gBattleMoves[move].effect;",
                "case EFFECT_MISTY_TERRAIN:",
                "return FACTORY_STYLE_WEATHER;",
                "case EFFECT_FINAL_GAMBIT:",
                "return FACTORY_STYLE_HIGH_RISK;",
                "case EFFECT_HEAL_PULSE:",
                "return FACTORY_STYLE_ENDURANCE;",
            )
        ),
        "Factory rentals update held-item-dependent forms": (
            factory.count("TryUpdateMonFormForHeldItem(&gPlayerParty[i]);") >= 2
            and "TryUpdateMonFormForHeldItem(&gEnemyParty[i]);" in factory
            and read("src/battle_factory_screen.c").count(
                "TryUpdateMonFormForHeldItem(&sFactorySelectScreen->mons[i + firstMonId].monData);"
            ) == 2
        ),
        "Factory resume preserves enemy Frustration power": (
            "SetMonData(&gEnemyParty[i], MON_DATA_FRIENDSHIP, &friendship);" in section(
                factory, "static void SetPlayerAndOpponentParties(void)\n{", "static void GenerateInitialRentalMons"
            )
        ),
        "Facility-created opponents update held-item-dependent forms": (
            tower.count("TryUpdateMonFormForHeldItem(&gEnemyParty") >= 4
            and "TryUpdateMonFormForHeldItem(&gEnemyParty[monPartyId]);" in frontier_util
            and "TryUpdateMonFormForHeldItem(&gEnemyParty[monPartyId]);" in dome
            and read("src/pokemon.c").count("TryUpdateMonFormForHeldItem(mon);") >= 3
        ),
        "Factory opponent hints cover the Fairy type": (
            "compare VAR_0x8005, TYPE_FAIRY" in factory_script
            and "BattleFrontier_BattleFactoryPreBattleRoom_EventScript_OpponentUsesFairy::" in factory_script
            and "in the handling of the FAIRY type.$" in factory_script
        ),
        "Palace record compares the active level mode directly": (
            "palaceWinStreaks[battleMode][lvlMode] > gSaveBlock2Ptr->frontier.palaceRecordWinStreaks[battleMode][lvlMode]"
            in palace
        ),
        "Custom partners receive matching OT gender metadata": (
            "encounterMusic_gender >> 7;\n            SetMonData(&gPlayerParty[i + 3], MON_DATA_OT_GENDER, &j);"
            in read("src/battle_tower.c")
        ),
        "Multi opponents draw from their own Frontier rosters": (
            "monSet = gFacilityTrainers[trainerId].monSet;" in tower
            and "monSet = gFacilityTrainers[gTrainerBattleOpponent_A].monSet;" not in section(
                tower, "static void FillTrainerParty(u16 trainerId", "static void Unused_CreateApprenticeMons"
            )
        ),
        "Record-mix Multi partner candidates fit a doubles record": (
            "u32 validSpecies[MAX_FRONTIER_PARTY_SIZE];" in section(
                tower, "static void GetRecordMixFriendMultiPartnerParty", "static void LoadMultiPartnerCandidatesData"
            )
            and "u32 validSpecies[3];" not in tower
        ),
        "Tower record checksums stop at the checksum field": (
            tower.count("offsetof(struct EmeraldBattleTowerRecord, checksum) / sizeof(u32)") >= 6
            and tower.count("offsetof(struct Apprentice, checksum) / sizeof(u32)") == 2
            and "offsetof(struct RSBattleTowerRecord, checksum) / sizeof(u32)" in tower
        ),
        "Tower records preserve hidden abilities in the spare bit": (
            "u32 hiddenAbility:1;" in read("include/global.h")
            and "value = src->hiddenAbility ? 2 : src->abilityNum;" in read("src/pokemon.c")
            and read("src/pokemon.c").count("value = src->hiddenAbility ? 2 : src->abilityNum;") == 2
            and "dest->hiddenAbility = (i == 2);" in read("src/pokemon.c")
            and "dest->abilityNum = (i == 1);" in read("src/pokemon.c")
        ),
        "Tower records preserve explicit nature without growing the format": (
            "u16 heldItem:10;" in read("include/global.h")
            and "u16 nature:5;" in read("include/global.h")
            and "u16 hasExplicitNature:1;" in read("include/global.h")
            and "#if ITEMS_COUNT > (1 << 10)" in read("src/pokemon.c")
            and read("src/pokemon.c").count("if (src->hasExplicitNature)") == 2
            and "dest->nature = GetMonData(mon, MON_DATA_NATURE, NULL);" in read("src/pokemon.c")
        ),
        "Ruby record class conversion uses its real mapping bounds": (
            tower.count("if (i != ARRAY_COUNT(sRubyFacilityClassToEmerald))") == 2
            and "if (i != FACILITY_CLASSES_COUNT)" not in tower
        ),
        "Long Frontier streaks stay in highest difficulty ranges": (
            "GetRandomScaledFrontierTrainerId(u16 challengeNum" in tower
            and "GetFactoryMonFixedIV(u16 challengeNum" in factory
            and "GetFactoryMonId(u8 lvlMode, u16 challengeNum" in factory
            and "u16 challengeNum = gSaveBlock2Ptr->frontier.factoryWinStreaks" in read("src/battle_factory_screen.c")
        ),
        "Factory rental rank does not wrap after 255 swaps": (
            "u16 rents = gSaveBlock2Ptr->frontier.factoryRentsCount[battleMode][lvlMode];" in factory
        ),
        "Frontier save path cannot fail a party backup allocation": (
            "struct Pokemon monsParty[PARTY_SIZE];" in frontier_util
            and "memcpy(monsParty, gPlayerParty, sizeof(monsParty));" in frontier_util
            and "calloc(PARTY_SIZE, sizeof(struct Pokemon))" not in frontier_util
        ),
        "Frontier ranking records cannot fail heap allocation": (
            frontier_util.count("struct PlayerHallRecords playerHallRecords = {0};") == 2
            and "calloc(" not in frontier_util
        ),
        "Frontier party restore clamps script and saved indices": (
            "selectedCount = min(gSpecialVar_0x8005, MAX_FRONTIER_PARTY_SIZE);" in frontier_util
            and "if (selected <= PARTY_SIZE)" in frontier_util
        ),
        "Pyramid layout offsets cannot fail heap allocation": (
            pyramid.count("u8 floorLayoutOffsets[16] = {0};") == 4
            and "floorLayoutOffsets = AllocZeroed(16)" not in pyramid
        ),
        "Pyramid neighbor scans bounds-check before indexing": (
            pyramid.count(">= ARRAY_COUNT(sBorderedSquareIds[squareId])") == 4
            and "sBorderedSquareIds[squareId][borderedIndex] == 0xFF || borderedIndex >= 4" not in pyramid
            and "sBorderedSquareIds[squareId][borderOffset] == 0xFF || borderOffset >= 4" not in pyramid
        ),
        "Pyramid floor random packing uses unsigned shifts": (
            "u32 rand = (u32)gSaveBlock2Ptr->frontier.pyramidRandoms[0]" in pyramid
            and "((u32)gSaveBlock2Ptr->frontier.pyramidRandoms[3] << 16)" in pyramid
        ),
        "Pyramid party restore tracks Pokemon identity across forms": (
            "ClearSelectedPartyOrder();" in section(
                pyramid, "static void RestorePyramidPlayerParty(void)\n{", "static u8 GetPostBattleDirectionHintTextIndex"
            )
            and "MON_DATA_OT_ID" in section(
                pyramid, "static void RestorePyramidPlayerParty(void)\n{", "static u8 GetPostBattleDirectionHintTextIndex"
            )
            and "MON_DATA_PERSONALITY" in section(
                pyramid, "static void RestorePyramidPlayerParty(void)\n{", "static u8 GetPostBattleDirectionHintTextIndex"
            )
            and "GET_BASE_SPECIES_ID" in section(
                pyramid, "static void RestorePyramidPlayerParty(void)\n{", "static u8 GetPostBattleDirectionHintTextIndex"
            )
        ),
        "Ally Switch has a real visual/data swap": (
            "createvisualtask AnimTask_AllySwitchAttacker" in ally_anim_script
            and "createvisualtask AnimTask_AllySwitchPartner" in ally_anim_script
            and "AnimTask_AllySwitchDataSwap" in ally_anim
            and "SwitchTwoBattlersInParty(battler, partner);" in ally_anim
        ),
        "Ally Switch carries volatile state and external references": (
            "SwapAllySwitchBattlerReferences(battler, partner);" in ally_anim
            and "SwapAllySwitchAttractReferences" in ally_anim
            and "gBattleStruct->wrappedBy[i]" in ally_anim
            and "SwapAllySwitchLastTakenMoves" in ally_anim
            and "gEnigmaBerries[battler]" in ally_anim
        ),
        "Final Mega indicator frees gfx in singles and doubles": (
            "for (i = 0; i < gBattlersCount; i++)" in section(
                battle_interface, "void DestroyMegaIndicatorSprite(u32 healthboxSpriteId)\n{", "static void SpriteCb_MegaIndicator"
            )
            and "if (i == gBattlersCount)" in section(
                battle_interface, "void DestroyMegaIndicatorSprite(u32 healthboxSpriteId)\n{", "static void SpriteCb_MegaIndicator"
            )
        ),
        "Ally Switch advances the scripting attacker after swapping": (
            "allyswitchswapbattlers" in ally_battle_script
            and "case VARIOUS_ALLY_SWITCH_SWAP:" in ally_commands
        ),
        "Ally Switch rejects separately owned partner slots": (
            "BATTLE_TYPE_MULTI | BATTLE_TYPE_INGAME_PARTNER | BATTLE_TYPE_TOWER_LINK_MULTI" in ally_commands
            and "BATTLE_TYPE_TWO_OPPONENTS | BATTLE_TYPE_TOWER_LINK_MULTI" in ally_commands
            and "BATTLE_TYPE_TOWER_LINK_MULTI | BATTLE_TYPE_TWO_OPPONENTS" not in read("src/battle_tower.c")
        ),
        "Ally-target moves fail after the target switches slots": (
            "gProtectStructs[BATTLE_PARTNER(gBattlerAttacker)].usedAllySwitch" in battle_util
        ),
    }

    failures = [name for name, passed in checks.items() if not passed]
    for name, passed in checks.items():
        print(f"{'PASS' if passed else 'FAIL'}: {name}")
    print(f"\n{len(checks) - len(failures)}/{len(checks)} Frontier runtime checks passed")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
