#!/usr/bin/env python3
"""Per-Mega-Stone register: enabled form(s), world acquisition source, every
trainer holder (with class and whether mega_slots actually authorizes battle
evolution), a signature owner, secondary owners, and the earliest cap window
in which the player can obtain the matching base family.

This is a design-review index, not a reachability or showcase-acceptance
claim. In particular:
  - "player family access" only traces ordinary wild encounter tables,
    givemon/giveegg script grants and legendary-sign wild/visible sources.
    Trades, Pickup/lottery odds and other indirect routes are not modeled;
    a species with none of those recorded sources is printed "unmapped",
    not "unavailable".
  - "mega_slots permits evolution" reports whether the holder's authored
    mega_slots bitmask covers that party position, not whether the AI will
    actually trigger the Mega Evolution in a given battle.
  - Signature owner is the design tier used across this file, not a claim
    about difficulty or story weight:
      elite/leader > gym > admin/boss > rival > ace > brain > regular/casual/grunt
    Ties break on first-access order: the lower data/emerald_champions/trainer-review-index.json
    review_index (falling back to encounter number when a trainer id is not
    indexed) wins.
"""
from __future__ import annotations

import heapq
import json
import re
from collections import defaultdict
from pathlib import Path

import emerald_champions_teams as teams
import export_trainer_catalogue as trainers
from generate_emerald_champions_mega_archive import stones
from verify_mega_stone_rewards import world_reward_sources
from verify_trainer_ability_legality import preprocess_species_info

ROOT = Path(__file__).resolve().parents[1]
FORM_TABLES = ROOT / "src/data/pokemon/form_change_tables.h"
MASTER = ROOT / "data/emerald_champions/emerald_champions_master_battle_design.txt"
REVIEW_INDEX = ROOT / "data/emerald_champions/trainer-review-index.json"
WILD = ROOT / "src/data/wild_encounters.json"
LEGENDARY_SIGNS = ROOT / "src/data/pokemon/legendary_signs.h"
MAPS_DIR = ROOT / "data/maps"

# Design tier used only to pick a "signature" holder among several; not a
# difficulty or story-importance ranking. "boss" is not named in the user's
# order and is grouped with admin as the nearest equivalent multi-owner class.
TIER = {
    "elite": 6, "leader": 6,
    "gym": 5,
    "admin": 4, "boss": 4,
    "rival": 3,
    "ace": 2,
    "brain": 1,
    "regular": 0, "casual": 0, "grunt": 0,
}

SPECIAL_FORM_NOTES = {
    "SPECIES_RAYQUAZA_MEGA": "Mega Rayquaza -- move-triggered (Dragon Ascent), not one of the 99 item-based stones; listed separately.",
    "SPECIES_MEOWSTIC_F_MEGA": "female Meowstic form of Meowsticite (shares the stone with SPECIES_MEOWSTIC_M_MEGA).",
    "SPECIES_MAGEARNA_ORIGINAL_MEGA": "original-color Magearna form of Magearnite (shares the stone with SPECIES_MAGEARNA_MEGA).",
    "SPECIES_TATSUGIRI_CURLY_MEGA": "Tatsugiri Curly form of Tatsugirinite.",
    "SPECIES_TATSUGIRI_DROOPY_MEGA": "Tatsugiri Droopy form of Tatsugirinite.",
    "SPECIES_TATSUGIRI_STRETCHY_MEGA": "Tatsugiri Stretchy form of Tatsugirinite.",
}


def pretty(token: str) -> str:
    return re.sub(r"^(SPECIES|ITEM|MOVE)_", "", token).replace("_", " ").title()


def item_forms() -> dict[str, list[str]]:
    """ITEM_x -> every SPECIES_x_MEGA[...] it triggers (some items cover more
    than one form: Meowsticite, Magearnite, Tatsugirinite)."""
    text = FORM_TABLES.read_text()
    forms: dict[str, list[str]] = defaultdict(list)
    for species, item in re.findall(
        r"FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM,\s*(SPECIES_[A-Z0-9_]+),\s*(ITEM_[A-Z0-9_]+)", text
    ):
        forms[item].append(species)
    return dict(forms)


