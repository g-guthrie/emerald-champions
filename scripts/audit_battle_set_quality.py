#!/usr/bin/env python3
"""Audit the competitive quality of every Emerald Champions battle set.

The existing gate (verify_emerald_champions_battle_sets.py) proves a set is
*legal and coherent*: the item has a trigger, the Ability can activate, the
nature does not lower the only attack stat it uses.  It does not ask whether
the set is any *good*.  This audit does: it compares each configuration
against the species' full legal pool and its own base stats, and reports the
places where a strictly better choice was available.

Usage:
    audit_battle_set_quality.py --start 1 --count 100
    audit_battle_set_quality.py --all --summary
"""

from __future__ import annotations

import argparse
import json
import re
from collections import Counter, defaultdict
from pathlib import Path

from generate_emerald_champions_battle_sets import (
    move_metadata,
    national_species_order,
    species_build_metadata,
)

ROOT = Path(__file__).resolve().parents[1]
SETS = ROOT / "data/emerald_champions/emerald_champions_battle_sets.json"
LEARNABLES = ROOT / "src" / "data" / "pokemon" / "all_learnables.json"

STAT_NAMES = ("HP", "Atk", "Def", "SpA", "SpD", "Spe")
BUDGET = 66
PER_STAT_CAP = 32

NATURE_EFFECT = {
    "NATURE_LONELY": (1, 2), "NATURE_BRAVE": (1, 5), "NATURE_ADAMANT": (1, 3),
    "NATURE_NAUGHTY": (1, 4), "NATURE_BOLD": (2, 1), "NATURE_RELAXED": (2, 5),
    "NATURE_IMPISH": (2, 3), "NATURE_LAX": (2, 4), "NATURE_TIMID": (5, 1),
    "NATURE_HASTY": (5, 2), "NATURE_JOLLY": (5, 3), "NATURE_NAIVE": (5, 4),
    "NATURE_MODEST": (3, 1), "NATURE_MILD": (3, 2), "NATURE_QUIET": (3, 5),
    "NATURE_RASH": (3, 4), "NATURE_CALM": (4, 1), "NATURE_GENTLE": (4, 2),
    "NATURE_SASSY": (4, 5), "NATURE_CAREFUL": (4, 3),
}

# Damaging moves whose slot is not justified by raw power, so a
# higher-power same-type move does not dominate them.
UTILITY_ATTACKS = {
    "MOVE_ACID_SPRAY", "MOVE_BODY_PRESS", "MOVE_CLEAR_SMOG", "MOVE_COUNTER",
    "MOVE_ELECTROWEB", "MOVE_ENDEAVOR", "MOVE_FAKE_OUT", "MOVE_FEINT",
    "MOVE_FINAL_GAMBIT", "MOVE_FOUL_PLAY", "MOVE_ICY_WIND", "MOVE_METAL_BURST",
    "MOVE_MIRROR_COAT", "MOVE_NIGHT_SHADE", "MOVE_NUZZLE", "MOVE_RAPID_SPIN",
    "MOVE_MORTAL_SPIN", "MOVE_RUINATION", "MOVE_SALT_CURE", "MOVE_SEISMIC_TOSS",
    "MOVE_SNARL", "MOVE_STRUGGLE_BUG", "MOVE_SUPER_FANG", "MOVE_KNOCK_OFF",
    "MOVE_U_TURN", "MOVE_VOLT_SWITCH", "MOVE_FLIP_TURN", "MOVE_PARTING_SHOT",
    "MOVE_SCALD", "MOVE_LAVA_PLUME", "MOVE_WILL_O_WISP", "MOVE_INFERNAL_PARADE",
    "MOVE_TRIPLE_AXEL", "MOVE_POPULATION_BOMB", "MOVE_DRAGON_DARTS",
    "MOVE_BOUNCE", "MOVE_SKY_DROP", "MOVE_RELIC_SONG", "MOVE_FREEZE_DRY",
    "MOVE_BOLT_BEAK", "MOVE_FISHIOUS_REND", "MOVE_GRASS_KNOT", "MOVE_LOW_KICK",
    "MOVE_HEAT_CRASH", "MOVE_HEAVY_SLAM", "MOVE_ELECTRO_BALL", "MOVE_GYRO_BALL",
    "MOVE_PURSUIT", "MOVE_DRAIN_PUNCH", "MOVE_GIGA_DRAIN", "MOVE_HORN_LEECH",
    "MOVE_LEECH_LIFE", "MOVE_DRAINING_KISS", "MOVE_PARABOLIC_CHARGE",
    "MOVE_MATCHA_GOTCHA", "MOVE_BITTER_BLADE", "MOVE_STRENGTH_SAP",
}

