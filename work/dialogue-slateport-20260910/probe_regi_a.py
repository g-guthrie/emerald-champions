from pathlib import Path
import sys,subprocess,struct,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
base=out/'regi-a-base.mgba'
if 'base' in sys.argv:
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames','1000','--state-out',str(base)]
 for f,n,v in [(59,'gEcHeadlessFixtureParam',241),(60,'gEcHeadlessFixtureScenario',71)]:c+=['--write',f'{f}:4:{sy[n]}:{v}']
 print('base',subprocess.run(c,capture_output=True,text=True).stdout,flush=True)
def run(case):
 name,m,x,y,lic,badge,flag,variant=case;tag=f'{name}-a-{variant}'
 if variant=='clue':x,y=8,21
 program=b''
 for slot in range(6):
  species=151 if slot==3 and variant!='incompatible' else 129
  program+=bytes([0x23])+struct.pack('<I',(sy['ScrCmd_createmon']|1)+0x02000000)+struct.pack('<BBHHIHHHH',0,slot,species,50,15<<17,33,0,0,0)
 for f in [lic,badge]:program+=bytes([0x2a if (variant=='no-license' and f==lic) or (variant=='no-badge' and f==badge) else 0x29])+struct.pack('<H',f)
 program+=bytes([0x29 if variant=='completed' else 0x2a])+struct.pack('<H',flag)
 program+=bytes([0x39,24,m,255])+struct.pack('<HH',x,y+(variant=='outside'))+bytes([0x27,0x6b,2]);addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext'];assert len(program)<200
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(base),'--frames','6500','--state-out',str(out/(tag+'.mgba')),'--screenshot',str(out/(tag+'-end.png'))]
 for j,v in enumerate(program):c+=['--write',f'50:1:{addr+j}:{v}']
 for w,a,v in [(1,ctx,0),(4,ctx+8,addr),(1,ctx+1,1),(1,sy['sGlobalScriptContextStatus'],0),(4,sy['gEcHeadlessCampaignQueryKind'],1),(4,sy['gEcHeadlessCampaignQueryId'],flag)]:c+=['--write',f'50:{w}:{a}:{v}']
 keys=([(700,2,'UP')] if variant=='clue' else [])+[(900,2,'A')]
 if variant=='accept':keys += [(1600,2,'A'),(2100,2,'A'),(2600,2,'B'),(4600,2,'A')]
 elif variant=='clue':
  directions=['LEFT','LEFT','DOWN','DOWN'] if name=='regirock' else ['DOWN']*4
  keys += [(1600,2,'B')]+[(1800+i*150,16,k) for i,k in enumerate(directions)]+[(2600,2,'A'),(3300,2,'A'),(3900,2,'A')]
 elif variant=='cancel':keys += [(1600,2,'B')]
 else: keys += [(2400,2,'B'),(2900,2,'B')]
 for f,d,k in keys:c+=['--key',f'{f}:{d}:{k}']
 for f in [1450,1950,2220,2300,2500,3100,4000,4100]:c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
 c+=['--read',f'4:{sy["gFieldEffectArguments"]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
variants=['clue'] if 'clue' in sys.argv else ['accept'] if 'base' in sys.argv else ['cancel','no-license','no-badge','incompatible','outside','completed']
cases=[(*c,v) for c in [('regirock',6,6,23,0x6b,0x869,0x8b0),('registeel',68,8,25,0x6d,0x868,0x8b2)] for v in variants]
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(run,cases))
