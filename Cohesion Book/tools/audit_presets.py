from pathlib import Path
import sys,json,collections,re
B=Path(__file__).resolve().parents[1];R=B/'baseline/source';sys.dont_write_bytecode=True;sys.path.insert(0,str(R/'scripts'))
import audit_emerald_champions_master_battles as a
j=json.loads((R/'data/emerald_champions/emerald_champions_battle_sets.json').read_text());rows=[];unmapped=[]
for group in ['defaults','alternatives','singles_defaults','singles_alternatives']:
 for i,p in enumerate(j[group]):
  sid=a.showdown_id_for_species(p['species']);legal=a.pinned_legal_moves(p['species'])
  if sid is None:
   unmapped.append({'group':group,'index':i,'species':p['species'],'name':p['name'],'reason':'Reference resolver cannot identify this form; not proof of illegality.'});continue
  bad=[m for m in p['moves'] if m!='MOVE_NONE' and m not in legal]
  if bad:rows.append({'group':group,'index':i,'species':p['species'],'name':p['name'],'mismatches':bad,'baseline':p,'status':'SOURCE_REFERENCE_MISMATCH','reason':'Pinned Champions/latest-mainline-plus-reviewed-extension comparison; game itself accepts preset-supplied moves. Requires explicit policy reconciliation.'})
(B/'inventory/preset-reference-mismatches.json').write_text(json.dumps(rows,indent=2)+'\n');(B/'inventory/preset-reference-unmapped.json').write_text(json.dumps(unmapped,indent=2)+'\n')
print('Totalsets',sum(len(j[k]) for k in ['defaults','alternatives','singles_defaults','singles_alternatives']),'mismatchsets',len(rows),'unmappedsets',len(unmapped));print('mismatchesbygroup',collections.Counter(x['group'] for x in rows));print('moves',collections.Counter(x for p in rows for x in p['mismatches']).most_common(35))
for p in rows[:40]:print(p['species'],p['name'],','.join(p['mismatches']))
