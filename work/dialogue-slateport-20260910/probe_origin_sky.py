from pathlib import Path
import subprocess,sys,re,json,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');s=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
flags=Path('include/constants/flags.h').read_text();vs=Path('include/constants/vars.h').read_text();pos=json.loads((out/'origin-sky-sign-positions.json').read_text());ice=json.loads((out/'sootopolis-ice-paths.json').read_text())
def flag(n):return eval(re.search(r'#define '+n+r'\s+([^\n/]+)',flags)[1].strip(),{'SYSTEM_FLAGS':0x860,'__builtins__':{}})
def var(n):return int(re.search(r'#define '+n+r'\s+(0x\w+)',vs)[1],0)
def probe(arg):
 p=int(str(arg).split(':')[0]);opts=str(arg).split(':')[1:];tag=str(arg).replace(':','-');again='again' in opts;end=24000 if again or 'recover' in opts else 15000
 if 'checkpoint' in opts:end=7900
 kind,q=2,var('VAR_SKY_PILLAR_STATE')
 if p<=240:kind,q=3,len(json.loads((Path('data/maps')/list(pos)[p-231]/'map.json').read_text())['object_events'])
 if p in [241,242]:kind,q=2,var('VAR_ICE_STEP_COUNT')
 if p in [243,244]:kind,q=3,1
 if p in [245,249]:kind,q=1,flag('FLAG_EC_CAUGHT_DIANCIE')
 if p in [246,250]:kind,q=1,flag('FLAG_DEFEATED_RAYQUAZA')
 if p in [247,248]:kind,q=6,1431
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(end),'--screenshot',f'{out}/origin-sky-{tag}-end.png']
 if 'checkpoint' in opts:c+=['--state-out',f'{out}/ice-basement.mgba']
 writes=[(59,'gEcHeadlessFixtureParam',p),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',kind),(70,'gEcHeadlessCampaignQueryId',q)]
 if 'clean' in opts:writes += [(59,'gEcHeadlessFixtureParam',p)]
 if again:writes += [(12000,'gEcHeadlessFixtureTrigger',1),(19500,'gEcHeadlessFixtureTrigger',1)]
 for f,n,v in writes:c+=['--write',f'{f}:4:{s[n]}:{v}']
 keys=[(300,2,'UP'),(500,2,'A')]
 if p==244:keys=[(300,16,'UP')]
 if p in [241,242]:
  points=[(8,22),(8,21),(8,20)]+[tuple(x) for x in ice[0]]
  if p==241:points += [(8,16),(8,15)]+[tuple(x) for x in ice[1]]+[(8,11),(8,10)]+[tuple(x) for x in ice[2]]+[(8,5),(8,4),(8,3)]
  else:points=[(8,22),(8,21),(8,20),(8,19),(7,19),(8,19)]
  ds={(0,-1):'UP',(0,1):'DOWN',(-1,0):'LEFT',(1,0):'RIGHT'}
  keys=[(300+i*110,16,ds[(b[0]-a[0],b[1]-a[1])]) for i,(a,b) in enumerate(zip(points,points[1:]))]
 if 'walk' in opts:keys += [(12000,16,'UP')]
 if 'recover' in opts:
  keys += [(f,2,'B') for f in range(1200,6500,220)]
  points=[(8,19),(8,20),(8,21),(8,22),(8,23),(9,23),(10,23),(11,23),(11,22)]
  ds={(0,-1):'UP',(0,1):'DOWN',(-1,0):'LEFT',(1,0):'RIGHT'}
  keys += [(8000+i*110,16,ds[(b[0]-a[0],b[1]-a[1])]) for i,(a,b) in enumerate(zip(points,points[1:]))]
  points=[(11,23),(10,23),(9,23),(8,23),(8,22),(8,21),(8,20)]+[tuple(x) for x in ice[0]]+[(8,16),(8,15)]+[tuple(x) for x in ice[1]]+[(8,11),(8,10)]+[tuple(x) for x in ice[2]]+[(8,5),(8,4),(8,3)]
  keys += [(9500+i*110,16,ds[(b[0]-a[0],b[1]-a[1])]) for i,(a,b) in enumerate(zip(points,points[1:]))]
 if again:keys += [(12500,2,'UP'),(12800,2,'A'),(20000,2,'UP'),(20300,2,'A')]
 if p not in [241,242,244]:keys += [(f,2,'B') for f in range(1000,end-200,220) if not again or not (12000<f<13000 or 19500<f<20500)]
 for f,d,k in sorted(keys):c+=['--key',f'{f}:{d}:{k}']
 for f in [750,1500,3000,5000,8000,11000]+([14000,18000,22000] if again else []):c+=['--screenshot-at',f'{f}:{out}/origin-sky-{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignCaptureSerial','gEcHeadlessCampaignLastCapturedSpecies','gEcHeadlessCampaignCaptureBookkeepingValid','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{s[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(probe,sys.argv[1:] or [*range(241,245),'245:again','246:again',247,248,'249:again','250:again']))
