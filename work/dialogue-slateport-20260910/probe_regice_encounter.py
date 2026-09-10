from pathlib import Path
import sys,subprocess,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
def run(tag,faint):
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'regice-lap.mgba'),'--frames','12000','--state-out',str(out/(tag+'.mgba')),'--screenshot',str(out/(tag+'-end.png'))]
 for n,v in [('gEcHeadlessCampaignQueryId',0x1bc),('gEcHeadlessCampaignCaptureSerial',0),('gEcHeadlessCampaignBattleSerial',0),('gEcHeadlessFixtureParam',249 if faint else 242)]:c+=['--write',f'50:4:{sy[n]}:{v}']
 for f,d,k in [(300+i*110,16,'UP') for i in range(13)]+[(2200,2,'A')]+[(f,2,'B') for f in range(2700,11500,220)]:c+=['--key',f'{f}:{d}:{k}']
 for f in [2100,2600,4500,8000]:c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignCaptureSerial','gEcHeadlessCampaignLastCapturedSpecies','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(lambda p:run(*p),[('regice-capture',False),('regice-faint',True)]))
