#!/usr/bin/env python3
"""Static story, progression, discoverability, and dialogue-layout gates."""

from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
STORY_FILES = [
    "data/scripts/emerald_champions.inc",
    "data/maps/LittlerootTown_MaysHouse_1F/scripts.inc",
    "data/maps/LittlerootTown_MaysHouse_2F/scripts.inc",
    "data/maps/LittlerootTown_BrendansHouse_1F/scripts.inc",
    "data/maps/LittlerootTown_BrendansHouse_2F/scripts.inc",
    "data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc",
    "data/maps/RustboroCity_DevonCorp_3F/scripts.inc",
    "data/maps/GraniteCave_StevensRoom/scripts.inc",
    "data/maps/SlateportCity_OceanicMuseum_2F/scripts.inc",
    "data/maps/MtChimney/scripts.inc",
    "data/maps/MagmaHideout_4F/scripts.inc",
    "data/maps/Route119_WeatherInstitute_2F/scripts.inc",
    "data/maps/MtPyre_Summit/scripts.inc",
    "data/maps/MossdeepCity_SpaceCenter_2F/scripts.inc",
    "data/maps/SeafloorCavern_Room9/scripts.inc",
    "data/maps/SootopolisCity/scripts.inc",
    "data/maps/EverGrandeCity_ChampionsRoom/scripts.inc",
    "data/maps/RustboroCity_Gym/scripts.inc",
    "data/maps/DewfordTown_Gym/scripts.inc",
    "data/maps/MauvilleCity_Gym/scripts.inc",
    "data/maps/LavaridgeTown_Gym_1F/scripts.inc",
    "data/maps/PetalburgCity_Gym/scripts.inc",
    "data/maps/FortreeCity_Gym/scripts.inc",
    "data/maps/MossdeepCity_Gym/scripts.inc",
    "data/maps/SootopolisCity_Gym_1F/scripts.inc",
    "data/maps/EverGrandeCity_SidneysRoom/scripts.inc",
    "data/maps/EverGrandeCity_PhoebesRoom/scripts.inc",
    "data/maps/EverGrandeCity_GlaciasRoom/scripts.inc",
    "data/maps/EverGrandeCity_DrakesRoom/scripts.inc",
    "data/maps/PetalburgWoods/scripts.inc",
    "data/maps/DewfordTown/scripts.inc",
    "data/maps/VerdanturfTown/scripts.inc",
    "data/maps/Route111/scripts.inc",
    "data/maps/Route115/scripts.inc",
]
COHESION_FILES = [
    "data/text/trainers.inc",
    "data/text/shoal_cave.inc",
    "data/scripts/secret_power_tm.inc",
    "data/maps/Route104/scripts.inc",
    "data/maps/FallarborTown_CozmosHouse/scripts.inc",
    "data/maps/MauvilleCity/scripts.inc",
    "data/maps/PacifidlogTown_House2/scripts.inc",
    "data/maps/SlateportCity_OceanicMuseum_1F/scripts.inc",
    "data/maps/SootopolisCity_House1/scripts.inc",
    "data/maps/VerdanturfTown_BattleTentLobby/scripts.inc",
    "data/maps/FortreeCity_House2/scripts.inc",
    "data/maps/SSTidalRooms/scripts.inc",
    "data/maps/RustboroCity_PokemonSchool/scripts.inc",
    "data/maps/DewfordTown_House2/scripts.inc",
    "data/maps/ShoalCave_LowTideEntranceRoom/scripts.inc",
    "data/maps/Route112/scripts.inc",
    "data/maps/JaggedPass/scripts.inc",
    "data/maps/AshenWoods/scripts.inc",
    "data/maps/AlteringCave_B1F/scripts.inc",
    "data/maps/CaveOfOrigin_DianciesRoom/scripts.inc",
    "data/maps/MossdeepCity_House1/scripts.inc",
    "data/maps/CaveOfOrigin_1F/scripts.inc",
    "data/maps/Route114_FossilManiacsTunnel/scripts.inc",
    "data/maps/Route133/scripts.inc",
    "data/maps/NewMauville_Inside/scripts.inc",
    "data/maps/RustboroCity_DevonCorp_2F/scripts.inc",
    "data/maps/LilycoveCity_Harbor/scripts.inc",
    "data/maps/Route118/scripts.inc",
    "data/maps/VerdanturfMeadow/scripts.inc",
    "data/maps/LilycoveCity_CoveLilyMotel_2F/scripts.inc",
]
DIALOGUE_FILES = tuple(dict.fromkeys(STORY_FILES + COHESION_FILES))
STORY_BEATS = {
    "data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc": ("one tradition from nine", "Preparation is easy"),
    "data/maps/RustboroCity_DevonCorp_3F/scripts.inc": ("CHAMPION'S SIGNS", "ITEM_PIDGEOTITE"),
    "data/maps/GraniteCave_StevensRoom/scripts.inc": ("FLAG_BADGE02_GET", "ITEM_MEGA_RING", "CHAMPION'S SIGNS"),
    "data/maps/SlateportCity_OceanicMuseum_2F/scripts.inc": ("the sea", "trace the deep current"),
    "data/maps/MtChimney/scripts.inc": ("stable, permanent field", "fault line"),
    "data/maps/MagmaHideout_4F/scripts.inc": ("GROUDON", "my plan did"),
    "data/maps/Route119_WeatherInstitute_2F/scripts.inc": ("ROUTE 111", "SCORCHED SLAB", "SEASPRAY"),
    "data/maps/MtPyre_Summit/scripts.inc": ("network of", "RAYQUAZA"),
    "data/maps/MossdeepCity_SpaceCenter_2F/scripts.inc": ("partners", "METEOR FALLS"),
    "data/maps/SeafloorCavern_Room9/scripts.inc": ("KYOGRE", "We never understood the ORBS"),
    "data/maps/SootopolisCity/scripts.inc": ("RAYQUAZA", "restored their relationship"),
    "data/maps/EverGrandeCity_ChampionsRoom/scripts.inc": ("Nothing was hidden behind grinding", "FRONTIER waits"),
}

