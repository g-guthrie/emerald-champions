#!/usr/bin/env python3
"""Per-encounter quality audit for every campaign battle.

Walks all 513 encounters and all 561 trainer branches in the battle master and
checks each team against rules that describe a *stupid* team rather than a
merely easy one. Difficulty is meant to come from levels and team quality, so
none of these rules care how strong a team is; they care whether the team can
actually do the thing it was built to do.

Rule groups
-----------
ability      an Ability whose trigger the moveset never supplies
             (Iron Fist with no punches, Technician with nothing it boosts)
item         a held item the set cannot use (Assault Vest with a status move,
             Choice lock on a set that needs to change moves, Eviolite on a
             final evolution)
weather      a weather- or terrain-dependent Ability/move with no source of it
             on the team
support      redirection or setup that has nothing to protect or set up for
composition  duplicate species, no damaging move, no way to touch a common
             immunity
stats        Stat Points spent on a stat the set never uses, or a Nature that
             lowers the only attacking stat
ai           a trainer whose AI flags sit below the competent floor

Every finding names the encounter, the trainer and the Pokemon, so each one can
be judged individually. Run with --list to print every branch it inspected.
"""
from __future__ import annotations

import argparse
import collections
import glob
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MASTER = ROOT / "docs" / "emerald_champions_master_battle_design.txt"
PARTY = ROOT / "src" / "data" / "trainers.party"

MON_RE = re.compile(
    r"(?m)^  \d+\. (SPECIES_[A-Z0-9_]+) @ (ITEM_[A-Z0-9_]+) \| "
    r"level_offset=(-?\d+) \| ability=(ABILITY_[A-Z0-9_]+) \| "
    r"nature=(NATURE_[A-Z0-9_]+) \| stat_points=([0-9/]+) \| moves=([A-Z0-9_,]+)$"
)

FINDINGS: list[tuple[str, str, str]] = []


def finding(group: str, where: str, message: str) -> None:
    FINDINGS.append((group, where, message))


# ---------------------------------------------------------------- move data


def move_table() -> dict[str, dict]:
    text = (ROOT / "src" / "data" / "moves_info.h").read_text(errors="ignore")
    moves: dict[str, dict] = {}
    for block in re.finditer(r"\[(MOVE_[A-Z0-9_]+)\] =\s*\{(.*?)\n    \},", text, re.S):
        name, body = block.group(1), block.group(2)
        def flag(field: str) -> bool:
            return f".{field} = TRUE" in body
        power = re.search(r"\.power = ([^,\n]+)", body)
        effect = re.search(r"\.effect = (EFFECT_[A-Z0-9_]+)", body)
        mtype = re.search(r"\.type = (TYPE_[A-Z0-9_]+)", body)
        cat = re.search(r"\.category = (DAMAGE_CATEGORY_[A-Z]+)", body)
        prio = re.search(r"\.priority = (-?\d+)", body)
        raw_power = power.group(1).strip() if power else "0"
        numeric = re.findall(r"\d+", raw_power)
        moves[name] = {
            "power": int(numeric[-1]) if numeric and "GEN_" not in raw_power else (int(numeric[0]) if numeric else 0),
            "effect": effect.group(1) if effect else "EFFECT_HIT",
            "type": mtype.group(1) if mtype else "TYPE_NORMAL",
            "category": cat.group(1) if cat else "DAMAGE_CATEGORY_STATUS",
            "priority": int(prio.group(1)) if prio else 0,
            "punch": flag("punchingMove"),
            "bite": flag("bitingMove"),
            "sound": flag("soundMove"),
            "pulse": flag("pulseMove"),
            "slice": flag("slicingMove"),
            "wind": flag("windMove"),
            "dance": flag("danceMove"),
            "contact": flag("makesContact"),
            "body": body,
        }
    return moves


