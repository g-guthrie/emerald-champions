#!/usr/bin/env python3
"""Validate Emerald wild encounter tables against the rarity-ladder contract.

Checks src/data/wild_encounters.json (gWildMonHeaders entries):

1. Every land, water, Rock Smash, fishing and Honey table carries an explicit
   encounter_rates array with one weight per slot; each method totals 100 and
   each fishing rod group (old, good, super) totals 100 on its own.
2. Legendary-class slots are exactly 1%, Ultra Beast and Paradox slots are 2%
   or 3%, ordinary slots are at least 2%, a table holds at most two
   Legendary-class slots, and Rock Smash, fishing and Honey tables hold no
   Legendary, Ultra Beast or Paradox species.
3. Surf slots lie within levels 3-100 and every Surf and rod slot spans at most
   five levels.
4. Land slot levels satisfy 1 <= min <= max <= 100.
5. With --placement SPEC, each wild/quest placement appears exactly once on its
   map and method at its rate, and no restricted species appears elsewhere.
6. The shared Cut-tree habitat (sCutTreeHabitat in src/wild_encounter.c) totals
   100, follows the ordinary ladder, and is the only home of its species: none
   of them appears in any map table. It counts as a valid species home.

Species classes follow GetRestrictedPartyClass in src/pokemon.c: the flags of
the form table's base species (Ultra Beast, then Legendary-class, then
Paradox), read from the preprocessed species_info at run time.
"""
from pathlib import Path
import argparse
import json
import re
import shutil
import subprocess
from verify_trainer_ability_legality import (SPECIES_MARKER, configured_species_abilities,
    species_aliases, resolve_species)

ROOT = Path(__file__).resolve().parents[1]
ORDINARY_METHODS = {"land_mons", "water_mons", "rock_smash_mons", "fishing_mons", "honey_mons"}
# Tables that must never hold Legendary-class, Ultra Beast or Paradox species.
UNRESTRICTED_ONLY_METHODS = {"rock_smash_mons", "fishing_mons", "honey_mons"}
LEGENDARY, ULTRA_BEAST, PARADOX, ORDINARY = "legendary", "ub", "paradox", "none"
RESTRICTED_RATES = {LEGENDARY: {1}, ULTRA_BEAST: {2, 3}, PARADOX: {2, 3}}
MIN_ORDINARY_SLOT_PERCENT = 2
MAX_LEGENDARY_SLOTS_PER_TABLE = 2
WATER_LEVEL_BOUNDS = (3, 100)
MAX_WATER_ROD_LEVEL_SPAN = 5
PLACEMENT_METHODS = {"land": "land_mons", "water": "water_mons"}

# These families previously existed only in disabled DexNav data or Rock Smash
# tables on maps without smashable rocks. Keep an actual usable local source.
REQUIRED_OBTAINABLE_SPECIES = {
    "SPECIES_CHERUBI", "SPECIES_HATENNA", "SPECIES_SANDYGAST", "SPECIES_PINCURCHIN",
}

