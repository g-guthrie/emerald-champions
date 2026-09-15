#!/usr/bin/env python3
"""Materialize authored trainer teams into master, AI plans and native parties.

Edit data/emerald_champions/emerald_champions_battle_teams.txt for exact loadouts,
per-member live-cap offsets, strategy, tactics and intended plan/counterplay.
The short Game Book is design guidance, not parsed build input.
Use --write after authoring and --check for materialized agreement and configured
ability validation. Neither check certifies battle quality or route access.
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

import ec_moves
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


def retain_authored_encounters(master_text: str, branches: list[Branch]) -> str:
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
        raise SystemExit(f"authored trainers need explicit native encounter metadata: {sorted(wanted - found)}")
    prefix = set_field(prefix, "rematch_free_physical_encounter_groups", str(len(output)))
    prefix = set_field(prefix, "rematch_free_explicit_trainer_branch_blocks", str(len(branches)))
    prefix = set_field(prefix, "included_content", f"{len(output)} active authored encounter groups; excluded branches remain retired")
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


def check_move_legality(branches: list[Branch]) -> tuple[list[str], list[str]]:
    """Gate 1: every member's moves must be pinned-legal (ec_moves.pinned_legal_moves)
    or, failing that, present in the ROM's own learnset data. An unresolvable
    species (no Showdown id and no reviewed extension) is reported as its own
    violation instead of silently reading as zero legal moves."""
    violations: list[str] = []
    notes: list[str] = []
    for branch in branches:
        tag = f"E{branch.encounter:04d} {branch.trainer}"
        for mon in branch.mons:
            species = f"SPECIES_{mon.species}"
            if not ec_moves.species_is_resolvable(species):
                violations.append(
                    f"{tag} {mon.species}: cannot resolve a Showdown id for move-legality "
                    f"(extend SHOWDOWN_FORM_SUFFIXES/SHOWDOWN_ID_OVERRIDES in scripts/ec_moves.py)"
                )
                continue
            legal, rom_only = ec_moves.legal_moves_with_rom_union(species)
            for move in mon.moves:
                token = f"MOVE_{move}"
                if token not in legal:
                    violations.append(f"{tag} {mon.species}: {move} is not pinned-legal or ROM-learnable")
                elif token in rom_only:
                    notes.append(
                        f"# note: {tag} {mon.species} {move} allowed via ROM learnset "
                        f"(src/data/pokemon/all_learnables.json); not in the Showdown pin"
                    )
    return violations, notes


WEATHER_SETTERS = {
    "RAIN": ({"DRIZZLE", "PRIMORDIAL_SEA"}, {"RAIN_DANCE"}),
    "SUN": ({"DROUGHT", "DESOLATE_LAND", "ORICHALCUM_PULSE"}, {"SUNNY_DAY"}),
    "SAND": ({"SAND_STREAM", "SAND_SPIT"}, {"SANDSTORM"}),
    "SNOW": ({"SNOW_WARNING"}, {"SNOWSCAPE", "HAIL", "CHILLY_RECEPTION"}),
}
REDIRECTION_MOVES = {"FOLLOW_ME", "RAGE_POWDER", "ALLY_SWITCH"}
REDIRECTION_ABILITIES = {"STORM_DRAIN", "LIGHTNING_ROD"}
ACTIVATE_EXEMPT_ABILITIES = {
    "WATER_ABSORB", "STORM_DRAIN", "DRY_SKIN", "VOLT_ABSORB", "LIGHTNING_ROD",
    "MOTOR_DRIVE", "FLASH_FIRE", "SAP_SIPPER", "LEVITATE", "STEAM_ENGINE",
}


def check_strategy_coherence(branches: list[Branch]) -> list[str]:
    """Gate 2: authored strategy flags and ACTIVATE tactics must actually be
    executable/sane given the party's moves, abilities and typing."""
    move_types = ec_moves.move_types()
    move_categories = ec_moves.move_categories()
    chart = ec_moves.type_chart()
    violations: list[str] = []
    # Two-owner battles (one E group, two trainer blocks) share the field: a
    # weather set by the partner's branch satisfies this branch's flag.
    group_mons: dict[int, list] = {}
    for branch in branches:
        group_mons.setdefault(branch.encounter, []).extend(branch.mons)
    for branch in branches:
        tag = f"E{branch.encounter:04d} {branch.trainer}"
        flags = set(branch.strategy)
        field_mons = group_mons[branch.encounter]
        if "TRICK_ROOM" in flags and not any("TRICK_ROOM" in mon.moves for mon in branch.mons):
            violations.append(f"{tag}: strategy TRICK_ROOM flagged but no member knows Trick Room")
        weather_present = [flag for flag in ("RAIN", "SUN", "SAND", "SNOW") if flag in flags]
        for flag in weather_present:
            weather_abilities, weather_moves = WEATHER_SETTERS[flag]
            if not any(
                mon.ability in weather_abilities or any(move in weather_moves for move in mon.moves)
                for mon in field_mons
            ):
                violations.append(f"{tag}: strategy {flag} flagged but no member has a matching weather ability/move")
        if len(weather_present) > 1 and not re.search(r"(manual|replace)", branch.plan, re.I):
            violations.append(
                f"{tag}: mutually exclusive weather flags {sorted(weather_present)} "
                f"without 'manual'/'replace' named in the plan"
            )
        if "REDIRECTION" in flags and not any(
            mon.ability in REDIRECTION_ABILITIES or any(move in REDIRECTION_MOVES for move in mon.moves)
            for mon in branch.mons
        ):
            violations.append(
                f"{tag}: strategy REDIRECTION flagged but no member has Follow Me/Rage Powder/"
                f"Ally Switch/Storm Drain/Lightning Rod"
            )
        if "PERISH_TRAP" in flags and not any(
            mon.ability == "PERISH_BODY" or "PERISH_SONG" in mon.moves for mon in branch.mons
        ):
            violations.append(f"{tag}: strategy PERISH_TRAP flagged but no member knows Perish Song / has Perish Body")
        if "TAILWIND" in flags and not any("TAILWIND" in mon.moves for mon in branch.mons):
            violations.append(f"{tag}: strategy TAILWIND flagged but no member knows Tailwind")

        for kind, actor, move, recipient in branch.tactics:
            if kind != "ACTIVATE" or move == "NONE":
                continue
            move_token = f"MOVE_{move}"
            if move_categories.get(move_token) == "STATUS":
                continue
            move_type = move_types.get(move_token)
            recipient_mon = next((mon for mon in branch.mons if mon.species == recipient), None)
            if move_type is None or recipient_mon is None:
                continue
            defender_types = ec_moves.TYPES.get(f"SPECIES_{recipient_mon.species}")
            if not defender_types:
                continue
            multiplier = 1.0
            for defender_type in defender_types:
                multiplier *= chart.get(move_type, {}).get(defender_type, 1.0)
            if multiplier <= 1.0:
                continue
            exempt = recipient_mon.ability in ACTIVATE_EXEMPT_ABILITIES or recipient_mon.item == "WEAKNESS_POLICY"
            if not exempt:
                violations.append(
                    f"{tag}: tactic ACTIVATE {actor} {move} {recipient}: {move} is super effective "
                    f"({multiplier:g}x) against {recipient}'s typing and its ability/item "
                    f"({recipient_mon.ability}/{recipient_mon.item}) does not absorb it"
                )
    return violations


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
    args = parser.parse_args()

    branches = read_teams()
    master_source = retain_authored_encounters(MASTER.read_text(), branches)
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
        MASTER.write_text(master_text)
        PLANS.write_text(render_plans(branches))
        run([sys.executable, "scripts/implement_emerald_champions_master_battles.py"])
        print(f"wrote {MASTER} and src/data/trainers.party")
        return

    if args.check:
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

        move_violations, move_notes = check_move_legality(branches)
        if move_violations:
            for violation in move_violations:
                print(f"FAIL: move legality: {violation}")
            FAILED_GATES.append("move-legality")
        else:
            print("PASS: move legality: every member's moves are pinned-legal or ROM-learnable")
        for note in move_notes:
            print(note)

        strategy_violations = check_strategy_coherence(branches)
        if strategy_violations:
            for violation in strategy_violations:
                print(f"FAIL: strategy coherence: {violation}")
            FAILED_GATES.append("strategy-coherence")
        else:
            print("PASS: strategy coherence: authored strategy flags and ACTIVATE tactics check out")

        if FAILED_GATES:
            raise SystemExit(f"gates failed: {FAILED_GATES}")
        print("PASS: generated master/plans/party match authored teams; configured trainer abilities are valid")
        return

    parser.print_help()


if __name__ == "__main__":
    main()
