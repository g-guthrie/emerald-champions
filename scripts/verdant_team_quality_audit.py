#!/usr/bin/env python3
"""Audit every Verdant trainer party as the battle engine will execute it."""

from __future__ import annotations

import argparse
import json
import re
from collections import Counter, defaultdict
from pathlib import Path

import verdant_custom_teams as custom
import verdant_doubles_conversion as doubles


ROOT = Path(__file__).resolve().parents[1]
FORMATS_PATH = ROOT / "docs/verdant_doubles_manifest.json"
TRAINERS_PATH = ROOT / "src/data/trainers.h"
PARTIES_PATH = ROOT / "src/data/trainer_parties.h"
REPORT_JSON_PATH = ROOT / "docs/verdant_team_quality_audit.json"
REPORT_MD_PATH = ROOT / "docs/verdant_team_quality_audit.md"

PROTECT_MOVES = {
    "MOVE_PROTECT", "MOVE_DETECT", "MOVE_KINGS_SHIELD", "MOVE_SPIKY_SHIELD",
    "MOVE_BANEFUL_BUNKER", "MOVE_OBSTRUCT", "MOVE_WIDE_GUARD", "MOVE_QUICK_GUARD",
    "MOVE_CRAFTY_SHIELD", "MOVE_MAT_BLOCK",
}
SPEED_MOVES = {
    "MOVE_TAILWIND", "MOVE_TRICK_ROOM", "MOVE_ICY_WIND", "MOVE_ELECTROWEB",
    "MOVE_BULLDOZE", "MOVE_GLACIATE", "MOVE_STRING_SHOT", "MOVE_THUNDER_WAVE",
    "MOVE_NUZZLE", "MOVE_QUASH", "MOVE_AFTER_YOU", "MOVE_STICKY_WEB",
    "MOVE_SCARY_FACE", "MOVE_GLARE",
}
REDIRECTION_MOVES = {"MOVE_FOLLOW_ME", "MOVE_RAGE_POWDER", "MOVE_SPOTLIGHT", "MOVE_ALLY_SWITCH"}
SETUP_MOVES = {
    "MOVE_SWORDS_DANCE", "MOVE_NASTY_PLOT", "MOVE_DRAGON_DANCE", "MOVE_QUIVER_DANCE",
    "MOVE_CALM_MIND", "MOVE_BULK_UP", "MOVE_SHELL_SMASH", "MOVE_GEOMANCY",
    "MOVE_BELLY_DRUM", "MOVE_COIL", "MOVE_SHIFT_GEAR", "MOVE_TAIL_GLOW",
}
PIVOT_MOVES = {"MOVE_U_TURN", "MOVE_VOLT_SWITCH", "MOVE_PARTING_SHOT", "MOVE_FLIP_TURN"}
DEFENSIVE_TEMPO_MOVES = PROTECT_MOVES | REDIRECTION_MOVES | SPEED_MOVES | {
    "MOVE_FAKE_OUT", "MOVE_REFLECT", "MOVE_LIGHT_SCREEN", "MOVE_AURORA_VEIL",
    "MOVE_SNARL", "MOVE_PARTING_SHOT", "MOVE_WILL_O_WISP", "MOVE_ENCORE",
    "MOVE_TAUNT", "MOVE_QUASH", "MOVE_STRENGTH_SAP", "MOVE_SPORE",
    "MOVE_SLEEP_POWDER", "MOVE_HYPNOSIS", "MOVE_YAWN", "MOVE_GLARE",
    "MOVE_ROAR", "MOVE_WHIRLWIND", "MOVE_LEECH_SEED", "MOVE_TOXIC",
}
TRAP_MOVES = {"MOVE_MEAN_LOOK", "MOVE_BLOCK", "MOVE_SPIDER_WEB", "MOVE_ANCHOR_SHOT", "MOVE_SPIRIT_SHACKLE"}
WEATHER_MOVES = {
    "rain": {"MOVE_RAIN_DANCE"},
    "sun": {"MOVE_SUNNY_DAY"},
    "sand": {"MOVE_SANDSTORM"},
    "snow": {"MOVE_HAIL", "MOVE_SNOWSCAPE"},
}
WEATHER_SETTERS = {
    "rain": {"ABILITY_DRIZZLE"},
    "sun": {"ABILITY_DROUGHT", "ABILITY_ORICHALCUM_PULSE"},
    "sand": {"ABILITY_SAND_STREAM"},
    "snow": {"ABILITY_SNOW_WARNING"},
}
WEATHER_ABUSERS = {
    "rain": {"ABILITY_SWIFT_SWIM", "ABILITY_RAIN_DISH", "ABILITY_HYDRATION", "ABILITY_DRY_SKIN"},
    "sun": {"ABILITY_CHLOROPHYLL", "ABILITY_SOLAR_POWER", "ABILITY_FLOWER_GIFT"},
    "sand": {"ABILITY_SAND_RUSH", "ABILITY_SAND_FORCE", "ABILITY_SAND_VEIL"},
    "snow": {"ABILITY_SLUSH_RUSH", "ABILITY_ICE_BODY", "ABILITY_SNOW_CLOAK"},
}
TERRAIN_MOVES = {
    "electric terrain": {"MOVE_ELECTRIC_TERRAIN"},
    "grassy terrain": {"MOVE_GRASSY_TERRAIN"},
    "psychic terrain": {"MOVE_PSYCHIC_TERRAIN"},
    "misty terrain": {"MOVE_MISTY_TERRAIN"},
}
TERRAIN_ABILITIES = {
    "electric terrain": {"ABILITY_ELECTRIC_SURGE"},
    "grassy terrain": {"ABILITY_GRASSY_SURGE"},
    "psychic terrain": {"ABILITY_PSYCHIC_SURGE"},
    "misty terrain": {"ABILITY_MISTY_SURGE"},
}
PRIORITY_MOVES = {
    "MOVE_FAKE_OUT", "MOVE_EXTREME_SPEED", "MOVE_SUCKER_PUNCH", "MOVE_AQUA_JET",
    "MOVE_MACH_PUNCH", "MOVE_BULLET_PUNCH", "MOVE_ICE_SHARD", "MOVE_SHADOW_SNEAK",
    "MOVE_GRASSY_GLIDE", "MOVE_FIRST_IMPRESSION", "MOVE_QUICK_ATTACK", "MOVE_VACUUM_WAVE",
}
CHOICE_ITEMS = {"ITEM_CHOICE_BAND", "ITEM_CHOICE_SPECS", "ITEM_CHOICE_SCARF"}
MEGA_ITEM_EXCEPTIONS = {"ITEM_EVIOLITE"}
INTENTIONAL_LEVEL_ONE = {"TRAINER_KEIRA"}


