from pathlib import Path
import sys,subprocess,struct,concurrent.futures,re
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
ids=re.findall(r'^\s+(LEGENDARY_SIGN_\w+),',Path('include/constants/legendary_signs.h').read_text(),re.M);id=ids.index('LEGENDARY_SIGN_VICTINI');print('Victini ID',id,flush=True)
c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames','1000','--state-out',str(out/'victory-audit-base.mgba')]
for f,n,v in [(59,'gEcHeadlessFixtureParam',241),(60,'gEcHeadlessFixtureScenario',71)]:c+=['--write',f'{f}:4:{sy[n]}:{v}']
print('base',subprocess.run(c,capture_output=True,text=True).stdout,flush=True)
def run(case):
 guide,caught=case;tag=f'victini-{"guide" if guide else "edgar"}-{"caught" if caught else "waiting"}'
 program=bytes([0x16])+struct.pack('<HH',0x40fc+id//16,(1<<(id%16)) if caught else 0)+bytes([0x29])+struct.pack('<H',0x500+79)
 program+=bytes([0x39,16 if guide else 24,12 if guide else 43,255])+struct.pack('<HH',14 if guide else 33,5 if guide else 23)+bytes([0x27,0x6b,2]);addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext']
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'victory-audit-base.mgba'),'--frames','13500','--screenshot',str(out/(tag+'-end.png'))]
 for j,v in enumerate(program):c+=['--write',f'50:1:{addr+j}:{v}']
 for w,a,v in [(1,ctx,0),(4,ctx+8,addr),(1,ctx+1,1),(1,sy['sGlobalScriptContextStatus'],0)]:c+=['--write',f'50:{w}:{a}:{v}']
 for f,d,k in [(700,2,'UP'),(900,2,'A')]+[(f,2,'B') for f in range(1700,13100,650)]:c+=['--key',f'{f}:{d}:{k}']
 for f in [1500,2200,2800,3400,4000]:c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial']:c+=['--read',f'4:{sy[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(run,[(True,False),(True,True)]))