def move_triggered_forms() -> dict[str, str]:
    """SPECIES_x_MEGA -> MOVE_x for the non-item Mega triggers (Rayquaza)."""
    text = FORM_TABLES.read_text()
    return dict(re.findall(
        r"FORM_CHANGE_BATTLE_MEGA_EVOLUTION_MOVE,\s*(SPECIES_[A-Z0-9_]+),\s*(MOVE_[A-Z0-9_]+)", text
    ))


def base_species(mega_species: str) -> str:
    """Strip a Mega/Mega-X/Y/Z suffix back to the base species token."""
    return re.sub(r"_MEGA(_[XYZ])?$", "", mega_species)


def evolution_edges() -> dict[str, set[str]]:
    """Undirected adjacency over configured evolution relationships, used to
    find every family member (pre-evolutions and evolutions) of a species."""
    text = preprocess_species_info()
    text = text[text.index("const struct SpeciesInfo gSpeciesInfo[]"):]
    marks = list(re.finditer(r"\[(SPECIES_\w+)\]\s*=\s*\{", text))
    adjacency: dict[str, set[str]] = defaultdict(set)
    for i, m in enumerate(marks):
        body = text[m.end():marks[i + 1].start() if i + 1 < len(marks) else len(text)]
        evo = re.search(r"\.evolutions\s*=\s*(.*?)(?:\n\s*\.\w+\s*=|\n\s*\},?\s*\Z)", body, re.S)
        if not evo:
            continue
        for target in re.findall(r"SPECIES_\w+", evo[1]):
            adjacency[m[1]].add(target)
            adjacency[target].add(m[1])
    return adjacency


def family_of(species: str, adjacency: dict[str, set[str]]) -> set[str]:
    if species not in adjacency:
        return {species}
    seen = {species}
    stack = [species]
    while stack:
        cur = stack.pop()
        for nxt in adjacency[cur]:
            if nxt not in seen:
                seen.add(nxt)
                stack.append(nxt)
    return seen


def map_dir_to_id() -> dict[str, str]:
    result = {}
    for path in sorted(MAPS_DIR.glob("*/map.json")):
        try:
            result[path.parent.name] = json.loads(path.read_text())["id"]
        except (OSError, ValueError, KeyError):
            continue
    return result


