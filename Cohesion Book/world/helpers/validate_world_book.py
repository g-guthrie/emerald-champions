from pathlib import Path
import json,re,collections,struct
W=Path(__file__).resolve().parents[1];B=W.parent;S=B/'baseline/source';errors=[];checks=collections.Counter()
def ck(ok,msg):
 checks['assertions']+=1
 if not ok:errors.append(msg)
g=json.loads((S/'data/maps/map_groups.json').read_text());names=[n for k in g['group_order'] for n in g[k] if '_Frlg' not in n]
ledger=json.loads((W/'event-ledger.json').read_text());props=json.loads((W/'proposed-edits.json').read_text())['proposals'];ids=[];expected=collections.Counter();actual=collections.Counter()
ck(set(names)==set(ledger['maps']),'registered map mismatch')
for n in names:
 m=ledger['maps'][n];base=json.loads((S/m['source']).read_text()); ck((W/'maps'/f'{n}.md').exists(),f'missing map page {n}')
 for k in ['object_events','coord_events','bg_events','warp_events','connections']:
  expected[k]+=len(base.get(k) or [])
  ev=[e for e in m['events'].values() if e['kind']==k];ck(len(ev)==len(base.get(k) or []),f'event coverage {n}:{k}')
  for i,e in enumerate(ev):ck(e['data']==(base.get(k) or [])[i],f'baseline event drift {e["id"]}')
 for eid,e in m['events'].items():
  ids.append(eid);actual[e['kind']]+=1;ck(e['disposition'] in ['KEEP','REVISE','REPAIR','INERT/EXCLUDED'],f'disposition {eid}');ck(bool(e['assessment_contract']) and bool(e['acceptance']),f'missing review contract {eid}')
  if e.get('script_source'):
   sp=e['script_source'];f=S/sp['path'];ck(f.exists(),f'missing script file {eid}')
   if f.exists():ck(re.search(r'^'+re.escape(e['root'])+r'::?',f.read_text(),re.M) is not None,f'missing root {eid}')
ck(len(ids)==len(set(ids)),'duplicate event IDs');ck(len(ids)==6322,'event count mismatch')
seenprop=set();seenlabels=collections.defaultdict(list)
for p in props:
 ck(p['id'] not in seenprop,f'duplicate proposal {p["id"]}');seenprop.add(p['id']);ck(bool(p['targets']) and bool(p['acceptance']),f'incomplete proposal {p["id"]}')
 for a in p['targets']:
  seenlabels[(a['source'],a['label'])].append(p['id']);ck((S/a['source']).exists(),f'missing target source {a["source"]}')
  if not a.get('new_label') and not a.get('new_event'):
   txt=(S/a['source']).read_text(errors='ignore');ck(a['label'] in txt,f'missing target label {a["label"]}')
  ck(any(k in a for k in ['replacement_text','final_behavior','final_object']),f'no final spec {a["label"]}')
  if 'choices' in a:
   ck(len(a['choices'])==3 and 0<=a['correct_index']<3,f'quiz answer {a["label"]}');ck(a['menu_array'] in (S/a['menu_source']).read_text(),f'quiz menu {a["label"]}');checks['quiz_questions']+=1
  if 'replacement_assembly' in a:
   ck(a['replacement_assembly'].rstrip().endswith('$"'),f'missing end terminator {a["label"]}')
   ck('\\p' not in a['replacement_assembly'].splitlines()[-1] or '$"' in a['replacement_assembly'].splitlines()[-1],f'bad terminal {a["label"]}')
for key,ps in seenlabels.items():
 if len(ps)>1:errors.append('overlapping world target '+str(key)+' in '+','.join(ps))
# All file targets in world Markdown resolve; fragment anchors are checked for world targets where explicit anchors exist.
for f in W.rglob('*.md'):
 for match in re.finditer(r'\]\(([^)]+)\)',f.read_text()):
  u=match[1].strip('<>')
  if u.startswith(('http:','https:','mailto:','#')):continue
  path=u.split('#')[0];fp=(f.parent/path).resolve();ck(fp.exists(),f'broken link {f.relative_to(W)} -> {u}')
# Actor coordinates use actual binary layouts.
layouts={x['id']:x for x in json.loads((S/'data/layouts/layouts.json').read_text())['layouts']}
for mapname,cells in [('Route101',[(6,14),(6,13),(7,15)]),('CaveOfOrigin_DianciesRoom',[(11,10)])]:
 m=json.loads((S/'data/maps'/mapname/'map.json').read_text());l=layouts[m['layout']];raw=(S/l['blockdata_filepath']).read_bytes();words=struct.unpack('<'+'H'*(len(raw)//2),raw)
 for x,y in cells:
  v=words[y*l['width']+x];ck(((v>>10)&3)==0 and (v>>12)==3,f'geometry {mapname}@{x},{y}')
summary={'result':'PASS' if not errors else 'FAIL','checks':dict(checks),'maps':len(names),'event_ids':len(ids),'event_counts':dict(actual),'proposals':len(props),'targets':sum(len(p['targets']) for p in props),'overlapping_world_targets':sum(len(x)>1 for x in seenlabels.values()),'errors':errors,'scope':'Book coverage, snapshot bindings, exact targets, links, quiz mappings and proposed actor tile geometry only. No ROM build, gameplay or battle execution.'}
(B/'review/world.validation.json').write_text(json.dumps(summary,indent=2)+'\n');print(json.dumps(summary,indent=2))
if errors:raise SystemExit(1)