# Moves whose drawback means they must not be recommended as a blind upgrade.
DRAWBACK_ATTACKS = {
    "MOVE_HYPER_BEAM", "MOVE_GIGA_IMPACT", "MOVE_BLAST_BURN", "MOVE_HYDRO_CANNON",
    "MOVE_FRENZY_PLANT", "MOVE_ROCK_WRECKER", "MOVE_ROAR_OF_TIME",
    "MOVE_PRISMATIC_LASER", "MOVE_ETERNABEAM", "MOVE_METEOR_ASSAULT",
    "MOVE_SOLAR_BEAM", "MOVE_SOLAR_BLADE", "MOVE_SKY_ATTACK", "MOVE_METEOR_BEAM",
    "MOVE_ELECTRO_SHOT", "MOVE_SKULL_BASH", "MOVE_FREEZE_SHOCK", "MOVE_ICE_BURN",
    "MOVE_RAZOR_WIND", "MOVE_DIG", "MOVE_DIVE", "MOVE_FLY", "MOVE_PHANTOM_FORCE",
    "MOVE_SHADOW_FORCE", "MOVE_GEOMANCY", "MOVE_EXPLOSION", "MOVE_SELF_DESTRUCT",
    "MOVE_MISTY_EXPLOSION", "MOVE_MEMENTO", "MOVE_HEALING_WISH", "MOVE_LUNAR_DANCE",
    "MOVE_FINAL_GAMBIT", "MOVE_LAST_RESORT", "MOVE_SYNCHRONOISE", "MOVE_BIDE",
    "MOVE_FUTURE_SIGHT", "MOVE_DOOM_DESIRE", "MOVE_FOCUS_PUNCH", "MOVE_BEAK_BLAST",
    "MOVE_SHELL_TRAP", "MOVE_STEEL_BEAM", "MOVE_MIND_BLOWN", "MOVE_CHLOROBLAST",
    "MOVE_SUPERCELL_SLAM", "MOVE_HIGH_JUMP_KICK", "MOVE_JUMP_KICK",
    "MOVE_DYNAMIC_PUNCH", "MOVE_INFERNO", "MOVE_ZAP_CANNON", "MOVE_SHEER_COLD",
    "MOVE_FISSURE", "MOVE_GUILLOTINE", "MOVE_HORN_DRILL", "MOVE_NATURAL_GIFT",
    "MOVE_RETURN", "MOVE_FRUSTRATION", "MOVE_HIDDEN_POWER", "MOVE_MIRROR_MOVE",
    "MOVE_ASSIST", "MOVE_METRONOME", "MOVE_SLEEP_TALK", "MOVE_SNORE",
    "MOVE_DREAM_EATER", "MOVE_FALSE_SWIPE", "MOVE_HOLD_BACK", "MOVE_STOMPING_TANTRUM",
    "MOVE_UPPER_HAND", "MOVE_BURN_UP", "MOVE_DOUBLE_SHOCK", "MOVE_MAGIC_POWDER",
}

# Self-stat-dropping moves: legitimate, but not an unambiguous upgrade.
SELF_LOWERING = {
    "MOVE_OVERHEAT", "MOVE_DRACO_METEOR", "MOVE_LEAF_STORM", "MOVE_PSYCHO_BOOST",
    "MOVE_MAKE_IT_RAIN", "MOVE_CLOSE_COMBAT", "MOVE_SUPERPOWER", "MOVE_V_CREATE",
    "MOVE_HAMMER_ARM", "MOVE_ICE_HAMMER", "MOVE_HEADLONG_RUSH", "MOVE_ARMOR_CANNON",
    "MOVE_FLEUR_CANNON", "MOVE_SPIN_OUT", "MOVE_LAST_RESPECTS",
}

