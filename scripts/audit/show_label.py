#!/usr/bin/env python3
import sys, json, difflib, hashlib
sys.argv=[sys.argv[0]]+sys.argv[1:]
import ec_baseline_diff as D
def bodies(m):
    cj=D.load_json(D.CUR/"data/maps"/m/"map.json"); bj=D.load_json(D.BASE/"data/maps"/m/"map.json")
    bl,bt=D.parse_script_file(D.BASE/"data/maps"/m/"scripts.inc", D.localid_map(bj))
    cl,ct=D.parse_script_file(D.CUR/"data/maps"/m/"scripts.inc", D.localid_map(cj))
    bn={k:D.normalize_body(v) for k,v in bl.items() if k not in bt}
    cn={k:D.normalize_body(v) for k,v in cl.items() if k not in ct}
    return bn,cn,bt,ct
def fmt(body): return [c+" "+", ".join(a) for c,a in body]
m=sys.argv[1]
bn,cn,bt,ct=bodies(m)
for lab in sys.argv[2:]:
    print(f"===== {m} :: {lab}")
    b=fmt(bn.get(lab,[])); c=fmt(cn.get(lab,[]))
    for line in difflib.unified_diff(b,c,"inclement","champions",lineterm="",n=1): print(line)
