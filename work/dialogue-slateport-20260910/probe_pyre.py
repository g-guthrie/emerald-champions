from pathlib import Path
import subprocess,sys,re,concurrent.futures
sys.path.insert(0,'scripts')
from native_tools import symbols
out=Path('work/dialogue-slateport-20260910')
sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
flags=Path('include/constants/flags.h').read_text()
def flag(n):
    return eval(re.search(r'#define '+n+r'\s+([^\n/]+)',flags)[1].strip(),{'SYSTEM_FLAGS':0x860,'__builtins__':{}})
def probe(value):
    p=int(str(value).split(':')[0]);again=str(value).endswith((':recover',':again'));end=25000 if again else 14000;tag=str(value).replace(':','-')
    kind,qid=(1,flag('FLAG_RECEIVED_RED_OR_BLUE_ORB')) if p<=151 else (3,9) if p==152 else (1,flag('FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT'))
    if p in [154,155]:kind,qid=2,0x40b0
    if str(value).endswith(':pc'):kind,qid=5,744
    cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(end),'--screenshot',str(out/f'pyre-{tag}-end.png')]
    writes=[(59,'gEcHeadlessFixtureParam',p),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',kind),(70,'gEcHeadlessCampaignQueryId',qid)]
    if again:writes += [(15000,'gEcHeadlessFixtureTrigger',2 if p==151 else 1)]
    for f,n,v in writes:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
    keys=[(300,16,'UP')] if p<=151 else [(300,2,'RIGHT' if p<=153 or p==156 else 'UP'),(400,2,'A')]
    if str(value).endswith(':hint'):keys=[(300,2,'DOWN'),(600,16,'UP'),(900,2,'A')]
    keys += [(f,2,'B') for f in range(6500 if str(value).endswith(':hint') else 850,end-200,180) if not again or not 15000<f<16100]
    if again:keys += [(15600,2,'UP'),(15900,2,'A')]
    for f,d,k in sorted(keys):cmd+=['--key',f'{f}:{d}:{k}']
    for f in ([800,1600,2600,4000,5600,7400,9500] + ([16400,17400] if again else [])):cmd+=['--screenshot-at',f'{f}:{out}/pyre-{tag}-{f}.png']
    names=['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignCaptureSerial','gEcHeadlessCampaignLastCapturedSpecies','gEcHeadlessCampaignCaptureBookkeepingValid','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']
    for n in names:cmd+=['--read',f'4:{sy[n]}']
    r=subprocess.run(cmd,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(probe,sys.argv[1:] or range(147,156)))
