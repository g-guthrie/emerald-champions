#!/usr/bin/env python3
"""Generate the complete Emerald Champions encounter and acquisition report."""

from __future__ import annotations

import argparse
import json
import re
import sqlite3
import subprocess
from collections import Counter, defaultdict, deque
from datetime import datetime, timezone
from pathlib import Path

import verdant_battle_set_presets as battle_sets


ROOT = Path(__file__).resolve().parents[1]
REPORT_MD = ROOT / "docs/emerald_champions_pokemon_availability_report.md"
REPORT_JSON = ROOT / "docs/emerald_champions_pokemon_availability_report.json"
ARTIFACT_JSON = ROOT / "docs/emerald_champions_pokemon_availability_artifact.json"

METHOD_SPECS = {
    "land_mons": [("Land", 0, [13, 13, 10, 10, 10, 10, 5, 5, 8, 8, 4, 4])],
    "water_mons": [("Surf", 0, [60, 30, 5, 5])],
    "rock_smash_mons": [("Rock Smash", 0, [60, 30, 5, 5])],
    "fishing_mons": [
        ("Old Rod", 0, [60, 40]),
        ("Good Rod", 2, [60, 20, 20]),
        ("Super Rod", 5, [40, 30, 15, 10, 5]),
    ],
    "honey_mons": [("Honey", 0, [50, 15, 15, 10, 5, 5])],
}

LEVEL_CAPS = [14, 20, 30, 40, 45, 55, 60, 70, 80, 101]
PHASE_NAMES = [
    "Opening — before the Stone Badge",
    "Stone Badge — Rustboro to Dewford",
    "Knuckle Badge — Dewford to Slateport",
    "Dynamo Badge — Mauville, ash country, and Mt. Chimney",
    "Heat Badge — Petalburg return and eastern routes",
    "Balance Badge — Fortree, Safari Zone, and Mt. Pyre",
    "Feather Badge — Lilycove, ocean routes, and Mossdeep",
    "Mind Badge — deep ocean, Sootopolis, and Cave of Origin",
    "Rain Badge — Sky Pillar, Victory Road, and the League approach",
    "Champion / postgame — open-world cleanup and Battle Frontier",
]


def read(path: str | Path) -> str:
    return (ROOT / path).read_text()


def clean_token(token: str, prefix: str) -> str:
    return token.removeprefix(prefix)


def humanize(value: str) -> str:
    value = value.removeprefix("MAP_").replace("POKEMON", "Pokémon")
    value = value.replace("_", " ")
    value = re.sub(r"(?<=[a-z0-9])(?=[A-Z])", " ", value)
    value = re.sub(r"\bROUTE\s*(\d+)\b", r"Route \1", value, flags=re.I)
    value = re.sub(r"\bRoute(\d+)\b", r"Route \1", value)
    value = re.sub(r"\bB(\d+)F\b", r"B\1F", value)
    value = re.sub(r"\b(\d+) F\b", r"\1F", value)
    value = re.sub(r"\bB(\d+) F\b", r"B\1F", value)
    value = re.sub(r"\b(\d+) R\b", r"\1R", value)
    return " ".join(word if word in {"1F", "2F", "3F", "4F", "5F", "6F", "7F", "8F", "9F"} else word.title() for word in value.split())


def species_display(species: str) -> str:
    name = humanize(clean_token(species, "SPECIES_"))
    replacements = {
        "Ho Oh": "Ho-Oh", "Porygon Z": "Porygon-Z", "Type Null": "Type: Null",
        "Nidoran F": "Nidoran♀", "Nidoran M": "Nidoran♂", "Mr Mime": "Mr. Mime",
        "Mime Jr": "Mime Jr.", "Farfetchd": "Farfetch'd", "Sirfetchd": "Sirfetch'd",
        "Jangmo O": "Jangmo-o", "Hakamo O": "Hakamo-o", "Kommo O": "Kommo-o",
    }
    return replacements.get(name, name)


def map_location_from_path(path: str) -> str:
    match = re.search(r"data/maps/([^/]+)/", path)
    return humanize(match.group(1)) if match else "Global event system"


def common_prefix_length(left: str, right: str) -> int:
    count = 0
    for a, b in zip(left, right):
        if a != b:
            break
        count += 1
    return count


def phase_context() -> tuple[dict[str, dict], list[dict]]:
    guide = json.loads(read("docs/verdant_battle_guide.json"))
    by_map: dict[str, dict] = {}
    for entry in guide["entries"]:
        source_map = entry.get("sourceMap")
        if not source_map or entry.get("badge") is None:
            continue
        key = re.sub(r"[^a-z0-9]", "", source_map.lower())
        candidate = {
            "order": entry.get("order") or 9999,
            "badge": int(entry["badge"]),
            "cap": int(entry["levelCap"]),
            "source_map": source_map,
        }
        if key not in by_map or candidate["order"] < by_map[key]["order"]:
            by_map[key] = candidate
    phases = [
        {"phase_order": index, "badge_count": index, "level_cap": cap, "name": PHASE_NAMES[index]}
        for index, cap in enumerate(LEVEL_CAPS)
    ]
    return by_map, phases


GUIDE_PHASES, PHASES = phase_context()


def manual_phase(map_token: str) -> int:
    token = map_token.removeprefix("MAP_")
    route = re.match(r"ROUTE(\d+)", token)
    if route:
        number = int(route.group(1))
        if number <= 104:
            return 0
        if number in {105, 106, 116}:
            return 1
        if 107 <= number <= 110:
            return 2
        if number in {111, 112, 113, 114, 117}:
            return 3
        if number in {115, 118, 119}:
            return 4
        if 120 <= number <= 123:
            return 5
        if 124 <= number <= 126:
            return 6
        if 127 <= number <= 128:
            return 7
        if number >= 129:
            return 8
    patterns = (
        (0, ("PETALBURG_WOODS", "RUSTBORO", "LITTLEROOT", "OLDALE")),
        (1, ("GRANITE_CAVE", "DEWFORD", "RUSTURF_TUNNEL")),
        (2, ("SLATEPORT",)),
        (3, ("MAUVILLE", "FIERY_PATH", "JAGGED_PASS", "MT_CHIMNEY", "METEOR_FALLS", "FALLARBOR", "LAVARIDGE")),
        (4, ("WEATHER_INSTITUTE",)),
        (5, ("SAFARI_ZONE", "MT_PYRE", "LILYCOVE")),
        (6, ("SHOAL_CAVE", "AQUA_HIDEOUT", "MOSSDEEP")),
        (7, ("SEAFLOOR_CAVERN", "SOOTOPOLIS", "CAVE_OF_ORIGIN", "MARINE_CAVE", "TERRA_CAVE")),
        (8, ("VICTORY_ROAD", "EVER_GRANDE", "SKY_PILLAR")),
        (9, ("BATTLE_FRONTIER", "ALTERING_CAVE", "ARTISAN_CAVE", "DESERT_UNDERPASS", "NAVEL_ROCK", "BIRTH_ISLAND", "FARAWAY_ISLAND")),
    )
    for phase, needles in patterns:
        if any(needle in token for needle in needles):
            return phase
    return 9


