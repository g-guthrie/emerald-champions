#!/usr/bin/env python3
"""Generate the exhaustive plain-text evidence appendix for the campaign book.

The reader-facing campaign book explains the route through Hoenn.  This file
keeps that narrative honest by materializing every finite physical trainer
encounter, its implemented teams, its source dialogue, the badge/cap ladder,
and every Legendary Sign acquisition definition from the current source tree.
"""

from __future__ import annotations

import hashlib
import argparse
import re
import sys
from collections import Counter, OrderedDict
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MASTER = ROOT / "data/emerald_champions/emerald_champions_master_battle_design.txt"
OUTPUT = ROOT / "work" / "exports" / "EMERALD_CHAMPIONS_CAMPAIGN_EVIDENCE.txt"


@dataclass(frozen=True)
class SourceBlock:
    path: Path
    line: int
    lines: tuple[str, ...]


@dataclass(frozen=True)
class TrainerOccurrence:
    path: Path
    line: int
    script: str
    macro: str
    line_labels: tuple[str, ...]
    block_msgboxes: tuple[str, ...]


def field(block: str, name: str) -> str:
    match = re.search(rf"(?m)^{re.escape(name)}: (.*)$", block)
    return match.group(1).strip() if match else ""


def display(token: str) -> str:
    for prefix in ("TRAINER_", "SPECIES_", "ITEM_", "ABILITY_", "MOVE_", "FLAG_", "MAP_"):
        if token.startswith(prefix):
            token = token[len(prefix):]
            break
    special = {
        "HO_OH": "Ho-Oh",
        "TYPE_NULL": "Type: Null",
        "MR_MIME": "Mr. Mime",
        "MIME_JR": "Mime Jr.",
        "PORYGON_Z": "Porygon-Z",
        "TAPU_KOKO": "Tapu Koko",
        "TAPU_LELE": "Tapu Lele",
        "TAPU_BULU": "Tapu Bulu",
        "TAPU_FINI": "Tapu Fini",
    }
    return special.get(token, token.replace("_", " ").title())


def campaign_source_paths() -> list[Path]:
    """Canonical authored sources read by the evidence generator.

    Map builds create connections/events/header `.inc` files. Those outputs do
    not contain campaign dialogue and must not make the fingerprint depend on
    whether the checkout has already been built.
    """
    paths = list((ROOT / "data/maps").glob("*/scripts.inc"))
    paths += list((ROOT / "data/scripts").rglob("*.inc"))
    paths += list((ROOT / "data/text").rglob("*.inc"))
    paths += list((ROOT / "data").glob("*.s"))
    paths += list((ROOT / "asm").rglob("*.s"))
    return paths


def campaign_input_snapshot() -> str:
    """Hash every canonical source file that can change this appendix."""
    paths = [Path(__file__), MASTER, ROOT / "src/data/trainers.party", ROOT / "src/data/pokemon/legendary_signs.h"]
    paths += campaign_source_paths()

    digest = hashlib.sha256()
    for path in sorted(set(paths), key=lambda value: str(value.relative_to(ROOT))):
        relative = str(path.relative_to(ROOT)).encode()
        digest.update(relative)
        digest.update(b"\0")
        digest.update(path.read_bytes())
        digest.update(b"\0")
    return digest.hexdigest()


def source_blocks() -> dict[str, SourceBlock]:
    labels: dict[str, SourceBlock] = {}
    for path in sorted(campaign_source_paths()):
        lines = path.read_text(errors="ignore").splitlines()
        starts: list[tuple[int, str]] = []
        for index, line in enumerate(lines):
            match = re.match(r"^\s*([A-Za-z_]\w*)::?\s*$", line)
            if match:
                starts.append((index, match.group(1)))
        for ordinal, (index, name) in enumerate(starts):
            end = starts[ordinal + 1][0] if ordinal + 1 < len(starts) else len(lines)
            labels.setdefault(name, SourceBlock(path, index + 1, tuple(lines[index + 1:end])))
    return labels


def clean_text(label: str, labels: dict[str, SourceBlock]) -> str | None:
    block = labels.get(label)
    if block is None:
        return None
    pieces = []
    for line in block.lines:
        match = re.search(r'\.string\s+"(.*)"', line)
        if match:
            pieces.append(match.group(1).rstrip("$"))
    if not pieces:
        return None
    text = "".join(pieces)
    text = text.replace(r"\p", "\n\n").replace(r"\n", "\n").replace(r"\l", "\n")
    text = re.sub(r"\{([A-Z0-9_]+)\}", r"<\1>", text)
    return text.strip()


