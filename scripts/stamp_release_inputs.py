#!/usr/bin/env python3
"""Bind declared source/generator inputs and release ROM/ELF artifact bytes.

The release verifier used to prove freshness by comparing file modification
times.  That check is fooled by the normal Docker workflow: the container copy
of the tree can be older than the host tree while the ROM it produced is newer
than every host file.  This stamp is content based.  Run it in the same tree
that ``make release`` compiled (inside the builder container, immediately after
the build) and copy the stamp out beside ``pokeemerald-release.gba``.

    python3 scripts/stamp_release_inputs.py            # write the stamp
    python3 scripts/stamp_release_inputs.py --check    # compare tree to stamp

The runtime-gate runner writes the same digest to ``pokeemerald-test.inputs.json``
after building the shared test ELF and requires it in ``--run-only`` mode, so the
curated runtime suite is tied to the same sources as the release ROM. This is
an integrity record, not a substitute for a successful build: it cannot prove
that stale cached objects were recompiled or authenticate the build toolchain.
"""

from __future__ import annotations

import argparse
import fnmatch
import hashlib
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
STAMP = ROOT / "pokeemerald-release.inputs.json"
INPUT_DIRS = ("src", "data", "include", "asm", "graphics", "sound", "libagbsyscall")
INPUT_FILES = {"Makefile", "config.mk", "make_tools.mk", "charmap.txt", "check_history.sh",
               ".gitignore"}
INPUT_SUFFIXES = {".mk", ".ld"}
SCHEMA_VERSION = 2
# These JSON files are consumed by make_teachables.py during the ROM build.
# Authored trainer/preset documents are checked against their materialized C
# inputs by separate gates; they are not substitutes for those compiled inputs.
GENERATOR_INPUTS = (
    "data/emerald_champions/emerald_champions_move_access_review.json",
    "data/emerald_champions/emerald_champions_preparation_form_learnsets.json",
    "scripts/stamp_release_inputs.py",
)
GENERATOR_DIRS = ("learnset_helpers", "wild_encounters", "misc")
TOOL_SOURCE_SUFFIXES = {".c", ".h", ".cpp", ".hpp", ".py", ".sh", ".pl", ".mk", ".json", ".txt", ".s", ".S", ".inc"}


def ignore_rules() -> list[tuple[str, str, bool]]:
    """Parse every .gitignore under the build inputs into (scope, pattern, negated).

    The builder container has no .git directory, so this mirrors git's ignore
    rules directly instead of shelling out.  Only the pattern forms the
    repository actually uses (suffix globs, path globs, directories, negations,
    nested per-directory files) are supported; the same code runs on both
    sides, so the digests agree.
    """
    rules: list[tuple[str, str, bool]] = []
    files = [ROOT / ".gitignore"]
    for directory in INPUT_DIRS:
        files.extend(sorted((ROOT / directory).rglob(".gitignore")))
    for ignore_file in files:
        if not ignore_file.is_file():
            continue
        scope = ignore_file.parent.relative_to(ROOT).as_posix()
        scope = "" if scope == "." else scope + "/"
        for raw in ignore_file.read_text().splitlines():
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            negated = line.startswith("!")
            rules.append((scope, line[1:] if negated else line, negated))
    return rules


def is_ignored(relative: str, rules: list[tuple[str, str, bool]]) -> bool:
    ignored = False
    for scope, pattern, negated in rules:
        if scope and not relative.startswith(scope):
            continue
        local = relative[len(scope):]
        name = local.rsplit("/", 1)[-1]
        parts = local.split("/")
        anchored = pattern.startswith("/")
        pat = pattern.lstrip("/")
        directory = pat.endswith("/")
        pat = pat.rstrip("/")
        if "/" in pat:
            if pat.startswith("**/"):
                tail = pat[3:]
                candidates = ["/".join(parts[i:]) for i in range(len(parts))]
                hit = any(fnmatch.fnmatchcase(c, tail) for c in candidates)
            else:
                hit = fnmatch.fnmatchcase(local, pat) or (directory and local.startswith(pat + "/"))
                if not hit and "**" in pat:
                    hit = fnmatch.fnmatchcase(local, pat.replace("**/", "*/").replace("**", "*"))
        elif directory:
            hit = pat in parts[:-1]
        else:
            hit = fnmatch.fnmatchcase(name, pat) or (anchored and fnmatch.fnmatchcase(local, pat))
        if hit:
            ignored = not negated
    return ignored


