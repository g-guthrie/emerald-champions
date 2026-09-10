from pathlib import Path
import sys,subprocess,re
sys.path.insert(0,'scripts');from native_tools import symbols
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';s=symbols(root/'pokeemerald-mauville-audit-headless.elf',root,first=True);base=['build/headless/emerald_champions_mgba_runner','--rom','pokeemerald-mauville-audit-headless.gba','--state-in',str(out/'pacifidlog-sun.mgba')]
def peek(width,addr):
 r=subprocess.run(base+['--frames','1','--read',f'{width}:{addr}'],capture_output=True,text=True,check=True)
 return int(re.findall(r'value=([0-9a-f]+)',r.stdout)[-1],16)
save2=peek(4,s['gSaveBlock2Ptr']);offset=peek(2,save2+0x98)
# Offset 0x98 was checked using the actual target compiler and SaveBlock2 header.
# Advance this scratch game's local date seven days; preserve its receipt flag/date.
c=base+['--frames','7000','--write',f'50:2:{save2+0x98}:{(offset-7)&65535}','--key','700:2:UP','--key','900:2:A','--screenshot',str(out/'pacifidlog-week-local-end.png'),'--screenshot-at',f'1200:{out}/pacifidlog-week-local-1200.png']
for f in range(1600,6500,220):c+=['--key',f'{f}:2:B']
for n,w in [('gEcHeadlessCampaignQueryValue',4),('gEcHeadlessCampaignControlsLocked',4),('gEcHeadlessCampaignScriptEnabled',4),('gLocalTime',2)]:c+=['--read',f'{w}:{s[n]}']
r=subprocess.run(c,capture_output=True,text=True);print('save2',hex(save2),'old local offset',offset,r.stdout,r.stderr);raise SystemExit(r.returncode)
