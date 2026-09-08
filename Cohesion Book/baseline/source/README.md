# Emerald Champions

A doubles-focused Emerald adventure built on pokeemerald-expansion, with free team-preparation services, adjustable trainer levels, and separate battle Retry and save Reload actions.

The executable source defines the game. Documentation explains that source; passing a check establishes only the behavior that check actually covers.

- [Implemented systems](docs/SYSTEMS.md)
- [Build and verification](docs/VERIFICATION.md)
- [Known issues and audit limits](docs/KNOWN_ISSUES.md)
- [Source and data ownership](docs/README.md)
- [Credits](CREDITS.md) and [third-party notices](THIRD_PARTY_NOTICES.md)

Game logic lives in `src/`, `include/`, and `data/`. Authored configuration and imported datasets belong in `data/emerald_champions/`; executable scenarios and reference baselines belong in `tests/`. The `docs/` directory contains explanations, not build inputs.

The gameplay traversal pipeline and battle-testing pipeline have different purposes. Traversal can automatically resolve battles to test progression. Combat evaluation must run real battles with that automation disabled. Neither a static check nor an automatically won campaign demonstrates difficulty or balance.
