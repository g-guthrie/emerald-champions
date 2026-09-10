from pathlib import Path
import sys,subprocess,struct,json
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'route133-current-crossed.mgba'),'--frames','2600','--state-out',str(out/'route133-nurse-reached.mgba'),'--screenshot',str(out/'route133-nurse-reached-end.png')]
keys=[(300,80,'LEFT'),(900,8,'DOWN'),(1200,8,'LEFT'),(1500,8,'LEFT'),(1800,2,'UP')]
for f,d,k in keys:c+=['--key',f'{f}:{d}:{k}']
for f in [800,1100,1700,2400]:c+=['--screenshot-at',f'{f}:{out}/route133-nurse-reached-{f}.png']
for n in ['gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
r=subprocess.run(c,capture_output=True,text=True);print(r.returncode,r.stdout,r.stderr)
