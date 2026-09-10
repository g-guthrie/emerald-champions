#!/usr/bin/env python3
"""Scratch evidence inventory for the canonical Emerald Champions preset EV catalog.

This is deliberately read-only.  It prints one JSON document to stdout and imports
the production generator so mechanical aliases and configured species stats are
resolved by the same current code path as materialization.
"""

from __future__ import annotations

import collections
import hashlib
import json
import re
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))

from generate_emerald_champions_battle_sets import (  # noqa: E402
    HAND_AUDITED_SOURCE,
    load_hand_audited_catalog,
    load_hand_audited_source,
    species_build_metadata,
)

STAT_NAMES = ("hp", "attack", "defense", "sp_attack", "sp_defense", "speed")
SLOW_NATURES = {"NATURE_BRAVE", "NATURE_RELAXED", "NATURE_QUIET", "NATURE_SASSY"}

# These effects do damage without the user's Attack or Sp. Atk in the ordinary
# damage formula.  EFFECT_FOUL_PLAY and EFFECT_BODY_PRESS are reported separately.
NON_OFFENSIVE_STAT_EFFECTS = {
    "EFFECT_BIDE",
    "EFFECT_OHKO",
    "EFFECT_FIXED_PERCENT_DAMAGE",
    "EFFECT_FIXED_HP_DAMAGE",
    "EFFECT_LEVEL_DAMAGE",
    "EFFECT_PSYWAVE",
    "EFFECT_REFLECT_DAMAGE",
    "EFFECT_ENDEAVOR",
    "EFFECT_FINAL_GAMBIT",
}
DYNAMIC_BOTH_EFFECTS = {
    "EFFECT_PHOTON_GEYSER",
    "EFFECT_SHELL_SIDE_ARM",
    "EFFECT_TERA_BLAST",
    "EFFECT_TERA_STARSTORM",
}

# Only held items in the catalog whose value/activation materially depends on an
# HP threshold or integer healing breakpoint.  Resist/status berries are omitted.
HP_THRESHOLD_ITEMS = {
    "ITEM_BERRY_JUICE",
    "ITEM_ORAN_BERRY",
    "ITEM_SITRUS_BERRY",
    "ITEM_FIGY_BERRY",
    "ITEM_WIKI_BERRY",
    "ITEM_MAGO_BERRY",
    "ITEM_AGUAV_BERRY",
    "ITEM_IAPAPA_BERRY",
    "ITEM_LIECHI_BERRY",
    "ITEM_GANLON_BERRY",
    "ITEM_SALAC_BERRY",
    "ITEM_PETAYA_BERRY",
    "ITEM_APICOT_BERRY",
    "ITEM_LANSAT_BERRY",
    "ITEM_STARF_BERRY",
    "ITEM_MICLE_BERRY",
    "ITEM_CUSTAP_BERRY",
}

def configured_moves() -> dict[str, dict[str, str]]:
    """Preprocess the current move table with the repository's live config."""
    text = subprocess.check_output(
        [
            "cc", "-E", "-P", "-DTRUE=1", "-DFALSE=0",
            "-I", str(ROOT / "include"), "-I", str(ROOT),
            "-include", "global.h",
            str(ROOT / "src/data/moves_info.h"),
        ],
        text=True,
    )
    markers = list(re.finditer(r"^\s*\[(MOVE_[A-Z0-9_]+)\]\s*=", text, re.MULTILINE))
    result: dict[str, dict[str, str]] = {}
    for index, marker in enumerate(markers):
        end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
        block = text[marker.start():end]
        category = re.search(r"\.category\s*=\s*([^,]+)", block)
        effect = re.search(r"\.effect\s*=\s*([^,]+)", block)
        if category and effect:
            def resolve(expression: str, prefix: str) -> str:
                expression = expression.strip()
                if re.fullmatch(rf"{prefix}[A-Z0-9_]+", expression):
                    return expression
                ternary = re.fullmatch(
                    rf"([0-9 +<>=!&|()]+)\?\s*({prefix}[A-Z0-9_]+)\s*:\s*({prefix}[A-Z0-9_]+)",
                    expression,
                )
                assert ternary, (marker.group(1), expression)
                condition = ternary.group(1).strip()
                assert re.fullmatch(r"[0-9 +<>=!&|()]+", condition), condition
                return ternary.group(2) if eval(condition, {"__builtins__": {}}, {}) else ternary.group(3)

            result[marker.group(1)] = {
                "category": resolve(category.group(1), "DAMAGE_CATEGORY_"),
                "effect": resolve(effect.group(1), "EFFECT_"),
            }
    aliases = re.findall(
        r"^\s*(MOVE_[A-Z0-9_]+)\s*=\s*(MOVE_[A-Z0-9_]+)\s*,",
        (ROOT / "include/constants/moves.h").read_text(),
        re.MULTILINE,
    )
    for alias, target in aliases:
        if target in result:
            result[alias] = result[target]
    return result


