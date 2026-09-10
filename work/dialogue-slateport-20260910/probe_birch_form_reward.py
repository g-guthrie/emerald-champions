import sys,struct
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_homecoming import run,sy,var
for owned in [False,True]:
 p=b''.join(var(n,v) for n,v in [(0x4084,5),(0x4085,7),(0x40e2,4),(0x40d3,4)])+bytes([0x29])+struct.pack('<H',0x864)
 if owned:
  for item in [697,698]:p+=bytes([0x44])+struct.pack('<HH',item,1)
 p+=bytes([0x39,1,4,255])+struct.pack('<HH',6,12)+bytes([0x27,0x6b,2])
 addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext'];w=[(50,1,addr+i,v) for i,v in enumerate(p)]
 w +=[(50,1,ctx,0),(50,4,ctx+8,addr),(50,1,ctx+1,1),(50,1,sy['sGlobalScriptContextStatus'],0),(50,4,sy['gEcHeadlessCampaignQueryKind'],2),(50,4,sy['gEcHeadlessCampaignQueryId'],0x40d3)]
 tag='birch-form-kit-'+('part-owned' if owned else 'empty')
 run(tag,'postgame-audit-base',18000,[(f,2,'B') for f in range(1500,16000,500)],w,[2400,5000,8500,12000,16000])
