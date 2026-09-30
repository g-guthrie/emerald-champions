"""Exercise Studio's real flash exporter without truncating a mapped save.

CI builds the release ROM before this suite. Every core here uses SAVE='-',
so its flash is heap backed even when the pre-fix exporter is under test.
"""
import asyncio
import os
from pathlib import Path
import struct
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
from native_tools import build_runner

ROM = ROOT / "pokeemerald-release.gba"
FLASH_SIZE = 131072
HEADER = struct.Struct("<II")


@unittest.skipUnless(ROM.is_file(), "build the release ROM before native Studio tests")
class StudioSaveExport(unittest.IsolatedAsyncioTestCase):
    @classmethod
    def setUpClass(cls):
        cls.build_dir = tempfile.TemporaryDirectory(prefix="studio-export-runner-")
        cls.addClassCleanup(cls.build_dir.cleanup)
        cls.runner = build_runner(ROOT / "tools/studio/core.c", Path(cls.build_dir.name) / "core")

    async def asyncSetUp(self):
        self.directory = tempfile.TemporaryDirectory(prefix="studio-export-save-")
        self.addCleanup(self.directory.cleanup)
        self.path = Path(self.directory.name)
        self.core = await asyncio.create_subprocess_exec(
            str(self.runner), str(ROM), "-",
            stdin=asyncio.subprocess.PIPE, stdout=asyncio.subprocess.PIPE,
            stderr=None,
        )
        self.addAsyncCleanup(self.close_core)

    async def close_core(self):
        self.core.stdin.close()
        try:
            await asyncio.wait_for(self.core.wait(), 10)
        except asyncio.TimeoutError:
            self.core.kill()
            await self.core.wait()

    async def rpc(self, opcode, payload=b""):
        self.core.stdin.write(HEADER.pack(opcode, len(payload)) + payload)
        await self.core.stdin.drain()
        header = await asyncio.wait_for(self.core.stdout.readexactly(HEADER.size), 10)
        status, size = HEADER.unpack(header)
        data = await asyncio.wait_for(self.core.stdout.readexactly(size), 10)
        return status, data

    async def advance(self):
        status, packet = await self.rpc(1, struct.pack("<III", 2, 0, 0))
        self.assertEqual(status, 0)
        self.assertGreaterEqual(len(packet), 16 + 240 * 160 * 4)
        return struct.unpack_from("<I", packet)[0]

    async def test_export_replaces_path_twice_without_changing_open_reader(self):
        destination = self.path / "campaign.sav"
        marker = b"previous destination\n" * (FLASH_SIZE // 21) + b"!" * (FLASH_SIZE % 21)
        self.assertEqual(len(marker), FLASH_SIZE)
        destination.write_bytes(marker)
        frame = await self.advance()
        # The parent retains the old inode. O_TRUNC changes these bytes and
        # fails this assertion safely; no process has mapped this file.
        with destination.open("rb", buffering=0) as previous:
            for _ in range(2):
                old_inode = destination.stat().st_ino
                status, data = await self.rpc(6, os.fsencode(destination))
                self.assertEqual((status, data), (0, b""))
                previous.seek(0)
                self.assertEqual(previous.read(), marker)
                self.assertNotEqual(destination.stat().st_ino, old_inode)
                # No game save has been made: the forced FLASH1M starts erased.
                self.assertEqual(destination.read_bytes(), b"\xff" * FLASH_SIZE)
                next_frame = await self.advance()
                self.assertGreater(next_frame, frame)
                frame = next_frame
        self.assertEqual(set(self.path.iterdir()), {destination})

    async def test_failed_rename_preserves_destination_and_cleans_only_own_temp(self):
        destination = self.path / "campaign.sav"
        destination.mkdir()
        marker = destination / "keep"
        marker.write_bytes(b"existing destination contents")
        neighbor = self.path / "campaign.sav.other-export"
        neighbor.write_bytes(b"another export belongs to someone else")
        frame = await self.advance()
        status, data = await self.rpc(6, os.fsencode(destination))
        self.assertNotEqual(status, 0)
        self.assertEqual(data, b"")
        self.assertEqual(marker.read_bytes(), b"existing destination contents")
        self.assertEqual(neighbor.read_bytes(), b"another export belongs to someone else")
        self.assertEqual(set(self.path.iterdir()), {destination, neighbor})
        self.assertGreater(await self.advance(), frame)


if __name__ == "__main__":
    unittest.main()
