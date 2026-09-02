#!/usr/bin/env python3
"""Compile the hand-authored campaign teams file into the battle master.

``docs/emerald_champions_battle_teams.txt`` is the human-authored source of
truth for every campaign trainer team.  Each block is one trainer branch:

    ## E0002 TRAINER_CALVIN_1 class=regular
    plan: what the team is trying to do
    crack: how the player is meant to beat it
    ZORUA @EXPERT_BELT ILLUSION TIMID SS -1 | DARK_PULSE, EXTRASENSORY, SUCKER_PUNCH, PROTECT

Species, items, abilities, natures and moves are written without their
``SPECIES_``/``ITEM_``/``ABILITY_``/``NATURE_``/``MOVE_`` prefixes.  Stat
Points accept either six slash-separated values or one of the spreads in
``POINT_SPREADS``.  The level column is the offset from the encounter's strict
level cap and is applied verbatim: difficulty comes from levels and team size,
never from a hidden tier nudge.

The battle class decides the AI profile, the fatigue role and the difficulty
band recorded in the master.  ``--write`` rewrites the master document's team
and design fields in place while preserving every other encounter field
(ids, chronology, location, caps, dialogue status).  ``--check`` compiles to a
scratch master and runs the static audit, the trainer implementation and the
runtime-coherence and Ability-legality gates against scratch output.
"""

from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
import tempfile
from collections import Counter
from dataclasses import dataclass, field
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TEAMS = ROOT / "docs" / "emerald_champions_battle_teams.txt"
MASTER = ROOT / "docs" / "emerald_champions_master_battle_design.txt"
ENCOUNTER_RE = re.compile(r"(?m)^=== ENCOUNTER (\d{4}) ===$")
BRANCH_RE = re.compile(r"(?m)^--- BRANCH ([A-Z0-9_]+) ---$")
HEADER_RE = re.compile(r"^## E(\d{4}) (TRAINER_[A-Z0-9_]+)(?:\s+class=([a-z_]+))?\s*$")
MON_RE = re.compile(
    r"^([A-Z0-9_]+)\s+@([A-Z0-9_]+)\s+([A-Z0-9_]+)\s+([A-Z]+)\s+([A-Z0-9/]+)\s+([+-]?\d+)\s*\|\s*(.+)$"
)

POINT_SPREADS = {
    # fast physical / fast special sweepers
    "PS": "2/32/0/0/0/32",
    "SS": "2/0/0/32/0/32",
    # bulky attackers
    "PB": "32/32/2/0/0/0",
    "SB": "32/0/2/32/0/0",
    # walls: physical, special, mixed
    "WD": "32/0/32/0/2/0",
    "WS": "32/0/2/0/32/0",
    "WM": "32/0/16/0/18/0",
    # bulky speed control / support that still needs to move
    "FS": "32/0/2/0/0/32",
    # mixed attackers
    "MX": "2/32/0/32/0/0",
    "MB": "32/16/0/16/0/2",
}

# class -> (difficulty_target, fatigue_role, ai_profile)
CLASSES = {
    "casual": (6.5, "ordinary_breather", "sharp"),
    "regular": (7.5, "ordinary_standard", "sharp"),
    "grunt": (7.8, "ordinary_standard", "sharp"),
    "gym": (7.8, "ordinary_standard", "sharp"),
    "ace": (8.6, "notable_optional_or_route_ace", "sharp"),
    "brain": (9.4, "notable_optional_or_route_ace", "master"),
    "admin": (9.4, "mini_boss_or_exceptional_trainer", "master"),
    "rival": (9.4, "mini_boss_or_exceptional_trainer", "master"),
    "boss": (9.4, "notable_optional_or_route_ace", "master"),
    "leader": (10.0, "marquee_boss", "master"),
    "elite": (10.0, "marquee_boss", "master"),
}
MARQUEE_TOKENS = (
    "ROXANNE", "BRAWLY", "WATTSON", "FLANNERY", "NORMAN", "WINONA",
    "TATE_AND_LIZA", "JUAN", "SIDNEY", "PHOEBE", "GLACIA", "DRAKE",
    "WALLACE", "MAXIE", "ARCHIE", "STEVEN", "CYNTHIA",
)
REPLACED_FIELDS = (
    "difficulty_target", "fatigue_role", "theme_and_tempo", "primary_question",
    "intentional_weakness", "first_loss_lesson", "strongest_part", "weakest_link",
    "competitive_references", "reservation_status",
)


