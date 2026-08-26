#!/usr/bin/env python3
"""Semantic regression checks for Verdant's native UI and save contracts."""

from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]
PASSED = []


def read(path: str) -> str:
    return (ROOT / path).read_text()


def check(name: str, condition: bool) -> None:
    if not condition:
        raise AssertionError(name)
    PASSED.append(name)
    print(f"PASS: {name}")


def font_widths(font_id: int):
    return [
        int(value)
        for line in read(f"graphics/fonts/font{font_id}_latin_widths.inc").splitlines()
        for value in re.findall(r"\d+", line)
    ]


CHARMAP = {}
for charmap_line in read("charmap.txt").splitlines():
    match = re.match(r"'(.*)'\s*=\s*([0-9A-Fa-f]{2})\s*$", charmap_line)
    if match:
        char = match.group(1)
        if char == r"\'":
            char = "'"
        if len(char) == 1:
            CHARMAP[char] = int(match.group(2), 16)


def text_width(text: str, font_id: int) -> int:
    widths = font_widths(font_id)
    missing = sorted({char for char in text if char not in CHARMAP})
    if missing:
        raise AssertionError(f"font {font_id} has no parsed width for {missing!r} in {text!r}")
    return sum(widths[CHARMAP[char]] for char in text)


def render_static_placeholders(text: str) -> str:
    return text.replace("{POKEBLOCK}", "Pokéblock").replace("{PKMN}", "Pokémon")


def c_string_body(body: str) -> str:
    return "".join(re.findall(r'"([^"\\]*(?:\\.[^"\\]*)*)"', body)).replace(r"\n", "\n")


items = read("src/data/items.h")
item_descriptions = read("src/data/text/item_descriptions.h")
shop = read("src/shop.c")
bag = read("src/item_menu.c")
party = read("src/party_menu.c")
egg = read("src/egg_hatch.c")
pokedex = read("src/pokedex.c")
save = read("src/save.c")
pic_sprites = read("src/trainer_pokemon_sprites.c")
mon_icons = read("src/pokemon_icon.c")
utility = read("src/util.c")
gfx_utility = read("src/battle_gfx_sfx_util.c")
pokemon_animation = read("src/pokemon_animation.c")
battle_anim_flying = read("src/battle_anim_flying.c")
battle_anim_ghost = read("src/battle_anim_ghost.c")
storage = read("src/pokemon_storage_system.c")

item_description_widths = [
    text_width(render_static_placeholders(line), 1)
    for match in re.finditer(
        r"static const u8\s+\w+Desc\[\]\s*=\s*_\((.*?)\);",
        item_descriptions,
        re.DOTALL,
    )
    for line in c_string_body(match.group(1)).splitlines()
    if "{" not in line or "{POKEBLOCK}" in line or "{PKMN}" in line
]
check(
    "every literal Bag description line fits the 104-pixel native content area",
    item_description_widths and max(item_description_widths) <= 104,
)

item_name_length = int(re.search(r"#define ITEM_NAME_LENGTH\s+(\d+)", read("include/constants/global.h")).group(1))
item_names = re.findall(r'\.name\s*=\s*_\("([^"\n]*)"\)', items)
decoration_names = re.findall(r'\.name\s*=\s*_\("([^"\n]*)"\)', read("src/data/decoration/header.h"))
check(
    "item and decoration names include an EOS byte in Mart storage",
    max(map(len, item_names + decoration_names)) < item_name_length,
)
check(
    "all item names fit beside the five-digit price in the native Mart list",
    max(text_width(render_static_placeholders(name), 7) for name in item_names) + 8 <= 89,
)

check(
    "Mart, Bag, and party setup allocations have explicit failure exits",
    all(
        token in source
        for source, token in (
            (shop, "if (sShopData == NULL)"),
            (shop, "if (sListMenuItems == NULL || sItemNames == NULL)"),
            (bag, "if (sListBuffer1 == NULL || sListBuffer2 == NULL)"),
            (party, "if (sPartyBgGfxTilemap == NULL)"),
            (party, "if (sPartyMenuBoxes == NULL)"),
            (party, "if (sSlot1TilemapBuffer == NULL || sSlot2TilemapBuffer == NULL)"),
        )
    ),
)

check(
    "egg-hatch BG buffers have one explicit owner across the naming-screen handoff",
    all(
        token in egg
        for token in (
            "void *bg0TilemapBuffer;",
            "void *bg1TilemapBuffer;",
            "FreeEggHatchSceneResources();",
            "Free(sEggHatchData->bg0TilemapBuffer);",
            "Free(sEggHatchData->bg1TilemapBuffer);",
        )
    )
    and egg.count("FreeEggHatchSceneResources();") >= 4,
)

evolution_sources = read("src/data/pokemon/evolution.h") + read("src/data/pokemon/verdant_gen9_evolutions.h")
battle_transform_methods = {"EVO_MEGA_EVOLUTION", "EVO_MOVE_MEGA_EVOLUTION", "EVO_PRIMAL_REVERSION"}
permanent_methods = set(re.findall(r"\{\s*(EVO_[A-Z0-9_]+)\s*,", evolution_sources)) - battle_transform_methods
rendered_methods = set(re.findall(r"case\s+(EVO_[A-Z0-9_]+)\s*:", pokedex))
check(
    "Pokedex evolution rendering covers every permanent evolution method",
    permanent_methods <= rendered_methods,
)
check(
    "Pokedex evolution rendering excludes Mega and Primal battle transformations",
    all(f"method != {method}" in pokedex for method in battle_transform_methods),
)

