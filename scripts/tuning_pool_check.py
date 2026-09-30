#!/usr/bin/env python3
"""Check a tuning party manifest against a milestone availability pool.

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
offers; one Legendary/Mythical/Ultra Beast/Paradox in total; level equals
the cap when given; friendship does not exceed the milestone maximum;
IV/EV bounds; the opening starter pick rule before the Game Corner.

  python3 scripts/tuning_pool_check.py party.json --milestone badge3
  python3 scripts/tuning_pool_check.py party.json --milestone 40
Prints PASS, or FAIL with one line per problem (exit status 1).
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
POOLS = ROOT / "work/tuning-20260930/pools"


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
        level = mon.get("level")
        if level is not None and level != cap:
            problems.append(f"{tag}: level {level} must equal the {milestone} cap {cap}")
        friendship = mon.get("friendship")
        if friendship is not None and (type(friendship) is not int or not 0 <= friendship <= fmax):
            problems.append(f"{tag}: friendship {friendship} exceeds the {milestone} maximum {fmax}")
        ivs = mon.get("ivs", [31] * 6)
        if len(ivs) != 6 or any(type(v) is not int or not 0 <= v <= 31 for v in ivs):
            problems.append(f"{tag}: IVs must be six values 0-31")
        evs = mon.get("evs", [])
        if len(evs) != 6 or any(type(v) is not int or not 0 <= v <= 252 for v in evs) or sum(evs) > 510:
            problems.append(f"{tag}: EVs must be six values 0-252 totalling at most 510")
    for item, count in held.items():
        if count > 1 and not items[item].get("unlimited"):
            problems.append(f"{item} is held by {count} members but no restocking source exists by {milestone} "
                            f"(only {items[item]['source']['kind']}: {items[item]['source']['detail']})")
    if len(restricted) > 1:
        problems.append("more than one Legendary/Mythical/Ultra Beast/Paradox: "
                        + ", ".join(f"slot {s} {sp} ({c})" for s, sp, c in restricted)
                        + " (src/pokemon.c GetRestrictedPartyClass / PlayerPartyWithinRestrictedLimit)")
    gc = pool.get("game_corner_gate")
    order = pool.get("milestone_order") or []
    before_gc = gc is None or (order and milestone in order and gc in order and order.index(milestone) < order.index(gc))
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


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("party", type=Path, help="party manifest (scripts/playthrough/prepare_party.py format)")
    parser.add_argument("--milestone", required=True, help="start, badge1..badge8, groudon, champion, or a cap")
    parser.add_argument("--pools", type=Path, default=POOLS, help="pool directory (default: %(default)s)")
    args = parser.parse_args(argv)
    manifest = json.loads(args.party.read_text())
    pool = load_pool(args.milestone, args.pools)
    problems = check(manifest, pool, species_aliases())
    if problems:
        print(f"FAIL {args.party} vs {pool['milestone']} (cap {pool['cap']}):")
        for p in problems:
            print(f"  - {p}")
        return 1
    print(f"PASS {args.party} vs {pool['milestone']} (cap {pool['cap']}): {len(manifest['party'])} members")
    return 0


if __name__ == "__main__":
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    sys.exit(main())