@dataclass
class Mon:
    species: str
    item: str
    ability: str
    nature: str
    points: str
    offset: int
    moves: list[str]

    def master_line(self, index: int) -> str:
        return (
            f"  {index}. SPECIES_{self.species} @ ITEM_{self.item} | level_offset={self.offset} | "
            f"ability=ABILITY_{self.ability} | nature=NATURE_{self.nature} | "
            f"stat_points={self.points} | moves=" + ",".join(f"MOVE_{move}" for move in self.moves)
        )


@dataclass
class Branch:
    encounter: int
    trainer: str
    cls: str
    plan: str
    crack: str
    mons: list[Mon] = field(default_factory=list)
    line: int = 0


def parse_points(text: str, where: str) -> str:
    if text in POINT_SPREADS:
        return POINT_SPREADS[text]
    values = text.split("/")
    if len(values) != 6 or not all(value.isdigit() for value in values):
        raise SystemExit(f"{where}: bad stat points {text!r}")
    ints = [int(value) for value in values]
    if any(value > 32 for value in ints) or sum(ints) > 66:
        raise SystemExit(f"{where}: illegal stat points {text!r}")
    return text


def read_teams(path: Path = TEAMS) -> list[Branch]:
    branches: list[Branch] = []
    current: Branch | None = None
    plans: dict[int, tuple[str, str]] = {}
    for number, raw in enumerate(path.read_text().splitlines(), 1):
        line = raw.rstrip()
        where = f"{path.name}:{number}"
        if not line or line.startswith("#!"):
            continue
        header = HEADER_RE.match(line)
        if header:
            encounter = int(header.group(1))
            cls = header.group(3) or "regular"
            if cls not in CLASSES:
                raise SystemExit(f"{where}: unknown class {cls!r}")
            current = Branch(encounter, header.group(2), cls, "", "", line=number)
            branches.append(current)
            continue
        if line.startswith("#"):
            continue
        if current is None:
            raise SystemExit(f"{where}: team line before any header")
        if line.startswith("plan:"):
            current.plan = line[5:].strip()
            continue
        if line.startswith("crack:"):
            current.crack = line[6:].strip()
            continue
        mon = MON_RE.match(line)
        if not mon:
            raise SystemExit(f"{where}: cannot parse team line {line!r}")
        moves = [move.strip() for move in mon.group(7).split(",") if move.strip()]
        if not 1 <= len(moves) <= 4:
            raise SystemExit(f"{where}: {len(moves)} moves")
        current.mons.append(Mon(
            species=mon.group(1),
            item=mon.group(2),
            ability=mon.group(3),
            nature=mon.group(4),
            points=parse_points(mon.group(5), where),
            offset=int(mon.group(6)),
            moves=moves,
        ))
    seen: set[str] = set()
    for branch in branches:
        if branch.trainer in seen:
            raise SystemExit(f"{path.name}:{branch.line}: duplicate branch {branch.trainer}")
        seen.add(branch.trainer)
        if not branch.mons:
            raise SystemExit(f"{path.name}:{branch.line}: {branch.trainer} has no Pokemon")
        if len(branch.mons) > 6:
            raise SystemExit(f"{path.name}:{branch.line}: {branch.trainer} has more than six Pokemon")
        if branch.plan and branch.crack:
            plans.setdefault(branch.encounter, (branch.plan, branch.crack))
    for branch in branches:
        if not branch.plan or not branch.crack:
            inherited = plans.get(branch.encounter)
            if inherited is None:
                raise SystemExit(f"{path.name}:{branch.line}: {branch.trainer} needs plan: and crack: lines")
            branch.plan = branch.plan or inherited[0]
            branch.crack = branch.crack or inherited[1]
    return branches


