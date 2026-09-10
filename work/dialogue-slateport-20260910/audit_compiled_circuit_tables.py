import sys,struct,json,hashlib
from pathlib import Path
sys.path.insert(0,'scripts')
from native_tools import symbols
root=Path.cwd();sy=symbols(root/'pokeemerald-northern-audit-20260910-release.elf',root,first=True);rom=Path('pokeemerald-northern-audit-20260910-release.gba').read_bytes()
def data(name):return sy[name]-0x8000000
def h(off):return struct.unpack_from('<H',rom,off)[0]
vs,vo,vc,ts,ta,tmc,tac,tau,tev,ss,sa,sh,na,nd,*enumSizes=struct.unpack('<18I',Path('work/dialogue-slateport-20260910/circuit-layout-audit.bin').read_bytes())
assert enumSizes==[2,2,2,2]
errors=[];groups=[];seen=set();last=None;authored=set();owned=set();used=set()
for i in range(756):
 p=data('gShowdownCircuitVariants')+i*vs
 species,form,item=h(p),h(p+2),h(p+4);start=h(p+vo);count=rom[p+vc]
 if not(0<species<sy['NUM_SPECIES'] and 0<form<sy['NUM_SPECIES'] and item<sy['ITEMS_COUNT'] and 0<count and start+count<=1322):errors.append(['variant bounds',i]);continue
 s=data('gSpeciesInfo')+species*ss;f=data('gSpeciesInfo')+form*ss
 if not rom[s] or not rom[f]:errors.append(['disabled variant',i,species,form])
 dex=h(s+60) # ARM compiler's CircuitAuditDex disassembly: ldrh [r0,#60].
 if not 0<dex<=nd:errors.append(['dex bounds',i,dex])
 if dex!=last:
  if dex in seen:errors.append(['noncontiguous dex group',i,dex])
  seen.add(dex);groups.append(dex);last=dex
 legal={h(s+sa+a*2) for a in range(na)}
 for ti in range(start,start+count):
  used.add(ti);t=data('gShowdownCircuitTemplates')+ti*ts;mc=rom[t+tmc];ac=rom[t+tac]
  moves=[h(t+j*2) for j in range(9)];abilities=[h(t+ta+j*2) for j in range(3)]
  if not(0<mc<=9 and 0<ac<=3):errors.append(['template counts',i,ti,mc,ac]);continue
  if len(set(moves[:mc]))!=mc or any(not 0<m<sy['MOVES_COUNT'] for m in moves[:mc]) or any(moves[mc:]):errors.append(['move bounds/padding',ti,moves,mc])
  if any(a not in legal or a==0 for a in abilities[:ac]) or any(abilities[ac:]):errors.append(['ability legality/padding',i,ti,abilities,list(legal)])
  if rom[t+tau]:
   authored.add(ti);evs=list(rom[t+tev:t+tev+6])
   if mc>4 or sum(evs)>510 or max(evs)>252:errors.append(['authored bounds',ti,mc,evs])
result={'release_rom_sha256':hashlib.sha256(rom).hexdigest(),'variants':756,'templates':1322,'referenced_templates':len(used),'national_dex_groups':len(groups),'authored_templates':len(authored),'compiler_derived_layout':[vs,vo,vc,ts,ta,tmc,tac,tau,tev,ss,sa,sh,na,nd,*enumSizes],'errors':errors}
Path('work/dialogue-slateport-20260910/circuit-compiled-table-audit.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2));assert not errors and len(used)==1322
