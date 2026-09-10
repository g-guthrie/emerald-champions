import sys,struct,subprocess
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_homecoming import run,sy,var,flag,out
c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--frames','1000','--state-out',str(out/'postgame-audit-base.mgba')]
for f,n,v in [(59,'gEcHeadlessFixtureParam',241),(60,'gEcHeadlessFixtureScenario',71)]:c+=['--write',f'{f}:4:{sy[n]}:{v}']
r=subprocess.run(c,capture_output=True,text=True);print('fresh-base',r.returncode,r.stdout,r.stderr,flush=True)
if r.returncode:raise SystemExit(r.returncode)
for name,met,ship in [('new-invite',False,False),('already-met',True,False),('ship-clears-pending',False,True)]:
 p=b''.join(var(n,v) for n,v in [(0x4084,5),(0x4085,7),(0x40e2,4),(0x40d3,1),(0x40d4,0),(0x40b4,0)])
 p+=flag('FLAG_MET_SCOTT_ON_SS_TIDAL',met)+flag('FLAG_SCOTT_CALL_BATTLE_FRONTIER',True)+flag('FLAG_HIDE_SS_TIDAL_CORRIDOR_SCOTT',False)+bytes([0x29])+struct.pack('<H',0x864)
 p+=bytes([0x39,25 if ship else 1,41 if ship else 4,255])+struct.pack('<HH',1 if ship else 6,10 if ship else 5)+bytes([0x27,0x6b,2])
 addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext'];w=[(50,1,addr+i,v) for i,v in enumerate(p)]
 w +=[(50,1,ctx,0),(50,4,ctx+8,addr),(50,1,ctx+1,1),(50,1,sy['sGlobalScriptContextStatus'],0),(50,4,sy['gEcHeadlessCampaignQueryKind'],1),(50,4,sy['gEcHeadlessCampaignQueryId'],0x72)]
 run('postgame-scott-'+name,'postgame-audit-base',22000,[(f,2,'B') for f in range(1000,20000,450)],w,[1500,4000,7000,12000,18000])
