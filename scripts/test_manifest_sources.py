#!/usr/bin/env python3
"""Verify campaign acquisition alternatives against executable source paths."""
import unittest

from manifest_pokemon_sources import PokemonSources
from manifest_battle_progression import command, is_battle, ProgressionParser, _contradiction, _known_result, extract_completed_event_transitions


class SourceEligibility(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.model=PokemonSources()
        cls.model.enrich_eligibility()

    def test_live_scripted_sources_have_complete_paths(self):
        rows=[r for r in self.model.rows if r['kind'] in ('gift','egg','trade','static','game_corner','fossil')
              and not r.get('campaign_scope')]
        self.assertTrue(rows)
        self.assertFalse([r['id'] for r in rows if not r.get('eligibility_paths')])
        self.assertFalse(self.model.eligibility_diagnostics)
        coverage=self.model.export()['source_coverage']
        self.assertFalse(coverage['unrepresented_literal_sites'])
        self.assertEqual(coverage['trade_sources'],4)

    def test_harvest_sources_keep_one_gated_credit_recipe_each(self):
        from manifest_item_sources import build_item_sources
        result = build_item_sources(self.model.builder, enrich=False)
        trades = [row for row in result['sources'] if row['kind'] == 'harvest_trade']
        self.assertEqual(len(trades), 3)
        for row in trades:
            self.assertEqual(row['requires'], [['FLAG_BADGE07_GET']])
            self.assertTrue(row['recipe'])
            self.assertTrue(row['receipt'])
            self.assertEqual(row['activity_requirement']['kind'], 'harvest_credit_trade')

    def test_each_static_requires_its_real_capture_battle(self):
        for row in self.model.rows:
            if row['kind']!='static' or row.get('campaign_scope'):continue
            self.assertEqual(row['ownership_condition'],'successful_capture')
            self.assertTrue(row['capture_sites'],row['species'])
            for site in row['capture_sites']:
                label,line=site.rsplit(':',1)
                text=next(text for n,text in self.model.builder.scripts.labels[label]['body'] if n==int(line))
                self.assertTrue(is_battle(*command(text)),site)

    def test_coin_prizes_preserve_selected_species_and_price(self):
        prizes=[r for r in self.model.rows if r['kind']=='game_corner']
        self.assertTrue(prizes)
        for row in prizes:
            self.assertIn('ITEM_COIN_CASE',row['needs_items'])
            self.assertGreater(row['cost']['amount'],0)
            for path in row['eligibility_paths']:
                self.assertEqual(self.model.sd.resolve(path['assignments']['VAR_TEMP_1']),row['species'])

    def test_starter_alternatives_are_limited_to_one_selected_region(self):
        rows=[r for r in self.model.rows if r['kind']=='starter']
        self.assertEqual(len(rows),3*len(self.model.builder.starter_regions))
        for row in rows:
            trio=self.model.builder.starter_regions[row['starter_region']-1]
            self.assertIn(row['species'],[self.model.sd.resolve(s) for s in trio])
            self.assertIn('At most two of the three starters, both from the one selected region',row['conditions'])


class AcquisitionParser(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.parser=ProgressionParser(PokemonSources().builder.scripts)

    def query(self,label,needle):
        line=next(n for n,text in self.parser.scripts.labels[label]['body'] if text.startswith(needle))
        paths=self.parser.paths_to(label,line)
        self.assertTrue(paths)
        self.assertFalse([d for d in self.parser.diagnostics if d.get('target')==(label,line)])
        return paths

    def test_boolean_opposite_values_are_incompatible(self):
        self.assertTrue(_contradiction([('FLAG_ENTERED_CONTEST','eq','TRUE')],('FLAG_ENTERED_CONTEST','eq','FALSE')))
        self.assertFalse(_contradiction([('FLAG_ENTERED_CONTEST','eq','TRUE')],('FLAG_ENTERED_CONTEST','ne','FALSE')))
        self.assertIsNone(_known_result(('PLAYER_GENDER','eq','MALE')))
        self.assertTrue(_known_result(('4','eq','CONTESTANT_COUNT')))

    def test_milk_purchase_retains_funds_check_and_actual_debit(self):
        paths=self.query('LavaridgeTown_PokemonCenter_1F_EventScript_Buy12MoomooMilk','giveitem')
        for path in paths:
            self.assertTrue(any(c['key']=='CHECKMONEY:6000' for c in path['conditions']))
            self.assertTrue(any(e['op']=='removemoney' and e['args']==['6000'] for e in path['effects']))

    def test_independent_papers_are_factored_without_granting_them(self):
        paths=self.query('EC_Deliver_MYSTIC_TICKET','giveitem')
        for path in paths:
            self.assertTrue(any(c['key']=='FLAG_ENABLE_SHIP_NAVEL_ROCK' for c in path['conditions']))
            self.assertFalse(any(e['op']=='giveitem' and e['args'][0]!='ITEM_MYSTIC_TICKET' for e in path['effects']))
            self.assertTrue(any(e['op']=='optional_delivery_summary' for e in path['effects']))

    def test_center_clerk_empty_alias_keeps_actual_map_roots(self):
        paths=self.query('EmeraldChampions_EventScript_TryStarterKit','special GiveEmeraldChampionsStarterBattleItems')
        locations={tuple(path['event'][:3]) for path in paths}
        self.assertIn(('OldaleTown_PokemonCenter_1F',2,2),locations)
        self.assertIn(('EverGrandeCity_PokemonLeague_1F',15,2),locations)
        for path in paths:
            self.assertTrue(any(c['key']=='FLAG_EC_RECEIVED_STARTER_BATTLE_ITEMS' and c['value']=='TRUE' and c['op']=='ne'
                for c in path['conditions']))

    def test_easy_chat_output_replaces_its_input_type(self):
        paths=self.query('Route123_BerryMastersHouse_EventScript_GiveSpelonBerry','giveitem')
        for path in paths:
            self.assertTrue(any(c['key'].startswith('SPECIAL:NativeScratchOutput(function=ShowEasyChatScreen,variable=VAR_0x8004,')
                and c['value']=='PHRASE_GREAT_BATTLE' for c in path['conditions']))

    def test_game_corner_compares_actual_coin_balance_to_price(self):
        paths=self.query('MauvilleCity_GameCorner_EventScript_ConfirmStarterPrize','special GiveEmeraldChampionsGameCornerPokemon')
        for path in paths:
            self.assertEqual(path['assignments']['VAR_0x8007'],'CHECKCOINS:VAR_0x8007')
            self.assertTrue(any(c['key']=='CHECKCOINS:VAR_0x8007' and c['op']=='ge' for c in path['conditions']))

    def test_removeobject_native_hide_flag_keeps_brawly_completed_event(self):
        result=extract_completed_event_transitions(self.parser.scripts,{'FLAG_HIDE_SLATEPORT_CITY_BRAWLY'})
        paths=[p for p in result['transitions'] if p['entry']=='SlateportCity_EventScript_Brawly']
        self.assertTrue(paths)
        self.assertFalse(result['diagnostics'])
        for path in paths:
            self.assertIs(path['final_writes']['FLAG_HIDE_SLATEPORT_CITY_BRAWLY'],True)
            self.assertTrue(any(e.get('implicit_from')=='removeobject' for e in path['effects']))

    def test_independent_map_setup_and_all_ferry_commits_do_not_truncate(self):
        keys={'VAR_LITTLEROOT_HOUSES_STATE_MAY','VAR_LITTLEROOT_HOUSES_STATE_BRENDAN',
            'VAR_OLDALE_RIVAL_STATE','VAR_MIRAGE_TOWER_STATE','FLAG_HIDE_ROUTE111_CHANSEY',
            'FLAG_HIDE_ROUTE_111_NURSE','VAR_SS_TIDAL_STATE','FLAG_SHOWN_AURORA_TICKET',
            'FLAG_SHOWN_EON_TICKET','FLAG_SHOWN_OLD_SEA_MAP','FLAG_SHOWN_MYSTIC_TICKET',
            'VAR_TEMP_0','VAR_TEMP_1'}
        result=extract_completed_event_transitions(self.parser.scripts,keys,
            maps={'LittlerootTown','Route111','LilycoveCity_Harbor','MauvilleCity_GameCorner','SlateportCity_PokemonFanClub'})
        self.assertFalse(result['diagnostics'])
        written={key for path in result['transitions'] for key in path['final_writes']}
        self.assertTrue({'VAR_SS_TIDAL_STATE','FLAG_SHOWN_AURORA_TICKET','FLAG_SHOWN_EON_TICKET',
            'FLAG_SHOWN_OLD_SEA_MAP','FLAG_SHOWN_MYSTIC_TICKET'} <= written)
        self.assertFalse(any(path['entry'] in ('MauvilleCity_GameCorner_EventScript_PrizeCornerPokemon',
            'MauvilleCity_GameCorner_EventScript_PrizeCornerStarter','SlateportCity_PokemonFanClub_EventScript_Chairman')
            for path in result['transitions']))

    def test_wally_caller_aftermath_requires_the_completed_battle(self):
        result=extract_completed_event_transitions(self.parser.scripts,{'FLAG_DEFEATED_WALLY_MAUVILLE'},maps={'MauvilleCity'})
        completed=[p for p in result['transitions'] if p['final_writes'].get('FLAG_DEFEATED_WALLY_MAUVILLE')]
        self.assertTrue(completed)
        self.assertFalse(result['diagnostics'])
        for path in completed:
            self.assertIn('TRAINER_WALLY_MAUVILLE',path['prior_battles'])
            self.assertTrue(any(effect['op']=='removeobject' for effect in path['effects']))
        self.assertFalse(any(p['final_writes'].get('FLAG_DEFEATED_WALLY_MAUVILLE') and not p['prior_battles']
            for p in result['transitions']))

    def test_cosmetic_heart_summary_rejects_real_movement(self):
        from unittest.mock import patch
        label = 'ContestHall_EventScript_AudienceHeartEmotes'
        movement = self.parser.scripts.labels['ContestHall_Movement_Heart']
        self.assertIn(label, self.parser._verified_cosmetic_summaries())
        with patch.dict(movement, body=[(0, 'walk_right'), (1, 'step_end')]):
            self.assertNotIn(label, self.parser._verified_cosmetic_summaries())

    def test_contest_and_scarf_paths_finish_without_truncation(self):
        self.query('ContestHall_EventScript_GiveLuxuryBall','giveitem')
        for color in ('Red','Blue','Pink','Green','Yellow'):
            self.query('SlateportCity_PokemonFanClub_EventScript_Give'+color+'Scarf','giveitem')


if __name__=='__main__':unittest.main()
