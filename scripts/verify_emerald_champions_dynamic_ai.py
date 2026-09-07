#!/usr/bin/env python3
"""Verify the small, explicit Emerald Champions per-trainer AI layer."""

from __future__ import annotations

import sys
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from emerald_champions_teams import read_teams


PROFILES = {
    "AI_EC_TrickRoomDiscipline": {
        "TRAINER_ROXANNE_1",
        "TRAINER_NORMAN_1",
        "TRAINER_TATE_AND_LIZA_1",
    },
    "AI_EC_FlanneryAfterYou": {
        "TRAINER_FLANNERY_1",
    },
    "AI_EC_QuincyTruant": {
        "TRAINER_QUINCY",
    },
    "AI_EC_SnowScreen": {
        "TRAINER_SHELLY_WEATHER_INSTITUTE",
        "TRAINER_GLACIA",
    },
    "AI_EC_RedirectionSetup": {
        "TRAINER_BRAWLY_1",
        "TRAINER_WALLY_VR_1",
        "TRAINER_WALLY_VR_2",
        "TRAINER_LEAF_ALTERING_CAVE",
        "TRAINER_CYNTHIA_1",
    },
}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def main() -> None:
    source = (ROOT / "src/emerald_champions_ai.c").read_text()
    ai_main = (ROOT / "src/battle_ai_main.c").read_text()
    runner = (ROOT / "scripts/run_emerald_champions_runtime_gates.py").read_text()
    teams = {branch.trainer: branch for branch in read_teams()}
    # Team moves are authored independently. A profile also reacts to field
    # conditions introduced by the player; requiring its namesake move on
    # every assigned team incorrectly rejects those supported situations.
    dispatch = source.split("AiScoreFunc GetEmeraldChampionsDynamicAiFunc", 1)[1]
    actual_profiles = {}
    for cases, profile in re.findall(r"((?:\s*case TRAINER_[A-Z0-9_]+:)+)\s*return (AI_EC_\w+);", dispatch):
        for trainer in re.findall(r"case (TRAINER_[A-Z0-9_]+):", cases):
            actual_profiles[trainer] = profile

    expected_trainers = {trainer for rows in PROFILES.values() for trainer in rows}
    require(
        "sDynamicAiFunc == NULL" in ai_main
        and "GetEmeraldChampionsDynamicAiFunc(TRAINER_BATTLE_PARAM.opponentA)" in ai_main,
        "trainer-ID profiles are not installed before AI flags are built",
    )
    require(
        ai_main.index("sDynamicAiFunc == NULL") < ai_main.index("gAiThinkingStruct->aiFlags[B_BATTLER_1]"),
        "dynamic profile is installed after opponent flags",
    )
    require("test/battle/ai/emerald_champions_dynamic.c" in runner,
            "dynamic AI runtime tests are absent from the curated suite")

    for profile, rows in PROFILES.items():
        require(f"s32 {profile}(" in source, f"missing profile implementation: {profile}")
        # Runtime scenarios may use the real trainer-ID dispatcher. Their
        # behavior is established by executing the registered native suite,
        # not by requiring one literal spelling of a helper call in its source.
        for trainer in rows:
            require(actual_profiles.get(trainer) == profile, f"{trainer} is not mapped to {profile}")
            require(trainer in teams, f"{trainer} is absent from the team source")

    mapped_cases = {
        line.strip()[5:-1]
        for line in source.splitlines()
        if line.strip().startswith("case TRAINER_") and line.strip().endswith(":")
    }
    require(mapped_cases == expected_trainers,
            f"dynamic trainer mapping drifted: extra={sorted(mapped_cases-expected_trainers)} missing={sorted(expected_trainers-mapped_cases)}")
    print(f"PASS: {len(PROFILES)} focused dynamic AI profiles map to {len(expected_trainers)} reviewed marquee trainers")


if __name__ == "__main__":
    main()
