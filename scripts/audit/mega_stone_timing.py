#!/usr/bin/env python3
"""Mega Stone sparkle timing audit.

For every overworld Mega Stone object: which chapter cap the map belongs to, when the
stone's evolution line first becomes obtainable (wild tables, gift scripts, in-game trades,
Game Corner, Legendary Signs), and how far apart those are.  Also lists stones whose
object is not drawn as a sparkle and sparkles that are not Mega Stones.
"""
import json, re, glob, collections
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]

# ---- map -> chapter cap (master battle design + route sheet + prefix inheritance)
caps = {}
master = (ROOT/"data/emerald_champions/emerald_champions_master_battle_design.txt").read_text()
for block in re.split(r"(?m)^=== ENCOUNTER \d{4} ===$", master)[1:]:
    loc = re.search(r"location: (\S+)", block); cap = re.search(r"strict_cap: (\d+)", block)
    if loc and cap:
        key = "MAP_" + re.sub(r"(?<=[a-z0-9])(?=[A-Z])", "_", loc.group(1).rstrip(";")).upper()
        caps.setdefault(key, int(cap.group(1)))
sheet = json.load(open(ROOT/"data/emerald_champions/wild_route_sheet.json"))
for k, v in sheet.items():
    if isinstance(v, dict) and "cap" in v: caps.setdefault(k, v["cap"])
BADGE_CAP = {"FLAG_BADGE01_GET": 20, "FLAG_BADGE02_GET": 30, "FLAG_BADGE03_GET": 40, "FLAG_BADGE04_GET": 45,
             "FLAG_BADGE05_GET": 55, "FLAG_BADGE06_GET": 60, "FLAG_BADGE07_GET": 70, "FLAG_BADGE08_GET": 80,
             "FLAG_SYS_GAME_CLEAR": 100, "0": 14, "FLAG_TEMP_1": 14}
# dungeons and side areas with no authored trainer cap: the chapter they open in
FALLBACK_CAPS = {
    "MAP_GRANITE_CAVE": 20, "MAP_RUSTURF_TUNNEL": 20, "MAP_DEWFORD_MANOR": 20, "MAP_DEWFORD_MEADOW": 20,
    "MAP_PETALBURG_WOODS": 14, "MAP_ROUTE104_": 14, "MAP_VERDANTURF_MEADOW": 30, "MAP_FIERY_PATH": 40,
    "MAP_METEOR_FALLS": 40, "MAP_NEW_MAUVILLE": 40, "MAP_MT_CHIMNEY": 40, "MAP_JAGGED_PASS": 40, "MAP_EMBER_PATH": 45,
    "MAP_ASHEN_WOODS": 45, "MAP_DESERT": 30, "MAP_MIRAGE_TOWER": 30, "MAP_SANDSTREWN_RUINS": 30, "MAP_SEASPRAY_CAVE": 30,
    "MAP_SCORCHED_SLAB": 55, "MAP_SAFARI_ZONE": 55, "MAP_ABANDONED_SHIP": 55, "MAP_MT_PYRE": 60, "MAP_SHOAL_CAVE": 60,
    "MAP_UNDERWATER": 60, "MAP_AQUA_HIDEOUT": 60, "MAP_MAGMA_HIDEOUT": 60, "MAP_SEAFLOOR_CAVERN": 70, "MAP_CAVE_OF_ORIGIN": 70,
    "MAP_SKY_PILLAR": 80, "MAP_VICTORY_ROAD": 80, "MAP_EVER_GRANDE": 80, "MAP_SEALED_CHAMBER": 70, "MAP_ISLAND_CAVE": 70,
    "MAP_DESERT_RUINS": 70, "MAP_ANCIENT_TOMB": 70, "MAP_MARINE_CAVE": 70, "MAP_TERRA_CAVE": 70, "MAP_SOUTHERN_ISLAND": 80,
    "MAP_ALTERING_CAVE": 100, "MAP_ARTISAN_CAVE": 100, "MAP_TRAINER_HILL": 100, "MAP_BATTLE_FRONTIER": 100,
    "MAP_FARAWAY_ISLAND": 100, "MAP_BIRTH_ISLAND": 100, "MAP_NAVEL_ROCK": 100, "MAP_DESERT_UNDERPASS": 30,
}
groups = json.load(open(ROOT/"data/maps/map_groups.json"))
maps = [m for g in groups["group_order"] if "Frlg" not in g for m in groups[g] if not m.endswith("_Frlg")]
map_id = {m: json.load(open(ROOT/"data/maps"/m/"map.json"))["id"] for m in maps}
def cap_of(mid):
    if mid in caps: return caps[mid]
    # inherit from the longest known prefix (e.g. MAP_RUSTBORO_CITY_GYM -> MAP_RUSTBORO_CITY)
    best = None
    for k, v in caps.items():
        if mid.startswith(k + "_") and (best is None or len(k) > len(best[0])): best = (k, v)
    if best: return best[1]
    for k, v in FALLBACK_CAPS.items():
        if mid.startswith(k): return v
    return None

