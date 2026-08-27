#!/usr/bin/env python3
"""Import and verify Verdant's deliberately small Gen 9 content package."""

from __future__ import annotations

import argparse
import json
import re
import shutil
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "docs/verdant_gen9_curated_manifest.json"
ENCOUNTERS = ROOT / "src/data/wild_encounters.json"
ITEM_BALL_SCRIPTS = ROOT / "data/scripts/item_ball_scripts.inc"

BASE_BATTLE_ASSETS = ("front.png", "back.png", "icon.png", "normal.pal", "shiny.pal")
BASE_OVERWORLD_ASSETS = ("overworld.png", "overworld_normal.pal", "overworld_shiny.pal")
MIN_CURATED_CATCH_RATE = 45

# Every family is deliberately obtainable without a low-percent hunt.  The
# slot indices are paired with the native method rates in wild_encounters.json;
# all core sources are at least 10%, except the alternate Tatsugiri forms,
# which share a 60/20/20 Good Rod pool.
WILD_AVAILABILITY = {
    "MAP_ROUTE101": {"land_mons": {4: "SPECIES_SPRIGATITO"}},
    "MAP_ROUTE102": {"land_mons": {4: "SPECIES_NACLI"}},
    "MAP_ROUTE103": {"land_mons": {4: "SPECIES_FUECOCO"}},
    "MAP_ROUTE104": {"fishing_mons": {4: "SPECIES_FINIZEN"}},
    "MAP_RUSTBORO_CITY": {"land_mons": {4: "SPECIES_GIMMIGHOUL"}},
    "MAP_GRANITE_CAVE_1F": {"land_mons": {3: "SPECIES_GLIMMET"}},
    "MAP_ROUTE110": {"land_mons": {4: "SPECIES_GIMMIGHOUL_ROAMING"}},
    "MAP_ROUTE111": {"land_mons": {4: "SPECIES_GREAT_TUSK"}},
    "MAP_ROUTE115": {"land_mons": {4: "SPECIES_DURALUDON"}},
    "MAP_ROUTE118": {
        "water_mons": {1: "SPECIES_DONDOZO"},
        "fishing_mons": {
            2: "SPECIES_TATSUGIRI",
            3: "SPECIES_TATSUGIRI_DROOPY",
            4: "SPECIES_TATSUGIRI_STRETCHY",
        },
    },
    "MAP_ROUTE119": {"land_mons": {4: "SPECIES_RAGING_BOLT"}},
    "MAP_ROUTE120": {"land_mons": {4: "SPECIES_OGERPON"}},
    "MAP_MT_PYRE_SUMMIT": {"land_mons": {4: "SPECIES_FLUTTER_MANE"}},
    "MAP_NEW_MAUVILLE_INSIDE": {"land_mons": {4: "SPECIES_IRON_HANDS"}},
    "MAP_CAVE_OF_ORIGIN_1F": {"land_mons": {4: "SPECIES_WALKING_WAKE"}},
    "MAP_MAGMA_HIDEOUT_1F": {"land_mons": {4: "SPECIES_GOUGING_FIRE"}},
    "MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM": {
        "land_mons": {4: "SPECIES_IRON_BUNDLE", 5: "SPECIES_CHIEN_PAO"}
    },
    "MAP_DESERT_UNDERPASS": {"land_mons": {4: "SPECIES_TING_LU"}},
    "MAP_ASHEN_WOODS": {"land_mons": {4: "SPECIES_CHI_YU"}},
    # Steven's Cave is opened only after game clear.  Use the Waterfall-gated
    # pre-League room so every curated endpoint can join the campaign before
    # the credits.
    "MAP_METEOR_FALLS_B1F_2R": {"land_mons": {4: "SPECIES_ROARING_MOON"}},
    "MAP_VICTORY_ROAD_1F": {"land_mons": {4: "SPECIES_IRON_VALIANT"}},
}

# Clear the former postgame-only source when restoring availability.  This is
# not an additional curated source and therefore is excluded from the reported
# 25-source total.
RETIRED_WILD_SOURCES = {
    "MAP_METEOR_FALLS_STEVENS_CAVE": {"land_mons": {4: "SPECIES_METAGROSS"}},
}

# These replace redundant Rare Candy balls; Rare Candy remains universal Mart
# stock for $1,000, so exploration rewards unique progression instead.
WORLD_ITEM_AVAILABILITY = {
    "Route111_EventScript_ItemTM63RockSlide": "ITEM_CORNERSTONE_MASK",
    "Route114_EventScript_ItemRareCandy": "ITEM_LEADERS_CREST",
    "Route119_EventScript_ItemTM84_Poison_Jab": "ITEM_GLIMMORANITE",
    "Route119_EventScript_ItemRareCandy": "ITEM_METAL_ALLOY",
    "Route127_EventScript_ItemRareCandy": "ITEM_WELLSPRING_MASK",
    "Route132_EventScript_ItemRareCandy": "ITEM_TATSUGIRINITE",
    "GraniteCave_B2F_EventScript_ItemRareCandy": "ITEM_GIMMIGHOUL_COIN",
    "MagmaHideout_1F_EventScript_ItemRareCandy": "ITEM_FOSSILIZED_DRAKE",
    "MagmaHideout_3F_3R_EventScript_ItemTM35Flamethrower": "ITEM_HEARTHFLAME_MASK",
}


def load_manifest() -> dict:
    data = json.loads(MANIFEST.read_text())
    entries = data["entries"]
    policy = data["policy"]
    if len(entries) != policy["numeric_entry_count"]:
        raise ValueError(
            f"manifest declares {policy['numeric_entry_count']} entries, found {len(entries)}"
        )
    if sum(entry["kind"] == "mega" for entry in entries) != policy["mega_forms"]:
        raise ValueError("manifest Mega count drifted")
    if sum(entry["kind"] == "endpoint" for entry in entries) != policy["endpoint_count"]:
        raise ValueError("manifest endpoint count drifted")
    if sum(entry["kind"] != "mega" for entry in entries) != policy["standard_species_and_forms"]:
        raise ValueError("manifest standard species/form count drifted")
    if any("TERA" in entry["species"] for entry in entries):
        raise ValueError("Tera-only species entered the curated package")
    return data


def required_files(entry: dict) -> tuple[str, ...]:
    files = list(BASE_BATTLE_ASSETS)
    if entry.get("overworld_asset", True):
        files.extend(BASE_OVERWORLD_ASSETS)
    return tuple(files)


