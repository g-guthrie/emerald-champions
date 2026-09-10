from pathlib import Path
from collections import deque
import json,struct,re,sys,subprocess
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');src=Path('src/rotating_gate.c').read_text();m=json.load(open('data/maps/FortreeCity_Gym/map.json'));l=next(x for x in json.load(open('data/layouts/layouts.json'))['layouts'] if x['id']==m['layout']);w,h=l['width'],l['height'];cells=struct.unpack('<'+'H'*(w*h),Path(l['blockdata_filepath']).read_bytes());objects={(o['x'],o['y']) for o in m['object_events']};warps={(o['x'],o['y']) for o in m['warp_events']}
def block(name):return re.search(re.escape(name)+r'[^=]*=\s*\{(.*?)\n\};',src,re.S)[1]
shapes=['L1','L2','L3','L4','T1','T2','T3','T4','UNUSED_T1','UNUSED_T2','UNUSED_T3','UNUSED_T4'];arms=['NORTH','EAST','SOUTH','WEST']
config=[(int(x),int(y),shapes.index(shape),int(angle)//90) for x,y,shape,angle in re.findall(r'\{\s*(\d+),\s*(\d+), GATE_SHAPE_(\w+), GATE_ORIENTATION_(\d+)\}',block('sRotatingGate_FortreePuzzleConfig'))]
layouts=[tuple(map(int,re.findall(r'\d+',x))) for x in re.findall(r'\{([^{}]+)\}',block('sRotatingGate_ArmLayout'))]
positions={delta:[tuple(map(int,xy)) for xy in re.findall(r'\{\s*(-?\d+),\s*(-?\d+)\s*\}',block(name))] for delta,name in [(1,'sRotatingGate_ArmPositionsClockwiseRotation'),(-1,'sRotatingGate_ArmPositionsAntiClockwiseRotation')]}
rot={}
for key,name in [('UP','North'),('DOWN','South'),('LEFT','West'),('RIGHT','East')]:
 rot[key]=[]
 for token in re.findall(r'GATE_ROT_NONE|GATE_ROT_(?:ACW|CW)\(GATE_ARM_\w+, \d\)',block('sRotatingGate_RotationInfo'+name)):
  if token=='GATE_ROT_NONE':rot[key].append(None)
  else:
   a=re.match(r'GATE_ROT_(ACW|CW)\(GATE_ARM_(\w+), (\d)\)',token);rot[key].append((-1 if a[1]=='ACW' else 1,arms.index(a[2]),int(a[3])))
 assert len(rot[key])==16

def collision(x,y):return not(0<=x<w and 0<=y<h) or cells[y*w+x]&0xc00

def advance(state,key,dx,dy):
 x,y,angles=state;x+=dx;y+=dy
 if collision(x,y) or (x,y) in objects or (x,y) in warps:return None
 for idx,(gx,gy,shape,_) in enumerate(config):
  if gx-2<=x<=gx+1 and gy-2<=y<=gy+1:
   info=rot[key][(y-gy+2)*4+(x-gx+2)]
   if info:
    delta,arm,long=info;ori=angles[idx]
    if layouts[shape][((arm-ori+4)%4)*2+long]:
     for i in range(4):
      for j in range(2):
       if layouts[shape][2*i+j]:
        ax,ay=positions[delta][2*((ori+i)%4)+j]
        if collision(gx+ax,gy+ay):return None
     angles=angles[:idx]+((ori+delta)%4,)+angles[idx+1:]
     break
 return x,y,angles

def plan(start,target):
 q=deque([start]);prev={start:None};end=None
 while q:
  p=q.popleft()
  if p[:2]==target:end=p;break
  for key,dx,dy in [('UP',0,-1),('LEFT',-1,0),('RIGHT',1,0),('DOWN',0,1)]:
   nxt=advance(p,key,dx,dy)
   if nxt is not None and nxt not in prev:prev[nxt]=(p,key);q.append(nxt)
 assert end,(start,target,len(prev))
 path=[];p=end
 while prev[p]:before,key=prev[p];path.append(key);p=before
 return list(reversed(path)),end

start=(16,23,tuple(c[3] for c in config));path,end=plan(start,(15,3));back,returned=plan(end,(16,23));print('path',len(path),'return',len(back),'orientations',end[2],flush=True)
(out/'fortree-gates-itinerary.json').write_text(json.dumps({'outbound':path,'orientations':end[2],'return':back},indent=2)+'\n')
sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True);frame=400;inputs=[]
for i,key in enumerate(path):
 inputs+=['--key',f'{frame}:16:{key}'];frame+=100
 if i%15==0:inputs+=['--screenshot-at',f'{frame-10}:{out}/fortree-gates-step-{i}.png']
inputs+=['--screenshot-at',f'{frame+100}:{out}/fortree-gates-arrival.png','--key',f'{frame+200}:2:UP','--key',f'{frame+300}:2:A']
for f in range(frame+800,frame+11000,150):inputs+=['--key',f'{f}:2:B']
frame+=11500
if '--return' in sys.argv:
 for key in back:inputs+=['--key',f'{frame}:16:{key}'];frame+=100
 inputs+=['--key',f'{frame}:16:DOWN','--key',f'{frame+400}:16:DOWN'];frame+=1700
cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(frame+400),'--screenshot',str(out/('fortree-gates-return.png' if '--return' in sys.argv else 'fortree-gates-end.png'))]
for f,n,v in [(59,'gEcHeadlessFixtureParam',97),(60,'gEcHeadlessFixtureScenario',71)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
cmd+=inputs
for n in ['gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY','gEcHeadlessCampaignBattleSerial']:cmd+=['--read',f'4:{sy[n]}']
r=subprocess.run(cmd,capture_output=True,text=True);print(r.returncode,r.stdout,r.stderr,flush=True)
