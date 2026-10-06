"""The route manifest covers the campaign's battles in story order, with none skipped.

Every step names its battles by encounter number (E0001... in
data/emerald_champions/emerald_champions_battle_teams.txt). Listed numbers must exist and be live,
must rise from step to step, and every live battle up to the highest listed number must be listed:
the manifest grows as a prefix of the campaign, so the trainer audit can walk it in order.
"""
import json
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / 'artifacts/progression-manifest/route-manifest.txt'


def live_battles():
    teams = (ROOT / 'data/emerald_champions/emerald_champions_battle_teams.txt').read_text()
    ids = {int(n) for n in re.findall(r'^## E(\d+) ', teams, re.M)}
    retired = json.loads((ROOT / 'data/emerald_champions/retired_battles.json').read_text())
    gone = {int(g[1:]) for g in re.findall(r'"group":\s*"(E\d+)"', json.dumps(retired))}
    return ids - gone, gone


def manifest_steps():
    """[(step header, [battle numbers])] in manifest order."""
    steps, cur = [], None
    for line in MANIFEST.read_text().splitlines():
        if line.startswith('STEP '):
            cur = (line, []); steps.append(cur)
        elif cur and line.strip().startswith('Battles:'):
            cur[1].extend(int(n) for n in re.findall(r'\bE(\d{4})\b', line))
    return steps


class ManifestBattleOrder(unittest.TestCase):
    def test_battles_are_live_ordered_and_contiguous(self):
        live, retired = live_battles()
        steps = manifest_steps()
        listed = [n for _, ns in steps for n in ns]
        self.assertTrue(listed, 'no step lists its battles')
        for n in listed:
            self.assertIn(n, live, f'E{n:04d} is not a live battle' + (' (retired)' if n in retired else ''))
        self.assertEqual(len(listed), len(set(listed)), 'a battle is listed in two steps')
        prev = 0
        for header, ns in steps:
            if not ns: continue
            self.assertGreater(min(ns), prev, f'{header}: battles go back in story order')
            prev = max(ns)
        missing = sorted(n for n in live if n <= max(listed) and n not in listed)
        self.assertEqual(missing, [], 'live battles skipped: ' + ', '.join(f'E{n:04d}' for n in missing))


if __name__ == '__main__':
    unittest.main()
