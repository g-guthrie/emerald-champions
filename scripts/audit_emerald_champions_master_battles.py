#!/usr/bin/env python3
"""Reject an incomplete or internally inconsistent campaign battle master."""

from __future__ import annotations

import argparse
import json
import re
import statistics
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_MASTER = ROOT / "docs" / "emerald_champions_master_battle_design_v2.txt"
ENCOUNTER_RE = re.compile(r"(?m)^=== ENCOUNTER (\d{4}) ===$")
BRANCH_RE = re.compile(r"(?m)^--- BRANCH ([A-Z0-9_]+) ---$")
MON_RE = re.compile(
    r"(?m)^  (\d+)\. (SPECIES_[A-Z0-9_]+) @ (ITEM_[A-Z0-9_]+) \| "
    r"level_offset=(-?\d+) \| ability=(ABILITY_[A-Z0-9_]+) \| "
    r"nature=(NATURE_[A-Z0-9_]+) \| stat_points=([0-9/]+) \| "
    r"moves=(MOVE_[A-Z0-9_]+(?:,MOVE_[A-Z0-9_]+){0,3})$"
)

PLANNED_RESTORE_TRAINERS = {
    "TRAINER_ARCHIE_SLATEPORT",
    "TRAINER_BUFFEL",
    "TRAINER_COURTNEY_MAGMA_HIDEOUT",
    "TRAINER_COURTNEY_METEOR_FALLS",
    "TRAINER_COURTNEY_MOSSDEEP",
    "TRAINER_CYNTHIA_1",
    "TRAINER_GRETA_SLATEPORT",
    "TRAINER_GRUNT_METEOR_FALLS",
    "TRAINER_LEAF_ALTERING_CAVE",
    "TRAINER_LUCY_LAVARIDGE",
    "TRAINER_MAGIKARP_GUY",
    "TRAINER_MATT_MT_PYRE",
    "TRAINER_SPENSER_FORTREE",
    "TRAINER_WALLACE_DOUBLES_LEGENDS",
}

REMATCH_TRAINERS = {
    f"TRAINER_{leader}_{tier}"
    for leader in ("ROXANNE", "BRAWLY", "WATTSON", "FLANNERY", "NORMAN", "WINONA", "JUAN")
    for tier in range(2, 6)
}
REMATCH_TRAINERS.add("TRAINER_CYNTHIA_2")

MARQUEE_TOKENS = (
    "ROXANNE", "BRAWLY", "WATTSON", "FLANNERY", "NORMAN", "WINONA",
    "TATE_AND_LIZA", "JUAN", "SIDNEY", "PHOEBE", "GLACIA", "DRAKE",
    "WALLACE", "MAXIE", "ARCHIE", "STEVEN", "CYNTHIA",
)
MINIBOSS_TOKENS = ("TABITHA", "COURTNEY", "MATT", "SHELLY", "WALLY", "BRENDAN", "MAY_")


def constants(path: str, prefix: str) -> set[str]:
    return set(re.findall(rf"\b{prefix}[A-Z0-9_]+\b", (ROOT / path).read_text()))


SPECIES = constants("include/constants/species.h", "SPECIES_")
ITEMS = constants("include/constants/items.h", "ITEM_")
MOVES = constants("include/constants/moves.h", "MOVE_")
ABILITIES = constants("include/constants/abilities.h", "ABILITY_")
NATURES = constants("include/constants/pokemon.h", "NATURE_")
TRAINERS = constants("include/constants/opponents.h", "TRAINER_")
MOVES_BY_ID = {}
for _move in sorted(MOVES, key=lambda token: (token.count("_"), len(token)), reverse=True):
    MOVES_BY_ID.setdefault(re.sub(r"[^a-z0-9]", "", _move.removeprefix("MOVE_").lower()), _move)
MEGA_STONES = set(re.findall(
    r"ITEM_[A-Z0-9_]+",
    (ROOT / "src" / "data" / "emerald_champions_mega_stones.h").read_text(),
))
SIGN_SPECIES = {
    "SPECIES_" + species
    for species in re.findall(
        r"(?:WILD|OTHER)_SIGN\([^,]+,\s*([A-Z0-9_]+)",
        (ROOT / "src" / "data" / "pokemon" / "legendary_signs.h").read_text(),
    )
}