# Bodies and geometry come from Inclement Emerald v1.13.  Every migrated NPC
# must now point to an explicit native service or map-specific replacement;
# the generic fallback is never an acceptable live script.
INCLEMENT_RESTORED_NPCS = {
    "DewfordTown": ((8, 18, "DewfordTown_EventScript_GymGuide"),),
    "DewfordTown_Hall": ((3, 7, "DewfordTown_Hall_EventScript_Trader"),),
    "FallarborTown_Mart": ((3, 2, "FallarborTown_Mart_EventScript_MoveSpecialist"),),
    "LilycoveCity": ((41, 15, "LilycoveCity_EventScript_AltarianiteKeeper"),),
    "MauvilleCity_GameCorner": ((12, 2, "MauvilleCity_GameCorner_EventScript_PrizeCornerPokemon"),),
    "MauvilleCity_House2": ((7, 4, "MauvilleCity_House2_EventScript_MoveSpecialist"),),
    "MeteorFalls_1F_1R": ((14, 21, "MeteorFalls_1F_1R_EventScript_RivalTalkAfterBattle"),),
    "MeteorFalls_1F_2R": ((18, 2, "MeteorFalls_1F_2R_EventScript_MoveSpecialist"),),
    "Route109": (
        (13, 19, "Route109_EventScript_SandMound1"),
        (24, 22, "Route109_EventScript_SandMound2"),
        (31, 14, "Route109_EventScript_SandMound3"),
    ),
    "Route114": ((22, 29, "Route114_EventScript_GoodRodFisherman"),),
    "Route117_PokemonDayCare": ((9, 6, "Route117_PokemonDayCare_EventScript_TogepiEgg"),),
    "Route121": (
        (29, 16, "Route121_EventScript_Nurse"),
        (29, 17, "Route121_EventScript_Skitty"),
    ),
    "RustboroCity": ((17, 21, "RustboroCity_EventScript_RoxanneRestored"),),
    "RustboroCity_DevonCorp_2F": ((6, 8, "RustboroCity_DevonCorp_2F_EventScript_EeveeResearcher"),),
    "RustboroCity_Mart": ((5, 2, "RustboroCity_Mart_EventScript_MoveSpecialist"),),
    "SlateportCity": (
        (11, 51, "SlateportCity_EventScript_BattleItemScholar"),
        (11, 43, "SlateportCity_EventScript_IncenseScholar"),
        (19, 26, "SlateportCity_EventScript_BrawlyRestored"),
    ),
    "SlateportCity_PokemonFanClub": ((12, 10, "SlateportCity_PokemonFanClub_EventScript_FurfrouStylist"),),
    "SootopolisCity_House2": ((6, 3, "SootopolisCity_House2_EventScript_MoveSpecialist"),),
    "VerdanturfTown_FriendshipRatersHouse": ((7, 4, "VerdanturfTown_FriendshipRatersHouse_EventScript_Trader"),),
    "VerdanturfTown_Mart": ((8, 2, "VerdanturfTown_Mart_EventScript_MoveSpecialist"),),
}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def font_widths() -> list[int]:
    text = (ROOT / "src/fonts.c").read_text()
    match = re.search(r"gFontNormalLatinGlyphWidths\[\]\s*=\s*\{(.*?)\};", text, re.S)
    require(match is not None, "normal font width table is missing")
    return [int(value) for value in re.findall(r"\d+", match.group(1))]


