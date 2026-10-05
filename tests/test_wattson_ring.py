import sys
import unittest
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'scripts'))
import reference_pool as rp
from manifest_battle_progression import ProgressionParser

class WattsonRingTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.builder = rp.Builder()
        cls.encounters = rp.Encounters(cls.builder)
        cls.pools = rp.Pools(cls.builder, cls.encounters).result
        cls.parser = ProgressionParser(cls.builder.scripts)

    def test_ring_moves_to_badge_three_and_starter_stones_wait_for_badge_five(self):
        self.assertFalse(self.pools['badge2']['ring'])
        self.assertTrue(self.pools['badge3']['ring'])
        self.assertNotIn('ITEM_SCEPTILITE', self.pools['badge4']['items'])
        self.assertIn('ITEM_SCEPTILITE', self.pools['badge5']['items'])

    def test_wattson_cannot_be_benchmarked_with_his_own_victory_ring(self):
        pool = rp.encounter_pool('TRAINER_WATTSON_1', 'badge2', builder=self.builder, encounters=self.encounters)
        self.assertFalse(pool['mega_ring']['available'])
        pool = rp.encounter_pool('TRAINER_FLANNERY_1', 'badge3', builder=self.builder, encounters=self.encounters)
        self.assertTrue(pool['mega_ring']['available'])

    def test_real_ring_and_starter_gift_scripts_have_victory_requirements(self):
        for label, command, badge in [
                ('MauvilleCity_Gym_EventScript_GiveMegaRing', 'giveuniqueitem ITEM_MEGA_RING', 'FLAG_BADGE03_GET'),
                ('PetalburgCity_Gym_EventScript_NormanNextStarterStone', 'giveitem VAR_0x8004', 'FLAG_BADGE05_GET')]:
            line = next(n for n,text in self.builder.scripts.labels[label]['body'] if text.startswith(command))
            paths = self.parser.paths_to(label, line)
            self.assertTrue(paths)
            for path in paths:
                # The first Wattson win reaches the gift after the battle; later calls retain his battle prerequisite.
                self.assertTrue(any(badge in e.get('key','') or badge in str(e.get('args',[]))
                                    for e in path.get('effects',[]))
                                or any(c['key']==badge and (c['op'], c['value']) in [('eq','TRUE'),('ne','FALSE')]
                                       for c in path['conditions'])
                                or ('TRAINER_WATTSON_1' in path.get('prior_battles',[]) and badge=='FLAG_BADGE03_GET'), (label,path))

    def test_wattson_preflights_both_rewards_before_giving_the_ring(self):
        label='MauvilleCity_Gym_EventScript_GiveMegaRing'
        line=next(n for n,text in self.builder.scripts.labels[label]['body'] if text.startswith('giveuniqueitem ITEM_MEGA_RING'))
        paths=self.parser.paths_to(label,line)
        self.assertTrue(paths)
        for path in paths:
            self.assertTrue(any('CanReceiveWattsonMegaGift' in c['key'] and c['op']=='ne' and c['value']=='FALSE'
                                for c in path['conditions']),path['conditions'])
