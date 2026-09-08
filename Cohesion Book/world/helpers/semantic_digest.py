import json,pathlib
w=pathlib.Path(__file__).resolve().parents[1];d=json.load(open(w/'inventory.raw.json'));idx=json.load(open(w/'script-index.json'));seen=set();rows=[]
for m in d['maps']:
 out=[]
 for e in m['events']:
  root=e.get('root')
  if not root or root in seen or root not in idx:continue
  seen.add(root);body='\n'.join(idx[k]['body'] for k in e.get('local_labels',[]))
  if 'trainerbattle' in body and len(e.get('local_labels',[]))<10:continue
  texts=e.get('text',[])
  out.append(f"{root} @{e.get('data',{}).get('x','-')},{e.get('data',{}).get('y','-')} "+' | '.join(t['text'] for t in texts))
  effects=[]
  for k in e.get('local_labels',[]):
   for line in idx[k]['code']:
    if line.split()[0] in ['giveitem','givemon','giveegg','removeitem','setflag','clearflag','setvar','warp','warpsilent','setwarp','special','specialvar','callnative','checkitem','checkpartymove','checkspecies'] and line not in effects:effects.append(line)
  if effects:out.append('  STATE: '+'; '.join(effects))
 if out:rows.append('## '+m['name']+'\n'+'\n'.join(out))
(w/'packets'/'semantic-all.txt').write_text('\n\n'.join(rows))
print('semantic chars',sum(map(len,rows)),'maps',len(rows))
