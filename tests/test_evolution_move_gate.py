import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'scripts'))
import evolution_move_gate as gate
from reference_pool import Pools
from tuning_pool_check import check


class EvolutionMoveGateTests(unittest.TestCase):
    def test_natural_level_boundaries(self):
        for species, move, level in [('YANMA', 'ANCIENT_POWER', 33), ('AIPOM', 'DOUBLE_HIT', 32),
                                     ('STEENEE', 'STOMP', 28), ('MIME_JR', 'MIMIC', 32),
                                     ('DUNSPARCE', 'HYPER_DRILL', 32), ('PRIMEAPE', 'RAGE_FIST', 35),
                                     ('LICKITUNG', 'ROLLOUT', 6), ('PILOSWINE', 'ANCIENT_POWER', 1)]:
            with self.subTest(species=species):
                species, move = 'SPECIES_' + species, 'MOVE_' + move
                self.assertEqual(gate.natural_level(species, move), level)
                self.assertFalse(gate.ready(species, move, level - 1, ['FLAG_BADGE04_GET']))
                self.assertTrue(gate.ready(species, move, level, []))

    def test_absent_natural_move_needs_fourth_badge(self):
        self.assertIsNone(gate.natural_level('SPECIES_STANTLER', 'MOVE_PSYSHIELD_BASH'))
        self.assertFalse(gate.ready('SPECIES_STANTLER', 'MOVE_PSYSHIELD_BASH', 100, ['FLAG_BADGE03_GET']))
        self.assertTrue(gate.ready('SPECIES_STANTLER', 'MOVE_PSYSHIELD_BASH', 1, ['FLAG_BADGE04_GET']))

    def test_only_evolution_triggers_are_gated(self):
        self.assertTrue(gate.ready('SPECIES_YANMA', 'MOVE_PROTECT', 1, []))
        self.assertTrue(gate.ready('SPECIES_YANMEGA', 'MOVE_ANCIENT_POWER', 1, []))

    def test_pool_evolution_checks_gate(self):
        args = ('EVO_LEVEL', '0', [('IF_KNOWS_MOVE', 'MOVE_ANCIENT_POWER')], 30, {}, lambda s: False, lambda m: False, lambda m: False)
        self.assertFalse(Pools.evo_ok(*args, evolution_move_ready=lambda m: gate.ready('SPECIES_YANMA', m, 30, []))[0])
        self.assertTrue(Pools.evo_ok(*args, evolution_move_ready=lambda m: gate.ready('SPECIES_YANMA', m, 40, []))[0])

    def test_legality_rejects_locked_move(self):
        party = {'availability_audit': 'test', 'party': [{'species': 'SPECIES_YANMA', 'moves': ['MOVE_ANCIENT_POWER'],
                 'availability': 'test', 'evs': [252, 0, 0, 252, 0, 6]}]}
        pool = {'cap': 30, 'milestone': 'badge2', 'species': [{'species': 'SPECIES_YANMA'}], 'items': [], 'megas': []}
        self.assertTrue(any('evolution trigger locked' in p for p in check(party, pool)))
        pool['cap'] = 40
        self.assertEqual(check(party, pool), [])
