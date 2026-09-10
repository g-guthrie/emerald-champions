from pathlib import Path
import sys,subprocess,re
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
var=int(re.search(r'#define VAR_ROUTE118_STATE\s+(0x[0-9A-Fa-f]+)',Path('include/constants/vars.h').read_text())[1],0)
for param in map(int,sys.argv[1:] or ['79','80','81']):
 inputs=[];frame=1700
 if param==79:
  inputs+=['--key','300:2:RIGHT','--key','400:2:A','--key','700:2:A','--key','1000:2:A','--screenshot-at',f'1500:{out}/route118-surf-start.png']
  directions=['RIGHT']*15+['DOWN']*4+['RIGHT']*10+['UP']*2
  for i,key in enumerate(directions):
   inputs+=['--key',f'{frame}:16:{key}'];frame+=100
   if i in [9,14,18,28]:inputs+=['--screenshot-at',f'{frame-10}:{out}/route118-crossing-{i}.png']
 else:inputs+=['--key','300:16:UP'];frame=400
 for f in range(frame+300,frame+5000,100):inputs+=['--key',f'{f}:2:B']
 inputs+=['--screenshot-at',f'{frame+800}:{out}/route118-{param}-steven.png']
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(frame+5300),'--screenshot',str(out/f'route118-{param}-end.png')]
 for f,n,v in [(59,'gEcHeadlessFixtureParam',param),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',2),(70,'gEcHeadlessCampaignQueryId',var)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
 cmd+=inputs
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY','gEcHeadlessCampaignBattleSerial']:cmd+=['--read',f'4:{sy[n]}']
 r=subprocess.run(cmd,capture_output=True,text=True);print(param,r.returncode,r.stdout,r.stderr,flush=True)
