#!/usr/bin/env python3
"""Forward-dependency check for story gates.

Every place a Hoenn script (or trigger) tests a flag or a story-variable value is a gate.
The gate is satisfiable only if something sets that flag/value in an area the player can
reach no later than the gate's area.  Chapter caps (from the authored battle design and
route sheet, inherited by prefix for indoor maps) stand in for reachability order.
Reports gates whose only setters live in strictly later chapters, or nowhere.
"""
import json, re, glob, collections
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]

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
caps.setdefault("MAP_LITTLEROOT_TOWN", 14); caps.setdefault("MAP_OLDALE_TOWN", 14); caps.setdefault("MAP_ROUTE101", 14)
groups = json.load(open(ROOT/"data/maps/map_groups.json"))
maps = [m for g in groups["group_order"] if "Frlg" not in g for m in groups[g] if not m.endswith("_Frlg")]
mj = {m: json.load(open(ROOT/"data/maps"/m/"map.json")) for m in maps}
def cap_of(mid):
    if mid in caps: return caps[mid]
    best = None
    for k, v in caps.items():
        if mid.startswith(k + "_") and (best is None or len(k) > len(best[0])): best = (k, v)
    return best[1] if best else None

setters_flag = collections.defaultdict(set)      # flag -> set of caps (None = common/C)
setters_var = collections.defaultdict(set)       # (var, value) -> caps
gates = []                                       # (map, cap, kind, key, line)
common = "\n".join(open(f, errors="ignore").read() for f in glob.glob(str(ROOT/"data/scripts/*.inc")) + [str(ROOT/"data/event_scripts.s")])
csrc = "\n".join(open(f, errors="ignore").read() for f in glob.glob(str(ROOT/"src/**/*.c"), recursive=True))
for f in re.findall(r"^\s*setflag\s+(FLAG_\w+)", common, re.M): setters_flag[f].add(None)
for v, val in re.findall(r"^\s*setvar\s+(VAR_\w+),\s*(\w+)", common, re.M): setters_var[(v, val)].add(None)
for f in set(re.findall(r"FlagSet\(\s*(FLAG_\w+)", csrc)): setters_flag[f].add(None)
for v, val in re.findall(r"VarSet\(\s*(VAR_\w+),\s*(\w+)", csrc): setters_var[(v, val)].add(None)
for v in set(re.findall(r"VarSet\(\s*(VAR_\w+),", csrc)) | set(re.findall(r"GetVarPointer\(\s*(VAR_\w+)", csrc)): setters_var[(v, "*")].add(None)
for v in re.findall(r"^\s*(?:addvar|subvar|copyvar|setorcopyvar)\s+(VAR_\w+)", common, re.M): setters_var[(v, "*")].add(None)

for m in maps:
    sc = ROOT/"data/maps"/m/"scripts.inc"; c = cap_of(mj[m]["id"])
    txt = sc.read_text(errors="ignore") if sc.exists() else ""
    for f in re.findall(r"^\s*setflag\s+(FLAG_\w+)", txt, re.M): setters_flag[f].add(c)
    for v, val in re.findall(r"^\s*setvar\s+(VAR_\w+),\s*(\w+)", txt, re.M): setters_var[(v, val)].add(c)
    for v in re.findall(r"^\s*(?:addvar|subvar|copyvar|setorcopyvar)\s+(VAR_\w+)", txt, re.M): setters_var[(v, "*")].add(c)
    # removeobject/hideobjectat set the object's hide flag in the engine
    local = {}
    for i, o in enumerate(mj[m].get("object_events", [])):
        if o.get("local_id"): local[o["local_id"]] = str(o.get("flag", "0"))
        local[str(i + 1)] = str(o.get("flag", "0"))
    for ident in re.findall(r"^\s*(?:removeobject|hideobjectat)\s+([A-Za-z0-9_]+)", txt, re.M):
        f = local.get(ident)
        if f and f.startswith("FLAG_"): setters_flag[f].add(c)
    for ident in re.findall(r"^\s*removeobject\s+([A-Za-z0-9_]+),\s*(MAP_\w+)", txt, re.M):
        pass
    for n, line in enumerate(txt.splitlines(), 1):
        mm = re.match(r"\s*(goto_if_set|goto_if_unset|call_if_set|call_if_unset)\s+(FLAG_\w+)", line)
        if mm: gates.append((m, c, "flag", mm.group(2), n)); continue
        mm = re.match(r"\s*(?:goto_if_eq|call_if_eq|goto_if_ge|call_if_ge|goto_if_gt|call_if_gt|map_script_2)\s+(VAR_\w+),\s*(\w+)", line)
        if mm and mm.group(2) not in ("0", "FALSE", "NO"): gates.append((m, c, "var", (mm.group(1), mm.group(2)), n))
    for e in mj[m].get("coord_events", []):
        if str(e.get("var", "")).startswith("VAR_") and str(e.get("var_value")) not in ("0",):
            gates.append((m, c, "var", (e["var"], str(e["var_value"])), "trigger"))
    for o in mj[m].get("object_events", []):
        f = str(o.get("flag", "0"))
        if f.startswith("FLAG_"): gates.append((m, c, "hideflag", f, "object"))

SKIP = ("FLAG_TEMP_", "FLAG_SYS_", "FLAG_BADGE", "FLAG_DAILY", "FLAG_UNUSED", "FLAG_DECORATION", "FLAG_HIDE_SECRET_BASE",
        "FLAG_ITEM_", "FLAG_HIDDEN_ITEM_", "FLAG_EC_ITEM_", "FLAG_RECEIVED_", "FLAG_DEFEATED_", "FLAG_EC_CAUGHT", "FLAG_LANDMARK_",
        "FLAG_MET_", "FLAG_VISITED_", "FLAG_REGISTERED_", "FLAG_LEGENDARY", "FLAG_EC_", "FLAG_HIDE_MAP_NAME")
def earliest(capset):
    if None in capset: return -1
    known = [c for c in capset if c is not None]
    return min(known) if known else None

forward = []; never = []
for m, c, kind, key, where in gates:
    if kind in ("flag", "hideflag"):
        if key.startswith(SKIP): continue
        sets = setters_flag.get(key, set())
        e = earliest(sets)
        if e is None: never.append((m, kind, key, where))
        elif c is not None and e > c: forward.append((m, c, kind, key, where, e))
    else:
        v, val = key
        if v.startswith(("VAR_TEMP", "VAR_0x8", "VAR_RESULT", "VAR_FACING", "VAR_LAST_TALKED", "VAR_TEXT_BOX", "VAR_STARTER", "VAR_EC", "VAR_DIFFICULTY")): continue
        if not val.isdigit(): continue
        sets = setters_var.get((v, val), set()) | setters_var.get((v, "*"), set())
        e = earliest(sets)
        if e is None: never.append((m, kind, key, where))
        elif c is not None and e > c: forward.append((m, c, kind, key, where, e))
print(f"gates: {len(gates)}; maps with a cap: {sum(1 for m in maps if cap_of(mj[m]['id']) is not None)}/{len(maps)}")
print(f"\nFORWARD DEPENDENCIES (gate chapter earlier than its earliest setter): {len(forward)}")
for r in sorted(set(forward), key=str): print("  ", r)
print(f"\nGATES WITH NO SETTER ANYWHERE: {len(never)}")
for r in sorted(set(never), key=str): print("  ", r)
