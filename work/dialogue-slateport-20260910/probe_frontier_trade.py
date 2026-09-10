import sys,struct
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_southern import execute,sy,out
from probe_circuit_storage import inject
p=b''
for slot in range(6):
 p+=bytes([0x23])+struct.pack('<I',(sy['ScrCmd_createmon']|1)+0x02000000)+struct.pack('<BBHHIHHHH',0,slot,sy['SPECIES_SKITTY'] if slot==4 else sy['SPECIES_MAGIKARP'],50,15<<17,33,0,0,0)
mid=sy['MAP_BATTLE_FRONTIER_LOUNGE6'];p+=bytes([0x2a])+struct.pack('<H',0x9c)+bytes([0x39,mid>>8,mid&255,255])+struct.pack('<HH',2,5)+bytes([0x27,0x6b,2])
assert len(p)<200
inject('frontier-null-ready','frontier-audit-base',p,extra=[(50,4,sy['gEcHeadlessCampaignQueryKind'],1),(50,4,sy['gEcHeadlessCampaignQueryId'],0x9c)])
execute('frontier-null-offer','frontier-null-ready',3500,[(200,2,'UP'),(400,2,'A'),(1400,2,'B'),(2400,2,'B')],shots=[1100,2100,3200])
execute('frontier-null-declined','frontier-null-offer',3000,[(200,2,'B'),(1200,2,'B'),(2100,2,'B')],shots=[800])
execute('frontier-null-choice','frontier-null-offer',1800,[(200,2,'A')],shots=[1400])
