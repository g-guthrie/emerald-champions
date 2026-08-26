#!/usr/bin/env python3
"""Static ownership gate for Verdant's audited allocation paths.

This intentionally inventories a small, explicit function set. If an
allocation is added or removed, the expected inventory must be reviewed along
with its failure/rollback assertions rather than silently falling out of the
audit.
"""

from __future__ import annotations

import collections
import pathlib
import re
import sys


ROOT = pathlib.Path(__file__).resolve().parents[1]

HEAP_CALLS = ("Alloc", "AllocZeroed")
RESOURCE_CALLS = (
    "AllocSpritePalette",
    "CreateSprite",
    "CreateTask",
    "LoadCompressedSpritePalette",
    "LoadSpritePalette",
    "LoadSpriteSheet",
    "RequestDma3Copy",
)

# (file, function): (heap call inventory, resource call inventory)
EXPECTED = {
    ("src/item_icon.c", "AllocItemIconTemporaryBuffers"): (
        collections.Counter({"Alloc": 1, "AllocZeroed": 1}),
        collections.Counter(),
    ),
    ("src/item_icon.c", "AddItemIconSprite"): (
        collections.Counter(),
        collections.Counter({"LoadSpriteSheet": 1, "LoadCompressedSpritePalette": 1, "CreateSprite": 1}),
    ),
    ("src/item_icon.c", "AddCustomItemIconSprite"): (
        collections.Counter(),
        collections.Counter({"LoadSpriteSheet": 1, "LoadCompressedSpritePalette": 1, "CreateSprite": 1}),
    ),
    ("src/battle_ai_util.c", "AI_CalcPartyMonDamage"): (
        collections.Counter({"Alloc": 1}),
        collections.Counter(),
    ),
    ("src/battle_anim_utility_funcs.c", "InitStatsChangeAnimation"): (
        collections.Counter({"AllocZeroed": 1}),
        collections.Counter(),
    ),
    ("src/battle_anim_utility_funcs.c", "AnimTask_AllocBackupPalBuffer"): (
        collections.Counter({"AllocZeroed": 1}),
        collections.Counter(),
    ),
    ("src/battle_anim_effects_2.c", "AnimTask_LoadMusicNotesPals"): (
        collections.Counter({"AllocZeroed": 1}),
        collections.Counter({"AllocSpritePalette": 1}),
    ),
    ("src/battle_anim_mons.c", "CreateAdditionalMonSpriteForMoveAnim"): (
        collections.Counter({"AllocZeroed": 1}),
        collections.Counter({"LoadSpriteSheet": 1, "AllocSpritePalette": 1, "CreateTask": 1, "RequestDma3Copy": 1, "CreateSprite": 2}),
    ),
    ("src/battle_interface.c", "RestoreOverwrittenPixels"): (
        collections.Counter({"Alloc": 1}),
        collections.Counter(),
    ),
    ("src/battle_interface.c", "CreateAbilityPopUp"): (
        collections.Counter(),
        collections.Counter({"LoadSpriteSheet": 1, "LoadSpritePalette": 1, "CreateSprite": 2, "CreateTask": 1}),
    ),
    ("src/battle_gfx_sfx_util.c", "AllocateBattleSpritesData"): (
        collections.Counter({"AllocZeroed": 5}),
        collections.Counter(),
    ),
    ("src/battle_gfx_sfx_util.c", "TryAllocateMonSpritesGfx"): (
        collections.Counter({"AllocZeroed": 3}),
        collections.Counter(),
    ),
    ("src/battle_util2.c", "AllocateBattleResources"): (
        collections.Counter({"AllocZeroed": 13}),
        collections.Counter(),
    ),
    ("src/trainer_hill.c", "InitTrainerHillBattleStruct"): (
        collections.Counter({"AllocZeroed": 1}),
        collections.Counter(),
    ),
    ("src/trainer_hill.c", "SetUpDataStruct"): (
        collections.Counter({"AllocZeroed": 1}),
        collections.Counter(),
    ),
}


