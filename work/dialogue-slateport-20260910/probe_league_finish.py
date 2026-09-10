from pathlib import Path
import sys,subprocess
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
def run(tag,state,frames,keys=(),writes=(),save=False,shots=()):
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/(state+'.mgba')),'--frames',str(frames),'--state-out',str(out/(tag+'.mgba')),'--screenshot',str(out/(tag+'-end.png'))]
 if save:
  p=out/(tag+'-scratch.sav');p.write_bytes(b'\xff'*131072);c+=['--save',str(p),'--save-out',str(out/(tag+'.sav'))]
 for f,w,a,v in writes:c+=['--write',f'{f}:{w}:{a}:{v}']
 for f,d,k in keys:c+=['--key',f'{f}:{d}:{k}']
 for f in shots:c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY','gSaveBlock1Ptr','gSaveBlock2Ptr','gSaveFileStatus','gDamagedSaveSectors']:
  if n in sy:c+=['--read',f'4:{sy[n]}']
 for i in range(2):
  for offset in [86,88]:c+=['--read',f'2:{sy["gParties"]+100*i+offset}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
 if r.returncode:raise SystemExit(r.returncode)
if __name__=='__main__':
 w=[(1,4,sy['gEcHeadlessCampaignQueryKind'],9)]+[(1,2,sy['gParties']+100*i+86,1 if i==0 else 0) for i in range(6)]
 run('league-retire-base','league-sidney-cleared',500,writes=w)
 for action in ['stay','retire']:
  k=[(700,16,'UP'),(1000,2,'A'),(1700,2,'B'),(2300,2,'A' if action=='retire' else 'B')]+[(f,2,'B') for f in range(3000,7000,700)]
  run('league-'+action,'league-retire-base',8000,k,shots=[1500,2100,2800,4400])
 run('league-champion-hof','league-drake-cleared',22000,[(f,2,'B') for f in range(300,18000,450)],writes=[(1,4,sy['gEcHeadlessCampaignQueryKind'],1),(1,4,sy['gEcHeadlessCampaignQueryId'],0x864)],save=True,shots=[2000,4500,7000,9500,12000,15000,18000,21000])
