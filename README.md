# Emerald Champions — native v4 development

**A competitive doubles campaign for the Game Boy Advance. Development is ongoing.**
The canonical Game Book is v4; the older 7.0/7.1 labels refer to its ROM lineage.
Start with [the current checkpoint](docs/CONTINUE.md).

## Current progress — September 14, 2026

| Track | Current state |
| --- | --- |
| Collaborative team review | Reviewed through battle **28**, including the entire Rustboro Gym. Approved revisions are synchronized in the book and code; Joey (16) remains on hold. |
| Rustboro Gym | Four six-Pokémon teams: Josh’s minerals, Tommy’s fossil reef, Marc’s Power Spot team, and Roxanne’s **Mega Aerodactyl / Tailwind** offense. Roxanne has no Trick Room. |
| Opening economy | Gifts/pickups through battle 16 reconciled; pending-reward fixes and scoped native checks completed. Roxanne gives Old Amber; Steven later gives the bracelet and Aerodactylite together. |
| Campaign roster | **343 encounters, 325 encounter groups, 370 party variants.** All 65 Gym trainer/leader parties have six Pokémon. The book, active parties and Hoenn battle IDs pass the exact roster check. |
| Earned playthrough | **C15, Medium, cap 24.** 41 distinct trainer wins are evidenced; the paused Dwayne retest save contains 40 clears. Brawly and the manor song are complete. |
| Verification | Production ROM builds and passes release gates. Focused Mega reveal/eligibility tests and native reward-dialogue checks pass. Revised team levels remain provisional; full-game balance and acceptance are unfinished. |

**What we are doing now:** implementing the book’s overworld scenes with
**Emerald Studio**, a local browser workspace
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
The collaborative team review, earned playthrough and trainer-export delivery
are paused at the user’s request.
Current scene work implements the book’s Scanner/ruins sequence and research
flights using Studio recordings and contact sheets.
The next review starts after Rustboro Gym; do not silently resume the Dwayne
experiment or replace its preserved save with a synthetic fixture.

Every team review asks **“Could this team be cooler?”** Design memorable,
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
`src/`, `include/` and `data/`; trainer tables are generated from the book using
`scripts/emerald_champions_teams.py`. Build a normal ROM with `make -j4 release`
and follow the stamp/release checks in the verification guide.

Preserve ROM/ELF/save ancestry. Continue a changed ROM through a normal battery
save and clean boot, never a savestate from another ROM. Synthetic victories
verify scenes only. Source agreement, native fixtures and earlier earned wins
do not certify newly revised teams or the whole game.

[Credits](CREDITS.md) · [Third-party notices](THIRD_PARTY_NOTICES.md)
