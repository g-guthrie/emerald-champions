from pathlib import Path
import sys,subprocess,struct
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
x=int(sys.argv[1]) if len(sys.argv)>1 else 2;single='single' in sys.argv;tag=f'wally-entrance-{x}-'+('single' if single else 'pair')+'-'+('fixed' if 'fixed' in sys.argv else 'before')
program=bytes([0x16])+struct.pack('<HH',0x40c3,0)+bytes([0x2a])+struct.pack('<H',0x7e)+bytes([0x29])+struct.pack('<H',0x35a)+bytes([0x39,24,43,255])+struct.pack('<HH',x,24)+bytes([0x27,0x6b,2]);addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext']
c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'victory-audit-base.mgba'),'--frames','11000','--state-out',str(out/(tag+'.mgba')),'--screenshot',str(out/(tag+'-end.png'))]
if single:
 for j in range(100,600,4):c+=['--write',f'40:4:{sy["gParties"]+j}:0']
 c+=['--write',f'40:1:{sy["gPartiesCount"]}:1']
for j,v in enumerate(program):c+=['--write',f'50:1:{addr+j}:{v}']
for w,a,v in [(1,ctx,0),(4,ctx+8,addr),(1,ctx+1,1),(1,sy['sGlobalScriptContextStatus'],0),(4,sy['gEcHeadlessCampaignQueryKind'],2),(4,sy['gEcHeadlessCampaignQueryId'],0x40c3)]:c+=['--write',f'50:{w}:{a}:{v}']
for f,d,k in [(900,16,'UP')]+[(f,2,'B') for f in range(1900,10500,550)]:c+=['--key',f'{f}:{d}:{k}']
for f in [800,1800,2800,4500,6500]:c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr)
