#!/usr/bin/env python3
"""Overlay hand-authored team blocks onto docs/emerald_champions_battle_teams.txt by trainer id."""
import re, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
TEAMS = ROOT / "docs" / "emerald_champions_battle_teams.txt"
HEAD = re.compile(r"^## E(\d{4}) (TRAINER_[A-Z0-9_]+)")
def blocks(text):
    out, cur = {}, None
    order = []
    for line in text.splitlines():
        m = HEAD.match(line)
        if m:
            cur = m.group(2); out[cur] = [line]; order.append(cur)
        elif cur is not None and not line.startswith("# ----"):
            out[cur].append(line)
        elif cur is not None and line.startswith("# ----"):
            cur = None
    return out, order
src = TEAMS.read_text()
new_blocks = {}
for path in sys.argv[1:]:
    b, _ = blocks(Path(path).read_text())
    new_blocks.update(b)
lines = src.splitlines(); out = []; i = 0; replaced = 0
while i < len(lines):
    m = HEAD.match(lines[i])
    if m and m.group(2) in new_blocks:
        j = i + 1
        while j < len(lines) and not HEAD.match(lines[j]) and not lines[j].startswith("# ----"):
            j += 1
        block = [l for l in new_blocks[m.group(2)] if l.strip()]
        out.extend(block); i = j; replaced += 1
    else:
        out.append(lines[i]); i += 1
missing = sorted(set(new_blocks) - {HEAD.match(l).group(2) for l in lines if HEAD.match(l)})
if missing:
    raise SystemExit(f"unknown trainers in overlay: {missing}")
TEAMS.write_text("\n".join(out) + "\n")
print(f"overlaid {replaced} blocks")