def species_table() -> tuple[dict[str, dict], set[str]]:
    """Parse species info. Many form families (Floette, Deoxys, Oricorio...) build
    their entry from a `#define FOO_INFO(form, FORM, ...)` macro, so `.evolutions`,
    `.types` and `.abilities` live in the macro body, not the species block. Resolve
    macros before deciding anything, or every macro-built species looks like a
    typeless Pokemon that cannot evolve."""
    info: dict[str, dict] = {}
    evolves: set[str] = set()
    for path in glob.glob(str(ROOT / "src/data/pokemon/species_info/gen_*_families.h")):
        text = Path(path).read_text(errors="ignore")
        type_macros: dict[str, tuple] = {}
        for tm in re.finditer(r"(?m)^\s*#define ([A-Z0-9_]*TYPES) \{([^}]*)\}", text):
            key = tm.group(1)
            value = tuple(x.strip() for x in tm.group(2).split(",") if x.strip())
            # first definition wins: the modern `#if P_UPDATED_TYPES >= GEN_6` branch
            # is always written before the legacy `#else` branch.
            type_macros.setdefault(key, value)
        macros: dict[str, str] = {}
        for mac in re.finditer(r"(?m)^#define ([A-Z0-9_]+)\(", text):
            start = mac.end()
            body, depth = [], 0
            i = start
            while i < len(text):
                ch = text[i]
                if ch == "\n" and not body[-1:] == ["\\"]:
                    if not text[i - 1] == "\\":
                        break
                body.append(ch)
                i += 1
            macros[mac.group(1)] = "".join(body)
        for block in re.finditer(r"\[(SPECIES_[A-Z0-9_]+)\] =\s*\{(.*?)\n    \},", text, re.S):
            name, body = block.group(1), block.group(2)
            expanded, seen_macros = body, set()
            while True:
                pending = [u for u in re.findall(r"\b([A-Z][A-Z0-9_]*_INFO)\s*\(", expanded)
                           if u not in seen_macros and u in macros]
                if not pending:
                    break
                for used in pending:
                    seen_macros.add(used)
                    expanded += "\n" + macros[used]
            stats = {}
            for stat in ("baseHP", "baseAttack", "baseDefense", "baseSpAttack", "baseSpDefense", "baseSpeed"):
                m = re.search(rf"\.{stat}\s*=\s*(\d+)", expanded)
                stats[stat] = int(m.group(1)) if m else 0
            abil = re.search(r"\.abilities\s*=\s*\{([^}]*)\}", expanded)
            stats["abilities"] = tuple(a.strip() for a in abil.group(1).split(",")) if abil else ()
            types = re.search(r"\.types\s*=\s*MON_TYPES\(([^)]*)\)", expanded)
            if types:
                found = [t.strip() for t in types.group(1).split(",")]
            else:
                # `.types = ROTOM_FAMILY_TYPES` -> object-like macro, sometimes guarded
                # by `#if P_UPDATED_TYPES >= GEN_6`; take the modern branch.
                alias = re.search(r"\.types\s*=\s*([A-Z][A-Z0-9_]*)\s*,", expanded)
                found = list(type_macros.get(alias.group(1), ())) if alias else []
            stats["types"] = tuple(dict.fromkeys(found))
            info[name] = stats
            if ".evolutions" in expanded:
                evolves.add(name)
    # A base name like SPECIES_FLOETTE is the form-0 alias of SPECIES_FLOETTE_RED.
    for name in list(info):
        base = re.sub(r"_(RED|NORMAL|PLANT|WEST|EAST|ORDINARY|BAILE|MIDDAY|AMPED|GREEN|ICE|FULL_BELLY)$", "", name)
        if base != name and base not in info:
            info[base] = info[name]
            if name in evolves:
                evolves.add(base)
    return info, evolves


MOVES = move_table()
SPECIES, CAN_EVOLVE = species_table()


def m(move: str, key: str, default=0):
    return MOVES.get(move, {}).get(key, default)


# ------------------------------------------------------- semantic rule data

