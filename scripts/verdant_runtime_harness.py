#!/usr/bin/env python3
"""Build and boot the current Verdant checkout with deterministic mGBA state."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import time


ROOT = Path(__file__).resolve().parents[1]
RUNNER_SOURCE = ROOT / "tests/headless/verdant_mgba_runner.c"
FIXTURE_PLAN = ROOT / "tests/headless/fixtures.json"
ROM_BASE = 0x08000000
ROM_LIMIT = 0x0A000000
EWRAM_BASE = 0x02000000
EWRAM_LIMIT = 0x02040000
FLASH_SIZE = 128 * 1024
RESULT_PATTERN = re.compile(
    r"^RESULT frames=(?P<frames>\d+) stop_matched=(?P<stop>\d+) "
    r"pc=(?P<pc>[0-9a-f]{8}) rtc=(?P<rtc>-?\d+) width=(?P<width>\d+) "
    r"height=(?P<height>\d+) video_hash=(?P<video>[0-9a-f]{16}) "
    r"nonzero_pixels=(?P<nonzero>\d+) save_bytes=(?P<save>\d+)$"
)
READ_PATTERN = re.compile(
    r"^READ width=(?P<width>[124]) address=(?P<address>[0-9a-f]{8}) "
    r"value=(?P<value>[0-9a-f]{8})$"
)


def run(command: list[str], *, cwd: Path = ROOT, timeout: float | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, cwd=cwd, text=True, capture_output=True, timeout=timeout)


def require_success(proc: subprocess.CompletedProcess[str], label: str) -> None:
    if proc.returncode == 0:
        return
    raise RuntimeError(
        f"{label} failed with exit {proc.returncode}\n"
        f"stdout:\n{proc.stdout[-4000:]}\n"
        f"stderr:\n{proc.stderr[-4000:]}"
    )


def find_mgba_prefix(explicit: Path | None) -> Path:
    candidates: list[Path] = []
    if explicit is not None:
        candidates.append(explicit)
    if os.environ.get("MGBA_PREFIX"):
        candidates.append(Path(os.environ["MGBA_PREFIX"]))
    brew = shutil.which("brew")
    if brew:
        proc = run([brew, "--prefix", "mgba"])
        if proc.returncode == 0 and proc.stdout.strip():
            candidates.append(Path(proc.stdout.strip()))
    candidates.extend((Path("/opt/homebrew/opt/mgba"), Path("/usr/local/opt/mgba")))
    for candidate in candidates:
        if (candidate / "include/mgba/core/core.h").is_file() and any((candidate / "lib").glob("libmgba*")):
            return candidate.resolve()
    raise RuntimeError("mGBA development headers/library not found; set MGBA_PREFIX")


def build_runner(output: Path, mgba_prefix: Path | None) -> dict[str, object]:
    prefix = find_mgba_prefix(mgba_prefix)
    output.parent.mkdir(parents=True, exist_ok=True)
    cc = os.environ.get("CC", "cc")
    command = [
        cc,
        "-std=c11",
        "-O2",
        "-Wall",
        "-Wextra",
        "-Werror",
        f"-I{prefix / 'include'}",
        str(RUNNER_SOURCE),
        f"-L{prefix / 'lib'}",
        "-lmgba",
        f"-Wl,-rpath,{prefix / 'lib'}",
        "-o",
        str(output),
    ]
    proc = run(command)
    require_success(proc, "headless runner build")
    return {"runner": str(output), "mgba_prefix": str(prefix), "command": command}


def tool(name: str) -> str:
    found = shutil.which(name)
    if found:
        return found
    local = ROOT / "tools" / name
    if local.is_file():
        return str(local)
    raise RuntimeError(f"required tool not found: {name}")


def force_build_rom(jobs: int) -> dict[str, object]:
    source_before = working_tree_fingerprint()
    started_ns = time.time_ns()
    proc = run(["make", "-B", f"-j{jobs}"], timeout=900)
    require_success(proc, "forced production ROM build")
    rom = ROOT / "pokeemerald.gba"
    elf = ROOT / "pokeemerald.elf"
    if not rom.is_file() or not elf.is_file():
        raise RuntimeError("forced build did not produce pokeemerald.gba and pokeemerald.elf")
    if rom.stat().st_mtime_ns < started_ns or elf.stat().st_mtime_ns < started_ns:
        raise RuntimeError("ROM or ELF predates the forced build invocation")
    source_after = working_tree_fingerprint()
    if source_after != source_before:
        raise RuntimeError("working source changed during the forced build; retry from a stable tree")
    return {
        "rom": str(rom),
        "elf": str(elf),
        "rom_sha256": hashlib.sha256(rom.read_bytes()).hexdigest(),
        "elf_sha256": hashlib.sha256(elf.read_bytes()).hexdigest(),
        "rom_bytes": rom.stat().st_size,
        "source_fingerprint": source_after,
    }


def working_tree_fingerprint() -> str:
    digest = hashlib.sha256()
    digest.update(subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=ROOT))
    digest.update(subprocess.check_output(
        ["git", "diff", "--binary", "--no-ext-diff", "HEAD", "--"], cwd=ROOT
    ))
    untracked = subprocess.check_output(
        ["git", "ls-files", "--others", "--exclude-standard", "-z"], cwd=ROOT
    ).split(b"\0")
    for raw_path in sorted(path for path in untracked if path):
        path = ROOT / os.fsdecode(raw_path)
        if not path.is_file():
            continue
        digest.update(raw_path)
        digest.update(b"\0")
        digest.update(path.read_bytes())
    return digest.hexdigest()


def symbols(elf: Path, names: set[str]) -> dict[str, int]:
    nm = tool("arm-none-eabi-nm")
    proc = run([nm, "-S", str(elf)])
    require_success(proc, "ELF symbol scan")
    found: dict[str, int] = {}
    for line in proc.stdout.splitlines():
        parts = line.split()
        if parts and parts[-1] in names:
            found[parts[-1]] = int(parts[0], 16)
    missing = names - found.keys()
    if missing:
        raise RuntimeError(f"missing ELF symbols: {sorted(missing)}")
    return found


def parse_runner_output(stdout: str) -> dict[str, object]:
    result: dict[str, object] | None = None
    reads: dict[int, int] = {}
    for line in stdout.splitlines():
        match = RESULT_PATTERN.match(line)
        if match:
            result = {
                "frames": int(match.group("frames")),
                "stop_matched": bool(int(match.group("stop"))),
                "pc": int(match.group("pc"), 16),
                "rtc": int(match.group("rtc")),
                "width": int(match.group("width")),
                "height": int(match.group("height")),
                "video_hash": match.group("video"),
                "nonzero_pixels": int(match.group("nonzero")),
                "save_bytes": int(match.group("save")),
            }
            continue
        match = READ_PATTERN.match(line)
        if match:
            reads[int(match.group("address"), 16)] = int(match.group("value"), 16)
    if result is None:
        raise RuntimeError(f"runner produced no RESULT line:\n{stdout}")
    result["reads"] = reads
    return result


def run_boot_once(
    runner: Path,
    rom: Path,
    addresses: dict[str, int],
    out: Path,
    run_id: int,
    frames: int,
    rtc: int,
) -> dict[str, object]:
    screenshot = out / f"boot-{run_id}.png"
    save_in = out / f"boot-{run_id}-erased.sav"
    save_out = out / f"boot-{run_id}.sav"
    save_in.write_bytes(b"\xff" * FLASH_SIZE)
    gmain = addresses["gMain"]
    read_addresses = {
        "callback2": gmain + 4,
        "vblank_counter": gmain + 0x20,
        "save_block_1_ptr": addresses["gSaveBlock1Ptr"],
        "save_block_2_ptr": addresses["gSaveBlock2Ptr"],
    }
    command = [
        str(runner),
        "--rom", str(rom),
        "--frames", str(frames),
        "--rtc", str(rtc),
        "--save", str(save_in),
        "--screenshot", str(screenshot),
        "--save-out", str(save_out),
    ]
    for address in read_addresses.values():
        command.extend(("--read", f"4:0x{address:x}"))
    proc = run(command, timeout=60)
    require_success(proc, f"production boot run {run_id}")
    parsed = parse_runner_output(proc.stdout)
    parsed["named_reads"] = {
        name: parsed["reads"][address] for name, address in read_addresses.items()
    }
    parsed["screenshot"] = str(screenshot)
    parsed["save"] = str(save_out)
    parsed["stdout"] = proc.stdout.strip()

    png = screenshot.read_bytes() if screenshot.is_file() else b""
    if not png.startswith(b"\x89PNG\r\n\x1a\n") or len(png) < 1000:
        raise RuntimeError(f"boot run {run_id} did not produce a credible PNG")
    if not save_out.is_file() or save_out.stat().st_size != FLASH_SIZE:
        raise RuntimeError(f"boot run {run_id} save geometry is not {FLASH_SIZE} bytes")
    if parsed["nonzero_pixels"] == 0:
        raise RuntimeError(f"boot run {run_id} rendered a blank frame")
    callback = parsed["named_reads"]["callback2"]
    if not ROM_BASE <= callback < ROM_LIMIT:
        raise RuntimeError(f"boot run {run_id} has invalid callback2 {callback:#x}")
    if parsed["named_reads"]["vblank_counter"] < frames // 2:
        raise RuntimeError(f"boot run {run_id} did not advance VBlank")
    for name in ("save_block_1_ptr", "save_block_2_ptr"):
        value = parsed["named_reads"][name]
        if not EWRAM_BASE <= value < EWRAM_LIMIT:
            raise RuntimeError(f"boot run {run_id} has invalid {name} {value:#x}")
    return parsed


def validate_plan() -> dict[str, object]:
    plan = json.loads(FIXTURE_PLAN.read_text())
    if plan.get("schema_version") != 1:
        raise RuntimeError("unsupported fixture-plan schema")
    fixtures = plan.get("fixtures")
    if not isinstance(fixtures, list) or not fixtures:
        raise RuntimeError("fixture plan is empty")
    ids = [fixture.get("id") for fixture in fixtures]
    if len(ids) != len(set(ids)) or any(not fixture_id for fixture_id in ids):
        raise RuntimeError("fixture IDs must be nonempty and unique")
    categories = {fixture.get("category") for fixture in fixtures}
    expected = {"boot", "save", "ui", "battle", "frontier"}
    if categories != expected:
        raise RuntimeError(f"fixture categories drifted: {sorted(categories)}")
    return {"fixtures": len(fixtures), "categories": sorted(categories)}


def command_build_runner(args: argparse.Namespace) -> int:
    metadata = build_runner(args.output.resolve(), args.mgba_prefix)
    print(json.dumps(metadata, indent=2))
    return 0


def command_plan(args: argparse.Namespace) -> int:
    del args
    print(json.dumps(validate_plan(), indent=2))
    return 0


def command_boot(args: argparse.Namespace) -> int:
    plan = validate_plan()
    args.out.mkdir(parents=True, exist_ok=True)
    runner = args.out / "verdant_mgba_runner"
    runner_metadata = build_runner(runner, args.mgba_prefix)
    build_metadata = force_build_rom(args.jobs)
    rom = Path(build_metadata["rom"])
    elf = Path(build_metadata["elf"])
    addresses = symbols(elf, {"gMain", "gSaveBlock1Ptr", "gSaveBlock2Ptr"})
    runs = [
        run_boot_once(runner, rom, addresses, args.out, run_id, args.frames, args.rtc)
        for run_id in (1, 2)
    ]
    deterministic_fields = ("pc", "video_hash", "nonzero_pixels", "named_reads")
    mismatches = {
        field: [runs[0][field], runs[1][field]]
        for field in deterministic_fields
        if runs[0][field] != runs[1][field]
    }
    if mismatches:
        raise RuntimeError(f"fixed-input boot is nondeterministic: {mismatches}")

    manifest = {
        "schema_version": 1,
        "repo": str(ROOT),
        "git_head": run(["git", "rev-parse", "HEAD"]).stdout.strip(),
        "dirty_paths": run(["git", "status", "--short"]).stdout.splitlines(),
        "runner": runner_metadata,
        "build": build_metadata,
        "plan": plan,
        "rtc": args.rtc,
        "frames": args.frames,
        "runs": runs,
        "deterministic": True,
    }
    manifest_path = args.out / "boot-results.json"
    manifest_path.write_text(json.dumps(manifest, indent=2, sort_keys=True) + "\n")
    print(json.dumps({
        "status": "pass",
        "rom_sha256": build_metadata["rom_sha256"],
        "video_hash": runs[0]["video_hash"],
        "callback2": f"0x{runs[0]['named_reads']['callback2']:08x}",
        "save_bytes": runs[0]["save_bytes"],
        "manifest": str(manifest_path),
    }, indent=2))
    return 0


def parser() -> argparse.ArgumentParser:
    result = argparse.ArgumentParser(description=__doc__)
    subparsers = result.add_subparsers(dest="command", required=True)

    build = subparsers.add_parser("build-runner", help="compile the mGBA host runner")
    build.add_argument("--output", type=Path, required=True)
    build.add_argument("--mgba-prefix", type=Path)
    build.set_defaults(func=command_build_runner)

    plan = subparsers.add_parser("plan", help="validate and summarize the fixture plan")
    plan.set_defaults(func=command_plan)

    boot = subparsers.add_parser("boot", help="force-build and boot production twice")
    boot.add_argument("--out", type=Path, required=True)
    boot.add_argument("--mgba-prefix", type=Path)
    boot.add_argument("--jobs", type=int, default=max(1, min(8, os.cpu_count() or 1)))
    boot.add_argument("--frames", type=int, default=900)
    boot.add_argument("--rtc", type=int, default=946684800)
    boot.set_defaults(func=command_boot)
    return result


def main() -> int:
    args = parser().parse_args()
    try:
        return args.func(args)
    except (OSError, RuntimeError, subprocess.TimeoutExpired) as error:
        print(f"runtime harness: FAIL: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
