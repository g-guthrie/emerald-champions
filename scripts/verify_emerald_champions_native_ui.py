#!/usr/bin/env python3
"""Fail-closed source checks for Emerald Champions' native field UI.

This gate deliberately reads the executable map, script, and C sources.  It
does not accept design documents as evidence and it does not claim that a
static check replaces an in-emulator presentation pass.
"""

from __future__ import annotations

import ast
import hashlib
import json
import re
import runpy
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SCREEN_TILE_HEIGHT = 20
MESSAGE_BOX_TOP = 14

STANDARD_CENTER_LAYOUT = "LAYOUT_POKEMON_CENTER_1F"
LAVARIDGE_CENTER_LAYOUT = "LAYOUT_LAVARIDGE_TOWN_POKEMON_CENTER_1F"

CENTER_LAYOUTS = {
    STANDARD_CENTER_LAYOUT: {
        "path": "data/layouts/PokemonCenter_1F/map.bin",
        "sha256": "efe2c230ea5d46845064bd2b104d468f9b65e972bdae9e0477be4722696cde3d",
    },
    LAVARIDGE_CENTER_LAYOUT: {
        "path": "data/layouts/LavaridgeTown_PokemonCenter_1F/map.bin",
        "sha256": "708c976486634dc4a1164c47549645ff19f5344501d5ee66ddcb2fd7852a5345",
    },
}

PARTY_VISUAL_ASSETS = {
    "graphics/party_menu/bg.png": "66f37a9528470daf3169d6e2ff781081a8555cd4aa15df2320afc3283fe6f78d",
    "graphics/party_menu/bg.pal": "1b44d706c441eb77a61d9e477084a25806a9ddf70745d6cd4d238a489ac54b8c",
    "graphics/party_menu/bg.bin": "a14ed626ff3008b23714aff36ecccddf37c6fe994dad0b83b66be99f2c85c462",
}

EXPECTED_CENTERS = {
    "BattleFrontier_PokemonCenter_1F",
    "DewfordTown_PokemonCenter_1F",
    "EverGrandeCity_PokemonCenter_1F",
    "FallarborTown_PokemonCenter_1F",
    "FortreeCity_PokemonCenter_1F",
    "LavaridgeTown_PokemonCenter_1F",
    "LilycoveCity_PokemonCenter_1F",
    "MauvilleCity_PokemonCenter_1F",
    "MossdeepCity_PokemonCenter_1F",
    "OldaleTown_PokemonCenter_1F",
    "PacifidlogTown_PokemonCenter_1F",
    "PetalburgCity_PokemonCenter_1F",
    "RustboroCity_PokemonCenter_1F",
    "SlateportCity_PokemonCenter_1F",
    "SootopolisCity_PokemonCenter_1F",
    "VerdanturfTown_PokemonCenter_1F",
}

GAME_CORNER_REGIONAL_MENUS = (
    ("Kanto", "MULTI_EC_STARTER_ARCHIVE_KANTO"),
    ("Johto", "MULTI_EC_STARTER_ARCHIVE_JOHTO"),
    ("Hoenn", "MULTI_EC_STARTER_ARCHIVE_HOENN"),
    ("Sinnoh", "MULTI_EC_STARTER_ARCHIVE_SINNOH"),
    ("Unova", "MULTI_EC_STARTER_ARCHIVE_UNOVA"),
    ("Kalos", "MULTI_EC_STARTER_ARCHIVE_KALOS"),
    ("Alola", "MULTI_EC_STARTER_ARCHIVE_ALOLA"),
    ("Galar", "MULTI_EC_STARTER_ARCHIVE_GALAR"),
    ("Paldea", "MULTI_EC_STARTER_ARCHIVE_PALDEA"),
)

REQUIRED_HEADLESS_SCENARIOS = {
    "center-oldale",
    "center-lavaridge",
    "ability-menu",
    "party-overview",
    "party-action-menu",
    "options",
    "battle-vendor",
    "battle-vendor-shop",
    "move-specialist-root",
    "move-specialist-party-prompt",
    "battle-set-list",
    "all-legal-moves",
    "all-legal-moves-direct",
    "all-legal-moves-mew",
    "all-legal-moves-mew-middle",
    "all-legal-moves-mew-final",
    "battle-set-current",
    "game-corner-prizes",
    "game-corner-regions",
    "game-corner-region-list",
    "storage-root",
    "storage-boxes",
    "storage-box-popup",
    "storage-move-items",
    "naming",
    "starter-regions",
    "leveler-complete",
    "circuit-lobby",
    "circuit-welcome",
    "circuit-room",
    "wild-action-menu",
    "move-details",
    "thundurus",
    "tornadus",
    "landorus",
    "pokedex",
    "summary-info",
    "summary-skills",
    "summary-moves",
    "summary-move-detail",
    "summary-party-roundtrip",
    "bag",
    "frontier-pass",
    "frontier-pass-map",
    "double-status-ability",
    "mega-ready",
    "mega-active",
    "opposing-primals",
    "safari-action",
    "title-live",
    "birch-introduction",
    "pokeblock-condition",
    "trainer-card-gold",
    "battle-dome-info-card",
    "contest-results",
    "slot-machine",
    "fairy-summary-info",
    "fairy-summary-moves",
}

OVERWORLD_FIXTURE_PATTERN = re.compile(
    r"^EC_HEADLESS_OVERWORLD_FIXTURE\(\s*(\d+),\s*(MAP_[A-Z0-9_]+),\s*"
    r"(SPECIES_[A-Z0-9_]+),\s*(-?\d+),\s*(-?\d+)\)\s*$",
    re.MULTILINE,
)


def fail(message: str) -> None:
    raise SystemExit(message)


def require(condition: bool, message: str) -> None:
    if not condition:
        fail(message)


def read(relative: str) -> str:
    return (ROOT / relative).read_text()


