import sys,struct
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_homecoming import run,sy,var,flag
p=var(0x40d4,1)+flag('FLAG_RECEIVED_SS_TIDAL_REAPER_CLOTH',False)+flag('FLAG_HIDE_SS_TIDAL_ROOMS_SNATCH_GIVER',False)+bytes([0x39,25,43,255])+struct.pack('<HH',28,6)+bytes([0x27,0x6b,2])
addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext'];w=[(50,1,addr+i,v) for i,v in enumerate(p)]
w +=[(50,1,ctx,0),(50,4,ctx+8,addr),(50,1,ctx+1,1),(50,1,sy['sGlobalScriptContextStatus'],0),(50,4,sy['gEcHeadlessCampaignQueryKind'],4),(50,4,sy['gEcHeadlessCampaignQueryId'],233)]
run('tidal-reaper-cloth','postgame-audit-base',9000,[(700,2,'UP'),(900,2,'A')]+[(f,2,'B') for f in range(1600,4900,500)]+[(5500,2,'A'),(6200,2,'B'),(6900,2,'B')],w,[1450,3500,5800])
