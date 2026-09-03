#!/usr/bin/env python3
"""Story order vs level cap vs trainer distribution, from compiled data only.

Sources (no design docs):
  * trainer levels        src/data/trainers.party  (Hard difficulty base; Normal is -2, Easy -4)
  * trainer placement     data/maps/*/map.json object -> script -> trainerbattle id in scripts.inc
  * player level cap      src/caps.c  (0 badges 14, 1: 20, 2: 30, 3: 40, 4: 45, 5: 55, 6: 60, 7: 70, 8: 80, Champion 100)
  * map geometry          data/layouts/*/map.bin collision + metatile behaviours (water = Surf)

Reachability model: EARLIEST_BADGES gives the badge count at which a map is first
enterable under the current scripts (hand-verified gates, listed below).  Within a map a
trainer standing on a tile that cannot be reached from any entrance without crossing water
needs Surf (5 badges).  Route 111 has two extra regions: the desert (Go-Goggles, 4 badges)
and the west lane north of the Dynamo guard at y=97 (3 badges).  Route 120 north of
Steven's bridge is 6 badges.

Design offsets: a trainer above the player's cap by up to OFFSET_ALLOWED[class] is intended
(gym leaders +5, rivals/admins +3, ordinary +2).  Only larger gaps are reported.

Gates encoded (read from data/maps/*/scripts.inc, 2026-09-03):
  * Briney -> Slateport: Letter + Knuckle Badge + Mega Ring.
  * Route 111 north of y=97, Route 112-114, Fiery Path, Meteor Falls, Mt. Chimney, Jagged Pass,
    Lavaridge: road worker until the Dynamo Badge.
  * Desert: Go-Goggles (Flannery).  Ember Path/Ashen Woods: Strength (Heat Badge).
  * Norman: four badges.  Surf: Balance Badge + Wally's father.
  * Route 120 past Steven's bridge, 121, Mt. Pyre, hideouts, Mossdeep Gym: Feather Badge.
  * Space Center, Dive, Seafloor Cavern, Sootopolis Gym: Mind Badge.  Victory Road: Waterfall.
  * Gabby & Ty parties 4-6: Feather Badge.  Trick House puzzle N: badge N+1 (8: Champion).
"""
import re, json, glob, struct, collections
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]

CAP_BY_BADGES = {0: 14, 1: 20, 2: 30, 3: 40, 4: 45, 5: 55, 6: 60, 7: 70, 8: 80, 9: 100}

EARLIEST_BADGES = {
    "Route101": 0, "Route102": 0, "Route103": 0, "Route104": 0, "PetalburgWoods": 0, "PetalburgCity": 0,
    "RustboroCity": 0, "Route115": 0, "Route116": 0, "RusturfTunnel": 1, "OldaleTown": 0, "LittlerootTown": 0,
    "Route106": 1, "DewfordTown": 1, "GraniteCave": 1, "DewfordMeadow": 1, "DewfordManor": 1, "Route105": 5,
    "Route109": 2, "SlateportCity": 2, "Route110": 2, "MauvilleCity": 2, "Route117": 2, "VerdanturfTown": 2,
    "VerdanturfMeadow": 2, "Route111": 2, "Route118": 2, "Seaspray_Cave": 2, "NewMauville": 3,
    "Route112": 3, "FieryPath": 3, "Route113": 3, "FallarborTown": 3, "Route114": 3, "MeteorFalls": 3,
    "MtChimney": 3, "JaggedPass": 3, "LavaridgeTown": 3,
    "EmberPath": 4, "AshenWoods": 4, "PetalburgCity_Gym": 4, "MirageTower": 4, "SandstrewnRuins": 4, "DesertUnderpass": 4,
    "Route107": 5, "Route108": 5, "AbandonedShip": 5, "Route119": 5, "FortreeCity": 5, "Route120": 5, "Route123": 5,
    "Route121": 6, "Route122": 6, "LilycoveCity": 6, "MtPyre": 6, "MagmaHideout": 6, "AquaHideout": 6, "SafariZone": 6,
    "Route124": 6, "Route125": 6, "Route126": 6, "Route127": 6, "Route128": 6, "MossdeepCity": 6, "ShoalCave": 6,
    "MossdeepCity_SpaceCenter": 7, "Route129": 7, "Route130": 7, "Route131": 7, "Route132": 7, "Route133": 7,
    "Route134": 7, "SeafloorCavern": 7, "SootopolisCity": 7, "SkyPillar": 7, "Underwater": 7, "CaveOfOrigin": 7,
    "VictoryRoad": 8, "EverGrandeCity": 8,
    "SSTidal": 9, "CaveOfOrigin_DianciesRoom": 9, "AlteringCave": 9, "MeteorFalls_StevensCave": 9,
    "MossdeepCity_House1": 9, "TrainerHill": 9, "BattleFrontier": 9, "script:gabby_and_ty": 2,
}
for n, b in {1: 2, 2: 3, 3: 4, 4: 5, 5: 6, 6: 7, 7: 8, 8: 9}.items():
    EARLIEST_BADGES[f"Route110_TrickHousePuzzle{n}"] = b
