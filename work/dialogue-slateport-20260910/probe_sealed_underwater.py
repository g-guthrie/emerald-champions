from pathlib import Path
import sys,subprocess,struct,json
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'sealed-dived.mgba'),'--frames','14500','--state-out',str(out/'sealed-underwater-walked.mgba'),'--screenshot',str(out/'sealed-underwater-walked-end.png')]
keys=[(300,16,'DOWN')]+[(1000+i*150,16,k) for i,k in enumerate(json.loads((out/'sealed-underwater-path.json').read_text()))]+[(8700,2,'UP'),(9000,2,'A'),(10000,2,'B'),(10500,2,'B'),(11400,2,'A'),(12200,2,'A')]
for f,d,k in keys:c+=['--key',f'{f}:{d}:{k}']
for f in [800,4000,8400,9600,11100,13600]:c+=['--screenshot-at',f'{f}:{out}/sealed-underwater-walked-{f}.png']
for n in ['gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
r=subprocess.run(c,capture_output=True,text=True);print(r.returncode,r.stdout,r.stderr)
