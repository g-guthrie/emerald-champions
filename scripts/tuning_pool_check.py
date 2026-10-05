#!/usr/bin/env python3
"""Check a tuning party against resources available before its trainer battle.

The manifest uses the scripts/playthrough/prepare_party.py format:
  {"encounter": ..., "availability_audit": ..., "party": [
     {"species", "nature", "ability", "item", "moves"[4], "evs"[6],
      "ivs"?, "friendship"?, "pokerus"?, "level"?, "availability", "role"}]}

The pool is work/tuning-20260930/pools/pool-<milestone>.json written by
scripts/reference_pool.py. Checks: every member names an `availability`
citation; species/form is in the pool; a Mega Stone needs its Mega form in
the pool and the Mega Ring (the party carries base forms, prepare_party
does not accept Mega species); held items are in the pool, and a
non-restocking item is not held twice; the Ability is one the party menu
offers; one Legendary/Mythical/Ultra Beast/Paradox in total; no one-hit KO
or evasion boost (never tuned for); level equals
the cap when given; friendship does not exceed the milestone maximum;
IV/EV bounds; the opening starter pick rule before the Game Corner.

  python3 scripts/tuning_pool_check.py party.json --milestone start --trainer TRAINER_BRENDAN_ROUTE_103_MUDKIP
  python3 scripts/tuning_pool_check.py party.json --milestone badge3 --window-only

Encounter checks compute a fresh pool from source with the battle's victory
flags withheld. --window-only is inventory screening, not pre-battle proof.
Source pools are upper bounds; they do not prove finite preparation, compatible
story choices or earned progress. Use battle_opening_arsenal certificates for
the first rival's concrete preparation and regional starter branch.
Prints PASS, or FAIL with one line per problem (exit status 1).
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from collections import Counter
import evolution_move_gate
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
POOLS = ROOT / "work/tuning-20260930/pools"
PROGRESSION = ROOT / "work/progression-manifest/progression.pickle"
# Owner rule: the iconic one-hit KOs (hatched Lapras, Spheal, Rhyhorn, Krabby,
# Diglett and Swinub lines only) are outliers the campaign is never tuned
# around; the evasion boosts are out of the game (src/pokemon.c
# IsMoveRemovedFromGame).
UNTUNED_MOVES = {"MOVE_SHEER_COLD", "MOVE_HORN_DRILL", "MOVE_GUILLOTINE", "MOVE_FISSURE",
                 "MOVE_DOUBLE_TEAM", "MOVE_MINIMIZE"}


def load_pool(milestone: str, pools_dir: Path = POOLS) -> dict:
    """Pool by milestone name (start, badge1..badge8, groudon, champion) or cap."""
    path = pools_dir / f"pool-{milestone}.json"
    if path.exists():
        return json.loads(path.read_text())
    if str(milestone).isdigit():
        for p in sorted(pools_dir.glob("pool-*.json")):
            pool = json.loads(p.read_text())
            if pool.get("cap") == int(milestone):
                return pool
    names = sorted(p.stem[5:] for p in pools_dir.glob("pool-*.json"))
    raise SystemExit(f"no pool for milestone {milestone!r} in {pools_dir} (have: {', '.join(names) or 'none'}; "
                     "run scripts/reference_pool.py)")


def species_aliases() -> dict[str, str]:
    """SPECIES_X = SPECIES_Y enum aliases (include/constants/species.h), so a
    manifest may use either spelling."""
    try:
        from verify_trainer_ability_legality import species_aliases as aliases
        return aliases()
    except Exception:  # pragma: no cover - the checker still works on canonical names
        return {}


def resolve(species: str, aliases: dict[str, str]) -> str:
    seen = 0
    while species in aliases and seen < 4:
        species = aliases[species]
        seen += 1
    return species


def ivy_iv_spreads() -> set[tuple[int, ...]]:
    """Only spreads reachable through the actual IV/Hidden Power menus.

    Player acquisitions start at 31; Ivy offers 0/31 Attack and Speed, plus
    the authored Hidden Power templates. Other arbitrary IVs cannot be set.
    Templates use native stat order; manifests use display order
    HP, Attack, Defense, Sp. Attack, Sp. Defense, Speed (EC_IV_DATA).
    """
    text = (ROOT / "src/inclement_stat_services.c").read_text()
    body = text.split("sHiddenPowerSpreads[16][NUM_STATS] = {", 1)[1].split("};", 1)[0]
    seeds = [(31,) * 6] + [tuple(map(int, row.split(",")))
                              for row in re.findall(r"\{([\d, ]+)\}", body)]
    order_text = (ROOT / "src/emerald_champions_battle_sets.c").read_text()
    order_body = order_text.split("gEmeraldChampionsEvOrder[NUM_STATS] =", 1)[1].split("};", 1)[0]
    native_order = ["STAT_HP", "STAT_ATK", "STAT_DEF", "STAT_SPEED", "STAT_SPATK", "STAT_SPDEF"]
    order = [native_order.index(stat) for stat in re.findall(r"STAT_\w+", order_body)]
    attack_index, speed_index = order.index(1), order.index(3)
    result = set()
    for native_seed in seeds:
        seed = tuple(native_seed[i] for i in order)
        for attack in {seed[attack_index], 0, 31}:
            for speed in {seed[speed_index], 0, 31}:
                row = list(seed)
                row[attack_index], row[speed_index] = attack, speed
                result.add(tuple(row))
    return result


def check(manifest: dict, pool: dict, aliases: dict[str, str] | None = None) -> list[str]:
    aliases = aliases or {}
    problems: list[str] = []
    party = manifest.get("party") or []
    cap = pool["cap"]
    milestone = pool["milestone"]
    species_rows = {s["species"]: s for s in pool["species"]}
    megas_by_stone: dict[str, list[dict]] = {}
    for m in pool["megas"]:
        if m.get("stone"):
            megas_by_stone.setdefault(m["stone"], []).append(m)
    items = {i["item"]: i for i in pool["items"]}
    fmax = pool.get("friendship", {}).get("max_this_milestone", 255)
    all_mega_stones = set(pool.get("all_mega_stones", [])) | set(megas_by_stone)
    iv_spreads = None
    if "iv_service_available" in pool:
        iv_spreads = ivy_iv_spreads() if pool["iv_service_available"] else {(31,) * 6}

    encounter = str(manifest.get("encounter", ""))
    claimed_trainer = manifest.get("trainer") or (encounter if encounter.startswith("TRAINER_") else None)
    if pool.get("trainer") and claimed_trainer and claimed_trainer != pool["trainer"]:
        problems.append(f"manifest names {claimed_trainer}, not the checked trainer {pool['trainer']}")
    if pool.get("encounter") and re.fullmatch(r"E\d{4}", encounter) and encounter != pool["encounter"]:
        problems.append(f"manifest names {encounter}, not the checked encounter {pool['encounter']}")

    if not 1 <= len(party) <= 6:
        problems.append(f"party has {len(party)} members (prepare one to six)")
    if not manifest.get("availability_audit"):
        problems.append("manifest has no availability_audit")
    restricted = []
    held = Counter()
    for slot, mon in enumerate(party, 1):
        tag = f"slot {slot} {mon.get('species', '?')}"
        if not str(mon.get("availability", "")).strip():
            problems.append(f"{tag}: missing `availability` citation")
        sp = resolve(str(mon.get("species", "")), aliases)
        row = species_rows.get(sp)
        if re.search(r"_MEGA(_[XYZ])?$|_PRIMAL$", sp):
            problems.append(f"{tag}: list the base form; Mega/Primal forms happen in battle from the held item")
        elif row is None:
            problems.append(f"{tag}: {sp} is not obtainable by {milestone} (cap {cap})")
        else:
            if row.get("restricted_class"):
                restricted.append((slot, sp, row["restricted_class"]))
            ability = mon.get("ability")
            if ability and row.get("abilities") and ability not in row["abilities"]:
                problems.append(f"{tag}: {ability} is not a party-menu Ability for {sp} (offers {', '.join(row['abilities'])})")
        item = mon.get("item") or "ITEM_NONE"
        if item != "ITEM_NONE":
            info = items.get(item)
            if info is None:
                problems.append(f"{tag}: held item {item} is not obtainable by {milestone}")
            else:
                held[item] += 1
            if item in all_mega_stones:
                forms = [m for m in megas_by_stone.get(item, []) if m.get("base") == sp]
                if not pool.get("mega_ring", {}).get("available"):
                    problems.append(f"{tag}: {item} is a Mega Stone but the Mega Ring is not available until "
                                    f"{pool.get('mega_ring', {}).get('gate')}")
                elif not forms:
                    problems.append(f"{tag}: {item} does not Mega Evolve {sp} in this pool (no such Mega by {milestone})")
        for move in mon.get("moves") or []:
            if not evolution_move_gate.ready(sp, move, mon.get("level", cap), pool.get("story_flags", [])):
                problems.append(f"{tag}: {move} is an evolution trigger locked at this level and badge count")
            if move in UNTUNED_MOVES:
                problems.append(f"{tag}: {move} is never tuned for (one-hit KOs are hatchling-only outliers; "
                                "evasion boosts are removed)")
        level = mon.get("level")
        if level is not None and level != cap:
            problems.append(f"{tag}: level {level} must equal the {milestone} cap {cap}")
        friendship = mon.get("friendship")
        if friendship is not None and (type(friendship) is not int or not 0 <= friendship <= fmax):
            problems.append(f"{tag}: friendship {friendship} exceeds the {milestone} maximum {fmax}")
        ivs = mon.get("ivs", [31] * 6)
        if len(ivs) != 6 or any(type(v) is not int or not 0 <= v <= 31 for v in ivs):
            problems.append(f"{tag}: IVs must be six values 0-31")
        elif iv_spreads is not None and tuple(ivs) not in iv_spreads:
            problems.append(f"{tag}: IV spread is not obtainable through acquisition or Ivy before this encounter")
        if "hot_spring_available" in pool:
            pokerus = mon.get("pokerus", 0)
            # New infections use FC/FD/FE; soaking and treatment use F8..FB.
            # These are the saved-byte states authored in src/pokerus.c.
            allowed = {0, 0xFC, 0xFD, 0xFE}
            if pool["hot_spring_available"]:
                allowed |= {0xF8, 0xF9, 0xFA, 0xFB}
            if type(pokerus) is not int or pokerus not in allowed:
                problems.append(f"{tag}: Pokérus state is not obtainable before this encounter (hot-spring access required for F8..FB)")
        evs = mon.get("evs", [])
        if len(evs) != 6 or any(type(v) is not int or not 0 <= v <= 252 for v in evs) or sum(evs) > 510:
            problems.append(f"{tag}: EVs must be six values 0-252 totalling at most 510")
    for item, count in held.items():
        if count > 1 and not items[item].get("unlimited"):
            problems.append(f"{item} is held by {count} members but no restocking source exists by {milestone} "
                            f"(only {items[item]['source']['kind']}: {items[item]['source']['detail']})")
    if len(restricted) > 1:
        problems.append("more than one restricted Pokemon (Legendary/Mythical/Ultra Beast/Paradox, Gholdengo or either Ursaluna form): "
                        + ", ".join(f"slot {s} {sp} ({c})" for s, sp, c in restricted)
                        + " (src/pokemon.c GetRestrictedPartyClass / PlayerPartyWithinRestrictedLimit)")
    gc = pool.get("game_corner_gate")
    order = pool.get("milestone_order") or []
    before_gc = gc is None or (order and milestone in order and gc in order and order.index(milestone) < order.index(gc))
    if "game_corner_available" in pool:
        before_gc = not pool["game_corner_available"]
    lines = pool.get("starter_lines") or {}
    if before_gc and lines:
        used = {}
        for mon in party:
            sp = resolve(str(mon.get("species", "")), aliases)
            for root, line in lines.items():
                if line.get("pick_only") and sp in line.get("species", []):
                    used[root] = line["region"]
        if len(used) > 2 or len(set(used.values())) > 1:
            problems.append("starter pick: at most two starter lines, both from one region, before the Game Corner "
                            f"archive ({gc}); party uses {', '.join(sorted(used))}")
    return problems


def narrow_to_progression(pool: dict, trainer: str, path: Path) -> tuple[dict, str]:
    """Keep only what the battle-ordered progression has opened before this
    battle. The milestone pool is a broad upper bound; the progression orders
    every victory, so it withholds resources a same-cap later fight opens."""
    import pickle
    from manifest_text import resources_before
    have = resources_before(pickle.loads(path.read_bytes()), trainer)
    if have["cap"] != pool["cap"]:
        pool = dict(pool, cap=have["cap"])
    # The progression follows the Treecko + Mudkip reference pick. Any other
    # starter line may stand in for those two at the same evolution stage.
    from reference_pool import SpeciesData
    sd = SpeciesData()

    def stage(sp):
        depth, cur = 0, sp
        while sd.pre_evo.get(cur):
            cur, depth = sorted(sd.pre_evo[cur])[0], depth + 1
        return depth
    reference = sd.family("SPECIES_TREECKO") | sd.family("SPECIES_MUDKIP")
    stages = {stage(sp) for sp in have["species"] & reference}
    starter_species = {sp for line in (pool.get("starter_lines") or {}).values() for sp in line.get("species", [])}

    def keep(sp):
        return sp in have["species"] or (sp in starter_species and stage(sp) in stages)
    pool = dict(pool, species=[s for s in pool["species"] if keep(s["species"])],
                items=[i for i in pool["items"] if i["item"] in have["items"]])
    return pool, f" (progression battle #{have['number']}, cap {have['cap']})"


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("party", type=Path, help="party manifest (scripts/playthrough/prepare_party.py format)")
    parser.add_argument("--milestone", required=True, help="start, badge1..badge8, groudon, champion, or a cap")
    parser.add_argument("--pools", type=Path, default=POOLS, help="pool directory (default: %(default)s)")
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--trainer", help="TRAINER_* whose victory/resources must remain unavailable")
    mode.add_argument("--window-only", action="store_true", help="screen a broad inventory pool; not encounter legality proof")
    parser.add_argument("--progression", type=Path, default=PROGRESSION,
                        help="battle-ordered progression from manifest_campaign.py (default: %(default)s)")
    args = parser.parse_args(argv)
    manifest = json.loads(args.party.read_text())
    trainer = args.trainer or manifest.get("trainer")
    if args.window_only:
        pool = load_pool(args.milestone, args.pools)
    else:
        if not trainer:
            encounter = str(manifest.get("encounter", ""))
            trainer = encounter if encounter.startswith("TRAINER_") else None
        if not trainer:
            print("FAIL: name --trainer for encounter availability; --window-only is inventory screening only")
            return 1
        from reference_pool import encounter_pool
        try:
            pool = encounter_pool(trainer, args.milestone)
        except ValueError as exc:
            print(f"FAIL: {exc}")
            return 1
    narrowed = ""
    if trainer and not args.window_only:
        if not args.progression.exists():
            print(f"FAIL: {args.progression} is missing; run scripts/manifest_campaign.py first")
            return 1
        pool, narrowed = narrow_to_progression(pool, trainer, args.progression)
    problems = check(manifest, pool, species_aliases())
    if problems:
        print(f"FAIL {args.party} vs {pool['milestone']} (cap {pool['cap']}):")
        for p in problems:
            print(f"  - {p}")
        return 1
    scope = "WINDOW INVENTORY ONLY" if args.window_only else f"before {trainer}{narrowed}"
    print(f"PASS {args.party} vs {pool['milestone']} (cap {pool['cap']}): {len(manifest['party'])} members; {scope}")
    return 0


if __name__ == "__main__":
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    sys.exit(main())
