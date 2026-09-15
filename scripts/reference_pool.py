#!/usr/bin/env python3
"""Generate the per-cap-window "available pool" -- the strongest legal, fully
invested player teams a cap window allows are drawn from this pool, not
authored by this script.

For every cap window in src/caps.c order (14 is the baseline before the first
milestone; the rest come from sCampaignMilestones), this lists every species
(and Mega form) obtainable at or before that window, together with the source
that first makes it obtainable and the window ("gate") that source clears.

Reuse and provenance
---------------------
- Map -> earliest cap window comes from scripts/mega_register.py's
  map_cap_index(): a graph-reachability index, not a trainer-presence index.
  Nodes are data/maps/*/map.json directories; edges are each map's
  'connections' and 'warp_events', plus a handful of synthetic EXTRA_EDGES
  for scripted boat trips and Dive surfacing points that have no literal
  map.json entry. Starting from the six early-game towns, a map's window is
  the cheapest path in: the minimum, over every incoming edge, of
  max(neighbour's window, that edge's gate cost), where gate cost folds in
  both the map's own authored trainer strict_cap (the old index, kept as a
  floor) and the story/field-move gate table documented at the top of
  mega_register.py (MAP_GATES / EDGE_GATES / EXTRA_EDGES -- run
  `python3 reference_pool.py --explain MAP_NAME` to print the path and
  gates that produced any one map's window). This covers almost every map
  that is reachable at all in this tree (382 of 958, versus 96 under the
  old trainer-only index); this script's one remaining extension on top of
  mega_register's index is a same-directory-prefix fallback ("CityName_*")
  for the rare map that still has no direct entry, marked 'derived' wherever
  it is used (a direct hit is marked 'direct'). Maps with neither are
  'unmapped' and are excluded, not guessed at -- what remains unmapped after
  both steps is genuinely postgame/mystery-event/unused content (Battle
  Frontier facility interiors and its own reception routing, FRLG twin
  maps, Navel/Birth/Southern/Faraway Island, secret bases, unused/prototype
  maps), not ordinary overworld content. See summary.md for the current
  list and mega_register.py's own docstring for the gate table and its
  documented judgment calls.
- Wild encounters: src/data/wild_encounters.json, mapped to a window via the
  map's resolved cap plus a field-move gate for the encounter table's
  'type': water_mons needs Surf (after Norman, window 48); rock_smash_mons
  needs Rock Smash (after Wattson, window 30); fishing_mons needs a rod, and
  the old_rod sub-slots are free from the start (Mom, window 14) while
  good_rod/super_rod sub-slots are gated by that rod's own gift-map window
  (see ROD_SOURCE_MAPS below). land_mons carries only the map's own window.
  Cut/Rock Smash/Dive are additionally modeled as map-graph reachability
  gates inside mega_register.map_cap_index() itself (see its gate table),
  so a map behind one of those already carries the right window before this
  script ever looks at it; this script's own water_mons/rock_smash_mons
  Surf/Rock-Smash bump above is specific to which *encounter table* on an
  already-reachable map needs the move, which map_cap_index does not
  distinguish. Strength/Fly/Waterfall have no per-wild-table equivalent and
  no instance named by the guide beyond what map_cap_index's gate table
  already encodes; that is a real limit on this index -- it is called out
  again in summary.md.
- Gifts/eggs/fossils and legendary signs reuse
  scripts/mega_register.py's species_gift_maps() (any givemon/giveegg in a
  map's scripts.inc -- no separate Hoenn fossil-revival pattern was found,
  matching mega_register's own note) and species_legendary_sign_maps()
  (RARE_WILD_SIGN/NATIVE_WILD_SIGN/VISIBLE_SIGN/ORDINARY_WILD_SIGN entries in
  legendary_signs.h; OTHER_SIGN entries have no map -- mastery/circuit/
  breeding/game-corner sources -- and are listed separately as unmapped).
- NPC trades (ingame_trade) are read from src/data/trade.h + the
  'ingame_trade' script command in data/maps/*/scripts.inc.
- Game Corner (Coin) and Champions Circuit BP prizes come from
  scripts/economy_reference.py's build_catalog()['game_corner_offers'] /
  ['circuit_bp_pokemon_offers'].
- Evolution-item and Mega-stone shop windows also come from
  economy_reference.build_catalog()['records'] (kind in {'shop',
  'native shop'}), read for their 'possible_maps' and resolved the same way
  as any other map. Mega stone *world* (non-shop) sources come from
  scripts/verify_mega_stone_rewards.world_reward_sources(), whose strings are
  always "MapDirName: description".
- Evolutions expand each obtainable species to every family member whose
  evolutions clause (preprocessed gSpeciesInfo, the same table
  verify_trainer_ability_legality.py resolves) is satisfiable by a window:
  EVO_LEVEL with a level > 0 needs that level <= the window's cap; EVO_LEVEL
  with 0 (friendship/move/gender conditions) is free once the source species
  is obtainable; EVO_ITEM is gated by that item's own shop/gift window
  (ITEM_LINKING_CORD is hardcoded to window 14, matching the guide's "Cut
  after Roxanne... Linking Cord in the [Rustboro] Mart" chapter and this
  index's own derived RustboroCity_Mart fallback); EVO_TRADE with an
  IF_HOLD_ITEM condition uses that held item's window (a same-generation
  in-game trade is not modeled beyond that); bare EVO_TRADE (no held item)
  is skipped -- it needs a second player/cartridge, which this single-player
  reachability index does not model. Other evolution triggers
  (EVO_SCRIPT_TRIGGER, EVO_BATTLE_END, EVO_SPIN, EVO_LEVEL_BATTLE_ONLY,
  EVO_SPLIT_FROM_EVO -- Nincada/Shedinja, Karrablast/Shelmet, Runerigus,
  Urshifu, Primeape-Annihilape, Qwilfish-Overqwil, Stantler-Wyrdeer, etc.)
  are treated as free once the source species is obtainable and flagged
  method='other:<EVO_KIND>'; this is a real known simplification.
- A Mega form is available once its base family is obtainable AND the
  stone's resolved world-source window is reached AND the Mega Ring is held
  (after Brawly, window 24, per the task's fixed rule -- not derived).

None of this is a physical-reachability proof beyond what the reused
map_cap_index (plus this script's same-prefix fallback) encodes. See
summary.md for the concrete list of maps/items this run could not place.
"""
from __future__ import annotations

