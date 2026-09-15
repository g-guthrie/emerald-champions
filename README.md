# Emerald Champions — native v4 development

**A competitive doubles campaign for the Game Boy Advance. Development is ongoing.**
The canonical Game Book is v4; the older 7.0/7.1 labels refer to its ROM lineage.
Start with [the short reading guide](Game%20Blueprint/Game_Guide_Reading_Copy.txt)
or [the current checkpoint](docs/CONTINUE.md).

## Current progress — September 15, 2026

| Track | Current state |
| --- | --- |
| Goal | Consolidation goal active (`docs/GOAL.md`): one baseline, the new campaign spine, narrow economy, then the battle acceptance ladder. |
| Campaign roster | **341 encounters, 324 encounter groups, 368 party variants.** All 65 Gym trainer/leader parties have six Pokémon. The book, active parties and Hoenn battle IDs pass the exact roster check. |
| Spine and caps | Four acts, 15 cap milestones, continuous Groudon-to-Kyogre crisis; Birth Island, Scanner and Regis optional. Sandstrewn's collapse seals no unique content. |
| Megas | All 99 stones held; generated Mega register with a signature owner for each. Seven mandatory legendary scenes carry authored sets. |
| Economy | Money buys evolution items, Balls and cosmetics; first-clear prize money only; Poipole and Genesect on Circuit BP. |
| Acceptance ladder | L0 authored 341 · L1 AI fixtures 50 · L2 benchmarked 2 · L3 earned clears 40 (generated in the book). |
| Verification | Normal, headless and test ROMs build; 76/76 campaign tests; seven book-consistency checks and every static gate pass. |

**Current work:** dialogue unification against the new spine, then resume the earned run and move encounters up the ladder major-battle-first (League, whole Gyms, Magma/Aqua groups, then the rest).

**Emerald Studio** is a local browser workspace
backed by native mGBA. Start it with:

~~~sh
python3 tools/studio/cli.py start
~~~

Open the printed localhost URL in the Codex browser. Studio supports live
play/audio, NPC source editing and rebuild/return, map navigation, party
preparation, native trainer tests, and build-bound bookmarks. Its Library adds
headless scene replay, automatic contact sheets, before/after comparisons, NPC
dialogue search, sprite atlases and source history. Install the Codex skill with
`python3 tools/studio/install_skill.py`.
See [verification](docs/VERIFICATION.md) for setup and supported chapter fixtures.

The main creative check after every team build or review is
**“Could this team be cooler?”** Design memorable,
regionally appropriate partnerships first, verify the AI, then tune individual
levels against strong teams the player can actually obtain. Reserve Coalossal
for the volcano and consider Regirock for a later important desert encounter.

## Project guide

| Read | Purpose |
| --- | --- |
| [Continue here](docs/CONTINUE.md) | Exact paused save, next actions and evidence limits |
| [AGENTS.md](AGENTS.md) | Working rules; no additional subagents |
| [Full goal](docs/GOAL.md) | Campaign objective and completion criteria |
| [Canonical Game Book](Game%20Blueprint/Emerald_Champions_Game_Book.txt) | Single design specification, exact teams and acceptance records |
| [Build, play and verify](docs/VERIFICATION.md) | Build/import commands, native tools and focused regressions |
| [GitHub releases](https://github.com/g-guthrie/emerald-champions/releases) | Immutable ROM, save and evidence downloads; check their dates and manifests |

A new coding session should read `AGENTS.md`, `docs/CONTINUE.md` and the relevant
book sections before continuing the user’s current review. Engine code lives in
`src/`, `include/` and `data/`; exact trainer records live in
`data/emerald_champions/emerald_champions_battle_teams.txt` and are materialized by
`scripts/emerald_champions_teams.py`. `scripts/sync_game_book.py --write` updates
the book’s generated source reference; `--check` rejects drift. Build a normal ROM with `make -j4 release`
and follow the stamp/release checks in the verification guide.

Preserve ROM/ELF/save ancestry. Continue a changed ROM through a normal battery
save and clean boot, never a savestate from another ROM. Synthetic victories
verify scenes only. Source agreement, native fixtures and earlier earned wins
do not certify newly revised teams or the whole game.

[Credits](CREDITS.md) · [Third-party notices](THIRD_PARTY_NOTICES.md)

The book updates after a successful normal/release build. For live source edits:
`python3 scripts/sync_game_book.py --watch`. CI rejects stale generated references.
