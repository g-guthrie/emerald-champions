#!/usr/bin/env python3
"""Story order vs level cap vs trainer distribution.

The player's level cap is a function of badge count (src/caps.c):
    0 badges 14, 1: 20, 2: 30, 3: 40, 4: 45, 5: 55, 6: 60, 7: 70, 8: 80, Champion: 100.
Every authored encounter carries a strict_cap (docs/emerald_champions_master_battle_design.txt).
This tool compares each encounter's strict_cap with the player's cap at the moment its map
first becomes reachable under the *current scripts*, using a hand-verified progression model
(EARLIEST_BADGES below, keyed by map-name prefix).  Update the model when a story gate changes.

Gates the model encodes (all read from data/maps/*/scripts.inc on 2026-09-03):
  * Slateport / Route 109 / Museum / Route 110 / Mauville / 117 / 111 north / 118 west:
    Briney sails to Slateport only after the Letter, the Knuckle Badge and the Mega Ring.
  * Route 111 north ... Lavaridge: nothing requires the Dynamo Badge (same as vanilla); the
    intended order puts Wattson first, so those maps are modelled at 3 badges.
  * Route 111 desert: Go-Goggles (after Flannery).  Ember Path / Ashen Woods: Strength (Heat Badge).
  * Petalburg Gym: Norman battles with 4 badges.  Surf: Balance Badge + Wally's father.
  * Route 120 past the bridge, 121, Mt. Pyre, hideouts, Mossdeep: Steven holds the bridge
    until the Feather Badge.  Space Center, Dive routes, Seafloor, Sootopolis: Mind Badge.
  * Victory Road / League: Waterfall (Rain Badge).  Postgame maps: Champion.
"""
import re, collections
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]

CAP_BY_BADGES = {0: 14, 1: 20, 2: 30, 3: 40, 4: 45, 5: 55, 6: 60, 7: 70, 8: 80, 9: 100}

# longest matching prefix wins
EARLIEST_BADGES = {
    "Route101": 0, "Route102": 0, "Route103": 0, "Route104": 0, "PetalburgWoods": 0, "PetalburgCity": 0,
    "RustboroCity": 0, "Route115": 0, "Route116": 0, "RusturfTunnel": 1,
    "Route106": 1, "DewfordTown": 1, "GraniteCave": 1, "DewfordMeadow": 1, "DewfordManor": 1,
    "Route109": 2, "SlateportCity": 2, "Route110": 2, "MauvilleCity": 2, "Route117": 2, "VerdanturfTown": 2,
    "VerdanturfMeadow": 2, "Route111": 2, "Route118": 2, "Seaspray_Cave": 2,
    "Route112": 3, "FieryPath": 3, "Route113": 3, "FallarborTown": 3, "Route114": 3, "MeteorFalls_1F": 3,
    "MtChimney": 3, "JaggedPass": 3, "LavaridgeTown": 3, "Global_gabby_and_ty": 3,
    "Route111_desert": 4, "EmberPath": 4, "AshenWoods": 4, "PetalburgCity_Gym": 4, "MirageTower": 4,
    "SandstrewnRuins": 4, "DesertUnderpass": 4,
    "Route105": 5, "Route107": 5, "Route108": 5, "AbandonedShip": 5, "Route103_east": 5, "Route109_water": 5,
    "Route115_north": 5, "Route118_east": 5, "Route119": 5, "FortreeCity": 5, "Route120": 5, "Route123": 5,
    "Route121": 6, "LilycoveCity": 6, "MtPyre": 6, "MagmaHideout": 6, "AquaHideout": 6,
    "Route124": 6, "Route125": 6, "Route126": 6, "Route127": 6, "Route128": 6, "MossdeepCity": 6,
    "MossdeepCity_SpaceCenter": 7, "Route129": 7, "Route130": 7, "Route131": 7, "Route132": 7, "Route133": 7,
    "Route134": 7, "SeafloorCavern": 7, "SootopolisCity": 7, "SkyPillar": 7,
    "VictoryRoad": 8, "EverGrandeCity": 8,
    "SSTidal": 9, "CaveOfOrigin_DianciesRoom": 9, "AlteringCave": 9, "MeteorFalls_StevensCave": 9,
    "MossdeepCity_House1": 9, "TrainerHill": 9, "BattleFrontier": 9,
}
# Trick House puzzle N opens with badge N+1; puzzle 8 needs the game clear
# (Route110_TrickHouseEntrance_EventScript_CheckReadyForPuzzleN).
for n, b in {1: 2, 2: 3, 3: 4, 4: 5, 5: 6, 6: 7, 7: 8, 8: 9}.items():
    EARLIEST_BADGES[f"Route110_TrickHousePuzzle{n}"] = b
