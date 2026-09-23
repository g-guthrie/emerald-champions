# Move restriction and PP pass — September 20, 2026

Partial battle_move_resolution.c source review: action-canceler ordering,
Disable/Taunt/Imprison checks, target adjustment, PP debit, submove selection and
permanent-versus-temporary PP synchronization. No production code change in
this pass. Earlier freeze/Leppa fixes remain documented separately.

Native Encore suite: all16 non-Dynamax test groups pass, including modern and
legacy duration behavior, use-before/after timing, selected-move override,
flinch/sleep/failed-move history, doubles targets, PP exhaustion and selected vs
forced move priority. Two Dynamax-only groups were not executed because release
campaign disables Dynamax; this is not a claim that every engine feature passed.

Added native doubles Pressure coverage: zero/one/two opposing Pressure users,
an allied Pressure user, and two initial PP quantities (six cases). Opposing
abilities stack, allied Pressure does not add cost, debit saturates at zero,
and party PP agrees with battle PP. The test preserves current mechanics rather
than changing AI move forecasting. All cases pass.

Evidence: work/move-restrictions-20260920/tests.log and pressure.log. Only tests
and audit records changed, so no new normal release build was required. Remaining
scope includes complete action-canceler interactions, move damage/effects,
multitarget failure, called moves, and broad campaign tactical validation.

## Transform / Power Trick level-up synchronization

Cmd_getexp's level-up refresh preserved a transformed battler's copied combat
stats, but then unconditionally reapplied PowerTrick's Attack/Defense swap. A
native Mew→Shuckle→PowerTrick→KO→level-up regression proved Attack400 reverted
to100. Move that swap into the non-transformed branch, where base stats actually
were recalculated. The same transformed branch copied level/currentHP but omitted
maxHP; a second native baseline proved battle maxHP53 while party maxHP was56.
It now refreshes maxHP too, retaining copied non-HP stats.

Normal and transformed controls level13to14 through an actual wild-battle KO;
assert PowerTrick, expected attack/defense and maximum-HP agreement. Both baseline
failures are preserved in power-trick-level-baseline-* and transform-hp-baseline-*.
All31 combined convenience EXP/Inclement integration groups pass after repair;
evidence work/ability-audit-20260920/transform-level-*. No AI scoring/information
model, trainer teams or damage formula changes. Broader transformation/form-change
lifecycles remain outside this targeted coverage.

Strict release build/gates pass, stamp 9663d3feafc7. Related source scan found
another active candidate: the non-transformed level-up refresh copies Speed even
when speedSwapped remains set; RecalcBattlerStats preserves that speed under the
current config. Next reproduce SpeedSwap+level-up natively before changing it.
Dynamax/Transform refresh risk is dormant in the production Mega-only ruleset.

## Speed Swap level-up repair

Native level-up baseline reproduced Speed1 becoming200 while speedSwapped stayed
active. Normal level-up control passed. The non-transformed refresh now uses the
existing updateSpeed parameter to preserve swapped battle Speed; party stats,
other battle stats and maximum HP still update normally. No shared AI behavior or
Mega Evolution configuration changed. The test also checks the party's own Speed
remains distinct and the volatile stays set.

All32 combined convenience EXP/Inclement groups pass. Baseline and final evidence:
work/ability-audit-20260920/speed-swap-level-*. Related source scan found no further
active direct Speed overwrite, but RecalcBattlerStats still overwrites PowerTrick
Attack/Defense during Mega/form refresh. Medicham has a legal PowerTrick+Mega path;
next reproduce it natively before fixing the shared refresh helper. That helper
also serves simulated form changes, so any repair needs runtime/forecast parity
review. Production-disabled Dynamax remains outside this scoped pass.

Strict release build and gates pass, stamp 850a4d3abe20.

## Power Trick across Mega/form stat refresh

