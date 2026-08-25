#!/usr/bin/env python3
"""Generate Verdant's campaign-ordered, source-backed battle guide."""

from __future__ import annotations

import argparse
import json
import re
from collections import Counter, defaultdict
from pathlib import Path

import verdant_custom_teams as custom
import verdant_doubles_conversion as doubles
import verdant_team_quality_audit as quality


ROOT = Path(__file__).resolve().parents[1]
JSON_PATH = ROOT / "docs/verdant_battle_guide.json"
MARKDOWN_PATH = ROOT / "docs/verdant_battle_guide.md"
BESPOKE_PATH = ROOT / "docs/verdant_bespoke_battle_designs.json"

STRICT_CAPS = [14, 20, 30, 40, 45, 55, 60, 70, 80, 100]

MAP_ORDER = [
    "Route103", "Route102", "PetalburgCity", "Route104", "PetalburgWoods", "RustboroCity_Gym",
    "Route116", "RusturfTunnel", "RustboroCity", "Route105", "Route106", "DewfordTown_Gym", "GraniteCave",
    "Route109", "SlateportCity", "Route110", "MauvilleCity", "MauvilleCity_Gym",
    "Route111", "Route112", "FieryPath", "Route113", "FallarborTown", "Route114", "MeteorFalls",
    "MtChimney", "JaggedPass", "LavaridgeTown", "LavaridgeTown_Gym",
    "PetalburgCity_Gym", "Route117", "Route118", "Route119", "WeatherInstitute", "FortreeCity", "Route120",
    "Route115", "AbandonedShip", "Route121", "Route122", "MtPyre", "LilycoveCity", "MagmaHideout",
    "AquaHideout", "Route123", "Route124", "Route125", "MossdeepCity", "MossdeepCity_Gym",
    "MossdeepCity_SpaceCenter", "Route126", "Route127", "Route128", "SeafloorCavern", "SootopolisCity",
    "CaveOfOrigin", "SkyPillar", "SootopolisCity_Gym", "Route129", "Route130", "Route131", "Route132",
    "Route133", "Route134", "VictoryRoad", "EverGrandeCity", "BattleFrontier", "SSTidal", "AlteringCave",
]

REQUIRED_TRAINERS = {
    "TRAINER_ROXANNE_1", "TRAINER_BRAWLY_1", "TRAINER_WATTSON_1", "TRAINER_FLANNERY_1",
    "TRAINER_NORMAN_1", "TRAINER_WINONA_1", "TRAINER_TATE_AND_LIZA_1", "TRAINER_JUAN_1",
    "TRAINER_SIDNEY", "TRAINER_PHOEBE", "TRAINER_GLACIA", "TRAINER_DRAKE", "TRAINER_WALLACE",
    "TRAINER_GRUNT_PETALBURG_WOODS", "TRAINER_GRUNT_RUSTURF_TUNNEL",
    "TRAINER_MAXIE_MT_CHIMNEY", "TRAINER_MAXIE_MAGMA_HIDEOUT", "TRAINER_ARCHIE",
    "TRAINER_SHELLY_WEATHER_INSTITUTE", "TRAINER_SHELLY_SEAFLOOR_CAVERN",
    "TRAINER_MATT", "TRAINER_MATT_MT_PYRE", "TRAINER_TABITHA_MT_CHIMNEY",
    "TRAINER_TABITHA_MAGMA_HIDEOUT", "TRAINER_COURTNEY_METEOR_FALLS",
    "TRAINER_COURTNEY_MAGMA_HIDEOUT", "TRAINER_MAXIE_MOSSDEEP", "TRAINER_COURTNEY_MOSSDEEP",
    "TRAINER_STEVEN_MOSSDEEP", "TRAINER_WALLY_MAUVILLE", "TRAINER_WALLY_VR_1",
    "TRAINER_GRUNT_METEOR_FALLS", "TRAINER_MAXIE_MOSSDEEP", "TRAINER_COURTNEY_MOSSDEEP",
}

POSTGAME_SPECIALS = {
    "TRAINER_ANABEL", "TRAINER_TUCKER", "TRAINER_SPENSER", "TRAINER_GRETA",
    "TRAINER_NOLAND", "TRAINER_LUCY", "TRAINER_BRANDON",
}