def trainer_occurrences(labels: dict[str, SourceBlock]) -> dict[str, list[TrainerOccurrence]]:
    result: dict[str, list[TrainerOccurrence]] = {}
    for script, block in labels.items():
        if "_Frlg" in str(block.path):
            continue
        for offset, raw_line in enumerate(block.lines, 1):
            line = raw_line.split("@", 1)[0].strip()
            if not line:
                continue
            command = line.split(None, 1)[0]
            if not (command.startswith("trainerbattle_") or command == "multi_2_vs_2"):
                continue
            trainers = re.findall(r"\bTRAINER_[A-Z0-9_]+\b", line)
            text_labels = tuple(re.findall(r"\b[A-Za-z_]\w*_Text_[A-Za-z0-9_]+\b", line))
            msgboxes = tuple(
                match.group(1)
                for later in block.lines[offset:]
                if (match := re.search(r"\bmsgbox\s+([A-Za-z_]\w*)", later))
            )
            for trainer in trainers:
                result.setdefault(trainer, []).append(
                    TrainerOccurrence(
                        path=block.path,
                        line=block.line + offset,
                        script=script,
                        macro=command,
                        line_labels=text_labels,
                        block_msgboxes=msgboxes,
                    )
                )
    return result


def trainer_party_blocks() -> dict[str, str]:
    text = (ROOT / "src/data/trainers.party").read_text()
    markers = list(re.finditer(r"(?m)^=== (TRAINER_[A-Z0-9_]+) ===$", text))
    result = {}
    for index, marker in enumerate(markers):
        end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
        result[marker.group(1)] = text[marker.end():end].strip()
    return result


def encounters() -> list[dict[str, object]]:
    text = MASTER.read_text()
    rows = []
    for number, block in re.findall(
        r"=== ENCOUNTER (\d{4}) ===\n(.*?)=== END ENCOUNTER ===", text, re.S
    ):
        branches = []
        for trainer_id, body in re.findall(
            r"--- BRANCH (TRAINER_[A-Z0-9_]+) ---\n(.*?)(?=--- BRANCH |source_note:|\Z)",
            block,
            re.S,
        ):
            mons = []
            for mon in re.finditer(
                r"(?m)^  \d+\. (SPECIES_[A-Z0-9_]+) @ (ITEM_[A-Z0-9_]+) \| "
                r"level_offset=(-?\d+) \| ability=(ABILITY_[A-Z0-9_]+) \| "
                r"nature=(NATURE_[A-Z0-9_]+) \| stat_points=([0-9/]+) \| "
                r"moves=(MOVE_[A-Z0-9_]+(?:,MOVE_[A-Z0-9_]+){0,3})$",
                body,
            ):
                mons.append(
                    {
                        "species": mon.group(1),
                        "item": mon.group(2),
                        "offset": int(mon.group(3)),
                        "ability": mon.group(4),
                        "nature": mon.group(5),
                        "stat_points": mon.group(6),
                        "moves": mon.group(7).split(","),
                    }
                )
            branches.append({"trainer_id": trainer_id, "format": field(body, "format"), "mons": mons})
        rows.append(
            {
                "number": int(number),
                "chapter": field(block, "chapter"),
                "location": field(block, "location"),
                "requirement": field(block, "requirement"),
                "cap": field(block, "strict_cap"),
                "question": field(block, "primary_question"),
                "first_loss": field(block, "first_loss_lesson"),
                "strongest": field(block, "strongest_part"),
                "weakest": field(block, "weakest_link"),
                "branches": branches,
            }
        )
    return rows


def choose_occurrence(trainer: str, location: str, occurrences: dict[str, list[TrainerOccurrence]]) -> TrainerOccurrence | None:
    choices = occurrences.get(trainer, [])
    if not choices:
        return None
    live = [row for row in choices if "rematch" not in row.macro.lower() and "Rematch" not in row.script]
    if live:
        choices = live
    location_parts = [part.strip() for part in re.split(r"[;·]", location)]
    matching = [row for row in choices if any(part and part in str(row.path) for part in location_parts)]
    return (matching or choices)[0]


