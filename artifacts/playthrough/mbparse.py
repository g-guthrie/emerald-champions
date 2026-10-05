import re
def parse_mb(path='include/constants/metatile_behaviors.h'):
    t = open(path).read()
    t = re.sub(r'//[^\n]*', '', t); t = re.sub(r'/\*.*?\*/', '', t, flags=re.S)
    body = t[t.index('enum'):]; body = body[body.index('{') + 1: body.index('}')]
    mb, v = {}, 0
    for e in body.split(','):
        e = e.strip()
        if not e: continue
        if '=' in e:
            n, val = [s.strip() for s in e.split('=')]; v = int(val, 0)
        else: n = e
        mb[n] = v; v += 1
    return mb
