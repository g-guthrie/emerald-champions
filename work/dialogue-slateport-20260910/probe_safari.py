from pathlib import Path
import subprocess,sys,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
def probe(spec):
 param,mode=spec
 tag=str(param)+mode
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames','12500','--screenshot',str(out/f'safari-{tag}-end.png')]
 for f,n,v in [(59,'gEcHeadlessFixtureParam',param),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',9)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
 keys=[(300,16,'LEFT'),(1000,2,'A'),(1250,2,'A'),(1550,2,'A')]+[(f,2,'B') for f in range(1900,8500,200)]
 if mode:
  keys=[x for x in keys if x[0]<5000]
  if mode=='timer':
   cmd+=['--write',f'5400:2:{sy["gSafariZoneStepCounter"]}:1']
   keys += [(5600,16,'DOWN')]
  else:
   keys += [(5200,2,'UP'),(5300,2,'A'),(5700,2,'B'),(6100,2,'B')]
   if mode=='west':keys += [(6500,16,'LEFT'),(6800,16,'UP'),(7100,2,'RIGHT')]
   keys += [(7500,2,'A'),(8100,2,'A')]
  keys += [(f,2,'B') for f in range(8600,12000,200)]
 for f,d,k in keys:cmd+=['--key',f'{f}:{d}:{k}']
 for f in [900,1800,2800,4800]:cmd+=['--screenshot-at',f'{f}:{out}/safari-{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:cmd+=['--read',f'4:{sy[n]}']
 for n,w in [('gNumSafariBalls',1),('gSafariZoneStepCounter',2)]:cmd+=['--read',f'{w}:{sy[n]}']
 r=subprocess.run(cmd,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(probe,[(125,m) for m in sys.argv[1:]] if len(sys.argv)>1 else [(p,'') for p in range(125,129)]))
