#!/usr/bin/env python3
"""Render a map's tiles into a plate for the guide, the way the original's town
maps look: the real overworld art, seen whole, with labels added on the page.

    .venv-studio/bin/python guide/mapplate.py PetalburgCity --out guide/assets/maps

A GBA map is a grid of metatiles. Each metatile is 16x16 pixels built from eight
8x8 tiles: four on a bottom layer and four on a top layer, each with a palette
index and optional horizontal or vertical flip. Tiles 0..511 come from the
layout's primary tileset and 512.. from its secondary. This walks that structure
and composites the result, so the plate is the same art the player sees rather
than a screenshot of part of it.
"""
import argparse
import json
import struct
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parent.parent
TILES_PER_TILESET = 512
METATILE_TILES = 8          # four bottom, four top
TILE = 8


def read_palettes(tileset_dir):
    """JASC-PAL files, sixteen palettes of sixteen colours."""
    palettes = []
    for path in sorted((tileset_dir / "palettes").glob("*.pal")):
        lines = path.read_text().splitlines()
        colours = [tuple(int(v) for v in line.split()) for line in lines[3:3 + 16]]
        palettes.append(colours)
    return palettes


def read_tiles(tileset_dir):
    """tiles.png is a 128px-wide sheet of 8x8 indexed tiles."""
    sheet = Image.open(tileset_dir / "tiles.png")
    if sheet.mode != "P":
        sheet = sheet.convert("P")
    raw = sheet.load()
    per_row = sheet.width // TILE
    count = (sheet.height // TILE) * per_row
    tiles = []
    for index in range(count):
        ox, oy = (index % per_row) * TILE, (index // per_row) * TILE
        tiles.append([[raw[ox + x, oy + y] for x in range(TILE)] for y in range(TILE)])
    return tiles


class Tileset:
    def __init__(self, name, secondary):
        self.dir = ROOT / "data/tilesets" / ("secondary" if secondary else "primary") / name
        self.tiles = read_tiles(self.dir)
        self.palettes = read_palettes(self.dir)
        data = (self.dir / "metatiles.bin").read_bytes()
        # Each metatile is eight u16: tile index, palette and flip bits.
        self.metatiles = [struct.unpack_from("<8H", data, i * 16)
                          for i in range(len(data) // 16)]


def tileset_name(symbol):
    """gTileset_PetalburgWoods -> petalburg_woods, matching the data folders."""
    bare = symbol.replace("gTileset_", "")
    out = []
    for i, ch in enumerate(bare):
        if ch.isupper() and i and not bare[i - 1].isupper():
            out.append("_")
        out.append(ch.lower())
    return "".join(out)


def draw_metatile(canvas, px, py, entry, primary, secondary):
    for layer in range(2):
        for slot in range(4):
            value = entry[layer * 4 + slot]
            tile_id = value & 0x03FF
            hflip = bool(value & 0x0400)
            vflip = bool(value & 0x0800)
            pal_id = (value >> 12) & 0x0F

            source = primary if tile_id < TILES_PER_TILESET else secondary
            local = tile_id if tile_id < TILES_PER_TILESET else tile_id - TILES_PER_TILESET
            if source is None or local >= len(source.tiles):
                continue
            pixels = source.tiles[local]
            palettes = primary.palettes if pal_id < len(primary.palettes) and pal_id < 6 else None
            # Palettes 0-5 belong to the primary tileset, 6-15 to the secondary.
            if pal_id < 6:
                palette = primary.palettes[pal_id] if pal_id < len(primary.palettes) else None
            else:
                idx = pal_id
                palette = (secondary.palettes[idx] if secondary and idx < len(secondary.palettes)
                           else (primary.palettes[idx] if idx < len(primary.palettes) else None))
            if palette is None:
                continue

            ox = px + (slot % 2) * TILE
            oy = py + (slot // 2) * TILE
            for y in range(TILE):
                sy = TILE - 1 - y if vflip else y
                for x in range(TILE):
                    sx = TILE - 1 - x if hflip else x
                    index = pixels[sy][sx]
                    if layer and index == 0:
                        continue       # colour 0 is transparent on the top layer
                    canvas.putpixel((ox + x, oy + y), palette[index])


def render(map_name, out_dir, scale):
    layouts = {x["id"]: x for x in
               json.loads((ROOT / "data/layouts/layouts.json").read_text())["layouts"]}
    m = json.loads((ROOT / "data/maps" / map_name / "map.json").read_text())
    layout = layouts[m["layout"]]

    primary = Tileset(tileset_name(layout["primary_tileset"]), secondary=False)
    secondary = (Tileset(tileset_name(layout["secondary_tileset"]), secondary=True)
                 if layout.get("secondary_tileset") else None)

    width, height = layout["width"], layout["height"]
    cells = (ROOT / layout["blockdata_filepath"]).read_bytes()
    canvas = Image.new("RGB", (width * 16, height * 16), (0, 0, 0))

    for y in range(height):
        for x in range(width):
            block = struct.unpack_from("<H", cells, (y * width + x) * 2)[0]
            metatile_id = block & 0x03FF
            source = primary if metatile_id < len(primary.metatiles) else secondary
            local = metatile_id if source is primary else metatile_id - len(primary.metatiles)
            if source is None or local >= len(source.metatiles):
                continue
            draw_metatile(canvas, x * 16, y * 16, source.metatiles[local], primary, secondary)

    if scale > 1:
        canvas = canvas.resize((canvas.width * scale, canvas.height * scale), Image.NEAREST)
    out_dir.mkdir(parents=True, exist_ok=True)
    path = out_dir / f"{map_name}.png"
    canvas.save(path)
    return path, width, height


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("maps", nargs="+", help="map directory names, e.g. PetalburgCity")
    ap.add_argument("--out", type=Path, default=ROOT / "guide/assets/maps")
    ap.add_argument("--scale", type=int, default=1)
    args = ap.parse_args()

    failed = 0
    for name in args.maps:
        try:
            path, w, h = render(name, args.out.resolve(), args.scale)
            print(f"  ok   {name:<34} {w}x{h} metatiles -> {path.name}")
        except Exception as exc:                       # noqa: BLE001 - reported, not raised
            failed += 1
            print(f"  FAIL {name:<34} {type(exc).__name__}: {exc}")
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