def phase_for_map(map_token: str) -> dict:
    normalized = re.sub(r"[^a-z0-9]", "", map_token.removeprefix("MAP_").lower())
    exact = GUIDE_PHASES.get(normalized)
    candidate = exact
    if candidate is None:
        ranked = sorted(
            ((common_prefix_length(normalized, key), value) for key, value in GUIDE_PHASES.items()),
            key=lambda row: (row[0], -row[1]["order"]),
            reverse=True,
        )
        if ranked and ranked[0][0] >= 8:
            candidate = ranked[0][1]
    phase_index = min(9, candidate["badge"]) if candidate else manual_phase(map_token)
    return PHASES[phase_index]


def aggregate_slots(mons: list[dict], start: int, weights: list[int]) -> list[dict]:
    if sum(weights) != 100:
        raise ValueError(f"method weights do not sum to 100: {weights}")
    rows: dict[str, dict] = {}
    for offset, weight in enumerate(weights):
        mon = mons[start + offset]
        species = mon["species"]
        if species == "SPECIES_NONE":
            continue
        row = rows.setdefault(species, {
            "species_id": species,
            "chance_percent": 0,
            "min_level": mon["min_level"],
            "max_level": mon["max_level"],
        })
        row["chance_percent"] += weight
        row["min_level"] = min(row["min_level"], mon["min_level"])
        row["max_level"] = max(row["max_level"], mon["max_level"])
    return sorted(rows.values(), key=lambda row: (-row["chance_percent"], row["species_id"]))


def collect_wild(dex: battle_sets.LocalDex) -> tuple[list[dict], dict[str, list[dict]], list[dict]]:
    payload = json.loads(read("src/data/wild_encounters.json"))
    rows: list[dict] = []
    sources: dict[str, list[dict]] = defaultdict(list)
    pools: list[dict] = []
    for group in payload["wild_encounter_groups"]:
        label = group["label"]
        catchable = label in {"gWildMonHeaders", "gBerryTreeWildMonHeaders"}
        facility_only = label in {"gBattlePyramidWildMonHeaders", "gBattlePikeWildMonHeaders"}
        for table_index, encounter in enumerate(group["encounters"], 1):
            map_token = encounter.get("map")
            if map_token:
                location = humanize(map_token)
                phase = phase_for_map(map_token)
            elif label == "gBerryTreeWildMonHeaders":
                location = f"Berry tree encounter table {table_index}: {humanize(encounter['base_label'])}"
                phase = PHASES[0]
            else:
                location = f"{humanize(label.removeprefix('g'))} table {table_index}"
                phase = PHASES[9]
            for field, specs in METHOD_SPECS.items():
                info = encounter.get(field)
                if not info:
                    continue
                for method_name, start, weights in specs:
                    if start + len(weights) > len(info["mons"]):
                        continue
                    aggregated = aggregate_slots(info["mons"], start, weights)
                    pool_total = sum(row["chance_percent"] for row in aggregated)
                    pools.append({
                        "group": label,
                        "location": location,
                        "method": method_name,
                        "total_percent": pool_total,
                    })
                    for row in aggregated:
                        species_name = species_display(row["species_id"])
                        output = {
                            "phase_order": phase["phase_order"],
                            "phase": phase["name"],
                            "level_cap": phase["level_cap"],
                            "group": label,
                            "location": location,
                            "map_id": map_token or encounter["base_label"],
                            "method": method_name,
                            "encounter_rate": info.get("encounter_rate"),
                            "species_id": row["species_id"],
                            "species": species_name,
                            "chance_percent": row["chance_percent"],
                            "min_level": row["min_level"],
                            "max_level": row["max_level"],
                            "catchable": catchable,
                            "facility_only": facility_only,
                        }
                        rows.append(output)
                        if catchable:
                            sources[row["species_id"]].append(output)
    feebas = {
        "phase_order": 4,
        "phase": PHASES[4]["name"],
        "level_cap": PHASES[4]["level_cap"],
        "group": "special_fishing",
        "location": "Route 119 under the bridge",
        "map_id": "MAP_ROUTE119",
        "method": "Any Rod under bridge",
        "encounter_rate": None,
        "species_id": "SPECIES_FEEBAS",
        "species": species_display("SPECIES_FEEBAS"),
        "chance_percent": 100,
        "min_level": 20,
        "max_level": 25,
        "catchable": True,
        "facility_only": False,
    }
    rows.append(feebas)
    sources["SPECIES_FEEBAS"].append(feebas)
    pools.append({"group": "special_fishing", "location": feebas["location"], "method": feebas["method"], "total_percent": 100})
    return rows, sources, pools


def add_source(rows: list[dict], seen: set[tuple], species: str, category: str, location: str, details: str, source_file: str) -> None:
    key = (species, category, location, details)
    if key in seen:
        return
    seen.add(key)
    rows.append({
        "species_id": species,
        "category": category,
        "location": location,
        "details": details,
        "source_file": source_file,
    })


