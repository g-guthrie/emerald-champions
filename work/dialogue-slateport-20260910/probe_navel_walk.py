import sys,struct
sys.path.insert(0,'work/dialogue-slateport-20260910')
from walk_island_maps import execute,sy,walk
from probe_southern import flag,out
p=b''.join(flag(n,False) for n in ['FLAG_CAUGHT_HO_OH','FLAG_CAUGHT_LUGIA','FLAG_DEFEATED_HO_OH','FLAG_DEFEATED_LUGIA'])+bytes([0x39,26,67,255])+struct.pack('<HH',8,4)+bytes([0x27,0x6b,2])
a=sy['gStringVar4']+800;c=sy['sGlobalScriptContext'];w=[(50,1,a+i,v) for i,v in enumerate(p)]+[(50,1,c,0),(50,4,c+8,a),(50,1,c+1,1),(50,1,sy['sGlobalScriptContextStatus'],0),(50,4,sy['gEcHeadlessFixtureParam'],242)]
execute('navel-harbor-start','postgame-audit-base',1700,(),w)
s='navel-harbor-start'
for i,(m,warp) in enumerate([('NavelRock_Harbor',0),('NavelRock_Exterior',1),('NavelRock_Entrance',0),('NavelRock_B1F',1)]):s=walk(m,s,f'navel-approach-{i}',warp)
(out/'navel-fork-state.txt').write_text(s+'\n')
for label,route in [('up',[('NavelRock_Fork',0)]+[(f'NavelRock_Up{i}',1) for i in range(1,5)]),('down',[('NavelRock_Fork',2)]+[(f'NavelRock_Down{i:02d}',1) for i in range(1,11)]+[('NavelRock_Down11',0)])]:
 s=(out/'navel-fork-state.txt').read_text().strip()
 for i,(m,warp) in enumerate(route):s=walk(m,s,f'navel-{label}-{i}',warp)
 (out/f'navel-{label}-arrival-state.txt').write_text(s+'\n')