def asset_source_path(root: Path, entry: dict, name: str, *, imported: bool) -> Path:
    asset = entry["asset"]
    if not imported and name in {"normal.pal", "shiny.pal"}:
        asset = entry.get("palette_asset", asset)
    return root / "graphics/pokemon" / asset / name


def asset_problems(root: Path, entries: list[dict], *, imported: bool) -> list[str]:
    problems = []
    for entry in entries:
        for name in required_files(entry):
            path = asset_source_path(root, entry, name, imported=imported)
            if not path.is_file():
                problems.append(f"{entry['species']}: missing {path}")
    return problems


def import_assets(source: Path, entries: list[dict]) -> None:
    problems = asset_problems(source, entries, imported=False)
    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))

    copied = 0
    for entry in entries:
        source_dir = source / "graphics/pokemon" / entry["asset"]
        target_dir = ROOT / "graphics/pokemon" / entry["asset"]
        target_dir.mkdir(parents=True, exist_ok=True)
        for name in required_files(entry):
            source_file = asset_source_path(source, entry, name, imported=False)
            target_file = target_dir / name
            if target_file.exists() and target_file.read_bytes() != source_file.read_bytes():
                raise SystemExit(f"FAIL: refusing to overwrite different asset {target_file}")
            if not target_file.exists():
                shutil.copy2(source_file, target_file)
                copied += 1
        # Footprints are family-shared for several alternate forms. Copy a
        # dedicated footprint when upstream actually provides one.
        footprint = source_dir / "footprint.png"
        if footprint.is_file():
            target = target_dir / "footprint.png"
            if target.exists() and target.read_bytes() != footprint.read_bytes():
                raise SystemExit(f"FAIL: refusing to overwrite different asset {target}")
            if not target.exists():
                shutil.copy2(footprint, target)
                copied += 1
    print(f"imported {copied} curated Gen 9 asset files")


def symbol(species: str) -> str:
    return "".join(part.title() for part in species.removeprefix("SPECIES_").split("_"))


def species_blocks(source: Path) -> dict[str, str]:
    text = "\n".join(
        path.read_text()
        for path in sorted((source / "src/data/pokemon/species_info").glob("*.h"))
    )
    blocks = {
        species: body
        for species, body in re.findall(
            r"^\s*\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{(.*?)"
            r"(?=^\s*\[SPECIES_[A-Z0-9_]+\]\s*=|\Z)",
            text,
            re.M | re.S,
        )
    }
    blocks.update(
        {
            species: f"OGERPON_SPECIES_INFO({arguments})"
            for species, arguments in re.findall(
                r"^\s*\[(SPECIES_OGERPON_[A-Z0-9_]+)\]\s*=\s*"
                r"OGERPON_SPECIES_INFO\((.*?)\),\s*$",
                text,
                re.M,
            )
        }
    )
    return blocks


def integer_field(body: str, name: str, default: int = 0) -> int:
    match = re.search(rf"\.{name}\s*=\s*(-?\d+)", body)
    return int(match.group(1)) if match else default


def field_expression(body: str, name: str, default: str) -> str:
    match = re.search(rf"\.{name}\s*=\s*([^,\n]+)", body)
    if not match:
        return default
    expression = match.group(1).strip()
    if "?" in expression and ":" in expression:
        expression = expression.split("?", 1)[1].split(":", 1)[0].strip()
    return expression


def integer_expression(body: str, name: str, default: int) -> int:
    expression = field_expression(body, name, str(default))
    match = re.search(r"-?\d+", expression)
    return int(match.group(0)) if match else default


def footprint_owner(entry: dict) -> str:
    path = Path(entry["asset"])
    while path.parts:
        if (ROOT / "graphics/pokemon" / path / "footprint.png").is_file():
            for candidate in load_manifest()["entries"]:
                if candidate["asset"] == path.as_posix():
                    return symbol(candidate["species"])
            return "".join(part.title() for part in path.parts[-1].split("_"))
        path = path.parent
    return "Bulbasaur"


def write_generated(path: str, text: str) -> None:
    target = ROOT / path
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_text(text.rstrip() + "\n")