import json
import re
from collections import defaultdict
from pathlib import Path

import mega_register as mr
import economy_reference as econ
from verify_mega_stone_rewards import world_reward_sources
from verify_trainer_ability_legality import preprocess_species_info, species_aliases, resolve_species
from generate_emerald_champions_mega_archive import stones as mega_stones

ROOT = Path(__file__).resolve().parents[1]
WILD = ROOT / "src/data/wild_encounters.json"
TRADE_H = ROOT / "src/data/trade.h"
MAPS_DIR = ROOT / "data/maps"
OUT_DIR = ROOT / "work/reference-pools"
BENCHMARK = ROOT / "handoff/benchmarks/c15_available_pool.json"

CAPS = [14, 20, 24, 30, 36, 42, 48, 54, 60, 68, 76, 84, 90, 96, 100]

SURF_CAP = 48          # Norman
ROCK_SMASH_CAP = 30    # Wattson
MEGA_RING_CAP = 24      # Brawly, fixed by the task
LINKING_CORD_CAP = 14   # Rustboro Mart, fixed by the task

# Good/Super Rod gift sources (giveitem, not givemon, so mega_register's
# species_gift_maps() does not pick these up). Hoenn only; FRLG twins in
# this tree (Fuchsia/Route12) are out of scope everywhere else here too.
ROD_SOURCE_MAPS = {
    "good_rod": ["Route118", "Route114"],
    "super_rod": ["MossdeepCity_House3"],
}