# ---------------------------------------------------------------------------
# Map -> cap-window reachability index
# ---------------------------------------------------------------------------
#
# A map's window used to be "the earliest strict_cap of any authored trainer
# encounter located there" -- which left 188 map directories with no trainer
# (Route101, GraniteCave_1F/B1F/B2F, ArtisanCave, most houses/interiors)
# unplaced. This index instead walks the physical map graph -- nodes are map
# directories, edges are each map.json's 'connections' (routes/towns) and
# 'warp_events' (doors/staircases/cave mouths) -- starting from the six early
# towns everyone begins in, and floods outward. A map's window is the
# cheapest path from a seed: the minimum, over every incoming edge, of
# max(neighbour's window, that edge's gate cost). Gate cost comes from:
#   (a) the map's own authored trainer strict_cap (the old index), when
#       that is higher than the graph alone would give -- MAP_GATES folds
#       this in as a floor via _trainer_cap_by_dir();
#   (b) field-move and story gates from the guide, encoded below.
# This is still a design-review index, not a tile-by-tile proof of walkable
# geometry -- see the gate table's own notes for the judgment calls (e.g.
# where a gate has no distinct map node to hang off, or a scripted boat trip
# has no literal warp_events entry and needs a synthetic EXTRA_EDGES entry).
#
# gate                                  window  where (map = MAP_GATES, edge = EDGE_GATES)      rationale
# ------------------------------------  ------  -----------------------------------------------  ---------------------------------------------
# Cut                                       20  edge Route116 <-> RusturfTunnel                  cut trees screen the tunnel mouth
#                                            20  edge PetalburgWoods <-> PetalburgWoods_2          cut trees deeper in the woods
#                                            20  edge PetalburgWoods_2 <-> PetalburgWoods_3         "
# Rock Smash                                30  edge RusturfTunnel <-> VerdanturfTown            the Wanda/Peeko reunion rubble
# Flash (free w/ Stone Badge, not          20  map GraniteCave_B1F, GraniteCave_B2F              lower floors need Flash, not Rock Smash;
#   Rock Smash)                                                                                   Stone Badge = cap 20
# Go-Goggles (after Flannery, badge 4)      42  map DesertRuins, MirageTower_1F,                  the whole Route 111 desert dungeon complex
#                                                    DesertUnderpass, SandstrewnRuins
# Mach/Acro Bike (after Mauville)           30  map Route110_SeasideCyclingRoadNorthEntrance,     the cycling road needs a bike to cross
#                                                    Route110_SeasideCyclingRoadSouthEntrance
# Briney ferry (after Mr. Stone's letter)   20  extra edge Route104 <-> DewfordTown               scripted sail, no literal warp_events entry
# Dewford -> Slateport (after the manor)    24  extra edge DewfordTown <-> SlateportCity          scripted sail, no literal warp_events entry
# Route110 north (after the museum)         24  edge Route110 <-> MauvilleCity                    Oceanic Museum story gate
# Route111 north of Mauville (Wattson)      30  edge Route111 <-> Route113                        badge-3 story gate on the desert-side leg
# Jagged Pass / Lavaridge (after Chimney)   36  map JaggedPass, LavaridgeTown                     Team Magma driven off Mt. Chimney
# Surf                                      48  edge Route118 <-> Route123                        the river crossing partway along Route 118
# Lilycove east (after Winona)              60  edge LilycoveCity <-> Route124                    badge-7 story gate
# S.S. Tidal islands (Lilycove rival)       60  map SSTidalCorridor; extra edges                 rival battle reveals the ship's other stops;
#                                                    LilycoveCity_Harbor/SlateportCity_Harbor      no literal warp -- the ferry destination is
#                                                    <-> SSTidalCorridor                            picked by script, not stored in map.json
# Mt. Pyre (after the voyage)               68  map MtPyre_1F, MtPyre_Exterior                    Route122 warps straight into 1F, not Exterior
# Magma Hideout (after Mt. Pyre)            68  map MagmaHideout_1F
# Scorched Slab (after Groudon)             68  map ScorchedSlab
# Aqua Hideout (after Heatran)              76  map AquaHideout_1F
# Mossdeep (after the hideout)              76  map MossdeepCity
# Seafloor Cavern / Sootopolis (Space       84  map SeafloorCavern_Entrance, SootopolisCity;       Dive surfaces straight into these with no
#   Center)                                        extra edges Underwater_SeafloorCavern <->        map.json connection either; every 'dive'/
#                                                    SeafloorCavern_Entrance, Underwater_            'emerge' connection is also auto-gated at
#                                                    SootopolisCity <-> SootopolisCity                the Dive constant (84), see below
# Sky Pillar (after Kyogre)                 84  map SkyPillar_Entrance
# Waterfall / eight badges                  90  map EverGrandeCity, VictoryRoad_1F
# League                                    96  map EverGrandeCity_PokemonLeague_1F
#
# Dive (84) is additionally applied automatically to every map.json
# connection whose direction is 'dive' or 'emerge' (see _build_map_graph),
# not just the two extra edges above. Strength (42) and Fly (60) have no
# instance called out by the guide beyond what is already covered above;
# FIELD_MOVE_CAPS keeps all of these as the canonical constants a designer
# should reach for when adding a new MAP_GATES/EDGE_GATES/EXTRA_EDGES entry,
# rather than inventing a new number.
#
# A designer correcting this table: add/move entries in MAP_GATES (whole
# map, any incoming edge), EDGE_GATES (one specific connection/warp, keyed
# by an unordered pair of directory names), or EXTRA_EDGES (a connection
# with no literal map.json entry at all, e.g. another scripted boat trip).
# Every window must be one of CAP_WINDOWS (src/caps.c's sCampaignMilestones
# order, plus the 14 baseline before the first milestone).

