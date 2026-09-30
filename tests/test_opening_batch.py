"""Opening context coverage uses actual native branch and regional-set code."""
import copy
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'scripts'))
sys.path.insert(0,str(ROOT/'tools/agent_player'))
import export_opening_batch as batch
import battle_opening_arsenal as opening
import generate_battle_suite as suites
from doubles_policy import source_metadata


class OpeningBatchTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.rules=opening.rules()
        cls.raw=batch.raw_starter_sets(cls.rules['starters'])
        cls.regional=batch.regional_opening_sets(cls.raw)
        cls.native=batch.native_branch_rows(cls.rules['starters'])
        cls.opponents={p['trainer_id']:p for p in suites.opponent_catalogue()}
        cls.context={'rules':cls.rules,'source_evidence':[opening.source_evidence(ROOT/p) for p in opening.SOURCE_PATHS]}
        cls.context['source_evidence'].extend(opening.source_evidence(p) for p in sorted((ROOT/'src/data/pokemon/species_info').glob('*.h')))

    def test_all_native_pairs_match_unchosen_species_and_script_branch_on_every_mode(self):
        self.assertEqual(len(self.native),54)
        observed=set()
        for row in self.native:
            g,a,b=row['generation'],row['first'],row['second']
            self.assertEqual(row['unchosen_species'],self.rules['starters'][g][3-a-b])
            for mode in ('easy','medium','hard'):
                for gender in ('male','female'):
                    scenario=opening._opening_scenario(mode,generation=g,first=a,second=b,player_gender=gender,
                        _fingerprint='branch-probe',_context=self.context)
                    identity=batch.expected_opponent(scenario,self.opponents,self.regional,internal_rules=self.rules)
                    self.assertEqual(identity['branch'],row['branch'])
                    self.assertEqual(identity['unchosen_species'],row['unchosen_species'])
                    member=identity['team'][identity['regional_replacement']['slot']-1]
                    self.assertEqual(member['species'],row['unchosen_species'])
                    self.assertTrue(scenario['trainer_id'].startswith('TRAINER_MAY_' if gender=='male' else 'TRAINER_BRENDAN_'))
                    observed.add((g,row['unchosen_index'],mode,gender))
        self.assertEqual(len(observed),162)

    def test_native_regional_patches_and_hoenn_early_return_are_preserved(self):
        self.assertEqual(self.regional['SPECIES_BULBASAUR']['ability'],'ABILITY_OVERGROW')
        self.assertEqual(self.regional['SPECIES_CHARMANDER']['moves'][1],'MOVE_FLAMETHROWER')
        self.assertEqual(self.regional['SPECIES_ROWLET']['moves'][3],'MOVE_PROTECT')
        self.assertEqual(self.regional['SPECIES_SOBBLE']['moves'][2],'MOVE_MUD_SHOT')
        self.assertNotIn('ITEM_EVIOLITE',{p['item'] for p in self.regional.values()})
        scenario=opening._opening_scenario('hard',generation=3,first=0,second=1,_context=self.context)
        identity=batch.expected_opponent(scenario,self.opponents,self.regional,internal_rules=self.rules)
        self.assertFalse(identity['regional_replacement']['applied'])
        authored=self.opponents[scenario['trainer_id']]['team'][0]
        for field in ('species','moves','ability','item','evs','ivs','nature','friendship'):
            self.assertEqual(identity['team'][0][field],authored[field])
        scenario=opening._opening_scenario('hard',generation=1,first=0,second=1,_context=self.context)
        changed=batch.expected_opponent(scenario,self.opponents,self.regional,internal_rules=self.rules)
        self.assertTrue(changed['regional_replacement']['applied'])
        self.assertEqual(changed['team'][0]['ivs'],[31]*6)

    def test_full_export_hashes_once_and_keeps_unplayed_gender_and_order_coverage_explicit(self):
        with tempfile.TemporaryDirectory(dir=ROOT/'work',prefix='opening-batch-test-') as parent:
            output=Path(parent)/'batch'
            with patch.object(suites,'input_hashes',wraps=suites.input_hashes) as hashes:
                report=batch.export_batch(output)
                self.assertEqual(hashes.call_count,1)
            self.assertEqual(report['config_count'],81)
            self.assertEqual(len(report['contexts']),162)
            self.assertEqual(len(report['reversed_order_pending']),162)
            self.assertTrue(all(c['native_execution']=='pending' for c in report['contexts']))
            import json
            facts=source_metadata()['moves']
            for row in report['contexts']:
                if not row['included']:continue
                manifest=json.loads((output/row['party_file']).read_text())
                self.assertEqual(len(manifest['party']),6)
                for mon in manifest['party']:
                    if mon['item']=='ITEM_CHOICE_BAND':
                        self.assertTrue(all(facts[m]['category']=='PHYSICAL' for m in mon['moves']))
                    if mon['item']=='ITEM_EVIOLITE':
                        self.assertTrue(self.rules['evolutions'][mon['species']])
                    self.assertEqual(mon['pp_bonuses'],0)
                    self.assertEqual(mon['pokerus'],0)

    def test_serialized_producer_cannot_supply_trusted_internal_context(self):
        scenario=opening._opening_scenario(_context=self.context)
        for name,value in (('_context',self.context),('_fingerprint','forged'),('internal_context',self.context)):
            forged=copy.deepcopy(scenario)
            forged['producer']['arguments'][name]=value
            with self.assertRaisesRegex(ValueError,'unsupported internal'):
                opening.certify_scenario(forged,internal_context=self.context)


if __name__=='__main__':unittest.main()
