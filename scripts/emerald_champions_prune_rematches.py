#!/usr/bin/env python3
"""Create the rematch-free Emerald Champions campaign design source.

The preserved master intentionally indexed every Match Call tier. Emerald
Champions now treats the Battle Frontier as its repeatable endgame, so this
script keeps each first campaign encounter and removes later trainer IDs from
the pinned rematch table. Rival/story branches are unaffected unless their ID
is explicitly a later Match Call tier.
"""

from __future__ import annotations

import argparse
import re
import subprocess
from collections import Counter
from pathlib import Path


SOURCE_COMMIT = "33202c162ebc34a1dbe2000acd26b0720baa109d"
MASTER_PATH = "docs/emerald_champions_master_battle_design.txt"
REMATCH_PATH = "src/battle_setup.c"

ENCOUNTER_RE = re.compile(r"(?m)^=== ENCOUNTER \d{4} ===$")
BRANCH_RE = re.compile(r"(?m)^--- BRANCH ([A-Z0-9_]+) ---$")
TRAINER_ID_RE = re.compile(r"(?m)^trainer_id: ([A-Z0-9_]+)$")
TRAINER_IDS_RE = re.compile(r"(?m)^trainer_ids: (.*)$")


def git_show(commit: str, path: str) -> str:
    return subprocess.run(
        ["git", "show", f"{commit}:{path}"],
        check=True,
        text=True,
        stdout=subprocess.PIPE,
    ).stdout


def parse_later_rematch_ids(source: str) -> tuple[set[str], set[str]]:
    first_ids: set[str] = set()
    later_ids: set[str] = set()
    line_re = re.compile(
        r"^\s*\[REMATCH_[A-Z0-9_]+\]\s*=\s*REMATCH\((.*?)\),?\s*$",
        re.MULTILINE,
    )
    for match in line_re.finditer(source):
        args = [part.strip() for part in match.group(1).split(",")]
        trainer_ids = [part for part in args[:-1] if part.startswith("TRAINER_")]
        if not trainer_ids:
            continue
        first = trainer_ids[0]
        first_ids.add(first)
        later_ids.update(trainer_id for trainer_id in trainer_ids[1:] if trainer_id != first)
    if not first_ids or not later_ids:
        raise RuntimeError("Could not parse the pinned rematch table")
    return first_ids, later_ids


def split_marked_chunks(text: str, marker: re.Pattern[str]) -> tuple[str, list[str]]:
    matches = list(marker.finditer(text))
    if not matches:
        return text, []
    prefix = text[: matches[0].start()]
    chunks = [
        text[match.start() : matches[index + 1].start() if index + 1 < len(matches) else len(text)]
        for index, match in enumerate(matches)
    ]
    return prefix, chunks


def filter_trainer_ids_line(prefix: str, later_ids: set[str]) -> tuple[str, int, int]:
    removed = 0
    retained = 0

    def replace(match: re.Match[str]) -> str:
        nonlocal removed, retained
        values = [value.strip() for value in match.group(1).split(";") if value.strip()]
        kept = [value for value in values if value not in later_ids]
        removed += len(values) - len(kept)
        retained += len(kept)
        return "trainer_ids: " + "; ".join(kept)

    return TRAINER_IDS_RE.sub(replace, prefix, count=1), removed, retained


def filter_encounter(block: str, later_ids: set[str]) -> tuple[str | None, int]:
    prefix, branches = split_marked_chunks(block, BRANCH_RE)
    prefix, removed_from_line, retained_from_line = filter_trainer_ids_line(prefix, later_ids)
    kept_branches: list[str] = []
    removed = removed_from_line

    for branch in branches:
        marker = BRANCH_RE.search(branch)
        trainer = TRAINER_ID_RE.search(branch)
        trainer_id = trainer.group(1) if trainer else marker.group(1)
        if trainer_id in later_ids:
            removed += 1
        else:
            kept_branches.append(branch)

    had_explicit_trainers = bool(TRAINER_IDS_RE.search(block) or branches)
    if had_explicit_trainers and retained_from_line == 0 and not kept_branches:
        return None, removed

    if removed:
        note = "rematch_policy: first_campaign_encounter_only; later Match Call tiers excluded\n"
        requirement = re.search(r"(?m)^requirement: .*?$", prefix)
        insert_at = requirement.end() + 1 if requirement else len(prefix)
        prefix = prefix[:insert_at] + note + prefix[insert_at:]

    return prefix + "".join(kept_branches), removed


