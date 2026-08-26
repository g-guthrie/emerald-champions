#!/usr/bin/env python3
"""Static coverage and regression gates for Verdant's usable battle mechanics."""

from pathlib import Path
import re
import sys


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


def require(condition: bool, label: str) -> None:
    global checks
    checks += 1
    if not condition:
        failures.append(label)


def blocks(text: str, prefix: str) -> dict[str, str]:
    pattern = re.compile(rf"^\s*\[({prefix}_[A-Z0-9_]+)\]\s*=\s*\{{", re.M)
    matches = list(pattern.finditer(text))
    return {
        match.group(1): text[match.end() : matches[i + 1].start() if i + 1 < len(matches) else len(text)]
        for i, match in enumerate(matches)
    }


def strip_comments(text: str) -> str:
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    text = re.sub(r"//[^\n]*", "", text)
    return re.sub(r"@[^\n]*", "", text)


failures: list[str] = []
checks = 0

learnset_files = (
    "src/data/pokemon/level_up_learnsets.h",
    "src/data/pokemon/tmhm_learnsets.h",
    "src/data/pokemon/tutor_learnsets.h",
    "src/data/pokemon/egg_moves.h",
    "src/data/pokemon/verdant_gen9_level_up_learnsets.h",
    "src/data/pokemon/verdant_gen9_tmhm_learnsets.h",
    "src/data/pokemon/verdant_gen9_tutor_learnsets.h",
    "src/data/pokemon/verdant_gen9_egg_moves.h",
)
learnset_text = "\n".join(read(path) for path in learnset_files)
available_moves = set(re.findall(r"(?<![A-Z0-9_])MOVE_[A-Z0-9_]+", learnset_text))
trainer_move_tokens = set(re.findall(r"\bMOVE_[A-Z0-9_]+\b", read("src/data/trainer_parties.h")))
script_move_tokens: set[str] = set()
for path in (ROOT / "data").rglob("*"):
    if path.is_file() and path.suffix in {".inc", ".s"}:
        script_move_tokens.update(re.findall(r"\bMOVE_[A-Z0-9_]+\b", path.read_text(errors="ignore")))

move_data_text = read("src/data/battle_moves.h") + read("src/data/verdant_gen9_battle_moves.h")
move_blocks = blocks(move_data_text, "MOVE")
available_moves.update(
    move
    for move in trainer_move_tokens | script_move_tokens
    if move != "MOVE_NONE" and move in move_blocks
)
require(available_moves <= move_blocks.keys(), "every player-, trainer-, or script-usable move has battle data")
require(
    not [move for move in available_moves if "EFFECT_PLACEHOLDER" in move_blocks[move]],
    "no player-, trainer-, or script-usable move uses EFFECT_PLACEHOLDER",
)

name_text = "\n".join(
    read(path)
    for path in (
        "src/data/text/move_names.h",
        "src/data/text/verdant_gen9_move_names_short.h",
        "src/data/text/verdant_gen9_move_names_long.h",
    )
)
named_moves = set(re.findall(r"\[(MOVE_[A-Z0-9_]+)\]", name_text))
require(available_moves <= named_moves, "every player-, trainer-, or script-usable move has a displayed name")

description_text = read("src/data/text/move_descriptions.h") + read("src/data/text/verdant_gen9_move_description_pointers.h")
described_moves = set(re.findall(r"\[(MOVE_[A-Z0-9_]+)\s*-\s*1\]", description_text))
require(available_moves <= described_moves, "every player-, trainer-, or script-usable move has a description")

effect_constants = read("include/constants/battle_move_effects.h")
effect_count = int(re.search(r"#define\s+NUM_BATTLE_MOVE_EFFECTS\s+(\d+)", effect_constants).group(1))
effect_definitions = [
    (name, int(value))
    for name, value in re.findall(r"^#define\s+(EFFECT_[A-Z0-9_]+)\s+(\d+)\s*$", effect_constants, re.M)
]
require(
    [value for _, value in effect_definitions] == list(range(effect_count)),
    "move effect constants are contiguous and end immediately before NUM_BATTLE_MOVE_EFFECTS",
)
script_table = read("data/battle_scripts_1.s").split("gBattleScriptsForMoveEffects::", 1)[1]
script_table = script_table.split("BattleScript_BeakBlastSetUp::", 1)[0]
require(len(re.findall(r"^\s*\.4byte\s+", script_table, re.M)) == effect_count,
        "every move effect has a script-table entry")