def evolution_level_requirements() -> dict[str, int]:
    result = {}
    for path in sorted((ROOT / "src" / "data" / "pokemon" / "species_info").glob("gen_*_families.h")):
        text = path.read_text()
        for level, species in re.findall(r"\{EVO_LEVEL,\s*(\d+),\s*(SPECIES_[A-Z0-9_]+)", text):
            result[species] = min(result.get(species, 1000), int(level))
    return result


EVOLUTION_LEVEL_REQUIREMENTS = evolution_level_requirements()

SHOWDOWN_DATA = json.loads((ROOT / "docs" / "showdown_champions_learnsets.json").read_text())
SHOWDOWN_LEARNSETS = {species: set(moves) for species, moves in SHOWDOWN_DATA["learnsets"].items()}
SHOWDOWN_FORM_SUFFIXES = (
    "50powerconstruct", "10powerconstruct", "powerconstruct", "curly", "droopy", "stretchy",
    "incarnate", "ordinary", "aria", "amped", "midday", "male", "female", "natural",
    "west", "east", "normal", "altered", "land", "sky", "small", "large", "super",
    "average", "antique", "phony", "rubycream", "marine", "autumn", "roaming",
    "debutante", "kabuki",
)


def showdown_id_for_species(species: str) -> str | None:
    showdown_id = re.sub(r"[^a-z0-9]", "", species.removeprefix("SPECIES_").lower())
    if showdown_id in SHOWDOWN_LEARNSETS:
        return showdown_id
    for suffix in SHOWDOWN_FORM_SUFFIXES:
        if showdown_id.endswith(suffix) and showdown_id[:-len(suffix)] in SHOWDOWN_LEARNSETS:
            return showdown_id[:-len(suffix)]
    return None


def pinned_legal_moves(species: str) -> set[str]:
    showdown_id = showdown_id_for_species(species)
    if showdown_id is None:
        return set()
    return {move for move_id in SHOWDOWN_LEARNSETS[showdown_id] if (move := MOVES_BY_ID.get(move_id)) is not None}


GYM_TYPES = {
    "RustboroCity_Gym": "TYPE_ROCK",
    "DewfordTown_Gym": "TYPE_FIGHTING",
    "MauvilleCity_Gym": "TYPE_ELECTRIC",
    "LavaridgeTown_Gym_1F": "TYPE_FIRE",
    "LavaridgeTown_Gym_B1F": "TYPE_FIRE",
    "PetalburgCity_Gym": "TYPE_NORMAL",
    "FortreeCity_Gym": "TYPE_FLYING",
    "MossdeepCity_Gym": "TYPE_PSYCHIC",
    "SootopolisCity_Gym_1F": "TYPE_WATER",
}


