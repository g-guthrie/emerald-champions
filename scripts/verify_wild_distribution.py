#!/usr/bin/env python3
"""Validate active encounter tables and selected campaign habitat contracts."""
from pathlib import Path
import argparse
import copy
import json
import re
from verify_trainer_ability_legality import (SPECIES_MARKER, configured_species_abilities,
    preprocess_species_info, species_aliases, resolve_species)

ROOT = Path(__file__).resolve().parents[1]
# Ordinary residents must remain discoverable without rare-slot grinding.
MIN_ORDINARY_SPECIES_PERCENT = 4
ORDINARY_METHODS = {"land_mons", "water_mons", "rock_smash_mons", "fishing_mons", "honey_mons"}

# These families previously existed only in disabled DexNav data or Rock Smash
# tables on maps without smashable rocks. Keep an actual usable local source.
REQUIRED_OBTAINABLE_SPECIES = {
    "SPECIES_CHERUBI", "SPECIES_HATENNA", "SPECIES_SANDYGAST", "SPECIES_PINCURCHIN",
}

# Base encounter odds. Existing Lure, Sweet Scent and lead abilities may improve
# discovery odds; those deliberate tools are not an accidental alternate source.
RARE_RESIDENTS = {}
RARE_RESIDENTS.update({
    ("MAP_ROUTE116", "land_mons", "SPECIES_DREEPY"): 2,
    ("MAP_ROUTE108", "water_mons", "SPECIES_LAPRAS"): 2,
    ("MAP_ROUTE111", "water_mons", "SPECIES_DRATINI"): 2,
})

