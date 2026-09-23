# Day Care progression pass — September 19, 2026

Partial core-file audit: deposit/withdrawal, slot compaction, Mail transfer,
experience, level previews/cost text, and step/breeding callers. Full inheritance,
egg move tables, menu choreography and all lifecycle combinations remain to audit.

Consolidated duplicated preview/withdrawal normalization and experience math into
PrepareDaycareWithdrawal. The temporary Pokemon gets the native withdrawal form,
species cap and an overflow-safe experience gain. Preview reads its resulting
level without teaching moves or mutating the deposited BoxPokemon; withdrawal
retains the native level-by-level move-learning process.

Native before.log reproduced unsigned addition wrapping experience from 125 to
124 with UINT_MAX steps. after.log passes ordinary/zero/cap/large-step coverage,
and verifies Hoopa Unbound previews the Confined withdrawal cap and leaves the
stored record unchanged. Also clamp negative reported gains to zero for a stored
Pokemon above today's cap. The u32 step counter's eventual wrap after billions
of steps is recorded as low-priority hardening, not fixed by changing breeding
cadence in this pass.

Existing Nursery tests exposed event.inc depending on movement.inc's macro
factory. Replaced the store_lock_anim factory invocation with an equivalent
standalone byte-emitting macro; no game opcode changed. Both Day Care tests and
five Nursery groups pass in work/daycare-audit-20260919/after.log. Release evidence
is in release-build.log and release-gates.log. No AI files changed.

## Free-care dialogue and script cleanup

GetDaycareCostAndPrepareString now reuses GetNumLevelsGainedForDaycareMon rather
than repeating level/nickname formatting behind two cost wrappers. It preserves
zero in VAR_0x8005. The conditional FRLG caller likewise reuses the formatter and
retains its prior zero fee in that build; Emerald's inactive Route5 fallback is
unchanged. Removed unreferenced OnlyOneMon, UnusedEnd and UnusedRetrieveMon script
blocks and their unused standalone text. The active one-conscious-mon deposit
guard remains. Removed the fictional Champions-settlement explanation; both the
initial service offer and withdrawal prompt now state that care is free.

Native experience tests additionally compare buffered names/level gains and zero
cost for ordinary/zero/huge step counts and Hoopa's withdrawal form. All4 combined
Day Care experience/campaign-gift groups pass, including14 gift capacity cases.
Current deposit flow was source-traced: menu cancellation exits; party deposit
protects the last conscious non-Egg; PC selection bypasses that party-only check;
pending Eggs route to the husband; full-party withdrawal exits before mutation.
No native menu/choreography/PC-deposit/record-mixing playback was performed here.
Evidence work/ability-audit-20260920/daycare-cleanup-*. This extends partial audit
coverage, not full Day Care lifecycle acceptance or the complete core audit.

Strict release build/gates pass, stamp a9a4d1f518e8.
