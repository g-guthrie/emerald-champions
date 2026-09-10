from pathlib import Path
import subprocess,sys,re,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');s=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
flags=Path('include/constants/flags.h').read_text()
def flag(n):return eval(re.search(r'#define '+n+r'\s+([^\n/]+)',flags)[1].strip(),{'SYSTEM_FLAGS':0x860,'__builtins__':{}})
def probe(arg):
 p=int(str(arg).split(':')[0]);opts=str(arg).split(':')[1:];tag=str(arg).replace(':','-');again='again' in opts;end=22000 if again else 15000
 kind,q=(1,flag('FLAG_SYS_SHOAL_TIDE')) if p in [174,184] else (1,flag('FLAG_EC_CAUGHT_ARTICUNO')) if p in [175,176] else (6,646) if p in [177,178] else (1,flag('FLAG_RECEIVED_SHOAL_SALT_3')) if p in [179,180] else (4,146 if p==181 else 147 if p==182 else 398)
 if p>=185:kind,q=3,3 if p in [185,189] else 2
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(end),'--rtc',str(946706400 if 'low' in opts else 946684800),'--screenshot',f'{out}/shoal-{tag}-end.png']
 writes=[(59,'gEcHeadlessFixtureParam',p),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',kind),(70,'gEcHeadlessCampaignQueryId',q)]
 if again:writes += [(13000,'gEcHeadlessFixtureTrigger',1)]
 for f,n,v in writes:cmd+=['--write',f'{f}:4:{s[n]}:{v}']
 keys=[(300,16,'UP')] if p in [174,184] else [(300,2,'RIGHT' if p in [179,180] else 'UP'),(500,2,'A')]
 if p==183 and 'early' in opts:keys=[(150,2,'UP'),(200,2,'A'),(240,2,'A'),(280,2,'A'),(320,2,'A')]
 if p==183 and 'left' in opts:keys=[(300,16,'UP'),(420,2,'LEFT'),(450,2,'A'),(550,2,'A'),(700,2,'A')]
 if again:keys += [(13500,2,'UP'),(14000,2,'A')]
 keys += [(f,2,'B') for f in range(1500 if p in [177,178] else 850,end-200,220) if not again or not 13000<f<14100]
 for f,d,k in sorted(keys):cmd+=['--key',f'{f}:{d}:{k}']
 for f in [750,1300,2400,4000,6000,9000]:cmd+=['--screenshot-at',f'{f}:{out}/shoal-{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignCaptureSerial','gEcHeadlessCampaignLastCapturedSpecies','gEcHeadlessCampaignCaptureBookkeepingValid','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:cmd+=['--read',f'4:{s[n]}']
 
 if p in [174,184]:
  for n in ['gLastUsedWarp','gLocalTime','gSaveBlock2Ptr']:
   cmd+=['--read',f'4:{s[n]}']
  cmd+=['--read',f'4:{s["gLocalTime"]+4}']
 r=subprocess.run(cmd,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(probe,sys.argv[1:] or ['174:low','174:high','175:again',176,177,178,179,180,181,182,183]))
