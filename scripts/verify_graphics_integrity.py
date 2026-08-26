#!/usr/bin/env python3
"""Validate native graphics tables and source assets without trusting a ROM build."""

from __future__ import annotations

from pathlib import Path
import re
import struct
import sys
import zlib


ROOT = Path(__file__).resolve().parents[1]
PNG_SIGNATURE = b"\x89PNG\r\n\x1a\n"
failures: list[str] = []
checks = 0


def require(condition: bool, label: str) -> None:
    global checks
    checks += 1
    if not condition:
        failures.append(label)


def expanded_table(path: str) -> str:
    source_path = ROOT / path
    text = source_path.read_text()
    for include in re.findall(r'#include\s+"([^"]+)"', text):
        candidates = (
            source_path.parent / include,
            ROOT / "src/data/pokemon_graphics" / Path(include).name,
        )
        include_path = next((candidate for candidate in candidates if candidate.is_file()), None)
        if include_path is not None:
            text += "\n" + include_path.read_text()
    return text


def species_set(path: str, pattern: str) -> set[str]:
    return set(re.findall(pattern, expanded_table(path)))


def read_png(path: Path) -> tuple[int, int, int, int]:
    data = path.read_bytes()
    if not data.startswith(PNG_SIGNATURE):
        raise ValueError("invalid PNG signature")

    offset = len(PNG_SIGNATURE)
    width = height = color_type = palette_entries = -1
    saw_end = False
    while offset + 12 <= len(data):
        length = struct.unpack_from(">I", data, offset)[0]
        chunk_type = data[offset + 4 : offset + 8]
        chunk_start = offset + 8
        chunk_end = chunk_start + length
        crc_end = chunk_end + 4
        if crc_end > len(data):
            raise ValueError(f"truncated {chunk_type.decode(errors='replace')} chunk")
        expected_crc = struct.unpack_from(">I", data, chunk_end)[0]
        actual_crc = zlib.crc32(chunk_type)
        actual_crc = zlib.crc32(data[chunk_start:chunk_end], actual_crc) & 0xFFFFFFFF
        if actual_crc != expected_crc:
            raise ValueError(f"bad {chunk_type.decode(errors='replace')} CRC")
        if chunk_type == b"IHDR":
            width, height, _, color_type = struct.unpack_from(">IIBB", data, chunk_start)
        elif chunk_type == b"PLTE":
            if length % 3:
                raise ValueError("invalid PLTE length")
            palette_entries = length // 3
        elif chunk_type == b"IEND":
            saw_end = True
            if crc_end != len(data):
                raise ValueError("bytes remain after IEND")
            break
        offset = crc_end

    if not saw_end or width <= 0 or height <= 0:
        raise ValueError("missing IHDR or IEND")
    return width, height, color_type, palette_entries


def check_png_group(root: str, filename: str, allowed_dimensions: set[tuple[int, int]]) -> int:
    count = 0
    for path in sorted((ROOT / root).rglob(filename)):
        count += 1
        try:
            width, height, color_type, palette_entries = read_png(path)
        except (OSError, ValueError) as error:
            failures.append(f"{path.relative_to(ROOT)}: {error}")
            continue
        require(
            (width, height) in allowed_dimensions,
            f"{path.relative_to(ROOT)} has unexpected dimensions {width}x{height}",
        )
        require(
            color_type in {2, 3, 6},
            f"{path.relative_to(ROOT)} has unsupported PNG color type {color_type}",
        )
        if palette_entries >= 0:
            require(
                palette_entries <= 16,
                f"{path.relative_to(ROOT)} has {palette_entries} palette entries; GBA sprites allow 16",
            )
    return count


def check_jasc_palettes() -> int:
    count = 0
    for path in sorted((ROOT / "graphics").rglob("*.pal")):
        lines = path.read_text(errors="replace").splitlines()
        if not lines or lines[0] != "JASC-PAL":
            continue
        count += 1
        if len(lines) < 3 or not lines[2].isdigit():
            failures.append(f"{path.relative_to(ROOT)} has an invalid JASC header")
            continue
        declared = int(lines[2])
        colors = [line for line in lines[3:] if re.fullmatch(r"\d+\s+\d+\s+\d+", line)]
        require(declared == len(colors), f"{path.relative_to(ROOT)} declares {declared} colors but stores {len(colors)}")
        relative = path.relative_to(ROOT)
        is_single_sprite_palette = (
            relative.is_relative_to("graphics/pokemon")
            or relative.is_relative_to("graphics/items/icon_palettes")
            or relative.is_relative_to("graphics/trainers/palettes")
        )
        maximum = 16 if is_single_sprite_palette else 256
        require(0 < declared <= maximum, f"{relative} has unsupported {declared}-color palette")
    return count


def main() -> int:
    front = species_set(
        "src/data/pokemon_graphics/front_pic_table.h", r"SPECIES_SPRITE\(([A-Z0-9_]+),"
    )
    tables = {
        "back pictures": species_set(
            "src/data/pokemon_graphics/back_pic_table.h", r"SPECIES_SPRITE\(([A-Z0-9_]+),"
        ),
        "normal palettes": species_set(
            "src/data/pokemon_graphics/palette_table.h", r"SPECIES_PAL\(([A-Z0-9_]+),"
        ),
        "shiny palettes": species_set(
            "src/data/pokemon_graphics/shiny_palette_table.h", r"SPECIES_SHINY_PAL\(([A-Z0-9_]+),"
        ),
        "front coordinates": species_set(
            "src/data/pokemon_graphics/front_pic_coordinates.h", r"\[SPECIES_([A-Z0-9_]+)\]"
        ),
        "back coordinates": species_set(
            "src/data/pokemon_graphics/back_pic_coordinates.h", r"\[SPECIES_([A-Z0-9_]+)\]"
        ),
    }
    icon_source = (ROOT / "src/pokemon_icon.c").read_text() + (
        ROOT / "src/data/pokemon_graphics/verdant_gen9_icon_table.h"
    ).read_text()
    tables["icons"] = set(re.findall(r"\[SPECIES_([A-Z0-9_]+)\]\s*=", icon_source))

    require(len(front) > 1200, f"front-picture table is unexpectedly small ({len(front)} species)")
    for label, entries in tables.items():
        missing = sorted(front - entries)
        extra = sorted(entries - front)
        require(not missing and not extra, f"{label} table mismatch; missing={missing}, extra={extra}")

    png_counts = {
        "front": check_png_group("graphics/pokemon", "front.png", {(64, 64), (64, 128)}),
        "back": check_png_group("graphics/pokemon", "back.png", {(64, 64)}),
        "icon": check_png_group("graphics/pokemon", "icon.png", {(32, 64)}),
        "overworld": check_png_group("graphics/pokemon", "overworld.png", {(192, 32), (384, 64)}),
        "item": check_png_group("graphics/items/icons", "*.png", {(24, 24)}),
        "trainer front": check_png_group("graphics/trainers/front_pics", "*.png", {(64, 64)}),
        "trainer back": check_png_group("graphics/trainers/back_pics", "*.png", {(64, 256), (64, 320)}),
    }
    palette_count = check_jasc_palettes()

    require(png_counts["front"] > 1200, "front-picture source asset inventory is incomplete")
    require(png_counts["back"] > 1200, "back-picture source asset inventory is incomplete")
    require(png_counts["icon"] > 1200, "icon source asset inventory is incomplete")

    if failures:
        for failure in failures:
            print(f"FAIL: {failure}")
        return 1
    print(
        f"graphics integrity: PASS ({len(front)} species tables, "
        f"{sum(png_counts.values())} PNG assets, {palette_count} JASC palettes)"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
