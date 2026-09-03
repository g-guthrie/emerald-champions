#!/usr/bin/env python3
"""Four-way: old vanilla (Inclement base), Inclement, new vanilla 1.16.3, Champions.
Reports only labels Inclement actually changed/added and Champions did not keep."""
import json, hashlib, collections, sys
from pathlib import Path
import ec_baseline_diff as D
from threeway import norm_map, h
OLD=Path(__file__).resolve().parent/"oldbase"; VAN=Path(__file__).resolve().parents[2]/"work"/"audit-baselines"/"vanilla"
maps=D.hoenn_maps(); stats=collections.Counter(); rows=[]
for m in maps:
    cj,cn=norm_map(D.CUR,m); bj,bn=norm_map(D.BASE,m); vj,vn=norm_map(VAN,m); oj,on=norm_map(OLD,m)
    vn=vn or {}; on=on or {}
    for k,b in bn.items():
        o=on.get(k); c=cn.get(k); v=vn.get(k)
        incl_changed = (o is None) or h(b)!=h(o)
        if not incl_changed: stats["inclement_untouched"]+=1; continue
        if c is not None and h(c)==h(b): stats["kept"]+=1; continue
        if c is None:
            # maybe renamed: same body under another name?
            if any(h(x)==h(b) for x in cn.values()): stats["kept_renamed"]+=1; continue
            rows.append((m,k,"DROPPED" if o is None else "DROPPED_INCLEMENT_EDIT_OF_VANILLA")); stats["dropped"]+=1; continue
        if v is not None and h(c)==h(v): rows.append((m,k,"REVERTED_TO_VANILLA")); stats["reverted"]+=1; continue
        rows.append((m,k,"MODIFIED")); stats["modified"]+=1
print(stats)
json.dump(rows,open("fourway_rows.json","w"),indent=1)
bym=collections.defaultdict(list)
for m,k,t in rows: bym[m].append((k,t))
for m in maps:
    if m in bym:
        print(f"## {m}")
        for k,t in bym[m]: print(f"  {t}: {k}")
