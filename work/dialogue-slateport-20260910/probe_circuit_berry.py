import sys,struct
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_southern import execute,sy,var
from probe_circuit_storage import inject
p=b''
for slot in range(6):
 species=sy['SPECIES_SNORLAX'] if slot==0 else sy['SPECIES_INDEEDEE_F'] if slot==1 else sy['SPECIES_MAGIKARP']
 moves=[sy['MOVE_BELLY_DRUM'],sy['MOVE_PROTECT'],sy['MOVE_TACKLE'],sy['MOVE_REST']] if slot==0 else [sy['MOVE_FOLLOW_ME'],sy['MOVE_PROTECT'],sy['MOVE_PSYCHIC'],sy['MOVE_HELPING_HAND']] if slot==1 else [sy['MOVE_SPLASH'],0,0,0]
 item=sy['ITEM_SITRUS_BERRY'] if slot==0 else 0
 p+=bytes([0x23])+struct.pack('<I',(sy['ScrCmd_createmon']|1)+0x02000000)+struct.pack('<BBHHIH4H',0,slot,species,80,1|(15<<17),item,*moves)
p+=bytes([0x6b,2])
inject('circuit-berry-ready','circuit-entry-cancel-ready',p,extra=[(50,4,sy['gEcHeadlessFixtureActiveScenario'],0)])
execute('circuit-berry-offer','circuit-berry-ready',5000,[(200,2,'UP'),(400,2,'A')]+[(f,2,'B') for f in range(1200,4600,700)],shots=[4400])
