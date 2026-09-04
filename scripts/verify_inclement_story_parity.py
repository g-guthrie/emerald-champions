#!/usr/bin/env python3
"""Selected static story regressions against a required Inclement reference.

Inclement's plot is canonical; Champions modernizes systems, not the story. This gate
catches the class of defect that produced a permanently visible guide sealing Dewford
Gym: a partial story reversal that leaves incompatible halves of two versions behind.

It checks three things against the Inclement baseline:
  A. Objects that lost their story flag and became permanent, so their scene can never
     end (the Dewford guide bug).
  B. Story gates that are still referenced but have lost their script setters and
     may no longer open through those setters. C setters and dynamic script
     behavior are not modeled.
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


def reference_paths() -> set[str]:
    """Resolve the reference before deciding that any individual path is absent."""
    subprocess.check_output(
        ["git", "rev-parse", "--verify", f"{BASELINE}^{{commit}}"],
        cwd=ROOT, text=True, stderr=subprocess.PIPE,
    )
    raw = subprocess.check_output(
        ["git", "ls-tree", "-r", "--name-only", "-z", BASELINE],
        cwd=ROOT, text=True, stderr=subprocess.PIPE,
    )
    paths = set(filter(None, raw.split("\0")))
    if not any(p.startswith("data/maps/") and p.endswith("/map.json") for p in paths):
        raise ValueError("reference contains no map JSON files")
    return paths


def show(path: str, paths: set[str]) -> str | None:
    if path not in paths:
        return None
    # A known path that cannot be read is a reference/tool failure, not a new file.
    return subprocess.check_output(
        ["git", "show", f"{BASELINE}:{path}"], cwd=ROOT, text=True, stderr=subprocess.PIPE
    )


def verify() -> None:
    paths = reference_paths()
    failures: list[str] = []
    compared_maps = new_maps = compared_callbacks = new_scripts = reviewed = 0

    # A. objects that became permanent
    for map_json in sorted((ROOT / "data/maps").glob("*/map.json")):
        name = map_json.parent.name
        if name.endswith("_Frlg"):
            continue
        old_raw = show(f"data/maps/{name}/map.json", paths)
        if old_raw is None:
            new_maps += 1
            continue
        compared_maps += 1
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
            if story_flags:
                if (name, key[0]) in REVIEWED_PERMANENT_OBJECTS:
                    reviewed += 1
                else:
                    failures.append(
                        f"A. {name}: {key[0]} is now permanent; Inclement hid it behind {story_flags[0]}"
                    )
    if not compared_maps:
        raise ValueError("no current maps could be compared with the reference")

    # B. story gates that can never open
    sources = [
        str(p.relative_to(ROOT))
        for p in list((ROOT / "data/maps").rglob("scripts.inc")) + list((ROOT / "data/scripts").glob("*.inc"))
    ]
    current = "\n".join((ROOT / p).read_text(errors="ignore") for p in sources)
    # Include reference scripts deleted from the current tree: their flag setters
    # must not disappear from the comparison merely because the file disappeared.
    baseline_sources = sorted(
        p for p in paths
        if (p.startswith("data/maps/") and p.endswith("/scripts.inc"))
        or (p.startswith("data/scripts/") and p.endswith(".inc") and p.count("/") == 2)
    )
    if not sources or not baseline_sources:
        raise ValueError("current and reference script inventories must both be nonempty")
    baseline = "\n".join(show(p, paths) for p in baseline_sources)
    set_now = set(re.findall(r"(?m)^\s*setflag\s+(FLAG_[A-Z0-9_]+)", current))
    set_before = set(re.findall(r"(?m)^\s*setflag\s+(FLAG_[A-Z0-9_]+)", baseline))
    referenced = set(re.findall(r"\bFLAG_[A-Z0-9_]+\b", current))
    for flag in sorted((set_before - set_now) & referenced):
        if flag not in REVIEWED_DEAD_GATES:
            failures.append(f"B. {flag} remains referenced but lost all inventoried script setflag commands")
        else:
            reviewed += 1

    # C. missing Inclement callbacks
    for scripts in sorted((ROOT / "data/maps").glob("*/scripts.inc")):
        name = scripts.parent.name
        old_raw = show(f"data/maps/{name}/scripts.inc", paths)
        if old_raw is None:
            new_scripts += 1
            continue
        compared_callbacks += 1
        hooks = lambda text: set(re.findall(r"map_script\s+(MAP_SCRIPT_[A-Z_]+)", text))
        missing = hooks(old_raw) - hooks(scripts.read_text(errors="ignore"))
        if missing and name not in REVIEWED_REMOVED_CALLBACKS:
            failures.append(f"C. {name} lost Inclement callback(s): {', '.join(sorted(missing))}")
        elif missing:
            reviewed += 1

    if not compared_callbacks:
        raise ValueError("no current map scripts could be compared with the reference")
    removed_maps = sum(
        p.startswith("data/maps/") and p.endswith("/map.json")
        and not p.split("/")[-2].endswith("_Frlg") and not (ROOT / p).is_file()
        for p in paths
    )
    removed_scripts = sum(
        p.startswith("data/maps/") and not (ROOT / p).is_file()
        for p in baseline_sources
    )
    print(
        f"Coverage: {compared_maps} map object comparisons; {compared_callbacks} callback comparisons; "
        f"{len(sources)} current and {len(baseline_sources)} reference scripts scanned for flag setters; "
        f"{new_maps} new maps and {new_scripts} new map scripts have no historical counterpart; "
        f"{removed_maps} removed maps and {removed_scripts} removed map scripts are outside object/callback comparison"
    )

    if failures:
        print(f"INCLEMENT STORY PARITY: FAIL ({len(failures)})")
        for failure in failures:
            print(f"  - {failure}")
        sys.exit(1)
    print(f"INCLEMENT STORY PARITY: PASS ({reviewed} observed reviewed divergences, baseline {BASELINE[:8]}; selected static checks only)")


def main() -> None:
    try:
        verify()
    except (OSError, subprocess.CalledProcessError, ValueError, KeyError) as exc:
        print(f"INCLEMENT STORY PARITY: FAIL (required input/reference unavailable or invalid: {exc})")
        sys.exit(1)


if __name__ == "__main__":
    main()
