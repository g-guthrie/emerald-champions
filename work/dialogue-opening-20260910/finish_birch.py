import json,subprocess
from pathlib import Path
out=Path('work/dialogue-opening-20260910'); sy=json.loads((out/'probe-symbols.json').read_text())
def wr(f,n,v):return ['--write',f'{f}:4:{sy[n]}:{v}']
def run(p,retry=False):
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-opening-dialogue-headless.gba','--state-in',str(out/f'birch-{p}.state'),'--frames','4000','--until',f'4:{sy["gEcHeadlessCampaignControlsLocked"]}:4294967295:0','--state-out',str(out/f'birch-{p}-idle.state'),'--screenshot',str(out/f'birch-{p}-idle.png')]
 for f in range(50,3900,100):cmd+=['--key',f'{f}:2:B']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled']:cmd+=['--read',f'4:{sy[n]}']
 r=subprocess.run(cmd,capture_output=True,text=True);print(p,r.stdout,flush=True)
for p in range(4,10):run(p)