# Ability -> predicate over the moveset. A finding means the Ability's trigger
# is never supplied by the set, so the slot is doing nothing.
ABILITY_NEEDS = {
    "ABILITY_IRON_FIST": ("a punching move", lambda mv: any(m(x, "punch") for x in mv)),
    "ABILITY_STRONG_JAW": ("a biting move", lambda mv: any(m(x, "bite") for x in mv)),
    "ABILITY_MEGA_LAUNCHER": ("a pulse move", lambda mv: any(m(x, "pulse") for x in mv)),
    "ABILITY_SHARPNESS": ("a slicing move", lambda mv: any(m(x, "slice") for x in mv)),
    "ABILITY_PUNK_ROCK": ("a sound move", lambda mv: any(m(x, "sound") for x in mv)),
    "ABILITY_LIQUID_VOICE": ("a sound move", lambda mv: any(m(x, "sound") for x in mv)),
    "ABILITY_TECHNICIAN": ("a move of 60 BP or less", lambda mv: any(
        0 < m(x, "power") <= 60 for x in mv)),
    "ABILITY_RECKLESS": ("a recoil or high-jump move", lambda mv: any(
        "recoil" in m(x, "body", "") or m(x, "effect") in ("EFFECT_RECOIL_IF_MISS",) for x in mv)),
    "ABILITY_SHEER_FORCE": ("a move with a secondary effect", lambda mv: any(
        "additionalEffects" in m(x, "body", "") for x in mv)),
    "ABILITY_TRIAGE": ("a healing move", lambda mv: any(
        m(x, "effect") in ("EFFECT_ABSORB", "EFFECT_RESTORE_HP", "EFFECT_MORNING_SUN",
                           "EFFECT_SYNTHESIS", "EFFECT_MOONLIGHT", "EFFECT_SOFT_BOILED",
                           "EFFECT_HEAL_PULSE", "EFFECT_JUNGLE_HEALING", "EFFECT_DRAIN")
        or "DRAIN" in x or x in ("MOVE_FLORAL_HEALING", "MOVE_DRAINING_KISS", "MOVE_GIGA_DRAIN")
        for x in mv)),
    "ABILITY_STAKEOUT": (None, None),
}

# Weather / terrain dependence: (ability or move) -> the source it needs.
WEATHER_SOURCES = {
    "RAIN": {"ABILITY_DRIZZLE"}, "SUN": {"ABILITY_DROUGHT", "ABILITY_ORICHALCUM_PULSE"},
    "SAND": {"ABILITY_SAND_STREAM", "ABILITY_SAND_SPIT"},
    "HAIL": {"ABILITY_SNOW_WARNING"},
}
WEATHER_MOVES = {
    "MOVE_RAIN_DANCE": "RAIN", "MOVE_SUNNY_DAY": "SUN",
    "MOVE_SANDSTORM": "SAND", "MOVE_HAIL": "HAIL", "MOVE_SNOWSCAPE": "HAIL",
    "MOVE_CHILLY_RECEPTION": "HAIL",
}
WEATHER_ABILITY_NEEDS = {
    "ABILITY_SWIFT_SWIM": "RAIN", "ABILITY_RAIN_DISH": "RAIN", "ABILITY_HYDRATION": "RAIN",
    "ABILITY_CHLOROPHYLL": "SUN", "ABILITY_SOLAR_POWER": "SUN", "ABILITY_LEAF_GUARD": "SUN",
    "ABILITY_FLOWER_GIFT": "SUN", 
    "ABILITY_SAND_RUSH": "SAND", "ABILITY_SAND_FORCE": "SAND", "ABILITY_SAND_VEIL": "SAND",
    "ABILITY_SLUSH_RUSH": "HAIL", "ABILITY_SNOW_CLOAK": "HAIL", "ABILITY_ICE_BODY": "HAIL",
}
TERRAIN_SOURCES = {
    "ELECTRIC": {"ABILITY_ELECTRIC_SURGE", "ABILITY_HADRON_ENGINE"},
    "PSYCHIC": {"ABILITY_PSYCHIC_SURGE"},
    "GRASSY": {"ABILITY_GRASSY_SURGE"},
    "MISTY": {"ABILITY_MISTY_SURGE"},
}
TERRAIN_MOVES = {
    "MOVE_ELECTRIC_TERRAIN": "ELECTRIC", "MOVE_PSYCHIC_TERRAIN": "PSYCHIC",
    "MOVE_GRASSY_TERRAIN": "GRASSY", "MOVE_MISTY_TERRAIN": "MISTY",
}
TERRAIN_ABILITY_NEEDS = {"ABILITY_SURGE_SURFER": "ELECTRIC"}
TERRAIN_MOVE_NEEDS = {
    "MOVE_EXPANDING_FORCE": "PSYCHIC", "MOVE_RISING_VOLTAGE": "ELECTRIC",
    "MOVE_GRASSY_GLIDE": "GRASSY", "MOVE_MISTY_EXPLOSION": "MISTY",
    "MOVE_TERRAIN_PULSE": None,
}

