#!/usr/bin/env python3
"""Apply explicit, trainer-specific party polish from the checked-in manifest."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

import verdant_custom_teams as custom
import verdant_doubles_conversion as doubles


ROOT = Path(__file__).resolve().parents[1]
MANIFEST_PATH = ROOT / "docs/verdant_team_polish_manifest.json"
TRAINERS_PATH = ROOT / "src/data/trainers.h"
PARTIES_PATH = ROOT / "src/data/trainer_parties.h"


def parse_entry(entry: str) -> dict:
    def value(name: str, default: str) -> str:
        match = re.search(rf"\.{name}\s*=\s*([^,\n}}]+)", entry)
        return match.group(1).strip() if match else default

    moves_match = re.search(r"\.moves\s*=\s*([^\n}]+)", entry)
    moves = re.findall(r"MOVE_[A-Z0-9_]+", moves_match.group(1)) if moves_match else []
    ability_text = value("ability", "0").split()[0]
    return {
        "level": int(value("lvl", "0")),
        "species": value("species", "SPECIES_NONE"),
        "item": value("heldItem", "ITEM_NONE"),
        "ability_slot": int(ability_text) if ability_text.isdigit() else 0,
        "spread": value("spread", "SPREAD_0_IV_EV"),
        "moves": moves,
    }


def render(build: dict, trainer_id: str) -> str:
    moves = ", ".join(build["moves"])
    return (
        "    {\n"
        f"    .lvl = {build['level']},\n"
        f"    .species = {build['species']},\n"
        f"    .heldItem = {build['item']},\n"
        f"    .ability = {build['ability_slot']},\n"
        f"    .spread = {build['spread']},\n"
        f"    .moves = {moves}\n"
        f"    }} /* Verdant polish: {trainer_id} */"
    )


def resolved_parties() -> dict[str, list[dict]]:
    manifest = json.loads(MANIFEST_PATH.read_text())
    trainers_text = TRAINERS_PATH.read_text()
    parties_text = PARTIES_PATH.read_text()
    blocks = doubles.trainer_blocks(trainers_text)
    result = {}

    trainer_ids = set(manifest.get("patches", {})) | set(manifest.get("level_profiles", {})) | set(manifest.get("rewrites", {}))
    for trainer_id in sorted(trainer_ids):
        block = blocks[trainer_id].group(0)
        party_name = doubles.party_name(block)
        body = doubles.party_match(parties_text, party_name).group(2)
        builds = [parse_entry(entry) for entry in custom.party_entries(body)]

        if trainer_id in manifest.get("rewrites", {}):
            builds = manifest["rewrites"][trainer_id]
        if trainer_id in manifest.get("level_profiles", {}):
            profile = manifest["level_profiles"][trainer_id]
            if len(profile) != len(builds):
                raise ValueError(f"{trainer_id}: level profile has {len(profile)} slots, party has {len(builds)}")
            for build, level in zip(builds, profile):
                build["level"] = level
        for index_text, patch in manifest.get("patches", {}).get(trainer_id, {}).items():
            index = int(index_text)
            if index >= len(builds):
                raise ValueError(f"{trainer_id}: patch slot {index} is outside {len(builds)}-mon party")
            builds[index].update(patch)
        result[trainer_id] = builds
    return result


def apply() -> None:
    trainers_text = TRAINERS_PATH.read_text()
    parties_text = PARTIES_PATH.read_text()
    blocks = doubles.trainer_blocks(trainers_text)
    parties = resolved_parties()
    for trainer_id, builds in parties.items():
        party_name = doubles.party_name(blocks[trainer_id].group(0))
        entries = [render(build, trainer_id) for build in builds]
        parties_text = custom.replace_party_body(parties_text, party_name, entries)
    parties_text = custom.normalize_disabled_entry_commas(parties_text)
    PARTIES_PATH.write_text(parties_text)
    print(f"applied explicit polish to {len(parties)} trainer parties")


def check() -> None:
    manifest = json.loads(MANIFEST_PATH.read_text())
    parties = resolved_parties()
    constants = "\n".join(
        (ROOT / path).read_text()
        for path in (
            "include/constants/species.h", "include/constants/items.h", "include/constants/moves.h",
            "include/constants/verdant_gen9_species.h",
            "include/constants/spreads.h",
        )
    )
    problems = []
    for trainer_id, builds in parties.items():
        if not 1 <= len(builds) <= 6:
            problems.append(f"{trainer_id}: invalid party size {len(builds)}")
        for index, build in enumerate(builds):
            is_imposter_ditto = (
                build["species"] == "SPECIES_DITTO"
                and build["ability_slot"] == 2
                and build["moves"] == ["MOVE_TRANSFORM", "MOVE_NONE", "MOVE_NONE", "MOVE_NONE"]
            )
            if not is_imposter_ditto and (len(build["moves"]) != 4 or len(set(build["moves"])) != 4):
                problems.append(f"{trainer_id}[{index}]: moves must contain four distinct entries")
            if not -100 <= build["level"] <= 7:
                problems.append(f"{trainer_id}[{index}]: invalid cap offset {build['level']}")
            for constant in (build["species"], build["item"], build["spread"], *build["moves"]):
                if not re.search(rf"\b{re.escape(constant)}\b", constants):
                    problems.append(f"{trainer_id}[{index}]: unknown constant {constant}")
    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))
    print(
        f"PASS: {len(parties)} explicitly polished parties; "
        f"{len(manifest.get('rewrites', {}))} full rewrites, "
        f"{len(manifest.get('patches', {}))} patched parties"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--apply", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.apply and not args.check:
        parser.error("choose --apply or --check")
    if args.apply:
        apply()
    if args.check:
        check()


if __name__ == "__main__":
    main()
