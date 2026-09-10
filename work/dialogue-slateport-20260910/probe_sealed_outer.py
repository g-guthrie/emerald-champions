from pathlib import Path
import sys,subprocess,struct,json
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'sealed-underwater-walked.mgba'),'--frames','11500','--state-out',str(out/'sealed-inner-arrived.mgba'),'--screenshot',str(out/'sealed-inner-arrived-end.png')]
keys=[(300+i*150,16,k) for i,k in enumerate(json.loads((out/'sealed-outer-path.json').read_text()))]+[(4200,2,'UP'),(4400,2,'A'),(5100,2,'B'),(5500,2,'START'),(5800,2,'A'),(6200,2,'A'),(6600,2,'DOWN'),(6900,2,'A'),(7900,2,'A'),(9200,16,'UP'),(9600,16,'UP')]
for f,d,k in keys:c+=['--key',f'{f}:{d}:{k}']
for f in [4100,4900,7600,8900,10900]:c+=['--screenshot-at',f'{f}:{out}/sealed-inner-arrived-{f}.png']
for n in ['gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
r=subprocess.run(c,capture_output=True,text=True);print(r.returncode,r.stdout,r.stderr)