def build_inputs(*, include_tests: bool = False) -> list[Path]:
    rules = ignore_rules()
    paths: list[Path] = []
    input_dirs = INPUT_DIRS + (("test",) if include_tests else ())
    for directory in input_dirs:
        if not (ROOT / directory).is_dir():
            raise ValueError(f"missing required source directory: {directory}")
        for path in (ROOT / directory).rglob("*"):
            if path.is_file() and not path.name.startswith("._"):
                paths.append(path)
    for path in ROOT.iterdir():
        if path.is_file() and (path.name in INPUT_FILES or path.suffix in INPUT_SUFFIXES):
            paths.append(path)
    if include_tests:
        # This script determines the exact source manifest compiled into the
        # shared test ELF, so changing it invalidates that ELF even when no C
        # source changed.
        paths.append(ROOT / "scripts/run_emerald_champions_runtime_gates.py")
    paths = [
        p for p in paths
        if p.is_file() and not p.name.startswith("._")
        and not is_ignored(p.relative_to(ROOT).as_posix(), rules)
    ]
    # Native generators may live in ignored directories (compresSmol), so
    # enumerate their source independently of the game asset ignore rules.
    make_tools = (ROOT / "make_tools.mk").read_text()
    tool_names = re.search(r"(?m)^TOOL_NAMES\s*:?=\s*(.+)$", make_tools)
    if tool_names is None:
        raise ValueError("make_tools.mk lacks the native generator list")
    names = [*tool_names.group(1).split(), *GENERATOR_DIRS]
    if include_tests:
        check_tools = re.search(r"(?m)^CHECK_TOOL_NAMES\s*:?=\s*(.+)$", make_tools)
        if check_tools is None:
            raise ValueError("make_tools.mk lacks the test harness tool list")
        names.extend(check_tools.group(1).split())
    for name in names:
        directory = ROOT / "tools" / name
        if not directory.is_dir():
            raise ValueError(f"missing build generator source: {directory}")
        for path in directory.rglob("*"):
            relative_parts = path.relative_to(directory).parts
            if any(part in {"build", "__pycache__", ".git"} for part in relative_parts):
                continue
            if path.is_file() and (path.name == "Makefile" or path.suffix in TOOL_SOURCE_SUFFIXES):
                paths.append(path)
    for relative in GENERATOR_INPUTS:
        path = ROOT / relative
        if not path.is_file():
            raise ValueError(f"missing build input: {relative}")
        paths.append(path)
    return sorted(set(paths))


def digest_tree(*, include_tests: bool = False) -> tuple[str, int]:
    tree = hashlib.sha256()
    count = 0
    for path in build_inputs(include_tests=include_tests):
        relative = path.relative_to(ROOT).as_posix()
        tree.update(relative.encode())
        tree.update(b"\0")
        tree.update(hashlib.sha256(path.read_bytes()).digest())
        count += 1
    return tree.hexdigest(), count


def artifacts_for_stamp(stamp_path: Path) -> tuple[Path, ...]:
    if not stamp_path.name.endswith(".inputs.json"):
        raise ValueError("stamp filename must end in .inputs.json")
    stem = stamp_path.name.removesuffix(".inputs.json")
    suffixes = (".elf",) if stem.startswith("pokeemerald-test") else (".gba", ".elf")
    return tuple(stamp_path.with_name(stem + suffix) for suffix in suffixes)


def verify_stamp(stamp: dict, digest: str, count: int, artifacts: tuple[Path, ...]) -> None:
    if not isinstance(stamp, dict) or stamp.get("schema_version") != SCHEMA_VERSION:
        raise ValueError("unsupported input stamp schema; rebuild and stamp the artifacts")
    if stamp.get("inputs_sha256") != digest or stamp.get("input_count") != count:
        raise ValueError("input stamp does not match this source tree")
    hashes = stamp.get("artifacts")
    if not isinstance(hashes, dict) or set(hashes) != {path.name for path in artifacts}:
        raise ValueError("stamp must bind every expected artifact by filename and SHA-256")
    for artifact in artifacts:
        expected = hashes[artifact.name]
        if not isinstance(expected, str) or re.fullmatch(r"[0-9a-f]{64}", expected) is None:
            raise ValueError(f"invalid artifact SHA-256: {artifact.name}")
        if not artifact.is_file():
            raise ValueError(f"missing stamped artifact: {artifact}")
        if hashlib.sha256(artifact.read_bytes()).hexdigest() != expected:
            raise ValueError(f"artifact bytes differ from stamp: {artifact.name}")


def newest_input(paths: list[Path]) -> Path:
    return max(paths, key=lambda path: path.stat().st_mtime)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true")
    parser.add_argument(
        "--stamp",
        type=Path,
        default=STAMP,
        help="stamp file to write or check (default: the release ROM stamp)",
    )
    args = parser.parse_args()
    stamp_path = args.stamp if args.stamp.is_absolute() else ROOT / args.stamp
    artifacts = artifacts_for_stamp(stamp_path)
    include_tests = stamp_path.name.startswith("pokeemerald-test")
    inputs = build_inputs(include_tests=include_tests)
    digest, count = digest_tree(include_tests=include_tests)
    if args.check:
        if not stamp_path.is_file():
            print(f"missing input stamp: {stamp_path.name}; run scripts/stamp_release_inputs.py in the built tree")
            return 1
        verify_stamp(json.loads(stamp_path.read_text()), digest, count, artifacts)
        print(f"PASS: {stamp_path.name} matches {count} build inputs and artifact bytes: "
              + ", ".join(path.name for path in artifacts))
        return 0
    newest = newest_input(inputs)
    artifact_hashes = {}
    for artifact in artifacts:
        if not artifact.is_file():
            raise ValueError(f"refusing to stamp missing artifact: {artifact.name}")
        if artifact.stat().st_mtime < newest.stat().st_mtime:
            raise ValueError(f"refusing to stamp: {artifact.name} is older than build input "
                             f"{newest.relative_to(ROOT).as_posix()}")
        artifact_hashes[artifact.name] = hashlib.sha256(artifact.read_bytes()).hexdigest()
    stamp_path.write_text(json.dumps({
        "schema_version": SCHEMA_VERSION,
        "inputs_sha256": digest,
        "input_count": count,
        "artifacts": artifact_hashes,
    }, indent=2) + "\n")
    print(f"stamped {count} build inputs: {digest[:12]} (binds " + ", ".join(artifact_hashes) + ")")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (ValueError, OSError) as error:
        sys.exit(str(error))