# Complete callable fan-out. "deferred" means the legacy call is intentionally
# retained because a local allocation failure cannot produce a correct scene
# outcome without the stated non-local redesign.
MON_GFX_CALLERS = (
    ("src/battle_main.c", "CB2_InitBattle", "checked", "battle-specific loss callback"),
    ("src/trade.c", "CB2_LinkTrade", "checked", "rebuild saved trade menu"),
    ("src/trade.c", "CB2_InGameTrade", "checked", "return to field before trade mutation"),
    ("src/egg_hatch.c", "CB2_EggHatch_0", "checked", "free early scene root and return to field"),
    ("src/pokeblock_feed.c", "LoadPokeblockFeedScene", "checked", "restore palette transfer and saved callback"),
    ("src/evolution_scene.c", "EvolutionScene", "checked", "free scene root/windows and resume evolution callback"),
    (
        "src/contest.c",
        "CB2_StartContest",
        "deferred",
        "requires checked 17-allocation contest cluster, true 0x4000 two-slot pointer graph, and script/link synchronized abort",
    ),
    ("src/contest_painting.c", "ShowContestPainting", "checked", "saved callback before painting resources exist"),
    (
        "src/contest_util.c",
        "AllocContestResults",
        "deferred",
        "requires checked 9-allocation results cluster and no-allocation link close/wait abort callback",
    ),
    ("src/contest_util.c", "ShowContestEntryMonPic", "checked", "optional portrait may be omitted with full local rollback"),
)


CALLER_REQUIRED_SNIPPETS = {
    ("src/trade.c", "CB2_LinkTrade"): (
        "FREE_AND_SET_NULL(sTradeData)",
        "SetMainCallback2(gMain.savedCallback)",
    ),
    ("src/trade.c", "CB2_InGameTrade"): (
        "FREE_AND_SET_NULL(sTradeData)",
        "SetMainCallback2(CB2_ReturnToField)",
    ),
    ("src/egg_hatch.c", "CB2_EggHatch_0"): (
        "FREE_AND_SET_NULL(sEggHatchData)",
        "SetMainCallback2(CB2_ReturnToField)",
    ),
    ("src/pokeblock_feed.c", "LoadPokeblockFeedScene"): (
        "gPaletteFade.bufferTransferDisabled = FALSE",
        "FREE_AND_SET_NULL(sPokeblockFeed)",
        "SetMainCallback2(gMain.savedCallback)",
    ),
    ("src/evolution_scene.c", "EvolutionScene"): (
        "FREE_AND_SET_NULL(sEvoStructPtr)",
        "FreeAllWindowBuffers()",
        "SetMainCallback2(gCB2_AfterEvolution)",
    ),
    ("src/contest_painting.c", "ShowContestPainting"): (
        "SetMainCallback2(gMain.savedCallback)",
    ),
    ("src/contest_util.c", "ShowContestEntryMonPic"): (
        "taskId >= NUM_TASKS",
        "IndexOfSpritePaletteTag(palette->tag) == 0xFF",
        "spriteId == MAX_SPRITES",
        "DestroyTask(taskId)",
        "FreeMonSpritesGfx()",
    ),
}


