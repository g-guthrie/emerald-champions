"""Camera observations must not issue redundant inputs or approve blank finals."""
from pathlib import Path
import hashlib
import struct
import sys
import tempfile
import unittest
from unittest.mock import patch
import zlib

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'scripts'))
import run_emerald_champions_campaign as campaign
import render_emerald_champions_ui as ui


class CampaignObservationTests(unittest.TestCase):
    def exercise_face(self, facing):
        state = {'gEcHeadlessCampaignPlayerFacing': facing,
                 'gEcHeadlessCampaignPlayerX': 19, 'gEcHeadlessCampaignPlayerY': 103}
        inputs = []
        def chunk(**kwargs):
            if kwargs.get('keys'):
                inputs.append(kwargs['keys'])
                # The observed bug: Up moves north when already facing north.
                if state['gEcHeadlessCampaignPlayerFacing'] == 2:
                    state['gEcHeadlessCampaignPlayerY'] -= 1
                else:
                    state['gEcHeadlessCampaignPlayerFacing'] = 2
            return state.copy(), ''
        with tempfile.TemporaryDirectory() as tmp:
            p = Path(tmp)
            with patch.object(campaign, 'run_state_chunk', side_effect=chunk):
                actual, _ = campaign.apply_semantic_actions(
                    {'id': 'face', 'semantic_actions': [{'type': 'face', 'direction': 'UP'}]},
                    runner=p/'runner', rom=p/'rom', state=p/'state', addresses={},
                    initial=state.copy(), screenshot_dir=p)
        return actual, inputs

    def test_already_facing_does_not_walk_onto_adjacent_trigger(self):
        actual, inputs = self.exercise_face(2)
        self.assertEqual(inputs, [])
        self.assertEqual(actual['gEcHeadlessCampaignPlayerY'], 103)

    def test_different_facing_still_receives_directional_input(self):
        actual, inputs = self.exercise_face(4)
        self.assertEqual(len(inputs), 1)
        self.assertEqual(inputs[0][0][2], 'UP')
        self.assertEqual(actual['gEcHeadlessCampaignPlayerFacing'], 2)

    def blank_png(self):
        def chunk(kind, data):
            return struct.pack('>I', len(data)) + kind + data + struct.pack('>I', zlib.crc32(kind+data) & 0xffffffff)
        return (b'\x89PNG\r\n\x1a\n'
                + chunk(b'IHDR', struct.pack('>IIBBBBB', 240, 160, 8, 2, 0, 0, 0))
                + chunk(b'IDAT', zlib.compress((b'\0' + bytes(240*3))*160))
                + chunk(b'IEND', b''))

    def test_uniform_transition_is_valid_but_blank_final_stays_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            p = Path(tmp)/'frame.png'; p.write_bytes(self.blank_png())
            with self.assertRaisesRegex(RuntimeError, 'uniform blank'):
                ui.validate_screenshot_png(p)
            self.assertEqual(ui.validate_screenshot_png(p, allow_uniform=True),
                             hashlib.sha256(bytes(240*160*3)).hexdigest())

    def test_transition_allowance_does_not_accept_corrupt_png(self):
        with tempfile.TemporaryDirectory() as tmp:
            p = Path(tmp)/'frame.png'; data = bytearray(self.blank_png()); data[-1] ^= 1; p.write_bytes(data)
            with self.assertRaisesRegex(RuntimeError, 'CRC mismatch'):
                ui.validate_screenshot_png(p, allow_uniform=True)


if __name__ == '__main__':
    unittest.main()
