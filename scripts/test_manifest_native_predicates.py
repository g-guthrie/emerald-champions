#!/usr/bin/env python3
"""Source-observable native result and mutation regressions."""
import unittest
from manifest_native_predicates import evaluate, native_effects, numeric_domain, initial_lady_variants, native_output_names, _lady_rows, FlagConstants, _flag, _battle_stock_rows, decoration_trade_possible, _caught


def special(name):return 'SPECIAL:'+name+'()'


class NativePredicates(unittest.TestCase):
    def test_pseudo_mega_harvest_trade_requires_mind_badge(self):
        state={'flags':set(), 'items':set(), 'pc_items':set(),
               'harvested_berries':{'RAZZ':30,'BLUK':30,'NANAB':30,'WEPEAR':30}}
        key='SPECIAL:TradeEmeraldChampionsGardenBerries(VAR_0x8004=0)'
        self.assertEqual(evaluate(key,state,{}),{5})
        state['flags'].add('FLAG_BADGE07_GET')
        state['bag_has_space']=True
        self.assertEqual(evaluate(key,state,{}),{0})

    def test_caught_form_lookup_never_caches_mutable_pool_or_confuses_available(self):
        state={'caught':set(),'species':{'SPECIES_ROTOM_WASH'}}
        self.assertFalse(_caught(state,'SPECIES_ROTOM'))
        state['caught'].add('SPECIES_ROTOM_WASH')
        self.assertTrue(_caught(state,'SPECIES_ROTOM'))
        self.assertFalse(_caught(state,'SPECIES_EEVEE'))
        state['caught'].clear()
        self.assertFalse(_caught(state,'SPECIES_ROTOM'))
        self.assertIsNone(_caught({'species':{'SPECIES_ROTOM_WASH'}},'SPECIES_ROTOM'))
        self.assertTrue(_caught({'species':{'SPECIES_ROTOM_WASH'},'species_is_caught':True},'SPECIES_ROTOM'))

    def test_catalogue_stock_requires_source_unlock(self):
        rows=_battle_stock_rows();category='EC_BATTLE_ITEM_CATEGORY_OFFENSE'
        state={'items':set(),'caught':set(),'flags':set()}
        constants={category:0}
        key='SPECIAL:BufferEmeraldChampionsBattleItemStock(VAR_0x8004='+category+')'
        self.assertEqual(evaluate(key,state,constants),{0})
        state['items'].add('ITEM_CHOICE_BAND')
        self.assertEqual(evaluate(key,state,constants),{1})
        state['flags'].update({'FLAG_BADGE01_GET','FLAG_BADGE02_GET'})
        expected=1+sum(rows['badge_stock'].get(item,99)<=2 for item in rows['categories'][category])
        self.assertEqual(evaluate(key,state,constants),{expected})
        self.assertEqual(rows['equipment']['ITEM_FIGHTING_MEMORY'],'SPECIES_SILVALLY')
        self.assertNotIn('ITEM_SOUL_DEW',rows['equipment'])

    def test_trader_needs_actual_identity_and_donation(self):
        state={'trainer_id':4,'items':{'DECOR_RED_PLANT'},'preparation_possible':True}
        self.assertTrue(decoration_trade_possible(state))
        state['trainer_id']=3
        self.assertFalse(decoration_trade_possible(state))
        state['trainer_id']=4;state['trader']={'already_traded':True}
        self.assertFalse(decoration_trade_possible(state))

    def test_flag_alias_index_tracks_mutable_constants(self):
        constants=FlagConstants({'FLAG_A':7,'FLAG_B':8})
        state={'flags':{'FLAG_B','OBJECT_CLEAR:Route1:2'}}
        self.assertFalse(_flag(state,'FLAG_A',constants))
        constants['FLAG_B']=7
        self.assertTrue(_flag(state,'FLAG_A',constants))
        constants.update(FLAG_B=9)
        self.assertFalse(_flag(state,'FLAG_A',constants))
        constants|={'FLAG_B':'FLAG_A'}
        self.assertTrue(_flag(state,'FLAG_A',constants))
        constants.pop('FLAG_B')
        self.assertFalse(_flag(state,'FLAG_A',constants))
    def test_starter_kit_is_atomic_and_tutorial_grants_no_items(self):
        state={'preparation_possible':True,'flags':set()}
        self.assertEqual(evaluate(special('GiveEmeraldChampionsStarterBattleItems'),state),{0,1})
        delivered=native_effects(special('GiveEmeraldChampionsStarterBattleItems'),1,state)
        self.assertEqual(len(delivered['items_add']),6)
        self.assertIn('ITEM_FOCUS_SASH',delivered['items_add'])
        self.assertIn('FLAG_EC_RECEIVED_STARTER_BATTLE_ITEMS',delivered['flags_set'])
        self.assertFalse(native_effects(special('GiveEmeraldChampionsStarterBattleItems'),0,state)['items_add'])
        self.assertEqual(evaluate(special('StartInitialToolsBagTutorial'),state),{0,1})
        self.assertFalse(native_effects(special('StartInitialToolsBagTutorial'),1,state)['items_add'])

    def test_friendship_preparation_needs_an_actual_available_mon(self):
        state={'preparation_possible':True,'species':set()}
        self.assertIsNone(evaluate(special('GetLeadMonFriendshipScore'),state))
        state['species']={'SPECIES_TREECKO'}
        self.assertEqual(evaluate(special('GetLeadMonFriendshipScore'),state),{6})
        state['preparation_possible']=False;state['lead_friendship']=199
        self.assertEqual(evaluate(special('GetLeadMonFriendshipScore'),state),{4})

    def test_soot_is_earned_ash_with_delivery_before_debit(self):
        key='SPECIAL:ExchangeSootForCaps(VAR_0x8004=5)'
        state={'vars':{'VAR_ASH_GATHER_COUNT':2499},'bag_has_space':True}
        self.assertEqual(evaluate(key,state),{0})
        state['vars']['VAR_ASH_GATHER_COUNT']=2500
        self.assertEqual(evaluate(key,state),{1})
        effect=native_effects(key,1,state)
        self.assertEqual(effect['items_add'],{'ITEM_BOTTLE_CAP':5})
        self.assertEqual(effect['vars']['VAR_ASH_GATHER_COUNT'],0)
        state['bag_has_space']=False
        self.assertEqual(evaluate(key,state),{2})
        self.assertNotIn('VAR_ASH_GATHER_COUNT',native_effects(key,2,state)['vars'])

    def test_native_scratch_outputs_invalidate_callback_writes(self):
        self.assertIn('VAR_0x8004',native_output_names('ShowEasyChatScreen'))
        self.assertIn('VAR_0x8005',native_output_names('GetContestWinnerId'))
        self.assertIn('VAR_0x8006',native_output_names('FossilToSpecies'))
        self.assertIn('VAR_0x8004',native_output_names('SetSpeciesAndEggMove'))

    def test_berry_phrases_keep_game_clear_and_seen_word_gates(self):
        key='SPECIAL:NativeScratchOutput(function=ShowEasyChatScreen,variable=VAR_0x8004,VAR_0x8004=EASY_CHAT_TYPE_GOOD_SAYING)'
        state={'preparation_possible':True,'flags':set(),'species':set()}
        self.assertEqual(evaluate(key,state),{0,1})
        state['flags'].add('FLAG_SYS_GAME_CLEAR')
        self.assertEqual(evaluate(key,state),{0,1,2,5})
        state['seen']={'SPECIES_LATIAS'}
        self.assertEqual(evaluate(key,state),{0,1,2,3,5})
        state['easy_chat_phrase']=['EC_WORD_COOL','EC_POKEMON(LATIOS)']
        self.assertEqual(evaluate(key,state),{0})
        state['seen'].add('SPECIES_LATIOS')
        self.assertEqual(evaluate(key,state),{4})

    def test_initializer_variants_preserve_trainer_id_and_reward_correlations(self):
        import json
        quiz=initial_lady_variants(0)
        self.assertEqual(len(quiz),16)
        self.assertTrue(all(row['id']==0 for row in quiz))
        self.assertEqual(quiz[6]['quiz']['prize'],'ITEM_WATMEL_BERRY')
        self.assertTrue(all(row['id']==1 for row in initial_lady_variants(2)))
        self.assertEqual(len(initial_lady_variants(4)),5)
        json.dumps(initial_lady_variants())

    def test_funding_requires_explicit_proof_and_coin_exchange_access(self):
        state={'money':6000,'coins':0,'preparation_possible':True,'items':set(),'maps':set()}
        self.assertEqual(evaluate('CHECKMONEY:6000',state),{1})
        self.assertEqual(evaluate('CHECKMONEY:6001',state),{0})
        self.assertEqual(evaluate('COINS',state),{0})
        state['funding_possible']=True
        self.assertEqual(evaluate('CHECKMONEY:100000',state),{1})
        self.assertEqual(evaluate('COINS',state),{0})
        state['items'].add('ITEM_COIN_CASE');state['maps'].add('MauvilleCity_GameCorner')
        self.assertEqual(evaluate('COINS',state),set(range(10000)))

    def test_symbolic_cost_and_native_money_debit(self):
        state={'money':6000,'vars':{'VAR_0x8005':'(BASE_PRICE * 10)'}}
        constants={'BASE_PRICE':1000,'MAX_COINS':9999}
        self.assertEqual(numeric_domain('(MAX_COINS + 1 - 5000)',state,constants),{5000})
        self.assertEqual(evaluate('SPECIAL:IsEnoughForCostInVar0x8005()',state,constants),{0})
        state['funding_possible']=True
        self.assertEqual(evaluate('SPECIAL:IsEnoughForCostInVar0x8005()',state,constants),{1})
        self.assertEqual(native_effects('SPECIAL:SubtractMoneyFromVar0x8005()',None,state,constants)['money_remove'],10000)

    def test_trade_returns_item_to_correct_storage_and_clears_outgoing(self):
        state={'outgoing_trade_item':'ITEM_ORAN_BERRY'}
        effect=native_effects(special('ReturnInGameTradeHeldItem'),3,state)
        self.assertEqual(effect['pc_items_add'],{'ITEM_ORAN_BERRY':1})
        self.assertEqual(effect['outgoing_trade_item'],'ITEM_NONE')
        self.assertNotIn('outgoing_trade_item',native_effects(special('ReturnInGameTradeHeldItem'),0,state))

    def test_unknown_is_not_accepted_by_preparation(self):
        state={'preparation_possible':True}
        for name in ('IsQuizAnswerCorrect','IsFavorLadyThresholdMet','TryEnterContestMon','UnknownNativePredicate'):
            self.assertIsNone(evaluate(special(name),state))

    def test_ship_key_reads_flag_and_writes_scratch_flag_id(self):
        key=special('FoundAbandonedShipRoom4Key');flag='FLAG_HIDDEN_ITEM_ABANDONED_SHIP_RM_4_KEY'
        self.assertEqual(evaluate(key,{'flags':set()}),{0})
        self.assertEqual(evaluate(key,{'flags':{flag}}),{1})
        self.assertEqual(native_effects(key,1,{})['vars'],{'VAR_0x8004':flag})

    def test_contest_rank_is_category_specific_and_eggs_fainted_fail(self):
        state={'contest_mon':{'hp':1,'ribbons':{0:2,1:0}},'contest_rank':1,'contest_category':0}
        self.assertEqual(evaluate(special('TryEnterContestMon'),state),{2})
        self.assertEqual(evaluate(special('HasMonWonThisContestBefore'),state),{1})
        state['contest_category']=1
        self.assertEqual(evaluate(special('TryEnterContestMon'),state),{0})
        state['contest_mon']['hp']=0
        self.assertEqual(evaluate(special('TryEnterContestMon'),state),{4})
        state['contest_mon']['is_egg']=True
        self.assertEqual(evaluate(special('TryEnterContestMon'),state),{3})

    def test_favor_exact_native_table_and_threshold(self):
        row=_lady_rows()[1][0];item=next(iter(row['accepted']))
        state={'selected_item':item,'lilycove_lady':{'favor':{'favor_id':0,'num_items_given':0,'best_item':item}}}
        self.assertEqual(evaluate(special('Script_DoesFavorLadyLikeItem'),state),{1})
        self.assertEqual(evaluate(special('IsFavorLadyThresholdMet'),state),{0})
        effect=native_effects(special('Script_DoesFavorLadyLikeItem'),1,state)
        self.assertEqual(effect['lilycove_lady_update']['favor']['num_items_given'],5)
        state['selected_item']='ITEM_NONE'
        self.assertEqual(evaluate(special('Script_DoesFavorLadyLikeItem'),state),{0})

    def test_quiz_answer_and_prize_share_question_row(self):
        row=_lady_rows()[0][6]
        state={'lilycove_lady':{'quiz':{'question_id':6,'player_answer':row['answer']}}}
        self.assertEqual(evaluate(special('IsQuizAnswerCorrect'),state),{1})
        self.assertEqual(native_effects(special('BufferQuizPrizeItem'),None,state)['vars'],{'VAR_0x8005':row['prize']})
        state['lilycove_lady']['quiz']['player_answer']='EC_EMPTY_WORD'
        self.assertEqual(evaluate(special('IsQuizAnswerCorrect'),state),{0})


if __name__=='__main__':unittest.main()
