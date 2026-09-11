#!/usr/bin/env python3
"""Generate the Emerald Champions seam manifest.

Every point where Emerald Champions logic touches inherited pokeemerald-expansion
code, classified by whether it can be relocated into EC-owned files.

Usage: python3 scripts/generate_seam_manifest.py [--out docs/SEAM_MANIFEST.md]
"""
import os,re,sys,json,argparse,subprocess

# Replaced at runtime by main() with a pattern derived from EC-owned symbols.
EC_PAT = re.compile(r'(?!x)x')

# Token patterns that are unambiguously ours regardless of symbol naming.
EC_TOKENS = re.compile(
    r'\bEC_(?!WORD|GROUP|POKEMON|MOVE|EMPTY|MASK|DYNAMIC|NUM|TRAINER|INDEX|CASE)[A-Z]'
    r'|\bFLAG_EC_|\bVAR_EC_|emerald_champions|champions_circuit|legendary_signs')

# Symbols DEFINED by EC-owned files. Derived from the source, never guessed:
# every identifier an inherited file could call to reach our code.
# A function whose NAME marks it as ours, wherever it currently lives.
OWNED_NAME = re.compile(r'EmeraldChampions|ChampionsCircuit|LegendarySign|LegendaryEncounter|LegendaryRelic|LocalLegendary|MegaStoneReward|LevelCap')

DEF_RE = re.compile(
    r'^(?:[A-Za-z_][A-Za-z0-9_]*[ \t\*]+)+([A-Za-z_][A-Za-z0-9_]*)\s*\(', re.M)
DECL_RE = re.compile(
    r'^\s*(?:extern\s+)?(?:[A-Za-z_][A-Za-z0-9_]*[ \t\*]+)+([A-Za-z_][A-Za-z0-9_]*)\s*\([^;]*\)\s*;', re.M)

# Files whose NAME marks them as EC-owned: not seams, they are the corpus.
OWNED = re.compile(
    r'(?:^|/)(?:emerald_champions[a-z_]*|legendary_signs|champions_circuit|'
    r'showdown_champions_circuit|mega_stone_rewards|caps)\.(?:c|h)$')

SYSTEM = [
    ('legendary', re.compile(r'LegendarySign|LEGENDARY_SIGN|legendary_signs')),
    ('levelcap',  re.compile(r'GetCurrentLevelCap|GetLevelCap|GetSoftLevelCapExpValue|EC_LEVEL')),
    ('circuit',   re.compile(r'ChampionsCircuit|CHAMPIONS_CIRCUIT|champions_circuit')),
    ('mega',      re.compile(r'MegaStoneReward|mega_stone_rewards|EC_MEGA')),
    ('battleset', re.compile(r'BattleSet|BATTLE_SET|battle_sets|BattlePlan')),
    ('headless',  re.compile(r'Headless|HEADLESS|headless|AgentPrep|agent_prep')),
    ('economy',   re.compile(r'EvolutionPrice|FreeCatalogue|FreePokemart|BattleItemMart|EvolutionSpecialist|EvolutionItemArchive')),
]
def system_of(text):
    for name,pat in SYSTEM:
        if pat.search(text): return name
    return 'other'

FUNC_DEF = re.compile(r'^[A-Za-z_][A-Za-z0-9_ \t\*]*\b([A-Za-z_][A-Za-z0-9_]*)\s*\([^;]*$')

def enclosing_functions(lines):
    """Return list mapping line index -> enclosing function name (or None)."""
    owner=[None]*len(lines)
    cur=None; depth=0; pend=None
    for i,l in enumerate(lines):
        s=l.rstrip()
        if depth==0:
            m=FUNC_DEF.match(s)
            if m and not s.lstrip().startswith(('#','//','/*')) and 'typedef' not in s:
                pend=m.group(1)
        opens=s.count('{'); closes=s.count('}')
        if depth==0 and opens>0 and pend:
            cur=pend; pend=None
        depth+=opens-closes
        owner[i]=cur
        if depth<=0:
            depth=0; cur=None
    return owner

