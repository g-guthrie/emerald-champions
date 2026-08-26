#!/usr/bin/env python3
"""Generate Verdant's descriptive trainer-species and showcase ledger.

Normal verification keeps this ledger synchronized with closed canonical
encounters. Cross-encounter repetition and open showcase slots are information,
not failures. The explicit final-coverage mode is reserved for campaign closure.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
from collections import defaultdict
from pathlib import Path
from typing import Any

import verdant_battle_context as context
import verdant_custom_teams as custom
import verdant_doubles_conversion as doubles
import verdant_team_quality_audit as quality


ROOT = Path(__file__).resolve().parents[1]
SEQUENCE_PATH = ROOT / "docs/verdant_battle_sequence.json"
DESIGNS_PATH = ROOT / "docs/verdant_bespoke_battle_designs.json"
RESERVATIONS_PATH = ROOT / "docs/verdant_historic_team_reservations.json"
MARQUEE_PATH = ROOT / "docs/verdant_marquee_battle_designs.json"
OUTPUT_PATH = ROOT / "docs/verdant_species_usage_ledger.json"
TRAINERS_PATH = ROOT / "src/data/trainers.h"
PARTIES_PATH = ROOT / "src/data/trainer_parties.h"
STARTERS_PATH = ROOT / "src/starter_choose.c"


def load_json(path: Path) -> Any:
    return json.loads(path.read_text())


def file_sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def family_for_species(species: str) -> str | None:
    matches = []
    for family in custom.LEGENDARY_FAMILIES:
        prefix = f"SPECIES_{family}"
        if species == prefix or species.startswith(prefix + "_"):
            matches.append(family)
    return max(matches, key=len) if matches else None


def starter_runtime_species() -> dict[str, set[str]]:
    """Read closed rival encounters whose starter species change at runtime."""
    source = STARTERS_PATH.read_text()
    base_species: set[str] = set()
    for name in ("Kanto", "Johto", "Hoenn", "Sinnoh", "Unova", "Kalos", "Alola"):
        match = re.search(rf"sStarterMon{name}\[STARTER_MON_COUNT\]\s*=\s*\{{(.*?)\}};", source, re.S)
        if match is None:
            raise ValueError(f"missing runtime starter table sStarterMon{name}")
        values = set(re.findall(r"SPECIES_[A-Z0-9_]+", match.group(1)))
        if len(values) != 3:
            raise ValueError(f"runtime starter table sStarterMon{name} has {len(values)} species")
        base_species.update(values)

    middle_match = re.search(
        r"u16 GetMiddleEvolutionForStarter\(u16 species\)\s*\{(.*?)\n\}",
        source,
        re.S,
    )
    if middle_match is None:
        raise ValueError("missing GetMiddleEvolutionForStarter runtime mapping")
    middle_species = set(re.findall(r"return\s+(SPECIES_[A-Z0-9_]+)\s*;", middle_match.group(1)))
    if len(middle_species) != 21:
        raise ValueError(f"runtime middle-starter mapping has {len(middle_species)} species")

    return {
        "BATTLE_001_ROUTE_103_RIVAL": base_species,
        "BATTLE_028_RUSTBORO_RIVAL": middle_species,
        "BATTLE_054_ROUTE_110_RIVAL": middle_species,
    }


def exact_party_mons(entries: list[dict]) -> tuple[dict[str, list[dict]], list[dict]]:
    """Return exact unique source sets for each indexed physical encounter."""
    trainers_text = TRAINERS_PATH.read_text()
    parties_text = PARTIES_PATH.read_text()
    blocks = doubles.trainer_blocks(trainers_text)
    move_data = quality.parse_moves()
    species_data = quality.parse_species()
    result: dict[str, list[dict]] = {}
    errors: list[dict] = []

    for encounter in entries:
        unique: dict[tuple, dict] = {}
        for trainer_id in encounter.get("trainer_ids", []):
            block_match = blocks.get(trainer_id)
            if block_match is None:
                errors.append({"encounter_id": encounter["encounter_id"], "message": f"missing trainer {trainer_id}"})
                continue
            block = block_match.group(0)
            party_name = doubles.party_name(block)
            party_match = doubles.party_match(parties_text, party_name)
            if party_match is None:
                errors.append({"encounter_id": encounter["encounter_id"], "message": f"missing party {party_name}"})
                continue
            for raw in custom.party_entries(party_match.group(2)):
                mon = quality.parse_mon(raw, move_data, species_data)
                signature = (
                    mon["species"],
                    mon["item"],
                    mon["ability"],
                    tuple(mon["moves"]),
                    mon["level_offset"],
                )
                unique.setdefault(signature, mon)
        result[encounter["encounter_id"]] = list(unique.values())
    return result, errors


def mega_requirements() -> list[dict]:
    requirements: list[dict] = []
    for path in (ROOT / "src/data/pokemon/evolution.h", ROOT / "src/data/pokemon/verdant_gen9_evolutions.h"):
        source = path.read_text()
        for match in re.finditer(r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{\{(.*?)\}\},?", source, re.S):
            species, body = match.groups()
            for item, mega_species in re.findall(
                r"EVO_MEGA_EVOLUTION,\s*(ITEM_[A-Z0-9_]+),\s*(SPECIES_[A-Z0-9_]+)", body
            ):
                requirements.append({
                    "key": f"{species}+{item}",
                    "species": species,
                    "mechanism": "item",
                    "requirement": item,
                    "mega_species": mega_species,
                })
            for move, mega_species in re.findall(
                r"EVO_MOVE_MEGA_EVOLUTION,\s*(MOVE_[A-Z0-9_]+),\s*(SPECIES_[A-Z0-9_]+)", body
            ):
                requirements.append({
                    "key": f"{species}+{move}",
                    "species": species,
                    "mechanism": "move",
                    "requirement": move,
                    "mega_species": mega_species,
                })
    return sorted(requirements, key=lambda row: row["key"])


def exact_reservations() -> tuple[dict[str, list[dict]], dict[str, list[dict]], list[dict]]:
    legendary: dict[str, list[dict]] = defaultdict(list)
    megas: dict[str, list[dict]] = defaultdict(list)
    errors: list[dict] = []
    requirements = {row["key"]: row for row in mega_requirements()}

    if MARQUEE_PATH.exists():
        payload = load_json(MARQUEE_PATH)
        for anchor, dossier in payload.get("designs", {}).items():
            if dossier.get("status", {}).get("source") != "unimplemented":
                continue
            for mon in dossier.get("team", []):
                species = mon.get("species")
                item = mon.get("item")
                moves = set(mon.get("moves", []))
                if not isinstance(species, str):
                    continue
                family = family_for_species(species)
                if family:
                    legendary[family].append({
                        "anchor_id": anchor,
                        "species": species,
                        "basis": "unimplemented-marquee-team",
                    })
                for key, requirement in requirements.items():
                    if species != requirement["species"]:
                        continue
                    if requirement["mechanism"] == "item" and item == requirement["requirement"]:
                        megas[key].append({"anchor_id": anchor, "basis": "unimplemented-marquee-team"})
                    if requirement["mechanism"] == "move" and requirement["requirement"] in moves:
                        megas[key].append({"anchor_id": anchor, "basis": "unimplemented-marquee-team"})

    if RESERVATIONS_PATH.exists():
        payload = load_json(RESERVATIONS_PATH)
        entries = payload.get("marquee_blueprints", {}).get("entries", [])
        known = set(custom.LEGENDARY_FAMILIES)
        for entry in entries:
            anchor = entry.get("anchor")
            for family in entry.get("reserved_legendary_families", []):
                if family not in known:
                    errors.append({"anchor_id": anchor, "message": f"unknown reserved legendary family {family}"})
                    continue
                legendary[family].append({
                    "anchor_id": anchor,
                    "species": None,
                    "basis": "structured-soft-blueprint",
                })
    return legendary, megas, errors


def build() -> tuple[dict, list[dict]]:
    sequence_payload = load_json(SEQUENCE_PATH)
    entries = sequence_payload.get("entries", [])
    closed = [entry for entry in entries if entry.get("status") == "closed"]
    source_facts, source_errors = context.parse_source_facts(closed)
    party_mons, party_errors = exact_party_mons(closed)
    runtime = starter_runtime_species()
    appearances: dict[str, dict[str, set[str]]] = defaultdict(lambda: defaultdict(set))

    for encounter in closed:
        encounter_id = encounter["encounter_id"]
        for species in source_facts.get(encounter_id, {}).get("species", []):
            appearances[species][encounter_id].add("trainer-party")
        for species in runtime.get(encounter_id, set()):
            appearances[species][encounter_id].add("runtime-source")

    entry_by_id = {entry["encounter_id"]: entry for entry in entries}
    species_rows = []
    for species in sorted(appearances):
        rows = []
        for encounter_id in sorted(appearances[species], key=lambda value: entry_by_id[value]["index"]):
            encounter = entry_by_id[encounter_id]
            rows.append({
                "battle_index": encounter["index"],
                "encounter_id": encounter_id,
                "trainer_ids": encounter.get("trainer_ids", []),
                "basis": sorted(appearances[species][encounter_id]),
            })
        species_rows.append({
            "species": species,
            "appearance_count": len(rows),
            "appearances": rows,
        })

    duplications = [
        {
            "species": row["species"],
            "appearance_count": row["appearance_count"],
            "encounter_ids": [appearance["encounter_id"] for appearance in row["appearances"]],
        }
        for row in species_rows
        if row["appearance_count"] > 1
    ]

    legendary_reservations, mega_reservations, reservation_errors = exact_reservations()
    legendary_rows = []
    for family in custom.LEGENDARY_FAMILIES:
        used = [row for row in species_rows if family_for_species(row["species"]) == family]
        reservations = legendary_reservations.get(family, [])
        status = "used" if used else "reserved" if reservations else "unplaced"
        legendary_rows.append({
            "family_id": family,
            "status": status,
            "used_species": [row["species"] for row in used],
            "used_encounter_ids": sorted({
                appearance["encounter_id"]
                for row in used
                for appearance in row["appearances"]
            }),
            "reservations": reservations,
        })

    mega_uses: dict[str, list[str]] = defaultdict(list)
    requirements = mega_requirements()
    for encounter in closed:
        encounter_id = encounter["encounter_id"]
        for mon in party_mons.get(encounter_id, []):
            for requirement in requirements:
                if mon["species"] != requirement["species"]:
                    continue
                if requirement["mechanism"] == "item" and mon["item"] == requirement["requirement"]:
                    mega_uses[requirement["key"]].append(encounter_id)
                if requirement["mechanism"] == "move" and requirement["requirement"] in mon["moves"]:
                    mega_uses[requirement["key"]].append(encounter_id)

    mega_rows = []
    for requirement in requirements:
        uses = sorted(set(mega_uses.get(requirement["key"], [])), key=lambda value: entry_by_id[value]["index"])
        reservations = mega_reservations.get(requirement["key"], [])
        status = "used" if uses else "reserved" if reservations else "unplaced"
        mega_rows.append({**requirement, "status": status, "used_encounter_ids": uses, "reservations": reservations})

    legendary_counts = {status: sum(row["status"] == status for row in legendary_rows) for status in ("used", "reserved", "unplaced")}
    mega_counts = {status: sum(row["status"] == status for row in mega_rows) for status in ("used", "reserved", "unplaced")}
    report = {
        "version": 1,
        "scope": "Closed canonical physical opponent encounters; one appearance per exact species constant per encounter.",
        "policy": {
            "ordinary_species": "Usage is descriptive. Missing ordinary species never create coverage debt; repetition requires judgment, not replacement.",
            "legendary_families": "Every supported legendary and mythical family must be used by campaign end. Reserved and unplaced families never force the next battle.",
            "mega_showcases": "Every supported Mega mechanism must be used by campaign end. Reserved and unplaced showcases never force the next battle.",
            "normal_check": "Fails only for stale output or malformed canonical source.",
            "final_coverage": "Requires an explicitly complete canonical sequence and actual use of every required legendary family and Mega mechanism.",
        },
        "source_identity": {
            str(path.relative_to(ROOT)): file_sha256(path)
            for path in (SEQUENCE_PATH, DESIGNS_PATH, RESERVATIONS_PATH, MARQUEE_PATH, TRAINERS_PATH, PARTIES_PATH, STARTERS_PATH)
            if path.exists()
        },
        "summary": {
            "sequence_entries": len(entries),
            "closed_encounters": len(closed),
            "used_species": len(species_rows),
            "duplicated_species": len(duplications),
            "legendary_families": legendary_counts,
            "mega_showcases": mega_counts,
        },
        "species": species_rows,
        "duplications": duplications,
        "legendary_families": legendary_rows,
        "mega_showcases": mega_rows,
    }
    return report, source_errors + party_errors + reservation_errors


def serialized(report: dict) -> str:
    return json.dumps(report, indent=2) + "\n"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--final-coverage", action="store_true")
    args = parser.parse_args()
    if not args.write and not args.check:
        parser.error("choose --write or --check")
    if args.final_coverage and not args.check:
        parser.error("--final-coverage requires --check")

    report, errors = build()
    if errors:
        raise SystemExit("\n".join(f"FAIL: {json.dumps(error, sort_keys=True)}" for error in errors))
    expected = serialized(report)
    if args.write:
        OUTPUT_PATH.write_text(expected)
        print(f"WROTE: {OUTPUT_PATH.relative_to(ROOT)}")
    if args.check:
        if not OUTPUT_PATH.exists() or OUTPUT_PATH.read_text() != expected:
            raise SystemExit(f"FAIL: {OUTPUT_PATH.relative_to(ROOT)} is stale; run this script with --write")
        summary = report["summary"]
        print(
            "PASS: species ledger matches "
            f"{summary['closed_encounters']} closed encounters and {summary['used_species']} used species"
        )
        print(f"INFO: {summary['duplicated_species']} species repeat across closed physical encounters")
        print(f"INFO: legendary family status {summary['legendary_families']}")
        print(f"INFO: Mega showcase status {summary['mega_showcases']}")

        if args.final_coverage:
            sequence = load_json(SEQUENCE_PATH)
            if sequence.get("campaign_complete") is not True:
                raise SystemExit("FAIL: canonical sequence does not declare campaign_complete=true")
            open_entries = [entry["encounter_id"] for entry in sequence.get("entries", []) if entry.get("status") != "closed"]
            if open_entries:
                raise SystemExit(f"FAIL: canonical sequence still has {len(open_entries)} open encounters")
            missing_legends = [row["family_id"] for row in report["legendary_families"] if row["status"] != "used"]
            missing_megas = [row["key"] for row in report["mega_showcases"] if row["status"] != "used"]
            if missing_legends or missing_megas:
                raise SystemExit(
                    "FAIL: final coverage remains open: "
                    f"legendary_families={missing_legends}, mega_showcases={missing_megas}"
                )
            print("PASS: final campaign uses every required legendary family and Mega mechanism")


if __name__ == "__main__":
    main()