SOURCE_PRIORITY = ["wild", "gift", "trade", "legendary_sign", "game_corner", "circuit_bp",
                    "evolution", "mega"]


def snap(raw: int | None) -> int | None:
    """Round a raw cap value up to the next official window in CAPS."""
    if raw is None:
        return None
    for w in CAPS:
        if w >= raw:
            return w
    return CAPS[-1]


class MapResolver:
    """Map directory name -> (window, confidence), reusing mega_register's
    index directly, and falling back to the cheapest same-prefix map when a
    directory has no direct authored-encounter cap."""

    def __init__(self):
        self.dir_to_id = mr.map_dir_to_id()
        self.cap_by_map = mr.map_cap_index()
        self.id_to_dir = {v: k for k, v in self.dir_to_id.items()}
        # Two fallback keys per mapped directory's first '_'-segment: the raw
        # segment ("PetalburgWoods") and the same segment with a trailing
        # "Town"/"City" stripped ("DewfordTown" -> "Dewford"), so a satellite
        # area named "<City><Area>" with no separator (DewfordManor,
        # DewfordMeadow) still falls back to its town's cap. This is a
        # directory-naming heuristic only; see the module docstring.
        by_prefix: dict[str, list[int]] = defaultdict(list)
        stripped_roots: dict[str, list[int]] = defaultdict(list)
        for mapid, cap in self.cap_by_map.items():
            d = self.id_to_dir.get(mapid)
            if not d:
                continue
            seg = d.split("_")[0]
            by_prefix[seg].append(cap)
            stripped = re.sub(r"(Town|City)$", "", seg)
            if len(stripped) >= 4 and stripped != seg:
                stripped_roots[stripped].append(cap)
        self.by_prefix = {k: min(v) for k, v in by_prefix.items()}
        self.stripped_roots = {k: min(v) for k, v in stripped_roots.items()}
        self._cache: dict[str, tuple[int | None, str]] = {}
        self.unmapped: set[str] = set()

    def resolve(self, mapdir: str) -> tuple[int | None, str]:
        if mapdir in self._cache:
            return self._cache[mapdir]
        mapid = self.dir_to_id.get(mapdir)
        if mapid and mapid in self.cap_by_map:
            result = (snap(self.cap_by_map[mapid]), "direct")
        else:
            seg = mapdir.split("_")[0]
            if seg in self.by_prefix:
                result = (snap(self.by_prefix[seg]), "derived")
            else:
                root = next((r for r in self.stripped_roots if seg.startswith(r)), None)
                if root is not None:
                    result = (snap(self.stripped_roots[root]), "derived")
                else:
                    result = (None, "unmapped")
                    self.unmapped.add(mapdir)
        self._cache[mapdir] = result
        return result

    def best(self, mapdirs) -> tuple[int | None, str | None]:
        """Cheapest window among several alternative maps for one source."""
        best_window = None
        best_conf = None
        for m in mapdirs:
            window, conf = self.resolve(m)
            if window is not None and (best_window is None or window < best_window):
                best_window, best_conf = window, conf
        return best_window, best_conf


# ---------------------------------------------------------------------------
# Root sources: species -> earliest (window, kind, detail)
# ---------------------------------------------------------------------------

