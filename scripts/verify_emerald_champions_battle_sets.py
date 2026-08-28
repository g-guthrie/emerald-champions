#!/usr/bin/env python3
"""Verify generated battle-set data and current wild-table coverage."""

from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def main() -> None:
    manifest = json.loads((ROOT / "docs" / "emerald_champions_battle_sets.json").read_text())
    defaults = manifest["defaults"]
    alternatives = manifest["alternatives"]
    entries = defaults + alternatives

    assert manifest["source_commit"] == "33202c162ebc34a1dbe2000acd26b0720baa109d"
    assert manifest["default_count"] == len(defaults) == 1143
    assert manifest["alternative_count"] == len(alternatives) == 166
    assert manifest["set_count"] == len(entries) == 1309
    assert len({entry["species"] for entry in defaults}) == len(defaults)

    for entry in entries:
        assert 1 <= len(entry["moves"]) <= 4, entry["species"]
        assert len(entry["moves"]) == len(set(entry["moves"])), entry["species"]
        assert len(entry["name"]) <= 23, entry["name"]
        assert len(entry["stat_points"]) == 6
        assert sum(entry["stat_points"]) == 66, entry["species"]
        assert max(entry["stat_points"]) <= 32, entry["species"]
        assert not entry["item"].endswith("ITE") or entry["item"] == "ITEM_EVIOLITE", entry["item"]

    wild_text = (ROOT / "src" / "data" / "wild_encounters.json").read_text()
    wild_species = set(re.findall(r"SPECIES_[A-Z0-9_]+", wild_text))
    default_species = {entry["species"] for entry in defaults}
    missing = sorted(wild_species - default_species)
    assert not missing, f"Current wild tables lack presets: {missing}"

    generated = (ROOT / "src" / "data" / "pokemon" / "emerald_champions_battle_sets.h").read_text()
    assert generated.count(".statPoints =") == 1309
    assert "gEmeraldChampionsDefaultBattleSets[NUM_SPECIES]" in generated
    assert "gEmeraldChampionsBattleSetAlternatives[]" in generated

    print("battle_set_static_checks=PASS")
    print(f"sets={len(entries)}")
    print(f"wild_species_with_presets={len(wild_species)}")


if __name__ == "__main__":
    main()
