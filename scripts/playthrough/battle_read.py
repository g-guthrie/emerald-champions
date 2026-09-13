"""Read-only native checkpoint inspection; never advances the saved primary state.
Member offsets reflect the current engine layouts; audit them when layouts change.
"""
from pathlib import Path
import sys,json,re,shutil
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'scripts'));import render_emerald_champions_ui as ui
out=Path(sys.argv[1]);elf=out/'scene.elf';rom=out/'scene.gba';state=out/'current.ss1';runner=ui.build_runner()
syms={p[-1]:int(p[0],16) for l in ui.run([shutil.which('arm-none-eabi-nm'),'-S',str(elf)]).stdout.splitlines() if len(p:=l.split())>=3 and re.fullmatch('[0-9a-fA-F]+',p[0])}
def read(fields):
 cmd=[str(runner),'--rom',str(rom),'--state-in',str(state),'--frames','1']
 for w,a in fields.values():cmd+=['--read',f'{w}:0x{a:x}']
 r=ui.run(cmd);v={int(a,16):int(b,16) for a,b in re.findall(r'READ width=\d+ address=([0-9a-f]+) value=([0-9a-f]+)',r.stdout)}
 return {k:v[a] for k,(w,a) in fields.items()}
names={v:k for k,v in syms.items()}
p=read({s:(4,syms[s]) for s in ['gAiLogicData','gAiBattleData','gBattleStruct']}); fields={}
if not all(p.values()):
 n=len(json.loads((out/'trace.json').read_text())['steps'])-1; v={'battle_resources_live':False,'pointers':p}; (out/f'{n:03d}-battle-read.json').write_text(json.dumps(v,indent=2)+'\n'); print(json.dumps(v));sys.exit(0)
for s,w,n in [('gBattleCommunication',1,4),('gChosenActionByBattler',1,4),('gChosenMoveByBattler',2,4),('gMoveSelectionCursor',1,4),('gActionSelectionCursor',1,4),('gBattlerPartyIndexes',2,4)]:
 for i in range(n):fields[f'{s}[{i}]']=(w,syms[s]+i*w)
for s,w in [('gBattleTurnCounter',2),('gCurrentMove',2),('gBattlerAttacker',1),('gBattlerTarget',1),('gBattleOutcome',1),('gMultiUsePlayerCursor',1)]:fields[s]=(w,syms[s])
fields['battleMain']=(4,syms['gBattleMainFunc'])
for b in range(4):fields[f'controller{b}']=(4,syms['gBattlerControllerFuncs']+b*4)
fields['partyCursor']=(1,syms['gPartyMenu']+9)
fields['vblank']=(4,syms['gMain']+32)
for label,off in [('decisionStartFrame',0),('decisionSetupFrames',4),('flags',2324)]:fields[label]=(4,p['gAiLogicData']+off)
for b in range(4):
 fields[f'mon{b}.aiMoveIndex']=(1,p['gAiBattleData']+262+b)
 fields[f'mon{b}.aiTarget']=(1,p['gAiBattleData']+266+b)
 for label,w,off in [('species',2,0),('attack',2,2),('defense',2,4),('speed',2,6),('spAttack',2,8),('spDefense',2,10),('hp',2,42),('level',1,44),('maxHP',2,46),('item',2,48),('ability',2,32),('status',4,80)]:fields[f'mon{b}.{label}']=(w,syms['gBattleMons']+140*b+off)
 for i in range(4):fields[f'mon{b}.move{i}']=(2,syms['gBattleMons']+140*b+12+2*i)
 for i in range(8):fields[f'mon{b}.stage{i}']=(1,syms['gBattleMons']+140*b+24+i)
 for label,w,off in [('chosenIndex',1,369),('chosenTarget',2,288),('switchSlot',1,333)]:fields[f'mon{b}.{label}']=(w,p['gBattleStruct']+off+w*b)
v=read(fields);v['movesScoredMask']=(v['flags']>>19)&15;v['pointers']=p
n=len(json.loads((out/'trace.json').read_text())['steps'])-1;(out/f'{n:03d}-battle-read.json').write_text(json.dumps(v,indent=2)+'\n');print(json.dumps({'phase':names.get(v['battleMain']&~1,hex(v['battleMain'])),'controllers':[names.get(v[f'controller{b}']&~1,hex(v[f'controller{b}'])) for b in range(4)],'partyCursor':v['partyCursor'],'turn':v['gBattleTurnCounter'],'selection':[v[f'gBattleCommunication[{b}]'] for b in range(4)],'scored':v['movesScoredMask'],'pointers':p,'vblank':v['vblank'],'decisionStartFrame':v['decisionStartFrame'],'decisionSetupFrames':v['decisionSetupFrames'],'moveCursors':[v[f'gMoveSelectionCursor[{b}]'] for b in range(4)],'actionCursors':[v[f'gActionSelectionCursor[{b}]'] for b in range(4)],'targetCursor':v['gMultiUsePlayerCursor'],'currentMove':v['gCurrentMove'],'attacker':v['gBattlerAttacker'],'target':v['gBattlerTarget'],'mons':[{k.split('.',1)[1]:val for k,val in v.items() if k.startswith(f'mon{b}.')} for b in range(4)]}))
