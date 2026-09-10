from pathlib import Path
import sys,subprocess,struct
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';sy=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True)
def run(tag,state,mons,faint=False):
 program=b''
 for slot,sp in enumerate(mons):program+=bytes([0x23])+struct.pack('<I',(sy['ScrCmd_createmon']|1)+0x02000000)+struct.pack('<BBHHI',0,slot,sp,50,0)
 if faint:program+=bytes([0x2a])+struct.pack('<H',0x4c1)
 program+=bytes([0x39,24,72,255])+struct.pack('<HH',10,13)+bytes([0x27,0x6b,2]);addr=sy['gStringVar4']+800;ctx=sy['sGlobalScriptContext'];c=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/(state+'.mgba')),'--frames','12000','--state-out',str(out/(tag+'.mgba')),'--screenshot',str(out/(tag+'-end.png'))]
 for j,v in enumerate(program):c+=['--write',f'50:1:{addr+j}:{v}']
 writes=[(1,ctx,0),(4,ctx+8,addr),(1,ctx+1,1),(1,sy['sGlobalScriptContextStatus'],0),(4,sy['gEcHeadlessCampaignQueryKind'],1),(4,sy['gEcHeadlessCampaignQueryId'],0x4c1)]
 if faint:writes += [(4,sy['gEcHeadlessFixtureParam'],249),(4,sy['gEcHeadlessCampaignBattleSerial'],0),(4,sy['gEcHeadlessCampaignCaptureSerial'],0)]
 for w,a,v in writes:c+=['--write',f'50:{w}:{a}:{v}']
 for f,d,k in [(700,2,'UP'),(900,2,'A')]+[(f,2,'B') for f in range(1400,11500,220)]:c+=['--key',f'{f}:{d}:{k}']
 for f in [1200,2000,2800,4500]:c+=['--screenshot-at',f'{f}:{out}/{tag}-{f}.png']
 for n in ['gEcHeadlessCampaignQueryValue','gEcHeadlessCampaignControlsLocked','gEcHeadlessCampaignScriptEnabled','gEcHeadlessCampaignBattleSerial','gEcHeadlessCampaignCaptureSerial','gEcHeadlessCampaignLastCapturedSpecies']:c+=['--read',f'4:{sy[n]}']
 r=subprocess.run(c,capture_output=True,text=True);print(tag,r.returncode,r.stdout,r.stderr,flush=True);assert r.returncode==0
run('regigigas-faint','field-audit-base',[377,378,379],True)
run('regigigas-missing-partner','regigigas-faint',[377,378,25])
run('regigigas-restored-partner','regigigas-missing-partner',[377,378,379])
run('regigigas-caught-reentry','regigigas-restored-partner',[])
