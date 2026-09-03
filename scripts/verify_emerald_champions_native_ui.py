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
    "nurse-heal-facing-machine",
    "nurse-heal-tray",
    "nurse-heal-return",
    "whiteout-heal-placement",
    "whiteout-heal-league-placement",
    "whiteout-heal-lavaridge-placement",
    "trainer-hill-nurse-heal-placement",
    "ability-menu",
    "ability-back-to-actions",
    "ability-cancel-to-actions",
    "ability-applied-message",
    "ability-applied-return",
    "party-overview",
    "party-action-menu",
    "options",
    "battle-vendor",
    "battle-vendor-category-back",
    "battle-vendor-postbadge-root",
    "battle-vendor-postbadge-held-items",
    "battle-vendor-shop",
    "battle-vendor-quantity",
    "battle-vendor-quantity-adjusted",
    "battle-vendor-quantity-back",
    "battle-vendor-confirm",
    "battle-vendor-confirm-no",
    "battle-vendor-purchase-success",
    "battle-vendor-purchase-return",
    "move-specialist-root",
    "move-specialist-root-back",
    "move-specialist-party-prompt",
    "move-specialist-battle-set-party",
    "move-specialist-party-back",
    "battle-set-format",
    "move-specialist-learn-move-party",
    "move-specialist-learn-move-back",
    "move-specialist-forget-intro",
    "move-specialist-forget-decline",
    "move-specialist-forget-party",
    "move-specialist-forget-party-back",
    "move-specialist-rename-prompt",
    "move-specialist-rename-party",
    "move-specialist-rename-back",
    "battle-set-list",
    "battle-set-singles-list",
    "battle-set-list-back",
    "battle-set-confirm",
    "battle-set-confirm-no",
    "battle-set-applied",
    "stat-point-party",
    "stat-point-egg-rejected",
    "stat-point-external-entry",
    "stat-point-external-exit",
    "stat-point-list",
    "stat-point-list-scrolled",
    "stat-point-adjust-list",
    "stat-point-adjust-scrolled",
    "stat-point-adjusted",
    "stat-point-boundary-feedback",
    "stat-point-adjust-back",
    "stat-point-list-back",
    "stat-point-reset-confirm",
    "stat-point-reset-no",
    "stat-point-reset-yes",
    "stat-point-reset-zero-list",
    "all-legal-moves",
    "all-legal-moves-direct",
    "all-legal-move-selected",
    "all-legal-move-selected-back",
    "all-legal-move-confirmed",
    "all-legal-move-give-up",
    "all-legal-move-give-up-no",
    "all-legal-moves-mew",
    "all-legal-moves-mew-middle",
    "all-legal-moves-mew-final",
    "all-legal-hm-replacement",
    "dewford-gym-entry",
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
    "wild-foe-types",
    "move-details",
    "move-foe-types",
    "pokedex",
    "pokedex-info",
    "pokedex-area",
    "pokedex-stats",
    "pokedex-evolutions",
    "pokedex-forms",
    "pokedex-cry",
    "pokedex-size",
    "pokedex-search",
    "pokedex-search-results",
    "summary-info",
    "summary-skills",
    "summary-moves",
    "summary-contest-moves",
    "summary-move-detail",
    "summary-party-roundtrip",
    "bag",
    "bag-items",
    "bag-medicine",
    "bag-tms-hms",
    "bag-berries",
    "bag-poke-balls",
    "bag-key-items",
    "bag-mega-stones",
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
    "magma-sparkle-placement",
    "furfrou-trims",
    "furfrou-trims-scrolled",
    "furfrou-trims-b-cancel",
    "furfrou-trims-back",
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
    # Asset hashes for graphics/party_menu are owned by verify_inclement_visual_sources.py.
    graphics = read("src/graphics.c")
    require(
        'INCGFX_U16("graphics/party_menu/bg.pal", ".gbapal")' in graphics,
        "party menu is not loading the reviewed 176-color Verdant palette",
    )
    require(
        'INCGFX_U16("graphics/party_menu/bg.png", ".gbapal")' not in graphics,
        "party menu incorrectly derives one 16-color bank from the restored indexed PNG",
    )
    print("PASS: party skin loads the full 11-bank palette")


