import sys,struct,subprocess,hashlib
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_southern import execute,sy,var,flag,out
print('headless',hashlib.sha256(open('pokeemerald-mauville-audit-headless.gba','rb').read()).hexdigest(),flush=True)
c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames','1000','--state-out',str(out/'circuit-lobby-base.mgba'),'--write',f'60:4:{sy["gEcHeadlessFixtureScenario"]}:11']
r=subprocess.run(c,capture_output=True,text=True,check=True);print(r.stdout,flush=True)
def seed(tag,licensed=True,party='healthy'):
 p=bytes([0x29 if licensed else 0x2a])+struct.pack('<H',0x86c)
 p+=flag('FLAG_EC_CHAMPIONS_CIRCUIT_EXPLAINED',False)+var(0x40db,0)+var(0x40dc,0)+var(0x40e3,0)+var(0x40e5,0)+bytes([0x6b,2])
 a=sy['gStringVar4']+800;c=sy['sGlobalScriptContext']
 w=[(50,1,a+i,v) for i,v in enumerate(p)]+[(50,1,c,0),(50,4,c+8,a),(50,1,c+1,1),(50,1,sy['sGlobalScriptContextStatus'],0),(50,4,sy['gEcHeadlessFixtureActiveScenario'],71),(50,4,sy['gEcHeadlessFixtureParam'],241),(50,4,sy['gEcHeadlessCampaignQueryKind'],2),(50,4,sy['gEcHeadlessCampaignQueryId'],0x40e5)]
 if party=='fainted':w.append((50,2,sy['gParties']+5*100+86,0))
 if party=='five':w.extend((50,4,sy['gParties']+5*100+i,0) for i in range(0,100,4))
 execute(tag+'-ready','circuit-lobby-base',500,writes=w)
 execute(tag,tag+'-ready',10500,[(200,2,'UP'),(400,2,'A')]+[(f,2,'B') for f in range(1400,10000,900)],shots=[1000,3000,5200,8000,10100])
if __name__=='__main__':
 for args in [('circuit-unlicensed',False,'healthy'),('circuit-fainted',True,'fainted'),('circuit-five',True,'five'),('circuit-entry-cancel',True,'healthy')]:seed(*args)