def line_value(text: str, key: str) -> str:
    match = re.search(rf"(?m)^{re.escape(key)}: (.*)$", text)
    return match.group(1) if match else ""


def set_field(block: str, key: str, value: str, after: str | None = None) -> str:
    pattern = rf"(?m)^{re.escape(key)}: .*$"
    if re.search(pattern, block):
        return re.sub(pattern, lambda _m: f"{key}: {value}", block, count=1)
    anchor = re.search(rf"(?m)^{re.escape(after)}: .*$", block) if after else None
    if anchor is None:
        raise SystemExit(f"cannot place field {key}")
    return block[:anchor.end()] + f"\n{key}: {value}" + block[anchor.end():]


def split_encounters(text: str) -> tuple[str, list[tuple[int, str]]]:
    markers = list(ENCOUNTER_RE.finditer(text))
    prefix = text[:markers[0].start()]
    blocks = []
    for index, marker in enumerate(markers):
        end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
        blocks.append((int(marker.group(1)), text[marker.start():end]))
    return prefix, blocks


def render_branch(block_branch: str, branch: Branch) -> str:
    head, _sep, _tail = block_branch.partition("team:\n")
    lines = [head + "team:"]
    lines.extend(mon.master_line(index) for index, mon in enumerate(branch.mons, 1))
    lines.append("source_note: Hand-authored Emerald Champions team; implementation must match exactly.")
    return "\n".join(lines) + "\n"


def compile_master(branches: list[Branch], master_text: str) -> str:
    by_trainer = {branch.trainer: branch for branch in branches}
    prefix, blocks = split_encounters(master_text)
    out = [prefix]
    used: set[str] = set()
    for number, block in blocks:
        marks = list(BRANCH_RE.finditer(block))
        header = block[:marks[0].start()] if marks else block
        trainers = [mark.group(1) for mark in marks]
        team = [by_trainer[trainer] for trainer in trainers if trainer in by_trainer]
        if len(team) != len(trainers):
            missing = [trainer for trainer in trainers if trainer not in by_trainer]
            raise SystemExit(f"encounter {number}: teams file has no branch for {missing}")
        used.update(trainers)
        classes = {branch.cls for branch in team}
        if len(classes) != 1:
            raise SystemExit(f"encounter {number}: branches disagree on class {sorted(classes)}")
        cls = team[0].cls
        difficulty, fatigue_role, ai_profile = CLASSES[cls]
        if any(token in trainer for trainer in trainers for token in MARQUEE_TOKENS):
            difficulty = 10.0
        lead = team[0]
        names = ", ".join(display(mon.species) for mon in lead.mons)
        header = set_field(header, "difficulty_target", f"{difficulty:.1f}")
        header = set_field(header, "fatigue_role", fatigue_role)
        header = set_field(header, "battle_class", cls, after="fatigue_role")
        header = set_field(header, "ai_profile", ai_profile, after="battle_class")
        header = set_field(header, "primary_question", f"Can the player beat this {cls} team ({names}) on its own terms: {lead.plan}")
        header = set_field(header, "theme_and_tempo", lead.plan)
        header = set_field(header, "intentional_weakness", lead.crack)
        header = set_field(header, "first_loss_lesson", lead.crack)
        header = set_field(header, "strongest_part", lead.plan)
        header = set_field(header, "weakest_link", lead.crack)
        header = set_field(header, "competitive_references", "Hand-authored 2026-09 campaign rebuild against the pinned Champions learnsets")
        header = set_field(header, "reservation_status", "hand-authored; species, Mega and legendary placement tracked in docs/emerald_champions_battle_teams.txt")
        pieces = [header]
        for index, mark in enumerate(marks):
            end = marks[index + 1].start() if index + 1 < len(marks) else len(block)
            segment = block[mark.start():end]
            trailer = ""
            if "=== END ENCOUNTER ===" in segment:
                segment, _, trailer = segment.partition("=== END ENCOUNTER ===")
                trailer = "=== END ENCOUNTER ===" + trailer
            pieces.append(render_branch(segment, by_trainer[mark.group(1)]) + trailer)
        out.append("".join(pieces))
    unused = sorted(set(by_trainer) - used)
    if unused:
        raise SystemExit(f"teams file has branches absent from the master: {unused}")
    return "".join(out)


