from pathlib import Path
import sys,subprocess,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
def run(choice):
 tag='pacifidlog-trade-'+choice
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'pacifidlog-trade-choice.mgba'),'--frames','15000','--state-out',str(out/(tag+'.mgba')),'--screenshot',str(out/(tag+'-end.png'))]
 if choice=='accept':keys=[(300,2,'RIGHT'),(600,2,'DOWN'),(900,2,'DOWN'),(1200,2,'DOWN'),(1500,2,'A')]+[(f,2,'A') for f in range(2200,12200,500)]+[(f,2,'B') for f in range(12700,14700,300)]
 else:keys=[(300,2,'B' if choice=='cancel' else 'A')]+[(f,2,'B') for f in range(1000,4000,500)]
 for f,d,k in keys:c+=['--key',f'{f}:{d}:{k}']
 for f in [1400,2000,3200,5500,9000,12000]:c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gSpecialVar_0x8004','gSpecialVar_0x8005']:c+=['--read',f'{2 if n.startswith("gSpecial") else 4}:{sy[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/(tag+'.mgba')),'--frames','30','--write',f'1:4:{sy["gEcHeadlessCampaignQueryKind"]}:12','--write',f'1:4:{sy["gEcHeadlessCampaignQueryId"]}:4','--read',f'4:{sy["gEcHeadlessCampaignQueryValue"]}']
 r=subprocess.run(c,capture_output=True,text=True);print('slot4',choice,r.returncode,r.stdout,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(run,['cancel','wrong','accept']))
