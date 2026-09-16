#!/usr/bin/env python3
"""Run the deterministic release gates for an Emerald Champions ROM."""

from __future__ import annotations

import argparse
import re
import shutil
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PYTHON = sys.executable
sys.path.insert(0, str(ROOT / "scripts"))
from rom_artifacts import verify_rom_elf_pair


# Required gates protect current source structure and authored/generated agreement.
# Historical snapshots, prose, team-strength heuristics and corpus quotas are
# deliberately outside the release path. See docs/VERIFICATION.md.
STATIC_GATES = (
    ("compiled map and tile integrity", (
        PYTHON, "scripts/audit/map_integrity.py", "--out", "work/audits/map_tile_inventory.json",
    )),
    ("canonical trainer output agreement and configured abilities", (
        PYTHON, "scripts/emerald_champions_teams.py", "--check",
    )),
    ("preset structural validation and output agreement (not strategy quality)", (PYTHON, "scripts/generate_emerald_champions_battle_sets.py", "--check")),
    ("authored Circuit projection", (PYTHON, "scripts/generate_showdown_champions_circuit.py", "--check")),
    ("wild table integrity", (PYTHON, "scripts/verify_wild_distribution.py")),
    ("one acquisition source per Mega Stone", (PYTHON, "scripts/verify_mega_stone_rewards.py")),
    ("ground Mega Stone sparkles on authored tiles", (PYTHON, "scripts/check_stone_placement.py")),
)

# Guide/README/docs drift against source (roster census, economy prices, caps,
# retired items, Mega Stone sourcing, duplicate guide paragraphs). This is run
# every release, but --strict-book controls whether a FAIL here blocks the
# release: the guide/README/docs currently contradict each other (and source)
# in known, not-yet-reconciled ways, so it stays advisory until that prose is
# fixed. Pass --strict-book once scripts/check_book_consistency.py is clean.
BOOK_CONSISTENCY_GATE = ("hand-authored guide/docs agree with source", (PYTHON, "scripts/check_book_consistency.py"))


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def run_gate(label: str, command: tuple[str, ...]) -> None:
    print(f"\n== {label} ==", flush=True)
    subprocess.run(command, cwd=ROOT, check=True)


def gba_header_checksum(data: bytes) -> int:
    return (-(sum(data[0xA0:0xBD]) + 0x19)) & 0xFF


def verify_release_symbols(symbols: str) -> None:
    # These compiled interfaces change boot behavior or permit fixture/agent
    # mutation. The ordinary release keeps only the disabled test-runner stub.
    forbidden_prefixes = (
        "CB2_EmeraldChampionsHeadlessFixture", "EmeraldChampionsHeadlessObserve",
        "EmeraldChampionsAgentPrepPoll", "gEcHeadless", "gEcAgentPrep",
        "CB2_TestRunner", "gTestRunnerState", "gTestRunnerHeadless",
    )
    names = re.findall(r"(?m)^[0-9a-fA-F]+\s+\w\s+(\S+)$", symbols)
    forbidden = sorted(name for name in names if name.startswith(forbidden_prefixes))
    require(not forbidden, f"release ELF contains test/fixture interfaces: {forbidden}")


def elf_memory(elf: Path) -> tuple[int, int, int]:
    nm = shutil.which("arm-none-eabi-nm")
    size = shutil.which("arm-none-eabi-size")
    require(nm is not None and size is not None, "arm-none-eabi-nm/size are required for release verification")
    symbols = subprocess.check_output((nm, "-n", str(elf)), text=True)
    verify_release_symbols(symbols)
    match = re.search(r"(?m)^([0-9a-fA-F]+)\s+\w\s+__rom_end$", symbols)
    require(match is not None, "ELF lacks __rom_end")
    rom_used = int(match.group(1), 16) - 0x08000000

    sections = subprocess.check_output((size, "-A", str(elf)), text=True)
    section_sizes = {
        name: int(value)
        for name, value in re.findall(r"(?m)^(\.\S+)\s+(\d+)\s+\d+$", sections)
    }
    ewram_used = section_sizes.get(".ewram", 0) + section_sizes.get(".ewram.sbss", 0)
    iwram_used = section_sizes.get(".iwram", 0) + section_sizes.get(".iwram.bss", 0)
    return rom_used, ewram_used, iwram_used