Native Medicham PowerTrick→Mega baseline reproduced raw Attack236 instead of
swapped206 while the volatile stayed set. Normal Mega control passes. Central
RecalcBattlerStats now reapplies the swap exactly once after copying recalculated
base stats, and shares one updateSpeed predicate between calculation/copy. This
preserves existing Champions SpeedSwap configuration and avoids duplicated tests.

The helper also serves AI_ApplyMegaForm and switch-form forecasts. Source review
of both callers and their snapshots found local party copies, preserved battle
volatile state and no second swap. The omission also exists at preintegration
508775fad8. New native cases cover swapped/normal x liveMega/AIpreview; the actual
AI_ApplyMegaForm helper must produce the same stats and leave the real party record
byte-identical. No AI source, scoring weights, information policy or team edits.

All24 combined Mega/EXP native groups pass. Initial fixture incorrectly compared
against the restored base-form party stats; corrected by independently calculating
Mega-form reference stats before accepting the native swapped-case failure.
Evidence: work/ability-audit-20260920/power-trick-mega-baseline-* and
power-trick-mega-*. This does not resolve the separate guard-scoring issue or prove
all AI planner/gameplay behavior. Other form pathways are source-covered through
the shared helper, not each individually replayed here.

Strict release build and gates pass, stamp 839a5e053524.

## Shared base-stat refresh ownership

Only two production callers use CopyMonLevelAndBaseStatsToBattleMon: the ordinary
level-up branch and RecalcBattlerStats. Both required the same PowerTrick correction.
Moved that correction into the shared copy helper and removed caller duplicates
and the level-up temporary. The transformed level-up branch deliberately does not
call this helper, so its copied stats remain untouched. Existing updateSpeed
arguments preserve their respective SpeedSwap rules. All24 existing Mega/EXP
native groups pass without new tests; live Mega, AI preview and transformed/
ordinary level-up coverage exercise both callers. Evidence:
work/ability-audit-20260920/stat-refresh-shared-*.

Next source candidate: CanMegaEvolve and AI_ApplyMegaForm query form targets without
the transformed-battler guard that CanBattlerFormChange applies in runtime. A
transformed user holding a stone matching its copied species may therefore be
considered eligible for an impossible Mega. This is not yet a proven reachable
bug; reproduce eligibility/runtime/preview behavior before changing protected
forecast logic. No such behavior was modified in this consolidation pass.

Strict release build and gates pass, stamp 23c71e691bcc.

## Transformed form-query / execution agreement

Native Ditto→Medicham holding Medichamite showed CanMegaEvolve returning TRUE
while TryBattleFormChange rejected the transformed battler. A separate native
baseline showed AI_ApplyMegaForm also accepted that impossible preview. The shared
GetBattleFormChangeTargetSpecies now returns the current species for transformed
battlers under the same GEN5+ form-change rule as runtime execution. Every caller
uses current-species as the no-change sentinel; older configured behavior remains.

Four native cases cover stone-based Medicham and move-based Rayquaza copies, both
eligibility and AI previews. Each verifies native execution rejection and unchanged
battle/party records. Existing ordinary Mega controls and PowerTrick previews still
pass; all25 combined Mega/EXP groups pass. Caller and preintegration508775fad8 review
confirms the omission predates integration, not an authored scoring convention.
No direct AI source/weights, teams or future-command knowledge changed; planners
now omit only forms the native runtime already disallows.

Evidence work/ability-audit-20260920/transform-mega-eligibility-baseline-tests.log,
transform-mega-baseline-* and transform-mega-*. Other target-query callers (Ultra
Burst, switch-form forecasts, G-Max lookup) use the same no-change contract by
source review; production remains Mega-only and those paths were not all played.
This does not establish full AI/game completion or resolve the guard-scoring issue.

Strict release build and gates pass, stamp 766676bf259f.

## Set-effect flinch cleanup and status controls

Read set-effect entry/dispatch and initial status/flinch handlers, tracing shared
nonvolatile protections and flinch cancellation. Consolidated five identical
continuation assignments in HandleSetEffectFlinch into one exit; kept InnerFocus
recording conditions, already-flinched guard, action-order/Dynamax exclusions and
actual flag update unchanged. No confirmed mechanics bug or AI tuning change.

