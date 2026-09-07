"""The native avatar may turn only or turn and move on the first directional input."""
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch
import sys

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'scripts'))
import run_emerald_champions_campaign as campaign


class CampaignStepIntegrityTests(unittest.TestCase):
    def exercise(self, movement, *, allowed_map_change=False):
        state={'gEcHeadlessFixtureSetupResult':1,'gEcHeadlessCampaignControlsLocked':0,
               'gEcHeadlessCampaignScriptEnabled':0,'gEcHeadlessCampaignInBattle':0,
               'gEcHeadlessCampaignMapId':3,'gEcHeadlessCampaignPlayerX':27,
               'gEcHeadlessCampaignPlayerY':20,'gEcHeadlessCampaignPlayerFacing':4}
        inputs=[]
        def chunk(**kwargs):
            if kwargs.get('keys'):
                inputs.append(kwargs['keys'])
                state['gEcHeadlessCampaignPlayerFacing']=2
                if movement=='turn_only' and len(inputs)==1:
                    return state.copy(), ''
                state['gEcHeadlessCampaignPlayerY']-=2 if movement=='double_step' else 1
                if movement=='map_change':state['gEcHeadlessCampaignMapId']=4
            return state.copy(), ''
        with tempfile.TemporaryDirectory(prefix='ec-step-contract-') as temporary:
            path=Path(temporary)
            with patch.object(campaign,'run_state_chunk',side_effect=chunk):
                telemetry,_=campaign.apply_semantic_actions(
                    {'id':'step','semantic_actions':[{'type':'step','direction':'UP','allow_map_change':allowed_map_change}]},
                    runner=path/'runner',rom=path/'rom',state=path/'state',addresses={},
                    initial=state.copy(),screenshot_dir=path)
        return telemetry,inputs

    def test_turn_and_step_does_not_receive_second_movement_input(self):
        actual,inputs=self.exercise('turn_and_step')
        self.assertEqual(actual['gEcHeadlessCampaignPlayerY'],19)
        self.assertEqual(len(inputs),1)

    def test_turn_only_receives_one_later_step(self):
        actual,inputs=self.exercise('turn_only')
        self.assertEqual(actual['gEcHeadlessCampaignPlayerY'],19)
        self.assertEqual(len(inputs),2)

    def test_wrong_displacement_is_still_rejected(self):
        with self.assertRaisesRegex(RuntimeError,'expected'):
            self.exercise('double_step')

    def test_unexpected_map_change_is_still_rejected(self):
        with self.assertRaisesRegex(RuntimeError,'expected'):
            self.exercise('map_change')
        _,inputs=self.exercise('map_change',allowed_map_change=True)
        self.assertEqual(len(inputs),1)


if __name__=='__main__':unittest.main()