def verify_rom(rom: Path, elf: Path) -> None:
    require(rom.is_file(), f"release ROM is missing: {rom}")
    require(elf.is_file(), f"release ELF is missing: {elf}")
    verify_rom_elf_pair(rom, elf)
    data = rom.read_bytes()
    require(data[0xA0:0xAC] == b"EM CHAMPIONS", f"wrong ROM title: {data[0xA0:0xAC]!r}")
    require(data[0xAC:0xB0] == b"BPEE", f"wrong ROM game code: {data[0xAC:0xB0]!r}")
    require(data[0xB0:0xB2] == b"01", f"wrong ROM maker code: {data[0xB0:0xB2]!r}")
    require(data[0xBD] == gba_header_checksum(data), "GBA header checksum is invalid")

    rom_used, ewram_used, iwram_used = elf_memory(elf)
    require(0 < rom_used <= 32 * 1024 * 1024, f"ROM address space overflow: {rom_used}")
    require(ewram_used <= 256 * 1024, f"EWRAM overflow: {ewram_used}")
    require(iwram_used <= 32 * 1024, f"IWRAM overflow: {iwram_used}")
    print("PASS: ROM header, checksum, and GBA memory regions are valid")
    print(
        "memory: "
        f"ROM {rom_used:,}/33,554,432 ({rom_used / (32 * 1024 * 1024):.2%}), "
        f"EWRAM {ewram_used:,}/262,144 ({ewram_used / (256 * 1024):.2%}), "
        f"IWRAM {iwram_used:,}/32,768 ({iwram_used / (32 * 1024):.2%})"
    )


def verify_build_freshness(rom: Path, elf: Path) -> None:
    require(rom.parent == ROOT and elf.parent == ROOT, "release artifacts must live at the repository root")
    require(rom.name.replace(".gba", ".elf") == elf.name, "ROM and ELF names do not describe the same build")
    require(rom.is_file() and elf.is_file(), f"release artifacts are missing: {rom}, {elf}")

    # Modification times cannot prove the ROM came from *these* sources: the
    # Docker builder compiles a copied tree, a checkout rewrites every mtime,
    # and a stray `touch` fails a correct build.  The content stamp written by
    # scripts/stamp_release_inputs.py inside the tree that was actually built is
    # required integrity evidence; a successful build remains a separate step.
    run_gate(
        "release input content stamp",
        (PYTHON, "scripts/stamp_release_inputs.py", "--check", "--stamp",
         str(rom.with_suffix(".inputs.json"))),
    )


def verify_patch_integrity(*, allow_source_bundle: bool) -> None:
    probe = subprocess.run(
        ("git", "rev-parse", "--is-inside-work-tree"),
        cwd=ROOT,
        check=False,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    if probe.returncode != 0:
        require(
            allow_source_bundle,
            "Git metadata is required for whitespace and patch-integrity verification; "
            "pass --allow-source-bundle only for an intentionally metadata-free export",
        )
        print("SKIP (explicit): git diff --check for metadata-free source bundle")
        return
    # The vendored official mGBA snapshot is preserved byte-for-byte. Its
    # upstream tree intentionally contains legacy line endings, whitespace,
    # and conflict-marker examples, so apply our patch-integrity policy only
    # to Emerald Champions-owned source and evidence.
    run_gate(
        "whitespace and patch integrity",
        ("git", "diff", "--check", "--", ".", ":(exclude)tools/mgba-source/**"),
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--rom", type=Path, default=ROOT / "pokeemerald-release.gba")
    parser.add_argument("--elf", type=Path, default=ROOT / "pokeemerald-release.elf")
    parser.add_argument(
        "--allow-source-bundle",
        action="store_true",
        help="allow an intentional metadata-free export to skip git diff --check",
    )
    parser.add_argument(
        "--lenient-book",
        action="store_true",
        help="downgrade scripts/check_book_consistency.py to advisory (it is release-blocking by default since September 15, 2026)",
    )
    args = parser.parse_args()

    for label, command in STATIC_GATES:
        run_gate(label, command)
    label, command = BOOK_CONSISTENCY_GATE
    try:
        run_gate(label, command)
    except subprocess.CalledProcessError:
        if not args.lenient_book:
            raise
        print(f"NOTE: '{label}' failed but --lenient-book was passed; continuing (see docstring in "
              "scripts/check_book_consistency.py).")
    verify_patch_integrity(allow_source_bundle=args.allow_source_bundle)
    verify_build_freshness(args.rom.resolve(), args.elf.resolve())
    verify_rom(args.rom.resolve(), args.elf.resolve())
    print("\nEMERALD CHAMPIONS RELEASE GATES: PASS")


if __name__ == "__main__":
    main()