CAP_WINDOWS = [14, 20, 30, 40, 45, 55, 60, 65, 70, 80, 100]

# The six early towns/routes every save starts able to walk between.
SEED_MAP_DIRS = ["LittlerootTown", "Route101", "OldaleTown", "Route103", "Route102", "PetalburgCity"]
SEED_WINDOW = 14

FIELD_MOVE_CAPS = {
    "cut": 20, "rock_smash": 30, "strength": 42, "surf": 48,
    "fly": 60, "dive": 84, "waterfall": 90,
}

MAP_GATES: dict[str, int] = {
    "GraniteCave_B1F": 20, "GraniteCave_B2F": 20,
    "DesertRuins": 42, "MirageTower_1F": 42, "DesertUnderpass": 42, "SandstrewnRuins": 42,
    "Route110_SeasideCyclingRoadNorthEntrance": 30, "Route110_SeasideCyclingRoadSouthEntrance": 30,
    "JaggedPass": 36, "LavaridgeTown": 36,
    "SSTidalCorridor": 60,
    # Route122 warps straight into MtPyre_1F (Exterior is reached only from
    # inside), so the story floor has to sit on the actual entry point too.
    "MtPyre_1F": 68, "MtPyre_Exterior": 68,
    "MagmaHideout_1F": 68,
    "ScorchedSlab": 68,
    "AquaHideout_1F": 76,
    "MossdeepCity": 76,
    "SeafloorCavern_Entrance": 84, "SootopolisCity": 84,
    "SkyPillar_Entrance": 84,
    "EverGrandeCity": 90, "VictoryRoad_1F": 90,
    "EverGrandeCity_PokemonLeague_1F": 96,
}

EDGE_GATES: dict[frozenset, int] = {
    frozenset({"Route116", "RusturfTunnel"}): 20,
    frozenset({"PetalburgWoods", "PetalburgWoods_2"}): 20,
    frozenset({"PetalburgWoods_2", "PetalburgWoods_3"}): 20,
    frozenset({"RusturfTunnel", "VerdanturfTown"}): 30,
    frozenset({"Route110", "MauvilleCity"}): 24,
    frozenset({"Route111", "Route113"}): 30,
    frozenset({"Route118", "Route123"}): 48,
    frozenset({"LilycoveCity", "Route124"}): 60,
}

# Scripted boat trips and Dive surfacing points with no literal
# connections/warp_events entry in map.json (the destination is chosen by a
# script/coordinate event at runtime, not stored as static map data).
EXTRA_EDGES: list[tuple[str, str, int]] = [
    ("Route104", "DewfordTown", 20),
    ("DewfordTown", "SlateportCity", 24),
    ("LilycoveCity_Harbor", "SSTidalCorridor", 60),
    ("SlateportCity_Harbor", "SSTidalCorridor", 60),
    # Dive surfaces directly into these cities; no map.json connection covers it.
    ("Underwater_SootopolisCity", "SootopolisCity", 84),
    ("Underwater_SeafloorCavern", "SeafloorCavern_Entrance", 84),
]


def _trainer_cap_by_dir() -> dict[str, int]:
    """Directory -> earliest strict_cap of any authored trainer encounter
    whose 'location' field names that directory. This is the previous,
    trainer-presence-only index; it is now used only as a floor (gate table
    point (a)), folded into the graph walk alongside MAP_GATES."""
    dir_to_id = map_dir_to_id()
    _, blocks = teams.split_encounters(MASTER.read_text())
    cap_by_dir: dict[str, int] = {}
    for _number, block in blocks:
        meta = trainers.fields(block)
        loc = meta.get("location", "").strip()
        cap = meta.get("strict_cap", "").strip()
        if loc not in dir_to_id or not cap.isdigit():
            continue
        cap = int(cap)
        if loc not in cap_by_dir or cap < cap_by_dir[loc]:
            cap_by_dir[loc] = cap
    return cap_by_dir