def has_complete_moveset(mon: dict) -> bool:
    return len(mon["moves"]) == 4 or (
        mon["species"] == "SPECIES_DITTO"
        and mon["ability"] == "ABILITY_IMPOSTER"
        and mon["moves"] == ["MOVE_TRANSFORM"]
    )


def field(body: str, name: str, default: str = "") -> str:
    match = re.search(rf"\.{name}\s*=\s*([^,\n}}]+)", body)
    return match.group(1).strip() if match else default


def active_blocks(text: str, prefix: str) -> dict[str, str]:
    return {
        name: body
        for name, body in re.findall(
            rf"^\s*\[({prefix}_[A-Z0-9_]+)\]\s*=\s*\{{(.*?)(?=^\s*\[{prefix}_|\Z)",
            doubles.select_rebalanced(text),
            re.M | re.S,
        )
    }


def parse_moves() -> dict[str, dict]:
    result = {}
    for name, body in active_blocks((ROOT / "src/data/battle_moves.h").read_text(), "MOVE").items():
        power_text = field(body, "power", "0")
        priority_text = field(body, "priority", "0")
        result[name] = {
            "power": int(power_text) if re.fullmatch(r"-?\d+", power_text) else 0,
            "priority": int(priority_text) if re.fullmatch(r"-?\d+", priority_text) else 0,
            "effect": field(body, "effect"),
            "type": field(body, "type"),
            "target": field(body, "target"),
            "split": field(body, "split"),
        }
    return result


