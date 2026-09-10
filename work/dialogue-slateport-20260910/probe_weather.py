from pathlib import Path
import sys,subprocess,re
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
for param in map(int,[x for x in sys.argv[1:] if x.isdigit()] or ['82','83','85','86','87','88','89','90']):
 retry='--retry' in sys.argv; gate='--gate' in sys.argv; pre='--pre' in sys.argv
 end=500 if pre else 22000 if retry else 12000
 inputs=[]
 if not pre:
  if param<85:inputs+=['--key','300:2:LEFT','--key','400:2:A']
  else:inputs+=['--key','300:16:UP']
  for f in range(900,end-300,100):inputs+=['--key',f'{f}:2:B']
 if retry:
  inputs+=['--write',f'12500:4:{sy["gEcHeadlessFixtureTrigger"]}:2','--key','13200:2:UP','--key','13300:2:A']
  for f in [11500,13000,14400,17000]:inputs+=['--screenshot-at',f'{f}:{out}/weather-retry-{param}-{f}.png']
 kind,q=(6,642 if '--thundurus' in sys.argv else 641) if gate else (1,0x97 if param<85 else 0x6e)
 if '--state' in sys.argv:
  kind=2;q=int(re.search(r'#define VAR_ROUTE119_STATE\s+(0x[0-9A-Fa-f]+)',Path('include/constants/vars.h').read_text())[1],0)
 suffix=('retry' if retry else 'pre' if pre else 'gift')+('-gate' if gate else '')
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(end),'--screenshot',str(out/f'weather-{param}-{suffix}-end.png')]
 for f,n,v in [(59,'gEcHeadlessFixtureParam',param),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',kind),(70,'gEcHeadlessCampaignQueryId',q)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
 cmd+=inputs
 if not pre:
  for f in [1800,4300,6500,8700]:cmd+=['--screenshot-at',f'{f}:{out}/weather-{param}-{f}.png']
 names=['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY','gEcHeadlessCampaignBattleSerial']
 for n in names:cmd+=['--read',f'4:{sy[n]}']
 cmd+=['--read',f'1:{sy["gPartiesCount"]}']
 r=subprocess.run(cmd,capture_output=True,text=True);print(param,suffix,'kind',kind,'id',q,r.returncode,r.stdout,r.stderr,flush=True)
