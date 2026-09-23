# Storage system audit — September 19, 2026

Scoped source pass: release/deposit/withdraw/shift and minimum-party guards;
moving-mon origin/placement, summary save/reopen, held-item persistence, and
configured release behavior. This is not an exhaustive audit of all storage UI,
sprite, wallpaper, selection or save combinations. AI was not changed.

Confirmed and fixed: AtLeastThreeUsableMons counted party Eggs as usable while
its PC count excluded them. Two real Pokemon plus a party Egg could therefore
pass the release check. Reused CountPartyNonEggMons for consistent semantics.
The carried Pokemon contribution remains valid: Release targets the carried mon,
and a carried Egg is rejected before this check. A proposed carried-Egg bug was
retracted after tracing the actual target; no speculative guard was added.

Removed obsolete RunCanReleaseMon polling/busy-loop logic because release
eligibility is now calculated synchronously once. Preserved the existing release
animation and refusal sequence. SetMovingMonData now reads its explicit position
argument instead of the global cursor; all current callers pass the same value,
so that cleanup does not intentionally change behavior.

Related source checks: deposit/move preserve the last living non-Egg party mon;
shift rejects replacing that mon with a fainted mon or Egg. Summary restores
full party-origin moving mons and authoritative box data for box-origin mons.
OW_PC_RELEASE_ITEM is GEN_3: ordinary held items intentionally leave with released
Pokemon. The inactive modern item-return branch was not treated as a current bug.

Evidence: work/storage-audit-20260919/before.log reproduces the party-Egg failure;
after.log passes the native regression covering party Egg, boxed Egg, a real PC
reserve, and a carried release target. Initial fixture overallocated a second
PokemonStorage and was corrected to initialize the test runner's existing storage;
production memory layout was not changed. Strict release compilation and all deterministic release gates passed; results
are recorded in release-build.log and release-gates.log. No manual full playthrough.
