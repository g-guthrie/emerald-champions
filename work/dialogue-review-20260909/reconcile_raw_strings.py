import json,re
from pathlib import Path
p=Path('work/dialogue-review-20260909')
report={}
for owner in ['main','npcs_a','npcs_b','shared']:
 files=json.loads((p/f'{owner}-files.json').read_text())
 entries=json.loads((p/f'{owner}-dialogue.json').read_text())
 covered=set()
 for e in entries:
  lines=(p/'before'/e['file']).read_text().splitlines()
  i=e['line']
  while i<len(lines) and (not lines[i].strip() or re.match(r'^\s*\.string\s+',lines[i])):
   if re.match(r'^\s*\.string\s+',lines[i]):covered.add((e['file'],i+1))
   i+=1
 raw=[]
 for f in files:
  label=None;frlg=False;stack=[]
  for i,line in enumerate((p/'before'/f).read_text().splitlines(),1):
   m=re.match(r'^([A-Za-z_][\w.]*):',line)
   if m:label=m[1]
   if re.match(r'^#if\s+IS_FRLG\s*$',line):stack.append(frlg);frlg=True
   elif line.startswith('#else') and stack:frlg=stack[-1]
   elif line.startswith('#endif') and stack:frlg=stack.pop()
   if re.match(r'^\s*\.string\s+',line):raw.append({'file':f,'line':i,'label':label,'text':line.strip(),'inactive_frlg':frlg,'covered_by_extraction':(f,i) in covered})
 excluded=[r for r in raw if r['inactive_frlg']]
 live=[r for r in raw if not r['inactive_frlg']]
 missing=[r for r in live if not r['covered_by_extraction']]
 report[owner]={'files':len(files),'extracted_blocks':len(entries),'unique_labeled_blocks':len({(r['file'],r['label']) for r in live}),'raw_string_lines':len(live),'uncovered_raw_string_lines':len(missing),'inactive_frlg_lines_excluded':len(excluded),'uncovered':missing,'excluded':excluded}
 print(owner,{k:v for k,v in report[owner].items() if k not in ['uncovered','excluded']})
 for r in missing:print(r['file'],r['line'],r['label'],r['text'])
 for r in excluded:print('EXCLUDE FRLG',r['file'],r['line'],r['text'])
(p/'raw-string-reconciliation.json').write_text(json.dumps(report,indent=2)+'\n')
