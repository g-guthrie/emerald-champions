import sys,struct
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_southern import execute,sy,flag,var
from probe_circuit_storage import inject
from walk_island_maps import walk
p=flag('FLAG_RECEIVED_HM_SURF',True)+flag('FLAG_RECEIVED_HM_DIVE',True)+var(0x4021,5000)
for f in range(0x867,0x86f):p+=bytes([0x29])+struct.pack('<H',f)
mid=sy['MAP_ROUTE108'];p+=bytes([0x39,mid>>8,mid&255,255])+struct.pack('<HH',29,7)+bytes([0x27,0x6b,2])
inject('abandoned-route108-ready','circuit-lobby-base',p,extra=[(50,4,sy['gEcHeadlessFixtureActiveScenario'],71),(50,4,sy['gEcHeadlessFixtureParam'],241)])
s=walk('Route108','abandoned-route108-ready','abandoned-deck-entry',warp=0)
s=walk('AbandonedShip_Deck',s,'abandoned-corridor-entry',warp=2)
print('CHECKPOINT',s)