def renumber(block: str, number: int) -> str:
    block = ENCOUNTER_RE.sub(f"=== ENCOUNTER {number:04d} ===", block, count=1)
    block = re.sub(r"(?m)^campaign_order: \d+$", f"campaign_order: {number}", block, count=1)
    return block


def replace_scope(header: str, blocks: list[str], excluded_ids: set[str], removed: int) -> str:
    branch_count = sum(len(BRANCH_RE.findall(block)) for block in blocks)
    trainer_ids: set[str] = set()
    statuses: Counter[str] = Counter()
    formats: Counter[str] = Counter()
    for block in blocks:
        trainer_ids.update(TRAINER_ID_RE.findall(block))
        ids_line = TRAINER_IDS_RE.search(block)
        if ids_line:
            trainer_ids.update(value.strip() for value in ids_line.group(1).split(";") if value.strip())
        status = re.search(r"(?m)^status: (.+)$", block)
        if status:
            statuses[status.group(1)] += 1
        formats.update(re.findall(r"(?m)^format: (single|double|multi)$", block))

    scope = (
        "SCOPE\n"
        f"source_checkpoint: {SOURCE_COMMIT}\n"
        f"rematch_free_physical_encounter_groups: {len(blocks)}\n"
        f"rematch_free_explicit_trainer_branch_blocks: {branch_count}\n"
        f"rematch_free_resolved_opponent_trainer_ids: {len(trainer_ids)}\n"
        f"excluded_later_match_call_trainer_ids: {len(excluded_ids)}\n"
        f"removed_branch_or_trainer_references: {removed}\n"
        f"status_counts: {dict(statuses)}\n"
        f"format_counts: {dict(formats)}\n"
        "source_inventory_note: modern map/script reachability must be regenerated before authoring closes\n\n"
        "REMATCH POLICY\n"
        "Match Call and Gym Leader escalation teams are not campaign content. Keep each first encounter,\n"
        "every genuine rival/story milestone, and reusable League teams. The Battle Frontier is the\n"
        "repeatable endgame. Do not spend design or implementation work on later rematch tiers.\n\n"
    )
    header = re.sub(r"(?s)SCOPE\n.*?\nDESIGN THESIS\n", scope + "DESIGN THESIS\n", header, count=1)
    header = header.replace(
        "AUTHORING_STATUS: ACTIVE — THIS TXT IS THE CAMPAIGN DESIGN SOURCE OF TRUTH",
        "AUTHORING_STATUS: ACTIVE — REMATCH-FREE CAMPAIGN DESIGN SOURCE OF TRUTH",
    )
    return header


def build_document(master: str, rematch_source: str) -> tuple[str, dict[str, int]]:
    _, later_ids = parse_later_rematch_ids(rematch_source)
    matches = list(ENCOUNTER_RE.finditer(master))
    if not matches:
        raise RuntimeError("No encounter blocks found")
    header = master[: matches[0].start()]
    source_blocks = [
        master[match.start() : matches[index + 1].start() if index + 1 < len(matches) else len(master)]
        for index, match in enumerate(matches)
    ]

    blocks: list[str] = []
    removed = 0
    for source_block in source_blocks:
        block, block_removed = filter_encounter(source_block, later_ids)
        removed += block_removed
        if block is not None:
            blocks.append(renumber(block, len(blocks) + 1))

    output = replace_scope(header, blocks, later_ids, removed) + "".join(blocks)
    leaked = sorted(later_ids.intersection(re.findall(r"\bTRAINER_[A-Z0-9_]+\b", output)))
    if leaked:
        raise RuntimeError(f"Later rematch IDs leaked into output: {leaked[:5]}")
    return output, {
        "source_encounters": len(source_blocks),
        "retained_encounters": len(blocks),
        "source_branches": len(BRANCH_RE.findall(master)),
        "retained_branches": len(BRANCH_RE.findall(output)),
        "later_rematch_ids": len(later_ids),
        "removed_references": removed,
    }


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--commit", default=SOURCE_COMMIT)
    parser.add_argument("--master")
    parser.add_argument("--rematch-source")
    parser.add_argument("--output", required=True)
    args = parser.parse_args()

    master = Path(args.master).read_text() if args.master else git_show(args.commit, MASTER_PATH)
    rematch_source = (
        Path(args.rematch_source).read_text()
        if args.rematch_source
        else git_show(args.commit, REMATCH_PATH)
    )
    output, metrics = build_document(master, rematch_source)
    Path(args.output).write_text(output)
    for key, value in metrics.items():
        print(f"{key}={value}")


if __name__ == "__main__":
    main()