# Independent campaign contracts: mirroring a bad roster into both JSON files
# must not make early fishing lose its intended residents again.
OLD_ROD_HABITATS = {
    "MAP_ROUTE102": {"CORPHISH", "GOLDEEN"},
    "MAP_PETALBURG_CITY": {"MAGIKARP", "MARILL"},
    "MAP_ROUTE103": {"MAGIKARP", "REMORAID"},
    "MAP_ROUTE104": {"MAGIKARP", "SKRELP"},
    "MAP_DEWFORD_TOWN": {"MAGIKARP", "MANTYKE"},
    "MAP_ROUTE106": {"REMORAID", "CLAUNCHER"},
    "MAP_ROUTE109": {"MAGIKARP", "LUVDISC"},
    "MAP_SLATEPORT_CITY": {"HORSEA", "FRILLISH"},
    "MAP_ROUTE110": {"TADBULB", "CHINCHOU"},
    "MAP_ROUTE105": {"CLAUNCHER", "SKRELP"},
    "MAP_ROUTE107": {"FINNEON", "ARROKUDA"},
    "MAP_ROUTE108": {"REMORAID", "FINNEON"},
    "MAP_ROUTE111": {"BARBOACH", "GOLDEEN"},
    "MAP_ROUTE114": {"WISHIWASHI", "BARBOACH"},
    "MAP_ROUTE115": {"WISHIWASHI", "WAILMER"},
    "MAP_ROUTE117": {"WOOPER_PALDEA", "TYMPOLE"},
    "MAP_ROUTE118": {"MAGIKARP", "CARVANHA"},
    "MAP_ROUTE119": {"CARVANHA", "TYMPOLE"},
    "MAP_ROUTE120": {"BARBOACH", "MAGIKARP"},
    "MAP_ROUTE121": {"MAGIKARP", "WAILMER"},
    "MAP_ROUTE123": {"GOLDEEN", "CORPHISH"},
    "MAP_ROUTE122": {"WAILMER", "CHINCHOU"},
    "MAP_LILYCOVE_CITY": {"MAGIKARP", "FINNEON"},
    "MAP_ROUTE124": {"MAGIKARP", "FINNEON"},
    "MAP_ROUTE125": {"FINNEON", "ARROKUDA"},
    "MAP_ROUTE126": {"MAGIKARP", "FINNEON"},
    "MAP_ROUTE127": {"FINNEON", "FRILLISH"},
    "MAP_ROUTE128": {"MAGIKARP", "LUVDISC"},
    "MAP_MOSSDEEP_CITY": {"MAGIKARP", "FINNEON"},
    "MAP_EVER_GRANDE_CITY": {"LUVDISC", "HORSEA"},
    "MAP_ROUTE129": {"FINNEON", "CHINCHOU"},
    "MAP_ROUTE130": {"FINNEON", "REMORAID"},
    "MAP_ROUTE131": {"MAGIKARP", "RELICANTH"},
    "MAP_PACIFIDLOG_TOWN": {"MAGIKARP", "SKRELP"},
    "MAP_ROUTE132": {"ARROKUDA", "TENTACOOL"},
    "MAP_ROUTE133": {"SKRELP", "TENTACOOL"},
    "MAP_ROUTE134": {"MAGIKARP", "RELICANTH"},
    "MAP_SOOTOPOLIS_CITY": {"MAGIKARP", "TENTACOOL"},
    "MAP_SEAFLOOR_CAVERN_ENTRANCE": {"MAGIKARP", "CHINCHOU"},
    "MAP_SEAFLOOR_CAVERN_ROOM6": {"MAGIKARP", "QWILFISH"},
    "MAP_SEAFLOOR_CAVERN_ROOM7": {"MAGIKARP", "RELICANTH"},
    "MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM": {"MAGIKARP", "SHELLDER"},
    "MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM": {"MAGIKARP", "CLAMPERL"},
    "MAP_METEOR_FALLS_1F_1R": {"BARBOACH", "DRATINI"},
    "MAP_METEOR_FALLS_1F_2R": {"BARBOACH", "DRATINI"},
    "MAP_METEOR_FALLS_B1F_1R": {"BARBOACH", "DRATINI"},
    "MAP_METEOR_FALLS_B1F_2R": {"BARBOACH", "DRATINI"},
    "MAP_PETALBURG_WOODS_3": {"POLIWAG", "SLOWPOKE"},
    "MAP_SAFARI_ZONE_NORTHWEST": {"GRIMER_ALOLA", "GRIMER"},
    "MAP_SAFARI_ZONE_SOUTHEAST": {"REMORAID", "DRATINI"},
    "MAP_SAFARI_ZONE_SOUTHWEST": {"MAGIKARP", "GOLDEEN"},
    "MAP_SANDSTREWN_RUINS": {"RELICANTH", "BARBOACH"},
    "MAP_SCORCHED_SLAB_B1F": {"GOLDEEN", "BARBOACH"},
    "MAP_SEASPRAY_CAVE": {"WISHIWASHI", "KRABBY"},
    "MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS": {"FRILLISH", "SKRELP"},
    "MAP_ABANDONED_SHIP_ROOMS_B1F": {"FRILLISH", "SKRELP"},
    "MAP_ALTERING_CAVE_B1F": {"MAGIKARP", "BARBOACH"},
    "MAP_VICTORY_ROAD_B2F": {"MAGIKARP", "BARBOACH"},
}

