"""Nearby trainer copies are rejected independently of item and move ordering."""
import sys
import unittest
from pathlib import Path
from types import SimpleNamespace as Row
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'scripts'))
import trainer_set_copies as copies
from emerald_champions_teams import read_teams


def trainer(name, encounter, moves=('TACKLE','PROTECT','HELPING_HAND','QUICK_ATTACK'), ability='RUN_AWAY', cls='regular'):
    return Row(trainer=name, encounter=encounter, cls=cls,
               mons=[Row(species='EEVEE', ability=ability, moves=list(moves))])


class TrainerSetCopies(unittest.TestCase):
    def test_late_campaign_has_no_nearby_copies(self):
        self.assertEqual(copies.late_copies(read_teams()), [])

    def test_move_order_does_not_disguise_a_copy_and_distance_is_inclusive(self):
        first=trainer('TRAINER_A',100)
        other=trainer('TRAINER_B',160,tuple(reversed(first.mons[0].moves)))
        self.assertEqual(len(copies.find_copies([first,other])),1)
        other.encounter=161
        self.assertEqual(copies.find_copies([first,other]),[])
        other.encounter=100;other.mons[0].ability='ADAPTABILITY'
        self.assertEqual(copies.find_copies([first,other]),[])

    def test_rematches_and_rival_alternatives_are_excluded_but_grunts_are_distinct(self):
        self.assertEqual(copies.find_copies([trainer('TRAINER_JOE_1',100),trainer('TRAINER_JOE_2',101)]),[])
        self.assertEqual(copies.find_copies([trainer('TRAINER_BRENDAN_ROUTE_103_TREECKO',1,cls='rival'),
                                           trainer('TRAINER_MAY_ROUTE_103_TORCHIC',1,cls='rival')]),[])
        self.assertEqual(len(copies.find_copies([trainer('TRAINER_GRUNT_AQUA_HIDEOUT_1',340),
                                               trainer('TRAINER_GRUNT_AQUA_HIDEOUT_2',341)])),1)

if __name__ == '__main__':
    unittest.main()
