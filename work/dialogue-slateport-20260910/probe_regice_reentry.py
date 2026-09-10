from pathlib import Path
import sys,subprocess,struct,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
def run(state):
 tag=state+'-reentry';program=bytes([0x39,24,67,255])+struct.pack('<HH',8,8)+bytes([0x27,0x6b,2]);addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext'];c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/(state+'.mgba')),'--frames','10000','--screenshot',str(out/(tag+'-end.png')),'--screenshot-at',f'800:{out}/{tag}-800.png']
 for j,v in enumerate(program):c+=['--write',f'50:1:{addr+j}:{v}']
 for w,a,v in [(1,ctx,0),(4,ctx+8,addr),(1,ctx+1,1),(1,sy['sGlobalScriptContextStatus'],0)]:c+=['--write',f'50:{w}:{a}:{v}']
 for f,d,k in [(700,2,'UP'),(900,2,'A')]+[(f,2,'B') for f in range(1400,9500,220)]:c+=['--key',f'{f}:{d}:{k}']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignCaptureSerial','gEcHeadlessCampaignLastCapturedSpecies']:c+=['--read',f'4:{sy[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(run,['regice-faint','regice-capture']))
