import json,sys
from pathlib import Path
root=Path(__file__).resolve().parents[2]
bs=json.loads((Path(__file__).parent/'baseline.json').read_text())
a,z=map(int,sys.argv[1:3])
for b in bs:
 if not a<=b['encounter']<=z:continue
 print(f"\nE{b['encounter']:04} {b['trainer']} {b['cls']} AI={b['ai']}")
 print('PLAN',b['plan']); print('CRACK',b['crack'])
 for i,m in enumerate(b['mons'],1): print(i,m['species'],m['item'],m['ability'],m['nature'],m['points'],m['offset'],','.join(m['moves']))
