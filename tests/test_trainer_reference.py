"""Trainer projection must preserve authored moves."""
import sys
from pathlib import Path
import re
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


    def test_no_campaign_trainer_passes_level_100_on_any_difficulty(self):
        # Every live-cap trainer is battled at or below the Champion cap, and
        # Hard has the smallest reduction, so its ace is the highest level.
        root=Path(__file__).resolve().parents[1]
        top_cap=max(int(c) for c in re.findall(r'\{FLAG_\w+, (\d+)\}', (root/'src/caps.c').read_text().split('sCampaignMilestones[]',1)[1].split('};',1)[0]))
        from battle_campaign_arsenal import _level_rows
        parties=export.parse_parties()
        offsets=tuple(sorted({mon['offset'] for data in parties.values() for mon in data['mons']}))
        source=(root/'src/difficulty.c').read_text()
        # Execute the actual native level function, including its ceiling.
        # An authored offset may exceed 100 before that clamp is applied.
        for mode in range(3):
            levels=_level_rows(source,mode,top_cap,offsets,0)
            for offset,level in zip(offsets,levels):
                self.assertGreaterEqual(level,1,(mode,offset))
                self.assertLessEqual(level,100,(mode,offset))
