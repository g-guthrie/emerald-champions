import json,pathlib,re
w=pathlib.Path(__file__).resolve().parents[1];d=json.load(open(w/'inventory.raw.json'));idx=json.load(open(w/'script-index.json'));seen=set();rows=[]
for m in d['maps']:
 out=[];local=set(k for e in m['events'] for k in e.get('local_labels',[]))
 for k,b in idx.items():
  if b['path']!=m['script_path']:continue
  if b['text'] and k not in seen:
   seen.add(k);out.append(('TEXT' if k in local else 'UNROOTED_TEXT')+' '+k+': '+b['text'])
 effects=[]
 for k,b in idx.items():
  if b['path']!=m['script_path']:continue
  ef=[l for l in b['code'] if l.split()[0] in ['giveitem','givemon','giveegg','removeitem','setflag','clearflag','setvar','warp','warpsilent','setwarp','special','specialvar','callnative','checkitem','checkpartymove','checkspecies']]
  if ef:out.append('EFFECT '+k+': '+'; '.join(ef))
 out.insert(0,'ROOTS: '+'; '.join(f"{e['id'].split(':')[1]}:{e['index'] if 'index' in e else '-'} {e.get('root') or e['data'].get('dest_map') or e['data'].get('map') or e['data'].get('item') or e['data'].get('weather')}" for e in m['events']))
 rows.append('## '+m['name']+'\n'+'\n'.join(out))
(w/'packets'/'compact-all.txt').write_text('\n\n'.join(rows))
for i in range(0,len(rows),10):(w/'packets'/f'compact-{i+1:03d}.txt').write_text('\n\n'.join(rows[i:i+10]))
print(len(rows),sum(map(len,rows)))