CHOICE_ITEMS = {"ITEM_CHOICE_BAND", "ITEM_CHOICE_SPECS", "ITEM_CHOICE_SCARF"}
REDIRECTION = {"MOVE_FOLLOW_ME", "MOVE_RAGE_POWDER"}
SETUP_EFFECTS = {
    "EFFECT_ATTACK_UP_2", "EFFECT_SPECIAL_ATTACK_UP_2", "EFFECT_DRAGON_DANCE",
    "EFFECT_SWORDS_DANCE", "EFFECT_NASTY_PLOT", "EFFECT_CALM_MIND", "EFFECT_BULK_UP",
    "EFFECT_SHELL_SMASH", "EFFECT_BELLY_DRUM", "EFFECT_QUIVER_DANCE", "EFFECT_GEOMANCY",
    "EFFECT_ATTACK_UP", "EFFECT_SPECIAL_ATTACK_UP", "EFFECT_SPEED_UP_2", "EFFECT_COSMIC_POWER",
}

STAT_NAMES = ("HP", "Atk", "Def", "SpA", "SpD", "Spe")
MINUS_ATK = {"NATURE_BOLD", "NATURE_MODEST", "NATURE_CALM", "NATURE_TIMID"}
MINUS_SPA = {"NATURE_ADAMANT", "NATURE_IMPISH", "NATURE_CAREFUL", "NATURE_JOLLY"}


# Belly Drum halves HP for +6 Attack. The classic payments are Sitrus (heals back
# at 50%), a Gluttony pinch Berry (same trick, one turn earlier), Focus Sash on the
# lead, Clear Amulet (protects the +6 from Intimidate), and Ice Face (a free hit).
GLUTTONY_BERRIES_OK = {"ABILITY_GLUTTONY"}
BELLY_DRUM_ITEMS = {"ITEM_SITRUS_BERRY", "ITEM_FOCUS_SASH", "ITEM_CLEAR_AMULET"}
BELLY_DRUM_ABILITIES = {"ABILITY_ICE_FACE", "ABILITY_STURDY", "ABILITY_MULTISCALE"}


def belly_drum_is_supported(species: str, item: str, ability: str) -> bool:
    if item in BELLY_DRUM_ITEMS or ability in BELLY_DRUM_ABILITIES:
        return True
    if ability in GLUTTONY_BERRIES_OK and item.endswith("_BERRY"):
        return True
    return False


def type_chart() -> tuple[dict[tuple[str, str], float], list[str]]:
    """Read gTypeEffectivenessTable straight out of src/data/types_info.h so the
    audit uses the ROM's own matchups, including this build's B_UPDATED_TYPE_MATCHUPS
    resolutions, instead of a hand-copied chart that can drift."""
    text = (ROOT / "src/data/types_info.h").read_text(errors="ignore")
    header = re.search(r"//\s*Attacker\s+(None.*?)\n", text)
    order = ["TYPE_" + n.upper() for n in header.group(1).split()]
    gen_defaults = {"STL_RS": 1.0, "PSN_RS": 0.5, "BUG_RS": 1.0, "PSY_RS": 2.0, "FIR_RS": 0.5}
    chart: dict[tuple[str, str], float] = {}
    for row in re.finditer(r"\[(TYPE_[A-Z]+)\]\s*=\s*\{([^}]*)\},", text):
        attacker, cells = row.group(1), [c.strip() for c in row.group(2).split(",")]
        if len(cells) != len(order):
            continue
        for defender, cell in zip(order, cells):
            if cell == "______":
                value = 1.0
            elif cell.startswith("X("):
                value = float(cell[2:-1])
            else:
                value = gen_defaults.get(cell, 1.0)
            chart[(attacker, defender)] = value
    real = [x for x in order if x not in ("TYPE_NONE", "TYPE_MYSTERY", "TYPE_STELLAR")]
    return chart, real


TYPE_CHART, REAL_TYPES = type_chart()


def best_multiplier(attack_types: set[str], defender: str) -> float:
    return max((TYPE_CHART.get((a, defender), 1.0) for a in attack_types), default=0.0)


def is_setup(move: str) -> bool:
    return m(move, "effect") in SETUP_EFFECTS


