from pathlib import Path
from collections import deque
import struct,re,json,sys,subprocess
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
s=Path('data/maps/NewMauville_Inside/scripts.inc').read_text();raw=Path('data/layouts/NewMauville_Inside/map.bin').read_bytes();base=list(struct.unpack('<'+'H'*(len(raw)//2),raw));layouts=[base]
labels={k:int(v,16) for k,v in re.findall(r'#define (METATILE_BikeShop_\w+)\s+(0x[0-9A-Fa-f]+)',Path('include/constants/metatile_labels.h').read_text())}
for color in ['Blue','Green']:
 alt=base.copy();block=s.split('NewMauville_Inside_EventScript_SetBarrierState'+color+'Button::')[1].split('\n\treturn')[0]
 for x,y,t,c in re.findall(r'setmetatile (\d+), (\d+), (\w+), (TRUE|FALSE)',block):alt[int(y)*41+int(x)]=labels[t]|(0x400 if c=='TRUE' else 0)
 layouts.append(alt)
m=json.loads(Path('data/maps/NewMauville_Inside/map.json').read_text());cleared={(16,22),(17,10),(2,11),(25,18),(6,11),(13,10)};blocked={(o['x'],o['y']) for o in m['object_events']}-cleared
buttons={(e['x'],e['y']):(1 if 'BlueButton' in e['script'] else 2) for e in m['coord_events'] if e['var'].startswith('VAR_TEMP')}
start=(32,34,0);todo=deque([start]);prev={start:None};goal=None
while todo:
 p=todo.popleft();x,y,st=p
 if (x,y)==(32,6):goal=p;break
 for key,dx,dy in [('UP',0,-1),('LEFT',-1,0),('RIGHT',1,0),('DOWN',0,1)]:
  a,b=x+dx,y+dy
  if not(0<=a<41 and 0<=b<41) or (a,b) in blocked or (a,b)==(33,6) or layouts[st][b*41+a]&0xc00:continue
  q=(a,b,buttons.get((a,b),st))
  if q not in prev:prev[q]=(p,key);todo.append(q)
assert goal,'No source route to generator'
route=[];p=goal
while prev[p]:before,key=prev[p];route.append((key,p));p=before
route.reverse();(out/'new-mauville-route.json').write_text(json.dumps(route,indent=2)+'\n');print('planned steps',len(route),flush=True)
var=int(re.search(r'#define VAR_NEW_MAUVILLE_STATE\s+(0x[0-9A-Fa-f]+)',Path('include/constants/vars.h').read_text())[1],0)
for param in map(int,sys.argv[1:] or ['77']):
 frame=400;inputs=[]
 if param==77:
  inputs+=['--key','400:16:UP']
  for f in range(800,2400,100):inputs+=['--key',f'{f}:2:A']
  inputs+=['--key','2800:16:UP','--screenshot-at',f'3400:{out}/new-mauville-door.png'];frame=3600
  for i,(key,p) in enumerate(route):
   inputs+=['--key',f'{frame}:16:{key}'];frame+=100
   if (p[0],p[1]) in buttons or i==len(route)-1:inputs+=['--screenshot-at',f'{frame-10}:{out}/new-mauville-step-{i}.png']
  inputs+=['--screenshot-at',f'{frame+200}:{out}/new-mauville-generator-arrival.png'];frame+=400
 for stage,(key,delay) in enumerate([('RIGHT',1500),('LEFT',1000),('RIGHT',1500),('LEFT',1000),('RIGHT',8500)]):
  inputs+=['--key',f'{frame}:16:{key}','--screenshot-at',f'{frame+700}:{out}/new-mauville-{param}-stage-{stage}.png']
  for f in range(frame+300,frame+delay,100):inputs+=['--key',f'{f}:2:B']
  frame+=delay
 inputs+=['--write',f'{frame+500}:4:{sy["gEcHeadlessFixtureTrigger"]}:1','--key',f'{frame+1100}:2:UP','--key',f'{frame+1200}:2:A']
 for f in range(frame+1500,frame+5000,100):inputs+=['--key',f'{f}:2:B']
 inputs+=['--write',f'{frame+5100}:4:{sy["gEcHeadlessCampaignQueryKind"]}:1','--write',f'{frame+5100}:4:{sy["gEcHeadlessCampaignQueryId"]}:209']
 frame+=5400
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(frame+300),'--screenshot',str(out/f'new-mauville-return-{param}-end.png')]
 for f,n,v in [(59,'gEcHeadlessFixtureParam',param),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',2),(70,'gEcHeadlessCampaignQueryId',var)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
 cmd+=inputs
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignLastResolution']:cmd+=['--read',f'4:{sy[n]}']
 r=subprocess.run(cmd,capture_output=True,text=True);print(param,r.returncode,r.stdout,r.stderr,flush=True)
