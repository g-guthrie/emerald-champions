"""Check the approved habitat edits without rewriting levels or other slots."""
import json
import unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]

class UnrestrictedHabitats(unittest.TestCase):
    def test_land_slots_and_rates(self):
        data=json.loads((ROOT/'src/data/wild_encounters.json').read_text())
        maps={r['map']:r for g in data['wild_encounter_groups'] for r in g.get('encounters',[]) if 'map' in r}
        for name,slot,species,levels in (
            ('ASHEN_WOODS',11,'URSARING',(41,43)),
            ('SAFARI_ZONE_SOUTHEAST',10,'URSALUNA_BLOODMOON',(50,50)),
            ('ROUTE116',10,'ROOKIDEE',(8,10)),
            ('ROUTE121',10,'GIMMIGHOUL_ROAMING',(45,45)),
            ('MIRAGE_TOWER_B1F',2,'SANDILE',(39,39)),
            ('MT_PYRE_2F',9,'GIMMIGHOUL_CHEST',(51,51)),
        ):
            with self.subTest(map=name):
                mon=maps['MAP_'+name]['land_mons']['mons'][slot]
                self.assertEqual(mon['species'],'SPECIES_'+species)
                self.assertEqual((mon['min_level'],mon['max_level']),levels)
        for name,slots in [('ASHEN_WOODS',{4:12,11:4}),('METEOR_FALLS_STEVENS_CAVE',{2:16,4:15})]:
            rates=maps['MAP_'+name]['land_mons']['encounter_rates']
            self.assertEqual(sum(rates),100)
            for slot,rate in slots.items():self.assertEqual(rates[slot],rate)

if __name__=='__main__':unittest.main()
