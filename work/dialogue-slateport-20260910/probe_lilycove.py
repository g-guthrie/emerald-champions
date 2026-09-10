from pathlib import Path
import subprocess,sys,re,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
flags=Path('include/constants/flags.h').read_text()
def flag(n):
 v=re.search(r'#define '+n+r'\s+([^\n/]+)',flags)[1].strip()
 return eval(v,{'SYSTEM_FLAGS':0x860,'__builtins__':{}})
def probe(spec):
 mode=str(spec).split(":")[1] if ":" in str(spec) else ""
 p=int(str(spec).split(":")[0])
 tag=str(p)+mode
 end=22000 if mode else 16000
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(end),'--screenshot',str(out/f'lily-{tag}-end.png')]
 q=flag('FLAG_MET_RIVAL_LILYCOVE' if p<=134 else 'FLAG_ENABLE_SHIP_FARAWAY_ISLAND' if p in [135,136,139] else 'FLAG_SHOWN_OLD_SEA_MAP')
 for f,n,v in [(59,'gEcHeadlessFixtureParam',p),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',1),(70,'gEcHeadlessCampaignQueryId',q)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
 keys=[(300,2,'RIGHT' if p in [138,140] else 'UP'),(400,2,'A')]
 if p<=134 or p==135:
  keys += [(f,2,'A') for f in range(850,6000 if p==135 else 2600,250)]
  start=6500 if p==135 else 3000
 else:start=850
 keys += [(f,2,'B') for f in range(start,end-300,180)]
 if mode:
  keys=[(300,2,'UP'),(400,2,'A')]+[(f,2,'B') for f in range(850,5000,180)]
  if mode=='retry':keys += [(6000,2,'A')]+[(f,2,'A') for f in range(6400,8200,250)]+[(f,2,'B') for f in range(8600,end-300,180)]
 for f,d,k in keys:cmd+=['--key',f'{f}:{d}:{k}']
 for f in [900,1500,2500,5000,9000]:cmd+=['--screenshot-at',f'{f}:{out}/lily-{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignLastOpponentA','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:cmd+=['--read',f'4:{sy[n]}']
 r=subprocess.run(cmd,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(probe,sys.argv[1:] or range(129,141)))
