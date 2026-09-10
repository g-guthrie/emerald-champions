from pathlib import Path
import sys,subprocess
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
state='route133-nurse-adjacent'
for choice in ['decline','accept']:
 tag='route133-vial-'+choice
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/(state+'.mgba')),'--frames','5500','--state-out',str(out/(tag+'.mgba')),'--screenshot',str(out/(tag+'-end.png'))]
 for n,v in [('gEcHeadlessCampaignQueryKind',2),('gEcHeadlessCampaignQueryId',0x409b)]:c+=['--write',f'50:4:{sy[n]}:{v}']
 keys=[(300,2,'A'),(1100,2,'B'),(1900,2,'B' if choice=='decline' else 'A'),(2600,2,'B'),(3400,2,'B'),(4200,2,'B')]
 for f,d,k in keys:c+=['--key',f'{f}:{d}:{k}']
 for f in [900,1700,2400,3200,4500]:c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
 for kind,q in [(2,0x4091),(1,0x4ea),(3,11),(3,12)]:
  c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/(tag+'.mgba')),'--frames','30','--write',f'1:4:{sy["gEcHeadlessCampaignQueryKind"]}:{kind}','--write',f'1:4:{sy["gEcHeadlessCampaignQueryId"]}:{q}','--read',f'4:{sy["gEcHeadlessCampaignQueryValue"]}']
  r=subprocess.run(c,capture_output=True,text=True);print('query',choice,kind,q,r.returncode,r.stdout,flush=True)
 state=tag
