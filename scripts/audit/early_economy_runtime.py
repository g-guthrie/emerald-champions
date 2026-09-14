#!/usr/bin/env python3
"""Native early-reward and Steven Aerodactylite transactions; synthetic prerequisites."""
import argparse, hashlib, json, re, shutil, sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'scripts'))
import render_emerald_champions_ui as ui
import run_emerald_champions_campaign as c
from rom_artifacts import verify_rom_elf_pair
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);a=p.parse_args()
out=a.out.resolve();out.mkdir(parents=True,exist_ok=False)
rom=ROOT/'pokeemerald-headless.gba';elf=ROOT/'pokeemerald-headless.elf';verify_rom_elf_pair(rom,elf)
shutil.copy2(rom,out/'scene.gba');shutil.copy2(elf,out/'scene.elf');rom=out/'scene.gba'
const=c.parse_numeric_constants();enum=(ROOT/'include/emerald_champions_headless.h').read_text().split('enum EmeraldChampionsHeadlessScenario',1)[1].split('};',1)[0]
scenario=re.findall(r'EC_HEADLESS_SCENARIO_\w+',enum).index('EC_HEADLESS_SCENARIO_STORY_HANDOFF')
addresses={n:ui.resolve_symbol(elf,n) for n in (*c.TELEMETRY_SYMBOLS,'gEcHeadlessFixtureScenario','gEcHeadlessFixtureParam','gEcHeadlessFixtureTrigger')}
runner=ui.build_runner();rows=[];state=out/'current.ss1'
common=dict(runner=runner,rom=rom,state=state,addresses=addresses)
result=dict(status='running',scope='Synthetic prerequisite fixtures, native map/NPC transactions. No earned clears or difficulty acceptance.',rom_sha256=hashlib.sha256(rom.read_bytes()).hexdigest(),trace=rows)
def require(ok,msg):
 if not ok:raise RuntimeError(msg)
def step(label,frames=100,keys=None,writes=None):
 shot=out/f'{len(rows):03d}-{label}.png'
 v,_=c.run_state_chunk(**common,frames=frames,keys=keys,writes=writes,screenshot=shot)
 rows.append(dict(label=label,telemetry=v,screenshot=str(shot)));return v
def query(kind,name):
 value,_=c.query_campaign_value(kind=kind,identifier=const.get(name,name),**common)
 rows.append(dict(query=kind,name=name,value=value));return value
def boot(param):
 ui.run([str(runner),'--rom',str(rom),'--rtc','946684800','--frames','300','--state-out',str(state),'--write',f"59:4:0x{addresses['gEcHeadlessFixtureParam']:x}:{param}",'--write',f"60:4:0x{addresses['gEcHeadlessFixtureScenario']:x}:{scenario}"])
 v=step(f'boot-{param}',2);require(c.is_stable_overworld(v),f'boot{param} locked')
def talk(label):
 step(label+'-face',32,[(0,2,'UP')])
 for i in range(140):
  v=step(label,70,[(0,2,'A')])
  if c.is_stable_overworld(v):return
 raise RuntimeError(label+' did not settle')
def actor(wanted):
 active,position,_=c.query_campaign_object(local_id=1,**common);rows.append(dict(actor=1,active=active,position=position));require(active==wanted,'Steven visibility')
def check(kind,name,want):
 got=query(kind,name);require(got==want,f'{name}: {got} != {want}')
def reload():step('reload-native-map',220,writes=[(0,4,addresses['gEcHeadlessFixtureTrigger'],1)])
try:
 boot(276);talk('oldale-heal-balls');check(4,'ITEM_HEAL_BALL',3);check(1,'FLAG_RECEIVED_POTION_OLDALE',1);talk('oldale-repeat');check(4,'ITEM_HEAL_BALL',3)
 boot(275);talk('oldale-full-ball-pocket');check(4,'ITEM_HEAL_BALL',0);check(1,'FLAG_RECEIVED_POTION_OLDALE',0)
 boot(282);talk('woods-gift-at-nurse');check(4,'ITEM_DUSK_BALL',3);check(1,'FLAG_EC_WOODS_GREAT_BALL_PENDING',0);talk('nurse-repeat');check(4,'ITEM_DUSK_BALL',3)
 boot(256);talk('nurse-full-ball-pocket');check(4,'ITEM_DUSK_BALL',0);check(1,'FLAG_EC_WOODS_GREAT_BALL_PENDING',1)
 boot(281);talk('route115-net-pack');check(4,'ITEM_NET_BALL',3);check(1,'FLAG_EC_GARDEN_BUNDLE_ROUTE115_PINAP_BERRY',1);check(4,'ITEM_PINAP_BERRY',0)
 for param in (265,266):
  boot(param);talk('roxanne-amber-only');check(4,'ITEM_AERODACTYLITE',0);check(1,'FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE',0);check(1,'FLAG_RECEIVED_ROXANNE_OLD_AMBER',1)
 boot(272);talk('steven-ring-and-aerodactylite');check(4,'ITEM_MEGA_RING',1);check(4,'ITEM_AERODACTYLITE',1);check(1,'FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE',1);actor(False);reload();actor(False);check(4,'ITEM_AERODACTYLITE',1)
 boot(283);talk('steven-pc-fallback');check(4,'ITEM_MEGA_RING',1);check(5,'ITEM_AERODACTYLITE',1);check(1,'FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE',1)
 boot(279);talk('steven-full-storage');check(4,'ITEM_MEGA_RING',1);check(4,'ITEM_AERODACTYLITE',0);check(5,'ITEM_AERODACTYLITE',0);check(1,'FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE',0);actor(True)
 reload();talk('steven-full-storage-repeat');check(1,'FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE',0);actor(True)
 step('free-one-pc-slot',220,writes=[(0,4,addresses['gEcHeadlessFixtureTrigger'],2)])
 talk('steven-deliver-after-room');check(5,'ITEM_AERODACTYLITE',1);check(1,'FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE',1);actor(True)
 talk('steven-no-duplicate-pending-starters');check(5,'ITEM_AERODACTYLITE',1)
 boot(280);before=query(9,0);talk('steven-owned-stone-alternative');check(4,'ITEM_AERODACTYLITE',1);check(9,0,before+3000);check(1,'FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE',1);reload();actor(False);check(9,0,before+3000)
 for name in ('ITEM_POKE_BALL','ITEM_HEAL_BALL','ITEM_DUSK_BALL','ITEM_NET_BALL','ITEM_ORAN_BERRY','ITEM_LIFE_ORB','ITEM_AERODACTYLITE'):check(7,name,0)
 for name,value in [('ITEM_POKE_BALL',100),('ITEM_GREAT_BALL',300),('ITEM_NET_BALL',600),('ITEM_LEAF_STONE',3000),('ITEM_METAL_COAT',5000)]:check(8,name,value)
 result['status']='pass';print('PASS: early gifts/duplicate refusals/full pockets, Roxanne Amber only, Steven Ring/Aero Bag+PC+retry+duplicate, zero-sale and actual prices.',flush=True)
except Exception as e:
 result.update(status='fail',error=str(e));raise
finally:(out/'result.json').write_text(json.dumps(result,indent=2)+'\n')
