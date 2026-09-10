from pathlib import Path
import sys,subprocess,re,concurrent.futures,json
sys.path.insert(0,'scripts');from native_tools import symbols
out=Path('work/dialogue-slateport-20260910');sy=symbols(Path('pokeemerald-mauville-audit-headless.elf'),Path.cwd(),first=True)
items=Path('include/constants/items.h').read_text();species=Path('include/constants/species.h').read_text()
def item(n):return int(re.search(r'\b'+n+r'\s*=\s*(\d+)',items)[1])
def sp(n):return int(re.search(r'\b'+n+r'\s*=\s*(\d+)',species)[1])
def probe(spec):
 param,kind,q,tag,expected=spec
 end=26000 if 'recovery' in tag else 12500 if param<=106 else 8000;cmd=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames',str(end),'--screenshot',str(out/f'economy-{tag}.png')]
 for f,n,v in [(59,'gEcHeadlessFixtureParam',param),(60,'gEcHeadlessFixtureScenario',71),(70,'gEcHeadlessCampaignQueryKind',kind),(70,'gEcHeadlessCampaignQueryId',q)]:cmd+=['--write',f'{f}:4:{sy[n]}:{v}']
 if 'recovery' in tag:
  cmd+=['--write',f'7000:4:{sy["gEcHeadlessFixtureTrigger"]}:2','--key','7400:2:UP','--key','7500:2:A','--write',f'16000:4:{sy["gEcHeadlessFixtureTrigger"]}:3','--key','16400:2:UP','--key','16500:2:A']
 if 'spend' in tag:
  for f in range(2300,4600,220):cmd+=['--key',f'{f}:2:A']
 if 'purchase' in tag:
  for f,key in [(700,'B'),(1000,'A'),(1450,'A'),(1850,'A'),(2250,'A'),(2600,'B'),(3000,'B'),(3400,'B'),(3800,'B'),(4200,'B'),(4600,'B'),(5000,'B')]:
   if param==108 and f==2600:key='A'
   cmd+=['--key',f'{f}:2:{key}']
 if param!=112:
  d='LEFT' if param>=113 else 'UP' if param<=106 else 'DOWN' if param<=108 else 'RIGHT'
  cmd+=['--key',f'300:2:{d}','--key','400:2:A']
  for f in range(850,end-300,140):
   if 'purchase' in tag:continue
   if 109<=param<=111 and 1300<=f<=1600:continue
   if param>=113 and 1100<=f<=2000:continue
   if 'spend' in tag and 2200<=f<=4900:continue
   cmd+=['--key',f'{f}:2:B']
  if 109<=param<=111:
   cmd+=['--key','1450:2:A']
  if param>=113:cmd+=['--key','1150:2:A','--key','1450:2:A','--key','1850:2:A']
 for f in ([850,1250,1800,2400,3600] if 'purchase' in tag else [1400,3600]):cmd+=['--screenshot-at',f'{f}:{out}/economy-{tag}-{f}.png']
 names=['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial']
 for n in names:cmd+=['--read',f'4:{sy[n]}']
 r=subprocess.run(cmd,capture_output=True,text=True)
 print(tag,'expected',expected,r.returncode,r.stdout.strip(),r.stderr.strip(),flush=True)
 return {'tag':tag,'expected':expected,'code':r.returncode,'stdout':r.stdout,'stderr':r.stderr}
specs=[(101,4,item('ITEM_LINKING_CORD'),'soot99',0),(102,4,item('ITEM_LINKING_CORD'),'soot100',1),(103,6,sp('SPECIES_MARSHADOW'),'soot250-unlock',1),(104,4,item('ITEM_HOUNDOOMINITE'),'soot500',1),(104,2,0x40B8,'soot500-progress',33268),(105,5,item('ITEM_LINKING_CORD'),'soot100-pc',1),(106,6,sp('SPECIES_MARSHADOW'),'soot-full',0),(107,8,item('ITEM_FIRE_STONE'),'stone-price',3000),(108,8,item('ITEM_LINKING_CORD'),'cord-price',10000),(109,4,item('ITEM_GLALITITE'),'shoal-first',1),(110,4,item('ITEM_BIG_PEARL'),'shoal-repeat',1),(111,4,item('ITEM_SHOAL_SALT'),'shoal-full',4),(112,10,0,'pp-retained',228)]
specs += [(113,11,0,'bp-cord-buy',92),(114,11,0,'bp-cord-already-pc',100)]
specs += [(107,9,0,'purchase-stone-money',47000),(107,4,item('ITEM_DAWN_STONE'),'purchase-stone-item',1),(108,9,0,'purchase-tool-money',45000)]
specs += [(106,5,item('ITEM_HOUNDOOMINITE'),'soot-recovery',1),(104,2,0x4048,'soot-spend-balance',0),(104,2,0x40B8,'soot-spend-lifetime',33268)]
for n,e in [('ITEM_CHOICE_BAND',0),('ITEM_SITRUS_BERRY',0),('ITEM_RAZZ_BERRY',5),('ITEM_FIRE_STONE',750),('ITEM_NUGGET',5000)]:specs.append((112,7,item(n),'sale-'+n,e))
if len(sys.argv)>1:specs=[x for x in specs if any(k in x[3] for k in sys.argv[1:])]
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:result=list(ex.map(probe,specs))
(out/('economy-native-results-'+('-'.join(sys.argv[1:]) or 'all')+'.json')).write_text(json.dumps(result,indent=2)+'\n')
