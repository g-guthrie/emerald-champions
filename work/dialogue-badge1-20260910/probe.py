from pathlib import Path
import sys,subprocess,json,re
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-badge1-20260910');sy=symbols(Path('pokeemerald-badge1-dialogue-headless.elf'),Path.cwd(),first=True);(out/'symbols.json').write_text(json.dumps(sy))
sc=re.findall(r'EC_HEADLESS_SCENARIO_\w+',Path('include/emerald_champions_headless.h').read_text().split('enum EmeraldChampionsHeadlessLeafState')[0]).index('EC_HEADLESS_SCENARIO_STORY_HANDOFF')
const=Path('include/constants/vars.h').read_text();var=lambda n:int(re.search(r'#define '+n+r'\s+(0x[0-9A-Fa-f]+)',const)[1],16)
for param in range(10,17):
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-badge1-dialogue-headless.gba','--frames',str(18000 if param==16 else 4800),'--screenshot',str(out/f'scene-{param}.png'),'--state-out',str(out/f'scene-{param}.state')]
 for f,n,v in [(59,'gEcHeadlessFixtureParam',param),(60,'gEcHeadlessFixtureScenario',sc),(70,'gEcHeadlessCampaignQueryKind',2),(70,'gEcHeadlessCampaignQueryId',var('VAR_SCOTT_PETALBURG_ENCOUNTER' if param<14 else 'VAR_PETALBURG_GYM_STATE'))]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
 cmd+=['--key',f'300:2:{"UP" if param==16 else "LEFT"}']
 if param>=14:cmd+=['--key','400:2:A']
 cmd+=['--screenshot-at',f'700:{out}/scene-{param}-text.png']
 for f in range(900,17000 if param==16 else 3900,100):cmd+=['--key',f'{f}:2:B']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:
  cmd+=['--read',f'4:{sy[n]}']
 r=subprocess.run(cmd,capture_output=True,text=True);(out/f'scene-{param}.log').write_text(r.stdout+r.stderr);print(param,r.stdout,flush=True)
