from pathlib import Path
import subprocess,sys,re,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');s=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
flags=Path('include/constants/flags.h').read_text();vs=Path('include/constants/vars.h').read_text()
def flag(n):return eval(re.search(r'#define '+n+r'\s+([^\n/]+)',flags)[1].strip(),{'SYSTEM_FLAGS':0x860,'__builtins__':{}})
def var(n):return int(re.search(r'#define '+n+r'\s+(0x\w+)',vs)[1],0)
def probe(arg):
 tag=str(arg).replace(':','-');options=str(arg).split(':')[1:];p=int(str(arg).split(':')[0]);entry='entry' in options;end=20000 if p in [168,170] else 12000
 kind,q=(1,flag('FLAG_BADGE07_GET')) if p==164 else (1,flag('FLAG_RECEIVED_HM_DIVE')) if p==171 else (2,var('VAR_MOSSDEEP_CITY_STATE' if p==173 else 'VAR_MOSSDEEP_SPACE_CENTER_STAIR_GUARD_STATE' if p in [165,166] else 'VAR_MOSSDEEP_SPACE_CENTER_STATE'))
 if 'species0' in options:kind,q=12,0
 if 'species1' in options:kind,q=12,1
 if 'guard' in options:kind,q=3,9
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(end),'--screenshot',f'{out}/mossdeep-{tag}-end.png']
 for f,n,v in [(59,'gEcHeadlessFixtureParam',p),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',kind),(70,'gEcHeadlessCampaignQueryId',q)]:c+=['--write',f'{f}:4:{s[n]}:{v}']
 keys=[]
 if p in [164,165]:keys=[(300,2,'UP'),(400,2,'A')]
 if p in [169,170]:keys=[(300,2,'LEFT'),(400,2,'A'),(1000,2,'A')]
 if p==170:keys += [(2000,2,'A'),(2200,2,'A'),(3000,2,'A'),(3400,2,'START'),(3600,2,'A')]
 if p==170 and 'second' in options:keys=[(300,2,'LEFT'),(400,2,'A'),(1000,2,'A'),(2000,2,'A'),(2600,2,'RIGHT'),(3000,2,'A'),(3400,2,'A'),(3800,2,'START'),(4200,2,'A')]
 if p==169 and 'cancel' in options:keys += [(2000,2,'A'),(2600,2,'B'),(3000,2,'A')]
 if p==168:keys=[(850,2,'A'),(1100,2,'A')]
 if p==172:keys=[(300,16,'UP')]
 if p==173:keys=[(300,16,'RIGHT')]
 keys += [(f,2,'B') for f in range(5000 if p==170 or 'cancel' in options else 2000 if p==169 else 1400 if p==168 else 850,end-250,180)]
 for f,d,k in sorted(keys):c+=['--key',f'{f}:{d}:{k}']
 for f in [800,1400,2400,3200,4800,7500]:c+=['--screenshot-at',f'{f}:{out}/mossdeep-{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{s[n]}']
 c+=['--read',f'1:{s["gPartiesCount"]}']
 c+=['--read',f'1:{s["gSelectedOrderFromParty"]}','--read',f'4:{s["gBattleTypeFlags"]}']
 if entry:c+=['--until',f'4:{s["gEcHeadlessCampaignBattleSerial"]}:4294967295:1']
 if 'guard' in options:
  for n in ['gEcHeadlessCampaignQueryObjectX','gEcHeadlessCampaignQueryObjectY']:c+=['--read',f'4:{s[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(probe,sys.argv[1:] or range(164,174)))
