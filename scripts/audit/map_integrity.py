#!/usr/bin/env python3
"""Static visual/structural integrity sweep over every Hoenn map.

Checks, using only the raw data files:
  * every block's metatile id is inside its primary/secondary tileset
  * border blocks are valid
  * object graphics ids exist and objects sit inside the map
  * warps: destination map and warp id exist; the arrival tile is passable (or a door)
  * connections are reciprocal (A says B is to the east -> B says A is to the west)
  * coord/bg events sit inside the map
"""
import json, re, struct, sys
from pathlib import Path
import os
ROOT = Path(os.environ.get("EC_ROOT", Path(__file__).resolve().parents[2]))

def load(p): return json.load(open(p))

groups = load(ROOT/"data/maps/map_groups.json")
maps = [m for g in groups["group_order"] if "Frlg" not in g for m in groups[g] if not m.endswith("_Frlg")]
layouts = {L["id"]: L for L in load(ROOT/"data/layouts/layouts.json")["layouts"]}

# tileset -> metatile count
headers = (ROOT/"src/data/tilesets/headers.h").read_text()
graphics = (ROOT/"src/data/tilesets/metatiles.h").read_text()
meta_sym = dict(re.findall(r"const struct Tileset (gTileset_\w+) =\s*\{.*?\.metatiles = (\w+)", headers, re.S))
meta_path = dict(re.findall(r"(gMetatiles_\w+)\[\] = INCBIN_U16\(\"([^\"]+)\"", graphics))
def metatile_count(tileset):
    sym = meta_sym.get(tileset); path = meta_path.get(sym or "")
    if not path: return None
    p = ROOT/path
    return p.stat().st_size // 16 if p.exists() else None
def behaviors(tileset):
    sym = meta_sym.get(tileset); path = meta_path.get(sym or "")
    if not path: return None
    p = ROOT/path.replace("metatiles.bin", "metatile_attributes.bin")
    if not p.exists(): return None
    b = p.read_bytes(); n = metatile_count(tileset) or 0
    per = len(b)//n if n else 2
    return [int.from_bytes(b[i*per:i*per+per], "little") & (0xFF if per == 2 else 0x1FF) for i in range(n)]

gfx_ids = set(re.findall(r"\b(OBJ_EVENT_GFX_\w+)", (ROOT/"include/constants/event_objects.h").read_text()))
map_json = {m: load(ROOT/"data/maps"/m/"map.json") for m in maps}
by_id = {j["id"]: m for m, j in map_json.items()}

OPP = {"up": "down", "down": "up", "left": "right", "right": "left"}
problems = []
def prob(m, kind, detail): problems.append((m, kind, detail))

for m in maps:
    j = map_json[m]; L = layouts.get(j["layout"])
    if not L: prob(m, "layout-missing", j["layout"]); continue
    w, h = L["width"], L["height"]
    data = (ROOT/L["blockdata_filepath"]).read_bytes()
    prim, sec = metatile_count(L["primary_tileset"]), metatile_count(L["secondary_tileset"])
    if prim is None or sec is None:
        prob(m, "tileset-unresolved", (L["primary_tileset"], L["secondary_tileset"]))
    passable = [[True]*w for _ in range(h)]
    bad = 0
    for y in range(h):
        for x in range(w):
            v = struct.unpack_from("<H", data, (y*w+x)*2)[0]
            mt = v & 0x3FF
            passable[y][x] = ((v >> 10) & 3) == 0
            if prim is not None and sec is not None:
                if (mt < 512 and mt >= prim) or (mt >= 512 and mt-512 >= sec): bad += 1
    if bad: prob(m, "metatile-out-of-range", bad)
    bpath = ROOT/L["border_filepath"]
    if bpath.exists() and prim is not None and sec is not None:
        b = bpath.read_bytes()
        for i in range(0, len(b), 2):
            mt = struct.unpack_from("<H", b, i)[0] & 0x3FF
            if (mt < 512 and mt >= prim) or (mt >= 512 and mt-512 >= sec): prob(m, "border-metatile-out-of-range", mt); break
    for o in j.get("object_events", []):
        if o["graphics_id"] not in gfx_ids and not o["graphics_id"].startswith("OBJ_EVENT_GFX_VAR"): prob(m, "bad-gfx", o["graphics_id"])
        if not (0 <= o["x"] < w and 0 <= o["y"] < h): prob(m, "object-outside-map", (o["graphics_id"], o["x"], o["y"]))
    for e in j.get("coord_events", []):
        if not (0 <= e["x"] < w and 0 <= e["y"] < h): prob(m, "trigger-outside-map", (e["x"], e["y"], e.get("script")))
    for e in j.get("bg_events", []):
        if not (-1 <= e["x"] <= w and -1 <= e["y"] <= h): prob(m, "sign-outside-map", (e["x"], e["y"]))
    for i, wp in enumerate(j.get("warp_events", [])):
        if not (0 <= wp["x"] < w and 0 <= wp["y"] < h): prob(m, "warp-outside-map", (wp["x"], wp["y"]))
        dm = wp["dest_map"]
        if dm in ("MAP_DYNAMIC", "MAP_NONE"): continue
        if dm not in by_id: prob(m, "warp-dest-missing", dm); continue
        dj = map_json[by_id[dm]]; dw = dj.get("warp_events", [])
        did = wp["dest_warp_id"]
        if isinstance(did, str) and not did.isdigit(): continue
        did = int(did)
        if did >= len(dw): prob(m, "warp-dest-id-missing", (dm, did)); continue
        # arrival tile passable?
        DL = layouts[dj["layout"]]; dd = (ROOT/DL["blockdata_filepath"]).read_bytes()
        ax, ay = dw[did]["x"], dw[did]["y"]
        if not (0 <= ax < DL["width"] and 0 <= ay < DL["height"]):
            prob(m, "warp-arrival-outside-dest", (dm, did, ax, ay, DL["width"], DL["height"])); continue
        v = struct.unpack_from("<H", dd, (ay*DL["width"]+ax)*2)[0]
        if (v >> 10) & 3:
            mt = v & 0x3FF
            bp, bs = behaviors(DL["primary_tileset"]), behaviors(DL["secondary_tileset"])
            beh = (bp[mt] if bp and mt < len(bp) else None) if mt < 512 else (bs[mt-512] if bs and mt-512 < len(bs) else None)
            if beh in (0, None): prob(m, "warp-arrival-impassable-plain-tile", (dm, did, ax, ay, beh))
    for c in j.get("connections") or []:
        other = by_id.get(c["map"])
        if not other: prob(m, "connection-dest-missing", c["map"]); continue
        back = [k for k in (map_json[other].get("connections") or []) if k["map"] == j["id"] and k["direction"] == OPP.get(c["direction"])]
        if not back: prob(m, "connection-not-reciprocal", (c["direction"], c["map"]))
        elif back[0]["offset"] != -c["offset"]: prob(m, "connection-offset-mismatch", (c["direction"], c["map"], c["offset"], back[0]["offset"]))

by_kind = {}
for m, k, d in problems: by_kind.setdefault(k, []).append((m, d))
print(f"maps checked: {len(maps)}; problems: {len(problems)}")
for k, rows in sorted(by_kind.items()):
    print(f"== {k}: {len(rows)}")
    for r in rows[:40]: print("   ", r)
