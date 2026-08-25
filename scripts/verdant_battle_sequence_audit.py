#!/usr/bin/env python3
"""Keep Verdant's bespoke work queue complete in physical campaign order."""

from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
INDEX_PATH = ROOT / "docs/verdant_battle_sequence.json"
DESIGNS_PATH = ROOT / "docs/verdant_bespoke_battle_designs.json"
GUIDE_PATH = ROOT / "docs/verdant_battle_guide.json"


def read(path: Path) -> str:
    return path.read_text()


def trainer_for_script(map_name: str, script_name: str) -> set[str]:
    source = read(ROOT / f"data/maps/{map_name}/scripts.inc")
    match = re.search(
        rf"^{re.escape(script_name)}::.*?(?=^\S[^\n]*::|\Z)",
        source,
        re.M | re.S,
    )
    if not match:
        raise ValueError(f"{map_name}: missing event script {script_name}")
    return set(re.findall(r"\bTRAINER_[A-Z0-9_]+\b", match.group(0)))


def object_trainers(map_name: str, predicate) -> set[str]:
    data = json.loads(read(ROOT / f"data/maps/{map_name}/map.json"))
    result: set[str] = set()
    for event in data["object_events"]:
        if event.get("trainer_type") == "TRAINER_TYPE_NONE" or not predicate(event):
            continue
        result.update(trainer_for_script(map_name, event["script"]))
    return result


def ids_in(entries: list[dict], start: int, end: int) -> set[str]:
    return {
        trainer_id
        for entry in entries
        if start <= entry["index"] <= end
        for trainer_id in entry["trainer_ids"]
    }


def main() -> None:
    index = json.loads(read(INDEX_PATH))
    entries = index["entries"]
    designs = json.loads(read(DESIGNS_PATH))["designs"]
    guide = json.loads(read(GUIDE_PATH))
    problems: list[str] = []

    if index.get("source_definition_count") != len(guide["entries"]):
        problems.append(
            f"master guide count drifted: index promises {index.get('source_definition_count')}, "
            f"guide has {len(guide['entries'])}"
        )
    if [entry["index"] for entry in entries] != list(range(1, len(entries) + 1)):
        problems.append("encounter indices are not contiguous")

    encounter_ids = [entry["encounter_id"] for entry in entries]
    if len(encounter_ids) != len(set(encounter_ids)):
        problems.append("duplicate encounter id in canonical sequence")
    all_trainer_ids = [trainer_id for entry in entries for trainer_id in entry["trainer_ids"]]
    if len(all_trainer_ids) != len(set(all_trainer_ids)):
        problems.append("one trainer definition is assigned to multiple encounter indices")

    guide_ids = {entry["trainerId"] for entry in guide["entries"]}
    for trainer_id in all_trainer_ids:
        if trainer_id not in guide_ids:
            problems.append(f"canonical sequence names unknown guide trainer {trainer_id}")

    by_encounter = {entry["encounter_id"]: entry for entry in entries}
    for encounter_id, design in designs.items():
        entry = by_encounter.get(encounter_id)
        if not entry:
            problems.append(f"closed/designing encounter is absent from sequence: {encounter_id}")
            continue
        if entry["index"] != design["guide_order"]:
            problems.append(
                f"{encounter_id}: sequence index {entry['index']} != design order {design['guide_order']}"
            )
        if set(entry["trainer_ids"]) != set(design["trainer_ids"]):
            problems.append(f"{encounter_id}: sequence/source trainer branches differ")
        if design.get("status") == "closed" and entry.get("status") != "closed":
            problems.append(f"{encounter_id}: closed design is not closed in the sequence")

    nonclosed = [entry for entry in entries if entry["status"] != "closed"]
    next_entries = [entry for entry in entries if entry["status"] == "next"]
    if len(next_entries) != 1 or not nonclosed or next_entries[0] is not nonclosed[0]:
        problems.append("exactly the first non-closed encounter must be marked next")

    route102 = object_trainers("Route102", lambda event: True)
    if route102 != ids_in(entries, 2, 5):
        problems.append(f"Route 102 optional coverage drifted: source={sorted(route102)}")

    route104_south = object_trainers("Route104", lambda event: event["y"] >= 38)
    if route104_south != ids_in(entries, 6, 8):
        problems.append(f"Route 104 south optional coverage drifted: source={sorted(route104_south)}")

    woods_sight = object_trainers("PetalburgWoods", lambda event: True)
    woods_source = read(ROOT / "data/maps/PetalburgWoods/scripts.inc")
    if "TRAINER_GRUNT_PETALBURG_WOODS" not in woods_source:
        problems.append("Petalburg Woods mandatory Aqua encounter is missing")
    woods_all = woods_sight | {"TRAINER_GRUNT_PETALBURG_WOODS"}
    if woods_all != ids_in(entries, 9, 11):
        problems.append(f"Petalburg Woods trainer coverage drifted: source={sorted(woods_all)}")

    route104_north = object_trainers("Route104", lambda event: event["y"] < 38)
    if route104_north != ids_in(entries, 12, 15):
        problems.append(f"Route 104 north optional coverage drifted: source={sorted(route104_north)}")

    rustboro_gym = object_trainers("RustboroCity_Gym", lambda event: True) | {"TRAINER_ROXANNE_1"}
    if rustboro_gym != ids_in(entries, 16, 19):
        problems.append(f"Rustboro Gym coverage drifted: source={sorted(rustboro_gym)}")

    route103 = read(ROOT / "data/maps/Route103/scripts.inc")
    for trainer_id in entries[0]["trainer_ids"]:
        if trainer_id not in route103:
            problems.append(f"Route 103 rival branch missing from source: {trainer_id}")

    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))

    closed = sum(entry["status"] == "closed" for entry in entries)
    print(
        f"PASS: canonical encounter index is contiguous through {len(entries)}; "
        f"{closed} closed and Battle {next_entries[0]['index']} is next"
    )
    print("PASS: every opening optional, story, branch, and twin trainer is indexed exactly once")


if __name__ == "__main__":
    main()
