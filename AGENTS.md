# Working on Emerald Champions

Emerald Champions is a hard, doubles-focused Emerald romhack built on
pokeemerald-expansion. Read this before you touch anything.

## The one rule

**`docs/` is generated output. It is not ground truth. Do not summarize it,
and never cite it as evidence that something works.**

Every prose document in this repo was written by a script or an agent
describing intent. None of it was produced by playing the game. If you
report back a summary of `docs/`, you have not looked at this project.

The narrative documentation that used to live here — a campaign book, a story
audit, and a parity ledger reading `COMPLETE` on 38 rows — was deleted
precisely because agents kept reading it instead of the game. Do not
reconstruct it. If you want to know whether something works, query the data.

Specific traps that remain:

- `scripts/verify_emerald_champions_release.py` prints 40+ `PASS:` lines and
  exits 0. A green gate means the data is **self-consistent**, not that the
  game is good, balanced, or fun.
- Every one of the 513 encounters in the battle master is marked
  `difficulty_observed: UNPLAYED`. Nobody has played this game. Any claim
  about difficulty or pacing that is not derived from the data is a guess.

## Ground truth

| What you want to know | Read this |
|---|---|
| Trainer teams, items, moves, natures, AI | `src/data/trainers.party` (Showdown format) |
| Wild encounters | `src/data/wild_encounters.json` (409 maps) |
| Base stats, types, abilities, evolutions | `src/data/pokemon/species_info/gen_*_families.h` |
| Level/EV caps, battle rules | `include/config/caps.h`, `include/config/battle.h` |
| Authored competitive sets | `docs/emerald_champions_battle_sets.json` (1,461 sets) |
| Encounter design rationale | `docs/emerald_champions_master_battle_design.txt` |
| Story scripts and dialogue | `data/maps/*/scripts.inc`, `data/scripts/emerald_champions.inc` |

The last two live in `docs/` but are **load-bearing inputs**, not prose — six
scripts parse the battle master, and five parse the battle sets. Do not delete
them.

## Recipes

Read one trainer's full team:

```bash
awk '/^=== TRAINER_WALLACE ===$/{f=1} f&&/^=== /&&!/^=== TRAINER_WALLACE ===$/{exit} f' src/data/trainers.party
```

Check a species' legal abilities:

```bash
awk '/\[SPECIES_WAILORD\] =/,/^    },/' src/data/pokemon/species_info/*.h | grep '\.abilities'
```

Dump a route's encounter table:

```bash
python3 -c "import json;d=json.load(open('src/data/wild_encounters.json'));[print(e) for e in d['wild_encounter_groups'][0]['encounters'] if e.get('map')=='MAP_ROUTE103']"
```

Run every gate (~13s total):

```bash
for f in scripts/verify_*.py scripts/audit_*.py; do echo "== $f"; python3 "$f" >/dev/null 2>&1 && echo PASS || echo FAIL; done
```

The ROM test suite (22 tests) runs **only in GitHub CI on Linux** —
`tools/mgba/mgba-rom-test` is an x86-64 binary and `mgba-rom-test-mac` does not
exist. You cannot run it on macOS.

## Known open defects

These are confirmed against the data. Do not re-report them as new, and do not
report "all gates pass" as though these do not exist — no gate covers any of
them.

- **86 trainer Pokémon have abilities their species cannot legally have.**
  `trainer_util.c:80` fails the lookup and, because
  `B_TRAINER_MON_RANDOM_ABILITY = 0`, silently falls back to ability slot 0.
  Authored in the battle master by flavour (Wailord→Drizzle, Palossand→Sand
  Stream, Trevenant→Grassy Surge). 18 are weather or terrain setters whose
  teams are built on weather that never gets set.
- **12 teams run Trick Room and Tailwind together**, which cancel. One is
  Drake of the Elite Four.
- **42 Pokémon with base Speed ≤50 carry +Speed natures**, including Wallace's
  Ferrothorn (base Speed 20, Jolly). 92 fast Pokémon carry +Speed natures on
  Trick Room teams.
- **Wild generation curve runs backwards** (r = −0.299 against campaign
  order). Early routes average generation 4.47; the last eight average 3.69.
- **Starters appear in early wild tables** — Fuecoco on Route 103, the first
  route; Sobble in Route 104's Old Rod slots; Primarina (fully evolved) on
  Routes 126/128.
- **Ordinary difficulty is flat** (7.32 → 7.19 across quarters) and misses the
  project's own target: 54.2% of fights sit at 6.x–7.x against a 35% target
  for 7.x, only 13.9% at 8.x against 25%, and **zero** at 9.x.
- **All 2,147 campaign Pokémon have exactly 66 EVs** across only 9 spreads,
  two of which cover 71% of the game. At the mean trainer level that is ~4
  stat points.
- **Six vitamins are sold in 9 mart lists and do nothing** —
  `B_EV_CAP_TYPE = EV_CAP_NO_GAIN` makes the EV cap zero.
- **`MAP_ROUTE109.rock_smash_mons` has `encounter_rate: 255`**, which clamps to
  a guaranteed encounter. It is the only value above 100 in 761 tables.

## If you are asked to evaluate the game

Open and read individual trainer entries and encounter tables. Aggregate
statistics are a starting point for finding where to look, not an answer —
they hide exactly the defects above. Roxanne and Brawly are adjacent gyms of
very different quality; no metric shows that. Read them.

State plainly when something cannot be verified without play.

## Build

```bash
make -j$(sysctl -n hw.ncpu) release   # produces pokeemerald-release.gba
```

The tracked ROM is `pokeemerald-release.gba`, SHA-1
`61301ca6e1fa0d08e528ed7be19dd0d1ca2e0b4b`. The release gate refuses to pass
if any source input is newer than the ROM — rebuild before verifying.