# Gabby & Ty: interview N is on Route 111 south / 118 east bank / 120 north in rotation;
# 2 and 3 need Surf (5 badges), 4-6 stay reachable at 5 badges (optional, authored 60).
EARLIEST_BADGES.update({"Global_gabby_and_ty_1": 2, "Global_gabby_and_ty_2": 5, "Global_gabby_and_ty_3": 5,
                        "Global_gabby_and_ty_4": 5, "Global_gabby_and_ty_5": 5, "Global_gabby_and_ty_6": 5})

# Locations whose trainer set is split across chapters in the design (same map, later water/area).
SPLIT_BY_CAP = {("Route111", 45): "Route111_desert", ("Route103", 55): "Route103_east", ("Route109", 55): "Route109_water",
                ("Route115", 55): "Route115_north", ("Route118", 55): "Route118_east", ("Route111", 40): "Route111",
                ("RusturfTunnel", 40): "RusturfTunnel_west",   # Mike, Verdanturf side, needs Rock Smash
                ("Route106", 55): "Route106_water",            # swimmers, need Surf
                ("Route104", 20): "Route104_rival",            # rival after the PokéNav (1 badge)
                ("VictoryRoad_1F", 100): "VictoryRoad_postgame"}
EARLIEST_BADGES.update({"RusturfTunnel_west": 3, "Route106_water": 5, "Route104_rival": 1, "VictoryRoad_postgame": 9})

def badges_for(loc, cap):
    key = SPLIT_BY_CAP.get((loc, cap), loc)
    best = None
    for k, v in EARLIEST_BADGES.items():
        if key == k or key.startswith(k + "_") or key.startswith(k):
            if best is None or len(k) > len(best[0]): best = (k, v)
    return best[1] if best else None

master = (ROOT/"docs/emerald_champions_master_battle_design.txt").read_text()
rows = []
for b in re.split(r"(?m)^=== ENCOUNTER \d{4} ===$", master)[1:]:
    g = lambda k: (re.search(rf"(?m)^{k}: (.+)$", b) or [None, None])[1]
    loc = g("location").split(";")[0].strip(); cap = int(g("strict_cap")); order = int(g("campaign_order"))
    if loc == "Global_gabby_and_ty": loc += "_" + (g("trainer_ids") or "").strip()[-1]
    req = g("requirement") or ""
    rows.append((order, loc, cap, req, g("physical_group_id")))

print(f"{len(rows)} encounters")
bad = []; unknown = []
for order, loc, cap, req, pid in rows:
    bd = badges_for(loc, cap)
    if bd is None: unknown.append((order, loc, cap, pid)); continue
    player = CAP_BY_BADGES[bd]
    if cap > player:
        bad.append((cap - player, order, loc, cap, player, "required" if "required" in req else "optional", pid))

print(f"\nENCOUNTERS ABOVE THE PLAYER'S CAP WHEN FIRST REACHABLE: {len(bad)}")
for gap, order, loc, cap, player, req, pid in sorted(bad, key=lambda r: (-r[0], r[1])):
    print(f"  +{gap:2d}  order {order:3d}  {loc:34s} trainer cap {cap:3d}  player cap {player:3d}  {req:8s} {pid}")
print(f"\nENCOUNTERS WITH NO MODELLED LOCATION: {len(unknown)}")
for r in unknown: print("  ", r)

# Where the design's campaign order disagrees with the reach model (an earlier-reached area
# authored later, or vice versa) even when caps line up.
print("\nCHAPTER CAP vs PLAYER CAP BY LOCATION (only where they differ):")
seen = set()
for order, loc, cap, req, pid in rows:
    bd = badges_for(loc, cap)
    if bd is None or (loc, cap) in seen: continue
    seen.add((loc, cap))
    player = CAP_BY_BADGES[bd]
    if player != cap: print(f"  {loc:34s} authored {cap:3d}  player {player:3d}  ({bd} badges)")
