#!/usr/bin/env python3
"""Compose labeled native screenshots without changing their captured pixels.

Requires Pillow (available in the bundled workspace Python). The JSON input owns
an ordered panels list with path/label entries, title, scope, and build identity.
Original files are retained; a sidecar records their hashes and sheet placement.
"""
import argparse
import hashlib
import json
import math
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def font(size):
    for name in ('/System/Library/Fonts/Supplemental/Arial.ttf',
                 '/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf'):
        path = Path(name)
        if path.exists():
            return ImageFont.truetype(str(path), size)
    try:
        return ImageFont.load_default(size=size)
    except TypeError:  # Distribution Pillow before 10.1.
        return ImageFont.load_default()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('manifest', type=Path)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--columns', type=int, choices=range(1, 7), default=3)
    parser.add_argument('--scale', type=int, choices=range(1, 5), default=2)
    args = parser.parse_args()
    spec = json.loads(args.manifest.read_text())
    frames = []
    for panel in spec['panels']:
        path = Path(panel['path'])
        if not path.is_absolute():
            path = args.manifest.parent / path
        path = path.resolve()
        if path == args.out.resolve():
            raise ValueError('Output must not overwrite an original.')
        with Image.open(path) as source:
            pixels = source.convert('RGB')
        frames.append((path, panel['label'], pixels, sha(path)))
    if not frames or len({frame[2].size for frame in frames}) != 1:
        raise ValueError('Use a nonempty group of equally sized native screenshots.')
    width, height = frames[0][2].size
    cell_w, image_h = width * args.scale, height * args.scale
    gap, header, caption = 14, 98, 58
    cols = min(args.columns, len(frames))
    rows = math.ceil(len(frames) / cols)
    canvas = Image.new('RGB', (gap + cols * (cell_w + gap), header + rows * (image_h + caption + gap)), '#edf1f5')
    draw = ImageDraw.Draw(canvas)
    draw.text((gap, 12), spec['title'], font=font(24), fill='#172334')
    draw.text((gap, 44), spec['scope'], font=font(17), fill='#34455c')
    build = spec['build']
    draw.text((gap, 70), f"ROM {build['rom_sha256'][:16]} | ELF {build['elf_sha256'][:16]} | Originals and evidence in sidecar", font=font(15), fill='#34455c')
    records = []
    for index, (path, label, pixels, digest) in enumerate(frames):
        x = gap + (index % cols) * (cell_w + gap)
        y = header + (index // cols) * (image_h + caption + gap)
        preview = pixels.resize((cell_w, image_h), Image.Resampling.NEAREST)
        canvas.paste(preview, (x, y))
        draw.rectangle((x, y + image_h, x + cell_w - 1, y + image_h + caption - 1), fill='white')
        lines = ['']
        for word in f'{index + 1}. {label}'.split():
            trial = (lines[-1] + ' ' + word).strip()
            if draw.textbbox((0, 0), trial, font=font(17))[2] > cell_w - 16:
                lines.append(word)
            else:
                lines[-1] = trial
        if len(lines) > 2:
            raise ValueError(f'Caption is too long: {label}')
        draw.multiline_text((x + 8, y + image_h + 7), '\n'.join(lines), font=font(17), fill='#172334', spacing=3)
        records.append({'path': str(path), 'label': label, 'sha256': digest, 'sheet_box': [x, y, cell_w, image_h]})
    args.out.parent.mkdir(parents=True, exist_ok=True)
    canvas.save(args.out)
    for path, _, _, digest in frames:
        if sha(path) != digest:
            raise RuntimeError(f'Original changed during composition: {path}')
    sidecar = args.out.with_suffix('.json')
    sidecar.write_text(json.dumps({'title': spec['title'], 'scope': spec['scope'], 'build': build,
        'composition': 'Integer nearest-neighbor scaling; labels outside captured pixels.',
        'sheet_sha256': sha(args.out), 'panels': records, 'evidence': spec.get('evidence', [])}, indent=2) + '\n')
    print(args.out)
    print(sidecar)


if __name__ == '__main__':
    main()
