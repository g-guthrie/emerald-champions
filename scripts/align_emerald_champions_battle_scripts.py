#!/usr/bin/env python3
"""Keep campaign trainer script presentation aligned with the battle master."""

from __future__ import annotations

import argparse
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MASTER = ROOT / "data/emerald_champions/emerald_champions_master_battle_design.txt"
NEED_TWO = "EmeraldChampions_Text_NeedTwoPokemon"
BRANCH_RE = re.compile(
    r"--- BRANCH (TRAINER_[A-Z0-9_]+) ---.*?^format: (single|double|multi)$",
    re.M | re.S,
)
CALL_RE = re.compile(
    r"^(?P<indent>\s*)(?P<macro>trainerbattle_[a-z0-9_]+)\s+"
    r"(?P<args>[^@\n]*?)(?P<trailing>\s*(?:@.*)?)$",
    re.M,
)


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def formats() -> dict[str, str]:
    result = dict(BRANCH_RE.findall(MASTER.read_text()))
    require(result, "campaign master has no trainer branches")
    return result


def source_paths() -> list[Path]:
    return sorted((ROOT / "data" / "maps").glob("*/scripts.inc")) + sorted(
        (ROOT / "data" / "scripts").glob("*.inc")
    )


def rewrite_call(match: re.Match[str], authored: dict[str, str]) -> str:
    macro = match.group("macro")
    args = [part.strip() for part in match.group("args").split(",")]
    if not args:
        return match.group(0)
    trainer = args[0]
    fmt = authored.get(trainer)
    if fmt not in {"double", "multi"}:
        return match.group(0)

    if macro == "trainerbattle_single":
        require(len(args) in {3, 4, 5}, f"unexpected trainerbattle_single arguments for {trainer}: {args}")
        macro = "trainerbattle_double"
        args.insert(3, NEED_TWO)
    elif macro == "trainerbattle_no_intro":
        require(len(args) == 2, f"unexpected trainerbattle_no_intro arguments for {trainer}: {args}")
        macro = "trainerbattle_no_intro_double"
        args.append(NEED_TWO)
    elif macro == "trainerbattle_rematch":
        require(len(args) == 3, f"unexpected trainerbattle_rematch arguments for {trainer}: {args}")
        macro = "trainerbattle_rematch_double"
        args.append(NEED_TWO)
    else:
        return match.group(0)

    return f'{match.group("indent")}{macro} {", ".join(args)}{match.group("trailing")}'


def aligned_source(path: Path, authored: dict[str, str]) -> str:
    return CALL_RE.sub(lambda match: rewrite_call(match, authored), path.read_text())


def mismatches(authored: dict[str, str]) -> list[str]:
    errors: list[str] = []
    seen: set[str] = set()
    for path in source_paths():
        for line_number, line in enumerate(path.read_text().splitlines(), 1):
            lavaridge = re.search(r"\b(trainerbattle_lavaridge(?:_double)?)\s+[^,]+,\s*(TRAINER_[A-Z0-9_]+)", line)
            if lavaridge is not None:
                trainer = lavaridge.group(2)
                if trainer in authored:
                    seen.add(trainer)
                    if authored[trainer] == "single" and "double" in lavaridge.group(1):
                        errors.append(f"{path.relative_to(ROOT)}:{line_number}: {trainer} is single but uses {lavaridge.group(1)}")
                    if authored[trainer] in {"double", "multi"} and "double" not in lavaridge.group(1):
                        errors.append(f"{path.relative_to(ROOT)}:{line_number}: {trainer} is {authored[trainer]} but uses {lavaridge.group(1)}")
                continue

            paired = re.search(
                r"\b(trainerbattle_double_two_trainers|multi_2_vs_2)\s+"
                r"(TRAINER_[A-Z0-9_]+),[^,]+,\s*(TRAINER_[A-Z0-9_]+)",
                line,
            )
            if paired is not None:
                for trainer in paired.groups()[1:]:
                    if trainer not in authored:
                        continue
                    seen.add(trainer)
                    if authored[trainer] == "single":
                        errors.append(f"{path.relative_to(ROOT)}:{line_number}: {trainer} is single but uses {paired.group(1)}")
                continue

            match = re.search(r"\b(trainerbattle_[a-z0-9_]+)\s+(TRAINER_[A-Z0-9_]+)", line)
            if match is None or match.group(2) not in authored:
                continue
            macro, trainer = match.groups()
            seen.add(trainer)
            fmt = authored[trainer]
            if fmt in {"double", "multi"} and "double" not in macro and macro != "trainerbattle_two_trainers":
                errors.append(f"{path.relative_to(ROOT)}:{line_number}: {trainer} is {fmt} but uses {macro}")
            if fmt == "single" and "double" in macro:
                errors.append(f"{path.relative_to(ROOT)}:{line_number}: {trainer} is single but uses {macro}")
    missing = sorted(set(authored) - seen)
    if missing:
        errors.append("master trainers without a campaign invocation: " + ",".join(missing))
    return errors


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    args = parser.parse_args()
    authored = formats()

    if args.write:
        changed = 0
        for path in source_paths():
            original = path.read_text()
            updated = aligned_source(path, authored)
            if updated != original:
                path.write_text(updated)
                changed += 1
        print(f"updated_script_files={changed}")

    require(NEED_TWO in (ROOT / "data/scripts/emerald_champions.inc").read_text(), "shared doubles guard text is missing")
    require("trainerbattle_no_intro_double" in (ROOT / "asm/macros/event.inc").read_text(), "no-intro doubles macro is missing")
    errors = mismatches(authored)
    require(not errors, "\n".join(errors))
    print(f"PASS: all {len(authored)} campaign trainer branches use their authored battle format")


if __name__ == "__main__":
    main()
