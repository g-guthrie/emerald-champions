import sys,re,json,subprocess
from pathlib import Path
sys.path.insert(0,'scripts')
from native_tools import symbols
root=Path.cwd(); out=root/'work/dialogue-opening-20260910'; sy=symbols(root/'pokeemerald-opening-dialogue-headless.elf',root,first=True)
sc=re.findall(r'EC_HEADLESS_SCENARIO_\w+',Path('include/emerald_champions_headless.h').read_text().split('enum EmeraldChampionsHeadlessLeafState')[0]).index('EC_HEADLESS_SCENARIO_STORY_HANDOFF')
def wr(f,n,v):return ['--write',f'{f}:4:{sy[n]}:{v}']
for param in range(4,10):
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-opening-dialogue-headless.gba','--frames','5000','--state-out',str(out/f'birch-{param}.state'),'--screenshot',str(out/f'birch-{param}.png')]
 cmd+=wr(59,'gEcHeadlessFixtureParam',param)+wr(60,'gEcHeadlessFixtureScenario',sc)+wr(65,'gEcHeadlessCampaignQueryKind',2)+wr(65,'gEcHeadlessCampaignQueryId',0x4084)
 for frame in range(600,4600,100):cmd+=['--key',f'{frame}:2:A']
 names=['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled']
 for name in names:cmd+=['--read',f'4:{sy[name]}']
 p=subprocess.run(cmd,text=True,capture_output=True); (out/f'birch-{param}.log').write_text(p.stdout+p.stderr)
 print(param,p.returncode,p.stdout[-1300:],flush=True)
(out/'probe-symbols.json').write_text(json.dumps(sy))