def glyph_codes() -> dict[str, int]:
    result = {
        " ": 0x00, ";": 0x36, "%": 0x5B, "(": 0x5C, ")": 0x5D,
        "!": 0xAB, "?": 0xAC, ".": 0xAD, "-": 0xAE, "…": 0xB0,
        "“": 0xB1, "”": 0xB2, "'": 0xB4, "¥": 0xB7, ",": 0xB8, "/": 0xBA,
        ":": 0xF0, "é": 0x1B, "&": 0x2D, "+": 0x2E,
    }
    result.update({char: 0xA1 + i for i, char in enumerate("0123456789")})
    result.update({char: 0xBB + i for i, char in enumerate("ABCDEFGHIJKLMNOPQRSTUVWXYZ")})
    result.update({char: 0xD5 + i for i, char in enumerate("abcdefghijklmnopqrstuvwxyz")})
    return result


def expanded_line(line: str) -> str:
    placeholders = {
        "PLAYER": "WWWWWWW",
        "RIVAL": "WWWWWWW",
        "KUN": "",
        "STR_VAR_1": "WWWWWWWWWWWW",
        "STR_VAR_2": "WWWWWWWWWWWW",
        "STR_VAR_3": "WWWWWWWWWWWW",
    }
    return re.sub(r"\{([^}]+)\}", lambda match: placeholders.get(match.group(1), ""), line)


def verify_widths() -> int:
    widths = font_widths()
    codes = glyph_codes()
    checked = 0
    for relative in DIALOGUE_FILES:
        for line_number, source_line in enumerate((ROOT / relative).read_text().splitlines(), 1):
            match = re.search(r'\.string "(.*)"', source_line)
            if match is None:
                continue
            for visual_line in re.split(r"\\[npl]", match.group(1)):
                visual_line = visual_line.rstrip("$")
                expanded = expanded_line(visual_line)
                unknown = sorted({char for char in expanded if char not in codes and char != "$"})
                require(not unknown, f"{relative}:{line_number}: unsupported width glyphs {unknown}")
                width = sum(widths[codes[char]] for char in expanded if char in codes)
                require(width <= 216, f"{relative}:{line_number}: {width}px dialogue overflow: {visual_line}")
                checked += 1
    return checked


