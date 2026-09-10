import sys,struct,subprocess,concurrent.futures
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_homecoming import sy,var,flag,out

def probe(tag,choice=0,gate=True,faint=False,state='postgame-audit-base',reset=True):
 p=var(0x40d5,choice)
 if reset:
  for n in ['FLAG_CAUGHT_LATIAS_OR_LATIOS','FLAG_DEFEATED_LATIAS_OR_LATIOS','FLAG_ENCOUNTERED_LATIAS_OR_LATIOS']:p+=flag(n,False)
  for n in ['FLAG_HIDE_SOUTHERN_ISLAND_EON_STONE','FLAG_HIDE_SOUTHERN_ISLAND_UNCHOSEN_EON_DUO_MON']:p+=flag(n,True)
 p+=bytes([0x29 if gate else 0x2a])+struct.pack('<H',0x8b3)
 p+=bytes([0x39,26,10,255])+struct.pack('<HH',13,12)+bytes([0x27,0x6b,2])
 addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext']
 w=[(50,1,addr+i,v) for i,v in enumerate(p)]+[(50,1,ctx,0),(50,4,ctx+8,addr),(50,1,ctx+1,1),(50,1,sy['sGlobalScriptContextStatus'],0)]
 for n,v in [('gEcHeadlessFixtureParam',249 if faint else 242),('gEcHeadlessCampaignBattleSerial',0),('gEcHeadlessCampaignCaptureSerial',0),('gEcHeadlessCampaignQueryKind',1),('gEcHeadlessCampaignQueryId',0x1c9)]:w.append((50,4,sy[n],v))
 return execute(tag,state,12500,[(700,2,'UP'),(900,2,'A')]+[(f,2,'B') for f in range(1600,12000,280)],w,[1100,1700,2200,3000,4500,7500])

def execute(tag,state,frames,keys=(),writes=(),shots=()):
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/(state+'.mgba')),'--frames',str(frames),'--state-out',str(out/(tag+'.mgba')),'--screenshot',str(out/(tag+'-end.png'))]
 for f,w,a,v in writes:c+=['--write',f'{f}:{w}:{a}:{v}']
 for f,d,k in keys:c+=['--key',f'{f}:{d}:{k}']
 for f in shots:c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignCaptureSerial','gEcHeadlessCampaignLastCapturedSpecies','gEcHeadlessCampaignCaptureBookkeepingValid']:c+=['--read',f'4:{sy[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
 if r.returncode:raise RuntimeError(tag)
 return r.stdout
if __name__=='__main__':
 cases=[('southern-latios-capture',0,True,False),('southern-latias-capture',1,True,False),('southern-latios-faint',0,True,True),('southern-no-pass',0,False,False)]
 with concurrent.futures.ThreadPoolExecutor(max_workers=2) as ex:list(ex.map(lambda args:probe(*args),cases))
