#!/usr/bin/env python3
"""Spine encoding for pokeemerald map scripts.

Layer 1 (roundtrip.py) proved the IR is a complete model of the .inc format.
Layer 2 compresses that IR into the form a reader actually holds in context:

  * a symbol belonging to this map loses its map prefix and becomes ~Name
  * labels lose the tab/colon ceremony: >Name is global, -Name is local
  * ops lose the leading tab and collapse runs of identical movement steps

Every transform is mechanical and invertible. The contract is unchanged:
  inc -> IR -> spine -> IR -> inc   must be byte-identical.
"""
import re, os, sys, json, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from roundtrip import parse_inc, emit_inc

SYM = re.compile(r'\b[A-Za-z_]\w*\b')

# Constant-prefix dictionary. These prefixes carry no information a reader
# needs -- OBJ_EVENT_GFX_NINJA_BOY says nothing NINJA_BOY doesn't -- but they
# cost 14 bytes every time they appear, and object_event lines carry several.
# Longest first so replacement is unambiguous, and '$' never occurs in a
# non-.string argument, which render() asserts.
ABBREV = [
    ('OBJ_EVENT_GFX_', '$g'), ('MOVEMENT_TYPE_', '$m'), ('EventScript_', '$E'),
    ('METATILE_', '$t'), ('SPECIES_', '$s'), ('MSGBOX_', '$b'),
    ('TRAINER_', '$T'), ('Movement_', '$V'), ('Common_', '$C'),
    ('ITEM_', '$i'), ('FLAG_', '$f'), ('Text_', '$X'),
    ('VAR_', '$v'), ('MAP_', '$M'), ('MUS_', '$u'), ('SE_', '$e'),
]
def abbrev(a):
    for long, short in ABBREV: a = a.replace(long, short)
    return a
def unabbrev(a):
    for long, short in reversed(ABBREV): a = a.replace(short, long)
    return a
MOVE = re.compile(r'^(walk|step|face|slide|jump|delay|lock|unlock|nop)_?\w*$')

def map_name_of(ir):
    """The map prefix is the longest common prefix of this file's own labels."""
    labels = [it['n'] for it in ir['items'] if it['k'] == 'label']
    if not labels: return None
    for suf in ('_MapScripts', '_EventScript_', '_Text_', '_Movement_', '_OnTransition', '_OnLoad'):
        for l in labels:
            i = l.find(suf)
            if i > 0: return l[:i]
    return None

def render(ir):
    mp = map_name_of(ir)
    pre = (mp + '_') if mp else None
    out = [f"#map {mp}" if mp else "#map -"]
    prev_move = None; run = 0
    def flush():
        nonlocal prev_move, run
        if prev_move is not None:
            out.append(f" {prev_move}" + (f" x{run}" if run > 1 else ""))
            prev_move = None; run = 0
    for it in ir['items']:
        k = it['k']
        if k == 'blank': flush(); out.append(""); continue
        if k == 'raw':   flush(); out.append("!" + it['t']); continue
        if k == 'label':
            flush()
            n = it['n']
            short = '~' + n[len(pre):] if pre and n.startswith(pre) else n
            short = abbrev(short)
            out.append(('>' if it['v'] == '::' else '-') + short)
            continue
        op, args = it['o'], it['a']
        if MOVE.match(op) and not args:
            if op == prev_move: run += 1
            else: flush(); prev_move, run = op, 1
            continue
        flush()
        if op != '.string':
            if pre: args = re.sub(r'\b' + re.escape(pre) + r'(\w+)', r'~\1', args)
            assert '$' not in args, f'sigil collision in {args!r}'
            args = abbrev(args)
        out.append(" " + op + (" " + args if args else ""))
    flush()
    meta = {'trailing': ir['trailing'],
            'fmt': [(it.get('i',''), it.get('s',''), it.get('e','')) for it in ir['items'] if it['k']=='op']}
    return '\n'.join(out), meta

def parse_spine(text, meta):
    lines = text.split('\n')
    mp = lines[0][5:].strip()
    pre = (mp + '_') if mp != '-' else None
    items = []; fmt = list(meta['fmt']); fi = 0
    for ln in lines[1:]:
        if ln == '': items.append({'k': 'blank'}); continue
        if ln.startswith('!'): items.append({'k': 'raw', 't': ln[1:]}); continue
        if ln[0] in '>-':
            n = unabbrev(ln[1:])
            if n.startswith('~') and pre: n = pre + n[1:]
            items.append({'k': 'label', 'n': n, 'v': '::' if ln[0] == '>' else ':'})
            continue
        body = ln[1:]
        m = re.match(r'^(\S+)(?: x(\d+))?$', body)
        if m and MOVE.match(m.group(1)) and (m.group(2) or ' ' not in body):
            op, n = m.group(1), int(m.group(2) or 1)
            for _ in range(n):
                i, s, e = fmt[fi]; fi += 1
                items.append({'k': 'op', 'i': i, 'o': op, 's': s, 'a': '', 'e': e})
            continue
        parts = body.split(' ', 1)
        op = parts[0]; args = parts[1] if len(parts) > 1 else ''
        if op != '.string':
            args = unabbrev(args)
            if pre: args = re.sub(r'~(\w+)', pre + r'\1', args)
        i, s, e = fmt[fi]; fi += 1
        items.append({'k': 'op', 'i': i, 'o': op, 's': s, 'a': args, 'e': e})
    return {'items': items, 'trailing': meta['trailing'], 'raw': 0}

if __name__ == '__main__':
    root = sys.argv[1] if len(sys.argv) > 1 else '.'
    files = [os.path.join(r, f) for r, _, fs in os.walk(root) for f in fs if f.endswith('.inc')]
    ok = fail = 0; before = after = 0; errs = collections.Counter()
    for p in sorted(files):
        src = open(p, encoding='utf-8', errors='surrogateescape').read()
        try:
            ir = parse_inc(src)
            sp, meta = render(ir)
            back = emit_inc(parse_spine(sp, meta))
        except Exception as e:
            errs[f'{type(e).__name__}: {str(e)[:40]}'] += 1; fail += 1; continue
        if back == src:
            ok += 1; before += len(src); after += len(sp)
        else:
            fail += 1
            a, b = src.split('\n'), back.split('\n')
            w = 'length'
            for x, y in zip(a, b):
                if x != y: w = repr(x)[:55]; break
            errs[w] += 1
    print(f"spine round-trip over {len(files)} files: {ok} byte-identical, {fail} failed")
    if ok:
        print(f"bytes  {before:,} -> {after:,}   ({100*(1-after/before):.1f}% smaller)")
        print(f"tokens ~{before//4:,} -> ~{after//4:,}")
    if errs:
        print("\nfailures:")
        for k, v in errs.most_common(12): print(f"   {v:5d}  {k}")