STORY_RIVAL_PREFIXES = (
    "TRAINER_MAY_ROUTE_103_", "TRAINER_BRENDAN_ROUTE_103_",
    "TRAINER_MAY_RUSTBORO_", "TRAINER_BRENDAN_RUSTBORO_",
    "TRAINER_MAY_ROUTE_110_", "TRAINER_BRENDAN_ROUTE_110_",
    "TRAINER_MAY_ROUTE_119_", "TRAINER_BRENDAN_ROUTE_119_",
    "TRAINER_MAY_LILYCOVE_", "TRAINER_BRENDAN_LILYCOVE_",
)

MEGA_ITEMS = quality.mega_items()


def pretty(symbol: str, prefix: str) -> str:
    value = symbol.removeprefix(prefix)
    forms = {
        "ALOLAN": "Alolan", "GALARIAN": "Galarian", "HISUIAN": "Hisuian",
        "THERIAN": "Therian", "RAPID_STRIKE_STYLE": "Rapid Strike",
        "ICE_RIDER": "Ice Rider", "SHADOW_RIDER": "Shadow Rider",
    }
    for suffix, label in forms.items():
        if value.endswith("_" + suffix):
            value = value[: -(len(suffix) + 1)] + f" ({label})"
            break
    return value.replace("_", " ").title().replace("Ho Oh", "Ho-Oh").replace("Porygon Z", "Porygon-Z")


def map_rank(map_name: str) -> int:
    compact = map_name.replace("_", "")
    matches = [
        (len(marker), index)
        for index, marker in enumerate(MAP_ORDER)
        if marker.replace("_", "").lower() in compact.lower()
    ]
    if matches:
        return max(matches)[1]
    return len(MAP_ORDER) + 100


def chapter_for(map_name: str, trainer_id: str) -> dict:
    if trainer_id.startswith(("TRAINER_MAY_ROUTE_103_", "TRAINER_BRENDAN_ROUTE_103_")):
        return {"id": "opening", "title": "Opening rival battle", "badge": 0, "cap": 14, "rank": 0}
    if trainer_id.startswith(("TRAINER_MAY_RUSTBORO_", "TRAINER_BRENDAN_RUSTBORO_")):
        return {"id": "stone", "title": "Stone Badge", "badge": 0, "cap": 14, "rank": 10}
    if trainer_id.startswith(("TRAINER_MAY_ROUTE_110_", "TRAINER_BRENDAN_ROUTE_110_")):
        return {"id": "dynamo", "title": "Dynamo Badge", "badge": 2, "cap": 30, "rank": 30}
    if trainer_id.startswith(("TRAINER_MAY_ROUTE_119_", "TRAINER_BRENDAN_ROUTE_119_")):
        return {"id": "feather", "title": "Feather Badge", "badge": 5, "cap": 55, "rank": 60}
    if trainer_id.startswith(("TRAINER_MAY_LILYCOVE_", "TRAINER_BRENDAN_LILYCOVE_")):
        return {"id": "mind", "title": "Mind Badge", "badge": 6, "cap": 60, "rank": 70}
    if "TrickHousePuzzle" in map_name:
        match = re.search(r"Puzzle(\d+)", map_name)
        puzzle = int(match.group(1)) if match else 1
        badge = min(9, puzzle + 1)
        return {"id": f"trick-house-{puzzle}", "title": f"Trick House puzzle {puzzle}", "badge": badge, "cap": STRICT_CAPS[badge], "rank": 25 + puzzle}
    route_match = re.search(r"Route(\d+)", map_name)
    route_number = int(route_match.group(1)) if route_match else None
    if route_number == 103:
        return {"id": "feather", "title": "Feather Badge", "badge": 5, "cap": 55, "rank": 60}
    if route_number in (107, 108, 115):
        return {"id": "feather", "title": "Feather Badge", "badge": 5, "cap": 55, "rank": 60}
    if route_number == 117:
        return {"id": "heat", "title": "Heat Badge", "badge": 3, "cap": 40, "rank": 40}
    if route_number == 123:
        return {"id": "feather", "title": "Feather Badge", "badge": 5, "cap": 55, "rank": 60}
    if route_number in (126, 127, 128):
        return {"id": "mind", "title": "Mind Badge", "badge": 6, "cap": 60, "rank": 70}
    if route_number is not None and 129 <= route_number <= 134:
        return {"id": "rain", "title": "Rain Badge", "badge": 7, "cap": 70, "rank": 80}
    rank = map_rank(map_name)
    if rank <= 5:
        return {"id": "stone", "title": "Stone Badge", "badge": 0, "cap": 14, "rank": 10}
    if rank <= 12:
        return {"id": "knuckle", "title": "Knuckle Badge", "badge": 1, "cap": 20, "rank": 20}
    if rank <= 17:
        return {"id": "dynamo", "title": "Dynamo Badge", "badge": 2, "cap": 30, "rank": 30}
    if rank <= 28:
        return {"id": "heat", "title": "Heat Badge", "badge": 3, "cap": 40, "rank": 40}
    if rank <= 29:
        return {"id": "balance", "title": "Balance Badge", "badge": 4, "cap": 45, "rank": 50}
    if rank <= 37:
        return {"id": "feather", "title": "Feather Badge", "badge": 5, "cap": 55, "rank": 60}
    if rank <= 48:
        return {"id": "mind", "title": "Mind Badge", "badge": 6, "cap": 60, "rank": 70}
    if rank <= 57:
        return {"id": "rain", "title": "Rain Badge", "badge": 7, "cap": 70, "rank": 80}
    if rank <= 65:
        return {"id": "league", "title": "Pokémon League", "badge": 8, "cap": 80, "rank": 90}
    return {"id": "postgame", "title": "Postgame", "badge": 9, "cap": 100, "rank": 100}


