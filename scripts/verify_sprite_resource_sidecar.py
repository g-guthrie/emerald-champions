#!/usr/bin/env python3
"""Static and executable-model gates for deferred sprite resource ownership."""

from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def section(text: str, start: str, end: str) -> str:
    begin = text.index(start)
    finish = text.index(end, begin + len(start))
    return text[begin:finish]


sprite_c = read("gflib/sprite.c")
sprite_h = read("gflib/sprite.h")
event = read("src/event_object_movement.c")
anim_throw = read("src/battle_anim_throw.c")
anim_mons = read("src/battle_anim_mons.c")

sprite_struct = section(sprite_h, "struct Sprite\n{", "};\n\nstruct OamMatrix")
create = section(sprite_c, "u8 CreateSpriteAt(u8 index", "u8 CreateSpriteAndAnimate")
free_tiles = section(sprite_c, "void FreeSpriteTiles(struct Sprite *sprite)\n{", "void FreeSpritePalette")
free_palette = section(sprite_c, "void FreeSpritePalette(struct Sprite *sprite)\n{", "void FreeSpriteOamMatrix")
copy_out = section(sprite_c, "void CopyFromSprites", "void CopyToSprites")
copy_in = section(sprite_c, "void CopyToSprites", "void CopySpriteResourceTags")
copy_tags = section(sprite_c, "void CopySpriteResourceTags", "void ResetAllSprites")
load_sheet = section(sprite_c, "u16 LoadSpriteSheet", "void LoadSpriteSheets")
alloc_range = section(sprite_c, "static bool8 AllocSpriteTileRange", "void FreeAllSpritePalettes")

checks = {
    "Sprite ABI is unchanged by resource ownership metadata": (
        "SpriteResourceTags" not in sprite_struct
        and "resourceTileTag" not in sprite_struct
        and "resourcePaletteTag" not in sprite_struct
    ),
    "EWRAM sidecars include every sprite and the sentinel": (
        sprite_c.count("SpriteResourceTags sSpriteResourceTags[MAX_SPRITES + 1]") == 1
        and sprite_c.count("SpriteResourceTags sSavedSpriteResourceTags[MAX_SPRITES + 1]") == 1
    ),
    "CreateSpriteAt captures tags while the template is live": (
        "sSpriteResourceTags[index].tileTag = template->tileTag;" in create
        and "sSpriteResourceTags[index].paletteTag = template->paletteTag;" in create
        and create.index("sSpriteResourceTags[index].tileTag") < create.index("sprite->template = template")
    ),
    "Deferred frees never dereference Sprite.template": (
        "template" not in free_tiles
        and "template" not in free_palette
        and "sprite->template->tileTag" not in sprite_c
        and "sprite->template->paletteTag" not in sprite_c
    ),
    "Free operations consume sidecar tags before global release": (
        "sSpriteResourceTags[index].tileTag = SPRITE_INVALID_TAG" in free_tiles
        and "FreeSpriteTilesByTag(tag)" in free_tiles
        and "sSpriteResourceTags[index].paletteTag = SPRITE_INVALID_TAG" in free_palette
        and "FreeSpritePaletteByTag(tag)" in free_palette
    ),
    "Global tag releases invalidate current and saved aliases": all(
        token in sprite_c
        for token in (
            "ConsumeSpriteTileTag(tag);",
            "ConsumeSpritePaletteTag(tag);",
            "sSavedSpriteResourceTags[i].tileTag = SPRITE_INVALID_TAG",
            "sSavedSpriteResourceTags[i].paletteTag = SPRITE_INVALID_TAG",
        )
    ),
    "Reset paths invalidate sidecars including MAX_SPRITES": (
        "for (i = 0; i <= MAX_SPRITES; i++)" in section(sprite_c, "static void ResetAllSpriteResourceTags(void)\n{", "static void ConsumeSpriteTileTag(u16 tag)\n{")
        and "ResetSpriteResourceTags(sprite);" in section(sprite_c, "void ResetSprite(struct Sprite *sprite)\n{", "void CalcCenterToCornerVec")
        and "ResetAllSpriteResourceTags();" in section(sprite_c, "void ResetAllSprites(void)\n{", "void FreeSpriteTiles")
    ),
    "Pointer lookup safely maps out-of-array pointers to the sentinel": all(
        token in section(sprite_c, "static u8 GetSpriteIndex(const struct Sprite *sprite)\n{", "static void ResetSpriteResourceTags(struct Sprite *sprite)\n{")
        for token in ("&gSprites[MAX_SPRITES]", "address < base", "address > end", "return MAX_SPRITES")
    ),
    "Sprite save/restore preserves sidecar state": (
        "sSavedSpriteResourceTags[i] = sSpriteResourceTags[i]" in copy_out
        and "sSpriteResourceTags[i] = sSavedSpriteResourceTags[i]" in copy_in
    ),
    "Raw clones copy sidecars and surrender dynamic tile ownership": (
        "sSpriteResourceTags[destIndex] = sSpriteResourceTags[srcIndex]" in copy_tags
        and "if (!src->usingSheet)" in copy_tags
        and "dest->usingSheet = TRUE" in copy_tags
    ),
    "Every raw gSprites struct assignment updates the sidecar": (
        len(re.findall(r"gSprites\[[^\]]+\]\s*=\s*", event + anim_throw + anim_mons)) ==
        (event + anim_throw + anim_mons).count("CopySpriteResourceTags(")
    ),
    "Full tile-range metadata rolls back allocated OBJ tiles": (
        "if (!AllocSpriteTileRange" in load_sheet
        and "FREE_SPRITE_TILE(i)" in load_sheet
        and "if (freeIndex == 0xFF)" in alloc_range
        and "return FALSE" in alloc_range
    ),
    "Invalid tags are explicit no-resource paths": (
        sprite_c.count("if (tag == SPRITE_INVALID_TAG)\n        return;") >= 4
    ),
}


