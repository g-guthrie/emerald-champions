#!/usr/bin/env python3
"""Upper-bound availability pools per campaign milestone, the story-ordered
encounter work list, and the player-rule defaults the tuning playtests use.

Why: every campaign trainer is re-tuned by playing it with the strongest team
a near-optimal player could legally own at that moment. This script answers
"what could they own" per level-cap milestone, never granting anything before
its real gate. It is an upper bound: a source counts once any save state in
the milestone's window can reach it (missables, RNG, money and time are not
limiting), but nothing is counted before the story/field-move/key-item gate
the game source enforces.

Model
-----
- Milestone windows and reachability: scripts/mega_register.py `Story`
  (tile-level map graph per src/caps.c window, story events, field-move
  licences, scripted transport, generic load-script walls). A window is every
  save state whose campaign cap equals that milestone's cap, so it includes
  optional detours: e.g. Winona is optional until the League door
  (src/caps.c:41-45 comment; no badge-6 check at Mt Pyre, the hideouts or
  Juan, SootopolisCity_Gym_1F/scripts.inc:72-74), which puts everything up to
  the Mossdeep Gym inside the badge-5 window.
- Species sources: wild tables (src/data/wild_encounters.json; land/water/
  rock smash/rods/honey) with IsWildSlotLive legend gating
  (src/wild_encounter.c:61-64, src/legendary_signs.c:91-104,292-323,407-413),
  storm visitors, cut-tree habitat, Feebas, starters, Game Corner, scripted
  gifts/eggs, the Day-Care gift eggs, NPC trades, fossils, static encounters,
  breeding, evolutions (Leveler to cap, Bonding friendship, items, location),
  out-of-battle form items, Megas (stone + Mega Ring), Primal/relic forms.
- Item sources: Battle Vendor starter kit and its restock rule, badge-tier
  and specialty marts, item balls, hidden items, scripted gifts, wild held
  items, berries, Mega Stones (world and Norman's starter stones), the Form
  Items shelf, legendary relics.
- Encounters: every branch of data/emerald_champions/emerald_champions_battle_teams.txt
  located at the map script that starts it, with the windows in which it can
  be fought (a battle whose victory is itself a window's milestone, or leads
  to it, is not fightable in that window).

Outputs go to work/tuning-20260930/ (git-ignored): pools/pool-<milestone>.json,
pools/pool-<milestone>.txt, encounters.json, sanity.txt.

  python3 scripts/reference_pool.py            # regenerate everything
  python3 scripts/reference_pool.py --explain MapDir
"""
from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

import emerald_champions_teams as teams
import evolution_move_gate
import mega_register as mr
import walkthrough_order
from item_catalog import battle_item_categories
from verify_trainer_ability_legality import preprocess_species_info, resolve_species, species_aliases

ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "work/tuning-20260930"
WILD = ROOT / "src/data/wild_encounters.json"
TRADE_H = ROOT / "src/data/trade.h"
MAPS_DIR = ROOT / "data/maps"
SCRIPTS_DIR = ROOT / "data/scripts"
FIELD_SPECIALS = ROOT / "src/field_specials.c"
MASTER = ROOT / "data/emerald_champions/emerald_champions_master_battle_design.txt"
REVIEW_INDEX = ROOT / "data/emerald_champions/trainer-review-index.json"

sys.path.insert(0, str(ROOT / "scripts/playthrough"))


def rel(path: Path) -> str:
    return str(path.relative_to(ROOT))


def line_of(path: Path, needle: str) -> int:
    for n, line in enumerate(path.read_text().splitlines(), 1):
        if needle in line:
            return n
    return 0


def cite(path: Path, needle: str) -> str:
    return f"{rel(path)}:{line_of(path, needle)}"


# ---------------------------------------------------------------------------
# Player rules (defaults every tuning party may assume), with citations
# ---------------------------------------------------------------------------

def player_rules() -> dict:
    pc = ROOT / "src/pokemon.c"
    fs = FIELD_SPECIALS
    ec = SCRIPTS_DIR / "emerald_champions.inc"
    return {
        "ivs": {"rule": "31 in every stat for every Pokemon the player owns (catch, gift, starter, trade, egg).",
                "cite": [cite(pc, "void MaxPlayerMonIVs"), cite(pc, "GiveMonToPartyOrPC"), cite(ROOT / "src/trade.c", "MaxPlayerMonIVs")]},
        "evs": {"rule": "Any legal spread (510 total, 252 max) re-planned at every Pokemon Center tutor for a flat 500 "
                        "(free when unchanged); new arrivals come with 252 HP / 4 Def / 2 SpD / 252 Spe. Battles give no EVs. "
                        "Before the Knuckle Badge (cap 30) no Pokemon - player or trainer - has any EVs (src/caps.c AreEVsUnlocked).",
                "cite": [cite(ec, "EmeraldChampions_EventScript_EVTraining"), cite(ROOT / "include/constants/field_specials.h", "EV_PLAN_FEE"),
                         cite(ROOT / "include/config/caps.h", "B_EV_CAP_TYPE")]},
        "nature": {"rule": "Any nature. Before Slateport it depends on the catch/gift roll (repeatable); from Slateport the "
                           "nature chef rewrites the nature for one daily Berry (Cheri/Chesto/Pecha/Leppa/Oran). Mints are never given.",
                   "cite": [rel(MAPS_DIR / "SlateportCity_NameRatersHouse/scripts.inc"), cite(ROOT / "src/inclement_stat_services.c", "ChangePokemonNature")]},
        "ability": {"rule": "Any Ability the party menu offers: slots 0/1, the Inclement added slots 3-4 and the hidden slot 2; free, no gate.",
                    "cite": [cite(pc, "GetMonSelectableAbilitySlots"), cite(ROOT / "src/party_menu.c", "MENU_OPEN_ABILITY"),
                             rel(ROOT / "src/data/pokemon/inclement_layer.h")]},
        "moves": {"rule": "Any move in the species' preparation learnset (every generation's level-up/TM/tutor/egg/event moves plus "
                          "reviewed extensions and authored set moves) from any Center tutor, free, no stage gate. No TMs.",
                  "cite": [cite(ec, "MOVE_RELEARNER_ALL_MOVES"), cite(ROOT / "src/move_relearner.c", "IsAllMoveRelearnerActive"),
                           cite(ROOT / "src/emerald_champions_battle_sets.c", "BuildEmeraldChampionsPreparationMoveAccess")]},
        "level": {"rule": "Exactly the milestone cap: the Leveler raises the party to the cap; nothing exceeds it; no battle EXP.",
                  "cite": [cite(ROOT / "src/caps.c", "sCampaignMilestones"), cite(pc, "RaiseMonToLevelerTarget"),
                           cite(ROOT / "src/battle_script_commands.c", "B_SCR_OP_UNUSED_GETEXP")]},
        "evolution": {"rule": "Level evolutions at or below the cap (Leveler triggers them); friendship evolutions via Center Bonding "
                              "(sets 160; opens with the first badge; tuning assumes friendship evolutions as soon as the base is legal); item evolutions once the item is buyable/found; trade evolutions only through their "
                              "EVO_ITEM alternative (Linking Cord or the held item used as an item).",
                      "cite": [cite(pc, "IsMonEligibleForLeveler"), cite(fs, "ApplyEmeraldChampionsBonding"),
                               "src/data/pokemon/species_info/gen_1_families.h (EVO_ITEM ITEM_LINKING_CORD)"]},
        "pokerus": {"rule": "Pokerus adds +5 points to a beneficial nature (+15%); Lavaridge hot-spring treatment makes it +15%/+5%. "
                            "Obtainable from the start (DexNav chains, random infection); the spring needs Lavaridge.",
                    "cite": [cite(ROOT / "src/pokerus.c", "GetPokerusNatureModifier"), cite(ROOT / "src/dexnav.c", "Pokerus"),
                             rel(MAPS_DIR / "LavaridgeTown/scripts.inc")]},
        "restricted": {"rule": "At most ONE restricted Pokemon per party: Legendary/Mythical/Ultra Beast/Paradox. Official classes use the base species flags: "
                               "isUltraBeast / isRestrictedLegendary / isSubLegendary / isMythical / isParadox.",
                       "cite": [cite(pc, "GetRestrictedPartyClass"), cite(pc, "PlayerPartyWithinRestrictedLimit")]},
        "legend_level": {"rule": "Legend-class wild/static encounters arrive at the current cap with authored sets.",
                         "cite": [cite(ROOT / "src/legendary_signs.c", "GetLegendaryEncounterLevel")]},
        "items": {"rule": "No Item Clause for the player; any number of duplicates of a Battle Vendor catalogue item once it "
                          "has reached the Bag/PC or joined on a Pokemon (then sold without limit).",
                  "cite": [cite(ROOT / "src/emerald_champions_story.c", "no Item Clause"), cite(fs, "IsEmeraldChampionsBattleItemUnlocked")]},
        "gimmicks": {"rule": "Mega Evolution only (Mega Ring in the Bag), using the one restricted slot: a restricted member can Mega Evolve itself; otherwise choose one Mega or one restricted member. Z-Moves/Dynamax/Ultra Burst blocked; Terastallization does not exist.",
                     "cite": [cite(ROOT / "include/constants/emerald_champions.h", "EMERALD_CHAMPIONS_MEGA_ONLY"),
                              cite(ROOT / "src/battle_util.c", "ITEM_MEGA_RING")]},
    }


def friendship_model(story: mr.Story) -> dict:
    """Friendship facts and the per-window maximum. B_AFFECTION_MECHANICS is
    FALSE (owner, Sept 30 2026), so friendship grants no battle luck. Center Bonding sets any party Pokemon to 0, 160 or 255 for free,
    after the first badge. Tuning assumes walking reaches evolution friendship
    as soon as the base form is legal, including the start window."""
    ec = SCRIPTS_DIR / "emerald_champions.inc"
    oldale = MAPS_DIR / "OldaleTown_PokemonCenter_1F/map.json"
    return {
        "max": 255,
        "recommended": 255,
        "why": "Center tutor 'Bonding' sets friendship to 0 / 160 / 255 on demand, free and repeatable, once the first "
               "Gym Badge is won; the tutor stands in every Pokemon Center 1F from Oldale (no hide flag).",
        "starting_values": {
            "catch_gift_starter": "species base friendship (most species STANDARD_FRIENDSHIP 50; legends often 0)",
            "friend_ball": 150, "egg": 120, "trade": 70,
        },
        "ways_to_raise": [
            {"how": "Center tutor Bonding (0/160/255)", "from": "badge1",
             "cite": [cite(ec, "Common_EventScript_EmeraldChampionsBonding"), cite(FIELD_SPECIALS, "ApplyEmeraldChampionsBonding"),
                      cite(oldale, "Common_EventScript_EmeraldChampionsMoveTutor")]},
            {"how": "walking (+1 per 128 steps, 50%)", "cite": [cite(ROOT / "src/field_control_avatar.c", "UpdateFriendshipStepCounter")]},
            {"how": "Gym/E4/Champion battles, X items", "cite": [cite(ROOT / "src/battle_main.c", "FRIENDSHIP_EVENT")]},
            {"how": "EV-reducing Berries (Route 104 flower shop daily bundle)", "cite": [cite(FIELD_SPECIALS, "GiveFlowerShopBerryBundle")]},
            {"how": "Soothe Bell x1.5 (Route 104 item ball, Slateport Fan Club), Luxury Ball +1", "cite": [rel(MAPS_DIR / "Route104/map.json")]},
        ],
        "affection": {"effects": "none: affection battle effects are disabled",
                      "cite": [cite(ROOT / "include/config/battle.h", "B_AFFECTION_MECHANICS")]},
        "per_window": {w: 255 for w in story.window_names()},
    }


# ---------------------------------------------------------------------------
# Species data (preprocessed gSpeciesInfo + Inclement layer)
# ---------------------------------------------------------------------------

_TOKEN = re.compile(r"\s*(\d+|[A-Za-z_]\w*|==|!=|<=|>=|&&|\|\||[-+*/%()<>?:!,])")


