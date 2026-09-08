#!/usr/bin/env python3
"""Replay the inspected normal-key opening trace on a newly pinned release ROM.
This is a one-off evidence driver around the existing native runner, not a test gate.
"""
from pathlib import Path
import argparse
import hashlib
import json
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import native_tools
from rom_artifacts import verify_rom_elf_pair

SELECTED_ACTIONS = [0, 18, 24, 44, 445, 479, 778, 971, 978, 987, 993, 1015, 1035,
                    1082, 1093, 1150, 1176, 1220, 1380, 1393, 1408, 1542, 1642,
                    1673, 1705, 1713, 1721, 1771, 1797]

def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--rom', type=Path, required=True)
    p.add_argument('--elf', type=Path, required=True)
    p.add_argument('--stamp', type=Path)
    p.add_argument('--out', type=Path, required=True)
    p.add_argument('--through-action', type=int, default=1797)
    args = p.parse_args()
    out = args.out.resolve()
    if out.exists():
        raise SystemExit('Use a new evidence directory; prior evidence is immutable.')
    artifacts = out / 'artifacts'
    shots = out / 'screenshots'
    artifacts.mkdir(parents=True)
    shots.mkdir()
    identity = {}
    for kind, source in [('rom', args.rom), ('elf', args.elf), ('stamp', args.stamp)]:
        if source is None:
            continue
        source = source.resolve()
        target = artifacts / ('production' + source.suffix if kind != 'stamp' else 'inputs.json')
        before = sha(source)
        shutil.copy2(source, target)
        if before != sha(source) or before != sha(target):
            raise SystemExit(f'{kind} changed during pinning')
        identity[kind] = {'source': str(source), 'snapshot': str(target), 'sha256': before,
                          'size': target.stat().st_size}
    rom = Path(identity['rom']['snapshot'])
    elf = Path(identity['elf']['snapshot'])
    verify_rom_elf_pair(rom, elf)
    runner = ROOT / 'build/headless/emerald_champions_mgba_runner'
    identity['runner'] = {'path': str(runner), 'sha256': sha(runner)}
    symbols = native_tools.symbols(elf, ROOT)
    if 'gEcHeadlessFixtureScenario' in symbols or 'gEcAgentPrepCommand' in symbols:
        raise SystemExit('This replay requires production, with fixture setup compiled out.')
    original = ROOT / 'work/runtime-opening/intermediate-build3/normal-key-trace.json'
    trace = json.loads(original.read_text())
    trace = [e for e in trace if e['action_index'] <= args.through_action]
    (out / 'normal-key-trace.json').write_text(json.dumps(trace, indent=2) + '\n')
    frames = 1200 + args.through_action * 30
    command = [str(runner), '--rom', str(rom), '--frames', str(frames), '--rtc', '946684800',
               '--save', '-', '--state-out', str(out / 'final.ss1'),
               '--screenshot', str(out / 'final.png')]
    for event in trace:
        command += ['--key', f"{event['frame']}:{event['duration']}:{event['key']}"]
    for action in SELECTED_ACTIONS:
        if action <= args.through_action:
            command += ['--screenshot-at', f'{1200 + action * 30 - 1}:{shots / f"{action:06d}.png"}']
    probes = [('party_counts_packed', 'gPartiesCount', 4), ('battle_flags', 'gBattleTypeFlags', 4),
              ('battlers_count', 'gBattlersCount', 1), ('battle_outcome', 'gBattleOutcome', 1),
              ('save1_pointer', 'gSaveBlock1Ptr', 4), ('save2_pointer', 'gSaveBlock2Ptr', 4)]
    for name, symbol, width in probes:
        command += ['--read', f'{width}:{symbols[symbol]}']
    result = {'status': 'running', 'scope': 'Fresh blank flash; replay of inspected normal-key input only. No imported save, RAM writes, fixture automation or autowin. Screenshots require visual review; successful process exit is not a gameplay verdict.',
              'artifacts': identity, 'trace_source': str(original), 'key_events': len(trace),
              'frames': frames, 'through_action': args.through_action,
              'probe_addresses': {n: {'symbol': s, 'width': w, 'address': symbols[s]} for n,s,w in probes},
              'command': command}
    (out / 'replay.json').write_text(json.dumps(result, indent=2) + '\n')
    print(f'Replaying {len(trace)} normal key events / {frames} frames on {identity["rom"]["sha256"]}', flush=True)
    try:
        run = subprocess.run(command, text=True, capture_output=True, timeout=300)
        (out / 'runner.log').write_text(run.stdout + run.stderr)
        result['process_exit_code'] = run.returncode
        result['status'] = 'rendered-awaiting-review' if run.returncode == 0 else 'execution-failed'
        result['screenshots'] = [{'path': str(x), 'sha256': sha(x)} for x in sorted(shots.glob('*.png'))]
        if (out / 'final.ss1').exists():
            result['state_sha256'] = sha(out / 'final.ss1')
    except subprocess.TimeoutExpired as exc:
        result['status'] = 'timed-out'
        result['error'] = str(exc)
        raise
    finally:
        (out / 'replay.json').write_text(json.dumps(result, indent=2) + '\n')
    print(run.stdout)
    if run.returncode:
        raise SystemExit(run.returncode)

if __name__ == '__main__':
    main()