REQUIRED_SNIPPETS = {
    "src/item_icon.c": (
        "FREE_AND_SET_NULL(gItemIconDecompressionBuffer)",
        "TRY_FREE_AND_SET_NULL(gItemIconDecompressionBuffer)",
        "TRY_FREE_AND_SET_NULL(gItemIcon4x4Buffer)",
        "GetSpriteTileStartByTag(tilesTag) == 0xFFFF",
        "IndexOfSpritePaletteTag(paletteTag) == 0xFF",
        "if (spriteId == MAX_SPRITES)",
        "FreeSpritePaletteByTag(paletteTag)",
        "FreeSpriteTilesByTag(tilesTag)",
    ),
    "src/battle_ai_util.c": (
        "if (battleMons == NULL)",
        "return 0;",
        "Free(battleMons);",
    ),
    "src/battle_anim_utility_funcs.c": (
        "if (sAnimStatsChangeData == NULL)",
        "DestroyAnimVisualTask(taskId);",
        "static EWRAM_DATA u16 *sBackupPalBuffer = NULL;",
        "gMonSpritesGfxPtr->buffer != sBackupPalBuffer",
        "gMonSpritesGfxPtr->buffer == sBackupPalBuffer",
        "sBackupPalBuffer = NULL;",
    ),
    "src/battle_anim_effects_2.c": (
        "if (paletteNums[i] == 0xFF)",
        "if (buffer == NULL)",
        "TRY_FREE_AND_SET_NULL(buffer)",
        "if (!loaded)",
        "FreeSpritePaletteByTag(ANIM_SPRITES_START - i)",
    ),
    "src/battle_anim_mons.c": (
        "EWRAM_DATA static u16 *sAdditionalMonSpriteBuffer = NULL;",
        "cleanupTaskId >= NUM_TASKS",
        "dmaRequest < 0",
        "gMonSpritesGfxPtr->buffer != sAdditionalMonSpriteBuffer",
        "gMonSpritesGfxPtr->buffer == sAdditionalMonSpriteBuffer",
        "CheckForSpaceForDma3Request",
        "gTasks[cleanupTaskId].data[1] = TRUE",
        "FreeSpritePaletteByTag(paletteTag)",
        "FreeSpriteTilesByTag(tilesTag)",
    ),
    "src/battle_anim_effects_3.c": (
        "if (spriteId == MAX_SPRITES)",
        "if (spriteId2 == MAX_SPRITES)",
    ),
    "src/battle_interface.c": (
        "static bool32 RestoreOverwrittenPixels",
        "if (buffer == NULL)",
        "if (taskId >= NUM_TASKS)",
        "if (spriteId1 == MAX_SPRITES)",
        "if (spriteId2 == MAX_SPRITES)",
        "if (taskId < NUM_TASKS)",
        "FreeSpritePaletteByTag(ABILITY_POP_UP_TAG)",
        "FreeSpriteTilesByTag(ABILITY_POP_UP_TAG)",
    ),
    "src/battle_gfx_sfx_util.c": (
        "bool32 AllocateBattleSpritesData(void)",
        "FreeBattleSpritesData();",
        "bool32 TryAllocateMonSpritesGfx(void)",
        "if (gMonSpritesGfxPtr != NULL)",
        "void AllocateMonSpritesGfx(void)",
        "(void)TryAllocateMonSpritesGfx();",
        "FreeMonSpritesGfx();",
    ),
    "src/battle_util2.c": (
        "bool32 AllocateBattleResources(void)",
        "if (!InitTrainerHillBattleStruct())",
        "FreeBattleResources();",
        "FREE_AND_SET_NULL(gBattleStruct)",
    ),
    "src/trainer_hill.c": (
        "bool32 InitTrainerHillBattleStruct(void)",
        "if (sHillData == NULL)",
        "if (sRoomTrainers == NULL)",
        "FreeDataStruct();",
    ),
    "src/battle_main.c": (
        "static void AbortBattleInitAllocationFailure(void)",
        "gMonSpritesGfxPtr = NULL;",
        "if (!AllocateBattleResources()",
        "|| !AllocateBattleSpritesData()",
        "|| !TryAllocateMonSpritesGfx())",
        "gSpecialVar_Result = gBattleOutcome = B_OUTCOME_LOST;",
    ),
}


def extract_function(source: str, name: str) -> str:
    signature = re.compile(
        rf"^[A-Za-z_][^;\n]*\b{re.escape(name)}\s*\([^;]*?\)\s*\{{",
        re.MULTILINE | re.DOTALL,
    )
    match = signature.search(source)
    if match is None:
        raise ValueError(f"function not found: {name}")

    start = source.find("{", match.start())
    depth = 0
    for index in range(start, len(source)):
        char = source[index]
        if char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                return source[match.start() : index + 1]
    raise ValueError(f"unterminated function: {name}")


def call_inventory(body: str, names: tuple[str, ...]) -> collections.Counter[str]:
    return collections.Counter(
        match.group(1)
        for match in re.finditer(rf"\b({'|'.join(map(re.escape, names))})\s*\(", body)
    )