RECOIL = {
    "MOVE_DOUBLE_EDGE", "MOVE_FLARE_BLITZ", "MOVE_BRAVE_BIRD", "MOVE_WOOD_HAMMER",
    "MOVE_HEAD_SMASH", "MOVE_VOLT_TACKLE", "MOVE_WILD_CHARGE", "MOVE_TAKE_DOWN",
    "MOVE_SUBMISSION", "MOVE_WAVE_CRASH", "MOVE_LIGHT_OF_RUIN", "MOVE_STRUGGLE",
}

SPREAD_TARGETS = {"MOVE_TARGET_BOTH", "MOVE_TARGET_FOES_AND_ALLY", "TARGET_BOTH",
                  "TARGET_FOES_AND_ALLY"}

PROTECT_LIKE = {"MOVE_PROTECT", "MOVE_DETECT", "MOVE_SPIKY_SHIELD",
                "MOVE_KINGS_SHIELD", "MOVE_BANEFUL_BUNKER", "MOVE_OBSTRUCT",
                "MOVE_SILK_TRAP", "MOVE_BURNING_BULWARK", "MOVE_MAX_GUARD"}

LOCKED_ITEMS = {"ITEM_CHOICE_BAND", "ITEM_CHOICE_SCARF", "ITEM_CHOICE_SPECS",
                "ITEM_ASSAULT_VEST"}

# Items that only pay off on a Pokemon that is not fully evolved.
NFE_ONLY_ITEMS = {"ITEM_EVIOLITE"}

SETUP_MOVES = {
    "MOVE_SWORDS_DANCE", "MOVE_NASTY_PLOT", "MOVE_DRAGON_DANCE", "MOVE_QUIVER_DANCE",
    "MOVE_SHELL_SMASH", "MOVE_CALM_MIND", "MOVE_BULK_UP", "MOVE_COIL",
    "MOVE_HONE_CLAWS", "MOVE_SHIFT_GEAR", "MOVE_TAIL_GLOW", "MOVE_VICTORY_DANCE",
    "MOVE_TIDY_UP", "MOVE_CURSE", "MOVE_AGILITY", "MOVE_ROCK_POLISH",
    "MOVE_AUTOTOMIZE", "MOVE_IRON_DEFENSE", "MOVE_AMNESIA", "MOVE_BARRIER",
    "MOVE_ACID_ARMOR", "MOVE_COSMIC_POWER", "MOVE_GROWTH", "MOVE_WORK_UP",
    "MOVE_HOWL", "MOVE_MEDITATE", "MOVE_SHARPEN", "MOVE_DEFENSE_CURL",
    "MOVE_HARDEN", "MOVE_WITHDRAW", "MOVE_STOCKPILE", "MOVE_MINIMIZE",
    "MOVE_DOUBLE_TEAM", "MOVE_SHELTER", "MOVE_TAKE_HEART", "MOVE_CLANGOROUS_SOUL",
    "MOVE_NO_RETREAT", "MOVE_BELLY_DRUM", "MOVE_FILLET_AWAY",
}

