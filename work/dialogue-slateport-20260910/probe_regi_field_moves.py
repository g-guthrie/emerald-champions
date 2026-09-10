from pathlib import Path
import sys,subprocess,struct,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
def run(case):
 tag,mapnum,x,y,move,license,flag=case
 negative='negative' in sys.argv
 outside='outside' in sys.argv
 incompatible='incompatible' in sys.argv
 tag+='-fixed'
 if negative: tag+='-no-license'
 if incompatible:tag+='-incompatible'
 if outside: tag+='-outside';y+=1
 program=bytes([0x23])+struct.pack('<I',(sy['ScrCmd_createmon']|1)+0x02000000)+struct.pack('<BBHHIHHHH',0,0,129 if incompatible else 151,50,15<<17,move if move==91 else 33,0,0,0)
 for f in [0x860,license]:
  if f:program+=bytes([0x2a if negative and f==license else 0x29])+struct.pack('<H',f)
 for f in [0x861,flag]:program+=bytes([0x2a])+struct.pack('<H',f)
 program+=bytes([0x39,24,mapnum,255])+struct.pack('<HH',x,y)+bytes([0x27,0x6b,2]);addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext']
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'field-audit-base.mgba'),'--frames','10000','--state-out',str(out/(tag+'.mgba')),'--screenshot',str(out/(tag+'-end.png'))]
 for j,v in enumerate(program):c+=['--write',f'50:1:{addr+j}:{v}']
 for w,a,v in [(1,ctx,0),(4,ctx+8,addr),(1,ctx+1,1),(1,sy['sGlobalScriptContextStatus'],0),(4,sy['gEcHeadlessCampaignQueryKind'],1),(4,sy['gEcHeadlessCampaignQueryId'],flag)]:c+=['--write',f'50:{w}:{a}:{v}']
 keys=[(900,2,'START'),(1200,2,'A'),(1600,2,'A'),(1900,2,'DOWN'),(2150,2,'A')]
 if move==91:keys += [(3000,2,'A')]
 if negative or outside or incompatible:keys=keys[:3]+[(2400,2,'B'),(2750,2,'B'),(3100,2,'B')]
 for f,d,k in keys:c+=['--key',f'{f}:{d}:{k}']
 for f in [1100,1450,1800,2500,3500,5000]:c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
cases=[('regirock-smash',6,6,23,249,0x6b,0x8b0),('registeel-flash',68,8,25,148,0x6d,0x8b2),('sealed-dig',71,10,3,91,0,0x8af)]
cases=cases[:2]
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(run,cases))