# These areas previously mixed donor level-2 encounters with authored
# late-campaign encounters, or placed level-59/60 final forms among level-30/40
# residents. Keep the stage bounds broad enough for local rare discoveries.
CAMPAIGN_STAGE_BOUNDS = {
    ("MAP_ROUTE105", "land_mons"): (20, 32),
    ("MAP_ROUTE115", "land_mons"): (15, 19),
    ("MAP_ALTERING_CAVE_1F", "land_mons"): (40, 46),
    ("MAP_ALTERING_CAVE_B1F", "land_mons"): (40, 46),
    ("MAP_ALTERING_CAVE_B1F", "water_mons"): (35, 50),
    ("MAP_SCORCHED_SLAB_B1F", "land_mons"): (38, 46),
    ("MAP_SCORCHED_SLAB_B2F", "land_mons"): (38, 46),
    ("MAP_SCORCHED_SLAB_HEATRANS_ROOM", "land_mons"): (38, 47),
    ("MAP_MAGMA_HIDEOUT_3F_1R", "land_mons"): (26, 40),
    ("MAP_MAGMA_HIDEOUT_3F_2R", "land_mons"): (26, 40),
    ("MAP_MAGMA_HIDEOUT_3F_3R", "land_mons"): (26, 40),
    ("MAP_MAGMA_HIDEOUT_4F", "land_mons"): (26, 40),
    ("MAP_VICTORY_ROAD_1F", "land_mons"): (35, 46),
    ("MAP_VICTORY_ROAD_B1F", "land_mons"): (37, 48),
    ("MAP_VICTORY_ROAD_B2F", "land_mons"): (39, 52),
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


def check_campaign_habitats(group):
    errors = []
    entries = {entry["map"]: entry for entry in group["encounters"]}
    fishing = next(f for f in group["fields"] if f["type"] == "fishing_mons")
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
        indices = fishing["groups"]["old_rod"]
        actual = {mons[i]["species"].removeprefix("SPECIES_")
                  for i in indices if i < len(mons)}
        if actual != required or not table.get("encounter_rate"):
            errors.append(f"{map_id}/old_rod: fishing habitat must retain {sorted(required)}")
    surf_habitats = {
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
    for map_id, required in surf_habitats.items():
        table = entries.get(map_id, {}).get("water_mons", {})
        actual = {m["species"].removeprefix("SPECIES_") for m in table.get("mons", [])}
        if not table.get("encounter_rate") or not required <= actual:
            errors.append(f"{map_id}/surf: signature residents missing: {sorted(required - actual)}")
    return errors


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source-report', type=Path, help='Write potential Hoenn table sources, not a reachability certificate')
    args = parser.parse_args()
    sources = {}
    payload = json.loads((ROOT / 'src/data/wild_encounters.json').read_text())
    group = next(g for g in payload['wild_encounter_groups'] if g['label'] == 'gWildMonHeaders')
    groups = json.loads((ROOT / 'data/maps/map_groups.json').read_text())
    active = set()
    map_rows = {}
    for name in (n for g in groups['group_order'] for n in groups[g]):
        row = json.loads((ROOT / 'data/maps' / name / 'map.json').read_text())
        map_rows[row['id']] = row
        if row.get('region', 'REGION_HOENN') == 'REGION_HOENN':
            active.add(row['id'])
    fields = {f['type']: f for f in group['fields']}
    dexnav_config = (ROOT / 'include/config/dexnav.h').read_text()
    dexnav_enabled = re.search(r'#define DEXNAV_ENABLED\s+(TRUE|FALSE|0|1)', dexnav_config).group(1) in {'TRUE', '1'}
    hidden_probability = int(re.search(r'#define HIDDEN_MON_PROBABILTY\s+(\d+)', dexnav_config).group(1))
    available_methods = ORDINARY_METHODS | ({'hidden_mons'} if dexnav_enabled else set())
    configured = configured_species_abilities()
    species_info = preprocess_species_info()
    species_info = species_info[species_info.index('const struct SpeciesInfo gSpeciesInfo[]'):]
    markers = list(SPECIES_MARKER.finditer(species_info))
    paradox_species = {
        marker.group(1)
        for index, marker in enumerate(markers)
        if re.search(r'\.isParadox\s*=\s*1\b',
            species_info[marker.start():markers[index + 1].start() if index + 1 < len(markers) else len(species_info)])
    }
    aliases = species_aliases()
    errors, checked = check_campaign_habitats(group), 0
    wild_sources = set()
    for entry in group['encounters']:
        if entry['map'] not in active:
            continue
        if 'hidden_mons' in entry and not dexnav_enabled:
            errors.append(f"{entry['map']}/hidden_mons: DexNav is disabled; this is not an obtainable source")
        if 'rock_smash_mons' in entry:
            if not any(obj.get('script') == 'EventScript_RockSmash' for obj in map_rows[entry['map']]['object_events']):
                errors.append(f"{entry['map']}/rock_smash_mons: map has no smashable rock")
        for table in entry.values():
            if isinstance(table, dict) and 'mons' in table:
                wild_sources.update(mon['species'] for mon in table['mons'])
        if any(mon['species'] == 'SPECIES_MILOTIC'
               for table in entry.values() if isinstance(table, dict) and 'mons' in table
               for mon in table['mons']):
            errors.append(f"{entry['map']}: Milotic must be obtained through Feebas evolution")
        if 'honey_mons' in entry and 'land_mons' in entry:
            land_min = min(mon['min_level'] for mon in entry['land_mons']['mons'])
            honey_min = min(mon['min_level'] for mon in entry['honey_mons']['mons'])
            if honey_min < land_min:
                errors.append(f"{entry['map']}/honey_mons: levels fall below local land encounters")
        for name, field in fields.items():
            if name not in entry:
                continue
            table = entry[name]
            rates = table.get("encounter_rates", field["encounter_rates"])
            mons = table['mons']
            tag = f"{entry['map']}/{name}"
            stage = CAMPAIGN_STAGE_BOUNDS.get((entry['map'], name))
            if stage is not None and mons:
                if min(mon['min_level'] for mon in mons) < stage[0] or max(mon['max_level'] for mon in mons) > stage[1]:
                    errors.append(f'{tag}: encounter levels leave the {stage[0]}-{stage[1]} campaign stage')
            encounter_rate = table.get('encounter_rate')
            if type(encounter_rate) is not int or not 0 <= encounter_rate <= 255:
                errors.append(f'{tag}: encounter rate must fit the native unsigned byte')
            if name == 'hidden_mons' and encounter_rate != 0:
                errors.append(f'{tag}: DexNav land-hidden tables must use encounter_rate 0')
            if len(mons) != len(field["encounter_rates"]) or len(mons) != len(rates) or not mons:
                errors.append(f'{tag}: slots do not match encounter weights')
                continue
            if any(type(r) is not int or not 0 <= r <= 100 for r in rates):
                errors.append(f'{tag}: invalid probability weights')
                continue
            valid_mons = True
            for slot, mon in enumerate(mons):
                species = mon.get('species')
                if (not isinstance(species, str) or species in {'SPECIES_NONE', 'SPECIES_EGG'}
                        or not configured.get(resolve_species(species, aliases))):
                    errors.append(f'{tag}: invalid configured encounter species {species}')
                    valid_mons = False
                elif name in available_methods and rates[slot] > 0:
                    sources.setdefault(resolve_species(species, aliases), set()).add(tag)
                if (type(mon.get('min_level')) is not int or type(mon.get('max_level')) is not int
                        or not 1 <= mon['min_level'] <= mon['max_level'] <= 100):
                    errors.append(f'{tag}: invalid level range for {species}')
                    valid_mons = False
            if not valid_mons:
                continue
            methods = field.get('groups', {name: list(range(len(mons)))})
            for method, indices in methods.items():
                if not indices or len(set(indices)) != len(indices) or any(type(i) is not int or not 0 <= i < len(mons) for i in indices):
                    errors.append(f'{tag}/{method}: invalid slot indices')
                    continue
                if sum(rates[i] for i in indices) != 100:
                    errors.append(f'{tag}/{method}: weights must total100')
                if encounter_rate == 0 and name != 'hidden_mons':
                    continue
                totals = {}
                for index in indices:
                    species = resolve_species(mons[index]['species'], aliases)
                    totals[species] = totals.get(species, 0) + rates[index]
                for species, rate in totals.items():
                    if name == 'hidden_mons' and hidden_probability * rate < MIN_ORDINARY_SPECIES_PERCENT * 100:
                        errors.append(f'{tag}/{method}: {species} has less than {MIN_ORDINARY_SPECIES_PERCENT}% of DexNav searches')
                    expected = RARE_RESIDENTS.get((entry['map'], method, species))
                    if expected is not None:
                        if rate != expected:
                            errors.append(f'{tag}/{method}: {species} must have {expected}%, got {rate}%')
                    elif rate < MIN_ORDINARY_SPECIES_PERCENT:
                        errors.append(f'{tag}/{method}: unapproved rare resident {species} has {rate}%')
                    if species == 'SPECIES_FEEBAS':
                        errors.append(f'{tag}/{method}: Feebas is exclusive to the seeded Route119 fishing spots')
                for (map_id, rare_method, species), expected in RARE_RESIDENTS.items():
                    if map_id == entry['map'] and rare_method == method and species not in totals:
                        errors.append(f'{tag}/{method}: missing rare resident {species}')
            checked += 1
    # Compare materialized water tables to today's authored route sheet. This
    # protects edits to authoring inputs without freezing the old encounters.
    import emerald_champions_rebuild_wild_water as rebuild
    water_maps = {e['map'] for e in group['encounters']
                  if e['map'] in active and (e.get('water_mons') or e.get('fishing_mons'))}
    if set(rebuild.load_route_sheet()) != water_maps:
        errors.append('authored water rows differ from active Hoenn water maps')
    try:
        for method in rebuild.materialize_water(copy.deepcopy(payload)):
            errors.append(f'{method}: generated species differ from authored row')
    except (KeyError, ValueError) as error:
        errors.append(f'invalid water authoring: {error}')
    if not checked:
        errors.append('no active encounter tables checked')
    for species in sorted(paradox_species - wild_sources):
        errors.append(f'{species}: enabled Paradox species lacks a Hoenn wild source')
    for species in sorted(REQUIRED_OBTAINABLE_SPECIES):
        if not sources.get(species):
            errors.append(f'{species}: no playable ordinary Hoenn source')
    if errors:
        raise SystemExit('\n'.join(errors))
    print(f'PASS: {checked} Hoenn encounter data tables have configured species, valid numeric fields and slot probabilities')
    print(f'INFO: {len(sources)} canonical species referenced by ordinary Hoenn tables; campaign access is not verified')
    honey_tables = sum('honey_mons' in e for e in group['encounters'] if e['map'] in active)
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
