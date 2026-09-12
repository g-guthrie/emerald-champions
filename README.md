# Emerald Champions

A doubles-focused Emerald adventure built on pokeemerald-expansion, with free
team preparation, adjustable trainer levels, battle Retry and save Reload.

The executable source defines the game:

| Content | Owner |
| --- | --- |
| Engine and gameplay | `src/`, `include/`, `data/` |
| Authored trainer/preset inputs | `data/emerald_champions/` and their consuming generators in `scripts/` |
| Compiled trainer parties | `src/data/trainers.party` |
| Wild encounters | `src/data/wild_encounters.json` |
| Scenes and dialogue | `data/maps/`, `data/scripts/` |
| Build and verification | [docs/VERIFICATION.md](docs/VERIFICATION.md) |

Use a fresh save. Selected tests do not certify the full campaign or its balance.
Read [AGENTS.md](AGENTS.md) before editing. Credits and required notices remain in
[CREDITS.md](CREDITS.md) and [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