def generate_graphics(source: Path, entries: list[dict]) -> None:
    blocks = species_blocks(source)
    aliases = load_manifest().get("aliases", {})
    upstream_for = {
        entry["species"]: next(
            (alias for alias, target in aliases.items() if target == entry["species"]),
            entry["species"],
        )
        for entry in entries
    }
    missing = [
        entry["species"]
        for entry in entries
        if upstream_for[entry["species"]] not in blocks
    ]
    if missing:
        raise SystemExit("FAIL: missing upstream species info: " + ", ".join(missing))

    externs = ["// Generated by scripts/verdant_gen9_curated.py."]
    definitions = ["// Generated by scripts/verdant_gen9_curated.py."]
    front_table = []
    back_table = []
    palettes = []
    shiny_palettes = []
    front_coords = []
    back_coords = []
    icon_table = []
    icon_palettes = []
    footprint_table = []
    anim_table = []
    enemy_elevations = []
    defined_footprints: set[str] = set()

    for entry in entries:
        species = entry["species"]
        sym = symbol(species)
        asset = entry["asset"]
        body = blocks[upstream_for[species]]
        if body.startswith("OGERPON_SPECIES_INFO("):
            arguments = [part.strip() for part in body.removeprefix("OGERPON_SPECIES_INFO(").removesuffix(")").split(",")]
            front_y = int(arguments[-3])
            back_y = 0
            icon_pal = int(arguments[-2])
        else:
            front_y = integer_field(body, "frontPicYOffset")
            back_y = integer_field(body, "backPicYOffset")
            icon_pal = integer_field(body, "iconPalIndex")

        externs.extend(
            (
                f"extern const u32 gMonFrontPic_{sym}[];",
                f"extern const u32 gMonBackPic_{sym}[];",
                f"extern const u32 gMonPalette_{sym}[];",
                f"extern const u32 gMonShinyPalette_{sym}[];",
                f"extern const u8 gMonIcon_{sym}[];",
            )
        )
        definitions.extend(
            (
                f'const u32 gMonFrontPic_{sym}[] = INCBIN_U32("graphics/pokemon/{asset}/front.4bpp.lz");',
                f'const u32 gMonBackPic_{sym}[] = INCBIN_U32("graphics/pokemon/{asset}/back.4bpp.lz");',
                f'const u32 gMonPalette_{sym}[] = INCBIN_U32("graphics/pokemon/{asset}/normal.gbapal.lz");',
                f'const u32 gMonShinyPalette_{sym}[] = INCBIN_U32("graphics/pokemon/{asset}/shiny.gbapal.lz");',
                f'const u8 gMonIcon_{sym}[] = INCBIN_U8("graphics/pokemon/{asset}/icon.4bpp");',
            )
        )
        footprint = ROOT / "graphics/pokemon" / asset / "footprint.png"
        if footprint.is_file():
            externs.append(f"extern const u8 gMonFootprint_{sym}[];")
            definitions.append(
                f'const u8 gMonFootprint_{sym}[] = INCBIN_U8("graphics/pokemon/{asset}/footprint.1bpp");'
            )
            defined_footprints.add(sym)

        short = species.removeprefix("SPECIES_")
        front_table.append(f"    SPECIES_SPRITE({short}, gMonFrontPic_{sym}),")
        back_table.append(f"    SPECIES_SPRITE({short}, gMonBackPic_{sym}),")
        palettes.append(f"    SPECIES_PAL({short}, gMonPalette_{sym}),")
        shiny_palettes.append(f"    SPECIES_SHINY_PAL({short}, gMonShinyPalette_{sym}),")
        front_coords.append(
            f"    [{species}] = {{.size = 0x88, .y_offset = {front_y}}},"
        )
        back_coords.append(
            f"    [{species}] = {{.size = 0x88, .y_offset = {back_y}}},"
        )
        icon_table.append(f"    [{species}] = gMonIcon_{sym},")
        icon_palettes.append(f"    [{species}] = {icon_pal},")
        anim_table.append(f"    ANIM_CMD_FULL({short}, sAnims_VerdantGen9),")
        elevation = 0 if body.startswith("OGERPON_SPECIES_INFO(") else integer_field(body, "enemyMonElevation")
        if elevation:
            enemy_elevations.append(f"    [{species}] = {elevation},")

    for entry in entries:
        footprint_table.append(
            f"    [{entry['species']}] = gMonFootprint_{footprint_owner(entry)},"
        )

    write_generated("include/graphics/verdant_gen9_curated.h", "\n".join(externs))
    write_generated("src/data/graphics/verdant_gen9_curated.h", "\n".join(definitions))
    write_generated("src/data/pokemon_graphics/verdant_gen9_front_pic_table.h", "\n".join(front_table))
    write_generated("src/data/pokemon_graphics/verdant_gen9_back_pic_table.h", "\n".join(back_table))
    write_generated("src/data/pokemon_graphics/verdant_gen9_palette_table.h", "\n".join(palettes))
    write_generated("src/data/pokemon_graphics/verdant_gen9_shiny_palette_table.h", "\n".join(shiny_palettes))
    write_generated("src/data/pokemon_graphics/verdant_gen9_front_pic_coordinates.h", "\n".join(front_coords))
    write_generated("src/data/pokemon_graphics/verdant_gen9_back_pic_coordinates.h", "\n".join(back_coords))
    write_generated("src/data/pokemon_graphics/verdant_gen9_icon_table.h", "\n".join(icon_table))
    write_generated("src/data/pokemon_graphics/verdant_gen9_icon_palettes.h", "\n".join(icon_palettes))
    write_generated("src/data/pokemon_graphics/verdant_gen9_footprint_table.h", "\n".join(footprint_table))
    write_generated("src/data/pokemon_graphics/verdant_gen9_anim_table.h", "\n".join(anim_table))
    write_generated(
        "src/data/pokemon_graphics/verdant_gen9_enemy_mon_elevation.h",
        "\n".join(enemy_elevations),
    )
    print(f"generated graphics registration for {len(entries)} curated entries")


NAME_OVERRIDES = {
    "SPECIES_MEOWSCARADA": "Meowscrada",
    "SPECIES_FLUTTER_MANE": "FluttrMane",
    "SPECIES_IRON_BUNDLE": "IronBundle",
    "SPECIES_ROARING_MOON": "RoarngMoon",
    "SPECIES_IRON_VALIANT": "IronValnt",
    "SPECIES_WALKING_WAKE": "WalkngWake",
    "SPECIES_GOUGING_FIRE": "GougingFir",
    "SPECIES_RAGING_BOLT": "RagingBolt",
}


def upstream_species_map(data: dict) -> dict[str, str]:
    aliases = data.get("aliases", {})
    return {
        entry["species"]: next(
            (alias for alias, target in aliases.items() if target == entry["species"]),
            entry["species"],
        )
        for entry in data["entries"]
    }


def ogerpon_data(body: str) -> dict[str, str | int | list[str]]:
    args = [part.strip() for part in body.removeprefix("OGERPON_SPECIES_INFO(").removesuffix(")").split(",")]
    return {
        "baseHP": 80,
        "baseAttack": 120,
        "baseDefense": 84,
        "baseSpeed": 110,
        "baseSpAttack": 60,
        "baseSpDefense": 96,
        "types": ["TYPE_GRASS", args[2]],
        "catchRate": 5,
        "expYield": 275,
        "evYield_Attack": 3,
        "genderRatio": "MON_FEMALE",
        "eggCycles": 10,
        "friendship": 70,
        "growthRate": "GROWTH_SLOW",
        "eggGroups": ["EGG_GROUP_UNDISCOVERED", "EGG_GROUP_UNDISCOVERED"],
        "abilities": [args[3], "ABILITY_NONE", args[3]],
        "bodyColor": args[4],
        "noFlip": "FALSE",
        "name": "Ogerpon",
    }


