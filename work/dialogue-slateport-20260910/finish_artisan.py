import sys,json,struct
from collections import deque
from pathlib import Path
sys.path.insert(0,'work/dialogue-slateport-20260910')
import walk_island_maps as w
import probe_artisan_pickups as p

def cave_path(name,start,target):
 d=json.loads(Path('data/maps',name,'map.json').read_text());l=w.layouts[d['layout']];W=l['width'];H=l['height'];b=struct.unpack('<'+'H'*(W*H),Path(l['blockdata_filepath']).read_bytes())
 def tile(pos):return b[pos[1]*W+pos[0]]
 if tile(target)&0xc00:raise RuntimeError('blocked target')
 blocked={(o['x'],o['y']) for o in d['object_events']}|{(o['x'],o['y']) for o in d['warp_events']}
 q=deque([(start,tile(start)>>12,[])]);seen=set()
 while q:
  pos,e,path=q.popleft()
  if pos==target:return path
  for dx,dy,key in w.dirs:
   n=(pos[0]+dx,pos[1]+dy)
   if not(0<=n[0]<W and 0<=n[1]<H) or (n in blocked and n!=target):continue
   t=tile(n);ne=t>>12
   if t&0xc00 or (tile(pos)>>12!=0 and ne not in (0,15,e)):continue
   next_e=e if ne in (0,15) else ne
   # Leaving a zero-elevation stair adopts the destination level.
   if tile(pos)>>12==0:next_e=ne
   marker=(n,next_e)
   if marker not in seen:seen.add(marker);q.append((n,next_e,path+[key]))
 raise RuntimeError((name,start,target,'no elevation path'))
# Zero-elevation stairs allow exiting onto either level; handled in the edge condition below.
source=cave_path
w.path_in=cave_path;p.path_in=cave_path
if __name__=='__main__':
 s=p.pickup('ArtisanCave_B1F','artisan-big-pearl-approach','ITEM_BIG_PEARL',(7,5),'artisan-big-pearl-final')
 s=p.pickup('ArtisanCave_B1F',s,'ITEM_STAR_PIECE',(27,8),'artisan-star-piece-final')
 s=w.walk('ArtisanCave_B1F',s,'artisan-upstairs-final',warp=1)
 s=p.pickup('ArtisanCave_1F',s,'ITEM_BIG_NUGGET',(14,5),'artisan-big-nugget-final')
 s=w.walk('ArtisanCave_1F',s,'artisan-east-exit-final',warp=0)
 print('FINAL',s,w.position(s))