def wild_roots(resolver: MapResolver) -> dict[str, tuple[int, str, str]]:
    data = json.loads(WILD.read_text())
    id_to_dir = resolver.id_to_dir
    roots: dict[str, tuple[int, str, str]] = {}

    def rod_cap(submethod: str) -> int:
        if submethod == "old_rod":
            return 14
        maps = ROD_SOURCE_MAPS.get(submethod, [])
        window, _ = resolver.best(maps)
        return window if window is not None else CAPS[-1]

    for group in data["wild_encounter_groups"]:
        if not group.get("for_maps"):
            continue
        for entry in group["encounters"]:
            mapid = entry.get("map")
            mapdir = id_to_dir.get(mapid)
            if not mapdir:
                continue
            map_window, _ = resolver.resolve(mapdir)
            if map_window is None:
                continue
            for field in group["fields"]:
                t = field["type"]
                if t not in entry:
                    continue
                mons = entry[t].get("mons", [])
                groups_ = field.get("groups", {})
                for idx, mon in enumerate(mons):
                    submethod = next((rod for rod, idxs in groups_.items() if idx in idxs), t)
                    if t == "water_mons":
                        window = max(map_window, SURF_CAP)
                    elif t == "rock_smash_mons":
                        window = max(map_window, ROCK_SMASH_CAP)
                    elif t == "fishing_mons":
                        window = max(map_window, rod_cap(submethod))
                    else:
                        window = map_window
                    sp = mon["species"]
                    detail = f"wild:{mapdir}/{submethod}"
                    prev = roots.get(sp)
                    if prev is None or window < prev[0]:
                        roots[sp] = (window, "wild", detail)
    return roots


def gift_roots(resolver: MapResolver) -> dict[str, tuple[int, str, str]]:
    roots: dict[str, tuple[int, str, str]] = {}
    for sp, maps in mr.species_gift_maps().items():
        dirs = [resolver.id_to_dir.get(m) for m in maps]
        dirs = [d for d in dirs if d]
        window, _ = resolver.best(dirs)
        if window is None:
            continue
        detail = f"gift:{sorted(dirs)[0]}"
        roots[sp] = (window, "gift", detail)
    return roots


def legendary_sign_roots(resolver: MapResolver) -> tuple[dict[str, tuple[int, str, str]], list[str]]:
    roots: dict[str, tuple[int, str, str]] = {}
    text = mr.LEGENDARY_SIGNS.read_text()
    other = sorted(re.findall(r"OTHER_SIGN\(\s*LEGENDARY_SIGN_\w+,\s*(\w+),", text))
    for sp, maps in mr.species_legendary_sign_maps().items():
        dirs = [resolver.id_to_dir.get(m) for m in maps]
        dirs = [d for d in dirs if d]
        window, _ = resolver.best(dirs)
        if window is None:
            continue
        roots[sp] = (window, "legendary_sign", f"legendary_sign:{sorted(dirs)[0]}")
    return roots, other


def trade_roots(resolver: MapResolver) -> dict[str, tuple[int, str, str]]:
    trade_text = TRADE_H.read_text()
    trade_species: dict[str, str] = {}
    for m in re.finditer(r"\[(INGAME_TRADE_\w+)\]\s*=\s*\{(.*?)\n\s*\}", trade_text, re.S):
        sp = re.search(r"\.species\s*=\s*(SPECIES_\w+)", m[2])
        if sp:
            trade_species[m[1]] = sp[1]
    roots: dict[str, tuple[int, str, str]] = {}
    for path in sorted(MAPS_DIR.glob("*/scripts.inc")):
        if "frlg" in path.parent.name.lower():
            continue
        text = path.read_text()
        for trade_id in re.findall(r"\bingame_trade\s+(INGAME_TRADE_\w+)", text):
            sp = trade_species.get(trade_id)
            if not sp:
                continue
            window, _ = resolver.resolve(path.parent.name)
            if window is None:
                continue
            prev = roots.get(sp)
            detail = f"trade:{path.parent.name}/{trade_id}"
            if prev is None or window < prev[0]:
                roots[sp] = (window, "trade", detail)
    return roots


def corner_roots(resolver: MapResolver, catalog: dict) -> tuple[dict, dict]:
    coin_window, _ = resolver.resolve("MauvilleCity_GameCorner")
    bp_window, _ = resolver.resolve("BattleFrontier_ExchangeServiceCorner")
    coins: dict[str, tuple[int, str, str]] = {}
    bp: dict[str, tuple[int, str, str]] = {}
    if coin_window is not None:
        for row in catalog["game_corner_offers"]:
            coins[row["species"]] = (coin_window, "game_corner", f"game_corner:{row['coins']}coins")
    if bp_window is not None:
        for row in catalog["circuit_bp_pokemon_offers"]:
            bp[row["species"]] = (bp_window, "circuit_bp", f"circuit_bp:{row['bp']}bp")
    return coins, bp


