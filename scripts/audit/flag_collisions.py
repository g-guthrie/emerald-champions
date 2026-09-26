#!/usr/bin/env python3
"""Release gate: no two live flag names may resolve to the same save bit.

A flag name is live when something outside include/constants/flags.h (map
data, scripts, C source, tests) references it. Two live names on one bit means
one event silently toggles another (the September 23, 2026 hidden-item
collisions set story flags when items were picked up). Aliases, names whose
definition is exactly another flag name, are the same flag by design and are
exempt. The Kanto header and generated events.inc are not sources of truth
and are ignored.
"""
from __future__ import annotations

import collections
import os
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HEADER = ROOT / "include/constants/flags.h"
SOURCE_DIRS = ("data", "src", "include", "test")
SOURCE_SUFFIXES = (".inc", ".s", ".json", ".c", ".h", ".txt")
IGNORED_FILES = ("flags.h", "events.inc")
CONSTS = {"MAX_TRAINERS_COUNT": 0x360}


def load_defs(text: str) -> "collections.OrderedDict[str, str]":
    defs = collections.OrderedDict()
    pattern = re.compile(r"^#define\s+((?:FLAG_|TEMP_FLAGS_START|TRAINER_FLAGS|SYSTEM_FLAGS|DAILY_FLAGS)\w*)\s+(.+?)\s*(?://.*)?$", re.M)
    for match in pattern.finditer(text):
        defs.setdefault(match.group(1), match.group(2).strip())
    return defs


def resolve(defs):
    cache = {}

    def val(name, seen=()):
        if name in cache:
            return cache[name]
        if name in seen:
            return None
        expr = defs.get(name)
        if expr is None:
            return CONSTS.get(name)
        expanded = re.sub(r"\b([A-Z_][A-Z0-9_]*)\b",
                          lambda m: str(val(m.group(1), seen + (name,))) if (m.group(1) in defs or m.group(1) in CONSTS) else m.group(1),
                          expr)
        try:
            value = eval(expanded)  # arithmetic on integers only
        except Exception:
            value = None
        cache[name] = value
        return value

    return {name: val(name) for name in defs if name.startswith("FLAG_")}


def live_names() -> dict[str, set[str]]:
    refs: dict[str, set[str]] = collections.defaultdict(set)
    for directory in SOURCE_DIRS:
        for dirpath, _, filenames in os.walk(ROOT / directory):
            for filename in filenames:
                if not filename.endswith(SOURCE_SUFFIXES) or filename in IGNORED_FILES:
                    continue
                path = Path(dirpath) / filename
                try:
                    text = path.read_text(errors="ignore")
                except OSError:
                    continue
                for name in set(re.findall(r"FLAG_[A-Z0-9_x]+", text)):
                    refs[name].add(str(path.relative_to(ROOT)))
    return refs


def find_collisions():
    text = HEADER.read_text()
    defs = load_defs(text)
    values = resolve(defs)
    refs = live_names()
    aliases = {name for name, expr in defs.items() if re.fullmatch(r"FLAG_\w+", expr)}
    by_value = collections.defaultdict(list)
    for name, value in values.items():
        if value is not None:
            by_value[value].append(name)
    collisions = []
    for value in sorted(by_value):
        if value == 0:
            continue  # Kanto placeholders resolve to 0 and are never reached
        live = [n for n in by_value[value] if n in refs and n not in aliases]
        if len(live) > 1:
            collisions.append((value, live, {n: sorted(refs[n])[:3] for n in live}))
    return collisions


def main() -> int:
    collisions = find_collisions()
    if not collisions:
        print("PASS: every live flag name owns its own save bit")
        return 0
    print(f"FAIL: {len(collisions)} save bits are shared by live flag names")
    for value, names, where in collisions:
        print(f"  {value:#x}: " + " | ".join(f"{n} <{', '.join(where[n])}>" for n in names))
    return 1


if __name__ == "__main__":
    sys.exit(main())
