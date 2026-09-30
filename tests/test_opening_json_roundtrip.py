"""Exported acquisition certificates retain identity after sorted JSON storage."""
import copy
import json
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'scripts'))
import battle_opening_arsenal as opening


class SortedCertificateTests(unittest.TestCase):
    def test_sorted_evolution_mapping_preserves_certificate_and_rejects_forged_grant(self):
        fingerprint = 'internal-frozen-source-test'
        scenario = opening._opening_scenario('hard', generation=3, first=0, second=1,
            captures=['SPECIES_EEVEE', 'SPECIES_WURMPLE', 'SPECIES_SCATTERBUG', 'SPECIES_PACHIRISU'],
            evolutions={'SPECIES_EEVEE': 'SPECIES_SYLVEON', 'SPECIES_WURMPLE': 'SPECIES_DUSTOX',
                        'SPECIES_SCATTERBUG': 'SPECIES_VIVILLON'}, _fingerprint=fingerprint)
        stored = json.loads(json.dumps(scenario, sort_keys=True))
        opening.certify_scenario(stored, internal_fingerprint=fingerprint)
        self.assertEqual([p['base_species'] for p in stored['acquisition_states'][0]['evolution_proofs']],
                         ['SPECIES_EEVEE', 'SPECIES_WURMPLE', 'SPECIES_SCATTERBUG'])
        forged = copy.deepcopy(stored)
        forged['acquisition_states'][0]['resources']['SPECIES_DUSTOX'] += 1
        with self.assertRaisesRegex(ValueError, 'acquisition_states'):
            opening.certify_scenario(forged, internal_fingerprint=fingerprint)


if __name__ == '__main__':
    unittest.main()
