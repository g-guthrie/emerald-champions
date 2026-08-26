#!/usr/bin/env python3
"""Regression gates for canonicalized legacy BUGFIX/UBFIX paths."""

from pathlib import Path
import re
import struct
import sys


ROOT = Path(__file__).resolve().parents[1]
TOKEN = re.compile(r"\b(?:BUGFIX|UBFIX)\b")

CANONICAL_FILES = (
    "gflib/sprite.c",
    "include/global.h",
    "src/apprentice.c",
    "src/battle_anim_sound_tasks.c",
    "src/battle_arena.c",
    "src/battle_dome.c",
    "src/battle_factory.c",
    "src/battle_factory_screen.c",
    "src/battle_palace.c",
    "src/battle_pike.c",
    "src/battle_pyramid.c",
    "src/battle_tent.c",
    "src/battle_tower.c",
    "src/battle_transition.c",
    "src/berry_blender.c",
    "src/berry_crush.c",
    "src/contest_ai.c",
    "src/daycare.c",
    "src/dodrio_berry_picking.c",
    "src/easy_chat.c",
    "src/egg_hatch.c",
    "src/event_object_movement.c",
    "src/fieldmap.c",
    "src/frontier_pass.c",
    "src/frontier_util.c",
    "src/intro.c",
    "src/link.c",
    "src/m4a.c",
    "src/main.c",
    "src/match_call.c",
    "src/metatile_behavior.c",
    "src/pokeball.c",
    "src/pokedex.c",
    "src/pokemon.c",
    "src/pokemon_animation.c",
    "src/pokemon_storage_system.c",
    "src/pokenav_ribbons_2.c",
    "src/region_map.c",
    "src/roulette.c",
    "src/siirtc.c",
    "src/trainer_hill.c",
    "src/tv.c",
    "src/union_room.c",
)

DEFERRED_TOKEN_PATHS = {
    "include/config.h",
    "data/scripts/trainer_hill.inc",
    "data/maps/MossdeepCity_SpaceCenter_1F/scripts.inc",
}

DISPATCHERS = {
    "src/apprentice.c": ("CallApprenticeFunction", "sApprenticeFunctions"),
    "src/battle_arena.c": ("CallBattleArenaFunction", "sArenaFunctions"),
    "src/battle_dome.c": ("CallBattleDomeFunction", "sBattleDomeFunctions"),
    "src/battle_factory.c": ("CallBattleFactoryFunction", "sBattleFactoryFunctions"),
    "src/battle_palace.c": ("CallBattlePalaceFunction", "sBattlePalaceFunctions"),
    "src/battle_pike.c": ("CallBattlePikeFunction", "sBattlePikeFunctions"),
    "src/battle_pyramid.c": ("CallBattlePyramidFunction", "sBattlePyramidFunctions"),
    "src/battle_tent.c": (
        ("CallVerdanturfTentFunction", "sVerdanturfTentFuncs"),
        ("CallFallarborTentFunction", "sFallarborTentFuncs"),
        ("CallSlateportTentFunction", "sSlateportTentFuncs"),
    ),
    "src/battle_tower.c": ("CallBattleTowerFunc", "sBattleTowerFuncs"),
    "src/frontier_util.c": ("CallFrontierUtilFunc", "sFrontierUtilFuncs"),
    "src/trainer_hill.c": ("CallTrainerHillFunction", "sHillFunctions"),
}

checks = 0
failures: list[str] = []


