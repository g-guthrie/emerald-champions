"""The precise Littleroot repair preserves pixels and all unrelated source bytes."""
import hashlib
import importlib.util
import json
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location('overworld_parity', ROOT / 'scripts/verify_inclement_overworld_parity.py')
P = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(P)
ASSET = 'data/tilesets/secondary/petalburg/metatiles.bin'


class PetalburgCanonicalizationTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data = (ROOT / ASSET).read_bytes()
        cls.expected = json.loads(P.MANIFEST.read_text())['tileset_asset_sha256'][ASSET]

    def test_repair_retains_original_reference_hash(self):
        self.assertNotEqual(hashlib.sha256(self.data).hexdigest(), self.expected)
        self.assertTrue(P.tileset_matches_reference(ASSET, self.data, self.expected))
        self.assertFalse(P.tileset_matches_reference('another.bin', self.data, self.expected))
        self.assertFalse(P.tileset_matches_reference(ASSET, self.data, '0' * 64))

    def test_every_single_byte_mutation_is_rejected(self):
        for offset in range(len(self.data)):
            mutated = bytearray(self.data)
            mutated[offset] ^= 1
            with self.subTest(offset=offset):
                self.assertFalse(P.tileset_matches_reference(ASSET, mutated, self.expected))
        self.assertFalse(P.tileset_matches_reference(ASSET, self.data[:-1], self.expected))
        self.assertFalse(P.tileset_matches_reference(ASSET, self.data + b'\0', self.expected))

    def test_actual_gbagfx_pixels_are_identical(self):
        with tempfile.TemporaryDirectory() as directory:
            raw = {}
            for tileset in ('primary/general', 'secondary/petalburg'):
                output = Path(directory) / (tileset.replace('/', '_') + '.4bpp')
                subprocess.run([str(ROOT / 'tools/gbagfx/gbagfx'),
                                str(ROOT / 'data/tilesets' / tileset / 'tiles.png'), str(output)],
                               check=True, capture_output=True)
                raw[tileset] = output.read_bytes()
        primary = raw['primary/general']
        self.assertEqual(primary[:32], bytes(32))
        # The authored declaration emits159 tiles; the partition loader zeroes
        # the rest. The source PNG's last padding tile is transparent as well.
        graphics = (ROOT / 'src/data/tilesets/graphics.h').read_text()
        self.assertIn('"data/tilesets/secondary/petalburg/tiles.png", ".4bpp.fastSmol", "-num_tiles 159 -Wnum_tiles"', graphics)
        secondary = raw['secondary/petalburg'][:159 * 32]
        vram = primary + secondary + bytes((512 - 159) * 32)
        original = bytearray(self.data)
        original[0x4a0:0x4a8] = bytes.fromhex('20e321e330e331e3')
        original[0x4b0:0x4b8] = bytes.fromhex('22e323e332e333e3')
        self.assertEqual(hashlib.sha256(original).hexdigest(), self.expected)
        def pixels(word):
            tile, palette = word & 1023, word >> 12
            indices = [n for byte in vram[tile * 32:(tile + 1) * 32] for n in (byte & 15, byte >> 4)]
            return [palette * 16 + index if index else None for index in indices]
        for index in (0x4a, 0x4b):
            before = struct.unpack_from('<8H', original, index * 16)
            after = struct.unpack_from('<8H', self.data, index * 16)
            self.assertEqual(after[:4], (0, 0, 0, 0))
            self.assertEqual(before[4:], after[4:])
            for old, new in zip(before, after):
                self.assertEqual(pixels(old), pixels(new))
            for upper in after[4:]:
                self.assertNotIn(None, pixels(upper))


if __name__ == '__main__':
    unittest.main()
