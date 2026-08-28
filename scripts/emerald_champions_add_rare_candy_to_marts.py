#!/usr/bin/env python3
"""Add Rare Candy to each Hoenn mart list that already sells medicine."""

from __future__ import annotations

from pathlib import Path


MEDICINE = {
    "ITEM_POTION",
    "ITEM_SUPER_POTION",
    "ITEM_HYPER_POTION",
    "ITEM_MAX_POTION",
    "ITEM_FULL_RESTORE",
}


def update(path: Path) -> int:
    lines = path.read_text().splitlines(keepends=True)
    inserted = 0
    index = 0
    while index < len(lines):
        if lines[index].strip() != "pokemartlistend":
            index += 1
            continue

        start = index - 1
        while start >= 0 and lines[start].lstrip().startswith(".2byte ITEM_"):
            start -= 1
        items = {
            line.strip().split()[-1]
            for line in lines[start + 1 : index]
            if line.lstrip().startswith(".2byte ITEM_")
        }
        if items.intersection(MEDICINE) and "ITEM_RARE_CANDY" not in items:
            lines.insert(index, "\t.2byte ITEM_RARE_CANDY\n")
            inserted += 1
            index += 1
        index += 1

    if inserted:
        path.write_text("".join(lines))
    return inserted


def main() -> None:
    root = Path(__file__).resolve().parents[1]
    paths = [root / "data" / "scripts" / "mart_clerk.inc"]
    paths.extend(
        path
        for path in (root / "data" / "maps").glob("*/scripts.inc")
        if "_Frlg" not in path.parent.name
    )
    changed_lists = sum(update(path) for path in paths)
    print(f"medicine_mart_lists_updated={changed_lists}")


if __name__ == "__main__":
    main()
