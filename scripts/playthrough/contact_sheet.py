#!/usr/bin/env python3
"""Render a trace's screenshots, resolving original-machine paths after download."""
import argparse
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]


def existing_path(value):
    path = Path(value)
    if path.is_file():
        return path.resolve()
    if '/work/' in str(value):
        path = ROOT / 'work' / str(value).split('/work/', 1)[1]
    elif not path.is_absolute():
        path = ROOT / path
    if not path.is_file():
        raise FileNotFoundError(f'Fetch the corresponding evidence asset: {value}')
    return path.resolve()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('session', type=Path)
    parser.add_argument('label')
    parser.add_argument('--last', type=int, default=12)
    parser.add_argument('--range', nargs=2, type=int, metavar=('FIRST', 'LAST'))
    parser.add_argument('--moments', action='store_true')
    parser.add_argument('--columns', type=int, default=4)
    args = parser.parse_args()
    trace_path = args.session / 'trace.json'
    trace = json.loads(trace_path.read_text())
    steps = list(enumerate(trace['steps']))
    selected = steps[args.range[0]:args.range[1] + 1] if args.range else steps[-args.last:]
    panels = []
    for index, step in selected:
        if args.moments:
            for moment in step.get('intermediate', []):
                panels.append({'path': str(existing_path(moment['screenshot'])),
                               'label': f'{index:03d} {step["label"]} +{moment["frame"]}f'})
        panels.append({'path': str(existing_path(step['screenshot'])),
                       'label': f'{index:03d} {step["label"]}'})
    directory = ROOT / 'work/contact-sheets'
    directory.mkdir(parents=True, exist_ok=True)
    manifest = directory / f'{args.label}-input.json'
    manifest.write_text(json.dumps({'title': args.label.replace('-', ' '),
        'scope': trace['evidence'], 'build': {key: trace[key] for key in
        ('rom_sha256', 'elf_sha256')}, 'panels': panels,
        'evidence': [str(trace_path.resolve())]}, indent=2) + '\n')
    subprocess.run([sys.executable, str(ROOT / 'scripts/audit/render_contact_sheet.py'),
                    str(manifest), '--out', str(directory / f'{args.label}.png'),
                    '--columns', str(args.columns)], check=True, cwd=ROOT)


if __name__ == '__main__':
    main()
