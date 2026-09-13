#!/usr/bin/env python3
"""Advance an earned native session or an explicitly separate synthetic scene.

See docs/VERIFICATION.md. Uses immutable per-session ROM/ELF, native buttons,
read-only telemetry, before-state archives and screenshot/trace evidence.
"""
from pathlib import Path
import sys, argparse, re, json, shutil, hashlib
ROOT=Path(__file__).resolve().parents[2]; sys.path.insert(0,str(ROOT/'scripts'))
import render_emerald_champions_ui as ui
from rom_artifacts import verify_rom_elf_pair
from run_emerald_champions_campaign import parse_numeric_constants,parse_map_ids
p=argparse.ArgumentParser(); p.add_argument('out',type=Path);p.add_argument('label');p.add_argument('--boot',type=lambda x:int(x,0));p.add_argument('--scenario',default='CAMPAIGN_NATIVE');p.add_argument('--frames',type=int,default=240);p.add_argument('--key',action='append',default=[]);p.add_argument('--at',action='append',default=[]);p.add_argument('--flag');p.add_argument('--query',nargs=2);p.add_argument('--write',action='append',default=[]);p.add_argument('--read-symbol',action='append',default=[]);p.add_argument('--save',type=Path);p.add_argument('--until');p.add_argument('--read-memory',action='append',default=[]);p.add_argument('--save-out',type=Path,help='export current battery state after an actual native Save');args=p.parse_args()
rom=ROOT/'pokeemerald-headless.gba';elf=ROOT/'pokeemerald-headless.elf';out=args.out.resolve();out.mkdir(parents=True,exist_ok=True)
manifest=out/'trace.json'; state=out/'current.ss1'; scratch=out/'scene.gba'
if args.boot is not None:
 verify_rom_elf_pair(rom,elf)
 if manifest.exists(): raise SystemExit('Use a new output directory for each boot.')
 shutil.copy2(rom,scratch)
 shutil.copy2(elf,out/'scene.elf')
 trace={'rom_sha256':hashlib.sha256(rom.read_bytes()).hexdigest(),'elf_sha256':hashlib.sha256(elf.read_bytes()).hexdigest(),'param':args.boot,'scenario':args.scenario,'evidence':('fresh normal new game; native battle resolution; observer telemetry only' if args.scenario=='CAMPAIGN_NATIVE' else 'synthetic prerequisite state; real NPC/map scripts; automatic battle result; not combat validation'),'steps':[]}
else: trace=json.loads(manifest.read_text())
if trace.get('scenario') == 'CAMPAIGN_NATIVE' and args.write:
 raise SystemExit('Progress/setup writes are forbidden in an earned native session; use a separate synthetic output directory.')
before=out/f'{len(trace["steps"]):03d}-{args.label}.before.ss1' if args.boot is None else None
if before is not None: shutil.copy2(state,before)
if args.boot is not None:
 stamp=ROOT/'pokeemerald-headless.inputs.json'
 if stamp.exists():
  evidence=json.loads(stamp.read_text())
  if evidence.get('artifacts',{}).get('pokeemerald-headless.elf')==trace['elf_sha256'] and evidence.get('artifacts',{}).get('pokeemerald-headless.gba')==trace['rom_sha256']:shutil.copy2(stamp,out/'inputs.json')
elf=out/'scene.elf'
if not elf.exists() or hashlib.sha256(elf.read_bytes()).hexdigest()!=trace['elf_sha256']:raise SystemExit('This scene requires its matching saved ELF; do not use a newer root ELF.')
syms={line.split()[-1]:int(line.split()[0],16) for line in ui.run([shutil.which('arm-none-eabi-nm'),'-S',str(elf)]).stdout.splitlines() if len(line.split())>=3 and re.fullmatch('[0-9a-fA-F]+',line.split()[0])}
fields=['FixtureSetupResult','FixtureObservedResult','FixtureTrigger','CampaignMapId','CampaignPlayerX','CampaignPlayerY','CampaignPlayerFacing','CampaignControlsLocked','CampaignScriptEnabled','CampaignInBattle','CampaignBattleSerial','CampaignLastOpponentA','CampaignLastOpponentB','CampaignQueryObjectActive','CampaignQueryObjectX','CampaignQueryObjectY','CampaignCaptureSerial','CampaignLastResolution','CampaignForceLoss','CampaignQueryValue']
addresses={name:syms['gEcHeadless'+name] for name in fields}
runner=ui.build_runner();trace.setdefault('runner_sha256',hashlib.sha256(runner.read_bytes()).hexdigest()); nxt=out/'next.ss1'; shot=out/f'{len(trace["steps"]):03d}-{args.label}.png'
command=[str(runner),'--rom',str(scratch),'--rtc','946684800','--frames',str(args.frames),'--state-out',str(nxt),'--screenshot',str(shot)]
if args.save is not None:
 if args.boot is None: raise SystemExit('--save is only for a clean boot, not a savestate resume.')
 shutil.copy2(args.save,out/'scene.sav')
 trace['battery_parent']={'path':str(args.save.resolve()),'sha256':hashlib.sha256(args.save.read_bytes()).hexdigest(),'evidence':'normal in-game save imported on a clean boot; not a cross-ROM savestate'}
 trace['evidence']='earned fresh-save campaign continued by normal Save and Continue on an updated ROM; native combat and captures; observer telemetry only'
 command+=['--save',str(out/'scene.sav')]
