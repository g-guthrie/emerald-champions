#!/usr/bin/env python3
"""Run the deterministic release gates for an Emerald Champions ROM."""

from __future__ import annotations

import argparse
import collections
import re
import shutil
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PYTHON = sys.executable


STATIC_GATES = (
    ("core services", (PYTHON, "scripts/verify_emerald_champions_core.py")),
    ("finite rewards", (PYTHON, "scripts/emerald_champions_reward_rewrite.py")),
    ("wild distribution", (PYTHON, "scripts/emerald_champions_wild_distribution.py")),
    ("route signs", (PYTHON, "scripts/emerald_champions_route_signs.py")),
    ("competitive presets", (PYTHON, "scripts/verify_emerald_champions_battle_sets.py")),
    ("campaign roster", (PYTHON, "scripts/verify_emerald_champions_campaign_roster.py")),
    ("story and dialogue", (PYTHON, "scripts/verify_emerald_champions_story.py")),
    ("legendary availability", (PYTHON, "scripts/verify_legendary_availability.py")),
    ("legendary signs and Circuit", (PYTHON, "scripts/verify_legendary_signs_and_circuit.py")),
    ("regional starters", (PYTHON, "scripts/verify_regional_starters.py")),
    ("restored world", (PYTHON, "scripts/verify_restored_emerald_champions_world.py")),
    ("single-player evolutions", (PYTHON, "scripts/verify_solo_evolution_access.py")),
    ("Poke Vial quest", (PYTHON, "scripts/restore_poke_vial_quest.py")),
    ("campaign battle master", (PYTHON, "scripts/audit_emerald_champions_master_battles.py")),
    ("battle script formats", (PYTHON, "scripts/align_emerald_champions_battle_scripts.py")),
)


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def run_gate(label: str, command: tuple[str, ...]) -> None:
    print(f"\n== {label} ==", flush=True)
    subprocess.run(command, cwd=ROOT, check=True)


def verify_materialized_trainers() -> None:
    master = (ROOT / "docs/emerald_champions_master_battle_design.txt").read_text()
    encounter_count = len(re.findall(r"(?m)^=== ENCOUNTER \d{4} ===$", master))
    require(encounter_count > 0, "canonical battle master has no encounters")
    run_gate(
        "materialized trainer parties",
        (
            PYTHON,
            "scripts/implement_emerald_champions_master_battles.py",
            "--through-encounter",
            str(encounter_count),
            "--verify-only",
        ),
    )


def verify_unique_state_ids() -> None:
    for relative, prefix in (
        ("include/constants/flags.h", "FLAG_"),
        ("include/constants/vars.h", "VAR_"),
    ):
        values: dict[int, list[str]] = collections.defaultdict(list)
        for line in (ROOT / relative).read_text().splitlines():
            match = re.match(
                rf"#define\s+({prefix}[A-Z0-9_]+)\s+(0x[0-9A-Fa-f]+|\d+)\b",
                line,
            )
            if match is None:
                continue
            value = int(match.group(2), 0)
            if value != 0:  # FRLG compatibility aliases use zero deliberately.
                values[value].append(match.group(1))
        duplicates = {value: names for value, names in values.items() if len(names) > 1}
        require(not duplicates, f"{relative}: duplicate state IDs: {duplicates}")
    print("PASS: numeric flag and variable assignments are unique")


def verify_branding() -> None:
    forbidden = ("inclement emerald", "salt buffet", "buffet salt")
    violations = []
    for directory in (ROOT / "data", ROOT / "src"):
        for path in directory.rglob("*"):
            if path.suffix not in {".c", ".h", ".inc", ".s"}:
                continue
            for line_number, line in enumerate(path.read_text(errors="ignore").splitlines(), 1):
                quoted = " ".join(re.findall(r'"([^"]*)"', line)).lower()
                if any(token in quoted for token in forbidden):
                    violations.append(f"{path.relative_to(ROOT)}:{line_number}: {line.strip()}")
    require(not violations, "obsolete player-facing branding remains:\n" + "\n".join(violations))
    makefile = (ROOT / "Makefile").read_text()
    require("TITLE        ?= EM CHAMPIONS" in makefile, "ROM title is not Emerald Champions")
    require("GAME_CODE    ?= BPEE" in makefile, "ROM game code drifted")
    print("PASS: player-facing branding and build identity are Emerald Champions")


def gba_header_checksum(data: bytes) -> int:
    return (-(sum(data[0xA0:0xBD]) + 0x19)) & 0xFF


def elf_memory(elf: Path) -> tuple[int, int, int]:
    nm = shutil.which("arm-none-eabi-nm")
    size = shutil.which("arm-none-eabi-size")
    require(nm is not None and size is not None, "arm-none-eabi-nm/size are required for release verification")
    symbols = subprocess.check_output((nm, "-n", str(elf)), text=True)
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
    data = rom.read_bytes()
    require(len(data) == 32 * 1024 * 1024, f"ROM is not exactly 32 MiB: {len(data)} bytes")
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


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--rom", type=Path, default=ROOT / "pokeemerald.gba")
    parser.add_argument("--elf", type=Path, default=ROOT / "pokeemerald.elf")
    args = parser.parse_args()

    for label, command in STATIC_GATES:
        run_gate(label, command)
    verify_materialized_trainers()
    verify_unique_state_ids()
    verify_branding()
    run_gate("whitespace and patch integrity", ("git", "diff", "--check"))
    verify_rom(args.rom.resolve(), args.elf.resolve())
    print("\nEMERALD CHAMPIONS RELEASE GATES: PASS")


if __name__ == "__main__":
    main()
