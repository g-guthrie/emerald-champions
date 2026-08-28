#!/usr/bin/env python3
"""Validate one completed ten-encounter slice of the editable master plan."""

from __future__ import annotations

import argparse
import json
import re
from collections import Counter, defaultdict
from pathlib import Path
from statistics import mean, median

import verdant_battle_set_presets as presets
import verdant_doubles_conversion as doubles


ROOT = Path(__file__).resolve().parents[1]
MASTER = ROOT / "docs/emerald_champions_master_battle_design.txt"
ITEMS = ROOT / "include/constants/items.h"
CORPUS = ROOT / "docs/competitive_team_index.jsonl"


MEMBER_RE = re.compile(
    r"^\s+\d+\.\s+(SPECIES_[A-Z0-9_]+)\s+@\s+(ITEM_[A-Z0-9_]+)"
    r"\s+\|\s+level=(-?\d+)\s+\|\s+ability_slot=(\d+)"
    r"\s+\|\s+(SPREAD_[A-Z0-9_]+)\s+\|\s+moves=([A-Z0-9_,]+)$"
)


def field(block: str, name: str) -> str:
    match = re.search(rf"^{re.escape(name)}:\s*(.*)$", block, re.M)
    if not match:
        raise ValueError(f"missing field {name}")
    return match.group(1).strip()


def blocks() -> dict[int, str]:
    text = MASTER.read_text()
    result = {}
    for match in re.finditer(
        r"^=== ENCOUNTER \d{4} ===$(.*?)^=== END ENCOUNTER ===$",
        text,
        re.M | re.S,
    ):
        block = match.group(1)
        order = field(block, "campaign_order")
        if order.isdigit():
            result[int(order)] = block
    return result


def members(block: str) -> list[dict]:
    result = []
    for line in block.splitlines():
        match = MEMBER_RE.match(line)
        if match:
            result.append({
                "species": match.group(1),
                "item": match.group(2),
                "level": int(match.group(3)),
                "ability_slot": int(match.group(4)),
                "spread": match.group(5),
                "moves": match.group(6).split(","),
            })
    return result


def validate(start: int, end: int) -> None:
    if end - start != 9:
        raise SystemExit("FAIL: a check-in batch must contain exactly ten encounters")
    all_blocks = blocks()
    missing = [index for index in range(start, end + 1) if index not in all_blocks]
    if missing:
        raise SystemExit(f"FAIL: missing campaign orders {missing}")

    dex = presets.LocalDex()
    ability_slots = doubles.base_ability_slots()
    item_tokens = set(re.findall(r"\bITEM_[A-Z0-9_]+\b", ITEMS.read_text()))
    corpus_refs = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    species_to_encounters = defaultdict(list)
    questions = []
    difficulties = []
    formats = Counter()
    fatigue_roles = Counter()
    petalburg_normal = []

    for index in range(start, end + 1):
        block = all_blocks[index]
        if field(block, "status") != "master_authored_approved_not_implemented":
            raise SystemExit(f"FAIL: Battle {index} is not master-approved")
        if field(block, "quality_target") != "10":
            raise SystemExit(f"FAIL: Battle {index} quality target drifted")
        if "PENDING" in field(block, "fatigue_role"):
            raise SystemExit(f"FAIL: Battle {index} has no fatigue role")
        if "PENDING" in field(block, "strongest_part") or "PENDING" in field(block, "weakest_link"):
            raise SystemExit(f"FAIL: Battle {index} lacks author self-check")
        difficulty = float(field(block, "difficulty_target"))
        if not 7.5 <= difficulty <= 10:
            raise SystemExit(f"FAIL: Battle {index} difficulty out of bounds")
        difficulties.append(difficulty)
        questions.append(field(block, "primary_question"))
        fatigue_roles[field(block, "fatigue_role")] += 1

        branch_formats = re.findall(r"^format:\s*(single|double)$", block, re.M)
        if not branch_formats:
            raise SystemExit(f"FAIL: Battle {index} has no concrete format")
        formats.update(set(branch_formats))

        exact_members = members(block)
        if not exact_members:
            raise SystemExit(f"FAIL: Battle {index} has no exact team")
        encounter_species = {member["species"] for member in exact_members}
        for species in encounter_species:
            species_to_encounters[species].append(index)
        for member in exact_members:
            if member["species"] not in ability_slots:
                raise SystemExit(f"FAIL: Battle {index} unknown species {member['species']}")
            if member["ability_slot"] >= len(ability_slots[member["species"]]):
                raise SystemExit(f"FAIL: Battle {index} bad ability slot {member['species']}")
            if member["item"] not in item_tokens:
                raise SystemExit(f"FAIL: Battle {index} unknown item {member['item']}")
            illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
            if illegal:
                raise SystemExit(f"FAIL: Battle {index} illegal {member['species']} moves {illegal}")
            if not 0 <= member["level"] <= 10:
                raise SystemExit(f"FAIL: Battle {index} implausible cap offset {member['level']}")

        references = [ref.strip() for ref in field(block, "competitive_references").split(";") if ref.strip() and ref.strip() != "NONE"]
        missing_refs = [ref for ref in references if ref not in corpus_refs]
        if missing_refs:
            raise SystemExit(f"FAIL: Battle {index} unknown references {missing_refs}")

        if field(block, "location") == "PetalburgCity_Gym":
            normal = 0
            for species in encounter_species:
                stats = dex.stats[species]
                if "TYPE_NORMAL" in (stats.type1, stats.type2):
                    normal += 1
            petalburg_normal.append((index, normal, len(encounter_species)))
            if normal < 2:
                raise SystemExit(f"FAIL: Battle {index} does not respect Petalburg's Normal specialty")

    collisions = {species: indexes for species, indexes in species_to_encounters.items() if len(indexes) > 1}
    if collisions:
        raise SystemExit(f"FAIL: unrelated species repeat inside batch: {collisions}")
    if len(set(questions)) != 10:
        raise SystemExit("FAIL: primary questions are not unique")

    prior_species = set()
    for index in range(max(1, start - 10), start):
        if index in all_blocks:
            prior_species.update(member["species"] for member in members(all_blocks[index]))
    prior_collisions = sorted(set(species_to_encounters) & prior_species)
    if prior_collisions:
        raise SystemExit(f"FAIL: species collide with previous ten: {prior_collisions}")

    if petalburg_normal:
        normal_total = sum(normal for _, normal, _ in petalburg_normal)
        team_total = sum(total for _, _, total in petalburg_normal)
        if normal_total / team_total < 0.75:
            raise SystemExit("FAIL: Petalburg batch lost its Normal-type identity")
    print(
        f"PASS: Battles {start}-{end}: 10 approved puzzles, "
        f"{len(species_to_encounters)} distinct species, difficulty "
        f"{min(difficulties):.1f}-{max(difficulties):.1f} "
        f"(mean {mean(difficulties):.2f}, median {median(difficulties):.2f})"
    )
    print(f"PASS: formats {dict(formats)}, fatigue roles {dict(fatigue_roles)}, no previous-ten species collision")
    if petalburg_normal:
        print(f"PASS: Petalburg Normal specialty {normal_total}/{team_total} distinct team slots")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--start", type=int, required=True)
    parser.add_argument("--end", type=int, required=True)
    args = parser.parse_args()
    validate(args.start, args.end)


if __name__ == "__main__":
    main()
