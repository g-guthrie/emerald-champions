from pathlib import Path
import sys,subprocess,struct,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
def sv(i,v):return bytes([0x16])+struct.pack('<HH',i,v)
def special(n):return bytes([0x25])+struct.pack('<H',sy['SPECIAL_'+n])
def run(tag,level):
 program=sv(0x800a,0)+sv(0x8008,level)+special('ApplyEmeraldChampionsBonding')+bytes([0x2a])+struct.pack('<H',0x12b)+bytes([0x39,7,3,255])+struct.pack('<HH',3,6)+bytes([0x27,0x6b,2])
 addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext'];c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'ice-upstairs.mgba'),'--frames','12000','--state-out',str(out/(tag+'.mgba')),'--screenshot',str(out/(tag+'-end.png'))]
 for j,v in enumerate(program):c+=['--write',f'50:1:{addr+j}:{v}']
 for width,ptr,val in [(1,ctx,0),(4,ctx+8,addr),(1,ctx+1,1),(1,sy['sGlobalScriptContextStatus'],0),(4,sy['gEcHeadlessCampaignQueryKind'],4),(4,sy['gEcHeadlessCampaignQueryId'],216 if level else 217)]:c+=['--write',f'50:{width}:{ptr}:{val}']
 for f,d,k in [(700,2,'UP'),(900,2,'A'),(8500,2,'A')]+[(f,2,'B') for f in range(1300,11500,220) if not 8000<f<9000]:c+=['--key',f'{f}:{d}:{k}']
 for f in [1200,3500,9000]:c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(lambda p:run(*p),[('pacifidlog-sun',2),('pacifidlog-moon',0)]))
