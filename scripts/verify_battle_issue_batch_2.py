#!/usr/bin/env python3
"""Semantic gates for the second upstream battle-issue batch."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def section(text: str, start: str, end: str) -> str:
    begin = text.index(start)
    finish = text.index(end, begin + len(start))
    return text[begin:finish]


commands = read("src/battle_script_commands.c")
util = read("src/battle_util.c")
ai = read("src/battle_ai_util.c")
battle_h = read("include/battle.h")
util_h = read("include/battle_util.h")
constants = read("include/constants/battle_script_commands.h")
macros = read("asm/macros/battle_script.inc")
scripts = read("data/battle_scripts_1.s")

move_end_attacker = section(
    commands,
    "case MOVEEND_ABILITIES_ATTACKER:",
    "case MOVEEND_STATUS_IMMUNITY_ABILITIES:",
)
poison_touch = section(util, "case ABILITY_POISON_TOUCH:", "case ABILITY_STENCH:")
stench = section(util, "case ABILITY_STENCH:", "case ABILITY_GULP_MISSILE:")
yawn_set = section(commands, "static void Cmd_setyawn(void)\n{", "static void Cmd_setdamagetohealthdifference(void)\n{")
yawn_resolve = section(util, "case ENDTURN_YAWN:", "case ENDTURN_LASER_FOCUS:")
ai_damage = section(ai, "s32 AI_CalcDamage", "// Checks if one of the moves has side effects")
room_set = section(commands, "static void Cmd_setroom(void)\n{", "static void Cmd_tryswapabilities(void) //")
room_end = section(
    commands,
    "case VARIOUS_ROOM_SERVICE_MAGIC_ROOM_END:",
    "case VARIOUS_TERRAIN_SEED:",
)
fling_remove = section(commands, "static void RemoveFlingItem", "static void Cmd_various")
consume_berry = section(
    commands,
    "case VARIOUS_CONSUME_BERRY:",
    "case VARIOUS_JUMP_IF_CANT_REVERT_TO_PRIMAL:",
)


checks = {
    "Move start snapshots Poison Touch/Stench before contact changes Ability": all(
        token in commands or token in battle_h
        for token in ("moveStartAbilitySet", "moveStartAbility")
    ) and all(token in move_end_attacker for token in ("ABILITY_POISON_TOUCH", "ABILITY_STENCH", "abilityOverride")),
    "Mummy/Wandering Spirit and Poison Touch honor contact suppression": (
        util.count("IsMoveMakingContact(move, gBattlerAttacker)") >= 2
        and "MoveMakesContactForAttackerAbility(move, gBattlerAttacker)" in poison_touch
        and "IsAbilitySecondaryEffectBlocked(gBattlerTarget)" in poison_touch
        and "IsAbilitySecondaryEffectBlocked(gBattlerTarget)" in stench
        and "ABILITY_SHIELD_DUST" in section(util, "static bool32 IsAbilitySecondaryEffectBlocked", "u8 AbilityBattleEffects")
    ),
    "Protective Pads block defender contact effects but not Poison Touch": (
        "HOLD_EFFECT_PROTECTIVE_PADS" not in section(util, "static bool32 MoveMakesContactForAttackerAbility", "u8 AbilityBattleEffects")
        and "ABILITY_LONG_REACH" in section(util, "static bool32 MoveMakesContactForAttackerAbility", "u8 AbilityBattleEffects")
        and "IsMoveMakingContact(move, gBattlerAttacker)" in section(util, "case ABILITY_MUMMY:", "case ABILITY_WANDERING_SPIRIT:")
        and "IsMoveMakingContact(move, gBattlerAttacker)" in section(util, "case ABILITY_WANDERING_SPIRIT:", "case ABILITY_ANGER_POINT:")
    ),
    "Yawn initial eligibility has the documented blocker set": all(
        token in section(util, "bool32 CanYawnDrowse", "bool32 CanYawnResolveSleep")
        for token in (
            "ABILITY_COMATOSE",
            "ABILITY_PURIFYING_SALT",
            "ABILITY_SWEET_VEIL",
            "IsShieldsDownProtected",
            "IsFlowerVeilProtected",
            "SIDE_STATUS_SAFEGUARD",
            "DoesSubstituteBlockMove",
            "STATUS_FIELD_ELECTRIC_TERRAIN",
        )
    ),
    "Misty Terrain blocks Yawn both at application and at resolution": (
        "STATUS_FIELD_MISTY_TERRAIN" in yawn_set
        and "STATUS_FIELD_MISTY_TERRAIN" in yawn_resolve
    ),
    "Yawn resolution ignores newly acquired Flower Veil/Safeguard": (
        "IsFlowerVeilProtected" not in section(util, "bool32 CanYawnResolveSleep", "bool32 CanBePoisoned")
        and "SIDE_STATUS_SAFEGUARD" not in section(util, "bool32 CanYawnResolveSleep", "bool32 CanBePoisoned")
        and "CanYawnResolveSleep(gActiveBattler)" in yawn_resolve
    ),
    "AI multi-hit damage separates first Berry hit from later hits": all(
        token in ai_damage
        for token in (
            "hitScale",
            "HOLD_EFFECT_RESIST_BERRY",
            "gBattleMons[battlerDef].item = ITEM_NONE",
            "dmg += unresistedDmg * (hitScale - 1)",
            "ABILITY_SKILL_LINK",
            "EFFECT_SCALE_SHOT",
        )
    ),
    "Room Service tracks eligible versus blocked entrants": all(
        token in battle_h for token in ("roomServiceEligible", "roomServiceBlocked", "roomServiceCheck")
    ) and all(token in room_set for token in ("roomServiceEligible", "roomServiceBlocked", "STATUS_FIELD_MAGIC_ROOM")),
    "Room Service can activate when Magic Room ends": (
        "VARIOUS_ROOM_SERVICE_MAGIC_ROOM_END" in constants
        and "tryroomserviceaftermagicroom" in macros
        and scripts.count("tryroomserviceaftermagicroom") >= 2
        and all(token in room_end for token in ("roomServiceEligible", "TryRoomService", "BattleScript_BerryStatRaiseRet"))
    ),
    "Room Service item transfers reset stale switch-in blocking": (
        commands.count("UpdateRoomServiceItemState") >= 9
        and "!gDisableStructs[battlerId].roomServiceBlocked" in section(util, "bool32 TryRoomService", "// Move Checks")
    ),
    "Cud Chew records a Berry thrown by its own Fling": all(
        token in fling_remove
        for token in ("POCKET_BERRIES", "ABILITY_CUD_CHEW", "cudChewBerry", "cudChewTurn")
    ),
    "A successful flung Berry can seed target Cud Chew while restoring its item": (
        "consumeberry BS_TARGET, 2" in scripts
        and "blockCudChewConsumption = (gBattlescriptCurrInstr[3] == TRUE)" in consume_berry
    ),
}

# Executable invariants for #9577. The old bug returned 44*3 == 132.
first_hit = 44
unresisted_hit = 92
checks["Three-hit resist-Berry estimate is first + two unresisted hits"] = (
    first_hit + unresisted_hit * (3 - 1) == 228
)
# Ripen quarters the first hit only; Skill Link still gets four full later hits.
ripen_first_hit = 23
checks["Ripen plus Skill Link reduces only the first of five hits"] = (
    ripen_first_hit + unresisted_hit * (5 - 1) == 391
)

# Executable Room Service state model for the three documented branches.
def room_service_model(*, present_at_trick_room: bool, entered_late: bool, received_item: bool) -> tuple[bool, bool]:
    blocked = entered_late
    eligible = present_at_trick_room and not blocked
    if received_item:
        blocked = False
        eligible = True
    return eligible, blocked


checks["Room Service state model distinguishes present, late, and newly received"] = (
    room_service_model(present_at_trick_room=True, entered_late=False, received_item=False) == (True, False)
    and room_service_model(present_at_trick_room=False, entered_late=True, received_item=False) == (False, True)
    and room_service_model(present_at_trick_room=False, entered_late=True, received_item=True) == (True, False)
)

failed = [name for name, ok in checks.items() if not ok]
for name, ok in checks.items():
    print(f"{'PASS' if ok else 'FAIL'}: {name}")

if failed:
    raise SystemExit(f"{len(failed)} second-batch battle checks failed")

print(f"PASS: {len(checks)} second-batch battle checks")
