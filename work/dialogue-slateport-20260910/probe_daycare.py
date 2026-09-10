from pathlib import Path
import sys,subprocess
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames','6000','--screenshot',str(out/'daycare-end.png')]
for f,n,v in [(59,'gEcHeadlessFixtureParam',34),(60,'gEcHeadlessFixtureScenario',71)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
cmd+=['--key','300:2:UP','--key','400:2:A']
for frame in range(800,3000,200):cmd+=['--key',f'{frame}:2:A']
for frame in range(3200,5800,200):cmd+=['--key',f'{frame}:2:B']
for frame in [1600,2400,3000,4000]:cmd+=['--screenshot-at',f'{frame}:{out}/daycare-{frame}.png']
for n in ['gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled']:cmd+=['--read',f'4:{sy[n]}']
cmd+=['--read',f'1:{sy["gPartiesCount"]}']
r=subprocess.run(cmd,capture_output=True,text=True);print(r.returncode,r.stdout,r.stderr)
