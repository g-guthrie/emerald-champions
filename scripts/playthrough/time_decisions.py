#!/usr/bin/env python3
"""Replay selected archived input steps read-only and measure complete AI decisions.

The current AiLogicData flags offset is 2324, battlerMovesScored bits 19..22.
Audit these offsets against include/battle.h when changing native layouts.
The primary current.ss1 is never loaded for mutation or overwritten.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import native_tools
import render_emerald_champions_ui as ui
from contact_sheet import existing_path


def reads(output):
    return {int(a, 16): int(v, 16) for a, v in re.findall(
        r'READ width=\d+ address=([0-9a-f]+) value=([0-9a-f]+)', output)}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('session', type=Path)
    parser.add_argument('steps', nargs='+', type=int)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    trace = json.loads((args.session / 'trace.json').read_text())
    rom, elf = args.session / 'scene.gba', args.session / 'scene.elf'
    for path, key in ((rom, 'rom_sha256'), (elf, 'elf_sha256')):
        if hashlib.sha256(path.read_bytes()).hexdigest() != trace[key]:
            raise SystemExit(f'Artifact mismatch: {path}')
    syms = native_tools.symbols(elf, ROOT)
    runner = str(ui.build_runner())
    rows = []
    for index in args.steps:
        step = trace['steps'][index]
        state = existing_path(step['state_before'])
        base = [runner, '--rom', str(rom), '--state-in', str(state), '--rtc', '946684800']
        probe = ui.run([*base, '--frames', '1', '--read', f'4:0x{syms["gAiLogicData"]:x}',
            '--read', f'2:0x{syms["gBattleMons"] + 140 + 42:x}',
            '--read', f'2:0x{syms["gBattleMons"] + 420 + 42:x}'])
        values = reads(probe.stdout)
        ai = values[syms['gAiLogicData']]
        if not ai:
            raise SystemExit(f'Step {index}: no live battle AI')
        mask = sum(1 << b for b in (1, 3) if values[syms['gBattleMons'] + 140*b + 42]) << 19
        if not mask:
            raise SystemExit(f'Step {index}: no living enemy decision')
        command = [*base, '--frames', str(step['frames']), '--until', f'4:0x{ai+2324:x}:0x{mask:x}:0x{mask:x}']
        for key in step['keys']:
            command += ['--key', key]
        for address in (ai, ai + 4, syms['gMain'] + 32):
            command += ['--read', f'4:0x{address:x}']
        result = ui.run(command)
        if 'stop_matched=1' not in result.stdout:
            raise SystemExit(f'Step {index}: complete decision was not observed\n{result.stdout}')
        values = reads(result.stdout)
        frames = (values[syms['gMain'] + 32] - values[ai]) & 0xffffffff
        rows.append({'step': index, 'label': step['label'], 'decision_frames': frames,
            'setup_frames': values[ai+4], 'seconds_approx': frames / 59.7275,
            'runner_output': result.stdout})
    report = {'scope': 'Read-only exact input replay; complete opposing decisions including setup; sampled boards only.',
        'rom_sha256': trace['rom_sha256'], 'elf_sha256': trace['elf_sha256'], 'rows': rows,
        'max_native_frames': max(row['decision_frames'] for row in rows), 'budget_seconds': 1.2}
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({'decisions': len(rows), 'max_native_frames': report['max_native_frames'], 'report': str(args.out)}))


if __name__ == '__main__':
    main()
