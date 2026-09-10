import sys,struct,subprocess,re
sys.path.insert(0,'work/dialogue-slateport-20260910')
from probe_southern import execute,sy,var,flag,out
from probe_circuit_storage import inject
r=subprocess.run(['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'circuit-lobby-base.mgba'),'--frames','1','--read',f'4:{sy["gSaveBlock1Ptr"]}'],capture_output=True,text=True,check=True)
sb=int(re.search('value=([0-9a-f]+)',r.stdout)[1],16)
p=flag('FLAG_DEFEATED_SUDOWOODO',False)+flag('FLAG_HIDE_BATTLE_FRONTIER_SUDOWOODO',False)+flag('FLAG_RECEIVED_HM_SURF',True)
for f in range(0x867,0x86f):p+=bytes([0x29])+struct.pack('<H',f)
p+=bytes([0x44])+struct.pack('<HH',sy['ITEM_WAILMER_PAIL'],1)
mid=sy['MAP_BATTLE_FRONTIER_OUTSIDE_EAST'];p+=bytes([0x39,mid>>8,mid&255,255])+struct.pack('<HH',54,61)+bytes([0x27,0x6b,2])
extra=[(50,2,sb+1174,sy['ITEM_WAILMER_PAIL']),(50,4,sy['gEcHeadlessFixtureActiveScenario'],71),(50,4,sy['gEcHeadlessFixtureParam'],241),(50,4,sy['gEcHeadlessCampaignQueryKind'],1),(50,4,sy['gEcHeadlessCampaignQueryId'],0x1c6)]
inject('frontier-sudowoodo-ready','circuit-lobby-base',p,extra=extra)
execute('frontier-sudowoodo-shake','frontier-sudowoodo-ready',2300,[(200,2,'DOWN'),(400,2,'A')],shots=[600,900,1700])
execute('frontier-sudowoodo-water','frontier-sudowoodo-shake',9500,[(200,2,'SELECT')]+[(f,2,'B') for f in range(2000,9000,650)],shots=[900,2100,3800,5600,8200])
