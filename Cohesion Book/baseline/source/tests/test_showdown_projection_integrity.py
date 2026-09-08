"""Exact local Circuit projection and pinned importer failure boundaries."""
import hashlib
import json
import re
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'scripts'))
import generate_showdown_champions_circuit as circuit
import generate_showdown_champions_singles as singles
import showdown_import as policy


class ShowdownProjectionIntegrity(unittest.TestCase):
    def test_combined_sources_include_generator_code_and_cannot_be_substituted(self):
        manifest = json.loads(circuit.MANIFEST.read_text())
        for name in [circuit.GEN9_SOURCE_FILE, 'data/random-battles/champions/teams.ts', 'data/random-battles/gen9/teams.ts']:
            self.assertEqual(manifest['source_files'][name], policy.SOURCE_HASHES[name])
            changed = json.loads(json.dumps(manifest))
            changed['source_files'][name] = '0' * 64
            with self.assertRaises(ValueError):
                circuit.validate_manifest(changed)

    def test_supplement_stat_order_and_invalid_budget(self):
        manifest = json.loads(circuit.MANIFEST.read_text())
        authored = json.loads((ROOT / 'data/emerald_champions/emerald_champions_battle_sets.json').read_text())
        original = next(e for e in authored['defaults'] + authored['alternatives']
                        if e['species'] == 'SPECIES_XERNEAS' and e['name'] == 'Champions Geomancy')
        projected = next(t for t in manifest['templates'] if t.get('name') == 'Champions Geomancy')
        self.assertEqual(projected['stat_points'][3], original['stat_points'][5])  # Speed
        self.assertEqual(projected['stat_points'][4], original['stat_points'][3])  # Sp. Atk
        self.assertEqual(projected['item'], 'ITEM_POWER_HERB')
        projected['stat_points'][0] = 33
        with self.assertRaisesRegex(ValueError, 'budget'):
            circuit.validate_manifest(manifest)

    def test_non_mega_forms_retain_their_required_items(self):
        variants = {v['showdown_id']: v for v in json.loads(circuit.MANIFEST.read_text())['variants']}
        for species, item in [('arceusbug', 'ITEM_INSECT_PLATE'),
                              ('zaciancrowned', 'ITEM_RUSTED_SWORD'),
                              ('ogerponhearthflame', 'ITEM_HEARTHFLAME_MASK')]:
            self.assertEqual(variants[species]['required_item'], item)

    def test_local_cli_check_without_upstream_does_not_write(self):
        paths = [circuit.MANIFEST, circuit.C_OUTPUT, circuit.COUNTS_OUTPUT]
        before = {path: (path.read_bytes(), path.stat().st_mtime_ns) for path in paths}
        subprocess.run([sys.executable, str(ROOT / 'scripts/generate_showdown_champions_circuit.py'), '--check'], check=True, capture_output=True, timeout=30)
        self.assertEqual(before, {path: (path.read_bytes(), path.stat().st_mtime_ns) for path in paths})

    def test_exact_record_and_count_drift_rejected_without_writes(self):
        manifest = json.loads(circuit.MANIFEST.read_text())
        with tempfile.TemporaryDirectory() as temp:
            c_output, counts = Path(temp) / 'table.h', Path(temp) / 'counts.h'
            original_c, original_counts = circuit.C_OUTPUT.read_text(), circuit.COUNTS_OUTPUT.read_text()
            c_output.write_text(original_c)
            counts.write_text(original_counts)
            circuit.project(manifest, check=True, c_output=c_output, counts_output=counts)
            for path, original, modified in [
                (c_output, original_c, re.sub(r'\.partySpecies = SPECIES_[A-Z0-9_]+,', '.partySpecies = SPECIES_NONE,', original_c, count=1)),
                (counts, original_counts, re.sub(r'VARIANT_COUNT \d+', 'VARIANT_COUNT 0', original_counts)),
            ]:
                path.write_text(modified)
                before = path.read_bytes(), path.stat().st_mtime_ns
                with self.assertRaisesRegex(ValueError, 'stale'):
                    circuit.project(manifest, check=True, c_output=c_output, counts_output=counts)
                self.assertEqual(before, (path.read_bytes(), path.stat().st_mtime_ns))
                path.write_text(original)

    def test_manifest_count_and_provenance_rejected(self):
        manifest = json.loads(circuit.MANIFEST.read_text())
        for key, value in [('variant_count', manifest['variant_count'] + 1), ('source_commit', 'unverified'), ('source_sha256', '0' * 64)]:
            with self.subTest(key=key), self.assertRaises(ValueError):
                circuit.validate_manifest({**manifest, key: value})

    def test_wrong_commit_and_modified_source_rejected(self):
        with patch.object(policy.subprocess, 'check_output', return_value='wrong\n'):
            with self.assertRaisesRegex(ValueError, 'expected Showdown'):
                policy.verify_checkout(Path('/unused'))
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            relative = circuit.SOURCE_FILE
            path = root / relative
            path.parent.mkdir(parents=True)
            path.write_bytes(b'pinned fixture')
            with patch.dict(policy.SOURCE_HASHES, {relative: hashlib.sha256(path.read_bytes()).hexdigest()}):
                self.assertEqual(policy.read_pinned_source(root, relative), b'pinned fixture')
                path.write_bytes(b'modified fixture')
                with self.assertRaisesRegex(ValueError, 'source drifted'):
                    policy.read_pinned_source(root, relative)
            # Each JSON importer rejects changed bytes before parsing/projection.
            for module, arguments in [(circuit, (root,)), (singles, (root, {'source_file': relative}))]:
                with patch.object(module, 'verify_checkout'):
                    with self.assertRaisesRegex(ValueError, 'source drifted'):
                        module.build(*arguments)

    def test_learnsets_license_points_to_existing_notice(self):
        manifest = json.loads((ROOT / 'data/emerald_champions/showdown_champions_learnsets.json').read_text())
        self.assertEqual(manifest['license'], 'MIT; see THIRD_PARTY_NOTICES.md')
        self.assertTrue((ROOT / 'THIRD_PARTY_NOTICES.md').is_file())

    def test_import_policy_has_one_owner(self):
        self.assertIs(circuit.ABILITY_OVERRIDES, policy.ABILITY_OVERRIDES)
        self.assertIs(singles.ABILITY_OVERRIDES, policy.ABILITY_OVERRIDES)
        self.assertEqual(policy.to_id('Rotom-Fan'), 'rotomfan')
        self.assertEqual(policy.mega_suffix('charizardmegax'), 'megax')
        self.assertIsNone(policy.mega_suffix('meganium'))


if __name__ == '__main__':
    unittest.main()
