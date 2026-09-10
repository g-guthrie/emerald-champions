from pathlib import Path
import sys,subprocess,json,re
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-dewford-20260910');sy=symbols(Path('pokeemerald-dewford-audit-headless.elf'),Path.cwd(),first=True);(out/'devon-symbols.json').write_text(json.dumps(sy))
const=Path('include/constants/vars.h').read_text()
for param,query in [(26,'VAR_FOSSIL_RESURRECTION_STATE')]:
 var=int(re.search(r'#define '+query+r'\s+(0x[0-9A-Fa-f]+)',const)[1],16)
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-dewford-audit-headless.gba','--frames','12000','--screenshot',str(out/f'devon-{param}-end.png')]
 for f,n,v in [(59,'gEcHeadlessFixtureParam',param),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',2),(70,'gEcHeadlessCampaignQueryId',var)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
 if param==26:cmd+=['--key','300:2:UP','--key','400:2:A']
 for frame in [750,2000,4000,6000,8000]:cmd+=['--screenshot-at',f'{frame}:{out}/devon-{param}-{frame}.png']
 for frame in range(1000,11000,100):cmd+=['--key',f'{frame}:2:B']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId']:
  cmd+=['--read',f'4:{sy[n]}']
 cmd+=['--read',f'1:{sy["gPartiesCount"]}']
 r=subprocess.run(cmd,capture_output=True,text=True);(out/f'devon-{param}.log').write_text(r.stdout+r.stderr);print(param,query,r.returncode,r.stdout,flush=True)