# ---------------------------------------------------------------------------
# Item windows, for evolution items and Mega stones
# ---------------------------------------------------------------------------

def item_windows(resolver: MapResolver, catalog: dict) -> dict[str, int]:
    windows: dict[str, list[int]] = defaultdict(list)
    for rec in catalog["records"].values():
        if rec.get("kind") not in ("shop", "native shop"):
            continue
        maps = rec.get("possible_maps", [])
        window, _ = resolver.best(maps)
        if window is None:
            continue
        for item in rec.get("outputs", []):
            windows[item].append(window)
    result = {item: min(vals) for item, vals in windows.items()}
    result["ITEM_LINKING_CORD"] = min(result.get("ITEM_LINKING_CORD", LINKING_CORD_CAP), LINKING_CORD_CAP)
    return result


def mega_stone_windows(resolver: MapResolver) -> tuple[dict[str, int], list[str]]:
    rewards = world_reward_sources()
    windows = {}
    unmapped = []
    for item, sources in rewards.items():
        dirs = []
        for s in sources:
            mapdir = s.split(":", 1)[0]
            dirs.append(mapdir)
        window, _ = resolver.best(dirs)
        if window is None:
            unmapped.append(item)
            continue
        windows[item] = window
    return windows, unmapped


# ---------------------------------------------------------------------------
# Evolutions
# ---------------------------------------------------------------------------

def split_top_level(text: str) -> list[str]:
    """Split a comma-joined sequence of balanced {...} groups into each
    group's inner text (without the outer braces)."""
    groups = []
    depth = 0
    start = None
    for i, ch in enumerate(text):
        if ch == "{":
            if depth == 0:
                start = i + 1
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0 and start is not None:
                groups.append(text[start:i])
                start = None
    return groups


def evolution_triples() -> dict[str, list[tuple[str, str, str, str]]]:
    """SPECIES_x -> [(method, param, target_species, rest_of_tuple), ...]"""
    text = preprocess_species_info()
    text = text[text.index("const struct SpeciesInfo gSpeciesInfo[]"):]
    marks = list(re.finditer(r"\[(SPECIES_\w+)\]\s*=\s*\{", text))
    result: dict[str, list[tuple[str, str, str, str]]] = defaultdict(list)
    for i, m in enumerate(marks):
        body = text[m.end():marks[i + 1].start() if i + 1 < len(marks) else len(text)]
        evo = re.search(r"\.evolutions\s*=\s*(.*?)(?:\n\s*\.\w+\s*=|\n\s*\},?\s*\Z)", body, re.S)
        if not evo:
            continue
        raw = evo[1].strip()
        if raw.startswith("EVOLUTION("):
            raw = raw[len("EVOLUTION("):]
        # After cpp macro expansion, EVOLUTION(...) becomes
        # "(const struct Evolution[]) { {EVO_x, ...}, {EVOLUTIONS_END}, }" --
        # one top-level {...} group wrapping every individual tuple.
        outer = split_top_level(raw)
        inner = split_top_level(outer[0]) if outer else split_top_level(raw)
        for group in inner:
            gm = re.match(r"\s*(EVO_\w+)\s*,\s*([^,]+?)\s*,\s*(SPECIES_\w+)\s*(.*)", group, re.S)
            if not gm:
                continue
            result[m[1]].append((gm[1], gm[2].strip(), gm[3], gm[4]))
    return result


