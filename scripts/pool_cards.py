#!/usr/bin/env python3
"""Compact team-building cards for everything legal before one trainer battle.

One card per legal species: types, base stats, the party-menu Abilities
tuning_pool_check accepts, and its legal moves from the native All Legal
Moves table (src/data/pokemon/emerald_champions_preparation_learnsets.h,
the list the battle driver enforces). Damaging moves are grouped by type,
strongest first, with spread/priority marks; doubles utility moves are listed
separately. The pool is the same one tuning_pool_check uses, so a team built
from these cards should pass it.

  python3 scripts/pool_cards.py --trainer TRAINER_CALVIN_1
  python3 scripts/pool_cards.py --trainer TRAINER_ROXANNE_1 --species SPECIES_MARILL --all-moves
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from functools import lru_cache
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from tuning_pool_check import PROGRESSION, UNTUNED_MOVES, narrow_to_progression  # noqa: E402

PREP = ROOT / "src/data/pokemon/emerald_champions_preparation_learnsets.h"
MOVES_INFO = ROOT / "src/data/moves_info.h"
SPREAD = {"TARGET_BOTH", "TARGET_FOES_AND_ALLY"}
STAT_NAMES = ("HP", "Atk", "Def", "SpA", "SpD", "Spe")
# Status and low-power moves that decide doubles games.
UTILITY = (
    "PROTECT DETECT SPIKY_SHIELD KINGS_SHIELD BANEFUL_BUNKER SILK_TRAP BURNING_BULWARK "
    "FAKE_OUT FOLLOW_ME RAGE_POWDER SPOTLIGHT ALLY_SWITCH HELPING_HAND WIDE_GUARD QUICK_GUARD "
    "TAILWIND TRICK_ROOM ICY_WIND ELECTROWEB SNARL STRUGGLE_BUG BULLDOZE ROCK_TOMB SCARY_FACE "
    "THUNDER_WAVE NUZZLE WILL_O_WISP GLARE SPORE SLEEP_POWDER HYPNOSIS YAWN ENCORE TAUNT "
    "DISABLE IMPRISON TRICK SWITCHEROO KNOCK_OFF SUPER_FANG RUIN FEINT PARTING_SHOT U_TURN "
    "VOLT_SWITCH FLIP_TURN TELEPORT SWORDS_DANCE NASTY_PLOT CALM_MIND DRAGON_DANCE QUIVER_DANCE "
    "SHELL_SMASH BULK_UP COIL AGILITY BELLY_DRUM GROWTH WORK_UP COACHING DECORATE "
    "REFLECT LIGHT_SCREEN AURORA_VEIL SAFEGUARD MIST HAZE CLEAR_SMOG RECOVER ROOST "
    "SOFT_BOILED SLACK_OFF MILK_DRINK SYNTHESIS MOONLIGHT MORNING_SUN WISH LIFE_DEW "
    "POLLEN_PUFF HEAL_PULSE SUBSTITUTE RAIN_DANCE SUNNY_DAY SANDSTORM SNOWSCAPE HAIL "
    "ELECTRIC_TERRAIN GRASSY_TERRAIN MISTY_TERRAIN PSYCHIC_TERRAIN CHARM FEATHER_DANCE "
    "PLAY_NICE PARTING_SHOT STICKY_WEB STEALTH_ROCK MEMENTO HEALING_WISH LUNAR_DANCE "
    "BATON_PASS COURT_CHANGE AFTER_YOU INSTRUCT QUASH POWDER LEECH_SEED"
).split()


def _number(expr: str) -> int | None:
    """Read an int from a moves_info field, taking the modern branch of a
    `B_UPDATED_MOVE_DATA >= GEN_x ? new : old` conditional."""
    if "?" in expr:
        expr = expr.split("?", 1)[1].split(":", 1)[0]
    match = re.search(r"-?\d+", expr)
    return int(match.group()) if match else None


@lru_cache(maxsize=None)
def moves() -> dict[str, dict]:
    text = MOVES_INFO.read_text()
    result = {}
    for block in re.split(r"\n    \[MOVE_", text)[1:]:
        name = "MOVE_" + block.split("]")[0].strip()

        def field(key: str) -> str:
            match = re.search(r"\n        \." + key + r"\s*=\s*([^\n]+?),?\n", block)
            return match.group(1) if match else ""

        label = re.search(r'COMPOUND_STRING\("([^"]*)"\)', field("name"))
        target = field("target")
        if "?" in target:
            target = target.split("?", 1)[1].split(":", 1)[0]
        category = re.search(r"DAMAGE_CATEGORY_([A-Z]+)", field("category"))
        result[name] = dict(
            name=label.group(1) if label else name[5:].title(),
            power=_number(field("power")) or 0,
            priority=_number(field("priority")) or 0,
            type=(re.search(r"TYPE_[A-Z]+", field("type")) or re.search("", "")).group(),
            category=category.group(1) if category else "STATUS",
            target=target.strip(),
        )
    return result


@lru_cache(maxsize=None)
def legal_move_table() -> dict[str, list[str]]:
    text = PREP.read_text()
    arrays = {name: re.findall(r"MOVE_[A-Z0-9_]+", body)
              for name, body in re.findall(r"static const u16 (\w+)\[\] = \{(.*?)\};", text, re.S)}
    table = {}
    for species, array in re.findall(r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*(\w+)", text):
        table[species] = [m for m in arrays.get(array, []) if m != "MOVE_UNAVAILABLE"]
    return table


def legal_moves(species: str) -> list[str]:
    table = legal_move_table()
    token = species
    while token:
        if token in table:
            return [m for m in table[token] if m not in UNTUNED_MOVES]
        if token.count("_") <= 1:
            break
        token = token.rsplit("_", 1)[0]
    return []


def _short(token: str, prefix: str) -> str:
    return token.removeprefix(prefix).replace("_", " ").title()


def move_label(move: str, info: dict) -> str:
    marks = []
    if info["power"]:
        marks.append(f"{info['power']}{info['category'][0]}")
    if info["target"] in SPREAD:
        marks.append("spread")
    if info["priority"]:
        marks.append(f"{info['priority']:+d}")
    return info["name"] + (f"({','.join(marks)})" if marks else "")


def card(row: dict, all_moves: bool, per_type: int) -> dict:
    species = row["species"]
    data = moves()
    learnable = legal_moves(species)
    by_type: dict[str, list[str]] = {}
    for move in sorted(learnable, key=lambda m: -data.get(m, {}).get("power", 0)):
        info = data.get(move)
        if not info or not info["power"] or info["category"] == "STATUS":
            continue
        by_type.setdefault(_short(info["type"], "TYPE_"), []).append(move_label(move, info))
    utility = [move_label(m, data[m]) for m in learnable if m[5:] in UTILITY and m in data]
    priority = [move_label(m, data[m]) for m in learnable
                if m in data and data[m]["priority"] > 0 and data[m]["power"]]
    return dict(
        species=species,
        name=row.get("name") or _short(species, "SPECIES_"),
        types=[_short(t, "TYPE_") for t in _types().get(species, ())],
        stats=dict(zip(STAT_NAMES, row.get("stats") or [])),
        bst=row.get("bst"),
        abilities=[_short(a, "ABILITY_") for a in row.get("abilities", [])],
        restricted=row.get("restricted_class"),
        source=(row.get("source") or {}).get("where") or (row.get("source") or {}).get("kind"),
        attacks={t: (v if all_moves else v[:per_type]) for t, v in by_type.items()},
        priority=priority,
        utility=utility,
        move_count=len(learnable),
    )


@lru_cache(maxsize=None)
def _types() -> dict:
    from ec_moves import TYPES
    return TYPES


def render(cards: list[dict], pool: dict, items: list[str]) -> str:
    out = [f"Legal before {pool['trainer']} (cap {pool['cap']}): {len(cards)} species, {len(items)} held items",
           "Moves: power+P/S, spread, priority. Attacks listed strongest first per type.", ""]
    for c in sorted(cards, key=lambda c: -(c["bst"] or 0)):
        stats = " ".join(f"{k}{v}" for k, v in c["stats"].items())
        head = f"{c['name']} [{'/'.join(c['types'])}] BST {c['bst']}: {stats}"
        if c["restricted"]:
            head += f"  ({c['restricted']})"
        out.append(head)
        out.append(f"  Abilities: {', '.join(c['abilities'])}   from: {c['source']}")
        for type_, labels in sorted(c["attacks"].items()):
            out.append(f"  {type_}: {', '.join(labels)}")
        if c["priority"]:
            out.append(f"  Priority: {', '.join(c['priority'])}")
        if c["utility"]:
            out.append(f"  Utility: {', '.join(c['utility'])}")
        out.append("")
    out.append("Held items: " + ", ".join(sorted(_short(i, "ITEM_") for i in items)))
    return "\n".join(out)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--trainer", required=True, help="TRAINER_* the team is being built for")
    parser.add_argument("--progression", type=Path, default=PROGRESSION)
    parser.add_argument("--species", action="append", help="limit to these SPECIES_* (repeatable)")
    parser.add_argument("--all-moves", action="store_true", help="every legal attack, not the top few per type")
    parser.add_argument("--per-type", type=int, default=4, help="attacks shown per type (default %(default)s)")
    parser.add_argument("--json", action="store_true")
    args = parser.parse_args(argv)

    import pickle
    from manifest_text import resources_before
    from reference_pool import encounter_pool
    have = resources_before(pickle.loads(args.progression.read_bytes()), args.trainer)
    pool, _ = narrow_to_progression(encounter_pool(args.trainer, str(have["cap"])), args.trainer, args.progression)
    rows = pool["species"]
    if args.species:
        rows = [r for r in rows if r["species"] in set(args.species)]
    cards = [card(r, args.all_moves, args.per_type) for r in rows]
    items = [i["item"] for i in pool["items"]]
    if args.json:
        print(json.dumps(dict(trainer=args.trainer, cap=pool["cap"], species=cards, items=items), indent=1))
    else:
        print(render(cards, dict(trainer=args.trainer, cap=pool["cap"]), items))
    return 0


if __name__ == "__main__":
    sys.exit(main())
