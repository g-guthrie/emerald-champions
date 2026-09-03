#!/usr/bin/env python3
"""Three-way: for each Hoenn map label, is Champions == Inclement, == vanilla 1.16.3, or neither?"""
import json, hashlib, collections, sys
from pathlib import Path
import ec_baseline_diff as D
VAN=Path(__file__).resolve().parents[2]/"work"/"audit-baselines"/"vanilla"
def norm_map(root, m):
    mj=root/"data/maps"/m/"map.json"
    if not mj.exists(): return None,None
    j=D.load_json(mj)
    labels,texts=D.parse_script_file(root/"data/maps"/m/"scripts.inc", D.localid_map(j))
    n={k:D.normalize_body(v) for k,v in labels.items() if k not in texts}
    # canonicalize common movement rename + warp id arg
    out={}
    for k,v in n.items():
        body=[]
        for c,a in v:
            a=[x.replace("WalkInPlaceFastest","WalkInPlaceFaster").replace("WalkFastest","WalkFaster") for x in a]
            if c in ("warp","warpsilent","warpdoor","warphole","warpteleport","setwarp","setescapewarp","setdynamicwarp","warpwhitefade","warpmossdeepgym","setholewarp") and len(a)==4 and a[1] in ("255",): a=[a[0],a[2],a[3]]
            body.append((c,a))
        out[k]=body
    return j,out
def h(b): return hashlib.sha1(json.dumps(b).encode()).hexdigest()
if __name__=='__main__':
    maps=D.hoenn_maps()
    stats=collections.Counter()
    rows=[]
    for m in maps:
        cj,cn=norm_map(D.CUR,m); bj,bn=norm_map(D.BASE,m); vj,vn=norm_map(VAN,m)
        if vn is None: vn={}; vj=None
        labels=set(cn)|set(bn)|set(vn)
        for k in labels:
            c=cn.get(k); b=bn.get(k); v=vn.get(k)
            if c is None:
                if b is not None and v is not None and h(b)==h(v): stats["dropped_vanilla_label"]+=1; continue
                if b is not None: rows.append((m,k,"DROPPED_INCLEMENT_ONLY_LABEL")); stats["dropped_inclement_only"]+=1
                continue
            if b is None and v is None: stats["champions_new"]+=1; continue
            eqb = b is not None and h(c)==h(b); eqv = v is not None and h(c)==h(v)
            if eqb: stats["eq_inclement"]+=1
            elif eqv and b is not None and h(b)!=h(v): rows.append((m,k,"REVERTED_TO_VANILLA")); stats["reverted_to_vanilla"]+=1
            elif eqv: stats["eq_vanilla_same_as_inclement_missing"]+=1
            elif b is None: stats["vanilla_label_modified"]+=1
            else: stats["modified_from_inclement"]+=1
    print(stats)
    json.dump(rows,open("threeway_rows.json","w"),indent=1)
    bym=collections.defaultdict(list)
    for m,k,t in rows: bym[m].append((k,t))
    for m in maps:
        if m in bym:
            print(f"## {m}")
            for k,t in bym[m]: print(f"  {t}: {k}")