def all_hoenn_dialogue_files() -> list[Path]:
    """Return every player-facing Hoenn script/text source exactly once."""
    groups = json.loads((ROOT / "data/maps/map_groups.json").read_text())
    paths: list[Path] = []
    for group in groups["group_order"]:
        for map_name in groups[group]:
            if "_Frlg" in map_name:
                continue
            path = ROOT / "data/maps" / map_name / "scripts.inc"
            if path.is_file():
                paths.append(path)
    paths.extend(
        path
        for path in (ROOT / "data/scripts").glob("*.inc")
        if "frlg" not in path.name.lower() and path.name != "debug.inc"
    )
    paths.extend((ROOT / "data/text").glob("*.inc"))
    return list(dict.fromkeys(paths))


def verify_all_static_widths() -> int:
    """Measure every literal dialogue line that has no runtime substitution.

    Dynamic names and numbers are deliberately covered by their owning UI and
    selected conservative checks above. Literal lines, including every trainer
    and NPC sentence, have an exact width and must never clip the native box.
    """
    widths = font_widths()
    codes = glyph_codes()
    checked = 0
    for path in all_hoenn_dialogue_files():
        relative = path.relative_to(ROOT)
        for line_number, source_line in enumerate(path.read_text(errors="ignore").splitlines(), 1):
            match = re.search(r'\.string "(.*)"', source_line)
            if match is None:
                continue
            for visual_line in re.split(r"\\[npl]", match.group(1)):
                visual_line = visual_line.rstrip("$")
                if "{" in visual_line:
                    continue
                if any(char not in codes for char in visual_line):
                    # Japanese/debug/control-font strings do not use this table.
                    continue
                width = sum(widths[codes[char]] for char in visual_line)
                require(width <= 216, f"{relative}:{line_number}: {width}px dialogue overflow: {visual_line}")
                checked += 1
    return checked


def verify_cynthia_assets() -> None:
    required_assets = (
        "graphics/object_events/pics/people/cynthia.png",
        "graphics/trainers/front_pics/cynthia.png",
    )
    for relative in required_assets:
        require((ROOT / relative).is_file(), f"Cynthia asset is missing: {relative}")

    checks = {
        "include/constants/event_objects.h": ("OBJ_EVENT_GFX_CYNTHIA", "OBJ_EVENT_PAL_TAG_CYNTHIA"),
        "include/constants/trainers.h": ("TRAINER_PIC_CYNTHIA",),
        "src/data/object_events/object_event_graphics.h": ("gObjectEventPic_Cynthia", "gObjectEventPal_Cynthia"),
        "src/data/object_events/object_event_pic_tables.h": ("sPicTable_Cynthia",),
        "src/data/object_events/object_event_graphics_info.h": ("gObjectEventGraphicsInfo_Cynthia",),
        "src/data/object_events/object_event_graphics_info_pointers.h": ("[OBJ_EVENT_GFX_CYNTHIA]",),
        "src/event_object_movement.c": ("OBJ_EVENT_PAL_TAG_CYNTHIA",),
        "data/maps/MossdeepCity_House1/map.json": ('"graphics_id": "OBJ_EVENT_GFX_CYNTHIA"',),
        "src/data/graphics/trainers.h": ("gTrainerFrontPic_Cynthia", "gTrainerPalette_Cynthia"),
    }
    for relative, needles in checks.items():
        text = (ROOT / relative).read_text()
        for needle in needles:
            require(needle in text, f"{relative}: Cynthia integration is missing {needle!r}")

    trainers = (ROOT / "src/data/trainers.party").read_text()
    match = re.search(r"=== TRAINER_CYNTHIA_1 ===\n(.*?)(?=\n=== |\Z)", trainers, re.S)
    require(match is not None, "Cynthia trainer block is missing")
    require("Pic: Cynthia" in match.group(1), "Cynthia battle still uses a generic trainer portrait")


