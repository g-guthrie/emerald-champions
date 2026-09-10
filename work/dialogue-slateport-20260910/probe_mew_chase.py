import sys,struct,subprocess,re,hashlib
from pathlib import Path
from collections import deque
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_southern import execute,sy,flag,out
assert hashlib.sha256(Path('pokeemerald-mauville-audit-headless.gba').read_bytes()).hexdigest()=='76c81322758eba8bccd652666aefad08979c55fa388e87795b1e75ab2fbe8e75'
p=flag('FLAG_CAUGHT_MEW',False)+flag('FLAG_DEFEATED_MEW',False)+bytes([0x39,26,57,255])+struct.pack('<HH',13,18)+bytes([0x27,0x6b,2])
a=sy['gStringVar4']+800;c=sy['sGlobalScriptContext'];w=[(50,1,a+i,v) for i,v in enumerate(p)]+[(50,1,c,0),(50,4,c+8,a),(50,1,c+1,1),(50,1,sy['sGlobalScriptContextStatus'],0)]
for n,v in [('gEcHeadlessFixtureParam',242),('gEcHeadlessCampaignBattleSerial',0),('gEcHeadlessCampaignCaptureSerial',0),('gEcHeadlessCampaignQueryKind',3),('gEcHeadlessCampaignQueryId',1)]:w.append((50,4,sy[n],v))
execute('mew-chase-start','postgame-audit-base',2000,(),w,[400,650,850,1400])
blocks=struct.unpack('<754H',Path('data/layouts/FarawayIsland_Interior/map.bin').read_bytes())
dirs=[(0,-1,'UP'),(-1,0,'LEFT'),(1,0,'RIGHT'),(0,1,'DOWN')]
def toward(p,m):
 q=deque([(p,[])]);seen={p}
 while q:
  n,path=q.popleft()
  if abs(n[0]-m[0])+abs(n[1]-m[1])==1:return path
  for dx,dy,k in dirs:
   z=(n[0]+dx,n[1]+dy)
   if 0<=z[0]<29 and 0<=z[1]<26 and z not in seen and z!=m and not blocks[z[1]*29+z[0]]&0xc00:
    seen.add(z);q.append((z,path+[k]))
 raise RuntimeError((p,m))
state='mew-chase-start'
for i in range(100):
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/(state+'.mgba')),'--frames','30']
 names=['gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY','gEcHeadlessCampaignQueryObjectX','gEcHeadlessCampaignQueryObjectY','gEcHeadlessCampaignQueryValue']
 for n in names:cmd+=['--read',f'4:{sy[n]}']
 r=subprocess.run(cmd,capture_output=True,text=True,check=True);v=[int(x,16) for x in re.findall(r'value=([0-9a-f]+)',r.stdout)];p=tuple(v[:2]);m=tuple(v[2:4]);assert v[4],(i,p,m)
 path=toward(p,m);print('step',i,'player',p,'mew',m,'path',path,flush=True)
 if not path:
  (out/'mew-adjacent-state.txt').write_text(state+'\n');break
 tag=f'mew-chase-{i:02d}';cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/(state+'.mgba')),'--frames','100','--key',f'20:16:{path[0]}','--state-out',str(out/(tag+'.mgba'))]
 if i%4==0:cmd+=['--screenshot',str(out/(tag+'.png'))]
 subprocess.run(cmd,capture_output=True,text=True,check=True);state=tag
else:raise RuntimeError('No adjacent encounter after 100 native steps')
face=next(k for dx,dy,k in dirs if (p[0]+dx,p[1]+dy)==m)
(out/'mew-adjacent-facing.txt').write_text(face+'\n')
print('Adjacent state',state,'face',face,flush=True)
