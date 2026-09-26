"""Mutation tests against real map/tileset consumers; no ROM or user files edited."""

import json
import hashlib
import zlib
from pathlib import Path
import struct
import sys
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts/audit"))
import map_integrity as audit


class MapTileIntegrityTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.baseline = audit.inventory(ROOT, include_dynamic=False)

    def mutated_bytes(self, relative, payload):
        original = Path.read_bytes

        def read(path):
            return payload if path == ROOT / relative else original(path)

        with patch.object(Path, "read_bytes", read):
            return audit.inventory(ROOT, include_dynamic=False)

    def mutated_text(self, relative, payload):
        original = Path.read_text

        def read(path, *args, **kwargs):
            return (
                payload if path == ROOT / relative else original(path, *args, **kwargs)
            )

        with patch.object(Path, "read_text", read):
            return audit.inventory(ROOT, include_dynamic=False)

    def test_converter_preserves_nonblank_tail_under_warn_num_tiles(self):
        def chunk(kind, payload):
            return (
                struct.pack(">I", len(payload))
                + kind
                + payload
                + struct.pack(">I", zlib.crc32(kind + payload) & 0xFFFFFFFF)
            )

        def image(tail):
            rows = b"".join(b"\x00" + bytes(4) + bytes([tail]) * 4 for _ in range(8))
            return (
                b"\x89PNG\r\n\x1a\n"
                + chunk(b"IHDR", struct.pack(">IIBBBBB", 16, 8, 4, 3, 0, 0, 0))
                + chunk(b"PLTE", bytes([0, 0, 0, 255, 255, 255]))
                + chunk(b"IDAT", zlib.compress(rows))
                + chunk(b"IEND", b"")
            )

        converter = ROOT / "tools/gbagfx/gbagfx"
        identity = hashlib.sha256(converter.read_bytes()).hexdigest()
        blank = audit.converted_graphics(
            image(0), "-num_tiles 1 -Wnum_tiles", str(converter), identity
        )
        nonblank = audit.converted_graphics(
            image(0x11), "-num_tiles 1 -Wnum_tiles", str(converter), identity
        )
        self.assertEqual(blank[0], 1)
        self.assertEqual(nonblank[0], 2)
        self.assertIn("Ignoring -num_tiles", nonblank[2])

    def test_used_metatile_outside_tileset_fails_compiled_domain(self):
        path = "data/layouts/LittlerootTown/map.bin"
        raw = bytearray((ROOT / path).read_bytes())
        struct.pack_into("<H", raw, 0, 1023)
        report = self.mutated_bytes(path, bytes(raw))
        self.assertTrue(
            any(
                e["compiled"] and e["reason"] == "metatile outside owning tileset"
                for e in report["errors"]
            )
        )

    def test_truncated_map_binary_fails(self):
        path = "data/layouts/LittlerootTown/map.bin"
        with self.assertRaisesRegex(audit.InvalidData, "map binary too short"):
            self.mutated_bytes(path, (ROOT / path).read_bytes()[:-2])

    def test_wrong_primary_secondary_binding_fails(self):
        path = "src/data/tilesets/headers.h"
        source = (ROOT / path).read_text()
        start = source.index("const struct Tileset gTileset_General =")
        tail = source[start:].replace(".isSecondary = FALSE", ".isSecondary = TRUE", 1)
        with self.assertRaisesRegex(
            audit.InvalidData, "wrong tileset primary/secondary partition"
        ):
            self.mutated_text(path, source[:start] + tail)

    def test_missing_metatile_data_fails(self):
        original = Path.read_bytes

        def read(path):
            if path == ROOT / "data/tilesets/secondary/petalburg/metatiles.bin":
                raise FileNotFoundError(path)
            return original(path)

        with patch.object(Path, "read_bytes", read), self.assertRaises(
            FileNotFoundError
        ):
            audit.inventory(ROOT, include_dynamic=False)

    def test_invalid_layer_is_rejected(self):
        path = "data/tilesets/secondary/petalburg/metatile_attributes.bin"
        raw = bytearray((ROOT / path).read_bytes())
        struct.pack_into("<H", raw, 0x4A * 2, 0xF000)
        report = self.mutated_bytes(path, bytes(raw))
        self.assertTrue(
            any(e["compiled"] and e.get("layer") == 15 for e in report["errors"])
        )

    def test_unregistered_warp_destination_is_rejected(self):
        path = "data/maps/LittlerootTown/map.json"
        data = json.loads((ROOT / path).read_text())
        data["warp_events"][0]["dest_map"] = "MAP_UNREGISTERED"
        with self.assertRaisesRegex(audit.InvalidData, "missing destination MAP_UNREGISTERED"):
            self.mutated_text(path, json.dumps(data))


if __name__ == "__main__":
    unittest.main()
