import json,subprocess,re
from pathlib import Path
o=Path('work/dialogue-opening-20260910');s=json.loads((o/'probe-symbols.json').read_text())
def wr(f,n,v):return ['--write',f'{f}:4:{s[n]}:{v}']
for p in range(4,10):
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-opening-dialogue-headless.gba','--state-in',str(o/f'birch-{p}-idle.state'),'--frames','3500','--screenshot',str(o/f'birch-{p}-verified.png')]
 if p in (6,9):cmd+=wr(10,'gEcHeadlessFixtureTrigger',2)+['--key','250:2:UP','--key','300:2:A']
 for f in range(400,2200,100):cmd+=['--key',f'{f}:2:B']
 cmd+=wr(2300,'gEcHeadlessCampaignQueryKind',1)+wr(2300,'gEcHeadlessCampaignQueryId',0x74)
 # Sample adventure flag by writing its telemetry into a screenshot-on-change log; then query gift count.
 cmd+=['--screenshot-on-change',f'4:{s["gEcHeadlessCampaignQueryValue"]}:2:{o}/query-{p}']
 cmd+=wr(2500,'gEcHeadlessCampaignQueryKind',4 if p in (4,7) else 5)+wr(2500,'gEcHeadlessCampaignQueryId',2)
 # Another conversation must not award another stack.
 cmd+=['--key','2600:2:A','--key','2800:2:B','--key','3000:2:B','--key','3200:2:B']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled']:cmd+=['--read',f'4:{s[n]}']
 r=subprocess.run(cmd,capture_output=True,text=True);(o/f'birch-{p}-verified.log').write_text(r.stdout+r.stderr)
 vals=re.findall(r'READ.*value=([0-9a-f]+)',r.stdout)
 print(p,vals,flush=True)
 assert [int(v,16) for v in vals]==[10,0,0],r.stdout+r.stderr
