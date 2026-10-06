"""Campaign availability follows the revised habitats and native pickup gates."""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'scripts'))
import reference_pool as rp
from manifest_battle_progression import ProgressionParser


class Cap40Redesign(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.builder = rp.Builder()
        cls.pools = rp.Pools(cls.builder, rp.Encounters(cls.builder)).result

    def first(self, key, resource):
        return next(r['cap'] for r in self.pools.values() if key in r[resource])

    def test_corrected_evolutions_open_at_authored_caps(self):
        for species, cap in [('BUNEARY', 30), ('LOPUNNY', 30), ('SEWADDLE', 30), ('LEAVANNY', 30),
                             ('CHINGLING', 30), ('CHIMECHO', 30), ('RIOLU', 40), ('LUCARIO', 40),
                             ('WOOBAT', 40), ('SWOOBAT', 40), ('URSALUNA_BLOODMOON', 55)]:
            with self.subTest(species=species):
                self.assertEqual(self.first('SPECIES_' + species, 'species'), cap)

    def test_unrestricted_species_first_appear_at_cap_55(self):
        for species in ('GIMMIGHOUL_CHEST', 'GIMMIGHOUL_ROAMING', 'GHOLDENGO', 'URSALUNA_BLOODMOON'):
            with self.subTest(species=species):
                self.assertEqual(self.first('SPECIES_' + species, 'species'), 55)
        # Teddiursa can already evolve at 30; the restored wild home is Ashen Woods at 40.
        self.assertEqual(self.first('SPECIES_URSARING', 'species'), 30)

    def test_first_wave_stones_are_available_at_cap_45_or_55(self):
        for item in ('AUDINITE', 'BANETTITE', 'BUTTERFRENITE', 'BEEDRILLITE', 'SCOLIPITE', 'CHIMECHITE',
                     'MEGANIUMITE', 'ABOMASITE', 'GOLURKITE', 'SABLENITE', 'STEELIXITE', 'FALINKSITE',
                     'CRABOMINITE', 'CHESNAUGHTITE', 'TATSUGIRINITE', 'DRAMPANITE', 'PINSIRITE',
                     'SCOVILLAINITE', 'SKARMORITE', 'HOUNDOOMINITE', 'ABSOLITE_Z', 'RAICHUNITE_Y'):
            with self.subTest(item=item):
                self.assertIn(self.first('ITEM_' + item, 'items'), (45, 55))

    def test_ordinary_final_forms_and_pre_evolutions_are_unrestricted(self):
        for species in ('SPECIES_GHOLDENGO', 'SPECIES_URSALUNA', 'SPECIES_URSALUNA_BLOODMOON'):
            self.assertIsNone(self.builder.species.restricted_class(species))
        for species in ('SPECIES_GIMMIGHOUL', 'SPECIES_GIMMIGHOUL_ROAMING', 'SPECIES_TEDDIURSA', 'SPECIES_URSARING'):
            self.assertIsNone(self.builder.species.restricted_class(species))

    def test_second_wave_field_pickups_need_sixth_badge(self):
        parser = ProgressionParser(self.builder.scripts)
        for label in ('MtPyre_5F_EventScript_ItemAlakazite', 'NewMauville_Inside_EventScript_ItemManectite',
                      'Route120_EventScript_ItemAerodactylite', 'MagmaHideout_4F_EventScript_ItemCharizarditeX'):
            line = next(n for n, text in self.builder.scripts.labels[label]['body'] if text.startswith('finditem'))
            paths = parser.paths_to(label, line)
            self.assertTrue(paths)
            for path in paths:
                self.assertTrue(any(c['key'] == 'FLAG_BADGE06_GET' and (c['op'], c['value']) in [('eq', 'TRUE'), ('ne', 'FALSE')]
                                    for c in path['conditions']), (label, path['conditions']))

    def test_nonnorman_second_wave_stones_open_at_cap_60(self):
        for item in ('ALAKAZITE', 'MANECTITE', 'AERODACTYLITE'):
            self.assertEqual(self.first('ITEM_' + item, 'items'), 60)
        # Charizard X/Y starter gifts wait for Norman's victory at 55.
        self.assertGreaterEqual(self.first('ITEM_GARCHOMPITE', 'items'), 70)
        self.assertGreaterEqual(self.first('ITEM_ZYGARDITE', 'items'), 70)
        self.assertGreaterEqual(self.first('ITEM_DARKRANITE', 'items'), 70)
