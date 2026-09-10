from pathlib import Path
import sys,subprocess,re
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
flags=Path('include/constants/flags.h').read_text();variables=Path('include/constants/vars.h').read_text()
base=int(re.search(r'#define TRAINER_FLAGS_START\s+(0x[0-9A-Fa-f]+)',flags)[1],0)+int(re.search(r'#define MAX_TRAINERS_COUNT_EMERALD\s+(\d+)',Path('include/constants/opponents.h').read_text())[1])
for param,name,kind in [(p,'VAR_LAVARIDGE_TOWN_STATE',2) for p in [68]]:
 source=flags if kind==1 else variables
 value=re.search(r'#define '+name+r'\s+([^\n]+)',source)[1]
 query=base+int(re.search(r'SYSTEM_FLAGS \+ (0x[0-9A-Fa-f]+)',value)[1],0) if 'SYSTEM_FLAGS' in value else int(value.split()[0],0)
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames','18000','--screenshot',str(out/f'lavaridge-retry-{param}-end.png')]
 for f,n,v in [(59,'gEcHeadlessFixtureParam',param),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',kind),(70,'gEcHeadlessCampaignQueryId',query)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
 
 
 cmd+=['--write',f'9500:4:{sy["gEcHeadlessFixtureTrigger"]}:2','--key','10000:2:RIGHT','--key','10100:2:A']
 for frame in [8000,9900,11000,14000]:cmd+=['--screenshot-at',f'{frame}:{out}/lavaridge-retry-{param}-{frame}.png']
 for frame in range(1000,17500,100):cmd+=['--key',f'{frame}:2:'+'B']
 if '--pc-item' in sys.argv:cmd+=['--write',f'17550:4:{sy["gEcHeadlessCampaignQueryKind"]}:5','--write',f'17550:4:{sy["gEcHeadlessCampaignQueryId"]}:734']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignLastResolution','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:cmd+=['--read',f'4:{sy[n]}']
 result=subprocess.run(cmd,capture_output=True,text=True);(out/f'lavaridge-retry-{param}.log').write_text(result.stdout+result.stderr);print(param,name,result.returncode,result.stdout,result.stderr,flush=True)