def translated_species_data(species: str, body: str) -> dict:
    if body.startswith("OGERPON_SPECIES_INFO("):
        return ogerpon_data(body)

    types_match = re.search(r"\.types\s*=\s*MON_TYPES\(([^)]+)\)", body)
    types = [part.strip() for part in types_match.group(1).split(",")] if types_match else ["TYPE_NORMAL"]
    if len(types) == 1:
        types.append(types[0])

    egg_match = re.search(r"\.eggGroups\s*=\s*MON_EGG_GROUPS\(([^)]+)\)", body)
    egg_groups = [part.strip().replace("EGG_GROUP_NO_EGGS_DISCOVERED", "EGG_GROUP_UNDISCOVERED") for part in egg_match.group(1).split(",")] if egg_match else ["EGG_GROUP_UNDISCOVERED"]
    if len(egg_groups) == 1:
        egg_groups.append(egg_groups[0])

    abilities_match = re.search(r"\.abilities\s*=\s*\{([^}]+)\}", body)
    abilities = re.findall(r"ABILITY_[A-Z0-9_]+", abilities_match.group(1)) if abilities_match else ["ABILITY_NONE"]
    abilities = (abilities + ["ABILITY_NONE"] * 3)[:3]

    name_match = re.search(r'\.speciesName\s*=\s*_\("([^"]+)"\)', body)
    return {
        **{field: integer_expression(body, field, 1) for field in ("baseHP", "baseAttack", "baseDefense", "baseSpeed", "baseSpAttack", "baseSpDefense")},
        "types": types[:2],
        "catchRate": integer_expression(body, "catchRate", 45),
        "expYield": integer_expression(body, "expYield", 100),
        **{field: integer_expression(body, field, 0) for field in ("evYield_HP", "evYield_Attack", "evYield_Defense", "evYield_Speed", "evYield_SpAttack", "evYield_SpDefense")},
        "genderRatio": field_expression(body, "genderRatio", "MON_GENDERLESS"),
        "eggCycles": integer_expression(body, "eggCycles", 20),
        "friendship": 70 if field_expression(body, "friendship", "70") == "STANDARD_FRIENDSHIP" else integer_expression(body, "friendship", 70),
        "growthRate": field_expression(body, "growthRate", "GROWTH_MEDIUM_FAST"),
        "eggGroups": egg_groups[:2],
        "abilities": abilities,
        "bodyColor": field_expression(body, "bodyColor", "BODY_COLOR_GRAY"),
        "noFlip": field_expression(body, "noFlip", "FALSE"),
        "name": name_match.group(1) if name_match else species.removeprefix("SPECIES_").title(),
    }


def national_dex_suffix(species: str) -> str:
    if species.startswith("SPECIES_TATSUGIRI"):
        return "TATSUGIRI"
    if species.startswith("SPECIES_OGERPON"):
        return "OGERPON"
    if species.startswith("SPECIES_PALAFIN"):
        return "PALAFIN"
    if species.startswith("SPECIES_GIMMIGHOUL"):
        return "GIMMIGHOUL"
    if species == "SPECIES_GLIMMORA_MEGA":
        return "GLIMMORA"
    return species.removeprefix("SPECIES_").removesuffix("_MEGA")


def generate_species(source: Path, data: dict) -> None:
    entries = data["entries"]
    blocks = species_blocks(source)
    upstream_for = upstream_species_map(data)
    base_stats = ["// Generated by scripts/verdant_gen9_curated.py."]
    names = ["// Generated by scripts/verdant_gen9_curated.py."]
    national_map = ["// Generated by scripts/verdant_gen9_curated.py."]

    for entry in entries:
        species = entry["species"]
        body = blocks[upstream_for[species]]
        translated = translated_species_data(species, body)
        abilities = ", ".join(translated["abilities"])
        lines = [
            f"    [{species}] =",
            "    {",
            f"        .baseHP = {translated['baseHP']},",
            f"        .baseAttack = {translated['baseAttack']},",
            f"        .baseDefense = {translated['baseDefense']},",
            f"        .baseSpeed = {translated['baseSpeed']},",
            f"        .baseSpAttack = {translated['baseSpAttack']},",
            f"        .baseSpDefense = {translated['baseSpDefense']},",
            f"        .type1 = {translated['types'][0]},",
            f"        .type2 = {translated['types'][1]},",
            f"        .catchRate = {max(MIN_CURATED_CATCH_RATE, translated['catchRate'])},",
            f"        .expYield = {translated['expYield']},",
        ]
        for field in ("HP", "Attack", "Defense", "Speed", "SpAttack", "SpDefense"):
            value = translated.get(f"evYield_{field}", 0)
            if value:
                lines.append(f"        .evYield_{field} = {value},")
        lines.extend(
            (
                f"        .genderRatio = {translated['genderRatio']},",
                f"        .eggCycles = {translated['eggCycles']},",
                f"        .friendship = {translated['friendship']},",
                f"        .growthRate = {translated['growthRate']},",
                f"        .eggGroup1 = {translated['eggGroups'][0]},",
                f"        .eggGroup2 = {translated['eggGroups'][1]},",
                f"        .abilities = {{{abilities}}},",
                f"        .bodyColor = {translated['bodyColor']},",
                f"        .noFlip = {translated['noFlip']},",
                "    },",
            )
        )
        base_stats.extend(lines)

        display_name = NAME_OVERRIDES.get(species, translated["name"])
        if len(display_name) > 10:
            raise SystemExit(f"FAIL: no 10-character name policy for {species}: {display_name}")
        names.append(f'    [{species}] = _("{display_name}"),')
        national_map.append(
            f"    [{species} - 1] = NATIONAL_DEX_{national_dex_suffix(species)},"
        )

    write_generated("src/data/pokemon/verdant_gen9_base_stats.h", "\n".join(base_stats))
    write_generated("src/data/text/verdant_gen9_species_names.h", "\n".join(names))
    write_generated("src/data/pokemon/verdant_gen9_national_map.h", "\n".join(national_map))
    print(f"generated base species data for {len(entries)} curated entries")


OGERPON_POKEDEX_DESCRIPTION = (
    "This Pokémon's type changes based on\\n",
    "which mask it's wearing. It confounds\\n",
    "its enemies with nimble movements\\n",
    "and kicks.",
)

POKEDEX_DESCRIPTION_OVERRIDES = {
    "GLIMMORA": (
        "Glimmora's petals are made of\\n",
        "crystallized poison energy. When\\n",
        "it detects danger, it opens them\\n",
        "and fires a poisonous beam.",
    ),
    "RAGING_BOLT": (
        "It bears resemblance to a Pokémon\\n",
        "that became a hot topic for a short\\n",
        "while after a paranormal magazine\\n",
        "touted it as Raikou's ancestor.",
    ),
}


