from pathlib import Path
from collections import deque
import struct,re,json,sys,subprocess
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910')
# Plan movement from the map's collision data and its existing alternate layout.
raw=Path('data/layouts/MauvilleCity_Gym/map.bin').read_bytes();base=list(struct.unpack('<'+'H'*(len(raw)//2),raw));alt=base.copy()
labels={n:int(v,16) for n,v in re.findall(r'#define (METATILE_MauvilleGym_\w+)\s+(0x[\da-fA-F]+)',Path('include/constants/metatile_labels.h').read_text())}
s=Path('data/maps/MauvilleCity_Gym/scripts.inc').read_text().split('MauvilleCity_Gym_EventScript_SetAltBarriers::')[1].split('\n\tend')[0]
for x,y,t,c in re.findall(r'setmetatile (\d+), (\d+), (\w+), (TRUE|FALSE)',s):alt[int(y)*10+int(x)]=labels[t]|(0x400 if c=='TRUE' else 0)
objects=json.loads(Path('data/maps/MauvilleCity_Gym/map.json').read_text())['object_events'];blocked={(o['x'],o['y']) for o in objects}
switches={(0,15):1,(4,12):2,(3,9):3,(8,9):4}
start=(4,19,0,0,0);todo=deque([start]);prev={start:None};directions=[('UP',0,-1),('RIGHT',1,0),('DOWN',0,1),('LEFT',-1,0)];end=None
while todo:
 p=todo.popleft();x,y,state,last,visited=p
 if (x,y)==(5,3) and visited==15:end=p;break
 for key,dx,dy in directions:
  a,b=x+dx,y+dy
  if not(0<=a<10 and 0<=b<20) or (a,b) in blocked:continue
  if (alt if state else base)[b*10+a]&0xc00:continue
  sw=switches.get((a,b),last);state2=state^(sw!=last)
  q=(a,b,state2,sw,visited | (1 << (sw-1) if (a,b) in switches else 0))
  if q not in prev:prev[q]=(p,key);todo.append(q)
assert end,'no route'
route=[]
while prev[end]:
 p,key=prev[end];route.append((key,end));end=p
route.reverse();print('Planned route',route,flush=True)
(out/'gym-all-switches-route.json').write_text(json.dumps(route,indent=2)+'\n')
total_frames=400+len(route)*80+1800
sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(total_frames),'--screenshot',str(out/'gym-all-switches-end.png')]
for f,n,v in [(59,'gEcHeadlessFixtureParam',43),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',2),(70,'gEcHeadlessCampaignQueryId',0x4093)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
for i,(key,pos) in enumerate(route):
 f=400+i*80;cmd+=['--key',f'{f}:16:{key}']
 if i==0 or (pos[0],pos[1]) in switches or i==len(route)-1:cmd+=['--screenshot-at',f'{f+65}:{out}/gym-all-switches-step-{i}.png']
reload_frame=400+len(route)*80+120
cmd+=['--write',f'{reload_frame}:4:{sy["gEcHeadlessFixtureTrigger"]}:1','--screenshot-at',f'{reload_frame+500}:{out}/gym-all-switches-reloaded.png']
cmd+=['--key',f'{reload_frame+650}:16:LEFT']
for i in range(6):cmd+=['--key',f'{reload_frame+750+i*80}:16:DOWN']
for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:cmd+=['--read',f'4:{sy[n]}']
r=subprocess.run(cmd,capture_output=True,text=True);print(r.returncode,r.stdout,r.stderr,flush=True)
