import copy
from pathlib import Path
import sys
import unittest
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'scripts/playthrough'))
import battle_driver as driver
class AliasTests(unittest.TestCase):
 def test_real_aliases_pass_and_actual_forms_fail(self):
  c=driver.build_constants();values=c['species']['values']
  cases=[('SPECIES_FURFROU','SPECIES_FURFROU_NATURAL','SPECIES_FURFROU_HEART'),('SPECIES_VIVILLON','SPECIES_VIVILLON_ICY_SNOW','SPECIES_VIVILLON_MEADOW'),('SPECIES_PUMPKABOO','SPECIES_PUMPKABOO_AVERAGE','SPECIES_PUMPKABOO_SUPER'),('SPECIES_GIMMIGHOUL','SPECIES_GIMMIGHOUL_CHEST','SPECIES_GIMMIGHOUL_ROAMING')]
  for alias,canonical,other in cases:
   self.assertEqual(values[alias],values[canonical]);self.assertNotEqual(values[alias],values[other])
   mon={'slot':1,'species':canonical,'level':14,'item':'ITEM_NONE','ability':'ABILITY_NONE','nature':'NATURE_HARDY','friendship':50,'evs':[0]*6,'ivs':[31]*6,'moves':['MOVE_NONE']*4}
   roster={'generation':3,'unchosen_index':2,'unchosen_species':canonical,'owners':[{'owner':'A','trainer_id_native':1,'team':[mon]},{'owner':'B','trainer_id_native':0,'team':[]}]}
   expected={'trainer_id':'TRAINER_TEST','unchosen_species':alias,'team':[dict(mon,species=alias)]}
   driver.verify_opponent_identity(expected,roster,{'TRAINER_TEST':1},values)
   wrong=copy.deepcopy(roster);wrong['owners'][0]['team'][0]['species']=other
   with self.assertRaises(SystemExit):driver.verify_opponent_identity(expected,wrong,{'TRAINER_TEST':1},values)
   wrong=copy.deepcopy(expected);wrong['team'][0]['nature']='NATURE_JOLLY'
   with self.assertRaises(SystemExit):driver.verify_opponent_identity(wrong,roster,{'TRAINER_TEST':1},values)
if __name__=='__main__':unittest.main()
