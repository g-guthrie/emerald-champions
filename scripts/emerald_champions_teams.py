#!/usr/bin/env python3
"""Compile the hand-authored campaign teams file into the battle master.

``data/emerald_champions/emerald_champions_battle_teams.txt`` is the human-authored source of
truth for every campaign trainer team.  Each block is one trainer branch:

    ## E0002 TRAINER_CALVIN_1 class=regular
    strategy: SETUP
    plan: what the team is trying to do
    crack: how the player is meant to beat it
    ZORUA @EXPERT_BELT ILLUSION TIMID SS -1 | DARK_PULSE, EXTRASENSORY, SUCKER_PUNCH, PROTECT

Species, items, abilities, natures and moves are written without their
``SPECIES_``/``ITEM_``/``ABILITY_``/``NATURE_``/``MOVE_`` prefixes. EVs accept
six slash-separated values in HP/Atk/Def/SpA/SpD/Spe order or a spread in
``EV_SPREADS``. The level column is the offset from the live player cap on Normal.
TrainerMon stores it explicitly; native creation applies Easy -2 / Hard +2
before the native 1..255 representation bound, including gyms and the prepared opening rival.
The materialized absolute Level is a preview of the recorded encounter cap.

``strategy:`` supplies explicit contextual instructions to the bounded doubles
planner (comma-separated names, or NONE). ``plan:`` and ``crack:`` explain the
team but are never executable instructions. ``tactic: KIND ACTOR MOVE RECIPIENT``
names a concrete partnership to consider when selecting reserves; it never
overrides move legality, survival or the evaluated payoff. Classes retain their profile names
as aliases of the common expert profile. ``--write`` rewrites the master's team
and design fields and the trainer-indexed plan table while preserving every other encounter field
(ids, chronology, location, caps, dialogue status). ``--check`` compares the
generated master, strategy table and trainer party with their canonical inputs,
and validates configured trainer Abilities. It does not establish move legality
or strategic quality.
"""

from __future__ import annotations

import argparse
import hashlib
import os
import re
import subprocess
import sys
import tempfile
from collections import Counter
from dataclasses import dataclass, field
from pathlib import Path

from emerald_champions_evs import validate_evs

ROOT = Path(__file__).resolve().parents[1]
TEAMS = ROOT / "data/emerald_champions/emerald_champions_battle_teams.txt"
MASTER = ROOT / "data/emerald_champions/emerald_champions_master_battle_design.txt"
PLANS = ROOT / "src/data/emerald_champions_battle_plans.h"
STRATEGIES = {"TRICK_ROOM", "RAIN", "SUN", "SAND", "SNOW", "REDIRECTION", "SETUP", "TAILWIND", "ALLY_COMBO", "PERISH_TRAP", "PRESSURE", "MEGA_REVEAL"}
TACTICS = {"ACTIVATE", "AFTER_YOU", "INSTRUCT", "COMMANDER", "SUPPRESS"}
ENCOUNTER_RE = re.compile(r"(?m)^=== ENCOUNTER (\d{4}) ===$")
BRANCH_RE = re.compile(r"(?m)^--- BRANCH ([A-Z0-9_]+) ---$")
HEADER_RE = re.compile(r"^## E(\d{4}) (TRAINER_[A-Z0-9_]+)(?:\s+class=([a-z_]+))?\s*$")
MON_RE = re.compile(
    r"^([A-Z0-9_]+)\s+@([A-Z0-9_]+)\s+([A-Z0-9_]+)\s+([A-Z]+)\s+([A-Z0-9/]+)\s+([+-]?\d+)\s*\|\s*(.+)$"
)

EV_SPREADS = {
    # fast physical / fast special sweepers
    "PS": "4/252/0/0/0/252",
    "SS": "4/0/0/252/0/252",
    # bulky attackers
    "PB": "252/252/4/0/0/0",
    "SB": "252/0/4/252/0/0",
    # walls: physical, special, mixed
    "WD": "252/0/252/0/4/0",
    "WS": "252/0/4/0/252/0",
    "WM": "252/0/116/0/140/0",
    # bulky speed control / support that still needs to move
    "FS": "252/0/4/0/0/252",
    # mixed attackers
    "MX": "4/252/0/252/0/0",
    "MB": "252/124/0/124/0/8",
}

