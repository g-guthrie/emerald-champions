#!/usr/bin/env python3
"""List Mega Stone objects and candidate acquisition evidence, not usable timing.

Authored map caps are context, not physical access. Method prerequisites are
reported separately; stone access, evolution readiness and usable Mega timing
remain unknown. Script references and family ancestors are not proof of a
reachable gift or a ready evolved user. No gameplay data is changed.
"""
import json, re, glob, collections
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]

# ---- exact authored map-cap references, never inferred physical access
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
groups = json.load(open(ROOT/"data/maps/map_groups.json"))
map_data = {m: json.load(open(ROOT/"data/maps"/m/"map.json"))
            for g in groups["group_order"] for m in groups[g]}
maps = [m for m, row in map_data.items() if row.get("region", "REGION_HOENN") == "REGION_HOENN"]
map_id = {m: map_data[m]["id"] for m in maps}
def cap_of(mid):
    return caps.get(mid)

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

# ---- candidate evidence; no source is promoted to exact availability
avail = collections.defaultdict(list)
def note(sp, cap, how):
    row = (cap, how)
    if row not in avail[sp]:
        avail[sp].append(row)

field_moves = (ROOT/"src/field_move.c").read_text()
def field_move_gate(move):
    body = re.search(r"\[FIELD_MOVE_" + move + r"\] =\s*\{(.*?)\n    \},", field_moves, re.S)
    if body is None or ".unlockType = BADGE_UNLOCK" not in body[1]:
        return "field-move requirements UNKNOWN"
    badges = re.findall(r"FLAG_TO_BADGE\((FLAG_BADGE\d+_GET)\)", body[1])
    license = re.search(r"\[FIELD_MOVE_" + move + r"\]\s*=\s*(FLAG_RECEIVED_HM_\w+)", field_moves)
    # Current ternary arguments put the Hoenn branch last. HasBadgeForFieldMove
    # also checks the received-HM license; a badge alone does not certify use.
    return " + ".join([badges[-1] if badges else "badge UNKNOWN",
                       license[1] if license else "license UNKNOWN",
                       "eligible field-move user"])

rod_gifts = collections.defaultdict(list)
for m in maps:
    script = ROOT/"data/maps"/m/"scripts.inc"
    if script.exists():
        for rod in re.findall(r"^\s*giveitem (ITEM_(?:OLD|GOOD|SUPER)_ROD)\b", script.read_text(), re.M):
            rod_gifts[rod].append(m)

def method_gate(mid, method):
    if method.endswith("_rod"):
        item = "ITEM_" + method.upper()
        return item + " (gift script references: " + (", ".join(rod_gifts[item]) or "UNKNOWN") + ")"
    if method == "water_mons":
        return field_move_gate("SURF")
    if method == "rock_smash_mons":
        return field_move_gate("ROCK_SMASH")
    if mid.startswith("MAP_UNDERWATER_"):
        return field_move_gate("DIVE") + "; submerged walking"
    return "walkable encounter terrain; obstacles/story access UNKNOWN"

wild = json.load(open(ROOT/"src/data/wild_encounters.json"))
group = next(g for g in wild["wild_encounter_groups"] if g["label"] == "gWildMonHeaders")
active_maps, seen = set(map_id.values()), set()
ordinary_evidence = []
for e in group["encounters"]:
    mid = e["map"]
    if mid not in active_maps or mid in seen:
        continue
    seen.add(mid)  # Altering Cave's later rows are selector alternatives.
    for field in group["fields"]:
        k = field["type"]
        if k not in ("land_mons", "water_mons", "rock_smash_mons", "fishing_mons"):
            continue  # Hidden is not ordinary encounter evidence, even if configured later.
        table = e.get(k)
        if not table or table.get("encounter_rate", 0) <= 0:
            continue
        for method, indices in field.get("groups", {k: range(len(table["mons"]))}).items():
            gate = method_gate(mid, method)
            for species in sorted({table["mons"][i]["species"] for i in indices}):
                ordinary_evidence.append((species, mid, method, gate))
                note(species, cap_of(mid), f"wild {mid}/{method}; requires {gate}; physical access UNKNOWN")