def quote(text: str) -> list[str]:
    lines = []
    for paragraph in text.split("\n\n"):
        for line in paragraph.splitlines() or [""]:
            lines.append(f"      > {line}")
        lines.append("      >")
    while lines and lines[-1] == "      >":
        lines.pop()
    return lines


def trainer_meta(block: str) -> tuple[str, str]:
    name = field(block, "Name") or "UNKNOWN"
    trainer_class = field(block, "Class") or "UNKNOWN"
    return name, trainer_class


def legendary_rows() -> list[dict[str, str]]:
    path = ROOT / "src/data/pokemon/legendary_signs.h"
    rows = []
    for line_number, raw in enumerate(path.read_text().splitlines(), 1):
        line = raw.strip()
        if not line or line.startswith("//") or line.startswith("#"):
            continue
        macro = re.match(r"(WILD_SIGN|VISIBLE_SIGN|OTHER_SIGN|ORDINARY_WILD_SIGN)\((.*)\),", line)
        if not macro:
            continue
        kind, body = macro.groups()
        parts = [part.strip() for part in body.split(",")]
        row = {"kind": kind, "line": str(line_number), "id": parts[0], "species": parts[1]}
        if kind == "WILD_SIGN":
            row.update(map=parts[2], area=parts[3], odds=parts[4], badges=parts[5], offset=parts[6], required=parts[7], flag=parts[8])
        elif kind == "VISIBLE_SIGN":
            row.update(map=parts[2], badges=parts[3], offset=parts[4], required=parts[5], flag=parts[6])
        elif kind == "ORDINARY_WILD_SIGN":
            row.update(map=parts[2], badges="0", offset="0", required="NONE", flag="none")
        else:
            row.update(source=parts[2], map="n/a", badges="varies", offset="varies", required="NONE", flag="none")
        rows.append(row)
    return rows


