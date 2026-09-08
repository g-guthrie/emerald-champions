# Emerald Champions

Emerald Champions is a modified Emerald campaign built on pokeemerald-expansion, with native preparation tools and a focus on trainer battles. This documentation describes the current executable systems and how to verify changes. It does not certify encounter balance, complete campaign reachability, or release readiness.

- [Game systems](SYSTEMS.md): Retry, Reload, difficulty, caps, and preparation.
- [Build and verification](VERIFICATION.md): build commands, both testing pipelines, and evidence limits.
- [Known issues](KNOWN_ISSUES.md): outstanding defects and questions requiring an explicit decision.
- [Attribution](ATTRIBUTION.md): preserved project credits and third-party notices.

## Source ownership

| Question | Canonical source |
| --- | --- |
| Trainer parties consumed by the game | [trainers.party](https://github.com/g-guthrie/emerald-champions/blob/main/src/data/trainers.party) and [trainer generation](https://github.com/g-guthrie/emerald-champions/blob/main/src/battle_setup.c) |
| Authored trainer and preparation inputs | [data/emerald_champions](https://github.com/g-guthrie/emerald-champions/tree/main/data/emerald_champions/) and their consuming generators in [scripts](https://github.com/g-guthrie/emerald-champions/tree/main/scripts/) |
| Wild encounters | [wild_encounters.json](https://github.com/g-guthrie/emerald-champions/blob/main/src/data/wild_encounters.json) and [wild_encounter.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/wild_encounter.c) |
| Moves, species, abilities and items | [src/data](https://github.com/g-guthrie/emerald-champions/tree/main/src/data/), [pokemon.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/pokemon.c), and battle code in [src](https://github.com/g-guthrie/emerald-champions/tree/main/src/) |
| World events, rewards and progression | [data/maps](https://github.com/g-guthrie/emerald-champions/tree/main/data/maps/), [data/scripts](https://github.com/g-guthrie/emerald-champions/tree/main/data/scripts/), [legendary_signs.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/legendary_signs.c) |
| Difficulty and leveling | [difficulty.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/difficulty.c), [caps.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/caps.c), [configuration](https://github.com/g-guthrie/emerald-champions/blob/main/include/config/caps.h) |
| Save compatibility | `MigrateEmeraldChampionsCoreState` in [overworld.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/overworld.c) |
| Build and required checks | [Makefile](https://github.com/g-guthrie/emerald-champions/blob/main/Makefile), [CI workflow](https://github.com/g-guthrie/emerald-champions/blob/main/.github/workflows/build.yml), [release verifier](https://github.com/g-guthrie/emerald-champions/blob/main/scripts/verify_emerald_champions_release.py) |

Documents are explanations. Machine-readable authoring data belongs under `data/emerald_champions/`; executable campaign scenarios and reference fixtures belong under `tests/`. Generated data must agree with its authoring inputs, but that agreement alone does not prove the resulting battles or services work correctly.

Keep changes to game rules separate from defect repairs. Preserve both Retry and Reload and both testing pipelines. When reporting a result, identify the tested source and artifacts and state what was actually exercised.
