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
    checked = verify_widths()
    all_static_checked = verify_all_static_widths()
    print("PASS: core Hoenn story preserves Magma/Aqua, Rayquaza, Wallace, and the Frontier")
    print("PASS: Mega and restored-area progression is narratively discoverable")
    print("PASS: Cynthia uses her dedicated overworld sprite and trainer portrait")
    print("PASS: every League room has a one-survivor retirement and retry path")
    print(f"PASS: {checked} story dialogue lines fit the native 216px window")
    print(f"PASS: {all_static_checked} literal Hoenn dialogue lines fit the native 216px window")


if __name__ == "__main__":
    main()