def configured_hp_items() -> dict[str, dict[str, str | int]]:
    """Read current hold effects, parameters, and pockets after preprocessing."""
    text = subprocess.check_output(
        [
            "cc", "-E", "-P", "-DTRUE=1", "-DFALSE=0",
            "-I", str(ROOT / "include"), "-I", str(ROOT),
            "-include", "global.h",
            str(ROOT / "src/data/items.h"),
        ],
        text=True,
    )
    markers = list(re.finditer(r"^\s*\[(ITEM_[A-Z0-9_]+)\]\s*=", text, re.MULTILINE))
    result: dict[str, dict[str, str | int]] = {}
    for index, marker in enumerate(markers):
        item = marker.group(1)
        if item not in HP_THRESHOLD_ITEMS:
            continue
        end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
        block = text[marker.start():end]
        effect = re.search(r"\.holdEffect\s*=\s*(HOLD_EFFECT_[A-Z0-9_]+)", block)
        param = re.search(r"\.holdEffectParam\s*=\s*([0-9]+)", block)
        pocket = re.search(r"\.pocket\s*=\s*(POCKET_[A-Z0-9_]+)", block)
        assert effect and param and pocket, item
        result[item] = {
            "hold_effect": effect.group(1),
            "hold_effect_param": int(param.group(1)),
            "pocket": pocket.group(1),
        }
    assert set(result) == HP_THRESHOLD_ITEMS, sorted(HP_THRESHOLD_ITEMS - set(result))
    return result


def configured_confuse_berry_activation_fraction() -> int:
    """Resolve the live B_CONFUSE_BERRIES_HEAL branch used by HealConfuseBerry."""
    result = subprocess.check_output(
        [
            "cc", "-E", "-P", "-DTRUE=1", "-DFALSE=0",
            "-I", str(ROOT / "include"), "-I", str(ROOT),
            "-include", "global.h", "-",
        ],
        input=(
            "#if B_CONFUSE_BERRIES_HEAL >= GEN_7\n"
            "CODEX_CONFUSE_ACTIVATION_FRACTION_4\n"
            "#else\n"
            "CODEX_CONFUSE_ACTIVATION_FRACTION_2\n"
            "#endif\n"
        ),
        text=True,
    )
    match = re.search(r"CODEX_CONFUSE_ACTIVATION_FRACTION_([24])", result)
    assert match, "could not resolve B_CONFUSE_BERRIES_HEAL"
    return int(match.group(1))


def hp_item_activation_fraction(
    item_meta: dict[str, str | int], confuse_berry_activation_fraction: int,
) -> int:
    hold_effect = str(item_meta["hold_effect"])
    if hold_effect in {"HOLD_EFFECT_RESTORE_HP", "HOLD_EFFECT_RESTORE_PCT_HP"}:
        return 2
    if hold_effect == "HOLD_EFFECT_CONFUSE_FLAVOR":
        return confuse_berry_activation_fraction
    return int(item_meta["hold_effect_param"])


def hp_item_healing_formula(item_meta: dict[str, str | int]) -> str:
    hold_effect = str(item_meta["hold_effect"])
    hold_param = int(item_meta["hold_effect_param"])
    if hold_effect == "HOLD_EFFECT_RESTORE_HP":
        return f"fixed {hold_param} HP"
    if hold_effect == "HOLD_EFFECT_RESTORE_PCT_HP":
        return f"floor(maxHP * {hold_param} / 100)"
    if hold_effect == "HOLD_EFFECT_CONFUSE_FLAVOR":
        return f"floor(maxHP / {hold_param})"
    return "no direct HP healing"


