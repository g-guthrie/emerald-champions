from pathlib import Path
import subprocess,sys,re,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
def probe(param):
 end=25000 if param==116 else 13000 if param==115 else 6500
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(end),'--screenshot',str(out/f'slab-{param}-end.png')]
 kind,q=(1,0x4e2) if param<=116 else (2,0x40b0) # override route121 state below from actual constant
 if param in [121,122]:kind,q=4,int(re.search(r'\bITEM_SACHET\s*=\s*(\d+)',Path('include/constants/items.h').read_text())[1])
 if param>=123:q=int(re.search(r'#define VAR_ROUTE121_STATE\s+(0x\w+)',Path('include/constants/vars.h').read_text())[1],0)
 for f,n,v in [(59,'gEcHeadlessFixtureParam',param),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',kind),(70,'gEcHeadlessCampaignQueryId',q)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
 if param<=116:
  keys=[(300,16,'UP'),(700,2,'A'),(950,2,'A'),(2400,16,'UP'),(2700,2,'A')]
  if param==116:
   cmd+=['--write',f'14000:4:{sy["gEcHeadlessFixtureTrigger"]}:2']
   keys += [(14500,16,'UP'),(14900,16,'UP'),(15300,2,'A')]
  for f in range(3200,end-300,160):
   if param==116 and 14000<f<15800:continue
   keys.append((f,2,'B'))
 elif param>=123:
  keys=[(300,16,'RIGHT')]+[(f,2,'B') for f in range(850,end-300,150)]
 else:keys=[(300,2,'UP'),(400,2,'A')]+[(f,2,'B') for f in range(850,end-300,150)]
 for f,d,k in keys:cmd+=['--key',f'{f}:{d}:{k}']
 for f in [900,1100,1300,2100,4000]:cmd+=['--screenshot-at',f'{f}:{out}/slab-{param}-{f}.png']
 names=['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignCaptureSerial','gEcHeadlessCampaignLastCapturedSpecies','gEcHeadlessCampaignCaptureBookkeepingValid','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']
 for n in names:cmd+=['--read',f'4:{sy[n]}']
 r=subprocess.run(cmd,capture_output=True,text=True);print(param,r.returncode,r.stdout,r.stderr,flush=True)
params=list(map(int,sys.argv[1:])) or list(range(115,125))
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(probe,params))