def evolution_gate(method: str, param: str, rest: str, item_win: dict[str, int]) -> tuple[int | None, str]:
    """Returns (extra window beyond the source species' own window, method
    label), or (None, label) if this edge cannot be modeled as solo-reachable."""
    if method == "EVO_LEVEL":
        if param.isdigit() and int(param) > 0:
            return snap(int(param)), "level"
        return 0, ("friendship" if "IF_MIN_FRIENDSHIP" in rest else
                    "known-move" if "IF_KNOWS_MOVE" in rest else "level/condition")
    if method == "EVO_ITEM":
        win = item_win.get(param)
        if win is None:
            return None, f"item:{param}(unmapped)"
        return win, f"item:{param}"
    if method == "EVO_TRADE":
        held = re.search(r"IF_HOLD_ITEM,\s*(ITEM_\w+)", rest)
        if not held:
            return None, "trade(no held item, not modeled)"
        win = item_win.get(held[1])
        if win is None:
            return None, f"trade+item:{held[1]}(unmapped)"
        return win, f"trade+item:{held[1]}"
    return 0, f"other:{method}"


def expand_evolutions(roots: dict[str, tuple[int, str, str]], item_win: dict[str, int]) -> None:
    triples = evolution_triples()
    changed = True
    while changed:
        changed = False
        for species, (window, _, _) in list(roots.items()):
            for method, param, target, rest in triples.get(species, []):
                extra, label = evolution_gate(method, param, rest, item_win)
                if extra is None:
                    continue
                target_window = max(window, extra)
                prev = roots.get(target)
                if prev is None or target_window < prev[0]:
                    roots[target] = (target_window, "evolution", f"evolution:{label} from {species}")
                    changed = True


# ---------------------------------------------------------------------------
# Megas
# ---------------------------------------------------------------------------

def expand_megas(roots: dict[str, tuple[int, str, str]], stone_win: dict[str, int]) -> list[str]:
    unmapped = []
    for item in mega_stones():
        forms = mr.item_forms().get(item, [])
        stone_window = stone_win.get(item)
        for form_species in forms:
            base = mr.base_species(form_species)
            base_entry = roots.get(base)
            if base_entry is None or stone_window is None:
                unmapped.append(form_species)
                continue
            window = max(base_entry[0], stone_window, MEGA_RING_CAP)
            prev = roots.get(form_species)
            detail = f"mega:{item} stone-window={stone_window} base-window={base_entry[0]}"
            if prev is None or window < prev[0]:
                roots[form_species] = (window, "mega", detail)
    return unmapped


# ---------------------------------------------------------------------------
# Base stat totals, for the top-40 lists
# ---------------------------------------------------------------------------

def base_stat_totals() -> dict[str, int]:
    text = preprocess_species_info()
    text = text[text.index("const struct SpeciesInfo gSpeciesInfo[]"):]
    marks = list(re.finditer(r"\[(SPECIES_\w+)\]\s*=\s*\{", text))
    totals: dict[str, int] = {}
    fields = ["baseHP", "baseAttack", "baseDefense", "baseSpeed", "baseSpAttack", "baseSpDefense"]
    for i, m in enumerate(marks):
        body = text[m.end():marks[i + 1].start() if i + 1 < len(marks) else len(text)]
        vals = []
        for f in fields:
            fm = re.search(r"\." + f + r"\s*=\s*(\d+)", body)
            if fm:
                vals.append(int(fm[1]))
        if len(vals) == 6:
            totals[m[1]] = sum(vals)
    aliases = species_aliases()
    for alias in aliases:
        target = resolve_species(alias, aliases)
        if target in totals and alias not in totals:
            totals[alias] = totals[target]
    return totals


# ---------------------------------------------------------------------------
# Main build
# ---------------------------------------------------------------------------