def write_report(check: bool = False) -> None:
    labels = source_blocks()
    occurrences = trainer_occurrences(labels)
    parties = trainer_party_blocks()
    # The battle master is already validated as contiguous canonical campaign
    # order. Preserve it exactly instead of maintaining a second chronology.
    rows = encounters()
    input_hash = campaign_input_snapshot()

    output: list[str] = [
        "EMERALD CHAMPIONS — EXHAUSTIVE CAMPAIGN EVIDENCE",
        "=" * 57,
        "",
        f"Campaign input SHA-256: {input_hash}",
        f"Physical encounter groups: {len(rows)}",
        f"Trainer branches: {sum(len(row['branches']) for row in rows)}",
        "",
        "This appendix is generated from the current trainer master, materialized",
        "trainer party data, live Hoenn event scripts, and Legendary Sign table.",
        "It deliberately excludes rematch-only dialogue from the primary campaign",
        "path, while retaining each first campaign encounter's implemented text.",
        "",
        "LEVEL AND DIFFICULTY CONTRACT",
        "-----------------------------",
        "Before Stone Badge: cap 14",
        "After Stone Badge: cap 20",
        "After Knuckle Badge: cap 30",
        "After Dynamo Badge: cap 40",
        "After Heat Badge: cap 45",
        "After Balance Badge: cap 55",
        "After Feather Badge: cap 60",
        "After Mind Badge: cap 70",
        "After Rain Badge: cap 80",
        "After becoming Champion: cap 100",
        "Hard uses authored levels; Medium subtracts 2; Easy subtracts 4.",
        "",
        "PHYSICAL TRAINER ATLAS",
        "----------------------",
    ]

    missing_occurrences = []
    dialogue_coverage = Counter()
    for encounter in rows:
        output += [
            "",
            f"[{int(encounter['number']):04d}] {encounter['location']} — {encounter['requirement']}",
            f"  Chapter: {encounter['chapter']}",
            f"  Level cap: {encounter['cap']}",
            f"  Battle question: {encounter['question']}",
            f"  First-loss lesson: {encounter['first_loss']}",
            f"  Strongest feature: {encounter['strongest']}",
            f"  Weakest seam: {encounter['weakest']}",
        ]
        seen_dialogue: set[str] = set()
        for branch in encounter["branches"]:
            trainer_id = str(branch["trainer_id"])
            name, trainer_class = trainer_meta(parties.get(trainer_id, ""))
            output.append(f"  Branch: {trainer_class} {name} ({trainer_id}) — {branch['format']}")
            for mon in branch["mons"]:
                moves = ", ".join(display(move) for move in mon["moves"])
                output.append(
                    "    - "
                    f"{display(mon['species'])} @ {display(mon['item'])}; offset {mon['offset']:+d}; "
                    f"{display(mon['ability'])}; {display(mon['nature'])}; Stat Points {mon['stat_points']}; {moves}"
                )
            occurrence = choose_occurrence(trainer_id, str(encounter["location"]), occurrences)
            if occurrence is None:
                missing_occurrences.append(trainer_id)
                output.append("    Dialogue source: NOT FOUND")
                continue
            output.append(
                f"    Dialogue source: {occurrence.path.relative_to(ROOT)}:{occurrence.line} ({occurrence.script})"
            )
            candidates = list(occurrence.line_labels) + list(occurrence.block_msgboxes)
            emitted = 0
            duplicate_only = False
            for label in candidates:
                if label == "EmeraldChampions_Text_NeedTwoPokemon":
                    continue
                text = clean_text(label, labels)
                if not text:
                    continue
                if label in seen_dialogue:
                    duplicate_only = True
                    continue
                seen_dialogue.add(label)
                emitted += 1
                dialogue_coverage["labels"] += 1
                output.append(f"    {label}:")
                output.extend(quote(text))
            if emitted:
                dialogue_coverage["branches_with_dialogue"] += 1
            elif duplicate_only:
                output.append("    Dialogue: identical to the preceding branch in this encounter; not repeated.")
            else:
                output.append("    Dialogue: battle is introduced by the surrounding story scene; no standalone literal is attached to this branch.")

    signs = legendary_rows()
    output += [
        "",
        "",
        "LEGENDARY ACQUISITION DEFINITIONS",
        "---------------------------------",
        f"Definitions: {len(signs)}",
        "",
    ]
    for row in signs:
        if row["kind"] == "WILD_SIGN":
            detail = (
                f"conditional wild at {display(row['map'])}; method {row['area']}; odds parameter {row['odds']}; "
                f"minimum badges {row['badges']}; level offset {row['offset']}; requires {display(row['required'])} and {row['flag']}"
            )
        elif row["kind"] == "VISIBLE_SIGN":
            detail = (
                f"visible one-off at {display(row['map'])}; minimum badges {row['badges']}; level offset {row['offset']}; "
                f"requires {display(row['required'])} and {row['flag']}"
            )
        elif row["kind"] == "ORDINARY_WILD_SIGN":
            detail = f"ordinary battle-ready wild encounter at {display(row['map'])}"
        else:
            detail = f"non-map source {display(row['source'])}"
        output.append(f"- {display(row['species'])}: {detail} [src/data/pokemon/legendary_signs.h:{row['line']}]")

    output += [
        "",
        "",
        "GENERATOR COVERAGE",
        "------------------",
        f"Encounter groups emitted: {len(rows)}",
        f"Trainer branches emitted: {sum(len(row['branches']) for row in rows)}",
        f"Unique dialogue labels emitted: {dialogue_coverage['labels']}",
        f"Branches with at least one resolved literal: {dialogue_coverage['branches_with_dialogue']}",
        f"Trainer IDs without a live script occurrence: {len(set(missing_occurrences))}",
    ]
    if missing_occurrences:
        output.append("Missing IDs: " + ", ".join(sorted(set(missing_occurrences))))
    else:
        output.append("Missing IDs: none")

    report = "\n".join(output) + "\n"
    if check:
        if not OUTPUT.exists() or OUTPUT.read_text() != report:
            raise SystemExit(
                "FAIL: campaign evidence is stale; run "
                "python3 scripts/generate_emerald_champions_campaign_evidence.py"
            )
        print("PASS: campaign evidence matches its complete source snapshot")
        return

    OUTPUT.write_text(report)
    print(f"wrote {OUTPUT.relative_to(ROOT)}")
    print(f"encounters={len(rows)} branches={sum(len(row['branches']) for row in rows)}")
    print(f"legendary_definitions={len(signs)} missing_trainer_occurrences={len(set(missing_occurrences))}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--write", action="store_true")
    mode.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if args.write:
        OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    write_report(check=args.check)
