import json, difflib
from pathlib import Path
import ec_baseline_diff as D
from threeway import norm_map
OLD=Path(__file__).resolve().parents[2]/"work"/"audit-baselines"/"oldbase"; VAN=Path(__file__).resolve().parents[2]/"work"/"audit-baselines"/"vanilla"
rows=json.load(open("fourway_rows.json"))
def fmt(b): return [c+" "+", ".join(a) for c,a in b]
cache={}
def get(root,m):
    if (root,m) not in cache: cache[(root,m)]=norm_map(root,m)[1] or {}
    return cache[(root,m)]
for m,k,t in rows:
    if t!="REVERTED_TO_VANILLA": continue
    o=get(OLD,m).get(k,[]); b=get(D.BASE,m).get(k,[])
    print(f"===== {m} :: {k}  (what Inclement changed vs its base; Champions now equals vanilla)")
    for line in difflib.unified_diff(fmt(o),fmt(b),"old-vanilla","inclement",lineterm="",n=0):
        if line.startswith(("---","+++","@@")): continue
        print("   "+line)
