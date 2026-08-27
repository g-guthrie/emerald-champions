#!/usr/bin/env python3
"""Keep authored acquisition encounters out of ordinary wild tables.

This is intentionally an explicit design ledger, not a name heuristic.  A
scripted battle only reserves a species when the event is meant to be that
species' acquisition identity.  Repeated trap battles, tutorials, ordinary
gifts, and prize counters are classified separately.
"""

from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
ENCOUNTERS = ROOT / "src/data/wild_encounters.json"
LEDGER = ROOT / "docs/emerald_champions_bespoke_encounter_ledger.json"
METHODS = ("land_mons", "water_mons", "rock_smash_mons", "fishing_mons", "honey_mons")

# Genuine static acquisitions.  Alternate battle/form-change endpoints are
# included so a later encounter edit cannot bypass the rule through a form.
STATIC_ACQUISITION_FAMILIES = {
    "articuno": {"SPECIES_ARTICUNO"},
    "deoxys": {"SPECIES_DEOXYS", "SPECIES_DEOXYS_ATTACK", "SPECIES_DEOXYS_DEFENSE", "SPECIES_DEOXYS_SPEED"},
    "diancie": {"SPECIES_DIANCIE", "SPECIES_DIANCIE_MEGA"},
    "groudon": {"SPECIES_GROUDON", "SPECIES_GROUDON_PRIMAL"},
    "heatran": {"SPECIES_HEATRAN"},
    "ho_oh": {"SPECIES_HO_OH"},
    "jirachi": {"SPECIES_JIRACHI"},
    "kecleon": {"SPECIES_KECLEON"},
    "kyogre": {"SPECIES_KYOGRE", "SPECIES_KYOGRE_PRIMAL"},
    "latias": {"SPECIES_LATIAS", "SPECIES_LATIAS_MEGA"},
    "latios": {"SPECIES_LATIOS", "SPECIES_LATIOS_MEGA"},
    "lugia": {"SPECIES_LUGIA"},
    "mew": {"SPECIES_MEW"},
    "mewtwo": {"SPECIES_MEWTWO", "SPECIES_MEWTWO_MEGA_X", "SPECIES_MEWTWO_MEGA_Y"},
    "moltres": {"SPECIES_MOLTRES"},
    "rayquaza": {"SPECIES_RAYQUAZA", "SPECIES_RAYQUAZA_MEGA"},
    "regice": {"SPECIES_REGICE"},
    "regigigas": {"SPECIES_REGIGIGAS"},
    "regirock": {"SPECIES_REGIROCK"},
    "registeel": {"SPECIES_REGISTEEL"},
    "rotom": {
        "SPECIES_ROTOM", "SPECIES_ROTOM_HEAT", "SPECIES_ROTOM_WASH",
        "SPECIES_ROTOM_FROST", "SPECIES_ROTOM_FAN", "SPECIES_ROTOM_MOW",
    },
    "spiritomb": {"SPECIES_SPIRITOMB"},
    "zapdos": {"SPECIES_ZAPDOS"},
}

# Authored gifts/restoration systems whose point is obtaining the line or a
# special form, rather than merely receiving an ordinary convenience Pokémon.
UNIQUE_GIFT_FAMILIES = {
    "castform": {
        "SPECIES_CASTFORM", "SPECIES_CASTFORM_SUNNY", "SPECIES_CASTFORM_RAINY",
        "SPECIES_CASTFORM_SNOWY",
    },
    "cosmog": {"SPECIES_COSMOG", "SPECIES_COSMOEM", "SPECIES_SOLGALEO", "SPECIES_LUNALA"},
    "fossil_aerodactyl": {"SPECIES_AERODACTYL", "SPECIES_AERODACTYL_MEGA"},
    "fossil_amaura": {"SPECIES_AMAURA", "SPECIES_AURORUS"},
    "fossil_anorith": {"SPECIES_ANORITH", "SPECIES_ARMALDO"},
    "fossil_archen": {"SPECIES_ARCHEN", "SPECIES_ARCHEOPS"},
    "fossil_cranidos": {"SPECIES_CRANIDOS", "SPECIES_RAMPARDOS"},
    "fossil_kabuto": {"SPECIES_KABUTO", "SPECIES_KABUTOPS"},
    "fossil_lileep": {"SPECIES_LILEEP", "SPECIES_CRADILY"},
    "fossil_omanyte": {"SPECIES_OMANYTE", "SPECIES_OMASTAR"},
    "fossil_shieldon": {"SPECIES_SHIELDON", "SPECIES_BASTIODON"},
    "fossil_tirtouga": {"SPECIES_TIRTOUGA", "SPECIES_CARRACOSTA"},
    "fossil_tyrunt": {"SPECIES_TYRUNT", "SPECIES_TYRANTRUM"},
    "meltan": {"SPECIES_MELTAN", "SPECIES_MELMETAL"},
    "mystery_ash_greninja": {"SPECIES_GRENINJA_BATTLE_BOND", "SPECIES_GRENINJA_ASH"},
    "mystery_magearna": {"SPECIES_MAGEARNA"},
    "mystery_meloetta": {"SPECIES_MELOETTA", "SPECIES_MELOETTA_PIROUETTE"},
    "togepi_egg": {"SPECIES_TOGEPI", "SPECIES_TOGETIC", "SPECIES_TOGEKISS"},
}

# Every literal setwildbattle species must be deliberately classified.  These
# two are repeated traps/obstacles rather than sole acquisition identities.
GENERIC_SCRIPTED_BATTLES = {
    "SPECIES_ELECTRODE": "Repeated item-ball trap battle; not a unique acquisition event.",
    "SPECIES_SUDOWOODO": "Battle Frontier watering obstacle; early Bonsly remains ordinary team access.",
}

