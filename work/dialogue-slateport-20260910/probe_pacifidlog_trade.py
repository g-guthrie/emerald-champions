from pathlib import Path
import sys,subprocess,struct
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
program=b''
for slot in range(6):
 program+=bytes([0x23])+struct.pack('<I',(sy['ScrCmd_createmon']|1)+0x02000000)+struct.pack('<BBHHIHHHH',0,slot,371 if slot==4 else 129,50,15<<17,33,0,0,0)
program+=bytes([0x2a])+struct.pack('<H',0x9a)+bytes([0x39,7,4,255])+struct.pack('<HH',4,3)+bytes([0x27,0x6b,2]);addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext'];assert len(program)<200
c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'dig-text-base.mgba'),'--frames','4200','--state-out',str(out/'pacifidlog-trade-choice.mgba'),'--screenshot',str(out/'pacifidlog-trade-choice-end.png')]
for j,v in enumerate(program):c+=['--write',f'50:1:{addr+j}:{v}']
for w,a,v in [(1,ctx,0),(4,ctx+8,addr),(1,ctx+1,1),(1,sy['sGlobalScriptContextStatus'],0),(4,sy['gEcHeadlessCampaignQueryKind'],1),(4,sy['gEcHeadlessCampaignQueryId'],0x9a)]:c+=['--write',f'50:{w}:{a}:{v}']
for f,d,k in [(700,2,'UP'),(900,2,'A'),(1600,2,'B'),(2300,2,'B'),(3000,2,'B'),(3700,2,'A')]:c+=['--key',f'{f}:{d}:{k}']
for f in [1400,2100,2800,3500,4100]:c+=['--screenshot-at',f'{f}:{out}/pacifidlog-trade-choice-{f}.png']
for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled']:c+=['--read',f'4:{sy[n]}']
r=subprocess.run(c,capture_output=True,text=True);print(r.returncode,r.stdout,r.stderr)