# Per-trainer AI traits.  These append to the class AI profile so a single
# trainer can be given a personality (hold an ace back, play recklessly, set up
# on turn one) without changing every trainer that shares its class.  Names are
# the human form trainerproc turns into AI_FLAG_* constants.
AI_TRAITS = {
    "Ace Pokemon", "Double Ace Pokemon", "Risky", "Conservative",
    "Force Setup First Turn", "Prefer Status Moves", "Prefer Baton Pass",
    "Prefer Highest Damage Move", "Will Suicide",
}

# Preserve the existing class-to-AI behavior without assigning difficulty grades.
CLASSES = {
    "casual": "sharp",
    "regular": "sharp",
    "grunt": "sharp",
    "gym": "sharp",
    "ace": "sharp",
    "brain": "master",
    "admin": "master",
    "rival": "master",
    "boss": "master",
    "leader": "master",
    "elite": "master",
}


@dataclass
class Mon:
    species: str
    item: str
    ability: str
    nature: str
    evs: str
    offset: int
    moves: list[str]
    ivs: str = "31/31/31/31/31/31"
    friendship: int = 255

    def master_line(self, index: int) -> str:
        return (
            f"  {index}. SPECIES_{self.species} @ ITEM_{self.item} | level_offset={self.offset} | "
            f"ability=ABILITY_{self.ability} | nature=NATURE_{self.nature} | "
            f"evs={self.evs} | moves=" + ",".join(f"MOVE_{move}" for move in self.moves)
            + f" | ivs={self.ivs} | friendship={self.friendship}"
        )


@dataclass
class Branch:
    encounter: int
    trainer: str
    cls: str
    plan: str
    crack: str
    ai: list[str] = field(default_factory=list)
    mons: list[Mon] = field(default_factory=list)
    line: int = 0
    strategy: list[str] = field(default_factory=list)
    tactics: list[tuple[str, str, str, str]] = field(default_factory=list)
    mega_slots: int | None = None