def script_references() -> dict[str, list[dict]]:
    result = defaultdict(list)
    command_re = re.compile(r"\b(trainerbattle_(?:single|double|no_intro|rematch|rematch_double))\s+(TRAINER_[A-Z0-9_]+)")
    generic_re = re.compile(r"\btrainerbattle\s+(TRAINER_BATTLE_[A-Z0-9_]+),\s*(TRAINER_[A-Z0-9_]+)")
    multi_re = re.compile(
        r"\bmulti_2_vs_2\s+(TRAINER_[A-Z0-9_]+),\s*[^,]+,\s*"
        r"(TRAINER_[A-Z0-9_]+),\s*[^,]+,\s*([A-Z0-9_]+),"
    )
    defines = {}
    opponents = (ROOT / "include/constants/opponents.h").read_text()
    for symbol, value in re.findall(r"^#define\s+([A-Z0-9_]+)\s+(\d+)\s*$", opponents, re.M):
        defines[symbol] = int(value)
    trainer_by_value = {value: symbol for symbol, value in defines.items() if symbol.startswith("TRAINER_")}

    def resolve_trainer(symbol: str) -> str | None:
        if symbol.startswith(("TRAINER_", "MAY_", "BRENDAN_")):
            return symbol
        return trainer_by_value.get(defines.get(symbol, -1))

    def add(trainer_id: str | None, path: Path, line_number: int, command: str, role: str = "opponent") -> None:
        if not trainer_id:
            return
        row = {
            "map": path.parent.name, "command": command, "role": role,
            "line": line_number, "file": str(path.relative_to(ROOT)),
        }
        if row not in result[trainer_id]:
            result[trainer_id].append(row)

    for path in sorted((ROOT / "data/maps").glob("*/scripts.inc")):
        for line_number, line in enumerate(path.read_text(errors="ignore").splitlines(), 1):
            match = command_re.search(line)
            if match:
                add(match.group(2), path, line_number, match.group(1))
            generic = generic_re.search(line)
            if generic and "SET_TRAINER_" not in generic.group(1):
                add(generic.group(2), path, line_number, "trainerbattle")
            multi = multi_re.search(line)
            if multi:
                add(multi.group(1), path, line_number, "multi_2_vs_2", "opponent")
                add(multi.group(2), path, line_number, "multi_2_vs_2", "opponent")
                add(resolve_trainer(multi.group(3)), path, line_number, "multi_2_vs_2", "ally")
    return result


