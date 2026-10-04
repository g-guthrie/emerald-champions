#!/usr/bin/env python3
"""Rebuild the Hard retune ledger from the per-battle directories.

Every agent writes only inside its own work/retune/NN-TRAINER_X/ directory
(draft.json, a party file with "cap" and "plan", from the ledger drafter; party.json/design.md from prep or
battle agents, receipt.json when benchmarked). This script reads them all,
in play order from work/retune/order.json, and writes work/retune/LEDGER.md.
It ends with a running tally of the species in each cap window's finished
benchmark parties, as information for balance review (it changes nothing).

  python3 scripts/retune_ledger.py            # write LEDGER.md and print the summary
  python3 scripts/retune_ledger.py --cap 14   # only that cap window
"""
from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
RETUNE = ROOT / "work/retune"


def _load(path: Path):
    try:
        return json.loads(path.read_text())
    except (OSError, ValueError):
        return None


def _team(party: dict | None) -> str:
    if not party:
        return ""
    members = party.get("party", [])
    return ", ".join(
        m.get("species", "?").removeprefix("SPECIES_").title()
        + (f" @{m['item'].removeprefix('ITEM_').replace('_', ' ').title()}" if m.get("item") not in (None, "ITEM_NONE") else "")
        for m in members)


def battle_dirs() -> dict[int, list[Path]]:
    found: dict[int, list[Path]] = {}
    for path in sorted(RETUNE.glob("[0-9][0-9]*-TRAINER_*")):
        match = re.match(r"(\d+)-", path.name)
        if path.is_dir() and match:
            found.setdefault(int(match.group(1)), []).append(path)
    return found


def rows(cap_filter: int | None = None) -> list[dict]:
    order = _load(RETUNE / "order.json") or []
    dirs = battle_dirs()
    out = []
    for entry in order:
        n = entry["n"]
        paths = dirs.get(n, [])
        if not paths:
            out.append(dict(n=n, trainer=entry["trainers"][0], status="todo", cap=None, team="", d_star="", confirmed="", flag=""))
            continue
        for path in paths:
            receipt = _load(path / "receipt.json")
            party = _load(path / "party.json")
            draft = _load(path / "draft.json")
            if receipt:
                status = "done"
            elif any(p.is_dir() for p in path.glob("d*-s*")):
                status = "benchmarking"
            elif party:
                status = "prepped"
            elif draft:
                status = "drafted"
            else:
                status = "started"
            cap = (receipt or {}).get("cap") or (draft or {}).get("cap")
            out.append(dict(
                n=n, trainer=path.name.split("-", 1)[1], status=status, cap=cap,
                team=_team(party or draft),
                d_star="" if not receipt else f"{receipt.get('d_star'):+d}" if isinstance(receipt.get("d_star"), int) else str(receipt.get("d_star")),
                confirmed=(receipt or {}).get("confirmed", ""),
                flag=(receipt or {}).get("flag") or "",
            ))
    if cap_filter is not None:
        out = [r for r in out if r["cap"] in (None, cap_filter)]
    return out


def usage(cap_filter: int | None = None) -> dict[int, tuple[int, list[tuple[str, int]]]]:
    """cap -> (finished battles, [(species, battles using it)]) from receipts' parties."""
    by_cap: dict[int, dict[str, int]] = {}
    battles: dict[int, int] = {}
    for paths in battle_dirs().values():
        for path in paths:
            receipt = _load(path / "receipt.json")
            if not receipt or (cap_filter is not None and receipt.get("cap") != cap_filter):
                continue
            named = receipt.get("party")
            party = _load(ROOT / named if named and not Path(named).is_absolute() else Path(named)) if named else None
            party = party or _load(path / "party.json")
            if not party:
                continue
            cap = receipt.get("cap")
            battles[cap] = battles.get(cap, 0) + 1
            counts = by_cap.setdefault(cap, {})
            for species in {m.get("species", "?") for m in party.get("party", [])}:
                counts[species] = counts.get(species, 0) + 1
    return {cap: (battles[cap], sorted(counts.items(), key=lambda kv: (-kv[1], kv[0])))
            for cap, counts in sorted(by_cap.items())}


def render_usage(tally: dict[int, tuple[int, list[tuple[str, int]]]]) -> list[str]:
    lines = ["", "## Species in finished benchmark parties", "",
             "Running tally per cap window, for balance review only."]
    for cap, (total, counts) in tally.items():
        shown = ", ".join(f"{s.removeprefix('SPECIES_').replace('_', ' ').title()} {n}/{total}" for s, n in counts)
        lines += ["", f"- Cap {cap} ({total} battles): {shown}"]
    return lines


def render(table: list[dict], tally: dict | None = None) -> str:
    counts: dict[str, int] = {}
    for r in table:
        counts[r["status"]] = counts.get(r["status"], 0) + 1
    lines = ["# Hard retune ledger", "",
             "Generated by scripts/retune_ledger.py from work/retune/NN-*/. " +
             ", ".join(f"{k}: {v}" for k, v in sorted(counts.items())), "",
             "| # | Trainer | Cap | Status | Hard D* | Confirmed | Flag | Team |",
             "|---|---|---|---|---|---|---|---|"]
    for r in table:
        if r["status"] == "todo":
            continue
        lines.append(f"| {r['n']} | {r['trainer'].removeprefix('TRAINER_')} | {r['cap'] or ''} | {r['status']} | "
                     f"{r['d_star']} | {r['confirmed']} | {r['flag']} | {r['team']} |")
    if tally:
        lines += render_usage(tally)
    return "\n".join(lines) + "\n"


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--cap", type=int)
    args = parser.parse_args(argv)
    table = rows(args.cap)
    text = render(table, usage(args.cap))
    (RETUNE / "LEDGER.md").write_text(text)
    print(text.splitlines()[2])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