for m in maps:
    sc = ROOT/"data/maps"/m/"scripts.inc"
    if not sc.exists(): continue
    c = cap_of(map_id[m])
    for sp in re.findall(r"^\s*(?:givemon|giveegg|setwildbattle)\s+(SPECIES_[A-Z0-9_]+)", sc.read_text(), re.M): note(sp, c, f"script reference {m}; attachment/access UNKNOWN")
trade = (ROOT/"src/data/trade.h").read_text()
for sp in re.findall(r"\.species = (SPECIES_[A-Z0-9_]+)", trade): note(sp, None, "trade table reference; access UNKNOWN")
signs = (ROOT/"src/data/pokemon/legendary_signs.h").read_text()
for m in re.finditer(r"(?:VISIBLE_SIGN|LANDMARK_SIGN|OTHER_SIGN)\(LEGENDARY_SIGN_\w+,\s*(\w+),(.*)\)", signs):
    sp = "SPECIES_" + m.group(1)
    note(sp, None, "Legendary Sign definition; physical access/conditions UNKNOWN")
gc = (ROOT/"data/maps/MauvilleCity_GameCorner/scripts.inc").read_text()
for sp in re.findall(r"SPECIES_[A-Z0-9_]+", gc): note(sp, cap_of("MAP_MAUVILLE_CITY_GAME_CORNER"), "Game Corner script reference; requirements UNKNOWN")
starters = (ROOT/"src/starter_choose.c").read_text()
starter_table = re.search(r"sStarterMons\[.*?\n\};", starters, re.S)
if starter_table:
    for sp in re.findall(r"SPECIES_[A-Z0-9_]+", starter_table[0]): note(sp, None, "starter selection; evolved readiness UNKNOWN")

# ---- stone item -> mega species -> base species
items = (ROOT/"src/data/items.h").read_text()
stone_desc = {}
for m in re.finditer(r"\[(ITEM_[A-Z0-9_]+)\] =\s*\{(.*?)\n    \},", items, re.S):
    if "HOLD_EFFECT_MEGA_STONE" in m.group(2):
        d = re.search(r'"This stone enables\\n"\s*"([A-Za-z\.\' -]+?) to Mega', m.group(2).replace("\n", " "))
        stone_desc[m.group(1)] = d.group(1).strip() if d else None
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
        evidence = [(cap, how, s) for s in line for cap, how in avail.get(s, [])]
        evidence.sort(key=lambda e: (e[0] is None, e[0] or 0, e[2], e[1]))
        rows.append((m, item, cap_of(map_id[m]), base, evidence))

rows.sort(key=lambda r: (r[2] or 0))
print(f"{len(rows)} overworld Mega Stone objects")
print("Authoring caps below are NOT access dates; no earliest acquisition or usable Mega timing is certified.")
print("Usable Mega timing requires physical stone access, the Mega Ring and a ready evolved user; timing UNKNOWN.")
print("Only registered Hoenn ordinary methods and default selector rows count as wild evidence; Hidden is excluded.")
for m, item, cap, base, evidence in rows:
    print(f"\n  {item} at {m}: authored map cap={cap if cap is not None else 'UNKNOWN'}; usable Mega timing UNKNOWN")
    print(f"    item-description user={base or 'UNKNOWN'}; evolution readiness UNKNOWN")
    if not evidence:
        print("    No candidate source found by this limited scan; this does NOT mean unobtainable.")
    for source_cap, how, species in evidence[:3]:
        print(f"    candidate {species}: {how}; authored map cap={source_cap if source_cap is not None else 'UNKNOWN'}")
    if len(evidence) > 3:
        print(f"    {len(evidence) - 3} further candidate references; these samples are not an earliest-access ranking.")
print("\nsparkle/item mismatches:")
for x in mismatches: print("  ", x)