def audit_branch(enc: int, trainer: str, location: str, difficulty: float,
                 fmt: str, mons: list[tuple], ai: str, listing: bool,
                 ally_abilities: set | None = None, ally_moves: set | None = None) -> None:
    where = f"encounter {enc:04d} {trainer} @{location}"
    if listing:
        team = ", ".join(s.replace("SPECIES_", "") for s, *_ in mons)
        print(f"{where} [{fmt} d{difficulty}] {team}")

    species_seen = collections.Counter()
    mono_specialists: list[tuple[str, str]] = []
    team_damage_types: set[str] = set()
    team_abilities = {a for _, _, _, a, _, _, _ in mons}
    team_moves = {mv for *_, moves in mons for mv in moves}
    # In a multi battle the two opposing trainers share one field, so a partner's
    # Drought makes this trainer's Chlorophyll live. Judging a branch alone
    # reports every co-ordinated weather formation as a dead ability.
    if fmt == "multi":
        team_abilities |= (ally_abilities or set())
        team_moves |= (ally_moves or set())

    weather_on_team = set()
    for ability in team_abilities:
        for kind, sources in WEATHER_SOURCES.items():
            if ability in sources:
                weather_on_team.add(kind)
    for move in team_moves:
        if move in WEATHER_MOVES:
            weather_on_team.add(WEATHER_MOVES[move])

    terrain_on_team = set()
    for ability in team_abilities:
        for kind, sources in TERRAIN_SOURCES.items():
            if ability in sources:
                terrain_on_team.add(kind)
    for move in team_moves:
        if move in TERRAIN_MOVES:
            terrain_on_team.add(TERRAIN_MOVES[move])

    has_setup_partner = any(any(is_setup(x) for x in moves) for *_, moves in mons)

    for species, item, _off, ability, nature, points, moves in mons:
        tag = f"{where}/{species.replace('SPECIES_', '')}"
        pts = [int(v) for v in points.split("/")]
        species_seen[species] += 1

        FIXED = {"EFFECT_NIGHT_SHADE", "EFFECT_SEISMIC_TOSS", "EFFECT_DRAGON_RAGE",
                 "EFFECT_SONICBOOM", "EFFECT_PSYWAVE", "EFFECT_COUNTER", "EFFECT_MIRROR_COAT",
                 "EFFECT_BIDE", "EFFECT_ENDEAVOR", "EFFECT_SUPER_FANG", "EFFECT_FINAL_GAMBIT",
                 "EFFECT_OHKO", "EFFECT_LEVEL_DAMAGE"}
        damaging = [x for x in moves
                    if m(x, "category") != "DAMAGE_CATEGORY_STATUS" and m(x, "effect") not in FIXED]
        anything_that_damages = [x for x in moves if m(x, "category") != "DAMAGE_CATEGORY_STATUS"]
        team_damage_types |= {m(x, "type") for x in damaging}
        physical = [x for x in damaging if m(x, "category") == "DAMAGE_CATEGORY_PHYSICAL"]
        special = [x for x in damaging if m(x, "category") == "DAMAGE_CATEGORY_SPECIAL"]
        status = [x for x in moves if m(x, "category") == "DAMAGE_CATEGORY_STATUS"]

        # --- ability triggers ---
        legal = SPECIES.get(species, {}).get("abilities", ())
        forced = len([a for a in legal if a != "ABILITY_NONE"]) <= 1

        need = ABILITY_NEEDS.get(ability)
        if need and need[0] and not need[1](moves):
            finding("ability", tag, f"{ability} needs {need[0]}, set has none")

        # --- weather / terrain dependence ---
        want = WEATHER_ABILITY_NEEDS.get(ability)
        if want and want not in weather_on_team and not forced:
            finding("weather", tag, f"{ability} needs {want} and the team never sets it")
        want = TERRAIN_ABILITY_NEEDS.get(ability)
        if want and want not in terrain_on_team:
            finding("weather", tag, f"{ability} needs {want} terrain and the team never sets it")
        for move in moves:
            want = TERRAIN_MOVE_NEEDS.get(move)
            if want and want not in terrain_on_team:
                finding("weather", tag, f"{move} needs {want} terrain and the team never sets it")
        if ability == "ABILITY_HARVEST" and "BERRY" not in item:
            finding("ability", tag, "Harvest with no Berry to recycle")
        if "MOVE_SOLAR_BEAM" in moves and "SUN" not in weather_on_team:
            finding("weather", tag, "Solar Beam without sun is a two-turn move")

        # --- items ---
        if item == "ITEM_ASSAULT_VEST" and status:
            finding("item", tag, f"Assault Vest cannot use {', '.join(status)}")
        stuck = [x for x in status if x not in ("MOVE_TRICK", "MOVE_SWITCHEROO", "MOVE_TRANSFORM")]
        if item in CHOICE_ITEMS and stuck:
            finding("item", tag, f"{item} locks into one move but the set carries {', '.join(stuck)}")
        if item == "ITEM_EVIOLITE" and species not in CAN_EVOLVE:
            finding("item", tag, "Eviolite on a Pokemon that cannot evolve further")
        if item == "ITEM_LIFE_ORB" and not damaging:
            finding("item", tag, "Life Orb on a set with no damaging move")
        if item == "ITEM_BLACK_SLUDGE" and "TYPE_POISON" not in SPECIES.get(species, {}).get("types", ()):
            finding("item", tag, "Black Sludge damages this non-Poison holder every turn")

        # --- support coherence ---
        if any(x in REDIRECTION for x in moves) and fmt == "single":
            finding("support", tag, "redirection move in a single battle does nothing")
        if "MOVE_BELLY_DRUM" in moves and not belly_drum_is_supported(species, item, ability):
            finding("support", tag, f"Belly Drum with {item}, which does not pay for the halved HP")

        # --- composition ---
        residual = {"MOVE_TOXIC", "MOVE_LEECH_SEED", "MOVE_WILL_O_WISP", "MOVE_TOXIC_SPIKES",
                    "MOVE_CURSE", "MOVE_SALT_CURE", "MOVE_INFESTATION"}
        if not anything_that_damages and not (set(moves) & residual) and fmt == "single":
            finding("composition", tag, "no damaging move and no residual damage in a single battle")
        if len({m(x, "type") for x in damaging}) == 1 and len(damaging) >= 3:
            mono_specialists.append((tag, m(damaging[0], "type").replace("TYPE_", "")))

        # --- stat points ---
        if pts[1] > 8 and not physical and special:
            finding("stats", tag, f"{pts[1]} Atk points on a special-only set")
        if pts[3] > 8 and not special and physical:
            finding("stats", tag, f"{pts[3]} Sp. Atk points on a physical-only set")
        if nature in MINUS_ATK and physical and not special:
            finding("stats", tag, f"{nature} lowers Attack on a physical-only set")
        if nature in MINUS_SPA and special and not physical:
            finding("stats", tag, f"{nature} lowers Sp. Atk on a special-only set")

    # A single-type attacker is a legitimate specialist as long as the rest of the
    # team answers what walls it, so judge coverage at the team level using the
    # ROM's own type chart rather than counting move types.
    if team_damage_types:
        dead = [d.replace("TYPE_", "") for d in REAL_TYPES
                if best_multiplier(team_damage_types, d) == 0.0]
        if dead:
            finding("composition", where,
                    f"no move on the team can damage {', '.join(dead)} at all")
        elif mono_specialists:
            walled = [d.replace("TYPE_", "") for d in REAL_TYPES
                      if best_multiplier(team_damage_types, d) < 1.0]
            if walled:
                finding("composition", where,
                        f"every damaging move on the team is resisted by {', '.join(walled)}")

    for species, count in species_seen.items():
        if count > 1:
            finding("composition", where, f"{species.replace('SPECIES_', '')} appears {count} times")

    # --- AI floor ---
    if "Smart Trainer" not in ai:
        for flag in ("Hp Aware", "Smart Mon Choices", "Try To 2HKO"):
            if flag not in ai:
                finding("ai", where, f"AI missing {flag}: {ai}")
                break


