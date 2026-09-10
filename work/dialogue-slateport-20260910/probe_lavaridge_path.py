from pathlib import Path
from collections import deque
import json,struct,sys,subprocess
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');names=['LavaridgeTown_Gym_1F','LavaridgeTown_Gym_B1F']
def words(path):
 raw=Path(path).read_bytes();return struct.unpack('<'+'H'*(len(raw)//2),raw)
attrs=words('data/tilesets/secondary/lavaridge_gym/metatile_attributes.bin')
maps=[json.loads(Path('data/maps/'+n+'/map.json').read_text()) for n in names]
cells=[words('data/layouts/'+n+'/map.bin') for n in names]
blocked=[{(o['x'],o['y']) for o in m['object_events']} for m in maps]
warps=[{(w['x'],w['y']):w for w in m['warp_events']} for m in maps]
start=(0,13,17);goal=(0,13,10);todo=deque([start]);prev={start:None}
while todo:
 p=todo.popleft();f,x,y=p
 if p==goal:break
 for key,dx,dy in [('UP',0,-1),('LEFT',-1,0),('RIGHT',1,0),('DOWN',0,1)]:
  a,b=x+dx,y+dy
  if not(0<=a<17 and 0<=b<19) or (a,b) in blocked[f]:continue
  c=cells[f][b*17+a]
  if c&0xc00:continue
  t=c&1023;behavior=attrs[t-512]&255 if t>=512 else 0
  w=warps[f].get((a,b));q=(f,a,b);warp=False
  if w and w['dest_map']=='MAP_LAVARIDGE_TOWN':continue
  if w and behavior in [0x29,0x68]:
   dest=1-f;dw=maps[dest]['warp_events'][int(w['dest_warp_id'])]
   q=(dest,dw['x']+(1 if f==1 else 0),dw['y']);warp=True
   if (q[1],q[2]) in blocked[dest]:continue
  if q not in prev:prev[q]=(p,key,warp);todo.append(q)
assert goal in prev,'No source route to Flannery'
route=[];p=goal
while prev[p]:
 before,key,warp=prev[p];route.append({'key':key,'from':before,'to':p,'warp':warp});p=before
route.reverse();(out/'lavaridge-planned-route.json').write_text(json.dumps(route,indent=2)+'\n');print('steps',len(route),'warps',sum(r['warp'] for r in route),flush=True)
if '--roundtrip' in sys.argv:
 original=route.copy()
 start=(0,13,10);goal=(0,13,17);todo=deque([start]);prev={start:None}
 while todo:
  p=todo.popleft();f,x,y=p
  if p==goal:break
  for key,dx,dy in [('UP',0,-1),('LEFT',-1,0),('RIGHT',1,0),('DOWN',0,1)]:
   a,b=x+dx,y+dy
   if not(0<=a<17 and 0<=b<19) or (a,b) in blocked[f]:continue
   c=cells[f][b*17+a]
   if c&0xc00:continue
   t=c&1023;behavior=attrs[t-512]&255 if t>=512 else 0
   w=warps[f].get((a,b));q=(f,a,b);warp=False
   if w and w['dest_map']=='MAP_LAVARIDGE_TOWN':continue
   if w and behavior in [0x29,0x68]:
    dest=1-f;dw=maps[dest]['warp_events'][int(w['dest_warp_id'])]
    q=(dest,dw['x']+(1 if f==1 else 0),dw['y']);warp=True
    if (q[1],q[2]) in blocked[dest]:continue
   if q not in prev:prev[q]=(p,key,warp);todo.append(q)
 assert goal in prev,'No source route to Flannery'
 route=[];p=goal
 while prev[p]:
  before,key,warp=prev[p];route.append({'key':key,'from':before,'to':p,'warp':warp});p=before
 route.reverse();(out/'lavaridge-return-route.json').write_text(json.dumps(route,indent=2)+'\n');print('steps',len(route),'warps',sum(r['warp'] for r in route),flush=True)
 route=original+route+[{'key':'DOWN','warp':False},{'key':'DOWN','warp':True}]
sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
frame=400;inputs=[]
for i,r in enumerate(route):
 inputs+=['--key',f'{frame}:16:{r["key"]}'];frame+=400 if r['warp'] else 80
 if r['warp'] or i==len(route)-1:inputs+=['--screenshot-at',f'{frame-10}:{out}/lavaridge-path-{i}.png']
cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(frame+300),'--screenshot',str(out/('lavaridge-roundtrip-end.png' if '--roundtrip' in sys.argv else 'lavaridge-path-end.png'))]
for fr,n,v in [(59,'gEcHeadlessFixtureParam',71),(60,'gEcHeadlessFixtureScenario',71)]:cmd+=['--write',f'{fr}:4:{sy[n]}:{v}']
cmd+=inputs
for n in ['gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:cmd+=['--read',f'4:{sy[n]}']
r=subprocess.run(cmd,capture_output=True,text=True);print(r.returncode,r.stdout,r.stderr,flush=True)
