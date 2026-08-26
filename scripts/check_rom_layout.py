#!/usr/bin/env python3
"""Check linked GBA memory headroom and padded ROM geometry."""

from __future__ import annotations

import argparse
from pathlib import Path
import re
import subprocess
import sys


ROM_BASE = 0x08000000
ROM_LIMIT = 0x0A000000
IWRAM_BASE = 0x03000000
IWRAM_LIMIT = 0x03008000
MIN_IWRAM_STACK_HEADROOM = 4096


def command(*args: str) -> str:
    return subprocess.check_output(args, text=True)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--elf", type=Path, required=True)
    parser.add_argument("--rom", type=Path, required=True)
    args = parser.parse_args()

    if args.rom.stat().st_size != ROM_LIMIT - ROM_BASE:
        raise SystemExit(f"{args.rom} is not a padded 32 MiB GBA ROM")

    symbols = command("arm-none-eabi-nm", "-S", str(args.elf))
    end_match = re.search(r"^([0-9a-fA-F]+)\s+[Bb]\s+end$", symbols, re.M)
    if end_match is None:
        raise SystemExit(f"{args.elf}: missing IWRAM end symbol")
    iwram_end = int(end_match.group(1), 16)
    if not IWRAM_BASE <= iwram_end <= IWRAM_LIMIT:
        raise SystemExit(f"{args.elf}: invalid IWRAM end {iwram_end:#010x}")
    iwram_headroom = IWRAM_LIMIT - iwram_end
    if iwram_headroom < MIN_IWRAM_STACK_HEADROOM:
        raise SystemExit(
            f"{args.elf}: only {iwram_headroom} IWRAM bytes remain for stack; "
            f"minimum is {MIN_IWRAM_STACK_HEADROOM}"
        )

    sections = command("arm-none-eabi-readelf", "-S", "-W", str(args.elf))
    rom_end = ROM_BASE
    for address_text, size_text, flags in re.findall(
        r"^\s*\[\s*\d+\]\s+\S+\s+\S+\s+([0-9a-fA-F]+)\s+\S+\s+([0-9a-fA-F]+)\s+\S+\s+(\S+)",
        sections,
        re.M,
    ):
        address = int(address_text, 16)
        size = int(size_text, 16)
        if "A" in flags and ROM_BASE <= address < ROM_LIMIT:
            rom_end = max(rom_end, address + size)
    if rom_end > ROM_LIMIT:
        raise SystemExit(f"{args.elf}: allocated ROM data ends past 32 MiB at {rom_end:#010x}")

    print(
        f"{args.elf.name}: IWRAM used {iwram_end - IWRAM_BASE} bytes, "
        f"stack headroom {iwram_headroom} bytes; ROM headroom {ROM_LIMIT - rom_end} bytes"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