dispatch_effects = re.findall(r"^\s*\.4byte\s+[^@\n]+@\s*(EFFECT_[A-Z0-9_]+)\s*$", script_table, re.M)
require(
    dispatch_effects == [name for name, _ in effect_definitions],
    "move effect script-table comments align exactly with the effect constants",
)

base_stats = read("src/data/pokemon/base_stats.h") + read("src/data/pokemon/verdant_gen9_base_stats.h")
species_abilities = set(re.findall(r"ABILITY_[A-Z0-9_]+", base_stats))
ability_text = read("src/data/text/abilities.h") + read("src/data/text/verdant_gen9_ability_pointers.h")
text_abilities = set(re.findall(r"\[(ABILITY_[A-Z0-9_]+)\]", ability_text))
require(species_abilities <= text_abilities, "every species-used ability has display text")

runtime_text = strip_comments(
    "\n".join(path.read_text(errors="ignore") for path in (ROOT / "src").glob("*.c"))
    + read("data/battle_scripts_1.s")
)
require(not [ability for ability in species_abilities if ability != "ABILITY_NONE" and ability not in runtime_text],
        "every species-used ability has a runtime path")

used_hold_effects = set(re.findall(r"HOLD_EFFECT_[A-Z0-9_]+", read("src/data/items.h")))
hold_effect_exemptions = {"HOLD_EFFECT_RUSTED_SHIELD", "HOLD_EFFECT_RUSTED_SWORD"}
require(not [effect for effect in used_hold_effects - hold_effect_exemptions if effect not in runtime_text],
        "every battle-relevant hold effect has a runtime path")


def move_has(move: str, fragment: str) -> bool:
    return fragment in move_blocks[move]


target_expectations = {
    "MOVE_AVALANCHE": "MOVE_TARGET_SELECTED",
    "MOVE_OMINOUS_WIND": "MOVE_TARGET_SELECTED",
    "MOVE_CLANGING_SCALES": "MOVE_TARGET_BOTH",
    "MOVE_SHELL_TRAP": "MOVE_TARGET_BOTH",
    "MOVE_HEAL_BLOCK": "MOVE_TARGET_BOTH",
}
for move, target in target_expectations.items():
    require(move_has(move, f".target = {target}"), f"{move} has its native doubles target")

flag_expectations = {
    "MOVE_ROUND": "FLAG_SOUND",
    "MOVE_RAZOR_LEAF": "FLAG_KEEN_EDGE_BOOST",
    "MOVE_AIR_CUTTER": "FLAG_KEEN_EDGE_BOOST",
    "MOVE_TRICK_OR_TREAT": "FLAG_MAGIC_COAT_AFFECTED",
    "MOVE_FORESTS_CURSE": "FLAG_MAGIC_COAT_AFFECTED",
}
for move, flag in flag_expectations.items():
    require(move_has(move, flag), f"{move} includes {flag}")

for move, forbidden_flag in {
    "MOVE_NATURES_MADNESS": "FLAG_MAKES_CONTACT",
    "MOVE_QUASH": "FLAG_MAGIC_COAT_AFFECTED",
    "MOVE_REFLECT_TYPE": "FLAG_SNATCH_AFFECTED",
    "MOVE_SOFT_BOILED": "FLAG_MIRROR_MOVE_AFFECTED",
    "MOVE_GRUDGE": "FLAG_MIRROR_MOVE_AFFECTED",
    "MOVE_SNATCH": "FLAG_MIRROR_MOVE_AFFECTED",
    "MOVE_ORDER_UP": "FLAG_MIRROR_MOVE_AFFECTED",
}.items():
    require(not move_has(move, forbidden_flag), f"{move} excludes {forbidden_flag}")

