import sys,struct
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_homecoming import run,sy,var,flag
for name,state,cancel in [('east',2,False),('west',7,False),('cancel',2,True)]:
 p=var(0x40d4,1)+var(0x40b4,state)+var(0x404a,0)+flag('FLAG_HIDE_SS_TIDAL_CORRIDOR_SCOTT',True)+bytes([0x29])+struct.pack('<H',0x88d)+bytes([0x39,25,41,255])+struct.pack('<HH',2,2)+bytes([0x27,0x6b,2])
 addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext'];w=[(50,1,addr+i,v) for i,v in enumerate(p)]
 w +=[(50,1,ctx,0),(50,4,ctx+8,addr),(50,1,ctx+1,1),(50,1,sy['sGlobalScriptContextStatus'],0),(50,4,sy['gEcHeadlessCampaignQueryKind'],2),(50,4,sy['gEcHeadlessCampaignQueryId'],0x40b4)]
 keys=[(700,2,'UP'),(900,2,'A')]+([(1900,2,'A')] if cancel else [])+[(f,2,'B') for f in [4800,5600,6400,7200]]
 run('tidal-porthole-'+name,'postgame-audit-base',8500,keys,w,[1400,2400,3800,5000,7600])
