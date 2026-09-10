from pathlib import Path
import sys,subprocess,struct,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
def run(case):
 name,side,missing=case;tag='league-gate-'+name
 program=bytes([0x16])+struct.pack('<HH',0x409c,0)
 for f in [0x107,0x4fb,0x4fc,0x4fd,0x4fe]:program+=bytes([0x2a])+struct.pack('<H',f)
 for i in range(8):program+=bytes([0x2a if i==missing else 0x29])+struct.pack('<H',0x867+i)
 program+=bytes([0x39,16,10,255])+struct.pack('<HH',11 if side else 9,2 if side else 3)+bytes([0x27,0x6b,2]);addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext']
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'victory-audit-base.mgba'),'--frames','6500','--state-out',str(out/(tag+'.mgba')),'--screenshot',str(out/(tag+'-end.png'))]
 for j,v in enumerate(program):c+=['--write',f'50:1:{addr+j}:{v}']
 for w,a,v in [(1,ctx,0),(4,ctx+8,addr),(1,ctx+1,1),(1,sy['sGlobalScriptContextStatus'],0),(4,sy['gEcHeadlessCampaignQueryKind'],1),(4,sy['gEcHeadlessCampaignQueryId'],0x107)]:c+=['--write',f'50:{w}:{a}:{v}']
 keys=[(700,2,'LEFT' if side else 'UP'),(900,2,'A')]+[(f,2,'B') for f in range(1500,3800,550)]
 if missing==-1:keys +=[(4500,280,'UP')]
 for f,d,k in keys:c+=['--key',f'{f}:{d}:{k}']
 for f in [1400,2700,4200,6000]:c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(run,[('front',False,-1),('right',True,-1),('missing-dynamo',False,2)]))
