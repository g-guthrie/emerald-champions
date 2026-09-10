from pathlib import Path
import sys,subprocess
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
mode=sys.argv[1] if len(sys.argv)>1 else 'repeat';end=4000 if mode=='first' else 5300 if mode=='expired' else 10000
cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(end),'--screenshot',str(out/f'weather-expiry-{mode}-end.png')]
for f,n,v in [(59,'gEcHeadlessFixtureParam',91),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',2),(70,'gEcHeadlessCampaignQueryId',0x4037)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
cmd+=['--key','300:2:UP','--key','400:2:A']
if mode!='first':cmd+=['--write',f'4500:4:{sy["gEcHeadlessFixtureTrigger"]}:1','--key','4800:16:RIGHT','--key','5100:16:LEFT']
if mode=='repeat':cmd+=['--key','5500:2:UP','--key','5600:2:A','--screenshot-at',f'6100:{out}/weather-expiry-report.png']
for f in range(900,end-200,150):cmd+=['--key',f'{f}:2:B']
for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:cmd+=['--read',f'4:{sy[n]}']
r=subprocess.run(cmd,capture_output=True,text=True);print(mode,r.returncode,r.stdout,r.stderr,flush=True)
