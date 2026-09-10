# Emerald Champions — hazard repair playtest

Full ROM: `pokeemerald-champions-hazards-20260909-2058-release.gba`.
An identical copy is in Downloads; the earlier1848 Protect-fixed ROM remains.

This build fixes AI hazards targeting the trainer's own side. Stealth Rock,
Sticky Web, Spikes and Toxic Spikes now target the opponent; saturated hazards
stop receiving setup rewards, and two partners avoid duplicate wasted casts.
Useful extra layers and the attacks Stone Axe/Ceaseless Edge remain available.
It includes the earlier Protect fix and current trainer authoring through Ned,
including Rustboro regional-rival corrections. It is not a completed campaign
audit or a guarantee of optimal AI in every position.

Actual Marc native sequence: Stealth Rock once on the player side, then
High Horsepower and Rock Slide. Complete sampled decision39frames (~0.65s).
Four new shared regression groups cover16 hazard cases;52 selected groups
including temporary Marc diagnostics pass, then50 retained groups pass after
scratch removal. No extra decision search or battle RAM was added.

Fresh named release passes canonical data, geometry, artifact correspondence,
input freshness, header/checksum and memory checks. Boot captures use the
native headless emulator; they are not a complete fresh-save playthrough.
Use an in-game save, not an emulator save STATE from a different ROM build.
Back up saves before trying the new ROM; cross-build save compatibility has
not been comprehensively verified.

ROM used27,901,672/33,554,432bytes,224 fewer than1848; padded download is32MiB.
This net difference includes intervening authoring, not only the hazard patch.
EWRAM232,282 and linker IWRAM28,320bytes are unchanged (verifier IWRAM28,316
excluding alignment).

ROM SHA256: `0ade80ebe0ec0e671de4d83f5eb71a4ec9778f03a76b2f15bbc678c1e4131d33`.
ELF SHA256: `4a46cd7e4c65b3fb910506a8cecd334e40e1a2dacc3b0547f98eadc48253dd94`.
Input SHA256: `c9c8865aeb402681c7cde65ffdf83180a4f760cef6d189e33c70c8ee970a0a63`.

Evidence: `work/hazard-targeting-final.log`,
`work/hazard-shared-and-marc-final.log`, `work/ai-shared-hazard-clean.log`,
and `work/hazards-release-20260909-2058-{build,verification,boot,boot-menu}.log`.