# special trainers whose placement is not a plain map tile
TRAINER_OVERRIDES = {
    "TRAINER_GABBY_AND_TY_1": 3, "TRAINER_GABBY_AND_TY_2": 5, "TRAINER_GABBY_AND_TY_3": 5,
    "TRAINER_GABBY_AND_TY_4": 6, "TRAINER_GABBY_AND_TY_5": 6, "TRAINER_GABBY_AND_TY_6": 6,
    "TRAINER_WALLY_VR_2": 9,                       # postgame rematch slot (rematches are disabled)
    "TRAINER_MIKE_2": 3,                           # Rusturf Tunnel, Verdanturf side (Rock Smash)
}
OFFSET_ALLOWED = {"league": 6, "leader": 5, "boss": 5, "rival": 3, "regular": 2}

def base_badges(m):
    best = None
    for k, v in EARLIEST_BADGES.items():
        if m == k or m.startswith(k + "_") or m.startswith(k):
            if best is None or len(k) > len(best[0]): best = (k, v)
    return best[1] if best else None

# ---- compiled levels and classes
party = (ROOT/"src/data/trainers.party").read_text()
parts = re.split(r"(?m)^=== (TRAINER_\w+) ===\s*$", party)
level = {}; klass = {}
for i in range(1, len(parts), 2):
    tid, body = parts[i], parts[i+1]
    lv = [int(x) for x in re.findall(r"(?m)^Level: (\d+)", body)]
    if not lv: continue
    level[tid] = max(lv)
    cls = (re.search(r"(?m)^Class: (.+)$", body) or [None, ""])[1].strip().upper()
    name = tid
    if re.search(r"^TRAINER_(SIDNEY|PHOEBE|GLACIA|DRAKE|WALLACE)$", name):
        klass[tid] = "league"        # authored at cap 80 + 6
    elif "LEADER" in cls or re.search(r"ROXANNE|BRAWLY|WATTSON|FLANNERY|NORMAN_|WINONA|TATE_AND_LIZA|JUAN|STEVEN|CYNTHIA", name):
        klass[tid] = "leader"
    elif re.search(r"MAXIE|ARCHIE|TABITHA|COURTNEY|SHELLY|MATT|LUCY|SPENSER|GRETA|WALLY", name):
        klass[tid] = "boss"
    elif re.search(r"BRENDAN|MAY_", name):
        klass[tid] = "rival"
    else:
        klass[tid] = "regular"

# ---- placement: object -> script -> trainer ids
layouts = {L["id"]: L for L in json.load(open(ROOT/"data/layouts/layouts.json"))["layouts"]}
headers = (ROOT/"src/data/tilesets/headers.h").read_text()
graphics = (ROOT/"src/data/tilesets/metatiles.h").read_text()
meta_sym = dict(re.findall(r"const struct Tileset (gTileset_\w+) =\s*\{.*?\.metatiles = (\w+)", headers, re.S))
meta_path = dict(re.findall(r"(gMetatiles_\w+)\[\] = INCBIN_U16\(\"([^\"]+)\"", graphics))
mb_names = re.findall(r"^\s+(MB_\w+),", (ROOT/"include/constants/metatile_behaviors.h").read_text(), re.M)
WATER = {mb_names.index(n) for n in ("MB_POND_WATER", "MB_INTERIOR_DEEP_WATER", "MB_DEEP_WATER", "MB_WATERFALL",
         "MB_SOOTOPOLIS_DEEP_WATER", "MB_OCEAN_WATER", "MB_NO_SURFACING", "MB_SEAWEED", "MB_SEAWEED_NO_SURFACING",
         "MB_EASTWARD_CURRENT", "MB_WESTWARD_CURRENT", "MB_NORTHWARD_CURRENT", "MB_SOUTHWARD_CURRENT") if n in mb_names}
def behaviors(tileset):
    p = meta_path.get(meta_sym.get(tileset, ""), "")
    if not p: return []
    b = (ROOT/p.replace("metatiles.bin", "metatile_attributes.bin")).read_bytes()
    n = (ROOT/p).stat().st_size // 16; per = len(b)//n if n else 2
    return [int.from_bytes(b[i*per:i*per+per], "little") & (0xFF if per == 2 else 0x1FF) for i in range(n)]

