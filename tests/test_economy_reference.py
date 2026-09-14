"""Tests for demonstrated source-atlas omissions; no ROM or design snapshots."""
import json
from pathlib import Path
import sys
import tempfile
import unittest

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'scripts'))
import economy_reference as economy
from verify_mega_stone_rewards import world_reward_sources


class EconomyReferenceTests(unittest.TestCase):
    def test_anonymous_struct_arrays_extract_values_not_type_members(self):
        with tempfile.TemporaryDirectory() as d:
            root=Path(d);p=root/'src/prizes.c';p.parent.mkdir()
            p.write_text('static const struct { int flag; int value; } sCampaignMilestones[] =\n{\n {FLAG_A, 3000}, {FLAG_B, 8000},\n};\nstatic const struct\n{ int item; int cost; const char *name; } sBerryStoneTrades[] = { {ITEM_STONE, 20, \"Keep  spaces\"}, };\n')
            arrays,_=economy.array_records(root,[p])
            self.assertIn('FLAG_A, 3000',arrays['src/prizes.c:sCampaignMilestones']['body'])
            self.assertNotIn('int flag',arrays['src/prizes.c:sCampaignMilestones']['body'])
            self.assertEqual(arrays['src/prizes.c:sBerryStoneTrades']['tokens'],['ITEM_STONE'])
            self.assertIn('Keep  spaces',arrays['src/prizes.c:sBerryStoneTrades']['body'])

    def test_prices_follow_configured_c_ternaries_not_a_second_price_table(self):
        with tempfile.TemporaryDirectory() as d:
            root=Path(d)
            for name in ['config/general.h','constants/global.h','config/item.h','constants/items.h','constants/pokemon.h','item.h']:
                p=root/'include'/name;p.parent.mkdir(parents=True,exist_ok=True);p.write_text('')
            (root/'src/data/decoration').mkdir(parents=True)
            (root/'src/data/decoration/header.h').write_text('[DECOR_TEST] = {\n .price = 123,\n},\n')
            (root/'src/data/items.h').write_text('[ITEM_TEST] = {\n .price = (EC_GEN >= 9) ? 3000 : 1000,\n .pocket = POCKET_ITEMS,\n},\n')
            config=root/'include/config/item.h';config.write_text('#define EC_GEN 9\n')
            self.assertEqual(economy.item_prices(root)['ITEM_TEST']['base_price'],3000)
            config.write_text('#define EC_GEN 6\n')
            self.assertEqual(economy.item_prices(root)['ITEM_TEST']['base_price'],1000)
            self.assertEqual(economy.item_prices(root)['DECOR_TEST']['base_price'],123)

    def test_coin_offers_follow_shared_price_setup_and_reject_unmapped_prizes(self):
        with tempfile.TemporaryDirectory() as d:
            root=Path(d);source='data/maps/MauvilleCity_GameCorner/scripts.inc'
            p=root/source;p.parent.mkdir(parents=True)
            definitions={
                'Select': 'setvar VAR_MON, SPECIES_BULBASAUR\ngoto StarterPrice\n',
                'SelectLast': 'setvar VAR_MON, SPECIES_CHARMANDER\n',
                'StarterPrice': 'setvar VAR_COST, 500\ngoto Purchase\n',
                'Purchase': 'removecoins VAR_COST\nend\n',
            }
            p.write_text(''.join(label+'::\n'+body for label,body in definitions.items()))
            blocks={label:[(source,1,body)] for label,body in definitions.items()}
            tables={'native':{'name':'sEmeraldChampionsGameCornerPokemonPrizes','body':'{SPECIES_BULBASAUR, FLAG_BULBASAUR}, {SPECIES_CHARMANDER, FLAG_CHARMANDER},','source':'src/prizes.c:1'}}
            rows=economy.game_corner_offers(root,blocks,tables)
            self.assertEqual(rows[0]['coins'],500)
            self.assertEqual(rows[0]['species'],'SPECIES_BULBASAUR')
            self.assertEqual(rows[1]['coins'],500)
            self.assertEqual(rows[1]['species'],'SPECIES_CHARMANDER')
            tables['native']['body']+='{SPECIES_PIKACHU, FLAG_PIKACHU},'
            with self.assertRaisesRegex(ValueError,'differs from native'):
                economy.game_corner_offers(root,blocks,tables)

    def test_native_transfer_register_excludes_function_definitions_and_prototypes(self):
        with tempfile.TemporaryDirectory() as d:
            root=Path(d);(root/'src').mkdir()
            (root/'src/gifts.c').write_text('void AddBagItem(int item, int count);\nvoid AddBagItem(int item, int count) {}\nvoid Gift(void) { AddBagItem(ITEM_STONE, 2); }\n')
            nodes,names,effectful,calls,paths=economy.native_index(root)
            self.assertEqual(len(calls),1)
            self.assertEqual(calls[0]['arguments'],['ITEM_STONE','2'])
            self.assertEqual(calls[0]['function'],'src/gifts.c:Gift')
            self.assertIn('src/gifts.c:Gift',effectful)

    def test_native_held_mon_tables_are_not_mistaken_for_reward_shelves(self):
        self.assertTrue(economy.is_reward_table({'name':'sPickupTable'}))
        self.assertTrue(economy.is_reward_table({'name':'sStarterStones'}))
        self.assertFalse(economy.is_reward_table({'name':'gBattleFrontierMons'}))
        self.assertFalse(economy.is_reward_table({'name':'gSpeciesInfo'}))

    def test_additem_pc_fallback_does_not_hide_stevens_real_mega_reward(self):
        # Regression for the actual missing source route in the former verifier.
        routes=world_reward_sources()
        self.assertTrue(any('GraniteCave_StevensRoom' in source for source in routes['ITEM_AERODACTYLITE']))

if __name__=='__main__':unittest.main()