# Multiplayer link-room maps: every Pokemon Center 2F (and a couple of
# other rooms) carries a warp_events entry into one of these, and the maps
# themselves warp back out to a script-chosen 'MAP_DYNAMIC' partner instead
# of a fixed destination. Left in, they wormhole every Pokemon Center in the
# game together (18 distinct maps warp into MAP_TRADE_CENTER alone) -- not a
# walkable path, so they are excluded from the graph entirely.
HUB_EXCLUDE_MAPS = {
    "TradeCenter", "UnionRoom", "RecordCorner",
    "BattleColosseum_2P", "BattleColosseum_4P",
}


def _build_map_graph() -> dict[str, list[tuple[str, int]]]:
    """Undirected adjacency over data/maps/*/map.json 'connections' and
    'warp_events', plus EXTRA_EDGES. Each edge carries its own EDGE_GATES
    cost (0 if none). HUB_EXCLUDE_MAPS are dropped -- see its docstring."""
    dir_to_id = map_dir_to_id()
    id_to_dir = {v: k for k, v in dir_to_id.items()}
    adjacency: dict[str, list[tuple[str, int]]] = defaultdict(list)

    def add_edge(a: str, b: str, gate: int | None = None) -> None:
        if a == b or a in HUB_EXCLUDE_MAPS or b in HUB_EXCLUDE_MAPS:
            return
        if gate is None:
            gate = EDGE_GATES.get(frozenset({a, b}), 0)
        adjacency[a].append((b, gate))
        adjacency[b].append((a, gate))

    for path in sorted(MAPS_DIR.glob("*/map.json")):
        a = path.parent.name
        try:
            data = json.loads(path.read_text())
        except (OSError, ValueError):
            continue
        for conn in data.get("connections") or []:
            b = id_to_dir.get(conn.get("map"))
            if not b:
                continue
            if conn.get("direction") in ("dive", "emerge"):
                add_edge(a, b, FIELD_MOVE_CAPS["dive"])
            else:
                add_edge(a, b)
        for warp in data.get("warp_events") or []:
            b = id_to_dir.get(warp.get("dest_map"))
            if b:
                add_edge(a, b)
    for a, b, gate in EXTRA_EDGES:
        add_edge(a, b, gate)
    return adjacency


def _reachability_windows() -> tuple[dict[str, int], dict[str, tuple[str | None, int, str]]]:
    """Minimax-path (bottleneck shortest path) flood from SEED_MAP_DIRS:
    window(v) = min over incoming edges (u, v) of max(window(u), edge gate,
    MAP_GATES/trainer-floor(v)). Returns (window_by_dir, trace) where
    trace[dir] = (predecessor_dir_or_None, edge_gate_used, 'seed'|'edge'),
    kept for explain_map_window()."""
    adjacency = _build_map_graph()
    trainer_floor = _trainer_cap_by_dir()

    def floor_of(d: str) -> int:
        return max(trainer_floor.get(d, 0), MAP_GATES.get(d, 0))

    dist: dict[str, int] = {}
    trace: dict[str, tuple[str | None, int, str]] = {}
    heap: list[tuple[int, str]] = []
    for seed in SEED_MAP_DIRS:
        w = max(SEED_WINDOW, floor_of(seed))
        dist[seed] = w
        trace[seed] = (None, 0, "seed")
        heapq.heappush(heap, (w, seed))

    visited: set[str] = set()
    while heap:
        d, node = heapq.heappop(heap)
        if node in visited:
            continue
        visited.add(node)
        for neighbour, edge_gate in adjacency.get(node, []):
            candidate = max(d, edge_gate, floor_of(neighbour))
            if neighbour not in dist or candidate < dist[neighbour]:
                dist[neighbour] = candidate
                trace[neighbour] = (node, edge_gate, "edge")
                heapq.heappush(heap, (candidate, neighbour))
    return dist, trace


