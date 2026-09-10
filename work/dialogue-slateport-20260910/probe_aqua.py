from pathlib import Path
import subprocess,sys,re,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');s=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
flags=Path('include/constants/flags.h').read_text()
def flag(n):return eval(re.search(r'#define '+n+r'\s+([^\n/]+)',flags)[1].strip(),{'SYSTEM_FLAGS':0x860,'__builtins__':{}})
def probe(arg):
 p=int(arg);end=30000 if p==157 else 12000
 kind,q=(3,1) if p==163 else (1,flag('FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE' if p==162 else 'FLAG_MET_TEAM_AQUA_HARBOR'))
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(end),'--screenshot',f'{out}/aqua-{p}-end.png']
 for f,n,v in [(59,'gEcHeadlessFixtureParam',p),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',kind),(70,'gEcHeadlessCampaignQueryId',q)]:c+=['--write',f'{f}:4:{s[n]}:{v}']
 keys=[(300,16,'LEFT')] if p in range(158,162) else [(300,2,'RIGHT' if p==157 else 'LEFT' if p==162 else 'UP'),(400,2,'A')]
 keys += [(f,2,'B') for f in range(850,end-250,180)]
 if p==157:keys += [(12000,48,'LEFT')]
 for f,d,k in sorted(keys):c+=['--key',f'{f}:{d}:{k}']
 for f in [250,800,1400,2200,3200,4800,7500]:c+=['--screenshot-at',f'{f}:{out}/aqua-{p}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{s[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(p,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(probe,sys.argv[1:] or range(157,164)))