for move in (
    "MOVE_SOFT_BOILED", "MOVE_FOLLOW_ME", "MOVE_CHARGE", "MOVE_ASSIST", "MOVE_INGRAIN",
    "MOVE_MAGIC_COAT", "MOVE_RECYCLE", "MOVE_IMPRISON", "MOVE_REFRESH", "MOVE_GRUDGE",
    "MOVE_SNATCH", "MOVE_CAMOUFLAGE", "MOVE_MUD_SPORT", "MOVE_SLACK_OFF",
):
    require(move_has(move, ".accuracy = 0"), f"{move} displays non-applicable accuracy")

main = read("src/battle_main.c")
triage = main[main.index("else if (GetBattlerAbility(battlerId) == ABILITY_TRIAGE") : main.index("else if (GetBattlerAbility(battlerId) == ABILITY_BLITZ_BOXER")]
for effect in ("EFFECT_STRENGTH_SAP", "EFFECT_SHORE_UP", "EFFECT_PURIFY", "EFFECT_JUNGLE_HEALING"):
    require(effect in triage, f"Triage recognizes {effect}")

first_start = main.rindex("u8 GetWhoStrikesFirst")
first = main[first_start : main.index("static void SetActionsAndBattlersTurnOrder", first_start)]
roller = main[main.index("static void RollQuickMoveOrderEffects") : main.index("u8 GetWhoStrikesFirst")]
require("Random() % 100" not in first and "gRandomTurnNumber <" not in first,
        "turn-order comparator does not reroll quick effects")
require(main.count("RollQuickMoveOrderEffects();") == 1, "quick effects roll once before sorting")
require(first.count("gProtectStructs[battler1].quickDraw || gProtectStructs[battler1].usedCustapBerry") == 2
        and first.count("gProtectStructs[battler2].quickDraw || gProtectStructs[battler2].usedCustapBerry") == 2,
        "Quick Draw, Quick Claw, and Custap share one forced-first bracket")
turn_order_helpers = main[main.index("static bool32 IsTurnOrderBerryBlockedByUnnerve") : main.index("u8 GetWhoStrikesFirst")]
require(all(ability in turn_order_helpers for ability in ("ABILITY_UNNERVE", "ABILITY_AS_ONE_ICE_RIDER", "ABILITY_AS_ONE_SHADOW_RIDER")),
        "Custap recognizes Unnerve and both As One abilities")

commands = read("src/battle_script_commands.c")
conversion_start = commands.rindex("static void Cmd_settypetorandomresistance")
conversion = commands[conversion_start : commands.index("static void Cmd_setalwayshitflag", conversion_start)]
require("gLastResultingMoves[gBattlerTarget]" in conversion and "gLastUsedMoveType[gBattlerTarget]" in conversion,
        "Gen 5+ Conversion 2 uses the selected target's actual last move type")
require("gLastUsedMoveType[MAX_BATTLERS_COUNT]" in main and "GET_MOVE_TYPE(gCurrentMove, gLastUsedMoveType" in commands,
        "actual last-used move types are recorded")
require("MOVE_CLANGING_SCALES && baseMoveEffect == MOVE_EFFECT_DEF_MINUS_1" in commands
        and "spreadMoveStatDropped" in commands,
        "Clanging Scales drops Defense only once across both targets")

utility = read("src/battle_util.c")
mimicry = utility[utility.index("void TryToApplyMimicry") : utility.index("void TryToRevertMimicry")]
require("switch (gFieldStatuses & STATUS_FIELD_TERRAIN_ANY)" in mimicry, "Mimicry masks unrelated field bits")
require("GET_MOVE_TYPE" not in mimicry and "u32 moveType, move" not in mimicry, "Mimicry has no uninitialized move read")

bad_dreams = utility[utility.index("case ABILITY_BAD_DREAMS:") : utility.index("SOLAR_POWER_HP_DROP:")]
require("for (i = 0; i < gBattlersCount; i++)" in bad_dreams and "GetBattlerSide(i) != GetBattlerSide(battler)" in bad_dreams,
        "Bad Dreams scans both opposing slots")

