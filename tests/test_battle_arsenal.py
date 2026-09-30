"""Acquisition regressions that reject fake availability and illegal preparation."""
from pathlib import Path
import sys
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'scripts'))
import battle_arsenal as arsenal
import battle_checkpoint_lab as checkpoint


class AcquisitionTests(unittest.TestCase):
    def closure(self, initial, rows, **kwargs):
        with patch.object(arsenal, 'check_evidence'):
            return arsenal.acquisition_states(initial, rows, **kwargs)

    def test_optional_routes_unlock_another_route_and_evolution_chain(self):
        rows = [
            {'id': 'route-a-item', 'status': 'verified', 'requires': ['gate-open'],
             'grants': {'ITEM_STONE': 1}, 'facts': ['stone-found']},
            {'id': 'route-b-trade', 'status': 'verified', 'requires': ['gate-open'],
             'consumes': {'SPECIES_SEED': 1}, 'grants': {'SPECIES_BASE': 1}},
            {'id': 'evolve', 'status': 'verified', 'consumes': {'SPECIES_BASE': 1, 'ITEM_STONE': 1},
             'grants': {'SPECIES_EVOLVED': 1}, 'facts': ['third-route-open']},
            {'id': 'third-route', 'status': 'verified', 'requires': ['third-route-open'],
             'grants': {'ITEM_MEGA_STONE': 1}},
        ]
        data = self.closure({'facts': ['gate-open'], 'resources': {'SPECIES_SEED': 1}}, rows)
        self.assertTrue(any(s['resources'].get('SPECIES_EVOLVED') and s['resources'].get('ITEM_MEGA_STONE') for s in data['states']))
        self.assertFalse(any(s['resources'].get('SPECIES_EVOLVED') and s['resources'].get('ITEM_STONE') for s in data['states']))

    def test_exclusive_choices_never_become_one_available_team(self):
        rows = [
            {'id': 'starter-a', 'status': 'verified', 'forbids': ['starter-chosen'],
             'facts': ['starter-chosen'], 'grants': {'SPECIES_A': 1}},
            {'id': 'starter-b', 'status': 'verified', 'forbids': ['starter-chosen'],
             'facts': ['starter-chosen'], 'grants': {'SPECIES_B': 1}},
        ]
        data = self.closure({}, rows)
        self.assertFalse(any({'SPECIES_A', 'SPECIES_B'} <= set(s['resources']) for s in data['states']))

    def test_target_rewards_cannot_certify_the_target_battle(self):
        rows = [
            {'id': 'target-win', 'status': 'verified', 'grants': {'ITEM_REWARD': 1}},
            {'id': 'dependent', 'status': 'verified', 'consumes': {'ITEM_REWARD': 1},
             'grants': {'SPECIES_COUNTER': 1}},
        ]
        data = self.closure({}, rows, excluded=['target-win'])
        self.assertFalse(any(s['resources'] for s in data['states']))

    def test_unknown_native_provider_is_unresolved_not_free_stock(self):
        data = self.closure({}, [{'id': 'unknown-shop', 'status': 'unresolved',
                                 'grants': {'ITEM_ALL_STONES': 99}}])
        self.assertEqual(data['unresolved'], ['unknown-shop'])
        self.assertEqual(data['states'][0]['resources'], {})

    def test_changed_evidence_is_rejected(self):
        with self.assertRaisesRegex(ValueError, 'stale'):
            arsenal.check_evidence([{'path': 'src/caps.c', 'sha256': '0' * 64}])

    def test_unproven_party_never_reaches_native_preparation(self):
        with self.assertRaisesRegex(ValueError, 'unresolved'):
            arsenal.validate_party({'legality_status': 'unresolved'}, {})

    def test_campaign_access_and_identity_are_required_before_preparation(self):
        scenario = {'legality_status': 'proven', 'source_fingerprint': 'snapshot',
                    'producer': {'kind': 'campaign'}}
        with patch('battle_campaign_arsenal.certify_scenario') as certify:
            with self.assertRaisesRegex(ValueError, 'battle access is unresolved'):
                arsenal.validate_party(scenario, {}, internal_fingerprint='snapshot')
            scenario['battle_access_status'] = 'proven'
            with self.assertRaisesRegex(ValueError, 'opponent identity is unresolved'):
                arsenal.validate_party(scenario, {}, internal_fingerprint='snapshot')
            self.assertEqual(certify.call_count, 2)

    def test_legacy_ring_never_grants_unowned_mega_stones_or_evolution_items(self):
        item_names = checkpoint.explicit_enum_values(ROOT / 'include/constants/items.h', 'ITEM_')
        item_ids = {name: number for number, name in item_names.items()}
        inventory = [{'item_id': item_ids[name], 'quantity': 1}
                     for name in ['ITEM_MEGA_RING', 'ITEM_AERODACTYLITE']]
        candidate, _ = checkpoint.materialize_legal_arsenal([], 14, [], [], inventory)
        self.assertEqual(candidate['legality_status'], 'unresolved')
        self.assertTrue(candidate['mega_access'])
        self.assertEqual(candidate['mega_stones'], ['ITEM_AERODACTYLITE'])
        self.assertEqual(candidate['evolution_items'], [])
        self.assertNotIn('ITEM_VENUSAURITE', candidate['held_items'])


if __name__ == '__main__':
    unittest.main()
