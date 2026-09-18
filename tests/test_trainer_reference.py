"""Trainer projection must preserve authored moves."""
import sys
from pathlib import Path
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))
import emerald_champions_teams as teams
import export_trainer_catalogue as export

class TrainerReferenceTests(unittest.TestCase):
    def test_native_comparison_rejects_a_move_changed_only_in_projection(self):
        branches=teams.read_teams();parties=export.parse_parties()
        original=branches[0].mons[0].moves[0]
        parties[branches[0].trainer]['mons'][0]['moves'][0]='MOVE_SPLASH' if original!='SPLASH' else 'MOVE_TACKLE'
        with self.assertRaises(AssertionError):export.native_vs_authoring(parties,branches)

