from pathlib import Path
from collections import deque
import json,struct,sys,subprocess
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
frame=400;inputs=[];plans=[]
def load(n,position):
 global pos,w,h,cells,objects,los,warps
 pos=position;m=json.load(open(f'data/maps/{n}/map.json'));l=next(x for x in json.load(open('data/layouts/layouts.json'))['layouts'] if x['id']==m['layout']);w,h=l['width'],l['height'];cells=struct.unpack('<'+'H'*(w*h),Path(l['blockdata_filepath']).read_bytes());warps={(x['x'],x['y']) for x in m['warp_events']}
 objects={(o['x'],o['y']) for o in m['object_events'] if o['trainer_type']=='TRAINER_TYPE_NORMAL'};los={}
 for o in m['object_events']:
  if o['trainer_type']!='TRAINER_TYPE_NORMAL':continue
  dirs={'MOVEMENT_TYPE_FACE_LEFT':[(-1,0)],'MOVEMENT_TYPE_FACE_RIGHT':[(1,0)],'MOVEMENT_TYPE_FACE_DOWN':[(0,1)],'MOVEMENT_TYPE_FACE_DOWN_AND_UP':[(0,1),(0,-1)]}.get(o['movement_type'],[])
  los[o['x'],o['y']]={(o['x']+dx*k,o['y']+dy*k) for dx,dy in dirs for k in range(1,int(o['trainer_sight_or_berry_tree_id'])+1)}
def walk(target):
 global frame,pos
 blockedlos=set().union(*los.values());q=deque([pos]);prev={pos:None}
 while q:
  p=q.popleft()
  if p==target:break
  for key,dx,dy in [('UP',0,-1),('LEFT',-1,0),('RIGHT',1,0),('DOWN',0,1)]:
   a,b=p[0]+dx,p[1]+dy
   if not(0<=a<w and 0<=b<h) or cells[b*w+a]&0xc00 or (a,b) in objects or ((a,b) in (warps|blockedlos) and (a,b)!=target):continue
   if (a,b) not in prev:prev[a,b]=(p,key);q.append((a,b))
 assert target in prev,(pos,target)
 path=[];p=target
 while prev[p]:before,key=prev[p];path.append(key);p=before
 for key in reversed(path):inputs.extend(['--key',f'{frame}:16:{key}']);frame+=100
 plans.append({'from':pos,'to':target,'keys':list(reversed(path))});pos=target

def battle(target,facing,actor,name,moved=None,wait=6500):
 global frame
 walk(target);inputs.extend(['--key',f'{frame}:2:{facing}','--key',f'{frame+100}:2:A'])
 for f in range(frame+500,frame+wait,150):inputs.extend(['--key',f'{f}:2:B'])
 frame+=wait;inputs.extend(['--screenshot-at',f'{frame-10}:{out}/weather-path-{name}.png']);los.pop(actor,None)
 if moved:objects.remove(actor);objects.add(moved)
load('Route119_WeatherInstitute_1F',(5,11))
battle((10,7),'UP',(10,5),'grunt4',(10,6))
battle((14,3),'RIGHT',(15,3),'grunt1')
walk((17,1));frame+=1200
load('Route119_WeatherInstitute_2F',(17,2))
battle((19,5),'DOWN',(19,6),'grunt5')
battle((15,5),'DOWN',(15,6),'grunt2')
battle((10,7),'DOWN',(10,8),'grunt3')
battle((5,6),'LEFT',(4,6),'shelly',wait=12000)
if '--return' in sys.argv:
 objects={(4,6),(0,6),(1,7)};los={}
 walk((17,1));frame+=1200
 load('Route119_WeatherInstitute_1F',(17,2));objects={(0,5)};los={}
 walk((9,12));frame+=1500
 inputs.extend(['--key',f'{frame}:16:DOWN']);frame+=1000
 inputs.extend(['--screenshot-at',f'{frame-10}:{out}/weather-return-outside.png'])
 for key in ['DOWN']+['RIGHT']*4+['UP']+['RIGHT']*15+['UP']*2:
  inputs.extend(['--key',f'{frame}:16:{key}']);frame+=100
 for f in range(frame+300,frame+11000,150):inputs.extend(['--key',f'{f}:2:B'])
 frame+=11500
cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(frame+400),'--screenshot',str(out/('weather-return-end.png' if '--return' in sys.argv else 'weather-path-end.png'))]
for f,n,v in [(59,'gEcHeadlessFixtureParam',84),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',1),(70,'gEcHeadlessCampaignQueryId',(0x6e if '--return' in sys.argv else 0x97))]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
cmd+=inputs
for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY','gEcHeadlessCampaignBattleSerial']:cmd+=['--read',f'4:{sy[n]}']
(out/'weather-path-itinerary.json').write_text(json.dumps(plans,indent=2)+'\n');print('planned',sum(len(p['keys']) for p in plans),'steps',frame,'frames',flush=True)
r=subprocess.run(cmd,capture_output=True,text=True);print(r.returncode,r.stdout,r.stderr,flush=True)
