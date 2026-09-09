# Emerald Champions 6.0.1

Birch rescue fix on top of the published 6.0 snapshot:

- Both rescue opponents are level 2 (previously level 3).
- Poochyena follows Zigzagoon one tile behind through the entire opening chase, instead of moving only after the chase ends. Both finish in their existing pre-battle positions.

Zigzagoon retains its existing Belly Drum / Extreme Speed moveset, Adamant nature, Sitrus Berry, and Attack investment. This is a level reduction, not a replacement moveset.

Validation: all 36 synchronized chase steps were checked for one-step following and correct final positions. This is a script-path check, not emulator visual confirmation or a measured balance result. The release build and all existing release gates passed.

The parallel encounter AI authoring task remains separate; this patch contains only the requested rescue changes relative to 6.0.

ROM SHA-256: `35e420fac7663c1e77a6357afa4d120bd1803321af271ccca3a5621edac62947`
