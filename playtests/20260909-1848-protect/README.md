# Emerald Champions — Protect repair playtest

Open `pokeemerald-champions-protect-20260909-1848-release.gba` in your GBA emulator.
The previous1728 download is preserved separately.

This snapshot adds shared AI corrections for marginal repeated Protect and
empty double-Protect. Useful partner protection, orb activation, Speed Boost,
healing and countdown waits remain available. It also includes the intervening
trainer work through Devan. This is an unfinished campaign playtest, not a claim
that all516 branches have been audited or that every guard decision is optimal.

Use a fresh save for clean comparisons. Do not load an emulator save STATE
from a different ROM build. Back up existing saves before experimenting; no
cross-build save compatibility claim is made here.

Verification:61 selected native groups passed, then46 retained shared groups
passed with temporary diagnostics removed. The reproduced early empty double
guard and later back-to-back gamble are improved. Sampled complete opponent
decision is48 GBA frames, approximately0.8seconds, not a campaign-wide maximum.

The fresh named release passes canonical data, geometry, artifact correspondence,
input freshness, header/checksum and memory checks. Source inputs and artifact
hashes are included in the input stamp. Boot screenshots are native rendering;
this is not a full fresh-save campaign playthrough.

ROM used27,901,896/33,554,432 bytes,1,472 more than the1728 snapshot; the normal
download remains padded to32MiB. EWRAM232,282 bytes and linker IWRAM28,320 bytes
are unchanged. The verifier measures28,316 IWRAM bytes excluding trailing
alignment. The growth includes intervening authoring, not just the guard patch.

ROM SHA256:
`098d3a3fe471ae5c917d6350798d797bfa24a7c27e4e66d2db63ab4088d1c94c`

Input SHA256:
`d08175914b468a5f90f1cd7c95e8a97828c6a9bf01a353e023d13387e6f9d8bd`

Developer logs: `work/protect-empty-pair-final.log`,
`work/ai-shared-protect-repair.log`, and
`work/protect-release-20260909-1848-{build,verification,boot,boot-menu}.log`.
