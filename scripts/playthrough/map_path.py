from pathlib import Path
import json,struct,collections,sys,itertools
mname=sys.argv[1];start=tuple(map(int,sys.argv[2:4]));end=tuple(map(int,sys.argv[4:6]));m=json.load(open('data/maps/'+mname+'/map.json'));l=next(a for a in json.load(open('data/layouts/layouts.json'))['layouts'] if a['id']==m['layout']);p=Path(l['blockdata_filepath']);d=struct.unpack('<'+'H'*(p.stat().st_size//2),p.read_bytes());w=l['width'];h=l['height'];block={(o['x'],o['y']) for o in m['object_events']};block|={(o['x'],o['y']) for o in m['bg_events'] if o['type']=='sign'};block.discard(end)
q=collections.deque([start]);seen={start:None}
while q:
 a=q.popleft()
 if a==end:break
 ae=d[a[1]*w+a[0]]>>12
 for dx,dy,di in [(1,0,'RIGHT'),(-1,0,'LEFT'),(0,1,'DOWN'),(0,-1,'UP')]:
  b=(a[0]+dx,a[1]+dy)
  if not(0<=b[0]<w and 0<=b[1]<h)or b in seen or b in block or (d[b[1]*w+b[0]]&0xc00 and b!=end):continue
  be=d[b[1]*w+b[0]]>>12
  if ae and be and ae!=be:continue
  seen[b]=(a,di);q.append(b)
if end not in seen:raise SystemExit('No collision/elevation path; inspect stairs or dynamic obstacles.')
a=end;steps=[]
while seen[a]:a,di=seen[a];steps.append(di)
steps.reverse();chunks=[(k,len(list(g))) for k,g in itertools.groupby(steps)];print(chunks);f=1;keys=[]
for k,n in chunks:keys+=['--key',f'{f}:{16*n}:{k}'];f+=16*n+40
print(' '.join(keys),'--frames',f+450)