def collect_nonrandom(dex: battle_sets.LocalDex) -> tuple[list[dict], dict[str, list[dict]]]:
    rows: list[dict] = []
    seen: set[tuple] = set()
    for path in sorted((ROOT / "data").rglob("*.inc")):
        source = path.read_text()
        relative = str(path.relative_to(ROOT))
        location = map_location_from_path(relative)
        for command, species in re.findall(r"\b(setwildbattle|givemon|giveegg)\s+(SPECIES_[A-Z0-9_]+)", source):
            category = {"setwildbattle": "Scripted/static encounter", "givemon": "NPC or story gift", "giveegg": "Gift Egg"}[command]
            add_source(rows, seen, species, category, location, f"Literal {command} acquisition", relative)

    ledger = json.loads(read("docs/emerald_champions_bespoke_encounter_ledger.json"))
    for entry in ledger.get("protected_static_acquisitions", []):
        base_species = next((species for species in entry.get("species", []) if "_MEGA" not in species and "_PRIMAL" not in species), None)
        if base_species:
            add_source(rows, seen, base_species, "Authored static acquisition", map_location_from_path(entry["source"]), entry["family"], entry["source"])
    guaranteed_gifts = {
        "castform": "SPECIES_CASTFORM",
        "cosmog": "SPECIES_COSMOG",
        "meltan": "SPECIES_MELTAN",
        "togepi": "SPECIES_TOGEPI",
        "mystery_ash_greninja": "SPECIES_GRENINJA_BATTLE_BOND",
        "mystery_magearna": "SPECIES_MAGEARNA",
        "mystery_meloetta": "SPECIES_MELOETTA",
    }
    for entry in ledger.get("protected_unique_gifts_and_restoration", []):
        species = guaranteed_gifts.get(entry.get("family"))
        if species:
            add_source(rows, seen, species, "Authored gift/restoration", map_location_from_path(entry["source"]), entry["family"], entry["source"])

    starter_source = read("src/starter_choose.c")
    for generation in ("Kanto", "Johto", "Hoenn", "Sinnoh", "Unova", "Kalos", "Alola"):
        match = re.search(rf"sStarterMon{generation}\[STARTER_MON_COUNT\]\s*=\s*\{{(.*?)\}};", starter_source, re.S)
        if match:
            for species in re.findall(r"SPECIES_[A-Z0-9_]+", match.group(1)):
                add_source(rows, seen, species, "Starter choice", "Littleroot opening", f"Selectable {generation} starter", "src/starter_choose.c")

    game_corner = read("data/maps/MauvilleCity_GameCorner/scripts.inc")
    for species in re.findall(r"setvar\s+VAR_TEMP_1,\s*(SPECIES_[A-Z0-9_]+)", game_corner):
        add_source(rows, seen, species, "Prize Pokémon", "Mauville Game Corner", "Coin-exchange Pokémon prize", "data/maps/MauvilleCity_GameCorner/scripts.inc")

    field = read("src/field_specials.c")
    fossil_body = field.split("void FossilToSpecies", 1)[1].split("void CheckLeadMon", 1)[0]
    for item, species in re.findall(r"case\s+(ITEM_[A-Z0-9_]+):\s*species\s*=\s*(SPECIES_[A-Z0-9_]+)", fossil_body):
        add_source(rows, seen, species, "Fossil restoration", "Rustboro Devon Corporation", f"Restore {item}", "src/field_specials.c")
    mystery_body = field.split("mysteryGiftData[][4]", 1)[1].split("};", 1)[0]
    for species, item, received, required in re.findall(r"\{(SPECIES_[A-Z0-9_]+),\s*(ITEM_[A-Z0-9_]+),\s*(FLAG_[A-Z0-9_]+),\s*(FLAG_[A-Z0-9_]+)\}", mystery_body):
        add_source(rows, seen, species, "Pokémon Center mystery gift", "Pokémon Centers", f"Requires {required}; arrives holding {item}", "src/field_specials.c")

    trades = read("src/data/trade.h").split("static const struct InGameTrade sIngameTrades[]", 1)[1].split("static const u16 sIngameTradeMail", 1)[0]
    for block in re.findall(r"\[INGAME_TRADE_[A-Z0-9_]+\]\s*=\s*\{(.*?)\n\s*\}", trades, re.S):
        species = re.search(r"\.species\s*=\s*(SPECIES_[A-Z0-9_]+)", block)
        requested = re.search(r"\.requestedSpecies\s*=\s*(SPECIES_[A-Z0-9_]+)", block)
        level = re.search(r"\.level\s*=\s*(\d+)", block)
        if species:
            details = f"Trade for {requested.group(1) if requested else 'specified Pokémon'}; received at level {level.group(1) if level else '?'}"
            add_source(rows, seen, species.group(1), "In-game trade", "NPC trade", details, "src/data/trade.h")

    for species in ("SPECIES_LATIAS", "SPECIES_LATIOS"):
        add_source(rows, seen, species, "Roaming encounter", "Hoenn overworld after the television choice", "Roamer has no fixed route percentage", "src/roamer.c")

    by_species: dict[str, list[dict]] = defaultdict(list)
    for row in rows:
        row["species"] = species_display(row["species_id"])
        by_species[row["species_id"]].append(row)
    return sorted(rows, key=lambda row: (row["category"], row["location"], row["species_id"])), by_species


def evolution_edges() -> tuple[dict[str, list[dict]], dict[str, list[dict]]]:
    source = read("src/data/pokemon/evolution.h") + "\n" + read("src/data/pokemon/verdant_gen9_evolutions.h")
    parents: dict[str, list[dict]] = defaultdict(list)
    children: dict[str, list[dict]] = defaultdict(list)
    for parent, body in battle_sets.designated_blocks(source).items():
        for method, param, target in re.findall(r"\{\s*(EVO_[A-Z0-9_]+)\s*,\s*([^,{}]+),\s*(SPECIES_[A-Z0-9_]+)\s*\}", body):
            if "MEGA" in method or "PRIMAL" in method:
                continue
            edge = {"parent": parent, "target": target, "method": method, "param": param.strip()}
            parents[target].append(edge)
            children[parent].append(edge)
    return parents, children


def evolution_description(edge: dict) -> str:
    method = edge["method"]
    param = edge["param"]
    if method.startswith("EVO_LEVEL"):
        return f"{method.removeprefix('EVO_').replace('_', ' ').title()} ({param})"
    if method.startswith("EVO_FRIENDSHIP"):
        return method.removeprefix("EVO_").replace("_", " ").title()
    if method.startswith("EVO_TRADE"):
        return f"{method.removeprefix('EVO_').replace('_', ' ').title()} ({param})"
    if method.startswith("EVO_ITEM"):
        return f"Use/hold {param}"
    if method == "EVO_MOVE":
        return f"Level while knowing {param}"
    if method == "EVO_MOVE_TYPE":
        return f"Level while knowing a {param.removeprefix('TYPE_').title()} move"
    return f"{method.removeprefix('EVO_').replace('_', ' ').title()} ({param})"


def base_form_candidate(species: str, all_species: set[str]) -> str | None:
    parts = species.removeprefix("SPECIES_").split("_")
    for end in range(len(parts) - 1, 0, -1):
        candidate = "SPECIES_" + "_".join(parts[:end])
        if candidate in all_species:
            return candidate
    return None


