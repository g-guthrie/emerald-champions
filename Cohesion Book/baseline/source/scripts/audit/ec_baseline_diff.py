#!/usr/bin/env python3
"""Independent Champions-vs-Inclement baseline diff.

Compares every Hoenn map's events, warps, layout collision and scripts between the
current tree and the Inclement v1.13 baseline extracted into scratch/baseline.
Semantic normalisation collapses the compare+goto_if modernisation, local-ID
renames, numeric formats and text-label renames so that only real behaviour
differences remain.  Output: JSON report + markdown summary.
"""
from __future__ import annotations
import json, re, os, sys, collections, hashlib, struct
from pathlib import Path

CUR = Path(__file__).resolve().parents[2]
S = Path(__file__).resolve().parents[2] / "work" / "audit-baselines"
BASE = S / "inclement"

def load_json(p):
    with open(p) as f:
        return json.load(f)

def hoenn_maps():
    groups = load_json(CUR / "data/maps/map_groups.json")
    maps = []
    for g in groups["group_order"]:
        for m in groups[g]:
            if m.endswith("_Frlg") or "Frlg" in g:
                continue
            if (BASE / "data/maps" / m / "map.json").exists():
                maps.append(m)
    return maps

# ---------------------------------------------------------------- scripts
COND_JUMPS = {"goto_if_eq","goto_if_ne","goto_if_lt","goto_if_gt","goto_if_le","goto_if_ge",
              "call_if_eq","call_if_ne","call_if_lt","call_if_gt","call_if_le","call_if_ge"}
NUM_ALIASES = {"TRUE":"1","FALSE":"0","YES":"1","NO":"0","MSGBOX_DEFAULT":"MSGBOX_DEFAULT",
               "OBJ_EVENT_ID_PLAYER":"255","LOCALID_PLAYER":"255","OBJ_EVENT_ID_CAMERA":"127","LOCALID_CAMERA":"127"}

def strip_comment(line):
    out=[]; inq=False; i=0
    while i < len(line):
        c=line[i]
        if c=='"' : inq = not inq
        if c=='@' and not inq: break
        out.append(c); i+=1
    return "".join(out)

def norm_arg(a, localids):
    a=a.strip()
    if a in localids: return str(localids[a])
    if a in NUM_ALIASES: return NUM_ALIASES[a]
    m=re.fullmatch(r"0x([0-9a-fA-F]+)",a)
    if m: return str(int(m.group(1),16))
    return a

def parse_script_file(path, localids):
    """Return (labels: name->list[(cmd,args)], texts: name->str, order list)."""
    labels=collections.OrderedDict(); texts={}
    cur=None; cur_is_text=False
    if not path.exists():
        return labels, texts
    sets={}
    for raw in path.read_text(errors="ignore").splitlines():
        line=strip_comment(raw).rstrip()
        if not line.strip(): continue
        m=re.match(r"^\s*\.(set|equ)\s+([A-Za-z_][A-Za-z0-9_]*)\s*,\s*(.+)$",line)
        if m:
            v=m.group(3).strip()
            try: sets[m.group(2)]=int(v,0)
            except ValueError: sets[m.group(2)]=v
            continue
        m=re.match(r"^([A-Za-z_][A-Za-z0-9_]*):(:)?\s*$",line)
        if m:
            cur=m.group(1); labels[cur]=[]; cur_is_text=False; continue
        if cur is None: continue
        s=line.strip()
        if s.startswith(".string"):
            mm=re.match(r'\.string\s+"(.*)"\s*$',s)
            texts.setdefault(cur,""); texts[cur]+= (mm.group(1) if mm else s); cur_is_text=True; continue
        if s.startswith(".include") or s.startswith(".align") or s.startswith(".section"): continue
        parts=s.split(None,1)
        cmd=parts[0]
        args=[norm_arg(x,{**localids,**sets}) for x in split_args(parts[1])] if len(parts)>1 else []
        labels[cur].append((cmd,args))
    return labels, texts

def split_args(s):
    out=[]; buf=""; depth=0; inq=False
    for c in s:
        if c=='"': inq=not inq
        if not inq:
            if c in "([": depth+=1
            if c in ")]": depth-=1
            if c==',' and depth==0:
                out.append(buf); buf=""; continue
        buf+=c
    if buf.strip(): out.append(buf)
    return [x.strip() for x in out]

