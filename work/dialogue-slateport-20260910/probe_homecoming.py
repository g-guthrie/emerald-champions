from pathlib import Path
import sys,subprocess,struct,json,re
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
flags={m[1]:int(m[2],16) for m in re.finditer(r'^#define\s+(FLAG_\w+)\s+(0x[0-9A-Fa-f]+)\b',Path('include/constants/flags.h').read_text(),re.M)}
def flag(n,on):return bytes([0x29 if on else 0x2a])+struct.pack('<H',flags[n])
def var(n,v):return bytes([0x16])+struct.pack('<HH',n,v)
def run(tag,state,frames,keys=(),writes=(),shots=()):
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/(state+'.mgba')),'--frames',str(frames),'--state-out',str(out/(tag+'.mgba')),'--screenshot',str(out/(tag+'-end.png'))]
 for f,w,a,v in writes:c+=['--write',f'{f}:{w}:{a}:{v}']
 for f,d,k in keys:c+=['--key',f'{f}:{d}:{k}']
 for f in shots:c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignMapId','gEcHeadlessCampaignPlayerX','gEcHeadlessCampaignPlayerY','gSaveBlock1Ptr','gSaveBlock2Ptr']:c+=['--read',f'4:{sy[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True)
 if r.returncode:raise SystemExit(r.returncode)
 return r.stdout
if __name__=='__main__':
 c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'victory-audit-base.mgba'),'--frames','1','--read',f'4:{sy["gSaveBlock2Ptr"]}']
 r=subprocess.run(c,capture_output=True,text=True,check=True);sb2=int(re.search(r'value=([0-9a-f]+)',r.stdout)[1],16)
 for gender,early in [(0,False),(1,False),(0,True)]:
  tag='homecoming-'+('female' if gender else 'male')+('-early-pass' if early else '')
  p=b''.join(var(n,v) for n,v in [(0x4092,7),(0x4050,4),(0x4084,5),(0x4085,7),(0x40e2,4),(0x4082,2),(0x408c,2),(0x40d3,0)])
  p+=flag('FLAG_RECEIVED_SS_TICKET',early)+flag('FLAG_LATIOS_OR_LATIAS_ROAMING',early)+flag('FLAG_HIDE_PLAYERS_HOUSE_DAD',True)
  p+=bytes([0x04])+struct.pack('<I',sy['EverGrandeCity_HallOfFame_EventScript_SetGameClearFlags'])+bytes([0x29])+struct.pack('<H',0x864)
  for house in ['BRENDANS','MAYS']:
   for suffix in ['MOM','RIVAL_MOM','RIVAL_SIBLING', 'BRENDAN' if house=='BRENDANS' else 'MAY']:
    hide=suffix!='MOM' or (house=='MAYS')!=(gender==1)
    p+=flag('FLAG_HIDE_LITTLEROOT_TOWN_'+house+'_HOUSE_'+suffix,hide)
   p+=flag('FLAG_HIDE_LITTLEROOT_TOWN_'+house+'_HOUSE_RIVAL_BEDROOM',True)
  for n in ['FLAG_HIDE_LITTLEROOT_TOWN_PLAYERS_HOUSE_VIGOROTH_1','FLAG_HIDE_LITTLEROOT_TOWN_PLAYERS_HOUSE_VIGOROTH_2']:p+=flag(n,True)
  p+=bytes([0x39,1,3 if gender else 1,255])+struct.pack('<HH',1 if gender else 7,2)+bytes([0x27,0x6b,2])
  assert len(p)<200,len(p)
  addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext'];w=[(50,1,addr+i,v) for i,v in enumerate(p)]
  w +=[(50,1,sb2+8,gender),(50,1,ctx,0),(50,4,ctx+8,addr),(50,1,ctx+1,1),(50,1,sy['sGlobalScriptContextStatus'],0),(50,4,sy['gEcHeadlessCampaignQueryKind'],2),(50,4,sy['gEcHeadlessCampaignQueryId'],0x4082)]
  run(tag,'victory-audit-base',18000,[(800,16,'UP')]+[(f,2,'B') for f in range(1400,16000,450)],w,[1100,2000,5000,8500,11500,15000])
