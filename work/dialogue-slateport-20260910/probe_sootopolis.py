from pathlib import Path
import subprocess,sys,re,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');s=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
flags=Path('include/constants/flags.h').read_text();vs=Path('include/constants/vars.h').read_text()
def flag(n):return eval(re.search(r'#define '+n+r'\s+([^\n/]+)',flags)[1].strip(),{'SYSTEM_FLAGS':0x860,'DAILY_FLAGS_START':0x920,'__builtins__':{}})
def var(n):return int(re.search(r'#define '+n+r'\s+(0x\w+)',vs)[1],0)
def probe(arg):
 opts=str(arg).split(':')[1:];p=int(str(arg).split(':')[0]);tag=str(arg).replace(':','-');end=24000 if p<=224 else 15000
 kind,q=2,var('VAR_SOOTOPOLIS_CITY_STATE')
 if p in [215,216]:kind,q=1,flag('FLAG_STEVEN_GUIDES_TO_CAVE_OF_ORIGIN')
 if p==223:kind,q=1,flag('FLAG_RECEIVED_HM_WATERFALL')
 if p==224:kind,q=1,flag('FLAG_BADGE08_GET')
 if 225<=p<=228:kind,q=2,var('VAR_SEEDOT_SIZE_RECORD' if p<=226 else 'VAR_LOTAD_SIZE_RECORD')
 if p>=229:kind,q=1,flag('FLAG_DAILY_SOOTOPOLIS_RECEIVED_BERRY')
 if 'item' in opts:kind,q=4,51
 if any(o.startswith('qitem=') for o in opts):kind,q=4,int(next(o for o in opts if o.startswith('qitem=')).split('=')[1])
 if p==222:kind,q=1,flag('FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE')
 if p in [220,221]:kind,q=1,flag('FLAG_SYS_WEATHER_CTRL')
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(end),'--screenshot',f'{out}/sootopolis-{tag}-end.png']
 for f,n,v in [(59,'gEcHeadlessFixtureParam',p),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',kind),(70,'gEcHeadlessCampaignQueryId',q)]:c+=['--write',f'{f}:4:{s[n]}:{v}']
 keys=[]
 if p in [215,216,217,222,223,224,225,226,227,228,229,230]:keys=[(300,2,'LEFT' if p==215 else 'UP'),(500,2,'A')]
 if p==219:keys=[(300,16,'UP')]
 keys += [(f,2,'B') for f in range(900,end-200,220) if not (p==217 and 7600<f<9400) and not (225<=p<=228 and 3000<f<7200) and not(p==222 and 7400<f<8600)]
 if p==217:keys += [(8000,2,'DOWN'),(8300,2,'DOWN'),(8600,2,'A')]
 if p==222:keys += [(7700,16,'RIGHT'),(8100,2,'UP'),(8400,2,'A')]
 if 225<=p<=228:keys += [(4800,2,'A'),(5100,2,'A')]
 if p>=229:keys=[(300,16,'UP'),(500,2,'LEFT'),(650,2,'A')]+[(f,2,'B') for f in range(1000,end-200,220)]
 if 'a' in opts:keys=[(300,2,'UP')]+[(f,2,'A' if f<11000 else 'B') for f in range(500,end-200,240)]
 if 'early' in opts:keys=[(100,2,'UP'),(150,2,'A')]+[(f,2,'B') for f in range(600,end-200,220)]
 if 'inspect' in opts:keys=[(300,2,'UP')]+[(f,2,'A') for f in range(500,3500,180)]
 for f,d,k in sorted(keys):c+=['--key',f'{f}:{d}:{k}']
 for f in [750,1800,3200,4800,6500,9000,12500]+([18000] if end>18000 else []):c+=['--screenshot-at',f'{f}:{out}/sootopolis-{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY','gSpecialVar_0x8008','gSpecialVar_0x8009']:
  c+=['--read',f'{2 if n.startswith("gSpecial") else 4}:{s[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(probe,sys.argv[1:] or range(213,231)))
