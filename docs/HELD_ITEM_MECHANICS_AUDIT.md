# Held-item mechanics pass — September 19, 2026

Scoped battle_hold_effects.c review of HP berries, status healing and PP restoration;
traced controller synchronization, move-use PP deduction, Transform/Mimic copying,
Lunar Dance switch-in and destroyed-berry restoration tracking. Full held-effect
and broader battle mechanics coverage remains incomplete.

Confirmed Leppa defect: target selection inspected permanent party PP instead of
active battle PP. Consequently copied moves at zero PP were missed. Its write
contract was also reversed for temporary moves: controller writes altered party
PP while active battle PP was only updated when permanent. Now choose/restore
active slots and mirror the existing PP-deduction ownership rule: always update
battle PP, synchronize party PP only for permanent slots. Temporary copies use
the engine's five-or-base-PP allocation; ordinary moves retain configured maximum
PP and Ripen behavior. No AI scoring, teams, Regenerator timing or PP base data
changed. Transform and Mimic reference behavior was cross-checked at:
https://bulbapedia.bulbagarden.net/wiki/Transform
https://bulbapedia.bulbagarden.net/wiki/Mimic

Native before.log reproduces ignored exhausted copied PP; after.log verifies
Transform restoration, permanent PP preservation, ordinary/Ripen restoration,
and existing Bug Bite/Incinerate tracking. copied-tests.log extends copied-move
coverage to Mimic. Test setup uses explicit PP and copied move-slot selection:
the test runner's permanent moves cannot be selected by a newly copied name.
Evidence: work/leppa-audit-20260919/. Release build/gate evidence recorded there.
Remaining: forced Leppa activation, all other held effects, multibattler timing,
complete battle resolution and user-facing difficulty acceptance.

## Freeze infliction and cures

New source-proven mechanics defect: MOVE_EFFECT_FREEZE initialized freezeTurns
at 2, while action resolution interprets that as two elapsed immobilized actions
and forces thaw. Initialize at zero, preserving both pre-Champions probabilistic
thaw and Champions' configured two-action ceiling. The defect predates integration
(blame 231531c6a3e); it is not an AI or team-tuning change. Native infliction test
uses an actual Ice Beam secondary and forced failed thaw rolls; before.log fails,
then all 18 Freeze groups pass. Lum now clears elapsed freeze state as Aspear
already does; cure.log tests both after a frozen action and Trick-delivered Berry.
Evidence: work/freeze-audit-20260919/.

Related cure paths (medicine, Heal Bell, Natural Cure, Hydration/Shed Skin,
Magma Armor, Healing Wish and shared status cures) can retain dormant elapsed
counts. Normal re-infliction now resets those correctly. Record those paths for
later shared status-lifecycle cleanup; do not claim they were rewritten/tested
here. AI simulations remain untouched.

## Original Berry consumption survives Cud Chew / item-history changes

Native Farigiraf eats Sitrus, then CudChew clears usedHeldItem. With normal campaign
restoration flags, baseline incorrectly restored Sitrus without Regenerator.
Added a transient originalBerryConsumed bit in existing PartyState padding,
recorded alongside ordinary consumption and NaturalGift. It survives changes to
Recycle's last-item slot. Recycle/Harvest/Pickup recovery clears the consumption
bit when that holder recovers its original item. Destruction remains separately
tracked for BugBite/Incinerate. New caught/replacement restoration baselines clear
old consumption/destruction/recycle history, preventing slot history inheritance.
No save layout expansion, new item or AI scoring change.

Native CudChew case verifies spent without tool/restored with tool. Native
Recycle→KnockOff control verifies recovery cancels consumption debt and removal
still restores normally. Function-level tests cover a second consumed Berry,
fresh catch baseline, ordinary berries, FocusSash, destruction and special-battle
restoration. All4 focused groups pass. These tests evaluate campaign restoration
flags after native recorded battle actions; they are not a full field-return UI
playthrough. Evidence work/ability-audit-20260920/cud-chew-regenerator-baseline-*
and berry-history-*.

Related review initially misidentified KnockOff as calling removeitem; live source
shows battle_move_resolution directly removes and marks stolen, while
BattleScript_KnockedOff only shows animation/text. Native recovery/KnockOff control
passes. Do not treat that incorrect scan claim as a remaining defect. Fling keeps
its existing expended-item behavior; the separate original record changes retention
of consumption history, not what counts as an item spend.

Strict release build and gates pass, stamp8de1001a436a; EWRAM usage unchanged.

## Berry ownership across transfers and recovery

