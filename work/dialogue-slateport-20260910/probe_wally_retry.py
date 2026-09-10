from pathlib import Path
import sys,subprocess,struct,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
def run(x):
 tag=f'wally-retry-{x}'
 program=bytes([0x23])+struct.pack('<I',(sy['ScrCmd_createmon']|1)+0x02000000)+struct.pack('<BBHHI',0,1,151,50,0)+bytes([0x6b,2]);addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext']
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/f'wally-entrance-{x}-single-fixed.mgba'),'--frames','13000','--state-out',str(out/(tag+'.mgba')),'--screenshot',str(out/(tag+'-end.png'))]
 for j,v in enumerate(program):c+=['--write',f'50:1:{addr+j}:{v}']
 for w,a,v in [(1,ctx,0),(4,ctx+8,addr),(1,ctx+1,1),(1,sy['sGlobalScriptContextStatus'],0)]:c+=['--write',f'50:{w}:{a}:{v}']
 for f,d,k in [(800,16,'UP')]+[(f,2,'B') for f in range(1700,12600,550)]:c+=['--key',f'{f}:{d}:{k}']
 for f in [600,1500,2800,5000,9000]:c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(run,[2,3]))