def rematch_tiers() -> dict[str, dict]:
    text = (ROOT / "src/battle_setup.c").read_text()
    result = {}
    for match in re.finditer(r"REMATCH\((TRAINER_[A-Z0-9_]+),\s*(TRAINER_[A-Z0-9_]+),\s*(TRAINER_[A-Z0-9_]+),\s*(TRAINER_[A-Z0-9_]+),\s*([A-Z0-9_]+)\)", text):
        trainer_ids = list(match.group(1, 2, 3, 4))
        for tier, trainer_id in enumerate(trainer_ids[1:], 1):
            if trainer_id == trainer_ids[0]:
                continue
            result[trainer_id] = {"tier": tier, "base": trainer_ids[0], "map": match.group(5)}
    return result


def trainer_metadata() -> dict[str, dict]:
    text = (ROOT / "src/data/trainers.h").read_text()
    result = {}
    for trainer_id, match in doubles.trainer_blocks(text).items():
        body = match.group(0)
        name_match = re.search(r'\.trainerName\s*=\s*_\("([^"]+)"\)', body)
        trainer_class = re.search(r"\.trainerClass\s*=\s*(TRAINER_CLASS_[A-Z0-9_]+)", body)
        flags = re.search(r"\.aiFlags\s*=\s*([^,\n]+)", body)
        result[trainer_id] = {
            "name": name_match.group(1) if name_match else pretty(trainer_id, "TRAINER_"),
            "class": pretty(trainer_class.group(1), "TRAINER_CLASS_") if trainer_class else "Trainer",
            "ai": [value.strip() for value in (flags.group(1).split("|") if flags else [])],
        }
    return result


def inferred_theme(team: dict) -> str:
    if team["synergy_tags"]:
        return " + ".join(team["synergy_tags"][:3])
    types = Counter(type_name for mon in team["mons"] for type_name in set(mon["types"]))
    dominant = types.most_common(1)[0][0].removeprefix("TYPE_").title() if types else "mixed"
    if team["rare_count"] or team["mega_count"]:
        return f"{dominant} rare-showcase balance"
    if team["avg_bst"] < 500:
        return f"underused {dominant.lower()} mixture"
    return f"{dominant} mixed offense"


def location_motif(location: str) -> str:
    lower = location.lower()
    if "lilycove" in lower or "slateport" in lower:
        return "the coastal city's fast pivots and broad team-building options"
    if "gym" in lower:
        return "the Gym's mechanical lesson"
    if any(word in lower for word in ("woods", "forest", "route 119", "ashen")):
        return "the surrounding forest and overgrowth"
    route = re.search(r"route\s+(\d+)", lower)
    if route and int(route.group(1)) in {*range(105, 110), *range(124, 135)}:
        return "the surrounding water route and coastal terrain"
    if route and int(route.group(1)) in (111, 112, 113, 114, 115):
        return "the mountain route's rock, sand, and changing weather"
    if route:
        return "the route's grassland, roadside cover, and open speed lanes"
    if any(word in lower for word in ("seafloor", "ship", "sootopolis")):
        return "the surrounding water route and coastal terrain"
    if any(word in lower for word in ("magma", "chimney", "jagged", "lavaridge", "fiery")):
        return "the volcanic route and its heat-driven hazards"
    if any(word in lower for word in ("cave", "tunnel", "falls", "victory road")):
        return "the cave's rock, darkness, and confined-board pressure"
    return "this point in the campaign"


def mon_role(mon: dict, team: dict) -> str:
    moves = set(mon["moves"])
    if mon["item"] in MEGA_ITEMS:
        return "Mega ace"
    if moves & quality.REDIRECTION_MOVES:
        return "redirection support"
    if moves & quality.SPEED_MOVES:
        return "speed control"
    if moves & quality.SETUP_MOVES:
        return "setup attacker"
    if mon["item"] in quality.CHOICE_ITEMS:
        return "Choice-locked breaker"
    if moves & quality.PROTECT_MOVES and len(mon["attack_moves"]) >= 2:
        return "protected attacker"
    if any(family in mon["species"] for family in custom.LEGENDARY_FAMILIES):
        return "rare centerpiece"
    if len(mon["status_moves"]) >= 2:
        return "disruption and support"
    return "coverage attacker"


