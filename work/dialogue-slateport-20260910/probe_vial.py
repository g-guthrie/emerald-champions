from pathlib import Path
import sys,subprocess,re
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
var=int(re.search(r'#define VAR_CHANSEY_NURSE_STATE\s+(0x[\da-fA-F]+)',Path('include/constants/vars.h').read_text())[1],0)
params=[int(p) for p in sys.argv[1:]] or list(range(49,60))
for param in params:
 direction={49:'UP',50:'UP',51:'RIGHT',52:'UP',53:'LEFT',54:'DOWN',55:'LEFT',56:'DOWN',57:'UP',58:'UP',59:'LEFT'}[param]
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames','5000','--screenshot',str(out/f'vial-{param}-end.png')]
 for f,n,v in [(59,'gEcHeadlessFixtureParam',param),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',2),(70,'gEcHeadlessCampaignQueryId',var)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
 cmd+=['--key',f'300:{16 if param in [49,50,55,56] else 2}:{direction}']
 if param not in [49,50,55,56]:cmd+=['--key','400:2:A']
 for frame in [750,1200,1800,2400]:cmd+=['--screenshot-at',f'{frame}:{out}/vial-{param}-{frame}.png']
 for frame in range(900,4500,100):cmd+=['--key',f'{frame}:2:B']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:cmd+=['--read',f'4:{sy[n]}']
 r=subprocess.run(cmd,capture_output=True,text=True);(out/f'vial-{param}.log').write_text(r.stdout+r.stderr);print(param,r.returncode,r.stdout,r.stderr,flush=True)
