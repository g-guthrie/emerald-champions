#!/usr/bin/env python3
"""Run the deterministic release gates for an Emerald Champions ROM."""

from __future__ import annotations

import argparse
import ast
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
    ("native field UI", (PYTHON, "scripts/verify_emerald_champions_native_ui.py")),
    ("visual contracts", (PYTHON, "scripts/verify_emerald_champions_visual_contracts.py")),
    ("Inclement visual sources", (PYTHON, "scripts/verify_inclement_visual_sources.py")),
    ("Inclement overworld parity", (PYTHON, "scripts/verify_inclement_overworld_parity.py")),
    (
        "Verdant visual byte inventory",
        (PYTHON, "scripts/audit_verdant_visual_parity.py", "--check-fast"),
    ),
    ("finite rewards", (PYTHON, "scripts/emerald_champions_reward_rewrite.py")),
    ("reward economy", (PYTHON, "scripts/verify_emerald_champions_reward_economy.py")),
    ("wild distribution", (PYTHON, "scripts/emerald_champions_wild_distribution.py")),
    ("route signs", (PYTHON, "scripts/emerald_champions_route_signs.py")),
    ("competitive presets", (PYTHON, "scripts/verify_emerald_champions_battle_sets.py")),
    ("species stat rebalances", (PYTHON, "scripts/verify_species_stat_rebalances.py")),
    ("upstream critical fixes", (PYTHON, "scripts/verify_upstream_critical_fixes.py")),
    ("campaign roster", (PYTHON, "scripts/verify_emerald_champions_campaign_roster.py")),
    ("Game Corner starter archive", (PYTHON, "scripts/verify_game_corner_starter_archive.py")),
    ("trainer Ability legality", (PYTHON, "scripts/verify_trainer_ability_legality.py")),
    ("trainer runtime coherence", (PYTHON, "scripts/verify_trainer_runtime_coherence.py")),
    ("trainer row reachability", (PYTHON, "scripts/prune_unreachable_trainer_parties.py")),
    ("trainer dialogue species", (PYTHON, "scripts/audit_trainer_dialogue_species.py")),
    ("story and dialogue", (PYTHON, "scripts/verify_emerald_champions_story.py")),
    ("rematch-free Match Call", (PYTHON, "scripts/verify_rematch_free_match_call.py")),
    ("whole-campaign progression graph", (PYTHON, "scripts/verify_emerald_champions_progression.py")),
    ("legendary availability", (PYTHON, "scripts/verify_legendary_availability.py")),
    ("legendary signs and Circuit", (PYTHON, "scripts/verify_legendary_signs_and_circuit.py")),
    ("regional starters", (PYTHON, "scripts/verify_regional_starters.py")),
    ("restored world", (PYTHON, "scripts/verify_restored_emerald_champions_world.py")),
    ("single-player evolutions", (PYTHON, "scripts/verify_solo_evolution_access.py")),
    ("fossil revival", (PYTHON, "scripts/verify_fossil_revival.py")),
    ("Poke Vial quest", (PYTHON, "scripts/restore_poke_vial_quest.py")),
    ("campaign battle master", (PYTHON, "scripts/audit_emerald_champions_master_battles.py")),
    ("Frontier competitive loadouts", (PYTHON, "scripts/generate_emerald_champions_frontier_sets.py", "--check")),
    ("battle script formats", (PYTHON, "scripts/align_emerald_champions_battle_scripts.py")),
)


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def run_gate(label: str, command: tuple[str, ...]) -> None:
    print(f"\n== {label} ==", flush=True)
    subprocess.run(command, cwd=ROOT, check=True)