def read(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


def require(condition: bool, label: str) -> None:
    global checks
    checks += 1
    if not condition:
        failures.append(label)


def function_body(text: str, name: str) -> str:
    start = text.index(f"void {name}(void)")
    open_brace = text.index("{", start)
    depth = 0
    for index in range(open_brace, len(text)):
        if text[index] == "{":
            depth += 1
        elif text[index] == "}":
            depth -= 1
            if depth == 0:
                return text[open_brace + 1 : index]
    raise AssertionError(f"unterminated function {name}")


for path in CANONICAL_FILES:
    require(not TOKEN.search(read(path)), f"{path} has no legacy BUGFIX/UBFIX branch")

remaining: dict[str, list[int]] = {}
for path in ROOT.rglob("*"):
    if "build" in path.parts or ".git" in path.parts or path.suffix not in {".c", ".h", ".s", ".inc"}:
        continue
    try:
        lines = path.read_text(encoding="utf-8").splitlines()
    except UnicodeDecodeError:
        continue
    hits = [number for number, line in enumerate(lines, 1) if TOKEN.search(line)]
    if hits:
        remaining[str(path.relative_to(ROOT))] = hits

require(set(remaining) <= DEFERRED_TOKEN_PATHS,
        f"remaining BUGFIX/UBFIX paths are explicitly deferred: {sorted(remaining)}")
config = read("include/config.h")
require("#define BUGFIX" in config and "#define UBFIX" in config,
        "global compatibility switches remain while excluded consumers exist")

# Representative source invariants cover every high-risk canonicalization family.
source_invariants = {
    "gflib/sprite.c": ("if (j == 0)\n                break;",),
    "include/global.h": ("#define SAFE_DIV(a, b) ((b) ? (a) / (b) : 0)",),
    "src/berry_crush.c": ("game->sparkleAmount = 2;", "game->sparkleAmount = 3;"),
    "src/contest_ai.c": ("(Random() & 0xFF) < gAIScriptPtr[1]",),
    "src/daycare.c": ("RemoveIVIndexFromList(availableIVs, index);",),
    "src/dodrio_berry_picking.c": ("if (playerIdMissed != PLAYER_NONE)", "#define sFrozen data[1]"),
    "src/easy_chat.c": ("i < ARRAY_COUNT(gSaveBlock1Ptr->additionalPhrases)",),
    "src/event_object_movement.c": ("if (camera != NULL)", "if (obj == NULL)\n        return 0;"),
    "src/fieldmap.c": ("i < ARRAY_COUNT(gSaveBlock1Ptr->mapView)", "connections == NULL || connections->connections == NULL"),
    "src/link.c": ("memset(sSavedLinkPlayers, 0, sizeof(sSavedLinkPlayers));",),
    "src/m4a.c": ("u32 wav = 0;", "u32 unk = 0;"),
    "src/main.c": ("SeedRngWithRtc();",),
    "src/match_call.c": ("case FRONTIER_FACILITY_PIKE:", "case FRONTIER_FACILITY_FACTORY:"),
    "src/pokemon.c": ("if (currentHP <= 0)\n                currentHP = 1;",),
    "src/pokemon_animation.c": ("((u16)gTasks[taskId].tPtrLo)",),
    "src/pokemon_storage_system.c": ("if (boxMon != NULL)", "else\n        lvl = 0;"),
    "src/siirtc.c": ("u8 value = 0;", "rtc.status & SIIRTCINFO_POWER"),
}
for path, fragments in source_invariants.items():
    text = read(path)
    for fragment in fragments:
        require(fragment in text, f"{path} retains corrected fragment {fragment}")

dispatcher_count = 0
for path, entries in DISPATCHERS.items():
    if isinstance(entries[0], str):
        entries = (entries,)
    text = read(path)
    for function, table in entries:
        dispatcher_count += 1
        body = function_body(text, function)
        guard = f"gSpecialVar_0x8004 >= ARRAY_COUNT({table})"
        call = f"{table}[gSpecialVar_0x8004]();"
        require(guard in body and call in body and body.index(guard) < body.index(call),
                f"{function} bounds-checks {table} before dispatch")
        require("gSpecialVar_Result = FALSE;" in body,
                f"{function} returns a safe default for invalid selectors")
require(dispatcher_count == 13, "all 13 special-function dispatchers are inventoried")

contest_scripts = read("data/contest_ai_scripts.s")
require("if_appeal_num_eq 0, AI_CGM_BetterWhenAudienceExcited_Not1stAppeal" in contest_scripts
        and "if_appeal_num_not_eq 0, AI_CGM_BetterWhenAudienceExcited_Not1stAppeal" not in contest_scripts,
        "contest excitement scoring skips impossible appeal zero")
require(contest_scripts.count("if_not_used_combo_starter") >= 3
        and "if_used_combo_starter MON_1, AI_CGM_TargetMonWithJudgesAttention_CheckMon2" not in contest_scripts,
        "contest attention scoring rewards actual combo starters")

tv = read("src/tv.c")
require(tv.count("i < ARRAY_COUNT(sTVSecretBaseSecretsActions) - 1") == 2,
        "Secret Base TV ignores the unused high flag")
require("case SBSECRETS_STATE_HIT_CUSHION ... SBSECRETS_NUM_STATES - 1:" in tv,
        "Secret Base TV state switch stops at the last valid text state")
require("if (state >= ARRAY_COUNT(sTVSecretBaseSecretsTextGroup))" in tv,
        "Secret Base TV guards its final text-table lookup")

attributes = (ROOT / "data/tilesets/secondary/underwater/metatile_attributes.bin").read_bytes()
require(attributes[112 * 2 : 112 * 2 + 2] == b"\x6c\x10",
        "narrow underwater door uses MB_WATER_DOOR attributes")
sealed_map = (ROOT / "data/layouts/Underwater_SealedChamber/map.bin").read_bytes()
cells = struct.unpack(f"<{len(sealed_map) // 2}H", sealed_map)
require(any((cell & 0x3FF) == 0x270 for cell in cells),
        "the corrected narrow underwater door is used by Sealed Chamber")

sprite = read("gflib/sprite.c")
require("sSpriteResourceTags" in sprite
        and "sprite->template->tileTag" not in sprite
        and "sprite->template->paletteTag" not in sprite,
        "sprite resource release no longer dereferences temporary templates")

print(f"legacy bugfix verifier: {checks - len(failures)}/{checks} checks passed")
print(f"canonicalized files: {len(CANONICAL_FILES)}; guarded dispatchers: {dispatcher_count}")
print("remaining deferred BUGFIX/UBFIX inventory:")
for path, lines in sorted(remaining.items()):
    print(f"  {path}: {','.join(map(str, lines))}")
print("sprite template lifetime issue: resolved by the resource-tag sidecar")

if failures:
    for failure in failures:
        print(f"FAIL: {failure}")
    sys.exit(1)
