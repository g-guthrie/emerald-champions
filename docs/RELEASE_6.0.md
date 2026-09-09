# Emerald Champions 6.0

September 9, 2026 snapshot of the current working tree, isolated before compilation so ongoing battle AI work could continue independently.

Includes the current opening-battle AI repairs, authored trainer updates, level-3 Birch rescue and opening flow repairs, nurse flow improvements, and approved wild habitat revisions. The sequential executable AI review is still in progress; this release does not claim completion of every battle or full campaign playtesting. See docs/trainer_authoring/ai_restart_0001.md and subsequent encounter reports for previously recorded native diagnostic evidence.

Built from clean release objects using Arm GNU Toolchain 15.3.Rel1. Production artifacts have test and headless interfaces disabled. Use a fresh save.

The named download is `Emerald Champions 6.0.gba`; `pokeemerald-release.gba` contains identical bytes. The adjacent input stamp binds the compiled inputs and ROM/ELF hashes. Release verification results and SHA-256 are recorded below after build completion.

Release gates: PASS. Compiled map/tile integrity, configured trainer abilities, authored trainer materialization, preset/Circuit output agreement, wild table integrity, patch integrity, source stamp, ROM/ELF identity, header/checksum, memory regions, and forbidden test interfaces were checked. No new campaign playthrough or encounter-by-encounter runtime certification was performed for this snapshot.

SHA-256: `5202d59396e4ae4feaecebf2dd16ef7ad558d1f8f13f3e5021d044b97c6f0e82`
