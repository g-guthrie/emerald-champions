#!/usr/bin/env python3
"""Record a content digest of every ROM build input next to the release ROM.

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
curated runtime suite is tied to the same sources as the release ROM.
"""

from __future__ import annotations

import argparse
import fnmatch
import hashlib
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
STAMP = ROOT / "pokeemerald-release.inputs.json"
INPUT_DIRS = ("src", "data", "include", "asm", "graphics", "sound", "libagbsyscall")
INPUT_FILES = {"Makefile", "config.mk", "make_tools.mk", "charmap.txt"}
INPUT_SUFFIXES = {".mk", ".ld"}


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


def build_inputs() -> list[Path]:
    rules = ignore_rules()
    paths: list[Path] = []
    for directory in INPUT_DIRS:
        for path in (ROOT / directory).rglob("*"):
            if path.is_file() and not path.name.startswith("._"):
                paths.append(path)
    for path in ROOT.iterdir():
        if path.is_file() and (path.name in INPUT_FILES or path.suffix in INPUT_SUFFIXES):
            paths.append(path)
    return sorted(
        p for p in paths
        if p.is_file() and not p.name.startswith("._")
        and not is_ignored(p.relative_to(ROOT).as_posix(), rules)
    )


def digest_tree() -> tuple[str, int]:
    tree = hashlib.sha256()
    count = 0
    for path in build_inputs():
        relative = path.relative_to(ROOT).as_posix()
        tree.update(relative.encode())
        tree.update(b"\0")
        tree.update(hashlib.sha256(path.read_bytes()).digest())
        count += 1
    return tree.hexdigest(), count


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
    digest, count = digest_tree()
    if args.check:
        if not stamp_path.is_file():
            print(f"missing input stamp: {stamp_path.name}; run scripts/stamp_release_inputs.py in the built tree")
            return 1
        stamp = json.loads(stamp_path.read_text())
        if stamp.get("inputs_sha256") != digest:
            print(
                f"{stamp_path.name} was built from different sources than this tree: "
                f"stamp={stamp.get('inputs_sha256', '?')[:12]} tree={digest[:12]} "
                f"(stamp files={stamp.get('input_count')}, tree files={count})"
            )
            return 1
        print(f"PASS: {stamp_path.name} matches {count} build inputs by content")
        return 0
    stamp_path.write_text(json.dumps({"inputs_sha256": digest, "input_count": count}, indent=2) + "\n")
    print(f"stamped {count} build inputs: {digest[:12]}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
