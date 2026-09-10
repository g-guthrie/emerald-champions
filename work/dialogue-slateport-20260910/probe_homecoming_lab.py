import sys,struct
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_homecoming import run,sy
for gender,early in [('male',False),('female',False),('male',True)]:
 state='homecoming-'+gender+('-early-pass' if early else '-complete');tag='homecoming-lab-path-'+gender+('-met-scott' if early else '')
 keys=[]
 directions=(['DOWN']*6 if early else (['RIGHT'] if gender=='female' else ['LEFT'])*2+['DOWN']*3+(['LEFT'] if gender=='female' else ['RIGHT'])*6+['DOWN'])
 for i,d in enumerate(directions):keys.append((300+i*220,16,d))
 keys +=[(f,2,'B') for f in range(3800,24000,450)]
 writes=[(50,4,sy['gEcHeadlessCampaignQueryKind'],2),(50,4,sy['gEcHeadlessCampaignQueryId'],0x40d3)]
 if early:
  p=bytes([0x29])+struct.pack('<H',0x1d0)+bytes([0x6b,2]);addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext']
  writes +=[(50,1,addr+i,v) for i,v in enumerate(p)]+[(50,1,ctx,0),(50,4,ctx+8,addr),(50,1,ctx+1,1),(50,1,sy['sGlobalScriptContextStatus'],0),(50,4,sy['gEcHeadlessCampaignQueryKind'],1),(50,4,sy['gEcHeadlessCampaignQueryId'],0x72)]
 run(tag,state,26000,keys,writes,[2700,4500,7000,10500,14000,18000])
