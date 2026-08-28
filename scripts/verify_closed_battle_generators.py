#!/usr/bin/env python3
"""Verify closed battle generators against the latest campaign state.

The individual closure generators intentionally model the campaign state at
the instant each battle was closed.  Re-running their full payload comparison
after later battles close rewinds the expected `next` marker and therefore can
never be a valid final-state gate.  This verifier checks the durable outputs
that must remain exact—each design, ledger row, and source implementation—then
checks the latest progression frontier once.
"""

from __future__ import annotations

import importlib
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DESIGNS = json.loads((ROOT / "docs/verdant_bespoke_battle_designs.json").read_text())["designs"]
LEDGER = {
    row["index"]: row
    for row in json.loads((ROOT / "docs/verdant_battle_experience_ledger.json").read_text())["entries"]
}

MODULES = [
    f"emerald_champions_battle{index}"
    for index in (73, 74, 75, 76, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123)
]


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(f"FAIL: {message}")


def verify_entry(module_name: str) -> None:
    module = importlib.import_module(module_name)
    expected_design = module.design()
    expected_ledger = module.ledger_entry()
    encounter_id = expected_ledger["encounter_id"]
    index = expected_ledger["index"]

    require(DESIGNS.get(encounter_id) == expected_design, f"{module_name} design is stale")
    require(LEDGER.get(index) == expected_ledger, f"{module_name} ledger row is stale")
    module.verify_source()
    print(f"PASS: {module_name} durable design, ledger, and source")


def verify_winstrates() -> None:
    module = importlib.import_module("emerald_champions_winstrate_arc")
    for config in module.CONFIGS:
        index = config["index"]
        encounter_id = config["encounter_id"]
        require(DESIGNS.get(encounter_id) == module.design(config, True), f"Winstrate Battle {index} design is stale")
        require(LEDGER.get(index) == module.ledger_entry(config), f"Winstrate Battle {index} ledger row is stale")
    module.verify_source(80)
    print("PASS: Battles 77-80 durable Winstrate designs, ledgers, and source")


def verify_heat_epilogue() -> None:
    module = importlib.import_module("emerald_champions_battles124_133")
    for config in module.CONFIGS:
        require(DESIGNS.get(config["id"]) == module.design(config), f"Heat epilogue Battle {config['index']} design is stale")
        require(LEDGER.get(config["index"]) == module.ledger_entry(config), f"Heat epilogue Battle {config['index']} ledger row is stale")
    module.verify_source(False)
    print("PASS: Battles 124-133 durable Heat Badge epilogue designs, ledgers, and source")


def verify_route111_north() -> None:
    module = importlib.import_module("emerald_champions_battles134_143")
    for config in module.CONFIGS:
        require(DESIGNS.get(config["id"]) == module.design(config), f"Route 111 north Battle {config['index']} design is stale")
        require(LEDGER.get(config["index"]) == module.ledger_entry(config), f"Route 111 north Battle {config['index']} ledger row is stale")
    module.verify_source(False)
    print("PASS: Battles 134-143 durable Route 111 north designs, ledgers, and source")


def verify_frontier() -> None:
    sequence = json.loads((ROOT / "docs/verdant_battle_sequence.json").read_text())["entries"]
    by_index = {row["index"]: row for row in sequence}
    require(all(by_index[index]["status"] == "closed" for index in range(1, 144)), "Battles 1-143 are not all closed")
    require(by_index[144]["status"] == "next", "Battle 144 is not the unique chronological frontier")
    require(sum(row["status"] == "next" for row in sequence) == 1, "sequence does not have exactly one next encounter")

    operating_system = json.loads((ROOT / "docs/emerald_champions_battle_design_operating_system.json").read_text())["current_state"]
    require(operating_system["closed_encounters"] == 143, "operating system closed count is stale")
    require(operating_system["next_index"] == 144, "operating system next index is stale")
    require(operating_system["next_encounter_id"] == by_index[144]["encounter_id"], "operating system frontier ID is stale")
    print("PASS: latest campaign frontier is 143 closed / Battle 144 next")


def main() -> None:
    for module_name in MODULES[:4]:
        verify_entry(module_name)
    verify_winstrates()
    for module_name in MODULES[4:]:
        verify_entry(module_name)
    verify_heat_epilogue()
    verify_route111_north()
    verify_frontier()
    print("PASS: all progressive closure generators remain exact in the latest campaign state")


if __name__ == "__main__":
    main()