def map_cap_index() -> dict[str, int]:
    """MAP_x -> earliest cap window at which that map is reachable, by graph
    reachability over data/maps/*/map.json (see the gate table above), not
    merely trainer presence. Maps unreachable from SEED_MAP_DIRS through
    this graph -- postgame/unused areas such as the Battle Frontier's own
    interior chain (reached only by a ferry ticket this index does not
    model) -- are not included."""
    windows, _trace = _reachability_windows()
    dir_to_id = map_dir_to_id()
    cap_by_map: dict[str, int] = {}
    for d, w in windows.items():
        mapid = dir_to_id.get(d)
        if mapid:
            cap_by_map[mapid] = w
    return cap_by_map


def explain_map_window(mapdir: str) -> str:
    """Human-readable path + gate breakdown for one map directory's window,
    for scripts/reference_pool.py's --explain."""
    windows, trace = _reachability_windows()
    if mapdir not in windows:
        return f"{mapdir}: unreachable from SEED_MAP_DIRS through this graph (postgame/unused)."
    path: list[str] = []
    cur: str | None = mapdir
    while cur is not None:
        path.append(cur)
        cur = trace[cur][0]
    path.reverse()

    trainer_floor = _trainer_cap_by_dir()
    lines = [f"{mapdir}: window={windows[mapdir]}", "path: " + " -> ".join(path)]
    for name in path:
        pred, edge_gate, note = trace[name]
        parts = []
        if note == "seed":
            parts.append(f"seed window {SEED_WINDOW}")
        elif edge_gate:
            parts.append(f"edge gate {edge_gate} (from {pred})")
        elif pred:
            parts.append(f"no edge gate (from {pred})")
        map_gate = MAP_GATES.get(name)
        if map_gate:
            parts.append(f"map gate {map_gate}")
        floor = trainer_floor.get(name)
        if floor:
            parts.append(f"trainer strict_cap floor {floor}")
        lines.append(f"  {name}: window={windows[name]}" + (" (" + ", ".join(parts) + ")" if parts else ""))
    return "\n".join(lines)


def species_wild_maps() -> dict[str, set[str]]:
    """SPECIES_x -> set of MAP_x ids where it appears in the wild table."""
    data = json.loads(WILD.read_text())
    result: dict[str, set[str]] = defaultdict(set)
    for group in data["wild_encounter_groups"]:
        if not group.get("for_maps"):
            continue
        for entry in group.get("encounters", []):
            mapid = entry.get("map")
            if not mapid:
                continue
            for field in entry.values():
                if isinstance(field, dict) and "mons" in field:
                    for mon in field["mons"]:
                        result[mon["species"]].add(mapid)
    return result


def species_gift_maps() -> dict[str, set[str]]:
    """SPECIES_x -> set of MAP_x ids where a givemon/giveegg script grants it
    (map-local scripts only). No Hoenn-side fossil-revival script pattern distinct from
    givemon was found under data/maps or data/scripts."""
    dir_to_id = map_dir_to_id()
    result: dict[str, set[str]] = defaultdict(set)
    for path in sorted(MAPS_DIR.glob("*/scripts.inc")):
        mapid = dir_to_id.get(path.parent.name)
        if not mapid:
            continue
        text = path.read_text()
        for species in re.findall(r"\b(?:givemon|giveegg)\s+(SPECIES_\w+)", text):
            result[species].add(mapid)
    return result


def species_legendary_sign_maps() -> dict[str, set[str]]:
    """SPECIES_x -> set of MAP_x ids from wild/visible legendary sign macros
    (OTHER_SIGN entries use a non-map source constant and are skipped)."""
    text = LEGENDARY_SIGNS.read_text()
    result: dict[str, set[str]] = defaultdict(set)
    for kind, species, maparg in re.findall(
        r"\b(RARE_WILD_SIGN|NATIVE_WILD_SIGN|VISIBLE_SIGN|ORDINARY_WILD_SIGN)\(\s*\w+,\s*(\w+),\s*(\w+)", text
    ):
        result["SPECIES_" + species].add("MAP_" + maparg)
    return result