def difficulty_band(score: int) -> str:
    if score >= 80:
        return "Extreme"
    if score >= 65:
        return "Brutal"
    if score >= 50:
        return "Hard"
    if score >= 35:
        return "Demanding"
    return "Introductory"


def build_guide() -> dict:
    report = quality.audit()
    bespoke = json.loads(BESPOKE_PATH.read_text()) if BESPOKE_PATH.exists() else {"designs": {}}
    bespoke_designs = bespoke.get("designs", {})
    bespoke_by_trainer = {}
    bespoke_ids_by_trainer = {}
    for design_id, design in bespoke_designs.items():
        trainer_ids = design.get("trainer_ids", [design_id])
        for trainer_id in trainer_ids:
            if trainer_id in bespoke_by_trainer:
                raise ValueError(f"trainer belongs to multiple bespoke encounters: {trainer_id}")
            bespoke_by_trainer[trainer_id] = design
            bespoke_ids_by_trainer[trainer_id] = design_id
    teams = {team["trainer_id"]: team for team in report["teams"]}
    references = script_references()
    rematches = rematch_tiers()
    metadata = trainer_metadata()
    family_counts = defaultdict(set)
    for team in report["teams"]:
        for mon in team["mons"]:
            family_counts[mon["species"]].add(custom.trainer_family(team["trainer_id"]))

    reachable = set(references) | set(rematches) | POSTGAME_SPECIALS
    missing = sorted(reachable - set(teams))
    if missing:
        raise ValueError(f"reachable trainers missing from quality audit: {missing}")

    compositions = {trainer_id: set(mon["species"] for mon in teams[trainer_id]["mons"]) for trainer_id in reachable}
    entries = []
    for trainer_id in sorted(reachable):
        team = teams[trainer_id]
        design = bespoke_by_trainer.get(trainer_id)
        encounter_id = bespoke_ids_by_trainer.get(trainer_id)
        source_rows = references.get(trainer_id, [])
        rematch = rematches.get(trainer_id)
        if source_rows:
            primary = min(source_rows, key=lambda row: (map_rank(row["map"]), row["line"]))
            map_name = primary["map"]
        elif rematch:
            base_rows = references.get(rematch["base"], [])
            primary = min(base_rows, key=lambda row: (map_rank(row["map"]), row["line"])) if base_rows else {"map": rematch["map"], "line": 0, "file": "src/battle_setup.c", "command": "rematch"}
            map_name = primary["map"]
        else:
            primary = {"map": "BattleFrontier", "line": 0, "file": "src/data/trainers.h", "command": "special"}
            map_name = primary["map"]

        chapter = chapter_for(map_name, trainer_id)
        is_story_rival = trainer_id.startswith(STORY_RIVAL_PREFIXES)
        encounter_role = "ally" if source_rows and any(row.get("role") == "ally" for row in source_rows) else "opponent"
        if encounter_role == "ally":
            category = "required ally"
        elif rematch or (source_rows and all(row["command"] == "trainerbattle_no_intro" for row in source_rows) and re.search(r"_[2-5]$", trainer_id)):
            category = "rematch"
        elif trainer_id in POSTGAME_SPECIALS:
            category = "postgame"
        elif trainer_id in REQUIRED_TRAINERS or is_story_rival:
            category = "required branch" if is_story_rival else "required"
        else:
            category = "optional"

        if category == "rematch":
            cap = None
            badge = None
            chapter_title = f"Rematches · {chapter['title']}"
        elif category == "postgame":
            cap = 100
            badge = 9
            chapter_title = "Postgame challenges"
        else:
            cap = chapter["cap"]
            badge = chapter["badge"]
            chapter_title = chapter["title"]

        other_ids = [value for value in reachable if custom.trainer_family(value) != custom.trainer_family(trainer_id)]
        closest_id = None
        closest_similarity = 0.0
        for other_id in other_ids:
            union = compositions[trainer_id] | compositions[other_id]
            similarity = len(compositions[trainer_id] & compositions[other_id]) / max(1, len(union))
            if similarity > closest_similarity:
                closest_id, closest_similarity = other_id, similarity
        average_family_use = sum(len(family_counts[mon["species"]]) for mon in team["mons"]) / len(team["mons"])
        uniqueness = round(max(30, min(100, 100 - closest_similarity * 55 - average_family_use * 2 + team["rare_count"] * 3 + team["mega_count"] * 4)))

        combined_difficulty = round(float(design["manual_difficulty"]) * 10) if design else round(team["quality_score"] * 0.58 + team["manifest_difficulty"] * 0.42)
        theme = inferred_theme(team)
        lead_names = [pretty(mon["species"], "SPECIES_") for mon in team["mons"][:2 if team["format"] == "double" else 1]]
        rare_names = [pretty(mon["species"], "SPECIES_") for mon in team["mons"] if any(family in mon["species"] for family in custom.LEGENDARY_FAMILIES)]
        mega_names = [pretty(mon["species"], "SPECIES_") for mon in team["mons"] if mon["item"] in MEGA_ITEMS]
        levels = [max(1, min(100, cap + mon["level_offset"])) if cap is not None else None for mon in team["mons"]]

        if team["format"] == "double":
            plan = f"{lead_names[0]} and {lead_names[1]} are the authored opening pair. The board plan is {theme.lower()}."
        else:
            plan = f"{lead_names[0]} opens this deliberate singles fight. The roster is built as {theme.lower()}."
        if encounter_role == "ally":
            plan = f"{metadata[trainer_id]['name']} fights beside the player in this multi battle. The partner roster supplies {theme.lower()}."
        plan += f" The composition turns {location_motif(team['location'])} into the fight's identity instead of using a generic route-trainer core."
        if rare_names:
            plan += f" The rare showcase is {', '.join(rare_names)}."
        if mega_names:
            plan += f" {', '.join(mega_names)} carries the team's one Mega slot."

        ai_names = [pretty(flag, "AI_FLAG_") for flag in metadata[trainer_id]["ai"]]
        behavior = f"The AI uses {', '.join(ai_names) if ai_names else 'the baseline viability profile'}. "
        if team["synergy_tags"]:
            behavior += f"Expect it to preserve {', '.join(team['synergy_tags'][:3]).lower()} rather than select six unrelated strong attacks."
        else:
            behavior += "This singles or low-stakes roster emphasizes coverage, status, and matchup pressure rather than a scripted multi-Pokémon combination."
        if encounter_role == "ally":
            behavior = f"This allied AI uses {', '.join(ai_names) if ai_names else 'the baseline viability profile'} and deploys its first three party slots under the two-trainer battle rules."

        if any("Trick Room" in tag for tag in team["synergy_tags"]):
            counterplay = "Deny Trick Room with Taunt or immediate focus fire, then use Protect to exhaust its limited turns."
        elif any("rain" in tag.lower() or "sun" in tag.lower() or "sand" in tag.lower() or "snow" in tag.lower() for tag in team["synergy_tags"]):
            counterplay = "Contest the weather setter, preserve your speed-control answer, and use Protect or Wide Guard against the amplified spread turns."
        elif any("redirection" in tag.lower() for tag in team["synergy_tags"]):
            counterplay = "Use spread damage, Taunt, or a coordinated double target so redirection cannot buy a free setup turn."
        elif any("Perish" in tag for tag in team["synergy_tags"]):
            counterplay = "Keep a pivot or Ghost-type available, deny the trapper, and do not spend the countdown attacking into Protect."
        else:
            counterplay = "Win the speed exchange, identify the highest-leverage support slot, and avoid trading your answer into the team's ace too early."
        if encounter_role == "ally":
            counterplay = "Build your selected pair to complement the ally's first three slots; do not duplicate its speed mode or leave its support turns without offensive follow-through."
        if design:
            plan = design["team_intent"] + " " + design["uniqueness"]
            behavior = design["bespoke_ai"]
            counterplay = design["intended_counterplay"]

        entries.append({
            "trainerId": trainer_id,
            "encounterId": encounter_id,
            "encounterAlternatives": design.get("trainer_ids", []) if design else [],
            "name": metadata[trainer_id]["name"],
            "trainerClass": metadata[trainer_id]["class"],
            "category": category,
            "encounterRole": encounter_role,
            "chapter": chapter_title,
            "chapterRank": 110 if category == "rematch" else 120 if category == "postgame" else chapter["rank"],
            "location": team["location"] if team["location"] != "Unmapped" else pretty(map_name, "MAP_"),
            "sourceMap": map_name,
            "source": primary,
            "badge": badge,
            "levelCap": cap,
            "format": team["format"],
            "partySize": team["party_size"],
            "theme": theme,
            "plan": plan,
            "behavior": behavior,
            "counterplay": counterplay,
            "designStatus": design["status"] if design else "not individually closed",
            "manualQuality": design["manual_quality"] if design else None,
            "corpusReview": design.get("corpus_review") if design else None,
            "competitiveReferences": design["competitive_references"] if design else [],
            "closure": design["closure"] if design else "",
            "difficultyScore": combined_difficulty,
            "difficultyBand": "Allied support" if encounter_role == "ally" else difficulty_band(combined_difficulty),
            "difficultyExplanation": (
                f"{team['party_size']} fully authored Pokémon, average BST {team['avg_bst']}, "
                f"average cap offset {team['avg_level_offset']:+}, ace offset {team['max_level_offset']:+}, "
                f"and {round(team['item_coverage'] * 100)}% held-item coverage."
            ),
            "uniquenessScore": uniqueness,
            "uniquenessExplanation": (
                f"Closest unrelated roster: {metadata[closest_id]['name'] if closest_id else 'none'} "
                f"({round(closest_similarity * 100)}% species overlap). Its species appear across an average of "
                f"{average_family_use:.1f} unrelated trainer families."
            ),
            "synergyTags": team["synergy_tags"],
            "ai": ai_names,
            "party": [
                {
                    "slot": index + 1,
                    "speciesId": mon["species"],
                    "species": pretty(mon["species"], "SPECIES_"),
                    "levelOffset": mon["level_offset"],
                    "effectiveLevel": levels[index],
                    "itemId": mon["item"],
                    "item": pretty(mon["item"], "ITEM_"),
                    "ability": pretty(mon["ability"], "ABILITY_"),
                    "spread": pretty(mon["spread"], "SPREAD_") if mon["spread"] else "Default",
                    "moves": [pretty(move, "MOVE_") for move in mon["moves"]],
                    "role": mon_role(mon, team),
                }
                for index, mon in enumerate(team["mons"])
            ],
        })

    category_order = {"required": 0, "required branch": 1, "required ally": 2, "optional": 3, "rematch": 4, "postgame": 5}
    league_order = {
        "TRAINER_SIDNEY": 1, "TRAINER_PHOEBE": 2, "TRAINER_GLACIA": 3,
        "TRAINER_DRAKE": 4, "TRAINER_WALLACE": 5,
    }
    entries.sort(key=lambda row: (
        row["chapterRank"],
        -100 if row["trainerId"].startswith(("TRAINER_MAY_ROUTE_103_", "TRAINER_BRENDAN_ROUTE_103_")) else map_rank(row["sourceMap"]),
        category_order[row["category"]],
        league_order.get(row["trainerId"], 0),
        row["source"]["line"],
        row["trainerId"],
    ))
    for order, entry in enumerate(entries, 1):
        entry["order"] = order

    unused = sorted(set(teams) - reachable)
    return {
        "meta": {
            "title": "Verdant Complete Battle Guide",
            "ordering": "Canonical campaign route; optional fights follow their local chapter; rematches and postgame are separate.",
            "levelRule": "Opponent level = active strict cap + authored offset, clamped to levels 1-100.",
            "reachableBattleDefinitions": len(entries),
            "internalUnusedDefinitions": len(unused),
            "qualityAudit": report["summary"],
            "bespokeClosed": sum(value.get("status") == "closed" for value in bespoke_designs.values()),
        },
        "entries": entries,
        "unusedTrainerIds": unused,
    }


