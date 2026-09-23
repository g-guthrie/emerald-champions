# Stat-change mechanics pass — September 19, 2026

Scoped review: increase/decrease limits, queued reactive changes, stat prevention,
Intimidate handling, Contrary/Simple adjustment, Defiant/Competitive and copying
responses. Full stat copying, ability suppression, reflected multi-stat effects,
and ordering across all doubles combinations remain to verify.

Confirmed Guard Dog minimum-bound bug: IsIntimidateBlocked tested whether Attack
could fall before replacing that fall with +1. At -6 it skipped the ability and
lost the boost. Removed the incorrect gate. Always queue the replacement; native
IncreaseStat already caps at +6. Merely replacing the check with a maximum-bound
check would incorrectly allow Intimidate to lower a maxed Guard Dog, so no such
new branch was introduced.

Native before.log reproduces Charm three times then switching in Intimidate,
leaving Guard Dog at -6 instead of -5. after.log verifies minimum, neutral and
maximum Attack through actual move/entry sequences. doubles.log checks independent
Defiant and Competitive responses to one Intimidate activation. No AI scores,
authored teams or stat multipliers changed. Evidence: work/stat-audit-20260919/.

Related caller review found no analogous inverted bounds in other queued ability
responses: Defiant/Competitive use their raised-stat maximum, Mirror Armor keeps
reflection prevention independent, and Rattled/Adrenaline Orb queue capped raises.
This is not full battle-mechanics certification.

## Intimidate stat-preview side-effect repair

Read battle_stat_change.c's bounds, prevention, reactive queues and additional
state handlers. Native baseline reproduced CanStatChange with onlyChecking and
Intimidate appending a GuardDog secondary response (queue0->1). Same unguarded
prevention implementation exists in preintegration508775fad8, independent of the
already-fixed GuardDog minimum-stage issue. Two early returns now preserve the
TRUE blocked answer while preventing preview-only text/script/ability/queue writes
for GuardDog and Gen8 Intimidate-immune abilities. Real ability behavior unchanged.
No authored teams, AI weights, information policy or score logic changed.

Caller scan: AdjustSwitchinStat is the only live caller setting both flags.
Broad candidate snapshots restore these fields/history between candidates and on
exit; this is a local query-side-effect defect, not proof of a persistent live
extra boost. The candidate did not apply the erroneously queued GuardDog reaction.
Other prevention helpers already guard writes; CanAnyStatChange queue construction
belongs to the live move-resolution pipeline. No analogous second leak confirmed.

All5 compiled groups pass: parameterized preview purity across5immune abilities,
GuardDog min/neutral/max actual Intimidate, independent Defiant/Competitive doubles
responses, and two native AI switch-in forecast controls. Evidence under
work/ability-audit-20260920/intimidate-preview-*. Full AI/campaign balance is not
established; prior guard-scoring failure and larger battle/wild/core audit remain.

Strict release build/gates pass, stampea5b431616f3.

## Stat queue reset consolidation and FlowerVeil lifecycle check

Continued through battle_stat_change.c's move conditions, stage adjustment,
prevention helpers, reactions, queues and animation bookkeeping, tracing the
live resolver/command callers. ClearBothStatChangeQueues now reuses the two
existing reset functions instead of repeating their memset/count/animation logic.
Native doubles check verifies primary reset preserves secondary reactions,
per-battler secondary reset preserves other battlers, and full reset clears both.

A suspected fainted FlowerVeil-provider protection issue was not reproduced:
real spreadSnarl knocking out Comfey correctly lowers its surviving Grass ally's
SpAttack, while livingComfey protects it. Preserve current mechanics; the source
marks fainted providers off-field so fresh ability lookup returnsNONE. Added
this two-case regression without changing FlowerVeil/AI/teams. All5 compiled
groups pass with prior Intimidate preview/GuardDog bounds/DefiantCompetitive tests.
Evidence work/ability-audit-20260920/flower-veil-baseline-* and stat-queue-cleanup-*.
This is scoped coverage, not exhaustive reflected multi-stat/suppression/order or
full game acceptance. Known guard-scoring issue and larger objective remain open.

Strict release build/gates pass, stamp20edf92c7ade.
