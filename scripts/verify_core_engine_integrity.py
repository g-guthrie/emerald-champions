#!/usr/bin/env python3
"""Focused semantic invariants for Verdant's core-engine hardening pass.

This deliberately checks relationships and ordering, not line numbers or whole
function snapshots.  If a mechanic is redesigned, update the invariant only
after reviewing the new runtime contract.
"""

from pathlib import Path
import re
import sys


ROOT = Path(__file__).resolve().parents[1]
checks = 0
failures: list[str] = []


def read(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


def require(condition: bool, label: str) -> None:
    global checks
    checks += 1
    if condition:
        print(f"PASS: {label}")
    else:
        failures.append(label)
        print(f"FAIL: {label}")


def function(text: str, name: str) -> str:
    return_type = r"(?:void|bool8|bool16|bool32|u8|u16|u32|s8|s16|s32|int)"
    match = re.search(
        rf"^(?:static\s+)?{return_type}\s+{name}\s*\([^;{{}}]*?\)"
        rf"\s*(?://[^\n]*)?\s*\{{",
        text,
        re.M | re.S,
    )
    if not match:
        return ""
    depth = 1
    pos = match.end()
    while pos < len(text) and depth:
        if text[pos] == "{":
            depth += 1
        elif text[pos] == "}":
            depth -= 1
        pos += 1
    return text[match.start():pos]


def move_block(text: str, move: str) -> str:
    start = text.find(f"[{move}] =")
    if start < 0:
        return ""
    next_move = text.find("\n    [MOVE_", start + 1)
    return text[start: next_move if next_move >= 0 else len(text)]


config = read("include/constants/battle_config.h")
main = read("src/battle_main.c")
commands = read("src/battle_script_commands.c")
utility = read("src/battle_util.c")
ai_main = read("src/battle_ai_main.c")
ai_utility = read("src/battle_ai_util.c")
moves = read("src/data/battle_moves.h")
scripts = read("data/battle_scripts_1.s")
pokemon = read("src/pokemon.c")
daycare = read("src/daycare.c")
pyramid = read("src/battle_pyramid.c")
tower = read("src/battle_tower.c")
anim = read("src/battle_anim.c")
anim_effects = read("src/battle_anim_effects_1.c")
anim_water = read("src/battle_anim_water.c")
util = read("src/util.c")
decompress = read("src/decompress.c")
party_menu = read("src/party_menu.c")
opponent = read("src/battle_controller_opponent.c")
recorded = read("src/recorded_battle.c")

# Generation-policy consistency.
require(re.search(r"#define\s+B_ABILITY_WEATHER\s+GEN_8\b", config) is not None,
        "ability-created weather uses the finite modern duration")
weather = function(utility, "TryChangeBattleWeather")
require("B_ABILITY_WEATHER <= GEN_5" in weather and "gWishFutureKnock.weatherDuration = 5" in weather,
        "weather runtime separates legacy permanence from the five-turn path")

# Locked moves and Gen 8 Rapid Spin / Sheer Force.
priority = function(main, "GetChosenMovePriority")
require("STATUS2_MULTIPLETURNS | STATUS2_RECHARGE" in priority
        and "move = gLockedMoves[battlerId]" in priority,
        "locked Bide and other forced moves retain their own priority")
rapid_spin = move_block(moves, "MOVE_RAPID_SPIN")
require("FLAG_SHEER_FORCE_BOOST" in rapid_spin,
        "Rapid Spin is recognized as Sheer Force boosted in Gen 8")
rapid_script = scripts[scripts.index("BattleScript_MortalSpinRapidSpin:"):
                       scripts.index("BattleScript_EffectRapidSpinEnd::")]
require(rapid_script.index("seteffectwithchance")
        < rapid_script.index("jumpifability BS_ATTACKER, ABILITY_SHEER_FORCE")
        < rapid_script.index("setstatchanger STAT_SPEED"),
        "Sheer Force preserves Rapid Spin cleanup but suppresses its Speed raise")

# Compiler-discovered state and indexing defects.
set_effect = function(commands, "SetMoveEffect")
require(re.search(r"\bbyTwo\s*=\s*0\b", set_effect) is not None,
        "Spectral Thief animation aggregation starts initialized")
require("!gBattleMons[gBattlerTarget].status2 & STATUS2_ESCAPE_PREVENTION" not in commands
        and "!(gBattleMons[gBattlerTarget].status2 & STATUS2_ESCAPE_PREVENTION)" in commands,
        "two-sided trapping records the target's trapping battler")
reverse_stats = function(commands, "ReverseStatChangeMoveEffect")
require(re.search(r"case\s+MOVE_EFFECT_EVS_MINUS_1:\s*return\s+MOVE_EFFECT_EVS_PLUS_1", reverse_stats, re.S) is not None
        and "return moveEffect;" in reverse_stats,
        "Contrary reverses Evasion -1 correctly and has a defined fallback")
mirror = function(commands, "Cmd_trymirrormove")
require("validMovesCount < ARRAY_COUNT(movesArray)" in mirror
        and "move < MOVES_COUNT" in mirror,
        "Mirror Move bounds both its candidate array and move IDs")
nature_power = function(commands, "GetNaturePowerMove")
require("gBattleTerrain >= ARRAY_COUNT(sNaturePowerMoves)" in nature_power
        and "sNaturePowerMoves[gBattleTerrain] == MOVE_NONE" in nature_power,
        "Nature Power falls back safely for unsupported terrain entries")

known_move = function(ai_utility, "RecordKnownMove")
require("battlerId >= gBattlersCount" in known_move and "move >= MOVES_COUNT" in known_move,
        "AI history rejects invalid battlers and sentinel move IDs")
can_move_faint = function(ai_utility, "CanMoveFaintBattler")
require("for (i = 0; i < MAX_MON_MOVES; i++)" in can_move_faint
        and "gBattleMons[battlerDef].moves[i] != move" in can_move_faint
        and "move >= MOVES_COUNT" in can_move_faint,
        "Disable scoring resolves a real usable move slot before damage calculation")
can_target_faint = function(ai_utility, "CanTargetFaintAiWithMod")
require(can_target_faint.index("moves[i] >= MOVES_COUNT")
        < can_target_faint.index("AI_CalcDamage(moves[i]"),
        "AI damage simulation validates history moves before indexing move data")
accuracy = function(ai_utility, "AI_GetMoveAccuracy")
require("atkParam = GetBattlerHoldEffectParam(battlerAtk)" in accuracy
        and "defParam = GetBattlerHoldEffectParam(battlerDef)" in accuracy,
        "AI accuracy item modifiers use initialized parameters")
require("*(score)++" not in ai_utility and "*(score)--" not in ai_utility,
        "AI score helpers increment values rather than walking their score pointer")
require("gBattleScripting.statChanger = SET_STATCHANGER" not in utility,
        "held-item stat boosts do not modify statChanger twice in one expression")

psycho = ai_main[ai_main.index("case EFFECT_PSYCHO_SHIFT:"):
                 ai_main.index("case EFFECT_MUD_SPORT:")]
require(psycho.count("status1 &") >= 4 and psycho.count("score -= 10") == 5
        and re.search(r"status1 & STATUS1_PSN_ANY\)\s*\{", psycho) is not None,
        "Psycho Shift is not discouraged after a valid transferable status")
require("u16 partnerAbility = AI_DATA->atkPartnerAbility;" in ai_main,
        "curated ability IDs are not truncated in combo AI")
require(ai_main.count("if (predictedMove >= MOVES_COUNT)") >= 3,
        "all major AI scoring passes normalize sentinel last-move IDs")

# Items, capture, and EV/Pickup paths.
ball_switch = commands[commands.index("switch (gLastUsedItem)"):
                       commands.index("case ITEM_DIVE_BALL:")]
require(re.search(r"case ITEM_ULTRA_BALL:\s*ballMultiplier = 20;\s*break;", ball_switch, re.S) is not None,
        "Ultra Ball cannot fall through into another ball's multiplier")
require(re.search(r"case ITEM_GREAT_BALL:.*?ballMultiplier = 15;\s*break;", ball_switch, re.S) is not None,
        "Great, Safari, and Sport Balls cannot fall through into Net Ball logic")
require("gLocalTime.hours >= 20 || gLocalTime.hours <= 3" in commands,
        "Dusk Ball's overnight interval is reachable")
form_change = commands[commands.index("case VARIOUS_HANDLE_FORM_CHANGE:"):
                       commands.index("case VARIOUS_TRY_LAST_RESORT:")]
require("if (!gBattleTextBuff1)" not in form_change
        and "PREPARE_SPECIES_BUFFER(gBattleTextBuff1" in form_change,
        "form-change messages always prepare the new species name")
pickup = function(commands, "Cmd_pickup")
pyramid_branch = pickup[pickup.index("else if (InBattlePyramid())"):pickup.index("else\n    {", pickup.index("else if (InBattlePyramid())"))]
require(pyramid_branch.index("lvlDivBy10 =") < pyramid_branch.index("ABILITY_HONEY_GATHER"),
        "Battle Pyramid Honey Gather initializes its level bucket")
ev_gain = function(pokemon, "MonGainEVs")
require(ev_gain.count("heldItem = GetMonData") == 1
        and "powerItemStat = ItemId_GetSecondaryId(heldItem)" in ev_gain,
        "EV gain has one canonical held-item path and an initialized Power Item stat")

# Egg-move legality and weather-dependent forms.
egg_query = function(daycare, "SpeciesCanLearnEggMove")
require("GetEggSpecies(GET_BASE_SPECIES_ID(species))" in egg_query,
        "evolved species inherit their base lineage's egg-move query")
weather_form = function(utility, "TryWeatherFormChange")
require("GetBattlerAbility(battler) != ABILITY_FLOWER_GIFT" in weather_form
        and "gBattleMons[battler].species = SPECIES_CHERRIM" in weather_form,
        "Cherrim reverts when Flower Gift is removed or suppressed")
forecast = utility[utility.index("case ABILITYEFFECT_FORECAST:"):
                   utility.index("case ABILITYEFFECT_SYNCHRONIZE:")]
require("baseSpecies == SPECIES_CASTFORM || baseSpecies == SPECIES_CHERRIM" in forecast,
        "weather-form reevaluation includes suppressed Cherrim and Castform")
for label in (
    "BattleScript_EffectEntrainment:", "BattleScript_EffectSimpleBeam:",
    "BattleScript_EffectWorrySeed:", "BattleScript_EffectGastroAcid:",
    "BattleScript_EffectRolePlay::", "BattleScript_EffectSkillSwap:",
    "BattleScript_MummyActivates::", "BattleScript_WanderingSpiritActivates::",
):
    start = scripts.index(label)
    next_label = scripts.find("\nBattleScript_", start + len(label))
    require("BattleScript_WeatherFormChanges" in scripts[start:next_label],
            f"{label.rstrip(':')} reevaluates weather forms after ability changes")

# Animation and palette memory safety.
load_pal = function(anim, "TryLoadBattleAnimPalette")
require("index >= ARRAY_COUNT(gBattleAnimPaletteTable)" in load_pal
        and "IndexOfSpritePaletteTag(tag) != 0xFF" in load_pal,
        "battle animation palette loading validates tags and table bounds")
require("TryLoadBattleAnimPalette(ANIM_TAG_LEAF)" in anim_effects
        and "TryLoadBattleAnimPalette(ANIM_TAG_RAZOR_LEAF)" in anim_effects
        and "TryLoadBattleAnimPalette(ANIM_TAG_SLASH)" in anim_effects,
        "Magical Leaf and Night Slash load palettes before lookup")
require("TryLoadBattleAnimPalette(ANIM_TAG_RAINBOW_RINGS)" in anim_water,
        "Aurora Beam loads its rotating palette before lookup")
leaf_blade = function(anim_effects, "AnimTask_LeafBlade")
require(re.search(r"task->data\[2\] == MAX_SPRITES\)\s*\{.*?DestroyAnimVisualTask\(taskId\);\s*return;", leaf_blade, re.S) is not None,
        "Leaf Blade never indexes the MAX_SPRITES failure sentinel")
blend = function(util, "BlendPalette")
require("palOffset >= PLTT_BUFFER_SIZE" in blend
        and "numEntries > PLTT_BUFFER_SIZE - palOffset" in blend,
        "BlendPalette contains every caller within the hardware palette buffers")
for loader in ("LoadCompressedSpriteSheetUsingHeap", "LoadCompressedSpritePaletteUsingHeap"):
    body = function(decompress, loader)
    require("if (buffer == NULL)" in body,
            f"{loader} handles allocation failure before decompression")

# Frontier defensive state and bounds.
require("min(gSpecialVar_0x8005, MAX_FRONTIER_PARTY_SIZE)" in function(party_menu, "GetMaxBattleEntries")
        and "min(gSpecialVar_0x8005, MAX_FRONTIER_PARTY_SIZE)" in function(party_menu, "GetMinBattleEntries"),
        "Frontier entry requirements cannot exceed persisted party capacity")
hint = function(pyramid, "ShowPostBattleHintText")
require("id > ARRAY_COUNT(sHintTextTypes)" in hint and "default:" in hint,
        "Pyramid hints reject invalid local IDs and unknown hint states")
speech = function(tower, "FrontierSpeechToString")
require(speech.count("gStringVar4[i] != EOS") >= 2,
        "Frontier speech formatting cannot scan past a malformed terminator")
sort_speed = function(utility, "SortBattlersBySpeed")
require("battlerCount = min(gBattlersCount, MAX_BATTLERS_COUNT)" in sort_speed
        and "speeds[MAX_BATTLERS_COUNT]" in sort_speed,
        "speed sorting cannot overrun its battler array")
fainted = function(utility, "HandleFaintedMonActions")
require("default:" in fainted and "faintedActionsState = FAINTED_ACTIONS_MAX_CASE" in fainted,
        "invalid faint-action state exits instead of spinning forever")
forewarn = function(utility, "ForewarnChooseMove")
require("if (data == NULL)" in forewarn and "if (count == 0)" in forewarn
        and "moveId" in forewarn and "MOVES_COUNT" in forewarn,
        "Forewarn handles allocation failure, empty candidates, and invalid moves")

# Current applicability of selected live Expansion reports.
ai_control = function(ai_utility, "IsBattlerAIControlled")
require("B_POSITION_PLAYER_RIGHT" in ai_control and "BATTLE_TYPE_INGAME_PARTNER" in ai_control,
        "recorded in-game partner AI ownership follows battle position, not controller type")
require("sSavedPlayerParty[i]" in recorded and "sSavedOpponentParty[i]" in recorded,
        "recorded battles serialize the pre-battle party snapshots")
recorded_moves = function(recorded, "sub_818603C")
require("if (k == MAX_MON_MOVES)" in recorded_moves
        and "if (array1[j] >= MAX_MON_MOVES)" in recorded_moves,
        "recorded move remapping emits a complete record and validates playback slots")
resist = commands[commands.index("gSpecialStatuses[gBattlerTarget].berryReduced"):
                  commands.index("WEATHER_STRONG_WINDS", commands.index("gSpecialStatuses[gBattlerTarget].berryReduced"))]
require("gLastUsedItem = gBattleMons[gBattlerTarget].item" in resist,
        "resist-Berry text and animation receive the actual consumed item")
mega_sim = function(opponent, "TrySimulateMegaEvolutionForAI")
require("CalculateMonStats(&simulatedMon)" in mega_sim
        and "gBattleMons[gActiveBattler].species = megaSpecies" in mega_sim,
        "opponent move scoring simulates its impending Mega form")

print(f"core engine integrity: {checks - len(failures)}/{checks} checks passed")
if failures:
    for failure in failures:
        print(f" - {failure}")
    sys.exit(1)
