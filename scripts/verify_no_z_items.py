#!/usr/bin/env python3
"""Reject any remaining Z-Ring/Z-Crystal item implementation."""

from pathlib import Path
import re
import sys


ROOT = Path(__file__).resolve().parents[1]
SOURCE_ROOTS = ("asm", "data", "include", "src")
SOURCE_SUFFIXES = {".c", ".h", ".inc", ".s"}
FORBIDDEN = {
    "Z item constant": re.compile(r"\bITEM_(?:Z_RING|[A-Z0-9_]*IUM_Z)\b"),
    "Z Crystal hold effect": re.compile(r"\bHOLD_EFFECT_Z_CRYSTAL\b"),
    "Z item icon symbol": re.compile(
        r"\bgItemIcon(?:Palette)?_(?:ZRing|[A-Za-z0-9_]*iumZ)\b"
    ),
    "Z item description symbol": re.compile(
        r"\bs(?:ZRing|[A-Za-z0-9_]*iumZ)Desc\b"
    ),
    "Z item display text": re.compile(r'"(?:Z-Ring|[A-Za-z]+ium Z)"'),
    "Z-Move text": re.compile(r"\bZ-Moves?\b"),
}
ASSET_NAMES = (
    "graphics/items/icons/*ium_z.*",
    "graphics/items/icons/z_ring.*",
    "graphics/items/icon_palettes/*ium_z.*",
    "graphics/items/icon_palettes/z_ring.*",
)


def main() -> int:
    failures: list[str] = []

    constants = (ROOT / "include/constants/items.h").read_text()
    count_match = re.search(
        r"^#define\s+ITEMS_COUNT\s+\((ITEM_[A-Z0-9_]+)\s*\+\s*1\)\s*$",
        constants,
        re.M,
    )
    if count_match is None:
        failures.append("ITEMS_COUNT is not defined as the final active item plus one")
    else:
        terminal_item = count_match.group(1)
        prefix = constants[: count_match.start()]
        item_definitions = re.findall(r"^#define\s+(ITEM_[A-Z0-9_]+)\b", prefix, re.M)
        if not item_definitions or item_definitions[-1] != terminal_item:
            failures.append(
                f"ITEMS_COUNT terminal {terminal_item} is not the final active item definition"
            )

    for source_root in SOURCE_ROOTS:
        for path in (ROOT / source_root).rglob("*"):
            if not path.is_file() or path.suffix not in SOURCE_SUFFIXES:
                continue
            text = path.read_text(errors="replace")
            for label, pattern in FORBIDDEN.items():
                for match in pattern.finditer(text):
                    line = text.count("\n", 0, match.start()) + 1
                    failures.append(
                        f"{path.relative_to(ROOT)}:{line}: {label}: {match.group(0)}"
                    )

    for asset_pattern in ASSET_NAMES:
        for path in ROOT.glob(asset_pattern):
            failures.append(f"Z item asset still present: {path.relative_to(ROOT)}")

    if failures:
        print("Z item removal verification failed:")
        for failure in failures:
            print(f"  - {failure}")
        return 1

    print("Z item removal verification passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
