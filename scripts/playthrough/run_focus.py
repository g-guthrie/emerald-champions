#!/usr/bin/env python3
"""Run only explicitly selected native regression filters from a stamped test ELF."""
import argparse
from pathlib import Path
import platform
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
from run_emerald_champions_runtime_gates import RuntimeGate, resolve_tool, verify_gate


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', type=Path, default=ROOT / 'pokeemerald-test.elf')
    parser.add_argument('--filter', action='append', required=True)
    parser.add_argument('--patchelf')
    parser.add_argument('--hydra')
    parser.add_argument('--romtest')
    parser.add_argument('--runtime-cwd', type=Path, default=ROOT)
    args = parser.parse_args()
    stamp = args.elf.with_name(args.elf.stem + '.inputs.json').resolve()
    subprocess.run([sys.executable, str(ROOT / 'scripts/stamp_release_inputs.py'),
                    '--check', '--stamp', str(stamp)], check=True, cwd=ROOT)
    darwin = platform.system() == 'Darwin'
    patchelf = resolve_tool(args.patchelf, (), ROOT / 'tools/patchelf/patchelf')
    hydra = resolve_tool(args.hydra, ('mgba-rom-test-hydra',), ROOT / 'tools/mgba-rom-test-hydra/mgba-rom-test-hydra')
    romtest = resolve_tool(args.romtest, (), ROOT / 'tools/mgba' / ('mgba-rom-test-mac' if darwin else 'mgba-rom-test'))
    objcopy = shutil.which('arm-none-eabi-objcopy')
    if not objcopy:
        raise SystemExit('Put ARM binutils on PATH.')
    for name in args.filter:
        verify_gate(RuntimeGate(name), test_elf=args.elf.resolve(),
            headless_elf=args.elf.with_name(args.elf.stem + '-focus.elf').resolve(),
            patchelf=patchelf, hydra=hydra, romtest=romtest, objcopy=objcopy,
            runtime_cwd=args.runtime_cwd.resolve())


if __name__ == '__main__':
    main()
