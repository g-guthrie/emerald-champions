"""Check the approved habitat edits (species and rates). Levels belong to scripts/assign_wild_levels.py."""
import json
import unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]

class UnrestrictedHabitats(unittest.TestCase):
    def test_land_slots_and_rates(self):
        data=json.loads((ROOT/'src/data/wild_encounters.json').read_text())
        maps={r['map']:r for g in data['wild_encounter_groups'] for r in g.get('encounters',[]) if 'map' in r}
        for name,slot,species in (
            ('ASHEN_WOODS',11,'URSARING'),
            ('SAFARI_ZONE_SOUTHEAST',10,'URSALUNA_BLOODMOON'),
            ('ROUTE116',10,'ROOKIDEE'),
            ('ROUTE121',10,'GIMMIGHOUL_ROAMING'),
            ('MIRAGE_TOWER_B1F',2,'SANDILE'),
            ('MT_PYRE_2F',9,'GIMMIGHOUL_CHEST'),
        ):
            with self.subTest(map=name):
                mon=maps['MAP_'+name]['land_mons']['mons'][slot]
                self.assertEqual(mon['species'],'SPECIES_'+species)
        for name,slots in [('ASHEN_WOODS',{4:12,11:4}),('METEOR_FALLS_STEVENS_CAVE',{2:16,4:15})]:
            rates=maps['MAP_'+name]['land_mons']['encounter_rates']
            self.assertEqual(sum(rates),100)
            for slot,rate in slots.items():self.assertEqual(rates[slot],rate)

if __name__=='__main__':unittest.main()
