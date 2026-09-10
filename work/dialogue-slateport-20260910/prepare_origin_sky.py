from pathlib import Path
import json,struct
root=Path.cwd();ls=json.loads((root/'data/layouts/layouts.json').read_text())['layouts']
# Extra sign placement was removed at the user's request.
# Solve the three authored ice regions once, solely to drive ordinary input.
l=next(l for l in ls if l['id']=='LAYOUT_SOOTOPOLIS_CITY_GYM_1F');a=struct.unpack('<'+'H'*(l['width']*l['height']),(root/l['blockdata_filepath']).read_bytes());paths=[]
for lo,hi,start,end in [(17,19,(8,19),(8,17)),(12,14,(8,14),(8,12)),(6,9,(8,9),(8,6))]:
 cells={(x,y) for y in range(lo,hi+1) for x in range(l['width']) if a[y*l['width']+x]&0x3ff in [0x20d,0x215]}
 adj={c:[d for d in cells if abs(c[0]-d[0])+abs(c[1]-d[1])==1] for c in cells}
 def solve(path,remain):
  c=path[-1]
  if not remain:return path if c==end else None
  if c==end:return None
  seen=set();todo=[c]
  while todo:
   d=todo.pop()
   for e in adj[d]:
    if e in remain and e not in seen:seen.add(e);todo.append(e)
  if len(seen)!=len(remain):return None
  for d in sorted([d for d in adj[c] if d in remain],key=lambda d:sum(e in remain for e in adj[d])):
   ans=solve(path+[d],remain-{d})
   if ans:return ans
  return None
 answer=solve([start],cells-{start});assert answer;paths.append(answer);print('puzzle',len(cells),answer)
(root/'work/dialogue-slateport-20260910/sootopolis-ice-paths.json').write_text(json.dumps(paths)+'\n')
