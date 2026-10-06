"""Every live flag name owns its own save bit.

scripts/audit/flag_collisions.py is the release gate for this; running it with the unit suite catches
a new flag taken from a FLAG_UNUSED_* slot that is secretly live (the follower recall flag once took
0x468, the New Mauville Eelektrossite pickup's bit).
"""
import subprocess
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class FlagCollisions(unittest.TestCase):
    def test_no_two_live_flags_share_a_bit(self):
        r = subprocess.run([sys.executable, 'scripts/audit/flag_collisions.py'], cwd=ROOT,
                           capture_output=True, text=True)
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)


if __name__ == '__main__':
    unittest.main()
