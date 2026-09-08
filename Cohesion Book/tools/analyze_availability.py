#!/usr/bin/env python3
"""Potential acquisition graph, with explicit conditions; not a traversal proof."""
from pathlib import Path
import json,re,collections,sys
B=Path(__file__).resolve().parents[1];R=B/'baseline/source'
sys.dont_write_bytecode=True;sys.path.insert(0,str(R/'scripts'))
s=json.loads((B/'inventory/species.json').read_text());a=json.loads((B/'inventory/species-aliases.json').read_text());w=json.loads((B/'inventory/wild.json').read_text());sg=json.loads((B/'inventory/legendary-signs.json').read_text());maps=json.loads((B/'inventory/maps.json').read_text())
def res(x):
 seen=set()
 while x in a and x not in seen:seen.add(x);x=a[x]
 return x
roots=collections.defaultdict(list)
for e in w:
 for name,t in e['methods'].items():
  if t['encounter_rate']<=0:continue
  for p in t['slots']:roots[res(p['species'])].append({'kind':'wild','map':e['map'],'method':name,'percent':p['weight'],'level':[p['min_level'],p['max_level']]})
for p in sg:roots[res(p['species'])].append({'kind':'legendary','id':p['id'],'definition':p,'condition':'Physical root/claim path and story gates must be verified; not a claim of earliest access.'})
for x in ['TREECKO','TORCHIC','MUDKIP','BULBASAUR','CHARMANDER','SQUIRTLE','CHIKORITA','CYNDAQUIL','TOTODILE','TURTWIG','CHIMCHAR','PIPLUP','SNIVY','TEPIG','OSHAWOTT','CHESPIN','FENNEKIN','FROAKIE','ROWLET','LITTEN','POPPLIO','GROOKEY','SCORBUNNY','SOBBLE','SPRIGATITO','FUECOCO','QUAXLY']:
 roots[res('SPECIES_'+x)].append({'kind':'starter-archive','map':'MAP_MAUVILLE_CITY_GAME_CORNER','condition':'Initial choice or one-time500-Coin archive; INTRO-01 expands initial permanent choice to two.'})
for x in ['OMANYTE','KABUTO','AERODACTYL','LILEEP','ANORITH','SHIELDON','CRANIDOS','TIRTOUGA','ARCHEN','TYRUNT','AMAURA']:
 roots[res('SPECIES_'+x)].append({'kind':'fossil','map':'MAP_RUSTBORO_CITY_DEVON_CORP_2F','condition':'Own matching fossil; actual fossil pickup/story access recorded in world volume.'})
for x,m,kind in [('CASTFORM_NORMAL','ROUTE119_WEATHER_INSTITUTE_2F','gift'),('BELDUM','MOSSDEEP_CITY_STEVENS_HOUSE','gift'),('TOGEPI','ROUTE117_POKEMON_DAYCARE','egg'),('WYNAUT','LAVARIDGE_TOWN','egg'),('ROTOM','NEW_MAUVILLE_INSIDE','scripted encounter'),('SPIRITOMB','ABANDONED_SHIP_ROOM_B1F','scripted encounter'),('KECLEON','ROUTE120','scripted encounter'),('SUDOWOODO','BATTLE_FRONTIER_OUTSIDE_EAST','scripted encounter'),('ELECTRODE','AQUA_HIDEOUT_B1F','scripted encounter'),('VOLTORB','NEW_MAUVILLE_INSIDE','scripted encounter')]:roots[res('SPECIES_'+x)].append({'kind':kind,'map':'MAP_'+m,'condition':'See named physical script and world review.'})
raw=(R/'src/legendary_signs.c').read_text().split('sNativeLegendaryEncounters[] =',1)[1].split('};',1)[0]
for x in re.findall('SPECIES_\w+',raw):roots[res(x)].append({'kind':'native-legendary','source':'src/legendary_signs.c:sNativeLegendaryEncounters','condition':'Actual one-off script, physical presence, story state and retry path in world volume.'})
trade=(R/'src/data/trade.h').read_text()
for name in maps:
 p=R/'data/maps'/name/'scripts.inc'
 if not p.exists():continue
 for t in set(re.findall('INGAME_TRADE_\w+',p.read_text())):
  b=re.search(r'\['+t+r'\]\s*=\s*\{(.*?)\n    \}',trade,re.S)
  if b:
   sp=re.search(r'\.species\s*=\s*(SPECIES_\w+)',b[1]);rq=re.search(r'\.requestedSpecies\s*=\s*(SPECIES_\w+)',b[1]); roots[res(sp[1])].append({'kind':'trade','map':maps[name]['id'],'requested':rq[1],'trade':t})
# Source-defined directed potential links; never union EVO_NONE or regions that do not exist.
edges=[]
for n,row in s.items():
 for e in row['evolutions']:
  if e['method']=='EVO_NONE':continue
  blocked=any(c.startswith('IF_REGION,') and c.split(',')[1].strip() not in ['REGION_HOENN','REGION_KANTO'] for c in e['conditions'])
  edges.append({'from':n,'to':res(e['target']),'kind':'evolution','definition':e,'blocked_region':blocked})
f=json.loads((B/'inventory/form-changes.json').read_text())
for n,row in s.items():
 for ent in f.get(row['change_table'],{}).get('entries',[]):
  v=re.match(r'\{\s*(FORM_CHANGE_\w+)\s*,\s*(SPECIES_\w+)',ent)
  if v and res(v[2]) in s:edges.append({'from':n,'to':res(v[2]),'kind':'form','definition':ent,'trigger':v[1],'blocked_region':False})
# Evolution descendants can be bred back if breedable; this is potential not an incense/parent proof.
for e in list(edges):
 if e['kind']=='evolution' and e['definition']['method'] not in ['EVO_NONE']:
  edges.append({'from':e['to'],'to':e['from'],'kind':'breeding-potential','definition':e['definition'],'blocked_region':False})
seen=set(roots);trace={k:{'kind':'direct','root':k} for k in roots};changed=True
while changed:
 changed=False
 for e in edges:
  if e['from'] in seen and e['to'] not in seen and not e['blocked_region']:
   seen.add(e['to']);trace[e['to']]={'kind':e['kind'],'from':e['from'],'definition':e['definition']};changed=True
for n in s:
 if n not in trace:trace[n]={'kind':'not-reached-by-potential-graph'}
(B/'inventory/acquisition-roots.json').write_text(json.dumps(roots,indent=2)+'\n');(B/'inventory/acquisition-edges.json').write_text(json.dumps(edges,indent=2)+'\n');(B/'inventory/acquisition-potential.json').write_text(json.dumps(trace,indent=2)+'\n')
missing=collections.defaultdict(list)
for n,row in s.items():
 if n not in seen and n!='SPECIES_NONE':missing[row['dex']].append(n)
print('Direct species/forms',len(roots),'potential reached',len(seen),'missing dex wholly',sum(all(k not in seen for k,v in s.items() if v['dex']==d) for d in missing),'missing exact forms',sum(len(v) for v in missing.values()))
for d,ls in missing.items():
 print(d.replace('NATIONAL_DEX_',''),','.join(x.replace('SPECIES_','') for x in ls))
