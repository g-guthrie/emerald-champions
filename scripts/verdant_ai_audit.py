#!/usr/bin/env python3
"""Three-layer static audit of Verdant's singles/doubles battle AI."""

from __future__ import annotations

import json
import re
from pathlib import Path

import verdant_custom_teams as custom
import verdant_doubles_conversion as doubles
import verdant_team_quality_audit as quality


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def main() -> None:
    ai_main = read("src/battle_ai_main.c")
    ai_switch = read("src/battle_ai_switch_items.c")
    flags = read("include/constants/battle_ai.h")
    trainers_text = read("src/data/trainers.h")
    manifest = json.loads(read("docs/verdant_doubles_manifest.json"))
    trainer_blocks = doubles.trainer_blocks(trainers_text)
    report = quality.audit()
    teams = {team["trainer_id"]: team for team in report["teams"]}
    problems = []

    # Layer 1: move/target scoring must understand both enemy slots and ally
    # collateral or activation. These are exact guards for previously observed
    # deterministic defects.
    scoring_tokens = (
        "AI_DATA->partnerMove != MOVE_NONE && gBattleMoves[AI_DATA->partnerMove].effect == EFFECT_HELPING_HAND",
        "score += IS_MOVE_STATUS(move) ? -12 : 7",
        "if ((target & MOVE_TARGET_FOES_AND_ALLY)",
        "s32 allyDamage = AI_CalcDamage(move, battlerAtk, battlerAtkPartner)",
        "AI_DATA->atkPartnerAbility != ABILITY_TELEPATHY",
        "&& !DoesBattlerIgnoreAbilityChecks(AI_DATA->atkAbility, move))",
        "[11] = AI_HelpPartner",
        "effect == EFFECT_FOLLOW_ME && (partnerChoosingSetup || PartnerHasSetupMove(partner))",
        "score += partnerChoosingSetup ? 12 : 6",
        "move == MOVE_ROUND && HasMove(BATTLE_PARTNER(battlerAtk), MOVE_ROUND)",
        "AI_DATA->partnerMove == MOVE_ROUND ? 10 : 4",
        "partnerAbility == ABILITY_DANCER && TestMoveFlags(move, FLAG_DANCE)",
        "effect == EFFECT_GRAVITY",
        "HasMoveWithLowAccuracy(BATTLE_PARTNER(battlerAtk)",
        "HasMove(BATTLE_PARTNER(battlerAtk), MOVE_GRAVITY)",
        "AI_DATA->partnerMove == MOVE_SKILL_SWAP",
        "AI_DATA->atkAbility == ABILITY_CONTRARY",
        "HasMove(battlerDef, MOVE_OVERHEAT)",
        "effect == EFFECT_SIMPLE_BEAM",
        "IsStatRaisingEffect(gBattleMoves[AI_DATA->partnerMove].effect)",
        "effect == EFFECT_GUARD_SPLIT",
        "effect == EFFECT_INSTRUCT",
        "effect == EFFECT_SAFEGUARD",
        "effect == EFFECT_SWAGGER",
        "> gBattleMons[battlerDef].defense + gBattleMons[battlerDef].spDefense",
    )
    for token in scoring_tokens:
        if token not in ai_main:
            problems.append(f"move-scoring guard missing: {token}")

    # Layer 2: switching must leave doomed Perish Song slots in time, never
    # request an absent bench mon, and respect trapping.
    switching_tokens = (
        "perishSongTimer <= 1",
        "CountUsablePartyMons(gActiveBattler) > 0",
        "!IsBattlerTrapped(gActiveBattler, TRUE)",
        "GetBestMonForSwitch",
        "AI_CalcPartyMonHazardDamage",
    )
    for token in switching_tokens:
        if token not in ai_switch:
            problems.append(f"switching guard missing: {token}")
    if "Random() % 3 < 2" in ai_switch:
        problems.append("forced tactical switching still contains a random two-thirds gate")

    # Layer 3: authored team profiles must be wired into the function table and
    # attached only to teams whose source contains the corresponding plan.
    profile_functions = {
        "AI_FLAG_PERISH_TRAP": "AI_PerishTrap",
        "AI_FLAG_COMBO_SETUP": "AI_ComboSetup",
        "AI_FLAG_SPEED_CONTROL": "AI_SpeedControl",
        "AI_FLAG_FIELD_CONTROL": "AI_FieldControl",
    }
    for bit, (flag, function) in enumerate(profile_functions.items(), 17):
        if f"#define {flag}" not in flags or f"[{bit}] = {function}" not in ai_main:
            problems.append(f"profile is not wired at bit {bit}: {flag}/{function}")

    for flag, trainer_ids in custom.AI_PROFILES.items():
        for trainer_id in trainer_ids:
            if trainer_id not in trainer_blocks:
                problems.append(f"profile references unknown trainer: {trainer_id}")
                continue
            if flag not in trainer_blocks[trainer_id].group(0):
                problems.append(f"{trainer_id}: source missing {flag}")

    for trainer_id in custom.AI_PROFILES["AI_FLAG_PERISH_TRAP"]:
        if "Perish trap" not in teams[trainer_id]["synergy_tags"]:
            problems.append(f"{trainer_id}: Perish profile has no Perish-trap team")
    combo_tags = {
        "Beat Up + Justified", "Frost Breath + Anger Point", "Surf ally activation",
        "Neutralizing Gas + Regigigas", "Dancer recital", "Guard Split transfer", "Instruct repetition",
        "Safeguard + Swagger", "protected Explosion",
    }
    for trainer_id in custom.AI_PROFILES["AI_FLAG_COMBO_SETUP"]:
        moves = {move for mon in teams[trainer_id]["mons"] for move in mon["moves"]}
        abilities = {mon["ability"] for mon in teams[trainer_id]["mons"]}
        native_motor_drive_circuit = (
            trainer_id == "TRAINER_JANICE"
            and "MOVE_DISCHARGE" in moves
            and any(mon["ability"] == "ABILITY_MOTOR_DRIVE" for mon in teams["TRAINER_JERRY_1"]["mons"])
        ) or (
            trainer_id == "TRAINER_JERRY_1"
            and "ABILITY_MOTOR_DRIVE" in abilities
            and any("MOVE_DISCHARGE" in mon["moves"] for mon in teams["TRAINER_JANICE"]["mons"])
        )
        gravity_partner = {"TRAINER_JACE": "TRAINER_ELI", "TRAINER_ELI": "TRAINER_JACE"}.get(trainer_id)
        native_gravity_accuracy_pair = False
        if gravity_partner is not None:
            pair_moves = moves | {
                move
                for mon in teams[gravity_partner]["mons"]
                for move in mon["moves"]
            }
            native_gravity_accuracy_pair = "MOVE_GRAVITY" in pair_moves and "MOVE_INFERNO" in pair_moves
        if not (
            combo_tags & set(teams[trainer_id]["synergy_tags"])
            or moves & {"MOVE_BEAT_UP", "MOVE_FROST_BREATH", "MOVE_SURF", "MOVE_SKILL_SWAP", "MOVE_SIMPLE_BEAM", "MOVE_ROUND", "MOVE_AFTER_YOU"}
            or "ABILITY_COMMANDER" in abilities
            or native_motor_drive_circuit
            or native_gravity_accuracy_pair
        ):
            problems.append(f"{trainer_id}: combo profile has no ally activation")
    for trainer_id in custom.AI_PROFILES["AI_FLAG_SPEED_CONTROL"]:
        team = teams[trainer_id]
        if not (team["speed_control_count"] or any("engine" in tag.lower() or "Trick Room" in tag for tag in team["synergy_tags"])):
            problems.append(f"{trainer_id}: speed profile has no speed mode")
    field_markers = {
        "rain", "sun", "sand", "snow", "terrain", "screens", "aurora",
        "reflect", "light_screen", "trick room", "trick_room", "gravity",
        "wonder room", "wonder_room",
    }
    for trainer_id in custom.AI_PROFILES["AI_FLAG_FIELD_CONTROL"]:
        tags = " ".join(teams[trainer_id]["synergy_tags"]).lower()
        moves = " ".join(move for mon in teams[trainer_id]["mons"] for move in mon["moves"]).lower()
        abilities = " ".join(mon["ability"] for mon in teams[trainer_id]["mons"]).lower()
        if not any(marker.lower() in tags or marker.lower() in moves or marker.lower() in abilities for marker in field_markers):
            problems.append(f"{trainer_id}: field profile has no authored field mode")
    for trainer_id, rule in manifest["formats"].items():
        block = trainer_blocks[trainer_id].group(0)
        if "AI_FLAG_CHECK_FOE" not in block:
            problems.append(f"{trainer_id}: missing universal foe awareness")
        if rule.get("multi_partner") and not all(flag in block for flag in ("AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL")):
            problems.append(f"{trainer_id}: story partner lacks partner/speed AI")

    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))
    print("PASS: AI layer 1 — ally targeting, Helping Hand, collateral, and immunity scoring guards")
    print("PASS: AI layer 2 — Perish timing, trapping, bench availability, hazards, and deterministic switching")
    print(f"PASS: AI layer 3 — {sum(len(ids) for ids in custom.AI_PROFILES.values())} authored profile assignments across {len(manifest['formats'])} foe-aware trainer records")


if __name__ == "__main__":
    main()
