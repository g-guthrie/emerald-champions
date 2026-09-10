from pathlib import Path
from collections import deque
import json,struct,re,sys,subprocess
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');m=json.loads(Path('data/maps/PetalburgCity_Gym/map.json').read_text());raw=Path('data/layouts/PetalburgCity_Gym/map.bin').read_bytes();cells=list(struct.unpack('<'+'H'*(len(raw)//2),raw))
for x,y in re.findall(r'setmetatile (\d+), (\d+), \w+, FALSE',Path('data/maps/PetalburgCity_Gym/scripts.inc').read_text()):cells[int(y)*9+int(x)]&=~0xc00
objects={(o['x'],o['y']) for o in m['object_events'] if 'WALLY' not in o.get('local_id','')};warps={(w['x'],w['y']) for w in m['warp_events']}
sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
flags=Path('include/constants/flags.h').read_text();query=int(re.search(r'#define FLAG_RECEIVED_HM_SURF\s+(0x[0-9A-Fa-f]+)',flags)[1],0)
for branch in sys.argv[1:] or ['left','right']:
 frame=400;pos=(4,110);inputs=[];steps=[]
 def walk(target):
  global frame,pos
  q=deque([pos]);prev={pos:None}
  while q:
   p=q.popleft()
   if p==target:break
   for key,dx,dy in [('UP',0,-1),('LEFT',-1,0),('RIGHT',1,0),('DOWN',0,1)]:
    a,b=p[0]+dx,p[1]+dy
    if not(0<=a<9 and 0<=b<112) or (a,b) in objects or ((a,b) in warps and (a,b)!=target) or cells[b*9+a]&0xc00:continue
    if (a,b) not in prev:prev[a,b]=(p,key);q.append((a,b))
  assert target in prev,(pos,target)
  route=[];p=target
  while prev[p]:before,key=prev[p];route.append(key);p=before
  for key in reversed(route):inputs.extend(['--key',f'{frame}:16:{key}']);frame+=80
  steps.append({'from':pos,'to':target,'keys':list(reversed(route))});pos=target
 def door(stand,dest):
  global frame,pos
  walk(stand);inputs.extend(['--key',f'{frame}:2:UP']);frame+=80
  for f in range(frame,frame+701,100):inputs.extend(['--key',f'{f}:2:A'])
  frame+=1200;pos=dest;inputs.extend(['--screenshot-at',f'{frame-10}:{out}/norman-{branch}-door-{len(steps)}.png'])
 def battle(stand,facing,label,wait=5000):
  global frame
  walk(stand);inputs.extend(['--key',f'{frame}:2:{facing}','--key',f'{frame+80}:2:A'])
  for f in range(frame+500,frame+wait,100):inputs.extend(['--key',f'{f}:2:B'])
  inputs.extend(['--screenshot-at',f'{frame+750}:{out}/norman-{branch}-{label}-intro.png','--screenshot-at',f'{frame+wait-10}:{out}/norman-{branch}-{label}-after.png']);frame+=wait
 if branch=='left':
  door((1,106),(7,85));battle((3,81),'RIGHT','randall')
  door((1,80),(7,46));battle((3,42),'RIGHT','parker')
  door((7,41),(1,20));battle((3,16),'RIGHT','jody')
  door((7,15),(1,7))
 elif branch=='middle':
  door((1,106),(7,85));battle((3,81),'RIGHT','randall')
  door((7,80),(1,59));battle((3,55),'RIGHT','alexia')
  door((7,54),(1,33));battle((3,29),'RIGHT','berke')
  door((1,28),(7,7))
 else:
  door((7,106),(1,98));battle((3,94),'RIGHT','mary')
  door((7,93),(1,72));battle((3,68),'RIGHT','george')
  door((1,67),(7,33));battle((3,29),'RIGHT','berke')
  door((1,28),(7,7))
 battle((4,3),'UP','norman',11000)
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(frame+300),'--screenshot',str(out/f'norman-{branch}-end.png')]
 for f,n,v in [(59,'gEcHeadlessFixtureParam',76),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',1),(70,'gEcHeadlessCampaignQueryId',query)]:cmd.extend(['--write',f'{f}:4:{sy[n]}:{v}'])
 cmd+=inputs
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY','gEcHeadlessCampaignBattleSerial']:cmd.extend(['--read',f'4:{sy[n]}'])
 (out/f'norman-{branch}-itinerary.json').write_text(json.dumps(steps,indent=2)+'\n')
 r=subprocess.run(cmd,capture_output=True,text=True);print(branch,r.returncode,r.stdout,r.stderr,flush=True)
