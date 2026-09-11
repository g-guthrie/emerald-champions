#!/usr/bin/env python3
"""Lossless structured model of a pokeemerald map script.

parse_inc : text -> IR   emit_inc : IR -> text
Contract: emit_inc(parse_inc(x)) == x, byte for byte, for every .inc in the
tree. Until that holds the IR is not a complete model and a spine built on it
would silently lose content.

Line kinds:
  label   a script entry point            Foo::  /  Foo:
  op      an indented script command      \tmsgbox Foo_Text_Bar, MSGBOX_DEFAULT
  blank   positional empty line
  raw     comments and assembler directives, preserved verbatim and counted
"""
import re, os, sys, json, collections

LABEL = re.compile(r'^(\w+)(::?)\s*$')
OP    = re.compile(r'^([ \t]+)(\S+)([ \t]*)(.*?)([ \t]*)$')

def parse_inc(text):
    lines = text.split('\n')
    trailing = lines and lines[-1] == ''
    if trailing: lines = lines[:-1]
    items, raw_count = [], 0
    for ln in lines:
        if ln == '':
            items.append({'k': 'blank'}); continue
        st = ln.lstrip()
        if st.startswith('@') or st.startswith('#') or st.startswith('.') and LABEL.match(ln) is None and not ln.startswith((' ', '\t')):
            items.append({'k': 'raw', 't': ln}); raw_count += 1; continue
        m = LABEL.match(ln)
        if m:
            items.append({'k': 'label', 'n': m.group(1), 'v': m.group(2)}); continue
        m = OP.match(ln)
        if m:
            ind, op, sep, args, tail = m.groups()
            if op.startswith('@'):
                items.append({'k': 'raw', 't': ln}); raw_count += 1; continue
            items.append({'k': 'op', 'i': ind, 'o': op, 's': sep, 'a': args, 'e': tail})
            continue
        items.append({'k': 'raw', 't': ln}); raw_count += 1
    return {'items': items, 'trailing': trailing, 'raw': raw_count}

def emit_inc(ir):
    out = []
    for it in ir['items']:
        k = it['k']
        if   k == 'blank': out.append('')
        elif k == 'raw':   out.append(it['t'])
        elif k == 'label': out.append(f"{it['n']}{it['v']}")
        else:              out.append(f"{it['i']}{it['o']}{it['s']}{it['a']}{it['e']}")
    text = '\n'.join(out)
    if ir['trailing']: text += '\n'
    return text

if __name__ == '__main__':
    root = sys.argv[1] if len(sys.argv) > 1 else '.'
    files = [os.path.join(r, f) for r, _, fs in os.walk(root) for f in fs if f.endswith('.inc')]
    ok = fail = 0; raws = 0; ops = collections.Counter(); errs = collections.Counter()
    for p in sorted(files):
        src = open(p, encoding='utf-8', errors='surrogateescape').read()
        try:
            ir = parse_inc(src); back = emit_inc(ir)
        except Exception as e:
            errs[f'parse:{type(e).__name__}'] += 1; fail += 1; continue
        if back == src:
            ok += 1; raws += ir['raw']
            for it in ir['items']:
                if it['k'] == 'op': ops[it['o']] += 1
        else:
            fail += 1
            a, b = src.split('\n'), back.split('\n')
            w = 'length-mismatch'
            for x, y in zip(a, b):
                if x != y: w = repr(x)[:60]; break
            errs[w] += 1
    print(f"round-trip over {len(files)} .inc files: {ok} byte-identical, {fail} failed")
    print(f"comment/directive lines preserved as raw: {raws:,}")
    print(f"distinct script opcodes in the corpus: {len(ops)}")
    print("\ntop opcodes:")
    for k, v in ops.most_common(25): print(f"   {v:7,}  {k}")
    if errs:
        print("\nfailures:")
        for k, v in errs.most_common(12): print(f"   {v:5d}  {k}")
    json.dump(dict(ops), open('/tmp/opcodes.json', 'w'))
