import sys,struct,subprocess,re
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_southern import execute,sy,var,out

def inject(tag,state,p,keys=(),frames=1500,extra=(),shots=()):
 a=sy['gStringVar4']+800;c=sy['sGlobalScriptContext']
 w=[(50,1,a+i,v) for i,v in enumerate(p)]+[(50,1,c,0),(50,4,c+8,a),(50,1,c+1,1),(50,1,sy['sGlobalScriptContextStatus'],0)]+list(extra)
 return execute(tag,state,frames,keys,w,shots)
if __name__=='__main__':
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames','1000','--state-out',str(out/'circuit-full-storage-base.mgba')]
 for f,n,v in [(59,'gEcHeadlessFixtureParam',128),(60,'gEcHeadlessFixtureScenario',71)]:c+=['--write',f'{f}:4:{sy[n]}:{v}']
 r=subprocess.run(c,capture_output=True,text=True,check=True);print(r.stdout,flush=True)
 p=bytes([0x29])+struct.pack('<H',0x86c)+var(0x40dc,2)+var(0x40db,0)+var(0x40e5,0)
 p+=bytes([0x39,26,5,255])+struct.pack('<HH',23,6)+bytes([0x27,0x6b,2])
 inject('circuit-full-storage-ready','circuit-full-storage-base',p,extra=[(50,4,sy['gEcHeadlessCampaignQueryKind'],6),(50,4,sy['gEcHeadlessCampaignQueryId'],sy['SPECIES_CALYREX'])])
 execute('circuit-full-storage-blocked','circuit-full-storage-ready',4000,[(200,2,'UP'),(400,2,'A'),(1600,2,'B'),(2700,2,'B')],shots=[1200,2400,3700])
 # Scratch fixture only: remove one boxed resident to model making storage room.
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'circuit-full-storage-blocked.mgba'),'--frames','1','--read',f'4:{sy["gPokemonStoragePtr"]}']
 r=subprocess.run(c,capture_output=True,text=True,check=True);storage=int(re.search('value=([0-9a-f]+)',r.stdout)[1],16)
 execute('circuit-storage-recovery','circuit-full-storage-blocked',10000,[(300,2,'A')]+[(f,2,'B') for f in range(1400,9500,700)],writes=[(50,4,storage+4+i,0) for i in range(0,80,4)],shots=[1000,2300,4300,7300])
 execute('circuit-storage-repeat','circuit-storage-recovery',6000,[(300,2,'A')]+[(f,2,'B') for f in range(1400,5500,700)],shots=[1000,2500])