# Status moves with no competitive payoff worth a slot.
DEAD_STATUS = {
    "MOVE_SPLASH", "MOVE_CELEBRATE", "MOVE_HOLD_HANDS", "MOVE_HAPPY_HOUR",
    "MOVE_TEETER_DANCE", "MOVE_CONVERSION", "MOVE_CONVERSION_2", "MOVE_SHARPEN",
    "MOVE_HARDEN", "MOVE_WITHDRAW", "MOVE_DEFENSE_CURL", "MOVE_GROWL",
    "MOVE_LEER", "MOVE_TAIL_WHIP", "MOVE_STRING_SHOT", "MOVE_SMOKESCREEN",
    "MOVE_SAND_ATTACK", "MOVE_FLASH", "MOVE_KINESIS", "MOVE_LOCK_ON",
    "MOVE_MIND_READER", "MOVE_FORESIGHT", "MOVE_ODOR_SLEUTH", "MOVE_MIRROR_MOVE",
    "MOVE_ASSIST", "MOVE_METRONOME", "MOVE_SKETCH", "MOVE_ATTRACT",
    "MOVE_CAPTIVATE", "MOVE_CONFIDE", "MOVE_GRUDGE", "MOVE_SPITE",
    "MOVE_CHARM_", "MOVE_HOWL", "MOVE_MEDITATE", "MOVE_FOCUS_ENERGY",
    "MOVE_SUBMISSION", "MOVE_BIDE", "MOVE_RAGE", "MOVE_ROTOTILLER",
    "MOVE_MAGNETIC_FLUX", "MOVE_GEAR_UP", "MOVE_FLOWER_SHIELD", "MOVE_BESTOW",
    "MOVE_RECYCLE", "MOVE_NATURAL_GIFT", "MOVE_CAMOUFLAGE", "MOVE_MIMIC",
    "MOVE_PSYCH_UP", "MOVE_TELEKINESIS", "MOVE_MAGIC_POWDER", "MOVE_PURIFY",
    "MOVE_LUCKY_CHANT", "MOVE_SAFEGUARD", "MOVE_MIST", "MOVE_REFRESH",
    "MOVE_HEAL_BELL", "MOVE_AROMATHERAPY", "MOVE_MUD_SPORT", "MOVE_WATER_SPORT",
    "MOVE_ELECTRIFY", "MOVE_SOAK", "MOVE_TRICK_OR_TREAT", "MOVE_FORESTS_CURSE",
    "MOVE_POWDER", "MOVE_GASTRO_ACID", "MOVE_WORRY_SEED", "MOVE_SIMPLE_BEAM",
    "MOVE_ENTRAINMENT", "MOVE_ROLE_PLAY", "MOVE_SKILL_SWAP", "MOVE_SPEED_SWAP",
    "MOVE_POWER_SWAP", "MOVE_GUARD_SWAP", "MOVE_HEART_SWAP", "MOVE_POWER_TRICK",
    "MOVE_POWER_SPLIT", "MOVE_GUARD_SPLIT", "MOVE_PAIN_SPLIT", "MOVE_GRAVITY",
    "MOVE_INGRAIN", "MOVE_AQUA_RING", "MOVE_MAGNET_RISE", "MOVE_CHARGE",
    "MOVE_SNATCH", "MOVE_FOLLOW_ME_", "MOVE_IMPRISON", "MOVE_MAGIC_ROOM",
    "MOVE_WONDER_ROOM", "MOVE_ION_DELUGE", "MOVE_HAPPY_HOUR_",
}
# The generic-status entries above are only a *report* signal, never a
# correctness claim; several are perfectly good in the right role.
SOFT_DEAD_STATUS = DEAD_STATUS - {
    "MOVE_HEAL_BELL", "MOVE_AROMATHERAPY", "MOVE_SAFEGUARD", "MOVE_PAIN_SPLIT",
    "MOVE_INGRAIN", "MOVE_AQUA_RING", "MOVE_MAGNET_RISE", "MOVE_GRAVITY",
    "MOVE_SKILL_SWAP", "MOVE_GASTRO_ACID", "MOVE_IMPRISON", "MOVE_SOAK",
}

HARD_DEAD_STATUS = {
    "MOVE_SPLASH", "MOVE_CELEBRATE", "MOVE_HAPPY_HOUR", "MOVE_HOLD_HANDS",
    "MOVE_GROWL", "MOVE_LEER", "MOVE_TAIL_WHIP", "MOVE_STRING_SHOT",
    "MOVE_SMOKESCREEN", "MOVE_SAND_ATTACK", "MOVE_FLASH", "MOVE_KINESIS",
    "MOVE_HARDEN", "MOVE_WITHDRAW", "MOVE_DEFENSE_CURL", "MOVE_SHARPEN",
    "MOVE_MEDITATE", "MOVE_FORESIGHT", "MOVE_ODOR_SLEUTH", "MOVE_CONFIDE",
    "MOVE_CONVERSION", "MOVE_CAMOUFLAGE", "MOVE_MIMIC", "MOVE_RAGE",
    "MOVE_BIDE", "MOVE_LUCKY_CHANT", "MOVE_MUD_SPORT", "MOVE_WATER_SPORT",
    "MOVE_ATTRACT", "MOVE_CAPTIVATE", "MOVE_ROTOTILLER", "MOVE_FLOWER_SHIELD",
    "MOVE_TELEKINESIS", "MOVE_BESTOW", "MOVE_LOCK_ON", "MOVE_MIND_READER",
    "MOVE_DOUBLE_TEAM", "MOVE_MINIMIZE", "MOVE_MIRROR_MOVE", "MOVE_METRONOME",
    "MOVE_ASSIST", "MOVE_NATURAL_GIFT", "MOVE_RECYCLE", "MOVE_SPITE",
    "MOVE_GRUDGE", "MOVE_POWER_TRICK", "MOVE_POWER_SPLIT", "MOVE_GUARD_SPLIT",
    "MOVE_HEART_SWAP", "MOVE_POWER_SWAP", "MOVE_GUARD_SWAP", "MOVE_SPEED_SWAP",
    "MOVE_ELECTRIFY", "MOVE_ION_DELUGE", "MOVE_MAGNETIC_FLUX", "MOVE_GEAR_UP",
    "MOVE_PURIFY", "MOVE_REFRESH", "MOVE_PSYCH_UP", "MOVE_SNATCH",
}