def generate_pokedex(source: Path, data: dict) -> None:
    blocks = species_blocks(source)
    upstream_for = upstream_species_map(data)
    text_output = ["// Generated by scripts/verdant_gen9_curated.py."]
    entry_output = ["// Generated by scripts/verdant_gen9_curated.py."]
    generated: set[str] = set()

    for entry in data["entries"]:
        species = entry["species"]
        suffix = national_dex_suffix(species)
        if suffix in generated:
            continue
        generated.add(suffix)
        body = blocks[upstream_for[species]]
        sym = symbol("SPECIES_" + suffix)

        if body.startswith("OGERPON_SPECIES_INFO("):
            category = "Mask"
            height = 12
            weight = 398
            description = OGERPON_POKEDEX_DESCRIPTION
            pokemon_scale, pokemon_offset = 356, 17
            trainer_scale, trainer_offset = 256, 0
        else:
            category_match = re.search(r'\.categoryName\s*=\s*_\("([^"]+)"\)', body)
            if not category_match:
                raise SystemExit(f"FAIL: {species} has no upstream Pokédex category")
            category = category_match.group(1)
            height = integer_expression(body, "height", 0)
            weight = integer_expression(body, "weight", 0)
            description_match = re.search(
                r"\.description\s*=\s*COMPOUND_STRING\((.*?)\),\s*\.pokemonScale",
                body,
                re.S,
            )
            if not description_match:
                raise SystemExit(f"FAIL: {species} has no inline upstream Pokédex text")
            description = tuple(
                re.findall(r'"((?:\\.|[^"])*)"', description_match.group(1))
            )
            pokemon_scale = integer_expression(body, "pokemonScale", 356)
            pokemon_offset = integer_expression(body, "pokemonOffset", 17)
            trainer_scale = integer_expression(body, "trainerScale", 256)
            trainer_offset = integer_expression(body, "trainerOffset", 0)

        description = POKEDEX_DESCRIPTION_OVERRIDES.get(suffix, description)

        text_symbol = f"gVerdantGen9{sym}PokedexText"
        text_output.append(f"const u8 {text_symbol}[] = _(")
        for index, line in enumerate(description):
            terminator = ");" if index == len(description) - 1 else ""
            text_output.append(f'    "{line}"{terminator}')

        entry_output.extend(
            (
                f"    [NATIONAL_DEX_{suffix}] =",
                "    {",
                f'        .categoryName = _("{category}"),',
                f"        .height = {height},",
                f"        .weight = {weight},",
                f"        .description = {text_symbol},",
                f"        .pokemonScale = {pokemon_scale},",
                f"        .pokemonOffset = {pokemon_offset},",
                f"        .trainerScale = {trainer_scale},",
                f"        .trainerOffset = {trainer_offset},",
                "    },",
            )
        )

    write_generated(
        "src/data/pokemon/verdant_gen9_pokedex_text.h", "\n".join(text_output)
    )
    write_generated(
        "src/data/pokemon/verdant_gen9_pokedex_entries.h", "\n".join(entry_output)
    )

    local_orders = (ROOT / "src/data/pokemon/pokedex_orders.h").read_text()
    upstream_orders = (source / "src/data/pokemon/pokedex_orders.h").read_text()
    alphabetical_body = local_orders.split(
        "const u16 gPokedexOrder_Alphabetical[] =", 1
    )[1].split("};", 1)[0]
    allowed = set(re.findall(r"NATIONAL_DEX_[A-Z0-9_]+", alphabetical_body))
    allowed.update(f"NATIONAL_DEX_{suffix}" for suffix in generated)
    order_output = ["// Generated by scripts/verdant_gen9_curated.py."]
    for source_name, target_name in (
        ("gPokedexOrder_Alphabetical", "gVerdantPokedexOrder_Alphabetical"),
        ("gPokedexOrder_Weight", "gVerdantPokedexOrder_Weight"),
        ("gPokedexOrder_Height", "gVerdantPokedexOrder_Height"),
    ):
        body = upstream_orders.split(f"const u16 {source_name}[] =", 1)[1].split(
            "};", 1
        )[0]
        ordered = []
        seen = set()
        for dex_constant in re.findall(r"NATIONAL_DEX_[A-Z0-9_]+", body):
            if dex_constant in allowed and dex_constant not in seen:
                ordered.append(dex_constant)
                seen.add(dex_constant)
        missing = sorted(allowed - seen)
        if missing:
            raise SystemExit(
                f"FAIL: {source_name} lacks curated entries: {', '.join(missing)}"
            )
        order_output.append(f"const u16 {target_name}[] = {{")
        order_output.extend(f"    {dex_constant}," for dex_constant in ordered)
        order_output.append("};")
    write_generated(
        "src/data/pokemon/verdant_gen9_pokedex_orders.h", "\n".join(order_output)
    )
    print(
        f"generated {len(generated)} curated National-Dex entries and "
        f"{len(allowed)}-species native sort orders"
    )


def array_blocks(text: str, type_pattern: str) -> dict[str, str]:
    return {
        name: body
        for name, body in re.findall(
            rf"static const {type_pattern}\s+(s[A-Za-z0-9_]+)\[\]\s*=\s*\{{(.*?)\}};",
            text,
            re.S,
        )
    }


def pointer_field(body: str, name: str, default: str | None = None) -> str | None:
    match = re.search(rf"\.{name}\s*=\s*(s[A-Za-z0-9_]+)", body)
    return match.group(1) if match else default


