#!/usr/bin/env python3
"""Independent map reachability check.

For every Hoenn map, decode the layout collision bits, treat permanent NPC objects
(flag 0, not item balls) as blockers, and flood-fill from each warp arrival tile.
Report (a) warps that cannot reach every other warp on the same map, and (b) objects
whose interaction tile (any adjacent passable tile) is unreachable from every warp.
Run on both Champions and the Inclement baseline and report regressions only, so the
engine's ledges/water/elevation quirks cancel out.
"""
import json, struct, sys, collections
from pathlib import Path
import ec_baseline_diff as D

def layouts(root):
    d=D.load_json(root/"data/layouts/layouts.json"); return {L["id"]:L for L in d["layouts"]}

def passable_grid(root,L):
    p=root/L["blockdata_filepath"]
    if not p.exists(): return None
    data=p.read_bytes(); w,h=L["width"],L["height"]
    return [[((struct.unpack_from("<H",data,(y*w+x)*2)[0]>>10)&3)==0 for x in range(w)] for y in range(h)]

ITEM_GFX={"OBJ_EVENT_GFX_ITEM_BALL","OBJ_EVENT_GFX_GOLD_ITEM_BALL","OBJ_EVENT_GFX_MEGA_STONE"}
def analyse(root,m,lays):
    j=D.load_json(root/"data/maps"/m/"map.json")
    L=lays.get(j["layout"])
    if not L: return None
    g=passable_grid(root,L)
    if g is None: return None
    w,h=L["width"],L["height"]
    blockers=set()
    objs=[]
    for o in j.get("object_events",[]):
        if str(o.get("flag","0"))!="0": continue          # can disappear -> not a permanent blocker
        if o["graphics_id"] in ITEM_GFX: continue
        x,y=o["x"],o["y"]
        # wandering NPCs can move; only fixed ones are hard blockers
        mt=o.get("movement_type","")
        rx,ry=o.get("movement_range_x",0),o.get("movement_range_y",0)
        if "WANDER" in mt or "WALK" in mt or rx or ry:
            objs.append((x,y,o)); continue
        blockers.add((x,y)); objs.append((x,y,o))
    warps=[(wp["x"],wp["y"]) for wp in j.get("warp_events",[])]
    def bfs(sx,sy):
        seen={(sx,sy)}; q=collections.deque([(sx,sy)])
        while q:
            x,y=q.popleft()
            for dx,dy in ((1,0),(-1,0),(0,1),(0,-1)):
                nx,ny=x+dx,y+dy
                if 0<=nx<w and 0<=ny<h and (nx,ny) not in seen and (nx,ny) not in blockers and (g[ny][nx] or (nx,ny) in warps):
                    seen.add((nx,ny)); q.append((nx,ny))
        return seen
    reach={wp:bfs(*wp) for wp in set(warps)}
    problems=[]
    for wp,seen in reach.items():
        missing=[o for o in set(warps) if o not in seen]
        if missing: problems.append(("warp_unreachable",wp,sorted(missing)))
    # objects: any adjacent tile reachable from any warp?
    allseen=set().union(*reach.values()) if reach else set()
    for x,y,o in objs:
        adj=[(x+dx,y+dy) for dx,dy in ((1,0),(-1,0),(0,1),(0,-1))]
        if allseen and not any(a in allseen for a in adj) and (x,y) not in allseen:
            problems.append(("object_unreachable",(x,y),o["graphics_id"],o.get("script")))
    return problems

if __name__=="__main__":
    maps=D.hoenn_maps()
    cl=layouts(D.CUR); bl=layouts(D.BASE)
    regressions=0; same=0
    for m in maps:
        c=analyse(D.CUR,m,cl); b=analyse(D.BASE,m,bl)
        if c is None: continue
        key=lambda p: json.dumps(p[:3] if p[0]=="object_unreachable" else p,default=str)
        cs={key(p) for p in c}; bs={key(p) for p in (b or [])}
        new=cs-bs
        if new:
            regressions+=1
            print(f"## {m}")
            for p in sorted(new): print("   NEW:",p)
        elif cs: same+=1
    print(f"maps with new reachability problems vs Inclement: {regressions}; maps with problems shared with Inclement: {same}")
