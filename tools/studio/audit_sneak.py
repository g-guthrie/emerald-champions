#!/usr/bin/env python3
"""Frame-by-frame native walking/sneaking comparison outside any DexNav search."""
import asyncio,json,struct,subprocess,sys
from pathlib import Path
from PIL import Image,ImageDraw
sys.path.insert(0,str(Path('tools/studio').resolve()))
sys.path.insert(0,str(Path('scripts').resolve()))
import server,build_provenance as bp
from scenes import keys,packet_state
from native_tools import find_nm
OUT=Path(sys.argv[1]).resolve();OUT.mkdir(parents=True,exist_ok=True)
server.WORK=OUT;server.BUILD_STORE=OUT/'builds'
async def main():
 s=server.Studio(0);s.build=await s.stage();s.build_id=s.build['id'];s.core,p=await s.boot(s.build,chapter=3);s.ingest(p)
 nm=find_nm(server.ROOT);objdump=str(Path(nm).with_name('arm-none-eabi-objdump'))
 dies=bp._parse_dwarf(subprocess.check_output([objdump,'--dwarf=info','build/emerald-headless/save-layout/save_layout_probe.o'],text=True))
 def offset(structure,field):
  die=next(d for d in dies.values() if d['tag']=='DW_TAG_structure_type' and bp._name(d['attrs'].get('DW_AT_name'))==structure)
  return next(bp._number(dies[c]['attrs'].get('DW_AT_data_member_location')) for c in die['children'] if bp._name(dies[c]['attrs'].get('DW_AT_name'))==field)
 stats=offset('SaveBlock1','gameStats');cipher=offset('SaveBlock2','encryptionKey')
 async def u32(address):return struct.unpack('<I',await s.core.rpc(4,server.words(address,4)))[0]
 async def steps():
  one=await u32(s.core.syms['gSaveBlock1Ptr']);two=await u32(s.core.syms['gSaveBlock2Ptr'])
  return (await u32(one+stats+5*4))^(await u32(two+cipher))
 async def tick(n,k=0):
  while n:
   part=min(n,120);s.ingest(await s.core.tick(keys=k,frames=part));n-=part
 def actor():return next(a for a in packet_state(s.packet)['actors'] if a['local_id']==255)
 results={};movies={};passed=False
 try:
  await s.command(dict(op='setup',flags={'FLAG_EC_RIVAL_DEXNAV_TUTORIAL_COMPLETE':True,'FLAG_RECEIVED_DEXNAV':True,'FLAG_SYS_B_DASH':True},vars={'VAR_LITTLEROOT_RIVAL_STATE':4,'VAR_ROUTE101_STATE':3}))
  for name,buttons in [('walk','UP'),('sneak','A UP'),('both_walk','A B UP'),('run','B UP')]:
   await s.command(dict(op='warp',map='Route101',x=10,y=19,facing=2));await tick(150)
   before=await steps();trace=[];frames=[];tile_frames=[];previous=19
   for f in range(240):
    await tick(1,keys(buttons));a=actor();trace.append(dict(frame=f,y=a['y'],moving=a['moving']))
    if f%2==0:frames.append(Image.frombytes('RGBA',(240,160),s.packet[16:153616]).convert('RGB'))
    if a['y']!=previous:
     tile_frames.append(f);previous=a['y']
    if len(tile_frames)==4:break
   await tick(45)
   assert actor()['y']==15 and await steps()-before==4, (name,actor(),await steps()-before)
   intervals=[b-a for a,b in zip(tile_frames,tile_frames[1:])]
   pauses=[];idle=0
   for row in trace:
    if not row['moving']:idle+=1
    elif idle:pauses.append(idle);idle=0
   results[name]=dict(tile_frames=tile_frames,intervals=intervals,idle_runs=pauses,steps=4,trace=trace)
   movies[name]=frames
   Image.frombytes('RGBA',(240,160),s.packet[16:153616]).save(OUT/f'{name}-returned.png')
  assert results['walk']['intervals']==[16]*3,results['walk']
  # Native slow movement uses 31 frames between starts; the eight idle frames
  # are additional, and must never be counted as extra game-stat steps.
  assert results['sneak']['intervals']==[39]*3,results['sneak']
  assert results['both_walk']['intervals']==results['walk']['intervals'],results['both_walk']
  assert results['run']['intervals']==[8]*3,results['run']
  assert results['sneak']['idle_runs'].count(8)==3,results['sneak']
  # Releasing A during the planted-foot pause must start ordinary walking immediately.
  await s.command(dict(op='warp',map='Route101',x=10,y=19,facing=2));await tick(150)
  for f in range(80):
   await tick(1,keys('A UP'));a=actor()
   if a['y']==18 and not a['moving']:break
  else:raise AssertionError('No sneak pause observed')
  await tick(1,keys('UP'));assert actor()['y']==17,'A release left a movement delay'
  await tick(40)
  # Start/B remain usable at a paused tile, rather than waiting for another step.
  await tick(1,keys('START'));await tick(60)
  Image.frombytes('RGBA',(240,160),s.packet[16:153616]).save(OUT/'menu-open.png')
  await tick(1,keys('B'));await tick(60);assert s.state[0]
  Image.frombytes('RGBA',(240,160),s.packet[16:153616]).save(OUT/'menu-cancel.png')
  paired=[]
  for i in range(max(map(len,movies.values()))):
   im=Image.new('RGB',(480,184),'#12261f');draw=ImageDraw.Draw(im)
   for x,name in [(0,'walk'),(240,'sneak')]:
    frames=movies[name];im.paste(frames[min(i,len(frames)-1)],(x,24));draw.text((x+5,6),'Normal walk' if name=='walk' else 'Hold A: slow step + pause',fill='white')
   paired.append(im)
  paired[0].save(OUT/'walk-vs-sneak.webp',save_all=True,append_images=paired[1:],duration=33,loop=0,lossless=True)
  passed=True
 finally:
  (OUT/'evidence.json').write_text(json.dumps(dict(passed=passed,synthetic=True,build=s.build_info(),results=results),indent=2));await s.core.close()
 print(OUT,passed)
asyncio.run(main())