New native controls cover four InnerFocus/MoldBreaker combinations and doubles
twoFakeOut hits denying one action, Steadfast+1 once, and normal next-turn action.
An initial fixture attempted FakeOut again on turn2 and was rejected as an illegal
record; corrected to Celebrate to test flinch expiry, not illegal move selection.
Both flinch groups passed before/after. All12 combined flinch/sleep/paralysis
groups pass, including configured sleep duration, paralysis chance/speed and
ThunderWave immunity/failure messages. Evidence under work/ability-audit-20260920/
flinch-baseline-* and status-effect-cleanup-*. Rest of battle_set_effect.c and
full status/doubles/campaign/wild verification remain incomplete.

Strict release build/gates pass, stamp5ed1ce9dc3e5.

## Sleep-cure Nightmare cleanup and shared status clearing

Native doubles baselines reproduced stale Nightmare after both WakeUpSlap and
JungleHealing cure sleep followed by a second sleep later in the same turn.
Endturn cleanup sees the target asleep again, so it incorrectly retains/applies
the old Nightmare. WakeUpSlap now clears it immediately in the narrow status-mask
handler. Private ClearBattlerNonVolatileStatus consolidates command-level status
clearing for BS_CureStatus, BS_ClearStatus, Uproar wake and Refresh: release sleep
clause while old status is available, clear status/Nightmare, emit existing update.
Caller-owned text, targets and script continuations are preserved. BS_ClearStatus
now releases an active sleep clause as well. Existing old-branch scripts/AI/teams
were not retuned; preintegration508775fad8 has the WakeUpSlap omission too.

Related scan correction: Refresh cannot normally cure sleep because CAN_MOVE
excludes it; helper use there is consolidation. Uproar normally prevents immediate
re-sleep while active, so that exact exploit was not established there. Ordinary
waking/berries/other cures already cleared Nightmare; switching/fainting reset
volatiles. Do not classify those unchanged paths as newly demonstrated defects.

Broad run initially64/66: Insomnia/VitalSpirit fixture declared Celebrate only
but selectedSpore; addedSpore. Healer clause fixture expected30percent without
specifying generation, while configuredChampions chance50percent is intentional;
set explicitGEN9, preserving its30percent test contract. No production chance
change. Final all67sleep/SleepClause groups pass including both new doubles cases
and existing AI clause choices. Evidence work/ability-audit-20260920/
wake-slap-baseline-*, jungle-nightmare-baseline-*, wake-slap-* and sleep-cure-*.
Full campaign/core/battle/wild objective remains unfinished.

Strict release build/gates pass, stamp947411215489.

## EerieSpell / Spite PP-drain consolidation

Native EerieSpell baseline confirms permanent PP clamping/synchronization and
Transform/Mimic copied PP isolation, including Leppa recovery. Reused existing
GetMoveSlot in both EerieSpell and Spite instead of duplicate searches (including
Spite's separate identical Max-move lookup loop). Reused MOVE_IS_PERMANENT for
the controller-write guard. Removed plain decimal formatting immediately overwritten
by PREPARE_BYTE_NUMBER_BUFFER and battle_set_effect's now-unused string_util include.
Drain amounts, Max-move lookup/message semantics, exhaustion cancellation, copied
move policy and AI are unchanged. No new mechanics defect established in this pass.

Extended native checks to both drain moves: low/high permanent PP, Transform/Mimic
with originals preserved, EerieSpell-triggered Leppa recovery; existing Leppa/Ripen
and Pressure controls retained. All5 combined groups pass. Evidence under
work/ability-audit-20260920/eerie-pp-baseline-* and pp-drain-cleanup-*.
Full move-effect/gimmick combinations and broader battle/campaign/wild/core objective
remain unfinished. Prior guard-scoring issue remains open.

Strict release build/gates pass, stamp83220ef1c96e.
