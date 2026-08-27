#!/usr/bin/env python3
"""Verify Pokémon Center tutor row alignment and native window bounds."""

from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "data/maps/BattleFrontier_Lounge7/scripts.inc"
FIELD = ROOT / "src/field_specials.c"


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def designated_block(source: str, label: str) -> str:
    match = re.search(rf"{re.escape(label)}\s*=\s*\{{(.*?)\n\s*\}}", source, re.S)
    require(match is not None, f"missing array block: {label}")
    return match.group(1)


def font1_metrics() -> tuple[list[int], dict[str, int]]:
    widths = [
        int(value)
        for line in (ROOT / "graphics/fonts/font1_latin_widths.inc").read_text().splitlines()
        for value in re.findall(r"\d+", line)
    ]
    charmap: dict[str, int] = {}
    for line in (ROOT / "charmap.txt").read_text().splitlines():
        match = re.match(r"'(.*)'\s*=\s*([0-9A-Fa-f]{2})\s*$", line)
        if not match:
            continue
        char = match.group(1)
        if char == r"\'":
            char = "'"
        if len(char) == 1:
            charmap[char] = int(match.group(2), 16)
    return widths, charmap


def main() -> None:
    script = SCRIPT.read_text()
    field = FIELD.read_text()
    widths, charmap = font1_metrics()
    description_pattern = re.compile(
        r"^(PokemonCenterMoveTutor_Text_[A-Za-z0-9]+Desc)::\s*\n"
        r"(.*?)(?=^PokemonCenterMoveTutor_Text_[A-Za-z0-9]+Desc::|\Z)",
        re.M | re.S,
    )
    descriptions: dict[str, list[str]] = {}
    overflow: list[str] = []
    too_tall: list[str] = []
    widest = 0

    for label, body in description_pattern.findall(script):
        fragments = re.findall(r'\.string\s+"([^"\\]*(?:\\.[^"\\]*)*)"', body)
        text = "".join(fragments).replace(r"\n", "\n").replace("$", "")
        lines = text.splitlines()
        descriptions[label] = lines
        if len(lines) > 3:
            too_tall.append(f"{label}: {len(lines)} lines")
        for line in lines:
            unknown = [char for char in line if char not in charmap]
            require(not unknown, f"font-1 charmap missing {unknown!r} in {label}")
            width = sum(widths[charmap[char]] for char in line)
            widest = max(widest, width)
            if width > 96:
                overflow.append(f"{label}: {width}px: {line}")

    require(len(descriptions) == 125, f"expected 125 tutor descriptions, found {len(descriptions)}")
    require(not overflow, "description overflow: " + "; ".join(overflow))
    require(not too_tall, "description height overflow: " + "; ".join(too_tall))

    for set_number in range(1, 8):
        move_block = designated_block(field, f"static const u16 sPokemonCenter_TutorMoves{set_number}[]")
        desc_block = designated_block(field, f"static const u8 *const sPokemonCenter_TutorMoveDescriptions{set_number}[]")
        name_block = designated_block(field, f"[SCROLL_MULTI_PC_TUTOR_SET_{set_number}]")
        move_count = len(re.findall(r"\bMOVE_[A-Z0-9_]+\b", move_block))
        desc_count = len(re.findall(r"\b(?:PokemonCenterMoveTutor_Text_[A-Za-z0-9]+Desc|gText_Exit)\b", desc_block))
        name_count = len(re.findall(r"\bgText_[A-Za-z0-9_]+\b", name_block))
        require(desc_count == name_count, f"set {set_number}: name/description rows differ")
        require(move_count + 1 == name_count, f"set {set_number}: taught moves do not align with Exit row")

    for token in (
        ".width = 12",
        ".height = 6",
        "FillWindowPixelRect(sTutorMoveAndElevatorWindowId, PIXEL_FILL(1), 0, 0, 96, 48)",
        "task->tMaxItemsOnScreen = min(task->tNumItems, 4)",
    ):
        require(token in field, f"native tutor window invariant missing: {token}")

    mart = (ROOT / "data/scripts/general_mart.inc").read_text()
    require(re.search(r"\.align 2\s*\nPokeMart_Poke_Center_No_Badges::", mart) is not None,
            "first Poké Mart u16 inventory is not halfword-aligned")

    print(f"Pokémon Center tutor: {len(descriptions)} descriptions, widest line {widest}/96px")
    print("Tutor names, descriptions, taught moves, Exit rows, and four-row scrolling align")
    print("Pokémon Center tutor UI release gate: PASS")


if __name__ == "__main__":
    main()
