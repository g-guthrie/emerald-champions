#!/usr/bin/env python3
"""Read frozen source and emit factual book inventories. Never writes source."""
from pathlib import Path
import sys,json,re,collections,hashlib
BOOK=Path(__file__).resolve().parents[1]
ROOT=BOOK/'baseline/source'
sys.dont_write_bytecode=True
sys.path.insert(0,str(ROOT/'scripts'))
import verify_trainer_ability_legality as ability
import emerald_champions_teams as teams
import verify_mega_stone_rewards as rewards

def write(name,obj):
 (BOOK/'inventory'/name).write_text(json.dumps(obj,indent=2,ensure_ascii=False)+'\n')
def brace(text,start):
 p=text.index('{',start);depth=0;quote=False;escape=False
 for i in range(p,len(text)):
  c=text[i]
  if quote:
   if escape:escape=False
   elif c=='\\':escape=True
   elif c=='"':quote=False
   continue
  if c=='"':quote=True
  elif c=='{':depth+=1
  elif c=='}':
   depth-=1
   if not depth:return text[p:i+1]
 raise ValueError('unbalanced')
def entries(text):
 result=[];i=1
 while i<len(text)-1:
  if text[i]=='{':
   b=brace(text,i);result.append(b);i+=len(b)
  else:i+=1
 return result
species_text=(BOOK/'inventory/species-preprocessed.txt').read_text()
markers=list(ability.SPECIES_MARKER.finditer(species_text));species={}
rawloc={}
for p in (ROOT/'src/data/pokemon/species_info').glob('*.h'):
 for m in re.finditer(r'\[(SPECIES_\w+)\]\s*=',p.read_text()):rawloc.setdefault(m[1],{'file':str(p.relative_to(ROOT)),'line':p.read_text()[:m.start()].count('\n')+1})
for i,m in enumerate(markers):
 b=species_text[m.end():markers[i+1].start() if i+1<len(markers) else len(species_text)]
 if not re.search(r'\.natDexNum\s*=',b):continue
 row={'species':m[1], 'source':rawloc.get(m[1]),'dex':re.search(r'\.natDexNum\s*=\s*(\w+)',b)[1]}
 for f,rx in [('types',r'\.types\s*=\s*\{(.*?)\}'),('abilities',r'\.abilities\s*=\s*\{(.*?)\}')]:
  x=re.search(rx,b,re.S);row[f]=list(dict.fromkeys(re.findall(r'TYPE_\w+' if f=='types' else r'ABILITY_\w+',x[1]))) if x else []
 row['flags']=re.findall(r'\.(is\w+)\s*=\s*(?:1|TRUE)\b',b)
 row['stats']={f:re.search(r'\.'+f+r'\s*=\s*([^,]+)',b)[1].strip() for f in ['baseHP','baseAttack','baseDefense','baseSpeed','baseSpAttack','baseSpDefense'] if re.search(r'\.'+f+r'\s*=',b)}
 row['evolutions']=[]
 e=re.search(r'\.evolutions\s*=\s*\(const struct Evolution\[\]\)',b)
 if e:
  for ent in entries(brace(b,e.end())):
   x=re.match(r'\{\s*(EVO_\w+)\s*,\s*([^,]+)\s*,\s*(SPECIES_\w+)',ent)
   if x:row['evolutions'].append({'method':x[1],'parameter':x[2].strip(),'target':x[3],'conditions':re.findall(r'\{(IF_\w+[^{}]*)\}',ent)})
 x=re.search(r'\.formSpeciesIdTable\s*=\s*(\w+)',b);row['form_table']=x[1] if x else None
 x=re.search(r'\.formChangeTable\s*=\s*(\w+)',b);row['change_table']=x[1] if x else None
 species[m[1]]=row
aliases=dict(re.findall(r'\b(SPECIES_\w+)\s*=\s*(SPECIES_\w+)',(ROOT/'include/constants/species.h').read_text()))
write('species.json',species);write('species-aliases.json',aliases)
groups=json.loads((ROOT/'data/maps/map_groups.json').read_text());maps={}
for group in groups['group_order']:
 for name in groups[group]:
  p=ROOT/'data/maps'/name/'map.json';j=json.loads(p.read_text())
  if j.get('region')=='REGION_HOENN':maps[name]=j
write('maps.json',maps)
ids={j['id']:n for n,j in maps.items()}
wild=json.loads((ROOT/'src/data/wild_encounters.json').read_text());g=next(g for g in wild['wild_encounter_groups'] if g['label']=='gWildMonHeaders');fields={x['type']:x for x in g['fields']};wr=[]
for e in g['encounters']:
 if e['map'] not in ids:continue
 r={'map':e['map'],'name':ids[e['map']],'base_label':e.get('base_label'),'methods':{},'inactive_hidden':e.get('hidden_mons')}
 for name,f in fields.items():
  if name not in e:continue
  t=e[name]
  methods=f.get('groups',{name:list(range(len(f['encounter_rates'])))})
  for mn,indices in methods.items():
   slots=[dict(t['mons'][ix],weight=f['encounter_rates'][ix],slot=ix) for ix in indices]
   r['methods'][mn]={'table_field':name,'encounter_rate':t['encounter_rate'],'slots':slots}
 wr.append(r)
