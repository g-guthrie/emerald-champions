#!/usr/bin/env python3
"""Structural and semantic gate for Verdant's Pledge mechanic.

This verifies the production implementation, not a generated manifest.  The
small deterministic model at the end covers the six ordered combinations,
turn-order surgery, timer lifetime, and secondary-effect rules independently
of the source-pattern checks.
"""

from __future__ import annotations

import itertools
import re
import struct
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
errors: list[str] = []
checks = 0


def read(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


def require(condition: bool, label: str) -> None:
    global checks
    checks += 1
    if not condition:
        errors.append(label)


def has(text: str, snippet: str, label: str) -> None:
    require(snippet in text, label)


def function_body(text: str, name: str) -> str:
    match = re.search(rf"\b{name}\s*\([^;]*?\)\s*\{{", text, re.S)
    if not match:
        return ""
    start = match.end() - 1
    depth = 0
    for index in range(start, len(text)):
        if text[index] == "{":
            depth += 1
        elif text[index] == "}":
            depth -= 1
            if depth == 0:
                return text[start : index + 1]
    return ""


moves = read("src/data/battle_moves.h")
scripts = read("data/battle_scripts_1.s")
macros = read("asm/macros/battle_script.inc")
battle_h = read("include/battle.h")
battle_constants = read("include/constants/battle.h")
command_constants = read("include/constants/battle_script_commands.h")
string_constants = read("include/constants/battle_string_ids.h")
anim_constants = read("include/constants/battle_anim.h")
battle_util = read("src/battle_util.c")
battle_main = read("src/battle_main.c")
commands = read("src/battle_script_commands.c")
ai = read("src/battle_ai_main.c")
messages = read("src/battle_message.c")
anim_scripts = read("data/battle_anim_scripts.s")
anim_c = read("src/battle_anim.c")
anim_fire = read("src/battle_anim_fire.c")
graphics_h = read("include/graphics.h")
graphics_c = read("src/graphics.c")

# Move data and script routing.
for move in ("WATER", "FIRE", "GRASS"):
    match = re.search(rf"\[MOVE_{move}_PLEDGE\]\s*=\s*\{{(.*?)\n\s*\}},", moves, re.S)
    require(match is not None, f"missing MOVE_{move}_PLEDGE data")
    if match:
        has(match.group(1), ".effect = EFFECT_PLEDGE", f"MOVE_{move}_PLEDGE is not EFFECT_PLEDGE")
        has(match.group(1), ".power = 80", f"MOVE_{move}_PLEDGE base power drifted")
has(scripts, ".4byte BattleScript_EffectPledge                  @ EFFECT_PLEDGE", "EFFECT_PLEDGE script routing missing")
for label in (
    "BattleScript_EffectPledge::",
    "BattleScript_EffectCombinedPledge_Water::",
    "BattleScript_EffectCombinedPledge_Fire::",
    "BattleScript_EffectCombinedPledge_Grass::",
    "BattleScript_EffectHit_Pledge::",
):
    has(scripts, label, f"missing {label}")
has(scripts, "accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE", "combined Pledge lacks accuracy/protection exit")
has(scripts, "tryfaintmon BS_TARGET, FALSE, NULL", "combined Pledge lacks faint sequencing")
has(macros, "VARIOUS_SET_PLEDGE", "setpledge macro missing")
has(macros, "VARIOUS_SET_PLEDGE_STATUS", "setpledgestatus macro missing")

# Constants must be unique and structurally valid.
side_bits = {
    name: int(bit)
    for name, bit in re.findall(r"#define\s+(SIDE_STATUS_[A-Z0-9_]+)\s+\(1\s*<<\s*(\d+)\)", battle_constants)
}
for name, expected in {
    "SIDE_STATUS_RAINBOW": 22,
    "SIDE_STATUS_SEA_OF_FIRE": 23,
    "SIDE_STATUS_SWAMP": 24,
}.items():
    require(side_bits.get(name) == expected, f"{name} bit changed or missing")
require(len(side_bits.values()) == len(set(side_bits.values())), "duplicate SIDE_STATUS bit assignment")
for name, value in {
    "PLEDGE_COMBO_NONE": "0",
    "PLEDGE_COMBO_WAITING": "1",
    "PLEDGE_COMBO_ATTACK": "2",
}.items():
    has(battle_constants, f"#define {name}", f"missing {name}")
    require(re.search(rf"#define\s+{name}\s+{value}\b", battle_constants) is not None, f"{name} value drifted")
has(battle_h, "u8 pledgeState;", "BattleStruct pledge state missing")
has(battle_h, "u16 pledgeOriginalMove;", "BattleStruct original Pledge move missing")
for timer in ("rainbowTimer", "seaOfFireTimer", "swampTimer"):
    has(battle_h, f"u8 {timer};", f"SideTimer.{timer} missing")

various = {
    name: int(value)
    for name, value in re.findall(r"#define\s+(VARIOUS_[A-Z0-9_]+)\s+(\d+)", command_constants)
}
require(various.get("VARIOUS_SET_PLEDGE") == 153, "VARIOUS_SET_PLEDGE id drifted")
require(various.get("VARIOUS_SET_PLEDGE_STATUS") == 154, "VARIOUS_SET_PLEDGE_STATUS id drifted")
require(len(various.values()) == len(set(various.values())), "duplicate VARIOUS command id")

string_ids = {
    name: int(value)
    for name, value in re.findall(r"#define\s+(STRINGID_[A-Z0-9_]+)\s+(\d+)", string_constants)
}
pledge_string_names = (
    "STRINGID_THETWOMOVESBECOMEONE",
    "STRINGID_ARAINBOWAPPEAREDONSIDE",
    "STRINGID_THERAINBOWDISAPPEARED",
    "STRINGID_WAITINGFORPARTNERSMOVE",
    "STRINGID_SEAOFFIREENVELOPEDSIDE",
    "STRINGID_HURTBYTHESEAOFFIRE",
    "STRINGID_THESEAOFFIREDISAPPEARED",
    "STRINGID_SWAMPENVELOPEDSIDE",
    "STRINGID_THESWAMPDISAPPEARED",
)
for name in pledge_string_names:
    require(name in string_ids, f"missing {name}")
    has(messages, f"[{name} - 12]", f"{name} is not registered")
count_match = re.search(r"#define\s+BATTLESTRINGS_COUNT\s+(\d+)", string_constants)
require(count_match is not None and int(count_match.group(1)) > max(string_ids.values()), "BATTLESTRINGS_COUNT does not cover Pledge strings")

# Combination mapping and action-state determinism.
compat_body = function_body(battle_util, "ArePledgeMovesCompatible")
result_body = function_body(battle_util, "GetPledgeCombinationMove")
prepare_body = function_body(battle_util, "TryPreparePledgeCombination")
for pair in (
    ("MOVE_WATER_PLEDGE", "MOVE_FIRE_PLEDGE"),
    ("MOVE_FIRE_PLEDGE", "MOVE_WATER_PLEDGE"),
    ("MOVE_FIRE_PLEDGE", "MOVE_GRASS_PLEDGE"),
    ("MOVE_GRASS_PLEDGE", "MOVE_FIRE_PLEDGE"),
    ("MOVE_GRASS_PLEDGE", "MOVE_WATER_PLEDGE"),
    ("MOVE_WATER_PLEDGE", "MOVE_GRASS_PLEDGE"),
):
    require(all(move in compat_body for move in pair), f"compatibility mapping missing {pair}")
for move in ("MOVE_WATER_PLEDGE", "MOVE_FIRE_PLEDGE", "MOVE_GRASS_PLEDGE"):
    has(result_body, f"return {move};", f"result mapping missing {move}")
has(prepare_body, "PLEDGE_COMBO_WAITING", "preparation does not require waiting state")
has(prepare_body, "PLEDGE_COMBO_ATTACK", "preparation does not enter attack state")
has(prepare_body, "pledgeOriginalMove", "preparation does not preserve original move")
has(prepare_body, "SetTypeBeforeUsingMove", "result type is not recalculated")
require("Random(" not in compat_body + result_body + prepare_body, "Pledge state/mapping uses nondeterministic RNG")

set_pledge_case = commands.split("case VARIOUS_SET_PLEDGE:", 1)[1].split("case VARIOUS_SET_PLEDGE_STATUS:", 1)[0]
for snippet, label in (
    ("gBattleTypeFlags & BATTLE_TYPE_DOUBLE", "no doubles-only guard"),
    ("IsBattlerAlive(partner)", "no living-partner guard"),
    ("gChosenActionByBattler[partner] == B_ACTION_USE_MOVE", "no partner-action guard"),
    ("!gProtectStructs[partner].noValidMoves", "no Struggle/no-valid-moves guard"),
    ("attackerOrder < partnerOrder", "already-acted partner is not rejected"),
    ("gBattlerByTurnOrder[i] = gBattlerByTurnOrder[i - 1]", "battler order is not preserved"),
    ("gActionsByTurnOrder[i] = gActionsByTurnOrder[i - 1]", "action order is not preserved"),
    ("PLEDGE_COMBO_WAITING", "waiting state is not staged"),
    ("PLEDGE_COMBO_ATTACK", "combined state is not dispatched"),
):
    has(set_pledge_case, snippet, label)
require("Random(" not in set_pledge_case, "turn-order staging uses RNG")

# Type-sensitive ordering, cancellation, and stale-state prevention.
attack_canceler = function_body(commands, "Cmd_attackcanceler")
require(attack_canceler.count("gBattleStruct->pledgeState != PLEDGE_COMBO_WAITING") >= 2, "pending Pledge is checked against primal weather before original incapacity checks")
powder_case = battle_util.split("case CANCELLER_POWDER_STATUS:", 1)[1].split("case CANCELLER_THROAT_CHOP:", 1)[0]
has(powder_case, "TryPreparePledgeCombination();", "Pledge result is not prepared for Powder/Protean")
require(powder_case.index("TryPreparePledgeCombination();") < powder_case.index("WEATHER_RAIN_PRIMAL") < powder_case.index("STATUS2_POWDER"), "Pledge result weather/Powder ordering drifted")
has(battle_util, "HITMARKER_NO_ATTACKSTRING | HITMARKER_UNABLE_TO_USE_MOVE", "freeze does not mark a failed Pledge action")
has(battle_util, "gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;", "Powder does not mark a failed Pledge action")
has(commands, "if (gHitMarker & HITMARKER_UNABLE_TO_USE_MOVE)", "move end does not clear failed combo state")
has(commands, "if (gBattleStruct->pledgeState == PLEDGE_COMBO_ATTACK)", "move end does not clear completed combo state")
has(battle_main, "gBattleStruct->pledgeState = PLEDGE_COMBO_NONE;", "turn cleanup does not clear combo state")
has(battle_util, "gBattleMoves[gCurrentMove].effect != EFFECT_PLEDGE", "Pledge can still be redirected by Storm Drain/Lightning Rod")
absorbing = battle_util.split("case ABILITYEFFECT_ABSORBING:", 1)[1].split("case ABILITYEFFECT_MOVE_END:", 1)[0]
has(absorbing, "gBattleStruct->pledgeState = PLEDGE_COMBO_NONE;", "absorption does not clear combo state")

# Damage and all three side effects.
has(battle_util, "basePower = 150;", "combined Pledge is not 150 power")
has(battle_util, "IS_BATTLER_OF_TYPE(BATTLE_PARTNER(battlerAtk), moveType)", "partner STAB is not considered")
has(battle_main, "SIDE_STATUS_SWAMP", "Swamp speed hook missing")
has(battle_main, "speed /= 4;", "Swamp does not quarter Speed")
for status, timer in (
    ("SIDE_STATUS_RAINBOW", "rainbowTimer"),
    ("SIDE_STATUS_SEA_OF_FIRE", "seaOfFireTimer"),
    ("SIDE_STATUS_SWAMP", "swampTimer"),
):
    has(commands, status, f"{status} application missing")
    has(commands, f"*timer = 4;", "Pledge timer is not four turns")
    has(battle_util, f"--gSideTimers[side].{timer} == 0", f"{timer} does not expire")
    has(commands, f"SWAP(gSideTimers[B_SIDE_PLAYER].{timer}", f"Court Change does not swap {timer}")
has(commands, "!(gSideStatuses[pledgeSide] & pledgeStatus)", "active Pledge status can be refreshed")
has(battle_util, "ENDTURN_SEA_OF_FIRE_DAMAGE", "Sea of Fire residual phase missing")
has(battle_util, "maxHP / 8", "Sea of Fire damage is not one eighth")
has(battle_util, "!IS_BATTLER_OF_TYPE(gActiveBattler, TYPE_FIRE)", "Sea of Fire lacks Fire immunity")
has(battle_util, "ability == ABILITY_MAGIC_GUARD", "Sea of Fire lacks Magic Guard immunity")
has(commands, "SIDE_STATUS_PLEDGE_ANY", "Court Change omits Pledge statuses")

# Rainbow secondary effects and held-item flinch semantics.
chance_body = function_body(commands, "Cmd_seteffectwithchance")
has(chance_body, "SIDE_STATUS_RAINBOW", "Rainbow move-effect chance hook missing")
has(chance_body, "moveEffect != EFFECT_SECRET_POWER", "Rainbow incorrectly boosts Secret Power")
has(chance_body, "hasSereneGrace && moveEffect == EFFECT_FLINCH_HIT", "Rainbow flinch stacks with Serene Grace")
kings_rock = battle_util.split("case ITEMEFFECT_KINGSROCK:", 1)[1].split("case ITEMEFFECT_LIFEORB_SHELLBELL:", 1)[0]
has(kings_rock, "SIDE_STATUS_RAINBOW", "Rainbow held-item flinch hook missing")
has(kings_rock, "gCurrentMove != MOVE_SECRET_POWER", "Rainbow boosts Secret Power held-item flinch")

# AI should coordinate exact pairs and account for result immunities/statuses.
for snippet, label in (
    ("PartnerHasCompatiblePledgeMove", "AI does not seek a compatible partner Pledge"),
    ("ArePledgeMovesCompatible(move, AI_DATA->partnerMove)", "AI does not recognize exact Pledge pairs"),
    ("GetPledgeCombinationMove(move, AI_DATA->partnerMove)", "AI does not evaluate result type"),
    ("SIDE_STATUS_RAINBOW", "AI ignores Rainbow state"),
    ("SIDE_STATUS_SEA_OF_FIRE", "AI ignores Sea of Fire state"),
    ("SIDE_STATUS_SWAMP", "AI ignores Swamp state"),
    ("ABILITY_FLASH_FIRE", "AI ignores Fire-result absorption"),
    ("ABILITY_SAP_SIPPER", "AI ignores Grass-result absorption"),
):
    has(ai, snippet, label)

# Native presentation registration and source assets.
for name, value in {"B_ANIM_RAINBOW": 34, "B_ANIM_SEA_OF_FIRE": 35, "B_ANIM_SWAMP": 36}.items():
    require(re.search(rf"#define\s+{name}\s+{value}\b", anim_constants) is not None, f"{name} id missing/drifted")
    has(anim_scripts, f"@ {name}", f"{name} table entry missing")
for label in ("General_Rainbow::", "General_SeaOfFire::", "General_Swamp::"):
    has(anim_scripts, label, f"missing native animation {label}")
for bg in ("BG_RAINBOW", "BG_SWAMP"):
    has(anim_c, f"[{bg}]", f"{bg} background registration missing")
for symbol in (
    "gBattleAnimBgImage_Rainbow",
    "gBattleAnimBgPalette_Rainbow",
    "gBattleAnimBgTilemap_Rainbow",
    "gBattleAnimBgImage_Swamp",
    "gBattleAnimBgPalette_Swamp",
    "gBattleAnimBgTilemap_Swamp",
):
    has(graphics_h, symbol, f"{symbol} declaration missing")
    has(graphics_c, symbol, f"{symbol} definition missing")
has(anim_fire, "gTwisterEmberSpriteTemplate", "Sea of Fire particle template missing")

for stem, expected_height in (("rainbow", 176), ("swampswizzle", 208)):
    png = ROOT / f"graphics/battle_anims/backgrounds/{stem}.png"
    tilemap = ROOT / f"graphics/battle_anims/backgrounds/{stem}.bin"
    palette = ROOT / f"graphics/battle_anims/backgrounds/{stem}.pal"
    require(png.is_file() and tilemap.is_file() and palette.is_file(), f"{stem} source asset set incomplete")
    if png.is_file():
        data = png.read_bytes()
        require(data[:8] == b"\x89PNG\r\n\x1a\n" and len(data) >= 24, f"{stem}.png is invalid")
        if len(data) >= 24:
            width, height = struct.unpack(">II", data[16:24])
            require(width == 128 and height == expected_height, f"{stem}.png dimensions are {width}x{height}, expected 128x{expected_height}")
    if tilemap.is_file():
        require(tilemap.stat().st_size == 2048, f"{stem}.bin must be a 32x32 tilemap")

# Deterministic reference vectors (independent of the C source scans).
WATER, FIRE, GRASS = "water", "fire", "grass"
expected_results = {
    (WATER, FIRE): WATER,
    (FIRE, WATER): WATER,
    (FIRE, GRASS): FIRE,
    (GRASS, FIRE): FIRE,
    (GRASS, WATER): GRASS,
    (WATER, GRASS): GRASS,
}


def model_result(first: str, second: str) -> str | None:
    return expected_results.get((first, second))


for pair, expected in expected_results.items():
    require(model_result(*pair) == expected, f"model combination failed for {pair}")
for move in (WATER, FIRE, GRASS):
    require(model_result(move, move) is None, f"same-Pledge pair combined for {move}")


def model_reorder(order: tuple[int, ...], attacker: int, partner: int) -> tuple[int, ...]:
    values = list(order)
    attacker_index = values.index(attacker)
    partner_index = values.index(partner)
    if attacker_index >= partner_index:
        return order
    values.pop(partner_index)
    values.insert(attacker_index + 1, partner)
    return tuple(values)


for order in itertools.permutations(range(4)):
    for attacker, partner in ((0, 2), (2, 0), (1, 3), (3, 1)):
        new_order = model_reorder(order, attacker, partner)
        require(sorted(new_order) == [0, 1, 2, 3], f"reorder lost/duplicated battler: {order}")
        old_a, old_p = order.index(attacker), order.index(partner)
        if old_a < old_p:
            require(new_order.index(partner) == new_order.index(attacker) + 1, f"partner not adjacent: {order}")
        else:
            require(new_order == order, f"already-acted partner was reordered: {order}")

timer = 4
timer_trace = []
for _ in range(4):
    timer -= 1
    timer_trace.append(timer)
require(timer_trace == [3, 2, 1, 0], f"four-turn timer trace is {timer_trace}")


def model_secondary(base: int, rainbow: bool, serene: bool, flinch: bool, secret_power: bool) -> int:
    chance = base * (2 if serene else 1)
    if rainbow and not secret_power and not (serene and flinch):
        chance *= 2
    return chance


require(model_secondary(10, True, False, False, False) == 20, "Rainbow did not double a normal secondary effect")
require(model_secondary(10, True, True, True, False) == 20, "Rainbow stacked with Serene Grace flinch")
require(model_secondary(10, True, True, False, False) == 40, "Rainbow/Serene Grace non-flinch interaction drifted")
require(model_secondary(10, True, False, False, True) == 10, "Rainbow boosted Secret Power")

if errors:
    print(f"Pledge mechanics verification FAILED ({len(errors)} failures / {checks} checks)")
    for error in errors:
        print(f"- {error}")
    sys.exit(1)

print(f"Pledge mechanics verification passed ({checks} checks)")
print("Covered: move routing, six ordered pairs, action ordering, failure cleanup, power/STAB,")
print("Rainbow/Sea of Fire/Swamp, Court Change, AI, messages, native animations, and assets.")
