import sys,struct
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_southern import execute,sy,var,out
mid=sy['MAP_BATTLE_FRONTIER_EXCHANGE_SERVICE_CORNER']
p=var(0x8004,20)+bytes([0x23])+struct.pack('<I',sy['GiveFrontierBattlePoints']|1)
p+=bytes([0x39,mid>>8,mid&255,255])+struct.pack('<HH',9,7)+bytes([0x27,0x6b,2])
a=sy['gStringVar4']+800;c=sy['sGlobalScriptContext']
w=[(50,1,a+i,v) for i,v in enumerate(p)]+[(50,1,c,0),(50,4,c+8,a),(50,1,c+1,1),(50,1,sy['sGlobalScriptContextStatus'],0),(50,4,sy['gEcHeadlessCampaignQueryKind'],11)]
execute('frontier-exchange-ready','postgame-audit-base',1500,(),w)
execute('frontier-exchange-supplies','frontier-exchange-ready',3500,[(200,2,'UP'),(400,2,'A'),(1300,2,'B'),(2000,2,'B'),(2700,2,'B')],shots=[900,1700,2400,3200])
