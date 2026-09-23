# Party menu and held-item seams — September 19, 2026

Scope: source review of party-menu held-item give/take/move/Mail callbacks,
menu allocation/teardown, item-triggered forms, and linked PC storage handling.
Also inspected Leveler/evolution continuation, ability-selection menus, and
move-learning state handlers. This is not an exhaustive audit of all 8,900 lines
or of the whole game. Authored battle AI sources were not changed.

## Fixed

- Moving an ordinary held item onto a Mail holder used to swap only item IDs,
  leaving the saved message on the wrong holder. The extracted transaction now
  rejects Mail in either direction before any mutation; the menu uses the
  existing remove-Mail message and returns to party selection.
- Both canceled-Mail callbacks restored the original item without restoring its
  item-dependent form. They now share restoration that updates species/stats.
- Giving Mail from the Bag runs after party-menu teardown. A resulting form
  change could dereference freed party-box graphics. Teardown now clears freed
  pointers; form handling updates Pokemon data while skipping absent graphics.
- The Pyramid held-item toss path now applies the resulting item-form change.

## Related checks and preserved work

PC storage move/swap/give paths already reject Mail and update boxed/party forms.
No change was needed there. Existing Give/Take success paths retain their normal
item accounting; the fixes do not change battle restoration or the Regenerator.
A concurrent data1-to-learnMoveState correction appeared in the move-learning
continuation handler. Preserved and source-reviewed: data1 contains a move ID,
whereas learnMoveState selects ordinary/chained/tutor learning. Its authorship
and runtime verification are separate from this pass.

## Evidence

work/party-audit-20260919/before.log reproduces the Mail-transfer failure while
ordinary swapping passes. after.log passes all three native regressions: Mail
attachment preservation, ordinary/empty-holder transfers, and canceled Mail
restoring Giratina form and stats without party graphics. The first form-test
attempt needed CalculateMonStats in its fixture because CreateMon alone does
not initialize the calculated stats; no production behavior was changed to
satisfy that fixture. Strict release compilation and all deterministic release gates passed. Results
are in release-gates.log. These checks are scoped evidence, not a perfection claim.

## Script move-update boundary

ScriptSetMonMoveSlot allowed invalid move slots to alias unrelated mon-data
fields; the native baseline with slot MAX_MON_MOVES changed party bytes. Its
legacy last-member fallback also subtracted one from an empty party count.
The helper now rejects slots >=MAX_MON_MOVES and move IDs >=MOVES_COUNT_ALL,
recalculates party count for the last-member sentinel and returns if empty,
and rejects explicit empty party slots. Valid Eggs remain supported because
the Mystery Event Pichu script intentionally grants Surf to an Egg. MOVE_NONE
remains a valid clear operation. Removed the obsolete BUGFIX bounds split;
the event macro comment now matches the unconditional safe boundary.

Current repository setmonmove callers are Mystery Pichu slots1..5/slot2, all
valid; this is a latent API corruption fix, not evidence of a known campaign
crash. Related form service, move deletion and Route117 gift-egg callers guard
selection/slot inputs. No additional active analogous mutation was found.

All15 combined party and wild native groups pass. Regression covers full-party
byte preservation for invalid slot/move/empty selection, explicit Egg Surf/PP,
last-member indices6/255, MOVE_NONE clearing and empty-party fallback. Baseline
fails on the invalid-slot mutation. Evidence work/ability-audit-20260920/
script-move-baseline-* and script-move-*. No authored team or AI changes.

Strict release build/gates pass, stamp d545a10cb914.

## Native EV service arithmetic boundary

IncreaseChosenMonEVs subtracted unsigned current/total EVs from their caps without
checking whether an existing record already exceeded either cap. A native baseline
proved HP EV255 plus a request for4 became3. The script's preliminary check rejects
this input, so this is a latent direct-API/legacy-record defect rather than a proven
normal service-menu failure. Mutation now reports the existing stat and FALSE,
then returns unchanged when either cap is already reached/exceeded. Otherwise
one min expression clamps the request to both remaining capacities. Valid paid
menu choices and normal gameplay values are unchanged; no automatic repair of
legacy spreads or changes to authored opponents.

Native cases cover total-limited gain, stat-limited gain, over-stat and over-total
records, request65535 and zero. Verify complete mon-byte preservation on rejected
over-cap cases, result variables and unrelated stats. All23 Inclement integration
groups pass. Evidence work/ability-audit-20260920/ev-boundary-*.

