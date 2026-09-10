from pathlib import Path
import subprocess,sys,re,json,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');s=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
flags=Path('include/constants/flags.h').read_text();vs=Path('include/constants/vars.h').read_text();pos=json.loads((out/'seafloor-sign-positions.json').read_text())
maps=['SeafloorCavern_Entrance']+[f'SeafloorCavern_Room{i}' for i in range(1,9)]+['Route126','Route127','Route128','Underwater_Route126']
def flag(n):return eval(re.search(r'#define '+n+r'\s+([^\n/]+)',flags)[1].strip(),{'SYSTEM_FLAGS':0x860,'__builtins__':{}})
def var(n):return int(re.search(r'#define '+n+r'\s+(0x\w+)',vs)[1],0)
def probe(arg):
 p=int(str(arg).split(':')[0]);opts=str(arg).split(':')[1:];tag=str(arg).replace(':','-');end=30000 if p==190 else 15000
 if any(o.startswith('end=') for o in opts):end=int(next(o for o in opts if o.startswith('end=')).split('=')[1])
 kind,q=(2,var('VAR_ROUTE128_STATE' if p==190 else 'VAR_SEAFLOOR_CAVERN_STATE'))
 if p==194 or 'sub' in opts:kind,q=3,1
 if p==197:kind,q=2,var('VAR_HAS_TALKED_TO_SEAFLOOR_CAVERN_ENTRANCE_GRUNT')
 if p>=200:kind,q=3,len(json.loads((Path('data/maps')/maps[p-200]/'map.json').read_text())['object_events'])
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(end),'--screenshot',f'{out}/seafloor-{tag}-end.png']
 for f,n,v in [(59,'gEcHeadlessFixtureParam',p),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',kind),(70,'gEcHeadlessCampaignQueryId',q)]:c+=['--write',f'{f}:4:{s[n]}:{v}']
 keys=[];dismiss=850
 if p<=192:keys=[(300,16,'UP')]
 if p in [193,194]:keys=[(400,2,'B'),(850,2,'A'),(1200,2,'A')];dismiss=3000
 if p==195:keys=[(400,2,'A'),(850,2,'A'),(1200,2,'A')];dismiss=3000
 if p==196:keys=[(300,16,'UP'),(1100,2,'B'),(1550,2,'A'),(1900,2,'A')];dismiss=4000
 if p==197 or p>=200:keys=[(300,2,'DOWN' if p in [202,203] else 'UP'),(500,2,'A')]
 if 'sub' in opts:keys=[]
 if p==195 and 'full' in opts:
  keys += [(2000,16,'UP'),(3000,2,'B'),(3400,2,'A'),(3800,2,'A')];dismiss=5000
 if p==210 and 'after' in opts:keys += [(7000,2,'UP'),(7500,2,'A')]
 if 'inspect' not in opts and 'sub' not in opts:keys += [(f,2,'B') for f in range(dismiss,end-200,220) if 'after' not in opts or not 6800<f<8000]
 for f,d,k in sorted(keys):c+=['--key',f'{f}:{d}:{k}']
 for f in [750,1400,2600,4200,6000,9000,12500]+([17000,22000] if p==190 else []):
  if f<end:c+=['--screenshot-at',f'{f}:{out}/seafloor-{tag}-{f}.png']
 if p in [193,194,195,196,212]:c+=['--screenshot-on-change',f'4:{s["gEcHeadlessCampaignScriptEnabled"]}:0:{out}/seafloor-{tag}-script']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{s[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(probe,sys.argv[1:] or [*range(190,198),*range(200,213)]))