MOVE_RENAMES={}
for d in ("up","down","left","right"):
    MOVE_RENAMES[f"walk_in_place_fastest_{d}"]=f"walk_in_place_faster_{d}"
    MOVE_RENAMES[f"walk_fastest_{d}"]=f"walk_faster_{d}"
    MOVE_RENAMES[f"slide_{d}"]=f"walk_faster_{d}"  # not real; placeholder
def normalize_body(body):
    """Merge compare+conditional jumps, default args, etc."""
    out=[]; pending=None
    for cmd,args in body:
        cmd=MOVE_RENAMES.get(cmd,cmd)
        if cmd=="compare" and len(args)==2:
            pending=args; continue
        if cmd in COND_JUMPS:
            if len(args)==1 and pending is not None:
                out.append((cmd,[pending[0],pending[1],args[0]])); continue
            out.append((cmd,args)); continue
        if cmd in ("goto_if","call_if"):  # legacy checkflag style
            out.append((cmd,args)); continue
        pending=None if cmd not in ("compare",) else pending
        if cmd in ("giveitem","additem","removeitem","checkitem","checkitemspace") and len(args)==2 and args[1]=="1":
            args=args[:1]
        if cmd=="msgbox" and len(args)==1: args=args+["MSGBOX_DEFAULT"]
        if cmd=="release" or cmd=="releaseall": cmd="release"
        if cmd in ("lock","lockall"): cmd="lock"
        out.append((cmd,args))
    return out

TEXT_CMDS={"msgbox","message","messageautoscroll","messageinstant","braillemessage","loadword","bufferstring","showcontestwinner"}
def classify_diff(bb, cb, btexts, ctexts):
    """Classify how two normalised bodies differ."""
    classes=set()
    # compare instruction-by-instruction using difflib on tuples
    import difflib
    bs=[c+" "+" ".join(a) for c,a in bb]; cs=[c+" "+" ".join(a) for c,a in cb]
    sm=difflib.SequenceMatcher(a=bs,b=cs,autojunk=False)
    for tag,i1,i2,j1,j2 in sm.get_opcodes():
        if tag=="equal": continue
        chunk_b=bb[i1:i2]; chunk_c=cb[j1:j2]
        cmds={c for c,_ in chunk_b}|{c for c,_ in chunk_c}
        for c in cmds:
            if c in TEXT_CMDS: classes.add("text")
            elif c.startswith("trainerbattle") or c in ("setwildbattle","dowildbattle","setflag"+"x"): classes.add("trainer")
            elif c in ("setflag","clearflag","setvar","addvar","subvar","copyvar","setorcopyvar","special","specialvar","setrespawn","setdynamicwarp","incrementgamestat","setmetatile","setweather"): classes.add("state")
            elif c in ("giveitem","additem","removeitem","givemon","giveegg","adddecoration","givemoney","removemoney","checkitem","checkmoney","setmonmove"): classes.add("reward")
            elif c in ("applymovement","waitmovement","addobject","removeobject","setobjectxy","setobjectxyperm","setobjectmovementtype","turnobject","showobjectat","hideobjectat","setobjectsubpriority","resetobjectsubpriority","delay","playse","playbgm","fadescreen","fadescreenswapbuffers","fadedefaultbgm","fadenewbgm","playfanfare","waitfanfare","waitse","opendoor","closedoor","waitdooranim","setdooropen","setdoorclosed","faceplayer","lock","release","closemessage","waitmessage","warp","warpsilent","warpdoor","warphole","warpteleport","warpmossdeepgym","setescapewarp","hidemoney","showmoney","updatemoney","end","return"): classes.add("scene")
            elif c.startswith("goto") or c.startswith("call") or c in ("switch","case","waitstate","specialvar","special"): classes.add("flow")
            elif c=="map_script" or c=="map_script_2": classes.add("mapscript")
            elif c.startswith("step_") or c in (".byte",".2byte",".4byte") or c in MOVE_RENAMES.values() or c.startswith(("walk_","face_","jump","emote","delay_","lock_","slide_","run_")): classes.add("movement")
            else: classes.add("other:"+c)
    return classes