def verify_unique_state_ids() -> None:
    for relative, prefix in (
        ("include/constants/flags.h", "FLAG_"),
        ("include/constants/vars.h", "VAR_"),
    ):
        definitions: dict[str, list[tuple[int, str]]] = collections.defaultdict(list)
        for line_number, line in enumerate((ROOT / relative).read_text().splitlines(), 1):
            match = re.match(
                rf"#define\s+({prefix}[A-Z0-9_]+)\s+(.+?)(?:\s*//.*|\s*/\*.*)?$",
                line,
            )
            if match is not None:
                definitions[match.group(1)].append((line_number, match.group(2).strip()))
        duplicate_names = {
            name: rows for name, rows in definitions.items() if len(rows) > 1
        }
        require(
            not duplicate_names,
            f"{relative}: state macros are defined more than once: {duplicate_names}",
        )

        expressions = {name: rows[0][1] for name, rows in definitions.items()}
        resolved: dict[str, int] = {}

        def evaluate_node(node: ast.AST, stack: tuple[str, ...]) -> int:
            if isinstance(node, ast.Constant) and isinstance(node.value, int):
                return node.value
            if isinstance(node, ast.Name):
                return evaluate(node.id, stack)
            if isinstance(node, ast.UnaryOp) and isinstance(
                node.op, (ast.UAdd, ast.USub, ast.Invert)
            ):
                value = evaluate_node(node.operand, stack)
                if isinstance(node.op, ast.UAdd):
                    return value
                if isinstance(node.op, ast.USub):
                    return -value
                return ~value
            if isinstance(node, ast.BinOp) and isinstance(
                node.op, (ast.Add, ast.Sub, ast.BitOr, ast.BitAnd, ast.LShift, ast.RShift)
            ):
                left = evaluate_node(node.left, stack)
                right = evaluate_node(node.right, stack)
                if isinstance(node.op, ast.Add):
                    return left + right
                if isinstance(node.op, ast.Sub):
                    return left - right
                if isinstance(node.op, ast.BitOr):
                    return left | right
                if isinstance(node.op, ast.BitAnd):
                    return left & right
                if isinstance(node.op, ast.LShift):
                    return left << right
                return left >> right
            raise ValueError(f"unsupported state expression: {ast.dump(node)}")

        def evaluate(name: str, stack: tuple[str, ...] = ()) -> int:
            if name in resolved:
                return resolved[name]
            if name not in expressions or name in stack:
                raise ValueError(name)
            value = evaluate_node(ast.parse(expressions[name], mode="eval").body, stack + (name,))
            resolved[name] = value
            return value

        values: dict[int, list[str]] = collections.defaultdict(list)
        for name in expressions:
            if name.endswith(("_START", "_END")):
                continue
            try:
                value = evaluate(name)
            except (SyntaxError, ValueError):
                continue
            if value != 0:  # FRLG compatibility aliases use zero deliberately.
                values[value].append(name)
        duplicates = {value: names for value, names in values.items() if len(names) > 1}
        require(not duplicates, f"{relative}: duplicate state IDs: {duplicates}")
    print("PASS: resolved flag and variable names and assignments are unique")


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


def verify_build_freshness(rom: Path, elf: Path) -> None:
    require(rom.parent == ROOT and elf.parent == ROOT, "release artifacts must live at the repository root")
    require(rom.name.replace(".gba", ".elf") == elf.name, "ROM and ELF names do not describe the same build")
    require(rom.is_file() and elf.is_file(), f"release artifacts are missing: {rom}, {elf}")

    inputs: list[Path] = []
    for directory in ("src", "data", "include", "asm", "graphics", "sound", "libagbsyscall"):
        inputs.extend(
            path for path in (ROOT / directory).rglob("*")
            if path.is_file() and not path.name.startswith("._")
        )
    inputs.extend(
        path for path in ROOT.iterdir()
        if path.is_file()
        and (path.name in {"Makefile", "config.mk", "make_tools.mk", "charmap.txt"}
             or path.suffix in {".mk", ".ld"})
    )
    require(inputs, "no build inputs were found")
    # Modification times cannot prove the ROM came from *these* sources: the
    # Docker builder compiles a copied tree, a checkout rewrites every mtime,
    # and a stray `touch` fails a correct build.  The content stamp written by
    # scripts/stamp_release_inputs.py inside the tree that was actually built is
    # the only freshness evidence this gate accepts.
    run_gate(
        "release input content stamp",
        (PYTHON, "scripts/stamp_release_inputs.py", "--check"),
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
    args = parser.parse_args()

    for label, command in STATIC_GATES:
        run_gate(label, command)
    # Master-design/party equality is proven by the campaign battle master
    # gate; the former materialization re-check duplicated it.
    verify_unique_state_ids()
    verify_branding()
    verify_patch_integrity(allow_source_bundle=args.allow_source_bundle)
    verify_build_freshness(args.rom.resolve(), args.elf.resolve())
    verify_rom(args.rom.resolve(), args.elf.resolve())
    print("\nEMERALD CHAMPIONS RELEASE GATES: PASS")


if __name__ == "__main__":
    main()
