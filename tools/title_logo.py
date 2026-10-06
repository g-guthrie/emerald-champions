#!/usr/bin/env python3
"""Builds the title screen's game logo from its full-size artwork.

    python3 tools/title_logo.py graphics/title_screen/inclement_emerald_2_logo_master.png \
        graphics/title_screen/inclement_emerald_2_logo.png

The master is the logo itself, 145x42, hand-finished: INCLEMENT is
redrawn in a bold 8-pixel face on one line, its gold bow redrawn with a low
arch over the top and its straight bottom sitting right on EMERALD's white
border (two black pixels round every letter), the 2's white top outline
restored, and the outlines cleaned. It is shown at that
size, centred in a 192x64 8bpp sheet (three 64x64 sprites,
src/title_screen.c), with at most PALETTE_COLORS colours after the
transparent index 0.

A master larger than the sheet is taken as pixel art enlarged by some factor
that need not be a whole number: the tool finds that pixel grid from where
the colour edges fall and takes one colour per grid cell, the median of the
cell's middle, so the logo comes back at its own size with no pixel blended,
doubled or dropped. Redraw by hand anything the enlarger smeared.
"""
import sys
import numpy as np
from PIL import Image

SHEET_WIDTH, SHEET_HEIGHT = 192, 64
PALETTE_COLORS = 64  # OBJ palettes 0-8 hold up to 143
MIN_WIDTH = 96  # narrower and the lettering cannot read


def grid_score(edges, pitch, offset):
    lines = np.round(offset + np.arange(int(len(edges) / pitch)) * pitch).astype(int)
    lines = lines[lines < len(edges)]
    return edges[lines].sum() / edges.sum() * pitch


def find_grid(edges, pitches):
    """The pitch and offset whose grid lines land on the most colour edges."""
    return max((grid_score(edges, p, o), p, o) for p in pitches for o in np.arange(0, p, 0.25))[1:]


def cells(length, pitch, offset):
    first = int(np.floor(-offset / pitch))
    starts = offset + np.arange(first, int(length / pitch) + 2) * pitch
    return [(s, s + pitch) for s in starts if s + pitch > 0 and s < length]


def shrink_to_grid(art):
    rgb = art[..., :3] * (art[..., 3:] / 255)
    x0, y0, x1, y1 = Image.fromarray(((art[..., 3] >= 128) * 255).astype(np.uint8)).getbbox()
    edges = [np.abs(np.diff(rgb, axis=axis)).sum(2).sum(1 - axis) for axis in (1, 0)]
    # Across, the pitches that make the logo MIN_WIDTH to SHEET_WIDTH pixels
    # wide: every second line of the true grid lines up as well, so a wider
    # search finds a pitch twice too big. Art pixels are square, so the pitch
    # down is within 5% of the pitch across.
    across = find_grid(edges[0], np.arange((x1 - x0) / SHEET_WIDTH, (x1 - x0) / MIN_WIDTH, 0.02))
    down = find_grid(edges[1], np.arange(across[0] * 0.95, across[0] * 1.05, 0.02))
    grid = [across, down]
    cols, rows = cells(art.shape[1], *grid[0]), cells(art.shape[0], *grid[1])

    logo = np.zeros((len(rows), len(cols), 4), np.uint8)
    for j, (y0, y1) in enumerate(rows):
        for i, (x0, x1) in enumerate(cols):
            # The middle of the cell: its edges can carry the enlarger's blur.
            ys = slice(max(0, int(y0 + 0.3 * (y1 - y0))), int(y1 - 0.3 * (y1 - y0)) + 1)
            xs = slice(max(0, int(x0 + 0.3 * (x1 - x0))), int(x1 - 0.3 * (x1 - x0)) + 1)
            block = art[ys, xs].reshape(-1, 4)
            if len(block) == 0 or (block[:, 3] >= 128).mean() < 0.5:
                continue
            logo[j, i, :3] = np.median(block[block[:, 3] >= 128][:, :3], axis=0)
            logo[j, i, 3] = 255
    return Image.fromarray(logo), grid


def build(src, dst):
    logo = Image.open(src).convert('RGBA')
    grid = None
    if logo.width > SHEET_WIDTH or logo.height > SHEET_HEIGHT:
        logo, grid = shrink_to_grid(np.array(logo).astype(int))
    logo = logo.crop(logo.getchannel('A').point(lambda a: 255 if a >= 128 else 0).getbbox())
    if logo.width > SHEET_WIDTH or logo.height > SHEET_HEIGHT:
        sys.exit(f'{src}: the logo is {logo.width}x{logo.height} pixels, more than {SHEET_WIDTH}x{SHEET_HEIGHT}')

    colors = logo.convert('RGB').quantize(PALETTE_COLORS, method=Image.Quantize.MEDIANCUT, kmeans=4, dither=Image.Dither.NONE)
    palette = colors.getpalette()[:PALETTE_COLORS * 3]
    sheet = Image.new('P', (SHEET_WIDTH, SHEET_HEIGHT), 0)
    sheet.paste(colors.point(lambda i: i + 1),
                ((SHEET_WIDTH - logo.width) // 2, (SHEET_HEIGHT - logo.height) // 2), logo.getchannel('A'))
    # GBA colours have 5 bits a channel.
    gba = [0, 0, 0] + [v & 0xF8 for v in palette]
    sheet.putpalette(gba + [0] * (768 - len(gba)))
    sheet.save(dst, transparency=0)
    used = sum(1 for count in sheet.histogram()[1:] if count)
    found = f'grid {grid[0][0]:.2f}x{grid[1][0]:.2f}, ' if grid else ''
    print(f'{dst}: {found}logo {logo.width}x{logo.height}, {used} colours')


if __name__ == '__main__':
    build(sys.argv[1], sys.argv[2])
