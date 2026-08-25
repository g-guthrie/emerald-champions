#!/usr/bin/env python3
"""Static regression checks for Verdant's curated Gen 9 battle mechanics."""

from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def require(path: str, fragment: str, label: str) -> None:
    if fragment not in read(path):
        raise AssertionError(f"{label}: missing {fragment!r} in {path}")


def block(path: str, start: str, end: str) -> str:
    text = read(path)
    start_index = text.index(start)
    end_index = text.index(end, start_index + len(start))
    return text[start_index:end_index]


def check_description_width(path: str, width: int) -> None:
    for line_number, line in enumerate(read(path).splitlines(), 1):
        match = re.search(r'_\("(.*)"\)', line)
        if match is None:
            continue
        for visible_line in match.group(1).split(r"\n"):
            if len(visible_line) > width:
                raise AssertionError(
                    f"{path}:{line_number}: {len(visible_line)} chars exceeds {width}: {visible_line!r}"
                )


def main() -> None:
    moves = read("src/data/verdant_gen9_battle_moves.h")
    scripts = read("data/battle_scripts_1.s")
    commands = read("src/battle_script_commands.c")
    util = read("src/battle_util.c")
    ai_util = read("src/battle_ai_util.c")

    require("include/constants/battle_move_effects.h", "#define EFFECT_ELECTRO_SHOT 386", "Electro Shot effect id")
    require("data/battle_scripts_1.s", ".4byte BattleScript_EffectElectroShot", "Electro Shot effect dispatch")
    assert ".effect = EFFECT_ELECTRO_SHOT" in moves
    electro = block("data/battle_scripts_1.s", "BattleScript_EffectElectroShot:", "BattleScript_EffectGeomancy:")
    for fragment in (
        "setstatchanger STAT_SPATK, 1, FALSE",
        "jumpifweatheraffected BS_ATTACKER, WEATHER_RAIN_ANY, BattleScript_ElectroShotImmediateAttack",
        "HOLD_EFFECT_POWER_HERB",
        "clearstatusfromeffect BS_ATTACKER",
    ):
        assert fragment in electro, f"Electro Shot missing {fragment!r}"
    require("src/battle_ai_main.c", "case EFFECT_ELECTRO_SHOT:", "Electro Shot AI")
    electro_move = block(
        "src/data/verdant_gen9_battle_moves.h", "[MOVE_ELECTRO_SHOT]", "[MOVE_FLOWER_TRICK]"
    )
    assert "FLAG_BALLISTIC" in electro_move
    require("src/battle_script_commands.c", "[MOVE_ELECTRO_SHOT] = FORBIDDEN_SLEEP_TALK | FORBIDDEN_PARENTAL_BOND", "Electro Shot call restrictions")

    mortal = block("data/battle_scripts_1.s", "BattleScript_MortalSpinPoison:", ".else")
    assert "MOVE_EFFECT_POISON" in mortal
    assert "MOVE_EFFECT_RAPIDSPIN" in mortal
    assert "setstatchanger STAT_SPEED" not in mortal
    assert ".secondaryEffectChance = 100" in block(
        "src/data/verdant_gen9_battle_moves.h", "[MOVE_MORTAL_SPIN]", "[MOVE_ORDER_UP]"
    )
    assert "FLAG_MAKES_CONTACT" in block(
        "src/data/verdant_gen9_battle_moves.h", "[MOVE_MORTAL_SPIN]", "[MOVE_ORDER_UP]"
    )

    require("include/battle.h", "u8 rageFistHits[PARTY_SIZE][2];", "Rage Fist persistent state")
    require("src/battle_script_commands.c", "if (*hits < 6)", "Rage Fist hit cap")
    rage_power = block("src/battle_util.c", "case MOVE_RAGE_FIST:", "case MOVE_WATER_SHURIKEN:")
    assert "basePower += 50" in rage_power and "basePower = 350" in rage_power

    require("include/battle.h", "u32 burningBulwarked:1;", "Burning Bulwark state")
    require("src/battle_script_commands.c", "MOVE_EFFECT_BURN | MOVE_EFFECT_AFFECTS_USER", "Burning Bulwark contact burn")
    require("src/battle_util.c", "gProtectStructs[battlerId].burningBulwarked", "Burning Bulwark protection")
    burning_move = block(
        "src/data/verdant_gen9_battle_moves.h", "[MOVE_BURNING_BULWARK]", "[MOVE_ELECTRO_SHOT]"
    )
    assert "FLAG_PROTECTION_MOVE" in burning_move

    make_rain = block("data/battle_scripts_1.s", "BattleScript_EffectMakeItRain:", "BattleScript_EffectHammerArm::")
    assert "MOVE_EFFECT_SP_ATK_MINUS_1" in make_rain
    assert "MOVE_EFFECT_SP_ATK_TWO_DOWN" not in make_rain
    assert "makeItRainStatDropped" in commands
    assert "!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)" in commands
    assert commands.index("case MOVEEND_NEXT_TARGET") < commands.index("case MOVEEND_CLEAR_BITS")

    order_up = block(
        "src/battle_script_commands.c",
        "if (gCurrentMove == MOVE_ORDER_UP",
        "// Make It Rain is a spread move",
    )
    for fragment in (
        "SPECIES_DONDOZO",
        "ABILITY_COMMANDER",
        "SPECIES_TATSUGIRI_DROOPY",
        "MOVE_EFFECT_DEF_PLUS_1",
        "SPECIES_TATSUGIRI_STRETCHY",
        "MOVE_EFFECT_SPD_PLUS_1",
        "SPECIES_TATSUGIRI",
        "MOVE_EFFECT_ATK_PLUS_1",
    ):
        assert fragment in order_up, f"Order Up missing {fragment!r}"
    assert "FLAG_MAKES_CONTACT" not in block(
        "src/data/verdant_gen9_battle_moves.h", "[MOVE_ORDER_UP]", "[MOVE_RAGE_FIST]"
    )

    require("include/battle.h", "u16 cudChewBerry;", "Cud Chew stored Berry")
    require("src/battle_script_commands.c", "cudChewTurn = gBattleResults.battleTurnCounter", "Cud Chew consumption turn")
    require("src/battle_util.c", "case ABILITY_CUD_CHEW:", "Cud Chew end-turn activation")
    cud_script = block("data/battle_scripts_1.s", "BattleScript_CudChewActivates::", "BattleScript_ToxicDebrisActivates::")
    assert "consumeberry BS_SCRIPTING, TRUE" in cud_script
    assert commands.count("if (gDisableStructs[gActiveBattler].cudChewReplaying)") >= 2
    require("include/battle.h", "bool8 cudChewConsumptionContext;", "Cud Chew consumption context")
    require("include/battle.h", "bool8 blockCudChewConsumption;", "Cud Chew temporary-Berry block")
    berry_use = block("src/battle_util.c", "// Berry was successfully used on its holder.", "return effect;")
    assert "!gBattleScripting.blockCudChewConsumption" in berry_use
    assert "gBattleMons[battlerId].item == gLastUsedItem" in berry_use
    remove_item = block("src/battle_script_commands.c", "static void Cmd_removeitem(void)\n{", "static void Cmd_atknameinbuff1")
    assert "gBattleScripting.cudChewConsumptionContext" in remove_item
    assert "gBattleScripting.cudChewConsumptionBattler == gActiveBattler" in remove_item
    consume_berry = block("src/battle_script_commands.c", "case VARIOUS_CONSUME_BERRY:", "case VARIOUS_JUMP_IF_CANT_REVERT_TO_PRIMAL")
    assert "blockCudChewConsumption = gBattlescriptCurrInstr[3]" in consume_berry
    assert consume_berry.count("blockCudChewConsumption = FALSE") >= 2
    natural_gift = block("data/battle_scripts_1.s", "BattleScript_EffectNaturalGift:", "BattleScript_MakeMoveMissed::")
    assert "removeitem BS_ATTACKER" in natural_gift and "consumeberry" not in natural_gift
    destroyed_berries = block("src/battle_script_commands.c", "case MOVE_EFFECT_INCINERATE:", "case MOVE_EFFECT_RELIC_SONG:")
    assert "cudChewConsumptionContext" not in destroyed_berries
    assert "gBattleMons[gEffectBattler].item = 0" in destroyed_berries

    toxic = block("src/battle_util.c", "case ABILITY_TOXIC_DEBRIS:", "case ABILITY_BERSERK:")
    assert "IS_MOVE_PHYSICAL(gCurrentMove)" in toxic
    assert "toxicSpikesAmount < 2" in toxic
    assert "toxicSpikesAmount++" in toxic

    drive = block("src/battle_util.c", "bool32 IsCuratedDriveStatBoosted", "static u32 CountCuratedFaintedAllies")
    assert "ability != ABILITY_PROTOSYNTHESIS && ability != ABILITY_QUARK_DRIVE" in drive
    assert "gBattleMons[battlerId].item == ITEM_BOOSTER_ENERGY" in drive

    restricted_abilities = (
        "ABILITY_ZERO_TO_HERO",
        "ABILITY_COMMANDER",
        "ABILITY_PROTOSYNTHESIS",
        "ABILITY_QUARK_DRIVE",
    )
    for start, end, label in (
        ("static const u16 sSkillSwapBannedAbilities[]", "static const u16 sRolePlayBannedAbilities[]", "Skill Swap"),
        ("static const u16 sRolePlayBannedAbilities[]", "static const u16 sRolePlayBannedAttackerAbilities[]", "Role Play copy"),
        ("static const u16 sEntrainmentBannedAttackerAbilities[]", "static const u16 sEntrainmentTargetSimpleBeamBannedAbilities[]", "Entrainment copy"),
        ("static const u8 sAbilitiesNotTraced", "static const u8 sHoldEffectToType", "Trace"),
    ):
        restriction_block = block("src/battle_util.c", start, end)
        for ability in restricted_abilities:
            assert ability in restriction_block, f"{label} does not restrict {ability}"
    receiver = block("src/battle_script_commands.c", "case VARIOUS_TRY_ACTIVATE_RECEIVER", "case VARIOUS_TRY_ACTIVATE_BEAST_BOOST")
    for ability in restricted_abilities:
        assert ability in receiver, f"Receiver can copy {ability}"
    wandering = block("src/battle_util.c", "case ABILITY_WANDERING_SPIRIT:", "case ABILITY_ANGER_POINT:")
    for ability in restricted_abilities:
        assert ability in wandering, f"Wandering Spirit can swap {ability}"

    for start, end, label in (
        ("static const u16 sRolePlayBannedAttackerAbilities[]", "static const u16 sWorrySeedBannedAbilities[]", "Role Play overwrite"),
        ("static const u16 sWorrySeedBannedAbilities[]", "static const u16 sGastroAcidBannedAbilities[]", "Worry Seed"),
        ("static const u16 sGastroAcidBannedAbilities[]", "static const u16 sEntrainmentBannedAttackerAbilities[]", "Gastro Acid"),
        ("static const u16 sEntrainmentTargetSimpleBeamBannedAbilities[]", "static const u16 sTwoStrikeMoves[]", "Simple Beam/Entrainment overwrite"),
    ):
        assert "ABILITY_ZERO_TO_HERO" in block("src/battle_util.c", start, end), f"{label} can overwrite Zero to Hero"
    neutral_gas = block("src/battle_util.c", "bool32 IsNeutralizingGasBannedAbility", "bool32 IsNeutralizingGasOnField")
    assert "ABILITY_ZERO_TO_HERO" in neutral_gas
    mummy = block("src/battle_util.c", "case ABILITY_MUMMY:", "case ABILITY_WANDERING_SPIRIT:")
    assert "ABILITY_ZERO_TO_HERO" in mummy
    imposter = block("src/battle_util.c", "case ABILITY_IMPOSTER:", "case ABILITY_MOLD_BREAKER:")
    for ability in ("ABILITY_ZERO_TO_HERO", "ABILITY_PROTOSYNTHESIS", "ABILITY_QUARK_DRIVE"):
        assert ability in imposter, f"Imposter can transform into {ability}"

    require("src/battle_ai_main.c", "case ABILITY_GOOD_AS_GOLD:", "Good as Gold AI immunity")
    require("src/battle_ai_main.c", "AI_DATA->defAbility == ABILITY_GOOD_AS_GOLD", "Good as Gold ally-target AI immunity")
    require("src/battle_util.c", "gBattlerAttacker != battler", "Good as Gold blocks other battlers")
    require("src/battle_ai_switch_items.c", "static bool8 ShouldSwitchIfZeroToHero(void)", "Zero to Hero AI switch")
    require("src/battle_ai_switch_items.c", "gBattleMons[gActiveBattler].species != SPECIES_PALAFIN", "Zero-form Palafin gate")
    mold_breaker = block(
        "src/battle_util.c",
        "static const u8 sAbilitiesAffectedByMoldBreaker[ABILITIES_COUNT]",
        "static const u8 sAbilitiesNotTraced",
    )
    assert "sAbilitiesAffectedByMoldBreaker[ABILITIES_COUNT]" in mold_breaker
    for ability in (
        "ABILITY_ARMOR_TAIL",
        "ABILITY_BEADS_OF_RUIN",
        "ABILITY_GOOD_AS_GOLD",
        "ABILITY_PURIFYING_SALT",
        "ABILITY_SWORD_OF_RUIN",
        "ABILITY_VESSEL_OF_RUIN",
    ):
        assert ability in mold_breaker, f"Mold Breaker does not bypass {ability}"
    for ability, rating in {
        "ABILITY_ARMOR_TAIL": 5,
        "ABILITY_BEADS_OF_RUIN": 5,
        "ABILITY_COMMANDER": 10,
        "ABILITY_CUD_CHEW": 4,
        "ABILITY_GOOD_AS_GOLD": 8,
        "ABILITY_PROTOSYNTHESIS": 7,
        "ABILITY_PURIFYING_SALT": 6,
        "ABILITY_QUARK_DRIVE": 7,
        "ABILITY_SUPREME_OVERLORD": 6,
        "ABILITY_SWORD_OF_RUIN": 5,
        "ABILITY_TOXIC_DEBRIS": 4,
        "ABILITY_VESSEL_OF_RUIN": 5,
        "ABILITY_ZERO_TO_HERO": 10,
    }.items():
        assert f"[{ability}] = {rating}," in ai_util, f"missing AI rating for {ability}"
    switchout_abilities = block(
        "src/battle_script_commands.c",
        "static void Cmd_switchoutabilities(void)\n{",
        "static void Cmd_jumpifhasnohp",
    )
    zero_to_hero = switchout_abilities[switchout_abilities.index("case ABILITY_ZERO_TO_HERO:"):]
    assert "SetMonData(partyMon, MON_DATA_SPECIES, &heroSpecies)" in zero_to_hero
    assert "CalculateMonStats(partyMon)" in zero_to_hero
    hero_stats = block(
        "src/data/pokemon/verdant_gen9_base_stats.h", "[SPECIES_PALAFIN_HERO]", "[SPECIES_DONDOZO]"
    )
    for fragment in (".baseAttack = 160", ".baseDefense = 97", ".baseSpAttack = 106", ".baseSpDefense = 87"):
        assert fragment in hero_stats, f"Palafin Hero missing {fragment!r}"
    undo_form = block("src/battle_util.c", "void UndoFormChange", "bool32 DoBattlersShareType")
    assert "{SPECIES_PALAFIN_HERO,         SPECIES_PALAFIN,              FALSE}" in undo_form
    assert "CalculateMonStats(&party[monId])" in undo_form

    item_lock = block("src/battle_util.c", "bool32 CanBattlerGetOrLoseItem", "struct Pokemon *GetIllusionMonPtr")
    assert "GET_BASE_SPECIES_ID(species) == SPECIES_OGERPON" in item_lock
    for mask in ("ITEM_WELLSPRING_MASK", "ITEM_HEARTHFLAME_MASK", "ITEM_CORNERSTONE_MASK"):
        assert mask in item_lock, f"Ogerpon item lock missing {mask}"

    require("include/battle.h", "u8 saltCure:1;", "Salt Cure state")
    require("src/battle_util.c", "ENDTURN_SALT_CURE", "Salt Cure independent end-turn phase")
    salt_turn = block("src/battle_util.c", "case ENDTURN_SALT_CURE:", "case ENDTURN_WRAP:")
    assert "TYPE_WATER" in salt_turn and "TYPE_STEEL" in salt_turn
    assert "maxHP / 4" in salt_turn and "maxHP / 8" in salt_turn
    salt_apply = block("src/battle_script_commands.c", "if (gCurrentMove == MOVE_SALT_CURE)", "else if (gBattleMons[gEffectBattler].status2 & STATUS2_WRAPPED)")
    assert "saltCure = TRUE" in salt_apply
    assert "wrappedMove" not in salt_apply
    require("src/battle_anim_throw.c", "animationData->animArg == MOVE_SALT_CURE", "Salt Cure residual animation")

    check_description_width("src/data/text/verdant_gen9_ability_descriptions.h", 30)
    check_description_width("src/data/text/verdant_gen9_move_description_strings.h", 30)
    print("Verdant curated Gen 9 battle mechanics: PASS")


if __name__ == "__main__":
    main()