def review_index_order() -> dict[str, int]:
    """TRAINER_x -> review_index, from data/emerald_champions/trainer-review-index.json."""
    if not REVIEW_INDEX.exists():
        return {}
    data = json.loads(REVIEW_INDEX.read_text())
    order: dict[str, int] = {}
    for encounter in data["encounters"]:
        for trainer_id in encounter["trainer_ids"]:
            order[trainer_id] = encounter["review_index"]
    return order


def build_register() -> list[dict]:
    all_stones = stones()
    forms = item_forms()
    rewards = world_reward_sources()
    branches = teams.read_teams()
    order = review_index_order()
    adjacency = evolution_edges()
    cap_by_map = map_cap_index()
    wild_maps = species_wild_maps()
    gift_maps = species_gift_maps()
    sign_maps = species_legendary_sign_maps()

    rows = []
    for item in all_stones:
        holders = []
        for branch in branches:
            for index, mon in enumerate(branch.mons):
                if "ITEM_" + mon.item != item:
                    continue
                can_evolve = bool(branch.mega_slots is not None and (branch.mega_slots >> index) & 1)
                holders.append(dict(
                    trainer=branch.trainer, encounter=f"E{branch.encounter:04d}",
                    cls=branch.cls, can_evolve=can_evolve,
                    review_index=order.get(branch.trainer),
                ))
        if holders:
            def sort_key(h):
                tier = TIER.get(h["cls"], 0)
                access = h["review_index"] if h["review_index"] is not None else 10 ** 9
                return (-tier, access)
            holders.sort(key=sort_key)
            signature = holders[0]
            secondary = holders[1:]
        else:
            signature = None
            secondary = []

        species_forms = forms.get(item, [])
        family_caps = []
        unmapped_species = []
        for form_species in species_forms:
            root = base_species(form_species)
            family = family_of(root, adjacency)
            maps = set()
            for sp in family:
                maps |= wild_maps.get(sp, set())
                maps |= gift_maps.get(sp, set())
                maps |= sign_maps.get(sp, set())
            caps = [cap_by_map[m] for m in maps if m in cap_by_map]
            if caps:
                family_caps.append(min(caps))
            else:
                unmapped_species.append(root)

        rows.append(dict(
            stone=item,
            forms=species_forms,
            world_source=rewards.get(item, []),
            holders=holders,
            signature_owner=signature,
            secondary_owners=secondary,
            player_family_access=(min(family_caps) if family_caps else None),
            player_family_access_unmapped=sorted(set(unmapped_species)) if not family_caps else [],
            no_trainer_holder=not holders,
        ))
    return rows


def special_form_rows() -> list[dict]:
    """Rows for forms the book calls out that fall outside the 99 item-based
    stones (Mega Rayquaza) or that share one of those stones with a sibling
    form already covered by build_register (Meowstic/Magearna/Tatsugiri --
    reprinted here for a single reference list, per the task's request)."""
    branches = teams.read_teams()
    moves = move_triggered_forms()
    rows = []
    for species, move in moves.items():
        root = base_species(species)
        owners = []
        for branch in branches:
            for index, mon in enumerate(branch.mons):
                if mon.species != root[len("SPECIES_"):]:
                    continue
                has_move = move[len("MOVE_"):] in mon.moves
                owners.append(dict(trainer=branch.trainer, cls=branch.cls, encounter=f"E{branch.encounter:04d}", has_trigger_move=has_move))
        rows.append(dict(kind="move-triggered", species=species, trigger=move,
                          note=SPECIAL_FORM_NOTES.get(species, ""), owners=owners))
    for species, note in SPECIAL_FORM_NOTES.items():
        if species in moves:
            continue
        rows.append(dict(kind="shared-stone-form", species=species, trigger=None, note=note, owners=[]))
    return rows


