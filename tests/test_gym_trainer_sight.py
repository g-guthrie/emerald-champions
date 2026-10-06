"""Gym trainers see until something blocks them (owner rule): no Gym trainer is skipped by walking
past the edge of a short sight line. The game stops a trainer's line of sight at walls and objects
(src/trainer_see.c CheckPathBetweenTrainerAndPlayer), so the range only sets the reach."""
import json
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GYM_SIGHT = 15


class GymTrainerSight(unittest.TestCase):
    def test_every_gym_trainer_sees_to_the_wall(self):
        gyms = sorted(ROOT.glob('data/maps/*_Gym*/map.json'))
        self.assertGreaterEqual(len(gyms), 8)
        for f in gyms:
            for o in json.loads(f.read_text())['object_events']:
                if o.get('trainer_type') == 'TRAINER_TYPE_NORMAL':
                    with self.subTest(gym=f.parent.name, trainer=o['script']):
                        self.assertGreaterEqual(int(o['trainer_sight_or_berry_tree_id']), GYM_SIGHT)


if __name__ == '__main__':
    unittest.main()