def parse_species() -> dict[str, dict]:
    result = {}
    for name, body in active_blocks((ROOT / "src/data/pokemon/base_stats.h").read_text(), "SPECIES").items():
        stats = []
        for key in ("baseHP", "baseAttack", "baseDefense", "baseSpeed", "baseSpAttack", "baseSpDefense"):
            value = field(body, key, "0")
            stats.append(int(value) if value.isdigit() else 0)
        ability_match = re.search(r"\.abilities\s*=\s*\{([^}]+)\}", body)
        abilities = re.findall(r"ABILITY_[A-Z0-9_]+", ability_match.group(1)) if ability_match else []
        result[name] = {
            "bst": sum(stats),
            "speed": stats[3],
            "types": [value for value in (field(body, "type1"), field(body, "type2")) if value],
            "abilities": abilities,
        }
    return result


def mega_items() -> set[str]:
    text = (ROOT / "src/data/pokemon/evolution.h").read_text()
    return set(re.findall(r"\{EVO_MEGA_EVOLUTION,\s*(ITEM_[A-Z0-9_]+),", text))


def trainer_name(block: str) -> str:
    match = re.search(r'\.trainerName\s*=\s*_\("([^"]+)"\)', block)
    return match.group(1) if match else "Unknown"


def parse_mon(entry: str, move_data: dict[str, dict], species_data: dict[str, dict]) -> dict:
    species = field(entry, "species", "SPECIES_NONE")
    moves_match = re.search(r"\.moves\s*=\s*([^\n}]+)", entry)
    moves = re.findall(r"MOVE_[A-Z0-9_]+", moves_match.group(1)) if moves_match else []
    moves = [move for move in moves if move != "MOVE_NONE"]
    ability_slot_text = field(entry, "ability", "0").split()[0]
    ability_slot = int(ability_slot_text) if ability_slot_text.isdigit() else 0
    abilities = species_data.get(species, {}).get("abilities", [])
    ability = abilities[ability_slot] if ability_slot < len(abilities) else (abilities[0] if abilities else "ABILITY_NONE")
    return {
        "species": species,
        "level_offset": int(field(entry, "lvl", "0")),
        "item": field(entry, "heldItem", "ITEM_NONE"),
        "ability_slot": ability_slot,
        "ability": ability,
        "spread": field(entry, "spread"),
        "moves": moves,
        "bst": species_data.get(species, {}).get("bst", 0),
        "speed": species_data.get(species, {}).get("speed", 0),
        "types": species_data.get(species, {}).get("types", []),
        "attack_moves": [move for move in moves if move_data.get(move, {}).get("power", 0) > 0],
        "status_moves": [move for move in moves if move_data.get(move, {}).get("power", 0) == 0],
    }


