from pathlib import Path
import json,re,collections
B=Path(__file__).resolve().parents[2];S=B/'baseline/source';d=json.loads((B/'review/mega-rewards.json').read_text());src=json.loads((B/'inventory/mega-rewards.json').read_text());errs=[]
def ck(ok,msg):
 if not ok and msg not in errs:errs.append(msg)
ck(set(d['stones'])==set(src),'stone coverage differs from snapshot inventory')
actual_routes=0;formn=0;receipts=collections.defaultdict(set)
for item,r in d['stones'].items():
 ck(bool(r['individual_assessment']),item+' missing individual decision')
 wr=[x for x in r['routes'] if x['kind']!='conditional_original_starter'];actual_routes+=len(wr);ck([x['inventory_description'] for x in wr]==src[item],item+' raw route mismatch')
 ck(bool(r['native_bindings']),item+' missing binding')
 for x in r['native_bindings']:
  formn+=1;ck((S/x['table_source']).exists(),item+' missing form table');txt=(S/x['table_source']).read_text();ck(item in txt and x['mega'] in txt,item+' missing native form target')
 for rt in wr:
  if rt['kind']=='script_grant':ck(rt['source_root'] in (S/rt['source']).read_text(),item+' missing grant root');ck(bool(rt['physical_roots']),item+' unbound grant')
  if rt['kind']=='physical_pickup':
   ck(rt['event']['trainer_sight_or_berry_tree_id']==item,item+' wrong pickup');ck(rt['event']['graphics_id']=='OBJ_EVENT_GFX_MEGA_STONE',item+' wrong sparkle');ck(any(x['collision']==0 for x in rt['geometry']['adjacent_cells']),item+' no adjacent fixed-layout cell')
  for f in rt.get('receipt_flags',[]):
   ck(f['numeric'] is not None,item+' unresolved receipt '+f['flag']);receipts[f['numeric']].add(item)
  ck('earliest_exact_pickup_access' in rt['map_access'],item+' missing timing boundary')
 if 'item_description_change' in r:
  lines=r['item_description_change']['final_lines'];ck(len(lines)==3 and max(map(len,lines))<=20,item+' Bag description too long')
coll=[{'flag':hex(k),'items':sorted(v)} for k,v in receipts.items() if len(v)>1];ck(not coll,'different stones collide numerically')
cat=B/'appendices/mega-stone-catalog.md';chap=B/'chapters/05-mega-stones-and-legendary-rewards.md';ct=cat.read_text();ck(len(re.findall(r'^## MEGA-\d{3} —',ct,re.M))==len(src),'human per-stone coverage')
for f in [cat,chap]:
 for u in re.findall(r'\]\(([^)]+)\)',f.read_text()):
  if u.startswith(('#','https:','http:')):continue
  path=(f.parent/u.split('#')[0]).resolve();ck(path.exists(),str(f.relative_to(B))+' broken link '+u)
summary={'result':'PASS' if not errs else 'FAIL','native_stones':len(src),'raw_world_route_sites':actual_routes,'item_triggered_form_bindings':formn,'all_stones_have_individual_decisions':True,'numeric_cross_stone_receipt_collisions':coll,'errors':errs,'scope':'Book source bindings, coverage, descriptions, raw source sites and file links only. Not runtime access or game gate requirements.'}
d['validation']=summary;(B/'review/mega-rewards.json').write_text(json.dumps(d,ensure_ascii=False,indent=2)+'\n');print(json.dumps(summary,indent=2))
if errs:raise SystemExit(1)
