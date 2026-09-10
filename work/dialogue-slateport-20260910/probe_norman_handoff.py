from pathlib import Path
import sys,subprocess,re
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
flags=Path('include/constants/flags.h').read_text();variables=Path('include/constants/vars.h').read_text()
base=int(re.search(r'#define TRAINER_FLAGS_START\s+(0x[0-9A-Fa-f]+)',flags)[1],0)+int(re.search(r'#define MAX_TRAINERS_COUNT_EMERALD\s+(\d+)',Path('include/constants/opponents.h').read_text())[1])
for param,name,kind in [(p,'FLAG_RECEIVED_HM_SURF',1) for p in [73,74,75]]:
 source=flags if kind==1 else variables
 value=re.search(r'#define '+name+r'\s+([^\n]+)',source)[1]
 query=base+int(re.search(r'SYSTEM_FLAGS \+ (0x[0-9A-Fa-f]+)',value)[1],0) if 'SYSTEM_FLAGS' in value else int(value.split()[0],0)
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames','24000','--screenshot',str(out/f'norman-handoff-{param}-end.png')]
 for f,n,v in [(59,'gEcHeadlessFixtureParam',param),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',kind),(70,'gEcHeadlessCampaignQueryId',query)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
 cmd+=['--key','300:2:'+('UP' if param==73 else 'RIGHT' if param==74 else 'LEFT')]
 if param>=62:cmd+=['--key','400:2:A']
 for frame in [750,4000,6000,8000,10000,14000]:cmd+=['--screenshot-at',f'{frame}:{out}/norman-handoff-{param}-{frame}.png']
 for frame in range(1000,23500,100):cmd+=['--key',f'{frame}:2:'+'B']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignLastResolution','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:cmd+=['--read',f'4:{sy[n]}']
 result=subprocess.run(cmd,capture_output=True,text=True);(out/f'norman-handoff-{param}.log').write_text(result.stdout+result.stderr);print(param,name,result.returncode,result.stdout,flush=True)
