#!/usr/bin/env python3
"""Convert the Emerald Champions battle-set presets to a compact line format.

Fixed columns for the ten fields every preset has; a key=value tail for the
optional ones. Absent, empty and null are three distinct states and the
encoding keeps them apart -- 2,718 presets omit field_dependency entirely,
others set it to null, and five carry a required_move nothing else has.

Contract: text -> data must equal the original JSON structure exactly.
"""
import json, sys, os

FIXED = ['species','name','moves','nature','ability','item','required_item','evs','role','source']
STRIP = {'species':'SPECIES_','nature':'NATURE_','ability':'ABILITY_',
         'item':'ITEM_','required_item':'ITEM_'}
ESC = {'|':'&p;', '=':'&e;', ';':'&s;', '\n':'&n;', '&':'&a;'}
def esc(s):
    s = str(s).replace('&','&a;')
    for k,v in ESC.items():
        if k != '&': s = s.replace(k,v)
    return s
def unesc(s):
    for k,v in ESC.items():
        if k != '&': s = s.replace(v,k)
    return s.replace('&a;','&')
NULL = '\\N'

def enc(p):
    cols = []
    for f in FIXED:
        v = p.get(f)
        if f == 'moves':   v = ','.join(m.replace('MOVE_','') for m in (v or []))
        elif f == 'evs':   v = '/'.join(str(x) for x in (v or []))
        elif f in STRIP:   v = '' if v is None else str(v).replace(STRIP[f],'')
        else:              v = '' if v is None else str(v)
        cols.append(esc(v))
    tail = []
    for k in sorted(p):
        if k in FIXED: continue
        v = p[k]
        tail.append(f"{esc(k)}={NULL if v is None else esc(str(v))}")
    line = '|'.join(cols)
    if tail: line += '|;' + ';'.join(tail)
    return line

def dec(line):
    parts = line.split('|')
    tail = []
    if len(parts) > len(FIXED) and parts[len(FIXED)].startswith(';'):
        tail = parts[len(FIXED)][1:].split(';') if len(parts[len(FIXED)]) > 1 else []
    p = {}
    for f, v in zip(FIXED, parts):
        v = unesc(v)
        if f == 'moves':   p[f] = ['MOVE_'+m for m in v.split(',')] if v else []
        elif f == 'evs':   p[f] = [int(x) for x in v.split('/')] if v else []
        elif f in STRIP:   p[f] = STRIP[f] + v
        else:              p[f] = v
    for t in tail:
        if not t: continue
        k, _, v = t.partition('=')
        p[unesc(k)] = None if v == NULL else unesc(v)
    return p

if __name__ == '__main__':
    src = sys.argv[1]
    d = json.load(open(src))
    buckets = [k for k in d if isinstance(d[k], list)]
    lines, n = [], 0
    for b in buckets:
        lines.append(f'#{b}')
        for p in d[b]:
            lines.append(enc(p)); n += 1
    text = '\n'.join(lines) + '\n'
    back, cur = {}, None
    for ln in text.split('\n'):
        if not ln: continue
        if ln.startswith('#'): cur = ln[1:]; back[cur] = []; continue
        back[cur].append(dec(ln))
    ok = all(back[b] == d[b] for b in buckets)
    orig = os.path.getsize(src)
    print(f"{os.path.basename(src)}")
    print(f"  presets: {n:,} in {len(buckets)} buckets")
    print(f"  lossless round-trip: {'PASS' if ok else 'FAIL'}")
    if not ok:
        for b in buckets:
            for x, y in zip(back[b], d[b]):
                if x != y:
                    diff = [k for k in set(x)|set(y) if x.get(k) != y.get(k)]
                    print(f"   mismatch in {b} on fields {diff}")
                    for k in diff[:3]:
                        print(f"     {k}: orig={y.get(k,'<absent>')!r} back={x.get(k,'<absent>')!r}")
                    sys.exit(1)
    print(f"  bytes: {orig:,} -> {len(text):,}  ({orig/max(len(text),1):.1f}x smaller)")
    print(f"  tokens: ~{orig//4:,} -> ~{len(text)//4:,}")
    noprov = '\n'.join(l if l.startswith('#') else l.replace('|'+l.split('|')[9],'|',1) if len(l.split('|'))>9 else l for l in text.split('\n'))
    print(f"  dropping 'source' provenance would give ~{len(noprov)//4:,} tokens")
    if len(sys.argv) > 2: open(sys.argv[2],'w').write(text)