def c_int(expr: str) -> int | None:
    """Evaluate the small integer C expressions the preprocessed species
    table uses for configurable stats (literals, arithmetic, comparisons,
    && || !, ternaries, min/max)."""
    tokens, pos, expr = [], 0, expr.strip()
    while pos < len(expr):
        m = _TOKEN.match(expr, pos)
        if not m:
            return None
        tokens.append(m[1])
        pos = m.end()
    i = 0

    def peek():
        return tokens[i] if i < len(tokens) else None

    def take():
        nonlocal i
        i += 1
        return tokens[i - 1]

    def primary():
        t = take()
        if t == "(":
            v = ternary()
            take()
            return v
        if t == "!":
            return int(not primary())
        if t == "-":
            return -primary()
        if t in ("min", "max"):
            take()
            a = ternary()
            take()
            b = ternary()
            take()
            return min(a, b) if t == "min" else max(a, b)
        if t.isdigit():
            return int(t)
        raise ValueError(t)

    ops = [("||",), ("&&",), ("==", "!="), ("<", ">", "<=", ">="), ("+", "-"), ("*", "/", "%")]

    def binary(level):
        if level == len(ops):
            return primary()
        v = binary(level + 1)
        while peek() in ops[level]:
            op = take()
            r = binary(level + 1)
            v = {"||": lambda: int(bool(v) or bool(r)), "&&": lambda: int(bool(v) and bool(r)),
                 "==": lambda: int(v == r), "!=": lambda: int(v != r), "<": lambda: int(v < r),
                 ">": lambda: int(v > r), "<=": lambda: int(v <= r), ">=": lambda: int(v >= r),
                 "+": lambda: v + r, "-": lambda: v - r, "*": lambda: v * r,
                 "/": lambda: v // r if r else 0, "%": lambda: v % r if r else 0}[op]()
        return v

    def ternary():
        c = binary(0)
        if peek() == "?":
            take()
            a = ternary()
            take()
            b = ternary()
            return a if c else b
        return c

    try:
        value = ternary()
    except (ValueError, IndexError, TypeError):
        return None
    return value if i == len(tokens) else None


STAT_FIELDS = ("baseHP", "baseAttack", "baseDefense", "baseSpAttack", "baseSpDefense", "baseSpeed")
RESTRICTED_FLAGS = ("isUltraBeast", "isRestrictedLegendary", "isSubLegendary", "isMythical", "isParadox")


def _field(body: str, name: str) -> str | None:
    m = re.search(r"\." + name + r"\s*=\s*(.*?),\s*\.\w+\s*=", body, re.S)
    return m[1].strip() if m else None


class SpeciesData:
    def __init__(self):
        text = preprocess_species_info()
        text = text[text.index("const struct SpeciesInfo gSpeciesInfo[]"):]
        marks = list(re.finditer(r"\[(SPECIES_\w+)\]\s*=\s*\{", text))
        self.info: dict[str, dict] = {}
        for i, m in enumerate(marks):
            body = text[m.end():marks[i + 1].start() if i + 1 < len(marks) else len(text)]
            stats = [c_int(_field(body, f)) if _field(body, f) is not None else None for f in STAT_FIELDS]
            abil = re.search(r"\.abilities\s*=\s*\{([^}]*)\}", body)
            evo_raw = re.search(r"\.evolutions\s*=\s*\(const struct Evolution\[\]\)\s*\{(.*?)\{\s*EVOLUTIONS_END\s*\}", body, re.S)
            evolutions = []
            if evo_raw:
                for em in re.finditer(r"\{\s*(EVO_\w+)\s*,\s*([^,{}]+?)\s*,\s*(SPECIES_\w+)\s*(?:,\s*\(\(const struct EvolutionParam\[\]\)\s*\{(.*?)\{\s*CONDITIONS_END\s*\}\s*\}\))?\s*\}", evo_raw[1], re.S):
                    conds = re.findall(r"\{\s*(IF_\w+)\s*,?\s*([^{}]*?)\s*\}", em[4] or "")
                    evolutions.append((em[1], em[2].strip(), em[3], [(c, a.strip().rstrip(",").strip()) for c, a in conds]))
            name = re.search(r'\.speciesName\s*=\s*_\("([^"]*)"\)', body)
            egg = re.search(r"\.eggGroups\s*=\s*\{\s*(EGG_GROUP_\w+)", body)
            forms = re.search(r"\.formSpeciesIdTable\s*=\s*(\w+)", body)
            items = {k: (re.search(r"\." + k + r"\s*=\s*(ITEM_\w+)", body) or [None, None])[1] for k in ("itemCommon", "itemRare")}
            self.info[m[1]] = dict(
                stats=stats,
                abilities=re.findall(r"ABILITY_[A-Z0-9_]+", abil[1]) if abil else [],
                evolutions=evolutions,
                flags={f for f in RESTRICTED_FLAGS if re.search(r"\." + f + r"\s*=\s*1\b", body)},
                mega=bool(re.search(r"\.isMegaEvolution\s*=\s*1\b", body)),
                name=name[1] if name else m[1],
                egg_group=egg[1] if egg else None,
                form_table=forms[1] if forms else None,
                held=items,
            )
        self.aliases = species_aliases()
        form_text = (ROOT / "src/data/pokemon/form_species_tables.h").read_text()
        tables = {}
        for table, body in re.findall(r"static const u16 (\w+)\[\]\s*=\s*\{(.*?)\};", form_text, re.S):
            members = [self.resolve(s) for s in re.findall(r"SPECIES_\w+", body) if s != "SPECIES_NONE"]
            if members:
                tables[table] = members
        self.base_of = {sp: tables[i["form_table"]][0] for sp, i in self.info.items() if i["form_table"] in tables}
        self.layer = self._inclement_layer()
        self.pre_evo: dict[str, set[str]] = defaultdict(set)
        for sp, info in self.info.items():
            for _m, _p, target, _c in info["evolutions"]:
                self.pre_evo[self.resolve(target)].add(sp)

    @staticmethod
    def _inclement_layer() -> dict[str, dict]:
        text = (ROOT / "src/data/pokemon/inclement_layer.h").read_text()
        out: dict[str, dict] = {}
        for sp, body in re.findall(r"\[(SPECIES_\w+)\]\s*=\s*\{(.*?)\}\s*,\s*(?://[^\n]*)?\n", text):
            stats = re.search(r"INCLEMENT_BASE_STATS\(\s*([^)]*)\)", body)
            added = re.search(r"\.addedAbilities\s*=\s*\{([^}]*)", body)
            out[sp] = dict(stats=[int(x) for x in stats[1].split(",")] if stats else None,
                           abilities=re.findall(r"ABILITY_[A-Z0-9_]+", added[1]) if added else [])
        return out

    def resolve(self, species: str) -> str:
        return resolve_species(species, self.aliases)

    def exists(self, species: str) -> bool:
        return self.resolve(species) in self.info

    def base(self, species: str) -> str:
        sp = self.resolve(species)
        return self.base_of.get(sp, sp)

    def restricted_class(self, species: str) -> str | None:
        """GetRestrictedPartyClass (src/pokemon.c:3079-3094)."""
        flags = self.info.get(self.base(species), {}).get("flags", set())
        if "isUltraBeast" in flags:
            return "ultra_beast"
        if flags & {"isRestrictedLegendary", "isSubLegendary", "isMythical"}:
            return "legendary"
        if "isParadox" in flags:
            return "paradox"
        return None

    def stats(self, species: str) -> list[int]:
        sp = self.resolve(species)
        layer = self.layer.get(sp)
        if layer and layer["stats"]:
            return layer["stats"]
        return [s or 0 for s in self.info.get(sp, {}).get("stats", [0] * 6)]

    def bst(self, species: str) -> int:
        return sum(self.stats(species))

    def abilities(self, species: str) -> list[str]:
        """GetMonSelectableAbilitySlots order: 0, 1, Inclement 3-4, hidden 2."""
        sp = self.resolve(species)
        slots = list(self.info.get(sp, {}).get("abilities", [])) + ["ABILITY_NONE"] * 3
        order = [slots[0], slots[1], *self.layer.get(sp, {}).get("abilities", []), slots[2]]
        return list(dict.fromkeys(a for a in order if a != "ABILITY_NONE"))

    def egg_species(self, species: str) -> str:
        cur = self.resolve(species)
        for _ in range(4):
            pre = sorted(self.pre_evo.get(cur, ()))
            if not pre:
                break
            cur = pre[0]
        return cur

    def family(self, species: str) -> set[str]:
        start = self.resolve(species)
        seen, stack = {start}, [start]
        while stack:
            cur = stack.pop()
            nxt = {self.resolve(t) for _m, _p, t, _c in self.info.get(cur, {}).get("evolutions", [])} | self.pre_evo.get(cur, set())
            for n in nxt - seen:
                seen.add(n)
                stack.append(n)
        return seen

    def name(self, species: str) -> str:
        sp = self.resolve(species)
        n = self.info.get(sp, {}).get("name", sp)
        suffix = re.sub(r"^SPECIES_" + re.escape(n.upper().replace(" ", "_").replace(".", "").replace("'", "")), "", sp)
        return n + (f" ({suffix.strip('_').replace('_', ' ').title()})" if suffix and suffix != sp else "")


# ---------------------------------------------------------------------------
# Script index: labels, where they run, and what they give
# ---------------------------------------------------------------------------

LABEL_RE = re.compile(r"^([A-Za-z_]\w*)::?")
JUMP_RE = re.compile(r"^(goto|call|goto_if_\w+|call_if_\w+|case)\b")


class Scripts:
    """Every label in data/maps/*/scripts.inc and data/scripts/*.inc with its
    lines, the labels that jump to it, and the map events that run it."""

    def __init__(self, geo: mr.Geometry):
        self.geo = geo
        self.labels: dict[str, dict] = {}
        files = sorted(MAPS_DIR.glob("*/scripts.inc")) + sorted(SCRIPTS_DIR.glob("*.inc"))
        for path in files:
            cur = None
            for n, raw in enumerate(path.read_text().splitlines(), 1):
                line = raw.split("@")[0]
                if line and not line[0].isspace():
                    m = LABEL_RE.match(line.strip())
                    if m:
                        cur = m[1]
                        self.labels.setdefault(cur, dict(file=path, line=n, body=[],
                                                         map=path.parent.name if path.parent.parent == MAPS_DIR else None))
                        continue
                if cur and line.strip():
                    self.labels[cur]["body"].append((n, line.strip()))
        self.callers: dict[str, set[str]] = defaultdict(set)
        for label, info in self.labels.items():
            body = info["body"]
            for i, (_n, line) in enumerate(body):
                if JUMP_RE.match(line) or line.startswith("trainerbattle"):
                    for tok in re.findall(r"\b([A-Za-z_]\w*)\b", line):
                        if tok in self.labels and tok != label:
                            self.callers[tok].add(label)
            # Fall-through into the next label in the same file.
        by_file: dict[Path, list[tuple[int, str]]] = defaultdict(list)
        for label, info in self.labels.items():
            by_file[info["file"]].append((info["line"], label))
        self.next_label: dict[str, str] = {}
        for items in by_file.values():
            items.sort()
            for (_l1, a), (_l2, b) in zip(items, items[1:]):
                self.next_label[a] = b
                body = self.labels[a]["body"]
                if body and body[-1][1].split()[0] not in ("end", "return", "goto", "releaseall_end", "release_end") \
                        and not body[-1][1].startswith("goto "):
                    self.callers[b].add(a)
        self.entries: dict[str, list[tuple]] = defaultdict(list)
        for d, m in geo.maps.items():
            for o in m.get("object_events") or []:
                if isinstance(o.get("script"), str) and isinstance(o.get("x"), int):
                    self.entries[o["script"]].append((d, o["x"], o["y"], "object", o.get("flag", "0")))
            for c in m.get("coord_events") or []:
                if c.get("script"):
                    self.entries[c["script"]].append((d, c["x"], c["y"], "coord", None))
            for b in m.get("bg_events") or []:
                if b.get("script"):
                    self.entries[b["script"]].append((d, b["x"], b["y"], "bg", None))
        # Map scripts (MAP_SCRIPT_ON_*) run on the whole map.
        for path in sorted(MAPS_DIR.glob("*/scripts.inc")):
            for label in re.findall(r"map_script(?:_2)?\s+MAP_SCRIPT_\w+\s*,\s*(?:\w+\s*,\s*\w+\s*,\s*)?(\w+)", path.read_text()):
                self.entries[label].append((path.parent.name, None, None, "map", None))
        self.default_hidden = set(re.findall(r"setflag\s+(FLAG_\w+)", (SCRIPTS_DIR / "new_game.inc").read_text()))
        self.clearflags: dict[str, list[tuple[str, int]]] = defaultdict(list)
        for label, info in self.labels.items():
            if info["file"].name in ("new_game.inc", "debug.inc"):
                continue
            for n, line in info["body"]:
                m = re.match(r"clearflag\s+(FLAG_\w+)", line)
                if m:
                    self.clearflags[m[1]].append((label, n))
        self._unhide: dict = {}

    def unhide_requirements(self, map_name: str, flag: str, keep, depth: int) -> list[frozenset] | None:
        """Objects hidden at new game (data/scripts/new_game.inc setflag)
        appear once a script runs `clearflag <flag>`: the alternatives are
        that command's own path conditions, plus AT:<map> when it runs on
        another map. None when nothing clears it (left unconstrained)."""
        if flag not in self.default_hidden:
            return [frozenset()]
        key = (map_name, flag)
        if key in self._unhide:
            return self._unhide[key]
        self._unhide[key] = None  # recursion guard
        sites = self.clearflags.get(flag, [])
        if not sites or depth > 2:
            return None
        alts = []
        for label, n in sites:
            for req, where in self.path_requirements(label, n, keep, depth + 1):
                m = where[0] if isinstance(where, tuple) else where
                tokens = set(req) | ({f"AT:{m}"} if m and m != map_name else set())
                alts.append(frozenset(tokens))
        best = [a for a in alts if not any(b < a for b in alts)]
        self._unhide[key] = list(dict.fromkeys(best)) or None
        return self._unhide[key]

    def reach_entries(self, label: str, depth: int = 6) -> tuple[list[tuple], set[str]]:
        """Map events that can run `label` (through goto/call/case/fall
        through), and every label on those paths."""
        seen, frontier, found = {label}, [label], []
        for _ in range(depth):
            nxt = []
            for lab in frontier:
                found += self.entries.get(lab, [])
                for c in self.callers.get(lab, ()):
                    if c not in seen:
                        seen.add(c)
                        nxt.append(c)
            frontier = nxt
        for lab in frontier:
            found += self.entries.get(lab, [])
        return found, seen

    def path_requirements(self, target: str, line_no: int | None = None, keep=None, depth: int = 0) -> list[tuple]:
        """Positive conditions on the script paths from each map event that
        can run `target` down to the command at `line_no`: taken
        goto_if_set F / skipped goto_if_unset F add F; any trainerbattle the
        path passes (or continues from) adds DEFEATED:<trainer>. Negative
        conditions (already received, not yet set) are dropped, so this stays
        an upper bound. Returns [(requirement tokens, (map, x, y) or map)]."""
        keep = keep or (lambda token: True)
        entries, labels = self.reach_entries(target)
        results = []
        for d, x, y, _kind, _flag in entries:
            start = next((lab for lab in labels if (d, x, y, _kind, _flag) in self.entries.get(lab, [])), None)
            if start is None:
                continue
            unhide = self.unhide_requirements(d, _flag, keep, depth) if _kind == "object" else [frozenset()]
            unhide = unhide or [frozenset()]
            best: list[frozenset] = []
            stack = [(start, frozenset(), 0)]
            visited = set()
            while stack:
                label, req, depth = stack.pop()
                if (label, req) in visited or depth > 12 or label not in self.labels:
                    continue
                visited.add((label, req))
                cur = set(req)
                for n, line in self.labels[label]["body"]:
                    if label == target and (line_no is None or n == line_no):
                        found = frozenset(t for t in cur if keep(t))
                        if not any(b <= found for b in best):
                            best = [b for b in best if not found <= b] + [found]
                        break
                    op = line.split()[0]
                    args = [a.strip() for a in line[len(op):].split(",")]
                    if op in ("goto_if_set", "goto_if_unset", "goto_if_defeated", "goto_if_not_defeated",
                              "call_if_set", "call_if_unset", "call_if_defeated", "call_if_not_defeated"):
                        token = args[0] if "set" in op else "DEFEATED:" + args[0]
                        positive = op.endswith(("_if_set", "_if_defeated"))
                        dest = args[-1]
                        stack.append((dest, frozenset(cur | ({token} if positive else set())), depth + 1))
                        if op.startswith("goto"):
                            if not positive:
                                cur.add(token)
                            continue
                        continue
                    if op.startswith(("goto_if", "call_if")) or op == "case":
                        stack.append((args[-1], frozenset(cur), depth + 1))
                        continue
                    if op in ("goto", "call"):
                        stack.append((args[0], frozenset(cur), depth + 1))
                        if op == "goto":
                            break
                        continue
                    if op.startswith("trainerbattle") or op == "multi_2_vs_2":
                        trainers = [a for a in args if a.startswith("TRAINER_")]
                        cur |= {"DEFEATED:" + t for t in trainers}
                        for a in args[1:]:
                            if a in self.labels:
                                stack.append((a, frozenset(cur), depth + 1))
                        continue
                    if op in ("end", "return", "releaseall_end", "release_end", "waitstate_end"):
                        break
                else:
                    # Fell off the label: continue into the next label in the file.
                    nxt = self.next_label.get(label)
                    if nxt:
                        stack.append((nxt, frozenset(cur), depth + 1))
            for b in best:
                for u in unhide:
                    results.append((tuple(sorted(b | {t for t in u if keep(t) or t.startswith("AT:")})),
                                    (d, x, y) if x is not None else d))
        return list(dict.fromkeys(results))


# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------

def split_tokens(tokens) -> tuple[tuple, list[str], list[str]]:
    """(story flags, trainers that must be beaten, maps that must be
    reachable) from a requirement alternative."""
    flags = tuple(t for t in tokens if not t.startswith(("DEFEATED:", "AT:")))
    return flags, [t[9:] for t in tokens if t.startswith("DEFEATED:")], [t[3:] for t in tokens if t.startswith("AT:")]


class Source:
    __slots__ = ("kind", "key", "where", "requires", "detail", "cite", "needs_species", "needs_items", "extra")

    def __init__(self, kind, key, where, requires=None, detail="", cite="", needs_species=(), extra=None, needs_items=()):
        self.kind, self.key, self.where = kind, key, where
        # A list of tokens means "all of them" here.
        self.requires = mr.as_req(tuple(requires) if isinstance(requires, list) else requires)
        self.detail, self.cite = detail, cite
        self.needs_species = tuple(needs_species)
        self.needs_items = tuple(needs_items)
        self.extra = extra or {}

    def as_dict(self):
        where = self.where
        if isinstance(where, tuple):
            where = f"{where[0]} ({where[1]},{where[2]})"
        return dict(kind=self.kind, detail=self.detail, where=where,
                    requires=[list(a) for a in self.requires if a] or None, cite=self.cite)


class Builder:
    def __init__(self):
        self.story = mr.story()
        self.geo = self.story.geo
        self.species = SpeciesData()
        self.scripts = Scripts(self.geo)
        self.gates = {self.species.resolve(k): v for k, v in mr.legendary_gates().items()}
        self.team_trainers = {br.trainer for br in teams.read_teams()}
        self.catalogue = battle_item_categories(ROOT)
        self.vendor_items = {i for items in self.catalogue.values() for i in items}
        self.species_sources: list[Source] = []
        self.item_sources: list[Source] = []
        self._wild_tables()
        self._visitors()
        self._cut_trees()
        self._feebas()
        self._starters()
        self._game_corner()
        self._gifts()
        self._daycare_eggs()
        self._trades()
        self._fossils()
        self._statics()
        self._item_sources()

    # -- helpers ------------------------------------------------------------
    def keep_token(self, token: str) -> bool:
        if token.startswith("DEFEATED:"):
            return token[9:] in self.team_trainers
        return mr.canon(token) in STORY_FLAGS

    def scripted(self, label: str, line: int, extra=None) -> list[tuple[list, object]]:
        """[(requirement tokens, where)] for a command at label:line: the
        positive path conditions from each map event that runs it
        (Scripts.path_requirements), plus a manual override when the script
        tests something the scan cannot (items in the Bag, script vars)."""
        paths = self.scripts.path_requirements(label, line, self.keep_token)
        if not paths:
            m = self.scripts.labels.get(label, {}).get("map")
            paths = [((), m)] if m else []
        add = list((extra or GIFT_REQUIREMENTS.get(label) or ([], ""))[0])
        return [(list(req) + add, where) for req, where in paths]

    def S(self, species: str) -> str:
        return self.species.resolve(species)

    def legend_requirement(self, species: str) -> tuple[list, tuple, str]:
        """(requirement, species needed, note) for a legend-class slot/static:
        MeetsSignProgression + MeetsSignDiscovery (src/legendary_signs.c:292-323)."""
        sp = self.S(species)
        if self.species.restricted_class(sp) is None:
            return [], (), ""
        # GetLegendarySignIdBySpecies (src/legendary_signs.c:250-268): the
        # species itself, else (unless a regional form) its base species.
        row = self.gates.get(sp)
        if row is None and not re.search(r"_(ALOLA|GALAR|HISUI|PALDEA)\b", sp):
            row = self.gates.get(self.species.base(sp))
        if row is None:
            return [], (), "no gate row: live until caught"
        req = [f"BADGES>={row['badges']}"] if row["badges"] else []
        if row["flag"]:
            req.append(row["flag"])
        needs, note = (), f"legendary_signs.h:{row['line']}"
        base = self.species.base(sp)
        if base == "SPECIES_REGIGIGAS":
            needs = ("SPECIES_REGIROCK", "SPECIES_REGICE", "SPECIES_REGISTEEL")
        elif base == "SPECIES_KYUREM":
            needs = ("SPECIES_RESHIRAM|SPECIES_ZEKROM",)
        elif base == "SPECIES_PECHARUNT":
            needs = ("SPECIES_OKIDOGI", "SPECIES_MUNKIDORI", "SPECIES_FEZANDIPITI")
        elif base == "SPECIES_LANDORUS":
            needs = ("SPECIES_CASTFORM",)
            note += "; quest: Castform-family in party at Route111_RuinsExterior"
        elif base == "SPECIES_MELOETTA":
            note += "; quest: a party member knows Sing at DewfordMeadow"
        elif base == "SPECIES_MARSHADOW":
            note += "; quest: Glass Workshop soot target (Route 113 ash)"
        elif row["req_species"]:
            needs = (row["req_species"],)
        return req, needs, note

    # -- species: wild --------------------------------------------------------
    def _wild_tables(self):
        data = json.loads(WILD.read_text())
        group = next(g for g in data["wild_encounter_groups"] if g.get("for_maps"))
        fishing_groups = next(f for f in group["fields"] if f["type"] == "fishing_mons")["groups"]
        rod_of = {i: rod for rod, idx in fishing_groups.items() for i in idx}
        rod_flag = {"old_rod": "HAS_OLD_ROD", "good_rod": "HAS_GOOD_ROD", "super_rod": "HAS_SUPER_ROD"}
        wild_cite = rel(WILD)
        for entry in group["encounters"]:
            d = self.geo.id_to_dir.get(entry["map"])
            if not d or d not in self.geo.grid:
                continue
            label = entry.get("base_label", "")
            # Only table 0 of Altering Cave is ever selected (VAR_ALTERING_CAVE_WILD_SET
            # is never written; src/wild_encounter.c:792-799).
            if label.startswith("gAlteringCave") and label not in ("gAlteringCave1", "gAlteringCave1F", "gAlteringCaveB1F"):
                continue
            for method in ("land_mons", "water_mons", "rock_smash_mons", "fishing_mons", "honey_mons"):
                if method not in entry:
                    continue
                for idx, mon in enumerate(entry[method]["mons"]):
                    sp = self.S(mon["species"])
                    req, needs, note = self.legend_requirement(sp)
                    sub = method
                    if method == "water_mons":
                        where, extra_req = self._surf_where(d), ["CAN_SURF"]
                    elif method == "fishing_mons":
                        sub = rod_of.get(idx, "super_rod")
                        where, extra_req = self._fish_where(d), [rod_flag[sub]]
                    elif method == "rock_smash_mons":
                        where, extra_req = self._rock_where(d), ["CAN_ROCK_SMASH"]
                    elif method == "honey_mons":
                        where, extra_req = self._grass_where(d), []
                    else:
                        where, extra_req = self._grass_where(d), []
                    honey = ("ITEM_HONEY",) if method == "honey_mons" else ()
                    for w in where:
                        self.species_sources.append(Source(
                            "wild", sp, w, req + extra_req, f"{d} {sub} ({mon['min_level']}-{mon['max_level']})",
                            f"{wild_cite} {label}" + (f"; {note}" if note else "")
                            + ("; Honey on a land-encounter tile (src/item_use.c ItemUseOutOfBattle_Honey)" if honey else ""),
                            needs, needs_items=honey))
                    held = self.species.info.get(sp, {}).get("held", {})
                    for key in ("itemCommon", "itemRare"):
                        item = held.get(key)
                        if item and item != "ITEM_NONE":
                            for w in where:
                                self.item_sources.append(Source(
                                    "wild_held", item, w, req + extra_req, f"held by wild {self.species.name(sp)} ({d} {sub})",
                                    "src/pokemon.c SetWildMonHeldItem (50%/5%)", needs, needs_items=honey))

    def _tiles_where(self, d, pred):
        """Per-component sample tiles of map d whose behaviour matches pred;
        one representative per component keeps the source list small."""
        w, h, _words, beh = self.geo.grid[d]
        out, seen = [], set()
        for y in range(h):
            for x in range(w):
                if not pred(beh[y * w + x]):
                    continue
                nodes = tuple(self.geo.tile_nodes.get((d, x, y), ()))
                if nodes and nodes not in seen:
                    seen.add(nodes)
                    out.append((d, x, y))
        return out or [d]

    def _grass_where(self, d):
        return self._tiles_where(d, lambda b: b in self.geo.encounter_tiles and b not in self.geo.surfable)

    def _surf_where(self, d):
        return self._tiles_where(d, lambda b: b in self.geo.surfable)

    def _fish_where(self, d):
        """Fishing needs a tile facing fishable water: shore land tiles (one
        per component) or the water itself (while surfing)."""
        if d not in self.geo.grid:
            return [d]
        w, h, _words, beh = self.geo.grid[d]
        out, seen = [], set()
        for y in range(h):
            for x in range(w):
                if beh[y * w + x] not in self.geo.surfable:
                    continue
                for dx, dy in ((0, 0), (1, 0), (-1, 0), (0, 1), (0, -1)):
                    nodes = tuple(self.geo.tile_nodes.get((d, x + dx, y + dy), ()))
                    if nodes and nodes not in seen:
                        seen.add(nodes)
                        out.append((d, x + dx, y + dy))
        return out or [d]

    def _rock_where(self, d):
        out = [(m, x, y) for (m, x, y), req in self.geo.tile_gates.items()
               if m == d and req == (("CAN_ROCK_SMASH",),)]
        return out or [d]

    def _visitors(self):
        """Storm visitors: wild in their anomaly on the home map from the first
        Fortree visit until the skies calm (src/weather_anomaly.c:167-252,
        336-363; legendary_signs.h VISITOR rows); afterwards ordinary residents
        (their wild-table slots, handled above)."""
        for sp, row in self.gates.items():
            if not row["visitor"]:
                continue
            d = self.geo.id_to_dir.get(row["visitor"]["map"])
            if not d:
                continue
            req = [f"BADGES>={row['badges']}"] if row["badges"] else []
            if row["flag"]:
                req.append(row["flag"])
            req.append("FLAG_VISITED_FORTREE_CITY")
            habitat = row["visitor"]["habitat"]
            where = self._surf_where(d) if habitat == "WATER" else self._grass_where(d)
            if habitat == "WATER":
                req.append("CAN_SURF")
            for w in where:
                self.species_sources.append(Source(
                    "storm_visitor", self.S(sp), w, req, f"storm visitor on {d} ({row['visitor']['weather']})",
                    f"src/data/pokemon/legendary_signs.h:{row['line']}; src/weather_anomaly.c"))

    def _cut_trees(self):
        """Felled trees hide a separate habitat (src/wild_encounter.c:90-106,1310-1327)."""
        text = (ROOT / "src/wild_encounter.c").read_text()
        block = re.search(r"sCutTree\w*\[\]\s*=\s*\{(.*?)\};", text, re.S)
        species = re.findall(r"\{\s*(SPECIES_\w+)\s*,", block[1]) if block else []
        trees = [(d, x, y) for (d, x, y), req in self.geo.tile_gates.items() if req == (("CAN_CUT",),)]
        for sp in species:
            for t in trees:
                self.species_sources.append(Source("cut_tree", self.S(sp), t, ["CAN_CUT"], "cut-tree habitat",
                                                   cite(ROOT / "src/wild_encounter.c", "sCutTree")))

    def _feebas(self):
        text = (ROOT / "src/wild_encounter.c").read_text()
        if "SPECIES_FEEBAS" in text:
            for w in self._surf_where("Route119"):
                for rod in ("HAS_OLD_ROD", "HAS_GOOD_ROD", "HAS_SUPER_ROD"):
                    self.species_sources.append(Source("wild", "SPECIES_FEEBAS", w, [rod], "Route119 Feebas tiles (any rod)",
                                                       cite(ROOT / "src/wild_encounter.c", "SPECIES_FEEBAS")))

    def _starters(self):
        """Two of one region's three starters at the start (players_house.inc
        region pick; src/emerald_champions_story.c GiveEmeraldChampionsStarterPair,
        src/starter_choose.c sStarterMons). Constraint: at most two, same region."""
        text = (ROOT / "src/starter_choose.c").read_text()
        block = re.search(r"sStarterMons\[\]\[STARTER_MON_COUNT\]\s*=\s*\{(.*?)\};", text, re.S)[1]
        self.starter_regions = [re.findall(r"SPECIES_\w+", row) for row in re.findall(r"\[(?:[1-9])\]\s*=\s*\{([^}]*)\}", block)]
        for region in self.starter_regions:
            for sp in region:
                self.species_sources.append(Source("starter", self.S(sp), "LittlerootTown", None,
                                                   "starter pick (two of one region's three)",
                                                   cite(ROOT / "src/starter_choose.c", "sStarterMons") + "; " +
                                                   cite(ROOT / "src/emerald_champions_story.c", "GiveEmeraldChampionsStarterPair")))

    def _game_corner(self):
        path = MAPS_DIR / "MauvilleCity_GameCorner/scripts.inc"
        text = path.read_text()
        for sp in re.findall(r"setvar VAR_TEMP_1, (SPECIES_\w+)", text):
            req, needs, note = self.legend_requirement(sp)
            if sp == "SPECIES_GENESECT":
                req = sorted(set(req) | {"FLAG_BADGE08_GET"})
            elif sp in ("SPECIES_PORYGON", "SPECIES_MUNCHLAX"):
                req = sorted(set(req) | {"FLAG_BADGE03_GET"})
            self.species_sources.append(Source("game_corner", self.S(sp), "MauvilleCity_GameCorner", req,
                                               "Mauville Game Corner prize (coins bought with money)",
                                               cite(path, f"setvar VAR_TEMP_1, {sp}"), needs))
        fs = FIELD_SPECIALS.read_text()
        block = re.search(r"sEmeraldChampionsGameCornerPokemonPrizes\[\]\s*=\s*\{(.*?)\};", fs, re.S)[1]
        for sp in re.findall(r"\{(SPECIES_\w+),", block):
            if sp == "SPECIES_GENESECT":
                continue
            self.species_sources.append(Source("game_corner", self.S(sp), "MauvilleCity_GameCorner", None,
                                               "Game Corner starter archive (500 coins)",
                                               cite(FIELD_SPECIALS, "sEmeraldChampionsGameCornerPokemonPrizes")))

    def _gifts(self):
        """givemon / giveegg with a literal species (and the held item it
        comes with), located at the map events that run them."""
        for label, info in self.scripts.labels.items():
            for n, line in info["body"]:
                m = re.match(r"(givemon|giveegg)\s+(SPECIES_\w+)(?:\s*,\s*[^,]+\s*,\s*(ITEM_\w+))?", line)
                if not m or info["file"].name == "debug.inc":
                    continue
                sp = self.S(m[2])
                req, needs, note = self.legend_requirement(sp)
                for path_req, where in self.scripted(label, n):
                    self.species_sources.append(Source(
                        "gift" if m[1] == "givemon" else "egg", sp, where, req + path_req, f"{m[1]} at {label}",
                        f"{rel(info['file'])}:{n}" + (f"; {note}" if note else ""), needs))
                    if m[3] and m[3] != "ITEM_NONE":
                        self.item_sources.append(Source("gift", m[3], where, req + path_req,
                                                        f"held by the gift {self.species.name(sp)}", f"{rel(info['file'])}:{n}", needs))
        for sp, where, req, note, c in SPECIAL_GIFTS:
            lreq, needs, lnote = self.legend_requirement(sp)
            self.species_sources.append(Source("gift", self.S(sp), where, lreq + list(req), note, c, needs))

    def _daycare_eggs(self):
        """Day-Care lady's daily gift egg pool (src/field_specials.c
        SetSpeciesAndEggMove): licence flag + badge + bike gates."""
        fs = FIELD_SPECIALS.read_text()
        block = re.search(r"sEggGifts\[\]\s*=\s*\{(.*?)\n    \};", fs, re.S)[1]
        for sp, lic, badge, bike in re.findall(r"\{(SPECIES_\w+),\s*\{[^}]*\},\s*(\w+),\s*(\w+),\s*(TRUE|FALSE)\}", block):
            req = [f for f in (lic, badge) if f != "0"]
            if bike == "TRUE":
                req.append("FLAG_RECEIVED_BIKE")
            self.species_sources.append(Source("egg", self.S(sp), "Route117_PokemonDayCare", req,
                                               "Day-Care lady daily gift egg",
                                               cite(FIELD_SPECIALS, f"{{{sp},")))

    def _trades(self):
        """NPC trades: `setvar VAR_0x8008, INGAME_TRADE_*` + src/data/trade.h."""
        text = TRADE_H.read_text()
        defs = {}
        for m in re.finditer(r"\[(INGAME_TRADE_\w+)\]\s*=\s*\{(.*?)\n\s*\},", text, re.S):
            got = re.search(r"\.species\s*=\s*(SPECIES_\w+)", m[2])
            want = re.search(r"\.requestedSpecies\s*=\s*(SPECIES_\w+)", m[2])
            if got and want:
                defs[m[1]] = (got[1], want[1])
        for label, info in self.scripts.labels.items():
            for n, line in info["body"]:
                m = re.match(r"setvar\s+VAR_0x8008,\s*(INGAME_TRADE_\w+)", line)
                if m and m[1] in defs:
                    got, want = defs[m[1]]
                    req, needs, note = self.legend_requirement(got)
                    for path_req, w in self.scripted(label, n):
                        self.species_sources.append(Source(
                            "trade", self.S(got), w, req + path_req, f"NPC trade {m[1]} (gives {self.species.name(want)})",
                            f"{rel(info['file'])}:{n}; {cite(TRADE_H, '[' + m[1] + ']')}", (self.S(want),) + tuple(needs)))

    def _fossils(self):
        fs = FIELD_SPECIALS.read_text()
        block = re.search(r"sRevivableFossils\[\]\s*=\s*\{(.*?)\};", fs, re.S)[1]
        self.fossils = dict(re.findall(r"\{(ITEM_\w+),\s*(SPECIES_\w+)\}", block))

    def _statics(self):
        """setwildbattle and the legend static specials."""
        for label, info in self.scripts.labels.items():
            body = info["body"]
            for i, (n, line) in enumerate(body):
                sp = None
                m = re.match(r"setwildbattle\s+(SPECIES_\w+)", line)
                if m:
                    sp = m[1]
                m2 = re.match(r"setvar\s+VAR_0x8004,\s*(SPECIES_\w+)", line)
                if m2 and any(("CreateEmeraldChampionsStaticLegendaryEncounter" in l or "CreateEventLegalEnemyMon" in l)
                              for _n, l in body[i:i + 6]):
                    sp = m2[1]
                m3 = re.match(r"setvar\s+VAR_0x8004,\s*LEGENDARY_SIGN_(\w+)", line)
                if m3 and any("CreateSelectedLegendarySignEncounter" in l for _n, l in body[i:i + 6]):
                    sp = "SPECIES_" + m3[1]
                if not sp or not self.species.exists(sp):
                    continue
                sp = self.S(sp)
                req, needs, note = self.legend_requirement(sp)
                extra = STATIC_REQUIREMENTS.get(label) or STATIC_REQUIREMENTS.get(sp)
                if extra:
                    req = req + list(extra[0])
                for path_req, w in self.scripted(label, n):
                    self.species_sources.append(Source(
                        "static", sp, w, req + path_req, f"static encounter {label}",
                        f"{rel(info['file'])}:{n}" + (f"; {extra[1]}" if extra else "") + (f"; {note}" if note else ""), needs))
        # Latias/Latios roam Hoenn after the Hall of Fame TV report
        # (data/scripts/hall_of_fame.inc:50 -> players_house.inc; src/roamer.c).
        for sp in ("SPECIES_LATIAS", "SPECIES_LATIOS"):
            self.species_sources.append(Source("roamer", sp, "Route101", ["FLAG_SYS_GAME_CLEAR"], "roams after the Hall of Fame",
                                               cite(ROOT / "src/roamer.c", "SPECIES_LATIAS")))

    # -- items -----------------------------------------------------------------
    def _item_sources(self):
        # Battle Vendor starter kit (every Center, from Oldale; field_specials.c).
        kit = re.search(r"GiveEmeraldChampionsStarterBattleItems\(void\).*?items\[\]\s*=\s*\{(.*?)\};", FIELD_SPECIALS.read_text(), re.S)[1]
        for item in re.findall(r"ITEM_\w+", kit):
            self.item_sources.append(Source("vendor_kit", item, "OldaleTown_PokemonCenter_1F", None,
                                            "Battle Vendor first-talk starter kit",
                                            cite(FIELD_SPECIALS, "GiveEmeraldChampionsStarterBattleItems")))
        fs = FIELD_SPECIALS.read_text()
        for item, badges in re.findall(r"\{(ITEM_\w+),\s*(\d)\}", re.search(r"sBadgeStockedBattleItems\[\]\s*=\s*\{(.*?)\};", fs, re.S)[1]):
            self.item_sources.append(Source("vendor_badge_stock", item, "OldaleTown_PokemonCenter_1F", [f"BADGES>={badges}"],
                                            f"Battle Vendor stocks it at {badges} badges", cite(FIELD_SPECIALS, "sBadgeStockedBattleItems")))
        # Badge-tier held-item marts (data/scripts/poke_mart.inc).
        pm = SCRIPTS_DIR / "poke_mart.inc"
        text = pm.read_text()
        tiers = {"PokeMart_No_Badges": 0}
        for badge, label in re.findall(r"goto_if_set FLAG_BADGE0(\d)_GET, (Mart_\w+)", text):
            tiers[label.replace("Mart_", "PokeMart_")] = int(badge)
        marts = [d for d, m in self.geo.maps.items() for o in m.get("object_events") or []
                 if o.get("script") == "General_Pokemart_Script"]
        for table, body in re.findall(r"(PokeMart_\w+)::\s*\n(.*?)(?=\n\s*release|\n\w)", text, re.S):
            badges = tiers.get(table)
            if badges is None:
                continue
            for item in re.findall(r"\.2byte\s+(ITEM_\w+)", body):
                if item == "ITEM_NONE":
                    continue
                for d in marts:
                    self.item_sources.append(Source("mart", item, d, [f"BADGES>={badges}"] if badges else None,
                                                    f"Poke Mart held-item tier ({badges} badges)", cite(pm, table + "::")))
        # Every other pokemart list in map/common scripts, located at its clerk.
        for label, info in self.scripts.labels.items():
            for n, line in info["body"]:
                m = re.match(r"(pokemart|pokemartbuy)\s+(\w+)", line)
                if not m or m[2].startswith("PokeMart_"):
                    continue
                items = self._mart_table(info["file"], m[2])
                for req, w in self.scripted(label, n, MART_REQUIREMENTS.get(m[2])):
                    for item in items:
                        self.item_sources.append(Source("mart", item, w, req, f"{m[2]}", f"{rel(info['file'])}:{n}"))
        tent = (ROOT / "src/battle_tent.c").read_text()
        for town, table, req in [("SlateportCity", "sSlateportTentRewards", ["FLAG_BADGE03_GET"]),
                                 ("SlateportCity", "sSlateportTentEarlyRewards", ["!FLAG_BADGE03_GET"]),
                                 ("VerdanturfTown", "sVerdanturfTentRewards", [])]:
            body = re.search(table + r"\[\]\s*=\s*\{(.*?)\};", tent, re.S)[1]
            for item in re.findall(r"ITEM_\w+", body):
                self.item_sources.append(Source("tent_prize", item, town + "_BattleTentLobby", req,
                                                "random prize after three Tent wins",
                                                cite(ROOT / "src/battle_tent.c", table)))
        # Lilycove Department Store 4F evolution specialists (field_specials.c
        # OpenEmeraldChampionsEvolutionSpecialist): every stone and paid item.
        paid = (ROOT / "src/data/emerald_champions_paid_evolution_items.h").read_text()
        stones = re.findall(r"ITEM_\w+_STONE\b", fs[fs.index("OpenEmeraldChampionsEvolutionSpecialist"):][:3000])
        for item in set(re.findall(r"ITEM_\w+", paid)) | set(stones) | {"ITEM_LINKING_CORD"}:
            self.item_sources.append(Source("mart", item, "LilycoveCity_DepartmentStore_4F", None,
                                            "Lilycove Dept. Store 4F evolution specialist",
                                            cite(MAPS_DIR / "LilycoveCity_DepartmentStore_4F/scripts.inc", "OpenEmeraldChampionsEvolutionSpecialist")))
        # Form Items shelf (needs the Mega Ring in the Bag; general_mart.inc:21-22).
        forms = (ROOT / "src/data/emerald_champions_form_items.h").read_text()
        for item in re.findall(r"ITEM_\w+", forms):
            self.item_sources.append(Source("vendor_form_items", item, "OldaleTown_PokemonCenter_1F", ["FLAG_SYS_RECEIVED_KEYSTONE"],
                                            "Center clerk Form Items shelf (Mega Ring in Bag)",
                                            cite(SCRIPTS_DIR / "general_mart.inc", "FORM") + "; " + cite(FIELD_SPECIALS, "OpenEmeraldChampionsEvolutionItemArchive")))
        # Item balls, hidden items, scripted gifts.
        for d, m in self.geo.maps.items():
            for b in m.get("bg_events") or []:
                if b.get("type") == "hidden_item" and b.get("item"):
                    self.item_sources.append(Source("hidden", b["item"], (d, b["x"], b["y"]), None, f"hidden item on {d}",
                                                    f"data/maps/{d}/map.json"))
        for label, info in self.scripts.labels.items():
            for n, line in info["body"]:
                m = re.match(r"(finditem|giveitem|giveuniqueitem|additem|addpcitem|giveitem_msg)\s+(ITEM_\w+)", line)
                if not m:
                    continue
                item = m[2]
                if info["file"].name == "debug.inc":
                    continue
                extra = GIFT_REQUIREMENTS.get(label) or GIFT_REQUIREMENTS.get((label, item))
                kind = "item_ball" if m[1] == "finditem" else "gift"
                for req, w in self.scripted(label, n, extra):
                    self.item_sources.append(Source(kind, item, w, req, f"{m[1]} at {label}",
                                                    f"{rel(info['file'])}:{n}" + (f"; {extra[1]}" if extra else "")))
        # Berry trees (new_game.inc setberrytree + BerryTreeScript objects).
        ng = (SCRIPTS_DIR / "new_game.inc").read_text()
        tree_berry = dict(re.findall(r"setberrytree\s+(BERRY_TREE_\w+),\s*BERRY_ID_(\w+)", ng))
        berry_h = (ROOT / "include/constants/berry.h").read_text()
        tree_ids = {v: k for k, v in re.findall(r"#define\s+(BERRY_TREE_\w+)\s+(\d+)", berry_h)}
        for d, m in self.geo.maps.items():
            for o in m.get("object_events") or []:
                if o.get("script") == "BerryTreeScript":
                    tree_token = str(o.get("trainer_sight_or_berry_tree_id"))
                    tree = tree_token if tree_token in tree_berry else tree_ids.get(tree_token)
                    berry = tree_berry.get(tree)
                    if berry:
                        self.item_sources.append(Source("berry_tree", f"ITEM_{berry}_BERRY", (d, o["x"], o["y"]), None,
                                                        f"berry tree {tree}", cite(SCRIPTS_DIR / "new_game.inc", tree)))
        for item, where, req, note, c in BERRY_GIFTS:
            self.item_sources.append(Source("gift", item, where, req, note, c))
        # Harvest rewards are native transactions, not giveitem macros. The
        # pouch accepts only berries the player harvested, after badge seven.
        harvest = (ROOT / "src/mega_stone_rewards.c").read_text().split("sBerryStoneTrades[]", 1)[1].split("STATIC_ASSERT", 1)[0]
        entries = list(re.finditer(r"\{(ITEM_\w+),\s*(FLAG_EC_BERRY_TRADE_\w+|0),", harvest))
        for index, entry in enumerate(entries):
            if entry[1] == "ITEM_NONE":
                continue
            body = harvest[entry.end():entries[index + 1].start() if index + 1 < len(entries) else len(harvest)]
            berries = tuple("ITEM_" + b + "_BERRY" for b in re.findall(r"\{BERRY_ID_(\w+),", body))
            self.item_sources.append(Source("harvest_trade", entry[1], "Route123_BerryMastersHouse",
                ["FLAG_BADGE07_GET"], "one-time harvested berry trade after badge seven",
                cite(ROOT / "src/mega_stone_rewards.c", "TradeEmeraldChampionsGardenBerries"), needs_items=berries))
        for item, where, req, note, c in EXTRA_ITEM_SOURCES:
            self.item_sources.append(Source("gift", item, where, req, note, c))

    def _mart_table(self, path: Path, table: str) -> list[str]:
        text = path.read_text()
        m = re.search(re.escape(table) + r"::?\s*\n(.*?)(?:ITEM_NONE|\n\w)", text, re.S)
        if not m:
            for p in list(MAPS_DIR.glob("*/scripts.inc")) + list(SCRIPTS_DIR.glob("*.inc")):
                t = p.read_text()
                m = re.search(re.escape(table) + r"::?\s*\n(.*?)(?:ITEM_NONE|\n\w)", t, re.S)
                if m:
                    break
        return re.findall(r"ITEM_\w+", m[1]) if m else []


# Requirements some scripted gifts check beyond reaching their NPC.
# label (or (label, item)) -> (requirement tokens, citation).
GIFT_REQUIREMENTS: dict = {
    "GraniteCave_StevensRoom_EventScript_GiveReward": (["FLAG_RECEIVED_POKENAV"],
                                                       "needs the Letter (GraniteCave_StevensRoom/scripts.inc:9-14)"),
    # Norman's post-battle talk is reached through `switch VAR_PETALBURG_GYM_STATE`
    # (case 7/8), which the path scan does not track.
    "PetalburgCity_Gym_EventScript_GiveFacade": (["DEFEATED:TRAINER_NORMAN_1"], "PetalburgCity_Gym/scripts.inc:100-107,346-352,469"),
    "PetalburgCity_Gym_EventScript_GiveEnigmaBerry": (["DEFEATED:TRAINER_NORMAN_1"], "PetalburgCity_Gym/scripts.inc:346-350"),
    "MauvilleCity_Gym_EventScript_GiveMegaRing": (["FLAG_BADGE03_GET"], "MauvilleCity_Gym/scripts.inc (Wattson victory or owed Ring retry)"),
    "PetalburgCity_Gym_EventScript_NormanStarterGift": (["FLAG_BADGE05_GET"], "PetalburgCity_Gym/scripts.inc (after Norman victory)"),
    **{f"Route110_TrickHouseEnd_EventScript_CompletedPuzzle{n}": (
        [req] if req else [], f"Route110_TrickHouseEnd/scripts.inc:44-151 (switch VAR_TRICK_HOUSE_LEVEL, puzzle {n})")
       for n, req in ((1, None), (2, "FLAG_BADGE03_GET"), (3, "FLAG_BADGE04_GET"), (4, "FLAG_BADGE05_GET"),
                      (5, "FLAG_BADGE06_GET"), (6, "FLAG_BADGE07_GET"), (7, "FLAG_BADGE08_GET"), (8, "FLAG_SYS_GAME_CLEAR"))},
    # Trick Master rewards follow the solved puzzle (VAR_TRICK_HOUSE_LEVEL), whose
    # door needs the badges in Route110_TrickHouseEntrance/scripts.inc:55-100.
    **{f"Route110_TrickHouseEntrance_EventScript_GivePuzzle{n}Reward": (
        [req] if req else [], f"Route110_TrickHouseEntrance/scripts.inc:{72 + 4 * n} (puzzle {n})")
       for n, req in ((1, None), (2, "FLAG_BADGE03_GET"), (3, "FLAG_BADGE04_GET"), (4, "FLAG_BADGE05_GET"),
                      (5, "FLAG_BADGE06_GET"), (6, "FLAG_BADGE07_GET"), (7, "FLAG_BADGE08_GET"), (8, "FLAG_SYS_GAME_CLEAR"))},
}
# Gifts made by C specials rather than a literal givemon.
SPECIAL_GIFTS = [
    ("SPECIES_COSMOG", "LittlerootTown_ProfessorBirchsLab", [], "Cosmog pair at the National Dex upgrade",
     "src/script_pokemon_util.c:347-378; LittlerootTown_ProfessorBirchsLab/scripts.inc:183"),
    ("SPECIES_MAGEARNA", "RustboroCity_DevonCorp_2F", [], "Devon Corp 2F gift",
     "RustboroCity_DevonCorp_2F/scripts.inc:59-67"),
    ("SPECIES_ARCEUS", "RustboroCity_DevonCorp_2F", [], "Devon finale reward",
     "RustboroCity_DevonCorp_2F/scripts.inc:94; src/legendary_signs.c:995-1019"),
]
# Static encounters whose own script adds a check to the sign gate.
STATIC_REQUIREMENTS: dict = {
    "SPECIES_RAYQUAZA": (["FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE"], "SkyPillar_Top/scripts.inc:51"),
    "SPECIES_GROUDON": (["EV_ABNORMAL_WEATHER"], "Terra Cave after the skies calm"),
    "SPECIES_KYOGRE": (["EV_ABNORMAL_WEATHER"], "Marine Cave after the skies calm"),
    "SPECIES_DEOXYS": (["FLAG_ENABLE_SHIP_BIRTH_ISLAND"], "finale expedition"),
    "SPECIES_MEWTWO": (["FLAG_BADGE07_GET"], "AlteringCave_B1F/scripts.inc:10"),
}
MART_REQUIREMENTS: dict = {}
# Berries handed out by NPCs (daily or once).
BERRY_GIFTS = [
    (f"ITEM_{b}_BERRY", "Route104_PrettyPetalFlowerShop", None, "flower shop daily bundle",
     "src/field_specials.c GiveFlowerShopBerryBundle") for b in ("POMEG", "KELPSY", "QUALOT", "HONDEW", "GREPA", "TAMATO")
] + [
    (f"ITEM_{b}_BERRY", "Route123_BerryMastersHouse", None, "Berry Master's wife daily (Cheri..Sitrus)",
     "data/maps/Route123_BerryMastersHouse/scripts.inc:115") for b in
    ("CHERI", "CHESTO", "PECHA", "RAWST", "ASPEAR", "LEPPA", "ORAN", "PERSIM", "LUM", "SITRUS")
]
EXTRA_ITEM_SOURCES = [
    # Norman awards after his victory the stones of the starter pair's final
    # forms (src/mega_stone_rewards.c:200-231 sStarterMegaStones; the pair can
    # be any region's), and later the Hoenn stones when shown that line.
    *[(item, "PetalburgCity_Gym", ["FLAG_BADGE05_GET"], "Norman's starter-pair Mega Stone (after his victory)",
       "src/mega_stone_rewards.c:200-231; PetalburgCity_Gym/scripts.inc:394-446")
      for item in ("ITEM_VENUSAURITE", "ITEM_CHARIZARDITE_X", "ITEM_CHARIZARDITE_Y", "ITEM_BLASTOISINITE",
                   "ITEM_MEGANIUMITE", "ITEM_FERALIGITE", "ITEM_SCEPTILITE", "ITEM_BLAZIKENITE", "ITEM_SWAMPERTITE",
                   "ITEM_EMBOARITE", "ITEM_CHESNAUGHTITE", "ITEM_DELPHOXITE", "ITEM_GRENINJITE")],
    *[(item, "Route119_WeatherInstitute_1F", ["FLAG_HIDE_ROUTE_119_TEAM_AQUA"], "Weather Institute rocks after Aqua leave",
       "Route119_WeatherInstitute_1F/scripts.inc:17-31 (CanReceiveWeatherInstituteRocks)")
      for item in ("ITEM_HEAT_ROCK", "ITEM_DAMP_ROCK", "ITEM_ICY_ROCK", "ITEM_SMOOTH_ROCK")],
    ("ITEM_HONEY", "OldaleTown_PokemonCenter_1F", ["FLAG_BADGE01_GET"], "Center Supplies (1-badge tier)",
     "data/scripts/general_mart.inc:86,146"),
    ("ITEM_MEGA_RING", "MauvilleCity_Gym", ["FLAG_SYS_RECEIVED_KEYSTONE"], "Wattson (after badge three)",
     "data/maps/MauvilleCity_Gym/scripts.inc: MauvilleCity_Gym_EventScript_GiveMegaRing"),
]


# ---------------------------------------------------------------------------
# Per-window availability (fixed point over sources, evolutions, breeding,
# fossils, forms and Megas)
# ---------------------------------------------------------------------------

EQUIPMENT_SPECIES = {  # GetFormEquipmentSpecies, src/field_specials.c:690-706
    "ITEM_LUCKY_PUNCH": "SPECIES_CHANSEY", "ITEM_ADAMANT_CRYSTAL": "SPECIES_DIALGA",
    "ITEM_LUSTROUS_GLOBE": "SPECIES_PALKIA", "ITEM_GRISEOUS_CORE": "SPECIES_GIRATINA",
    "ITEM_DOUSE_DRIVE": "SPECIES_GENESECT", "ITEM_SHOCK_DRIVE": "SPECIES_GENESECT",
    "ITEM_BURN_DRIVE": "SPECIES_GENESECT", "ITEM_CHILL_DRIVE": "SPECIES_GENESECT",
}
# Legendary relics (src/legendary_signs.c:433-468): given on capture; the
# championOnly groups wait for FLAG_IS_CHAMPION.
RELIC_ITEMS = {
    "SPECIES_GROUDON": (["ITEM_RED_ORB"], True), "SPECIES_KYOGRE": (["ITEM_BLUE_ORB"], True),
    "SPECIES_ZACIAN": (["ITEM_RUSTED_SWORD"], True), "SPECIES_ZAMAZENTA": (["ITEM_RUSTED_SHIELD"], True),
    "SPECIES_OGERPON": (["ITEM_WELLSPRING_MASK", "ITEM_HEARTHFLAME_MASK", "ITEM_CORNERSTONE_MASK"], False),
    "SPECIES_KYUREM": (["ITEM_DNA_SPLICERS"], True), "SPECIES_CALYREX": (["ITEM_REINS_OF_UNITY"], True),
    "SPECIES_NECROZMA": (["ITEM_N_SOLARIZER", "ITEM_N_LUNARIZER"], True), "SPECIES_HOOPA": (["ITEM_PRISON_BOTTLE"], True),
    "SPECIES_ZYGARDE": (["ITEM_ZYGARDE_CUBE"], True),
}
FREE_FORM_METHODS = {"FORM_CHANGE_END_BATTLE_ENVIRONMENT", "FORM_CHANGE_TIME_OF_DAY", "FORM_CHANGE_OVERWORLD_WEATHER",
                     "FORM_CHANGE_DAYS_PASSED", "FORM_CHANGE_WITHDRAW", "FORM_CHANGE_DEPOSIT", "FORM_CHANGE_MOVE"}
ITEM_FORM_METHODS = {"FORM_CHANGE_ITEM_USE", "FORM_CHANGE_ITEM_USE_MULTICHOICE", "FORM_CHANGE_ITEM_HOLD"}
SOURCE_PRIORITY = ["starter", "wild", "gift", "egg", "trade", "game_corner", "static", "storm_visitor", "roamer",
                   "cut_tree", "fossil", "breeding", "evolution", "form", "mega", "primal"]


def form_changes(sd: SpeciesData) -> list[tuple[str, str, str, str]]:
    """(from species, method, target, item) for every out-of-battle form
    change in src/data/pokemon/form_change_tables.h, applied to every species
    that uses the table."""
    text = mr.FORM_TABLES.read_text()
    tables = {}
    for name, body in re.findall(r"static const struct FormChange (\w+)\[\]\s*=\s*\{(.*?)\};", text, re.S):
        tables[name] = re.findall(r"\{\s*(FORM_CHANGE_\w+)\s*,\s*(SPECIES_\w+)\s*(?:,\s*(\w+))?", body)
    species_info_text = preprocess_species_info()
    owners = defaultdict(list)
    for sp, table in re.findall(r"\[(SPECIES_\w+)\]\s*=\s*\{[^\[]*?\.formChangeTable\s*=\s*(\w+)", species_info_text):
        owners[table].append(sd.resolve(sp))
    out = []
    for table, rows in tables.items():
        for method, target, param in rows:
            if method not in FREE_FORM_METHODS | ITEM_FORM_METHODS:
                continue
            for sp in owners.get(table, []):
                if sd.resolve(target) != sp:
                    out.append((sp, method, sd.resolve(target), param or ""))
    return out


class Pools:
    def __init__(self, b: Builder, enc: "Encounters", *, compute_windows=True):
        self.b = b
        self.enc = enc
        self.story = b.story
        self.sd = b.species
        self.forms = form_changes(self.sd)
        self.megas = mr.item_forms()
        text = mr.FORM_TABLES.read_text()
        self.primals = re.findall(r"FORM_CHANGE_BATTLE_PRIMAL_REVERSION,\s*(SPECIES_\w+),\s*(ITEM_\w+)", text)
        self.mapsec = {d: m.get("region_map_section") for d, m in self.story.geo.maps.items()}
        self.by_key_species = defaultdict(list)
        for s in b.species_sources:
            self.by_key_species[s.key].append(s)
        self.windows = self.story.window_names()
        self.result = {w: self.compute(w) for w in self.windows} if compute_windows else {}

    def live(self, w: str, src: Source, closure=None, pending=frozenset()) -> bool:
        for alt in src.requires:
            flags, beaten, at = split_tokens(alt)
            if "OUT_OF_SCOPE" in flags:
                continue
            if not self.story.reachable(w, src.where, (flags,) if flags else None, closure=closure):
                continue
            if (all(self.story.reachable(w, m, closure=closure) for m in at)
                    and all(t not in pending and self.enc.defeated_by(t, w, closure=closure) for t in beaten)):
                return True
        return False

    def compute(self, w: str, *, closure=None, pending=frozenset()) -> dict:
        story, sd, b = self.story, self.sd, self.b
        cap = story.cap(w)
        closure = closure if closure is not None else story.closure[w]
        flags = closure["flags"]
        species: dict[str, dict] = {}
        items: dict[str, dict] = {}
        live_species_src = [s for s in b.species_sources if self.live(w, s, closure, pending)]
        live_item_src = [s for s in b.item_sources if self.live(w, s, closure, pending)]

        def reachable(where):
            return story.reachable(w, where, closure=closure)

        def have(sp_expr: str) -> bool:
            return any(sd.resolve(x) in species or any(m in species for m in sd.family(x)) for x in sp_expr.split("|"))

        def add_species(sp, how):
            sp = sd.resolve(sp)
            if sp not in species and sd.exists(sp):
                species[sp] = how
                return True
            return False

        def add_item(item, how):
            if item not in items:
                items[item] = how
                return True
            return False

        def map_ok(token: str) -> bool:
            d = self.story.geo.id_to_dir.get(token)
            return d is not None and reachable(d)

        def mapsec_ok(token: str) -> bool:
            return any(sec == token and reachable(d) for d, sec in self.mapsec.items())

        changed = True
        while changed:
            changed = False
            for s in sorted(live_species_src, key=lambda s: SOURCE_PRIORITY.index(s.kind) if s.kind in SOURCE_PRIORITY else 99):
                if s.key in species or not all(have(n) for n in s.needs_species) or not all(i in items for i in s.needs_items):
                    continue
                changed |= add_species(s.key, dict(kind=s.kind, detail=s.detail, where=s.as_dict()["where"], cite=s.cite))
            for s in live_item_src:
                if s.key in items or not all(have(n) for n in s.needs_species) or not all(i in items for i in s.needs_items):
                    continue
                changed |= add_item(s.key, dict(kind=s.kind, detail=s.detail, where=s.as_dict()["where"], cite=s.cite))
            # Vendor species equipment and relics.
            for item, sp in EQUIPMENT_SPECIES.items():
                if item == "ITEM_THICK_CLUB" and "FLAG_BADGE04_GET" not in flags:
                    continue
                if have(sp) and item not in items:
                    if item in ("ITEM_LUCKY_PUNCH",) or "FLAG_SYS_RECEIVED_KEYSTONE" in flags:
                        changed |= add_item(item, dict(kind="vendor_species", detail=f"shelf opens once {sd.name(sp)} is caught",
                                                       cite="src/field_specials.c:490-497,690-706"))
            for sp, (relics, champion_only) in RELIC_ITEMS.items():
                if have(sp) and (not champion_only or "FLAG_IS_CHAMPION" in flags):
                    for item in relics:
                        changed |= add_item(item, dict(kind="relic", detail=f"relic for {sd.name(sp)}" + (" after the Hall of Fame" if champion_only else ""),
                                                       cite="src/legendary_signs.c:433-468"))
            if have("SPECIES_ARCEUS"):
                for item in re.findall(r"ITEM_\w+_PLATE", "".join(RELIC_PLATES)):
                    changed |= add_item(item, dict(kind="relic", detail="plates with Arceus", cite="src/legendary_signs.c:433-468"))
            # Fossils revived at Devon Corp 2F.
            if reachable("RustboroCity_DevonCorp_2F"):
                for item, sp in b.fossils.items():
                    if item in items:
                        changed |= add_species(sp, dict(kind="fossil", detail=f"revived from {item} at Devon Corp 2F",
                                                        cite=cite(FIELD_SPECIALS, "sRevivableFossils")))
            # Evolutions.
            for sp in list(species):
                for method, param, target, conds in sd.info.get(sp, {}).get("evolutions", []):
                    target = sd.resolve(target)
                    if target in species:
                        continue
                    ok, label = self.evo_ok(method, param, conds, cap, items, have, map_ok, mapsec_ok,
                                            evolution_move_ready=lambda move: evolution_move_gate.ready(sp, move, cap, flags))
                    if ok:
                        changed |= add_species(target, dict(kind="evolution", detail=f"{label} from {sd.name(sp)}",
                                                            cite="gSpeciesInfo evolutions (src/data/pokemon/species_info)"))
            # Breeding at the Route 117 Day-Care (src/daycare.c): the egg is the
            # mother's lowest pre-evolution; Undiscovered cannot breed; Manaphy -> Phione.
            if reachable("Route117_PokemonDayCare"):
                for sp in list(species):
                    info = sd.info.get(sp, {})
                    if info.get("egg_group") in (None, "EGG_GROUP_NO_EGGS_DISCOVERED"):
                        continue
                    egg = "SPECIES_PHIONE" if sd.base(sp) == "SPECIES_MANAPHY" else sd.egg_species(sp)
                    if egg not in species:
                        changed |= add_species(egg, dict(kind="breeding", detail=f"Day-Care egg from {sd.name(sp)}",
                                                         cite="src/daycare.c; data/scripts/day_care.inc"))
            # Cozmo swaps Deoxys forms (FallarborTown_CozmosHouse/scripts.inc:116-131).
            if "SPECIES_DEOXYS" in species and reachable("FallarborTown_CozmosHouse"):
                for form in ("SPECIES_DEOXYS_ATTACK", "SPECIES_DEOXYS_DEFENSE", "SPECIES_DEOXYS_SPEED"):
                    changed |= add_species(form, dict(kind="form", detail="Cozmo's meteorite form change",
                                                      cite="data/maps/FallarborTown_CozmosHouse/scripts.inc:116-131"))
            # Out-of-battle form changes.
            for sp, method, target, param in self.forms:
                if sp in species and target not in species:
                    if method in ITEM_FORM_METHODS and param not in ("", "ITEM_NONE") and param not in items:
                        continue
                    changed |= add_species(target, dict(kind="form", detail=f"{method} {param} from {sd.name(sp)}".strip(),
                                                        cite="src/data/pokemon/form_change_tables.h"))
        # Megas: base + stone + Mega Ring in the Bag (src/battle_util.c:8503-8508).
        megas = []
        ring = "FLAG_SYS_RECEIVED_KEYSTONE" in flags
        for stone, forms in self.megas.items():
            for form in forms:
                base = sd.resolve(mr.base_species(form))
                if not ring or base not in species or stone not in items:
                    continue
                megas.append(dict(species=form, base=base, stone=stone, stone_source=items[stone]))
                species.setdefault(form, dict(kind="mega", detail=f"{stone} + Mega Ring", cite="src/battle_util.c:8503-8508"))
        # Mega Rayquaza: move-triggered, player's only after the Hall of Fame
        # (src/emerald_champions_battle_plan.c:43-51).
        if ring and "SPECIES_RAYQUAZA" in species and "FLAG_IS_CHAMPION" in flags:
            species.setdefault("SPECIES_RAYQUAZA_MEGA", dict(kind="mega", detail="Dragon Ascent, after the Hall of Fame",
                                                             cite="src/emerald_champions_battle_plan.c:43-51"))
            megas.append(dict(species="SPECIES_RAYQUAZA_MEGA", base="SPECIES_RAYQUAZA", stone=None, stone_source=None))
        for form, orb in self.primals:
            base = sd.base(form)
            if base in species and orb in items:
                species.setdefault(sd.resolve(form), dict(kind="primal", detail=f"{orb} (battle)", cite="src/data/pokemon/form_change_tables.h"))
        return dict(species=species, items=items, megas=megas, ring=ring, flags=flags, cap=cap)

    @staticmethod
    def evo_ok(method, param, conds, cap, items, have, map_ok, mapsec_ok, evolution_move_ready=lambda move: True) -> tuple[bool, str]:
        if method in ("EVO_TRADE", "EVO_NONE", "EVO_LEVEL_BATTLE_ONLY"):
            # No link trades; no battle EXP (battle-only level-ups never happen).
            return False, method
        label = method
        if method == "EVO_ITEM":
            if param not in items:
                return False, method
            label = f"use {param}"
        elif method == "EVO_LEVEL":
            lvl = int(param) if param.isdigit() else 0
            if lvl > cap:
                return False, method
            label = f"level {lvl}" if lvl else "level-up"
        elif method == "EVO_SPLIT_FROM_EVO":
            label = "split evolution"
        for cond, arg in conds:
            if cond == "IF_HOLD_ITEM":
                if arg not in items:
                    return False, method
                label += f" holding {arg}"
            elif cond == "IF_IN_MAP":
                if not map_ok(arg):
                    return False, method
                label += f" in {arg}"
            elif cond == "IF_IN_MAPSEC":
                if not mapsec_ok(arg):
                    return False, method
                label += f" in {arg}"
            elif cond == "IF_SPECIES_IN_PARTY":
                if not have(arg):
                    return False, method
            elif cond == "IF_TRADE_PARTNER_SPECIES":
                return False, method
            elif cond == "IF_KNOWS_MOVE":
                if not evolution_move_ready(arg):
                    return False, method
            elif cond == "IF_MIN_FRIENDSHIP":
                label += " (Bonding)"
        return True, label


RELIC_PLATES = ["ITEM_FLAME_PLATE ITEM_SPLASH_PLATE ITEM_ZAP_PLATE ITEM_MEADOW_PLATE ITEM_ICICLE_PLATE ITEM_FIST_PLATE "
                "ITEM_TOXIC_PLATE ITEM_EARTH_PLATE ITEM_SKY_PLATE ITEM_MIND_PLATE ITEM_INSECT_PLATE ITEM_STONE_PLATE "
                "ITEM_SPOOKY_PLATE ITEM_DRACO_PLATE ITEM_DREAD_PLATE ITEM_IRON_PLATE ITEM_PIXIE_PLATE"]


# ---------------------------------------------------------------------------
# Encounters
# ---------------------------------------------------------------------------

# Extra requirements a battle has beyond its map script's own path conditions:
# objects hidden at new game until a story script clears their flag, and a
# few script states the path scan cannot see. trainer -> (tokens, citation).
ENCOUNTER_REQUIREMENTS: dict[str, tuple[list[str], str]] = {
    "TRAINER_GRUNT_RUSTURF_TUNNEL": (["FLAG_DEVON_GOODS_STOLEN"], "shown by RustboroCity/scripts.inc:315"),
    "TRAINER_SHELBY_1": (["FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY"], "LavaridgeTown/scripts.inc:14,41"),
    "TRAINER_SHIRLEY": (["FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY"], "LavaridgeTown/scripts.inc:14,41"),
    **{t: (["FLAG_BADGE07_GET"], "Space Center Magma appear at MossdeepCity_Gym/scripts.inc:73-74")
       for t in ("TRAINER_GRUNT_SPACE_CENTER_1", "TRAINER_GRUNT_SPACE_CENTER_2", "TRAINER_GRUNT_SPACE_CENTER_3",
                 "TRAINER_GRUNT_SPACE_CENTER_4", "TRAINER_GRUNT_SPACE_CENTER_5", "TRAINER_GRUNT_SPACE_CENTER_7",
                 "TRAINER_TABITHA_MOSSDEEP", "TRAINER_MAXIE_MOSSDEEP", "TRAINER_COURTNEY_MOSSDEEP")},
    "TRAINER_WALLY_VR_2": (["FLAG_SYS_GAME_CLEAR"], "shown by data/scripts/hall_of_fame.inc:14"),
    **{t: (["EV_FINALE_WALLY_DONE"], "SSTidalRooms/scripts.inc:37-75 (EC_FINALE_VOYAGE)")
       for t in ("TRAINER_COLTON", "TRAINER_MICAH", "TRAINER_THOMAS", "TRAINER_LEA_AND_JED", "TRAINER_NAOMI")},
    "TRAINER_STEVEN": (["EV_FINALE_VOYAGE_DONE"], "MeteorFalls_StevensCave/scripts.inc:8-9 (EC_FINALE_STEVEN)"),
    "TRAINER_BUFFEL": (["FLAG_EC_FINALE_DEOXYS_RESOLVED"], "LilycoveCity_CoveLilyMotel_2F/scripts.inc:38-39 (EC_FINALE_BUFFEL)"),
    "TRAINER_CYNTHIA_1": (["FLAG_SYS_GAME_CLEAR"], "MossdeepCity_House1/scripts.inc:13-25"),
    "TRAINER_LEAF_ALTERING_CAVE": (["FLAG_SYS_GAME_CLEAR"], "Altering Cave opens on FLAG_SYS_GAME_CLEAR (Route103/scripts.inc:13-19)"),
    "TRAINER_WALLACE_DOUBLES_LEGENDS": (["FLAG_SYS_GAME_CLEAR"], "CaveOfOrigin_DianciesRoom/scripts.inc:40"),
    "TRAINER_NORMAN_1": (["FLAG_BADGE04_GET"], "PetalburgCity_Gym/scripts.inc:PetalburgCity_Gym_EventScript_Norman (state 6)"),
    **{f"TRAINER_{r}_RUSTBORO_{st}": (["FLAG_RECEIVED_POKENAV"],
                                       "Route104 rival: VAR_ROUTE104_STATE 1 from the PokeNav scientist (RustboroCity/scripts.inc:61-85)")
       for r in ("BRENDAN", "MAY") for st in ("MUDKIP", "TORCHIC", "TREECKO")},
    "TRAINER_GABBY_AND_TY_2": (["DEFEATED:TRAINER_GABBY_AND_TY_1"], "data/scripts/gabby_and_ty.inc:1-8 (stops in order)"),
    "TRAINER_GABBY_AND_TY_5": (["DEFEATED:TRAINER_GABBY_AND_TY_2"], "data/scripts/gabby_and_ty.inc:1-8"),
    "TRAINER_GABBY_AND_TY_6": (["DEFEATED:TRAINER_GABBY_AND_TY_5"], "data/scripts/gabby_and_ty.inc:1-8"),
}
# Story flags a victory sets (so the battle cannot still be pending in a
# window that needs them). trainer -> flags (all STORY_EVENTS tokens).
ENCOUNTER_CONSEQUENCES: dict[str, list[str]] = {
    **{f"TRAINER_{r}_ROUTE_103_{st}": ["FLAG_DEFEATED_RIVAL_ROUTE103"]
       for r in ("BRENDAN", "MAY") for st in ("MUDKIP", "TORCHIC", "TREECKO")},
    "TRAINER_ROXANNE_1": ["FLAG_BADGE01_GET"], "TRAINER_BRAWLY_1": ["FLAG_BADGE02_GET"],
    "TRAINER_WATTSON_1": ["FLAG_BADGE03_GET"], "TRAINER_FLANNERY_1": ["FLAG_BADGE04_GET"],
    "TRAINER_NORMAN_1": ["FLAG_BADGE05_GET"], "TRAINER_WINONA_1": ["FLAG_BADGE06_GET"],
    "TRAINER_TATE_AND_LIZA_1": ["FLAG_BADGE07_GET"], "TRAINER_JUAN_1": ["FLAG_BADGE08_GET"],
    "TRAINER_GRUNT_RUSTURF_TUNNEL": ["FLAG_RECOVERED_DEVON_GOODS"],
    "TRAINER_GRUNT_MUSEUM_1": ["FLAG_HIDE_ROUTE_110_TEAM_AQUA"], "TRAINER_ARCHIE_SLATEPORT": ["FLAG_HIDE_ROUTE_110_TEAM_AQUA"],
    "TRAINER_WALLY_MAUVILLE": ["FLAG_DEFEATED_WALLY_MAUVILLE"],
    "TRAINER_COURTNEY_METEOR_FALLS": ["FLAG_HIDE_ROUTE_112_TEAM_MAGMA", "FLAG_MET_ARCHIE_METEOR_FALLS"],
    "TRAINER_GRUNT_METEOR_FALLS": ["FLAG_HIDE_ROUTE_112_TEAM_MAGMA", "FLAG_MET_ARCHIE_METEOR_FALLS"],
    "TRAINER_TABITHA_MT_CHIMNEY": ["FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY"], "TRAINER_MAXIE_MT_CHIMNEY": ["FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY"],
    "TRAINER_SHELLY_WEATHER_INSTITUTE": ["FLAG_HIDE_ROUTE_119_TEAM_AQUA"],
    "TRAINER_MATT_MT_PYRE": ["FLAG_RECEIVED_RED_OR_BLUE_ORB"],
    "TRAINER_MAXIE_MAGMA_HIDEOUT": ["FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT"],
    "TRAINER_MATT": ["FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE"],
    "TRAINER_MAXIE_MOSSDEEP": ["FLAG_DEFEATED_MAGMA_SPACE_CENTER"], "TRAINER_COURTNEY_MOSSDEEP": ["FLAG_DEFEATED_MAGMA_SPACE_CENTER"],
    "TRAINER_ARCHIE": ["FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN"],
    "TRAINER_WALLY_VR_1": ["FLAG_DEFEATED_WALLY_VICTORY_ROAD"],
    **{t: ["FLAG_IS_CHAMPION"] for t in ("TRAINER_SIDNEY", "TRAINER_PHOEBE", "TRAINER_GLACIA", "TRAINER_DRAKE", "TRAINER_WALLACE")},
    "TRAINER_WALLY_VR_2": ["EV_FINALE_WALLY_DONE"],
    **{t: ["EV_FINALE_VOYAGE_DONE"] for t in ("TRAINER_COLTON", "TRAINER_MICAH", "TRAINER_THOMAS", "TRAINER_LEA_AND_JED", "TRAINER_NAOMI")},
    "TRAINER_STEVEN": ["FLAG_ENABLE_SHIP_BIRTH_ISLAND"],
    "TRAINER_BUFFEL": ["EV_BUFFEL_DONE"],
}
# Story trainers whose objects are hidden by a story script: they can no
# longer be fought once that story flag is set. object hide flag -> story flag.
VANISH_FLAGS = {
    "FLAG_HIDE_MT_CHIMNEY_TEAM_MAGMA": "FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY",       # MtChimney/scripts.inc:60
    "FLAG_HIDE_ROUTE_119_TEAM_AQUA": "FLAG_HIDE_ROUTE_119_TEAM_AQUA",              # Route119_WeatherInstitute_2F:77
    "FLAG_HIDE_MT_PYRE_SUMMIT_TEAM_AQUA": "FLAG_RECEIVED_RED_OR_BLUE_ORB",         # MtPyre_Summit/scripts.inc:65
    "FLAG_HIDE_JAGGED_PASS_MAGMA_GUARD": "FLAG_RECEIVED_RED_OR_BLUE_ORB",          # MtPyre_Summit/scripts.inc:82
    "FLAG_HIDE_MAGMA_HIDEOUT_GRUNTS": "FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT",       # MagmaHideout_4F/scripts.inc:97
    "FLAG_HIDE_AQUA_HIDEOUT_GRUNTS": "FLAG_BADGE07_GET",                           # MossdeepCity_Gym/scripts.inc:66
    "FLAG_HIDE_SEAFLOOR_CAVERN_AQUA_GRUNTS": "FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN", # SeafloorCavern_Room9:154
    "FLAG_HIDE_MOSSDEEP_CITY_SPACE_CENTER_1F_TEAM_MAGMA": "FLAG_DEFEATED_MAGMA_SPACE_CENTER",  # SpaceCenter_2F:291
    "FLAG_HIDE_MOSSDEEP_CITY_SPACE_CENTER_2F_TEAM_MAGMA": "FLAG_DEFEATED_MAGMA_SPACE_CENTER",  # SpaceCenter_2F:292
}
REPLAYABLE_AFTER_CHAMPION = {"TRAINER_SIDNEY", "TRAINER_PHOEBE", "TRAINER_GLACIA", "TRAINER_DRAKE", "TRAINER_WALLACE"}


def master_fields() -> dict[int, dict]:
    text = MASTER.read_text()
    out = {}
    for m in re.finditer(r"(?m)^=== ENCOUNTER (\d{4}) ===$(.*?)(?=^=== ENCOUNTER|\Z)", text, re.S):
        block = m[2]
        fields = {}
        for key in ("strict_cap", "location", "requirement", "chapter", "campaign_order"):
            fm = re.search(r"(?m)^" + key + r":\s*(.*)$", block)
            if fm:
                fields[key] = fm[1].strip()
        out[int(m[1])] = fields
    return out


class Encounters:
    def __init__(self, b: Builder):
        self.b, self.story, self.scripts = b, b.story, b.scripts
        self.branches = teams.read_teams()
        self.team_trainers = {br.trainer for br in self.branches}
        self.master = master_fields()
        order = {}
        if REVIEW_INDEX.exists():
            for e in json.loads(REVIEW_INDEX.read_text())["encounters"]:
                for t in e["trainer_ids"]:
                    if e.get("review_index") is not None:
                        order[t] = e["review_index"]
        self.review = order
        self.calls = self._calls()
        self.keep = self._keep_token
        self.paths = {t: self._paths(t) for t in self.team_trainers}
        self.window_index = {w: i for i, w in enumerate(self.story.window_names())}
        self.vanish = defaultdict(list)
        for t in self.team_trainers:
            for call in self.calls.get(t, []):
                for _d, _x, _y, kind, flag in self.scripts.reach_entries(call["label"])[0]:
                    if kind == "object" and flag in VANISH_FLAGS:
                        self.vanish[t].append(VANISH_FLAGS[flag])
        self.fightable = self._fightable()

    def _keep_token(self, token: str) -> bool:
        if token.startswith("DEFEATED:"):
            return token[9:] in self.team_trainers
        return mr.canon(token) in STORY_FLAGS

    def _calls(self) -> dict[str, list[dict]]:
        calls = defaultdict(list)
        for label, info in self.scripts.labels.items():
            for n, line in info["body"]:
                op = line.split()[0]
                if not (op.startswith("trainerbattle") or op == "multi_2_vs_2"):
                    continue
                args = [a.strip() for a in line[len(op):].split(",")]
                for t in [a for a in args if a.startswith("TRAINER_")]:
                    calls[t].append(dict(label=label, file=rel(info["file"]), line=n, op=op, args=args))
        return calls

    def _paths(self, trainer: str) -> list[tuple]:
        out = []
        for call in self.calls.get(trainer, []):
            for req, where in self.scripts.path_requirements(call["label"], call["line"], self.keep):
                req = tuple(t for t in req if t != f"DEFEATED:{trainer}")
                out.append((req, where, call))
        return out

    def _requirement(self, trainer: str, req: tuple) -> list[str]:
        extra = ENCOUNTER_REQUIREMENTS.get(trainer, ([], ""))[0]
        return sorted(set(req) | set(extra))

    def _fightable(self) -> dict[str, list[str]]:
        """trainer -> windows in which it can be fought (fixed point over
        DEFEATED: prerequisites between trainers)."""
        names = self.story.window_names()
        fight: dict[str, set[str]] = {t: set() for t in self.team_trainers}
        caps = {n: self.story.cap(n) for n in names}
        forbid_closure = {}

        def closure_for(w, forbid):
            key = (w, forbid)
            if key not in forbid_closure:
                c = self.story.close(caps[w], forbid)
                ok = mr.Story.realizes(w, self.story.flag_of(w), c["flags"])
                forbid_closure[key] = c if ok else None
            return forbid_closure[key]

        changed = True
        while changed:
            changed = False
            for t in self.team_trainers:
                forbid = frozenset(mr.canon(f) for f in ENCOUNTER_CONSEQUENCES.get(t, []) + self.vanish.get(t, []))
                for w in names:
                    if w in fight[t]:
                        continue
                    c = closure_for(w, forbid) if forbid else self.story.closure[w]
                    if c is None:
                        continue
                    for req, where, _call in self.paths[t]:
                        tokens = self._requirement(t, req)
                        flags_ok, deps, at = split_tokens(tokens)
                        if not self.story.reachable(w, where, flags_ok or None, closure=c):
                            continue
                        if not all(self.story.reachable(w, m, closure=c) for m in at):
                            continue
                        if all(any(self.window_index[x] <= self.window_index[w] for x in fight.get(dep, ())) for dep in deps):
                            fight[t].add(w)
                            changed = True
                            break
        return {t: sorted(ws, key=lambda w: self.window_index[w]) for t, ws in fight.items()}

    def first_window(self, trainer: str) -> str | None:
        ws = self.fightable.get(trainer)
        return ws[0] if ws else None

    def defeated_by(self, trainer: str, window: str, *, closure=None) -> bool:
        """DEFEATED:<trainer> can hold in `window`: fought then or earlier,
        and the flags its victory sets (a badge, for a leader) fit the
        window. Non-campaign trainers are unconstrained."""
        if trainer not in self.fightable:
            return True
        ws = self.fightable[trainer]
        if not ws or self.window_index[ws[0]] > self.window_index[window]:
            return False
        flags = (closure if closure is not None else self.story.closure[window])["flags"]
        return all(mr.canon(f) in flags for f in ENCOUNTER_CONSEQUENCES.get(trainer, []))

    def build(self) -> list[dict]:
        story = self.story
        rows = []
        required_cache = {}
        champion = story.window_names()[-1]
        for br in self.branches:
            t = br.trainer
            calls = self.calls.get(t, [])
            call = calls[0] if calls else None
            ws = self.fightable.get(t, [])
            first = ws[0] if ws else None
            where = None
            reqs = []
            beat = 10 ** 6
            for req, wh, c in self.paths.get(t, []):
                tokens = self._requirement(t, req)
                flags_ok, _deps, at = split_tokens(tokens)
                if first and story.reachable(first, wh, flags_ok or None) and all(story.reachable(first, m) for m in at):
                    b = story.beat(first, wh, flags_ok or None)
                    if b < beat:
                        beat, where, reqs, call = b, wh, tokens, c
            fmt, partner, co_owner = "double", None, None
            if call and call["op"] == "multi_2_vs_2":
                fmt = "multi (2 vs 2 with an ally)"
                owners = [a for a in call["args"] if a.startswith("TRAINER_")]
                co_owner = next((o for o in owners if o != t), None)
                # The ally follows the rival's gender/starter branch: list them all.
                partner = sorted({(c["args"][-1] if c["args"][-1].startswith("PARTNER_") else "PARTNER_" + c["args"][-1])
                                  for c in calls if c["op"] == "multi_2_vs_2"})
            elif call and "two_trainers" in call["op"]:
                fmt = "double (two trainers)"
            kinds = {e[3] for e in self.scripts.reach_entries(call["label"])[0]} if call else set()
            consequences = ENCOUNTER_CONSEQUENCES.get(t, [])
            if consequences:
                key = frozenset(mr.canon(f) for f in consequences)
                if key not in required_cache:
                    c = story.close(story.cap(champion), key)
                    required_cache[key] = "EV_BUFFEL_DONE" not in c["flags"]
                requirement = "required" if required_cache[key] else (
                    "forced (story trigger)" if "coord" in kinds else "optional")
            elif "coord" in kinds or (call and call["op"].startswith("trainerbattle_no_intro") and "object" not in kinds):
                requirement = "forced (story trigger)"
            else:
                requirement = "optional"
            m = self.master.get(br.encounter, {})
            strict = int(m["strict_cap"]) if m.get("strict_cap", "").isdigit() else None
            first_cap = story.cap(first) if first else None
            group = None
            gm = re.match(r"TRAINER_(BRENDAN|MAY)_(\w+?)_(MUDKIP|TORCHIC|TREECKO)$", t)
            if gm:
                group = f"rival_{gm[2].lower()}"
            elif re.match(r"TRAINER_(BRENDAN|MAY)_METEOR_FALLS", t) or br.encounter == 127:
                group = "meteor_falls_multi"
            where_s = where if isinstance(where, str) or where is None else f"{where[0]} ({where[1]},{where[2]})"
            rows.append(dict(
                encounter=f"E{br.encounter:04d}", trainer=t, trainer_class=br.cls,
                team_size=len(br.mons), mega_slots=br.mega_slots,
                map=(where[0] if isinstance(where, tuple) else where) if where else (call and call["file"].split("/")[2] if call and call["file"].startswith("data/maps") else None),
                location=where_s,
                battle_script=f"{call['file']}:{call['line']} ({call['label']})" if call else None,
                format=fmt, partner=partner, co_owner=co_owner,
                first_milestone=first, first_cap=first_cap,
                fightable_milestones=ws,
                replay_milestones=[champion] if t in REPLAYABLE_AFTER_CHAMPION else [],
                requires=reqs, requirement_note=ENCOUNTER_REQUIREMENTS.get(t, (None, None))[1],
                victory_sets=consequences, requirement=requirement,
                master_requirement=m.get("requirement"), master_strict_cap=strict,
                strict_cap_disagrees=(strict is not None and first_cap is not None and strict != first_cap),
                rival_group=group, beat=beat if beat < 10 ** 6 else None,
                review_index=self.review.get(t),
            ))
        idx = self.window_index
        rows.sort(key=lambda r: (idx.get(r["first_milestone"], 99), r["beat"] if r["beat"] is not None else 10 ** 6,
                                 r["review_index"] if r["review_index"] is not None else 10 ** 6, r["encounter"], r["trainer"]))
        for i, r in enumerate(rows, 1):
            r["story_order"] = i
        return rows


STORY_FLAGS = {mr.canon(f) for f, *_ in mr.STORY_EVENTS} | set(mr.MILESTONE_FLAGS) | {"FLAG_SYS_GAME_CLEAR"}


# ---------------------------------------------------------------------------
# Outputs
# ---------------------------------------------------------------------------

NOTABLE_ITEMS = ["ITEM_LIFE_ORB", "ITEM_CHOICE_BAND", "ITEM_CHOICE_SPECS", "ITEM_CHOICE_SCARF", "ITEM_FOCUS_SASH",
                 "ITEM_LEFTOVERS", "ITEM_EVIOLITE", "ITEM_ASSAULT_VEST", "ITEM_EXPERT_BELT", "ITEM_WEAKNESS_POLICY",
                 "ITEM_SITRUS_BERRY", "ITEM_LUM_BERRY", "ITEM_ROCKY_HELMET", "ITEM_HEAVY_DUTY_BOOTS", "ITEM_BOOSTER_ENERGY",
                 "ITEM_LIGHT_CLAY", "ITEM_TOXIC_ORB", "ITEM_FLAME_ORB", "ITEM_CLEAR_AMULET", "ITEM_COVERT_CLOAK",
                 "ITEM_LOADED_DICE", "ITEM_EJECT_PACK", "ITEM_MIRROR_HERB", "ITEM_TERRAIN_EXTENDER", "ITEM_HEAT_ROCK",
                 "ITEM_DAMP_ROCK", "ITEM_SAFETY_GOGGLES", "ITEM_THROAT_SPRAY", "ITEM_BLUNDER_POLICY", "ITEM_MEGA_RING"]


def git_head() -> str:
    try:
        return subprocess.run(["git", "rev-parse", "--short", "HEAD"], cwd=ROOT, capture_output=True, text=True).stdout.strip()
    except OSError:
        return "?"


def build_all() -> dict:
    b = Builder()
    enc = Encounters(b)
    pools = Pools(b, enc)
    rows = enc.build()
    return dict(builder=b, encounters=enc, pools=pools, rows=rows)


def species_row(b: Builder, sp: str, how: dict, gate: str) -> dict:
    sd = b.species
    return dict(species=sp, name=sd.name(sp), gate=gate, bst=sd.bst(sp), stats=sd.stats(sp),
                abilities=sd.abilities(sp), restricted_class=sd.restricted_class(sp),
                source=dict(kind=how.get("kind"), detail=how.get("detail"), where=how.get("where"), cite=how.get("cite")))


def encounter_map(enc, trainer: str) -> str | None:
    """The map whose script starts this battle."""
    for _req, where, _c in enc.paths.get(trainer, []):
        if where:
            return where[0] if isinstance(where, tuple) else where
    calls = enc.calls.get(trainer, [])
    if calls and calls[0]["file"].startswith("data/maps"):
        return calls[0]["file"].split("/")[2]
    return None


def encounter_pool(trainer: str, milestone: str, *, builder=None, encounters=None) -> dict:
    """Fresh source upper bound while this battle is still pending.

    Reuse the existing story graph, but withhold this encounter's victory and
    disappearance flags. In particular, a cap-14 rival cannot borrow resources
    from the post-rival part of the same cap window. Optional detours and earned
    Legendary counters remain available. This is not an earned-save receipt.
    """
    b = builder if builder is not None else Builder()
    enc = encounters if encounters is not None else Encounters(b)
    story = b.story
    windows = story.window_names()
    if milestone not in windows and milestone.isdigit():
        milestone = next((w for w in windows if story.cap(w) == int(milestone)), milestone)
    if milestone not in windows:
        raise ValueError(f"unknown milestone {milestone!r}")
    if trainer not in enc.team_trainers:
        raise ValueError(f"unknown authored trainer {trainer!r}")
    if milestone not in enc.fightable[trainer]:
        raise ValueError(f"{trainer} cannot still be pending in {milestone}")

    # Both enemy owners must remain undefeated for a multi-trainer battle.
    pending = {trainer}
    for call in enc.calls.get(trainer, []):
        pending.update(a for a in call["args"] if a.startswith("TRAINER_"))
    forbid = frozenset(mr.canon(f) for t in pending
                       for f in ENCOUNTER_CONSEQUENCES.get(t, []) + enc.vanish.get(t, []))
    closure = story.close(story.cap(milestone), forbid)
    if not story.realizes(milestone, story.flag_of(milestone), closure["flags"]):
        raise ValueError(f"{milestone} requires defeating {trainer}")
    battle_map = encounter_map(enc, trainer)
    route_maps = walkthrough_order.allowed_maps(battle_map, milestone, story.geo.by_map)
    if route_maps is not None:
        # A route trainer sees only the walkthrough chapters up to its own.
        nodes = {n for m in route_maps for n in story.geo.by_map[m]}
        closure = dict(closure, reached={n: v for n, v in closure["reached"].items() if n in nodes})
    p = Pools(b, enc, compute_windows=False)
    r = p.compute(milestone, closure=closure, pending=frozenset(pending))
    unlimited_kinds = {"vendor_kit", "vendor_badge_stock", "vendor_form_items", "vendor_species", "mart"}
    species = [species_row(b, sp, how, milestone) for sp, how in sorted(r["species"].items())
               if how.get("kind") != "mega"]
    items = [dict(item=it, unlimited=it in b.vendor_items or how.get("kind") in unlimited_kinds,
                  source=dict(kind=how.get("kind"), detail=how.get("detail"), cite=how.get("cite")))
             for it, how in sorted(r["items"].items())]
    return dict(
        trainer=trainer, encounter=next(f"E{br.encounter:04d}" for br in enc.branches if br.trainer == trainer),
        pending_trainers=sorted(pending), excluded_victory_flags=sorted(forbid),
        milestone=milestone, cap=r["cap"], source_head=git_head(), story_flags=sorted(r["flags"]),
        battle_map=battle_map,
        scope=("Walkthrough-order pool: chapters up to this route (scripts/walkthrough_order.py)."
               if route_maps is not None else
               "Source upper bound before this encounter; optional detours, money and RNG are unconstrained."),
        species=species, items=items, megas=r["megas"], all_mega_stones=sorted(p.megas),
        mega_ring=dict(available=r["ring"], gate="Wattson's victory reward after badge three"),
        friendship=dict(max_this_milestone=255),
        game_corner_available=story.reachable(milestone, "MauvilleCity_GameCorner", closure=closure),
        starter_lines=starter_lines(b, r),
        iv_service_available=story.reachable(milestone, "FallarborTown_MoveRelearnersHouse", closure=closure),
        hot_spring_available=story.reachable(milestone, "LavaridgeTown", closure=closure),
    )


def write_outputs(result: dict) -> list[str]:
    b, enc, pools, rows = result["builder"], result["encounters"], result["pools"], result["rows"]
    story = b.story
    windows = story.window_names()
    out_pools = OUT_DIR / "pools"
    out_pools.mkdir(parents=True, exist_ok=True)
    first_species: dict[str, str] = {}
    first_items: dict[str, str] = {}
    for w in windows:
        for sp in pools.result[w]["species"]:
            first_species.setdefault(sp, w)
        for it in pools.result[w]["items"]:
            first_items.setdefault(it, w)
    ring_gate = next((w for w in windows if pools.result[w]["ring"]), None)
    rules = player_rules()
    friendship = friendship_model(story)
    sanity = []
    head = git_head()
    unlimited_kinds = {"vendor_kit", "vendor_badge_stock", "vendor_form_items", "vendor_species", "mart"}
    for w in windows:
        r = pools.result[w]
        cap = r["cap"]
        species = []
        for sp, how in sorted(r["species"].items()):
            if how.get("kind") == "mega":
                continue
            species.append(species_row(b, sp, how, first_species[sp]))
        legend = [dict(s, gate_note=b.legend_requirement(s["species"])[2]) for s in species if s["restricted_class"]]
        megas = [dict(species=m["species"], base=m["base"], stone=m["stone"],
                      gate=first_species.get(m["species"]), stone_gate=first_items.get(m["stone"]) if m["stone"] else None,
                      stone_source=m["stone_source"], bst=b.species.bst(m["species"])) for m in r["megas"]]
        items = []
        for it, how in sorted(r["items"].items()):
            in_vendor = it in b.vendor_items
            items.append(dict(item=it, gate=first_items[it], vendor_catalogue=in_vendor,
                              unlimited=in_vendor or how.get("kind") in unlimited_kinds,
                              source=dict(kind=how.get("kind"), detail=how.get("detail"), where=how.get("where"), cite=how.get("cite"))))
        pool = dict(
            milestone=w, cap=cap, flag=story.flag_of(w), source_head=head,
            window=("Every save state whose campaign cap is %d (src/caps.c GetCampaignLevelCap). Upper bound: anything "
                    "obtainable in any such state counts (optional detours included, e.g. skipping Winona until the "
                    "League door); nothing is counted before its gate." % cap),
            story_flags=sorted(r["flags"]),
            player_rules=rules,
            friendship=dict(friendship, max_this_milestone=friendship["per_window"][w]),
            mega_ring=dict(available=r["ring"], gate=ring_gate, cite="data/maps/MauvilleCity_Gym/scripts.inc:MauvilleCity_Gym_EventScript_GiveMegaRing; src/battle_util.c"),
            starter_rule=("At most two starters, both from the region picked at the start, until the Mauville Game Corner "
                          "starter archive is reachable (then any starter for 500 coins). src/emerald_champions_story.c "
                          "GiveEmeraldChampionsStarterPair; src/field_specials.c sEmeraldChampionsGameCornerPokemonPrizes"),
            game_corner_gate=b.story.first_window("MauvilleCity_GameCorner"),
            starter_lines=starter_lines(b, r),
            milestone_order=windows,
            all_mega_stones=sorted(pools.megas),
            counts=dict(species=len(species), legend_class=len(legend), megas=len(megas), items=len(items)),
            species=species, legend_class=legend, megas=megas, items=items,
        )
        (out_pools / f"pool-{w}.json").write_text(json.dumps(pool, indent=1) + "\n")
        (out_pools / f"pool-{w}.txt").write_text(render_summary(b, pool) + "\n")
        top = sorted((s for s in species if not s["restricted_class"]), key=lambda s: -s["bst"])[:5]
        top_leg = max(legend, key=lambda s: s["bst"], default=None)
        top_mega = max(megas, key=lambda m: m["bst"], default=None)
        sanity.append((w, cap, len(species), len(legend), len(megas), len(items),
                       ", ".join(f"{s['name']} {s['bst']}" for s in top),
                       f"{top_leg['name']} {top_leg['bst']}" if top_leg else "-",
                       f"{b.species.name(top_mega['species'])} {top_mega['bst']}" if top_mega else "-"))
    enc_json = dict(
        source_head=head, count=len(rows),
        note=("Story order = first milestone in which the battle can be fought, then closure round (beat), then the "
              "review index. first_milestone is the EARLIEST legal cap; fightable_milestones lists every milestone in "
              "which it can still be pending (trainer levels follow the live cap). strict_cap_disagrees compares with "
              "the master file's historical strict_cap."),
        per_milestone={w: sum(1 for r in rows if r["first_milestone"] == w) for w in windows},
        encounters=rows,
    )
    (OUT_DIR / "encounters.json").write_text(json.dumps(enc_json, indent=1) + "\n")
    lines = ["milestone  cap  species legend megas items  top-5 non-restricted by BST (Inclement stats) | best restricted | best Mega"]
    for w, cap, ns, nl, nm, ni, top, leg, mega in sanity:
        lines.append(f"{w:9s} {cap:4d} {ns:8d} {nl:6d} {nm:5d} {ni:5d}  {top} | {leg} | {mega}")
    lines.append("")
    lines.append("encounters by first milestone: " + ", ".join(f"{w}={enc_json['per_milestone'][w]}" for w in windows))
    dis = [r for r in rows if r["strict_cap_disagrees"]]
    lines.append(f"strict_cap disagreements: {len(dis)} of {len(rows)} branches")
    unresolved = story.unresolved_gate_tokens()
    lines.append("gate tokens with no story event: " + (", ".join(unresolved) if unresolved else "none"))
    (OUT_DIR / "sanity.txt").write_text("\n".join(lines) + "\n")
    return lines


def starter_lines(b: Builder, r: dict) -> dict:
    """Starter lines in this pool. `pick_only` lines are reachable only
    through the opening pick (and its evolutions/breeding): a party may hold
    at most two such lines, both from one region, until the Game Corner
    starter archive is reachable."""
    out = {}
    for i, region in enumerate(b.starter_regions, 1):
        for root in region:
            fam = sorted(b.species.family(root) & set(r["species"]))
            kinds = {r["species"][sp]["kind"] for sp in fam}
            out[root] = dict(region=i, species=fam,
                             pick_only=kinds <= {"starter", "evolution", "breeding", "form", "mega"})
    return out


def render_summary(b: Builder, pool: dict) -> str:
    sd = b.species
    w, cap = pool["milestone"], pool["cap"]
    out = [f"POOL {w} (cap {cap}) -- {pool['counts']}", pool["window"], "",
           f"Mega Ring: {'yes' if pool['mega_ring']['available'] else 'no'} (first at {pool['mega_ring']['gate']})",
           f"Friendship max: {pool['friendship']['max_this_milestone']} (Center Bonding)",
           f"Starters: {pool['starter_rule']}", ""]
    out.append("LEGEND-CLASS (one per party): species [class] gate <- source")
    for s in sorted(pool["legend_class"], key=lambda s: -s["bst"]):
        out.append(f"  {s['name']:28s} [{s['restricted_class']}] gate={s['gate']} BST={s['bst']} <- {s['source']['kind']}: "
                   f"{s['source']['detail']} @ {s['source']['where']}" + (f" ({s['gate_note']})" if s.get("gate_note") else ""))
    out.append("")
    out.append("MEGAS: form (stone gate) <- stone source")
    for m in sorted(pool["megas"], key=lambda m: -m["bst"]):
        src = m["stone_source"] or {}
        out.append(f"  {sd.name(m['species']):28s} BST={m['bst']} gate={m['gate']} stone={m['stone']} <- "
                   f"{src.get('kind')}: {src.get('detail')} @ {src.get('where')}")
    out.append("")
    out.append("NOTABLE HELD ITEMS: item gate unlimited <- source")
    by = {i["item"]: i for i in pool["items"]}
    for it in NOTABLE_ITEMS:
        i = by.get(it)
        out.append(f"  {it:24s} " + (f"gate={i['gate']} unlimited={i['unlimited']} <- {i['source']['kind']}: "
                                      f"{i['source']['detail']} @ {i['source']['where']}" if i else "NOT AVAILABLE"))
    out.append("")
    out.append("SPECIES (non-restricted, by BST): name BST gate <- source kind")
    rows = sorted((s for s in pool["species"] if not s["restricted_class"]), key=lambda s: (-s["bst"], s["name"]))
    for s in rows:
        out.append(f"  {s['name']:30s} {s['bst']:4d} gate={s['gate']:9s} {s['source']['kind']}: {s['source']['detail']}")
    return "\n".join(out)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--explain", metavar="MAP_NAME", help="print a map directory's first window and exit")
    args = parser.parse_args()
    if args.explain:
        print(mr.explain_map_window(args.explain))
        return
    result = build_all()
    lines = write_outputs(result)
    print("\n".join(lines))
    print(f"\nwrote {OUT_DIR}/pools/pool-<milestone>.json|.txt, encounters.json, sanity.txt")


if __name__ == "__main__":
    main()
