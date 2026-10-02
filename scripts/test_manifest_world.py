#!/usr/bin/env python3
"""Acquisition proof checks independent of full campaign geometry."""
import unittest
from collections import defaultdict
from manifest_world import World
from manifest_native_predicates import initial_lady_variants


class AcquisitionProof(unittest.TestCase):
    def setUp(self):
        self.world=World.__new__(World)
        self.world.names={};self.world.unresolved=defaultdict(set)
        self.world.reachable=lambda where,state:True
        self.world.requirements=lambda req,state:True
        self.state={'flags':set(),'vars':{},'species':{'SPECIES_TREECKO'},'caught':{'SPECIES_TREECKO'},
                    'items':set(),'money':6000,'coins':0,'funding_possible':False,'preparation_possible':True}

    def live(self,**fields):
        return self.world.source_live(dict(key='ITEM_NUGGET',kind='gift',cite='source:1',**fields),self.state)

    def test_price_is_checked_before_offer_enters_collection(self):
        self.assertTrue(self.live(price=6000))
        self.assertFalse(self.live(price=6001))
        self.state['funding_possible']=True
        self.assertTrue(self.live(price=100000))

    def test_tent_three_victories_are_required(self):
        activity={'kind':'tent_battle_run','town':'Slateport','wins':3}
        self.assertFalse(self.live(activity_requirement=activity))
        self.assertIn('ACTIVITY:tent_battle_run',self.world.unresolved)
        self.state['tent_wins']={'Slateport':2}
        self.assertFalse(self.live(activity_requirement=activity))
        self.state['tent_wins']['Slateport']=3
        self.assertTrue(self.live(activity_requirement=activity))

    def test_bag_berries_are_not_harvest_credits(self):
        self.state['items'].add('ITEM_RAZZ_BERRY')
        activity={'kind':'harvest_credit_trade','recipe':[{'item':'ITEM_RAZZ_BERRY','count':6}]}
        self.assertFalse(self.live(activity_requirement=activity))
        self.state['harvestable_berries']={'ITEM_RAZZ_BERRY'}
        self.assertTrue(self.live(activity_requirement=activity))

    def test_quiz_table_does_not_grant_all_prizes(self):
        self.state['lilycove_lady']=initial_lady_variants(0)[6]
        row={'kind':'quiz_prize','key':'ITEM_WATMEL_BERRY','cite':'quiz:1'}
        self.assertTrue(self.world.source_live(row,self.state))
        row['key']='ITEM_STAR_PIECE'
        self.assertFalse(self.world.source_live(row,self.state))


if __name__=='__main__':unittest.main()
