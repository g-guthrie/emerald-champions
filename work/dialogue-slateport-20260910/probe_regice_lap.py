from pathlib import Path
import re,sys,subprocess,struct,concurrent.futures
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
s=(root/'src/braille_puzzles.c').read_text();block=s.split('sRegicePathCoords[][2] =')[1].split('};')[0];cells={tuple(map(int,p)) for p in re.findall(r'\{(\d+),\s*(\d+)\}',block)};path=[(8,21),(7,21)]
while path[-1]!=(8,21):
 a=path[-1];ns=[b for b in cells if abs(a[0]-b[0])+abs(a[1]-b[1])==1 and b!=path[-2]];assert len(ns)==1;path.append(ns[0])
assert len(path)==37 and set(path)==cells
(out/'regice-perimeter-path.json').write_text(__import__('json').dumps(path)+'\n')
def run(tag,invalid):
 program=bytes([0x2a])+struct.pack('<H',0x8b1)+bytes([0x39,24,67,255])+struct.pack('<HH',8,21)+bytes([0x27,0x6b,2]);addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext']
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'ice-upstairs.mgba'),'--frames','9000','--screenshot',str(out/(tag+'-end.png')),'--state-out',str(out/(tag+'.mgba'))]
 for j,v in enumerate(program):c+=['--write',f'50:1:{addr+j}:{v}']
 for width,ptr,val in [(1,ctx,0),(4,ctx+8,addr),(1,ctx+1,1),(1,sy['sGlobalScriptContextStatus'],0),(4,sy['gEcHeadlessCampaignQueryKind'],1),(4,sy['gEcHeadlessCampaignQueryId'],0x8b1)]:c+=['--write',f'50:{width}:{ptr}:{val}']
 keys=[(700,2,'UP'),(900,2,'A'),(1400,2,'B')];ds={(0,-1):'UP',(0,1):'DOWN',(-1,0):'LEFT',(1,0):'RIGHT'}
 if invalid:keys += [(1800,16,'DOWN'),(2050,16,'UP'),(2400,2,'A'),(2900,2,'B')]
 start=3400 if invalid else 1800
 keys += [(start+i*110,16,ds[(b[0]-a[0],b[1]-a[1])]) for i,(a,b) in enumerate(zip(path,path[1:]))]
 for f,d,k in keys:c+=['--key',f'{f}:{d}:{k}']
 for f in [1200,2200,3000,7800]:c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY']:c+=['--read',f'4:{sy[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(lambda p:run(*p),[('regice-lap',False),('regice-restart',True)]))
