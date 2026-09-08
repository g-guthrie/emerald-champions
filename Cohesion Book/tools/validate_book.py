#!/usr/bin/env python3
"""Validate this book's coverage and artifact integrity, not game design quotas."""
from pathlib import Path
import json,re,hashlib,urllib.parse,collections,datetime
B=Path(__file__).resolve().parents[1];R=B/'baseline/source'
def load(p):return json.loads((B/p).read_text())
errors=[];warnings=[]
source=load('inventory/trainers.json');expected={x['trainer']:x for x in source};e=load('review/early-battles.json');l=load('battles/late/review.json')
assert not set(e)&set(l)
reviews={**e,**l}
master=(R/'data/emerald_champions/emerald_champions_master_battle_design.txt').read_text()
formats={}
for block in re.split(r'(?m)^--- BRANCH ',master)[1:]:
 tid=re.search(r'^trainer_id: (TRAINER_\w+)',block,re.M); fmt=re.search(r'^format: (\w+)',block,re.M)
 if tid and fmt:formats[tid[1]]=fmt[1]

if set(reviews)!=set(expected):errors.append({'trainer_coverage':{'missing':sorted(set(expected)-set(reviews)),'extra':sorted(set(reviews)-set(expected))}})
def norm(m):
 def clean(s,p):return str(s).removeprefix(p)
 ps=m.get('points',m.get('stat_points'));ps=[int(v)for v in ps.split('/')]if isinstance(ps,str)else ps
 return {'species':clean(m['species'],'SPECIES_'),'item':clean(m['item'],'ITEM_'),'ability':clean(m['ability'],'ABILITY_'),'nature':clean(m['nature'],'NATURE_'),'points':ps,'offset':m.get('offset',m.get('level_offset')),'moves':[clean(v,'MOVE_')for v in m['moves']]}
combined={};changed=[];counts=collections.Counter();sp=load('inventory/species.json');aliases=load('inventory/species-aliases.json')
def resolve(s):
 seen=set()
 while s in aliases and s not in seen:seen.add(s);s=aliases[s]
 return s
for tid,rec in reviews.items():
 baseline=[norm(m)for m in expected[tid]['mons']];final=[norm(m)for m in rec['final_team']];dis=rec.get('decision',rec.get('disposition'));counts[dis]+=1
 if baseline!=final:changed.append(tid)
 if len(final)<4 and len(final)!=3:errors.append({'party_size':tid,'size':len(final)})
 if len(final)==3 and formats.get(tid)!='multi':errors.append({'unexplained_three_mon_component':tid,'format':formats.get(tid)})
 if expected[tid]['encounter']==1 and len(final)!=4:errors.append({'opening_size':tid})
 for mon in final:
  if not isinstance(mon['points'],list)or len(mon['points'])!=6 or sum(mon['points'])>66 or any(p<0 or p>32 for p in mon['points']):errors.append({'points':tid,'mon':mon})
  if len(mon['moves'])>4 or len(set(mon['moves']))!=len(mon['moves']):errors.append({'moves_shape':tid,'mon':mon})
  sid=resolve('SPECIES_'+mon['species']);row=sp.get(sid)
  if row is None:errors.append({'configured_species':tid,'species':sid})
  elif 'ABILITY_'+mon['ability']not in row['abilities']:errors.append({'configured_ability':tid,'species':sid,'ability':mon['ability'],'allowed':row['abilities']})
  authored=int(expected[tid]['metadata']['strict_cap'])+mon['offset']
  if not 1<=authored<=100:errors.append({'authored_level':tid,'level':authored})
 combined[tid]={'encounter':expected[tid]['encounter'],'decision':dis,'baseline':baseline,'final':final,'loadout_changed':baseline!=final,'assessment':rec['assessment'],'chapter_volume':'early'if tid in e else'late','shared_contracts':['DIFF-01','TEAM-01','POINTS-01','SAI-01..12']}
(B/'review/all-trainers.json').write_text(json.dumps(combined,indent=2,ensure_ascii=False)+'\n')
world=load('world/event-ledger.json');maps=load('inventory/maps.json');wild=load('review/wild-distribution.json');mega=load('review/mega-rewards.json')
if set(world['maps'])!=set(maps):errors.append('world map ledger coverage')
event_counts=collections.Counter(x['kind'] for row in world['maps'].values() for x in row['events'].values())
if dict(event_counts)!=world['scope']:errors.append({'world_event_counts':dict(event_counts),'expected':world['scope']})
for name in maps:
 if not (B/'world/maps'/f'{name}.md').is_file():errors.append({'missing_world_page':name})
