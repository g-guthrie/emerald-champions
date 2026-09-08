#!/usr/bin/env python3
"""Write an atomic configuration prerequisite only when its identity changes.

This describes build inputs, not successful compilation; make still owns the
object/archive/link dependency graph and failed-target cleanup.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import shlex
import shutil
import tempfile
import time


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def tool_identity(command: str) -> dict:
    words = shlex.split(command)
    executable = shutil.which(words[0]) if words else None
    if executable is None:
        raise ValueError(f"build tool is unavailable: {command}")
    path = Path(executable).resolve()
    return {"command": command, "path": str(path), "sha256": digest(path)}


def library_inputs(flags: str, directory: Path) -> list[Path]:
    """Resolve the explicit static-library search used by the GBA link rules."""
    words = shlex.split(flags)
    search = []
    names = []
    files = []
    index = 0
    while index < len(words):
        word = words[index]
        if word in ("-L", "-l"):
            index += 1
            if index == len(words):
                raise ValueError(f"missing argument after {word}")
            value = words[index]
            (search if word == "-L" else names).append(value)
        elif word.startswith("-L"):
            search.append(word[2:])
        elif word.startswith("-l"):
            names.append(word[2:])
        elif word.endswith((".a", ".so")):
            files.append(directory / word)
        index += 1
    for name in names:
        filename = name[1:] if name.startswith(":") else f"lib{name}.a"
        found = next((directory / prefix / filename for prefix in search if (directory / prefix / filename).is_file()), None)
        if found is None:
            raise ValueError(f"cannot resolve linked library {name}; provide its explicit -L directory")
        files.append(found)
    return files


def write_config(path: Path, payload: dict) -> bool:
    data = (json.dumps(payload, indent=2, sort_keys=True) + "\n").encode()
    if path.is_file() and path.read_bytes() == data:
        return False
    # GNU make 3.81 compares whole seconds. A changed prerequisite must be
    # strictly newer than outputs that may have been built just before this
    # call. No-op receipts do not wait or change their timestamp.
    now = time.time()
    time.sleep(int(now) + 1 - now)
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(dir=path.parent, prefix=path.name + ".", delete=False) as stream:
            temporary = Path(stream.name)
            stream.write(data)
            stream.flush()
            os.fsync(stream.fileno())
        temporary.replace(path)
    finally:
        if temporary is not None:
            temporary.unlink(missing_ok=True)
    return True


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output", type=Path)
    parser.add_argument("--value", action="append", default=[])
    parser.add_argument("--tool", action="append", default=[])
    parser.add_argument("--file", action="append", type=Path, default=[])
    parser.add_argument("--env", action="append", default=[])
    parser.add_argument("--libraries", default="")
    parser.add_argument("--library-directory", type=Path, default=Path.cwd())
    args = parser.parse_args()
    files = [*args.file, *library_inputs(args.libraries, args.library_directory)]
    payload = {
        "schema_version": 1,
        "receipt_generator": digest(Path(__file__)),
        "values": args.value,
        "tools": [tool_identity(command) for command in args.tool],
        "files": {str(path.resolve()): digest(path) for path in files},
        "environment": {name: os.environ.get(name, "") for name in args.env},
    }
    write_config(args.output, payload)


if __name__ == "__main__":
    main()