def verify_elite_four_retirement_path() -> None:
    """Prove a one-survivor League run can never become a locked-room save trap."""
    helper = (ROOT / "data/scripts/elite_four.inc").read_text()
    helper_contract = (
        "PokemonLeague_EliteFour_EventScript_CheckReadyOrOfferRetire::",
        "special HasEnoughMonsForDoubleBattle",
        "goto_if_eq VAR_RESULT, PLAYER_HAS_TWO_USABLE_MONS",
        "msgbox PokemonLeague_EliteFour_Text_RetirePrompt, MSGBOX_YESNO",
        "setrespawn HEAL_LOCATION_EVER_GRANDE_CITY_POKEMON_LEAGUE",
        "special Script_FadeOutMapMusic",
        "fadescreen FADE_TO_BLACK",
        "special SetCB2WhiteOut",
    )
    for needle in helper_contract:
        require(needle in helper, f"Elite Four retirement helper is missing {needle!r}")

    fights = {
        "data/maps/EverGrandeCity_SidneysRoom/scripts.inc": "TRAINER_SIDNEY",
        "data/maps/EverGrandeCity_PhoebesRoom/scripts.inc": "TRAINER_PHOEBE",
        "data/maps/EverGrandeCity_GlaciasRoom/scripts.inc": "TRAINER_GLACIA",
        "data/maps/EverGrandeCity_DrakesRoom/scripts.inc": "TRAINER_DRAKE",
        "data/maps/EverGrandeCity_ChampionsRoom/scripts.inc": "TRAINER_WALLACE",
    }
    helper_call = "call PokemonLeague_EliteFour_EventScript_CheckReadyOrOfferRetire"
    for relative, trainer in fights.items():
        text = (ROOT / relative).read_text()
        require(helper_call in text, f"{relative}: League readiness check is missing")
        require(text.index(helper_call) < text.index(f"trainerbattle_no_intro_double {trainer}"),
                f"{relative}: readiness check runs after the battle command")
        require("goto_if_eq VAR_RESULT, FALSE" in text,
                f"{relative}: declining retirement cannot return control to the player")

    champion_map = json.loads((ROOT / "data/maps/EverGrandeCity_ChampionsRoom/map.json").read_text())
    wallace = next(
        (event for event in champion_map["object_events"]
         if event["local_id"] == "LOCALID_CHAMPIONS_ROOM_WALLACE"),
        None,
    )
    require(wallace is not None, "Champion room Wallace object is missing")
    require(wallace["script"] == "EverGrandeCity_ChampionsRoom_EventScript_Wallace",
            "Wallace cannot be challenged again after declining retirement to use the Bag")

    whiteout = (ROOT / "data/event_scripts.s").read_text()
    require("EventScript_WhiteOut::\n\tcall EverGrandeCity_HallOfFame_EventScript_ResetEliteFour" in whiteout,
            "native whiteout no longer resets the active League run")
    lobby = (ROOT / "data/maps/EverGrandeCity_PokemonLeague_1F/scripts.inc").read_text()
    require("setrespawn HEAL_LOCATION_EVER_GRANDE_CITY_POKEMON_LEAGUE" in lobby,
            "League lobby no longer establishes the retirement respawn point")