def species_types() -> dict[str, tuple[str, ...]]:
    paths = sorted((ROOT / "src" / "data" / "pokemon" / "species_info").glob("gen_*_families.h"))
    macros = {}
    for path in paths:
        text = path.read_text()
        for name, first, second in re.findall(
            r"#define\s+([A-Z0-9_]+)\s+MON_TYPES\((TYPE_[A-Z0-9_]+)(?:,\s*(TYPE_[A-Z0-9_]+))?\)", text
        ):
            macros.setdefault(name, tuple(value for value in (first, second) if value))
    result = {}
    for path in paths:
        text = path.read_text()
        markers = list(re.finditer(r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{", text))
        for index, marker in enumerate(markers):
            body = text[marker.end():markers[index + 1].start() if index + 1 < len(markers) else len(text)]
            direct = re.search(r"\.types\s*=\s*MON_TYPES\((TYPE_[A-Z0-9_]+)(?:,\s*(TYPE_[A-Z0-9_]+))?\)", body)
            if direct:
                result[marker.group(1)] = tuple(value for value in direct.groups() if value)
                continue
            macro = re.search(r"\.types\s*=\s*([A-Z0-9_]+)", body)
            if macro and macro.group(1) in macros:
                result[marker.group(1)] = macros[macro.group(1)]
    aliases = dict(re.findall(
        r"(?m)^\s*(SPECIES_[A-Z0-9_]+)\s*=\s*(SPECIES_[A-Z0-9_]+)\s*,",
        (ROOT / "include" / "constants" / "species.h").read_text(),
    ))
    for alias, target in aliases.items():
        if target in result:
            result[alias] = result[target]
    result.update({
        "SPECIES_KIRLIA": ("TYPE_PSYCHIC", "TYPE_FAIRY"),
        "SPECIES_MELOETTA": ("TYPE_NORMAL", "TYPE_PSYCHIC"),
        "SPECIES_TORNADUS": ("TYPE_FLYING",),
        "SPECIES_GASTRODON": ("TYPE_WATER", "TYPE_GROUND"),
        "SPECIES_FURFROU": ("TYPE_NORMAL",),
        "SPECIES_WIGGLYTUFF": ("TYPE_NORMAL", "TYPE_FAIRY"),
        "SPECIES_SILVALLY": ("TYPE_NORMAL",),
        "SPECIES_GARDEVOIR": ("TYPE_PSYCHIC", "TYPE_FAIRY"),
        "SPECIES_MINIOR": ("TYPE_ROCK", "TYPE_FLYING"),
    })
    return result


SPECIES_TYPES = species_types()


def line_value(text: str, key: str) -> str:
    match = re.search(rf"(?m)^{re.escape(key)}: (.*)$", text)
    return match.group(1) if match else ""


def blocks(text: str) -> list[str]:
    marks = list(ENCOUNTER_RE.finditer(text))
    return [text[m.start():marks[i + 1].start() if i + 1 < len(marks) else len(text)] for i, m in enumerate(marks)]


def current_campaign_trainer_refs() -> set[str]:
    paths = [p for p in (ROOT / "data" / "maps").rglob("*.inc") if "_Frlg" not in str(p)]
    paths += [p for p in (ROOT / "data" / "scripts").rglob("*.inc") if p.name != "trainers_frlg.inc"]
    paths.append(ROOT / "data" / "event_scripts.s")
    result = set()
    for path in paths:
        for line in path.read_text(errors="ignore").splitlines():
            if "trainerbattle" in line or "multi_2_vs_2" in line:
                result.update(re.findall(r"\bTRAINER_[A-Z0-9_]+\b", line))
    return result


def audit(path: Path) -> tuple[list[str], list[str]]:
    text = path.read_text()
    errors: list[str] = []
    notes: list[str] = []
    groups = blocks(text)

    if not groups:
        return ["no encounter blocks"], notes
    numbers = [int(ENCOUNTER_RE.search(block).group(1)) for block in groups]
    if numbers != list(range(1, len(groups) + 1)):
        errors.append("encounter numbers are not contiguous from 1")
    campaign_orders = [line_value(block, "campaign_order") for block in groups]
    if campaign_orders != [str(i) for i in range(1, len(groups) + 1)]:
        errors.append("campaign_order values do not match encounter order")
    if text.count("=== END ENCOUNTER ===") != len(groups):
        errors.append("every encounter must have exactly one END ENCOUNTER marker")

    encounter_body = "\n".join(groups)
    for forbidden in ("PENDING", "audit_pending", "design_pending_source_baseline_only", "ITEM_MILOTICITE", "level="):
        if forbidden in encounter_body:
            errors.append(f"unfinished or stale token remains: {forbidden}")

    all_trainers: list[str] = []
    all_species: list[str] = []
    all_items: list[str] = []
    fingerprint_encounters: dict[tuple, set[int]] = {}
    formats = Counter()
    difficulties: list[float] = []
    ordinary_difficulties: list[float] = []
    team_sizes: Counter[int] = Counter()
    encounter_species_sets: list[set[str]] = []
    primary_strategies: list[str] = []

    strategy_patterns = (
        ("Trick Room", r"MOVE_TRICK_ROOM"),
        ("Tailwind", r"MOVE_TAILWIND"),
        ("rain", r"ABILITY_DRIZZLE|MOVE_RAIN_DANCE"),
        ("sun", r"ABILITY_DROUGHT|MOVE_SUNNY_DAY"),
        ("sand", r"ABILITY_SAND_STREAM|MOVE_SANDSTORM"),
        ("snow", r"ABILITY_SNOW_WARNING|MOVE_SNOWSCAPE"),
        ("redirection", r"MOVE_FOLLOW_ME|MOVE_RAGE_POWDER"),
        ("Perish Song", r"MOVE_PERISH_SONG"),
        ("setup", r"MOVE_SWORDS_DANCE|MOVE_CALM_MIND|MOVE_DRAGON_DANCE|MOVE_QUIVER_DANCE|MOVE_SHELL_SMASH|MOVE_BULK_UP"),
        ("spread pressure", r"MOVE_ROCK_SLIDE|MOVE_HEAT_WAVE|MOVE_SURF|MOVE_EARTHQUAKE|MOVE_DAZZLING_GLEAM|MOVE_HYPER_VOICE"),
    )

    required_fields = (
        "physical_group_id", "proposed_encounter_id", "campaign_order", "chapter",
        "strict_cap", "location", "requirement", "status", "quality_target",
        "difficulty_target", "difficulty_observed", "fatigue_role", "primary_question",
        "theme_and_tempo", "intentional_weakness", "first_loss_lesson", "strongest_part",
        "weakest_link", "competitive_references", "dialogue_status", "reservation_status",
        "trainer_ids",
    )

    for encounter_index, block in enumerate(groups, 1):
        encounter_species_sets.append(set(re.findall(r"(?m)^  \d+\. (SPECIES_[A-Z0-9_]+)", block)))
        primary_strategies.append(next((name for name, pattern in strategy_patterns if re.search(pattern, block)), "balanced tempo"))
        for field in required_fields:
            if not line_value(block, field):
                errors.append(f"encounter {encounter_index}: missing {field}")
        if line_value(block, "status") != "master_audited_ready_for_implementation":
            errors.append(f"encounter {encounter_index}: status is not implementation-ready")
        cap = line_value(block, "strict_cap")
        if not cap.isdigit() or not 1 <= int(cap) <= 100:
            errors.append(f"encounter {encounter_index}: invalid strict cap {cap!r}")
        try:
            difficulty = float(line_value(block, "difficulty_target"))
        except ValueError:
            errors.append(f"encounter {encounter_index}: invalid difficulty")
            difficulty = 0.0
        difficulties.append(difficulty)
        if any(token in line_value(block, "trainer_ids") for token in MARQUEE_TOKENS) and difficulty != 10.0:
            errors.append(f"encounter {encounter_index}: marquee boss difficulty must be 10.0, found {difficulty:.1f}")
        trainer_line = set(line_value(block, "trainer_ids").split("; "))
        location = line_value(block, "location")
        marks = list(BRANCH_RE.finditer(block))
        branch_trainers = set()
        if not marks:
            errors.append(f"encounter {encounter_index}: no branches")
        for branch_index, mark in enumerate(marks):
            branch = block[mark.start():marks[branch_index + 1].start() if branch_index + 1 < len(marks) else len(block)]
            trainer = line_value(branch, "trainer_id")
            branch_trainers.add(trainer)
            all_trainers.append(trainer)
            if trainer not in TRAINERS and trainer not in PLANNED_RESTORE_TRAINERS:
                errors.append(f"encounter {encounter_index}: unknown trainer {trainer}")
            if trainer in REMATCH_TRAINERS:
                errors.append(f"encounter {encounter_index}: excluded Gym rematch {trainer}")
            fmt = line_value(branch, "format")
            if fmt not in ("single", "double", "multi"):
                errors.append(f"encounter {encounter_index}: invalid format {fmt!r}")
            formats[fmt] += 1
            mons = list(MON_RE.finditer(branch))
            team_sizes[len(mons)] += 1
            if not 1 <= len(mons) <= 6:
                errors.append(f"encounter {encounter_index}/{trainer}: invalid team size {len(mons)}")
            if fmt in ("double", "multi") and len(mons) < 2:
                errors.append(f"encounter {encounter_index}/{trainer}: doubles team has fewer than two Pokemon")
            species_in_team = []
            items_in_team = []
            fingerprint = []
            for expected_slot, mon in enumerate(mons, 1):
                slot, species, item, level, ability, nature, points, moves_text = mon.groups()
                moves = moves_text.split(",")
                points_list = [int(value) for value in points.split("/")]
                if int(slot) != expected_slot:
                    errors.append(f"encounter {encounter_index}/{trainer}: noncontiguous party slots")
                if species not in SPECIES:
                    errors.append(f"encounter {encounter_index}/{trainer}: unknown species {species}")
                if item not in ITEMS:
                    errors.append(f"encounter {encounter_index}/{trainer}: unknown item {item}")
                if ability not in ABILITIES:
                    errors.append(f"encounter {encounter_index}/{trainer}: unknown ability {ability}")
                if nature not in NATURES:
                    errors.append(f"encounter {encounter_index}/{trainer}: unknown nature {nature}")
                if len(points_list) != 6 or any(value < 0 or value > 32 for value in points_list) or sum(points_list) > 66:
                    errors.append(f"encounter {encounter_index}/{trainer}/{species}: illegal Stat Points {points}")
                bad_moves = set(moves) - MOVES
                if bad_moves:
                    errors.append(f"encounter {encounter_index}/{trainer}/{species}: unknown moves {sorted(bad_moves)}")
                legal_moves = pinned_legal_moves(species)
                if not legal_moves:
                    errors.append(f"encounter {encounter_index}/{trainer}/{species}: no pinned Showdown learnset mapping")
                elif species != "SPECIES_SMEARGLE":
                    illegal = set(moves) - legal_moves - {"MOVE_NONE"}
                    if illegal:
                        errors.append(
                            f"encounter {encounter_index}/{trainer}/{species}: moves outside pinned Champions/mainline learnset {sorted(illegal)}"
                        )
                real_moves = [move for move in moves if move != "MOVE_NONE"]
                if not real_moves or len(real_moves) != len(set(real_moves)):
                    errors.append(f"encounter {encounter_index}/{trainer}/{species}: empty or duplicate moves")
                if not -10 <= int(level) <= 10:
                    errors.append(f"encounter {encounter_index}/{trainer}/{species}: unreasonable level offset {level}")
                if not 1 <= int(cap) + int(level) <= 100:
                    errors.append(f"encounter {encounter_index}/{trainer}/{species}: effective level {int(cap) + int(level)} is outside 1-100")
                evolution_level = EVOLUTION_LEVEL_REQUIREMENTS.get(species)
                if int(cap) <= 45 and evolution_level is not None and int(cap) + int(level) < evolution_level:
                    errors.append(
                        f"encounter {encounter_index}/{trainer}/{species}: appears at level {int(cap) + int(level)} "
                        f"before its level-{evolution_level} evolution"
                    )
                species_in_team.append(species)
                items_in_team.append(item)
                all_species.append(species)
                all_items.append(item)
                fingerprint.append((species, item, ability, nature, tuple(moves)))
            if len(species_in_team) != len(set(species_in_team)):
                errors.append(f"encounter {encounter_index}/{trainer}: duplicate species within party")
            held_items = [item for item in items_in_team if item != "ITEM_NONE"]
            if len(held_items) != len(set(held_items)):
                errors.append(f"encounter {encounter_index}/{trainer}: duplicate held item violates Item Clause")
            if sum(item in MEGA_STONES for item in items_in_team) > 1:
                errors.append(f"encounter {encounter_index}/{trainer}: more than one Mega Stone")
            if int(cap) < 30 and any(item in MEGA_STONES for item in items_in_team):
                errors.append(f"encounter {encounter_index}/{trainer}: Mega appears before the post-Brawly bracelet")
            if location in GYM_TYPES:
                specialty = GYM_TYPES[location]
                unknown_types = [species for species in species_in_team if species not in SPECIES_TYPES]
                if unknown_types:
                    errors.append(f"encounter {encounter_index}/{trainer}: unresolved Gym species types {unknown_types}")
                specialty_count = sum(specialty in SPECIES_TYPES.get(species, ()) for species in species_in_team)
                if specialty_count * 2 < len(species_in_team):
                    errors.append(
                        f"encounter {encounter_index}/{trainer}: only {specialty_count}/{len(species_in_team)} Pokemon match {specialty}"
                    )
            fingerprint_encounters.setdefault(tuple(sorted(fingerprint)), set()).add(encounter_index)
        if branch_trainers != trainer_line:
            errors.append(f"encounter {encounter_index}: trainer_ids field differs from branches")
        if not any(token in line_value(block, "trainer_ids") for token in MARQUEE_TOKENS + MINIBOSS_TOKENS):
            ordinary_difficulties.append(difficulty)

    if len(all_trainers) != len(set(all_trainers)):
        duplicates = sorted(trainer for trainer, count in Counter(all_trainers).items() if count > 1)
        errors.append(f"trainer branches occur in multiple encounter groups: {duplicates[:20]}")
    branches = len(all_trainers)
    doubles = formats["double"] + formats["multi"]
    doubles_pct = doubles / branches * 100
    if not 83 <= doubles_pct <= 87:
        errors.append(f"doubles share {doubles_pct:.2f}% is outside 83-87%")
    duplicate_teams = sum(len(encounters) - 1 for encounters in fingerprint_encounters.values() if len(encounters) > 1)
    if duplicate_teams:
        errors.append(f"{duplicate_teams} exact duplicate team fingerprints remain")

    missing_megas = sorted(MEGA_STONES - set(all_items))
    if missing_megas:
        errors.append(f"missing Mega showcases: {missing_megas}")
    missing_signs = sorted(SIGN_SPECIES - set(all_species))
    if missing_signs:
        errors.append(f"missing legendary showcases: {missing_signs}")

    current_refs = current_campaign_trainer_refs()
    documented = set(all_trainers)
    missing_current = sorted(current_refs - documented)
    if missing_current:
        errors.append(f"current Hoenn battle references absent from master: {missing_current}")
    planned = documented - current_refs
    unclassified_planned = sorted(planned - PLANNED_RESTORE_TRAINERS)
    if unclassified_planned:
        errors.append(f"non-runtime trainers not declared for restoration: {unclassified_planned}")
    missing_planned = sorted(PLANNED_RESTORE_TRAINERS - documented)
    if missing_planned:
        errors.append(f"declared restoration trainers absent from master: {missing_planned}")

    ordinary_bands = Counter(min(9, int(value)) for value in ordinary_difficulties)
    ordinary_total = len(ordinary_difficulties)
    low_share = sum(6.0 <= value < 7.0 for value in ordinary_difficulties) / ordinary_total * 100
    if not 20 <= low_share <= 40:
        errors.append(f"ordinary 6.x share {low_share:.1f}% is outside fatigue-safe 20-40%")
    high_share = sum(value >= 9.0 for value in ordinary_difficulties) / ordinary_total * 100
    if high_share > 12:
        errors.append(f"ordinary 9.x share {high_share:.1f}% exceeds 12%")
    if any(value < 6.0 or value > 9.5 for value in ordinary_difficulties):
        errors.append("ordinary encounter difficulty falls outside 6.0-9.5")

    run_start = 0
    while run_start < len(primary_strategies):
        run_end = run_start + 1
        while run_end < len(primary_strategies) and primary_strategies[run_end] == primary_strategies[run_start]:
            run_end += 1
        if run_end - run_start >= 5:
            errors.append(
                f"primary strategy {primary_strategies[run_start]} repeats from encounters {run_start + 1}-{run_end}"
            )
        run_start = run_end
    rolling_repeat_encounters = 0
    for index, species_set in enumerate(encounter_species_sets):
        recent = set().union(*encounter_species_sets[max(0, index - 2):index]) if index else set()
        if species_set & recent:
            rolling_repeat_encounters += 1
    if rolling_repeat_encounters > 35:
        errors.append(f"species repeat in the prior-two window occurs in {rolling_repeat_encounters} encounters (max 35)")

    usage = Counter(all_species)
    notes.extend([
        f"encounters={len(groups)} branches={branches} formats={dict(formats)} doubles={doubles_pct:.2f}%",
        f"difficulty mean={statistics.mean(difficulties):.2f} median={statistics.median(difficulties):.1f}",
        f"ordinary bands={dict(sorted(ordinary_bands.items()))} 6.x={low_share:.1f}% 9.x={high_share:.1f}%",
        f"team sizes={dict(sorted(team_sizes.items()))}",
        f"unique species={len(usage)} top usage={usage.most_common(15)}",
        f"Mega showcases={len(MEGA_STONES - set(missing_megas))}/{len(MEGA_STONES)} legendary showcases={len(SIGN_SPECIES - set(missing_signs))}/{len(SIGN_SPECIES)}",
        f"current runtime trainer ids={len(current_refs)} planned restores={len(planned)}",
        f"primary strategies={dict(Counter(primary_strategies))} prior-two species-repeat encounters={rolling_repeat_encounters}",
    ])
    return errors, notes


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("master", nargs="?", type=Path, default=DEFAULT_MASTER)
    args = parser.parse_args()
    errors, notes = audit(args.master)
    for note in notes:
        print(f"INFO: {note}")
    if errors:
        for error in errors:
            print(f"FAIL: {error}")
        raise SystemExit(1)
    print("PASS: campaign battle master satisfies all static closure gates")


if __name__ == "__main__":
    main()