if set(wild)!={x['map']for x in load('inventory/wild.json')}:errors.append('wild map coverage')
if len(mega['stones'])!=len(load('inventory/mega-rewards.json')):errors.append('Mega coverage')
# Snapshot equality is an evidence binding, not a future-game prohibition.
manifest=load('baseline/manifest.json');bad=[]
for rel,info in manifest['files'].items():
 p=R/rel
 if not p.exists()or hashlib.sha256(p.read_bytes()).hexdigest()!=info['sha256']:bad.append(rel)
if bad:errors.append({'snapshot_mutated':bad})
# Check ordinary probabilities on the complete proposed final methods.
method_count=0
for mid,rec in wild.items():
 for h in rec['header_rows']:
  for method,m in h['methods'].items():
   method_count+=1;slots=m['final']['slots']
   if sum(x['weight']for x in slots)!=100:errors.append({'probability_sum':mid,'method':method})
   for rev in [False,True]:
    totals=collections.Counter()
    for weightmon,mon in zip(slots,list(reversed(slots))if rev else slots):totals[mon['species']]+=weightmon['weight']
    for species,pct in totals.items():
     if species!='SPECIES_FEEBAS' and pct<5:errors.append({'probability_floor':mid,'method':method,'species':species,'percent':pct,'reversed':rev})
# Only authored book Markdown is checked; frozen historical docs are not rewritten.
links=0;broken=[];mdfiles=[]
for p in B.rglob('*.md'):
 if 'baseline'in p.relative_to(B).parts:continue
 mdfiles.append(p);text=p.read_text();text=re.sub(r'```.*?```','',text,flags=re.S)
 for m in re.finditer(r'\[[^\]]*\]\((<[^>]+>|[^)]+)\)',text):
  target=m[1].strip().strip('<>')
  if target.startswith(('http:','https:','mailto:','app:','#')):continue
  target=target.split(' "',1)[0];target=urllib.parse.unquote(target.split('#',1)[0]);target=re.sub(r':\d+(?::\d+)?$','',target)
  if not target:continue
  q=Path(target);q=q if q.is_absolute()else p.parent/q;links+=1
  if not q.exists():broken.append({'from':str(p.relative_to(B)),'target':m[1]})
if broken:errors.append({'broken_links':broken})
opening=load('review/opening.json');openingv=opening['verification']
if openingv.get('ordered_choices')!=54 or not openingv.get('rival_never_owned'):errors.append({'opening_matrix':openingv})
formsfile=B/'review/acquisition-forms.json';forms=load('review/acquisition-forms.json')if formsfile.exists()else None
if forms is None:errors.append('missing forms volume')
elif set(forms['species'])!=set(sp):errors.append('configured form review coverage')
alloc=load('review/state-allocations.json')
for field in ['flags','variables']:
 if len({x['address']for x in alloc[field]})!=len(alloc[field]):errors.append({'duplicate_state_address':field})
dialogue={(x['source'],x['label'])for x in load('battles/early/gym-dialogue.json')}
world_dialogue={(t.get('source'),t.get('label'))for p in load('world/proposed-edits.json')['proposals']for t in p['targets']if t.get('label')}
if dialogue&world_dialogue:errors.append({'conflicting_world_gym_target':sorted(dialogue&world_dialogue)})
report={'checked_at_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'scope':'Book consistency, source snapshot and source-derived calculations only. No game source edits, ROM build or new native battle execution. Counts bind this book, not future game design.','trainer_branches':len(reviews),'encounter_groups':len({x['encounter']for x in source}),'baseline_pokemon_slots':sum(len(x['mons'])for x in source),'proposed_pokemon_slots':sum(len(x['final'])for x in combined.values()),'preserved_complete_loadouts':len(reviews)-len(changed),'changed_loadout_branches':changed,'review_dispositions':dict(counts),'registered_maps':len(maps),'world_ledger_type':type(world).__name__,'wild_maps':len(wild),'wild_method_records_checked':method_count,'Mega_stones':len(mega['stones']),'snapshot_files_verified':len(manifest['files']),'mutated_snapshot_files':bad,'book_markdown_files':len(mdfiles),'book_markdown_bytes':sum(p.stat().st_size for p in mdfiles),'local_links_checked':links,'forms_volume_present':forms is not None,'warnings':warnings,'errors':errors,'status':'PASS'if not errors else'FAIL'}
(B/'review/book-validation.json').write_text(json.dumps(report,indent=2,ensure_ascii=False)+'\n')
print(json.dumps({k:v for k,v in report.items()if k not in ['errors','changed_loadout_branches']},indent=2));print('errors',json.dumps(errors,ensure_ascii=False)[:5000])
raise SystemExit(bool(errors))
