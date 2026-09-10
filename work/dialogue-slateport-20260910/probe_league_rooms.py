from pathlib import Path
import sys,subprocess
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
state='league-gate-front'
for index,name in enumerate(['sidney','phoebe','glacia','drake']):
 tag='league-'+name+'-cleared'
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/(state+'.mgba')),'--frames','12500','--state-out',str(out/(tag+'.mgba')),'--screenshot',str(out/(tag+'-end.png'))]
 for n,v in [('gEcHeadlessCampaignQueryKind',2),('gEcHeadlessCampaignQueryId',0x409c)]:c+=['--write',f'50:4:{sy[n]}:{v}']
 keys=[(700,16,'UP'),(1000,2,'A')]+[(f,2,'B') for f in range(1700,7400,500)]+[(7700,16,'RIGHT'),(8000,16,'UP'),(8300,16,'UP'),(8600,16,'UP'),(8900,16,'LEFT'),(9200,700 if name=='drake' else 280,'UP')]
 for f,d,k in keys:c+=['--key',f'{f}:{d}:{k}']
 for f in [1500,2400,3900,6500,7600,11200]:c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
 if r.returncode:raise SystemExit(r.returncode)
 state=tag