def parse_evs(text: str, where: str) -> str:
    text = EV_SPREADS.get(text, text)
    values = text.split("/")
    if len(values) != 6 or not all(value.isdigit() for value in values):
        raise SystemExit(f"{where}: bad EVs {text!r}")
    try:
        validate_evs([int(value) for value in values])
    except ValueError as error:
        raise SystemExit(f"{where}: {error}") from error
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
        if line.startswith("ai:"):
            wanted = [trait.strip() for trait in line[3:].split(",") if trait.strip()]
            unknown = [trait for trait in wanted if trait not in AI_TRAITS]
            if unknown:
                raise SystemExit(f"{where}: unknown AI trait(s) {unknown}; known: {sorted(AI_TRAITS)}")
            current.ai = wanted
            continue
        if line.startswith("strategy:"):
            current.strategy = [value.strip() for value in line[9:].split(",") if value.strip() != "NONE"]
            unknown = set(current.strategy) - STRATEGIES
            if unknown:
                raise SystemExit(f"{where}: unknown battle strategies {sorted(unknown)}")
            continue
        if line.startswith("mega_slots:"):
            slots = line.partition(":")[2].strip()
            values = [] if slots == "NONE" else [int(value.strip()) for value in slots.split(",")]
            if len(values) != len(set(values)) or any(not 1 <= value <= 6 for value in values):
                raise SystemExit(f"{where}: Mega slots must be distinct party positions 1..6")
            current.mega_slots = sum(1 << (value - 1) for value in values)
            continue
        if line.startswith("tactic:"):
            fields = line[7:].split()
            if len(fields) != 4 or fields[0] not in TACTICS:
                raise SystemExit(f"{where}: expected tactic: KIND ACTOR MOVE RECIPIENT")
            current.tactics.append(tuple(fields))
            continue
        mon = MON_RE.match(line)
        if not mon:
            raise SystemExit(f"{where}: cannot parse team line {line!r}")
        parts = mon.group(7).split("|")
        moves = [move.strip() for move in parts[0].split(",") if move.strip()]
        attributes = {}
        for extra in parts[1:]:
            key, separator, value = extra.strip().partition("=")
            if not separator or key not in {"ivs", "friendship"} or key in attributes:
                raise SystemExit(f"{where}: invalid or duplicate member attribute {extra!r}")
            attributes[key] = value
        ivs = attributes.get("ivs", "31/31/31/31/31/31")
        if len(ivs.split("/")) != 6 or any(not v.isdigit() or not 0 <= int(v) <= 31 for v in ivs.split("/")):
            raise SystemExit(f"{where}: invalid IVs {ivs!r}")
        friendship = int(attributes.get("friendship", "0" if "FRUSTRATION" in moves else "255"))
        if not 0 <= friendship <= 255:
            raise SystemExit(f"{where}: friendship must be 0..255")
        if not 1 <= len(moves) <= 4:
            raise SystemExit(f"{where}: {len(moves)} moves")
        if not -254 <= int(mon.group(6)) <= 254:
            raise SystemExit(f"{where}: native level offset must fit -254..254")
        current.mons.append(Mon(
            species=mon.group(1),
            item=mon.group(2),
            ability=mon.group(3),
            nature=mon.group(4),
            evs=parse_evs(mon.group(5), where),
            offset=int(mon.group(6)),
            moves=moves,
            ivs=ivs,
            friendship=friendship,
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
        if branch.mega_slots is not None and branch.mega_slots >> len(branch.mons):
            raise SystemExit(f"{branch.trainer}: Mega permission references an absent party slot")
        for kind, actor, move, recipient in branch.tactics:
            actors = [mon for mon in branch.mons if mon.species == actor]
            if not actors or not any(mon.species == recipient for mon in branch.mons):
                raise SystemExit(f"{branch.trainer}: tactic references an absent actor or recipient")
            if kind in {"COMMANDER", "SUPPRESS"}:
                ability = "COMMANDER" if kind == "COMMANDER" else "NEUTRALIZING_GAS"
                valid = move == "NONE" and any(mon.ability == ability for mon in actors)
            else:
                valid = any(move in mon.moves for mon in actors)
                if kind in {"AFTER_YOU", "INSTRUCT"}:
                    valid = valid and move == kind
            if not valid:
                raise SystemExit(f"{branch.trainer}: tactic actor cannot execute {kind} {move}")
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


def read_book(path: Path) -> list[Branch]:
    """Read the book's explicit U, E/B, T and executable-intent owners.

    This imports authoring into the existing team pipeline; it does not infer
    executable tactics from narrative prose or invent missing trainer metadata.
    """
    text = path.read_text()

    def section(number: int) -> str:
        start = re.search(rf"(?m)^{number}\. [A-Z][A-Z ]{{2}}", text)
        end = re.search(rf"(?m)^{number + 1}\. [A-Z][A-Z ]{{2}}", text)
        if start is None:
            raise SystemExit(f"{path}: missing book section {number}")
        return text[start.start():end.start() if end else len(text)]

    def constant(value: str) -> str:
        return re.sub(r"[^A-Z0-9]+", "_", value.upper().replace("'", "").replace("’", "")).strip("_")

    evs = dict(re.findall(r"(V\d{3})=(\d+/\d+/\d+/\d+/\d+/\d+)", section(9)))
    builds: dict[str, Mon] = {}
    for match in re.finditer(r"(?m)^(U\d{4}) (.+?) @(.+?) \| (.+?) \| (.+?) \| (V\d{3}) \| (.+)$", section(9)):
        uid, species, item, ability, nature, spread, rest = match.groups()
        fields = rest.split(" | ")
        ivs = "31/31/31/31/31/31"
        friendship = 255
        for extra in fields[1:]:
            if extra.startswith("IV "):
                ivs = extra[3:]
            elif extra.startswith("friendship "):
                friendship = int(extra[11:])
            else:
                raise SystemExit(f"{uid}: unknown book member field {extra!r}")
        if uid in builds or spread not in evs:
            raise SystemExit(f"{uid}: duplicate build or unknown EV key {spread}")
        builds[uid] = Mon(constant(species), constant(item), constant(ability), constant(nature),
                          parse_evs(evs[spread], uid), 0, [constant(move) for move in fields[0].split(", ")],
                          ivs, friendship)

    cards = {m[1]: m[2].strip() for m in re.finditer(
        r"(?ms)^(T\d{3}) — [^\n]+\n(.*?)(?=^T\d{3} — |^BUILD ROLES AND NATIVE INTERACTION OBLIGATIONS)", section(22))}
    branches: list[Branch] = []
    encounter = None
    cls = ""
    for line in section(8).splitlines():
        group = re.match(r"^E(\d{4}) [^|]+ \| ([a-z]+) \|", line)
        if group:
            encounter, cls = int(group[1]), group[2]
        party = re.match(r"^  (.+?) \[(E\d{4}-B\d{2})\]: (.+?); intent (T\d{3}); Mega-eligible slots ([^;]+)(?:; never Mega slots ([0-9,]+))?$", line)
        if party:
            name, bid, members, tid, allowed, forbidden = party.groups()
            if encounter != int(bid[1:5]) or cls not in CLASSES or tid not in cards:
                raise SystemExit(f"{bid}: missing group/class/tactical card")
            card = cards[tid]
            # A joint owner card references one authoritative joint plan.
            refs = set(re.findall(r"(?:joint (?:Meteor Falls )?plan |joint Meteor Falls plan )(T\d{3})", card))
            if any(ref not in cards for ref in refs):
                raise SystemExit(f"{bid}: missing referenced joint plan")
            paragraphs = [*[(cards[ref]) for ref in sorted(refs)], card]
            plan = " ".join(" ".join(paragraph.splitlines()) for paragraph in paragraphs)
            counter = [ln.partition(":")[2].strip() for ln in card.splitlines() if ln.lower().startswith("counterplay")]
            branch = Branch(encounter, "TRAINER_" + constant(name), cls, plan,
                            " ".join(counter) or plan)
            for member in members.split(","):
                m = re.fullmatch(r"(U\d{4})\(([+-]\d+)\)", member)
                if m is None or m[1] not in builds:
                    raise SystemExit(f"{bid}: invalid member {member!r}")
                base = builds[m[1]]
                branch.mons.append(Mon(**{**vars(base), "moves": base.moves.copy(), "offset": int(m[2])}))
            slots = [] if allowed == "none" else [int(value) for value in allowed.split(",")]
            if len(set(slots)) != len(slots) or any(not 1 <= slot <= len(branch.mons) for slot in slots):
                raise SystemExit(f"{bid}: invalid Mega slot")
            if forbidden and set(slots) & {int(value) for value in forbidden.split(",")}:
                raise SystemExit(f"{bid}: contradictory Mega permission")
            branch.mega_slots = sum(1 << (slot - 1) for slot in slots)
            branches.append(branch)
        elif line.startswith("    Existing executable intent:"):
            if not branches:
                raise SystemExit("book intent without a party")
            intent = line.split(": ", 1)[1].removesuffix(".")
            preference = re.search(r"^conditional preferences ([^;]+)", intent)
            if preference is None:
                raise SystemExit(f"{branches[-1].trainer}: missing conditional preferences")
            branches[-1].strategy = [] if preference[1] == "none" else preference[1].split(", ")
            traits = re.search(r"; trainer traits ([^;]+)", intent)
            if traits:
                branches[-1].ai = traits[1].split(", ")
            if "; partnership candidates " in intent:
                for tactic in intent.split("; partnership candidates ", 1)[1].split("; "):
                    branches[-1].tactics.append(tuple(tactic.split(" / ")))
    if not branches or len({branch.trainer for branch in branches}) != len(branches):
        raise SystemExit("book contains no parties or duplicate trainer identities")
    # Reuse the normal authoring parser's complete validation, including tactics.
    with tempfile.TemporaryDirectory() as scratch:
        target = Path(scratch) / "book-teams.txt"
        target.write_text(render_teams(branches, path))
        return read_teams(target)


def render_teams(branches: list[Branch], book: Path) -> str:
    digest = hashlib.sha256(book.read_bytes()).hexdigest()
    lines = ["#! Generated from the canonical Emerald Champions Game Book; edit the book.",
             f"#! Book SHA256: {digest}", "#! Reconcile with: emerald_champions_teams.py --book PATH --write", ""]
    for branch in branches:
        lines += [f"## E{branch.encounter:04} {branch.trainer} class={branch.cls}",
                  "strategy: " + (",".join(branch.strategy) or "NONE")]
        if branch.mega_slots is not None:
            lines.append("mega_slots: " + (",".join(str(i + 1) for i in range(6) if branch.mega_slots & (1 << i)) or "NONE"))
        if branch.ai:
            lines.append("ai: " + ", ".join(branch.ai))
        lines += ["tactic: " + " ".join(tactic) for tactic in branch.tactics]
        lines += ["plan: " + branch.plan, "crack: " + branch.crack]
        for mon in branch.mons:
            lines.append(f"{mon.species} @{mon.item} {mon.ability} {mon.nature} {mon.evs} {mon.offset} | "
                         + ", ".join(mon.moves) + f" | ivs={mon.ivs} | friendship={mon.friendship}")
        lines.append("")
    return "\n".join(lines)


def retain_book_encounters(master_text: str, branches: list[Branch]) -> str:
    """Retire absent branches without inventing locations, actors or formations."""
    wanted = {branch.trainer for branch in branches}
    prefix, blocks = split_encounters(master_text)
    output = []
    found = set()
    for number, block in blocks:
        markers = list(BRANCH_RE.finditer(block))
        kept = [marker for marker in markers if marker[1] in wanted]
        if not kept:
            continue
        header = block[:markers[0].start()]
        parts = [header]
        for index, marker in enumerate(markers):
            if marker[1] not in wanted:
                continue
            end = markers[index + 1].start() if index + 1 < len(markers) else len(block)
            parts.append(block[marker.start():end].split("=== END ENCOUNTER ===", 1)[0].rstrip() + "\n")
            found.add(marker[1])
        parts.append("=== END ENCOUNTER ===\n\n")
        output.append("".join(parts))
    if wanted != found:
        raise SystemExit(f"book trainers need explicit native encounter metadata: {sorted(wanted - found)}")
    prefix = set_field(prefix, "rematch_free_physical_encounter_groups", str(len(output)))
    prefix = set_field(prefix, "rematch_free_explicit_trainer_branch_blocks", str(len(branches)))
    prefix = set_field(prefix, "included_content", f"{len(output)} active book encounter groups; excluded branches remain retired")
    return prefix + "".join(output)


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
    head = re.sub(r"(?m)^ai_extra:.*\n", "", head)
    head = re.sub(r"(?m)^strategy:.*\n", "", head)
    head = re.sub(r"(?m)^tactic:.*\n", "", head)
    head = set_field(head, "strategy", ", ".join(branch.strategy) or "NONE", after="format")
    if branch.tactics:
        head = head.rstrip() + "\n" + "\n".join("tactic: " + " ".join(tactic) for tactic in branch.tactics) + "\n"
    if branch.ai:
        head = set_field(head, "ai_extra", ", ".join(branch.ai), after="format")
    lines = [head + "team:"]
    lines.extend(mon.master_line(index) for index, mon in enumerate(branch.mons, 1))
    lines.append("source_note: Hand-authored Emerald Champions team; implementation must match exactly.")
    return "\n".join(lines) + "\n"


def render_plans(branches: list[Branch]) -> str:
    """Compile explicit authoring, never infer a strategy from moves at runtime."""
    lines = ["// Generated by scripts/emerald_champions_teams.py --write; edit the teams source.",
             "static const u16 sEmeraldChampionsBattlePlans[TRAINERS_COUNT] =", "{"]
    for branch in branches:
        flags = " | ".join("EC_BATTLE_PLAN_" + value for value in branch.strategy) or "0"
        lines.append(f"    [{branch.trainer}] = {flags},")
    lines += ["};", "", "// Bit 7 marks an explicit policy; bits 0-5 authorize party slots.",
              "static const u8 sEmeraldChampionsMegaPermissions[TRAINERS_COUNT] =", "{"]
    for branch in branches:
        if branch.mega_slots is not None:
            lines.append(f"    [{branch.trainer}] = 0x{0x80 | branch.mega_slots:02X},")
    lines += ["};", "", "static const struct EmeraldChampionsBattleTactic sEmeraldChampionsBattleTactics[] =", "{"]
    for branch in branches:
        for kind, actor, move, recipient in branch.tactics:
            lines.append(f"    {{{branch.trainer}, SPECIES_{actor}, SPECIES_{recipient}, MOVE_{move}, EC_BATTLE_TACTIC_{kind}}},")
    return "\n".join(lines + ["};", ""])


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
        ai_profile = CLASSES[cls]
        lead = team[0]
        names = ", ".join(display(mon.species) for mon in lead.mons)
        header = set_field(header, "battle_class", cls)
        header = set_field(header, "ai_profile", ai_profile, after="battle_class")
        header = set_field(header, "primary_question", f"Can the player beat this {cls} team ({names}) on its own terms: {lead.plan}")
        header = set_field(header, "theme_and_tempo", lead.plan)
        header = set_field(header, "intentional_weakness", lead.crack)
        header = set_field(header, "first_loss_lesson", lead.crack)
        header = set_field(header, "strongest_part", lead.plan)
        header = set_field(header, "weakest_link", lead.crack)
        header = set_field(header, "competitive_references", "Hand-authored 2026-09 campaign rebuild against the pinned Champions learnsets")
        header = set_field(header, "reservation_status", "hand-authored; species, Mega and legendary placement tracked in data/emerald_champions/emerald_champions_battle_teams.txt")
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
    return "".join(out).rstrip() + "\n"


def display(species: str) -> str:
    return species.replace("_", " ").title()


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
    parser.add_argument("--write", action="store_true", help="rewrite the master and trainers.party")
    parser.add_argument("--check", action="store_true", help="compare generated master/plans/party and validate configured Abilities")
    parser.add_argument("--summary", action="store_true", help="print class, legendary and Mega coverage")
    parser.add_argument("--book", type=Path, help="import/check the canonical book's explicit party and intent records")
    args = parser.parse_args()

    branches = read_book(args.book) if args.book else read_teams()
    master_source = retain_book_encounters(MASTER.read_text(), branches) if args.book else MASTER.read_text()
    master_text = compile_master(branches, master_source)

    if args.summary:
        sys.path.insert(0, str(ROOT / "scripts"))
        import ec_moves as reference  # noqa: E402

        species = Counter(mon.species for branch in branches for mon in branch.mons)
        items = Counter(mon.item for branch in branches for mon in branch.mons)
        classes = Counter(branch.cls for branch in branches)
        print("classes:", dict(classes))
        print("branches:", len(branches), "pokemon:", sum(species.values()), "distinct species:", len(species))
        print("top species:", species.most_common(20))
        print("top items:", items.most_common(15))
        missing_megas = sorted(stone for stone in reference.MEGA_STONES if stone[5:] not in items)
        print(f"megas used {len(reference.MEGA_STONES) - len(missing_megas)}/{len(reference.MEGA_STONES)}; missing:", missing_megas)
        used = set(species)
        missing_signs = sorted(
            sign for sign in reference.SIGN_SPECIES
            if not ({alias[8:] for alias in reference.LEGENDARY_SHOWCASE_ALIASES.get(sign, {sign})} & used)
        )
        print(f"legendary signs used {len(reference.SIGN_SPECIES) - len(missing_signs)}/{len(reference.SIGN_SPECIES)}; missing:", missing_signs)
        return

    if args.write:
        if args.book:
            TEAMS.write_text(render_teams(branches, args.book))
        MASTER.write_text(master_text)
        PLANS.write_text(render_plans(branches))
        run([sys.executable, "scripts/implement_emerald_champions_master_battles.py"])
        print(f"wrote {MASTER} and src/data/trainers.party")
        return

    if args.check:
        if args.book and TEAMS.read_text() != render_teams(branches, args.book):
            raise SystemExit("authored teams differ from the canonical book; run --book PATH --write")
        if MASTER.read_text() != master_text:
            raise SystemExit("generated master differs from authored teams; run --write")
        if not PLANS.exists() or PLANS.read_text() != render_plans(branches):
            raise SystemExit("compiled battle plans differ from authored strategies; run --write")
        with tempfile.TemporaryDirectory() as scratch:
            scratch_master = Path(scratch) / "master.txt"
            scratch_party = Path(scratch) / "trainers.party"
            scratch_master.write_text(master_text)
            run([
                sys.executable, "scripts/implement_emerald_champions_master_battles.py",
                "--master", str(scratch_master), "--output", str(scratch_party),
            ])
            if scratch_party.read_text() != (ROOT / "src/data/trainers.party").read_text():
                raise SystemExit("generated trainer party differs from authored teams; run --write")
            run([sys.executable, "scripts/verify_trainer_ability_legality.py"],
                {"EC_TRAINERS_PARTY": str(scratch_party)}, fatal=False)
        run([sys.executable, "scripts/verify_campaign_trainer_roster.py"])
        if FAILED_GATES:
            raise SystemExit(f"gates failed: {FAILED_GATES}")
        print("PASS: generated master/plans/party match authored teams; configured trainer abilities are valid")
        return

    parser.print_help()


if __name__ == "__main__":
    main()
