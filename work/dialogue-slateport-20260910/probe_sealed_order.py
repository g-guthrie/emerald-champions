from pathlib import Path
import sys,subprocess,struct,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
def run(tag,reverse):
 mons=[369 if reverse else 321,25,25,25,25,321 if reverse else 369];program=b''
 for slot,sp in enumerate(mons):program+=bytes([0x23])+struct.pack('<I',(sy['ScrCmd_createmon']|1)+0x02000000)+struct.pack('<BBHHI',0,slot,sp,50,0)
 program+=bytes([0x2a])+struct.pack('<H',0xe4)+bytes([0x39,24,72,255])+struct.pack('<HH',10,5)+bytes([0x27,0x6b,2]);assert len(program)<200
 addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext'];c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'ice-upstairs.mgba'),'--frames','9000','--state-out',str(out/(tag+'.mgba')),'--screenshot',str(out/(tag+'-end.png'))]
 for j,v in enumerate(program):c+=['--write',f'50:1:{addr+j}:{v}']
 for width,ptr,val in [(1,ctx,0),(4,ctx+8,addr),(1,ctx+1,1),(1,sy['sGlobalScriptContextStatus'],0),(4,sy['gEcHeadlessCampaignQueryKind'],1),(4,sy['gEcHeadlessCampaignQueryId'],0xe4)]:c+=['--write',f'50:{width}:{ptr}:{val}']
 for f,d,k in [(700,2,'UP'),(900,2,'A'),(6000,2,'A')]+[(f,2,'B') for f in range(1400,8500,220) if not 5600<f<6200]:c+=['--key',f'{f}:{d}:{k}']
 for f in [1200,1750,2300,6200]:c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(lambda p:run(*p),[('sealed-correct-order',False),('sealed-wrong-order',True)]))
