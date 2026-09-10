from pathlib import Path
import sys,subprocess,struct
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
program=bytes([0x16])+struct.pack('<HH',0x409b,2)+bytes([0x16])+struct.pack('<HH',0x4091,0)+bytes([0x39,0,48,255])+struct.pack('<HH',79,7)+bytes([0x27,0x6b,2]);addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext']
c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'dig-text-base.mgba'),'--frames','6500','--state-out',str(out/'route133-current-crossed.mgba'),'--screenshot',str(out/'route133-current-end.png')]
for j,v in enumerate(program):c+=['--write',f'50:1:{addr+j}:{v}']
for w,a,v in [(1,ctx,0),(4,ctx+8,addr),(1,ctx+1,1),(1,sy['sGlobalScriptContextStatus'],0)]:c+=['--write',f'50:{w}:{a}:{v}']
keys=[(900,16,'LEFT')]
for f,d,k in keys:c+=['--key',f'{f}:{d}:{k}']
for f in [650,705,715,735,1800,2050,2210,2250,3650,4700]:c+=['--screenshot-at',f'{f}:{out}/route133-current-{f}.png']
for n in ['gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
r=subprocess.run(c,capture_output=True,text=True);print(r.returncode,r.stdout,r.stderr)
