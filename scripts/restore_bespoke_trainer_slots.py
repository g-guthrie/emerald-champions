#!/usr/bin/env python3
"""Repurpose disabled rematch slots for one-time Emerald Champions trainers."""

from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PARTY = ROOT / "src" / "data" / "trainers.party"
BLOCK_RE = re.compile(r"(?m)^=== (TRAINER_[A-Z0-9_]+) ===$")

RESTORATIONS = {
    "TRAINER_BRAWLY_2": ("TRAINER_ARCHIE_SLATEPORT", "ARCHIE", "Aqua Leader", "Aqua Leader Archie", "Male", "Aqua"),
    "TRAINER_BRAWLY_3": ("TRAINER_COURTNEY_METEOR_FALLS", "COURTNEY", "Team Magma", "Magma Grunt F", "Female", "Magma"),
    "TRAINER_BRAWLY_4": ("TRAINER_GRUNT_METEOR_FALLS", "GRUNT", "Team Magma", "Magma Grunt M", "Male", "Magma"),
    "TRAINER_BRAWLY_5": ("TRAINER_LUCY_LAVARIDGE", "LUCY", "Pike Queen", "Pike Queen Lucy", "Female", "Female"),
    "TRAINER_WINONA_2": ("TRAINER_GRETA_SLATEPORT", "GRETA", "Arena Tycoon", "Arena Tycoon Greta", "Female", "Female"),
    "TRAINER_WINONA_3": ("TRAINER_SPENSER_FORTREE", "SPENSER", "Palace Maven", "Palace Maven Spenser", "Male", "Male"),
    "TRAINER_WINONA_4": ("TRAINER_MAGIKARP_GUY", "FISHERMAN", "Fisherman", "Fisherman", "Male", "Hiker"),
    "TRAINER_WINONA_5": ("TRAINER_BUFFEL", "BUFFEL", "Expert", "Expert M", "Male", "Hiker"),
    "TRAINER_WATTSON_2": ("TRAINER_COURTNEY_MAGMA_HIDEOUT", "COURTNEY", "Team Magma", "Magma Grunt F", "Female", "Magma"),
    "TRAINER_WATTSON_3": ("TRAINER_MATT_MT_PYRE", "MATT", "Aqua Admin", "Aqua Admin M", "Male", "Aqua"),
    "TRAINER_WATTSON_4": ("TRAINER_COURTNEY_MOSSDEEP", "COURTNEY", "Team Magma", "Magma Grunt F", "Female", "Magma"),
    "TRAINER_WATTSON_5": ("TRAINER_WALLACE_DOUBLES_LEGENDS", "WALLACE", "Champion", "Champion Wallace", "Male", "Male"),
    "TRAINER_ROXANNE_2": ("TRAINER_LEAF_ALTERING_CAVE", "LEAF", "Rival", "Leaf", "Female", "Female"),
    "TRAINER_ROXANNE_3": ("TRAINER_CYNTHIA_1", "CYNTHIA", "Cooltrainer", "Cooltrainer F", "Female", "Female"),
}


def placeholder_block(data: tuple[str, str, str, str, str, str]) -> str:
    trainer, name, trainer_class, pic, gender, music = data
    return (
        f"=== {trainer} ===\n"
        f"Name: {name}\n"
        f"Class: {trainer_class}\n"
        f"Pic: {pic}\n"
        f"Gender: {gender}\n"
        f"Music: {music}\n"
        "Double Battle: No\n"
        "AI: Basic Trainer\n\n"
        "Beldum\n"
        "Level: 5\n"
        "IVs: 0 HP / 0 Atk / 0 Def / 0 SpA / 0 SpD / 0 Spe\n\n"
    )


def main() -> None:
    text = PARTY.read_text()
    markers = list(BLOCK_RE.finditer(text))
    prefix = text[:markers[0].start()]
    output = [prefix]
    restored = set()
    for index, marker in enumerate(markers):
        block = text[marker.start():markers[index + 1].start() if index + 1 < len(markers) else len(text)]
        donor = marker.group(1)
        if donor in RESTORATIONS:
            block = placeholder_block(RESTORATIONS[donor])
            restored.add(donor)
        output.append(block)
    missing = set(RESTORATIONS) - restored
    if missing:
        raise SystemExit(f"missing donor trainer blocks: {sorted(missing)}")
    PARTY.write_text("".join(output))
    print(f"restored_bespoke_trainer_slots={len(restored)}")


if __name__ == "__main__":
    main()
