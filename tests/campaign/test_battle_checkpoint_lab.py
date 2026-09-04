#!/usr/bin/env python3
from __future__ import annotations

import importlib.util
import json
from pathlib import Path
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[2]
MODULE = ROOT / "scripts/battle_checkpoint_lab.py"
SPEC = importlib.util.spec_from_file_location("battle_checkpoint_lab", MODULE)
assert SPEC and SPEC.loader
lab = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(lab)


class BattleCheckpointLabTests(unittest.TestCase):
    def test_indexes_every_declared_campaign_trainer_encounter(self) -> None:
        manifest = lab.campaign.load_manifest(lab.DEFAULT_MANIFEST)
        recipes = lab.load_json(lab.DEFAULT_RECIPES)
        with tempfile.TemporaryDirectory() as raw:
            index = lab.checkpoint_index(manifest, recipes, Path(raw))
        self.assertEqual(index["campaign_trainer_encounter_count"], len(lab.trainer_segments(manifest)))
        self.assertEqual(index["authored_scope"]["physical_encounters"], 513)
        self.assertEqual(index["authored_scope"]["trainer_branches"], 561)
        self.assertTrue(any(row["recipe"] == "route102-calvin" for row in index["encounters"]))

    def test_checkpoint_validator_rejects_autowin(self) -> None:
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            state, shot = root / "state.ss1", root / "shot.png"
            state.write_bytes(b"state")
            shot.write_bytes(b"png")
            payload = {
                "schema_version": 1, "encounter": {}, "campaign": {}, "identity": {},
                "checkpoint": {"state": str(state), "state_sha256": lab.sha256(state), "screenshot": str(shot), "screenshot_sha256": lab.sha256(shot)},
                "player": {"party": [], "inventory": []}, "services": {},
                "launch": {"campaign_autowin_disabled": False},
            }
            path = root / "checkpoint.json"
            path.write_text(json.dumps(payload))
            with self.assertRaises(lab.LabError):
                lab.validate_checkpoint(path)

    def test_calvin_arsenal_is_source_derived_and_cap_limited(self) -> None:
        manifest = lab.campaign.load_manifest(lab.DEFAULT_MANIFEST)
        maps = lab.world_reachable_maps(manifest, "main-05-route102-calvin")
        self.assertIn("MAP_OLDALE_TOWN_POKEMON_CENTER_1F", maps)
        services, _ = lab.center_services(maps)
        methods = {
            "land_mons": {"available": True, "evidence": "walking"},
            "hidden_mons": {"available": False, "evidence": "DexNav disabled"},
            "water_mons": {"available": False, "evidence": "Surf locked"},
            "rock_smash_mons": {"available": False, "evidence": "Rock Smash locked"},
            "old_rod": {"available": False, "evidence": "rod absent"},
            "good_rod": {"available": False, "evidence": "rod absent"},
            "super_rod": {"available": False, "evidence": "rod absent"},
        }
        all_methods = {name: {"available": True, "evidence": "test fixture"} for name in methods}
        unfiltered, _ = lab.wild_arsenal(maps, all_methods)
        unfiltered_species = {row["species"] for row in unfiltered}
        self.assertTrue({"SPECIES_AZUMARILL", "SPECIES_DRATINI", "SPECIES_BINACLE"} <= unfiltered_species)
        arsenal, sources = lab.materialize_legal_arsenal(
            maps, 14, [{"species_id": 4, "slot": 0}, {"species_id": 179, "slot": 1}], services, [], methods)
        species = {row["species"] for row in arsenal["pokemon"]}
        self.assertTrue({"SPECIES_CHARMANDER", "SPECIES_MAREEP", "SPECIES_LOTAD", "SPECIES_PIDGEY"} <= species)
        self.assertFalse({"SPECIES_CHARMELEON", "SPECIES_TREECKO", "SPECIES_RAYQUAZA",
                          "SPECIES_AZUMARILL", "SPECIES_DRATINI", "SPECIES_BINACLE"} & species)
        self.assertIn("ITEM_FOCUS_SASH", arsenal["held_items"])
        self.assertFalse(arsenal["mega_access"])
        self.assertEqual(arsenal["mega_stones"], [])
        charmander = next(row for row in arsenal["pokemon"] if row["species"] == "SPECIES_CHARMANDER")
        self.assertIn("ABILITY_BLAZE", charmander["abilities"])
        self.assertIn("protect", charmander["legal_moves"])
        self.assertTrue(charmander["presets"])
        self.assertGreater(len(sources), 20)


if __name__ == "__main__":
    unittest.main()