EXPECTED_LITERAL_SETWILD = {
    "SPECIES_ARTICUNO", "SPECIES_DIANCIE", "SPECIES_ELECTRODE", "SPECIES_GROUDON",
    "SPECIES_HEATRAN", "SPECIES_JIRACHI", "SPECIES_KECLEON", "SPECIES_KYOGRE",
    "SPECIES_MEWTWO", "SPECIES_MOLTRES", "SPECIES_RAYQUAZA", "SPECIES_REGICE",
    "SPECIES_REGIGIGAS", "SPECIES_REGIROCK", "SPECIES_REGISTEEL", "SPECIES_ROTOM",
    "SPECIES_SPIRITOMB", "SPECIES_SUDOWOODO", "SPECIES_ZAPDOS",
}

# These before/after slots are the closed collision ledger.  Slot rates and
# levels remain untouched by this policy.
RESOLVED_SLOTS = (
    ("MAP_ROUTE103", "land_mons", 9, "SPECIES_ROTOM", "SPECIES_YAMPER"),
    ("MAP_ROUTE110", "land_mons", 8, "SPECIES_ROTOM", "SPECIES_PINCURCHIN"),
    ("MAP_ROUTE118", "land_mons", 4, "SPECIES_KECLEON", "SPECIES_PASSIMIAN"),
    ("MAP_ROUTE119", "land_mons", 6, "SPECIES_KECLEON", "SPECIES_CRAMORANT"),
    ("MAP_ROUTE119", "land_mons", 7, "SPECIES_KECLEON", "SPECIES_CRAMORANT"),
    ("MAP_ROUTE121", "land_mons", 9, "SPECIES_SPIRITOMB", "SPECIES_SINISTEA"),
    ("MAP_ROUTE121", "land_mons", 11, "SPECIES_SPIRITOMB", "SPECIES_SINISTEA"),
    ("MAP_MAGMA_HIDEOUT_4F", "land_mons", 8, "SPECIES_HEATRAN", "SPECIES_VOLCANION"),
    ("MAP_MAGMA_HIDEOUT_4F", "land_mons", 10, "SPECIES_HEATRAN", "SPECIES_VOLCANION"),
    ("MAP_VERDANTURF_MEADOW", "honey_mons", 5, "SPECIES_TOGETIC", "SPECIES_MILCERY"),
)


def ordinary_encounters(data: dict) -> list[dict]:
    group = next(group for group in data["wild_encounter_groups"] if group.get("for_maps"))
    return [entry for entry in group["encounters"] if "map" in entry]


def main() -> None:
    problems: list[str] = []
    data = json.loads(ENCOUNTERS.read_text())
    encounters = ordinary_encounters(data)
    by_map = {entry["map"]: entry for entry in encounters}
    wild_locations: dict[str, list[str]] = {}
    for entry in encounters:
        for method in METHODS:
            for index, mon in enumerate(entry.get(method, {}).get("mons", [])):
                wild_locations.setdefault(mon["species"], []).append(f"{entry['map']}:{method}[{index}]")

    protected = set().union(*STATIC_ACQUISITION_FAMILIES.values(), *UNIQUE_GIFT_FAMILIES.values())
    collisions = sorted(protected & set(wild_locations))
    if collisions:
        for species in collisions:
            problems.append(f"protected acquisition leaked into ordinary wild tables: {species} at {wild_locations[species]}")

    for map_id, method, index, old_species, replacement in RESOLVED_SLOTS:
        actual = by_map[map_id][method]["mons"][index]["species"]
        if actual != replacement:
            problems.append(f"resolved slot drifted: {map_id} {method}[{index}] is {actual}, expected {replacement}")
        if replacement in protected:
            problems.append(f"replacement creates a new protected collision: {replacement}")
        if old_species not in protected:
            problems.append(f"ledger old species is not protected: {old_species}")

    script_paths = list((ROOT / "data" / "maps").rglob("scripts.inc")) + list((ROOT / "data" / "scripts").rglob("*.inc"))
    literal_setwild = {
        species
        for path in script_paths
        for species in re.findall(r"\bsetwildbattle\s+(SPECIES_[A-Z0-9_]+)", path.read_text())
    }
    if literal_setwild != EXPECTED_LITERAL_SETWILD:
        problems.append(
            "literal setwildbattle inventory drifted: "
            f"missing={sorted(EXPECTED_LITERAL_SETWILD - literal_setwild)} "
            f"new={sorted(literal_setwild - EXPECTED_LITERAL_SETWILD)}"
        )
    classified_literal = protected | set(GENERIC_SCRIPTED_BATTLES)
    unclassified = sorted(literal_setwild - classified_literal)
    if unclassified:
        problems.append(f"literal scripted battles lack acquisition classification: {unclassified}")

    ledger = json.loads(LEDGER.read_text())
    if ledger.get("policy_version") != 1:
        problems.append("bespoke encounter ledger policy version drifted")
    ledger_slots = {
        (row["map"], row["method"], row["slot"], row["removed"], row["replacement"])
        for row in ledger.get("resolved_collisions", [])
    }
    if ledger_slots != set(RESOLVED_SLOTS):
        problems.append("documented collision ledger differs from enforced resolved slots")
    if ledger.get("unresolved_ambiguities") != []:
        problems.append("bespoke encounter ledger contains an unresolved ambiguity")

    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))
    print(
        f"PASS: {len(STATIC_ACQUISITION_FAMILIES)} static acquisitions, "
        f"{len(UNIQUE_GIFT_FAMILIES)} unique gift/restoration families, "
        f"{len(RESOLVED_SLOTS)} resolved wild slots, and zero protected collisions"
    )


if __name__ == "__main__":
    main()
