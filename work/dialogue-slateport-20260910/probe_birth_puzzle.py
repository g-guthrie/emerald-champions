import sys,struct
from collections import deque
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_southern import execute,sy,flag
rocks=[(15,12),(11,14),(15,8),(19,14),(12,11),(18,11),(15,14),(11,14),(19,14),(15,15),(15,10)]
dirs=[(0,-1,'UP'),(-1,0,'LEFT'),(1,0,'RIGHT'),(0,1,'DOWN')]
# Temporary native route: shortest cardinal path to each successive rock, never entering its occupied tile.
limits=[0,4,8,8,8,4,4,4,6,3,3]
def paths_to(pos,rock):
 q=deque([(pos,[])]);seen={pos};results=[]
 while q:
  p,path=q.popleft()
  if abs(p[0]-rock[0])+abs(p[1]-rock[1])==1:
   face=next(k for dx,dy,k in dirs if (p[0]+dx,p[1]+dy)==rock)
   results.append((p,path,face))
  for dx,dy,k in dirs:
   n=(p[0]+dx,p[1]+dy)
   if n!=rock and n not in seen and 9<=n[0]<=21 and 7<=n[1]<=16:
    seen.add(n);q.append((n,path+[k]))
 return results
def solve(i,pos):
 if i==len(rocks):return []
 for end,path,face in paths_to(pos,rocks[i]):
  if len(path)>limits[i]:continue
  tail=solve(i+1,end)
  if tail is not None:return [(end,path,face)]+tail
 return None
route=solve(0,(15,13));assert route is not None
p=b''.join(flag(n,False) for n in ['FLAG_BATTLED_DEOXYS','FLAG_DEFEATED_DEOXYS'])+flag('FLAG_HIDE_DEOXYS',True)
p+=bytes([0x39,26,58,255])+struct.pack('<HH',15,13)+bytes([0x27,0x6b,2])
a=sy['gStringVar4']+800;c=sy['sGlobalScriptContext'];w=[(50,1,a+i,v) for i,v in enumerate(p)]+[(50,1,c,0),(50,4,c+8,a),(50,1,c+1,1),(50,1,sy['sGlobalScriptContextStatus'],0)]
for n,v in [('gEcHeadlessFixtureParam',249 if '--faint' in sys.argv else 242),('gEcHeadlessCampaignBattleSerial',0),('gEcHeadlessCampaignCaptureSerial',0),('gEcHeadlessCampaignQueryKind',2),('gEcHeadlessCampaignQueryId',0x4035)]:w.append((50,4,sy[n],v))
keys=[];shots=[];pos=(15,13);t=700
for i,rock in enumerate(rocks):
 pos,path,face=route[i]
 print(i,rock,pos,len(path),path,face,flush=True)
 for k in path:keys.append((t,16,k));t+=55
 keys.append((t,2,face));keys.append((t+120,2,'A'));shots.append(t+400);t+=620
keys +=[(f,2,'B') for f in range(t+700,t+7000,250)]
execute('birth-puzzle-faint' if '--faint' in sys.argv else 'birth-puzzle-solved','postgame-audit-base',t+7400,keys,w,shots+[t+1500,t+4000])
