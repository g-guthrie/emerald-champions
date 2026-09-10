from pathlib import Path
import json,re,textwrap
base=Path('work/dialogue-review-20260909')
ns={'__file__':str(Path('scripts/audit/textwidth.py').resolve())}
exec(Path('scripts/audit/textwidth.py').read_text().split('worst=[]')[0],ns)
files=[]
for o in ['main','npcs_a','npcs_b','shared']: files.extend(json.loads((base/(o+'-files.json')).read_text()))
records=[]
for f in files:
 if not f.endswith(('.inc','.s')) or f.endswith('/debug.inc'):continue
 p=Path(f);s=p.read_text();subs=[]
 for m in re.finditer(r'(?m)^(\w+)::?\n((?:\s*\.string[^\n]*\n)+)',s):
  raw=''.join(re.findall(r'\.string\s+"(.*)"',m[2]));t=raw.replace(r'\"','"')
  if not t.endswith('$'):continue
  # Formatted menu descriptions use different windows; restrict scrolling repair to trainer dialogue.
  wide=any(ns['line_width'](v.replace('$',''))>208 for v in re.split(r'\\[nlp]',t))
  scroll=('_Gym' in f or f=='data/text/trainers.inc') and any(v.count(r'\n')>1 for v in t.split(r'\p'))
  if not wide and not scroll:continue
  if re.search(r'\{(?:FONT|CLEAR|SKIP|DYNAMIC|COLOR|PAUSE)',t):continue
  paragraphs=[re.sub(r'\\[nl]',' ',v) for v in t[:-1].split(r'\p')]
  chunks=[]
  for pi,para in enumerate(paragraphs):
   lines=textwrap.wrap(para,width=33,break_long_words=False,break_on_hyphens=False)
   for li,line in enumerate(lines):
    end=(r'\n' if li==0 else r'\l') if li<len(lines)-1 else (r'\p' if pi<len(paragraphs)-1 else '$')
    chunks.append('\t.string '+json.dumps(line+end,ensure_ascii=False).replace('\\\\','\\'))
  header=m[0].split('\n',1)[0]+'\n';new=header+'\n'.join(chunks)+'\n'
  # Only whitespace/control placement changes in a reflow.
  norm=lambda z: re.sub(r'\s+',' ',re.sub(r'\\[nlp]',' ',z)).strip()
  newraw=''.join(re.findall(r'\.string\s+"(.*)"',new)).replace(r'\"','"')
  assert norm(t)==norm(newraw),(f,m[1])
  subs.append((m.start(),m.end(),new));records.append({'file':f,'label':m[1],'wide_line':wide,'scrolling':scroll})
 for start,end,new in reversed(subs):s=s[:start]+new+s[end:]
 if subs:p.write_text(s)
(base/'reflow-edits.json').write_text(json.dumps(records,indent=2)+'\n')
print('Reflowed',len(records),'blocks in',len({r['file'] for r in records}),'files')
