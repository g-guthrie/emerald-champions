import sys,struct
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_homecoming import run,sy,var,flag
for side,start in [('east',1),('west',5)]:
 p=var(0x40d4,1)+var(0x40b4,start)+flag('FLAG_HIDE_SS_TIDAL_CORRIDOR_SCOTT',True)+bytes([0x39,25,41,255])+struct.pack('<HH',1,10)+bytes([0x27,0x6b,2])
 addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext'];w=[(50,1,addr+i,v) for i,v in enumerate(p)]
 w +=[(50,1,ctx,0),(50,4,ctx+8,addr),(50,1,ctx+1,1),(50,1,sy['sGlobalScriptContextStatus'],0),(50,4,sy['gEcHeadlessCampaignQueryKind'],2),(50,4,sy['gEcHeadlessCampaignQueryId'],0x40b4)]
 keys=[(f,2,'B') for f in range(600,2200,400)]+[(2500,96,'RIGHT'),(3000,24,'UP')]
 run('tidal-'+side+'-cabin','postgame-audit-base',4300,keys,w,[1000,2700,3500])