if args.boot is None: command+=['--state-in',str(state)]
else:
 enum=(ROOT/'include/emerald_champions_headless.h').read_text().split('enum EmeraldChampionsHeadlessScenario')[1].split('};')[0]
 scenario=re.findall(r'EC_HEADLESS_SCENARIO_\w+',enum).index('EC_HEADLESS_SCENARIO_'+args.scenario)
 command+=['--write',f'59:4:0x{syms["gEcHeadlessFixtureParam"]:x}:{args.boot}','--write',f'60:4:0x{syms["gEcHeadlessFixtureScenario"]:x}:{scenario}']
if args.save_out is not None:
 args.save_out.parent.mkdir(parents=True,exist_ok=True)
 command+=['--save-out',str(args.save_out)]
for key in args.key: command+=['--key',key]
intermediate=[]
for capture in args.at:
 frame,label=capture.split(':',1);filename=out/f'{len(trace["steps"]):03d}-{args.label}-{label}.png';command+=['--screenshot-at',f'{frame}:{filename}'];intermediate.append({'frame':int(frame),'screenshot':str(filename)})
for spec in args.write:
 name,val=spec.split('=',1); command+=['--write',f'0:4:0x{syms["gEcHeadless"+name]:x}:{int(val,0)}']
constants=parse_numeric_constants()
query_frame=61 if args.boot is not None else 0
query=args.query
if args.flag: query=['1',str(constants[args.flag])]
if query:
 kind,ident=query; ident=constants.get(ident,ident)
 command+=['--write',f'{query_frame}:4:0x{syms["gEcHeadlessCampaignQueryKind"]:x}:{kind}','--write',f'{query_frame}:4:0x{syms["gEcHeadlessCampaignQueryId"]:x}:{ident}']
for addr in addresses.values():command+=['--read',f'4:0x{addr:x}']
extra_reads={}
for spec in args.read_symbol:
 width,name=spec.split(':',1);parts=name.split('+',1);address=syms[parts[0]]+(int(parts[1],0) if len(parts)>1 else 0);command+=['--read',f'{width}:0x{address:x}'];extra_reads[name]=address
for spec in args.read_memory:
 width,address=spec.split(':',1);command+=['--read',spec];extra_reads[spec]=int(address,0)
if args.until:command+=['--until',args.until]
result=ui.run(command);nxt.replace(state)
reads={int(a,16):int(v,16) for a,v in re.findall(r'READ width=\d+ address=([0-9a-f]+) value=([0-9a-f]+)',result.stdout)};values={name:reads[addr] for name,addr in addresses.items()};values['readback']={name:reads[addr] for name,addr in extra_reads.items()}
values['map']=next((name for name,n in parse_map_ids().items() if n==values['CampaignMapId']),'unknown') if trace.get('scenario','BOOK_RESEARCH') in ['BOOK_RESEARCH','STORY_HANDOFF','CAMPAIGN_AUTOWIN','CAMPAIGN_NATIVE'] else 'campaign telemetry not populated for this UI fixture'
trace['steps'].append({'label':args.label,'state_before':str(before) if before else None,'frames':args.frames,'until':args.until,'frames_run':int(re.search(r'RESULT frames=(\d+)',result.stdout).group(1)),'keys':args.key,'intermediate':intermediate,'query':query,'telemetry':values,'screenshot':str(shot)})
manifest.write_text(json.dumps(trace,indent=2)+'\n');print(json.dumps(values));print(shot)