def render_markdown(guide: dict) -> str:
    lines = [
        "# Verdant Complete Battle Guide",
        "",
        "This is the campaign-order guide to every reachable trainer definition. Optional fights are kept in their earliest chapter; rematches and postgame challenges are separate. Levels use the active strict cap plus the listed offset.",
        "",
        f"Reachable battle definitions: **{guide['meta']['reachableBattleDefinitions']}**  ",
        f"Internal unused definitions excluded from the walkthrough: **{guide['meta']['internalUnusedDefinitions']}**",
        "",
    ]
    current_chapter = None
    for entry in guide["entries"]:
        if entry["chapter"] != current_chapter:
            current_chapter = entry["chapter"]
            lines.extend([f"# {current_chapter}", ""])
        cap = f"Cap {entry['levelCap']}" if entry["levelCap"] is not None else "Active cap at rematch time"
        lines.extend([
            f"## {entry['order']:03}. {entry['name']} — {entry['location']}",
            "",
            f"**{entry['category'].title()} · {entry['format'].title()} · {cap} · {entry['difficultyBand']} {entry['difficultyScore']}/100 · Uniqueness {entry['uniquenessScore']}/100**",
            "",
            f"**Theme:** {entry['theme']}",
            "",
            entry["plan"],
            "",
            f"**How it behaves:** {entry['behavior']}",
            "",
            f"**Why it is hard:** {entry['difficultyExplanation']}",
            "",
            f"**How unique it is:** {entry['uniquenessExplanation']}",
            "",
            f"**Counterplay:** {entry['counterplay']}",
            "",
            *( [f"**Manual closure:** {entry['closure']}", ""] if entry["closure"] else [] ),
            "| # | Pokémon | Level | Item | Ability | Role | Moves |",
            "| ---: | --- | --- | --- | --- | --- | --- |",
        ])
        for mon in entry["party"]:
            level = f"{mon['effectiveLevel']} (cap {mon['levelOffset']:+})" if mon["effectiveLevel"] is not None else f"active cap {mon['levelOffset']:+}"
            lines.append(f"| {mon['slot']} | {mon['species']} | {level} | {mon['item']} | {mon['ability']} | {mon['role']} | {', '.join(mon['moves'])} |")
        lines.append("")
    return "\n".join(lines)