def generate_learnsets(source: Path, data: dict) -> None:
    entries = data["entries"]
    blocks = species_blocks(source)
    upstream_for = upstream_species_map(data)
    defined_moves = set(
        re.findall(
            r"^#define\s+(MOVE_[A-Z0-9_]+)\b",
            (ROOT / "include/constants/moves.h").read_text(),
            re.M,
        )
    )

    level_text = (source / "src/data/pokemon/level_up_learnsets/gen_9.h").read_text()
    level_arrays = array_blocks(level_text, r"struct LevelUpMove")
    egg_arrays = array_blocks(
        (source / "src/data/pokemon/egg_moves.h").read_text(), r"u16"
    )
    teachable_arrays = array_blocks(
        (source / "src/data/pokemon/teachable_learnsets.h").read_text(), r"u16"
    )

    tm_body = (ROOT / "src/data/party_menu.h").read_text().split(
        "static const u16 sTMHMMoves[] =", 1
    )[1].split("};", 1)[0]
    tm_moves = re.findall(r"MOVE_[A-Z0-9_]+", tm_body)
    tutor_body = (ROOT / "src/data/pokemon/tutor_learnsets.h").read_text().split(
        "const u16 gTutorMoves[] =", 1
    )[1].split("};", 1)[0]
    tutor_moves = re.findall(r"MOVE_[A-Z0-9_]+", tutor_body)

    level_output = ["// Generated by scripts/verdant_gen9_curated.py."]
    pointer_output = ["// Generated by scripts/verdant_gen9_curated.py."]
    egg_output = ["// Generated by scripts/verdant_gen9_curated.py."]
    tm_output = ["// Generated by scripts/verdant_gen9_curated.py."]
    tutor_output = ["// Generated by scripts/verdant_gen9_curated.py."]
    max_level_moves = 0
    max_egg_moves = 0

    for entry in entries:
        species = entry["species"]
        upstream = upstream_for[species]
        body = blocks[upstream]
        if body.startswith("OGERPON_SPECIES_INFO("):
            level_pointer = "sOgerponLevelUpLearnset"
            teachable_pointer = "sOgerponTeachableLearnset"
            egg_pointer = None
        else:
            level_pointer = pointer_field(body, "levelUpLearnset")
            teachable_pointer = pointer_field(body, "teachableLearnset")
            egg_pointer = pointer_field(body, "eggMoveLearnset")

        if level_pointer not in level_arrays:
            raise SystemExit(f"FAIL: {species} has no upstream Gen 9 level learnset")
        level_moves = [
            (int(level), move)
            for level, move in re.findall(
                r"LEVEL_UP_MOVE\(\s*(\d+)\s*,\s*(MOVE_[A-Z0-9_]+)\s*\)",
                level_arrays[level_pointer],
            )
            if move in defined_moves
        ]
        if not level_moves:
            level_moves = [(1, "MOVE_TACKLE")]
        max_level_moves = max(max_level_moves, len(level_moves))
        array_name = f"sVerdantGen9{symbol(species)}LevelUpLearnset"
        level_output.append(f"static const struct LevelUpMove {array_name}[] = {{")
        level_output.extend(
            f"    LEVEL_UP_MOVE({level:2d}, {move})," for level, move in level_moves
        )
        level_output.extend(("    LEVEL_UP_END", "};"))
        pointer_output.append(f"    [{species}] = {array_name},")

        egg_moves = []
        if egg_pointer in egg_arrays:
            egg_moves = [
                move
                for move in re.findall(r"MOVE_[A-Z0-9_]+", egg_arrays[egg_pointer])
                if move in defined_moves and move != "MOVE_UNAVAILABLE"
            ]
        max_egg_moves = max(max_egg_moves, len(egg_moves))
        if egg_moves:
            short = species.removeprefix("SPECIES_")
            egg_output.append(f"    egg_moves({short},")
            egg_output.append("        " + ",\n        ".join(egg_moves) + "),")

        teachable = set()
        if teachable_pointer in teachable_arrays:
            teachable = {
                move
                for move in re.findall(
                    r"MOVE_[A-Z0-9_]+", teachable_arrays[teachable_pointer]
                )
                if move in defined_moves and move != "MOVE_UNAVAILABLE"
            }
        tm_words = [0, 0, 0, 0]
        for index, move in enumerate(tm_moves[:128]):
            if move in teachable:
                tm_words[index // 32] |= 1 << (index % 32)
        tm_output.append(
            f"    [{species}] = {{{','.join(f'0x{word:08X}' for word in tm_words)}}},"
        )
        tutor_words = [0, 0, 0, 0, 0]
        for index, move in enumerate(tutor_moves[:160]):
            if move in teachable:
                tutor_words[index // 32] |= 1 << (index % 32)
        tutor_output.append(
            f"    [{species}] = {{{','.join(f'0x{word:08X}' for word in tutor_words)}}},"
        )

    write_generated("src/data/pokemon/verdant_gen9_level_up_learnsets.h", "\n".join(level_output))
    write_generated("src/data/pokemon/verdant_gen9_level_up_pointers.h", "\n".join(pointer_output))
    write_generated("src/data/pokemon/verdant_gen9_egg_moves.h", "\n".join(egg_output))
    write_generated("src/data/pokemon/verdant_gen9_tmhm_learnsets.h", "\n".join(tm_output))
    write_generated("src/data/pokemon/verdant_gen9_tutor_learnsets.h", "\n".join(tutor_output))
    print(
        f"generated curated learnsets; max level moves {max_level_moves}, "
        f"max egg moves {max_egg_moves}"
    )


def encounter_tables(data: dict) -> tuple[dict[str, list[int]], dict[str, dict]]:
    group = data["wild_encounter_groups"][0]
    rates = {field["type"]: field["encounter_rates"] for field in group["fields"]}
    by_map = {entry["map"]: entry for entry in group["encounters"] if "map" in entry}
    return rates, by_map


def apply_availability() -> None:
    data = json.loads(ENCOUNTERS.read_text())
    _, by_map = encounter_tables(data)
    changed_slots = 0
    for map_id, methods in WILD_AVAILABILITY.items():
        if map_id not in by_map:
            raise SystemExit(f"FAIL: missing encounter map {map_id}")
        for method, slots in methods.items():
            if method not in by_map[map_id]:
                raise SystemExit(f"FAIL: {map_id} has no {method}")
            mons = by_map[map_id][method]["mons"]
            for index, species in slots.items():
                if index >= len(mons):
                    raise SystemExit(f"FAIL: {map_id} {method} has no slot {index}")
                if mons[index]["species"] != species:
                    mons[index]["species"] = species
                    changed_slots += 1
    for map_id, methods in RETIRED_WILD_SOURCES.items():
        for method, slots in methods.items():
            mons = by_map[map_id][method]["mons"]
            for index, species in slots.items():
                if mons[index]["species"] != species:
                    mons[index]["species"] = species
                    changed_slots += 1
    ENCOUNTERS.write_text(json.dumps(data, indent=2) + "\n")

    scripts = ITEM_BALL_SCRIPTS.read_text()
    changed_items = 0
    for label, item in WORLD_ITEM_AVAILABILITY.items():
        pattern = re.compile(
            rf"(^\s*{re.escape(label)}::[^\n]*\n\s*finditem\s+)(ITEM_[A-Z0-9_]+)",
            re.M,
        )
        match = pattern.search(scripts)
        if not match:
            raise SystemExit(f"FAIL: missing item-ball label {label}")
        if match.group(2) != item:
            if match.group(2) != "ITEM_RARE_CANDY":
                raise SystemExit(
                    f"FAIL: refusing to replace non-Rare-Candy reward at {label}: "
                    f"{match.group(2)}"
                )
            scripts = scripts[: match.start(2)] + item + scripts[match.end(2) :]
            changed_items += 1
    ITEM_BALL_SCRIPTS.write_text(scripts)
    print(
        f"applied curated availability: {changed_slots} wild slots and "
        f"{changed_items} world items changed"
    )


def availability_problems() -> list[str]:
    problems = []
    data = json.loads(ENCOUNTERS.read_text())
    rates, by_map = encounter_tables(data)
    all_wild_species = {
        mon["species"]
        for encounter in by_map.values()
        for method in ("land_mons", "water_mons", "rock_smash_mons", "fishing_mons", "honey_mons")
        for mon in encounter.get(method, {}).get("mons", [])
    }
    constants = (
        (ROOT / "include/constants/species.h").read_text()
        + (ROOT / "include/constants/verdant_gen9_species.h").read_text()
    )
    for map_id, methods in WILD_AVAILABILITY.items():
        encounter = by_map.get(map_id)
        if encounter is None:
            problems.append(f"missing encounter map {map_id}")
            continue
        for method, slots in methods.items():
            if method not in encounter:
                problems.append(f"{map_id} has no {method}")
                continue
            mons = encounter[method]["mons"]
            for index, species in slots.items():
                if index >= len(mons):
                    problems.append(f"{map_id} {method} has no slot {index}")
                    continue
                if mons[index]["species"] != species:
                    problems.append(
                        f"{map_id} {method}[{index}] is {mons[index]['species']}, "
                        f"expected {species}"
                    )
                if species not in constants:
                    problems.append(f"undefined wild species {species}")
                rate = rates[method][index]
                if rate < 10:
                    problems.append(
                        f"{map_id} {method}[{index}] {species} is grindy at {rate}%"
                    )

    for map_id, methods in RETIRED_WILD_SOURCES.items():
        for method, slots in methods.items():
            mons = by_map.get(map_id, {}).get(method, {}).get("mons", [])
            for index, species in slots.items():
                if index >= len(mons):
                    problems.append(f"{map_id} {method} has no retired-source slot {index}")
                elif mons[index]["species"] != species:
                    problems.append(
                        f"{map_id} {method}[{index}] retained former curated source "
                        f"{mons[index]['species']}, expected {species}"
                    )

    for species in ("SPECIES_PAWNIARD", "SPECIES_PRIMEAPE", "SPECIES_GIRAFARIG"):
        if species not in all_wild_species:
            problems.append(f"existing ancestor source disappeared for {species}")

    scripts = ITEM_BALL_SCRIPTS.read_text()
    for label, item in WORLD_ITEM_AVAILABILITY.items():
        match = re.search(
            rf"^\s*{re.escape(label)}::[^\n]*\n\s*finditem\s+(ITEM_[A-Z0-9_]+)",
            scripts,
            re.M,
        )
        if not match:
            problems.append(f"missing world-item label {label}")
        elif match.group(1) != item:
            problems.append(f"{label} gives {match.group(1)}, expected {item}")
    return problems


def check(data: dict) -> None:
    entries = data["entries"]
    problems = asset_problems(ROOT, entries, imported=True) + availability_problems()
    constants = "\n".join(
        (ROOT / path).read_text()
        for path in (
            "include/constants/species.h",
            "include/constants/verdant_gen9_species.h",
        )
    )
    registered = sum(entry["species"] in constants for entry in entries)
    if registered != len(entries):
        problems.append(f"only {registered}/{len(entries)} species constants are registered")

    base_stats = (ROOT / "src/data/pokemon/verdant_gen9_base_stats.h").read_text()
    for entry in entries:
        species = entry["species"]
        match = re.search(
            rf"^\s*\[{re.escape(species)}\]\s*=.*?^\s*\.catchRate\s*=\s*(\d+),",
            base_stats,
            re.M | re.S,
        )
        if not match:
            problems.append(f"{species} lacks generated base stats/catch rate")
        elif int(match.group(1)) < MIN_CURATED_CATCH_RATE:
            problems.append(f"{species} catch rate remains grindy at {match.group(1)}")

    dex_entries = (ROOT / "src/data/pokemon/verdant_gen9_pokedex_entries.h").read_text()
    dex_entry_count = len(set(re.findall(r"\[NATIONAL_DEX_[A-Z0-9_]+\]", dex_entries)))
    if dex_entry_count != 34:
        problems.append(f"curated Pokédex entry count is {dex_entry_count}, expected 34")
    dex_orders = (ROOT / "src/data/pokemon/verdant_gen9_pokedex_orders.h").read_text()
    for order in ("Alphabetical", "Weight", "Height"):
        try:
            body = dex_orders.split(
                f"const u16 gVerdantPokedexOrder_{order}[] =", 1
            )[1].split("};", 1)[0]
        except IndexError:
            problems.append(f"missing curated Pokédex {order} order")
            continue
        values = re.findall(r"NATIONAL_DEX_[A-Z0-9_]+", body)
        if len(values) != 932 or len(set(values)) != 932:
            problems.append(
                f"curated Pokédex {order} order has {len(values)} entries/"
                f"{len(set(values))} unique, expected 932"
            )

    cry_tables = (ROOT / "sound/cry_tables.inc").read_text()
    if "gCryTable::" not in cry_tables or "gCryTable2::" not in cry_tables:
        problems.append("native cry tables are missing")
    else:
        first, second = cry_tables.split("gCryTable::", 1)[1].split("gCryTable2::", 1)
        cry_pattern = re.compile(r"^\s*cry(?:2)?(?:_uncomp)?\s+", re.M)
        first_count = len(cry_pattern.findall(first))
        second_count = len(cry_pattern.findall(second))
        if (first_count, second_count) != (1270, 1270):
            problems.append(
                f"cry table coverage is {first_count}/{second_count}, expected 1270/1270"
            )
    source_cries = list(
        (ROOT / "sound/direct_sound_samples/cries").glob("uncomp_gen9_*.aif")
    )
    if len(source_cries) != 37:
        problems.append(f"curated source cry count is {len(source_cries)}, expected 37")

    elevation_source = (
        ROOT / "src/data/pokemon_graphics/verdant_gen9_enemy_mon_elevation.h"
    ).read_text()
    expected_elevations = {
        "SPECIES_DONDOZO": 1,
        "SPECIES_FLUTTER_MANE": 18,
        "SPECIES_GLIMMET": 11,
        "SPECIES_GLIMMORA": 8,
        "SPECIES_CHI_YU": 15,
        "SPECIES_GLIMMORA_MEGA": 7,
    }
    for species, elevation in expected_elevations.items():
        if f"[{species}] = {elevation}," not in elevation_source:
            problems.append(f"{species} lacks enemy elevation {elevation}")

    pokedex_source = (ROOT / "src/pokedex.c").read_text()
    summary_source = (ROOT / "src/pokemon_summary_screen.c").read_text()
    hall_source = (ROOT / "src/hall_of_fame.c").read_text()
    if pokedex_source.count("GetPokedexNumberDigitCount(") < 4:
        problems.append("Pokédex screens do not consistently render four-digit numbers")
    if "PrintInfoScreenTextWhite(str, 117, 17)" not in pokedex_source:
        problems.append("four-digit Pokédex info number is not separated from the name")
    if "dexNum >= 1000 ? 4 : 3" not in summary_source:
        problems.append("summary screen lacks four-digit National-Dex numbers")
    if "if (dexNumber >= 1000)" not in hall_source:
        problems.append("Hall of Fame lacks four-digit National-Dex numbers")
    pokedex_text = (ROOT / "src/data/pokemon/verdant_gen9_pokedex_text.h").read_text()
    for stale_text in ("Tera Jewels", "paranomal"):
        if stale_text in pokedex_text:
            problems.append(f"stale curated Pokédex prose remains: {stale_text}")

    items_source = (ROOT / "src/data/items.h").read_text()
    if '.name = _("Leader\'s Crest")' not in items_source:
        problems.append("Leader's Crest does not use its official fitting name")

    party_menu = (ROOT / "src/party_menu.c").read_text()
    for item in ("ITEM_GIMMIGHOUL_COIN", "ITEM_LEADERS_CREST", "ITEM_METAL_ALLOY"):
        if f"gSpecialVar_ItemId != {item}" not in party_menu:
            problems.append(f"{item} is not protected as a reusable evolution unlock")
    pokemon_source = (ROOT / "src/pokemon.c").read_text()
    storage_source = (ROOT / "src/pokemon_storage_system.c").read_text()
    if "bool32 TryUpdateMonFormForHeldItem" not in pokemon_source:
        problems.append("party held-item forms lack a central reconciliation path")
    if "bool32 TryUpdateBoxMonFormForHeldItem" not in pokemon_source:
        problems.append("boxed held-item forms lack a central reconciliation path")
    if party_menu.count("UpdateMonFormForHeldItem(") < 9:
        problems.append("party held-item mutation paths do not all reconcile forms")
    if storage_source.count("UpdateBoxMonFormForHeldItem(") < 5:
        problems.append("PC box held-item mutation paths do not all reconcile forms")
    if storage_source.count("UpdatePartyMonFormForHeldItem(") < 5:
        problems.append("PC party held-item mutation paths do not all reconcile forms")
    for path in (
        "src/battle_pyramid.c",
        "src/frontier_util.c",
        "src/battle_dome.c",
        "src/battle_tower.c",
        "src/battle_util.c",
        "src/battle_main.c",
    ):
        if "TryUpdateMonFormForHeldItem" not in (ROOT / path).read_text():
            problems.append(f"{path} can desynchronize held-item forms")
    item_source = (ROOT / "src/item.c").read_text()
    for item in (
        "ITEM_BOOSTER_ENERGY",
        "ITEM_WELLSPRING_MASK",
        "ITEM_HEARTHFLAME_MASK",
        "ITEM_CORNERSTONE_MASK",
    ):
        if f"{{{item}," not in item_source:
            problems.append(f"{item} does not unlock in the unlimited battle-item shop")

    save_source = (ROOT / "src/save.c").read_text()
    for gate in (
        "sizeof(struct SaveBlock1) == 0x380C",
        "sizeof(struct SaveBlock2) == 0x0F2C",
        "sizeof(struct Pokedex) == 0x78",
    ):
        if gate not in save_source:
            problems.append(f"missing save-layout compile gate: {gate}")
    if save_source.count("FLAG_ITEM_") < len(WORLD_ITEM_AVAILABILITY):
        problems.append("legacy item-ball save migration does not cover all world rewards")
    if "MigrateVerdantGen9WorldItems();" not in (ROOT / "src/overworld.c").read_text():
        problems.append("world-item migration is not scheduled after heap initialization")

    for path in (
        "src/decompress.c",
        "src/pokemon_icon.c",
        "src/pokemon.c",
        "src/battle_anim_mons.c",
        "src/battle_main.c",
    ):
        if "species > NUM_SPECIES" in (ROOT / path).read_text():
            problems.append(f"{path} accepts the out-of-bounds NUM_SPECIES sentinel")
    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))
    print(
        f"PASS: {len(entries)} curated entries have complete required assets; "
        f"{registered}/{len(entries)} species constants registered; "
        f"{sum(len(slots) for methods in WILD_AVAILABILITY.values() for slots in methods.values())} "
        f"non-grindy wild sources and {len(WORLD_ITEM_AVAILABILITY)} world items verified"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--import-assets", type=Path, metavar="EXPANSION_ROOT")
    parser.add_argument("--generate-graphics", type=Path, metavar="EXPANSION_ROOT")
    parser.add_argument("--generate-species", type=Path, metavar="EXPANSION_ROOT")
    parser.add_argument("--generate-pokedex", type=Path, metavar="EXPANSION_ROOT")
    parser.add_argument("--generate-learnsets", type=Path, metavar="EXPANSION_ROOT")
    parser.add_argument("--apply-availability", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.import_assets and not args.generate_graphics and not args.generate_species and not args.generate_pokedex and not args.generate_learnsets and not args.apply_availability and not args.check:
        parser.error("choose an import, generate, availability, or check action")
    data = load_manifest()
    if args.import_assets:
        import_assets(args.import_assets.resolve(), data["entries"])
    if args.generate_graphics:
        generate_graphics(args.generate_graphics.resolve(), data["entries"])
    if args.generate_species:
        generate_species(args.generate_species.resolve(), data)
    if args.generate_pokedex:
        generate_pokedex(args.generate_pokedex.resolve(), data)
    if args.generate_learnsets:
        generate_learnsets(args.generate_learnsets.resolve(), data)
    if args.apply_availability:
        apply_availability()
    if args.check:
        check(data)


if __name__ == "__main__":
    main()