def display(species: str) -> str:
    return species.replace("_", " ").title()


def seed_from_master(master_text: str) -> str:
    """Emit a teams file that reproduces the current master (bootstrap only)."""
    sys.path.insert(0, str(ROOT / "scripts"))
    import implement_emerald_champions_master_battles as impl  # noqa: E402

    mon_re = re.compile(
        r"(?m)^  \d+\. SPECIES_([A-Z0-9_]+) @ ITEM_([A-Z0-9_]+) \| level_offset=(-?\d+) \| "
        r"ability=ABILITY_([A-Z0-9_]+) \| nature=NATURE_([A-Z0-9_]+) \| stat_points=([0-9/]+) \| moves=([A-Z0-9_,]+)$"
    )
    tier_class = {
        impl.TIER_BREATHER: "casual", impl.TIER_STANDARD: "regular", impl.TIER_TEAM: "grunt",
        impl.TIER_GYM: "gym", impl.TIER_ACE: "ace", impl.TIER_ADMIN: "admin", impl.TIER_BOSS: "leader",
    }
    reverse_points = {value: key for key, value in POINT_SPREADS.items()}
    lines = ["#! Emerald Champions hand-authored campaign teams (see scripts/emerald_champions_teams.py)"]
    _prefix, blocks = split_encounters(master_text)
    for number, block in blocks:
        cap = int(line_value(block, "strict_cap"))
        location = line_value(block, "location")
        role = line_value(block, "fatigue_role")
        difficulty = float(line_value(block, "difficulty_target"))
        marks = list(BRANCH_RE.finditer(block))
        chapter = line_value(block, "chapter")
        lines.append("")
        lines.append(f"# ---- E{number:04d} {location} cap={cap} {chapter}")
        for index, mark in enumerate(marks):
            end = marks[index + 1].start() if index + 1 < len(marks) else len(block)
            segment = block[mark.start():end]
            trainer = mark.group(1)
            fmt = line_value(segment, "format")
            mons = mon_re.findall(segment)
            tier = impl.trainer_tier(trainer, location, role, len(mons), difficulty)
            cls = tier_class[tier]
            if tier == impl.TIER_BOSS:
                trainer_cls = impl.trainer_class(trainer)
                if trainer_cls == "Rival":
                    cls = "rival"
                elif trainer_cls in {"Arena Tycoon", "Pike Queen", "Palace Maven"}:
                    cls = "brain"
                elif trainer_cls in {"Elite Four", "Champion"}:
                    cls = "elite"
                elif not any(token in trainer for token in MARQUEE_TOKENS):
                    cls = "boss"
            lines.append(f"## E{number:04d} {trainer} class={cls}")
            lines.append(f"# {impl.trainer_class(trainer)} / {fmt}")
            if index == 0:
                lines.append("plan: " + line_value(block, "theme_and_tempo"))
                lines.append("crack: " + line_value(block, "intentional_weakness"))
            for species, item, offset, ability, nature, points, moves in mons:
                points = reverse_points.get(points, points)
                moves = ", ".join(move[5:] for move in moves.split(","))
                lines.append(f"{species} @{item} {ability} {nature} {points} {int(offset):+d} | {moves}")
    return "\n".join(lines) + "\n"