def verify_badge_leveler_and_field_move_contracts() -> None:
    expected = (
        ("FLAG_BADGE01_GET", 14, "RustboroCity_Gym", 20, "FIELD_MOVE_CUT", "FLAG_RECEIVED_HM_CUT"),
        ("FLAG_BADGE02_GET", 20, "DewfordTown_Gym", 30, "FIELD_MOVE_FLASH", "FLAG_RECEIVED_HM_FLASH"),
        ("FLAG_BADGE03_GET", 30, "MauvilleCity_Gym", 40, "FIELD_MOVE_ROCK_SMASH", "FLAG_RECEIVED_HM_ROCK_SMASH"),
        ("FLAG_BADGE04_GET", 40, "LavaridgeTown_Gym_1F", 45, "FIELD_MOVE_STRENGTH", "FLAG_RECEIVED_HM_STRENGTH"),
        ("FLAG_BADGE05_GET", 45, "PetalburgCity_Gym", 55, "FIELD_MOVE_SURF", "FLAG_RECEIVED_HM_SURF"),
        ("FLAG_BADGE06_GET", 55, "FortreeCity_Gym", 60, "FIELD_MOVE_FLY", "FLAG_RECEIVED_HM_FLY"),
        ("FLAG_BADGE07_GET", 60, "MossdeepCity_Gym", 70, "FIELD_MOVE_DIVE", "FLAG_RECEIVED_HM_DIVE"),
        ("FLAG_BADGE08_GET", 70, "SootopolisCity_Gym_1F", 80, "FIELD_MOVE_WATERFALL", "FLAG_RECEIVED_HM_WATERFALL"),
    )
    caps = (ROOT / "src/caps.c").read_text()
    field_moves = (ROOT / "src/field_move.c").read_text()
    for badge, prior_cap, map_name, next_cap, field_move, license_flag in expected:
        require(
            re.search(rf"\{{\s*{badge},\s*{prior_cap}\s*\}}", caps) is not None,
            f"{badge} no longer advances from the expected {prior_cap} cap",
        )
        gym_text = (ROOT / f"data/maps/{map_name}/scripts.inc").read_text()
        require(
            f"Lv. {next_cap}." in gym_text,
            f"{map_name}: badge speech does not state the live Leveler cap {next_cap}",
        )
        move_block = re.search(
            rf"\[{field_move}\]\s*=\s*\{{(?P<body>.*?)\n\s*\}},",
            field_moves,
            re.DOTALL,
        )
        require(move_block is not None and badge in move_block.group("body"),
                f"{field_move} is not licensed by {badge}")
        require(license_flag in field_moves,
                f"{field_move} no longer requires receipt of {license_flag}")


def verify_manaphy_clue() -> None:
    mossdeep = (ROOT / "data/maps/MossdeepCity/scripts.inc").read_text()
    block = re.search(
        r"MossdeepCity_Text_LifeNeedsSeaToLive:\n(?P<body>.*?)(?=\n[A-Za-z_][A-Za-z0-9_]*:)",
        mossdeep,
        re.DOTALL,
    )
    require(block is not None, "Mossdeep's Seafloor approach clue is missing")
    for phrase in ("RELICANTH", "SEAFLOOR CAVERN", "princely light"):
        require(phrase in block.group("body"), f"Mossdeep's Manaphy clue is missing {phrase!r}")


def verify_legendary_sign_completion_guidance() -> None:
    definitions = (ROOT / "src/data/pokemon/legendary_signs.h").read_text()
    source_contract = (
        "WILD_SIGN(",
        "VISIBLE_SIGN(",
        "ORDINARY_WILD_SIGN(",
        "LEGENDARY_SOURCE_BREEDING",
        "LEGENDARY_SOURCE_GAME_CORNER",
        "LEGENDARY_SOURCE_CIRCUIT",
        "LEGENDARY_SOURCE_MASTERY",
    )
    for token in source_contract:
        require(token in definitions, f"Legendary Sign source contract is missing {token}")

    devon = (ROOT / "data/maps/RustboroCity_DevonCorp_2F/scripts.inc").read_text()
    block = re.search(
        r"RustboroCity_DevonCorp_2F_Text_AllLegendarySignsRecorded:\n"
        r"(?P<body>.*?)(?=\n[A-Za-z_][A-Za-z0-9_]*:)",
        devon,
        re.DOTALL,
    )
    require(block is not None, "Devon's exhausted conditional-Sign guidance is missing")
    guidance = block.group("body")
    require("Every wild Legendary Sign is awake" not in guidance,
            "Devon still claims every wild Sign is awake after checking only conditional-wild Signs")
    for phrase in (
        "conditional wild SIGN",
        "visible shrines",
        "rare wild finds",
        "breeding",
        "GAME",
        "CORNER",
        "CIRCUIT rewards",
        "mastery",
        "MT.",
        "PYRE's three",
        "ARCEUS",
    ):
        require(phrase in guidance, f"Devon's Sign-completion guidance omits {phrase!r}")


