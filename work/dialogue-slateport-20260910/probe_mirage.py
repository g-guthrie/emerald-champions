from pathlib import Path
import sys,subprocess,struct,concurrent.futures,re
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'field-audit-base.mgba'),'--frames','1']
for i in range(6):c+=['--read',f'4:{sy["gParties"]+100*i}']
r=subprocess.run(c,capture_output=True,text=True);print('party personalities',r.stdout,flush=True)
pids=[int(x,16)&65535 for x in re.findall(r'value=([0-9a-f]+)',r.stdout)]
missing=next(i for i in range(65536) if i not in pids)
def run(present):
 tag='mirage-present' if present else 'mirage-absent';value=pids[0] if present else missing
 for stage in ('watcher','route'):
  name=tag+'-'+stage
  program=bytes([0x16])+struct.pack('<HH',0x4024,value)
  program+=bytes([0x39,7,6,255])+struct.pack('<HH',9,5) if stage=='watcher' else bytes([0x39,0,45,255])+struct.pack('<HH',52,10)
  program+=bytes([0x27,0x6b,2]);addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext']
  c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/('field-audit-base.mgba' if stage=='watcher' else tag+'-watcher.mgba')),'--frames','2200','--state-out',str(out/(name+'.mgba')),'--screenshot',str(out/(name+'-end.png'))]
  for j,v in enumerate(program):c+=['--write',f'50:1:{addr+j}:{v}']
  for w,a,v in [(1,ctx,0),(4,ctx+8,addr),(1,ctx+1,1),(1,sy['sGlobalScriptContextStatus'],0),(4,sy['gEcHeadlessCampaignQueryKind'],2),(4,sy['gEcHeadlessCampaignQueryId'],0x4024)]:c+=['--write',f'50:{w}:{a}:{v}']
  if stage=='watcher':
   for f,d,k in [(700,2,'UP'),(900,2,'A'),(1600,2,'B')]:c+=['--key',f'{f}:{d}:{k}']
   c+=['--screenshot-at',f'1400:{out}/{name}-dialogue.png']
  for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
  c+=['--read',f'2:{sy["gMapHeader"]+0x12}','--read',f'4:{sy["gMapHeader"]}','--read',f'2:{sy["gSpecialVar_Result"]}']
  r=subprocess.run(c,capture_output=True,text=True);print(name,value,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(run,[False,True]))