def verify_inclement_copy_and_dex_numbering() -> None:
    strings = read("src/strings.c")
    party = read("src/party_menu.c")
    fixture = read("src/emerald_champions_headless.c")
    pokedex = read("src/pokedex.c")
    dex_number = c_function(pokedex, "CreateMonDexNum")
    dex_number_print = c_function(pokedex, "PrintMonDexNum")
    dex_name = c_function(pokedex, "PrintMonName")
    dex_ball = c_function(pokedex, "CreateCaughtBall")

    for literal in (
        'const u8 gText_ChoosePokemon[] = _("Choose a Pokémon.");',
        'const u8 gText_ChoosePokemon2[] = _("Choose a Pokémon.");',
        'const u8 gText_ChoosePokemonCancel[] = _("Choose Pokémon or Cancel.");',
        'const u8 gText_ChoosePokemonConfirm[] = _("Choose Pokémon and confirm.");',
        'const u8 gText_Cancel[] = _("Cancel");',
        'const u8 gText_Cancel2[] = _("Cancel");',
        'const u8 gText_None[] = _("None");',
        'const u8 gText_CloseBag[] = _("Close Bag");',
        'const u8 gText_Info[] = _("Info");',
    ):
        require(literal in strings, f"Inclement UI copy drifted: {literal}")

    menu_data = read("src/data/party_menu.h")
    shouting = [
        label for label in re.findall(r'COMPOUND_STRING\("([^"]+)"\)', menu_data)
        if label.isupper() and len(label) > 1
    ]
    require(not shouting, f"party action labels regressed to all-caps copy: {shouting}")
    for literal in (
        'const u8 gMenuText_Use[] = _("Use");',
        'const u8 gMenuText_Toss[] = _("Toss");',
        'const u8 gMenuText_Register[] = _("Register");',
        'const u8 gMenuText_Give[] = _("Give");',
        'const u8 gText_MenuPokedex[] = _("Pokédex");',
        'const u8 gText_MenuBag[] = _("Bag");',
        'const u8 gText_MenuExit[] = _("Exit");',
        '[POCKET_BATTLE]      = COMPOUND_STRING("Battle Items"),',
    ):
        require(literal in strings, f"Start Menu/Bag copy regressed to all-caps: {literal}")
    require(
        "GetFontIdToFit(pocketName1, FONT_NORMAL, 0, 0x40)" in read("src/item_menu.c"),
        "Bag pocket names no longer fit long Inclement names into their 64px strip",
    )
    require(
        'static const u8 sText_CancelTitleCase[] = _("Cancel");' in party
        and party.count("sText_CancelTitleCase") >= 4,
        "party Cancel labels no longer use the Inclement title-case copy",
    )
    require(
        "SPECIES_GEODUDE" in c_function(fixture, "PrepareAbilityMenu")
        and "SPECIES_GARDEVOIR" in c_function(fixture, "PrepareAbilityMenu")
        and "SPECIES_PIKACHU" in c_function(fixture, "PrepareAbilityMenu"),
        "party fixture no longer proves Ability alongside inherited multi-mon Switch behavior",
    )
    require(
        "u16 offset = 2;" in dex_number
        and "if (dexNum > 999)" in dex_number
        and "memcpy(text, sText_No000, ARRAY_COUNT(sText_No000));" in dex_number
        and "memcpy(text, sText_No0000, ARRAY_COUNT(sText_No0000));" in dex_number,
        "Pokedex list no longer uses Inclement No001 formatting with a four-digit extension",
    )
    require(
        "left * 8" in dex_number_print
        and "xOffset" not in dex_number_print
        and "left * 8" in dex_name
        and "xOffset" not in dex_name
        and "x * 8" in dex_ball
        and "xMultiplier" not in dex_ball,
        "Pokedex list rows no longer use Inclement's exact number, name, and caught-ball origins",
    )
    print("PASS: inherited party/Summary/Bag copy and Pokedex No001 numbering match Inclement")


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
    heal_locations = json.loads(read("src/data/heal_locations.json"))["heal_locations"]
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
        whiteout_entries = [
            entry for entry in heal_locations
            if entry.get("respawn_map") == data.get("id")
        ]
        require(
            len(whiteout_entries) == 1,
            f"{map_name}: expected one whiteout destination, found {len(whiteout_entries)}",
        )
        whiteout = whiteout_entries[0]
        require(
            whiteout.get("respawn_x", 7) == nurse.get("x")
            and whiteout.get("respawn_y", 4) == 4,
            f"{map_name}: whiteout camera does not align its screen-fixed healing effect: "
            f"respawn=({whiteout.get('respawn_x', 7)},{whiteout.get('respawn_y', 4)}) "
            f"nurse=({nurse.get('x')},{nurse.get('y')})",
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

    print("PASS: all 16 Centers have collision-free native geometry and whiteout camera alignment")


def verify_league_tutor() -> None:
    relative = "data/maps/EverGrandeCity_PokemonLeague_1F/map.json"
    data = json.loads(read(relative))
    objects = data.get("object_events", [])
    heal_locations = json.loads(read("src/data/heal_locations.json"))["heal_locations"]
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
    nurse = next(
        (obj for obj in objects if obj.get("local_id") == "LOCALID_LEAGUE_NURSE"),
        None,
    )
    whiteout = next(
        (
            entry for entry in heal_locations
            if entry.get("respawn_map") == "MAP_EVER_GRANDE_CITY_POKEMON_LEAGUE_1F"
        ),
        None,
    )
    require(nurse is not None and whiteout is not None, "Pokemon League whiteout anchor is missing")
    require(
        whiteout.get("respawn_x", 7) == nurse.get("x")
        and whiteout.get("respawn_y", 4) == 4,
        "Pokemon League whiteout camera does not align with its nurse/healing machine",
    )
    print("PASS: the League lobby retains native services and aligned whiteout healing")


def verify_ability_selector() -> None:
    source = read("src/party_menu.c")
    menu_data = read("src/data/party_menu.h")
    display = c_function(source, "DisplayAbilitySelectionWindow")
    handle = c_function(source, "Task_HandleAbilitySelectionInput")
    field_actions = c_function(source, "SetPartyMonFieldSelectionActions")

    require(
        "GetStringWidth(FONT_NORMAL" in display
        and re.search(r"SetWindowTemplateFields\(&window,\s*2,\s*29 - windowWidth,", display) is not None
        and "sText_CancelTitleCase" in display
        and "InitMenuInUpperLeftCorner" in display,
        "Ability list is not a measured, right-aligned native list window",
    )
    require(
        "SetMonData(mon, MON_DATA_ABILITY_NUM" in handle
        and "CreateYesNoMenu" not in handle
        and "ReturnToPartyActionMenu(taskId);" in handle,
        "Ability selection is not a direct one-step apply that returns to the action menu",
    )
    require(
        "MENU_OPEN_ABILITY" in field_actions and 'COMPOUND_STRING("Ability")' in menu_data,
        "the native Pokemon action menu does not expose the Ability selector",
    )
    print("PASS: Ability selection is direct, measured, and returns in place")


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
    apply_item = c_function(source, "ItemUseCB_RareCandy")
    continue_evolution = c_function(source, "CB2_ContinueLevelerEvolution")

    require(
        "GetCurrentLevelCap()" in find_next and "MON_DATA_IS_EGG" in find_next,
        "Leveler batching does not stop at the cap or skip Eggs",
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
        "gCB2_AfterEvolution = CB2_ContinueLevelerEvolution;" in continue_evolution,
        "Leveler evolution chaining does not return to the same party batch",
    )
    print("PASS: Leveler batches to the cap, skips Eggs, and chains evolutions")


def verify_battle_set_preselection() -> None:
    script = read("data/scripts/emerald_champions.inc")
    tests = read("test/emerald_champions.c")
    battle_sets = json.loads(read("docs/emerald_champions_battle_sets.json"))
    story_width_gate = runpy.run_path(str(ROOT / "scripts/verify_emerald_champions_story.py"))
    widths = story_width_gate["font_widths"]()
    glyphs = story_width_gate["glyph_codes"]()

    # Recognition of the current orientation is proven by the runtime test
    # pinned below; this gate keeps only the script contract and text widths.
    choose_style = source_region(
        script,
        "EmeraldChampions_EventScript_BattleSetChooseFormat:",
        "EmeraldChampions_EventScript_BattleSetConfirm:",
    )
    require(
        "dynmultistack 30, 1, FALSE, 3, 0, VAR_0x8007" in choose_style
        and "special BufferSelectedMonCurrentEmeraldChampionsBattleSet" in choose_style
        and "goto_if_ge VAR_0x8006, VAR_RESULT, EmeraldChampions_EventScript_BattleSetChooseFormat"
        in choose_style,
        "battle-set script does not preserve current-choice preselection and reject Exit/B safely",
    )
    set_names = {
        entry["name"]
        for key in ("defaults", "alternatives", "singles_defaults", "singles_alternatives")
        for entry in battle_sets[key]
    }
    unknown = sorted({char for name in set_names for char in name if char not in glyphs})
    require(not unknown, f"battle-set menu names use unsupported normal-font glyphs: {unknown}")
    menu_widths = {
        name: sum(widths[glyphs[char]] for char in name)
        for name in set_names
    }
    require(
        max(menu_widths.values()) <= 160,
        "battle-set name exceeds the reviewed native scrolling-menu width: "
        f"{max(menu_widths, key=menu_widths.get)}={max(menu_widths.values())}px",
    )
    confirmation_widths = {
        name: sum(widths[glyphs[char]] for char in f"Apply {name} to")
        for name in set_names
    }
    require(
        max(confirmation_widths.values()) <= 216,
        "battle-set confirmation can overflow the native dialogue box: "
        f"{max(confirmation_widths, key=confirmation_widths.get)}="
        f"{max(confirmation_widths.values())}px",
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
        "checkitem ITEM_MEGA_RING, 1" in main and "goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_BattleVendorCompleteArchive" in main
        and "goto EmeraldChampions_EventScript_BattleItemCategoryChoices" in main,
        "pre-League battle vendor has an obsolete wrapper menu or wrong archive gate",
    )
    require(
        "dynmultistack 30, 1, FALSE, 4, 0, VAR_0x8009, DYN_MULTICHOICE_CB_NONE" in archive
        and archive.index("goto_if_eq VAR_RESULT, MULTI_B_PRESSED")
        < archive.index("copyvar VAR_0x8009, VAR_RESULT")
        and "case 0, EmeraldChampions_EventScript_BattleItemCategories" in archive
        and "case 1, EmeraldChampions_EventScript_OpenMegaArchive" in archive
        and "case 2, EmeraldChampions_EventScript_OpenEvolutionArchive" in archive,
        "post-Badge vendor cursor/B flow can store an invalid result or route to the wrong archive",
    )
    require(
        "dynmultistack 30, 1, FALSE, 4, 0, VAR_0x8008, DYN_MULTICHOICE_CB_NONE" in categories
        and categories.index("goto_if_eq VAR_RESULT, MULTI_B_PRESSED")
        < categories.index("copyvar VAR_0x8008, VAR_RESULT")
        and all(
            f"case {index}, EmeraldChampions_EventScript_Open{name}Items" in categories
            for index, name in enumerate(("Offense", "Defense", "Field", "Type", "Gem", "Species"))
        )
        and "goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_BattleVendorMain" in categories
        and "goto EmeraldChampions_EventScript_BattleVendorExit" in categories,
        "battle-item categories do not preserve a valid cursor or implement native B/Back behavior",
    )
    require(
        item_returns.count("goto EmeraldChampions_EventScript_BattleItemCategories") == 6
        and item_returns.count("goto EmeraldChampions_EventScript_BattleVendorMain") == 2,
        "battle vendor subshops do not return to the menu that opened them",
    )
    print("PASS: battle vendor keeps independent cursors and B/Back returns one native level")


def verify_move_specialist_navigation() -> None:
    source = read("data/scripts/emerald_champions.inc")
    root = source_region(
        source,
        "Common_EventScript_EmeraldChampionsMoveTutor::",
        "EmeraldChampions_EventScript_OpenMoveRelearner:",
    )
    learn = source_region(
        source,
        "EmeraldChampions_EventScript_OpenMoveRelearner:",
        "EmeraldChampions_EventScript_OpenMoveDeleter:",
    )
    delete = source_region(
        source,
        "EmeraldChampions_EventScript_OpenMoveDeleter:",
        "EmeraldChampions_EventScript_OpenNameRater:",
    )
    rename = source_region(
        source,
        "EmeraldChampions_EventScript_OpenNameRater:",
        "Common_EventScript_EmeraldChampionsStatPointEditor::",
    )
    sets = source_region(
        source,
        "EmeraldChampions_EventScript_BattleSetChooseMon:",
        "EmeraldChampions_EventScript_MoveTutorExit:",
    )

    require(
        "EmeraldChampions_EventScript_MoveTutorMain:" in root
        and "dynmultistack 30, 1, FALSE, 5, 0, 0, DYN_MULTICHOICE_CB_NONE" in root
        and "EmeraldChampions_Text_ChangeNature" in root
        and "EmeraldChampions_Text_EditStatPoints" in root
        and "EmeraldChampions_Text_OtherServices" in root
        and "case MULTI_B_PRESSED, EmeraldChampions_EventScript_MoveTutorExit" in root,
        "move specialist root is not a right-anchored native menu with an explicit B exit",
    )
    require(
        "setmoverelearnerstate MOVE_RELEARNER_ALL_MOVES" in learn
        and "goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, EmeraldChampions_EventScript_MoveTutorMain" in learn
        and "goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_MoveTutorMain" in learn
        and "goto EmeraldChampions_EventScript_MoveTutorMain" in learn
        and "releaseall" not in learn,
        "Learn a Move can still eject the player instead of backing up to the specialist root",
    )
    require(
        "goto_if_eq VAR_RESULT, NO, EmeraldChampions_EventScript_OtherServices" in delete
        and "goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, EmeraldChampions_EventScript_OtherServices" in delete
        and "releaseall" not in delete,
        "Forget a Move does not preserve one-level native backtracking",
    )
    require(
        "goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, EmeraldChampions_EventScript_OtherServices" in rename
        and "goto_if_eq VAR_RESULT, NO, EmeraldChampions_EventScript_OtherServices" in rename
        and "releaseall" not in rename,
        "Rename Pokemon does not preserve one-level native backtracking",
    )
    require(
        "goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, EmeraldChampions_EventScript_MoveTutorMain" in sets
        and "EmeraldChampions_EventScript_BattleSetChooseFormat:" in sets
        and "EC_BATTLE_FORMAT_DOUBLES" in sets
        and "EC_BATTLE_FORMAT_SINGLES" in sets
        and "goto EmeraldChampions_EventScript_BattleSetChooseStyle" in sets
        and "goto EmeraldChampions_EventScript_MoveTutorMain" in sets,
        "battle-set cancel/decline/success flow no longer returns to the correct native level",
    )
    print("PASS: every move-specialist branch has explicit one-level cancel/back behavior")


def verify_stat_point_editor() -> None:
    script = read("data/scripts/emerald_champions.inc")
    field = read("src/field_specials.c")
    constants = read("include/constants/field_specials.h")
    sets = read("src/emerald_champions_battle_sets.c")
    fallarbor = json.loads(read("data/maps/FallarborTown_MoveRelearnersHouse/map.json"))
    editor = source_region(
        script,
        "Common_EventScript_EmeraldChampionsStatPointEditor::",
        "EmeraldChampions_EventScript_BattleSetChooseMon:",
    )
    scrolling = c_function(field, "ShowScrollableMultichoice")

    require(
        "SCROLL_MULTI_EMERALD_CHAMPIONS_STAT_POINTS" in constants
        and "SCROLL_MULTI_EMERALD_CHAMPIONS_STAT_ADJUST" in constants
        and "case SCROLL_MULTI_EMERALD_CHAMPIONS_STAT_POINTS:" in scrolling
        and "case SCROLL_MULTI_EMERALD_CHAMPIONS_STAT_ADJUST:" in scrolling
        and scrolling.count("task->tMaxItemsOnScreen = 4;") >= 2,
        "Stat Point menus are not bounded four-row native scrolling lists",
    )
    require(
        "special ChoosePartyMon" in editor
        and "special BufferSelectedMonEmeraldChampionsStatPointSummary" in editor
        and "special BufferSelectedMonEmeraldChampionsStatPointDetail" in editor
        and "special AdjustSelectedMonEmeraldChampionsStatPoints" in editor
        and "special ResetSelectedMonEmeraldChampionsStatPoints" in editor
        and "goto_if_eq VAR_RESULT, MULTI_B_PRESSED" in editor
        and "EmeraldChampions_EventScript_StatPointExternalExit:" in editor,
        "Stat Point editor lacks the complete party/summary/adjust/reset/back hierarchy",
    )
    # The 0-32 / 66 clamps and live recalculation are proven at runtime by
    # "Emerald Champions Stat Point editor clamps every spread to 32 and 66".
    require(
        "EmeraldChampions_EventScript_StatPointAdjustmentBlocked:" in editor
        and "playse SE_FAILURE" in editor,
        "blocked Stat Point adjustments do not provide native failure feedback",
    )
    require(
        "perfectIv = MAX_PER_STAT_IVS" in sets
        and "MON_DATA_HP_IV + stat" in sets,
        "battle sets no longer normalize the hidden legacy IV storage to perfect Champions potential",
    )
    require(
        any(
            obj.get("script") == "Common_EventScript_EmeraldChampionsStatPointEditor"
            for obj in fallarbor["object_events"]
        ),
        "Fallarbor's restored training NPC does not open the canonical Stat Point editor",
    )
    print("PASS: free Stat Point editing is native, bounded to 66/32, and shared with Fallarbor")


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
        and "sMartInfo.freeItems = TRUE;" in c_function(shop, "CreateFreePokemartMenu")
        and "sMartInfo.martType = MART_TYPE_NORMAL;" in c_function(shop, "CreateFreePokemartMenu")
        and "CreateShopMenu" not in c_function(shop, "CreateFreePokemartMenu")
        and "Task_OpenFreeCatalog" in c_function(shop, "CreateFreePokemartMenu"),
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
    definitions = read("src/data/pokemon/legendary_signs.h")
    contracts = (
        ("Route110", "THUNDURUS", "LEGENDARY_SIGN_THUNDURUS"),
        ("Route111_RuinsExterior", "LANDORUS", "LEGENDARY_SIGN_LANDORUS"),
        ("Route119", "TORNADUS", "LEGENDARY_SIGN_TORNADUS"),
    )
    for map_name, species, sign_id in contracts:
        map_data = json.loads(read(f"data/maps/{map_name}/map.json"))
        require(
            all(species not in obj.get("graphics_id", "") for obj in map_data["object_events"]),
            f"{map_name}: {species} regressed into a permanent route-side body",
        )
        require(
            f"WILD_SIGN({sign_id}, {species}," in definitions,
            f"{species} is no longer a conditional Legendary Sign",
        )
    print("PASS: the Forces of Nature are conditional encounters, never permanent route props")


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
        [fixture["index"] for fixture in fixtures] == list(range(1, 9)),
        "physical encounter fixture rows must be exactly Inclement's 1..8 set",
    )

    map_rows: dict[str, tuple[Path, dict[str, object]]] = {}
    physical_objects: list[dict[str, object]] = []
    inclement_gfx_species = {
        "OBJ_EVENT_GFX_INCLEMENT_ARTICUNO": "ARTICUNO",
        "OBJ_EVENT_GFX_INCLEMENT_ZAPDOS": "ZAPDOS",
        "OBJ_EVENT_GFX_INCLEMENT_MOLTRES": "MOLTRES",
        "OBJ_EVENT_GFX_INCLEMENT_MEWTWO": "MEWTWO",
        "OBJ_EVENT_GFX_INCLEMENT_JIRACHI": "JIRACHI",
        "OBJ_EVENT_GFX_INCLEMENT_HEATRAN": "HEATRAN",
        "OBJ_EVENT_GFX_REGIGIGAS_STATUE": "REGIGIGAS",
        "OBJ_EVENT_GFX_INCLEMENT_DIANCIE": "DIANCIE",
    }
    for map_path in sorted((ROOT / "data/maps").glob("*/map.json")):
        map_data = json.loads(map_path.read_text())
        map_rows[map_data["id"]] = (map_path, map_data)
        for obj in map_data.get("object_events", []):
            graphics_id = obj.get("graphics_id", "")
            species = inclement_gfx_species.get(graphics_id)
            match = re.fullmatch(r"OBJ_EVENT_GFX_SPECIES\(([^)]+)\)", graphics_id)
            if species is None and match is not None and match.group(1) not in {"CARBINK", "CHANSEY"}:
                species = match.group(1)
            if species is None:
                continue
            physical_objects.append(
                {
                    "map": map_data["id"],
                    "species": f"SPECIES_{species}",
                    "object": obj,
                }
            )

    require(len(physical_objects) == 8, "live physical object roster drifted from Inclement's eight")
    authoritative_pairs = [(row["map"], row["species"]) for row in physical_objects]
    fixture_pairs = [(row["map"], row["species"]) for row in fixtures]
    require(
        fixture_pairs == authoritative_pairs,
        "reviewed encounter fixture rows do not exactly match live map objects:\n"
        f"fixtures={fixture_pairs}\nlive={authoritative_pairs}",
    )
    require(
        len(set(fixture_pairs)) == 8,
        "physical encounter fixture table duplicates a live map/species object",
    )

    layouts = {
        layout["id"]: layout
        for layout in json.loads(read("data/layouts/layouts.json"))["layouts"]
    }
    corrected_live_positions = {
        ("MAP_ALTERING_CAVE_B1F", "SPECIES_MEWTWO"): (7, 13),
        ("MAP_CAVE_OF_ORIGIN_DIANCIES_ROOM", "SPECIES_DIANCIE"): (9, 9),
        ("MAP_EMBER_PATH", "SPECIES_MOLTRES"): (21, 14),
        ("MAP_METEOR_FALLS_JIRACHIS_ROOM", "SPECIES_JIRACHI"): (7, 6),
        ("MAP_NEW_MAUVILLE_INSIDE", "SPECIES_ZAPDOS"): (33, 15),
        ("MAP_SCORCHED_SLAB_HEATRANS_ROOM", "SPECIES_HEATRAN"): (10, 12),
        ("MAP_SEALED_CHAMBER_INNER_ROOM", "SPECIES_REGIGIGAS"): (10, 12),
        ("MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM", "SPECIES_ARTICUNO"): (8, 8),
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
                ((object_block >> 10) & 3) == 0,
                f"reviewed live placement is not passable for {fixture['species']}",
            )

            # A cinematic screenshot is insufficient if the live object cannot
            # actually be challenged.  Prove that at least one cardinal
            # interaction tile is both clear and reachable from a map warp.
            def passable(x: int, y: int) -> bool:
                if not (0 <= x < width and 0 <= y < height):
                    return False
                tile_offset = 2 * (y * width + x)
                tile = int.from_bytes(blocks[tile_offset : tile_offset + 2], "little")
                return ((tile >> 10) & 3) == 0

            interaction_tiles = {
                (obj["x"] + dx, obj["y"] + dy)
                for dx, dy in ((0, -1), (1, 0), (0, 1), (-1, 0))
                if passable(obj["x"] + dx, obj["y"] + dy)
                and (obj["x"] + dx, obj["y"] + dy) not in occupied
            }
            require(
                interaction_tiles,
                f"{fixture['species']} has no clear cardinal interaction tile",
            )
            frontier = {
                (warp["x"], warp["y"])
                for warp in map_data.get("warp_events", [])
                if passable(warp["x"], warp["y"])
            }
            require(frontier, f"{fixture['map']} has no passable warp entry tile")
            visited = set(frontier)
            while frontier and not (visited & interaction_tiles):
                next_frontier = set()
                for x, y in frontier:
                    for dx, dy in ((0, -1), (1, 0), (0, 1), (-1, 0)):
                        candidate = (x + dx, y + dy)
                        if (
                            candidate not in visited
                            and candidate not in occupied
                            and passable(*candidate)
                        ):
                            visited.add(candidate)
                            next_frontier.add(candidate)
                frontier = next_frontier
            # Ember Path's Inclement-original route crosses elevation/Strength
            # transitions that a flat collision flood-fill cannot model.  Its
            # object and cardinal interaction tile are still checked above,
            # and the entire live map remains byte-identical to Inclement.
            scripted_paths = {
                # Elevation plus movable Strength boulders.
                ("MAP_EMBER_PATH", "SPECIES_MOLTRES"),
                # Dynamic blue/green barrier switches.
                ("MAP_NEW_MAUVILLE_INSIDE", "SPECIES_ZAPDOS"),
            }
            scripted_path = (fixture["map"], fixture["species"]) in scripted_paths
            require(
                visited & interaction_tiles or scripted_path,
                f"{fixture['species']} cannot be reached and interacted with from a map warp",
            )

    generator = read("scripts/populate_restored_emerald_champions_areas.py")
    require(
        'OBJ_EVENT_GFX_INCLEMENT_MEWTWO' in generator
        and 'OBJ_EVENT_GFX_INCLEMENT_DIANCIE' in generator
        and 'OBJ_EVENT_GFX_INCLEMENT_MOLTRES' in generator
        and 'OBJ_EVENT_GFX_INCLEMENT_JIRACHI' in generator
        and 'OBJ_EVENT_GFX_INCLEMENT_HEATRAN' in generator
        and all(
            f'OBJ_EVENT_GFX_SPECIES({species})' not in generator
            for species in (
                "HOOPA", "OKIDOGI", "TERAPAGOS", "MELOETTA", "MUNKIDORI",
                "COSMOG", "VIRIZION", "WO_CHIEN", "LANDORUS", "ZYGARDE",
                "ENAMORUS", "FEZANDIPITI", "CELEBI", "RESHIRAM", "PALKIA", "SHAYMIN",
            )
        ),
        "restored-area regeneration would drift from Inclement's physical roster",
    )
    remaining_generator = read("scripts/populate_remaining_legendary_quests.py")
    require(
        'OBJ_EVENT_GFX_INCLEMENT_ARTICUNO' in remaining_generator
        and 'OBJ_EVENT_GFX_INCLEMENT_ZAPDOS' in remaining_generator
        and 'OBJ_EVENT_GFX_REGIGIGAS_STATUE' in remaining_generator
        and 'obj("PECHARUNT",' not in remaining_generator
        and '("MELTAN",' not in remaining_generator
        and '("MAGEARNA",' not in remaining_generator,
        "retained-map regeneration would restore a rejected free-standing encounter",
    )

    fixture_c = read("src/emerald_champions_headless.c")
    require(
        '#include "emerald_champions_headless_overworld_fixtures.h"' in fixture_c
        and "ARRAY_COUNT(sEcHeadlessOverworldFixtures) == 8" in fixture_c
        and "PrepareHeadlessOverworldFixtureState" in fixture_c
        and "GetHeadlessOverworldFixtureGraphicsId" in fixture_c
        and "FlagSet(FLAG_SYS_USE_FLASH);" in fixture_c
        and "LoadHeadlessMap(fixture->map, fixture->playerX, fixture->playerY);" in fixture_c
        and "objectEvent->graphicsId != GetHeadlessOverworldFixtureGraphicsId(fixture->species)" in fixture_c
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
    require("pecharunt-shrine-background" not in scenarios, "removed Pecharunt body still has a sprite fixture")
    print("PASS: Inclement's exact eight physical encounters have one-to-one render coverage")


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
    semantic_params = {
        "pokedex": 0,
        "pokedex-info": 1,
        "pokedex-area": 2,
        "pokedex-stats": 3,
        "pokedex-evolutions": 4,
        "pokedex-forms": 5,
        "pokedex-cry": 6,
        "pokedex-size": 7,
        "pokedex-search": 8,
        "pokedex-search-results": 9,
        "summary-info": 0,
        "summary-skills": 1,
        "summary-moves": 2,
        "summary-contest-moves": 3,
        "summary-move-detail": 4,
        "summary-party-roundtrip": 5,
        "fairy-summary-info": 0,
        "fairy-summary-moves": 2,
        "bag": 2,
        "bag-items": 0,
        "bag-medicine": 1,
        "bag-tms-hms": 3,
        "bag-berries": 4,
        "bag-poke-balls": 5,
        "bag-key-items": 6,
        "bag-mega-stones": 7,
        "furfrou-trims": 0,
        "furfrou-trims-scrolled": 1,
        "furfrou-trims-b-cancel": 2,
        "furfrou-trims-back": 3,
    }
    for name, expected_param in semantic_params.items():
        require(
            scenarios[name].get("verify") is True
            and scenarios[name].get("param") == expected_param,
            f"{name} lacks its reviewed semantic state/parameter contract",
        )
    for name in ("wild-action-menu", "wild-foe-types", "move-details", "move-foe-types"):
        require(
            scenarios[name].get("verify") is True,
            f"{name} can render without runtime proof that its native battle UI was reached",
        )
    require(
        scenarios["all-legal-hm-replacement"].get("verify") is True
        and scenarios["all-legal-hm-replacement"].get("stop_on_observed") is True,
        "the all-moves tutor HM replacement lacks a synchronized runtime proof",
    )

    render_one = source_region(renderer, "def render_one(", "\ndef main() -> int:")
    png_validator = source_region(renderer, "def validate_screenshot_png(", "\ndef render_one(")
    require(
        'ihdr[:2] != (240, 160)' in png_validator
        and "zlib.decompress(bytes(idat))" in png_validator
        and "uniform blank screenshot" in png_validator
        and "return hashlib.sha256(pixels).hexdigest()" in png_validator
        and "st_size < 1000" not in renderer,
        "headless screenshots are not validated as nonblank 240x160 decoded pixel frames",
    )
    require(
        'command.extend(("--read", f"4:0x{setup_address:x}"))' in render_one
        and 'command.extend(("--read", f"4:0x{observed_address:x}"))' in render_one
        and "reads.get(setup_address) != 1 or reads.get(observed_address) != 1" in render_one
        and '"pixel_sha256": pixel_sha256' in render_one
        and '"verified_runtime_state": bool(spec.get("verify"))' in render_one,
        "verified renders do not record pixels plus both setup and observed runtime results",
    )
    main_region = renderer[renderer.index("def main() -> int:") :]
    require(
        'if args.scenario == "all":' in main_region
        and "names = list(SCENARIOS)" in main_region
        and 'elif args.scenario == "overworld-encounters":' in main_region
        and 'elif args.scenario == "inclement-seams":' in main_region
        and "names = list(INCLEMENT_SEAM_SCENARIOS)" in main_region
        and 'name.startswith("encounter-")' in main_region
        and '"rendered": rendered' in main_region
        and 'manifest_name = "manifest.json" if args.scenario == "all" else f"manifest.{args.scenario}.json"' in main_region
        and "manifest_path = args.out / manifest_name" in main_region,
        "the renderer can overwrite complete evidence with a focused manifest",
    )
    print(
        f"PASS: renderer preserves {len(REQUIRED_HEADLESS_SCENARIOS)} required scenarios; "
        "Inclement seam renders require decoded-pixel and semantic runtime proof"
    )


def verify_inclement_ui_semantic_observers() -> None:
    summary = read("src/pokemon_summary_screen.c")
    headless = read("src/emerald_champions_headless.c")
    require(
        "u32 maxPageIndex = C_HIDE_CONTEST_DATA ? PSS_PAGE_COUNT - 2 : PSS_PAGE_COUNT - 1;"
        in summary,
        "Summary's Contest Moves page is unreachable or the contest-data bound regressed",
    )
    for token in (
        "IsPokedexHeadlessOnScreen",
        "IsPokemonSummaryHeadlessOnPage",
        "IsPartyMenuHeadlessAwaitingSelection",
        "IsBagHeadlessOnPocket",
        "IsHeadlessPokedexStateObserved",
        "IsHeadlessSummaryStateObserved",
    ):
        require(token in headless, f"headless semantic observer is missing: {token}")
    require(
        "gEcHeadlessFixtureObservedResult = IsHeadlessPokedexStateObserved();" in headless
        and "gEcHeadlessFixtureObservedResult = IsHeadlessSummaryStateObserved();" in headless
        and "IsBagHeadlessOnPocket(gEcHeadlessFixtureParam)" in headless,
        "Pokedex, Summary, or Bag screenshots can pass without their exact final UI state",
    )
    print("PASS: Pokedex, Summary, and all eight Bag pockets expose semantic final-state observers")


def verify_furfrou_trim_menu() -> None:
    constants = read("include/constants/field_specials.h")
    field_specials = read("src/field_specials.c")
    headless = read("src/emerald_champions_headless.c")
    require(
        "SCROLL_MULTI_FURFROU_TRIMS" in constants,
        "Furfrou trim menu lost its dedicated scroll-menu ID",
    )
    menu_case = source_region(
        field_specials,
        "case SCROLL_MULTI_FURFROU_TRIMS:",
        "    default:",
    )
    for token in (
        "task->tMaxItemsOnScreen = 5;",
        "task->tNumItems = 11;",
        "task->tLeft = 18;",
        "task->tTop = 1;",
        "task->tHeight = task->tMaxItemsOnScreen * 2;",
        "task->tKeepOpenAfterSelect = FALSE;",
        "gSpecialVar_0x8005",
    ):
        require(token in menu_case, f"Furfrou trim geometry/cursor contract is missing: {token}")
    options = source_region(
        field_specials,
        "[SCROLL_MULTI_FURFROU_TRIMS] =",
        "[SCROLL_MULTI_GLASS_WORKSHOP_VENDOR] =",
    )
    expected = (
        'COMPOUND_STRING("Heart")',
        'COMPOUND_STRING("Star")',
        'COMPOUND_STRING("Diamond")',
        'COMPOUND_STRING("Debutante")',
        'COMPOUND_STRING("Matron")',
        'COMPOUND_STRING("Dandy")',
        'COMPOUND_STRING("La Reine")',
        'COMPOUND_STRING("Kabuki")',
        'COMPOUND_STRING("Pharaoh")',
        'COMPOUND_STRING("Natural")',
        "sText_Back",
    )
    cursor = 0
    for token in expected:
        position = options.find(token, cursor)
        require(position >= 0, f"Furfrou trim order drifted at {token}")
        cursor = position + len(token)
    require(
        "case LIST_CANCEL:" in field_specials
        and "gSpecialVar_Result = MULTI_B_PRESSED;" in field_specials
        and "gSpecialVar_Result = input;" in field_specials,
        "Furfrou B/Back results no longer use native scrolling-menu semantics",
    )
    require(
        "EC_HEADLESS_SCENARIO_FURFROU_TRIMS" in headless
        and "gScrollableMultichoice_ScrollOffset == 6" in headless
        and "gSpecialVar_Result == MULTI_B_PRESSED" in headless
        and "gSpecialVar_Result == 10" in headless,
        "Furfrou menu lacks semantic open/scroll/B/Back fixture observations",
    )
    require(1 + 10 + 1 < MESSAGE_BOX_TOP, "Furfrou menu frame can overlap the message box")
    print("PASS: Furfrou exposes ten ordered trims plus geometry-safe native B/Back scrolling")


def verify_extended_inclement_seam_matrix() -> None:
    rendered_module = runpy.run_path(str(ROOT / "scripts/render_emerald_champions_ui.py"))
    scenarios = rendered_module["SCENARIOS"]
    seam_names = set(rendered_module["INCLEMENT_SEAM_SCENARIOS"])
    heal_rows = rendered_module["HOENN_HEAL_FIXTURES"]
    require(len(heal_rows) == 21, "Hoenn whiteout fixture matrix is not canonical 21 rows")
    for row in heal_rows:
        slug = str(row["id"]).removeprefix("HEAL_LOCATION_").lower().replace("_", "-")
        name = f"heal-whiteout-{slug}"
        spec = scenarios.get(name, {})
        require(
            spec.get("id") == 44
            and spec.get("param") == row["heal_location_id"]
            and spec.get("verify") is True
            and spec.get("respawn_map") == row["respawn_map"]
            and name in seam_names,
            f"whiteout fixture drifted from canonical row: {row['id']}",
        )
    expected = {
        "hall-of-fame-record-1": (45, 1),
        "hall-of-fame-record-6": (45, 6),
        "multi-corridor-door-left-open": (46, 0),
        "multi-corridor-door-right-open": (46, 1),
        "multi-corridor-door-left-close": (46, 2),
        "multi-corridor-door-right-close": (46, 3),
    }
    for name, (scenario_id, param) in expected.items():
        spec = scenarios.get(name, {})
        require(
            spec.get("id") == scenario_id
            and spec.get("param") == param
            and spec.get("verify") is True
            and spec.get("stop_on_observed") is True
            and name in seam_names,
            f"extended seam fixture lacks semantic synchronization: {name}",
        )
    headless = read("src/emerald_champions_headless.c")
    for token in (
        "IsWhiteoutRespawnHeadlessState",
        "IsHallOfFameRecordHeadlessVisible",
        "FieldAnimateDoorOpen",
        "FieldAnimateDoorClose",
        "FieldIsDoorAnimationRunning",
        "MAP_BATTLE_FRONTIER_BATTLE_TOWER_MULTI_CORRIDOR",
    ):
        require(token in headless, f"extended seam observer is missing: {token}")
    print("PASS: 21 Hoenn whiteouts, HOF 1/6, and both 2x2 door halves have semantic fixtures")


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
    verify_inclement_copy_and_dex_numbering()
    verify_center_geometry(dimensions)
    verify_league_tutor()
    # Healing entry paths and world effect anchors are owned by
    # scripts/verify_emerald_champions_visual_contracts.py.
    verify_ability_selector()
    verify_battle_interface_sprite_guards()
    verify_leveler_batch_flow()
    verify_battle_set_preselection()
    verify_unified_move_list_width()
    verify_battle_vendor_navigation()
    verify_move_specialist_navigation()
    verify_stat_point_editor()
    verify_free_battle_vendor_list()
    verify_starter_region_cursor_memory()
    verify_visible_genie_placements()
    verify_physical_encounter_render_coverage()
    verify_headless_fixture_separation()
    verify_headless_renderer_contract()
    verify_inclement_ui_semantic_observers()
    verify_furfrou_trim_menu()
    verify_extended_inclement_seam_matrix()
    verify_high_risk_composed_screen_fixtures()
    verify_dynamic_menus()
    print("EMERALD CHAMPIONS NATIVE UI GATE: PASS")


if __name__ == "__main__":
    main()