def render_lines() -> tuple[list[str], list[dict], list[dict]]:
    rows = build_register()
    specials = special_form_rows()
    no_holder = [r["stone"] for r in rows if r["no_trainer_holder"]]
    lines = [
        "\nMEGA REGISTER",
        f"{len(rows)} item-based Mega Stones. Signature owner = highest design tier "
        "(elite/leader>gym>admin/boss>rival>ace>brain>regular/casual/grunt), ties broken "
        "by earlier data/emerald_champions/trainer-review-index.json review_index. mega_slots=yes means the "
        "holder's authored Mega permission bitmask covers that party position, not that the "
        "AI will trigger it in a given battle. player_family_access is the earliest strict_cap "
        "of any authored trainer encounter located on a map where the base family is available "
        "in the wild table, a givemon/giveegg script grant, or a legendary sign; 'unmapped' means "
        "no such source was found under this index's rules (trades and Pickup/lottery odds are "
        "not modeled).",
        f"NO TRAINER HOLDER rows: {len(no_holder)}" + (f" -- {', '.join(no_holder)}" if no_holder else " (none)"),
    ]
    for row in rows:
        forms_str = ", ".join(pretty(f) for f in row["forms"]) or "(no form found in form_change_tables.h)"
        source_str = "; ".join(row["world_source"]) or "NO WORLD SOURCE FOUND"
        lines.append(f"\n{pretty(row['stone'])} ({row['stone']}) -> {forms_str}")
        lines.append(f"  World source: {source_str}")
        if row["no_trainer_holder"]:
            lines.append("  NO TRAINER HOLDER")
        else:
            sig = row["signature_owner"]
            lines.append(f"  Signature owner: {sig['trainer']} ({sig['encounter']}, class={sig['cls']}, "
                          f"mega_slots evolves this slot={'yes' if sig['can_evolve'] else 'no'})")
            if row["secondary_owners"]:
                lines.append("  Secondary owners: " + "; ".join(
                    f"{h['trainer']} ({h['encounter']}, class={h['cls']}, "
                    f"mega_slots evolves this slot={'yes' if h['can_evolve'] else 'no'})"
                    for h in row["secondary_owners"]))
        if row["player_family_access"] is not None:
            lines.append(f"  Player family access: cap {row['player_family_access']}")
        else:
            lines.append("  Player family access: unmapped" + (
                f" ({', '.join(pretty(s) for s in row['player_family_access_unmapped'])})"
                if row["player_family_access_unmapped"] else ""))
    lines.append("\nSPECIAL FORMS CALLED OUT BY THE BOOK")
    for row in specials:
        if row["kind"] == "move-triggered":
            owners = row["owners"]
            if owners:
                status = "; ".join(f"{o['trainer']} ({o['encounter']}, class={o['cls']}, "
                                    f"has {pretty(row['trigger'])}={'yes' if o['has_trigger_move'] else 'no'})" for o in owners)
            else:
                status = "NO TRAINER HOLDER of the base species"
            lines.append(f"{pretty(row['species'])} -- move-triggered by {pretty(row['trigger'])}: {status}")
        else:
            lines.append(f"{pretty(row['species'])} -- {row['note']}")
    return lines, rows, specials


def generate(root: Path = ROOT) -> tuple[list[str], dict, set[Path]]:
    lines, rows, specials = render_lines()
    catalog = dict(mega_register=rows, special_forms=specials)
    paths = {
        Path(__file__), FORM_TABLES, MASTER, REVIEW_INDEX, WILD, LEGENDARY_SIGNS,
        root / "src/data/emerald_champions_mega_stones.h",
        teams.TEAMS,
    }
    return lines, catalog, paths


def main() -> None:
    lines, catalog, _paths = generate()
    print("\n".join(lines))
    out = ROOT / "work/mega-register.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(catalog, indent=2) + "\n")
    no_holder = [r["stone"] for r in catalog["mega_register"] if r["no_trainer_holder"]]
    print(f"\n{'PASS' if not no_holder else 'FAIL'}: {len(catalog['mega_register'])} stones; "
          f"{len(no_holder)} with NO TRAINER HOLDER")


if __name__ == "__main__":
    main()