def verify_stat_point_explanation_replaced_iv_rater() -> None:
    lounge = (ROOT / "data/maps/BattleFrontier_Lounge1/scripts.inc").read_text()
    for phrase in ("flawless potential", "STAT POINTS", "CENTER tutor"):
        require(phrase in lounge, f"Frontier Stat Point explanation is missing {phrase!r}")
    require(
        all(token not in lounge for token in ("chooseboxmon", "HighestIV", "TotalIVs", "BufferVarsForIVRater")),
        "the reachable Frontier lounge still rates stored IVs that do not affect battle stats",
    )
    require("BufferVarsForIVRater" not in (ROOT / "data/specials.inc").read_text(),
            "the obsolete IV-rater special remains registered")
    require("void BufferVarsForIVRater" not in (ROOT / "src/field_specials.c").read_text(),
            "the obsolete IV-rater implementation remains compiled")


def verify_repurposed_reward_services() -> None:
    route116 = (ROOT / "data/maps/Route116/scripts.inc").read_text()
    require("checkitem ITEM_DUSK_STONE" in route116 and "FoundRoute116DuskStone" in route116,
            "Route 116's seeker does not follow the live Dusk Stone pickup")
    require("ITEM_BLACK_GLASSES" not in route116 and "FoundBlackGlasses" not in route116,
            "Route 116 still describes the deleted Black Glasses pickup")
    require(
        "setflag FLAG_HIDE_ROUTE_116_DUSK_STONE_SEEKER" in route116,
        "Route 116's completed Dusk Stone seeker scene respawns after reloading the map",
    )

    vars_text = (ROOT / "include/constants/vars.h").read_text()
    require(
        re.search(r"#define\s+VAR_PACIFIDLOG_STONE_RECEIVED_DAY\s+0x40C2\b", vars_text) is not None,
        "Pacifidlog's weekly evolution-stone timer lost its save-compatible variable",
    )
    require("VAR_PACIFIDLOG_TM_RECEIVED_DAY" not in vars_text,
            "Pacifidlog's evolution-stone timer still has a deleted-TM name")


def verify_inclement_restored_npcs() -> None:
    labels = set()
    for script_path in (ROOT / "data").rglob("*.inc"):
        labels.update(re.findall(r"(?m)^([A-Za-z_][A-Za-z0-9_]*)::", script_path.read_text()))

    placeholder_rows = []
    for map_path in (ROOT / "data/maps").glob("*/map.json"):
        payload = json.loads(map_path.read_text())
        for event in payload.get("object_events", []) or []:
            if event.get("script") == "Common_EventScript_InclementRestoredNPC":
                placeholder_rows.append((map_path.parent.name, event.get("x"), event.get("y")))
    require(not placeholder_rows, f"generic Inclement NPC placeholders remain live: {placeholder_rows}")

    for map_name, expected_rows in INCLEMENT_RESTORED_NPCS.items():
        payload = json.loads((ROOT / f"data/maps/{map_name}/map.json").read_text())
        objects = payload.get("object_events", []) or []
        for x, y, script in expected_rows:
            require(
                sum(
                    event.get("x") == x
                    and event.get("y") == y
                    and event.get("script") == script
                    for event in objects
                ) == 1,
                f"{map_name}: restored NPC at {x},{y} is not mapped to {script}",
            )
            require(script in labels, f"{map_name}: mapped NPC script is undefined: {script}")

    require(
        "SCROLL_MULTI_FURFROU_TRIMS" in (ROOT / "include/constants/field_specials.h").read_text()
        and "special ChangeSelectedMonSpecies" in
            (ROOT / "data/maps/SlateportCity_PokemonFanClub/scripts.inc").read_text(),
        "the restored Furfrou stylist is not wired to its native trim selector",
    )


