"""Photograph the DexNav roster where a save stands: every icon's info panel, one sheet.

usage (repo root): .venv-studio/bin/python artifacts/playthrough/dexnav.py SAVE NAME
Each scene boots fresh from the save, so the Start menu cursor starts on Pokedex and DexNav is
one step down. The cursor visits every icon (Right wraps a row, Down stops at the last row);
identical frames are dropped. Writes work/studio/dexnav/NAME/sheet.png and frames.json, then
deletes the scene's frames.
"""
import hashlib, json, subprocess, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
sys.path.insert(0, str(HERE))
import explore as X

ROWS, COLS = 10, 5


def main():
    save, name = sys.argv[1], sys.argv[2]
    steps = [{'press': 'START', 'frames': 40}, {'press': 'DOWN', 'frames': 20},
             {'press': 'A', 'frames': 160, 'label': 'dex r0 c0'}]
    for r in range(ROWS):
        for c in range(1, COLS):
            steps.append({'press': 'RIGHT', 'frames': 24, 'label': f'dex r{r} c{c}'})
        if r < ROWS - 1:
            steps.append({'press': 'DOWN', 'frames': 24, 'label': f'dex r{r + 1} c0'})
    steps += [{'press': 'B', 'frames': 90}, {'tap': 'B', 'every': 30, 'frames': 300, 'min_frames': 30, 'until': 'idle'}]
    dest, res = X.run_chunk({'name': f'dexnav {name}', 'start': {'save': save}, 'steps': steps,
                             'expect': {'ready': True}}, f'dexnav-{name}')
    rec = json.loads((dest / 'recording.json').read_text())
    from PIL import Image
    seen, keep = set(), []
    for c in rec['captures']:
        if not c.get('label', '').startswith('dex '): continue
        p = dest / c['path']
        h = hashlib.sha1(Image.open(p).convert('RGB').tobytes()).hexdigest()
        if h in seen: continue
        seen.add(h); keep.append((c['label'], p))
    out = ROOT / 'work/studio/dexnav' / name
    out.mkdir(parents=True, exist_ok=True)
    w, h = 240, 160; per = 4
    rows = (len(keep) + per - 1) // per
    sheet = Image.new('RGB', (w * per, h * max(rows, 1)), (255, 255, 255))
    for i, (_, p) in enumerate(keep):
        sheet.paste(Image.open(p).convert('RGB').resize((w, h)), ((i % per) * w, (i // per) * h))
    sheet.save(out / 'sheet.png')
    fin = res['outcome']['final']
    (out / 'frames.json').write_text(json.dumps(dict(map=fin['map'], x=fin['x'], y=fin['y'], unique=len(keep),
                                                     labels=[l for l, _ in keep], end_save=fin.get('end_save')), indent=1))
    for f in dest.iterdir():
        if f.name not in ('end.sav', 'result.json'):
            subprocess.run(['rm', '-rf', str(f)])
    print(out / 'sheet.png', len(keep))


if __name__ == '__main__':
    main()