def c_function(source: str, name: str) -> str:
    """Return one C function definition, using balanced braces."""

    match = re.search(
        rf"(?m)^\s*(?:static\s+)?[A-Za-z_][A-Za-z0-9_\s\*]*\b{re.escape(name)}\s*\([^;]*?\)\s*\{{",
        source,
    )
    require(match is not None, f"missing C function definition: {name}")
    opening = source.find("{", match.start())
    depth = 0
    for index in range(opening, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0:
                return source[match.start() : index + 1]
    fail(f"unterminated C function definition: {name}")


def source_region(source: str, start: str, end: str) -> str:
    require(start in source, f"missing source marker: {start}")
    require(end in source, f"missing source marker: {end}")
    start_index = source.index(start)
    end_index = source.index(end, start_index + len(start))
    return source[start_index:end_index]


def python_literal_assignment(source: str, name: str) -> object:
    """Return one top-level Python literal assignment without importing it."""

    tree = ast.parse(source)
    matches: list[ast.expr] = []
    for node in tree.body:
        if isinstance(node, ast.Assign) and any(
            isinstance(target, ast.Name) and target.id == name for target in node.targets
        ):
            matches.append(node.value)
        elif (
            isinstance(node, ast.AnnAssign)
            and isinstance(node.target, ast.Name)
            and node.target.id == name
            and node.value is not None
        ):
            matches.append(node.value)
    require(len(matches) == 1, f"expected one literal Python assignment for {name}")
    try:
        return ast.literal_eval(matches[0])
    except (TypeError, ValueError) as error:
        fail(f"{name} is no longer a static literal contract: {error}")


def require_headless_references_guarded(relative: str) -> None:
    """Require every fixture reference to sit inside a true EC fixture branch."""

    fixture_symbols = (
        "gEcHeadlessFixture",
        "CB2_EmeraldChampionsHeadlessFixture",
        "EmeraldChampionsHeadlessObserve",
    )
    stack: list[bool | None] = []
    for line_number, line in enumerate(read(relative).splitlines(), 1):
        stripped = line.strip()
        if stripped.startswith("#if"):
            expression = stripped.removeprefix("#if").strip()
            stack.append(True if expression == "EC_HEADLESS_FIXTURES" else None)
            continue
        if stripped.startswith("#else"):
            require(stack, f"{relative}:{line_number}: unmatched #else")
            if stack[-1] is not None:
                stack[-1] = not stack[-1]
            continue
        if stripped.startswith("#endif"):
            require(stack, f"{relative}:{line_number}: unmatched #endif")
            stack.pop()
            continue
        if any(symbol in line for symbol in fixture_symbols):
            require(
                True in stack,
                f"{relative}:{line_number}: fixture reference escapes EC_HEADLESS_FIXTURES",
            )
    require(not stack, f"{relative}: unterminated preprocessor branch")


def verify_center_layouts() -> dict[str, tuple[int, int]]:
    layouts_data = json.loads(read("data/layouts/layouts.json"))
    layouts = {entry["id"]: entry for entry in layouts_data["layouts"]}
    dimensions: dict[str, tuple[int, int]] = {}

    for layout_id, contract in CENTER_LAYOUTS.items():
        require(layout_id in layouts, f"restored Center layout is missing: {layout_id}")
        layout = layouts[layout_id]
        width = layout.get("width")
        height = layout.get("height")
        require(
            (width, height) == (16, 9),
            f"{layout_id} is {width}x{height}; the restored native contract is 16x9",
        )
        require(
            layout.get("blockdata_filepath") == contract["path"],
            f"{layout_id} points at the wrong executable blockdata",
        )
        blockdata = ROOT / contract["path"]
        require(blockdata.is_file(), f"Center blockdata is missing: {contract['path']}")
        data = blockdata.read_bytes()
        require(
            len(data) == width * height * 2,
            f"{contract['path']} has {len(data)} bytes, expected {width * height * 2}",
        )
        digest = hashlib.sha256(data).hexdigest()
        require(
            digest == contract["sha256"],
            f"{contract['path']} drifted from the reviewed restored layout: {digest}",
        )
        dimensions[layout_id] = (width, height)

    print("PASS: both restored Pokemon Center layouts are exact reviewed 16x9 binaries")
    return dimensions


def verify_party_visual_assets() -> None:
    for relative, expected in PARTY_VISUAL_ASSETS.items():
        path = ROOT / relative
        require(path.is_file(), f"party visual asset is missing: {relative}")
        actual = hashlib.sha256(path.read_bytes()).hexdigest()
        require(actual == expected, f"party visual asset drifted: {relative}: {actual}")

    graphics = read("src/graphics.c")
    require(
        'INCGFX_U16("graphics/party_menu/bg.pal", ".gbapal")' in graphics,
        "party menu is not loading the reviewed 176-color Verdant palette",
    )
    require(
        'INCGFX_U16("graphics/party_menu/bg.png", ".gbapal")' not in graphics,
        "party menu incorrectly derives one 16-color bank from the restored indexed PNG",
    )
    print("PASS: party skin uses the reviewed tiles, tilemap, and full 11-bank palette")


def one_object(
    objects: list[dict[str, object]],
    *,
    script: str,
    map_name: str,
) -> dict[str, object]:
    matches = [obj for obj in objects if obj.get("script") == script]
    require(
        len(matches) == 1,
        f"{map_name}: expected exactly one object running {script}, found {len(matches)}",
    )
    return matches[0]


def verify_center_geometry(dimensions: dict[str, tuple[int, int]]) -> None:
    center_paths = sorted((ROOT / "data/maps").glob("*PokemonCenter_1F/map.json"))
    centers = {path.parent.name: path for path in center_paths}
    require(
        set(centers) == EXPECTED_CENTERS,
        "the live Pokemon Center set drifted: "
        f"missing={sorted(EXPECTED_CENTERS - set(centers))}, "
        f"extra={sorted(set(centers) - EXPECTED_CENTERS)}",
    )

    for map_name, path in centers.items():
        data = json.loads(path.read_text())
        layout_id = data.get("layout")
        require(layout_id in CENTER_LAYOUTS, f"{map_name}: does not use a restored Center layout")
        width, height = dimensions[layout_id]
        objects = data.get("object_events")
        warps = data.get("warp_events")
        require(isinstance(objects, list), f"{map_name}: object_events is not a list")
        require(isinstance(warps, list), f"{map_name}: warp_events is not a list")

        coords: dict[tuple[int, int], list[str]] = {}
        for obj in objects:
            x, y = obj.get("x"), obj.get("y")
            require(
                isinstance(x, int) and isinstance(y, int) and 0 <= x < width and 0 <= y < height,
                f"{map_name}: object is outside {width}x{height}: {obj}",
            )
            coords.setdefault((x, y), []).append(str(obj.get("script")))
        duplicates = {coord: scripts for coord, scripts in coords.items() if len(scripts) > 1}
        require(not duplicates, f"{map_name}: duplicate object coordinates: {duplicates}")

        nurses = [obj for obj in objects if obj.get("graphics_id") == "OBJ_EVENT_GFX_NURSE"]
        require(len(nurses) == 1, f"{map_name}: expected exactly one native nurse")
        nurse = nurses[0]
        require(
            (nurse.get("x"), nurse.get("y"), nurse.get("movement_type"))
            == (8, 2, "MOVEMENT_TYPE_FACE_DOWN")
            and str(nurse.get("script", "")).endswith("_EventScript_Nurse"),
            f"{map_name}: nurse placement or interaction drifted: {nurse}",
        )

        vendor = one_object(
            objects,
            script="Common_EventScript_EmeraldChampionsBattleVendor",
            map_name=map_name,
        )
        tutor = one_object(
            objects,
            script="Common_EventScript_EmeraldChampionsMoveTutor",
            map_name=map_name,
        )
        expected_vendor = (2, 2)
        expected_tutor = (13, 2)
        require(
            (vendor.get("x"), vendor.get("y"), vendor.get("graphics_id"), vendor.get("movement_type"))
            == (*expected_vendor, "OBJ_EVENT_GFX_MART_EMPLOYEE", "MOVEMENT_TYPE_FACE_DOWN"),
            f"{map_name}: battle vendor is not in the reviewed native position: {vendor}",
        )
        require(
            (tutor.get("x"), tutor.get("y"), tutor.get("graphics_id"), tutor.get("movement_type"))
            == (*expected_tutor, "OBJ_EVENT_GFX_OLD_MAN", "MOVEMENT_TYPE_FACE_DOWN"),
            f"{map_name}: move tutor is not in the reviewed native position: {tutor}",
        )

        entrance_tiles = {(warp.get("x"), warp.get("y")) for warp in warps}
        require(
            {(7, 8), (8, 8)} <= entrance_tiles,
            f"{map_name}: centered two-tile entrance is missing: {sorted(entrance_tiles)}",
        )

    print("PASS: all 16 Centers have collision-free native nurse, service, and entrance geometry")


def verify_league_tutor() -> None:
    relative = "data/maps/EverGrandeCity_PokemonLeague_1F/map.json"
    data = json.loads(read(relative))
    objects = data.get("object_events", [])
    coords: dict[tuple[int, int], list[str]] = {}
    for obj in objects:
        coords.setdefault((obj.get("x"), obj.get("y")), []).append(str(obj.get("script")))
    duplicates = {coord: scripts for coord, scripts in coords.items() if len(scripts) > 1}
    require(not duplicates, f"Pokemon League lobby has duplicate object coordinates: {duplicates}")
    tutor = one_object(
        objects,
        script="Common_EventScript_EmeraldChampionsMoveTutor",
        map_name="EverGrandeCity_PokemonLeague_1F",
    )
    require(
        (tutor.get("x"), tutor.get("y"), tutor.get("graphics_id"), tutor.get("movement_type"))
        == (17, 2, "OBJ_EVENT_GFX_OLD_MAN", "MOVEMENT_TYPE_FACE_DOWN"),
        f"Pokemon League move tutor placement drifted: {tutor}",
    )
    print("PASS: the Pokemon League lobby retains a collision-free native move tutor")


def verify_ability_selector() -> None:
    source = read("src/party_menu.c")
    constants = read("include/constants/party_menu.h")
    menu_data = read("src/data/party_menu.h")

    display = c_function(source, "DisplayAbilitySelectionWindow")
    open_menu = c_function(source, "CursorCb_OpenAbilityMenu")
    handle = c_function(source, "Task_HandleAbilitySelectionInput")
    return_actions = c_function(source, "ReturnToPartyActionMenu")
    return_after_text = c_function(source, "Task_ReturnToPartyActionsAfterAbilityText")
    field_actions = c_function(source, "SetPartyMonFieldSelectionActions")

    require(
        "SELECTWINDOW_ABILITY" not in constants
        and "SELECTWINDOW_ABILITY" not in source
        and "MENU_ABILITY_SLOT_" not in source
        and "MENU_ABILITY_SLOT_" not in menu_data,
        "Ability selection fell back to synthesized party-action rows instead of its dedicated window",
    )
    require(
        "SetWindowTemplateFields(&window, 2, 12, 19 - (choiceCount * 2), 17, choiceCount * 2, 14, 0x2E9);"
        in display
        and "sPartyMenuInternal->windowId[0] = AddWindow(&window);" in display
        and "gAbilitiesInfo[ability].name" in display
        and "gText_Cancel2" in display
        and "InitMenuInUpperLeftCorner" in display,
        "Ability selection is not rendered in the reviewed dedicated native list window",
    )
    require(
        "enum Ability currentAbility = GetMonAbility(mon);" in open_menu
        and "u8 initialCursor = 0;" in open_menu
        and "== currentAbility" in open_menu
        and "DisplayAbilitySelectionWindow(count, slots, initialCursor);" in open_menu,
        "Ability selection no longer opens with the current Ability highlighted",
    )
    require(
        "SetMonData(mon, MON_DATA_ABILITY_NUM, &newSlot);" in handle
        and "Task_ConfirmAbilityChange" not in source
        and "sText_askText" not in open_menu
        and "sText_askText" not in handle
        and "DisplayPartyMenuMessage" not in open_menu
        and "PartyMenuDisplayYesNoMenu" not in handle
        and "CreateYesNoMenu" not in handle,
        "Ability selection is not a direct one-step apply action",
    )
    require(
        "PartyMenuRemoveWindow(&sPartyMenuInternal->windowId[0]);" in open_menu
        and "PartyMenuRemoveWindow(&sPartyMenuInternal->windowId[1]);" in open_menu,
        "Ability list can open over the party action/prompt windows",
    )
    apply_index = handle.find("SetMonData(mon, MON_DATA_ABILITY_NUM, &newSlot);")
    message_index = handle.find("DisplayPartyMenuMessage(gStringVar4, FALSE);")
    remove_index = handle.find("PartyMenuRemoveWindow(&sPartyMenuInternal->windowId[0]);")
    require(
        apply_index >= 0 and remove_index > apply_index and message_index > remove_index,
        "Ability confirmation text can overlap the dedicated Ability window",
    )
    require(
        "SetPartyMonSelectionActions" in return_actions
        and "DisplaySelectionWindow(SELECTWINDOW_ACTIONS);" in return_actions
        and "DisplayPartyMenuStdMessage(PARTY_MSG_DO_WHAT_WITH_MON);" in return_actions
        and "Task_HandleSelectionMenuInput" in return_actions
        and handle.count("ReturnToPartyActionMenu(taskId);") >= 2
        and "ReturnToPartyActionMenu(taskId);" in return_after_text,
        "Ability cancel/apply does not return to the same Pokemon action menu",
    )
    require(
        "CollectSelectableAbilitySlots(&mons[slotId], NULL) > 1" in field_actions
        and "MENU_OPEN_ABILITY" in field_actions
        and 'COMPOUND_STRING("Ability")' in menu_data,
        "the native Pokemon action menu does not expose the Ability selector consistently",
    )

    # The dynamic template's reviewed worst case is four rows (three slots plus
    # Cancel): x=12..28 and y=11..18, with its frame inside the 30x20 screen.
    choice_count = 4
    left, top, width, height = 12, 19 - choice_count * 2, 17, choice_count * 2
    require(
        left >= 1
        and top >= 1
        and left + width <= 29
        and top + height <= SCREEN_TILE_HEIGHT - 1,
        "Ability selector's maximum native window exceeds the party screen",
    )
    print("PASS: Ability selection is direct, cursor-aware, overlap-free, and returns in place")


def verify_battle_interface_sprite_guards() -> None:
    source = read("src/battle_interface.c")
    add_ball = c_function(source, "TryAddLastUsedBallItemSprites")
    hide_move_info = c_function(source, "TryToHideMoveInfoWindow")
    guarded_hide = re.compile(
        r"if\s*\(\s*gBattleStruct->moveInfoSpriteId\s*!=\s*MAX_SPRITES\s*\)\s*"
        r"gSprites\[gBattleStruct->moveInfoSpriteId\]\.sHide\s*=\s*TRUE\s*;"
    )

    require(
        "gBattleStruct->ballSpriteIds[1]" in add_ball
        and guarded_hide.search(add_ball) is not None,
        "R-button Ball creation can dereference the MAX_SPRITES move-info sentinel",
    )
    require(
        guarded_hide.search(hide_move_info) is not None,
        "TryToHideMoveInfoWindow can dereference the MAX_SPRITES sentinel",
    )
    print("PASS: first-screen R-button and move-info paths guard the sprite sentinel")


def verify_leveler_batch_flow() -> None:
    source = read("src/party_menu.c")
    find_next = c_function(source, "FindNextLevelerSlot")
    start = c_function(source, "StartLevelerPartySequence")
    open_next = c_function(source, "CB2_ShowPartyMenuForLeveler")
    apply_item = c_function(source, "ItemUseCB_RareCandy")
    continue_next = c_function(source, "Task_ContinueLevelerAfterText")
    complete = c_function(source, "Task_ShowLevelerComplete")
    continue_evolution = c_function(source, "CB2_ContinueLevelerEvolution")

    require(
        "min(GetCurrentLevelCap(), MAX_LEVEL)" in find_next
        and "MON_DATA_SPECIES) != SPECIES_NONE" in find_next
        and "!GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_IS_EGG)" in find_next
        and "MON_DATA_LEVEL) < levelCap" in find_next
        and "return PARTY_SIZE;" in find_next,
        "Leveler batching does not skip empty, Egg, and already-capped slots safely",
    )
    require(
        all(
            token in start
            for token in (
                "sLevelerNextSlot = 0;",
                "sLevelerEvolutionSpecies = SPECIES_NONE;",
                "sLevelerExitCallback = exitCallback;",
                "sLevelerRaisedParty = FALSE;",
                "SetMainCallback2(CB2_ShowPartyMenuForLeveler);",
            )
        ),
        "Leveler batching does not initialize all transient state explicitly",
    )
    leveler_fast_path = re.search(
        r"if\s*\(isLeveler\)\s*\{\s*"
        r"sLevelerRaisedParty\s*=\s*TRUE\s*;\s*"
        r"PlaySE\(SE_USE_ITEM\)\s*;\s*"
        r"PartyMenuTryEvolution\(taskId\)\s*;\s*return\s*;\s*\}",
        apply_item,
    )
    require(
        leveler_fast_path is not None
        and leveler_fast_path.start() < apply_item.index("PlayFanfareByFanfareNum"),
        "Leveler still enters per-Pokemon level-up text/stat pages instead of batching",
    )
    require(
        "if (slot == PARTY_SIZE)" in open_next
        and "if (sLevelerRaisedParty)" in open_next
        and "Task_ShowLevelerComplete" in open_next
        and "SetMainCallback2(sLevelerExitCallback);" in open_next
        and "sLevelerNextSlot = slot + 1;" in open_next,
        "Leveler startup/end states do not distinguish a completed batch from a no-op",
    )
    require(
        "newSlot = FindNextLevelerSlot();" in continue_next
        and "if (newSlot == PARTY_SIZE)" in continue_next
        and "gTasks[taskId].func = Task_ShowLevelerComplete;" in continue_next
        and "sLevelerNextSlot = newSlot + 1;" in continue_next
        and "ItemUseCB_RareCandy(taskId, Task_ClosePartyMenuAfterText);" in continue_next,
        "Leveler does not advance monotonically through one party batch",
    )
    require(
        "gPaletteFade.active || IsPartyMenuTextPrinterActive()" in complete
        and "min(GetCurrentLevelCap(), MAX_LEVEL)" in complete
        and "sText_LevelerComplete" in complete
        and "gPartyMenuUseExitCallback = TRUE;" in complete
        and "sLevelerRaisedParty = FALSE;" in complete
        and "gTasks[taskId].func = Task_ClosePartyMenuAfterText;" in complete,
        "Leveler completion is not one bounded final message followed by the original exit callback",
    )
    require(
        "gCB2_AfterEvolution = CB2_ContinueLevelerEvolution;" in continue_evolution
        and "SetMainCallback2(CB2_ShowPartyMenuForLeveler);" in continue_evolution,
        "Leveler evolution chaining does not return to the same party batch",
    )
    print("PASS: Leveler batches eligible slots, preserves evolution chaining, and reports once")


def verify_battle_set_preselection() -> None:
    sets_source = read("src/emerald_champions_battle_sets.c")
    field_source = read("src/field_specials.c")
    script = read("data/scripts/emerald_champions.inc")
    tests = read("test/emerald_champions.c")

    match_moves = c_function(sets_source, "DoesMonMatchPresetMoves")
    match_preset = c_function(sets_source, "DoesMonMatchPreset")
    current_choice = c_function(sets_source, "GetEmeraldChampionsCurrentBattleSetChoice")
    buffer_current = c_function(field_source, "BufferSelectedMonCurrentEmeraldChampionsBattleSet")
    scrolling = c_function(field_source, "ShowScrollableMultichoice")
    choose_style = source_region(
        script,
        "EmeraldChampions_EventScript_BattleSetChooseStyle:",
        "EmeraldChampions_EventScript_BattleSetUseDefault:",
    )

    require(
        "for (u32 monSlot = 0; monSlot < MAX_MON_MOVES; monSlot++)" in match_moves
        and "for (u32 presetSlot = 0; presetSlot < MAX_MON_MOVES; presetSlot++)" in match_moves
        and "monMove == preset->moves[presetSlot]" in match_moves,
        "current battle-set recognition became move-order-sensitive",
    )
    require(
        all(
            token in match_preset
            for token in (
                "MON_DATA_HIDDEN_NATURE",
                "DoesMonMatchPresetAbility(mon, preset)",
                "DoesMonMatchPresetMoves(mon, preset)",
                "MON_DATA_HELD_ITEM",
                "preset->statPoints[stat]",
            )
        ),
        "current battle-set recognition omits part of the authored orientation",
    )
    require(
        "GetEmeraldChampionsBattleSetCount(mon)" in current_choice
        and "ResolveVisibleChoice(mon, choice, &preset, NULL)" in current_choice
        and "DoesMonMatchPreset(mon, preset)" in current_choice
        and "return -1;" in current_choice,
        "current battle-set choice is not resolved against the live visible-choice list",
    )
    require(
        "gSpecialVar_Result = FALSE;" in buffer_current
        and "gSpecialVar_0x8005 = 0;" in buffer_current
        and "gSpecialVar_0x800A >= gPartiesCount[B_TRAINER_PLAYER]" in buffer_current
        and "GetEmeraldChampionsCurrentBattleSetChoice(mon)" in buffer_current
        and "gSpecialVar_0x8005 = choice;" in buffer_current
        and "gSpecialVar_Result = TRUE;" in buffer_current,
        "battle-set preselection does not fail closed or publish the matched cursor",
    )
    require(
        "case SCROLL_MULTI_EMERALD_CHAMPIONS_BATTLE_SET:" in scrolling
        and "task->tNumItems = GetEmeraldChampionsBattleSetCount" in scrolling
        and "task->tMaxItemsOnScreen = min(task->tNumItems, 4);" in scrolling
        and "task->tHeight = task->tMaxItemsOnScreen * 2;" in scrolling
        and "task->tScrollOffset = min(gSpecialVar_0x8005, task->tNumItems - task->tMaxItemsOnScreen);"
        in scrolling
        and "task->tSelectedRow = gSpecialVar_0x8005 - task->tScrollOffset;" in scrolling,
        "battle-set cursor preselection is not converted into a valid scroll/row pair",
    )
    require(
        "special BufferSelectedMonCurrentEmeraldChampionsBattleSet" in choose_style
        and "goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_BattleSetChooseCurrentStyle" in choose_style
        and "special ShowScrollableMultichoice" in choose_style
        and "copyvar VAR_0x8006, VAR_RESULT" in choose_style
        and "goto_if_ge VAR_0x8006, VAR_RESULT, EmeraldChampions_EventScript_BattleSetChooseMon"
        in choose_style
        and "copyvar VAR_0x8005, VAR_0x8006" in choose_style,
        "battle-set script does not preserve current-choice preselection and reject Exit/B safely",
    )
    require(
        'TEST("Emerald Champions tutor recognizes and reopens on the current battle set")' in tests
        and "GetEmeraldChampionsCurrentBattleSetChoice(&mon), choice" in tests
        and "GetEmeraldChampionsCurrentBattleSetChoice(&mon), -1" in tests,
        "runtime coverage for current battle-set recognition/preselection was removed",
    )
    print("PASS: battle-set recognition and cursor preselection use the full current orientation")


def verify_unified_move_list_width() -> None:
    source = read("src/move_relearner.c")
    preparation = read("src/data/pokemon/emerald_champions_preparation_learnsets.h")
    match = re.search(
        r"sEmeraldChampionsPreparationMoves_MEW\[\]\s*=\s*\{(.*?)\n\};",
        preparation,
        re.S,
    )
    require(match is not None, "Mew preparation learnset is missing")
    moves = [move for move in re.findall(r"MOVE_[A-Z0-9_]+", match.group(1)) if move != "MOVE_UNAVAILABLE"]
    require(len(moves) > 255, f"maximum-list Mew no longer proves a >255 unified list: {len(moves)}")
    require(
        re.search(r"(?m)^\s*u16 numMenuChoices;\s*$", source) is not None,
        "move relearner menu count can truncate the live >255 unified move list",
    )
    print(f"PASS: unified move list uses a 16-bit count for Mew's {len(moves)} preparation moves")


def verify_battle_vendor_navigation() -> None:
    source = read("data/scripts/emerald_champions.inc")
    entry = source_region(
        source,
        "Common_EventScript_EmeraldChampionsBattleVendor::",
        "EmeraldChampions_EventScript_BattleVendorMain:",
    )
    main = source_region(
        source,
        "EmeraldChampions_EventScript_BattleVendorMain:",
        "EmeraldChampions_EventScript_BattleVendorCompleteArchive:",
    )
    archive = source_region(
        source,
        "EmeraldChampions_EventScript_BattleVendorCompleteArchive:",
        "EmeraldChampions_EventScript_BattleItemCategories:",
    )
    categories = source_region(
        source,
        "EmeraldChampions_EventScript_BattleItemCategories:",
        "EmeraldChampions_EventScript_OpenOffenseItems:",
    )
    item_returns = source_region(
        source,
        "EmeraldChampions_EventScript_OpenOffenseItems:",
        "EmeraldChampions_EventScript_BattleVendorExit:",
    )

    require(
        "setvar VAR_0x8008, 0" in entry and "setvar VAR_0x8009, 0" in entry,
        "battle vendor does not initialize category/archive cursors per conversation",
    )
    require(
        "goto_if_set FLAG_BADGE08_GET, EmeraldChampions_EventScript_BattleVendorCompleteArchive" in main
        and "goto EmeraldChampions_EventScript_BattleItemCategoryChoices" in main,
        "pre-League battle vendor has an obsolete wrapper menu or wrong archive gate",
    )
    require(
        "dynmultistack 0, 0, FALSE, 4, 0, VAR_0x8009, DYN_MULTICHOICE_CB_NONE" in archive
        and archive.index("goto_if_eq VAR_RESULT, MULTI_B_PRESSED")
        < archive.index("copyvar VAR_0x8009, VAR_RESULT")
        and "case 0, EmeraldChampions_EventScript_BattleItemCategories" in archive
        and "case 1, EmeraldChampions_EventScript_OpenMegaArchive" in archive
        and "case 2, EmeraldChampions_EventScript_OpenEvolutionArchive" in archive,
        "post-Badge vendor cursor/B flow can store an invalid result or route to the wrong archive",
    )
    require(
        "dynmultistack 0, 0, FALSE, 5, 0, VAR_0x8008, DYN_MULTICHOICE_CB_NONE" in categories
        and categories.index("goto_if_eq VAR_RESULT, MULTI_B_PRESSED")
        < categories.index("copyvar VAR_0x8008, VAR_RESULT")
        and all(
            f"case {index}, EmeraldChampions_EventScript_Open{name}Items" in categories
            for index, name in enumerate(("Offense", "Defense", "Field", "Type", "Gem", "Species"))
        )
        and "goto_if_set FLAG_BADGE08_GET, EmeraldChampions_EventScript_BattleVendorMain" in categories
        and "goto EmeraldChampions_EventScript_BattleVendorExit" in categories,
        "battle-item categories do not preserve a valid cursor or implement native B/Back behavior",
    )
    require(
        item_returns.count("goto EmeraldChampions_EventScript_BattleItemCategories") == 6
        and item_returns.count("goto EmeraldChampions_EventScript_BattleVendorMain") == 2,
        "battle vendor subshops do not return to the menu that opened them",
    )
    print("PASS: battle vendor keeps independent cursors and B/Back returns one native level")


def verify_free_battle_vendor_list() -> None:
    shop = read("src/shop.c")
    build_list = c_function(shop, "BuyMenuBuildListMenuTemplate")
    print_price = c_function(shop, "BuyMenuPrintPriceInList")

    require(
        "#define SHOP_ITEM_NAME_WIDTH_WITH_PRICE    84" in shop
        and "#define SHOP_ITEM_NAME_WIDTH_WITHOUT_PRICE 108" in shop
        and ".itemPrintFunc = BuyMenuPrintPriceInList" in shop
        and ".textNarrowWidth = SHOP_ITEM_NAME_WIDTH_WITH_PRICE" in shop,
        "ordinary paid shops no longer retain their native price column and item-name width",
    )
    require(
        "if (sMartInfo.freeItems)" in build_list
        and "gMultiuseListMenuTemplate.itemPrintFunc = NULL;" in build_list
        and "gMultiuseListMenuTemplate.textNarrowWidth = SHOP_ITEM_NAME_WIDTH_WITHOUT_PRICE;"
        in build_list,
        "free battle-vendor catalogs do not remove the price callback and reclaim the list width",
    )
    require(
        "if (sMartInfo.freeItems || itemId == LIST_CANCEL)" in print_price
        and "sText_Free" not in shop
        and '_("FREE")' not in shop,
        "free battle-vendor rows can still print a FREE label or price column",
    )
    require(
        "void CreatePokemartMenu" in shop
        and "sMartInfo.freeItems = FALSE;" in c_function(shop, "CreatePokemartMenu")
        and "sMartInfo.freeItems = TRUE;" in c_function(shop, "CreateFreePokemartMenu"),
        "paid and free mart entry points no longer set explicit independent price modes",
    )
    print("PASS: free battle-vendor lists show only item names; paid shops retain native prices")


def verify_starter_region_cursor_memory() -> None:
    field_source = read("src/field_specials.c")
    common_source = read("data/scripts/emerald_champions.inc")
    game_corner_source = read("data/maps/MauvilleCity_GameCorner/scripts.inc")
    scrolling = c_function(field_source, "ShowScrollableMultichoice")
    common = source_region(
        common_source,
        "Common_EventScript_ChooseStarterRegion::",
        "EmeraldChampions_EventScript_StarterKanto:",
    )
    game_corner_open = source_region(
        game_corner_source,
        "MauvilleCity_GameCorner_EventScript_OpenPokemonPrizeMenu::",
        "MauvilleCity_GameCorner_EventScript_SelectGenesect::",
    )
    game_corner_region = source_region(
        game_corner_source,
        "MauvilleCity_GameCorner_EventScript_ChooseStarterRegion::",
        "MauvilleCity_GameCorner_EventScript_StarterKanto::",
    )
    regional_menus = source_region(
        game_corner_source,
        "MauvilleCity_GameCorner_EventScript_StarterKanto::",
        "MauvilleCity_GameCorner_EventScript_SelectBulbasaur::",
    )

    require(
        "case SCROLL_MULTI_STARTER_REGIONS:" in scrolling
        and "task->tNumItems = 9;" in scrolling
        and "task->tMaxItemsOnScreen = 5;" in scrolling
        and "task->tHeight = task->tMaxItemsOnScreen * 2;" in scrolling
        and "task->tScrollOffset = min(gSpecialVar_0x8005, task->tNumItems - task->tMaxItemsOnScreen);"
        in scrolling
        and "task->tSelectedRow = gSpecialVar_0x8005 - task->tScrollOffset;" in scrolling,
        "starter-region cursor memory is not converted into a valid scrolling selection",
    )
    require(
        common.index("setvar VAR_0x8005, 0")
        < common.index("EmeraldChampions_EventScript_ChooseStarterRegionMenu:")
        and "special ShowScrollableMultichoice" in common
        and "goto_if_eq VAR_RESULT, MULTI_B_PRESSED, EmeraldChampions_EventScript_ChooseStarterRegionMenu"
        in common
        and common.index("goto_if_eq VAR_RESULT, MULTI_B_PRESSED")
        < common.index("copyvar VAR_0x8005, VAR_RESULT")
        and all(f"case {index}, EmeraldChampions_EventScript_Starter" in common for index in range(9)),
        "initial starter-region selection resets memory inside its loop or stores B as a cursor",
    )
    require(
        "setvar VAR_0x8005, 0" in game_corner_open
        and "showcoinsbox 0, 0" in game_corner_open
        and "setvar VAR_0x8005, 0" not in game_corner_region
        and "special ShowScrollableMultichoice" in game_corner_region
        and game_corner_region.index("goto_if_eq VAR_RESULT, MULTI_B_PRESSED")
        < game_corner_region.index("copyvar VAR_0x8005, VAR_RESULT")
        and "MauvilleCity_GameCorner_EventScript_ChoosePokemonPrizeMessage" in game_corner_region,
        "Game Corner starter-region memory resets on re-entry or stores B as a region",
    )
    require(
        regional_menus.count("goto MauvilleCity_GameCorner_EventScript_ChooseStarterRegion") == 9,
        "a Game Corner regional submenu cannot return to the remembered region list",
    )
    for index, (region, multichoice_id) in enumerate(GAME_CORNER_REGIONAL_MENUS):
        next_label = (
            f"MauvilleCity_GameCorner_EventScript_Starter{GAME_CORNER_REGIONAL_MENUS[index + 1][0]}::"
            if index + 1 < len(GAME_CORNER_REGIONAL_MENUS)
            else "MauvilleCity_GameCorner_EventScript_SelectBulbasaur::"
        )
        block = source_region(
            game_corner_source,
            f"MauvilleCity_GameCorner_EventScript_Starter{region}::",
            next_label,
        )
        require(
            re.search(rf"(?m)^\s*multichoice\s+11,\s*0,\s*{multichoice_id},\s*FALSE\s*$", block)
            is not None,
            f"Game Corner {region} starter menu is not anchored at reviewed x=11",
        )
    require(
        "multichoice 10, 0, MULTI_GAME_CORNER_POKEMON, FALSE" in game_corner_open,
        "Game Corner root prize menu drifted from its reviewed x=10 anchor",
    )
    print("PASS: both starter-region flows remember only valid selections across native backtracking")


def verify_visible_genie_placements() -> None:
    layouts = {
        layout["id"]: layout
        for layout in json.loads(read("data/layouts/layouts.json"))["layouts"]
    }
    contracts = (
        {
            "map": "Route119",
            "species": "TORNADUS",
            "coords": (29, 8),
            "open_approaches": ((28, 8), (30, 8), (29, 7), (29, 9)),
        },
        {
            "map": "Route111_RuinsExterior",
            "species": "LANDORUS",
            "coords": (9, 10),
            "open_approaches": ((8, 10), (9, 11)),
        },
    )

    for contract in contracts:
        map_name = str(contract["map"])
        species = str(contract["species"])
        map_data = json.loads(read(f"data/maps/{map_name}/map.json"))
        objects = [
            obj
            for obj in map_data["object_events"]
            if obj.get("graphics_id") == f"OBJ_EVENT_GFX_SPECIES({species})"
        ]
        require(len(objects) == 1, f"{map_name}: expected one visible {species} object")
        obj = objects[0]
        x, y = contract["coords"]
        require(
            (obj.get("x"), obj.get("y")) == (x, y),
            f"{map_name}: {species} drifted from reviewed position {(x, y)}: {obj}",
        )
        require(
            obj.get("elevation") == 3
            and obj.get("movement_type") == "MOVEMENT_TYPE_FACE_DOWN",
            f"{map_name}: {species} no longer has the reviewed fixed overworld presentation",
        )
        duplicate_positions = [
            other
            for other in map_data["object_events"]
            if other is not obj and (other.get("x"), other.get("y")) == (x, y)
        ]
        require(
            not duplicate_positions,
            f"{map_name}: {species} shares its tile with another object: {duplicate_positions}",
        )

        layout = layouts[map_data["layout"]]
        width, height = layout["width"], layout["height"]
        blocks = (ROOT / layout["blockdata_filepath"]).read_bytes()
        require(
            len(blocks) == width * height * 2,
            f"{map_name}: executable blockdata has the wrong size",
        )

        def block(x_pos: int, y_pos: int) -> tuple[int, int]:
            require(
                0 <= x_pos < width and 0 <= y_pos < height,
                f"{map_name}: reviewed approach {(x_pos, y_pos)} is outside the layout",
            )
            offset = 2 * (y_pos * width + x_pos)
            value = int.from_bytes(blocks[offset : offset + 2], "little")
            return (value >> 10) & 3, (value >> 12) & 0xF

        require(
            block(x, y) == (0, 3),
            f"{map_name}: {species} is no longer on a passable elevation-3 block",
        )
        for approach in contract["open_approaches"]:
            require(
                block(*approach) == (0, 3),
                f"{map_name}: {species} lost its reviewed passable approach at {approach}",
            )

    generator = read("scripts/populate_restored_emerald_champions_areas.py")
    require(
        'obj("OBJ_EVENT_GFX_SPECIES(LANDORUS)", 9, 10,' in generator,
        "restored-area regeneration would move Landorus away from its reviewed tile",
    )
    print("PASS: Tornadus and Landorus retain reviewed coordinates and executable collision geometry")


def verify_physical_encounter_render_coverage() -> None:
    fixture_source = read("include/emerald_champions_headless_overworld_fixtures.h")
    fixtures = [
        {
            "index": int(index),
            "map": map_id,
            "species": species,
            "player": (int(player_x), int(player_y)),
        }
        for index, map_id, species, player_x, player_y in OVERWORLD_FIXTURE_PATTERN.findall(
            fixture_source
        )
    ]
    require(
        [fixture["index"] for fixture in fixtures] == list(range(1, 33)),
        "physical encounter fixture rows must be exactly 1..32 in reviewed order",
    )

    map_rows: dict[str, tuple[Path, dict[str, object]]] = {}
    physical_objects: list[dict[str, object]] = []
    for map_path in sorted((ROOT / "data/maps").glob("*/map.json")):
        map_data = json.loads(map_path.read_text())
        map_rows[map_data["id"]] = (map_path, map_data)
        for obj in map_data.get("object_events", []):
            match = re.fullmatch(r"OBJ_EVENT_GFX_SPECIES\(([^)]+)\)", obj.get("graphics_id", ""))
            if match is None or match.group(1) in {"CARBINK", "CHANSEY"}:
                continue
            physical_objects.append(
                {
                    "map": map_data["id"],
                    "species": f"SPECIES_{match.group(1)}",
                    "object": obj,
                }
            )

    require(len(physical_objects) == 32, "live physical one-off object count drifted from 32")
    authoritative_pairs = [(row["map"], row["species"]) for row in physical_objects]
    fixture_pairs = [(row["map"], row["species"]) for row in fixtures]
    require(
        fixture_pairs == authoritative_pairs,
        "reviewed encounter fixture rows do not exactly match live map objects:\n"
        f"fixtures={fixture_pairs}\nlive={authoritative_pairs}",
    )
    require(
        len(set(fixture_pairs)) == 32,
        "physical encounter fixture table duplicates a live map/species object",
    )

    layouts = {
        layout["id"]: layout
        for layout in json.loads(read("data/layouts/layouts.json"))["layouts"]
    }
    corrected_live_positions = {
        ("MAP_PETALBURG_WOODS_2", "SPECIES_VIRIZION"): (42, 6),
        ("MAP_RUSTBORO_CITY_DEVON_CORP_2F", "SPECIES_MAGEARNA"): (8, 7),
        ("MAP_VERDANTURF_MEADOW", "SPECIES_FEZANDIPITI"): (13, 12),
    }
    for fixture, physical in zip(fixtures, physical_objects, strict=True):
        map_path, map_data = map_rows[str(fixture["map"])]
        obj = physical["object"]
        player_x, player_y = fixture["player"]
        layout = layouts[map_data["layout"]]
        width, height = layout["width"], layout["height"]
        require(
            0 <= player_x < width and 0 <= player_y < height,
            f"fixture {fixture['index']:02} player is outside {fixture['map']}",
        )
        require(
            (player_x, player_y) != (obj["x"], obj["y"]),
            f"fixture {fixture['index']:02} puts the player on {fixture['species']}",
        )
        occupied = {
            (other.get("x"), other.get("y"))
            for other in map_data.get("object_events", [])
        }
        require(
            (player_x, player_y) not in occupied,
            f"fixture {fixture['index']:02} puts the player on another map object",
        )
        blocks = (ROOT / layout["blockdata_filepath"]).read_bytes()
        require(
            len(blocks) == width * height * 2,
            f"{map_path.parent.name}: executable blockdata has the wrong size",
        )
        offset = 2 * (player_y * width + player_x)
        block = int.from_bytes(blocks[offset : offset + 2], "little")
        require(
            ((block >> 10) & 3) == 0,
            f"fixture {fixture['index']:02} player camera tile is impassable in {fixture['map']}",
        )
        require(
            abs(player_x - obj["x"]) <= 7 and abs(player_y - obj["y"]) <= 5,
            f"fixture {fixture['index']:02} cannot frame {fixture['species']} from {fixture['player']}",
        )
        corrected_position = corrected_live_positions.get((fixture["map"], fixture["species"]))
        if corrected_position is not None:
            require(
                (obj["x"], obj["y"]) == corrected_position,
                f"reviewed live placement drifted for {fixture['species']}: {obj}",
            )
            object_offset = 2 * (obj["y"] * width + obj["x"])
            object_block = int.from_bytes(blocks[object_offset : object_offset + 2], "little")
            require(
                ((object_block >> 10) & 3, (object_block >> 12) & 0xF) == (0, 3),
                f"reviewed live placement is not passable elevation-3 for {fixture['species']}",
            )

    generator = read("scripts/populate_restored_emerald_champions_areas.py")
    require(
        'obj("OBJ_EVENT_GFX_SPECIES(VIRIZION)", 42, 6,' in generator
        and 'obj("OBJ_EVENT_GFX_SPECIES(FEZANDIPITI)", 13, 12,' in generator,
        "restored-area regeneration would undo a reviewed physical encounter placement",
    )
    remaining_generator = read("scripts/populate_remaining_legendary_quests.py")
    require(
        '("MAGEARNA", 8, 7, 58,' in remaining_generator,
        "legendary-quest regeneration would return Magearna to the clipped bottom row",
    )

    fixture_c = read("src/emerald_champions_headless.c")
    require(
        '#include "emerald_champions_headless_overworld_fixtures.h"' in fixture_c
        and "ARRAY_COUNT(sEcHeadlessOverworldFixtures) == 32" in fixture_c
        and "FlagSet(FLAG_SYS_USE_FLASH);" in fixture_c
        and "LoadHeadlessMap(fixture->map, fixture->playerX, fixture->playerY);" in fixture_c
        and "objectEvent->graphicsId != OBJ_EVENT_MON + fixture->species" in fixture_c
        and "sprite->inUse" in fixture_c
        and "SpawnSpecialObjectEventParameterized" not in fixture_c,
        "generic encounter fixture does not load and observe the authored live-map object",
    )

    renderer_globals = runpy.run_path(str(ROOT / "scripts/render_emerald_champions_ui.py"))
    scenarios = renderer_globals["SCENARIOS"]
    generic_id = renderer_globals["GENERIC_OVERWORLD_SCENARIO_ID"]
    fixture_header = read("include/emerald_champions_headless.h")
    enum_region = source_region(
        fixture_header,
        "enum EmeraldChampionsHeadlessScenario",
        "#if EC_HEADLESS_FIXTURES",
    )
    enum_names = re.findall(r"(?m)^\s*(EC_HEADLESS_SCENARIO_[A-Z0-9_]+)\s*,", enum_region)
    require(
        enum_names.index("EC_HEADLESS_SCENARIO_SPECIES_OVERWORLD") == generic_id,
        "renderer generic encounter scenario ID drifted from the compiled fixture enum",
    )
    encounter_scenarios = {
        name: spec for name, spec in scenarios.items() if name.startswith("encounter-")
    }
    expected_names = {
        f"encounter-{fixture['index']:02d}-"
        + str(fixture["species"]).removeprefix("SPECIES_").lower().replace("_", "-")
        for fixture in fixtures
    }
    require(
        set(encounter_scenarios) == expected_names,
        "renderer encounter scenario coverage is not exactly one-to-one with the reviewed fixture table",
    )
    for fixture in fixtures:
        slug = str(fixture["species"]).removeprefix("SPECIES_").lower().replace("_", "-")
        name = f"encounter-{fixture['index']:02d}-{slug}"
        spec = encounter_scenarios[name]
        require(
            spec.get("id") == generic_id
            and spec.get("param") == fixture["index"] - 1
            and spec.get("verify") is True
            and spec.get("fixture_map") == fixture["map"]
            and spec.get("fixture_species") == fixture["species"]
            and tuple(spec.get("player", ())) == fixture["player"],
            f"renderer metadata drifted for {name}: {spec}",
        )
    print("PASS: all 32 live physical one-off objects have exact reviewed generic render coverage")


def verify_headless_fixture_separation() -> None:
    makefile = read("Makefile")
    require(
        len(re.findall(r"(?m)^EC_HEADLESS_FIXTURES\s*\?=\s*0\s*$", makefile)) == 1,
        "EC_HEADLESS_FIXTURES is not defined exactly once with a production-off default",
    )
    require(
        len(re.findall(r"(?m)^CPPFLAGS\s*\+=\s*-DEC_HEADLESS_FIXTURES=\$\(EC_HEADLESS_FIXTURES\)\s*$", makefile))
        == 1,
        "the explicit fixture setting is not passed to every C translation unit",
    )
    require_headless_references_guarded("src/main.c")
    require_headless_references_guarded("src/emerald_champions_headless.c")
    require_headless_references_guarded("include/emerald_champions_headless.h")

    implementation = read("src/emerald_champions_headless.c")
    require(
        implementation.startswith('#include "global.h"\n\n#if EC_HEADLESS_FIXTURES\n')
        and implementation.rstrip().endswith("#endif // EC_HEADLESS_FIXTURES"),
        "the fixture implementation is not wholly enclosed by its test-only compile guard",
    )
    main_source = read("src/main.c")
    require(
        "#if EC_HEADLESS_FIXTURES\n    SetMainCallback2(CB2_EmeraldChampionsHeadlessFixture);\n#else\n    SetMainCallback2(gInitialMainCB2);\n#endif"
        in main_source
        and "#if EC_HEADLESS_FIXTURES\n    EmeraldChampionsHeadlessObserve();\n#endif" in main_source,
        "production callback flow is not explicitly separated from fixture setup/observation",
    )
    print("PASS: headless fixtures default off and all production callback seams compile out")


def verify_headless_renderer_contract() -> None:
    renderer = read("scripts/render_emerald_champions_ui.py")
    scenarios = python_literal_assignment(renderer, "SCENARIOS")
    require(isinstance(scenarios, dict), "SCENARIOS is not a literal dictionary")
    scenario_names = set(scenarios)
    require(
        REQUIRED_HEADLESS_SCENARIOS <= scenario_names,
        "headless renderer lost required native-UI scenarios: "
        f"{sorted(REQUIRED_HEADLESS_SCENARIOS - scenario_names)}",
    )
    scenario_ids = [spec.get("id") for spec in scenarios.values() if isinstance(spec, dict)]
    require(
        len(scenario_ids) == len(scenarios)
        and all(isinstance(scenario_id, int) and scenario_id > 0 for scenario_id in scenario_ids),
        "a headless scenario has no positive literal fixture ID",
    )
    for name in ("wild-action-menu", "move-details"):
        require(
            scenarios[name].get("verify") is True,
            f"{name} can render without runtime proof that its native battle UI was reached",
        )

    render_one = source_region(renderer, "def render_one(", "\ndef main() -> int:")
    png_validator = source_region(renderer, "def validate_screenshot_png(", "\ndef render_one(")
    require(
        'ihdr[:2] != (240, 160)' in png_validator
        and "zlib.decompress(bytes(idat))" in png_validator
        and "uniform blank screenshot" in png_validator
        and "st_size < 1000" not in renderer,
        "headless screenshots are not structurally validated as nonblank 240x160 PNG frames",
    )
    require(
        'command.extend(("--read", f"4:0x{setup_address:x}"))' in render_one
        and 'command.extend(("--read", f"4:0x{observed_address:x}"))' in render_one
        and "reads.get(setup_address) != 1 or reads.get(observed_address) != 1" in render_one
        and '"verified_runtime_state": bool(spec.get("verify"))' in render_one,
        "verified battle renders do not require and record both setup and observed runtime results",
    )
    main_region = renderer[renderer.index("def main() -> int:") :]
    require(
        'if args.scenario == "all":' in main_region
        and "names = list(SCENARIOS)" in main_region
        and 'elif args.scenario == "overworld-encounters":' in main_region
        and 'name.startswith("encounter-")' in main_region
        and '"rendered": rendered' in main_region
        and 'manifest_name = "manifest.json" if args.scenario == "all" else f"manifest.{args.scenario}.json"' in main_region
        and "manifest_path = args.out / manifest_name" in main_region,
        "the renderer can overwrite complete evidence with a focused manifest",
    )
    print(
        f"PASS: renderer preserves {len(REQUIRED_HEADLESS_SCENARIOS)} required scenarios; "
        "battle UI renders require setup and observed runtime proof"
    )


def verify_high_risk_composed_screen_fixtures() -> None:
    fixture = read("src/emerald_champions_headless.c")
    title = read("src/title_screen.c")
    scenarios = runpy.run_path(str(ROOT / "scripts/render_emerald_champions_ui.py"))["SCENARIOS"]

    for name in (
        "double-status-ability",
        "mega-ready",
        "mega-active",
        "opposing-primals",
        "safari-action",
    ):
        require(scenarios[name].get("verify") is True, f"{name} lacks runtime-state verification")
    require(
        scenarios["double-status-ability"].get("stop_on_observed") is True
        and scenarios["double-status-ability"].get("trigger_frame") is not None,
        "Ability popup capture is not synchronized to its visible idle position",
    )
    require(
        all(
            token in fixture
            for token in (
                "CreateAbilityPopUp(B_BATTLER_0, ABILITY_INTIMIDATE, TRUE);",
                "gBattleStruct->abilityPopUpSpriteIds[B_BATTLER_0][0]",
                "gSprites[left].x + gSprites[left].x2 == 24",
                "ITEM_CHARIZARDITE_X",
                "SPECIES_CHARIZARD_MEGA_X",
                "SPECIES_KYOGRE_PRIMAL",
                "SPECIES_GROUDON_PRIMAL",
                "BATTLE_TYPE_SAFARI",
                "ChooseMonToGivePokeblock",
                "ShowPlayerTrainerCard",
                "MON_DATA_HELD_ITEM",
                "BATTLE_DOME_FUNC_INIT_TRAINERS",
                "BATTLE_DOME_FUNC_SHOW_OPPONENT_INFO",
                "ShowContestResults();",
                "PlaySlotMachine(0, gInitialMainCB2);",
                "SPECIES_SYLVEON",
            )
        ),
        "high-risk composed fixture lost a native state or visible-sprite proof",
    )
    require(
        "gEcHeadlessFixtureActiveScenario != EC_HEADLESS_SCENARIO_TITLE" in title
        and "#if EC_HEADLESS_FIXTURES" in title,
        "title fixture no longer suppresses the non-release Quickstart HUD test-only",
    )
    require(
        all(
            name in scenarios
            for name in (
                "storage-box-popup",
                "storage-move-items",
                "frontier-pass-map",
                "title-live",
                "birch-introduction",
                "pokeblock-condition",
                "trainer-card-gold",
                "battle-dome-info-card",
                "contest-results",
                "slot-machine",
                "fairy-summary-info",
                "fairy-summary-moves",
            )
        ),
        "a reviewed high-risk composed screen lost deterministic renderer coverage",
    )
    print("PASS: high-risk battle, boot, Pokeblock, Storage, Frontier, and Trainer Card fixtures remain composed")


DYNA_MENU = re.compile(
    r"(?m)^\s*dynmultistack\s+([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),"
)


def parse_script_integer(token: str, *, path: Path, line: int) -> int:
    token = token.strip()
    try:
        return int(token, 0)
    except ValueError:
        fail(f"{path.relative_to(ROOT)}:{line}: dynamic-menu geometry is not a literal: {token}")


def verify_dynamic_menus() -> None:
    violations: list[str] = []
    scanned = 0
    battle_categories_verified = False

    for path in sorted((ROOT / "data").rglob("*.inc")):
        if "debug" in {part.lower() for part in path.parts} or path.name.lower() == "debug.inc":
            continue
        source = path.read_text()
        for match in DYNA_MENU.finditer(source):
            # Assembly comments begin with '@'.  A commented example is not a
            # live menu and must not satisfy (or fail) this release contract.
            line_start = source.rfind("\n", 0, match.start()) + 1
            line_text = source[line_start : source.find("\n", match.start())]
            if line_text.lstrip().startswith("@"):
                continue
            line = source.count("\n", 0, match.start()) + 1
            left = parse_script_integer(match.group(1), path=path, line=line)
            top = parse_script_integer(match.group(2), path=path, line=line)
            max_visible = parse_script_integer(match.group(4), path=path, line=line)
            if max_visible == 0xFF:
                max_visible = 6
            scanned += 1

            # CreateWindowFromRect shifts the interior down one tile and the
            # standard frame occupies one tile above and below it.  Therefore
            # the final frame row is top + 2*max_visible + 1.  Row 14 begins
            # Emerald's bottom message box.
            frame_bottom = top + 2 * max_visible + 1
            if left < 0 or top < 0 or max_visible <= 0:
                violations.append(
                    f"{path.relative_to(ROOT)}:{line}: invalid dynamic-menu geometry"
                )
            elif frame_bottom >= MESSAGE_BOX_TOP:
                violations.append(
                    f"{path.relative_to(ROOT)}:{line}: {max_visible} visible rows at top={top} "
                    f"draw through tile row {frame_bottom}, overlapping the bottom textbox at row {MESSAGE_BOX_TOP}"
                )

            if path.relative_to(ROOT).as_posix() == "data/scripts/emerald_champions.inc":
                prefix = source[max(0, match.start() - 900) : match.start()]
                if "EmeraldChampions_EventScript_BattleItemCategories:" in prefix:
                    require(
                        max_visible <= 5,
                        f"battle-item category menu exposes {max_visible} rows; native limit is 5",
                    )
                    battle_categories_verified = True

    require(scanned > 0, "no live non-debug dynmultistack menus were found")
    require(battle_categories_verified, "battle-item category dynmultistack was not verified")
    require(not violations, "dynamic menu/textbox overlap risks:\n" + "\n".join(violations))
    print(
        f"PASS: {scanned} live non-debug dynamic menus stay above the bottom textbox; "
        "battle categories show at most 5 rows"
    )


def main() -> None:
    dimensions = verify_center_layouts()
    verify_party_visual_assets()
    verify_center_geometry(dimensions)
    verify_league_tutor()
    verify_ability_selector()
    verify_battle_interface_sprite_guards()
    verify_leveler_batch_flow()
    verify_battle_set_preselection()
    verify_unified_move_list_width()
    verify_battle_vendor_navigation()
    verify_free_battle_vendor_list()
    verify_starter_region_cursor_memory()
    verify_visible_genie_placements()
    verify_physical_encounter_render_coverage()
    verify_headless_fixture_separation()
    verify_headless_renderer_contract()
    verify_high_risk_composed_screen_fixtures()
    verify_dynamic_menus()
    print("EMERALD CHAMPIONS NATIVE UI GATE: PASS")


if __name__ == "__main__":
    main()
