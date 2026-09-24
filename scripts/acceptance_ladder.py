#!/usr/bin/env python3
"""Maintain the per-encounter acceptance ladder in data/emerald_champions/trainer-review-index.json
and render the generated ACCEPTANCE LADDER book section.

This is the official maintainer of data/emerald_champions/trainer-review-index.json's
source_sha256 and per-encounter "ladder" field: run
    python3 scripts/acceptance_ladder.py --write
after any change to the battle teams file, test/tests trees, work/ benchmark
receipts or handoff/checkpoint.json, and --check to verify no drift. Nothing
else should hand-edit those two fields.

Ladder levels (each is a design-review signal, not a gameplay guarantee):
  L0 authored     -- the encounter's trainer group exists in the current
                      battle teams file with a nonempty native party. Sourced
                      from emerald_champions_teams.read_teams() (the same
                      loader scripts/verify_campaign_trainer_roster.py uses).
  L1 ai_fixtures   -- some file under test/ or tests/ mentions the trainer id
                      or one of its E-groups (plain substring search over
                      *.c/*.h/*.py/*.json/*.inc/*.s files in those trees).
  L2 benchmark     -- a work/ subdirectory names the trainer (its first
                      TRAINER_x name segment, lowercased, matches a token in
                      the directory name -- e.g. "laura" in
                      work/brawly-gym-benchmark-laura) AND that directory
                      contains a recorded clear/victory artifact: a filename
                      matching clear*receipt*, cleared*party*, cleared*.sav,
                      or *-victory.*  (modeled on
                      work/brawly-gym-benchmark-laura's 060-laura-clear-receipt.png
                      + laura-cleared-party.json, and
                      work/gym-victory-fixture's roxanne-victory.png +
                      manifest.roxanne-victory.json). This is a naming
                      heuristic: a trainer whose first name segment is generic
                      (e.g. "grunt") can over-match, and a work/ directory
                      that abbreviates or misspells the trainer's name will
                      under-match. It does not replay or verify the battle.
  L3 earned_clear  -- the trainer id appears in handoff/checkpoint.json's
                      "cleared_trainers" list (the machine-readable earned-win
                      record).
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
from pathlib import Path

import emerald_champions_teams as teams
import export_trainer_catalogue as trainers

ROOT = Path(__file__).resolve().parents[1]
REVIEW_INDEX = ROOT / "data/emerald_champions/trainer-review-index.json"
MASTER = ROOT / "data/emerald_champions/emerald_champions_master_battle_design.txt"
CHECKPOINT = ROOT / "handoff/checkpoint.json"
WORK_DIR = ROOT / "work"
TEST_DIRS = (ROOT / "test", ROOT / "tests")
TEST_SUFFIXES = {".c", ".h", ".py", ".json", ".inc", ".s"}
EVIDENCE_RE = re.compile(r"clear.*receipt|cleared.*party|cleared.*\.sav|-victory\.", re.I)

MAINTAINER_NOTE = ("Ladder fields and source_sha256 are maintained only by "
                    "scripts/acceptance_ladder.py --write; do not hand-edit them.")


def encounter_meta() -> dict[int, dict[str, str]]:
    _prefix, blocks = teams.split_encounters(MASTER.read_text())
    return {number: trainers.fields(block) for number, block in blocks}


def test_corpus() -> str:
    parts = []
    for base in TEST_DIRS:
        if not base.exists():
            continue
        for path in base.rglob("*"):
            if path.is_file() and path.suffix in TEST_SUFFIXES:
                try:
                    parts.append(path.read_text(errors="ignore"))
                except OSError:
                    continue
    return "\n".join(parts)


def benchmark_dirs() -> list[tuple[str, set[str]]]:
    result = []
    if not WORK_DIR.exists():
        return result
    for path in sorted(WORK_DIR.iterdir()):
        if not path.is_dir():
            continue
        names = [p.name for p in path.rglob("*") if p.is_file()]
        if not any(EVIDENCE_RE.search(n) for n in names):
            continue
        tokens = set(re.split(r"[^a-z0-9]+", path.name.lower()))
        result.append((path.name, tokens))
    return result


def primary_token(trainer_id: str) -> str:
    name = trainer_id[len("TRAINER_"):] if trainer_id.startswith("TRAINER_") else trainer_id
    return name.split("_")[0].lower()


def cleared_trainer_names() -> set[str]:
    if not CHECKPOINT.exists():
        return set()
    data = json.loads(CHECKPOINT.read_text())
    return {row["name"] for row in data.get("cleared_trainers", [])}


def compute_ladder() -> tuple[dict[str, dict[str, bool]], dict]:
    """Pure recompute: never touches data/emerald_champions/trainer-review-index.json."""
    branches = teams.read_teams()
    known_trainers = {b.trainer for b in branches}
    class_by_trainer = {b.trainer: b.cls for b in branches}
    index = json.loads(REVIEW_INDEX.read_text())
    corpus = test_corpus()
    dirs = benchmark_dirs()
    cleared = cleared_trainer_names()
    meta = encounter_meta()

    ladder: dict[str, dict[str, bool]] = {}
    rows = []
    for enc in index["encounters"]:
        trainer_ids = enc["trainer_ids"]
        l0 = bool(trainer_ids) and all(t in known_trainers for t in trainer_ids)
        l1 = any(t in corpus for t in trainer_ids) or any(g in corpus for g in enc["e_groups"])
        needed_tokens = {primary_token(t) for t in trainer_ids}
        l2 = any(tokens & needed_tokens for _name, tokens in dirs)
        l3 = any(t in cleared for t in trainer_ids)
        entry = dict(L0_authored=l0, L1_ai_fixtures=l1, L2_benchmark=l2, L3_earned_clear=l3)
        ladder[enc["key"]] = entry
        numbers = sorted({int(g[1:]) for g in enc["e_groups"]})
        caps = [meta[n]["strict_cap"] for n in numbers if n in meta and "strict_cap" in meta[n]]
        chapters = [meta[n]["chapter"] for n in numbers if n in meta and "chapter" in meta[n]]
        rows.append(dict(
            key=enc["key"], e_groups=enc["e_groups"], trainer_ids=trainer_ids,
            cls=class_by_trainer.get(trainer_ids[0]) if trainer_ids else None,
            first_access_cap=caps[0] if caps else None,
            chapter=chapters[0] if chapters else None,
            **entry,
        ))
    summary = dict(
        total=len(rows),
        L0_authored=sum(r["L0_authored"] for r in rows),
        L1_ai_fixtures=sum(r["L1_ai_fixtures"] for r in rows),
        L2_benchmark=sum(r["L2_benchmark"] for r in rows),
        L3_earned_clear=sum(r["L3_earned_clear"] for r in rows),
    )
    return ladder, dict(rows=rows, summary=summary)


def render_lines() -> tuple[list[str], dict]:
    _ladder, catalog = compute_ladder()
    s = catalog["summary"]
    lines = [
        "\nACCEPTANCE LADDER",
        "Per-encounter design-review evidence. L0 authored: nonempty party in the current "
        "battle teams file. L1 ai_fixtures: referenced under test/ or tests/. L2 benchmark: a "
        "named, evidenced clear/victory receipt under work/ (naming heuristic; see "
        "scripts/acceptance_ladder.py). L3 earned_clear: named in handoff/checkpoint.json's "
        "earned-save cleared_trainers list. None of these levels alone certifies a finished, "
        "balanced or fully playtested encounter.",
        f"SUMMARY: {s['total']} encounters -- L0 {s['L0_authored']}, L1 {s['L1_ai_fixtures']}, "
        f"L2 {s['L2_benchmark']}, L3 {s['L3_earned_clear']}.",
    ]
    for row in catalog["rows"]:
        flags = "".join([
            "L0" if row["L0_authored"] else "--",
            " L1" if row["L1_ai_fixtures"] else " --",
            " L2" if row["L2_benchmark"] else " --",
            " L3" if row["L3_earned_clear"] else " --",
        ])
        cap = row["first_access_cap"] if row["first_access_cap"] is not None else "?"
        lines.append(f"{'/'.join(row['e_groups'])} {','.join(row['trainer_ids'])} | "
                      f"class={row['cls']} | cap {cap} | {flags}")
    return lines, catalog


def generate(root: Path = ROOT) -> tuple[list[str], dict, set[Path]]:
    """Development report render plus a light path set for the
    top-level source hash. The work/ and test(s)/ trees that L1/L2 actually
    scan are deliberately NOT included here (hundreds of files, many binary
    screenshots/saves); scripts/acceptance_ladder.py --check is the precise
    staleness gate for those, run separately from the book hash."""
    lines, catalog = render_lines()
    paths = {Path(__file__), REVIEW_INDEX, MASTER, CHECKPOINT, teams.TEAMS}
    return lines, catalog, paths


def write_index() -> None:
    ladder, _catalog = compute_ladder()
    index = json.loads(REVIEW_INDEX.read_text())
    for enc in index["encounters"]:
        enc["ladder"] = ladder[enc["key"]]
    index["source_sha256"] = hashlib.sha256(teams.TEAMS.read_bytes()).hexdigest()
    if MAINTAINER_NOTE not in index.get("scope", ""):
        index["scope"] = index.get("scope", "").rstrip() + " " + MAINTAINER_NOTE
    REVIEW_INDEX.write_text(json.dumps(index, indent=2) + "\n")


def check_index() -> bool:
    if not REVIEW_INDEX.exists():
        print(f"FAIL: {REVIEW_INDEX.relative_to(ROOT)} is missing")
        return False
    ladder, _catalog = compute_ladder()
    index = json.loads(REVIEW_INDEX.read_text())
    teams_sha = hashlib.sha256(teams.TEAMS.read_bytes()).hexdigest()
    ok = index.get("source_sha256") == teams_sha
    if not ok:
        print("FAIL: source_sha256 is stale; run scripts/acceptance_ladder.py --write")
    for enc in index["encounters"]:
        if enc.get("ladder") != ladder.get(enc["key"]):
            ok = False
            print(f"FAIL: stale ladder for {enc['key']}")
    print("PASS: data/emerald_champions/trainer-review-index.json ladder matches a fresh recompute" if ok else
          "FAIL: data/emerald_champions/trainer-review-index.json ladder is stale")
    return ok


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--write", action="store_true")
    group.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if args.write:
        write_index()
        print("PASS: data/emerald_champions/trainer-review-index.json ladder + source_sha256 refreshed")
    else:
        if not check_index():
            raise SystemExit(1)


if __name__ == "__main__":
    main()
