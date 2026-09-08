import contextlib
import io
import json
from pathlib import Path
import sys
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'scripts'))
import verify_daycare_discoveries as gate


class DaycareDiscoveries(unittest.TestCase):
    def rejected(self, relative, transform):
        original = Path.read_text
        target = gate.ROOT / relative
        changed = transform(original(target))
        def read(path, *args, **kwargs):
            return changed if path == target else original(path, *args, **kwargs)
        with patch.object(Path, 'read_text', read), contextlib.redirect_stdout(io.StringIO()):
            with self.assertRaises(AssertionError):
                gate.verify()

    def test_rejects_removing_early_munchlax(self):
        def remove(source):
            data = json.loads(source)
            group = next(g for g in data['wild_encounter_groups'] if g['label'] == 'gWildMonHeaders')
            entry = next(e for e in group['encounters'] if e['map'] == 'MAP_ROUTE116')
            next(m for m in entry['land_mons']['mons'] if m['species'] == 'SPECIES_MUNCHLAX')['species'] = 'SPECIES_SKWOVET'
            return json.dumps(data)
        self.rejected('src/data/wild_encounters.json', remove)

    def test_rejects_reward_before_bred_egg_hatches(self):
        self.rejected('data/maps/Route117_PokemonDayCare/scripts.inc',
                      lambda s: s.replace('call_if_set FLAG_EC_HATCHED_DAYCARE_EGG,', 'call_if_set FLAG_EC_RECEIVED_DAYCARE_TOGEPI_EGG,'))

    def test_rejects_counting_a_gift_egg(self):
        self.rejected('src/egg_hatch.c', lambda s: s.replace('if (GetMonData(mon, MON_DATA_MET_LOCATION) == METLOC_DAYCARE_EGG)', 'if (TRUE)'))

    def test_rejects_consuming_reward_on_full_bag(self):
        self.rejected('data/maps/Route117_PokemonDayCare/scripts.inc',
                      lambda s: s.replace('\tgoto_if_eq VAR_RESULT, FALSE, Route117_PokemonDayCare_EventScript_HatchRewardBagFull\n', ''))

    def test_rejects_old_kangaskhanite_pickup(self):
        def restore(source):
            data = json.loads(source)
            item = next(o for o in data['object_events'] if o.get('flag') == 'FLAG_EC_GARDEN_BUNDLE_ROUTE120_WEPEAR_BERRY')
            item.update(trainer_sight_or_berry_tree_id='ITEM_KANGASKHANITE', graphics_id='OBJ_EVENT_GFX_MEGA_STONE', movement_type='MOVEMENT_TYPE_LOOK_AROUND', movement_range_x=0)
            return json.dumps(data)
        self.rejected('data/maps/Route120/map.json', restore)


if __name__ == '__main__':
    unittest.main()