def synergy_tags(mons: list[dict], move_data: dict[str, dict]) -> list[str]:
    moves = {move for mon in mons for move in mon["moves"]}
    abilities = {mon["ability"] for mon in mons}
    tags = []

    for weather in WEATHER_MOVES:
        has_setter = bool(moves & WEATHER_MOVES[weather] or abilities & WEATHER_SETTERS[weather])
        weather_attacks = {
            "rain": {"MOVE_THUNDER", "MOVE_HURRICANE", "MOVE_WEATHER_BALL", "MOVE_WATER_SPOUT"},
            "sun": {"MOVE_SOLAR_BEAM", "MOVE_SOLAR_BLADE", "MOVE_WEATHER_BALL", "MOVE_ERUPTION"},
            "sand": {"MOVE_WEATHER_BALL"},
            "snow": {"MOVE_BLIZZARD", "MOVE_AURORA_VEIL", "MOVE_WEATHER_BALL"},
        }
        has_abuser = bool(abilities & WEATHER_ABUSERS[weather] or moves & weather_attacks[weather])
        if has_setter and has_abuser:
            tags.append(f"{weather} engine")
    for terrain in TERRAIN_MOVES:
        if moves & TERRAIN_MOVES[terrain] or abilities & TERRAIN_ABILITIES[terrain]:
            tags.append(terrain)
    if "MOVE_TRICK_ROOM" in moves:
        slow_count = sum(mon["speed"] <= 70 for mon in mons)
        tags.append("Trick Room" if slow_count >= max(2, len(mons) // 2) else "mixed-speed Trick Room")
    if "MOVE_TAILWIND" in moves:
        tags.append("Tailwind")
    if moves & {"MOVE_ICY_WIND", "MOVE_ELECTROWEB", "MOVE_BULLDOZE", "MOVE_GLACIATE"}:
        tags.append("active speed control")
    if moves & REDIRECTION_MOVES and moves & SETUP_MOVES:
        tags.append("redirection setup")
    if "MOVE_FAKE_OUT" in moves and (moves & SETUP_MOVES or moves & SPEED_MOVES):
        tags.append("Fake Out tempo")
    has_trapping_effect = bool(moves & TRAP_MOVES) or any(
        move_data.get(move, {}).get("effect") in {"EFFECT_TRAP", "EFFECT_MEAN_LOOK"}
        for move in moves
    )
    if "MOVE_PERISH_SONG" in moves and (has_trapping_effect or "ABILITY_SHADOW_TAG" in abilities):
        tags.append("Perish trap")
    if "MOVE_BEAT_UP" in moves and "ABILITY_JUSTIFIED" in abilities:
        tags.append("Beat Up + Justified")
    if "MOVE_FROST_BREATH" in moves and "ABILITY_ANGER_POINT" in abilities:
        tags.append("Frost Breath + Anger Point")
    if "MOVE_SURF" in moves and abilities & {
        "ABILITY_STEAM_ENGINE", "ABILITY_WATER_COMPACTION", "ABILITY_WATER_ABSORB", "ABILITY_DRY_SKIN",
    }:
        tags.append("Surf ally activation")
    if "MOVE_DISCHARGE" in moves and abilities & {
        "ABILITY_LIGHTNING_ROD", "ABILITY_MOTOR_DRIVE", "ABILITY_VOLT_ABSORB",
    }:
        tags.append("Discharge immunity")
    if "MOVE_EARTHQUAKE" in moves and (
        "ABILITY_LEVITATE" in abilities or any("TYPE_FLYING" in mon["types"] for mon in mons)
    ):
        tags.append("Earthquake immunity")
    if "ABILITY_NEUTRALIZING_GAS" in abilities and "ABILITY_SLOW_START" in abilities:
        tags.append("Neutralizing Gas + Regigigas")
    if "ABILITY_PLUS" in abilities and "ABILITY_MINUS" in abilities:
        tags.append("Plus + Minus")
    if moves & PIVOT_MOVES and ("ABILITY_INTIMIDATE" in abilities or "MOVE_FAKE_OUT" in moves):
        tags.append("pivot control")
    if len(moves & PIVOT_MOVES) >= 2:
        tags.append("pivot offense")
    if ("MOVE_REFLECT" in moves or "MOVE_LIGHT_SCREEN" in moves or "MOVE_AURORA_VEIL" in moves) and moves & SETUP_MOVES:
        tags.append("screens setup")
    if "MOVE_EXPLOSION" in moves or "MOVE_SELF_DESTRUCT" in moves:
        if "MOVE_PROTECT" in moves or any("TYPE_GHOST" in mon["types"] for mon in mons):
            tags.append("protected Explosion")
    if {"MOVE_TOXIC", "MOVE_LEECH_SEED"} & moves and moves & PROTECT_MOVES:
        tags.append("residual control")
    if len(moves & {"MOVE_HYPER_VOICE", "MOVE_BOOMBURST", "MOVE_OVERDRIVE", "MOVE_RELIC_SONG"}) >= 2:
        tags.append("sound offense")
    if len(moves & {"MOVE_SLEEP_POWDER", "MOVE_SPORE", "MOVE_HYPNOSIS", "MOVE_DARK_VOID", "MOVE_LOVELY_KISS"}) >= 2:
        tags.append("sleep pressure")
    if len(moves & {"MOVE_STEALTH_ROCK", "MOVE_SPIKES", "MOVE_TOXIC_SPIKES", "MOVE_STICKY_WEB"}) >= 2:
        tags.append("hazard stack")
    if "MOVE_STICKY_WEB" in moves and moves & SETUP_MOVES:
        tags.append("web offense")
    if len(moves & PRIORITY_MOVES) >= 3:
        tags.append("priority pressure")
    if "MOVE_ERUPTION" in moves and moves & SPEED_MOVES:
        tags.append("speed-assisted Eruption")
    if len(moves & DEFENSIVE_TEMPO_MOVES) >= 4:
        tags.append("status control")
    type_counts = Counter(type_name for mon in mons for type_name in set(mon["types"]))
    if type_counts:
        dominant_type, count = type_counts.most_common(1)[0]
        if count >= max(3, (len(mons) + 1) // 2):
            tags.append(f"{dominant_type.removeprefix('TYPE_').title()} pressure")
    spread_moves = {
        move for move in moves
        if "MOVE_TARGET_BOTH" in move_data.get(move, {}).get("target", "")
        or "MOVE_TARGET_FOES_AND_ALLY" in move_data.get(move, {}).get("target", "")
        or "MOVE_TARGET_ALL_BATTLERS" in move_data.get(move, {}).get("target", "")
    }
    if spread_moves and "MOVE_WIDE_GUARD" in moves:
        tags.append("spread + Wide Guard")
    return sorted(set(tags))


def mon_item_problems(mon: dict, move_data: dict[str, dict]) -> list[str]:
    problems = []
    item = mon["item"]
    if item == "ITEM_ASSAULT_VEST" and mon["status_moves"]:
        problems.append("Assault Vest with status move")
    if item in CHOICE_ITEMS and any(move in PROTECT_MOVES for move in mon["moves"]):
        problems.append("Choice item with protection")
    if item in CHOICE_ITEMS and any(move in SETUP_MOVES for move in mon["moves"]):
        problems.append("Choice item with setup move")
    if item == "ITEM_LIFE_ORB" and not mon["attack_moves"]:
        problems.append("Life Orb without attack")
    if len(mon["moves"]) != len(set(mon["moves"])):
        problems.append("duplicate move")
    unknown = [move for move in mon["moves"] if move not in move_data]
    if unknown:
        problems.append(f"unknown move: {', '.join(unknown)}")
    return problems


def audit() -> dict:
    formats = json.loads(FORMATS_PATH.read_text())
    trainers_text = TRAINERS_PATH.read_text()
    parties_text = PARTIES_PATH.read_text()
    trainer_blocks = doubles.trainer_blocks(trainers_text)
    move_data = parse_moves()
    species_data = parse_species()
    mega_stones = mega_items()
    boss_ids = {boss["trainer_id"] for boss in formats["bosses"]}
    records = []

    for trainer_id, rule in formats["formats"].items():
        block = trainer_blocks[trainer_id].group(0)
        party_name = doubles.party_name(block)
        body = doubles.party_match(parties_text, party_name).group(2)
        mons = [parse_mon(entry, move_data, species_data) for entry in custom.party_entries(body)]
        tags = synergy_tags(mons, move_data)
        all_moves = {move for mon in mons for move in mon["moves"]}
        spread_count = sum(
            "MOVE_TARGET_BOTH" in move_data.get(move, {}).get("target", "")
            or "MOVE_TARGET_FOES_AND_ALLY" in move_data.get(move, {}).get("target", "")
            or "MOVE_TARGET_ALL_BATTLERS" in move_data.get(move, {}).get("target", "")
            for move in all_moves
        )
        speed_count = sum(move in SPEED_MOVES or move_data.get(move, {}).get("priority", 0) > 0 for move in all_moves)
        protect_count = sum(move in PROTECT_MOVES for move in all_moves)
        defensive_tempo_count = sum(move in DEFENSIVE_TEMPO_MOVES for move in all_moves)
        defensive_tempo_count += sum(mon["ability"] == "ABILITY_INTIMIDATE" for mon in mons)
        if any(tag.endswith(" engine") for tag in tags):
            speed_count += 1
        item_coverage = sum(mon["item"] != "ITEM_NONE" for mon in mons) / max(1, len(mons))
        complete_coverage = sum(has_complete_moveset(mon) for mon in mons) / max(1, len(mons))
        rare_count = sum(
            any(family in mon["species"] for family in custom.LEGENDARY_FAMILIES)
            for mon in mons
        )
        mega_count = sum(mon["item"] in mega_stones for mon in mons)
        avg_bst = round(sum(mon["bst"] for mon in mons) / max(1, len(mons)), 1)
        avg_offset = round(sum(mon["level_offset"] for mon in mons) / max(1, len(mons)), 2)
        issues = []

        incomplete = [mon["species"] for mon in mons if not has_complete_moveset(mon)]
        if incomplete:
            issues.append({"severity": "blocking", "kind": "incomplete moves", "detail": incomplete})
        invalid_offsets = [mon["level_offset"] for mon in mons if abs(mon["level_offset"]) > 10]
        if invalid_offsets and trainer_id not in INTENTIONAL_LEVEL_ONE:
            issues.append({"severity": "blocking", "kind": "invalid level semantics", "detail": invalid_offsets})
        for mon in mons:
            for problem in mon_item_problems(mon, move_data):
                issues.append({"severity": "blocking", "kind": "item or move incompatibility", "detail": f"{mon['species']}: {problem}"})
        if trainer_id in boss_ids and item_coverage < 1:
            issues.append({"severity": "major", "kind": "boss item gap", "detail": item_coverage})
        elif rule["difficulty"] >= 60 and item_coverage < 1:
            issues.append({"severity": "major", "kind": "serious item gap", "detail": item_coverage})
        if rule["format"] == "double" and rule["difficulty"] >= 50:
            if speed_count == 0:
                issues.append({"severity": "major", "kind": "no speed control", "detail": None})
            if defensive_tempo_count == 0:
                issues.append({"severity": "major", "kind": "no defensive tempo layer", "detail": None})
            if spread_count == 0:
                issues.append({"severity": "major", "kind": "no spread pressure", "detail": None})
            if not tags:
                issues.append({"severity": "major", "kind": "no detected team interaction", "detail": None})

        quality = 20
        quality += min(14, len(mons) * 2)
        quality += round(item_coverage * 10)
        quality += round(complete_coverage * 12)
        quality += min(12, max(0, round((avg_bst - 350) / 20)))
        quality += min(15, len(tags) * 4)
        quality += min(5, speed_count * 2)
        quality += min(5, protect_count)
        quality += min(5, spread_count * 2)
        quality += min(5, rare_count + mega_count * 2)
        quality += max(-8, min(8, round(avg_offset * 3)))
        quality = max(0, min(100, quality))

        records.append({
            "trainer_id": trainer_id,
            "name": trainer_name(block),
            "location": rule["location"],
            "format": rule["format"],
            "manifest_difficulty": rule["difficulty"],
            "party_name": party_name,
            "party_size": len(mons),
            "avg_level_offset": avg_offset,
            "max_level_offset": max(mon["level_offset"] for mon in mons),
            "item_coverage": round(item_coverage, 3),
            "complete_moveset_coverage": round(complete_coverage, 3),
            "avg_bst": avg_bst,
            "rare_count": rare_count,
            "mega_count": mega_count,
            "speed_control_count": speed_count,
            "protect_count": protect_count,
            "defensive_tempo_count": defensive_tempo_count,
            "spread_count": spread_count,
            "synergy_tags": tags,
            "quality_score": quality,
            "issues": issues,
            "mons": mons,
        })

    composition_groups = defaultdict(list)
    for record in records:
        composition = tuple(sorted(mon["species"] for mon in record["mons"]))
        composition_groups[composition].append(record)
    repeated_rematches = []
    for composition, group in composition_groups.items():
        families = defaultdict(list)
        for record in group:
            families[custom.trainer_family(record["trainer_id"])].append(record["trainer_id"])
        for family, trainer_ids in families.items():
            if len(trainer_ids) >= 3 and len(composition) > 1:
                repeated_rematches.append({"trainer_family": family, "count": len(trainer_ids), "trainer_ids": trainer_ids, "composition": composition})

    location_showcases = defaultdict(lambda: {"trainers": 0, "showcases": 0})
    for record in records:
        location_showcases[record["location"]]["trainers"] += 1
        if record["rare_count"] or record["mega_count"] or any(mon["bst"] >= 570 for mon in record["mons"]):
            location_showcases[record["location"]]["showcases"] += 1
    uncovered_locations = [
        {"location": location, **counts}
        for location, counts in location_showcases.items()
        if location != "Unmapped" and counts["trainers"] >= 3 and counts["showcases"] == 0
    ]

    issue_counts = Counter(issue["kind"] for record in records for issue in record["issues"])
    severity_counts = Counter(issue["severity"] for record in records for issue in record["issues"])
    return {
        "summary": {
            "trainer_records": len(records),
            "trainer_pokemon": sum(len(record["mons"]) for record in records),
            "doubles": sum(record["format"] == "double" for record in records),
            "singles": sum(record["format"] == "single" for record in records),
            "median_quality_score": sorted(record["quality_score"] for record in records)[len(records) // 2],
            "blocking_issues": severity_counts["blocking"],
            "major_issues": severity_counts["major"],
            "issue_counts": dict(sorted(issue_counts.items())),
            "repeated_rematch_compositions": len(repeated_rematches),
            "uncovered_multi_trainer_locations": len(uncovered_locations),
        },
        "repeated_rematches": repeated_rematches,
        "uncovered_locations": sorted(uncovered_locations, key=lambda row: (-row["trainers"], row["location"])),
        "teams": records,
    }


def markdown(report: dict) -> str:
    summary = report["summary"]
    findings = [row for row in report["teams"] if row["issues"]]
    weak = sorted(
        findings or report["teams"],
        key=lambda row: (-sum(issue["severity"] == "blocking" for issue in row["issues"]), row["quality_score"], -row["manifest_difficulty"]),
    )
    priority_title = "Highest-priority teams" if findings else "Lowest-scoring pacing battles (validated)"
    lines = [
        "# Verdant trainer-team quality audit",
        "",
        "This report evaluates the current source as the battle engine executes it. Scores are triage signals, not simulated win rates.",
        "",
        "## Roster summary",
        "",
        f"- {summary['trainer_records']} trainer records and {summary['trainer_pokemon']} party slots",
        f"- {summary['doubles']} doubles and {summary['singles']} intentional singles",
        f"- Median quality score: {summary['median_quality_score']}/100",
        f"- Blocking findings: {summary['blocking_issues']}",
        f"- Major design findings: {summary['major_issues']}",
        f"- Repeated three-plus rematch compositions: {summary['repeated_rematch_compositions']}",
        f"- Multi-trainer locations without a showcase opponent: {summary['uncovered_multi_trainer_locations']}",
        "",
        "## Finding counts",
        "",
        "| Finding | Count |",
        "| --- | ---: |",
    ]
    lines.extend(f"| {kind} | {count} |" for kind, count in summary["issue_counts"].items())
    lines.extend([
        "",
        f"## {priority_title}",
        "",
        *( [] if findings else ["All blocking and major findings are clear. These are the lowest-scoring intentional pacing fights, not unresolved failures.", ""] ),
        "| Trainer | Location | Format | Score | Offset | Theme | Findings |",
        "| --- | --- | --- | ---: | ---: | --- | --- |",
    ])
    for row in weak[:100]:
        findings = "; ".join(sorted({issue["kind"] for issue in row["issues"]})) or "none"
        theme = ", ".join(row["synergy_tags"]) or "singles pacing and coverage"
        lines.append(
            f"| {row['trainer_id']} | {row['location']} | {row['format']} | {row['quality_score']} | "
            f"{row['avg_level_offset']:+} | {theme} | {findings} |"
        )
    lines.extend(["", "## Repeated rematch rosters", ""])
    if report["repeated_rematches"]:
        for row in report["repeated_rematches"]:
            lines.append(f"- `{row['trainer_family']}` repeats one exact roster {row['count']} times: {', '.join(row['trainer_ids'])}")
    else:
        lines.append("- None.")
    lines.extend(["", "## Locations needing a showcase", ""])
    if report["uncovered_locations"]:
        for row in report["uncovered_locations"]:
            lines.append(f"- {row['location']}: {row['trainers']} trainer records, no legendary/Mega/570+ BST centerpiece")
    else:
        lines.append("- None.")
    return "\n".join(lines) + "\n"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write-report", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    report = audit()
    if args.write_report:
        REPORT_JSON_PATH.write_text(json.dumps(report, indent=2) + "\n")
        REPORT_MD_PATH.write_text(markdown(report))
    print(json.dumps(report["summary"], indent=2))
    if args.check:
        blockers = report["summary"]["blocking_issues"]
        if blockers:
            raise SystemExit(f"FAIL: {blockers} blocking trainer-team finding(s)")
        print("PASS: all trainer parties satisfy engine-level quality invariants")


if __name__ == "__main__":
    main()