write('wild.json',wr)
master=teams.MASTER.read_text();meta={}
for block in re.split(r'(?m)^=== ENCOUNTER ',master)[1:]:
 number=int(block.split()[0]);kv=dict(re.findall(r'^([a-z_]+): (.+)$',block,re.M));meta[number]=kv
trs=[]
for b in teams.read_teams():
 d=vars(b).copy();d['mons']=[vars(m) for m in b.mons];d['metadata']=meta[b.encounter];trs.append(d)
write('trainers.json',trs)
write('mega-rewards.json',rewards.world_reward_sources())
# Record form-change instructions without assuming every change is reachable.
forms={}
for m in re.finditer(r'static const struct FormChange\s+(\w+)\[\]\s*=\s*\{',(ROOT/'src/data/pokemon/form_change_tables.h').read_text()):
 s=(ROOT/'src/data/pokemon/form_change_tables.h').read_text(); forms[m[1]]={'line':s[:m.start()].count('\n')+1,'entries':entries(brace(s,m.end()-1))}
write('form-changes.json',forms)
signs=[]
for i,line in enumerate((ROOT/'src/data/pokemon/legendary_signs.h').read_text().splitlines(),1):
 x=re.match(r'(\w+_SIGN)\((LEGENDARY_SIGN_\w+),\s*(\w+),\s*(.*?)\),',line)
 if not x:continue
 vals=[v.strip() for v in x[4].split(',')]
 row={'id':x[2],'species':'SPECIES_'+x[3],'kind':x[1],'line':i,'raw':line}
 if x[1] in ['LANDMARK_SIGN','VISIBLE_SIGN']:
  row.update(map='MAP_'+vals[0],badges=int(vals[1]),level_offset=int(vals[2]),required_species='SPECIES_'+vals[3],required_flag=vals[4])
 elif x[1]=='ORDINARY_WILD_SIGN':row.update(map='MAP_'+vals[0],badges=0,level_offset=0,required_species='SPECIES_NONE',required_flag='0')
 else:row['source']=vals[0]
 signs.append(row)
write('legendary-signs.json',signs)
# Bound script labels and all ordinary one-off gifts/trades, with evidence labels.
label=re.compile(r'^([A-Za-z_]\w*)::?',re.M);acq=[]
for name,j in maps.items():
 p=ROOT/'data/maps'/name/'scripts.inc'
 if not p.exists():continue
 text=p.read_text();ls=list(label.finditer(text))
 for k,m in enumerate(ls):
  body=text[m.end():ls[k+1].start() if k+1<len(ls) else len(text)]
  for cmd,sp in re.findall(r'\b(givemon|giveegg|setwildbattle)\s+(SPECIES_\w+)',body):acq.append({'map':j['id'],'file':str(p.relative_to(ROOT)),'line':text[:m.start()].count('\n')+1,'label':m[1],'species':sp,'kind':cmd,'caution':'Script-root binding and state conditions require world review.'})
  if 'GiveEmeraldChampionsPreparedPokemon' in body or 'GiveEmeraldChampionsGameCornerPokemon' in body:
   acq.append({'map':j['id'],'file':str(p.relative_to(ROOT)),'line':text[:m.start()].count('\n')+1,'label':m[1],'kind':'prepared-gift-function','body':body.strip()})
write('scripted-acquisition-candidates.json',acq)
summary={'registered_hoenn_maps':len(maps),'object_events':sum(len(j.get('object_events',[])) for j in maps.values()),'coordinate_events':sum(len(j.get('coord_events',[])) for j in maps.values()),'background_events':sum(len(j.get('bg_events',[])) for j in maps.values()),'warps':sum(len(j.get('warp_events',[])) for j in maps.values()),'connections':sum(len(j.get('connections') or []) for j in maps.values()),'wild_rows':len(wr),'wild_maps':len({r['map'] for r in wr}),'wild_method_records':sum(len(r['methods']) for r in wr),'configured_species_forms':sum(k!='SPECIES_NONE' for k in species),'national_dex_entries':len({r['dex'] for k,r in species.items() if k!='SPECIES_NONE'}),'trainer_branches':len(trs),'encounter_groups':len(meta),'mega_stones':len(rewards.world_reward_sources()),'legendary_signs':len(signs)}
write('summary.json',summary);print(json.dumps(summary,indent=2))