ball_fetch = utility[utility.index("case ABILITY_BALL_FETCH:") : utility.index("case ABILITY_HUNGER_SWITCH:")]
require("gLastUsedBall >= ITEM_ULTRA_BALL" in ball_fetch and "gLastUsedBall <= LAST_BALL_INDEX" in ball_fetch,
        "Ball Fetch bounds-checks catch-attempt indexing")
require("gBattleMons[battler].item = gLastUsedItem" in ball_fetch and "~RESOURCE_FLAG_UNBURDEN" in ball_fetch,
        "Ball Fetch synchronizes live item and Unburden state")

for move in (
    "MOVE_DRAGON_ENERGY", "MOVE_JET_PUNCH", "MOVE_MAKE_IT_RAIN", "MOVE_ORDER_UP", "MOVE_RAGE_FIST",
    "MOVE_RAGING_FURY", "MOVE_RUINATION", "MOVE_SALT_CURE", "MOVE_SNOWSCAPE", "MOVE_TWIN_BEAM",
):
    require(re.search(rf"\[{move}\]\s*=\s*[^\n]*FORBIDDEN_METRONOME", commands) is not None,
            f"Metronome excludes {move}")

require("[MOVE_BURNING_BULWARK] = FORBIDDEN_ASSIST | FORBIDDEN_COPYCAT" in commands,
        "Assist and Copycat exclude Burning Bulwark")
for move in ("MOVE_METRONOME", "MOVE_MIRROR_MOVE", "MOVE_SLEEP_TALK", "MOVE_NATURE_POWER", "MOVE_ASSIST", "MOVE_ME_FIRST", "MOVE_COPYCAT", "MOVE_BELCH", "MOVE_DYNAMAX_CANNON"):
    line = re.search(rf"\[{move}\]\s*=\s*([^\n]+)", commands)
    require(line is not None and "FORBIDDEN_MIMIC" in line.group(1), f"Mimic excludes {move}")

require(all(f"gLastMoves[gBattlerTarget] == {move}" in commands for move in ("MOVE_ASSIST", "MOVE_DYNAMAX_CANNON", "MOVE_MIMIC", "MOVE_SKETCH", "MOVE_TRANSFORM")),
        "Encore excludes all non-repeatable move callers")
require(all(f"gLastMoves[gBattlerTarget] == {move}" in commands for move in ("MOVE_CHATTER", "MOVE_DYNAMAX_CANNON", "MOVE_OBSTRUCT", "MOVE_SHELL_TRAP")),
        "Instruct excludes its identity-specific banned moves")
instruct = commands[commands.index("case VARIOUS_TRY_INSTRUCT:") : commands.index("case VARIOUS_ABILITY_POPUP:")]
require(instruct.index("gLastMoves[gBattlerTarget] != 0xFFFF")
        < instruct.index("gBattleMoves[gLastMoves[gBattlerTarget]].effect"),
        "Instruct validates its last-move index before move-data access")
mimic_start = commands.rindex("static void Cmd_mimicattackcopy")
mimic = commands[mimic_start : commands.index("static void Cmd_metronome", mimic_start)]
require(mimic.index("gLastMoves[gBattlerTarget] == 0xFFFF")
        < mimic.index("sForbiddenMoves[gLastMoves[gBattlerTarget]]"),
        "Mimic validates its last-move index before restriction-table access")
require(all(f"case {move}:" in commands for move in ("MOVE_BELCH", "MOVE_BEAK_BLAST", "MOVE_SHELL_TRAP")),
        "Me First excludes Belch, Beak Blast, and Shell Trap")
require("gLastPrintedMoves[gBattlerTarget] != MOVE_CHATTER" in commands, "Sketch excludes Chatter")

print(
    f"mechanics completeness: {checks - len(failures)}/{checks} checks passed; "
    f"{len(available_moves)} usable moves, {len(move_blocks)} move data entries, "
    f"{effect_count} move effects, {len(species_abilities)} species abilities, "
    f"{len(used_hold_effects)} hold effects"
)
if failures:
    for failure in failures:
        print(f"FAIL: {failure}")
    sys.exit(1)
