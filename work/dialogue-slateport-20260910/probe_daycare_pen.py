from pathlib import Path
import sys,subprocess
sys.path.insert(0,'scripts');from native_tools import symbols
sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
for p in [37]:
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames','600','--screenshot','work/dialogue-slateport-20260910/daycare-pen.png']
 for f,n,v in [(59,'gEcHeadlessFixtureParam',p),(60,'gEcHeadlessFixtureScenario',71)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
 for n in ['gSpecialVar_0x8005','gSpecialVar_0x8006']:cmd+=['--read',f'2:{sy[n]}']
 r=subprocess.run(cmd,capture_output=True,text=True);print(p,r.returncode,r.stdout,r.stderr,flush=True)