Native Trick baseline proved a player original Sitrus transferred to/eaten by a
foe was restored to the player without Regenerator. Item-name comparisons cannot
resolve this when several original owners hold identical Berries. Added transient
current-held and used-item origin bytes to PartyState, encoding trainer/slot+1.
Battle initialization and caught-slot rebasing initialize them; no saved data
format change. Original-owner consumption/destruction flags now drive restoration.
Removed last-used/current-item equality heuristics and the obsolete currentItem
argument from GetBattleRestoredHeldItem.

Trick swaps origins; Bestow/Symbiosis and ordinary theft move them. Delayed
Pickpocket uses the original off-field party slot, not its replacement. Consumption
and NaturalGift move current origin to used origin; Recycle/Harvest/Pickup recover
from that source and clear the decoded owner's consumption. CudChew clears used
history without losing consumption; BugBite/Incinerate mark the decoded owner.
KnockOff clears current origin without consumption. AirBalloon/CorrosiveGas clear
only current origin, retaining any earlier Recycle history; BallFetch creates an
unowned origin. Whole BattleStruct AI snapshots include the new fields, so temporary
candidate item simulations cannot leak them into runtime.

All42 focused native groups pass, including original Trick/Bestow consumption,
identical-Berry swaps, recovery then return/KnockOff, transferred destruction,
Thief/Covet consumption of opponent-owned Berries, CudChew, and delayed Pickpocket
origin assertions after U-turn/RedCard switching. The broader theft suite had a
stale HP101/200 fixture: current half-HP threshold is101, so it ate Oran before the
attack. SetHP102 while retaining strict animation checks and adding actual item
ownership assertions. No berry threshold/mechanics change was made for that test.
Evidence work/ability-audit-20260920/berry-transfer-baseline-* and berry-origin-*.

Separate pending seam from source review: Gen9 wild theft directly adds an item
to the Bag, and later capture may restore the same opponent baseline again;
AddBagItem failure is also unchecked there. This needs its own native reproduction
and policy/transaction review; do not claim the entire item lifecycle is complete.
Full campaign/core/battle/wild objective and guard-scoring issue remain unfinished.

Strict release build and gates pass, stamp67591383bea6.

## Wild theft extraction and capture restoration

Native Thief/Covet baseline verified HyperPotion entered the Bag once but the
same restoration endpoint used by capture still returned HyperPotion, recreating
it. StealTargetItem now validates Bag insertion before donor mutation and reports
success/failure. Successful Gen9 extraction retires only the original baseline
identified by its validated origin; cleanup/capture cannot recreate the extracted
item. Failed insertion preserves donor item/origin and avoids success scripts.
Native steal, Magician and StickyBarb callers honor failure; Pickpocket cannot hit
the Bag branch. Removed a redundant caller-side attacker-item assignment.

All43 focused theft/Pickpocket/Regenerator groups pass. Wild success cases assert
one Bag item and no capture-restoration item. Failure cases use GIVE_PLAYER_ITEM
with a full Sitrus stack, verifying unchanged quantity, held item and origin plus
no theft-success animation. Initial manual Bag setup was cleared by the test
runner's ResetTestInventory and was replaced with its supported inventory API;
that fixture failure was not a production regression. Capture modal/field return
was source-traced through the tested endpoint rather than replayed end-to-end.
Evidence work/ability-audit-20260920/wild-theft-baseline-* and wild-theft-*.

No item drop rates, battle teams, AI scores or save-data format changes. This
closes the previously recorded theft-to-Bag/capture duplication seam; full item,
core, campaign, battle and wild-distribution acceptance remains unfinished.

Strict release build and gates pass, stamp334caebf9ed2.

## Shared move-driven held-item destruction

Consolidated the identical Incinerate/BugBite transaction in battle_set_effect.c
into private DestroyHeldItemForMove: original-owner destruction record, item clear,
Unburden activation, controller update and continuation script push. Each move
retains its own eligibility checks and effect script; Pluck uses the existing
BugBite effect. No change to Regenerator timing, destruction/consumption policy,
StickyHold, resistant/Jaboca Berry precedence, Recycle history or authored AI.

Added six native ability/move cases (BugBite/Pluck/Incinerate x StickyHold/Unburden),
checking retained/destroyed item, original owner record, Unburden and no false
Recycle eligibility. All10 compiled Regenerator groups pass, including transfer/
identical-berry ownership, recovery, CudChew and restoration endpoint checks.
Those recorded-battle restoration checks evaluate the production endpoint under
campaign flags, not a full postbattle field UI replay. Evidence under
work/ability-audit-20260920/item-destruction-cleanup-*. Other set-effect handlers
and whole-game battle/campaign/wild acceptance remain incomplete.

Strict release build/gates pass, stamp893610da0375.