SURF_HABITATS = {
    "MAP_ROUTE103": {"WIGLETT"}, "MAP_ROUTE104": {"WIGLETT"},
    "MAP_ROUTE106": {"WIGLETT"}, "MAP_ROUTE109": {"WIGLETT"},
    "MAP_ROUTE105": {"CLAMPERL", "MANTINE"},
    "MAP_ROUTE107": {"ARROKUDA", "MANTYKE"},
    "MAP_ROUTE108": {"LAPRAS", "DHELMISE"},
    "MAP_ROUTE111": {"WOOPER"}, "MAP_ROUTE114": {"BUIZEL", "QUAGSIRE"},
    "MAP_ROUTE115": {"BARBARACLE"}, "MAP_ROUTE117": {"LOTAD", "CHEWTLE"},
    "MAP_ROUTE118": {"TENTACOOL", "TAUROS_PALDEA_AQUA"},
    "MAP_ROUTE119": {"GASTRODON_EAST", "POLIWHIRL"},
    "MAP_ROUTE120": {"STUNFISK"}, "MAP_ROUTE121": {"FRILLISH", "ALOMOMOLA"},
    "MAP_ROUTE123": {"BUIZEL", "SWANNA"}, "MAP_ROUTE122": {"FRILLISH"},
    "MAP_LILYCOVE_CITY": {"CRAMORANT", "BRUXISH"},
    "MAP_ROUTE124": {"CLAMPERL", "FINIZEN"},
    "MAP_ROUTE125": {"SPHEAL", "DEWGONG"},
    "MAP_ROUTE126": {"ALOMOMOLA", "MANTINE"},
    "MAP_ROUTE127": {"MAREANIE", "PYUKUMUKU"},
    "MAP_ROUTE128": {"WAILMER", "FINIZEN"},
    "MAP_MOSSDEEP_CITY": {"STARYU", "FINIZEN"},
    "MAP_EVER_GRANDE_CITY": {"TENTACRUEL", "LAPRAS"},
    "MAP_ROUTE129": {"WAILORD", "LAPRAS"},
    "MAP_ROUTE130": {"FRILLISH", "WISHIWASHI"},
    "MAP_ROUTE131": {"CLAMPERL", "RELICANTH"},
    "MAP_PACIFIDLOG_TOWN": {"CRAMORANT", "FINIZEN"},
    "MAP_ROUTE132": {"BARRASKEWDA", "FINIZEN"},
    "MAP_ROUTE133": {"DRAGALGE", "MANTINE"},
    "MAP_ROUTE134": {"SEEL", "GOREBYSS"},
    "MAP_SOOTOPOLIS_CITY": {"MAGIKARP", "GYARADOS"},
    "MAP_SEAFLOOR_CAVERN_ENTRANCE": {"CLAMPERL", "LANTURN"},
    "MAP_SEAFLOOR_CAVERN_ROOM6": {"FRILLISH", "BARRASKEWDA"},
    "MAP_SEAFLOOR_CAVERN_ROOM7": {"HUNTAIL", "GOREBYSS"},
    "MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM": {"SPHEAL", "SEALEO", "AVALUGG"},
    "MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM": {"WALREIN", "LAPRAS"},
    "MAP_METEOR_FALLS_1F_1R": {"DRATINI", "BUIZEL"},
    "MAP_METEOR_FALLS_1F_2R": {"DRATINI", "LUNATONE"},
    "MAP_METEOR_FALLS_B1F_1R": {"DRAGONAIR", "SOLROCK"},
    "MAP_METEOR_FALLS_B1F_2R": {"SLOWPOKE", "QUAGSIRE"},
    "MAP_PETALBURG_WOODS_3": {"POLIWAG", "SLOWPOKE"},
    "MAP_SAFARI_ZONE_NORTHWEST": {"GRIMER_ALOLA", "MUK_ALOLA"},
    "MAP_SAFARI_ZONE_SOUTHEAST": {"WOOPER", "QUAGSIRE"},
    "MAP_SAFARI_ZONE_SOUTHWEST": {"STUNFISK", "SLIGGOO"},
    "MAP_SANDSTREWN_RUINS": {"RELICANTH"},
    "MAP_SCORCHED_SLAB_B1F": {"GOLBAT", "CROBAT"},
    "MAP_SEASPRAY_CAVE": {"TYNAMO", "CLAMPERL"},
    "MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS": {"FRILLISH", "DHELMISE"},
    "MAP_ABANDONED_SHIP_ROOMS_B1F": {"GOLISOPOD", "BARBARACLE"},
    "MAP_ALTERING_CAVE_B1F": {"BASCULEGION", "MALAMAR"},
    "MAP_VICTORY_ROAD_B2F": {"DEWGONG", "LAPRAS"},
}

