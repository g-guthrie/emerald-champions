# Emerald Champions — continue the v4 build

**This is the active native GBA project. Start with [Continue here](docs/CONTINUE.md).**
The Game Book is version **v4**; it builds on the older **7.0/7.1 ROM line**.
Those are different version labels, not competing current projects.

The unfinished goal is to build, play and beat a complete, cohesive campaign for
hardcore competitive doubles players, repairing the game and updating its book
as the earned playthrough advances. The current Medium save has **33 unique
trainer victories**, is in **C13**, and has not beaten Cristian, Jocelyn or Brawly.

| Read | Purpose |
| --- | --- |
| [Continue here](docs/CONTINUE.md) | Exact checkpoint, next actions, progress and evidence limits |
| [AGENTS.md](AGENTS.md) | Current working rules for every coding agent |
| [Full goal](docs/GOAL.md) | Objective to start/resume and actual completion criteria |
| [Canonical Game Book](Game%20Blueprint/Emerald_Champions_Game_Book.txt) | Complete game specification; update with implementation |
| [Build, play and verify](docs/VERIFICATION.md) | Portable commands, native controls, contact sheets and AI testing |
| [Handoff downloads](https://github.com/g-guthrie/emerald-champions/releases/tag/v4-handoff-20260913) | Exact save/ROM/ELF, full evidence, current development ROM and archived 7.0/7.1 |

A new Codex run can be told: **“Read this repository, start the goal in
`docs/GOAL.md`, and continue from `docs/CONTINUE.md`.”** Do the work rather than
starting another planning or review loop. The one authorized independent full-book
review is already complete. No additional subagents are authorized.

Use the supplied earned save to continue development; start a fresh save for a
separate new-game test. Never import an unrelated old-edition save. The source,
book, tools and manifest are in Git; large immutable ROM/evidence downloads are
release assets, with hashes in [handoff/assets.json](handoff/assets.json).

Engine: `src/`, `include/`, `data/`. Trainer authoring is generated from the book
through `scripts/emerald_champions_teams.py`; wild tables are
`src/data/wild_encounters.json`. Current scenes are in `data/maps/` and
`data/scripts/`. Older branches/releases are reference archives, not active
instructions. This development checkpoint is not a claim that the full game has
been played, balanced or visually accepted.

[Credits](CREDITS.md) · [Third-party notices](THIRD_PARTY_NOTICES.md)