def audit_progression(ladders: list[tuple[str, int, int, str, float, int, int]]) -> None:
    """Cross-encounter checks that a single branch cannot see.

    A numbered rematch ladder (TRAINER_X_1 ... TRAINER_X_6) has to climb: the
    Gabby and Ty ladder used to peak at rematch 4 and then drop, so the last
    fight of a six-fight chain was the easiest one in it.

    A branch tagged as a breather has to play like one. A six-Pokemon team, or a
    difficulty rating in ace territory, means the label is wrong, and the label
    decides the AI and the Stat Point budget.
    """
    rungs: dict[str, list[tuple[int, int, int]]] = collections.defaultdict(list)
    for trainer, enc, cap, role, difficulty, top, size in ladders:
        # Numbered Team Magma/Aqua grunts are different people sharing one hideout,
        # not one person fought repeatedly, so their numbering is not a ladder.
        base = re.match(r"(TRAINER_.*)_(\d+)$", trainer)
        if base and "GRUNT" not in base.group(1):
            rungs[base.group(1)].append((int(base.group(2)), enc, top))
        where = f"encounter {enc:04d} {trainer}"
        if role == "ordinary_breather" and difficulty >= 8.0:
            finding("difficulty", where,
                    f"tagged a breather but rated {difficulty}, which is ace territory")
        if role == "ordinary_breather" and size >= 6:
            finding("difficulty", where,
                    f"tagged a breather but fields {size} Pokemon")
    for name, entries in rungs.items():
        entries.sort()
        if len(entries) < 2:
            continue
        for (rung_a, _, top_a), (rung_b, enc_b, top_b) in zip(entries, entries[1:]):
            if top_b < top_a:
                finding("difficulty", f"encounter {enc_b:04d} {name}_{rung_b}",
                        f"rematch {rung_b} tops out at L{top_b}, below rematch "
                        f"{rung_a} at L{top_a}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--list", action="store_true", help="print every branch inspected")
    parser.add_argument("--group", help="only show findings from one rule group")
    parser.add_argument("--master", help="audit a different master file (used by the self-test "
                                         "so it never has to edit the real one)")
    parser.add_argument("--fail-on-findings", action="store_true",
                        help="exit non-zero if anything was found (used as a release gate)")
    args = parser.parse_args()
    master = Path(args.master) if args.master else MASTER

    ladders: list[tuple[str, int, int, str, float, int, int]] = []
    ai_by_trainer: dict[str, str] = {}
    for block in PARTY.read_text().split("=== TRAINER_")[1:]:
        name = "TRAINER_" + block.split(" ===", 1)[0]
        match = re.search(r"(?m)^AI: (.*)$", block)
        ai_by_trainer[name] = match.group(1) if match else ""

    text = master.read_text()
    encounters = re.split(r"(?m)^=== ENCOUNTER (\d{4}) ===$", text)
    branches = 0
    for index in range(1, len(encounters), 2):
        enc = int(encounters[index])
        block = encounters[index + 1]
        location = re.search(r"location: (\S+)", block).group(1)
        difficulty = float(re.search(r"difficulty_target: ([\d.]+)", block).group(1))
        cap_match = re.search(r"strict_cap: (\d+)", block)
        cap = int(cap_match.group(1)) if cap_match else 0
        role_match = re.search(r"fatigue_role: (\S+)", block)
        role = role_match.group(1) if role_match else "?"
        parsed = []
        for chunk in re.split(r"(?m)^--- BRANCH ", block)[1:]:
            trainer = chunk.split(" ---", 1)[0]
            fmt_match = re.search(r"(?m)^format: (\S+)$", chunk)
            fmt = fmt_match.group(1) if fmt_match else "?"
            mons = MON_RE.findall(chunk)
            mons = [(a, b, c, d, e, f, g.split(",")) for a, b, c, d, e, f, g in mons]
            if not mons:
                finding("composition", f"encounter {enc:04d} {trainer}", "no Pokemon parsed")
                continue
            parsed.append((trainer, fmt, mons))
            # MON_RE captures level_offset, not an absolute level; the real level
            # is the encounter's strict_cap plus that offset.
            ladders.append((trainer, enc, cap, role, difficulty,
                            cap + max(int(offset) for _, _, offset, _, _, _, _ in mons),
                            len(mons)))

        # Multi battles put two enemy trainers on one board; each needs the other's
        # abilities and moves before its own weather and terrain can be judged.
        multi_abilities = {a for _, fmt, mons in parsed if fmt == "multi"
                           for _, _, _, a, _, _, _ in mons}
        multi_moves = {mv for _, fmt, mons in parsed if fmt == "multi"
                       for *_, moves in mons for mv in moves}

        for trainer, fmt, mons in parsed:
            branches += 1
            audit_branch(enc, trainer, location, difficulty, fmt, mons,
                         ai_by_trainer.get(trainer, ""), args.list,
                         multi_abilities, multi_moves)

    audit_progression(ladders)

    print(f"\ninspected {branches} trainer branches across "
          f"{len(encounters) // 2} encounters")

    groups = collections.Counter(g for g, _, _ in FINDINGS)
    shown = [f for f in FINDINGS if not args.group or f[0] == args.group]
    for group, where, message in shown:
        print(f"  [{group}] {where}: {message}")
    print("\nfindings by group: " + (", ".join(f"{k}={v}" for k, v in sorted(groups.items())) or "none"))
    print(f"total findings: {len(FINDINGS)}")
    if args.fail_on_findings and FINDINGS:
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