def species_availability(
    dex: battle_sets.LocalDex,
    wild_sources: dict[str, list[dict]],
    nonrandom_sources: dict[str, list[dict]],
) -> list[dict]:
    parents, children = evolution_edges()
    direct = set(wild_sources) | set(nonrandom_sources)
    predecessor: dict[str, dict] = {}
    reachable = set(direct)
    queue = deque(sorted(direct))
    while queue:
        parent = queue.popleft()
        for edge in children.get(parent, []):
            target = edge["target"]
            if target not in reachable:
                reachable.add(target)
                predecessor[target] = edge
                queue.append(target)

    family_graph: dict[str, set[str]] = defaultdict(set)
    for child, edges in parents.items():
        for edge in edges:
            family_graph[edge["parent"]].add(child)
            family_graph[child].add(edge["parent"])

    def obtainable_family_members(species: str) -> list[str]:
        component = {species}
        pending = [species]
        while pending:
            current = pending.pop()
            for neighbor in family_graph.get(current, set()):
                if neighbor not in component:
                    component.add(neighbor)
                    pending.append(neighbor)
        return sorted(component & reachable)

    all_species = {species for species_id, species in dex.canonical.items() if species_id and species_id < dex.num_species}
    rows: list[dict] = []
    for species_id, species in sorted(dex.canonical.items()):
        if not species_id or species_id >= dex.num_species or species == "SPECIES_EGG":
            continue
        name = species_display(species)
        wild = wild_sources.get(species, [])
        scripted = nonrandom_sources.get(species, [])
        exclusion = dex.excluded.get(species)
        source_refs: list[str] = []
        if wild:
            pools = sorted({f"{row['location']} / {row['method']}" for row in wild})
            source_refs.append(f"Random wild in {len(pools)} catchable method pool(s): " + "; ".join(pools))
        if scripted:
            source_refs.extend(f"{row['category']}: {row['location']} ({row['details']})" for row in scripted)

        if source_refs:
            acquisition_class = "Direct acquisition"
            path = " | ".join(source_refs)
        elif species in reachable and species in predecessor:
            chain = []
            current = species
            visited = set()
            while current in predecessor and current not in visited:
                visited.add(current)
                edge = predecessor[current]
                parent = edge["parent"]
                chain.append(f"{species_display(parent)} → {species_display(current)} via {evolution_description(edge)}")
                current = parent
            acquisition_class = "Evolution from obtainable Pokémon"
            path = " ; ".join(reversed(chain))
        elif exclusion in {"battle-transformation-endpoint", "automatic-or-battle-only-form", "unown-personality-graphic-slot"}:
            base = base_form_candidate(species, all_species)
            acquisition_class = "Form / battle transformation; not separately acquired"
            path = f"Derived from {species_display(base) if base else 'its base species'} through its form, personality, held-item, ability, or battle mechanic."
        else:
            family_sources = obtainable_family_members(species)
            base = base_form_candidate(species, all_species)
            if family_sources:
                source_species = family_sources[0]
                acquisition_class = "Breeding / obtainable evolution family"
                path = f"The permanent evolution family is obtainable through {species_display(source_species)}; breed or traverse the applicable branch to obtain this stage."
            elif base and base in reachable:
                acquisition_class = "Alternate form from obtainable base"
                path = f"Obtain {species_display(base)}, then use the applicable form-change mechanic."
            else:
                acquisition_class = "Unresolved by automated acquisition scan"
                path = "No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources."
        rows.append({
            "species_number": species_id,
            "species_id": species,
            "species": name,
            "acquisition_class": acquisition_class,
            "wild_pool_count": len({(row["map_id"], row["method"]) for row in wild}),
            "nonrandom_source_count": len(scripted),
            "acquisition_path": path,
            "runtime_status": exclusion or "party-selectable species/form",
        })
    return rows


def markdown_table(headers: list[str], rows: list[list[object]]) -> list[str]:
    lines = ["| " + " | ".join(headers) + " |", "|" + "|".join("---" for _ in headers) + "|"]
    for row in rows:
        values = [str(value).replace("|", "\\|").replace("\n", " ") for value in row]
        lines.append("| " + " | ".join(values) + " |")
    return lines


