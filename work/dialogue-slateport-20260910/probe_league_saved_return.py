from pathlib import Path
import sys,subprocess,shutil
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
for kind,ident,label in [(2,0x409c,'elite-state'),(1,0x4fb,'sidney'),(1,0x4fc,'phoebe'),(1,0x4fd,'glacia'),(1,0x4fe,'drake')]:
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'league-retired-lobby.mgba'),'--frames','30','--write',f'1:4:{sy["gEcHeadlessCampaignQueryKind"]}:{kind}','--write',f'1:4:{sy["gEcHeadlessCampaignQueryId"]}:{ident}','--read',f'4:{sy["gEcHeadlessCampaignQueryValue"]}']
 r=subprocess.run(c,capture_output=True,text=True);print(label,r.returncode,r.stdout,r.stderr,flush=True)
p=out/'league-reboot-scratch.sav';shutil.copyfile(out/'league-hof-record.sav',p)
c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--save',str(p),'--frames','2500','--state-out',str(out/'league-reboot-menu.mgba'),'--screenshot',str(out/'league-reboot-menu.png')]
for f,d,k in [(500,2,'A'),(900,2,'START'),(1500,2,'A')]:c+=['--key',f'{f}:{d}:{k}']
for f in [600,1100,1700,2300]:c+=['--screenshot-at',f'{f}:{out}/league-reboot-{f}.png']
for n in ['gSaveFileStatus','gSaveAttemptStatus']:
 if n in sy:c+=['--read',f'1:{sy[n]}']
c+=['--read',f'4:{sy["gDamagedSaveSectors"]}']
r=subprocess.run(c,capture_output=True,text=True);print('reboot',r.returncode,r.stdout,r.stderr,flush=True)
