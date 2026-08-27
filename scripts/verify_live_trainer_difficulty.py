#!/usr/bin/env python3
"""Release gate for the native live trainer-level difficulty option."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def main() -> None:
    constants = read("include/constants/global.h")
    option_menu = read("src/option_menu.c")
    battle_main = read("src/battle_main.c")
    menu = read("src/menu.c")
    new_game = read("src/new_game.c")
    save = read("src/save.c")
    flags = read("include/constants/flags.h")

    for token in (
        "TRAINER_LEVEL_DIFFICULTY_HARD   0",
        "TRAINER_LEVEL_DIFFICULTY_MEDIUM 1",
        "TRAINER_LEVEL_DIFFICULTY_EASY   2",
        "OPTIONS_TEXT_SPEED_EXTRA_FAST 3",
    ):
        require(token in constants, f"missing difficulty/text constant: {token}")

    require("[MENUITEM_DIFFICULTY]  = gText_Difficulty" in option_menu,
            "native option row is not labeled Difficulty")
    require(
        option_menu.index("gText_DifficultyHard")
        < option_menu.index("gText_DifficultyMedium")
        < option_menu.index("gText_DifficultyEasy"),
        "difficulty choices are not ordered Hard, Medium, Easy",
    )
    require("gText_TextSpeed" not in option_menu, "Text Speed remains in the option menu")
    require("optionsTextSpeed = TRAINER_LEVEL_DIFFICULTY_HARD" in new_game,
            "new games do not default to Hard")
    require("gameDifficulty = DIFFICULTY_CHALLENGE" in new_game,
            "Challenge Mode is no longer force-set")
    require("levelCaps = LEVEL_CAPS_STRICT" in new_game,
            "strict caps are no longer force-set")

    require("[OPTIONS_TEXT_SPEED_EXTRA_FAST] = 0" in menu,
            "extra-fast text is not zero-delay")
    require("return OPTIONS_TEXT_SPEED_EXTRA_FAST;" in menu,
            "runtime text speed is not fixed to extra-fast")
    require("{ 0x1, 0x2, 0x4, 0x8 }" in read("gflib/text.c"),
            "extra-fast vertical scroll speed is missing")
    require("{1, 2, 4, 8}" in read("src/unk_text_util_2.c"),
            "Braille/alternate text scroll lacks extra-fast support")
    require("game->textSpeed = GetPlayerTextSpeedDelay();" in read("src/berry_crush.c"),
            "Berry Crush still interprets difficulty as text speed")
    require("battleSave->textSpeed = OPTIONS_TEXT_SPEED_EXTRA_FAST" in read("src/recorded_battle.c"),
            "recorded battles still serialize difficulty as text speed")

    difficulty_body = battle_main.split("static void ApplyLiveTrainerLevelDifficulty(void)", 2)[2]
    difficulty_body = difficulty_body.split("static bool8 IsRoute103RivalTrainer", 1)[0]
    for token in (
        "case TRAINER_LEVEL_DIFFICULTY_MEDIUM:",
        "reduction = 2;",
        "case TRAINER_LEVEL_DIFFICULTY_EASY:",
        "reduction = 4;",
        "level = level > reduction ? level - reduction : 1;",
        "gExperienceTables[gBaseStats[species].growthRate][level]",
        "CalculateMonStats(&gEnemyParty[i]);",
        "MON_DATA_MAX_HP",
        "MON_DATA_HP",
    ):
        require(token in difficulty_body, f"level-only difficulty invariant missing: {token}")
    init_body = battle_main.split("static void CB2_InitBattleInternal", 1)[1].split("gMain.inBattle = TRUE", 1)[0]
    require("if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)" in init_body,
            "difficulty is not gated to trainer battles")
    require("ApplyLiveTrainerLevelDifficulty();" in init_body,
            "trainer difficulty is not applied after party creation")
    require(
        init_body.index("CreateNPCTrainerParty")
        < init_body.index("ApplyLiveTrainerLevelDifficulty();")
        < init_body.index("SetWildMonHeldItem();"),
        "difficulty is applied at the wrong battle-initialization stage",
    )

    require("FLAG_EMERALD_CHAMPIONS_DIFFICULTY_MIGRATED" in flags,
            "difficulty migration flag is missing")
    require("!FlagGet(FLAG_EMERALD_CHAMPIONS_DIFFICULTY_MIGRATED)" in save,
            "existing saves are not migrated from old Fast to Hard")
    require("FlagSet(FLAG_EMERALD_CHAMPIONS_DIFFICULTY_MIGRATED)" in new_game,
            "new games do not mark the migration complete")

    allowed_raw_storage_users = {
        "src/battle_main.c",
        "src/new_game.c",
        "src/option_menu.c",
        "src/save.c",
    }
    actual_raw_storage_users = {
        str(path.relative_to(ROOT))
        for base in (ROOT / "src", ROOT / "gflib")
        for path in base.rglob("*.c")
        if "optionsTextSpeed" in path.read_text()
    }
    require(actual_raw_storage_users == allowed_raw_storage_users,
            f"difficulty field is still interpreted as text speed by {sorted(actual_raw_storage_users - allowed_raw_storage_users)}")

    print("Live difficulty: Hard=authored, Medium=-2, Easy=-4")
    print("Text: permanently extra-fast with zero glyph delay and 8-pixel scrolling")
    print("Challenge Mode, strict caps, AI, sets, items, abilities, and EVs remain unchanged")
    print("Live trainer difficulty release gate: PASS")


if __name__ == "__main__":
    main()
