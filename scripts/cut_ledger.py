#!/usr/bin/env python3
"""Generated CUT LEDGER: the pacing-cut retirement receipt for section 4's
CUT POLICY paragraph.

Source of truth is data/emerald_champions/retired_battles.json, which was
built one time from work/trainer-85-100-implementation/cut-reconciliation.json
(first_pass groups + the JSON's own additional-retirements list) plus two
adopted retirements not present in that reconciliation file (E0126 Angelina's
branch, E0133 Eric), recovered from this repo's prior hand-authored ledger
text and git history -- see the "_source" field in that data file.

This module only renders that committed data file; it does not re-derive
retirements from source. The source roster verifier cross-checks the
data file's identities against active authored teams, nonempty native
parties and Hoenn battle opcodes (see check_cut_ledger there), and against
the CUT POLICY paragraph's stated retired-identity count.
"""
from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DATA = ROOT / "data/emerald_champions/retired_battles.json"


def load() -> dict:
    return json.loads(DATA.read_text())


def render_lines() -> tuple[list[str], dict]:
    data = load()
    retired = data["retired_battles"]
    additional = data["additional_retirements"]
    total_identities = data["total_retired_identities"]
    total_groups = data["total_retired_groups"]

    lines = [
        "\nCUT LEDGER (group | map | retired identities | tier)",
        f"{total_groups} retired authoring groups / {total_identities} retired identities, generated from "
        "data/emerald_champions/retired_battles.json. Civilian actors and dialogue for every retired "
        "identity remain where authored; only the battle loadout and sight-battle trigger are removed.",
    ]
    for row in retired:
        ids = ", ".join(row["identities"])
        lines.append(f"{row['group']} | {row['map']} | {ids} | {row['tier']}")

    parts = []
    for row in additional:
        ids = ", ".join(row["identities"])
        parts.append(f"{row['group']} {ids} -- {row['reason']}")
    lines.append("\nAdditional adopted retirements: " + " ".join(parts))
    return lines, dict(retired_battles=retired, additional_retirements=additional,
                        total_retired_groups=total_groups, total_retired_identities=total_identities)


def generate(root: Path = ROOT) -> tuple[list[str], dict, set[Path]]:
    lines, catalog = render_lines()
    paths = {Path(__file__), DATA}
    return lines, catalog, paths


def main() -> None:
    lines, catalog, _paths = generate()
    print("\n".join(lines))
    print(f"\nPASS: {catalog['total_retired_groups']} retired groups; "
          f"{catalog['total_retired_identities']} retired identities")


if __name__ == "__main__":
    main()
