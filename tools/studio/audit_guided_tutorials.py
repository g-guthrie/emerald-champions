#!/usr/bin/env python3
"""Native scripted-demo audit. Run with MODE OUTPUT (nurse/rival/wally).

Uses synthetic prerequisites, real menu and battle execution, and no victory
shortcut. Captures meaningful visual changes plus party and phase transitions.
"""
import asyncio, json, sys, subprocess
from pathlib import Path
from PIL import Image, ImageChops
sys.path.insert(0,str(Path('tools/studio').resolve()))
import server
from native_tools import find_nm, read_game_query
from scenes import keys, packet_state, TextDecoder
MODE=sys.argv[1]; VARIANT=sys.argv[3] if len(sys.argv)>3 else 'normal'; OUT=Path(sys.argv[2]).resolve();OUT.mkdir(parents=True,exist_ok=True)
server.WORK=OUT;server.BUILD_STORE=OUT/'builds'

async def main():
 s=server.Studio(0);s.build=await s.stage();s.build_id=s.build['id']
 s.core,packet=await s.boot(s.build,chapter=4 if MODE=='wally' else 3);s.ingest(packet)
 decoder=TextDecoder(server.ROOT);shots=[];inputs=[];last=None;last_state=None;passed=False
 nm=find_nm(server.ROOT)
 syms={}
 for line in subprocess.check_output([nm,'-S',str(s.build['elf'])],text=True).splitlines():
  parts=line.split()
  if len(parts)==4 and parts[3] in ('gParties','gPartiesCount'):
   syms[parts[3]]=(int(parts[0],16),int(parts[1],16))
 party_addr,total_party_size=syms['gParties'];owners=syms['gPartiesCount'][1];party_size=total_party_size//owners
 async def rawparty():return await s.core.rpc(4,server.words(party_addr,party_size))
 async def tick(n,button=0):
  inputs.append(dict(frames=n,keys=button))
  while n:
   part=min(n,120);s.ingest(await s.core.tick(keys=button,frames=part));n-=part
 async def query(kind,ident):
  ident=s.cat.constants[ident] if isinstance(ident,str) else ident
  return await read_game_query(s.core,kind,ident,lambda: tick(1))
 def capture(label,force=False):
  nonlocal last,last_state
  state=packet_state(s.packet,decoder);state['party']=list(s.state[12:30])
  im=Image.frombytes('RGBA',(240,160),s.packet[16:153616]).convert('RGB')
  changed=38400 if last is None else sum(p!=(0,0,0) for p in ImageChops.difference(last,im).getdata())
  signature=(state['ready'],state['battle'],state['text_serial'],state['party'])
  if force or changed>320 or signature!=last_state:
   path=OUT/f'{len(shots):03d}-{label}.png';im.save(path)
   shots.append(dict(file=str(path),label=label,state=state));last=im;last_state=signature
 try:
  if MODE=='rival':
   if VARIANT=='female':s.ingest(await s.core.field_command(11,[1]))
   await s.command(dict(op='setup',vars={'VAR_LITTLEROOT_RIVAL_STATE':4 if VARIANT=='optional' else 5,'VAR_BIRCH_LAB_STATE':5,'VAR_ROUTE101_STATE':3,'VAR_PETALBURG_GYM_STATE':0},flags={
    'FLAG_SYS_POKEDEX_GET':True,'FLAG_RECEIVED_DEXNAV':True,'FLAG_ADVENTURE_STARTED':True,
    'FLAG_EC_RIVAL_DEXNAV_TUTORIAL_COMPLETE':False,
    'FLAG_HIDE_ROUTE_101_BIRCH_ZIGZAGOON_BATTLE':True,'FLAG_HIDE_ROUTE_101_BIRCH_STARTERS_BAG':True,'FLAG_HIDE_ROUTE_101_ZIGZAGOON':True}))
  elif MODE=='nurse':
   await s.command(dict(op='setup',flags={'FLAG_EC_OLDALE_SHOP_TUTORIAL_COMPLETE':False}))
  elif MODE!='wally':raise ValueError(MODE)
  tracked=['ITEM_POKE_BALL','ITEM_CHERISH_BALL','ITEM_POTION','ITEM_FOCUS_SASH','ITEM_MENTAL_HERB','ITEM_POKE_VIAL','ITEM_LEVELER','ITEM_REGENERATOR','ITEM_REPEL_SPRAY','ITEM_FLIGHT_BEACON']
  tool_items=tracked[-5:]
  bag_before={name:await query(4,name) for name in tracked}
  pc_before={name:await query(5,name) for name in tool_items}
  original=await rawparty();money=await query(9,0)
  original_party=list(s.state[12:30]);capture('before',True)
  if MODE=='rival':
   await s.command(dict(op='warp',map='Route101',x=10,y=12 if VARIANT=='optional' else 19,facing=4 if VARIANT=='optional' else 2))
   if VARIANT=='optional':
    await tick(150);s.ingest(await s.core.field_command(1));await s.core.rpc(6,str(OUT/'before-scene.sav').encode());await tick(1,keys('A'))
  elif MODE=='nurse':
   await s.command(dict(op='warp',map='OldaleTown',x=14,y=7,facing=2))
   await tick(150);s.ingest(await s.core.field_command(1));await s.core.rpc(6,str(OUT/'before-scene.sav').encode());await tick(1,keys('UP'))
  else:
   s.ingest(await s.core.field_command(1));await s.core.rpc(6,str(OUT/'before-scene.sav').encode())
   await tick(1,keys('UP'));await tick(30);await tick(1,keys('A'))
  completed=False
  injected=False
  for frames in range(0,18000,30):
   await tick(1,keys('A') if frames%90==0 and (MODE=='wally' or s.state[11]) else 0);await tick(29)
   capture(f'frame-{frames:05d}')
   if MODE=='rival' and VARIANT=='abort' and not injected:
    state=packet_state(s.packet)
    # Menus report no field facing; wait for the live search, not a private phase number.
    if state['facing'] and not state['script'] and await query(1,'FLAG_DEXNAV_SEARCHING'):
     s.ingest(await s.core.field_command(10));injected=True;capture('missing-actor-injected',True)
   if frames>180 and s.state[0]:
    if MODE=='rival':completed=(await query(1,'FLAG_EC_RIVAL_DEXNAV_TUTORIAL_COMPLETE')==0 and injected) if VARIANT=='abort' else await query(1,'FLAG_EC_RIVAL_DEXNAV_TUTORIAL_COMPLETE')==1
    elif MODE=='nurse':completed=await query(1,'FLAG_EC_OLDALE_SHOP_TUTORIAL_COMPLETE')==1
    else:completed=await query(2,'VAR_PETALBURG_GYM_STATE')==2 and s.current_map=='PetalburgCity_Gym'
    if completed:break
  capture('returned',True)
  assert completed and s.state[0], 'Tutorial did not finish and return controls'
  after=await rawparty()
  assert after==original, 'Player party bytes changed'
  assert await query(9,0)==money, 'Tutorial spent player money'
  bag_after={name:await query(4,name) for name in tracked}
  for name in tracked:
   expected=max(1,bag_before[name]) if MODE=='nurse' and name in tool_items and pc_before[name]==0 else bag_before[name]
   assert bag_after[name]==expected,(name,expected,bag_after[name])
  if MODE=='nurse' or (MODE=='rival' and VARIANT!='abort'):
   await s.command(dict(op='warp',map='OldaleTown' if MODE=='nurse' else 'Route101',x=14 if MODE=='nurse' else 10,y=7 if MODE=='nurse' else 19,facing=2))
   await tick(180)
   if MODE=='nurse':await tick(1,keys('UP'));await tick(300)
   capture('repeat-entry',True)
   assert s.state[0], 'Completed tutorial repeated on re-entry'
   assert await rawparty()==original
   assert {name:await query(4,name) for name in tracked}==bag_after
  passed=True
 finally:
  (OUT/'evidence.json').write_text(json.dumps(dict(mode=MODE,variant=VARIANT,synthetic=True,build=s.build_info(),passed=passed,inputs=inputs,captures=shots,
   original_party=locals().get('original_party'),bag_before=locals().get('bag_before'),bag_after=locals().get('bag_after'),party_restored=locals().get('after')==locals().get('original'),final_map=s.current_map),indent=2))
  await s.core.close()
 print(OUT)

asyncio.run(main())