# ---- species evolution lines
pre = {}  # species -> set of pre-evolutions
for f in glob.glob(str(ROOT/"src/data/pokemon/species_info/*.h")):
    txt = open(f).read()
    for m in re.finditer(r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{(.*?)\n    \},", txt, re.S):
        sp, body = m.group(1), m.group(2)
        for t in re.findall(r"\{EVO_[A-Z_]+,\s*[^,}]+,\s*(SPECIES_[A-Z0-9_]+)", body):
            pre.setdefault(t, set()).add(sp)
def line_of(sp):
    seen = {sp}; stack = [sp]
    while stack:
        s = stack.pop()
        for p in pre.get(s, ()):
            if p not in seen: seen.add(p); stack.append(p)
    return seen

# ---- species availability (earliest cap)
avail = {}
def note(sp, cap, how):
    if cap is None: return
    if sp not in avail or cap < avail[sp][0]:
        avail[sp] = (cap, how)
wild = json.load(open(ROOT/"src/data/wild_encounters.json"))
for g in wild["wild_encounter_groups"]:
    for e in g["encounters"]:
        mid = e.get("map") or e.get("base_label")
        c = cap_of(mid) if mid else None
        for k in ("land_mons", "water_mons", "rock_smash_mons", "fishing_mons", "hidden_mons"):
            for mon in e.get(k, {}).get("mons", []): note(mon["species"], c, f"wild {mid}")
for m in maps:
    sc = ROOT/"data/maps"/m/"scripts.inc"
    if not sc.exists(): continue
    c = cap_of(map_id[m])
    for sp in re.findall(r"^\s*(?:givemon|giveegg|setwildbattle)\s+(SPECIES_[A-Z0-9_]+)", sc.read_text(), re.M): note(sp, c, f"gift {m}")
trade = (ROOT/"src/data/trade.h").read_text()
for sp in re.findall(r"\.species = (SPECIES_[A-Z0-9_]+)", trade): note(sp, 40, "in-game trade")
signs = (ROOT/"src/data/pokemon/legendary_signs.h").read_text()
for m in re.finditer(r"(?:VISIBLE_SIGN|WILD_SIGN|OTHER_SIGN)\(LEGENDARY_SIGN_\w+,\s*(\w+),(.*)\)", signs):
    sp = "SPECIES_" + m.group(1); flags = re.findall(r"FLAG_\w+", m.group(2))
    note(sp, BADGE_CAP.get(flags[-1], 55) if flags else 55, "Legendary Sign")
gc = (ROOT/"data/maps/MauvilleCity_GameCorner/scripts.inc").read_text()
for sp in re.findall(r"SPECIES_[A-Z0-9_]+", gc): note(sp, cap_of("MAP_MAUVILLE_CITY_GAME_CORNER") or 30, "Game Corner")
starters = (ROOT/"src/starter_choose.c").read_text()
for sp in re.findall(r"SPECIES_[A-Z0-9_]+", starters): note(sp, 14, "starter")

# ---- stone item -> mega species -> base species
items = (ROOT/"src/data/items.h").read_text()
stone_desc = {}
for m in re.finditer(r"\[(ITEM_[A-Z0-9_]+)\] =\s*\{(.*?)\n    \},", items, re.S):
    if "HOLD_EFFECT_MEGA_STONE" in m.group(2):
        d = re.search(r'"This stone enables\\n"\s*"([A-Za-z\.\' -]+?) to Mega', m.group(2).replace("\n", " "))
        stone_desc[m.group(1)] = d.group(1).strip() if d else None
def species_from_name(name):
    return "SPECIES_" + re.sub(r"[^A-Z0-9]", "", name.upper().replace(" ", "_").replace(".", "").replace("'", "")) if name else None
def base_species_of_stone(item):
    name = stone_desc.get(item)
    if not name: return None
    sp = "SPECIES_" + re.sub(r"[^A-Z0-9_]", "", name.upper().replace(" ", "_").replace("-", "_").replace(".", "").replace("'", ""))
    return sp

rows = []; mismatches = []
for m in maps:
    j = json.load(open(ROOT/"data/maps"/m/"map.json"))
    for o in j["object_events"]:
        item = str(o.get("trainer_sight_or_berry_tree_id", ""))
        is_stone_item = item in stone_desc
        is_sparkle = o["graphics_id"] == "OBJ_EVENT_GFX_MEGA_STONE"
        if is_sparkle != is_stone_item:
            mismatches.append((m, o["graphics_id"], item, o.get("script"), o.get("flag")))
        if not is_stone_item: continue
        base = base_species_of_stone(item)
        line = line_of(base) if base else set()
        earliest = min(((avail[s][0], avail[s][1], s) for s in line if s in avail), default=(None, "NOT OBTAINABLE", base))
        rows.append((m, item, cap_of(map_id[m]), earliest))

rows.sort(key=lambda r: (r[2] or 0))
print(f"{len(rows)} overworld Mega Stone objects")
print("\nstone placed AFTER its line is obtainable (gap = stone cap - species cap):")
late = [(r, r[2] - r[3][0]) for r in rows if r[2] is not None and r[3][0] is not None and r[2] > r[3][0]]
for r, gap in sorted(late, key=lambda x: -x[1]):
    print(f"  gap {gap:3d}  {r[1]:<22} {r[0]:<32} stone cap {r[2]:>3}  line via {r[3][2]} at cap {r[3][0]} ({r[3][1]})")
print("\nstone placed at/before its line becomes obtainable:")
for r in rows:
    if r[3][0] is not None and r[2] is not None and r[2] <= r[3][0]:
        print(f"  {r[1]:<22} {r[0]:<32} stone cap {r[2]:>3}  line at cap {r[3][0]} ({r[3][1]})")
print("\nstones whose line has no acquisition found / unknown map cap:")
for r in rows:
    if r[3][0] is None or r[2] is None: print("  ", r)
print("\nsparkle/item mismatches:")
for x in mismatches: print("  ", x)
