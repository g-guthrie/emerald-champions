#!/usr/bin/env python3
"""Inclement Emerald story parity gate.

Inclement's plot is canonical; Champions modernizes systems, not the story. This gate
catches the class of defect that produced a permanently visible guide sealing Dewford
Gym: a partial story reversal that leaves incompatible halves of two versions behind.

It checks three things against the Inclement baseline:
  A. Objects that lost their story flag and became permanent, so their scene can never
     end (the Dewford guide bug).
  B. Story gates that are still referenced but are no longer set anywhere, so they can
     never open.
  C. Inclement map callbacks that disappeared during migration.

Deliberate Champions divergences are listed as reviewed exceptions with a reason, so an
intentional change is recorded once rather than re-argued forever.
"""
from __future__ import annotations

import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BASELINE = "cf41a95b68a39ca74fefeb934c460f6f47eb0b3b"  # Inclement Emerald v1.13

# A. Frontier Brains are ordinary overworld trainers in Champions, so they no longer
#    wait on symbol flags.
REVIEWED_PERMANENT_OBJECTS = {
    ("FortreeCity_Mart", "OBJ_EVENT_GFX_SPENSER"): "Spenser is a Champions overworld trainer",
    ("LavaridgeTown_PokemonCenter_1F", "OBJ_EVENT_GFX_LUCY"): "Lucy is a Champions overworld trainer",
    ("SlateportCity", "OBJ_EVENT_GFX_GRETA"): "Greta is a Champions overworld trainer",
}

# B. Champions makes legendaries capture-only through Legendary Signs, so nothing is
#    ever recorded as having been defeated.
REVIEWED_DEAD_GATES = {
    "FLAG_DEFEATED_DEOXYS": "legendaries are capture-only in Champions",
    "FLAG_DEFEATED_HO_OH": "legendaries are capture-only in Champions",
    "FLAG_DEFEATED_LATIAS_OR_LATIOS": "legendaries are capture-only in Champions",
    "FLAG_DEFEATED_LUGIA": "legendaries are capture-only in Champions",
    "FLAG_DEFEATED_MEW": "legendaries are capture-only in Champions",
}

# C. Callbacks Champions deliberately dropped.
REVIEWED_REMOVED_CALLBACKS = {
    "AlteringCave_B1F": "capture-only legendary; no post-battle cleanup",
    "CaveOfOrigin_DianciesRoom": "capture-only legendary; no post-battle cleanup",
    "EmberPath": "capture-only legendary, and landmark flags were removed",
    "MeteorFalls_JirachisRoom": "capture-only legendary; no post-battle cleanup",
    "ScorchedSlab_HeatransRoom": "capture-only legendary; no post-battle cleanup",
    "SealedChamber_InnerRoom": "capture-only legendary; no post-battle cleanup",
    "ShoalCave_LowTideIceRoom": "capture-only legendary; no post-battle cleanup",
    "DewfordManor_1F": "landmark flags were removed from the build",
    "DewfordMeadow": "landmark flags were removed from the build",
    "Seaspray_Cave": "landmark flags were removed from the build",
    "SandstrewnRuins": "landmark flags removed; Champions lets the player take every fossil",
    "Route109": "FLAG_VISITED_SLATEPORT_CITY is set by Slateport itself",
    "MossdeepCity_House1": "Cynthia is triggered by talking to her, not an entry cutscene",
    "FallarborTown_MoveRelearnersHouse": "Ivy and Evie introduce themselves on first contact instead",
}


def show(path: str) -> str | None:
    try:
        return subprocess.check_output(
            ["git", "show", f"{BASELINE}:{path}"], cwd=ROOT, text=True, stderr=subprocess.DEVNULL
        )
    except subprocess.CalledProcessError:
        return None


def main() -> None:
    failures: list[str] = []

    # A. objects that became permanent
    for map_json in sorted((ROOT / "data/maps").glob("*/map.json")):
        name = map_json.parent.name
        if name.endswith("_Frlg"):
            continue
        old_raw = show(f"data/maps/{name}/map.json")
        if old_raw is None:
            continue
        old, new = json.loads(old_raw), json.loads(map_json.read_text())
        was: dict[tuple, list[str]] = {}
        for event in old.get("object_events") or []:
            was.setdefault((event.get("graphics_id"), event.get("script")), []).append(
                str(event.get("flag", "0"))
            )
        for event in new.get("object_events") or []:
            key = (event.get("graphics_id"), event.get("script"))
            if str(event.get("flag", "0")) not in ("0", "", "FLAG_0"):
                continue
            story_flags = [f for f in was.get(key, []) if f not in ("0", "", "FLAG_0")]
            if story_flags and (name, key[0]) not in REVIEWED_PERMANENT_OBJECTS:
                failures.append(
                    f"A. {name}: {key[0]} is now permanent; Inclement hid it behind {story_flags[0]}"
                )

    # B. story gates that can never open
    sources = [
        str(p.relative_to(ROOT))
        for p in list((ROOT / "data/maps").rglob("scripts.inc")) + list((ROOT / "data/scripts").glob("*.inc"))
    ]
    current = "\n".join((ROOT / p).read_text(errors="ignore") for p in sources)
    baseline = "\n".join(filter(None, (show(p) for p in sources)))
    set_now = set(re.findall(r"(?m)^\s*setflag\s+(FLAG_[A-Z0-9_]+)", current))
    set_before = set(re.findall(r"(?m)^\s*setflag\s+(FLAG_[A-Z0-9_]+)", baseline))
    referenced = set(re.findall(r"\bFLAG_[A-Z0-9_]+\b", current))
    for flag in sorted((set_before - set_now) & referenced):
        if flag not in REVIEWED_DEAD_GATES:
            failures.append(f"B. {flag} is still tested but is never set, so its gate can never open")

    # C. missing Inclement callbacks
    for scripts in sorted((ROOT / "data/maps").glob("*/scripts.inc")):
        name = scripts.parent.name
        old_raw = show(f"data/maps/{name}/scripts.inc")
        if old_raw is None:
            continue
        hooks = lambda text: set(re.findall(r"map_script\s+(MAP_SCRIPT_[A-Z_]+)", text))
        missing = hooks(old_raw) - hooks(scripts.read_text(errors="ignore"))
        if missing and name not in REVIEWED_REMOVED_CALLBACKS:
            failures.append(f"C. {name} lost Inclement callback(s): {', '.join(sorted(missing))}")

    if failures:
        print(f"INCLEMENT STORY PARITY: FAIL ({len(failures)})")
        for failure in failures:
            print(f"  - {failure}")
        sys.exit(1)
    reviewed = len(REVIEWED_PERMANENT_OBJECTS) + len(REVIEWED_DEAD_GATES) + len(REVIEWED_REMOVED_CALLBACKS)
    print(f"INCLEMENT STORY PARITY: PASS ({reviewed} reviewed divergences, baseline {BASELINE[:8]})")


if __name__ == "__main__":
    main()
