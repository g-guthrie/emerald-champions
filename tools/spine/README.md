# Spine: rebuilding Emerald Champions for LLM readability

Status: **exploratory, nothing wired into the build.** These tools measure and
encode; they do not yet generate anything the ROM depends on. The current ROM,
branch and build are untouched.

## Why

Emerald Champions works, but its design is spread across 3,870 files and about
**11.6M tokens**. Changing one thing — what an NPC gives, when a legendary
appears, what a shop stocks — requires reappraising an economy, a dialogue set
and a progression gate that live nowhere near each other. No person and no
model can hold that at once, so every change risks incoherence somewhere else.

The goal is to invert: a compact design corpus becomes the source of truth, and
the engine tree (map scripts, tables, flag constants) becomes build output. The
precedent already exists in this repo: `data/emerald_champions/*.json`
generates `src/data/pokemon/emerald_champions_battle_sets.h`, banner and all.

The Cohesion Book was the first attempt at this and failed for a specific,
instructive reason: it was **prose describing the game, stored beside the
game**, so the game moved and the description did not. A generated corpus
cannot drift, because drift becomes a build failure.

## The contract

Every tool here is held to the same standard:

> decode(encode(x)) must equal x, byte for byte, across the entire corpus.

An encoding that cannot reproduce the input exactly is not a complete model of
it, and a spine built on an incomplete model loses content silently. This is
the same proof that made the 7.1 deletion safe, pointed at format design.

It has already paid for itself three times. The round-trip caught:

* interior blank lines the first parser collapsed
* 2,296 files containing `@` comments the op parser rejected
* assembler directives (`.equ`, `.macro`, `#ifdef`) at column zero
* collapsed argument whitespace in `setmetatile  5, 16`
* in the battle sets: `field_dependency` can be `null`, is **absent** in 2,718
  presets, and five presets carry a `required_move` key a fixed column list
  drops on the floor

Each of those would have produced a ROM that did not match, with no obvious
cause.

## What is here

| file | what it does | verified |
|---|---|---|
| `roundtrip.py` | parses a `.inc` map script into a typed IR and emits it back | **3,796 / 3,796 byte-identical** |
| `spine.py` | compresses that IR: map-prefix stripping, label sigils, movement run-collapsing, a constant-prefix dictionary | **1,034 / 1,035 tracked files byte-identical**, 19.4% smaller |
| `battlesets.py` | converts battle-set presets to a compact line format | **lossless PASS**, 2.4x smaller |

Run them:

    python3 tools/spine/roundtrip.py data/maps
    python3 tools/spine/spine.py data/maps
    python3 tools/spine/battlesets.py data/emerald_champions/emerald_champions_battle_sets.json

## Measured, not projected

Three separate compression estimates were made before measuring, and all three
were too optimistic. They are recorded here so the pattern is visible:

| | projected | measured |
|---|---|---|
| map script encoding | 3-4x | **1.2x** |
| battle-set conversion | 34x | **2.4x** |
| resulting spine size | ~300K tokens | **~3.3M tokens** |

The spine as currently scoped is roughly **3.3M tokens**: scripts 1.17M,
battle sets ~950K, `map.json` 820K, constants 344K, C logic 52K. That is not a
context window. **Compression is not the lever; deletion and scope are.**

### Where the script corpus actually goes

| | tokens | share |
|---|---|---|
| Hoenn campaign | 936,322 | 64.5% |
| FRLG / Kanto import | 307,896 | 21.2% |
| Battle Frontier | 190,457 | 13.1% |
| Contest halls | 17,564 | 1.2% |

A third of the script corpus is Kanto maps and Frontier facilities. The
Frontier stays — it is the intended post-game. The **308K tokens of FRLG/Kanto
content** is imported expansion content with no known route from the campaign.
Confirming that is the next task and the largest single remaining win.

### Composition of the map scripts

Of 126,947 lines: 46% carries a design decision, 22% is label text that exists
only so one line can jump to another, 24% is dialogue (89% unique, so real
authored content), and the rest is boilerplate.

Dialogue splits into 6,610 plot-bearing lines (65K tokens, in blocks that also
set a flag, give an item, start a battle or warp) and 19,047 flavour lines
(180K). **Both belong in the spine.** A sign's text is design: if an NPC says
"the Devon researcher has a stone for you" and that gift moves, the game is
incoherent even though no flag changed.

### Things that turned out not to be source

`data/maps/*/events.inc` is **generated from `map.json` by `tools/mapjson` and
gitignored**. 962 of the 3,796 `.inc` files are build output. The bulky
`object_event` / `warp_def` / `map_header_flags` tables live there, so they are
not a compression target at all.

## The role of codebase-memory (MCP)

[codebase-memory-mcp](https://github.com/DeusData/codebase-memory-mcp) is
installed at `~/.local/bin/codebase-memory-mcp` (v0.10.3, release tarball,
SHA-256 verified against the Homebrew formula). It is driven here through its
`cli` subcommand rather than over stdio, which runs the same 15 tools.

    codebase-memory-mcp cli index_repository --repo-path . --mode full --name ec
    codebase-memory-mcp cli query_graph --args-file q.json

It is used as a **second opinion, never as the only one**, because the two
methods disagree in both directions and each has caught the other's errors:

* the graph **found a seam the regex missed** — `src/script.c:438` calls
  `ResetLegendaryEncounterVisits` in `legendary_signs.c`; the hand-written
  pattern was looking for `LegendarySign` and the function is
  `LegendaryEncounter`
* the graph **missed seams the regex found** — `src/shop.c:678` and
  `src/item.c:1132` both call `GetEmeraldChampionsEvolutionPrice`, verified by
  reading the source
* the graph **reported calls that do not exist** — `src/field_weather.c` and
  `src/script.c` were listed as callers of EC code; `field_weather.c` contains
  zero EC references

**It is blind to the script layer.** All 909 `scripts.inc` files are indexed
only as `File` and `Module` nodes — for `MauvilleCity_Gym/scripts.inc` the
graph knows the path and the filename and nothing else. No labels, no opcodes,
no dialogue. It parses C, Python and C++; it does not parse the pokeemerald
script DSL.

That sets the division of labour: **codebase-memory is authoritative for C, the
round-trip parser is authoritative for scripts**, and neither is complete
alone. The disagreement set is the audit queue.

It is also an argument for the inversion: once the spine exists as structured
data, the graph gains visibility into story and economy that it fundamentally
lacks today.

Related: `docs/SEAM_MANIFEST.md` and `scripts/generate_seam_manifest.py` map
every point where Emerald Champions logic touches inherited engine code — 452
sites, 119 irreducible hooks, 49 relocatable functions.

## Where this stands

Done:

* a proven-complete structural model of every map script in the game
* a lossless compressed encoding of it, 19.4% smaller
* a lossless battle-set encoding, 2.4x smaller
* a measured census of where the design tokens actually live

Not done, in order of value:

1. **FRLG/Kanto reachability** — is any Kanto map reachable from the campaign?
   If not, 308K tokens and ~200 files delete outright.
2. `map.json` (820K tokens) has had no encoding pass; it is the real source for
   map events and duplicates nothing.
3. The second battle-set file uses a different schema and is not yet converted.
4. One tracked `.inc` still fails the spine round-trip.
5. No generator exists yet. Nothing here writes the engine tree, so the
   inversion has not begun — this is the measurement and format work that has
   to be correct before it can.

The end-state test, when a generator does exist: regenerate the engine tree
from the corpus, build, and require the ROM to hash to `15b3f887...`, identical
to what ships today.