def build_pool():
    resolver = MapResolver()
    catalog = econ.build_catalog()

    roots: dict[str, tuple[int, str, str]] = {}
    for src_roots in (
        wild_roots(resolver),
        gift_roots(resolver),
        trade_roots(resolver),
    ):
        for sp, entry in src_roots.items():
            prev = roots.get(sp)
            if prev is None or entry[0] < prev[0]:
                roots[sp] = entry

    sign_roots, other_signs = legendary_sign_roots(resolver)
    coin_roots, bp_roots = corner_roots(resolver, catalog)
    for src_roots in (sign_roots, coin_roots, bp_roots):
        for sp, entry in src_roots.items():
            prev = roots.get(sp)
            if prev is None or entry[0] < prev[0]:
                roots[sp] = entry

    item_win = item_windows(resolver, catalog)
    expand_evolutions(roots, item_win)

    stone_win, stone_unmapped = mega_stone_windows(resolver)
    mega_unmapped = expand_megas(roots, stone_win)

    totals = base_stat_totals()

    per_window: dict[int, list[dict]] = {c: [] for c in CAPS}
    for species, (window, kind, detail) in roots.items():
        form = "mega" if species.endswith("_MEGA") or "_MEGA_" in species else "base"
        for cap in CAPS:
            if window <= cap:
                per_window[cap].append(dict(
                    species=species, form=form, source_kind=kind,
                    source_detail=detail, gate=window,
                ))

    return dict(
        resolver=resolver, roots=roots, per_window=per_window, totals=totals,
        catalog=catalog, item_win=item_win, stone_win=stone_win,
        stone_unmapped=stone_unmapped, mega_unmapped=mega_unmapped,
        other_signs=other_signs,
    )


def cross_check_c24(per_window: dict[int, list[dict]]) -> list[str]:
    if not BENCHMARK.exists():
        return ["handoff/benchmarks/c15_available_pool.json not found"]
    bench = json.loads(BENCHMARK.read_text())
    bench_species = set()
    for row in bench.get("direct_wild_roots", []):
        bench_species.update(row.get("species", []))
    for key in ("gifts", "eggs", "trades", "legendary_signs", "megas"):
        for row in bench.get(key, []) if isinstance(bench.get(key), list) else []:
            if isinstance(row, dict) and "species" in row:
                s = row["species"]
                bench_species.update(s if isinstance(s, list) else [s])
    generated = {r["species"] for r in per_window.get(24, [])}
    only_bench = sorted(bench_species - generated)
    only_gen = sorted(generated - bench_species)
    lines = [
        f"benchmark direct-root species counted: {len(bench_species)}",
        f"generated cap24 species (incl. evolutions/wild/gift/etc): {len(generated)}",
        f"in benchmark but not generated ({len(only_bench)}): {', '.join(only_bench[:60])}"
        + (" ..." if len(only_bench) > 60 else ""),
        f"in generated but not benchmark ({len(only_gen)}): likely evolutions of direct "
        f"roots, or gift/trade/legendary/corner sources the hand-built benchmark's "
        f"'direct_wild_roots' section does not enumerate (that section documents wild "
        f"tables only, per its own name).",
    ]
    return lines