Related scan: vitamin/EV-item increase paths guard caps before subtraction.
MonGainEVs battle code still uses signed/unsigned adjustment arithmetic that can
normalize a legacy255 stat down to252. That behavior and its compatibility intent
were not changed or accepted as fully audited here; normal current records cannot
reach that input through the inspected mutation paths. ChosenMon invalid-selection
fallback and comprehensive stat-service lifecycle review also remain outstanding.

Strict release build and gates pass, stamp dbd47dd7652d.

## Battle EV calculation compatibility refactor

MonGainEVs now collects the six authored yield fields into one stat-indexed array,
applies Power-item/Pokerus/Macho-Brace bonuses in one shared loop, and calculates
new capped values explicitly. Removed repeated stat branches and mixed signed/
unsigned adjustment arithmetic. Preserve existing stat order, total-cap stopping,
per-stat caps and the battle routine's legacy255-to252 normalization when total
room exists; records already at the total cap remain untouched. This is deliberate
behavior preservation, unlike the separately repaired service255-to3 wrap defect.
No AI, opposing-team or EV-yield data changes.

Thirteen native fixture cases passed before and after: ordinary multi-stat yields,
active/cured Pokerus, MachoBrace stacking, matching/nonmatching Power stats, per-
stat/total clamping, and legacy records below/at total cap. All24 Inclement
integration groups pass afterward. Evidence work/ability-audit-20260920/
battle-ev-baseline-* and battle-ev-*. This covers the award helper, not every
battle-exp recipient/capture/share lifecycle or all Pokemon core routines.

Strict release build and gates pass, stamp e569d1f89145.

## EXP/EV recipient eligibility at maximum level

Cmd_getexp checked IsValidForBattle only inside its ordinary-level branch; its
MAX_LEVEL branch could award EVs to a fainted former participant still present
in the historical sent-in bits. A native trainer battle reproduced this: fainted
level13 receives no EV, fainted level100 receives one Speed EV after the replacement
wins. Unified eligibility before the max-level split, reused the recipient pointer,
and removed the redundant inner validity check/zero assignments. Living capped
recipients still receive EVs while skipping EXP messages and animation.

The initial wild-battle reproduction timed out on forced replacement; it did not
prove a game defect. Switched to the existing trainer AI battle harness with a
single forced opponent move, which supports replacement and reproduced the EV
mismatch. No AI logic changed. Final native coverage includes both fainted levels,
living14/100 recipients, normal modern EXP, half share beside a capped lead, large
awards stopping at cap and individual legendary caps. All30 combined convenience
EXP/Inclement integration groups pass. Catch EXP shares this production path by
source trace, but was not separately replayed in this pass.

Related scan found no other max-level EV award loop; case1 distribution already
filters valid recipients while historical participant bits explain the edge case.
Evidence work/ability-audit-20260920/exp-recipient-baseline-* and exp-recipient-*.
Full battle/campaign/core/wild goal and the separate guard-scoring issue remain open.

Strict release build and gates pass, stamp 5c68a4bdad1e.

## Native stat-service selection boundary repair

Baseline native snapshot proved out-of-range slot PARTY_SIZE modified the lead
through ChosenMon's fallback. Replaced fallback with shared GetServiceMon(slot),
rejecting out-of-range, empty, Egg and BadEgg inputs. All selected-party mutators
and readers use it, including HiddenPower's saved800A slot. Invalid stat IDs no
longer alias HP. Invalid reads clear output buffers/numeric scratch. Consolidated
EV/IV list and single-stat formatting without changing valid text or stat order.
Previous EV saturation/legacy protections are retained.

Native tests check25 invalid-recipient/mutator combinations, valid EV/IV format,
invalid output clearing and bad-stat no-mutation; existing selected-partner,
HiddenPower16types, EV bounds and battle gains all pass. All27 Inclement groups
pass. Logs stat-selection-* under work/ability-audit-20260920 include real failing
baseline. Read-only worker caller audit confirms live menus already reject cancel
and Eggs before payment; empty slots cannot be chosen, and HiddenPower preserves
800A correctly. This was a latent C API defect, not proven normal-UI corruption.
Paid scripts still assume their validated selection stays valid; synthetic direct
entry into a payment block is not an atomic transaction API. No normal-flow
payment bug or other production fallback-to-lead helper was found.

Full core/campaign/battle/wild audit remains unfinished; no playthrough claim.

Strict release build/gates pass, stamp5af64d759a0b.
