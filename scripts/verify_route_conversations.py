#!/usr/bin/env python3
"""Verify intentionally retired route fights remain reachable conversations."""
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def verify(root: Path) -> list[str]:
    ledger = json.loads((root / "data/emerald_champions/route_conversations.json").read_text())
    master = (root / "data/emerald_champions/emerald_champions_master_battle_design.txt").read_text()
    parties = (root / "src/data/trainers.party").read_text()
    scripts = "\n".join(path.read_text() for path in (root / "data/maps").glob("*/scripts.inc"))
    active_species = set(re.findall(r"^  \d+\. (SPECIES_\w+)", master, re.M))
    errors = []
    seen = set()
    for row in ledger["retired"]:
        trainer = row["trainer"]
        if trainer in seen:
            errors.append(f"{trainer}: duplicate retirement")
        seen.add(trainer)
        if re.search(rf"^--- BRANCH {trainer} ---$", master, re.M):
            errors.append(f"{trainer}: retired battle remains in active master")
        if re.search(rf"^=== {trainer} ===$", parties, re.M):
            errors.append(f"{trainer}: retired party still compiled")
        if re.search(rf"^\s*trainerbattle\w*\s+{trainer}\b", scripts, re.M):
            errors.append(f"{trainer}: retired battle can still be invoked")
        missing = set(row["retained_species"]) - active_species
        if missing:
            errors.append(f"{trainer}: species lost from retained battles: {sorted(missing)}")
        folder = root / "data/maps" / row["map"]
        objects = json.loads((folder / "map.json").read_text())["object_events"]
        source = (folder / "scripts.inc").read_text()
        for number, original in zip(row["object_ids"], row["original_objects"], strict=True):
            expected = {**original, "trainer_type": "TRAINER_TYPE_NONE", "trainer_sight_or_berry_tree_id": "0"}
            if number < 1 or number > len(objects) or objects[number - 1] != expected:
                errors.append(f"{trainer}: object {number} lost its identity or still triggers sight battles")
        for script in row["scripts"]:
            block = re.search(rf"^{script}::?\n(.*?)(?=^\w+::?\n|\Z)", source, re.M | re.S)
            message = re.fullmatch(r"\s*msgbox (\w+), MSGBOX_NPC\s+end\s*", block[1]) if block else None
            if not message:
                errors.append(f"{trainer}: {script} is not a terminal NPC conversation")
                continue
            text = re.search(rf"^{message[1]}:\n(.*?)(?=^\w+::?\n|\Z)", source, re.M | re.S)
            if not text or not re.search(r'\.string "[^"\n]+', text[1]) or '$"' not in text[1]:
                errors.append(f"{trainer}: conversation text is missing or unterminated")
    return errors


if __name__ == "__main__":
    errors = verify(ROOT)
    if errors:
        raise SystemExit("\n".join(errors))
    print("PASS: retired route fights preserve NPC placement, dialogue and species showcases")