def land_region(mapname, j):
    """tiles reachable from any connection edge or warp without crossing water"""
    L = layouts[j["layout"]]; w, h = L["width"], L["height"]
    data = (ROOT/L["blockdata_filepath"]).read_bytes()
    bp, bs = behaviors(L["primary_tileset"]), behaviors(L["secondary_tileset"])
    def cell(x, y):
        v = struct.unpack_from("<H", data, (y*w+x)*2)[0]; mt = v & 0x3FF
        beh = bp[mt] if mt < 512 and mt < len(bp) else (bs[mt-512] if mt >= 512 and mt-512 < len(bs) else 0)
        return (v >> 10) & 3, beh
    passable = [[cell(x, y)[0] == 0 and cell(x, y)[1] not in WATER for x in range(w)] for y in range(h)]
    # warps into side caves whose only entrance is this map do not count as entries
    DEAD_END = ("TERRA_CAVE", "MARINE_CAVE", "SEASPRAY_CAVE", "MIRAGE_TOWER", "DESERT_RUINS", "ISLAND_CAVE",
                "ANCIENT_TOMB", "SCORCHED_SLAB", "ALTERING_CAVE", "SECRET_BASE", "TRAINER_HILL", "DESERT_UNDERPASS")
    entry_warps = [wp for wp in j.get("warp_events", []) if not any(d in wp["dest_map"] for d in DEAD_END)]
    seeds = [(wp["x"], wp["y"]) for wp in entry_warps]
    for c in j.get("connections") or []:
        d = c["direction"]
        if d == "up": seeds += [(x, 0) for x in range(w)]
        if d == "down": seeds += [(x, h-1) for x in range(w)]
        if d == "left": seeds += [(0, y) for y in range(h)]
        if d == "right": seeds += [(w-1, y) for y in range(h)]
    seen = set(); stack = [s for s in seeds if 0 <= s[0] < w and 0 <= s[1] < h and passable[s[1]][s[0]]]
    # a warp tile itself may be a door (impassable); also seed its neighbours
    for wp in entry_warps:
        for dx, dy in ((0, 1), (0, -1), (1, 0), (-1, 0)):
            x, y = wp["x"]+dx, wp["y"]+dy
            if 0 <= x < w and 0 <= y < h and passable[y][x]: stack.append((x, y))
    while stack:
        x, y = stack.pop()
        if (x, y) in seen: continue
        seen.add((x, y))
        for dx, dy in ((0, 1), (0, -1), (1, 0), (-1, 0)):
            nx, ny = x+dx, y+dy
            if 0 <= nx < w and 0 <= ny < h and passable[ny][nx] and (nx, ny) not in seen: stack.append((nx, ny))
    return seen, passable

def region_badges(mapname, x, y, base, land):
    b = base
    # standing next to (not on) the land region counts as land (trainers face a tile)
    if (x, y) not in land: b = max(b, 5)
    if mapname == "Route111":
        if y < 97: b = max(b, 3)
        if x >= 15 and 25 <= y <= 95: b = max(b, 4)      # desert behind the sandstorm triggers
    if mapname == "Route120" and y < 20: b = max(b, 6)    # north of Steven's bridge
    if mapname == "Route103" and x >= 40: b = max(b, 2)   # east bank, entered from Route 110
    if mapname == "Route118" and x >= 30: b = max(b, 5)   # east bank, entered from Route 119/123
    return b

rows = []; unknown = set()
for f in sorted(glob.glob(str(ROOT/"data/maps/*/map.json"))):
    m = Path(f).parent.name
    if m.endswith("_Frlg"): continue
    j = json.load(open(f)); sc = Path(f).parent/"scripts.inc"
    text = sc.read_text() if sc.exists() else ""
    base = base_badges(m)
    trainer_objs = [o for o in j.get("object_events", []) if o.get("trainer_type") != "TRAINER_TYPE_NONE" or "trainerbattle" in text.split(o.get("script", "\x00"), 1)[-1][:400]]
    land = None
    for o in j.get("object_events", []):
        script = o.get("script") or ""
        if not script or script not in text: continue
        block = re.split(r"(?m)^\S+::", text.split(script + "::", 1)[-1], maxsplit=1)[0]
        ids = re.findall(r"trainerbattle\w*\s+(?:[A-Z_0-9]+,\s*)?(TRAINER_[A-Z0-9_]+)", block)
        if not ids: continue
        if land is None: land, _ = land_region(m, j)
        for tid in dict.fromkeys(ids):
            if tid not in level: continue
            if base is None: unknown.add(m); continue
            b = TRAINER_OVERRIDES.get(tid, region_badges(m, o["x"], o["y"], base, land))
            rows.append((tid, m, o["x"], o["y"], level[tid], b))
# script-driven trainers not tied to an object (gabby & ty)
for tid, b in TRAINER_OVERRIDES.items():
    if tid in level and not any(r[0] == tid for r in rows): rows.append((tid, "script", 0, 0, level[tid], b))

print(f"{len(rows)} placed trainer parties checked; unmodelled maps: {sorted(unknown)}")
over = []
for tid, m, x, y, lv, b in rows:
    cap = CAP_BY_BADGES[b]; allowed = OFFSET_ALLOWED[klass.get(tid, "regular")]
    if lv > cap + allowed: over.append((lv - cap, tid, m, x, y, lv, cap, klass.get(tid)))
print(f"\nTRAINERS ABOVE THE PLAYER'S CAP (beyond the design offset) WHEN FIRST REACHABLE: {len(over)}")
for r in sorted(over, reverse=True): print("  +%2d %-36s %-28s (%3d,%3d) level %3d cap %3d %s" % r)
print("\nchapter summary (max trainer level per badge stage):")
by = collections.defaultdict(list)
for tid, m, x, y, lv, b in rows: by[b].append(lv)
for b in sorted(by): print(f"  {b} badges: cap {CAP_BY_BADGES[b]:3d}  trainers {len(by[b]):3d}  levels {min(by[b])}-{max(by[b])}")
