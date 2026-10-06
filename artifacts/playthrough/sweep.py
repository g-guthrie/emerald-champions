"""Sweep a list of maps in order: travel into each (adjacent hop) and explore it fully.

usage (repo root): .venv-studio/bin/python artifacts/playthrough/sweep.py SAVE MAP1 MAP2 ...
  A map written as MAP@x,y travels through that warp/edge tile. A map written as MAP! is travelled
  through without exploring (already swept). Stops at the first failure.
Appends one line per map to artifacts/playthrough/SWEEP.md and prints the last good end save.
"""
import json, subprocess, sys, time
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import explore as X
ROOT = HERE.parent.parent
PY = str(ROOT / '.venv-studio/bin/python')


def _wild_maps():
    import re
    d = json.loads((ROOT / 'src/data/wild_encounters.json').read_text())
    camel = {}
    for f in (ROOT / 'data/maps').glob('*/map.json'):
        m = json.loads(f.read_text()); camel[m.get('id')] = f.parent.name
    return {camel.get(e.get('map')) for g in d['wild_encounter_groups'] for e in g['encounters'] if e.get('map')}


WILD_MAPS = _wild_maps()


def log(line):
    with open(HERE / 'SWEEP.md', 'a') as f: f.write(line + '\n')


def main():
    save = sys.argv[1]
    caps = ''
    for spec in sys.argv[2:]:
        if spec.startswith('CAPS='):   # field moves the explorer may plan with from here on
            caps = spec[5:]; continue
        if spec == 'SPRAY':
            # Use the Repel Spray from the Bag (Key Items, 4th) and confirm the game's flag.
            # Bag cursor memory: valid only for the first use; the spray then renews itself
            # (wear-off Yes/No defaults to Yes and scenes are settled with A).
            r = json.loads((HERE / 'recipes/use-repel-spray.json').read_text()); r['start'] = {'save': save}
            dest, res = X.run_chunk(r, f'spray-{int(time.time()) % 100000}')
            fin = res['outcome']['final']
            if fin['queries'].get('spray_on') != 1 or not fin.get('end_save'):
                log(f"| SPRAY | FAILED | {fin['queries']} |"); print('STOP spray'); return
            save = fin['end_save']; log(f"| SPRAY | on, {fin['queries']['spray_steps']} steps | {fin['map']} | {save} |"); continue
        skip = spec.endswith('!'); spec = spec.rstrip('!')
        target, _, via = spec.partition('@')
        tag = f"{target.lower()}-{int(time.time()) % 100000}"
        cmd = [PY, str(HERE / 'travel.py'), save, target, 'go-' + tag] + (['--via', via] if via else []) + (['--caps', caps] if caps else [])
        r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
        try: t = json.loads(r.stdout.strip().splitlines()[-1])
        except Exception: log(f'| {target} | TRAVEL CRASH | {r.stderr[-300:]!r} |'); print('STOP', target); return
        story_moved = (not t['passed'] and t.get('end_save') and t['map'] != target
                       and all(f.startswith('map: expected') for f in t['failures']))
        if story_moved:   # a story scene took the player elsewhere (Devon Corp sends you up to 3F)
            log(f"| {target} | story moved the player to {t['map']} {t['x']},{t['y']} | {t['end_save']} |")
            target = t['map']; t['passed'] = True
        if not t['passed'] or not t.get('end_save'):
            log(f"| {target} | TRAVEL FAILED | {t['failures']} at {t['map']} {t['x']},{t['y']} |"); print('STOP', target, t); return
        save = t['end_save']
        if skip:
            log(f"| {target} | passed through | arrived {t['x']},{t['y']} | {save} |"); continue
        r = subprocess.run([PY, str(HERE / 'explore.py'), target, save, 'x-' + tag] + (['--caps', caps] if caps else []),
                           cwd=ROOT, capture_output=True, text=True)
        out = ROOT / 'work/studio/explore' / ('x-' + tag)
        if not (out / 'report.json').exists():
            log(f"| {target} | EXPLORE FAILED | {(r.stdout + r.stderr)[-300:]!r} |"); print('STOP', target); return
        rep = json.loads((out / 'report.json').read_text())
        visited = sum(1 for i in rep['interactions'] if i['status'] == 'visited')
        other = [f"{i['label']}:{i['status']}" for i in rep['interactions'] if i['status'] != 'visited']
        other += [f"{i['label']}:needs {i['needs']}" for i in rep.get('later', [])]
        log(f"| {target} | explored | photos {len(rep['photos'])}, interactions {visited} visited {other or ''}, "
            f"bag {rep['bag_gained'] or '-'}, failures {rep['failures'] or '-'} | {out}/report.md |")
        save = rep['end_save']
        if target in WILD_MAPS:   # photograph the DexNav roster where the sweep ended
            subprocess.run([PY, str(HERE / 'dexnav.py'), save, target.lower()], cwd=ROOT, capture_output=True, text=True)
        if rep['failures']: print('STOP after failures', target, rep['failures']); print(save); return
    print(save)


if __name__ == '__main__':
    main()
