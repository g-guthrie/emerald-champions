import json,pathlib
w=pathlib.Path(__file__).resolve().parents[1];d=json.load(open(w/'inventory.raw.json'));idx=json.load(open(w/'script-index.json'));seen=set();rows=[]
for m in d['maps']:
 out=[];effects=[];groups=[];texts={}
 for e in m['events']:
  root=e.get('root')
  if not root or root in seen or root not in idx:continue
  seen.add(root);body='\n'.join(idx[k]['body'] for k in e.get('local_labels',[]))
  if 'trainerbattle' in body and len(e.get('local_labels',[]))<10:continue
  for t in e.get('text',[]):texts[t['label']]=t['text']
  for k in e.get('local_labels',[]):
   for line in idx[k]['code']:
    if line.split()[0] in ['giveitem','givemon','giveegg','removeitem','setflag','clearflag','setvar','warp','warpsilent','setwarp','special','specialvar','callnative','checkitem','checkpartymove','checkspecies'] and line not in effects:effects.append(line)
  groups.append(root)
 if texts or effects:
  out.append('Roots: '+', '.join(groups))
  out.extend(k+': '+v for k,v in texts.items())
  if effects:out.append('State: '+'; '.join(effects))
 rows.append('## '+m['name']+'\n'+'\n'.join(out))
(w/'packets'/'unique-all.txt').write_text('\n\n'.join(rows))
for i in range(0,len(rows),10):(w/'packets'/f'unique-{i+1:03d}.txt').write_text('\n\n'.join(rows[i:i+10]))
print(len(rows),sum(map(len,rows)))