def main() -> None:
    combined = "\n".join((ROOT / relative).read_text() for relative in STORY_FILES)
    require("—" not in combined and "–" not in combined, "unsupported dash leaked into story text")
    for relative, phrases in STORY_BEATS.items():
        text = (ROOT / relative).read_text()
        for phrase in phrases:
            require(phrase in text, f"{relative}: missing story beat {phrase!r}")

    steven = (ROOT / "data/maps/GraniteCave_StevensRoom/scripts.inc").read_text()
    require(steven.index("goto_if_unset FLAG_BADGE02_GET") < steven.index("giveitem ITEM_MEGA_RING"),
            "Mega Ring is not gated behind Brawly")
    require("goto_if_eq VAR_RESULT, FALSE, GraniteCave_StevensRoom_EventScript_BagFull" in steven,
            "full Bag can still lose the Mega Ring")
    require("setflag FLAG_HIDE_GRANITE_CAVE_STEVEN" in steven,
            "Steven does not remain removed after completing his scene")

    magma_finale = (ROOT / "data/maps/MagmaHideout_4F/scripts.inc").read_text()
    awakening = magma_finale.index("playbgm MUS_ENCOUNTER_MAGMA")
    for admin in ("TRAINER_COURTNEY_MAGMA_HIDEOUT", "TRAINER_TABITHA_MAGMA_HIDEOUT"):
        gate = f"goto_if_not_defeated {admin}, MagmaHideout_4F_EventScript_AdminsStillStanding"
        require(gate in magma_finale and magma_finale.index(gate) < awakening,
                f"Maxie can awaken Groudon before defeating {admin}")

    devon = (ROOT / "data/maps/RustboroCity_DevonCorp_3F/scripts.inc").read_text()
    require("giveitem ITEM_EXP_SHARE" not in devon, "obsolete EXP Share reward remains")
    require("giveitem ITEM_PIDGEOTITE" in devon, "Devon letter reward is not progression-relevant")

    clues = {
        "data/maps/PetalburgWoods/scripts.inc": "BRELOOM",
        "data/maps/DewfordTown/scripts.inc": "abandoned manor",
        "data/maps/VerdanturfTown/scripts.inc": "meadow lies to the south",
        "data/maps/Route111/scripts.inc": "SANDSTREWN RUINS",
        "data/maps/Route115/scripts.inc": "SEASPRAY CAVE",
    }
    for relative, phrase in clues.items():
        require(phrase in (ROOT / relative).read_text(), f"restored-area clue missing from {relative}")

    trainer_dialogue = (ROOT / "data/text/trainers.inc").read_text()
    for stale in ("Hey, MAGIKARP", "My SANDSHREW", "my dear NUMEL", "My MACHOP"):
        require(stale not in trainer_dialogue, f"trainer dialogue still references a removed party member: {stale}")

    postgame_rewards = {
        "data/maps/AlteringCave_B1F/scripts.inc": ("ITEM_CATCHING_CHARM", "ITEM_ZERAORITE"),
        "data/maps/CaveOfOrigin_DianciesRoom/scripts.inc": ("ITEM_OVAL_CHARM", "ITEM_STARMINITE"),
        "data/maps/MossdeepCity_House1/scripts.inc": ("ITEM_SHINY_CHARM", "ITEM_GARCHOMPITE_Z"),
    }
    for relative, (reward, obsolete) in postgame_rewards.items():
        text = (ROOT / relative).read_text()
        require(f"giveitem {reward}" in text, f"{relative}: missing useful postgame reward {reward}")
        require(obsolete not in text, f"{relative}: redundant archived Mega Stone reward remains")

    verify_cynthia_assets()
    verify_elite_four_retirement_path()
    verify_badge_leveler_and_field_move_contracts()
    verify_manaphy_clue()
    verify_legendary_sign_completion_guidance()
    verify_stat_point_explanation_replaced_iv_rater()
    verify_repurposed_reward_services()
    verify_inclement_restored_npcs()
    checked = verify_widths()
    all_static_checked = verify_all_static_widths()
    print("PASS: core Hoenn story preserves Magma/Aqua, Rayquaza, Wallace, and the Frontier")
    print("PASS: Mega and restored-area progression is narratively discoverable")
    print("PASS: Cynthia uses her dedicated overworld sprite and trainer portrait")
    print("PASS: every League room has a one-survivor retirement and retry path")
    print("PASS: all 25 Inclement NPC bodies have explicit native services or dialogue")
    print(f"PASS: {checked} story dialogue lines fit the native 216px window")
    print(f"PASS: {all_static_checked} literal Hoenn dialogue lines fit the native 216px window")


if __name__ == "__main__":
    main()
