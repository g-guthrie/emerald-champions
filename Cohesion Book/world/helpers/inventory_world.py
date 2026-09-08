import json,re,pathlib,collections,hashlib
BOOK=pathlib.Path(__file__).resolve().parents[2]; ROOT=BOOK/'baseline/source'; OUT=BOOK/'world'
groups=json.loads((ROOT/'data/maps/map_groups.json').read_text()); names=[n for g in groups['group_order'] for n in groups[g] if '_Frlg' not in n]
labels={};paths=list((ROOT/'data').rglob('*.inc'))+list((ROOT/'data').rglob('*.s'))+list((ROOT/'asm').rglob('*.s'))
for p in paths:
 lines=p.read_text(errors='replace').splitlines(); marks=[]
 for i,l in enumerate(lines):
  m=re.match(r'^([A-Za-z_][A-Za-z0-9_]*)::?',l)
  if m: marks.append((i,m[1]))
 for j,(i,key) in enumerate(marks):
  end=marks[j+1][0] if j+1<len(marks) else len(lines)
  body='\n'.join(lines[i+1:end]); code=[x.split('@',1)[0].strip() for x in lines[i+1:end] if x.strip() and not x.lstrip().startswith('@')]
  labels[key]={'path':str(p.relative_to(ROOT)),'line':i+1,'end_line':end,'body':body,'code':code,'next':marks[j+1][1] if j+1<len(marks) else None}
for k,b in labels.items():
 code='\n'.join(b['code']); refs=list(dict.fromkeys(x for x in re.findall(r'\b[A-Za-z_]\w*\b', re.sub(r'"(?:\\.|[^"\\])*"','',code)) if x in labels and x!=k))
 last=b['code'][-1].split()[0] if b['code'] else ''
 if last not in {'end','return','endselectionscript','endmovement','goto','goto_if_unset','goto_if_set','jumpstd','goto_ram','endram','end_macro','releaseall','release','pokemartlistend','step_end'} and not last.startswith('.') and b['next'] and '_Text_' not in k and '_Movement_' not in k and b['next'] not in refs: refs.append(b['next'])
 b['refs']=refs
 b['text']=' '.join(re.findall(r'\.string\s+"(.*)"',b['body'])).replace('\\n',' / ').replace('\\l',' / ').replace('\\p',' // ').replace('$','')

def closure(root,path,cap=500):
 seen=set(); todo=[root]; external=[]
 while todo and len(seen)<cap:
  k=todo.pop()
  if k in seen or k not in labels:continue
  b=labels[k]
  if b['path']!=path and k!=root:
   external.append(k);continue
  seen.add(k);todo.extend(b['refs'])
 return sorted(seen,key=lambda k:labels[k]['line']),list(dict.fromkeys(external))
allmaps=[]; counts=collections.Counter(); rootcounts=collections.Counter()
for n in names:
 mp=ROOT/'data/maps'/n/'map.json'; m=json.loads(mp.read_text()); p=f'data/maps/{n}/scripts.inc'; sp=ROOT/p
 roots=[]
 for section in ['object_events','coord_events','bg_events','warp_events','connections']:
  for i,e in enumerate(m.get(section) or []):
   counts[section]+=1;key=f'{n}:{section}:{i+1:03d}';r={'id':key,'kind':section,'index':i+1,'data':e,'source':str(mp.relative_to(ROOT))}
   script=e.get('script');r['root']=script
   if script in labels:
    local,external=closure(script,p);r['local_labels']=local;r['external_labels']=external;r['text']=[{'label':k,'text':labels[k]['text']} for k in local if labels[k]['text']];r['script_source']={a:labels[script][a] for a in ['path','line','end_line']};rootcounts[script]+=1
   roots.append(r)
 map_script_root=f'{n}_MapScripts'
 if sp.exists():
  text=sp.read_text(); mr=re.search(r'(?m)^([A-Za-z_]\w*MapScripts)::?',text)
  if mr:map_script_root=mr[1]
  for num,line in enumerate(text.splitlines(),1):
   ma=re.match(r'\s*map_script\s+(\w+),\s*(\w+)',line)
   if ma:
    key=f'{n}:map_scripts:{len([r for r in roots if r["kind"]=="map_scripts"])+1:03d}';script=ma[2];local,external=closure(script,p);roots.append({'id':key,'kind':'map_scripts','root':script,'data':{'type':ma[1]},'source':p,'source_line':num,'local_labels':local,'external_labels':external,'text':[{'label':k,'text':labels[k]['text']} for k in local if labels[k]['text']],'script_source':{a:labels[script][a] for a in ['path','line','end_line']}});counts['map_scripts']+=1
 allmaps.append({'name':n,'source':str(mp.relative_to(ROOT)),'map':m,'script_path':p if sp.exists() else None,'map_script_root':map_script_root,'events':roots})
(OUT/'inventory.raw.json').write_text(json.dumps({'counts':dict(counts),'maps':allmaps},indent=2)+'\n')
(OUT/'script-index.json').write_text(json.dumps(labels,indent=2)+'\n')
(OUT/'shared-roots.json').write_text(json.dumps(rootcounts.most_common(),indent=2)+'\n')
# Packet: exact all text and all nontext script bodies per map, omitting movement instructions and trainer loadouts (other volume).
for idx in range(0,len(allmaps),20):
 rows=[]
 for m in allmaps[idx:idx+20]:
  n=m['name'];rows.append(f'\n## {n}\n'+json.dumps({k:v for k,v in m['map'].items() if k not in ['object_events','coord_events','bg_events','warp_events','connections']},ensure_ascii=False))
  for e in m['events']:
   rows.append(e['id']+' '+json.dumps(e['data'],ensure_ascii=False))
  used=set()
  for k,b in labels.items():
   if b['path']!=m['script_path']:continue
   if b['text']:rows.append(f'TEXT {k}:{b["line"]} '+b['text']);continue
   code=[l for l in b['code'] if l and not l.startswith(('.', '@'))]
   if '_Movement_' in k:rows.append(f'MOVE {k}: '+'; '.join(code));continue
   rows.append(f'SCRIPT {k}:{b["line"]} '+'; '.join(code))
 (OUT/'packets'/f'{idx+1:03d}-{min(idx+20,len(allmaps)):03d}.txt').write_text('\n'.join(rows)+'\n')
print(json.dumps({'maps':len(allmaps),'counts':dict(counts),'labels':len(labels),'source_paths':len(paths),'packets':(len(allmaps)+19)//20},indent=2))
