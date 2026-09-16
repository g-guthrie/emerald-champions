#!/usr/bin/env python3
"""Drift check between the hand-authored guide/docs and current source.

The Game Book's hand-authored guide (everything above the sync_game_book.py
BEGIN marker), README.md, docs/CONTINUE.md and docs/GOAL.md all restate
numbers that also live in source: the roster census, economy prices, cap/
stipend milestones, retired items and Mega Stone acquisition. Prose drifts
out of sync with source over time; this script is the drift detector.

It deliberately reuses the existing loaders instead of re-parsing:
  - scripts/emerald_champions_teams.py (read_teams) for the authored roster
  - scripts/sync_game_book.py (split_guide, source_inventory) for the guide
    text and native pokemart stock tables
  - scripts/economy_reference.py (item_prices) for configured cash prices
  - scripts/generate_emerald_champions_mega_archive.py (stones) and
    scripts/verify_mega_stone_rewards.py (world_reward_sources) for Mega
    Stone acquisition sources
  - docs/trainer-review-index.json for the authored count_rule (368 authored
    variants minus 25 rival gender/starter aliases minus 2 extra owners in
    two-owner battles = 341 encounters); only trusted when its source_sha256
    still matches the current battle teams file.
  - scripts/mega_register.py (build_register) for the generated MEGA REGISTER
    section's NO TRAINER HOLDER count.
  - scripts/acceptance_ladder.py (compute_ladder), the official maintainer of
    docs/trainer-review-index.json's per-encounter "ladder" field and
    source_sha256, for the generated ACCEPTANCE LADDER summary counts.

Each check prints PASS or FAIL with the offending values; the process exits
1 if any check fails. Run directly:
    python3 scripts/check_book_consistency.py
Add --fix-readme to rewrite only the roster-count sentence in README.md's
"Campaign roster" table row from the freshly computed numbers.

This script is wired into scripts/verify_emerald_champions_release.py as a
release-blocking gate (since September 15, 2026; pass --lenient-book to that
script to downgrade it to advisory). Historical note: the current guide/README/docs text is known to
contradict itself (see check 1), and failing the release gate on prose drift
before that prose is fixed would block unrelated release work. Pass
--strict-book to scripts/verify_emerald_champions_release.py once the
contradictions found here are resolved, to make this gate release-blocking.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(Path(__file__).resolve().parent))

import emerald_champions_teams as teams
import sync_game_book as book
from economy_reference import item_prices
from generate_emerald_champions_mega_archive import stones
from verify_mega_stone_rewards import world_reward_sources
import mega_register
import acceptance_ladder
import cut_ledger
from verify_campaign_trainer_roster import strip_comments as vcr_strip_comments

README = ROOT / "README.md"
CONTINUE = ROOT / "docs/CONTINUE.md"
GOAL = ROOT / "docs/GOAL.md"
REVIEW_INDEX = ROOT / "docs/trainer-review-index.json"
ITEMS_H = ROOT / "src/data/items.h"
CAPS_C = ROOT / "src/caps.c"
NEW_GAME_C = ROOT / "src/new_game.c"
PAID_EVOLUTION_ITEMS = ROOT / "src/data/emerald_champions_paid_evolution_items.h"

failures: list[str] = []


def report(label: str, ok: bool, detail: str = "") -> None:
    status = "PASS" if ok else "FAIL"
    print(f"{status}: {label}" + (f" -- {detail}" if detail and not ok else ""))
    if not ok:
        failures.append(label + (f": {detail}" if detail else ""))


# ---------------------------------------------------------------------------
# Check 1: roster counts (encounters / groups / party variants)
# ---------------------------------------------------------------------------

_METRIC_RE = re.compile(
    r"(\d+)\s*-?\s*(encounter\s*groups?|authoring\s*groups?|party\s*variants?|"
    r"E\s*groups?|encounters?|groups?|variants?)\b",
    re.I,
)
_ROSTER_KEYWORDS = re.compile(r"roster|census|retained total", re.I)
_WINDOW = 55


def _classify_metric(label: str) -> str:
    label = label.lower()
    if "group" in label:
        return "groups"
    if "variant" in label:
        return "variants"
    return "encounters"


def find_roster_claims(text: str) -> list[dict]:
    """Find 'N encounters'/'N groups'/'N variants' sentences that assert the
    campaign roster census (tolerant of the book's missing-space style, e.g.
    '341encounters' or 'roster is341'). A bare mention is only treated as a
    roster-total claim when it sits near the word roster/census/retained
    total, or near a different-metric number (a same-sentence cluster like
    '343 encounters, 325 encounter groups, 370 party variants')."""
    matches = list(_METRIC_RE.finditer(text))
    claims = []
    for m in matches:
        kind = _classify_metric(m.group(2))
        start, end = m.start(), m.end()
        window = text[max(0, start - _WINDOW):end + _WINDOW]
        has_keyword = bool(_ROSTER_KEYWORDS.search(window))
        nearby_other = any(
            _classify_metric(other.group(2)) != kind and abs(other.start() - start) < _WINDOW * 2
            for other in matches if other is not m
        )
        if has_keyword or nearby_other:
            claims.append(dict(metric=kind, value=int(m.group(1)), context=re.sub(r"\s+", " ", window).strip()))
    return claims


def true_roster_counts() -> tuple[int, int, int, list[str]]:
    """Authoritative (encounters, groups, variants) plus any notes about how
    they were derived (and whether the review index could be trusted)."""
    notes = []
    branches = teams.read_teams()
    variants_true = len(branches)
    groups_true = len({b.encounter for b in branches})
    teams_sha = hashlib.sha256(teams.TEAMS.read_bytes()).hexdigest()
    encounters_true = None
    if REVIEW_INDEX.exists():
        index = json.loads(REVIEW_INDEX.read_text())
        if index.get("source_sha256") == teams_sha:
            encs = index["encounters"]
            index_variants = sum(len(e["trainer_ids"]) for e in encs)
            index_groups = len({g for e in encs for g in e["e_groups"]})
            if index_variants == variants_true and index_groups == groups_true:
                encounters_true = len(encs)
            else:
                notes.append(
                    f"docs/trainer-review-index.json internal counts ({index_variants} variants/"
                    f"{index_groups} groups) disagree with a fresh count from the teams file "
                    f"({variants_true} variants/{groups_true} groups); encounter total not trusted"
                )
        else:
            notes.append(
                "docs/trainer-review-index.json source_sha256 is stale relative to "
                f"{teams.TEAMS.relative_to(ROOT)}; encounter total (with rival gender/starter "
                "aliases and two-owner battles collapsed) cannot be verified from it"
            )
    else:
        notes.append(f"{REVIEW_INDEX.relative_to(ROOT)} is missing; cannot verify the encounter total")
    return encounters_true, groups_true, variants_true, notes


def check_roster_counts(fix_readme: bool) -> None:
    encounters_true, groups_true, variants_true, notes = true_roster_counts()
    for note in notes:
        print("NOTE:", note)
    if encounters_true is None:
        report("roster counts agree everywhere", False, "encounter total could not be verified; see NOTE above")
        return
    print(f"Computed from source: {encounters_true} encounters, {groups_true} groups, {variants_true} party variants "
          "(368 authored variants minus 25 rival gender/starter aliases minus 2 extra two-owner-battle owners = "
          "encounters, per docs/trainer-review-index.json count_rule)")
    truth = dict(encounters=encounters_true, groups=groups_true, variants=variants_true)
    sources = {
        "README.md": README.read_text(),
        "docs/CONTINUE.md": CONTINUE.read_text(),
        "docs/GOAL.md": GOAL.read_text(),
        "Game Blueprint/Emerald_Champions_Game_Book.txt (hand-authored guide)":
            book.split_guide(book.BOOK.read_text()),
    }
    ok = True
    for name, text in sources.items():
        claims = find_roster_claims(text)
        mismatches = [c for c in claims if c["value"] != truth[c["metric"]]]
        matches = [c for c in claims if c["value"] == truth[c["metric"]]]
        if not claims:
            print(f"  {name}: no roster-count sentence found")
            continue
        if mismatches:
            ok = False
            for c in mismatches:
                print(f"  {name}: FAIL {c['metric']}={c['value']} (source says {truth[c['metric']]}) "
                      f"in \"...{c['context']}...\"")
        else:
            print(f"  {name}: {len(matches)} roster-count mention(s) agree with source "
                  f"({', '.join(sorted({c['metric']+'='+str(c['value']) for c in matches}))})")
    report("roster counts agree everywhere (README.md, docs/CONTINUE.md, docs/GOAL.md, hand-authored guide)", ok)
    if fix_readme:
        _fix_readme(encounters_true, groups_true, variants_true)


def _fix_readme(encounters: int, groups: int, variants: int) -> None:
    text = README.read_text()
    pattern = re.compile(
        r"(\| Campaign roster \| \*\*)\d+ encounters, \d+ encounter groups, \d+ party variants(\.\*\*)"
    )
    new_sentence = f"\\g<1>{encounters} encounters, {groups} encounter groups, {variants} party variants\\g<2>"
    new_text, count = pattern.subn(new_sentence, text)
    if count != 1:
        raise SystemExit("--fix-readme: could not find exactly one Campaign roster row to rewrite in README.md")
    README.write_text(new_text)
    print(f"--fix-readme: rewrote README.md Campaign roster row to {encounters} encounters, "
          f"{groups} encounter groups, {variants} party variants")


# ---------------------------------------------------------------------------
# Check 2: prices
# ---------------------------------------------------------------------------

def check_prices() -> None:
    prices = item_prices(ROOT)
    ok = True

    stones_ = {item: r for item, r in prices.items() if r["sort"] == "ITEM_TYPE_EVOLUTION_STONE"}
    bad_stones = {item: r["base_price"] for item, r in stones_.items() if r["base_price"] != 500}
    if bad_stones:
        ok = False
        print(f"  FAIL evolution stones not priced 500: {bad_stones}")
    else:
        print(f"  PASS all {len(stones_)} evolution stones priced 500 in {ITEMS_H.relative_to(ROOT)}")

    cord_price = prices.get("ITEM_LINKING_CORD", {}).get("base_price")
    if cord_price != 3000:
        ok = False
        print(f"  FAIL ITEM_LINKING_CORD price is {cord_price}, expected 3000")
    else:
        print("  PASS ITEM_LINKING_CORD priced 3000")

    balls = {"ITEM_POKE_BALL": 100, "ITEM_GREAT_BALL": 300, "ITEM_ULTRA_BALL": 600}
    bad_balls = {item: prices.get(item, {}).get("base_price") for item, want in balls.items()
                 if prices.get(item, {}).get("base_price") != want}
    if bad_balls:
        ok = False
        print(f"  FAIL Poke/Great/Ultra Ball prices disagree with the guide's 100/300/600: {bad_balls}")
    else:
        print("  PASS Poke/Great/Ultra Ball priced 100/300/600")

    paid_items = re.findall(r"ITEM_\w+", PAID_EVOLUTION_ITEMS.read_text())
    tools = [i for i in paid_items if i not in stones_ and i != "ITEM_LINKING_CORD"]
    bad_tools = {i: prices.get(i, {}).get("base_price") for i in tools
                 if prices.get(i, {}).get("base_price") not in (1000, 3000)}
    if bad_tools:
        ok = False
        print(f"  FAIL ordinary tools/reusable devices priced outside the guide's 1000/3000 rule: {bad_tools}")
    else:
        print(f"  PASS all {len(tools)} other paid evolution items priced 1000 (ordinary tool) or 3000 "
              "(reusable device)")

    # Native pokemart stock tables for Rustboro, Slateport, and the Lilycove
    # department store, reusing sync_game_book's shop table reader.
    _, _, _, shops, _ = book.source_inventory()
    mart_paths = ("data/maps/RustboroCity_Mart/scripts.inc", "data/maps/SlateportCity_Mart/scripts.inc")
    lilycove_prefix = "data/maps/LilycoveCity_DepartmentStore"
    cord_prices_seen = {}
    for shop in shops:
        if not (shop["source"] in mart_paths or shop["source"].startswith(lilycove_prefix)):
            continue
        for item in shop["items"]:
            price = prices.get(item, {}).get("base_price")
            if item in stones_ and price != 500:
                ok = False
                print(f"  FAIL {shop['source']} ({shop['label']}): stone {item} priced {price}, not 500")
            elif item in tools and price != 1000:
                ok = False
                print(f"  FAIL {shop['source']} ({shop['label']}): tool {item} priced {price}, not 1000")
            elif item == "ITEM_LINKING_CORD":
                cord_prices_seen[shop["source"]] = price
                if price != 3000:
                    ok = False
                    print(f"  FAIL {shop['source']} ({shop['label']}): Linking Cord priced {price}, not 3000")
    if "data/maps/SlateportCity_Mart/scripts.inc" in cord_prices_seen:
        print(f"  Slateport Linking Cord price: {cord_prices_seen['data/maps/SlateportCity_Mart/scripts.inc']}")
    report("economy section prices match src/data/items.h and pokemart stock", ok)


# ---------------------------------------------------------------------------
# Check 3: caps and stipends
# ---------------------------------------------------------------------------

def check_caps_stipends() -> None:
    caps_source = CAPS_C.read_text()
    rows = re.findall(r"\{FLAG_\w+,\s*(\d+),\s*(\d+)\}", caps_source)
    if not rows:
        report("caps and stipends match src/caps.c", False, "could not parse sCampaignMilestones rows")
        return
    caps = [int(c) for c, s in rows]
    stipends = [int(s) for c, s in rows]
    nonzero_caps = [c for c in caps if c != 0]
    opening_cap = 14  # GetCurrentLevelCap's default before any milestone flag
    cap_sequence = [opening_cap] + nonzero_caps
    stipend_total_from_milestones = sum(stipends)

    money_match = re.search(r"SetMoney\(&gSaveBlock1Ptr->money,\s*(\d+)\)", NEW_GAME_C.read_text())
    if not money_match:
        report("caps and stipends match src/caps.c", False, "could not find starting SetMoney(...) in src/new_game.c")
        return
    starting_money = int(money_match[1])
    stipend_total = stipend_total_from_milestones + starting_money

    print(f"Computed from source: opening cap {opening_cap}, cap sequence {cap_sequence}, "
          f"starting cash {starting_money}, stipend total (incl. starting cash) {stipend_total}")

    ok = True
    guide = book.split_guide(book.BOOK.read_text())
    caps_sentence = re.search(r"Caps:\s*([^\n]*)", guide)
    if not caps_sentence:
        ok = False
        print("  FAIL could not find a 'Caps: ...' sentence in the hand-authored guide")
    else:
        # The line ends with "...Champion100. The entire League is fought at96." --
        # take only as many leading numbers as there are milestones, so a
        # trailing unrelated number (that fought-at level cap) is not compared.
        all_numbers = [int(x) for x in re.findall(r"\d+", caps_sentence[1])]
        stated = all_numbers[:len(cap_sequence)]
        if stated != cap_sequence:
            ok = False
            print(f"  FAIL guide Caps sentence lists {stated}, source says {cap_sequence}")
        else:
            print(f"  PASS guide Caps sentence matches source cap sequence ({len(cap_sequence)} caps)")

    stipend_match = re.search(r"total\s*(\d+)\s*including starting cash and Champion", guide)
    if not stipend_match:
        ok = False
        print("  FAIL could not find the 'total<N> including starting cash and Champion' sentence in the guide")
    elif int(stipend_match[1]) != stipend_total:
        ok = False
        print(f"  FAIL guide states stipend total {stipend_match[1]}, source computes {stipend_total}")
    else:
        print(f"  PASS guide stipend total matches source ({stipend_total})")

    start_match = re.search(r"Starting money is\s*(\d+)", guide)
    if not start_match:
        ok = False
        print("  FAIL could not find 'Starting money is<N>' sentence in the guide")
    elif int(start_match[1]) != starting_money:
        ok = False
        print(f"  FAIL guide states starting money {start_match[1]}, source has SetMoney(...,{starting_money})")
    else:
        print(f"  PASS guide starting-money statement matches source ({starting_money})")

    report("caps and stipends match src/caps.c and starting money", ok)


# ---------------------------------------------------------------------------
# Check 4: retired items
# ---------------------------------------------------------------------------

RETIRED_ITEMS = {
    "ITEM_BOTTLE_CAP", "ITEM_GOLD_BOTTLE_CAP",
    "ITEM_PP_UP", "ITEM_PP_MAX",
    "ITEM_X_ATTACK", "ITEM_X_DEFENSE", "ITEM_X_SP_ATK", "ITEM_X_SP_DEF",
    "ITEM_X_SPEED", "ITEM_X_ACCURACY", "ITEM_X_SPECIAL",
    "ITEM_GUARD_SPEC", "ITEM_DIRE_HIT",
    # C3: gambling is gone, so nothing may hand out a Coin Case again.
    "ITEM_COIN_CASE",
    # C10: one Bicycle. ITEM_ACRO_BIKE is it; the Mach Bike has no producer.
    "ITEM_MACH_BIKE",
}


def _strip_comments(text: str) -> str:
    return re.sub(r"//[^\n]*|/\*.*?\*/", "", text, flags=re.S)


def check_retired_items() -> None:
    ok = True
    hits: list[str] = []

    # Pokemart stock (every mart, native shop table reused from sync_game_book).
    _, _, gifts, shops, _ = book.source_inventory()
    for shop in shops:
        if "frlg" in shop["source"].lower():
            continue
        bad = RETIRED_ITEMS & set(shop["items"])
        for item in sorted(bad):
            hits.append(f"pokemart stock {shop['source']} ({shop['label']}): {item}")

    # giveitem/additem/etc. deliveries across every map's scripts.inc.
    for item, source, label in ((g["item"], g["source"], g["label"]) for g in gifts):
        if item in RETIRED_ITEMS and "frlg" not in source.lower():
            hits.append(f"giveitem {source} ({label}): {item}")

    # Prize tables: Battle Frontier, Champions Circuit, Game Corner scripts.
    prize_paths = list((ROOT / "src").glob("battle_frontier*.c")) + [ROOT / "src/champions_circuit.c"] + \
        list((ROOT / "src/data/battle_frontier").glob("*.h")) + \
        list((ROOT / "data/maps").glob("*GameCorner*/scripts.inc"))
    for path in prize_paths:
        if not path.exists() or "frlg" in str(path).lower():
            continue
        found = RETIRED_ITEMS & set(re.findall(r"\bITEM_\w+", _strip_comments(path.read_text())))
        for item in sorted(found):
            hits.append(f"prize table {path.relative_to(ROOT)}: {item}")

    # Pickup tables.
    pickup_candidates = [ROOT / "src/battle_util.c", ROOT / "src/battle_script_commands.c"]
    pickup_candidates += list((ROOT / "src").glob("**/pickup_items.h"))
    for path in pickup_candidates:
        if not path.exists():
            continue
        text = _strip_comments(path.read_text())
        table = re.search(r"sPickupTable\[\]\s*=\s*\{(.*?)\n\};", text, re.S)
        if not table:
            continue
        found = RETIRED_ITEMS & set(re.findall(r"\bITEM_\w+", table[1]))
        for item in sorted(found):
            hits.append(f"pickup table {path.relative_to(ROOT)}: {item}")

    if hits:
        ok = False
        for hit in hits:
            print(f"  FAIL retired item found: {hit}")
    else:
        print(f"  PASS none of the {len(RETIRED_ITEMS)} retired items appear in any pokemart, giveitem, "
              "prize table or pickup table")
    report("retired items (Bottle Cap, Gold Bottle Cap, PP Up/Max, X-items, Guard Spec, Dire Hit) stay retired", ok)


# ---------------------------------------------------------------------------
# Check 5: Mega Stones
# ---------------------------------------------------------------------------

def check_mega_stones() -> None:
    all_stones = stones()
    rewards = world_reward_sources()
    branches = teams.read_teams()
    held = {"ITEM_" + m.item for b in branches for m in b.mons}

    no_source = [s for s in all_stones if not rewards.get(s)]
    multi_source = {s: rewards[s] for s in all_stones if len(rewards.get(s, [])) > 1}
    no_holder = [s for s in all_stones if s not in held]

    ok = not no_source and not multi_source and not no_holder
    if no_source:
        print(f"  FAIL Mega Stones with no world acquisition source: {sorted(no_source)}")
    if multi_source:
        print(f"  FAIL Mega Stones with more than one world acquisition source: {multi_source}")
    if no_holder:
        print(f"  FAIL Mega Stones with no trainer holder in {teams.TEAMS.relative_to(ROOT)}: {sorted(no_holder)}")
    if ok:
        print(f"  PASS all {len(all_stones)} item-based Mega Stones have exactly one world source "
              "and at least one trainer holder")
    report("every Mega Stone has one world source and at least one trainer holder", ok)


# ---------------------------------------------------------------------------
# Check 6: duplicate paragraphs in the hand-authored guide
# ---------------------------------------------------------------------------

def check_duplicate_paragraphs() -> None:
    guide = book.split_guide(book.BOOK.read_text())
    paragraphs = re.split(r"\n\s*\n", guide)
    counts = Counter(p.strip() for p in paragraphs if len(p.strip()) > 200)
    dupes = {p: n for p, n in counts.items() if n > 1}
    ok = not dupes
    for p, n in dupes.items():
        preview = re.sub(r"\s+", " ", p)[:100]
        print(f"  FAIL paragraph repeated {n}x verbatim ({len(p)} chars): \"{preview}...\"")
    if ok:
        print("  PASS no >200-char paragraph appears twice verbatim in the hand-authored guide")
    report("no duplicate paragraphs in the hand-authored guide", ok)


def check_mega_register_and_ladder() -> None:
    """Check 7: the generated MEGA REGISTER must show zero NO TRAINER HOLDER
    rows, and the generated ACCEPTANCE LADDER summary must equal a fresh
    recompute from docs/trainer-review-index.json's stored ladder fields
    (which is only true after scripts/acceptance_ladder.py --write has been
    run against the current teams/tests/work state)."""
    register_rows = mega_register.build_register()
    no_holder = [r["stone"] for r in register_rows if r["no_trainer_holder"]]
    ok_register = not no_holder
    if no_holder:
        print(f"  FAIL Mega Stones with NO TRAINER HOLDER in the generated register: {sorted(no_holder)}")
    else:
        print(f"  PASS all {len(register_rows)} Mega Register rows have at least one trainer holder")
    report("MEGA REGISTER has zero NO TRAINER HOLDER rows", ok_register)

    fresh_ladder, fresh_catalog = acceptance_ladder.compute_ladder()
    fresh_summary = fresh_catalog["summary"]
    if not REVIEW_INDEX.exists():
        print(f"  FAIL {REVIEW_INDEX.relative_to(ROOT)} is missing")
        report("ACCEPTANCE LADDER counts match docs/trainer-review-index.json", False)
        return
    index = json.loads(REVIEW_INDEX.read_text())
    stored_summary = dict(
        total=len(index["encounters"]),
        L0_authored=sum(e.get("ladder", {}).get("L0_authored", False) for e in index["encounters"]),
        L1_ai_fixtures=sum(e.get("ladder", {}).get("L1_ai_fixtures", False) for e in index["encounters"]),
        L2_benchmark=sum(e.get("ladder", {}).get("L2_benchmark", False) for e in index["encounters"]),
        L3_earned_clear=sum(e.get("ladder", {}).get("L3_earned_clear", False) for e in index["encounters"]),
    )
    ok_ladder = stored_summary == fresh_summary
    if ok_ladder:
        print(f"  PASS ACCEPTANCE LADDER counts agree: {fresh_summary}")
    else:
        print(f"  FAIL ACCEPTANCE LADDER counts disagree: book/index={stored_summary} fresh={fresh_summary}; "
              "run python3 scripts/acceptance_ladder.py --write")
    report("ACCEPTANCE LADDER counts match docs/trainer-review-index.json", ok_ladder)


def check_cut_ledger() -> None:
    """Check 8: every identity in data/emerald_champions/retired_battles.json
    (the CUT LEDGER's source of truth) must be absent from active authored
    teams, nonempty native parties and Hoenn battle opcodes -- the same three
    sources scripts/verify_campaign_trainer_roster.py checks -- and the
    retired-identity count must match CUT POLICY's "<N> identities are
    retired" sentence in the hand-authored guide."""
    data = cut_ledger.load()
    retired_ids: set[str] = set()
    for row in data["retired_battles"]:
        retired_ids.update(row["identities"])
    for row in data["additional_retirements"]:
        retired_ids.update(row["identities"])

    branches = teams.read_teams()
    active = {b.trainer for b in branches}

    source = vcr_strip_comments((ROOT / "src/data/trainers.party").read_text())
    parties = {block.split(" ===", 1)[0] for block in re.split(r"^=== ", source, flags=re.M)[1:]
               if re.search(r"^SPECIES_", block, re.M)}

    calls: set[str] = set()
    for path in [*(ROOT / "data/maps").rglob("scripts.inc"), *(ROOT / "data/scripts").rglob("*.inc")]:
        if "frlg" in str(path.relative_to(ROOT)).lower():
            continue
        for line in vcr_strip_comments(path.read_text()).splitlines():
            if re.match(r"\s*(trainerbattle\w*|multi_\w*)\s", line):
                calls.update(re.findall(r"\bTRAINER_\w+\b", line))

    still_present: dict[str, list[str]] = {}
    for label, actual in [("active authored teams", active), ("nonempty native parties", parties),
                           ("Hoenn battle opcodes", calls)]:
        hit = retired_ids & actual
        if hit:
            still_present[label] = sorted(hit)

    ok_absence = not still_present
    if ok_absence:
        print(f"  PASS all {len(retired_ids)} retired identities are absent from active authored teams, "
              "nonempty native parties and Hoenn battle opcodes")
    else:
        for label, hit in still_present.items():
            print(f"  FAIL retired identities still present in {label}: {hit}")
    report("every retired identity is absent from active teams/native parties/battle opcodes", ok_absence)

    guide = book.split_guide(book.BOOK.read_text())
    count_match = re.search(r"(\d+)\s*identities are retired", guide)
    if not count_match:
        print("  FAIL could not find '<N> identities are retired' sentence in the guide")
        report("CUT POLICY retired-identity count matches data/emerald_champions/retired_battles.json", False)
        return
    guide_count = int(count_match[1])
    ok_count = guide_count == len(retired_ids)
    if ok_count:
        print(f"  PASS guide's retired-identity count ({guide_count}) matches the data file ({len(retired_ids)})")
    else:
        print(f"  FAIL guide says {guide_count} identities are retired; "
              f"data/emerald_champions/retired_battles.json has {len(retired_ids)}")
    report("CUT POLICY retired-identity count matches data/emerald_champions/retired_battles.json", ok_count)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--fix-readme", action="store_true",
                         help="rewrite README.md's Campaign roster row from the computed source numbers")
    args = parser.parse_args()

    print("== 1. Roster counts ==")
    check_roster_counts(args.fix_readme)
    print("\n== 2. Prices ==")
    check_prices()
    print("\n== 3. Caps and stipends ==")
    check_caps_stipends()
    print("\n== 4. Retired items ==")
    check_retired_items()
    print("\n== 5. Mega Stones ==")
    check_mega_stones()
    print("\n== 6. Duplicate paragraphs ==")
    check_duplicate_paragraphs()
    print("\n== 7. Mega Register and Acceptance Ladder ==")
    check_mega_register_and_ladder()
    print("\n== 8. Cut Ledger ==")
    check_cut_ledger()

    print()
    if failures:
        print(f"BOOK CONSISTENCY CHECK: FAIL ({len(failures)} check(s) failed)")
        for f in failures:
            print(" -", f)
        return 1
    print("BOOK CONSISTENCY CHECK: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