def classify(line, func, path):
    st=line.strip()
    if st.startswith('#include'): return 'include'
    if path.endswith('.h'):
        if st.startswith(('extern','void','u8','u16','u32','s8','s16','s32','bool8','bool32','const','enum','struct')) and st.endswith(';'):
            return 'declaration'
        if st.startswith('#define'): return 'define'
        return 'header-other'
    if func and OWNED_NAME.search(func): return 'owned-function'
    if func is None: return 'file-scope-data'
    return 'hook'

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('--out',default='docs/SEAM_MANIFEST.md')
    ap.add_argument('--json',default=None)
    ap.add_argument('--root',default='.')
    a=ap.parse_args()
    os.chdir(a.root)
    tracked=[p for p in subprocess.run(['git','ls-files'],capture_output=True,text=True).stdout.split('\n') if p]

    # 1. Harvest every symbol defined or declared by EC-owned files.
    owned_syms=set()
    owned_files=[p for p in tracked if p.startswith(('src/','include/')) and OWNED.search(p)]
    for p in owned_files:
        try: t=open(p,encoding='utf-8',errors='replace').read()
        except OSError: continue
        for m in DEF_RE.finditer(t):  owned_syms.add(m.group(1))
        for m in DECL_RE.finditer(t): owned_syms.add(m.group(1))
    NOISE={'if','for','while','switch','return','sizeof','do','else','case','defined',
           'STATIC_ASSERT','ARRAY_COUNT','AGB_ASSERT','min','max','abs'}
    owned_syms={x for x in owned_syms if x not in NOISE and len(x)>3}
    # Drop ALL_CAPS macro spellings: those are engine macros used by our files,
    # not symbols our files export.
    owned_syms={x for x in owned_syms if not re.fullmatch(r'[A-Z0-9_]+',x)}
    # Drop anything ALSO defined by an inherited file. If the engine defines it,
    # a reference to it is not a seam into our code.
    foreign=set()
    for p2 in tracked:
        if not p2.startswith(('src/','include/')) or not p2.endswith(('.c','.h')): continue
        if OWNED.search(p2) or p2.startswith('src/data/'): continue
        try: t2=open(p2,encoding='utf-8',errors='replace').read()
        except OSError: continue
        for m in DEF_RE.finditer(t2):  foreign.add(m.group(1))
        for m in DECL_RE.finditer(t2): foreign.add(m.group(1))
    dropped=owned_syms & foreign
    owned_syms-=foreign
    if dropped:
        print(f"dropped {len(dropped)} symbols also defined by the engine")
    global EC_PAT
    EC_PAT=re.compile('|'.join([EC_TOKENS.pattern]+
                     [r'\b'+re.escape(x)+r'\b' for x in sorted(owned_syms)]))
    print(f"derived {len(owned_syms)} exported symbols from {len(owned_files)} EC-owned files")

    sites=[]
    for p in tracked:
        if not p.startswith(('src/','include/')) or not p.endswith(('.c','.h')): continue
        if p.startswith('src/data/') or OWNED.search(p): continue
        try: lines=open(p,encoding='utf-8',errors='replace').read().split('\n')
        except OSError: continue
        if not any(EC_PAT.search(l) for l in lines): continue
        owner=enclosing_functions(lines)
        for i,l in enumerate(lines):
            for m in EC_PAT.finditer(l):
                sites.append(dict(file=p,line=i+1,symbol=m.group(0),
                                  func=owner[i],kind=classify(l,owner[i],p),
                                  system=system_of(l),text=l.strip()[:160]))
    # dedupe multiple symbol hits on one line
    seen=set(); uniq=[]
    for s in sites:
        k=(s['file'],s['line'])
        if k in seen: continue
        seen.add(k); uniq.append(s)
    return uniq,a

if __name__=='__main__':
    sites,a=main()
    import collections
    bykind=collections.Counter(s['kind'] for s in sites)
    bysys=collections.Counter(s['system'] for s in sites)
    byfile=collections.Counter(s['file'] for s in sites)
    RELOC={'owned-function','include','declaration','define','file-scope-data','header-other'}
    reloc=[s for s in sites if s['kind'] in RELOC]
    hooks=[s for s in sites if s['kind']=='hook']
    print(f"seam sites (unique lines): {len(sites)}")
    print(f"  relocatable: {len(reloc)}")
    print(f"  irreducible hooks: {len(hooks)}  across {len(set(h['file'] for h in hooks))} files")
    print("\nby kind:")
    for k,v in bykind.most_common(): print(f"   {v:5d}  {k}")
    print("\nby system:")
    for k,v in bysys.most_common(): print(f"   {v:5d}  {k}")
    print("\nHOOK files (the irreducible floor):")
    hf=collections.Counter(h['file'] for h in hooks)
    for k,v in hf.most_common(25): print(f"   {v:4d}  {k}")
    print("\nOWNED-FUNCTION files (whole functions that can move out):")
    of=collections.Counter(s['file'] for s in sites if s['kind']=='owned-function')
    for k,v in of.most_common(15): print(f"   {v:4d}  {k}")
    funcs=sorted(set((s['file'],s['func']) for s in sites if s['kind']=='owned-function'))
    print(f"\ndistinct relocatable functions: {len(funcs)}")
    json.dump(sites,open('/tmp/seams.json','w'))

    # ---- markdown manifest ----
    os.makedirs(os.path.dirname(a.out) or '.',exist_ok=True)
    with open(a.out,'w',encoding='utf-8') as f:
        f.write("# Emerald Champions seam manifest\n\n")
        f.write("GENERATED by `scripts/generate_seam_manifest.py`. Do not edit by hand.\n\n")
        f.write("Every point where Emerald Champions logic touches inherited pokeemerald-expansion\n")
        f.write("code. Regenerate after any change that adds or moves a hook.\n\n")
        f.write(f"- **{len(sites)}** seam sites across **{len(byfile)}** inherited files\n")
        f.write(f"- **{len(hooks)}** irreducible hooks (branch points inside engine functions)\n")
        f.write(f"- **{len(reloc)}** relocatable (whole functions, data, defines, includes)\n")
        f.write(f"- **{len(funcs)}** whole EC functions currently living in inherited files\n\n")
        f.write("## Irreducible hooks\n\nThese must stay where they are; keep each to a single call.\n\n")
        f.write("| file | line | enclosing function | system | code |\n|---|---|---|---|---|\n")
        for s_ in sorted(hooks,key=lambda x:(x['file'],x['line'])):
            code=s_['text'].replace('|','\\|')[:100]
            f.write(f"| `{s_['file']}` | {s_['line']} | `{s_['func']}` | {s_['system']} | `{code}` |\n")
        f.write("\n## Whole EC functions inside inherited files\n\nThese can be relocated into EC-owned files.\n\n")
        f.write("| file | function |\n|---|---|\n")
        for fpath,fn in funcs:
            f.write(f"| `{fpath}` | `{fn}` |\n")
    print(f"\nwrote {a.out}")