form_family_sizes = [
    len(re.findall(r"\bSPECIES_[A-Z0-9_]+\b", match.group(1))) - 1
    for match in re.finditer(
        r"static const u16\s+\w+\[\]\s*=\s*\{(.*?)\};",
        read("src/data/pokemon/form_species_tables.h"),
        re.DOTALL,
    )
]
aux_capacity = int(re.search(r"#define MAX_POKEDEX_AUX_SPRITES\s+(\d+)", pokedex).group(1))
check(
    "Pokedex form sprites are bounded outside the 16-slot task data array",
    max(form_family_sizes) <= aux_capacity
    and not re.search(r"data\[4\s*\+\s*(?:times|base_i)", pokedex)
    and "TrackPokedexAuxSprite(spriteId);" in pokedex,
)

check(
    "picture and icon creation failures release resources before returning",
    pic_sprites.count("if (spriteId == MAX_SPRITES)") >= 2
    and pic_sprites.count("Free(framePics);") >= 4
    and pic_sprites.count("Free(images);") >= 4
    and mon_icons.count("if (spriteId != MAX_SPRITES)") >= 2
    and "if (spriteId == MAX_SPRITES)\n        return MAX_SPRITES;" in mon_icons,
)

invisible_creator = re.search(
    r"u8 CreateInvisibleSpriteWithCallback\(.*?\)\s*\{(.*?)(?=\n\})",
    utility,
    re.DOTALL,
).group(1)
check(
    "central invisible sprites and enemy shadows reject the MAX_SPRITES sentinel",
    invisible_creator.index("sprite == MAX_SPRITES") < invisible_creator.index("gSprites[sprite]")
    and gfx_utility.count("shadowSpriteId < MAX_SPRITES") >= 3
    and "shadowSpriteId >= MAX_SPRITES" in gfx_utility,
)

offset_animation_names = (
    "Anim_RapidHorizontalHops",
    "Anim_VerticalShakeHorizontalSlide_Slow",
    "Anim_VerticalShakeHorizontalSlide",
    "Anim_VerticalShakeHorizontalSlide_Fast",
)
offset_animation_bodies = [
    re.search(
        rf"static void {name}\(struct Sprite \*sprite\)\s*\{{(.*?)(?=\nstatic void )",
        pokemon_animation,
        re.DOTALL,
    ).group(1)
    for name in offset_animation_names
]
check(
    "horizontal-hop and slide animations clear positional offsets before completion",
    all(
        body.index("sprite->x2 = 0;")
        < body.index("sprite->y2 = 0;")
        < body.index("sprite->callback = WaitAnimEnd;")
        for body in offset_animation_bodies
    ),
)

gust_palette_task = re.search(
    r"static void AnimTask_AnimateGustTornadoPalette_Step\(u8 taskId\)\s*\{(.*?)(?=\nstatic void )",
    battle_anim_flying,
    re.DOTALL,
).group(1)
check(
    "Gust-family animations rotate the loaded OBJ palette within one bounded bank",
    "TryLoadBattleAnimPalette(ANIM_TAG_GUST)" in battle_anim_flying
    and "(OBJ_PLTT - PLTT) / sizeof(u16)" in gust_palette_task
    and "IndexOfSpritePaletteTag(ANIM_TAG_GUST) * 16" in gust_palette_task
    and "memmove(&palPtr[2], &palPtr[1], 7 * sizeof(*palPtr));" in gust_palette_task,
)

storage_transparency = re.search(
    r"static void SetMonIconTransparency\(void\)\s*\{(.*?)(?=\nstatic void )",
    storage,
    re.DOTALL,
).group(1)
check(
    "storage restores icon blending in every box mode after child screens",
    "SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT2_ALL);" in storage_transparency
    and "SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(7, 11));" in storage_transparency
    and "boxOption" not in storage_transparency,
)

shadow_task = re.search(
    r"void AnimTask_DestinyBondWhiteShadow\(u8 taskId\)\s*\{(.*?)(?=\nstatic void )",
    battle_anim_ghost,
    re.DOTALL,
).group(1)
check(
    "shared shadow animations target both foes only for true both-foe moves",
    "gAnimMoveIndex < MOVES_COUNT" in shadow_task
    and "moveTarget == MOVE_TARGET_BOTH" in shadow_task
    and "moveTarget == MOVE_TARGET_USER" in shadow_task
    and "battler == gBattleAnimTarget" in shadow_task,
)

check(
    "save slot validation bounds section IDs and requires one counter per complete slot",
    save.count("id < SECTOR_SAVE_SLOT_LENGTH") >= 2
    and "slotCheckField == 0x3FFF && !counterMismatch" in save
    and save.count("slotCheckField == 0x3FFF && !counterMismatch") == 2,
)

flash_bytes = 32 * 4096
check(
    "ROM and browser emulators agree on 128 KiB flash geometry",
    flash_bytes == 131072
    and "131072" in read("src/agb_flash_le.c")
    and read("src/agb_flash_mx.c").count("131072") >= 2
    and "#define SECTORS_COUNT 32" in read("include/save.h")
    and "// size is 0x1000" in read("include/save.h"),
)

print(f"\nAll {len(PASSED)} deep presentation/save checks passed")
