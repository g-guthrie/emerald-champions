#!/usr/bin/env python3
"""Generate the Emerald Champions seam manifest.

Every point where Emerald Champions logic touches inherited pokeemerald-expansion
code, classified by whether it can be relocated into EC-owned files.

Usage: python3 scripts/generate_seam_manifest.py [--out docs/SEAM_MANIFEST.md]
"""
import os,re,sys,json,argparse,subprocess

EC_PAT = re.compile(
    r'EmeraldChampions|emerald_champions|LegendarySign|LEGENDARY_SIGN|'
    r'GetCurrentLevelCap|GetLevelCap|GetSoftLevelCapExpValue|ChampionsCircuit|'
    r'CHAMPIONS_CIRCUIT|MegaStoneReward|mega_stone_rewards|'
    r'\bEC_(?!WORD|GROUP|POKEMON|MOVE|EMPTY|MASK|DYNAMIC|NUM|TRAINER|INDEX|CASE)[A-Z]')

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
    if func and EC_PAT.search(func): return 'owned-function'
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
