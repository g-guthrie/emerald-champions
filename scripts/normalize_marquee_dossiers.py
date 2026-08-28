#!/usr/bin/env python3
"""Normalize marquee dossiers to the Emerald Champions operating-system schema."""

from __future__ import annotations

import argparse
import json
from pathlib import Path

import verdant_team_quality_audit as quality


ROOT = Path(__file__).resolve().parents[1]
PATH = ROOT / "docs/verdant_marquee_battle_designs.json"

ANCHORS = {
    "ELITE_FOUR_SIDNEY": {
        "canonical_stage": "First main-story Elite Four member at League entry",
        "location": "EverGrandeCity_SidneysRoom",
        "gauntlet_position": "First Elite Four member. The player enters fully prepared, then must manually repair Sidney's HP and faint pressure before Phoebe.",
        "trainer_id": "TRAINER_SIDNEY",
        "party_source": "sParty_Sidney",
        "source_paths": [
            "src/data/trainer_parties.h:sParty_Sidney",
            "src/data/trainers.h:TRAINER_SIDNEY",
            "data/maps/EverGrandeCity_SidneysRoom/scripts.inc",
        ],
    },
    "ELITE_FOUR_PHOEBE": {
        "canonical_stage": "Second main-story Elite Four member immediately after Sidney",
        "location": "EverGrandeCity_PhoebesRoom",
        "gauntlet_position": "Second Elite Four member. Her clock can create faints, sleep, and burn that require manual Bag recovery before Glacia.",
        "trainer_id": "TRAINER_PHOEBE",
        "party_source": "sParty_Phoebe",
        "source_paths": [
            "src/data/trainer_parties.h:sParty_Phoebe",
            "src/data/trainers.h:TRAINER_PHOEBE",
            "data/maps/EverGrandeCity_PhoebesRoom/scripts.inc",
        ],
    },
    "ELITE_FOUR_GLACIA": {
        "canonical_stage": "Third main-story Elite Four member immediately after Phoebe",
        "location": "EverGrandeCity_GlaciasRoom",
        "gauntlet_position": "Third Elite Four member. Her poison, trapping, item disruption, and detonation pressure can force manual status, HP, PP, and held-item repair before Drake.",
        "trainer_id": "TRAINER_GLACIA",
        "party_source": "sParty_Glacia",
        "source_paths": [
            "src/data/trainer_parties.h:sParty_Glacia",
            "src/data/trainers.h:TRAINER_GLACIA",
            "data/maps/EverGrandeCity_GlaciasRoom/scripts.inc",
        ],
    },
    "ELITE_FOUR_DRAKE": {
        "canonical_stage": "Fourth and final main-story Elite Four member immediately after Glacia",
        "location": "EverGrandeCity_DrakesRoom",
        "gauntlet_position": "Fourth Elite Four member and the only intentional singles duel in the main-story League. Victory restores overworld control for manual repair before Wallace.",
        "trainer_id": "TRAINER_DRAKE",
        "party_source": "sParty_Drake",
        "source_paths": [
            "src/data/trainer_parties.h:sParty_Drake",
            "src/data/trainers.h:TRAINER_DRAKE",
            "data/maps/EverGrandeCity_DrakesRoom/scripts.inc",
        ],
    },
    "CHAMPION_WALLACE": {
        "canonical_stage": "Main-story Champion immediately after Drake",
        "location": "EverGrandeCity_ChampionsRoom",
        "gauntlet_position": "Final main-story League battle. The player may manually repair and save in Drake's room and Hall 4, but entering the Champion room forces Wallace's approach.",
        "trainer_id": "TRAINER_WALLACE",
        "party_source": "sParty_Wallace",
        "source_paths": [
            "src/data/trainer_parties.h:sParty_Wallace",
            "src/data/trainers.h:TRAINER_WALLACE",
            "data/maps/EverGrandeCity_ChampionsRoom/scripts.inc",
        ],
    },
}


def build() -> dict:
    payload = json.loads(PATH.read_text())
    source_teams = {team["trainer_id"]: team for team in quality.audit()["teams"]}

    for anchor_id, expected in ANCHORS.items():
        dossier = payload["designs"][anchor_id]
        campaign = dossier["campaign_state"]
        campaign.setdefault("canonical_stage", expected["canonical_stage"])
        campaign.setdefault("location", expected["location"])
        campaign.setdefault("gauntlet_position", expected["gauntlet_position"])
        campaign.setdefault(
            "live_difficulty",
            "Hard uses authored offsets; Medium subtracts two levels and Easy subtracts four from every opposing trainer Pokemon without changing teams, stages, sets, items, abilities, EVs, AI, formats, or the player cap.",
        )

        runtime = dossier["runtime"]
        runtime.setdefault("party_size", 6)
        runtime.setdefault("required", True)
        runtime.setdefault("source_paths", expected["source_paths"])
        source = source_teams[expected["trainer_id"]]
        runtime.setdefault(
            "current_source_baseline",
            {
                "party": [mon["species"] for mon in source["mons"]],
                "level_offsets": [mon["level_offset"] for mon in source["mons"]],
                "guide_difficulty": source["quality_score"],
                "guide_design_status": "not individually closed",
            },
        )

        rolling = dossier["rolling_context"]
        rolling.setdefault("previous_encounters", [])
        rolling.setdefault(
            "required_preimplementation_review",
            "Refresh the exact prior-ten physical encounters at chronological implementation, then review species, family, Mega, legendary, item, signature-move, tempo, primary-question, and historic-reference collisions before changing source.",
        )

        verification = dossier["verification"]
        if "source_blockers" not in verification:
            verification["source_blockers"] = list(verification.get("known_blockers", []))

    return payload


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.write and not args.check:
        parser.error("choose --write or --check")
    expected = json.dumps(build(), indent=2, ensure_ascii=False) + "\n"
    if args.write:
        PATH.write_text(expected)
    if args.check and PATH.read_text() != expected:
        raise SystemExit("FAIL: marquee dossiers are not normalized to the operating-system schema")
    print("PASS: five League dossiers use the complete normalized operating-system schema")


if __name__ == "__main__":
    main()
