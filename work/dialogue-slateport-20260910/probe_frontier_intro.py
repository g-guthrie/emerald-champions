import sys,struct
sys.path.insert(0,'work/dialogue-slateport-20260910')
from walk_island_maps import walk,execute,sy,out
from probe_southern import flag,var
p=flag('FLAG_RECEIVED_SS_TICKET',True)+flag('FLAG_HIDE_BATTLE_FRONTIER_RECEPTION_GATE_SCOTT',False)+var(0x40d0,0)+bytes([0x2a])+struct.pack('<H',0x8d2)+bytes([0x39,26,4,255])+struct.pack('<HH',19,67)+bytes([0x27,0x6b,2])
a=sy['gStringVar4']+800;c=sy['sGlobalScriptContext'];w=[(50,1,a+i,v) for i,v in enumerate(p)]+[(50,1,c,0),(50,4,c+8,a),(50,1,c+1,1),(50,1,sy['sGlobalScriptContextStatus'],0),(50,4,sy['gEcHeadlessCampaignQueryKind'],1),(50,4,sy['gEcHeadlessCampaignQueryId'],0x8d2)]
execute('frontier-ferry-arrival','postgame-audit-base',1500,(),w)
s=walk('BattleFrontier_OutsideWest','frontier-ferry-arrival','frontier-reception-enter',warp=8)
execute('frontier-reception-complete',s,19000,[(f,2,'B') for f in range(300,18200,300)],shots=[700,2200,4800,7400,10000,13000,17000])
s=walk('BattleFrontier_ReceptionGate','frontier-reception-complete','frontier-main-grounds',warp=1)
(out/'frontier-main-grounds-state.txt').write_text(s+'\n')
s=walk('BattleFrontier_OutsideWest',s,'frontier-scott-house-enter',warp=5)
(out/'frontier-scott-house-state.txt').write_text(s+'\n')
