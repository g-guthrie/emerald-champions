# Snapshot verification

Fresh named release build completed successfully:
`make -j6 release BUILD_NAME=emerald-champions-playtest-20260909-1728`.
The previous release artifact was preserved. Source branch: `codex/game-repair`,
base commit `635ae1f22ffd559b7e38df275832ac80286eaecb`, including current uncommitted work.
The commit alone does not identify this snapshot; the input stamp binds28,979
declared build inputs and the actual ROM/ELF hashes.

Release verification passed: canonical trainer/preset/Circuit projections,
map geometry, wild-table integrity, patch whitespace, ROM/ELF byte agreement,
header/checksum, memory bounds, and absence of fixture/test-runner interfaces.
These checks do not certify campaign balance or every encounter.

ROM address space used:27,900,424 of33,554,432 bytes (83.15%). The downloadable
ROM is padded to32MiB by the normal build. EWRAM232,282/262,144; IWRAM28,320/32,768.

Fresh-boot smoke check: the actual release ROM runs1,000 frames with normal
Start/A inputs and no memory writes or injected save/state. Screenshot inspected:
the New Game/Option menu renders. This is boot evidence, not an opening battle
playthrough or a complete campaign test.

Latest completed shared battle checkpoint before this snapshot:46/46 native
groups passed. Jose's current in-progress pressure variant additionally passed
five groups/22 parameter cases. Completed trainer coverage is24/516 branches,
19/468 encounters through Joey. More trainer work follows this frozen snapshot.

ROM SHA-256:
`36e8d39bb969e34a31bf4f93b7e2817e31bf4b4ebc8877c35fb2a691a6256679`

ELF SHA-256 (developer artifact retained in the project, not required to play):
`97fe122952b2e16bfb268147b0db8c06c668a74ed0ff6ae9908a928c7cfb839b`

Input SHA-256:
`47c79924005497df14304f30a33a84d1cfd601cf9cdb2946a66dda1d4c34e35e`

Logs: `work/playtest-20260909-1728-build.log`,
`work/playtest-20260909-1728-verification.log`,
`work/playtest-20260909-1728-boot.log`.
Later source edits will intentionally differ from this frozen input stamp.