# Executable ownership model: tags are captured values, never deferred pointers.
INVALID = 0xFFFF
MAX = 64
tags = [[INVALID, INVALID] for _ in range(MAX + 1)]
saved = [[INVALID, INVALID] for _ in range(MAX + 1)]
loaded_tiles = {100}
loaded_pals = {200}


def create(index: int, tile: int, pal: int) -> None:
    tags[index] = [tile, pal]


def consume_tile(tag: int) -> None:
    loaded_tiles.discard(tag)
    for table in (tags, saved):
        for row in table:
            if row[0] == tag:
                row[0] = INVALID


def free_sprite(index: int) -> None:
    tile, pal = tags[index]
    tags[index] = [INVALID, INVALID]
    if tile != INVALID:
        consume_tile(tile)
    if pal != INVALID:
        loaded_pals.discard(pal)
        for table in (tags, saved):
            for row in table:
                if row[1] == pal:
                    row[1] = INVALID


create(0, 100, 200)
create(1, 100, 200)  # shared resource aliases
saved[:] = [row[:] for row in tags]
free_sprite(0)
checks["Model: freeing one shared tag consumes every stale alias"] = (
    tags[0] == [INVALID, INVALID]
    and tags[1] == [INVALID, INVALID]
    and saved[0] == [INVALID, INVALID]
    and saved[1] == [INVALID, INVALID]
)

# Reloading the same numeric tag must not let an old sprite free the new asset.
loaded_tiles.add(100)
loaded_pals.add(200)
free_sprite(1)
checks["Model: stale double-free cannot release a reloaded same-number tag"] = (
    100 in loaded_tiles and 200 in loaded_pals
)

# Sentinel and no-resource sprites are always safe no-ops.
free_sprite(MAX)
checks["Model: MAX_SPRITES sentinel free is a no-op"] = (
    tags[MAX] == [INVALID, INVALID]
)

failed = [name for name, ok in checks.items() if not ok]
for name, ok in checks.items():
    print(f"{'PASS' if ok else 'FAIL'}: {name}")

if failed:
    raise SystemExit(f"{len(failed)} sprite sidecar checks failed")

print(f"PASS: {len(checks)} sprite sidecar checks")
