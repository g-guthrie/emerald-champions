#!/usr/bin/env python3
"""Builds the title screen's game logo from its full-size artwork.

    python3 tools/title_logo.py graphics/title_screen/inclement_emerald_2_logo_master.png \
        graphics/title_screen/inclement_emerald_2_logo.png

The master is pixel art drawn at about 9 source pixels per art pixel. It is
cropped to its opaque pixels and scaled to LOGO_WIDTH x LOGO_HEIGHT by
nearest-neighbour sampling, which lands near one art pixel per screen pixel
and never blends two colours into a new one, so the outlines stay hard. The
result goes into a 256x64 8bpp sheet (four 64x64 sprites, src/title_screen.c)
with at most PALETTE_COLORS colours after the transparent index 0: OBJ
palettes 0-8, the ones the title screen reserves below PRESS START's.
"""
import sys
from PIL import Image

SHEET_WIDTH, SHEET_HEIGHT = 256, 64
LOGO_WIDTH, LOGO_HEIGHT = 224, 64
PALETTE_COLORS = 9 * 16 - 1


def build(src, dst):
    art = Image.open(src).convert('RGBA')
    art = art.crop(art.getchannel('A').point(lambda a: 255 if a >= 128 else 0).getbbox())
    scale = max(art.width / LOGO_WIDTH, art.height / LOGO_HEIGHT)
    size = (round(art.width / scale), round(art.height / scale))
    logo = art.resize(size, Image.NEAREST)

    opaque = logo.getchannel('A').point(lambda a: 255 if a >= 128 else 0)
    colors = logo.convert('RGB').quantize(PALETTE_COLORS, method=Image.Quantize.MEDIANCUT, kmeans=4, dither=Image.Dither.NONE)
    palette = colors.getpalette()[:PALETTE_COLORS * 3]

    sheet = Image.new('P', (SHEET_WIDTH, SHEET_HEIGHT), 0)
    indices = colors.point(lambda i: i + 1)
    sheet.paste(indices, ((LOGO_WIDTH - size[0]) // 2, (SHEET_HEIGHT - size[1]) // 2), opaque)
    # GBA colours have 5 bits a channel.
    gba = [0, 0, 0] + [v & 0xF8 for v in palette]
    sheet.putpalette(gba + [0] * (768 - len(gba)))
    sheet.save(dst, transparency=0)
    used = sum(1 for count in sheet.histogram()[1:] if count)
    print(f'{dst}: {size[0]}x{size[1]} logo, {used} colours')


if __name__ == '__main__':
    build(sys.argv[1], sys.argv[2])
