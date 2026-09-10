from pathlib import Path
import json,struct,heapq,subprocess,sys
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');layouts=json.loads(Path('data/layouts/layouts.json').read_text())['layouts'];sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
dirs=[('UP',0,-1),('RIGHT',1,0),('DOWN',0,1),('LEFT',-1,0)]
for puzzle in [1,2]:
 name=f'Route110_TrickHousePuzzle{puzzle}';d=json.loads((Path('data/maps')/name/'map.json').read_text());l=next(l for l in layouts if l['id']==d['layout']);w,h=l['width'],l['height'];raw=Path(l['blockdata_filepath']).read_bytes();grid=list(struct.unpack('<'+'H'*(len(raw)//2),raw))
 trees={(o['x'],o['y']) for o in d['object_events'] if o['script']=='EventScript_CutTree'}
 blocked={(o['x'],o['y']) for o in d['object_events'] if o['script']!='EventScript_CutTree'}|{(0,21),(1,21)}
 pos=(0,20);frame=400;commands=[];shots=[];actions=[]
 def key(k,frames=16):
  global frame
  commands.extend(['--key',f'{frame}:{frames}:{k}']);frame+=80
 def dialogue(label,yes=False):
  global frame
  key('A',2)
  shots.extend(['--screenshot-at',f'{frame+250}:{out}/trick-{puzzle}-{label}.png'])
  for off in range(150,1350,100):commands.extend(['--key',f'{frame+off}:2:'+('A' if yes else 'B')])
  frame+=1500
 def walk_to(goals):
  global pos
  todo=[(0,pos)];dist={pos:0};prev={pos:None};end=None
  while todo:
   cost,p=heapq.heappop(todo)
   if cost!=dist[p]:continue
   if p in goals:end=p;break
   for k,dx,dy in dirs:
    q=(p[0]+dx,p[1]+dy);x,y=q
    if not(0<=x<w and 0<=y<h) or q in blocked or grid[y*w+x]&0xc00:continue
    v=cost+(25 if q in trees else 1)
    if v<dist.get(q,10**9):dist[q]=v;prev[q]=(p,k);heapq.heappush(todo,(v,q))
  assert end is not None,(puzzle,pos,goals)
  path=[]
  while prev[end]:p,k=prev[end];path.append((k,end));end=p
  for k,q in reversed(path):
   if q in trees:
    key(k,2);dialogue(f'cut-{q[0]}-{q[1]}',True);trees.remove(q);actions.append(('cut',q,frame))
   key(k);pos=q
  actions.append(('arrive',pos,frame))
 def face_object(target):
  x,y=target;walk_to({(x-dx,y-dy) for k,dx,dy in dirs})
  k=next(k for k,dx,dy in dirs if (pos[0]+dx,pos[1]+dy)==target);key(k,2)
 if puzzle==2:
  for i,(button,hole) in enumerate([((11,12),(1,13)),((0,4),(5,6)),((14,5),(7,15)),((7,11),(14,12))],1):
   walk_to({button});grid[hole[1]*w+hole[0]]&=~0xc00
   shots+=['--screenshot-at',f'{frame+40}:{out}/trick-{puzzle}-button-{i}.png'];frame+=150
 scroll=(3,16) if puzzle==1 else (14,14)
 face_object(scroll);dialogue('scroll');walk_to({(13,2)});key('UP',2);dialogue('door');key('UP');frame+=600
 # Native warp enters the reward room below its one-way door at (10,2).
 d=json.loads(Path('data/maps/Route110_TrickHouseEnd/map.json').read_text());l=next(l for l in layouts if l['id']==d['layout']);w,h=l['width'],l['height'];raw=Path(l['blockdata_filepath']).read_bytes();grid=list(struct.unpack('<'+'H'*(len(raw)//2),raw));trees=set();blocked={(4,5),(10,1),(2,1),(2,2)};pos=(10,2)
 walk_to({(4,4)});key('DOWN',2);dialogue('reward');frame+=500
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(frame+500),'--screenshot',str(out/f'trick-{puzzle}-end.png')]
 for f,n,v in [(59,'gEcHeadlessFixtureParam',45+puzzle),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',2),(70,'gEcHeadlessCampaignQueryId',0x4044)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
 cmd+=commands+shots
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:cmd+=['--read',f'4:{sy[n]}']
 (out/f'trick-{puzzle}-itinerary.json').write_text(json.dumps({'actions':actions,'frames':frame+500,'cmd':cmd},indent=2)+'\n')
 print('puzzle',puzzle,'frames',frame+500,'actions',actions,flush=True)
 r=subprocess.run(cmd,capture_output=True,text=True);print(r.returncode,r.stdout,r.stderr,flush=True)