def write_outputs(result: dict) -> None:
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    for cap in CAPS:
        rows = sorted(result["per_window"][cap], key=lambda r: (r["species"], r["form"]))
        out = OUT_DIR / f"pool-{cap}.json"
        out.write_text(json.dumps(dict(cap=cap, count=len(rows), species=rows), indent=2) + "\n")

    lines = ["# Reference pool summary", "", f"Generated by scripts/reference_pool.py", ""]
    lines.append("## Per-window counts (species+form rows; cumulative -- includes everything from earlier windows)")
    lines.append("")
    lines.append("| cap | total | wild | gift | trade | legendary_sign | game_corner | circuit_bp | evolution | mega |")
    lines.append("|---|---|---|---|---|---|---|---|---|---|")
    for cap in CAPS:
        rows = result["per_window"][cap]
        by_kind = defaultdict(int)
        for r in rows:
            by_kind[r["source_kind"]] += 1
        lines.append("| " + " | ".join([str(cap), str(len(rows))] + [str(by_kind.get(k, 0)) for k in
            ["wild", "gift", "trade", "legendary_sign", "game_corner", "circuit_bp", "evolution", "mega"]]) + " |")

    lines.append("")
    lines.append("## Top 40 by base stat total, per window")
    for cap in CAPS:
        rows = result["per_window"][cap]
        totals = result["totals"]
        ranked = sorted(rows, key=lambda r: -(totals.get(r["species"], 0)))[:40]
        lines.append(f"\n### cap {cap}")
        for r in ranked:
            lines.append(f"- {r['species']} ({r['form']}) BST={totals.get(r['species'], '?')} "
                         f"gate={r['gate']} via {r['source_kind']}: {r['source_detail']}")

    lines.append("\n## Cap-24 cross-check against handoff/benchmarks/c15_available_pool.json")
    for line in cross_check_c24(result["per_window"]):
        lines.append(f"- {line}")

    lines.append("\n## Index limitations (do not read beyond what these encode)")
    lines.append("- map_cap_index (scripts/mega_register.py) is a graph-reachability index over "
                  "data/maps/*/map.json connections + warp_events (plus a documented gate table "
                  "of story/field-move gates and a few synthetic boat/Dive edges), floored by "
                  "each map's own authored trainer strict_cap where that is higher. A map with no "
                  "direct entry falls back to the cheapest same-prefix (\"CityName_*\") map's cap "
                  "('derived'); a map with neither is excluded ('unmapped'). Run "
                  "`python3 reference_pool.py --explain MAP_NAME` for the path and gates behind "
                  "any one map's window.")
    lines.append(f"- Unmapped map directories this run could not place at all: "
                  f"{len(result['resolver'].unmapped)}")
    for d in sorted(result["resolver"].unmapped):
        lines.append(f"  - {d}")
    lines.append(f"- Mega stones with no resolvable world-source window (excluded from mega "
                  f"expansion): {', '.join(sorted(result['stone_unmapped'])) or '(none)'}")
    lines.append(f"- Mega forms excluded because their base family or stone window could not be "
                  f"resolved: {len(result['mega_unmapped'])}")
    if result["mega_unmapped"]:
        lines.append("  - " + ", ".join(sorted(result["mega_unmapped"])))
    lines.append(f"- Legendary signs with no map (OTHER_SIGN: mastery/circuit/breeding/game-corner "
                 f"sources, not modeled): {', '.join(result['other_signs'])}")
    lines.append("- Cut/Rock Smash/Dive are modeled as map-graph reachability gates inside "
                 "mega_register.map_cap_index() (see its gate table); this script's own "
                 "water_mons/rock_smash_mons handling above is a separate, encounter-table-level "
                 "bump on top of that. Strength/Fly/Waterfall have no instance named by the guide "
                 "beyond what map_cap_index's gate table already encodes. Physical reachability "
                 "(walking path, geometry, story order beyond a flag/badge) is never claimed "
                 "beyond what map_cap_index's graph walk, its documented gate table, and this "
                 "script's own same-prefix fallback encode.")
    lines.append("- Bare EVO_TRADE (link trade with no held-item condition) is not modeled as "
                 "solo-reachable and is skipped.")

    (OUT_DIR / "summary.md").write_text("\n".join(lines) + "\n")


def main() -> None:
    import argparse

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--explain", metavar="MAP_NAME", default=None,
                         help="Print the path and gates that gave one map directory its "
                              "cap-window in mega_register.map_cap_index(), then exit without "
                              "regenerating the pools.")
    args = parser.parse_args()

    if args.explain:
        print(mr.explain_map_window(args.explain))
        return

    result = build_pool()
    write_outputs(result)
    print(f"Wrote {len(CAPS)} pool-<cap>.json files and summary.md to {OUT_DIR}")
    for cap in CAPS:
        print(f"  cap {cap}: {len(result['per_window'][cap])} species+form rows")


if __name__ == "__main__":
    main()