def load_sets() -> dict:
    return json.loads(SETS.read_text())


def learnable_pool(species: str, meta: dict, learnables: dict) -> set[str]:
    info = meta.get(species, {})
    keys = {species.removeprefix("SPECIES_"), info.get("learnset_key", "")}
    pool: set[str] = set()
    for key in keys:
        if key:
            pool.update(learnables.get(key, []))
    return pool


def is_spread(move: str, moves: dict) -> bool:
    return moves.get(move, {}).get("target", "") in SPREAD_TARGETS


def score(move: str, moves: dict, doubles: bool) -> float:
    info = moves.get(move)
    if not info or info["category"] == "DAMAGE_CATEGORY_STATUS":
        return 0.0
    power = info["power"]
    acc = min(info["accuracy"], 100) / 100.0
    mult = 1.5 if (doubles and is_spread(move, moves)) else 1.0
    return power * acc * mult


def offense_stats(species: str, meta: dict) -> tuple[int, int]:
    info = meta.get(species, {})
    return info.get("attack", 0), info.get("sp_attack", 0)


def audit_set(entry: dict, bucket: str, doubles: bool, meta: dict,
              moves: dict, learnables: dict) -> list[tuple[str, str]]:
    """Return (severity, message) findings for one set."""
    species = entry["species"]
    info = meta.get(species, {})
    out: list[tuple[str, str]] = []
    mv = entry["moves"]
    pool = learnable_pool(species, meta, learnables) | set(mv)
    types = info.get("types", ())
    pts = entry["stat_points"]
    nature = entry["nature"]
    item = entry["item"]

    dmg = [m for m in mv if moves.get(m, {}).get("category",
           "DAMAGE_CATEGORY_STATUS") != "DAMAGE_CATEGORY_STATUS"]
    phys = [m for m in dmg if moves[m]["category"] == "DAMAGE_CATEGORY_PHYSICAL"
            and m not in UTILITY_ATTACKS]
    spec = [m for m in dmg if moves[m]["category"] == "DAMAGE_CATEGORY_SPECIAL"
            and m not in UTILITY_ATTACKS]

    # --- Stat points -----------------------------------------------------
    total = sum(pts)
    if total != BUDGET:
        out.append(("HIGH", f"stat points total {total}, not {BUDGET}"))
    if any(p > PER_STAT_CAP for p in pts):
        out.append(("HIGH", f"a stat exceeds the {PER_STAT_CAP} cap: {pts}"))

    atk_base, spa_base = offense_stats(species, meta)
    if pts[1] > 0 and not phys and not any(
            m in {"MOVE_BODY_PRESS", "MOVE_FOUL_PLAY"} for m in mv):
        if not any(moves.get(m, {}).get("category") == "DAMAGE_CATEGORY_PHYSICAL"
                   for m in mv):
            out.append(("HIGH", f"{pts[1]} points in Atk with no physical move"))
    if pts[3] > 0 and not spec:
        if not any(moves.get(m, {}).get("category") == "DAMAGE_CATEGORY_SPECIAL"
                   for m in mv):
            out.append(("HIGH", f"{pts[3]} points in SpA with no special move"))

    # Nature vs. where the points actually went.
    if nature in NATURE_EFFECT:
        up, down = NATURE_EFFECT[nature]
        if pts[up] == 0 and up in (1, 3, 5):
            out.append(("MED", f"{nature} raises {STAT_NAMES[up]} but 0 points are in it"))
        if pts[down] == PER_STAT_CAP:
            out.append(("MED", f"{nature} lowers {STAT_NAMES[down]} yet it is maxed"))

    # --- Offensive identity ---------------------------------------------
    if phys and spec and pts[1] and pts[3]:
        out.append(("MED", "splits Stat Points across both attack stats"))
    if phys and not spec and spa_base > atk_base + 20:
        out.append(("MED", f"physical set on a special species (Atk {atk_base} / SpA {spa_base})"))
    if spec and not phys and atk_base > spa_base + 20:
        out.append(("MED", f"special set on a physical species (Atk {atk_base} / SpA {spa_base})"))

    # --- STAB ------------------------------------------------------------
    if dmg:
        has_stab = any(moves[m]["type"] in types for m in dmg)
        if not has_stab:
            best = None
            for cand in pool:
                ci = moves.get(cand)
                if not ci or ci["type"] not in types:
                    continue
                if ci["category"] == "DAMAGE_CATEGORY_STATUS":
                    continue
                want = "DAMAGE_CATEGORY_PHYSICAL" if (phys and not spec) else \
                       "DAMAGE_CATEGORY_SPECIAL" if (spec and not phys) else None
                if want and ci["category"] != want:
                    continue
                if cand in DRAWBACK_ATTACKS:
                    continue
                s = score(cand, moves, doubles)
                if best is None or s > best[1]:
                    best = (cand, s)
            if best and best[1] > 0:
                out.append(("HIGH", f"no STAB attack; {best[0]} is legal"))
            else:
                out.append(("MED", "no STAB attack"))

    # --- Redundant coverage ---------------------------------------------
    seen: dict[tuple[str, str], list[str]] = defaultdict(list)
    for m in dmg:
        if m in UTILITY_ATTACKS:
            continue
        seen[(moves[m]["type"], moves[m]["category"])].append(m)
    for (ty, cat), group in seen.items():
        if len(group) > 1:
            spreads = [g for g in group if is_spread(g, moves)]
            # In doubles a spread + single-target pair of the same type is a
            # deliberate choice; in singles it is a wasted slot.
            if doubles and spreads and len(spreads) < len(group):
                continue
            out.append(("MED", f"two {ty.removeprefix('TYPE_').title()} "
                               f"{cat.removeprefix('DAMAGE_CATEGORY_').lower()} "
                               f"moves: {', '.join(g.removeprefix('MOVE_') for g in group)}"))

    # --- Dominated moves -------------------------------------------------
    for m in dmg:
        if m in UTILITY_ATTACKS or m in DRAWBACK_ATTACKS or m in RECOIL:
            continue
        mi = moves[m]
        mine = score(m, moves, doubles)
        if mine <= 0:
            continue
        better = []
        for cand in pool:
            if cand in mv:
                continue
            ci = moves.get(cand)
            if not ci or ci["type"] != mi["type"] or ci["category"] != mi["category"]:
                continue
            if cand in DRAWBACK_ATTACKS or cand in SELF_LOWERING or cand in RECOIL:
                continue
            if ci["priority"] < mi["priority"]:
                continue
            if doubles and is_spread(m, moves) and not is_spread(cand, moves):
                continue
            if score(cand, moves, doubles) >= mine + 20:
                better.append((score(cand, moves, doubles), cand))
        if better:
            better.sort(reverse=True)
            out.append(("HIGH", f"{m.removeprefix('MOVE_')} is dominated by "
                                f"{better[0][1].removeprefix('MOVE_')} "
                                f"({int(mine)} -> {int(better[0][0])} eff. power)"))

    # --- Dead slots ------------------------------------------------------
    for m in mv:
        if m in HARD_DEAD_STATUS:
            out.append(("HIGH", f"{m.removeprefix('MOVE_')} does nothing competitively"))

    # --- Items -----------------------------------------------------------
    if item in NFE_ONLY_ITEMS and not info.get("evolves", False):
        out.append(("HIGH", f"{item.removeprefix('ITEM_')} on a fully evolved species"))
    if item in LOCKED_ITEMS and any(m in PROTECT_LIKE for m in mv):
        out.append(("HIGH", f"{item.removeprefix('ITEM_')} cannot select Protect"))
    if item == "ITEM_CHOICE_SPECS" and phys and not spec:
        out.append(("HIGH", "Choice Specs on a physical set"))
    if item == "ITEM_CHOICE_BAND" and spec and not phys:
        out.append(("HIGH", "Choice Band on a special set"))
    if item == "ITEM_ASSAULT_VEST" and not dmg:
        out.append(("HIGH", "Assault Vest with no damaging move"))
    if item == "ITEM_LIFE_ORB" and not dmg:
        out.append(("MED", "Life Orb with no damaging move"))

    # --- Setup coherence -------------------------------------------------
    setups = [m for m in mv if m in SETUP_MOVES]
    if len(setups) > 1:
        out.append(("MED", f"two setup moves: {', '.join(s.removeprefix('MOVE_') for s in setups)}"))
    if setups and not dmg:
        out.append(("HIGH", f"{setups[0].removeprefix('MOVE_')} with no attack to use it"))

    return out


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--start", type=int, default=1)
    ap.add_argument("--count", type=int, default=100)
    ap.add_argument("--all", action="store_true")
    ap.add_argument("--summary", action="store_true")
    ap.add_argument("--severity", default="HIGH,MED")
    ap.add_argument("--species", default="")
    args = ap.parse_args()

    want_sev = set(args.severity.split(","))
    data = load_sets()
    meta = species_build_metadata()
    moves = move_metadata()
    learnables = json.loads(LEARNABLES.read_text())

    buckets = (
        ("doubles-default", data["defaults"], True),
        ("doubles-alt", data["alternatives"], True),
        ("singles-default", data["singles_defaults"], False),
        ("singles-alt", data["singles_alternatives"], False),
    )

    by_species: dict[str, list[tuple[str, dict, bool]]] = defaultdict(list)
    for label, entries, doubles in buckets:
        for entry in entries:
            by_species[entry["species"]].append((label, entry, doubles))

    order = [s for s in national_species_order() if s in by_species]
    seen = set(order)
    order += [s for s in by_species if s not in seen]

    if args.species:
        wanted = set(args.species.split(","))
        window = [s for s in order if s in wanted or
                  s.removeprefix("SPECIES_") in wanted]
    elif args.all:
        window = order
    else:
        window = order[args.start - 1: args.start - 1 + args.count]

    tally: Counter = Counter()
    flagged = 0
    for idx, species in enumerate(order):
        if species not in window:
            continue
        lines = []
        for label, entry, doubles in by_species[species]:
            findings = [f for f in audit_set(entry, label, doubles, meta, moves, learnables)
                        if f[0] in want_sev]
            for sev, msg in findings:
                tally[msg.split(":")[0].split(" is ")[0][:40]] += 1
                tally["__" + sev] += 1
            if findings:
                lines.append((label, entry, findings))
        if lines:
            flagged += 1
            if not args.summary:
                print(f"\n#{idx + 1} {species.removeprefix('SPECIES_')}")
                for label, entry, findings in lines:
                    print(f"  [{label}] {entry['name']} — "
                          f"{'/'.join(m.removeprefix('MOVE_') for m in entry['moves'])} "
                          f"| {entry['nature'].removeprefix('NATURE_')} "
                          f"| {entry['ability'].removeprefix('ABILITY_')} "
                          f"| {entry['item'].removeprefix('ITEM_')} "
                          f"| {entry['stat_points']}")
                    for sev, msg in findings:
                        print(f"      {sev:4} {msg}")

    print(f"\nspecies in window: {len(window)}; species with findings: {flagged}")
    print(f"HIGH={tally['__HIGH']} MED={tally['__MED']}")
    if args.summary:
        for msg, n in tally.most_common(40):
            if msg.startswith("__"):
                continue
            print(f"  {n:5}  {msg}")


if __name__ == "__main__":
    main()