def configured_natures() -> dict[str, tuple[str, str]]:
    """Read nature modifiers from the current native gNaturesInfo table."""
    text = (ROOT / "src/pokemon.c").read_text()
    start = text.index("const struct NatureInfo gNaturesInfo")
    end = text.index("\n};", start) + 3
    text = text[start:end]
    markers = list(re.finditer(r"^\s*\[(NATURE_[A-Z]+)\]\s*=", text, re.MULTILINE))
    result: dict[str, tuple[str, str]] = {}
    for index, marker in enumerate(markers):
        block_end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
        block = text[marker.start():block_end]
        up = re.search(r"\.statUp\s*=\s*(STAT_[A-Z]+)", block)
        down = re.search(r"\.statDown\s*=\s*(STAT_[A-Z]+)", block)
        assert up and down, marker.group(1)
        result[marker.group(1)] = (up.group(1), down.group(1))
    return result


def level_stats(
    base: dict[str, int], evs: list[int], nature: str,
    natures: dict[str, tuple[str, str]], level: int,
) -> dict[str, int]:
    """Gen 9 stat formula benchmark with effective IV 31."""
    if base["hp"] == 1:  # current P_BASE_HP_1_SHEDINJA_HANDLING behavior
        hp = 1
    else:
        hp = ((2 * base["hp"] + 31 + evs[0] // 4) * level) // 100 + level + 10
    values = {"hp": hp}
    up, down = natures[nature]
    native_stats = ("STAT_ATK", "STAT_DEF", "STAT_SPATK", "STAT_SPDEF", "STAT_SPEED")
    for offset, (key, native) in enumerate(zip(STAT_NAMES[1:], native_stats, strict=True), 1):
        raw = ((2 * base[key] + 31 + evs[offset] // 4) * level) // 100 + 5
        if up != down and native == up:
            raw = raw * 110 // 100
        elif up != down and native == down:
            raw = raw * 90 // 100
        values[key] = raw
    return values


def move_axes(move: str, info: dict[str, dict[str, str]]) -> tuple[set[str], str]:
    data = info[move]
    effect = data["effect"]
    category = data["category"]
    if effect == "EFFECT_BODY_PRESS":
        return {"defense"}, "user Defense"
    if effect == "EFFECT_FOUL_PLAY":
        return {"target_offense"}, "target Attack/Sp. Atk by runtime category"
    if effect in DYNAMIC_BOTH_EFFECTS:
        return {"attack", "sp_attack"}, "native dynamic physical/special category"
    if effect in NON_OFFENSIVE_STAT_EFFECTS:
        return set(), "fixed/reflected/level/HP damage; no user offensive stat"
    if category == "DAMAGE_CATEGORY_PHYSICAL":
        return {"attack"}, "user Attack"
    if category == "DAMAGE_CATEGORY_SPECIAL":
        return {"sp_attack"}, "user Sp. Atk"
    if category == "DAMAGE_CATEGORY_STATUS":
        return set(), "status"
    raise AssertionError((move, data))


def main() -> None:
    raw = load_hand_audited_source()
    catalog = load_hand_audited_catalog()
    species_meta = species_build_metadata()
    moves_meta = configured_moves()
    hp_items_meta = configured_hp_items()
    confuse_berry_activation_fraction = configured_confuse_berry_activation_fraction()
    natures = configured_natures()

    raw_aliases = {
        species: review["inherits_species"]
        for species, review in raw["species"].items()
        if "inherits_species" in review
    }
    entries = []
    flag_names = (
        "slow_nature_plus_speed_ev",
        "trick_room_plus_speed_ev",
        "physical_only_plus_sp_attack_ev",
        "special_only_plus_attack_ev",
        "unused_effective_ev_budget",
        "shedinja_hp_or_defensive_bulk_ev",
        "hp_item_breakpoint_review",
        "hp_item_with_substitute_or_belly_drum",
    )
    flags: dict[str, list[dict]] = collections.defaultdict(list)
    for flag_name in flag_names:
        flags[flag_name] = []
    spread_counts: collections.Counter[tuple[int, ...]] = collections.Counter()
    effective_spread_counts: collections.Counter[tuple[int, ...]] = collections.Counter()
    total_counts: collections.Counter[int] = collections.Counter()
    nature_counts: collections.Counter[str] = collections.Counter()
    format_counts: collections.Counter[str] = collections.Counter()
    item_counts: collections.Counter[str] = collections.Counter()
    move_effect_counts: collections.Counter[str] = collections.Counter()
    global_index = 0

    for species_order_index, species in enumerate(catalog["species_order"]):
        review = catalog["species"][species]
        assert species in species_meta and species_meta[species]["hp"] > 0, species
        base = {key: species_meta[species][key] for key in STAT_NAMES}
        for format_name in ("doubles", "singles"):
            for format_index, preset in enumerate(review[format_name]):
                assert preset["species"] == species
                identity = {
                    "canonical_index": global_index,
                    "species_order_index": species_order_index,
                    "species": species,
                    "format": format_name,
                    "format_index": format_index,
                    "name": preset["name"],
                    "stable_key": f"{species}|{format_name}|{format_index}|{preset['name']}",
                }
                global_index += 1
                evs = preset["evs"]
                total = sum(evs)
                effective = [4 * (value // 4) for value in evs]
                axes: set[str] = set()
                analyzed_moves = []
                for move in preset["moves"]:
                    assert move in moves_meta, move
                    move_axis, explanation = move_axes(move, moves_meta)
                    axes.update(move_axis)
                    move_effect_counts[moves_meta[move]["effect"]] += 1
                    analyzed_moves.append({"move": move, **moves_meta[move], "stat_basis": explanation})
                own_offense = axes & {"attack", "sp_attack"}
                benchmark_stats = {
                    str(level): level_stats(base, evs, preset["nature"], natures, level)
                    for level in (10, 12, 14, 50, 100)
                }
                stats50 = benchmark_stats["50"]
                stats100 = benchmark_stats["100"]
                evidence = {
                    **identity,
                    "alias_parent": raw_aliases.get(species),
                    "evs": evs,
                    "effective_evs": effective,
                    "total_evs": total,
                    "unused_raw_budget_to_510": 510 - total,
                    "unused_effective_budget_to_508": 508 - sum(effective),
                    "nature": preset["nature"],
                    "ability": preset["ability"],
                    "item": preset["item"],
                    "required_item": preset["required_item"],
                    "required_move": preset.get("required_move", "MOVE_NONE"),
                    "field_dependency": preset.get("field_dependency"),
                    "moves": analyzed_moves,
                    "own_offensive_axes": sorted(own_offense),
                    "all_damage_stat_axes": sorted(axes),
                    "base_stats": base,
                    "benchmark_stats_iv31": benchmark_stats,
                    "level50_stats_iv31": stats50,
                    "level100_stats_iv31": stats100,
                    "role": preset.get("role"),
                    "source": preset.get("source"),
                    "references": review.get("references", []),
                }
                entries.append(evidence)
                spread_counts[tuple(evs)] += 1
                effective_spread_counts[tuple(effective)] += 1
                total_counts[total] += 1
                nature_counts[preset["nature"]] += 1
                format_counts[format_name] += 1
                item_counts[preset["item"]] += 1

                if preset["nature"] in SLOW_NATURES and evs[5] > 0:
                    flags["slow_nature_plus_speed_ev"].append(evidence)
                if "MOVE_TRICK_ROOM" in preset["moves"] and evs[5] > 0:
                    flags["trick_room_plus_speed_ev"].append(evidence)
                if own_offense == {"attack"} and evs[3] > 0:
                    flags["physical_only_plus_sp_attack_ev"].append(evidence)
                if own_offense == {"sp_attack"} and evs[1] > 0:
                    flags["special_only_plus_attack_ev"].append(evidence)
                if 508 - sum(effective) >= 4:
                    flags["unused_effective_ev_budget"].append(evidence)
                if base["hp"] == 1 and (evs[0] > 0 or evs[2] > 0 or evs[4] > 0):
                    flags["shedinja_hp_or_defensive_bulk_ev"].append(evidence)
                if preset["item"] in HP_THRESHOLD_ITEMS:
                    hp_item = dict(evidence)
                    item_meta = hp_items_meta[preset["item"]]
                    hold_effect = str(item_meta["hold_effect"])
                    hold_param = int(item_meta["hold_effect_param"])
                    pocket = str(item_meta["pocket"])
                    activation_fraction = hp_item_activation_fraction(
                        item_meta, confuse_berry_activation_fraction
                    )

                    holder_has_gluttony = preset["ability"] == "ABILITY_GLUTTONY"
                    holder_has_ripen = preset["ability"] == "ABILITY_RIPEN"
                    gluttony_eligible = (
                        holder_has_gluttony
                        and pocket == "POCKET_BERRIES"
                        and activation_fraction <= 4
                    )
                    ripen_multiplier = 2 if holder_has_ripen and pocket == "POCKET_BERRIES" else 1
                    has_substitute = "MOVE_SUBSTITUTE" in preset["moves"]
                    has_belly_drum = "MOVE_BELLY_DRUM" in preset["moves"]

                    def base_heal_amount(level_hp: int) -> int:
                        if hold_effect == "HOLD_EFFECT_RESTORE_HP":
                            return hold_param
                        if hold_effect == "HOLD_EFFECT_RESTORE_PCT_HP":
                            return level_hp * hold_param // 100
                        if hold_effect == "HOLD_EFFECT_CONFUSE_FLAVOR":
                            return level_hp // hold_param
                        return 0

                    def breakpoint(level_hp: int) -> dict:
                        def activation_threshold(fraction: int) -> int:
                            if level_hp <= 1:
                                return 0
                            return min(level_hp - 1, level_hp // fraction + 1)

                        ordinary_threshold = activation_threshold(activation_fraction)
                        adjusted_fraction = (
                            min(activation_fraction, 2)
                            if gluttony_eligible
                            else activation_fraction
                        )
                        threshold = activation_threshold(adjusted_fraction)
                        heal_before_cap = base_heal_amount(level_hp) * ripen_multiplier

                        def item_event(hp_before: int, hp_cost: int) -> dict:
                            can_pay = hp_before > hp_cost
                            hp_after_cost = hp_before - hp_cost if can_pay else hp_before
                            threshold_reached = can_pay and hp_after_cost <= threshold
                            actual_heal = (
                                min(heal_before_cap, level_hp - hp_after_cost)
                                if threshold_reached
                                else 0
                            )
                            return {
                                "hp_before": hp_before,
                                "hp_cost": hp_cost,
                                "can_pay_hp_cost": can_pay,
                                "hp_after_cost": hp_after_cost,
                                "threshold_reached_with_item_available": threshold_reached,
                                "modeled_item_consumed_assuming_other_runtime_gates_pass": threshold_reached,
                                "configured_heal_before_hp_cap": heal_before_cap if threshold_reached else 0,
                                "modeled_actual_heal_after_hp_cap": actual_heal,
                                "hp_after_modeled_item": hp_after_cost + actual_heal,
                            }

                        result = {
                            "hp": level_hp,
                            "hp_mod2": level_hp % 2,
                            "hp_mod3": level_hp % 3,
                            "hp_mod4": level_hp % 4,
                            "ordinary_activation_threshold": ordinary_threshold,
                            "ability_adjusted_activation_threshold": threshold,
                            "gluttony_changes_threshold": threshold != ordinary_threshold,
                            "configured_base_heal_amount": base_heal_amount(level_hp),
                            "holder_adjusted_heal_before_hp_cap": heal_before_cap,
                        }
                        if has_substitute:
                            substitute_cost = level_hp // 4
                            hp = level_hp
                            item_available = True
                            sequence = []
                            for substitute_number in range(1, 4):
                                if item_available:
                                    event = item_event(hp, substitute_cost)
                                    item_available = not event[
                                        "modeled_item_consumed_assuming_other_runtime_gates_pass"
                                    ]
                                else:
                                    can_pay = hp > substitute_cost
                                    hp_after_cost = hp - substitute_cost if can_pay else hp
                                    event = {
                                        "hp_before": hp,
                                        "hp_cost": substitute_cost,
                                        "can_pay_hp_cost": can_pay,
                                        "hp_after_cost": hp_after_cost,
                                        "threshold_reached_with_item_available": False,
                                        "modeled_item_consumed_assuming_other_runtime_gates_pass": False,
                                        "configured_heal_before_hp_cap": 0,
                                        "modeled_actual_heal_after_hp_cap": 0,
                                        "hp_after_modeled_item": hp_after_cost,
                                    }
                                event["substitute_number"] = substitute_number
                                event["item_available_after_event"] = item_available
                                sequence.append(event)
                                hp = event["hp_after_modeled_item"]
                            result["substitute_sequence_assuming_other_runtime_gates_pass"] = sequence
                        if has_belly_drum:
                            result["belly_drum_event_assuming_other_runtime_gates_pass"] = item_event(
                                level_hp, level_hp // 2
                            )
                        return result

                    hp_item["hp_breakpoints"] = {
                        "item_metadata_from_preprocessed_table": item_meta,
                        "activation_fraction": activation_fraction,
                        "healing_formula": hp_item_healing_formula(item_meta),
                        "holder_has_gluttony": holder_has_gluttony,
                        "gluttony_eligible_by_native_pocket_and_fraction_gate": gluttony_eligible,
                        "holder_has_ripen": holder_has_ripen,
                        "ripen_heal_multiplier": ripen_multiplier,
                        "has_substitute": has_substitute,
                        "has_belly_drum": has_belly_drum,
                        "levels": {
                            level: breakpoint(benchmark_stats[level]["hp"])
                            for level in ("10", "12", "14", "50", "100")
                        },
                    }
                    flags["hp_item_breakpoint_review"].append(hp_item)
                    if has_substitute or has_belly_drum:
                        flags["hp_item_with_substitute_or_belly_drum"].append(hp_item)

    assert len(entries) == 6923, len(entries)
    assert sum(format_counts.values()) == len(entries)
    duplicate_stable_keys = [
        key for key, count in collections.Counter(e["stable_key"] for e in entries).items()
        if count != 1
    ]
    assert not duplicate_stable_keys, duplicate_stable_keys

    output = {
        "scope": {
            "canonical_source": str(HAND_AUDITED_SOURCE.relative_to(ROOT)),
            "canonical_source_sha256": hashlib.sha256(HAND_AUDITED_SOURCE.read_bytes()).hexdigest(),
            "canonical_source_schema": raw["schema_version"],
            "resolved_set_count": len(entries),
            "species_count": len(catalog["species_order"]),
            "mechanical_alias_species_count": len(raw_aliases),
            "format_counts": dict(sorted(format_counts.items())),
            "universal_effective_iv": 31,
            "ev_policy": "252 maximum per stat, 510 raw total; stats use floor(EV/4)",
            "hp_activation_threshold_policy": (
                "min(maxHP - 1, floor(maxHP / fraction) + 1); Gluttony changes the "
                "fraction of eligible Berries to min(fraction, 2)"
            ),
            "configured_hp_threshold_items": {
                item: {
                    **metadata,
                    "activation_fraction": hp_item_activation_fraction(
                        metadata, confuse_berry_activation_fraction
                    ),
                    "healing_formula": hp_item_healing_formula(metadata),
                }
                for item, metadata in sorted(hp_items_meta.items())
            },
            "sources": [
                "scripts/generate_emerald_champions_battle_sets.py: load_hand_audited_catalog/species_build_metadata",
                "current preprocessed src/data/moves_info.h under live include/config",
                "current preprocessed src/data/items.h under live include/config",
                "src/battle_hold_effects.c: ItemHealHp/HealConfuseBerry/stat-like item dispatch",
                "src/battle_util.c: GetBerryActivationThreshold/HasEnoughHpToEatBerry",
                "src/battle_util.c: SetDynamicMoveCategory and CalcAttackStat",
                "src/pokemon.c: gNaturesInfo and base-HP-1 handling",
                "include/constants/pokemon_stats.h: EV limits",
            ],
            "method_limits": [
                "Flags identify candidates for human judgment; no flag is asserted to be a bug.",
                "Shared presets have no level, so HP-item evidence gives level-10/12/14/50/100 benchmarks; other campaign-level breakpoints must be recomputed at the actual encounter level.",
                "HP-item breakpoint fields are source arithmetic from current preprocessed item metadata and native threshold formulas; they are not native battle execution proof.",
                "Substitute/Belly Drum item sequences assume every non-HP activation gate passes and the item is consumed once without Harvest, Recycle, Pickup, or another restoration path. Battle state such as Heal Block, Unnerve, stat caps, transformed HP, or move success can change actual behavior.",
                "The offensive-axis test follows native category/effect behavior but does not infer strategic utility from support moves, abilities, partner plans, transformations, or matchup-specific damage.",
                "Unused effective budget means at least one additional 4-EV stat point fits below the 508 effective-EV ceiling; it does not imply that spending it changes a meaningful benchmark.",
            ],
        },
        "distributions": {
            "exact_ev_spreads": [
                {"evs": list(spread), "count": count}
                for spread, count in sorted(spread_counts.items(), key=lambda row: (-row[1], row[0]))
            ],
            "effective_ev_spreads": [
                {"evs": list(spread), "count": count}
                for spread, count in sorted(effective_spread_counts.items(), key=lambda row: (-row[1], row[0]))
            ],
            "total_ev_counts": {str(key): value for key, value in sorted(total_counts.items())},
            "nature_counts": dict(sorted(nature_counts.items())),
            "item_counts": dict(sorted(item_counts.items())),
            "move_effect_use_counts": dict(sorted(move_effect_counts.items())),
        },
        "flag_counts": {key: len(value) for key, value in sorted(flags.items())},
        "flags": {key: value for key, value in sorted(flags.items())},
        "presets": entries,
    }
    json.dump(output, sys.stdout, indent=2, ensure_ascii=False)
    sys.stdout.write("\n")


if __name__ == "__main__":
    main()