# ---------------------------------------------------------------- objects/warps
OBJ_FIELDS=["graphics_id","x","y","elevation","movement_type","movement_range_x","movement_range_y","trainer_type","trainer_sight_or_berry_tree_id","script","flag"]
def obj_sig(o):
    return {k:o.get(k) for k in OBJ_FIELDS}

def localid_map(mapjson):
    ids={}
    for i,o in enumerate(mapjson.get("object_events",[])):
        if o.get("local_id"): ids[o["local_id"]]=i+1
    return ids

def layout_index(root):
    d=load_json(root/"data/layouts/layouts.json")
    return {L["id"]:L for L in d["layouts"]}

def read_blocks(root,L):
    p=root/L["blockdata_filepath"]
    if not p.exists(): return None
    data=p.read_bytes(); w,h=L["width"],L["height"]
    return [[struct.unpack_from("<H",data,(y*w+x)*2)[0] for x in range(w)] for y in range(h)]

def main():
    maps=hoenn_maps()
    cur_layouts=layout_index(CUR); base_layouts=layout_index(BASE)
    report={}; summary=collections.Counter()
    for m in maps:
        cj=load_json(CUR/"data/maps"/m/"map.json"); bj=load_json(BASE/"data/maps"/m/"map.json")
        r={}
        # header fields
        hdr=[]
        for k in ("layout","music","region_map_section","requires_flash","weather","map_type","allow_cycling","allow_escaping","allow_running","show_map_name","floor_number","battle_scene"):
            if cj.get(k)!=bj.get(k): hdr.append((k,bj.get(k),cj.get(k)))
        if hdr: r["header"]=hdr
        # connections
        bc={(c["direction"],c["map"],c["offset"]) for c in (bj.get("connections") or [])}
        cc={(c["direction"],c["map"],c["offset"]) for c in (cj.get("connections") or [])}
        if bc!=cc: r["connections"]={"removed":sorted(bc-cc),"added":sorted(cc-bc)}
        # objects by index
        bo=bj.get("object_events",[]); co=cj.get("object_events",[])
        objs=[]
        for i in range(max(len(bo),len(co))):
            if i>=len(bo): objs.append({"idx":i+1,"change":"added","cur":obj_sig(co[i]),"local_id":co[i].get("local_id")}); continue
            if i>=len(co): objs.append({"idx":i+1,"change":"removed","base":obj_sig(bo[i])}); continue
            a,b=obj_sig(bo[i]),obj_sig(co[i])
            if a!=b:
                objs.append({"idx":i+1,"change":"modified","local_id":co[i].get("local_id"),"fields":{k:(a[k],b[k]) for k in OBJ_FIELDS if a[k]!=b[k]}})
        if objs: r["objects"]=objs
        # warps
        def wsig(w): return (w["x"],w["y"],w["elevation"],w["dest_map"],str(w["dest_warp_id"]))
        bw=[wsig(w) for w in bj.get("warp_events",[])]; cw=[wsig(w) for w in cj.get("warp_events",[])]
        if bw!=cw:
            r["warps"]={"removed":[w for w in bw if w not in cw],"added":[w for w in cw if w not in bw],"reordered": sorted(bw)==sorted(cw)}
        def csig(c): return tuple(sorted((k,str(v)) for k,v in c.items()))
        bce={csig(c) for c in bj.get("coord_events",[])}; cce={csig(c) for c in cj.get("coord_events",[])}
        if bce!=cce: r["coord_events"]={"removed":[dict(x) for x in bce-cce],"added":[dict(x) for x in cce-bce]}
        bbg={csig(c) for c in bj.get("bg_events",[])}; cbg={csig(c) for c in cj.get("bg_events",[])}
        if bbg!=cbg: r["bg_events"]={"removed":[dict(x) for x in bbg-cbg],"added":[dict(x) for x in cbg-bbg]}
        # layout collision
        L=cur_layouts.get(cj["layout"]); BL=base_layouts.get(bj["layout"])
        if L and BL:
            cb=read_blocks(CUR,L); bb=read_blocks(BASE,BL)
            if cb is None or bb is None: r["layout"]="missing blockdata"
            elif (L["width"],L["height"])!=(BL["width"],BL["height"]):
                r["layout"]={"resized":[(BL["width"],BL["height"]),(L["width"],L["height"])]}
            else:
                coll=[]; meta=0
                for y in range(L["height"]):
                    for x in range(L["width"]):
                        a,b=bb[y][x],cb[y][x]
                        if a!=b:
                            if ((a>>10)&3)!=((b>>10)&3): coll.append((x,y,(a>>10)&3,(b>>10)&3))
                            else: meta+=1
                if coll or meta: r["layout"]={"collision_changes":coll,"metatile_only_changes":meta,"tileset":(BL.get("primary_tileset"),BL.get("secondary_tileset"),L.get("primary_tileset"),L.get("secondary_tileset"))}
        # scripts
        blabels,btexts=parse_script_file(BASE/"data/maps"/m/"scripts.inc", localid_map(bj))
        clabels,ctexts=parse_script_file(CUR/"data/maps"/m/"scripts.inc", localid_map(cj))
        bn={k:normalize_body(v) for k,v in blabels.items() if k not in btexts}
        cn={k:normalize_body(v) for k,v in clabels.items() if k not in ctexts}
        # replace references to movement labels and text labels by content hashes so renames vanish
        def refhash(labels_norm, texts):
            h={}
            for k,v in labels_norm.items():
                MOVE_PREFIX=("walk","step","face","jump","delay","emote","set_","lock_","unlock","slide","run","exclamation","question","hide","show","end_movement","set_invisible","set_visible","reveal","player","ride","enable","disable","store","turn","acro","init","level","spin","nurse","fly","dive","rock","cut","water","surf")
                if (v and all((c.startswith(MOVE_PREFIX) or c in (".byte",".2byte")) for c,_ in v)) or "_Movement_" in k:
                    h[k]="MV#"+hashlib.sha1(json.dumps(v).encode()).hexdigest()[:10]
            for k,v in texts.items():
                h[k]="TX#"+hashlib.sha1(v.encode()).hexdigest()[:10]
            return h
        bh=refhash(bn,btexts); ch=refhash(cn,ctexts)
        bn={k:[(c,[bh.get(a,a) for a in args]) for c,args in v] for k,v in bn.items()}
        cn={k:[(c,[ch.get(a,a) for a in args]) for c,args in v] for k,v in cn.items()}
        def bodyhash(body): return hashlib.sha1(json.dumps(body).encode()).hexdigest()
        chash=collections.defaultdict(list)
        for k,v in cn.items(): chash[bodyhash(v)].append(k)
        bhash=collections.defaultdict(list)
        for k,v in bn.items(): bhash[bodyhash(v)].append(k)
        sc={"modified":{}, "removed":[], "added":[]}
        for k,v in bn.items():
            if k in cn:
                if v!=cn[k]:
                    cls=classify_diff(v,cn[k],btexts,ctexts)
                    sc["modified"][k]=sorted(cls)
            else:
                if bodyhash(v) in chash: continue  # renamed, identical body
                if not v: continue
                sc["removed"].append(k)
        for k,v in cn.items():
            if k not in bn and bodyhash(v) not in bhash and v:
                sc["added"].append(k)
        tx={"modified":[k for k in btexts if k in ctexts and btexts[k]!=ctexts[k]],
            "removed":[k for k in btexts if k not in ctexts and btexts[k] not in ctexts.values()],
            "added":[k for k in ctexts if k not in btexts and ctexts[k] not in btexts.values()]}
        if sc["modified"] or sc["removed"] or sc["added"]: r["scripts"]=sc
        if any(tx.values()): r["texts"]={k:len(v) for k,v in tx.items()}
        if r:
            report[m]=r
            for k in r: summary[k]+=1
            for k,cls in r.get("scripts",{}).get("modified",{}).items():
                for c in cls: summary["script:"+c]+=1
    out=S/"baseline_diff.json"
    out.write_text(json.dumps(report,indent=1,default=list))
    print("maps compared:",len(maps),"maps with differences:",len(report))
    for k,v in sorted(summary.items()): print(f"  {k}: {v}")

if __name__=="__main__":
    main()
