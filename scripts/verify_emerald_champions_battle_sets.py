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
    assert manifest["default_count"] == len(defaults) == 1258
    assert manifest["alternative_count"] == len(alternatives) == 203
    assert manifest["set_count"] == len(entries) == 1461
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
    wild_species_for_sets = {
        "SPECIES_GIMMIGHOUL" if species == "SPECIES_GIMMIGHOUL_CHEST" else species
        for species in wild_species
    }
    missing = sorted(wild_species_for_sets - default_species)
    assert not missing, f"Current wild tables lack presets: {missing}"

    generated = (ROOT / "src" / "data" / "pokemon" / "emerald_champions_battle_sets.h").read_text()
    assert generated.count(".statPoints =") == 1461
    assert "gEmeraldChampionsDefaultBattleSets[NUM_SPECIES]" in generated
    assert "gEmeraldChampionsBattleSetAlternatives[]" in generated

    starter_species = {
        "SPECIES_GROOKEY", "SPECIES_SCORBUNNY", "SPECIES_SOBBLE",
        "SPECIES_SPRIGATITO", "SPECIES_FUECOCO", "SPECIES_QUAXLY",
        "SPECIES_THWACKEY", "SPECIES_RABOOT", "SPECIES_DRIZZILE",
        "SPECIES_FLORAGATO", "SPECIES_CROCALOR", "SPECIES_QUAXWELL",
        "SPECIES_RILLABOOM", "SPECIES_CINDERACE", "SPECIES_INTELEON",
        "SPECIES_MEOWSCARADA", "SPECIES_SKELEDIRGE", "SPECIES_QUAQUAVAL",
    }
    assert starter_species <= default_species, f"Gen 8/9 starter presets missing: {sorted(starter_species - default_species)}"

    sign_text = (ROOT / "src" / "data" / "pokemon" / "legendary_signs.h").read_text()
    sign_species = {
        "SPECIES_" + species
        for species in re.findall(r"(?:WILD|VISIBLE|OTHER)_SIGN\([^,]+,\s*([A-Z0-9_]+)", sign_text)
    }
    assert sign_species <= default_species, f"Legendary roots lack presets: {sorted(sign_species - default_species)}"

    modern_campaign_species = {
        "SPECIES_BASCULIN_WHITE_STRIPED", "SPECIES_CAPSAKID", "SPECIES_CHARCADET",
        "SPECIES_FLITTLE", "SPECIES_GIMMIGHOUL", "SPECIES_GREAVARD",
        "SPECIES_GROWLITHE_HISUI", "SPECIES_ORTHWORM", "SPECIES_POLTCHAGEIST",
        "SPECIES_QWILFISH_HISUI", "SPECIES_SNEASEL_HISUI", "SPECIES_TADBULB",
        "SPECIES_TANDEMAUS", "SPECIES_TAUROS_PALDEA_AQUA",
        "SPECIES_TAUROS_PALDEA_BLAZE", "SPECIES_TAUROS_PALDEA_COMBAT",
        "SPECIES_TINKATINK", "SPECIES_ZORUA_HISUI",
    }
    set_counts = {
        species: sum(entry["species"] == species for entry in entries)
        for species in modern_campaign_species
    }
    assert all(count >= 2 for count in set_counts.values()), set_counts

    pinned = json.loads((ROOT / "docs" / "showdown_champions_learnsets.json").read_text())["learnsets"]
    showdown_ids = {
        "SPECIES_GIMMIGHOUL": "gimmighoul",
        **{
            species: re.sub(r"[^a-z0-9]", "", species.removeprefix("SPECIES_").lower())
            for species in modern_campaign_species
            if species != "SPECIES_GIMMIGHOUL"
        },
    }
    move_ids = {
        move: re.sub(r"[^a-z0-9]", "", move.removeprefix("MOVE_").lower())
        for move in set(re.findall(r"MOVE_[A-Z0-9_]+", (ROOT / "include/constants/moves.h").read_text()))
    }
    for entry in entries:
        if entry["species"] not in modern_campaign_species:
            continue
        legal = set(pinned[showdown_ids[entry["species"]]])
        illegal = {move for move in entry["moves"] if move_ids[move] not in legal}
        assert not illegal, (entry["species"], sorted(illegal))

    runtime = (ROOT / "src" / "emerald_champions_battle_sets.c").read_text()
    assert "RandomUniform(RNG_NONE, 0, count - 1)" in runtime
    assert "ResolveBattleSetSpecies" in runtime and "formSpeciesIdTable" in runtime

    handbook_species = {
        entry["species"] for entry in defaults
        if entry["source"].startswith("Pokemon Champions doubles handbook")
    }
    assert len(handbook_species) == 80, len(handbook_species)

    print("battle_set_static_checks=PASS")
    print(f"sets={len(entries)}")
    print(f"wild_species_with_presets={len(wild_species)}")
    print(f"new_campaign_species_with_two_sets={len(modern_campaign_species)}")


if __name__ == "__main__":
    main()
