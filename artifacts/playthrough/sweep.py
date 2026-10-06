"""Sweep a list of maps in order: travel into each (adjacent hop) and explore it fully.

usage (repo root): .venv-studio/bin/python artifacts/playthrough/sweep.py SAVE MAP1 MAP2 ...
  A map written as MAP@x,y travels through that warp/edge tile. A map written as MAP! is travelled
  through without exploring (already swept). Stops at the first failure.
Appends one line per map to artifacts/playthrough/SWEEP.md and prints the last good end save.
"""
import json, subprocess, sys, time
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
PY = str(ROOT / '.venv-studio/bin/python')


def log(line):
    with open(HERE / 'SWEEP.md', 'a') as f: f.write(line + '\n')


def main():
    save = sys.argv[1]
    for spec in sys.argv[2:]:
        skip = spec.endswith('!'); spec = spec.rstrip('!')
        target, _, via = spec.partition('@')
        tag = f"{target.lower()}-{int(time.time()) % 100000}"
        cmd = [PY, str(HERE / 'travel.py'), save, target, 'go-' + tag] + (['--via', via] if via else [])
        r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
        try: t = json.loads(r.stdout.strip().splitlines()[-1])
        except Exception: log(f'| {target} | TRAVEL CRASH | {r.stderr[-300:]!r} |'); print('STOP', target); return
        if not t['passed'] or not t.get('end_save'):
            log(f"| {target} | TRAVEL FAILED | {t['failures']} at {t['map']} {t['x']},{t['y']} |"); print('STOP', target, t); return
        save = t['end_save']
        if skip:
            log(f"| {target} | passed through | arrived {t['x']},{t['y']} | {save} |"); continue
        r = subprocess.run([PY, str(HERE / 'explore.py'), target, save, 'x-' + tag], cwd=ROOT, capture_output=True, text=True)
        out = ROOT / 'work/studio/explore' / ('x-' + tag)
        if not (out / 'report.json').exists():
            log(f"| {target} | EXPLORE FAILED | {(r.stdout + r.stderr)[-300:]!r} |"); print('STOP', target); return
        rep = json.loads((out / 'report.json').read_text())
        visited = sum(1 for i in rep['interactions'] if i['status'] == 'visited')
        other = [f"{i['label']}:{i['status']}" for i in rep['interactions'] if i['status'] != 'visited']
        log(f"| {target} | explored | photos {len(rep['photos'])}, interactions {visited} visited {other or ''}, "
            f"bag {rep['bag_gained'] or '-'}, failures {rep['failures'] or '-'} | {out}/report.md |")
        save = rep['end_save']
        if rep['failures']: print('STOP after failures', target, rep['failures']); print(save); return
    print(save)


if __name__ == '__main__':
    main()
