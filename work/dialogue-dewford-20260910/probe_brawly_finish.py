from pathlib import Path
import sys,subprocess,json,re
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-dewford-20260910');sy=symbols(Path('pokeemerald-dewford-audit-headless.elf'),Path.cwd(),first=True)
(out/'handoff-symbols.json').write_text(json.dumps(sy))
const=Path('include/constants/flags.h').read_text()
for param,query in [(23,'FLAG_BADGE02_GET')]:
 base=int(re.search(r'#define TRAINER_FLAGS_START\s+(0x[0-9A-Fa-f]+)',const)[1],0)+int(re.search(r'#define MAX_TRAINERS_COUNT_EMERALD\s+(\d+)',Path('include/constants/opponents.h').read_text())[1])
 flag=base+int(re.search(r'#define '+query+r'\s+\(SYSTEM_FLAGS \+ (0x[0-9A-Fa-f]+)\)',const)[1],0)
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-dewford-audit-headless.gba','--frames','16000','--screenshot',str(out/f'handoff-{param}-end.png')]
 for f,n,v in [(59,'gEcHeadlessFixtureParam',param),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',1),(70,'gEcHeadlessCampaignQueryId',flag)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
 cmd+=['--key',f'300:2:{"RIGHT" if param==22 else "UP"}','--key','400:2:A']
 for frame in [750,2000,4000,6000,8000,10000]:cmd+=['--screenshot-at',f'{frame}:{out}/handoff-{param}-{frame}.png']
 for frame in range(1000,14000,80):cmd+=['--key',f'{frame}:2:B']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:
  cmd+=['--read',f'4:{sy[n]}']
 result=subprocess.run(cmd,capture_output=True,text=True);(out/f'handoff-{param}.log').write_text(result.stdout+result.stderr);print(param,query,result.returncode,result.stdout,flush=True)
