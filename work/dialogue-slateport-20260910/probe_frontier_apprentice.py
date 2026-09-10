import sys,struct,subprocess,hashlib
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_southern import execute,sy,var,flag,out
print('headless sha256',hashlib.sha256(open('pokeemerald-mauville-audit-headless.gba','rb').read()).hexdigest(),flush=True)
c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames','1000','--state-out',str(out/'frontier-audit-base.mgba')]
for f,n,v in [(59,'gEcHeadlessFixtureParam',241),(60,'gEcHeadlessFixtureScenario',71)]:c+=['--write',f'{f}:4:{sy[n]}:{v}']
r=subprocess.run(c,capture_output=True,text=True,check=True);print(r.stdout,flush=True)
for daily in [False,True]:
 tag='frontier-apprentice-'+('departed' if daily else 'fresh')
 mid=sy['MAP_BATTLE_FRONTIER_BATTLE_TOWER_LOBBY']
 p=bytes([0x29 if daily else 0x2a])+struct.pack('<H',0x934)+flag('FLAG_HIDE_APPRENTICE',True)
 p+=bytes([0x39,mid>>8,mid&255,255])+struct.pack('<HH',1,6)+bytes([0x27,0x6b,2])
 a=sy['gStringVar4']+800;c=sy['sGlobalScriptContext']
 w=[(50,1,a+i,v) for i,v in enumerate(p)]+[(50,1,c,0),(50,4,c+8,a),(50,1,c+1,1),(50,1,sy['sGlobalScriptContextStatus'],0),(50,4,sy['gEcHeadlessCampaignQueryKind'],1),(50,4,sy['gEcHeadlessCampaignQueryId'],0x2bd)]
 execute(tag+'-ready','frontier-audit-base',1500,(),w)
 execute(tag,tag+'-ready',4500,[(200,2,'UP'),(400,2,'A'),(1400,2,'B'),(2400,2,'B'),(3400,2,'B')],shots=[1100,2100,3100,4200])
 execute(tag+'-repeat',tag,4500,[(200,2,'A'),(1400,2,'B'),(2400,2,'B'),(3400,2,'B')],shots=[1100])