def main() -> int:
    errors: list[str] = []
    cache: dict[str, str] = {}

    print("allocation ownership inventory:")
    for (relative_path, function), (expected_heap, expected_resources) in EXPECTED.items():
        source = cache.setdefault(relative_path, (ROOT / relative_path).read_text())
        try:
            body = extract_function(source, function)
        except ValueError as error:
            errors.append(f"{relative_path}: {error}")
            continue

        actual_heap = call_inventory(body, HEAP_CALLS)
        actual_resources = call_inventory(body, RESOURCE_CALLS)
        print(
            f"  {relative_path}:{function}: "
            f"heap={dict(actual_heap)} resources={dict(actual_resources)}"
        )
        if actual_heap != expected_heap:
            errors.append(
                f"{relative_path}:{function}: heap inventory {dict(actual_heap)} "
                f"!= expected {dict(expected_heap)}"
            )
        if actual_resources != expected_resources:
            errors.append(
                f"{relative_path}:{function}: resource inventory {dict(actual_resources)} "
                f"!= expected {dict(expected_resources)}"
            )

    for relative_path, snippets in REQUIRED_SNIPPETS.items():
        source = cache.setdefault(relative_path, (ROOT / relative_path).read_text())
        for snippet in snippets:
            if snippet not in source:
                errors.append(f"{relative_path}: missing safety contract: {snippet}")

    print("mon-sprite allocation caller fan-out:")
    checked_count = 0
    deferred_count = 0
    for relative_path, function, mode, outcome in MON_GFX_CALLERS:
        source = cache.setdefault(relative_path, (ROOT / relative_path).read_text())
        try:
            body = extract_function(source, function)
        except ValueError as error:
            errors.append(f"{relative_path}: {error}")
            continue

        checked_calls = len(re.findall(r"\bTryAllocateMonSpritesGfx\s*\(\s*\)", body))
        legacy_calls = len(re.findall(r"\bAllocateMonSpritesGfx\s*\(\s*\)", body))
        print(f"  {mode:8} {relative_path}:{function}: {outcome}")
        if mode == "checked":
            checked_count += 1
            if checked_calls != 1 or legacy_calls != 0:
                errors.append(
                    f"{relative_path}:{function}: checked caller has "
                    f"Try={checked_calls}, legacy={legacy_calls}"
                )
        else:
            deferred_count += 1
            if checked_calls != 0 or legacy_calls != 1:
                errors.append(
                    f"{relative_path}:{function}: deferred caller has "
                    f"Try={checked_calls}, legacy={legacy_calls}"
                )

        for snippet in CALLER_REQUIRED_SNIPPETS.get((relative_path, function), ()):
            if snippet not in body:
                errors.append(f"{relative_path}:{function}: missing caller rollback: {snippet}")

    if checked_count != 8 or deferred_count != 2:
        errors.append(
            f"mon-gfx caller classification changed: checked={checked_count}, deferred={deferred_count}"
        )

    legacy_callers_in_tree = 0
    for path in (ROOT / "src").glob("*.c"):
        source = path.read_text()
        legacy_callers_in_tree += len(re.findall(r"(?<!Try)AllocateMonSpritesGfx\s*\(\s*\)\s*;", source))
    if legacy_callers_in_tree != deferred_count:
        errors.append(
            f"legacy mon-gfx callsites in src={legacy_callers_in_tree}; expected deferred={deferred_count}"
        )

    # The two former temporary SpriteTemplate heap allocations must stay gone.
    item_icon = cache["src/item_icon.c"]
    if re.search(r"Alloc\s*\(\s*sizeof\s*\(\s*\*?spriteTemplate", item_icon):
        errors.append("src/item_icon.c: temporary SpriteTemplate heap allocation was reintroduced")

    if errors:
        print("\nallocation ownership gate: FAIL", file=sys.stderr)
        for error in errors:
            print(f"  - {error}", file=sys.stderr)
        return 1

    print("allocation ownership gate: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
