#!/usr/bin/env python3
"""Generate and verify one draft or authored battle set per usable species.

Unaudited output is explicitly draft scaffolding, not a claim that competitive
Pokemon has one uniquely optimal set. Published sets are adapted to the moves
and abilities that actually exist in Emerald Champions; every release-ready
entry must ultimately come from a checked-in authored review batch.

Generation with fresh upstream snapshots:

    python3 scripts/verdant_battle_set_presets.py --generate \
      --smogon-gen4 /path/to/gen4.json \
      --smogon-gen5 /path/to/gen5.json \
      --smogon-gen6 /path/to/gen6.json \
      --smogon-gen7 /path/to/gen7.json \
      --smogon-gen8 /path/to/gen8.json \
      --smogon-gen9 /path/to/gen9.json

Repository-only verification (no network or upstream files required):

    python3 scripts/verdant_battle_set_presets.py --check
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
from collections import Counter, defaultdict
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "docs/verdant_battle_set_presets.json"
OVERRIDES = ROOT / "docs/verdant_battle_set_overrides.json"
GENERATED = ROOT / "src/data/pokemon/verdant_battle_sets.h"
INDEX = ROOT / "docs/competitive_team_index.jsonl"
REVIEW_DIR = ROOT / "docs/battle_set_reviews"
GENERATOR_VERSION = 1

# Rotom's appliance script teaches one move as part of the form change.  Those
# moves are runtime-legal even though the ordinary level/TM/tutor bitfields do
# not contain them for the appliance species.
FORM_SPECIFIC_LEGAL_MOVES = {
    "SPECIES_ROTOM_HEAT": {"MOVE_OVERHEAT"},
    "SPECIES_ROTOM_WASH": {"MOVE_HYDRO_PUMP"},
    "SPECIES_ROTOM_FROST": {"MOVE_FREEZE_DRY"},
    "SPECIES_ROTOM_FAN": {"MOVE_HURRICANE"},
    "SPECIES_ROTOM_MOW": {"MOVE_LEAF_STORM"},
}

NATURES = {
    "hardy", "lonely", "brave", "adamant", "naughty", "bold", "docile",
    "relaxed", "impish", "lax", "timid", "hasty", "serious", "jolly",
    "naive", "modest", "mild", "quiet", "bashful", "rash", "calm",
    "gentle", "sassy", "careful", "quirky",
}

# Party-inaccessible transformation endpoints.  Stable regional, cosmetic,
# item and fused forms remain supported because they can legitimately occupy
# a party slot in this engine.
BATTLE_ONLY_FORMS = {
    "SPECIES_CASTFORM_SUNNY", "SPECIES_CASTFORM_RAINY", "SPECIES_CASTFORM_SNOWY",
    "SPECIES_CHERRIM_SUNSHINE", "SPECIES_DARMANITAN_ZEN_MODE",
    "SPECIES_DARMANITAN_ZEN_MODE_GALARIAN", "SPECIES_MELOETTA_PIROUETTE",
    "SPECIES_GRENINJA_ASH", "SPECIES_AEGISLASH_BLADE", "SPECIES_XERNEAS_ACTIVE",
    "SPECIES_ZYGARDE_COMPLETE", "SPECIES_WISHIWASHI_SCHOOL",
    "SPECIES_MINIOR_CORE_RED", "SPECIES_MINIOR_CORE_ORANGE",
    "SPECIES_MINIOR_CORE_YELLOW", "SPECIES_MINIOR_CORE_GREEN",
    "SPECIES_MINIOR_CORE_BLUE", "SPECIES_MINIOR_CORE_INDIGO",
    "SPECIES_MINIOR_CORE_VIOLET", "SPECIES_MIMIKYU_BUSTED",
    "SPECIES_NECROZMA_ULTRA", "SPECIES_CRAMORANT_GULPING",
    "SPECIES_CRAMORANT_GORGING", "SPECIES_EISCUE_NOICE_FACE",
    "SPECIES_MORPEKO_HANGRY", "SPECIES_ETERNATUS_ETERNAMAX",
    "SPECIES_PALAFIN_HERO",
}

FORM_WORD_REPLACEMENTS = {
    "alolan": "alola",
    "galarian": "galar",
    "hisuian": "hisui",
    "female": "f",
    "male": "m",
    "rapidstrikestyle": "rapidstrike",
    "crownedsword": "crowned",
    "crownedshield": "crowned",
    "noiceface": "noice",
    "lowkey": "lowkey",
    "pokeball": "pokeball",
}

ABILITY_WEIGHTS = {
    "wonderguard": 120, "hugepower": 115, "purepower": 115, "shadowtag": 110,
    "prankster": 105, "speedboost": 100, "intimidate": 98, "drizzle": 98,
    "drought": 98, "sandstream": 95, "snowwarning": 94, "magicbounce": 94,
    "regenerator": 92, "multiscale": 92, "adaptability": 90, "technician": 88,
    "protean": 88, "libero": 88, "contrary": 86, "unaware": 86,
    "beastboost": 84, "moxie": 82, "guts": 82, "serenegrace": 80,
    "sheerforce": 80, "magicguard": 80, "levitate": 76, "sturdy": 74,
    "waterabsorb": 72, "voltabsorb": 72, "flashfire": 72, "thickfat": 70,
    "ironfist": 68, "toughclaws": 68, "strongjaw": 68, "chlorophyll": 67,
    "swiftswim": 67, "sandrush": 67, "slushrush": 67, "naturalcure": 65,
    "imposter": 120,
    "skilllink": 92, "furcoat": 100, "friendguard": 88, "simple": 86,
    "scrappy": 78, "galewings": 82, "triage": 84, "tintedlens": 82,
    "competitive": 84, "defiant": 84, "parentalbond": 105, "punkrock": 80,
    "pixilate": 82, "aerilate": 82, "refrigerate": 82, "filter": 76,
    "solidrock": 76, "purifyingsalt": 92, "goodasgold": 98,
    "runaway": -25, "pickup": -20, "honeygather": -35, "ballfetch": -35,
    "illuminate": -25, "bigpecks": 0, "keeneye": 0, "klutz": -20,
}

UTILITY_WEIGHTS = {
    "protect": 118, "detect": 116, "spore": 115, "fakeout": 112,
    "tailwind": 108, "trickroom": 108, "followme": 106, "ragepowder": 106,
    "helpinghand": 100, "icywind": 99, "electroweb": 99, "wideguard": 98,
    "willowisp": 96, "thunderwave": 95, "stealthrock": 92, "stickyweb": 92,
    "spikes": 90, "toxicspikes": 88, "partingshot": 92, "encore": 92,
    "taunt": 90, "recover": 94, "roost": 94, "softboiled": 94,
    "slackoff": 94, "synthesis": 90, "moonlight": 90, "strengthsap": 96,
    "nastyplot": 90, "swordsdance": 90, "dragondance": 94,
    "quiverdance": 98, "shellsmash": 98, "calmmind": 88, "bulkup": 86,
    "bellydrum": 92, "haze": 84, "clearsmog": 82, "reflect": 82,
    "lightscreen": 82, "auroraveil": 96, "leechseed": 86, "substitute": 78,
    "toxic": 84, "trick": 82, "switcheroo": 80, "defog": 80,
    "rapidspin": 82, "batonpass": 72, "endeavor": 72, "destinybond": 84,
    "perishsong": 84, "yawn": 82, "wish": 86, "healbell": 76,
    "sleeppowder": 112, "hypnosis": 94, "lovelykiss": 100, "darkvoid": 96,
    "glare": 98, "nuzzle": 96, "stunspore": 88, "fake tears": 88,
    "faketears": 88, "charm": 86, "babydolleyes": 82, "featherdance": 84,
    "scaryface": 78, "snarl": 84, "roar": 78, "whirlwind": 78,
    "memento": 82, "healingwish": 76, "lifedew": 88, "healpulse": 84,
    "painsplit": 86, "aromatherapy": 76, "safeguard": 68,
}

BAD_FALLBACK_MOVES = {
    "splash", "celebrate", "holdhands", "happyhour", "frustration", "present",
    "bestow", "quash", "afteryou", "mefirst", "assist", "metronome",
    "mimic", "copycat", "snatch", "sketch", "transform",
    "belch", "dreameater", "lastresort", "focuspunch", "naturalgift", "fling",
    "solarbeam", "solarblade", "skyattack", "skullbash", "fly", "dig", "dive",
    "bounce", "razorwind", "phantomforce", "shadowforce", "geomancy", "meteorbeam",
    "electroshot", "explosion", "selfdestruct", "mistyexplosion", "memento",
    "petaldance", "outrage", "thrash", "uproar", "rollout", "iceball",
    "dreameater", "storedpower", "venoshock", "hex", "facade", "reversal", "flail",
    "endeavor", "bide", "counter", "mirrorcoat", "synchronoise",
    "doubleedge", "takedown",
}

PROTECT_MOVES = {"protect", "detect", "kingsshield", "spikyshield", "banefulbunker", "burningbulwark", "obstruct"}
SETUP_MOVES = {"swordsdance", "nastyplot", "dragondance", "quiverdance", "shellsmash", "calmmind", "bulkup", "bellydrum", "growth", "curse", "agility", "rockpolish", "irondefense", "amnesia", "coil", "honeclaws"}
RECOVERY_MOVES = {"recover", "roost", "softboiled", "slackoff", "synthesis", "moonlight", "morningsun", "strengthsap", "shoreup", "milkdrink", "rest"}
SPECIAL_SETUP_MOVES = {"nastyplot", "calmmind", "quiverdance", "tailglow", "geomancy"}
PHYSICAL_SETUP_MOVES = {"swordsdance", "bulkup", "dragondance", "bellydrum", "coil", "honeclaws"}
STAT_INDEPENDENT_ATTACKS = {"foulplay", "bodypress", "seismictoss", "nightshade", "counter", "mirrorcoat"}
ABILITY_MOVE_SYNERGY = {
    "skilllink": {"tailslap", "rockblast", "iciclespear", "bulletseed", "pinmissile", "furyswipes", "furyattack", "armthrust", "bonerush", "spikecannon", "barrage"},
    "strongjaw": {"crunch", "bite", "psychicfangs", "icefang", "firefang", "thunderfang", "poisonfang", "hyperfang"},
    "ironfist": {"machpunch", "bulletpunch", "drainpunch", "firepunch", "icepunch", "thunderpunch", "poweruppunch", "meteormash", "shadowpunch"},
}

# The builder deliberately does not rewrite IVs or happiness.  These moves can
# therefore be legal yet misleading as a plug-and-play default.
UNSAFE_AUTOBUILD_MOVES = {"MOVE_RETURN", "MOVE_FRUSTRATION", "MOVE_HIDDEN_POWER"}


def read(path: Path) -> str:
    return path.read_text()


def compact(value: str) -> str:
    value = value.replace("♀", "f").replace("♂", "m")
    return re.sub(r"[^a-z0-9]+", "", value.lower())


def token_compact(token: str, prefix: str) -> str:
    value = compact(token.removeprefix(prefix))
    for old, new in FORM_WORD_REPLACEMENTS.items():
        value = value.replace(old, new)
    return value


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def json_dump(payload: object) -> str:
    return json.dumps(payload, indent=2, sort_keys=True, ensure_ascii=False) + "\n"


def macro_values() -> tuple[dict[str, int], dict[int, str], int]:
    source = read(ROOT / "include/constants/species.h") + "\n" + read(
        ROOT / "include/constants/verdant_gen9_species.h"
    )
    expressions: dict[str, str] = {}
    order: list[str] = []
    for name, expression in re.findall(r"^#define\s+([A-Z][A-Z0-9_]*)\s+([^/\n]+)", source, re.M):
        if not (name.startswith("SPECIES_") or name in {"FORMS_START", "VERDANT_GEN9_START", "VERDANT_GEN9_END", "NUM_SPECIES"}):
            continue
        expression = expression.strip()
        if "(" in name:
            continue
        expressions[name] = expression
        order.append(name)

    values: dict[str, int] = {}
    unresolved = dict(expressions)
    for _ in range(len(unresolved) + 2):
        progress = False
        for name, expression in list(unresolved.items()):
            expanded = expression
            for dependency in sorted(set(re.findall(r"\b[A-Z][A-Z0-9_]*\b", expression)), key=len, reverse=True):
                if dependency not in values:
                    break
                expanded = re.sub(rf"\b{dependency}\b", str(values[dependency]), expanded)
            else:
                if re.fullmatch(r"[0-9xXa-fA-F()+\-\s]+", expanded):
                    values[name] = int(eval(expanded, {"__builtins__": {}}, {}))
                    del unresolved[name]
                    progress = True
        if not progress:
            break
    if "NUM_SPECIES" not in values:
        raise ValueError(f"could not evaluate NUM_SPECIES; unresolved={sorted(unresolved)[:10]}")

    canonical: dict[int, str] = {}
    for name in order:
        if name.startswith("SPECIES_") and name in values:
            canonical.setdefault(values[name], name)
    return values, canonical, values["NUM_SPECIES"]


def exclusion_reason(species: str) -> str | None:
    if species == "SPECIES_EGG":
        return "egg-sentinel"
    if re.search(r"_(?:MEGA(?:_[XY])?|PRIMAL)$", species):
        return "battle-transformation-endpoint"
    if species.startswith("SPECIES_UNOWN_"):
        return "unown-personality-graphic-slot"
    if species in BATTLE_ONLY_FORMS:
        return "automatic-or-battle-only-form"
    return None


@dataclass(frozen=True)
class BaseStats:
    hp: int
    attack: int
    defense: int
    speed: int
    sp_attack: int
    sp_defense: int
    type1: str
    type2: str
    abilities: tuple[str, str, str]


def designated_blocks(source: str) -> dict[str, str]:
    matches = list(re.finditer(r"^\s*\[(SPECIES_[A-Z0-9_]+)\]\s*=", source, re.M))
    result: dict[str, str] = {}
    for index, match in enumerate(matches):
        end = matches[index + 1].start() if index + 1 < len(matches) else len(source)
        result[match.group(1)] = source[match.end():end]
    return result


def parse_base_stats() -> dict[str, BaseStats]:
    source = read(ROOT / "src/data/pokemon/base_stats.h") + "\n" + read(
        ROOT / "src/data/pokemon/verdant_gen9_base_stats.h"
    )
    result: dict[str, BaseStats] = {}
    for species, body in designated_blocks(source).items():
        def number(field: str) -> int:
            match = re.search(rf"\.{field}\s*=\s*(\d+)", body)
            return int(match.group(1)) if match else 0

        ability_match = re.search(r"\.abilities\s*=\s*\{([^}]+)\}", body)
        type1 = re.search(r"\.type1\s*=\s*(TYPE_[A-Z0-9_]+)", body)
        type2 = re.search(r"\.type2\s*=\s*(TYPE_[A-Z0-9_]+)", body)
        if not ability_match or not type1 or not type2:
            continue
        abilities = re.findall(r"ABILITY_[A-Z0-9_]+", ability_match.group(1))
        abilities = (abilities + ["ABILITY_NONE"] * 3)[:3]
        result[species] = BaseStats(
            number("baseHP"), number("baseAttack"), number("baseDefense"),
            number("baseSpeed"), number("baseSpAttack"), number("baseSpDefense"),
            type1.group(1), type2.group(1), tuple(abilities),
        )
    return result


def parse_names() -> dict[str, str]:
    source = read(ROOT / "src/data/text/species_names.h") + "\n" + read(
        ROOT / "src/data/text/verdant_gen9_species_names.h"
    )
    return dict(re.findall(r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*_\(\"([^\"]+)\"\)", source))


def parse_move_tokens() -> tuple[dict[str, str], set[str]]:
    source = read(ROOT / "include/constants/moves.h")
    moves = set(re.findall(r"^#define\s+(MOVE_[A-Z0-9_]+)\s+", source, re.M))
    by_name = {token_compact(move, "MOVE_"): move for move in moves}
    # Published analyses spell typed Hidden Power as if each type were a move.
    for move_type in ("bug", "dark", "dragon", "electric", "fighting", "fire", "flying", "ghost", "grass", "ground", "ice", "poison", "psychic", "rock", "steel", "water"):
        by_name[f"hiddenpower{move_type}"] = "MOVE_HIDDEN_POWER"
    by_name["softboiled"] = "MOVE_SOFT_BOILED"
    by_name["highjumpkick"] = "MOVE_HIGH_JUMP_KICK"
    by_name["doubleedge"] = "MOVE_DOUBLE_EDGE"
    return by_name, moves


def parse_level_moves() -> tuple[dict[str, str], dict[str, set[str]]]:
    pointer_source = read(ROOT / "src/data/pokemon/level_up_learnset_pointers.h") + "\n" + read(
        ROOT / "src/data/pokemon/verdant_gen9_level_up_pointers.h"
    )
    pointers = dict(re.findall(r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*(s[A-Za-z0-9_]+LevelUpLearnset)", pointer_source))
    learnset_source = read(ROOT / "src/data/pokemon/level_up_learnsets.h") + "\n" + read(
        ROOT / "src/data/pokemon/verdant_gen9_level_up_learnsets.h"
    )
    learnsets = {
        name: set(re.findall(r"MOVE_[A-Z0-9_]+", body))
        for name, body in re.findall(
            r"static const struct LevelUpMove\s+(s[A-Za-z0-9_]+LevelUpLearnset)\[\]\s*=\s*\{(.*?)\};",
            learnset_source, re.S,
        )
    }
    return pointers, learnsets


def parse_tm_moves() -> dict[str, set[str]]:
    move_table = read(ROOT / "src/data/party_menu.h").split("static const u16 sTMHMMoves[] =", 1)[1].split("};", 1)[0]
    ordered_moves = re.findall(r"MOVE_[A-Z0-9_]+", move_table)
    source = read(ROOT / "src/data/pokemon/tmhm_learnsets.h") + "\n" + read(
        ROOT / "src/data/pokemon/verdant_gen9_tmhm_learnsets.h"
    )
    result: dict[str, set[str]] = {}
    for species, body in designated_blocks(source).items():
        moves: set[str] = set()
        # Resolve by TM/HM slot, not by the item-token suffix: several item
        # names intentionally differ from their canonical move token (for
        # example TM22_SOLARBEAM -> MOVE_SOLAR_BEAM).
        for kind, number, _suffix in re.findall(r"(TM|HM)(\d+)_([A-Z0-9_]+)", body):
            index = int(number) - 1 if kind == "TM" else 100 + int(number) - 1
            if 0 <= index < len(ordered_moves):
                moves.add(ordered_moves[index])
        words = [int(value, 16) for value in re.findall(r"0x[0-9A-Fa-f]+", body)]
        if len(words) >= 4:
            for index, move in enumerate(ordered_moves):
                if index // 32 < 4 and words[index // 32] & (1 << (index % 32)):
                    moves.add(move)
        result[species] = moves
    return result


def parse_tutor_moves() -> dict[str, set[str]]:
    source = read(ROOT / "src/data/pokemon/tutor_learnsets.h")
    table = source.split("const u16 gTutorMoves[] =", 1)[1].split("};", 1)[0]
    ordered_moves = [
        match.group(1)
        for line in table.splitlines()
        if (match := re.search(r"\[TUTOR_MOVE_[A-Z0-9_]+\]\s*=\s*(MOVE_[A-Z0-9_]+)", line))
    ]
    learnsets = source.split("static const u32 sTutorLearnsets", 1)[1] + "\n" + read(
        ROOT / "src/data/pokemon/verdant_gen9_tutor_learnsets.h"
    )
    result: dict[str, set[str]] = {}
    for species, body in designated_blocks(learnsets).items():
        words = [int(value, 16) for value in re.findall(r"0x[0-9A-Fa-f]+", body)]
        moves: set[str] = set()
        if len(words) >= 5:
            for index, move in enumerate(ordered_moves):
                if index // 32 < 5 and words[index // 32] & (1 << (index % 32)):
                    moves.add(move)
        result[species] = moves
    return result


def parse_egg_moves() -> dict[str, set[str]]:
    source = read(ROOT / "src/data/pokemon/egg_moves.h") + "\n" + read(
        ROOT / "src/data/pokemon/verdant_gen9_egg_moves.h"
    )
    return {
        "SPECIES_" + species: set(re.findall(r"MOVE_[A-Z0-9_]+", body))
        for species, body in re.findall(r"egg_moves\(([A-Z0-9_]+),(.*?)\)(?:,|\s*$)", source, re.S | re.M)
    }


def parse_preevolutions(values: dict[str, int]) -> dict[str, str]:
    source = read(ROOT / "src/data/pokemon/evolution.h") + "\n" + read(
        ROOT / "src/data/pokemon/verdant_gen9_evolutions.h"
    )
    candidates: dict[str, list[str]] = defaultdict(list)
    for parent, body in designated_blocks(source).items():
        for method, target in re.findall(
            r"\{\s*(EVO_[A-Z0-9_]+)\s*,\s*[^,{}]+,\s*(SPECIES_[A-Z0-9_]+)\s*\}", body
        ):
            if "MEGA" not in method and "PRIMAL" not in method:
                candidates[target].append(parent)
    return {
        child: min(parents, key=lambda parent: values.get(parent, 1 << 30))
        for child, parents in candidates.items()
    }


def parse_move_info() -> dict[str, dict[str, object]]:
    source = read(ROOT / "src/data/battle_moves.h") + "\n" + read(
        ROOT / "src/data/verdant_gen9_battle_moves.h"
    )
    result: dict[str, dict[str, object]] = {}
    for move, body in re.findall(r"\[(MOVE_[A-Z0-9_]+)\]\s*=\s*\{(.*?)(?=^\s*\[MOVE_|\Z)", source, re.M | re.S):
        def token(field: str, default: str) -> str:
            match = re.search(rf"\.{field}\s*=\s*([A-Z][A-Z0-9_]*)", body)
            return match.group(1) if match else default

        def number(field: str, default: int = 0) -> int:
            match = re.search(rf"\.{field}\s*=\s*(-?\d+)", body)
            return int(match.group(1)) if match else default

        result[move] = {
            "power": number("power"), "accuracy": number("accuracy", 100),
            "priority": number("priority"), "type": token("type", "TYPE_NORMAL"),
            "split": token("split", "SPLIT_STATUS"), "target": token("target", "MOVE_TARGET_SELECTED"),
        }
    return result


class LocalDex:
    def __init__(self) -> None:
        self.values, self.canonical, self.num_species = macro_values()
        self.stats = parse_base_stats()
        self.names = parse_names()
        self.move_by_name, self.move_tokens = parse_move_tokens()
        self.level_pointers, self.level_moves = parse_level_moves()
        self.tm_moves = parse_tm_moves()
        self.tutor_moves = parse_tutor_moves()
        self.egg_moves = parse_egg_moves()
        self.parent = parse_preevolutions(self.values)
        self.move_info = parse_move_info()
        self.excluded = {
            species: reason
            for species_id, species in sorted(self.canonical.items())
            if species_id and species_id < self.num_species
            if (reason := exclusion_reason(species)) is not None
        }
        self.supported = [
            species for species_id, species in sorted(self.canonical.items())
            if species_id and species_id < self.num_species and species not in self.excluded
        ]
        self.species_by_name = self._species_aliases()
        self.display_groups: dict[str, list[str]] = defaultdict(list)
        for species in self.supported:
            self.display_groups[compact(self.names.get(species, species))].append(species)

    def _species_aliases(self) -> dict[str, str]:
        result: dict[str, str] = {}
        for species in self.supported:
            raw = species.removeprefix("SPECIES_")
            aliases = {token_compact(species, "SPECIES_"), compact(self.names.get(species, raw))}
            aliases.add(compact(raw.replace("_ALOLAN", "_ALOLA").replace("_GALARIAN", "_GALAR").replace("_HISUIAN", "_HISUI")))
            if raw.endswith("_FEMALE"):
                aliases.add(compact(raw.removesuffix("_FEMALE") + "_F"))
            if raw == "URSHIFU_RAPID_STRIKE_STYLE":
                aliases.add("urshifurapidstrike")
            if raw in {"ZACIAN_CROWNED_SWORD", "ZAMAZENTA_CROWNED_SHIELD"}:
                aliases.add(compact(raw.split("_CROWNED", 1)[0] + "_CROWNED"))
            for alias in aliases:
                result.setdefault(alias, species)
        return result

    def lineage(self, species: str) -> list[str]:
        result = []
        seen = set()
        current = species
        while current not in seen:
            seen.add(current)
            result.append(current)
            if current not in self.parent:
                break
            current = self.parent[current]
        return result

    def base_equivalent(self, species: str) -> str | None:
        """Return the stable/base representative sharing the in-game name."""
        group = self.display_groups.get(compact(self.names.get(species, species)), [])
        if not group:
            return None
        base = min(group, key=lambda candidate: self.values[candidate])
        return base if base != species else None

    def legal_moves(self, species: str) -> set[str]:
        moves: set[str] = set()
        lineage = self.lineage(species)
        for member in lineage:
            pointer = self.level_pointers.get(member)
            if pointer:
                moves.update(self.level_moves.get(pointer, set()))
            moves.update(self.tm_moves.get(member, set()))
            moves.update(self.tutor_moves.get(member, set()))
        root = lineage[-1]
        moves.update(self.egg_moves.get(root, set()))
        moves.update(self.egg_moves.get(species, set()))
        if root == "SPECIES_PICHU":
            moves.add("MOVE_VOLT_TACKLE")
        if root == "SPECIES_BAGON":
            moves.add("MOVE_WISH")
        moves.update(FORM_SPECIFIC_LEGAL_MOVES.get(species, set()))
        # The ordinary tutor bitfields cannot express Sketch's indirect
        # legality.  These hand-selected support moves are all obtainable by
        # using the locally implemented Sketch in battle.
        if species == "SPECIES_SMEARGLE":
            moves.update({"MOVE_SPORE", "MOVE_FAKE_OUT", "MOVE_FOLLOW_ME", "MOVE_WIDE_GUARD"})
        return {move for move in moves if move in self.move_tokens and move not in {"MOVE_NONE", "MOVE_STRUGGLE"}}

    def ability_slot(self, species: str, published: str | None = None) -> tuple[int, str, bool]:
        abilities = self.stats[species].abilities
        wanted = compact(published or "")
        for slot, ability in enumerate(abilities):
            if ability != "ABILITY_NONE" and token_compact(ability, "ABILITY_") == wanted:
                return slot, ability, True
        choices = [(ABILITY_WEIGHTS.get(token_compact(ability, "ABILITY_"), 10), -slot, slot, ability)
                   for slot, ability in enumerate(abilities) if ability != "ABILITY_NONE"]
        if not choices:
            raise ValueError(f"{species} has no selectable ability")
        _, _, slot, ability = max(choices)
        return slot, ability, False


def choose_option(value: object) -> list[str]:
    if isinstance(value, str):
        return [value]
    if isinstance(value, list):
        result: list[str] = []
        for option in value:
            if isinstance(option, str):
                result.append(option)
        return result
    return []


def dependency_context(battle_set: dict) -> list[str]:
    moves = {compact(move) for slot in battle_set.get("moves", []) for move in choose_option(slot)}
    items = {compact(item) for item in choose_option(battle_set.get("item"))}
    notes: list[str] = []
    if any("choice" in item for item in items):
        notes.append("published set assumes choice-item move locking")
    if moves & {"solarbeam", "solarblade", "weatherball", "growth"}:
        notes.append("weather-dependent value should be checked against sun access")
    if moves & {"thunder", "hurricane", "weatherball"}:
        notes.append("weather-sensitive accuracy or power should be checked against rain access")
    if moves & {"auroraveil", "blizzard"}:
        notes.append("snow or hail support may be required")
    if moves & {"dreameater"}:
        notes.append("Dream Eater requires a reliable sleep enabler")
    if moves & {"belch"}:
        notes.append("Belch requires consuming a berry first")
    if moves & {"lastresort"}:
        notes.append("Last Resort requires every other move to be used first")
    if moves & {"focuspunch"}:
        notes.append("Focus Punch normally requires Substitute, redirection, or forced passivity")
    if any(move.startswith("hiddenpower") for move in moves):
        notes.append("Hidden Power depends on IVs and is never installed by the draft builder")
    if moves & {"followme", "ragepowder", "helpinghand", "wideguard", "coaching", "healpulse"}:
        notes.append("move value is doubles-partner dependent")
    return notes


def resolve_published_moves(raw_moves: object, legal: set[str], move_by_name: dict[str, str]) -> tuple[list[str], list[str]]:
    if not isinstance(raw_moves, list):
        return [], []
    selected: list[str] = []
    rejected: list[str] = []
    for slot in raw_moves:
        options = choose_option(slot)
        chosen = None
        for option in options:
            move = move_by_name.get(compact(option))
            if move in legal and move not in selected:
                chosen = move
                break
        if chosen:
            selected.append(chosen)
        elif options:
            rejected.append(" / ".join(options))
    return selected, rejected


def format_score(fmt: str, species: str, dex: LocalDex) -> int:
    fmt = compact(fmt)
    is_preevo = species in dex.parent.values() or any(parent == species for parent in dex.parent.values())
    score = 0
    if "doubles" in fmt or "vgc" in fmt:
        score += 1000
    if is_preevo and (fmt == "lc" or "littlecup" in fmt):
        score += 800
    if is_preevo and "nfe" in fmt:
        score += 750
    tiers = {"uber": 90, "ou": 80, "uu": 70, "ru": 60, "nu": 50, "pu": 40, "zu": 30, "nfe": 20, "lc": 20}
    score += max((weight for tier, weight in tiers.items() if tier in fmt), default=0)
    return score


def smogon_candidates(paths: dict[int, Path], dex: LocalDex) -> dict[str, list[dict]]:
    result: dict[str, list[dict]] = defaultdict(list)
    for generation in sorted(paths, reverse=True):
        payload = json.loads(read(paths[generation]))
        for published_species, formats in payload.items():
            species = dex.species_by_name.get(compact(published_species))
            if not species or not isinstance(formats, dict):
                continue
            for fmt, named_sets in formats.items():
                if not isinstance(named_sets, dict):
                    continue
                for set_name, battle_set in named_sets.items():
                    if not isinstance(battle_set, dict):
                        continue
                    result[species].append({
                        "priority": (format_score(fmt, species, dex), generation, set_name),
                        "kind": "smogon-published-set",
                        "source_family": "smogon",
                        "battle_style": "doubles" if "doubles" in compact(fmt) or "vgc" in compact(fmt) else "singles",
                        "lc_or_nfe": compact(fmt) in {"lc", "nfe"} or "littlecup" in compact(fmt),
                        "locator": {"generation": generation, "format": fmt, "set": set_name, "species": published_species},
                        "moves": battle_set.get("moves", []),
                        "nature": battle_set.get("nature"),
                        "ability": battle_set.get("ability"),
                        "item": battle_set.get("item"),
                        "published_moves": battle_set.get("moves", []),
                        "dependencies": dependency_context(battle_set),
                    })
    for candidates in result.values():
        candidates.sort(key=lambda row: row["priority"], reverse=True)
    return result


def corpus_candidates(dex: LocalDex) -> dict[str, list[dict]]:
    result: dict[str, list[dict]] = defaultdict(list)
    if not INDEX.exists():
        return result
    for line in read(INDEX).splitlines():
        if not line.strip():
            continue
        record = json.loads(line)
        if record.get("completeness") != "full-sets":
            continue
        for battle_set in record.get("sets", []):
            species = dex.species_by_name.get(compact(battle_set.get("species", "")))
            if not species:
                continue
            result[species].append({
                "priority": (1 if record.get("battle_style") == "doubles" else 0, record.get("generation") or 0, record.get("reference_id", "")),
                "kind": "checked-in-competitive-corpus",
                "source_family": record.get("source_kind", "checked-in-corpus"),
                "battle_style": record.get("battle_style", "singles"),
                "lc_or_nfe": "lc" in compact(record.get("format", "")) or "nfe" in compact(record.get("format", "")),
                "locator": {"reference_id": record.get("reference_id")},
                "moves": battle_set.get("moves", []),
                "nature": battle_set.get("nature"),
                "ability": battle_set.get("ability"),
                "item": battle_set.get("item"),
                "published_moves": battle_set.get("moves", []),
                "dependencies": dependency_context(battle_set),
            })
    for candidates in result.values():
        candidates.sort(key=lambda row: row["priority"], reverse=True)
    return result


def damage_score(move: str, stats: BaseStats, info: dict[str, object]) -> int:
    move_info = info.get(move, {})
    power = int(move_info.get("power", 0))
    accuracy = int(move_info.get("accuracy", 100)) or 100
    split = str(move_info.get("split", "SPLIT_STATUS"))
    move_type = str(move_info.get("type", "TYPE_NORMAL"))
    if power <= 0 or split == "SPLIT_STATUS":
        return -10000
    effective = power * min(accuracy, 100) // 100
    if move_type in {stats.type1, stats.type2}:
        effective += 35
    if split == "SPLIT_PHYSICAL":
        effective += max(-25, min(35, (stats.attack - stats.sp_attack) // 2))
    elif split == "SPLIT_SPECIAL":
        effective += max(-25, min(35, (stats.sp_attack - stats.attack) // 2))
    if int(move_info.get("priority", 0)) > 0:
        effective += 18
    if str(move_info.get("target", "")) == "MOVE_TARGET_BOTH":
        effective += 12
    elif str(move_info.get("target", "")) == "MOVE_TARGET_FOES_AND_ALLY":
        effective -= 30
    name = token_compact(move, "MOVE_")
    if name in {"explosion", "selfdestruct", "mistyexplosion"}:
        effective -= 45
    if name in {"hyperbeam", "gigaimpact", "blastburn", "frenzyplant", "hydrocannon"}:
        effective -= 35
    return effective


def move_function_group(move: str, dex: LocalDex) -> str | None:
    name = token_compact(move, "MOVE_")
    if name in PROTECT_MOVES:
        return "protect"
    if name in SETUP_MOVES:
        return "setup"
    if name in RECOVERY_MOVES:
        return "recovery"
    if name in {"toxic", "willowisp", "thunderwave", "glare", "nuzzle", "stunspore", "sleeppowder", "spore", "hypnosis", "lovelykiss", "yawn"}:
        return "major-status"
    return None


def attack_orientation(move: str, dex: LocalDex) -> str | None:
    name = token_compact(move, "MOVE_")
    if name in STAT_INDEPENDENT_ATTACKS:
        return None
    split = dex.move_info.get(move, {}).get("split")
    if split == "SPLIT_PHYSICAL":
        return "physical"
    if split == "SPLIT_SPECIAL":
        return "special"
    return None


def fallback_moves(
    species: str,
    legal: set[str],
    dex: LocalDex,
    seed: Iterable[str] = (),
    ability_hint: str | None = None,
) -> list[str]:
    stats = dex.stats[species]
    selected = list(dict.fromkeys(seed))[:4]
    safe = {move for move in legal if token_compact(move, "MOVE_") not in BAD_FALLBACK_MOVES}
    pool = safe if safe else legal
    damaging = sorted(
        (move for move in pool if damage_score(move, stats, dex.move_info) > 0),
        key=lambda move: (-damage_score(move, stats, dex.move_info), move),
    )
    utility = sorted(
        (
            move for move in pool
            if dex.move_info.get(move, {}).get("split") == "SPLIT_STATUS"
            and token_compact(move, "MOVE_") not in BAD_FALLBACK_MOVES
        ),
        key=lambda move: (-UTILITY_WEIGHTS.get(token_compact(move, "MOVE_"), 35), move),
    )
    if ability_hint:
        ability = compact(ability_hint)
    else:
        _, ability_token, _ = dex.ability_slot(species)
        ability = token_compact(ability_token, "ABILITY_")
    if ability == "truant":
        safe = {move for move in safe if token_compact(move, "MOVE_") not in PROTECT_MOVES}
        pool = safe if safe else legal
        damaging = sorted(
            (move for move in pool if damage_score(move, stats, dex.move_info) > 0),
            key=lambda move: (-damage_score(move, stats, dex.move_info), move),
        )
        utility = [move for move in utility if token_compact(move, "MOVE_") not in PROTECT_MOVES]

    def add(move: str) -> bool:
        if move in selected or len(selected) >= 4:
            return False
        group = move_function_group(move, dex)
        if group and any(move_function_group(other, dex) == group for other in selected):
            return False
        selected.append(move)
        return True

    def is_attack(move: str) -> bool:
        return damage_score(move, stats, dex.move_info) > 0

    def attack_types() -> set[str]:
        return {str(dex.move_info.get(move, {}).get("type")) for move in selected if is_attack(move)}

    def utility_compatible(move: str) -> bool:
        name = token_compact(move, "MOVE_")
        physical = sum(attack_orientation(other, dex) == "physical" for other in selected)
        special = sum(attack_orientation(other, dex) == "special" for other in selected)
        if name in SPECIAL_SETUP_MOVES and physical > special:
            return False
        if name in PHYSICAL_SETUP_MOVES and special > physical:
            return False
        return True

    # Prankster defaults must actually exploit Prankster instead of receiving
    # four attacks. Protect/Detect do not count as the ability payoff.
    if ability == "prankster" and not any(
        dex.move_info.get(move, {}).get("split") == "SPLIT_STATUS"
        and token_compact(move, "MOVE_") not in PROTECT_MOVES
        for move in selected
    ):
        for move in utility:
            if token_compact(move, "MOVE_") not in PROTECT_MOVES and add(move):
                break

    # Abilities whose value is explicitly tied to a move family receive at
    # least one compatible move when the local learnset permits it.
    synergy_names = ABILITY_MOVE_SYNERGY.get(ability, set())
    if ability == "technician":
        synergy_moves = [
            move for move in damaging
            if 0 < int(dex.move_info.get(move, {}).get("power", 0)) <= 60
        ]
        if synergy_moves:
            add(synergy_moves[0])
    elif synergy_names and not any(token_compact(move, "MOVE_") in synergy_names for move in selected):
        synergy_moves = [move for move in damaging if token_compact(move, "MOVE_") in synergy_names]
        if synergy_moves:
            add(synergy_moves[0])

    # Establish one dependable STAB attack whenever the species has one.
    if not any(is_attack(move) and dex.move_info.get(move, {}).get("type") in {stats.type1, stats.type2} for move in selected):
        for move in damaging:
            if dex.move_info.get(move, {}).get("type") in {stats.type1, stats.type2} and add(move):
                break

    desired_attacks = 3
    if ability == "prankster" or max(stats.attack, stats.sp_attack) < 65:
        desired_attacks = 2
    if ability in {"protean", "libero"}:
        desired_attacks = 3

    # Coverage is selected by type, not merely by base power. This keeps
    # Protean/Libero useful and prevents redundant four-attack fallbacks.
    for move in damaging:
        if sum(is_attack(other) for other in selected) >= desired_attacks:
            break
        move_type = str(dex.move_info.get(move, {}).get("type"))
        if move_type not in attack_types():
            add(move)

    # Add coherent utility after the offensive core. Known competitive
    # utility outranks generic status moves, and duplicate function groups are
    # rejected by add().
    for move in utility:
        if utility_compatible(move) and add(move) and len(selected) >= 4:
            break

    # Fill remaining slots with new attack types, then any safe useful move.
    for move in damaging:
        if str(dex.move_info.get(move, {}).get("type")) not in attack_types():
            add(move)
        if len(selected) >= 4:
            break
    ranked_safe = sorted(
        pool,
        key=lambda move: (
            -max(damage_score(move, stats, dex.move_info), UTILITY_WEIGHTS.get(token_compact(move, "MOVE_"), 35)),
            move,
        ),
    )
    for move in ranked_safe:
        add(move)
        if len(selected) >= 4:
            break
    # Tiny/forced learnsets may need a conditional move; only use one after
    # exhausting the safe pool.
    for move in sorted(legal, key=lambda move: (-damage_score(move, stats, dex.move_info), move)):
        if move not in selected:
            selected.append(move)
        if len(selected) >= 4:
            break
    return selected[:4]


def fallback_nature(stats: BaseStats, moves: list[str], dex: LocalDex) -> str:
    names = {token_compact(move, "MOVE_") for move in moves}
    if "trickroom" in names:
        return "NATURE_BRAVE" if stats.attack >= stats.sp_attack else "NATURE_QUIET"
    physical = sum(attack_orientation(move, dex) == "physical" for move in moves)
    special = sum(attack_orientation(move, dex) == "special" for move in moves)
    if names & PHYSICAL_SETUP_MOVES:
        physical += 2
    if names & SPECIAL_SETUP_MOVES:
        special += 2
    if physical > special:
        return "NATURE_JOLLY" if stats.speed >= 70 else "NATURE_ADAMANT"
    if special > physical:
        return "NATURE_TIMID" if stats.speed >= 70 else "NATURE_MODEST"
    if stats.defense >= stats.sp_defense + 15:
        return "NATURE_IMPISH" if stats.attack >= stats.sp_attack else "NATURE_BOLD"
    if stats.sp_defense >= stats.defense + 15:
        return "NATURE_CAREFUL" if stats.attack >= stats.sp_attack else "NATURE_CALM"
    return "NATURE_JOLLY" if stats.attack >= stats.sp_attack else "NATURE_TIMID"


def published_nature(value: object) -> str | None:
    for option in choose_option(value):
        normalized = compact(option)
        if normalized in NATURES:
            return "NATURE_" + normalized.upper()
    return None


def load_overrides() -> dict[str, dict]:
    payload = json.loads(read(OVERRIDES))
    if payload.get("schema_version") != 1 or not isinstance(payload.get("presets"), dict):
        raise ValueError("battle-set overrides have an unsupported schema")
    return payload["presets"]


REVIEW_REQUIRED_FIELDS = {
    "species", "role", "moves", "nature", "ability", "suggested_item",
    "item_context", "source_refs", "doubles_rationale", "dependency_notes",
    "reviewed_by", "review_notes",
}


def load_reviews(dex: LocalDex) -> tuple[dict[str, dict], list[dict]]:
    reviews: dict[str, dict] = {}
    batches: list[dict] = []
    item_tokens = set(re.findall(
        r"^#define\s+(ITEM_[A-Z0-9_]+)\s+", read(ROOT / "include/constants/items.h"), re.M
    ))
    for path in sorted(REVIEW_DIR.glob("*.json")):
        payload = json.loads(read(path))
        if payload.get("schema_version") != 1 or not payload.get("batch_id") or not isinstance(payload.get("reviews"), list):
            raise ValueError(f"{path.relative_to(ROOT)} has an unsupported review-batch schema")
        batches.append({"path": str(path.relative_to(ROOT)), "batch_id": payload["batch_id"], "sha256": sha256(path)})
        for review in payload["reviews"]:
            missing = sorted(REVIEW_REQUIRED_FIELDS - set(review))
            if missing:
                raise ValueError(f"{path.name}: review missing fields {missing}")
            species = review["species"]
            if species not in dex.supported:
                raise ValueError(f"{path.name}: unknown or unsupported reviewed species {species}")
            if species in reviews:
                raise ValueError(f"{species} appears in more than one authored review")
            for field in ("role", "item_context", "doubles_rationale", "reviewed_by", "review_notes"):
                if not isinstance(review[field], str) or not review[field].strip():
                    raise ValueError(f"{species}: review field {field} must be nonempty text")
            for field in ("source_refs", "dependency_notes"):
                if not isinstance(review[field], list) or not review[field] or not all(isinstance(value, str) and value.strip() for value in review[field]):
                    raise ValueError(f"{species}: review field {field} must be a nonempty text list")
            moves = review["moves"]
            legal = dex.legal_moves(species)
            if not isinstance(moves, list) or not (1 <= len(moves) <= 4) or len(moves) != len(set(moves)):
                raise ValueError(f"{species}: authored moves must contain 1-4 distinct entries")
            if any(move not in legal for move in moves):
                raise ValueError(f"{species}: authored review contains a locally illegal move")
            if any(move in UNSAFE_AUTOBUILD_MOVES for move in moves):
                raise ValueError(f"{species}: authored review cannot install an IV/happiness-sensitive move")
            nature = review["nature"]
            if not isinstance(nature, str) or compact(nature.removeprefix("NATURE_")) not in NATURES:
                raise ValueError(f"{species}: authored review has invalid nature {nature}")
            ability = review["ability"]
            if ability not in dex.stats[species].abilities or ability == "ABILITY_NONE":
                raise ValueError(f"{species}: authored review has unavailable ability {ability}")
            if review["suggested_item"] not in item_tokens:
                raise ValueError(f"{species}: unknown advisory item {review['suggested_item']}")
            item = review["suggested_item"]
            move_names = {token_compact(move, "MOVE_") for move in moves}
            if item.endswith("_Z") or "Z_CRYSTAL" in item:
                raise ValueError(f"{species}: authored review cannot depend on a Z item")
            if item == "ITEM_ASSAULT_VEST" and any(
                dex.move_info.get(move, {}).get("split") == "SPLIT_STATUS" for move in moves
            ):
                raise ValueError(f"{species}: Assault Vest review contains a status move")
            if item in {"ITEM_CHOICE_BAND", "ITEM_CHOICE_SPECS", "ITEM_CHOICE_SCARF"} and move_names & (
                PROTECT_MOVES | SETUP_MOVES | RECOVERY_MOVES
            ):
                raise ValueError(f"{species}: Choice-item review contains Protect, setup, or recovery")
            if "sleeptalk" in move_names and "rest" not in move_names:
                raise ValueError(f"{species}: Sleep Talk review has no Rest")
            if "dreameater" in move_names and not move_names & {
                "spore", "sleeppowder", "hypnosis", "lovelykiss", "darkvoid", "yawn"
            }:
                raise ValueError(f"{species}: Dream Eater review has no sleep enabler")
            if "belch" in move_names and "BERRY" not in item:
                raise ValueError(f"{species}: Belch review does not recommend a Berry")
            alias = review.get("alias_of")
            if alias:
                if alias not in reviews:
                    raise ValueError(f"{species}: alias_of must reference an earlier authored review in batch order")
                base = reviews[alias]
                if dex.stats[species] != dex.stats[alias]:
                    raise ValueError(f"{species}: cosmetic alias stats/types/ability slots differ from {alias}")
                if dex.legal_moves(species) != dex.legal_moves(alias):
                    raise ValueError(f"{species}: cosmetic alias legal move pool differs from {alias}")
                for field in ("moves", "nature", "ability"):
                    if review[field] != base[field]:
                        raise ValueError(f"{species}: cosmetic alias {field} differs from {alias}")
            reviews[species] = dict(review, review_batch=payload["batch_id"], review_file=str(path.relative_to(ROOT)))
    return reviews, batches


def choose_source_candidate(
    species: str,
    smogon: dict[str, list[dict]],
    corpus: dict[str, list[dict]],
    selectable: set[str],
    dex: LocalDex,
) -> tuple[dict | None, int]:
    """Rank doubles-first sources, then choose a full or coherent 3/4 set.

    Direct Smogon doubles/VGC sets lead, followed by direct checked-in Showdown
    random-doubles sets. Singles/LC and equivalent-form adaptations remain
    useful scaffold sources but cannot silently outrank direct doubles data.
    """
    origins = [species]
    equivalent = dex.base_equivalent(species)
    if equivalent:
        origins.append(equivalent)
    resolved: list[tuple[tuple, dict]] = []
    rejected_count = 0

    def adapted(candidate: dict, origin: str, moves: list[str], rejected: list[str]) -> dict:
        row = dict(candidate, selected_moves=moves, rejected_moves=rejected)
        if origin != species:
            row["locator"] = dict(row["locator"], adapted_from_equivalent_species=origin)
        return row

    for origin in origins:
        direct = origin == species
        for candidate in smogon.get(origin, []) + corpus.get(origin, []):
            moves, rejected = resolve_published_moves(candidate["moves"], selectable, dex.move_by_name)
            safe_count = sum(token_compact(move, "MOVE_") not in BAD_FALLBACK_MOVES for move in moves)
            if len(moves) != 4 and not (len(moves) == 3 and safe_count == 3):
                rejected_count += 1
                continue
            is_doubles = candidate.get("battle_style") == "doubles"
            family = candidate.get("source_family")
            if direct and is_doubles and family == "smogon":
                source_rank = 600
                scaffold_class = "direct-smogon-doubles"
            elif direct and is_doubles and family == "showdown-random":
                source_rank = 580
                scaffold_class = "direct-showdown-random-doubles"
            elif direct and is_doubles:
                source_rank = 560
                scaffold_class = "direct-other-doubles"
            elif not direct and is_doubles:
                source_rank = 520
                scaffold_class = "equivalent-form-doubles-adaptation"
            elif direct and candidate.get("lc_or_nfe"):
                source_rank = 480
                scaffold_class = "direct-lc-or-nfe-adaptation"
            elif direct:
                source_rank = 460
                scaffold_class = "direct-singles-adaptation"
            elif candidate.get("lc_or_nfe"):
                source_rank = 420
                scaffold_class = "equivalent-form-lc-or-nfe-adaptation"
            else:
                source_rank = 400
                scaffold_class = "equivalent-form-singles-adaptation"
            completeness = 100 if len(moves) == 4 else 0
            row = adapted(candidate, origin, moves, rejected)
            row["scaffold_class"] = scaffold_class
            row["source_direct"] = direct
            resolved.append(((source_rank + completeness, candidate.get("priority", ())), row))
    if not resolved:
        return None, rejected_count
    resolved.sort(key=lambda entry: entry[0], reverse=True)
    return resolved[0][1], rejected_count


def build_presets(
    dex: LocalDex,
    smogon: dict[str, list[dict]],
    corpus: dict[str, list[dict]],
    reviews: dict[str, dict],
) -> list[dict]:
    overrides = load_overrides()
    unknown_overrides = sorted(set(overrides) - set(dex.supported))
    if unknown_overrides:
        raise ValueError(f"unknown or unsupported override species: {unknown_overrides}")
    result: list[dict] = []
    for species in dex.supported:
        legal = dex.legal_moves(species)
        if not legal:
            raise ValueError(f"{species} has no legal moves")
        selectable = legal - UNSAFE_AUTOBUILD_MOVES
        if not selectable:
            selectable = legal
        chosen: dict | None = None
        rejected_candidates = 0
        review = reviews.get(species)
        if review:
            chosen = {
                "kind": "authored-review",
                "locator": {
                    "batch_id": review["review_batch"], "file": review["review_file"],
                    "source_refs": review["source_refs"],
                },
                "selected_moves": list(review["moves"]), "rejected_moves": [],
                "nature": review["nature"], "ability": review["ability"],
                "item": review["suggested_item"], "dependencies": review["dependency_notes"],
                "review": review,
            }
        override = overrides.get(species)
        if chosen is None and override:
            moves = [str(move) for move in override.get("moves", [])]
            if len(moves) != len(set(moves)) or any(move not in selectable for move in moves):
                raise ValueError(f"{species} override contains duplicate or illegal moves")
            chosen = {
                "kind": "explicit-override", "locator": {"file": str(OVERRIDES.relative_to(ROOT)), "reason": override.get("reason", "")},
                "selected_moves": moves, "rejected_moves": [], "nature": override.get("nature"), "ability": override.get("ability"),
            }
        if chosen is None:
            chosen, rejected = choose_source_candidate(species, smogon, corpus, selectable, dex)
            rejected_candidates += rejected
        if chosen is None:
            chosen = {
                "kind": "deterministic-local-fallback", "locator": {"basis": "local stats and all-legal-moves union"},
                "selected_moves": fallback_moves(species, selectable, dex), "rejected_moves": [], "nature": None, "ability": None,
            }
        moves = list(chosen["selected_moves"])
        if len(moves) < 4 and chosen["kind"] not in {"explicit-override", "authored-review"}:
            moves = fallback_moves(
                species, selectable, dex, seed=moves,
                ability_hint=next(iter(choose_option(chosen.get("ability"))), None),
            )
        raw_nature = chosen.get("nature")
        nature = raw_nature if isinstance(raw_nature, str) and raw_nature.startswith("NATURE_") else published_nature(raw_nature)
        nature = nature or fallback_nature(dex.stats[species], moves, dex)
        raw_ability = chosen.get("ability")
        if isinstance(raw_ability, str) and raw_ability.startswith("ABILITY_"):
            ability = raw_ability
            ability_slot = dex.stats[species].abilities.index(ability)
            ability_exact = True
        else:
            ability_slot, ability, ability_exact = dex.ability_slot(species, next(iter(choose_option(raw_ability)), None))
        padded_moves = moves + ["MOVE_NONE"] * (4 - len(moves))
        adaptations = []
        if chosen.get("rejected_moves"):
            adaptations.append("published move alternatives absent from the local legal pool were discarded")
        if not published_nature(chosen.get("nature")):
            adaptations.append("nature selected deterministically from the retained local role")
        if chosen.get("ability") and not ability_exact:
            adaptations.append("published ability is unavailable locally; selected the strongest local slot")
        if len(moves) < 4:
            if chosen["kind"] == "authored-review":
                adaptations.append("authored review intentionally leaves the remaining move slots empty")
            elif chosen["kind"] == "explicit-override":
                adaptations.append("explicit competitive override intentionally leaves the remaining move slots empty")
            else:
                adaptations.append(f"only {len(moves)} safe distinct moves exist in the local all-legal-moves union")
        result.append({
            "species_id": dex.values[species], "species": species, "display_name": dex.names.get(species, species.removeprefix("SPECIES_").title()),
            "moves": padded_moves, "move_count": len(moves), "legal_move_count": len(legal),
            "autobuild_move_count": len(selectable),
            "nature": nature, "ability": ability, "ability_slot": ability_slot,
            "source_kind": chosen["kind"], "source_locator": chosen["locator"],
            "source_context": {
                "published_moves": chosen.get("published_moves"),
                "published_item": chosen.get("item"),
                "dependency_notes": chosen.get("dependencies", []),
                "battle_style": chosen.get("battle_style"),
                "scaffold_class": chosen.get("scaffold_class"),
                "source_direct": chosen.get("source_direct"),
            },
            "review_status": "authored" if review else "unaudited-draft",
            "authored_review": review,
            "adaptations": adaptations, "rejected_candidate_count": rejected_candidates,
        })
    return result


def render_header(presets: Iterable[dict]) -> str:
    lines = [
        "// Generated by scripts/verdant_battle_set_presets.py. Do not edit by hand.",
        "const struct VerdantBattleSetPreset gVerdantBattleSetPresets[NUM_SPECIES] =",
        "{",
    ]
    for preset in presets:
        lines.extend([
            f"    [{preset['species']}] =",
            "    {",
            "        .moves = {" + ", ".join(preset["moves"]) + "},",
            f"        .nature = {preset['nature']},",
            f"        .abilitySlot = {preset['ability_slot']},",
            "    },",
        ])
    lines.extend(["};", ""])
    return "\n".join(lines)


def write_review_packet(args: argparse.Namespace) -> None:
    if not MANIFEST.exists():
        raise ValueError("generate the draft manifest before emitting review packets")
    paths = {generation: getattr(args, f"smogon_gen{generation}") for generation in range(4, 10)}
    missing = [f"gen{generation}" for generation, path in paths.items() if path is None or not path.is_file()]
    if missing:
        raise ValueError(f"review packets require the raw Smogon snapshots for complete doubles candidates; missing={missing}")
    dex = LocalDex()
    reviews, _ = load_reviews(dex)
    smogon = smogon_candidates(paths, dex)
    corpus = corpus_candidates(dex)
    manifest = json.loads(read(MANIFEST))
    draft_by_species = {preset["species"]: preset for preset in manifest["presets"]}
    remaining = [species for species in dex.supported if species not in reviews]
    if args.review_id_min is not None:
        remaining = [species for species in remaining if dex.values[species] >= args.review_id_min]
    if args.review_id_max is not None:
        remaining = [species for species in remaining if dex.values[species] <= args.review_id_max]
    start = args.review_start
    count = args.review_count if args.review_count is not None else len(remaining) - start
    selected = remaining[start:start + count]
    rows = []
    for species in selected:
        stats = dex.stats[species]
        legal = dex.legal_moves(species)
        safe_legal = legal - UNSAFE_AUTOBUILD_MOVES
        direct_doubles = []
        for candidate in smogon.get(species, []) + corpus.get(species, []):
            if candidate.get("battle_style") != "doubles":
                continue
            resolved, rejected = resolve_published_moves(candidate["moves"], safe_legal, dex.move_by_name)
            direct_doubles.append({
                "source_kind": candidate["kind"],
                "source_family": candidate.get("source_family"),
                "source_locator": candidate["locator"],
                "published_moves": candidate.get("published_moves"),
                "locally_retained_moves": resolved,
                "rejected_move_slots": rejected,
                "nature": candidate.get("nature"),
                "ability": candidate.get("ability"),
                "item": candidate.get("item"),
                "dependency_notes": candidate.get("dependencies", []),
            })
        draft = draft_by_species[species]
        rows.append({
            "species": species,
            "display_name": draft["display_name"],
            "local": {
                "stats": {
                    "hp": stats.hp, "attack": stats.attack, "defense": stats.defense,
                    "speed": stats.speed, "sp_attack": stats.sp_attack, "sp_defense": stats.sp_defense,
                },
                "types": [stats.type1, stats.type2],
                "abilities": list(stats.abilities),
                "legal_moves": sorted(legal),
                "safe_autobuild_moves": sorted(safe_legal),
            },
            "current_unaudited_draft": draft,
            "compatible_direct_doubles_candidates": direct_doubles,
            "review_template": {
                "species": species,
                "role": "",
                "moves": [move for move in draft["moves"] if move != "MOVE_NONE"],
                "nature": draft["nature"],
                "ability": draft["ability"],
                "suggested_item": "ITEM_NONE",
                "item_context": "",
                "source_refs": [],
                "doubles_rationale": "",
                "dependency_notes": [],
                "reviewed_by": "",
                "review_notes": "",
            },
        })
    packet = {
        "schema_version": 1,
        "purpose": "Authorship packet; copy completed review_template objects into a unique docs/battle_set_reviews batch.",
        "remaining_population_in_scope": len(remaining),
        "scope": {"species_id_min": args.review_id_min, "species_id_max": args.review_id_max},
        "slice": {"start": start, "count": len(selected), "end_exclusive": start + len(selected)},
        "species": rows,
    }
    args.review_packet.parent.mkdir(parents=True, exist_ok=True)
    args.review_packet.write_text(json_dump(packet))
    print(f"wrote review packet {args.review_packet}: {len(selected)} species, remaining in scope {len(remaining)}")


def generate(args: argparse.Namespace) -> dict:
    paths = {generation: getattr(args, f"smogon_gen{generation}") for generation in range(4, 10)}
    missing = [str(path) for path in paths.values() if path is None or not path.is_file()]
    if missing:
        raise ValueError(f"--generate requires readable Gen 4-9 Smogon snapshots; missing={missing}")
    dex = LocalDex()
    reviews, review_batches = load_reviews(dex)
    presets = build_presets(dex, smogon_candidates(paths, dex), corpus_candidates(dex), reviews)
    source_counts = Counter(preset["source_kind"] for preset in presets)
    scaffold_counts = Counter(preset["source_context"].get("scaffold_class") for preset in presets)
    scaffold_counts.pop(None, None)
    direct_doubles_count = sum(
        count for name, count in scaffold_counts.items()
        if name.startswith("direct-") and name.endswith("doubles")
    )
    exclusions = Counter(dex.excluded.values())
    manifest = {
        "schema_version": 1,
        "generator_version": GENERATOR_VERSION,
        "description": "Draft scaffolding plus authored reviews for one legal default per player-selectable species/form. Only entries marked authored are release-reviewed.",
        "source_priority": ["explicit-override", "smogon-published-set", "checked-in-competitive-corpus", "deterministic-local-fallback"],
        "source_snapshots": {
            f"smogon_gen{generation}": {"path_hint": path.name, "sha256": sha256(path), "url": f"https://pkmn.github.io/smogon/data/sets/gen{generation}.json"}
            for generation, path in paths.items()
        },
        "competitive_corpus": {"path": str(INDEX.relative_to(ROOT)), "sha256": sha256(INDEX)},
        "review_batches": review_batches,
        "num_species": dex.num_species,
        "supported_count": len(dex.supported),
        "excluded_count": len(dex.excluded),
        "authored_review_count": len(reviews),
        "remaining_review_count": len(dex.supported) - len(reviews),
        "remaining_review_species": [species for species in dex.supported if species not in reviews],
        "source_counts": dict(sorted(source_counts.items())),
        "scaffold_class_counts": dict(sorted(scaffold_counts.items())),
        "direct_doubles_scaffold_count": direct_doubles_count,
        "exclusion_counts": dict(sorted(exclusions.items())),
        "excluded": [{"species_id": dex.values[species], "species": species, "reason": reason} for species, reason in sorted(dex.excluded.items(), key=lambda row: dex.values[row[0]])],
        "under_four_moves": [preset["species"] for preset in presets if preset["move_count"] < 4],
        "presets": presets,
    }
    MANIFEST.write_text(json_dump(manifest))
    GENERATED.write_text(render_header(presets))
    return manifest


def validate_manifest() -> list[str]:
    problems: list[str] = []
    if not MANIFEST.exists() or not GENERATED.exists():
        return ["generated manifest or C table is missing"]
    dex = LocalDex()
    reviews, review_batches = load_reviews(dex)
    manifest = json.loads(read(MANIFEST))
    presets = manifest.get("presets", [])
    by_species = {preset.get("species"): preset for preset in presets}
    if len(presets) != len(by_species):
        problems.append("preset manifest contains duplicate species")
    expected = set(dex.supported)
    actual = set(by_species)
    if expected != actual:
        problems.append(f"supported coverage differs: missing={sorted(expected - actual)}, extra={sorted(actual - expected)}")
    if manifest.get("num_species") != dex.num_species:
        problems.append(f"NUM_SPECIES drifted: manifest={manifest.get('num_species')} source={dex.num_species}")
    if manifest.get("supported_count") != len(dex.supported):
        problems.append("supported_count does not match current source")
    if manifest.get("excluded_count") != len(dex.excluded):
        problems.append("excluded_count does not match current source")
    if manifest.get("authored_review_count") != len(reviews):
        problems.append("authored_review_count does not match review batches")
    if manifest.get("remaining_review_count") != len(dex.supported) - len(reviews):
        problems.append("remaining_review_count does not match review batches")
    if manifest.get("review_batches") != review_batches:
        problems.append("review batch identity or hash differs from the manifest")
    if "MOVE_SOLAR_BEAM" not in dex.tm_moves.get("SPECIES_BULBASAUR", set()):
        problems.append("TM slot parser no longer resolves TM22_SOLARBEAM to MOVE_SOLAR_BEAM")
    expected_scaffold_counts = Counter(
        preset.get("source_context", {}).get("scaffold_class") for preset in presets
        if preset.get("source_context", {}).get("scaffold_class")
    )
    if manifest.get("scaffold_class_counts") != dict(sorted(expected_scaffold_counts.items())):
        problems.append("scaffold_class_counts does not match preset provenance")
    expected_direct_doubles = sum(
        count for name, count in expected_scaffold_counts.items()
        if name.startswith("direct-") and name.endswith("doubles")
    )
    if manifest.get("direct_doubles_scaffold_count") != expected_direct_doubles:
        problems.append("direct_doubles_scaffold_count does not match preset provenance")
    for species in sorted(expected & actual, key=lambda name: dex.values[name]):
        preset = by_species[species]
        legal = dex.legal_moves(species)
        selectable = legal - UNSAFE_AUTOBUILD_MOVES
        if not selectable:
            selectable = legal
        moves = [move for move in preset.get("moves", []) if move != "MOVE_NONE"]
        if len(preset.get("moves", [])) != 4:
            problems.append(f"{species}: runtime record does not contain four move slots")
        if len(moves) != len(set(moves)):
            problems.append(f"{species}: nonempty moves are not distinct")
        illegal = sorted(set(moves) - legal)
        if illegal:
            problems.append(f"{species}: illegal moves {illegal}")
        unsafe = sorted(set(moves) & UNSAFE_AUTOBUILD_MOVES)
        if unsafe and len(selectable) >= len(moves):
            problems.append(f"{species}: avoidable IV/happiness-sensitive defaults {unsafe}")
        if len(selectable) >= 4 and len(moves) != 4:
            problems.append(f"{species}: has {len(selectable)} safe legal moves but preset uses only {len(moves)}")
        nature = preset.get("nature", "")
        if not isinstance(nature, str) or compact(nature.removeprefix("NATURE_")) not in NATURES:
            problems.append(f"{species}: invalid nature {nature}")
        slot = preset.get("ability_slot")
        if not isinstance(slot, int) or slot not in range(3) or dex.stats[species].abilities[slot] == "ABILITY_NONE":
            problems.append(f"{species}: invalid ability slot {slot}")
        elif preset.get("ability") != dex.stats[species].abilities[slot]:
            problems.append(f"{species}: ability token and slot disagree")
        review = reviews.get(species)
        if review:
            if preset.get("review_status") != "authored" or preset.get("authored_review") != review:
                problems.append(f"{species}: authored review was not preserved exactly in the manifest")
            if moves != review["moves"] or preset.get("nature") != review["nature"] or preset.get("ability") != review["ability"]:
                problems.append(f"{species}: runtime preset differs from its authored review")
        elif preset.get("review_status") != "unaudited-draft" or preset.get("authored_review") is not None:
            problems.append(f"{species}: unaudited scaffold is mislabeled as reviewed")
        source_context = preset.get("source_context")
        if not isinstance(source_context, dict) or "dependency_notes" not in source_context or "published_item" not in source_context:
            problems.append(f"{species}: source item/dependency context is missing")
        if preset.get("review_status") == "unaudited-draft" and preset.get("source_kind") not in {"deterministic-local-fallback", "explicit-override"}:
            if not source_context.get("scaffold_class"):
                problems.append(f"{species}: sourced draft lacks a scaffold class")
        if preset.get("source_kind") == "deterministic-local-fallback":
            selectable = legal - UNSAFE_AUTOBUILD_MOVES
            safe_pool = {move for move in selectable if token_compact(move, "MOVE_") not in BAD_FALLBACK_MOVES}
            bad_defaults = [move for move in moves if token_compact(move, "MOVE_") in BAD_FALLBACK_MOVES]
            if len(safe_pool) >= 4 and bad_defaults:
                problems.append(f"{species}: fallback uses avoidable conditional moves {bad_defaults}")
            ability_name = token_compact(preset["ability"], "ABILITY_")
            move_names = {token_compact(move, "MOVE_") for move in moves}
            if ability_name == "truant" and move_names & PROTECT_MOVES:
                problems.append(f"{species}: Truant fallback wastes its active turn on Protect")
            if ability_name == "prankster":
                useful_status = {
                    move for move in selectable
                    if dex.move_info.get(move, {}).get("split") == "SPLIT_STATUS"
                    and token_compact(move, "MOVE_") not in PROTECT_MOVES | BAD_FALLBACK_MOVES
                }
                chosen_status = {
                    move for move in moves
                    if dex.move_info.get(move, {}).get("split") == "SPLIT_STATUS"
                    and token_compact(move, "MOVE_") not in PROTECT_MOVES
                }
                if useful_status and not chosen_status:
                    problems.append(f"{species}: Prankster fallback does not include useful priority status")
            if ability_name in {"protean", "libero"}:
                available_types = {
                    dex.move_info.get(move, {}).get("type") for move in selectable
                    if damage_score(move, dex.stats[species], dex.move_info) > 0
                    and token_compact(move, "MOVE_") not in BAD_FALLBACK_MOVES
                }
                selected_types = {
                    dex.move_info.get(move, {}).get("type") for move in moves
                    if damage_score(move, dex.stats[species], dex.move_info) > 0
                }
                if len(available_types) >= 3 and len(selected_types) < 3:
                    problems.append(f"{species}: Protean/Libero fallback lacks three-type attack diversity")
            if move_names & SPECIAL_SETUP_MOVES and preset["nature"] in {"NATURE_ADAMANT", "NATURE_JOLLY", "NATURE_IMPISH", "NATURE_CAREFUL"}:
                problems.append(f"{species}: special setup fallback uses a Special Attack-lowering nature")
            if move_names & PHYSICAL_SETUP_MOVES and preset["nature"] in {"NATURE_MODEST", "NATURE_TIMID", "NATURE_BOLD", "NATURE_CALM"}:
                problems.append(f"{species}: physical setup fallback uses an Attack-lowering nature")
    expected_header = render_header(sorted(presets, key=lambda row: row["species_id"]))
    if read(GENERATED) != expected_header:
        problems.append("generated C table differs from the checked-in manifest")
    return problems


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--generate", action="store_true")
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--strict-reviewed", action="store_true", help="fail unless every supported species/form has an authored review")
    for generation in range(4, 10):
        parser.add_argument(f"--smogon-gen{generation}", type=Path)
    parser.add_argument("--review-packet", type=Path, help="write a self-contained authorship packet")
    parser.add_argument("--review-start", type=int, default=0, help="zero-based slice within currently unreviewed species")
    parser.add_argument("--review-count", type=int, help="number of unreviewed species to include")
    parser.add_argument("--review-id-min", type=int, help="stable inclusive minimum local species ID")
    parser.add_argument("--review-id-max", type=int, help="stable inclusive maximum local species ID")
    args = parser.parse_args()
    if args.review_start < 0 or (args.review_count is not None and args.review_count < 1):
        parser.error("review packet start/count must be nonnegative with count at least 1")
    if args.review_id_min is not None and args.review_id_min < 1:
        parser.error("review-id-min must be at least 1")
    if args.review_id_max is not None and args.review_id_max < 1:
        parser.error("review-id-max must be at least 1")
    if args.review_id_min is not None and args.review_id_max is not None and args.review_id_min > args.review_id_max:
        parser.error("review-id-min cannot exceed review-id-max")
    if args.generate:
        manifest = generate(args)
        print(
            "generated battle-set presets: "
            f"{manifest['supported_count']} supported / {manifest['num_species'] - 1} nonzero IDs; "
            f"authored={manifest['authored_review_count']} remaining={manifest['remaining_review_count']}; "
            f"direct-doubles scaffold={manifest['direct_doubles_scaffold_count']}; "
            f"sources={manifest['source_counts']}; trailing-empty exceptions={manifest['under_four_moves']}"
        )
    if args.check or not args.generate:
        problems = validate_manifest()
        if problems:
            for problem in problems:
                print(f"FAIL: {problem}")
            raise SystemExit(1)
        manifest = json.loads(read(MANIFEST))
        if args.strict_reviewed and manifest["remaining_review_count"]:
            raise SystemExit(
                f"FAIL: release review gate is incomplete: {manifest['authored_review_count']} authored, "
                f"{manifest['remaining_review_count']} remaining"
            )
        print(
            f"PASS: {manifest['supported_count']} supported presets cover every selectable species/form; "
            f"{len(manifest['under_four_moves'])} justified trailing-empty exceptions; "
            f"AUTHORED {manifest['authored_review_count']}/{manifest['supported_count']} "
            f"(draft remaining {manifest['remaining_review_count']})"
        )
    if args.review_packet:
        write_review_packet(args)


if __name__ == "__main__":
    main()