FAILED_GATES: list[str] = []


def run(command: list[str], env: dict[str, str] | None = None, fatal: bool = True) -> None:
    result = subprocess.run(command, cwd=ROOT, env={**os.environ, **(env or {})}, text=True, capture_output=True)
    sys.stdout.write(result.stdout)
    if result.returncode != 0:
        sys.stdout.write(result.stderr)
        if fatal:
            raise SystemExit(f"gate failed: {' '.join(command)}")
        FAILED_GATES.append(command[1])


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--seed", action="store_true", help="bootstrap the teams file from the current master")
    parser.add_argument("--write", action="store_true", help="rewrite the master and trainers.party")
    parser.add_argument("--check", action="store_true", help="compile to scratch and run every static gate")
    parser.add_argument("--summary", action="store_true", help="print class, legendary and Mega coverage")
    args = parser.parse_args()

    if args.seed:
        if TEAMS.exists():
            raise SystemExit(f"{TEAMS} already exists; refusing to overwrite")
        TEAMS.write_text(seed_from_master(MASTER.read_text()))
        print(f"seeded {TEAMS}")
        return

    branches = read_teams()
    master_text = compile_master(branches, MASTER.read_text())

    if args.summary:
        sys.path.insert(0, str(ROOT / "scripts"))
        import audit_emerald_champions_master_battles as audit  # noqa: E402

        species = Counter(mon.species for branch in branches for mon in branch.mons)
        items = Counter(mon.item for branch in branches for mon in branch.mons)
        classes = Counter(branch.cls for branch in branches)
        print("classes:", dict(classes))
        print("branches:", len(branches), "pokemon:", sum(species.values()), "distinct species:", len(species))
        print("top species:", species.most_common(20))
        print("top items:", items.most_common(15))
        missing_megas = sorted(stone for stone in audit.MEGA_STONES if stone[5:] not in items)
        print(f"megas used {len(audit.MEGA_STONES) - len(missing_megas)}/{len(audit.MEGA_STONES)}; missing:", missing_megas)
        used = set(species)
        missing_signs = sorted(
            sign for sign in audit.SIGN_SPECIES
            if not ({alias[8:] for alias in audit.LEGENDARY_SHOWCASE_ALIASES.get(sign, {sign})} & used)
        )
        print(f"legendary signs used {len(audit.SIGN_SPECIES) - len(missing_signs)}/{len(audit.SIGN_SPECIES)}; missing:", missing_signs)
        return

    if args.write:
        MASTER.write_text(master_text)
        run([sys.executable, "scripts/implement_emerald_champions_master_battles.py", "--through-encounter", "513"])
        print(f"wrote {MASTER} and src/data/trainers.party")
        return

    if args.check:
        with tempfile.TemporaryDirectory() as scratch:
            scratch_master = Path(scratch) / "master.txt"
            scratch_party = Path(scratch) / "trainers.party"
            scratch_master.write_text(master_text)
            run([
                sys.executable, "scripts/implement_emerald_champions_master_battles.py",
                "--through-encounter", "513", "--master", str(scratch_master), "--output", str(scratch_party),
            ])
            run([sys.executable, "scripts/audit_emerald_champions_master_battles.py", str(scratch_master)],
                {"EC_TRAINERS_PARTY": str(scratch_party)}, fatal=False)
            run([sys.executable, "scripts/verify_trainer_runtime_coherence.py"],
                {"EC_TRAINERS_PARTY": str(scratch_party)}, fatal=False)
            run([sys.executable, "scripts/verify_trainer_ability_legality.py"],
                {"EC_TRAINERS_PARTY": str(scratch_party)}, fatal=False)
        if FAILED_GATES:
            raise SystemExit(f"gates failed: {FAILED_GATES}")
        print("PASS: teams compile and every static trainer gate passes on scratch output")
        return

    parser.print_help()


if __name__ == "__main__":
    main()
