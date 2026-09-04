"""Read the same held-item category table used by the game."""

from pathlib import Path
import re


def battle_item_categories(root: Path) -> dict[str, tuple[str, ...]]:
    source = (root / "src/field_specials.c").read_text()
    table = re.search(
        r"\bsEmeraldChampionsBattleItemCategories\s*\[\]\s*=\s*\{(.*?)\};",
        source, re.S,
    )
    if table is None:
        raise ValueError("game held-item category table is missing")
    categories = {}
    for category, array in re.findall(r"\[(EC_BATTLE_ITEM_CATEGORY_\w+)\]\s*=\s*(\w+)", table.group(1)):
        match = re.search(r"\b" + re.escape(array) + r"\s*\[\]\s*=\s*\{(.*?)\};", source, re.S)
        if match is None:
            raise ValueError(f"held-item category {category} references missing array {array}")
        items = re.findall(r"\bITEM_[A-Z0-9_]+\b", match.group(1))
        if not items or items[-1] != "ITEM_NONE" or "ITEM_NONE" in items[:-1]:
            raise ValueError(f"held-item category {category} has an invalid terminator")
        if category in categories:
            raise ValueError(f"duplicate held-item category {category}")
        categories[category] = tuple(items[:-1])
    if not categories:
        raise ValueError("game held-item category table is empty")
    return categories


def free_vendor_items(root: Path) -> set[str]:
    return {item for items in battle_item_categories(root).values() for item in items}
