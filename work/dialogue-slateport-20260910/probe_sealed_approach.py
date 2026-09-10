from pathlib import Path
import sys,subprocess,struct,json
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
program=bytes([0x23])+struct.pack('<I',(sy['ScrCmd_createmon']|1)+0x02000000)+struct.pack('<BBHHIHHHH',0,0,151,50,15<<17,91,0,0,0)
for f in [0x7b,0x860]:program+=bytes([0x29])+struct.pack('<H',f)
for f in [0x861,0x8af]:program+=bytes([0x2a])+struct.pack('<H',f)
program+=bytes([0x39,0,49,255])+struct.pack('<HH',60,31)+bytes([0x27,0x6b,2]);addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext']
c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'dig-text-base.mgba'),'--frames','4500','--state-out',str(out/'sealed-dived.mgba'),'--screenshot',str(out/'sealed-dived-end.png')]
for j,v in enumerate(program):c+=['--write',f'50:1:{addr+j}:{v}']
for w,a,v in [(1,ctx,0),(4,ctx+8,addr),(1,ctx+1,1),(1,sy['sGlobalScriptContextStatus'],0)]:c+=['--write',f'50:{w}:{a}:{v}']
keys=[(900,2,'A'),(1700,2,'A'),(2300,2,'A'),(3300,16,'DOWN'),(3550,16,'DOWN')]
for f,d,k in keys:c+=['--key',f'{f}:{d}:{k}']
for f in [800,1500,2100,3100,4200]:c+=['--screenshot-at',f'{f}:{out}/sealed-dived-{f}.png']
for n in ['gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
r=subprocess.run(c,capture_output=True,text=True);print(r.returncode,r.stdout,r.stderr)
