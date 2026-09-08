import json,sys
from pathlib import Path
p=Path(__file__).resolve().parents[2]/'review/early-battles.notes.json'
d=json.loads(p.read_text())
for line in sys.stdin:
 line=line.strip()
 if not line or line.startswith('#'):continue
 v=line.split('|')
 key=v[0]; assert key not in d,key
 d[key]={'assessment':v[1],'acceptance':v[2],'decision':'KEEP','shared_ai_requirements':v[3].split(',') if len(v)>3 and v[3] else []}
 if len(v)>4 and v[4]:d[key].update(decision='REPAIR',proposed_crack=v[4])
p.write_text(json.dumps(d,indent=2)+'\n')
print('Written:',len(d))
