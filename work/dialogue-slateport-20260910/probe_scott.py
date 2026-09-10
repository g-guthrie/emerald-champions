from pathlib import Path
import sys,subprocess,json,re
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');sy=symbols(Path('pokeemerald-slateport-audit-headless.elf'),Path.cwd(),first=True)
(out/'native-symbols.json').write_text(json.dumps(sy))
const=Path('include/constants/vars.h').read_text()
# Flag ids are obtained from the compiled enum header by their current definitions.
flag=int(re.search(r'#define VAR_SLATEPORT_OUTSIDE_MUSEUM_STATE\s+(0x[0-9A-Fa-f]+|\d+)',const)[1],0)
for param in [28]:
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-slateport-audit-headless.gba','--frames','18000','--screenshot',str(out/f'discovery-{param}-end.png')]
 for f,n,v in [(59,'gEcHeadlessFixtureParam',param),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',2),(70,'gEcHeadlessCampaignQueryId',flag)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
 for frame in [750,1600,2400,3200,4000,4800]:cmd+=['--screenshot-at',f'{frame}:{out}/discovery-{param}-{frame}.png']
 for frame in range(1000,17000,100):cmd+=['--key',f'{frame}:2:B']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId']:
  cmd+=['--read',f'4:{sy[n]}']
 result=subprocess.run(cmd,capture_output=True,text=True);(out/f'discovery-{param}.log').write_text(result.stdout+result.stderr);print(param,result.returncode,result.stdout,flush=True)
