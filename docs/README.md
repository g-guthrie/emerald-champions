# Emerald Champions documentation

Emerald Champions is the playable campaign built on this
`pokeemerald-expansion` fork. Start with the
[complete campaign book](EMERALD_CHAMPIONS_CAMPAIGN_BOOK.md), then use the
[story-mode source audit](EMERALD_CHAMPIONS_STORY_MODE_AUDIT.md) and
[verification audit](EMERALD_CHAMPIONS_VERIFICATION_AUDIT.md) to understand
what is checked and what still requires a build, runtime execution, visual
inspection, device coverage, and a human playthrough.

Current technical ledgers:

- [Evidence ledger](emerald_champions_migration/FEATURE_PARITY.md)
- [Save compatibility contract](emerald_champions_migration/SAVE_COMPATIBILITY.md)
- [Competitive battle-set migration](emerald_champions_migration/BATTLE_SET_MIGRATION.md)
- [Legendary Signs and Champions Circuit](emerald_champions_migration/LEGENDARY_SIGNS_AND_CIRCUIT.md)
- [Story bible](emerald_champions_migration/STORY_BIBLE.md)

These documents project executable source into readable form. They do not make
the game perfect or release-ready; fresh candidate logs and human/visual checks
remain authoritative for those claims.

## Upstream engine documentation

The included README below describes the `pokeemerald-expansion` engine base.
Its statement that the engine is not a game on its own does not describe the
Emerald Champions campaign layered on this fork.

{{#include ../README.md}}
