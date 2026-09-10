import sys,struct,re
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_circuit_storage import inject
from probe_southern import execute,sy,var
names=['CALYREX','CELESTEELA','GLASTRIER','NECROZMA','SPECTRIER','XURKITREE','ZACIAN','ZAMAZENTA','ZARUDE','KORAIDON','MIRAIDON','ETERNATUS']
p=var(0x40dc,40)+bytes([0x6b,2])
inject('circuit-mastery-ready','circuit-entry-cancel',p,extra=[(50,4,sy['gEcHeadlessCampaignQueryKind'],2),(50,4,sy['gEcHeadlessCampaignQueryId'],0x40e5)])
state='circuit-mastery-ready'
for i,name in enumerate(names):
 tag='circuit-mastery-'+name.lower()
 s=execute(tag,state,8500,[(200,2,'A')]+[(f,2,'B') for f in range(1200,8000,700)],shots=[1000,2200])
 assert re.findall('value=([0-9a-f]+)',s)[:3]==['00000000']*3,(tag,s)
 p=var(0x8004,254)+bytes([0x26])+struct.pack('<HH',0x800d,sy['SPECIAL_GetTradeSpecies'])+bytes([0x6b,2])
 extra=[(50,2,sy['gSpecialVar_MonBoxId'],0),(50,2,sy['gSpecialVar_MonBoxPos'],i),(50,4,sy['gEcHeadlessCampaignQueryKind'],2),(50,4,sy['gEcHeadlessCampaignQueryId'],0x800d)]
 r=inject(tag+'-species',tag,p,frames=100,extra=extra)
 actual=int(re.search('value=([0-9a-f]+)',r)[1],16);print('DELIVERED',name,actual,sy['SPECIES_'+name],flush=True);assert actual==sy['SPECIES_'+name]
 state=tag
execute('circuit-mastery-repeat',state,6000,[(200,2,'A')]+[(f,2,'B') for f in range(1200,5500,700)],shots=[1000,2200])
