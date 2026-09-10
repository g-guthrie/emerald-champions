from pathlib import Path
import sys,subprocess,re
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True);flags=Path('include/constants/flags.h').read_text()
base=int(re.search(r'#define TRAINER_FLAGS_START\s+(0x[0-9A-Fa-f]+)',flags)[1],0)+int(re.search(r'#define MAX_TRAINERS_COUNT_EMERALD\s+(\d+)',Path('include/constants/opponents.h').read_text())[1])
for param in map(int,[x for x in sys.argv[1:] if x.isdigit()] or ['92','93','94','95','96','98','99','100']):
 retry='--retry' in sys.argv;end=22000 if retry else 12000;inputs=[]
 name='FLAG_BADGE06_GET' if param==96 else 'FLAG_KECLEON_FLED_FORTREE' if param==98 else 'FLAG_WINGULL_SENT_ON_ERRAND' if param==99 else 'FLAG_HIDE_ROUTE_120_STEVEN' if param==100 else 'FLAG_RECEIVED_DEVON_SCOPE'
 val=re.search(r'#define '+name+r'\s+([^\n]+)',flags)[1];q=base+int(re.search(r'SYSTEM_FLAGS \+ (0x[0-9A-Fa-f]+)',val)[1],0) if 'SYSTEM_FLAGS' in val else int(val.split()[0],0)
 if param!=100:
  inputs+=['--key',f'300:2:'+('LEFT' if param in [93,95] else 'UP'),'--key','400:2:A']
  if param in [92,93,94,95,98]:
   for f in range(800,2700,200):inputs+=['--key',f'{f}:2:A']
  for f in range(3000 if param<96 or param==98 else 900,end-300,150):inputs+=['--key',f'{f}:2:B']
 if retry:
  inputs+=['--write',f'13000:4:{sy["gEcHeadlessFixtureTrigger"]}:2','--key','14000:2:LEFT','--key','14100:2:A']
 tag='retry' if retry else 'normal'
 cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(end),'--screenshot',str(out/f'fortree-{param}-{tag}-end.png')]
 for f,n,v in [(59,'gEcHeadlessFixtureParam',param),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',1),(70,'gEcHeadlessCampaignQueryId',q)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
 cmd+=inputs
 for f in [1800,4200,8000,11500]:cmd+=['--screenshot-at',f'{f}:{out}/fortree-{param}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY','gEcHeadlessCampaignBattleSerial']:cmd+=['--read',f'4:{sy[n]}']
 r=subprocess.run(cmd,capture_output=True,text=True);print(param,tag,name,r.returncode,r.stdout,r.stderr,flush=True)
