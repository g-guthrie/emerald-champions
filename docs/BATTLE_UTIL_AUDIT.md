# Battle utility audit — September19

Scope: battle_util.c and direct item-restoration, field-gift and hazard callers.
Solo code review, no full campaign playthrough. The previous speed-sort cleanup
is already merged in b677fd82ea; changes below are new local work after that merge.

## Source-proven lifetime defect

GiveEmeraldChampionsStarterPair runs in CB2_GiveStarter before the rescue starts.
AllocateBattleResources happens later in CB2_InitBattle; gBattleStruct is initially
NULL and is freed/set to NULL after battles. Yet the starter giver called
RecordPlayerPartyMonHeldItemForRestoration twice. An older prepared-field-gift
special also called it. That helper dereferenced the battle allocation directly.
The GBA can ignore writes in that address region, so this is not a claim that
previous successful playthroughs crashed. It is an invalid C lifetime dependency.

Removed those field-side recording calls and the now-unneeded party-slot scan.
Battle initialization already records all starting held items. Live capture/swap
recording remains, so incoming captures keep the correct restoration baseline.
The public restoration helpers now return safely when no battle allocation exists.
No starter species, presets, delivery results, receipt flags or story sequence changed.

## Behavior-preserving cleanup

Simplified duplicated Heavy-Duty Boots checks without changing the public function
signature or authored AI call sites. Poison-type Toxic Spikes absorption happens
in battle_switch_in.c before this predicate, and remains unchanged. The unused
context argument is retained deliberately to avoid unrelated caller/API churn.

Reviewed bag restrictions, evolution-counter boundaries and the Champions item
restoration path. No additional design changes were justified there. Retained
ordinary trainer Bag restrictions, the Pyramid exception, evolution conditions,
modern party EXP and postbattle berries. This is not exhaustive validation of every
ability/damage routine in the large file.

## Verification

Deus MCP caller traces (before edits) are stored in the adjacent work directory;
source inspection confirmed both caller lifetime and the battle-start snapshot.
Native test groups PASS:
- speed sorting in both directions: stable ties and unchanged RNG;
- Toxic Spikes: Poison+Boots absorbs; non-Poison+Boots stays safe; no Boots is poisoned;
- starter and prepared field gifts with gBattleStruct=NULL; no-op restoration outside battle;
- consumed Sitrus Berry restoration during a live battle allocation.
Normal release build with UNUSED_ERROR=1/DEPRECATED_ERROR=1 and release gates PASS.
Logs: work/battle-util-audit-20260919/final-tests.log and release-gates.log.
Authored AI and canonical trainer teams still match pre-integration508775fad8.
