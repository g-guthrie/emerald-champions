import sys,json,struct,re,subprocess
from pathlib import Path
from collections import deque
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_southern import execute,sy,out
layouts={x['id']:x for x in json.loads(Path('data/layouts/layouts.json').read_text())['layouts']}
dirs=[(0,-1,'UP'),(-1,0,'LEFT'),(1,0,'RIGHT'),(0,1,'DOWN')]
def position(state):
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/(state+'.mgba')),'--frames','30']
 for n in ['gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
 r=subprocess.run(c,capture_output=True,text=True,check=True);return [int(x,16) for x in re.findall(r'value=([0-9a-f]+)',r.stdout)]
def path_in(name,start,target):
 d=json.loads(Path('data/maps',name,'map.json').read_text());l=layouts[d['layout']];raw=Path(l['blockdata_filepath']).read_bytes();b=struct.unpack('<'+'H'*(len(raw)//2),raw);width=l['width'];height=l['height'];block={(e['x'],e['y']) for e in d['warp_events']}|{(e['x'],e['y']) for e in d['object_events'] if e['flag']=='0'}
 q=deque([(start,[])]);seen={start}
 while q:
  p,path=q.popleft()
  if p==target:return path
  for dx,dy,k in dirs:
   n=(p[0]+dx,p[1]+dy)
   if n not in seen and 0<=n[0]<width and 0<=n[1]<height and (n==target or (n not in block and not b[n[1]*width+n[0]]&0xc00)):
    seen.add(n);q.append((n,path+[k]))
 raise RuntimeError((name,start,target))
def walk(name,state,tag,warp=None,target=None):
 mid,x,y=position(state);d=json.loads(Path('data/maps',name,'map.json').read_text());assert mid==sy[d['id']],(name,mid)
 if warp is not None:target=(d['warp_events'][warp]['x'],d['warp_events'][warp]['y'])
 path=path_in(name,(x,y),target)
 if warp is not None and not path:
  l=layouts[d['layout']];raw=Path(l['blockdata_filepath']).read_bytes();b=struct.unpack('<'+'H'*(len(raw)//2),raw)
  for dx,dy,key in dirs:
   nx,ny=x+dx,y+dy
   if 0<=nx<l['width'] and 0<=ny<l['height'] and not b[ny*l['width']+nx]&0xc00:
    back=next(k for xx,yy,k in dirs if xx==-dx and yy==-dy);path=[key,back];break
  assert path,(name,'cannot step off arrival stair')
 print('WALK',name,(x,y),'to',target,'steps',len(path),flush=True)
 keys=[(300+i*48,16,k) for i,k in enumerate(path)]
 execute(tag,state,1500+len(path)*48,keys,shots=[1000+len(path)*48])
 end=position(tag)
 if warp is not None:
  expected=sy[d['warp_events'][warp]['dest_map']]
  if end[0]==mid and tuple(end[1:])==target and path:
   execute(tag+'-cross',tag,1500,[(200,16,path[-1])]);tag+='-cross';end=position(tag)
  assert end[0]==expected,(name,'expected',expected,'got',end,'state',tag)
 else:assert tuple(end[1:])==target,(tag,end,target)
 return tag