def build_report() -> tuple[dict, str, dict]:
    dex = battle_sets.LocalDex()
    wild_rows, wild_by_species, pools = collect_wild(dex)
    nonrandom_rows, nonrandom_by_species = collect_nonrandom(dex)
    species_rows = species_availability(dex, wild_by_species, nonrandom_by_species)
    source_commit = subprocess.check_output(["git", "log", "-1", "--format=%H", "--", "src", "data", "include"], cwd=ROOT, text=True).strip()
    source_clean = subprocess.run(["git", "diff", "--quiet", "--", "src", "data", "include"], cwd=ROOT).returncode == 0
    commit_time = subprocess.check_output(["git", "show", "-s", "--format=%cI", source_commit], cwd=ROOT, text=True).strip()
    generated_at = datetime.fromisoformat(commit_time).astimezone(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")
    sqlite = sqlite3.connect(":memory:")
    sqlite.execute("CREATE TABLE species_availability (acquisition_class TEXT NOT NULL)")
    sqlite.executemany("INSERT INTO species_availability VALUES (?)", [(row["acquisition_class"],) for row in species_rows])
    acquisition_summary = [
        {"acquisition_class": row[0], "species_form_count": row[1], "scope": "all runtime IDs"}
        for row in sqlite.execute(
            "SELECT acquisition_class, COUNT(*) AS species_form_count "
            "FROM species_availability GROUP BY acquisition_class ORDER BY species_form_count DESC"
        )
    ]
    sqlite.close()
    main_rows = [row for row in wild_rows if row["group"] == "gWildMonHeaders"]
    wild_payload = json.loads(read("src/data/wild_encounters.json"))
    main_configured_maps = len(next(group for group in wild_payload["wild_encounter_groups"] if group["label"] == "gWildMonHeaders")["encounters"])
    catchable_species = {row["species_id"] for row in wild_rows if row["catchable"]}
    main_maps = {row["map_id"] for row in main_rows}
    pool_errors = [pool for pool in pools if pool["total_percent"] != 100]
    loadouts = json.loads(read("docs/verdant_multi_battle_sets.json"))
    defaults = json.loads(read("docs/verdant_battle_set_presets.json"))
    battle_guide = json.loads(read("docs/verdant_battle_guide.json"))
    battle_meta = battle_guide["meta"]
    quality_meta = battle_meta["qualityAudit"]
    unresolved = [row for row in species_rows if row["acquisition_class"] == "Unresolved by automated acquisition scan"]

    payload = {
        "meta": {
            "title": "Emerald Champions — Complete Pokémon Encounter and Acquisition Report",
            "generated_at": generated_at,
            "source_commit": source_commit,
            "source_clean": source_clean,
            "scope": "All current random encounter tables plus source-resolved non-random acquisition paths and every runtime species/form ID.",
        },
        "game_context": {
            "identity": "Emerald Champions is an extremely hard, doubles-focused competitive Pokémon campaign built on Emerald/Inclement Emerald foundations.",
            "difficulty": "Challenge Mode, mandatory Set battle style, strict caps, most authored battles intended to require adaptation, and bosses designed as near-maximum difficulty puzzles.",
            "opening": "The player selects from 21 Generation 1–7 starters. The first Route 103 rival uses the same-region type-counter starter at level 15 against the opening cap of 14.",
            "battle_inventory": f"The current guide resolves {battle_meta['reachableBattleDefinitions']} reachable battle definitions: {quality_meta['doubles']} doubles and {quality_meta['singles']} intentional singles. The first {battle_meta['bespokeClosed']} canonical encounters have source-closed bespoke redesigns; the remaining campaign is still part of the continuing battle-authoring program.",
            "level_caps": LEVEL_CAPS,
            "moves": "No automatic level-up move learning. The Pokémon Center teacher offers every locally legal level, Egg, TM/HM, and tutor move.",
            "leveling": "The reusable Leveler raises the whole eligible party to the current cap; Rare Candy raises up to ten levels while stopping at the cap or next level evolution.",
            "loadouts": f"{defaults['supported_count']} authored defaults plus {loadouts['alternative_count']} handbook-derived alternatives ({loadouts['set_count']} total). Ordinary wild Pokémon uniformly roll their actual one-to-three tutor sets and fight with the rolled moves, nature, ability, and held item.",
            "items": f"{loadouts['free_item_count']} ordinary competitive held items and Berries are free and unlimited at the Pokémon Center vendor. Mega Stones, Primal Orbs, Plates, Drives, Memories, Ogerpon masks, Rusted items, and similar form/progression equipment remain protected.",
        },
        "level_cap_phases": PHASES,
        "wild_encounters": wild_rows,
        "nonrandom_acquisitions": nonrandom_rows,
        "species_availability": species_rows,
        "summary": {
            "main_overworld_maps": len(main_maps),
            "main_configured_maps": main_configured_maps,
            "all_random_table_rows": len(wild_rows),
            "catchable_random_species": len(catchable_species),
            "nonrandom_source_rows": len(nonrandom_rows),
            "runtime_species_form_rows": len(species_rows),
            "unresolved_rows": len(unresolved),
            "pool_total_errors": len(pool_errors),
            "loadout_set_count": loadouts["set_count"],
            "free_competitive_item_count": loadouts["free_item_count"],
        },
        "validation": {
            "all_method_pools_sum_to_100": not pool_errors,
            "pool_errors": pool_errors,
            "unresolved_species": [{"species_id": row["species_id"], "species": row["species"]} for row in unresolved],
            "probability_definition": "chance_percent is conditional on the named encounter method selecting a species. Encounter methods are independent and must never be summed together.",
        },
        "sources": [
            "src/data/wild_encounters.json",
            "src/data/wild_encounters.h",
            "src/wild_encounter.c",
            "docs/verdant_battle_guide.json",
            "docs/emerald_champions_bespoke_encounter_ledger.json",
            "src/data/pokemon/evolution.h",
            "src/data/pokemon/verdant_gen9_evolutions.h",
            "src/starter_choose.c",
            "src/field_specials.c",
            "src/data/trade.h",
            "docs/verdant_battle_set_presets.json",
            "docs/verdant_multi_battle_sets.json",
        ],
    }

    md: list[str] = [
        "# Emerald Champions — Complete Pokémon Encounter and Acquisition Report",
        "",
        "## What this document is",
        "",
        "This is a source-derived technical report describing the current Pokémon availability model in **Emerald Champions**. It is deliberately structured for ingestion by another language model: it first explains the game’s design goals and progression systems, then enumerates every random encounter table, then inventories non-random acquisitions, and finally gives one acquisition classification for every runtime species/form ID.",
        "",
        "The report distinguishes **within-method species probability** from an encounter method’s trigger rate. Grass, Surf, Rock Smash, each Rod, Honey, Berry-tree, roaming, and facility encounters are separate pools. Percentages are valid only inside their named method and are never meant to add together across methods.",
        "",
        "## Technical summary",
        "",
        f"- Source state: commit `{source_commit}`; clean working tree at generation: **{source_clean}**.",
        f"- Random encounter coverage: **{len(main_maps)}** populated main overworld maps from **{main_configured_maps}** configured entries, plus **{len(catchable_species)}** distinct species/forms in catchable random pools.",
        f"- Loadout system: **{loadouts['set_count']}** total competitive sets. Wild Pokémon roll the exact one/two/three-set tutor count at 100%, 50/50, or approximately one-third each.",
        f"- Item system: **{loadouts['free_item_count']}** ordinary competitive held items/Berries are free and unlimited; transformation and form-progression items remain protected.",
        f"- Species/form appendix: **{len(species_rows)}** runtime IDs classified; **{len(unresolved)}** remain unresolved by the automated source scan and are explicitly listed rather than guessed.",
        f"- Probability validation: **{'PASS' if not pool_errors else 'FAIL'}** — every emitted method pool sums independently to 100%.",
        f"- Battle context: **{battle_meta['reachableBattleDefinitions']}** reachable definitions, including **{quality_meta['doubles']} doubles** and **{quality_meta['singles']} singles**; **{battle_meta['bespokeClosed']}** canonical encounters are currently source-closed bespoke redesigns.",
        "",
        "## Acquisition classification summary",
        "",
        "The classification totals separate exact direct sources and permanent evolution/breeding paths from non-collectible form endpoints and unresolved audit rows. The HTML report renders these same counts as a chart; the Markdown table preserves them for language-model ingestion.",
        "",
    ]
    md.extend(markdown_table(["Acquisition classification", "Runtime species/form IDs"], [[row["acquisition_class"], row["species_form_count"]] for row in acquisition_summary]))
    md.extend([
        "",
        "## Game identity and why the encounter distribution exists",
        "",
        payload["game_context"]["identity"],
        "",
        payload["game_context"]["opening"],
        "",
        payload["game_context"]["battle_inventory"],
        "",
        "The game front-loads competitive agency and moves the challenge into battle solving. Players can catch unusual and powerful species early, teach every legal move at Pokémon Centers, switch legal abilities natively, obtain free ordinary competitive held items, and immediately raise a party to the strict cap. The campaign is mostly doubles, ordinary trainers are intended to be serious threats, and bosses are built as bespoke competitive puzzles rather than stat-only checks.",
        "",
        "Wild encounters follow the same philosophy. Eligible ordinary wild Pokémon do not appear with filler level-up moves: before battle, each one uniformly rolls one of its finalized competitive tutor sets and receives that set’s moves, nature, ability, and ordinary held item. The encountered Pokémon uses that loadout against the player and retains it if caught. Capture-hostile moves such as Explosion, Memento, Teleport, phazing, or Perish Song are not filtered.",
        "",
        "## Progression and strict level caps",
        "",
    ])
    md.extend(markdown_table(["Phase", "Badges", "Strict cap"], [[phase["name"], phase["badge_count"], phase["level_cap"]] for phase in PHASES]))
    md.extend([
        "",
        "The phase labels in this report use the earliest trainer-guide evidence for a map when available and a documented route/location heuristic otherwise. They are navigation context, not a replacement for story-event flags.",
        "",
        "## Random encounter methodology",
        "",
        "- `chance_percent` is the conditional chance of that species after the named method is active. Duplicate slots are aggregated.",
        "- `encounter_rate` is the table’s raw encounter-rate field; it is not multiplied into `chance_percent` because step checks, terrain, abilities, Repel, and method invocation differ.",
        "- Land weights are 13/13/10/10/10/10/5/5/8/8/4/4. Surf and Rock Smash are 60/30/5/5. Old Rod is 60/40; Good Rod 60/20/20; Super Rod 40/30/15/10/5; Honey 50/15/15/10/5/5.",
        "- Route 119 under-bridge Feebas is a separate 100% special fishing override at levels 20–25.",
        "- Battle Pyramid and Battle Pike tables are reported as facility-only random battles, not normal overworld acquisition promises.",
        "",
        "## Complete random encounter flow",
        "",
    ])
    for phase in PHASES:
        phase_rows = [row for row in wild_rows if row["phase_order"] == phase["phase_order"] and not row["facility_only"]]
        if not phase_rows:
            continue
        md.append(f"### {phase['name']} (cap {phase['level_cap']})")
        md.append("")
        md.extend(markdown_table(
            ["Location", "Method", "Raw rate", "Species", "Within-method %", "Levels"],
            [[row["location"], row["method"], row["encounter_rate"] if row["encounter_rate"] is not None else "special", row["species"], row["chance_percent"], f"{row['min_level']}–{row['max_level']}"] for row in phase_rows],
        ))
        md.append("")
    facility_rows = [row for row in wild_rows if row["facility_only"]]
    md.extend(["## Facility-only random battle tables", ""])
    md.extend(markdown_table(
        ["Facility table", "Method", "Species", "Source-table %", "Source levels"],
        [[row["location"], row["method"], row["species"], row["chance_percent"], f"{row['min_level']}–{row['max_level']}"] for row in facility_rows],
    ))
    md.extend([
        "",
        "Facility source levels may be transformed by the facility runtime. These rows document source-table composition, not a promise that the displayed level is the final battle level.",
        "",
        "## Non-random and special acquisition systems",
        "",
        "The following table covers source-resolved scripted/static battles, gifts, Eggs, starter choices, fossil restoration, mystery gifts, Game Corner prizes, NPC trades, and roamers. Dynamic event variables are expanded where their backing source table is explicit.",
        "",
    ])
    md.extend(markdown_table(
        ["Species", "Acquisition type", "Location/system", "Requirement/details", "Source"],
        [[row["species"], row["category"], row["location"], row["details"], row["source_file"]] for row in nonrandom_rows],
    ))
    md.extend([
        "",
        "## Complete species/form acquisition appendix",
        "",
        "This appendix covers every runtime species/form ID. `Direct acquisition` means the exact ID appears in a catchable random pool or source-resolved non-random event. `Evolution` means a permanent evolution chain from an obtainable parent. Battle-only and automatic forms are not separate collectibles. `Unresolved` is an audit flag, not a claim that the Pokémon is definitely unobtainable.",
        "",
    ])
    md.extend(markdown_table(
        ["#", "Species/form", "Runtime ID", "Classification", "Acquisition path", "Runtime status"],
        [[row["species_number"], row["species"], row["species_id"], row["acquisition_class"], row["acquisition_path"], row["runtime_status"]] for row in species_rows],
    ))
    md.extend([
        "",
        "## Limitations and robustness checks",
        "",
        "- Every method pool was recomputed from slot weights and validated to sum independently to 100%.",
        "- Effective encounter odds can change through Repel, terrain, lead abilities, outbreaks, scripted overrides, and facility scaling. The base conditional tables remain the canonical distribution reported here.",
        "- Static/gift extraction combines explicit script commands with the checked-in bespoke acquisition ledger, starter tables, fossils, mystery gifts, Game Corner prizes, trades, and roamers. Variable-driven or future event systems may still require manual annotation.",
        "- Alternate and battle-only forms are often not separate acquisitions. The appendix separates them from permanent obtainable species rather than pretending every graphics/form ID is independently catchable.",
        "- The existing ordinary held-item reward layer is intentionally pending redesign now that competitive items are free. Protected transformation/progression items remain meaningful.",
        "- The portable report intentionally uses exact tables rather than a summary chart: this artifact is optimized for complete lookup and language-model ingestion, and a chart would hide the map/method/species detail that is the point of the report.",
        "",
        "## Recommended next refinement",
        "",
        "Use this report to evaluate chapter-by-chapter roster quality, redundancy, missing competitive roles, overconcentration of premium species, unresolved acquisition flags, and whether direct access occurs at the intended cap. Any proposed change should update the source encounter table or acquisition script first and then regenerate this document.",
        "",
        "## Further questions for the next model",
        "",
        "1. Are all unresolved acquisition rows true gaps, or are some reachable through variable-driven scripts not captured by the scanner?",
        "2. Does each chapter expose enough distinct competitive roles—not merely enough species—to support the battle difficulty at that cap?",
        "3. Which now-redundant ordinary held-item rewards should become Pokémon, protected transformation items, services, invitations, or story access?",
        "4. Are any species technically available but placed so late that their intended competitive identity has little campaign value?",
        "",
    ])
    markdown = "\n".join(md)

    acquisition_sql = (
        "SELECT acquisition_class, COUNT(*) AS species_form_count "
        "FROM species_availability GROUP BY acquisition_class ORDER BY species_form_count DESC;"
    )
    source_list = [
        {"id": "availability_report", "label": "Generated availability dataset", "path": "docs/emerald_champions_pokemon_availability_report.json"},
        {"id": "wild_tables", "label": "ROM wild encounter tables", "path": "src/data/wild_encounters.json"},
        {"id": "acquisition_sources", "label": "ROM acquisition scripts and evolution tables", "path": "docs/emerald_champions_bespoke_encounter_ledger.json"},
        {"id": "availability_sql", "label": "Acquisition classification aggregation", "path": "docs/emerald_champions_pokemon_availability_report.json", "query": {"engine": "sqlite", "language": "sql", "description": "Counts every runtime species/form ID by its final acquisition classification.", "sql": acquisition_sql, "tables_used": ["species_availability"], "filters": ["All runtime species/form IDs except the Egg sentinel"], "metric_definitions": {"species_form_count": "COUNT(*) grouped by acquisition_class"}}},
    ]
    summary_row = payload["summary"]
    artifact = {
        "surface": "report",
        "manifest": {
            "version": 1,
            "surface": "report",
            "title": payload["meta"]["title"],
            "description": "Complete source-derived Emerald Champions wild encounter and Pokémon acquisition report.",
            "generatedAt": generated_at,
            "cards": [
                {"id": "maps", "description": "Main overworld maps with random tables.", "dataset": "summary", "sourceId": "availability_report", "metrics": [{"label": "Overworld maps", "field": "main_overworld_maps", "format": "number"}]},
                {"id": "wild_species", "description": "Distinct species/forms in catchable random pools.", "dataset": "summary", "sourceId": "availability_report", "metrics": [{"label": "Random-pool species", "field": "catchable_random_species", "format": "number"}]},
                {"id": "runtime_ids", "description": "All runtime species/form IDs classified.", "dataset": "summary", "sourceId": "availability_report", "metrics": [{"label": "Runtime IDs", "field": "runtime_species_form_rows", "format": "number"}]},
                {"id": "loadouts", "description": "Finalized competitive loadouts available to the tutor/wild system.", "dataset": "summary", "sourceId": "availability_report", "metrics": [{"label": "Competitive sets", "field": "loadout_set_count", "format": "number"}]},
            ],
            "charts": [{
                "id": "acquisition_class_chart",
                "title": "Runtime species/form IDs by acquisition classification",
                "subtitle": "Direct sources, permanent evolution chains, form endpoints, and unresolved audit rows",
                "type": "bar",
                "dataset": "acquisition_summary",
                "sourceId": "availability_report",
                "encodings": {
                    "x": {"field": "acquisition_class", "type": "nominal", "label": "Acquisition classification"},
                    "y": {"field": "species_form_count", "type": "quantitative", "label": "Species/form IDs", "format": "number"},
                },
                "yAxisTitle": "Species/form IDs",
                "valueFormat": "number",
                "layout": "full",
            }],
            "tables": [
                {"id": "wild_table", "title": "Complete random encounter table", "subtitle": "Each percentage is conditional on its named method; methods are independent", "dataset": "wild_encounters", "sourceId": "wild_tables", "defaultSort": {"field": "phase_order", "direction": "asc"}, "columns": [
                    {"field": "phase_order", "label": "Phase", "type": "number"}, {"field": "location", "label": "Location", "type": "text"}, {"field": "method", "label": "Method", "type": "text"}, {"field": "species", "label": "Species", "type": "text"}, {"field": "chance_percent", "label": "Within-method %", "type": "number"}, {"field": "min_level", "label": "Min Lv.", "type": "number"}, {"field": "max_level", "label": "Max Lv.", "type": "number"}, {"field": "encounter_rate", "label": "Raw rate", "type": "number"}, {"field": "group", "label": "Table group", "type": "text"},
                ]},
                {"id": "nonrandom_table", "title": "Non-random acquisition systems", "subtitle": "Source-resolved static encounters, gifts, Eggs, starters, fossils, prizes, trades, and roamers", "dataset": "nonrandom_acquisitions", "sourceId": "acquisition_sources", "defaultSort": {"field": "category", "direction": "asc"}, "columns": [
                    {"field": "species", "label": "Species", "type": "text"}, {"field": "category", "label": "Acquisition type", "type": "text"}, {"field": "location", "label": "Location/system", "type": "text"}, {"field": "details", "label": "Details", "type": "text"}, {"field": "source_file", "label": "Source", "type": "text"},
                ]},
                {"id": "species_table", "title": "Every runtime species/form acquisition classification", "subtitle": "Direct, evolution-derived, form endpoint, or unresolved; unresolved rows are audit flags", "dataset": "species_availability", "sourceId": "availability_report", "defaultSort": {"field": "species_number", "direction": "asc"}, "columns": [
                    {"field": "species_number", "label": "#", "type": "number"}, {"field": "species", "label": "Species/form", "type": "text"}, {"field": "species_id", "label": "Runtime ID", "type": "text"}, {"field": "acquisition_class", "label": "Classification", "type": "text"}, {"field": "acquisition_path", "label": "Acquisition path", "type": "text"}, {"field": "runtime_status", "label": "Runtime status", "type": "text"},
                ]},
            ],
            "sources": source_list,
            "blocks": [
                {"id": "title", "type": "markdown", "body": f"# {payload['meta']['title']}"},
                {"id": "technical_summary", "type": "markdown", "sourceId": "availability_report", "body": "## Technical summary\n\nEmerald Champions is an extremely hard, doubles-focused competitive campaign. The report enumerates every current random encounter pool and classifies every runtime Pokémon/form by how it is acquired. Percentages are conditional within one method only; grass, Surf, rods, Honey, and facilities are independent."},
                {"id": "summary_metrics", "type": "metric-strip", "cardIds": ["maps", "wild_species", "runtime_ids", "loadouts"]},
                {"id": "classification_intro", "type": "markdown", "body": "## Availability is broad, while forms and unresolved rows require careful interpretation\n\nThe chart separates exact direct acquisition from permanent evolution chains, non-collectible form endpoints, and unresolved audit rows. A form endpoint is not a missing Pokémon; an unresolved row is a prompt for manual source review."},
                {"id": "classification_chart", "type": "chart", "chartId": "acquisition_class_chart", "layout": "full"},
                {"id": "scope", "type": "markdown", "body": "## Scope, definitions, and game systems\n\nChallenge Mode uses strict caps of 14, 20, 30, 40, 45, 55, 60, 70, 80, and 101. Level-up move prompts are disabled; the Center teacher exposes every legal move. Wild Pokémon roll complete competitive loadouts, and ordinary held items are free while transformation/progression items remain protected."},
                {"id": "methodology", "type": "markdown", "body": "## Methodology\n\nSlot weights are aggregated by species independently for Land, Surf, Rock Smash, each Rod, Honey, Berry-tree, and facility pools. Direct acquisition scanning combines ROM scripts, the bespoke acquisition ledger, starter tables, fossils, mystery gifts, Game Corner prizes, trades, and roamers. Permanent evolution reachability is propagated from those direct sources."},
                {"id": "wild_intro", "type": "markdown", "body": "## Complete random encounter flow\n\nRead each row as: conditional on this method producing a Pokémon, this species owns the stated percentage. The raw encounter-rate field is included separately and should not be multiplied or summed across methods without modeling the actual trigger system."},
                {"id": "wild", "type": "table", "tableId": "wild_table", "layout": "full"},
                {"id": "nonrandom_intro", "type": "markdown", "body": "## The remaining direct acquisitions come from authored systems\n\nStatic encounters, gifts, Eggs, starters, fossils, mystery gifts, prizes, trades, and roamers cover Pokémon that are deliberately kept out of anonymous random pools or offered through another progression system."},
                {"id": "nonrandom", "type": "table", "tableId": "nonrandom_table", "layout": "full"},
                {"id": "species_intro", "type": "markdown", "body": "## Every runtime species/form is classified\n\nDirect rows identify an exact catch or gift source; evolution rows provide a permanent chain; battle/form rows are not separate collectibles; unresolved rows are explicit limitations rather than invented answers."},
                {"id": "species", "type": "table", "tableId": "species_table", "layout": "full"},
                {"id": "limitations", "type": "markdown", "body": "## Limitations, uncertainty, and robustness\n\nEvery emitted method pool sums to 100%. Effective odds still change through encounter checks, Repel, abilities, terrain, outbreaks, scripted overrides, and facility scaling. Dynamic variable-driven gifts or forms may escape static scanning; unresolved rows must be manually reviewed before being called unobtainable."},
                {"id": "next_steps", "type": "markdown", "body": "## Recommended next steps\n\nAudit unresolved rows, then evaluate each chapter for role diversity and timing rather than raw species count. Redesign redundant ordinary held-item rewards into Pokémon, protected items, services, invitations, or story access. Regenerate this report after source changes."},
                {"id": "questions", "type": "markdown", "body": "## Further questions\n\n1. Which unresolved rows are true gaps versus dynamic-script blind spots?\n2. Does every cap phase provide enough competitive roles to solve its battles?\n3. Which technically available Pokémon arrive too late to matter?\n4. What should replace ordinary held-item rewards now that loadout items are free?"},
            ],
        },
        "snapshot": {
            "version": 1,
            "generatedAt": generated_at,
            "status": "ready",
            "datasets": {
                "summary": [summary_row],
                "acquisition_summary": acquisition_summary,
                "wild_encounters": wild_rows,
                "nonrandom_acquisitions": nonrandom_rows,
                "species_availability": species_rows,
            },
        },
        "sources": source_list,
        "package_info": {"root": "emerald-champions", "manifestPath": "docs/emerald_champions_pokemon_availability_artifact.json", "snapshotPath": "docs/emerald_champions_pokemon_availability_artifact.json"},
    }
    # Portable HTML is intentionally markdown-first. The complete report is a
    # machine-ingestion and exact-lookup artifact; native chart/table widgets
    # would require a SQL source and would truncate or duplicate the audited
    # file-backed rows. Split peer sections into separate report blocks while
    # retaining the exact same Markdown content and source inventory.
    sections = re.split(r"(?=^## )", markdown, flags=re.M)
    blocks = []
    for index, section in enumerate(sections):
        body = section.strip()
        if not body:
            continue
        if index == 0:
            block_id = "title"
        else:
            heading = body.splitlines()[0].removeprefix("## ")
            block_id = re.sub(r"[^a-z0-9]+", "_", heading.lower()).strip("_")[:64] or f"section_{index}"
        blocks.append({"id": block_id, "type": "markdown", "body": body})
        if block_id == "acquisition_classification_summary":
            blocks.append({"id": "acquisition_classification_chart", "type": "chart", "chartId": "acquisition_class_chart", "layout": "full"})
    artifact = {
        "surface": "report",
        "manifest": {
            "version": 1,
            "surface": "report",
            "title": payload["meta"]["title"],
            "description": "Complete source-derived Emerald Champions wild encounter and Pokémon acquisition report, optimized for language-model ingestion.",
            "generatedAt": generated_at,
            "cards": [],
            "charts": [{"id": "acquisition_class_chart", "title": "Runtime species/form IDs by acquisition classification", "subtitle": "Direct sources, permanent family paths, form endpoints, and unresolved audit rows", "type": "bar", "dataset": "acquisition_summary", "sourceId": "availability_sql", "encodings": {"x": {"field": "acquisition_class", "type": "nominal", "label": "Acquisition classification"}, "y": {"field": "species_form_count", "type": "quantitative", "label": "Species/form IDs", "format": "number"}}, "yAxisTitle": "Species/form IDs", "valueFormat": "number", "layout": "full"}],
            "tables": [],
            "sources": source_list,
            "blocks": blocks,
        },
        "snapshot": {"version": 1, "generatedAt": generated_at, "status": "ready", "datasets": {"acquisition_summary": acquisition_summary}},
        "sources": source_list,
        "package_info": {"root": "emerald-champions", "manifestPath": "docs/emerald_champions_pokemon_availability_artifact.json", "snapshotPath": "docs/emerald_champions_pokemon_availability_artifact.json"},
    }
    return payload, markdown, artifact


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    payload, markdown, artifact = build_report()
    expected = {
        REPORT_JSON: json.dumps(payload, indent=2, ensure_ascii=False) + "\n",
        REPORT_MD: markdown,
        ARTIFACT_JSON: json.dumps(artifact, indent=2, ensure_ascii=False) + "\n",
    }
    if args.write:
        for path, content in expected.items():
            path.write_text(content)
    else:
        stale = [str(path.relative_to(ROOT)) for path, content in expected.items() if not path.exists() or path.read_text() != content]
        if stale:
            raise SystemExit(f"availability report artifacts are stale: {stale}")
    summary = payload["summary"]
    if summary["pool_total_errors"]:
        raise SystemExit("one or more method pools do not sum to 100")
    print(
        "PASS: availability report covers "
        f"{summary['main_overworld_maps']} main maps, "
        f"{summary['all_random_table_rows']} random encounter rows, "
        f"{summary['nonrandom_source_rows']} non-random source rows, and "
        f"{summary['runtime_species_form_rows']} runtime species/form IDs; "
        f"unresolved={summary['unresolved_rows']}"
    )


if __name__ == "__main__":
    main()
