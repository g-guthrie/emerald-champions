#!/usr/bin/env python3
"""Print pinned legal moves, Abilities and types for species (authoring aid).

    python3 scripts/ec_moves.py PARAS SAWK            # full legal move lists
    python3 scripts/ec_moves.py PARAS -- KNOCK_OFF SPORE   # check candidates
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import audit_emerald_champions_master_battles as audit  # noqa: E402

TYPES = audit.species_types()


def abilities() -> dict[str, list[str]]:
    result = {}
    for path in (ROOT / "src/data/pokemon/species_info").glob("*.h"):
        text = path.read_text(errors="ignore")
        for block in re.split(r"\n    \[SPECIES_", text)[1:]:
            name = "SPECIES_" + block.split("]")[0].strip()
            match = re.search(r"\.abilities\s*=\s*\{([^}]*)\}", block)
            if match:
                result[name] = [
                    token.strip()[8:] for token in match.group(1).split(",")
                    if token.strip() and "NONE" not in token
                ]
    return result


def main() -> None:
    args = sys.argv[1:]
    candidates = []
    if "--" in args:
        index = args.index("--")
        candidates = [f"MOVE_{move}" for move in args[index + 1:]]
        args = args[:index]
    ability_table = abilities()
    for name in args:
        species = f"SPECIES_{name}"
        legal = audit.pinned_legal_moves(species)
        types = "/".join(t[5:] for t in TYPES.get(species, ()))
        print(f"== {name} [{types}] abilities={ability_table.get(species)}")
        if candidates:
            for move in candidates:
                print(f"   {'OK ' if move in legal else 'NO '} {move[5:]}")
        else:
            print("   " + ", ".join(sorted(move[5:] for move in legal)))


if __name__ == "__main__":
    main()
