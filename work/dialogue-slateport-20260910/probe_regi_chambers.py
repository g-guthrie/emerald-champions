from pathlib import Path
import sys, subprocess, struct, concurrent.futures
sys.path.insert(0, 'scripts')
from native_tools import symbols
root=Path.cwd(); out=root/'work/dialogue-slateport-20260910'
sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
def run(case):
 name, state, mapnum, flag, right = case
 for stage in range(3):
  tag=f'{name}-chamber-{stage}'
  c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/(state+'.mgba')),'--frames','11000','--state-out',str(out/(tag+'.mgba')),'--screenshot',str(out/(tag+'-end.png'))]
  if stage==0:
   for n,v in [('gEcHeadlessCampaignQueryKind',1),('gEcHeadlessCampaignQueryId',flag),('gEcHeadlessCampaignCaptureSerial',0),('gEcHeadlessCampaignBattleSerial',0),('gEcHeadlessFixtureParam',249)]: c+=['--write',f'50:4:{sy[n]}:{v}']
   keys=[(300+i*110,16,'RIGHT') for i in range(right)] + [(650+i*150,16,'UP') for i in range(17)] + [(3500,2,'A')]
  else:
   program=bytes([0x39,24,mapnum,255])+struct.pack('<HH',8,8)+bytes([0x27,0x6b,2]); addr=sy['gStringVar4']+800; ctx=sy['sGlobalScriptContext']
   for j,v in enumerate(program):c+=['--write',f'50:1:{addr+j}:{v}']
   for w,a,v in [(1,ctx,0),(4,ctx+8,addr),(1,ctx+1,1),(1,sy['sGlobalScriptContextStatus'],0)]:c+=['--write',f'50:{w}:{a}:{v}']
   keys=[(700,2,'UP'),(900,2,'A')]
  keys += [(f,2,'B') for f in range(4000 if stage==0 else 1400,10500,220)]
  for f,d,k in keys:c+=['--key',f'{f}:{d}:{k}']
  for f in ([3300,3900] if stage==0 else [800,1300]):c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
  names=['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignCaptureSerial','gEcHeadlessCampaignLastCapturedSpecies','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']
  for n in names:c+=['--read',f'4:{sy[n]}']
  r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
  state=tag
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:
 list(ex.map(run,[('regirock','regirock-smash-fixed',6,0x1bb,2),('registeel','registeel-flash-fixed',68,0x1bd,0)]))
