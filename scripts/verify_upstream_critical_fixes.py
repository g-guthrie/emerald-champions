#!/usr/bin/env python3
"""Static contracts for critical inherited Expansion bug mitigations."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def require(path: str, needle: str, reason: str) -> None:
    text = (ROOT / path).read_text()
    if needle not in text:
        raise SystemExit(f"FAIL: {reason}: {path} lacks {needle!r}")


def main() -> None:
    require(
        "src/trainer_see.c",
        "if (task->tFuncId < funcCount)",
        "buried-trainer direct interaction must bounds-check its local dispatch table",
    )
    require(
        "src/trainer_see.c",
        "if (task->tFuncId >= funcCount",
        "buried-trainer completion must consume the main-table successor state",
    )
    require(
        "src/pokemon.c",
        "ctx.learnedMove != formChanges[i].param1",
        "FORM_CHANGE_MOVE must compare the changed move, not the action argument",
    )
    require(
        "src/chooseboxmon.c",
        "LearnMove_TryFormChange(partyIndex, boxmon, move);",
        "the native tutor/relearner must apply move-driven form changes",
    )
    require(
        "src/emerald_champions_battle_sets.c",
        "TryFormChangeOnMove(mon, preset->moves[i], B_TRAINER_PLAYER);",
        "authored tutor and wild presets must normalize move-driven forms",
    )
    require(
        "src/party_menu.c",
        "TryBoxMonFormChangeOnMove(boxmon, forgottenMove);",
        "the move deleter must apply move-driven form changes",
    )
    require(
        "include/config/battle.h",
        "#define B_RECORDED_BATTLES_ENABLED FALSE",
        "unsafe recorded battles must stay disabled",
    )
    require(
        "src/frontier_pass.c",
        "sPassData->hasBattleRecord = B_RECORDED_BATTLES_ENABLED",
        "old recordings must not remain playable from the Frontier Pass",
    )
    require(
        "src/pokemon_storage_system.c",
        "u8 itemName[ITEM_NAME_LENGTH + 1];",
        "the PC held-item buffer must reserve an EOS byte",
    )
    require(
        "src/pokemon_storage_system.c",
        "GetFontIdToFit(sStorage->messageText, FONT_NORMAL",
        "the PC held-item message must select a fitting native font",
    )
    print("PASS: critical upstream crash, form, recording, and PC text contracts")


if __name__ == "__main__":
    main()