def check(guide: dict) -> None:
    problems = []
    orders = [entry["order"] for entry in guide["entries"]]
    if orders != list(range(1, len(orders) + 1)):
        problems.append("battle order is not contiguous")
    for entry in guide["entries"]:
        if not entry["theme"] or not entry["plan"] or not entry["behavior"] or not entry["difficultyExplanation"] or not entry["uniquenessExplanation"]:
            problems.append(f"{entry['trainerId']}: incomplete guide commentary")
        if len(entry["party"]) != entry["partySize"]:
            problems.append(f"{entry['trainerId']}: party size drift")
        for mon in entry["party"]:
            is_imposter_ditto = (
                mon["speciesId"] == "SPECIES_DITTO"
                and mon["ability"] == "Imposter"
                and mon["moves"] == ["Transform"]
            )
            if len(mon["moves"]) != 4 and not is_imposter_ditto:
                problems.append(f"{entry['trainerId']} {mon['species']}: incomplete moves")
        if entry["designStatus"] == "closed":
            if entry["manualQuality"] != 10:
                problems.append(f"{entry['trainerId']}: closed battle quality is not 10/10")
            if entry["difficultyScore"] < 65:
                problems.append(f"{entry['trainerId']}: closed battle is below 6.5 difficulty")
    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))
    print(
        f"PASS: {len(guide['entries'])} campaign/rematch/postgame battle definitions documented; "
        f"{guide['meta']['internalUnusedDefinitions']} unreachable internal definitions identified"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    guide = build_guide()
    if args.write:
        JSON_PATH.write_text(json.dumps(guide, indent=2) + "\n")
        MARKDOWN_PATH.write_text(render_markdown(guide))
    if args.check or not args.write:
        check(guide)


if __name__ == "__main__":
    main()