CUT_TREE_SOURCE = ROOT / 'src/wild_encounter.c'
CUT_TREE_TABLE = re.compile(r"sCutTreeHabitat\[\]\s*=\s*\{(.*?)\};", re.S)
CUT_TREE_SLOT = re.compile(r"\{\s*(SPECIES_[A-Z0-9_]+)\s*,\s*(\d+)\s*\}")


def cut_tree_habitat():
    """Return [(species, odds)] from the native Cut-tree table."""
    match = CUT_TREE_TABLE.search(CUT_TREE_SOURCE.read_text())
    if match is None:
        raise SystemExit(f"{CUT_TREE_SOURCE}: sCutTreeHabitat table not found")
    return [(species, int(odds)) for species, odds in CUT_TREE_SLOT.findall(match.group(1))]


FLAG_FIELDS = ("isUltraBeast", "isRestrictedLegendary", "isSubLegendary", "isMythical", "isParadox")
FORM_TABLE = re.compile(r"static const u16 (s\w+FormSpeciesIdTable)\[\]\s*=\s*\{(.*?)\};", re.S)


def species_classes():
    """Return species -> restricted class, mirroring GetRestrictedPartyClass."""
    compiler = shutil.which("cc") or shutil.which("clang") or shutil.which("gcc")
    if compiler is None:
        raise SystemExit("a host C preprocessor (cc, clang, or gcc) is required")
    probe = ('#include "config/general.h"\n#include "constants/global.h"\n'
             '#include "constants/abilities.h"\n#include "constants/species.h"\n'
             '#include "data/pokemon/form_species_tables.h"\n'
             '#include "data/pokemon/species_info.h"\n')
    result = subprocess.run(
        (compiler, "-E", "-P", "-x", "c", "-DTRUE=1", "-DFALSE=0",
         f"-I{ROOT / 'include'}", f"-I{ROOT / 'src'}", f"-I{ROOT}", "-"),
        input=probe, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
    if result.returncode != 0:
        raise SystemExit("failed to preprocess species data:\n" + result.stderr)
    text = result.stdout
    forms = {name: re.findall(r"SPECIES_[A-Z0-9_]+", body) for name, body in FORM_TABLE.findall(text)}
    start = text.find("const struct SpeciesInfo gSpeciesInfo[]")
    if start < 0:
        raise SystemExit("preprocessed source does not define gSpeciesInfo")
    text = text[start:]
    markers = list(SPECIES_MARKER.finditer(text))
    flags, base_of = {}, {}
    for index, marker in enumerate(markers):
        block = text[marker.start():markers[index + 1].start() if index + 1 < len(markers) else len(text)]
        name = marker.group(1)
        own = set()
        for field in FLAG_FIELDS:
            match = re.search(r"\." + field + r"\s*=\s*([^,\n]+),", block)
            if match and match.group(1).strip() not in {"0", "FALSE"}:
                own.add(field)
        flags[name] = own  # later designated initialisers override earlier ones
        table = re.search(r"\.formSpeciesIdTable\s*=\s*(s\w+)", block)
        base_of[name] = forms.get(table.group(1), [name])[0] if table else name
    classes = {}
    for name in flags:
        base = flags.get(base_of[name], flags[name])
        if "isUltraBeast" in base:
            classes[name] = ULTRA_BEAST
        elif base & {"isRestrictedLegendary", "isSubLegendary", "isMythical"}:
            classes[name] = LEGENDARY
        elif "isParadox" in base:
            classes[name] = PARADOX
        else:
            classes[name] = ORDINARY
    return classes


def check_campaign_habitats(entries, fishing):
    errors = []
    opening = entries.get("MAP_ROUTE101", {}).get("land_mons", {})
    if not opening.get("encounter_rate") or any(
            mon["species"] in {"SPECIES_MIENFOO", "SPECIES_BONSLY"}
            for mon in opening.get("mons", [])):
        errors.append("MAP_ROUTE101/land_mons: Mienfoo and Bonsly belong in later habitats")

    # Route 130 land exists only when Mirage Island appears. Its Wynaut-only
    # roster also agrees with the Pokédex's special handling of that habitat.
    mirage = entries.get("MAP_ROUTE130", {}).get("land_mons", {})
    if not mirage.get("encounter_rate") or not mirage.get("mons") or any(
            mon["species"] != "SPECIES_WYNAUT" for mon in mirage["mons"]):
        errors.append("MAP_ROUTE130/land_mons: Mirage Island must remain Wynaut-only")

    for map_id, required in OLD_ROD_HABITATS.items():
        table = entries.get(map_id, {}).get("fishing_mons", {})
        mons = table.get("mons", [])
        actual = {mons[i]["species"].removeprefix("SPECIES_")
                  for i in fishing["groups"]["old_rod"] if i < len(mons)}
        if actual != required or not table.get("encounter_rate"):
            errors.append(f"{map_id}/old_rod: fishing habitat must retain {sorted(required)}")
    for map_id, required in SURF_HABITATS.items():
        table = entries.get(map_id, {}).get("water_mons", {})
        actual = {m["species"].removeprefix("SPECIES_") for m in table.get("mons", [])}
        if not table.get("encounter_rate") or not required <= actual:
            errors.append(f"{map_id}/surf: signature residents missing: {sorted(required - actual)}")
    return errors


def check_placements(path, emerald, classes, aliases):
    """Cross-check wild/quest placements against the tables (rule 5)."""
    spec = json.loads(Path(path).read_text())
    placements = [p for p in spec["placements"] if p.get("kind") in {"wild", "quest"}]
    errors, placed = [], {}
    for p in placements:
        method = PLACEMENT_METHODS.get(p.get("method"))
        species = resolve_species("SPECIES_" + p["species"], aliases)
        if method is None:
            errors.append(f"placement {p['species']}: unknown method {p.get('method')}")
            continue
        if species not in classes:
            errors.append(f"placement {p['species']}: unknown species")
            continue
        if p.get("class") and p["class"] != classes[species]:
            errors.append(f"placement {p['species']}: spec class {p['class']} but species_info says {classes[species]}")
        key = (p["map"], method, species)
        if key in placed:
            errors.append(f"placement {p['species']}: placed twice on {p['map']}/{method}")
        placed[key] = p
    slots = {}
    for entry in emerald:
        for method, table in entry.items():
            if not isinstance(table, dict) or "mons" not in table:
                continue
            rates = table.get("encounter_rates") or []
            for slot, mon in enumerate(table["mons"]):
                species = resolve_species(mon["species"], aliases)
                rate = rates[slot] if slot < len(rates) else None
                key = (entry["map"], method, species)
                if key in placed:
                    slots.setdefault(key, []).append(rate)
                elif classes.get(species, ORDINARY) != ORDINARY:
                    errors.append(f"{entry['map']}/{method}: {classes[species]} {species} "
                                  f"(slot {slot}, {rate}%) is not placed here")
    for key, p in placed.items():
        found = slots.get(key, [])
        tag = f"placement {p['species']} on {key[0]}/{key[1]}"
        if len(found) != 1:
            errors.append(f"{tag}: found {len(found)} slots, expected exactly 1")
        elif found[0] != p["rate"]:
            errors.append(f"{tag}: slot rate {found[0]}% but spec rate {p['rate']}%")
    return errors, len(placed)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--source-report', type=Path, help='Write potential Hoenn table sources, not a reachability certificate')
    parser.add_argument('--placement', type=Path, help='Cross-check wild/quest placements from this spec JSON')
    args = parser.parse_args()
    sources = {}
    payload = json.loads((ROOT / 'src/data/wild_encounters.json').read_text())
    group = next(g for g in payload['wild_encounter_groups'] if g['label'] == 'gWildMonHeaders')
    emerald = group['encounters']
    groups = json.loads((ROOT / 'data/maps/map_groups.json').read_text())
    map_rows = {}
    for name in (n for g in groups['group_order'] for n in groups[g]):
        row = json.loads((ROOT / 'data/maps' / name / 'map.json').read_text())
        row['_dir'] = name
        map_rows[row['id']] = row
    fields = {f['type']: f for f in group['fields']}
    dexnav_config = (ROOT / 'include/config/dexnav.h').read_text()
    dexnav_enabled = re.search(r'#define DEXNAV_ENABLED\s+(TRUE|FALSE|0|1)', dexnav_config).group(1) in {'TRUE', '1'}
    available_methods = ORDINARY_METHODS | ({'hidden_mons'} if dexnav_enabled else set())
    configured = configured_species_abilities()
    classes = species_classes()
    aliases = species_aliases()

    def class_of(species):
        return classes.get(resolve_species(species, aliases), ORDINARY)

    errors = check_campaign_habitats({e['map']: e for e in emerald}, fields['fishing_mons'])
    warnings, checked = [], 0
    wild_sources = set()
    for entry in emerald:
        map_id = entry['map']
        if 'hidden_mons' in entry and not dexnav_enabled:
            errors.append(f"{map_id}/hidden_mons: DexNav is disabled; this is not an obtainable source")
        if 'rock_smash_mons' in entry:
            # A breakable rock, or any local object whose script rolls the
            # Rock Smash table itself (Route 109's sand mounds).
            objects = map_rows.get(map_id, {}).get('object_events', [])
            scripts_path = ROOT / 'data' / 'maps' / map_rows.get(map_id, {}).get('_dir', '') / 'scripts.inc'
            local_scripts = scripts_path.read_text() if scripts_path.exists() else ''
            def script_body(label):
                body = local_scripts.split(label + '::', 1)
                return body[1].split('\n\n', 1)[0] if len(body) == 2 else ''
            def rolls_rock_table(obj):
                # The object's own script, or a script it jumps to in one
                # hop (Route 109's sand mounds share one dig script).
                script = obj.get('script', '')
                if script == 'EventScript_RockSmash':
                    return True
                body = script_body(script)
                if 'RockSmashWildEncounter' in body:
                    return True
                for target in re.findall(r'\b(?:goto|call)\s+(\w+)', body):
                    if 'RockSmashWildEncounter' in script_body(target):
                        return True
                return False
            if not any(rolls_rock_table(obj) for obj in objects):
                errors.append(f"{map_id}/rock_smash_mons: map has no smashable rock")
        for table in entry.values():
            if isinstance(table, dict) and 'mons' in table:
                wild_sources.update(resolve_species(mon['species'], aliases) for mon in table['mons'])
        if any(mon['species'] == 'SPECIES_MILOTIC'
               for table in entry.values() if isinstance(table, dict) and 'mons' in table
               for mon in table['mons']):
            errors.append(f"{map_id}: Milotic must be obtained through Feebas evolution")
        if 'honey_mons' in entry and 'land_mons' in entry:
            land_min = min(mon['min_level'] for mon in entry['land_mons']['mons']
                           if class_of(mon['species']) == ORDINARY)
            honey_min = min(mon['min_level'] for mon in entry['honey_mons']['mons'])
            if honey_min < land_min:
                warnings.append(f"{map_id}/honey_mons: minimum level {honey_min} is below local land minimum {land_min}")
        for name, field in fields.items():
            if name not in entry:
                continue
            table = entry[name]
            mons = table.get('mons') or []
            tag = f"{map_id}[{entry['base_label']}]/{name}"
            encounter_rate = table.get('encounter_rate')
            if type(encounter_rate) is not int or not 0 <= encounter_rate <= 255:
                errors.append(f'{tag}: encounter rate must fit the native unsigned byte')
            if name == 'hidden_mons' and encounter_rate != 0:
                errors.append(f'{tag}: DexNav land-hidden tables must use encounter_rate 0')
            # Rule 1: explicit per-table weights of the right length.
            rates = table.get('encounter_rates')
            if name in ORDINARY_METHODS and rates is None:
                errors.append(f'{tag}: missing explicit encounter_rates')
                continue
            rates = field['encounter_rates'] if rates is None else rates
            if not mons or len(mons) != len(field['encounter_rates']) or len(rates) != len(mons):
                errors.append(f'{tag}: {len(mons)} slots and {len(rates)} weights; expected {len(field["encounter_rates"])}')
                continue
            if any(type(r) is not int or not 0 <= r <= 100 for r in rates):
                errors.append(f'{tag}: invalid probability weights {rates}')
                continue
            valid_mons = True
            for slot, mon in enumerate(mons):
                species = mon.get('species')
                if (not isinstance(species, str) or species in {'SPECIES_NONE', 'SPECIES_EGG'}
                        or not configured.get(resolve_species(species, aliases))):
                    errors.append(f'{tag}: invalid configured encounter species {species}')
                    valid_mons = False
                elif name in available_methods and rates[slot] > 0:
                    sources.setdefault(resolve_species(species, aliases), set()).add(f"{map_id}/{name}")
                lo, hi = mon.get('min_level'), mon.get('max_level')
                # Rule 4 (and the native byte range for every method).
                if type(lo) is not int or type(hi) is not int or not 1 <= lo <= hi <= 100:
                    errors.append(f'{tag} slot {slot}: invalid level range {lo}-{hi} for {species}')
                    valid_mons = False
                    continue
                # Rule 3: Surf and rod level templates.
                if name == 'water_mons' and not WATER_LEVEL_BOUNDS[0] <= lo <= hi <= WATER_LEVEL_BOUNDS[1]:
                    errors.append(f'{tag} slot {slot}: {species} levels {lo}-{hi} leave {WATER_LEVEL_BOUNDS[0]}-{WATER_LEVEL_BOUNDS[1]}')
                if name in {'water_mons', 'fishing_mons'} and hi - lo > MAX_WATER_ROD_LEVEL_SPAN:
                    errors.append(f'{tag} slot {slot}: {species} levels {lo}-{hi} span more than {MAX_WATER_ROD_LEVEL_SPAN}')
            if not valid_mons:
                continue
            method_groups = field.get('groups', {name: list(range(len(mons)))})
            for method, indices in method_groups.items():
                if not indices or len(set(indices)) != len(indices) or any(type(i) is not int or not 0 <= i < len(mons) for i in indices):
                    errors.append(f'{tag}/{method}: invalid slot indices')
                    continue
                total = sum(rates[i] for i in indices)
                if total != 100:
                    errors.append(f'{tag}/{method}: weights total {total}, expected 100')
            # Rule 2: the rarity ladder, per slot.
            legendary_slots = 0
            for slot, mon in enumerate(mons):
                species, rate = mon['species'], rates[slot]
                kind = class_of(species)
                if species == 'SPECIES_FEEBAS':
                    errors.append(f'{tag}: Feebas is exclusive to the seeded Route119 fishing spots')
                if kind == ORDINARY:
                    if rate < MIN_ORDINARY_SLOT_PERCENT:
                        errors.append(f'{tag} slot {slot}: ordinary {species} has {rate}%, below {MIN_ORDINARY_SLOT_PERCENT}%')
                    continue
                if name in UNRESTRICTED_ONLY_METHODS:
                    errors.append(f'{tag} slot {slot}: {kind} {species} is not allowed in {name}')
                if rate not in RESTRICTED_RATES[kind]:
                    errors.append(f'{tag} slot {slot}: {kind} {species} has {rate}%, expected {sorted(RESTRICTED_RATES[kind])}')
                legendary_slots += kind == LEGENDARY
            if legendary_slots > MAX_LEGENDARY_SLOTS_PER_TABLE:
                errors.append(f'{tag}: {legendary_slots} Legendary-class slots, at most {MAX_LEGENDARY_SLOTS_PER_TABLE}')
            checked += 1
    if not checked:
        errors.append('no Emerald encounter tables checked')
    # Rule 6: the Cut-tree habitat is its species' only home.
    cut_slots = cut_tree_habitat()
    cut_species = {resolve_species(species, aliases) for species, _ in cut_slots}
    if not cut_slots or sum(odds for _, odds in cut_slots) != 100:
        errors.append(f'cut_tree: odds {[odds for _, odds in cut_slots]} must total 100')
    for species, odds in cut_slots:
        canonical = resolve_species(species, aliases)
        if not configured.get(canonical):
            errors.append(f'cut_tree: invalid configured species {species}')
        elif class_of(species) != ORDINARY:
            errors.append(f'cut_tree: {class_of(species)} {species} is not allowed on Cut trees')
        elif odds < MIN_ORDINARY_SLOT_PERCENT:
            errors.append(f'cut_tree: ordinary {species} has {odds}%, below {MIN_ORDINARY_SLOT_PERCENT}%')
        else:
            sources.setdefault(canonical, set()).add('CUT_TREES/cut_tree')
            wild_sources.add(canonical)
    for entry in emerald:
        for name, table in entry.items():
            if not isinstance(table, dict) or 'mons' not in table:
                continue
            for slot, mon in enumerate(table['mons']):
                if resolve_species(mon['species'], aliases) in cut_species:
                    errors.append(f"{entry['map']}/{name} slot {slot}: {mon['species']} lives only in Cut trees")
    paradox_species = {s for s, kind in classes.items() if kind == PARADOX and configured.get(s)}
    for species in sorted(paradox_species - wild_sources):
        errors.append(f'{species}: enabled Paradox species lacks a Hoenn wild source')
    for species in sorted(REQUIRED_OBTAINABLE_SPECIES):
        if not sources.get(species):
            errors.append(f'{species}: no playable ordinary Hoenn source')
    placement_count = None
    if args.placement is not None:
        placement_errors, placement_count = check_placements(args.placement, emerald, classes, aliases)
        errors.extend(placement_errors)
    for warning in warnings:
        print(f'WARN: {warning}')
    if errors:
        raise SystemExit('\n'.join(errors))
    print(f'PASS: {checked} Emerald encounter tables have explicit weights, the rarity ladder and valid level templates')
    print(f'PASS: the shared Cut-tree habitat ({len(cut_slots)} species) totals 100 and is their only home')
    if placement_count is not None:
        print(f'PASS: {placement_count} wild/quest placements from {args.placement} match their map, method and rate')
    print(f'INFO: {len(sources)} canonical species referenced by ordinary Hoenn tables; campaign access is not verified')
    honey_tables = sum('honey_mons' in e for e in emerald)
    print(f'INFO: {honey_tables} Honey tables require a usable land-encounter tile; geometric access is not verified')
    if args.source_report is not None:
        report = {
            'scope': 'Potential ordinary-table sources on Hoenn maps. Does not verify map access, method unlocks, or timing; excludes gifts, evolution and rare/scripted overlays. Zero-rate tables are retained because deliberate encounter methods need separate review.',
            'methods': sorted(ORDINARY_METHODS),
            'species_count': len(sources),
            'sources': {species: sorted(locations) for species, locations in sorted(sources.items())},
        }
        args.source_report.parent.mkdir(parents=True, exist_ok=True)
        args.source_report.write_text(json.dumps(report, indent=2) + '\n')


if __name__ == '__main__':
    main()
