#!/usr/bin/env python3
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


storage = read("src/pokemon_storage_system.c")
pokedex = read("src/pokedex.c")
palette = read("include/palette_util.h")
field_tasks = read("src/field_tasks.c")
move_text = read("src/data/text/move_descriptions.h")
safari_text = read("data/scripts/safari_zone.inc")
base_stats = read("src/data/pokemon/base_stats.h")
route_111 = read("data/maps/Route111/scripts.inc")

checks = {
    "PC item icon staging buffer covers the complete sprite sheet": (
        "sItemIconGfxBuffer[0x200 / sizeof(u32)]" in storage
        and "spriteSheet.size = 0x200;" in storage
    ),
    "Pokédex display personalities cannot select shiny palettes": (
        "pokedexOtId = personality ^ SHINY_ODDS;" in pokedex
        and "CreateMonPicSprite(nationalNum, pokedexOtId, personality" in pokedex
    ),
    "pulse blend coefficient preserves the full unsigned hardware range": (
        "u8 maxBlendCoeff:4;" in palette
        and "s8 maxBlendCoeff:4;" not in palette
    ),
    "Sootopolis ice history uses a bounded direct bit test": (
        "VarGet(sSootopolisGymIceRowVars[y]) & (1 << (x - 3))" in field_tasks
        and "0x10000 << (x - 3)" not in field_tasks
    ),
    "Mystical Fire describes its guaranteed stat drop": (
        '"fire. Lowers Sp. Atk."' in move_text
        and '"fire. May lower Sp. Atk."' not in move_text
    ),
    "only Mega Evolution remains an active battle gimmick": (
        "2x damage to Dynamaxed foes" not in move_text
        and all(
            f"[MOVE_{move}]" in read("src/data/battle_moves.h")
            and "EFFECT_HIT" in read("src/data/battle_moves.h").split(f"[MOVE_{move}]", 1)[1].split("},", 1)[0]
            for move in ("DYNAMAX_CANNON", "BEHEMOTH_BLADE", "BEHEMOTH_BASH")
        )
    ),
    "Safari Zone dialogue uses native species capitalization": (
        "see Pikachu here" in safari_text
        and "see PIKACHU here" not in safari_text
    ),
    "player-facing descriptions use release rather than loose": (
        "Looses" not in move_text
        and "Looses" not in read("src/data/text/item_descriptions.h")
        and "loosing power" not in read("data/text/battle_dome.inc")
    ),
    "Granbull has an explicit second type in the standard data branch": (
        ".type1 = TYPE_FAIRY," in base_stats.split("[SPECIES_GRANBULL]", 1)[1].split("[SPECIES_QWILFISH]", 1)[0]
        and ".type2 = TYPE_FAIRY," in base_stats.split("[SPECIES_GRANBULL]", 1)[1].split("[SPECIES_QWILFISH]", 1)[0]
    ),
    "Route 111 vial nurse disappears without walking through scenery": (
        "Route111_EventScript_UpgradeVialHideNurse::" in route_111
        and "removeobject LOCALID_NURSE" in route_111.split("Route111_EventScript_UpgradeVialHideNurse::", 1)[1].split("end", 1)[0]
        and "applymovement LOCALID_NURSE" not in route_111.split("Route111_EventScript_UpgradeVialHideNurse::", 1)[1].split("end", 1)[0]
    ),
}

failed = [name for name, passed in checks.items() if not passed]
for name, passed in checks.items():
    print(f"{'PASS' if passed else 'FAIL'}: {name}")
if failed:
    raise SystemExit(f"{len(failed)} presentation safety check(s) failed")
print(f"All {len(checks)} presentation safety checks passed")
