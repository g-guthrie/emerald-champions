from pathlib import Path
import subprocess,sys,re,concurrent.futures
sys.path.insert(0,'scripts')
from native_tools import symbols
out=Path('work/dialogue-slateport-20260910')
sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
flags=Path('include/constants/flags.h').read_text()
def flag(n):
    value=re.search(r'#define '+n+r'\s+([^\n/]+)',flags)[1].strip()
    return eval(value,{'SYSTEM_FLAGS':0x860,'__builtins__':{}})
def probe(value):
    p=int(str(value).split(':')[0]);entry=str(value).endswith(':enter');end=65000 if entry else 10000;tag=str(p)+('enter' if entry else '')
    kind,qid={141:(1,flag('FLAG_RECEIVED_POKEBLOCK_CASE')),142:(2,0x4094),143:(1,flag('FLAG_COOL_PAINTING_MADE')),144:(1,flag('FLAG_RECEIVED_POKEBLOCK_CASE')),145:(2,0x408A),146:(4,50)}[p]
    if entry:kind,qid=2,0x4086
    cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(end),'--screenshot',str(out/f'interior-{tag}-end.png')]
    for f,n,v in [(59,'gEcHeadlessFixtureParam',p),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',kind),(70,'gEcHeadlessCampaignQueryId',qid)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
    keys=[] if p==142 else [(300,2,'UP'),(400,2,'A')]
    keys += ([(850,2,'B')]+[(f,2,'B') for f in range(4000,end-200,240)]) if p==143 else [(f,2,'B') for f in range(850,end-200,240)]
    if entry:
        keys=[(300,2,'UP'),(400,2,'A')]+[(f,2,'A') for f in range(850,end-200,200)]
        cmd+=['--until',f'4:{sy["gEcHeadlessCampaignQueryValue"]}:4294967295:2','--state-out',str(out/'contest-entry-end.state')]
    for f,d,k in keys:cmd+=['--key',f'{f}:{d}:{k}']
    for f in ([5000,10000,20000,30000,45000] if entry else [800,1400,2000,2600,3200,4000]):cmd+=['--screenshot-at',f'{f}:{out}/interior-{tag}-{f}.png']
    for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:cmd+=['--read',f'4:{sy[n]}']
    result=subprocess.run(cmd,capture_output=True,text=True)
    print(tag,result.returncode,result.stdout,result.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(probe,sys.argv[1:] or range(141,147)))
