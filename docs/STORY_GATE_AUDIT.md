# Story gate audit — September 19, 2026

Scope: source-first progression and NPC dialogue, in story order. Review each
reachable branch, callback, reward, failure/retry and next gate. Refactor only
when it removes duplication or an actual error. Native checks below are narrow;
this ledger is not a claim of a completed campaign playthrough or full-game audit.
Authored AI and trainer teams remain protected.

## Opening pass

| Gate | Source review and current evidence | Remaining limits |
| --- | --- | --- |
| Truck → home → clock → TV | Both gender flag/warp paths; intro states1–7; Mom, movers, notebook, clock, TV and signs reviewed. Running Shoes booklet corrected to hold B to run, matching movement code and Mom's note. | No new visual choreography capture. Postgame home dialogue deferred. |
| Meet rival → Route101 access | Both genders, upstairs PokeBall interaction and downstairs coordinate-trigger alternative. Rival Mom/sibling dialogue and town blocker reviewed. Four duplicated region selectors replaced by one nine-region selector. | Native menu-result dispatch checked; no new full UI scene replay. |
| Birch rescue → lab | Pair grant, failure/retry, rescue flags, hidden actors and lab warp traced. Lab now acknowledges both partners and independently offers each nickname; no additional grant. Aide and lab flavor text reviewed for opening state. | Full native rescue fight was not replayed this pass. Naming UI callback/slot ownership checked in source. |
| Oldale preparation | All local house/Mart/Center NPC text, footprints blocker, rival return triggers and promotional gift reviewed. Clerk bindings and prebadge stock checked. Nurse tool-delivery entry and retry branches checked. | Shared PC/tutor/vendor implementations are separate subsystem work; source dialogue review does not certify their entire UI. |
| Route103 rival → lab → Running Shoes | Both genders' battle entry/return flags and directions toward lab checked. Pair references corrected. Pokédex receipt advances adventure/town/rival states; shoes advance town state4 and unlock B-dash. | Postgame Route103 rematches/east-bank return and home ticket/Lati branches deferred to their gates. Rod/Ball gifts have theoretical full-storage robustness gaps, not demonstrated reachable opening failures. |
| Route102 → Petalburg/Wally | Initial-visit source pass recorded below: local NPCs, callbacks, tutorial/return and west exit. | No new cinematic replay; later Norman/Surf visit remains pending. |

## Repairs and analogous-bug scans

- Upstairs Galar/Paldea options fell through to Hoenn. Shared selector handles all
  nine choices and cancel→Hoenn, with explicit Hoenn cursor. Both downstairs scene
  path variables remain intact. Sol Low scanned other expanded menus and found
  Furfrou's uninitialized cursor; it now starts on its first trim. No other missing
  active menu cases or cursor seeds were found in that bounded sweep.
- Birch's lab only acknowledged/nicknamed one member of the granted pair. Shared
  nickname helper now runs for slots0 and1. Sol found the same stale singular
  references in both Route103 rival variants; those and Mom's pair greeting fixed.
- Birch and Oldale's Mart shopper directed players to the wrong Ball seller.
  Text now points to the Center. Sol's analogous Mart scan found three Rustboro
  shoppers with the same mismatch; their text now identifies Center supplies or
  the Mart's actual held-item role. Sootopolis's distinct supply shop is retained.
- Oldale's promotional tour flag suppressed a gift retry after a full Bag. Returning
  to the employee now retries the gift directly, while the success-only receipt flag
  prevents duplicates. Sol checked nearby gift failures: Mom's Old Rod and Birch's
  Cherish Balls cannot normally overflow their pockets at this opening gate; those
  theoretical paths were not misrepresented as observed player failures.
- Oldale tutorials now explain two healthy doubles leads, battle XP stopping at cap,
  the party Leveler, and free healing/tutoring versus paid shop supplies.
- Running Shoes booklet claimed B walks, contrary to held-B running code. Fixed;
  Sol found no additional active reversed B/run instruction.

## Verification

Native `test/story_opening.c` groups:
- Real assembled region-result script: all9 rows plus B/cancel, correct generation
  and displayed region; downstream approach variable8008 unchanged.
- All9 regions ×6 ordered distinct starter pairs: success, correct species,
  leftover rival choice, opening-state transition and duplicate-grant refusal.
  Same-choice attempts fail without granting either mon.

Both groups PASS. Evidence: `work/story-gate-audit-20260919/final-tests.log`.
Normal release build/gates and artifact receipt are recorded beside that log.
No full campaign/runtime or all-dialogue-fit guarantee is implied.

The standalone older progression verifier is NOT green. It flags two inert
Meteor Falls off-map script0 placeholders (identical to Inclement) and Trainer
Hill reverse exits pointing at the removed Route111 entrance. Sol traced the
latter to intentionally sealing the facility: no reachable incoming warp remains.
Do not reopen it to satisfy a structural check. Legacy saves already inside are a
separate compatibility concern. The verifier's source-reference pass also scans
unassembled `data/scripts/mevent.inc` and reports its removed questionnaire text.
These are recorded qualifications, not silently waived checks or evidence of a
fresh-save story blocker. Current required release gates are a different suite.

## Next gates, still unreviewed

Route104/Woods → Rustboro/Roxanne/Devon/Peeko → Briney/Dewford/
Granite Cave → Slateport/Museum → Mauville/Wally/Wattson → Verdanturf/Tunnel →
Route111/Fallarbor/Meteor Falls → Mt.Chimney/Lavaridge → Norman/Surf → Weather
Institute/Fortree/Winona → Mt.Pyre/Lilycove/hideouts → Mossdeep/space center/Dive →
Seafloor/Sootopolis/Wallace/Sky Pillar → Waterfall/Victory Road/League → postgame,
legendary puzzles and optional-area return branches. Each area includes every
local NPC and re-entry branch, not just its mandatory battle.

## Early battle-item incentive contract

User reaffirmed: early Eviolite/Choice/Focus access must be reflected in later
placements, NPC wording and quest value. Current first Center shop visit grants
one of each not previously acquired (Choice Band, Specs, Scarf, Focus Sash,
Eviolite), then discovery stock sells additional copies. Early held acquisitions
already count. A duplicate can be useful in doubles; it is not a new unlock.
Review rewards individually; do not automatically delete all later copies or add
new overworld placements. Replacements must occupy an original Inclement item
location or gift. Source inventory to revisit at the corresponding story gates:
Dewford Eviolite; Slateport Choice Specs; Lavaridge Center Choice Band; Fortree
House4 and Mart Choice Scarf. Oldale Focus Sash remains an optional free copy;
its current text presents it as a promotion and does not claim a new unlock.
The inventory is a review queue, not a statement that these later rewards have
already been audited or redesigned.

## Petalburg initial-visit pass

Reviewed city transition/on-frame scripts, Gym-boy west-exit diversion, Scott's
four approach branches, Wally tutorial party save/loan/restore and gym return,
Norman's initial directions to Rustboro, both ordinary houses, Center local NPCs,
Mart clerks/shoppers, Wally's parents and town signs. Item roots remain Max Revive
and Venusaurite at existing positions; required release checks cover stone source
uniqueness and original placement. Later Norman/Surf branches are deferred to
that gate, not counted complete by reading this area's opening scripts.

Repairs:
- City full-party capture lesson now explains the actual party-swap/Box choice,
  matching B_CATCH_SWAP_INTO_PARTY and givecaughtmon. No capture behavior changed.
- Wally's parents now use a pre-tutorial branch while gym state<2; the existing
  thanks/left-for-Verdanturf text is reserved for after returning from the lesson.
  Early conversation no longer records the post-tutorial thanks flag.
- Center's personalized starter inference was stale: IsStarterInParty matches
  either partner/evolutions but script selected the first starter index. Replaced
  this duplicated branch tree with a general type lesson that mentions dual types.
- Sol Low fixed poison-travel claims in Petalburg Mart and Rustboro School. Current
  OW_POISON_DAMAGE disables walking damage. Main reviewed these edits; worker
  checked live bindings and changed-text width. No analogous false capture,
  premature Wally completion or personalized type lesson was found in its scope.

Source review and release validation only; no new Wally cinematic replay or
native before/after-parent interaction was performed. Evidence/build logs:
`work/petalburg-audit-20260919`. Continue with Route104 south/Briney/Petalburg Woods,
then Route104 north and Rustboro. Any unresolved choreography doubt needs a
bounded screenshot/scene, not an unqualified source-only acceptance claim.

## Route104 / Petalburg Woods pass

Read initial south-route NPCs and battle bindings, Briney's cottage visibility
(new game hides Briney/Peeko; sailing is revisited after Devon), Woods left/right
Devon encounter and pending Dusk Ball delivery, ordinary Woods NPCs/trainers,
Cut-side Miracle Seed gift, north-route Chesto/White Herb/Candy gifts and Flower
Shop services. Checked eight visible pickup roots against Inclement coordinates;
none moved or added. Audinite remains the existing replacement pickup. Hidden
items and later boat/rival visit still require their chronological return review.

Fixed Woods EXP sign: automatic healthy-party full/half shares and caps replace
uncapped switch-training advice. James/Winston keep their contacts but no longer
promise disabled rematches; James's two identical texts share one body.

Flower Shop's seven-item daily gift formerly committed each successful prefix,
then aborted without setting the daily flag when a later berry failed. A single
special now rolls the entire prefix back on failure; successful delivery still
contains the same random basic berry and six EV berries. Script shows one bundle
receipt and marks the daily flag only after success. Gift berries still grant no
harvest credit. Wailmer Pail's receipt now also checks successful delivery before
setting its flag (defensive handling, not an observed early-game bag overflow).

Native flower-shop regression PASS: each of seven possible failure positions,
complete delivery and rollback preserving a pre-owned berry count. This tests
inventory behavior; it is not a full NPC/UI replay. Test and release logs are in
`work/route104-woods-audit-20260919`. No new screenshot was needed for these
source-proven text/inventory changes. Authored teams/AI and placements unchanged.

Sol's bounded scans found no other active stale XP tutorial. It identified these
later gates for review (not fixed or accepted merely by being listed):
- Weather Institute: four rocks can partially deliver yet record full receipt.
- Late Lucy/Spenser/Greta rewards: Choice item delivery can fail while the encounter
  advances and Caps are delivered; verify full-pocket retry at those gates.
- Sootopolis Kiri: first berry marks daily receipt even when second berry fails.
- Rustboro Roxanne: active post-Gym call promises another Gym battle despite
  disabled rematches. Review with the entire Roxanne/field-unlock handoff next.

Next active area: Rustboro, all buildings/NPCs and Roxanne → Devon → Route116/
Rusturf/Peeko, followed by return-rival/Briney sailing. Later return branches in
reviewed maps are explicitly still pending.

## Midgame Regenerator — user design change

Norman's defeat (Badge5/Surf transition) awards the passive Regenerator Key Item.
An appended item ID preserves all old saved item IDs. Existing post-Norman saves
can claim it by speaking to Norman; a failed Bag delivery can be retried, and an
item already in Bag or PC prevents duplicates. No ground item placement changed.

Before ownership, consumed original held Berries stay absent after ordinary
persistent-party battles. With the tool they return after battle. Non-berry item
restoration is unchanged. A shared restoration policy also covers outgoing
catch-and-swap Pokémon and caught statics. Ordinary theft/Knock Off retains the
existing restoration convention; consumed-after-exchange state takes precedence.
Recycle/Harvest recovery remains live battle behavior. Bug Bite/Pluck/Incinerate
removal is tracked separately using one former padding bit, so those moves do not
newly enable Recycle. Temporary link/recorded/facility/tutorial modes retain their
existing restoration; Retry still reloads the prebattle party snapshot.

Focused native tests PASS: before/after tool, non-berry Focus Sash, Knock Off,
consumed-after-exchange, captured opponent policy, already recovered berry and
facility exemption. Native Bug Bite/Incinerate battles verify the destruction
marker while usedHeldItem remains empty (no new Recycle eligibility). These are
not a full runtime replay of Norman's gift or every possible cross-owner item
transfer chain. Evidence: work/regenerator-20260919. The older unconditional
berry test is updated to require ownership rather than weakening the new rule.

Rustboro continuation: Roxanne's active scene still claimed an Anorith/PokéCall
registration that never actually happened, and promised a disabled Gym rematch.
It now points to Cutter's house and explains Cut's actual license+badge action.
Removed the fake Anorith-registration fanfare/message. Sol found only unused
historical Exploud/Slowbro/Kabutops text elsewhere, not additional active scenes.
The remaining Rustboro/Roxanne/Devon map-by-map audit is still pending.

## Finale S.S. Tidal dialogue/reward pass

Reviewed all cabin and lower-deck interactions, Steven's one-time battle/ticket
receipt retry, Buffel's stage gate, and Birth Island outcome branches. The ship
has five required trainer wins (Lea/Jed share one ID); lower-deck workers and
Garret remain noncombat NPCs. Cabin introductions now acknowledge the Champion
and full doubles challenge while retaining their personalities. No teams or AI
changed. Removed six unreferenced retired battle text blocks and consolidated
identical Cleanup Brothers dialogue.

The former TM49 giver's Rare Candy reward was redundant with the free Leveler.
Replaced that existing gift with one Big Pearl, preserving FLAG_RECEIVED_TM49's
save identity, full-Bag retry and one-time receipt. Dialogue names the new item,
explains selling it toward supplies, and retains the tutor's Snatch direction.
No new overworld pickup, object, or gift location was added. This is source and
compiled-script verification; no visual/playthrough acceptance claimed.

Remaining finale work: targeted native world handoffs and tactical Wally/Buffel/
ship acceptance under restored authored AI; broader NPC/story audit unfinished.
Build evidence: work/finale-coherence-20260919/.

## Fishing gift ownership and nearby dialogue — September 20

Source-traced current Hoenn rod givers: Mom's Running Shoes scene gives Old Rod;
Route114 object20 uses Route114_EventScript_GoodRodFisherman; Mossdeep House3 gives
Super Rod. Good/Super Rod success flags follow successful giveitem; full-Bag
branches retain retry eligibility, decline/repeat branches release normally.
This is gift-flow coverage, not a complete earliest-traversal proof for each map.

The Route114 giver's implementation and five text blocks were incorrectly housed
in Route118/scripts.inc. Moved them to Route114, retaining the actual public map
entrypoint and receipt flag. Renamed only its private labels/text to Route114 and
folded the redundant yes branch into the ordinary fall-through gift path. Item,
object placement, quantity and dialogue content for the rod remain unchanged.

Active Route118 girl still required a Pokemon that knows Surf. Updated her text
to Balance Badge + Surf authorization, explicitly no teaching requirement. Dewford's
existing Eviolite fisherman now explains both-defense support for unevolved mons
and why an extra copy benefits a second doubles partner; keeps the bait flavor.
No new item placement/gift or team/AI change. Nearby Route114-120, Lilycove and
Mossdeep source scan found no further active field-HM teaching contradiction;
remaining move references were battle/Contest/item/travel advice.

Ownership/reference checks and strict release build/gates pass; evidence under
work/wild-audit-20260919/rod-dialogue-release-*. No native visual/interaction replay
was performed for these small script/text edits. Full route/NPC progression and
rod-method species-access timing remain unfinished.

## Route114 and adjacent Fallarbor reward coherence

Reviewed Route114's20current object entries: five active trainer battles, daily
Berry giver, rod giver, Roar gentleman/Poochyena, existing pickups/trees/rocks.
Seven retired actor scripts had no current map or source references (Shane,
Bernie, Claude, Nolan, Tyra, Ivy, Angelina); removed those scripts and their seven
post-battle text blocks. Nancy's historical conversation metadata still references
its script, so that record was not removed in this bounded pass. No actors or
active trainer teams/battles were changed.

Former TM05 reward: Rare Candy -> Big Pearl, same FLAG_RECEIVED_TM05 and full-Bag
retry. The gentleman's dialogue retains his noisy Pokemon and points to Roar at
the Center tutor while explaining the sellable reward. The original separate
Route114 ground Rare Candy was not changed. Daily Berry delivery/randomization/
receipt flag remain intact; its text now explains planting, a daily spare, and
Regenerator Key Item restoration after battle rather than Berry Crush multiplayer.

Nearby scan found two more former-TM Rare Candy gifts duplicating the free
Leveler: Fallarbor Mart now gives a Nugget, and Cozmo's Meteorite exchange gives
a Star Piece. Existing receipt flags, item locations and one-time behavior stay
unchanged. Cozmo still grants the reward successfully before handing over the
Meteorite; removed a redundant second full-Bag check after the successful receipt.
Dialogue identifies the new items and their sale value. No extra Mega Stone or
new gift object was introduced. Routes113/115 had no analogous active gift/HM
teaching contradiction in the bounded scan.

Reference/placement/receipt-order checks and strict release build/gates pass:
work/wild-audit-20260919/route114-rewards-release-*. This is source/compiled-script
coverage, not native visual or full interaction playback. Cozmo's separate Deoxys
form service and the larger route progression graph remain outside this pass.

## Cozmo form service and native party bounds

ChoosePartyMon currently normalizes cancellation to255, which Cozmo already
recognized. The native ChangeMonSpecies API nonetheless indexed the party without
bounds checks: a direct native regression with slotPARTY_SIZE changed the adjacent
party's Pokemon. It now rejects out-of-range slots, NONE/EGG/out-of-range or
unconfigured target species, and empty/Egg source slots. ScriptGetPartyMonSpecies
returnsNONE for an invalid party slot rather than reading outside the party.

Cozmo now checks PARTY_SIZE before Egg/species reads. Its four form branches only
select species and text; a single shared block handles unchanged-form detection,
mutation, message and release. Menu cancellation/unknown choices fall through to
the shared exit. No species options, form-service unlocks, fees or dialogue content
changed. Nearby active form/party callers already validate their selections;
the old Rotom late-check script is unassembled and its live objects are inert.

Native before/after test proves the invalid-slot overwrite is repaired. Five
party/form groups pass together, covering three invalid-slot sentinels, invalid
species, Egg/empty sources, four valid Deoxys forms with relative stat changes and
HP/personality/EXP retention, plus existing Mail/item safeguards. Valid-form fixture
explicitly calculates starting stats before injuring the mon; an initial fixture
omission was corrected without changing the assertions. Script cancellation and
shared text-pointer flow were source-traced/compiled, not a visual interaction
replay. Strict release passes. Evidence: work/ability-audit-20260920/party-form-*.

## Sealed Chamber Regi requirements

The active Regigigas statue initializes three distinct species requirements.
CheckSpeciesInParty previously counted matching party members: three Regice
could substitute for the trio, and an extra Regice alongside the correct trio
could reject it. Each requested slot now counts at most once; empty slots and
Eggs cannot satisfy requirements. The helper retains exact-count semantics over
all three requested variables, and rejects impossible requested counts above
three. No ordered Relicanth/Wailord puzzle or unlock requirements changed.

The native regression failed before the repair and passes afterward: duplicate
substitution, correct trio, extra duplicate, Egg and missing member. Related
Legendary Sign checks use independent species-family/caught-state predicates.
All 18 combined party, legendary pipeline and wild-slot test groups pass.
The Sweet Scent fixture was stale (five water slots and obsolete weights) and
forced an invalid tie index; it now enumerates current weights and both valid
tie choices, retaining legendary-slot and captured-species exclusion checks.
No production encounter odds or AI changed. Strict release build and gates pass,
stamp 340f364292e6. Evidence: work/ability-audit-20260920/regi-party-baseline-*,
regi-scent-build.log, regi-scent-tests.log, regi-scent-release-*.log.
This is native helper/selector and compiled integration evidence, not a full
Sealed Chamber interaction replay or whole-game completion claim.

## Visible legendary result and capture-flag integration

Regigigas, Articuno (Shoal ice room), Zapdos (New Mauville) and Mewtwo
(Altering Cave B1F) still used legacy permanent-defeat object flags, distinct
from their existing Legendary Sign capture flags. Defeating one removed an
uncaught resident permanently, contradicting the shared leave-and-return retry
policy. Their map objects now use the existing EC_CAUGHT flags. Old defeated
flags remain allocated and saved but no longer hide these four residents.
Capture continues to set the ledger and hide flag through the existing capture
hook; the native resume callback removes the captured object.

Four repeated outcome blocks now share EndVisibleLegendaryEncounter: clear the
resume deletion guard, release on capture, otherwise use the existing transient
hide/rest message. Leaving and returning recreates an uncaught object because
no persistent hide flag was set. Player defeat already takes the native whiteout
callback instead of resuming this script. Battle creation, levels, held items,
cry choreography, placement and Regigigas's trio requirement are unchanged.
Darkrai/Pecharunt already use shared shrine retries; Magearna is delivered.
Legacy non-Sign encounters were not changed by this specific flag repair.

A native baseline regression failed on the old Regigigas outcome branch. The
final test executes compiled result opcodes for all four maps and four returned
outcomes; it also resolves each actual map object by script and verifies capture
sets its flag. The initial fixture incorrectly assumed dynamic species graphics;
it now uses script identity (these objects use legacy/Inclement artwork IDs).
All 13 combined party/legendary pipeline groups pass. Evidence:
work/ability-audit-20260920/regigigas-retry-baseline-* and visible-retry-*.
No visual interaction replay was performed; this does not establish full map,
all-legendary progression, encounter difficulty, or campaign acceptance.

Strict release build and release gates pass for stamp 3518e59cde35.

## Route 117 daily gift-egg transaction

The Day Care's first Togepi/Extrasensory and subsequent random-species gift
branches now share one giveegg/move/receipt sequence. The existing party-capacity
check remains before generation. Receipt flags are set only after giveegg returns
MON_GIVEN_TO_PARTY and the selected move is applied; success messages/fanfare then
follow. Declining and full-party branches retain eligibility. The first-egg flag
is harmlessly set on every successful delivery, eliminating a duplicate branch.
No species/move pool, daily interval, NPC placement or party-space requirement
changed. Repeat dialogue now explicitly says the gift was received today and to
return tomorrow. FLAG_DAILY_RECEIVED_DAYCARE_EGG is in the daily flag range and
ClearDailyFlags is called by the existing clock daily update.

Native compiled-opcode coverage exercises14 cases: first/later gift x party
counts0..6. Verify exact firstTogepi/Extrasensory, later rolled species/move, Egg
status, count increment, both receipt flags, and full-party rejection preserving
eligibility. All9 combined campaign-gift/party groups pass. Logs:
work/ability-audit-20260920/daycare-gift-*. This is transaction and source-dialogue
coverage, not a complete Route117 NPC/visual traversal or RTC emulator test.
Route117 girl's learned-move dialogue was checked against daycare's automatic
level-up learning and remains mechanically true. Other route interactions remain
outside this scoped transaction pass.

Strict release build and gates pass for stamp ef63d2dadb80.

## Rusturf Tunnel and adjacent post-opening dialogue

Route116 and Verdanturf tunnel signs now share their existing public entrypoints
and closed text in the tunnel script. FLAG_RUSTURF_TUNNEL_OPENED selects a clear
open-for-travel notice after completion; before completion the canceled-project
notice remains. Route116 rest-house Tunneler2 now branches to the existing
Tunneler3 completed-tunnel interaction after that flag, rather than wondering
whether the boyfriend has broken through. Reused its locking/faceplayer/message
path and existing text. Other nearby town/couple/aunt dialogue already follows
the progression flag or is historically accurate.

Consolidated identical BoyfriendApproachWanda2/3 bodies and WandaExit1/Exit
movement streams with adjacent labels, preserving every public symbol. Direct
comparison against the pre-existing sequences confirms identical command order
for all four labels. No coordinates, frame delays, actor IDs, battle teams,
rewards, licenses or state transitions changed.

Source-traced gate: smashed-rock flags in this map move the tunnel state to4/5;
the frame script grants Strength authorization and concludes by moving the
couple to Wanda's house, setting state6 and FLAG_RUSTURF_TUNNEL_OPENED. Strength
text correctly requires Heat Badge + license and any party. Native visual
choreography/reentry and full regional traversal were not replayed in this pass.
No new native tests for this dialogue/identical-byte consolidation; release
assembly/link checks and existing release gates provide integration evidence.
Logs: work/ability-audit-20260920/rusturf-sign-release-*. Full campaign/core/battle/
wild-distribution acceptance remains incomplete.

Strict release build/gates pass, stamp 9382eadafbdb.

## Poke Vial tier chain and Route133 partner identity

Source-traced charge writers: first Center receipt1, Route111 Chansey reward2,
Route133 reward3. Center/whiteout refills copy the stored maximum. Initial
suspicion that a delayed Route111 reward could downgrade3to2 was invalidated by
the actual Route133 transition gate: it reveals the later nurse at capacity2,
after Route111's nurse has departed, and preserves completed hidden state at3.
The experimental direct-call test deliberately bypassed that gate and therefore
was not evidence of a normal-play defect. Its temporary helper/test were removed;
production reward capacities and ordering remain unchanged.

A native test now executes the assembled Route133 transition: capacity1 hides,
capacity2 reveals, completed capacity3 stays hidden and retains capacity. All7
campaign-gate groups pass (work/ability-audit-20260920/vial-gate-*). This tests the
actual prerequisite rather than inventing an unsupported reward ordering.

Route133's actor graphics, both cries and object identity are Chansey, but three
nurse text pages called it Audino. Corrected those pages and renamed the internal
GuideChansey script/text labels to match the current non-HM-teaching interaction.
No actor/artwork/cry/placement/reward/AI changes. Related route checks remain
source-level rather than visual playback. Route111's scripted HealBall handoff
currently does not test Bag insertion before advancing; quest can use any Heal
Ball, so this is a potential lost-gift/retry issue to inspect separately, not a
proven progression softlock. Full campaign/core/wild/battle audit remains open.

Related chase scan: Route111, Route112, JaggedPass and AshenWoods consistently
use Chansey graphics/cries and Chansey or Blob text; no additional mismatch.

Strict release/gates pass: stamp 57672f75d24e.

## Route111 Heal Ball handoff capacity repair

The initial Chansey chase advanced state0to1 and removed Chansey after giveitem
without checking delivery. A full Ball pocket could lose the promised gift;
this was not a hard quest softlock because Ashen Woods accepts any Heal Ball.
Add a native checkitemspace gate immediately after lockall and before any actor
moves. Full capacity now displays a make-room message and releases all actors,
leaving quest state0 and object flags intact for retry. No new flag/state/item
placement was added, and the successful chase/movement/gift sequence is unchanged.
The locked scene has no intervening inventory mutation before its gift.

Native compiled-opcode tests exercise full pocket, empty slot and partly filled
HealBall stack, verify state/visibility preservation and confirm real AddBagItem
agrees with the accepted/rejected gate. All10 combined gift/campaign-gate groups
pass. Related scan: Route111 daily Berry already checks receipt; Route112 and
JaggedPass chase stages have no gifts; AshenWoods checks HealBall possession before
its locked consumption and state6 commit. No additional analogous defect found.
Evidence work/ability-audit-20260920/heal-ball-*. This verifies the transaction
gate; the visual scene was not replayed and full regional audit is incomplete.

Strict release build and gates pass, stamp 1fd2b81e25a5.

Route113 workshop integration: ash collection now feeds both the restored
spendable shop balance and independent lifetime discovery counter. The glassmaker
now unlocks Marshadow at250 lifetime soot using the existing local discovery API;
Soot Sack receipt failure preserves the initial gift state. See WILD_ENCOUNTER_AUDIT
for native counter/discovery/encounter evidence and the dormant reward-helper
qualification. No new item reward or placement. Strict release stamp54aeec4340da.

## Glass workshop purchase-path consolidation

Seven item/decor branches now select item, price and existing pending-receipt ID,
then share name formatting, affordability, confirmation, debit and crafting flow.
The order ID uses scratch VAR_0x800B until confirmation; the standard yes/no script
only returns VAR_RESULT. Existing saved states10..16, prices250/500/500/1000/1000/
6000/8000 and item/decor IDs are unchanged. Initial delivery now reuses the existing
retry delivery block instead of duplicating it after the crafting animation.
No price, gift, menu order or save-state migration was introduced.

Native compiled-opcode tests exercise all seven selections with insufficient ash,
decline and acceptance (21 cases), then clear scratch vars and reconstruct each
of the seven paid pending receipts through the actual retry dispatcher. Verify
correct item/decor/price, exactly one debit, preserved lifetime soot and no second
debit on retry. All14 combined campaign-gift/legendary pipeline groups pass.
Actual modal menu, fanfare and Bag/PC-full UI were not replayed; tests stop at
those boundaries and preserve the existing delivery implementations. Evidence:
work/ability-audit-20260920/glass-orders-*. Whole-game acceptance remains open.

Strict release build/gates pass, stamp a0124d156b60.

## Fallarbor native Hidden Power service simplification

Read the native EV/IV/nature service implementation and the Fallarbor service
script. Retained its paid EV/BottleCap policies and authored IV spreads. Replaced
the Hidden Power menu's sixteen one-to-one case branches with a bounds check and
copy of VAR_RESULT into the existing type variable8007. Cancellation16/127 still
restores the party slot from800A before returning to the main service menu.
Removed sixteen unreferenced internal dispatch labels; no callers exist outside
the replaced switch. Shared price check, three-Cap debit, stat mutation and native
menu remain unchanged. No AI or trainer-team modification.

All22 Inclement integration native test groups pass. Added compiled selection
coverage for all16 types and both cancellation results. Existing service tests
verify the actual resulting HiddenPower type for every spread, low Attack IVs,
and correct saved party member; EV report, nature and broader HM/Leveler tests
also pass. Evidence work/ability-audit-20260920/stat-menu-*. This is script/native
service coverage, not rendered menu/intro choreography or a full house audit.
ChosenMon's fallback-to-lead policy and malformed-input/over-cap EV cases remain
candidates for a later API-boundary review, not claimed fixed here.

Strict release build and gates pass, stamp f15d15e88c45.

## Meteor Falls / Mt. Chimney source cleanup and receipt narration

Source-traced Meteor Falls victory gate (existing native regression), Archie
sequence and subsequent FLAG_HIDE_ROUTE_112_TEAM_MAGMA/state1 commit. Maxie's
Mt.Chimney win clears the summit factions, returns Cozmo home and enables the
cookie vendor. Meteorite machine requires the summit victory and commits its
one-time receipt only after successful Bag insertion. Cookie vendor checks money
and Bag capacity before its debit; failed delivery does not consume money.
These are source checks, not a fresh native traversal/choreography acceptance.

Removed15 Mt.Chimney unused movement streams (203 lines). Exact-label reference
search across source/scripts/tests found definitions only; JSON search found no
references. Live movement scripts, actors, coordinates and battles are unchanged.
Moved the Meteorite-removal success message after confirmed delivery/receipt.
Moved Cozmo's thank-you after confirmed StarPiece insertion and Meteorite handover,
and changed 'Take this Star Piece' to refer to the already-delivered reward.
Both full-Bag paths retain their existing retry/flags, now without false success
narration. Nearby Galladite/AuroraTicket handoffs already commit on delivery.

No new tests for these reversible unused-code/dialogue changes. Source ordering
assertions and strict release assembly/link/gates verify integration; evidence
work/ability-audit-20260920/chimney-cleanup-release-*. Full core/campaign/battle/
wild audit remains unfinished and these checks do not prove the visual scenes.

Strict release build and gates pass, stamp a6e926d23709.

## Lavaridge rewards and progression source pass

Flannery victory grants Badge4/Strength permission through the existing license
rule, advances Petalburg Gym readiness and starts the rival Goggles scene. Native
text accurately says HeatBadge + Rusturf license + any party. Goggles bundle
preflight and pending state3/reentry/retry remain unchanged. Heatranite retry now
calls the same delivery routine as the victory sequence, preserving the original
TM50 receipt flag, item and Bag-full return/release behavior.

Wynaut's one-time gift now performs giveegg and checks MON_GIVEN_TO_PARTY before
setting FLAG_RECEIVED_LAVARIDGE_EGG or playing receipt text/fanfare. Existing full-
party guard and decline/repeat dialogue remain. This is transaction hardening;
the original party-space precheck already prevented normal capacity failures.
Native compiled-opcode coverage for party counts0..6 verifies Wynaut/Egg status,
count increment, receipt flag and full-party retry. All combined campaign gift/
gate groups pass; evidence work/ability-audit-20260920/lavaridge-receipts-*.
No NPC placement, battle team, item identity or new overworld reward changed.
This pass does not establish all Gym tile/choreography or springs visual behavior.

All12 combined native groups and strict release gates pass, stamp78b89494f26f.

## Desert fossils and Devon revival mapping

Source-traced MirageTower fossil receipt before collapse/choice flags and the
DesertUnderpass opposite-fossil choice with full-Bag retries. Devon stores the
selected fossil item in VAR_WHICH_FOSSIL_REVIVED, advances to ready on transition
or another scientist interaction, and keeps its ready state if party/PC delivery
fails. This source review is not a full desert/tower visual traversal.

Unified the11 accepted fossils and their revived species into one mapping used
by item acceptance, Bag detection and species conversion. Removed the duplicated
item list/switch and the initial unused FossilToSpecies call whose input8008 had
not been initialized; the actual ready-state conversion still reads the saved
fossil explicitly. Invalid conversion retains its previous no-write contract.
Bag-picker cancellation now routes to the existing polite decline instead of
claiming the unselected item is not a fossil. Rewards/levels/flags unchanged.

Native checks enumerate every item ID against the accepted mappings, confirm all
11 revival species, non-fossil rejection and Bag detection across insertion/
removal. All13 combined gift/gate groups pass. Evidence:
work/ability-audit-20260920/fossil-mapping-*. Modal Bag/naming/PC UI was not replayed.
This extends partial campaign and field-specials coverage, not whole-game proof.

Strict release build and gates pass, stamp f9f9ae936b6b.

## New Mauville generator and Wattson reward

Source-traced post-Norman Wattson visibility, BasementKey receipt/failure flag,
Surf access and persistent entrance unlock. Generator stages2/4 stop then3/5
restart; the final Rotom encounter completes only on defeat or capture, retaining
state5 after escape/other returned outcomes. Shared the two completion bodies
while keeping their different messages. Native compiled outcome test covers all
outcomes through FORFEITED and confirms only the intended success branches are
selected; map metatile/visual scripts were not replayed.

Replaced Wattson's existing former-TM24 RareCandy gift with one Nugget, useful for
the retained paid services alongside the current-cap Leveler. Same NPC/location,
receipt flag and full-Bag retry; no new pickup or gift location. Updated dialogue
to describe the Nugget. FLAG_GOT_TM24_FROM_WATTSON still commits after receipt and
remains the prerequisite for Regieleki/Zekrom/Zeraora discoveries; their separate
badge/species conditions are unchanged. No wild roster, Rotom level, team or AI
change. All14 combined native gift/gate groups pass. Evidence:
work/ability-audit-20260920/new-mauville-*. Entire campaign/core/wild scope remains
incomplete; source tracing does not prove collision-level access or choreography.

Strict release build and gates pass, stampbdc858731f71.

## Route118 Magikarp challenge reward retry

The existing Gyaradosite gift set FLAG_ROUTE118_GYARADOSITE even when giveitem
failed, permanently marking an undelivered reward received. It now checks success
before setting the flag. Reinteraction first checks the receipt, then the actual
trainer defeated flag; a winner with an unclaimed gift goes directly to delivery,
without repeating the authored fight or rebuilding a six-Magikarp party. Original
challenge requirements, battle/team and reward/location remain unchanged.

Native compiled checks cover all four defeated/received combinations and both
receipt results, including no flag on failure and completed-repeat routing. The
ordinary no-intro trainer battle does set its defeated flag on victory; source
verified through SetBattledTrainersFlags. All16 combined campaign/gift/Weather
Institute groups pass. Evidence work/ability-audit-20260920/magikarp-reward-*.
No visual interaction or full battle playback was needed for the receipt branch.

Related scan: Weather Institute rock bundle preflights all four item insertions;
Bottle Caps check receipt; Castform preserves its pending gift on party/PC failure.
No additional analogous one-time reward loss was found in the bounded area scan.
Broader route traversal, wild distribution and core/battle audits remain unfinished.

Strict release build and gates pass, stampcb8d651b349c.

## Fortree services and Devon Scope receipt

Route120's Scope handoff previously set its receipt flag and removed Steven even
if giveitem failed. Add capacity preflight before the Kecleon scene begins, with
an explicit KeyItems-space retry message. The intervening scene has no KeyItems
insertion: Kecleon holds NONE and grants no key/relic, and normal battle cannot
consume that reserved pocket space. Loss exits before delivery and retries the
preflight. Item-based Fortree/Kecleon blockers and flag-based bridge/story gates
therefore remain aligned. No extra saved state or placement was introduced.
Native compiled preflight covers full/open pocket, unchanged receipt flag and
agreement with real Bag insertion; all16 combined gift/gate groups pass.

Fortree House2's formerTM10 coin-game gift is now Nugget and its formerTM49 gift
BigPearl, replacing redundant RareCandies at the same existing NPCs/flags. Removed
the false promise to teach SleepTalk from an NPC who only hands over a reward;
updated prize descriptions and decline text. HiddenPower advice still points to
the universal tutor. No added overworld item, altered challenge sequence, battle
team or AI. Gift receipt guards remain before their flags.

Evidence work/ability-audit-20260920/fortree-coherence-*. Source/compiled checks,
not native visual Kecleon/Gym traversal. Whole core/campaign/battle/wild acceptance
remains incomplete; key-pocket failure is a guarded capacity case rather than
proof that a typical five-badge player naturally fills the KeyItems pocket.

Strict release build and gates pass, stampfa6af90990d8.

## Route123 gift and retired-script cleanup

Replaced the existing formerTM19 RareCandy with a StarPiece, preserving the
Grass-type party requirement, receipt flag, NPC location and full-Bag retry.
Updated the friendship-gift text to identify the item and its sale value. The
native IsGrassTypeInParty already excludes Eggs and empty slots; no change needed.
Removed seven retired NPC entrypoints (Violet, Cameron, Jacki, Alberto, Ed, Davis,
Fernando) after confirming no map, cross-script, source or tooling references.
Kindra/Jonas and other referenced entries remain. Authored battles and map objects
are unchanged; no new pickup or gift placement.

Mt.Pyre Emblem handoff was source-checked: inventory receipt precedes the orb/guard
flags, and the elder retries the gift while its receipt flag is unset after the
story state advances. No additional failure-path defect found there in this pass.
No new native tests for reversible dialogue/dead-entrypoint cleanup; integration
is checked by strict release assembly/link and existing gates. Evidence:
work/ability-audit-20260920/route123-cleanup-release-*. This is partial source
coverage, not route traversal or all Mt.Pyre/story/legendary acceptance.

Strict release build and gates pass, stamp266f1bcf7ea5.

## Lilycove lottery prize contract repair

The restored live clerk gives VAR_0x8005, but a superseded Circuit-lottery change
removed the C prize assignment. Native baseline: a two-digit winning ticket kept
a previous Potion value instead of PPUp. Restored the exact local Inclement donor
table (inclement-game-source/src/lottery_corner.c): PPUp, BottleCap, MaxRevive,
MasterBall for two through five matched digits. Initialize prize/location scratch
outputs before scanning; assign the prize for the selected tier. Digit comparison
now uses local arithmetic rather than two EWRAM scratch globals. Daily RNG, match
selection/tie behavior and the live clerk's saved pending-prize flow are unchanged.

Removed the obsolete Circuit-end write to FLAG_EC_LOTTERY_TICKET_READY: the current
daily clerk never consumes it. Retained the flag constant/bit as reserved for save
compatibility, with an accurate comment. Source review confirms pending prizes
are checked before daily lockout, survive daily number refresh, and generate TV
reports only after delivery. No replacement reward policy was invented; the donor
prize contract was restored instead of using the older Ether/LinkingCord variant.

Native tests cover all four tiers, no/one-digit nonwins, low16-bit trainer IDs,
leading zeros, party/PC selection and Egg exclusion. Valid test mons are created
with preset OT IDs rather than changing encrypted identity keys after creation.
All25 Inclement integration groups pass. Evidence work/ability-audit-20260920/
lottery-baseline-* and lottery-*. The full clerk UI/overnight clock was source-
traced, not replayed. Larger campaign/core/battle/wild acceptance remains open.

Strict release build and gates pass, stampdc80274580cb.

## Lilycove gifts and vending payment consolidation

House2's formerRest-TM gift is now Pearl; rooftop formerSubstitute-TM gift is
Nugget. Same NPCs, locations, receipt flags and Bag-full checks; updated item text.
These replace redundant RareCandies alongside Leveler without adding placements.
Altarianite gift source already checks delivery before its flag and retains its
Altaria condition; no change was needed there.

The rooftop vending selections now assign item and price, then share existing
variable-cost money specials instead of six literal-price check/debit helpers.
Removed the obsolete menu-index scratch copy. Prices200/300/350, capacity check
before charging, dispensing messages and both conditional1/32 bonus rolls are
unchanged. Native compiled tests cover all three drinks with insufficient money,
full pocket and exactly enough money: failed purchases do not charge; success
charges the correct selected amount. Existing gift/gate checks also pass.
Evidence work/ability-audit-20260920/lilycove-vending-*. Dispensing/bonus UI was
source-compared, not visually replayed. Full game audit remains incomplete.

All17 combined native groups and strict release gates pass, stampfd8c2d2b31e9.

### Obsolete soot reward implementation retired

Confirmed no event-script callers for ClaimEmeraldChampionsSootMilestone or
BufferEmeraldChampionsSootProgress. Removed both implementations, unused threshold
constants and obsolete test/verifier assumptions. Retained special-table positions
and legacy saved receipt bit. Active workshop currency/discovery code is unchanged.
Strict release gates pass (528da81400a6); all99 finite Mega sources still verified.
The old headless research scene13 fixture is not current workshop evidence.

## Aqua Hideout source consolidation and gate trace

Traced entrance hints through Orb/Groudon state and Slateport theft, which hides
both entrance blockers. Matt victory's escape script sets the shared flag that
stops Lilycove's Wailmer collision tiles from being installed and hides city Aqua
NPCs. All eight grunt entries still have map objects; retained their authored
battles. Grunt6's MasterBall clue matches B1F's actual pickup. The former DarkPulse
TM object gives Barbaracite; Sharpedonite and remaining original pickups retained.

Both Electrode objects now call one identical setup/battle routine, keeping their
separate completion flags/outcome branches. Inlining it reproduced the original
entire script exactly. ScriptContext_Stop preserves the call stack, and non-loss
scripted wild outcomes resume it; loss whiteouts. Removed the unreferenced right-
travel submarine movement and unused B2F MasterBall script (no placed item removed).
Strict release build/gates pass, stamp4e3db8cf1c42; aqua-cleanup-release-* logs under
work/ability-audit-20260920. No visual traversal or native encounter replay in this
pass. Full progression, battle, wild distribution and core audit remain unfinished.


## Mossdeep post-invasion dialogue repair

Native baseline reproduced SpaceCenter civilian dialogue staying in invasion mode
at city state3 until League completion. Victory sets3 and removes Magma immediately.
Five 1F/2F gates now select invasion dialogue only at2 (retaining the existing
League override), removing duplicate comparisons. Related read-only Sol scan found
the outdoor Sailor waiting for Dive receipt; main verified the reachable interval
before Steven's house and added citystate>=3 as a completed-crisis condition.

Two compiled-script tests cover40 civilian combinations and8 Sailor combinations,
including before/during/after invasion and League/Dive flags. All11 campaign-gate
groups pass. Strict release build/gates pass, stampc36d17e01c91; mossdeep-dialogue-*
logs under work/ability-audit-20260920 include failing native baseline. No changes
to authored battles/AI, choreography, rewards or Dive requirements. Source trace
confirms Steven sets HM08 alias of Dive license; field movement checks that plus
MindBadge and a valid Dive warp. No visual scene/underwater traversal in this pass.
Full core, campaign, battle and wild-distribution audit remains unfinished.


## Route124 shard exchange review and consolidation

The initial separate-pocket loss suspicion was DISPROVED. Runtime GetItemPocket
keeps shards and BottleCaps in Items; removing the last shard legitimately frees
one reward slot. Initial native fixture overwrote the shard while filling that
same pocket, so its failure is not game-bug evidence. Corrected native test covers
four colors x quantities1/2 x full/available reward capacity, executing the compiled
capacity branch and actual remove/add APIs; all16 cases pass before and after.
Keep this valid fallback. Related scan also retracted its Shoal same-pocket claim:
ShellBell maps to Battle and Slowbronite to Mega, so independent checks are valid.

Consolidated four BottleCap assignments into the shared transaction, removed an
unused item-name buffer and15 redundant Exit cases. Existing default branches
handle Exit/B. Exhaustive source-dispatch comparison across15menus x256results
preserves destinations. All9 campaign-gift native groups pass. Evidence under
work/ability-audit-20260920/shard-* includes invalid initial fixture and corrected
baseline; do not cite the initial failure as a production defect. Full game audit
remains unfinished. This pass did not replay the modal exchange UI or Dive travel.

Strict release build/gates pass, stamp69fd5ae37d4d.

## Seafloor awakening / Sootopolis authorization pass

Read SeafloorRoom9 scene/retry and Route128 departure sequence, tracing completion
flags into Sootopolis crisis/weather and Wallace's later Waterfall handoff. Archie
battle remains authored doubles; scene completion commits Seafloor1/Route1281/
Sootopolis1 plus crisis actors/weather, and Route128 clears popup suppression and
advances to2 after Steven directs the player to Sootopolis. Source trace only for
full cinematics; no visual route replay or claim of complete Sootopolis coverage.

Native compiled MAP_SCRIPT_ON_TRANSITION callbacks verify unfinished Groudon and
Kyogre scenes unhide their sleeping objects on reentry; completed state leaves
those hide flags intact without altering progress. All12 campaign-gate groups
pass. Addobject is transient and does not clear the awake actors' persistent hide
flags; this supports retry logic, but actual sprite choreography remains unplayed.
Removed four movement blocks with no data/source/tool/test references (two each
SeafloorRoom9/Route128). Live movements, objects and trainer teams are unchanged.
Wallace now explicitly authorizes Waterfall instead of describing a physical gift;
existing next text still explains RainBadge/pressA/anyteam. No new item placements.
Evidence work/ability-audit-20260920/awakening-gates-*. Full core/campaign/battle/wild
objective remains unfinished; next broader progression follows Sootopolis/SkyPillar.

Strict release build/gates pass, stampb192ab1e6eaa.

## Sootopolis crisis-state branch consolidation

Source-traced CaveOfOrigin Wallace's correct-answer flag/state3, SkyPillar door
and outside scene advancing4, summit awakening advancing city5/pillar1, and the
Sootopolis layout/spectator callbacks consuming those states. Kept actor movement,
Rayquaza battle/capture behavior, trainer teams and unlock policy unchanged.

Replaced five repeated spectator-state calls with one guarded helper covering
states1..5; replaced layout's four identical state branches with one <=4 check
after existing0/>=6 exclusions. Native baseline and after tests cover32city/pillar
combinations starting from the normal map layout. A second compiled dispatch test
covers8spectator states, stopping before actual object movement. All14 campaign
gate groups pass. No new production bug established; this is simpler equivalent
branching and stronger state coverage, not visual/collision/save-reentry proof.
Evidence work/ability-audit-20260920/sootopolis-layout-*. Full Sootopolis NPCs,
SkyPillar encounter/retry and larger campaign/core/battle/wild scope remain open.

Strict release build/gates pass, stampf013d6d6f648.

## Kiri daily Berry bundle delivery repair

Source review found Kiri sets her daily receipt after Berry1, then may fail to
give Berry2. A full pocket with room only for the first loses the second that day.
She now chooses both original rewards (same first range and Figy/Iapapa choice),
uses existing CanReceiveBerryPair before either delivery, shares one delivery path,
and sets the same daily flag only after both gifts. No new flags, placements or
reward policy. Failed preflight preserves the entire gift for another attempt;
random selection now precedes both deliveries rather than occurring between them.

Native compiled preflight checks enumerate every firstBerry/secondBerry pair with
0/1/2 free slots, verify no inventory/receipt mutation on failure and agree with
actual AddBagItem for accepted pairs. This does not replay both item-popup dialogs.
Read-only related scan found Route123/FlowerShop already protect their bundles.
Reviewed worker's only production edit: BerryBlender's two sparePecha paths now
check delivery success before daily receipt. Those are defensive, not a normal
full-pocket exploit: PlayerHasBerries(FALSE) means an empty effective Berry pocket.
Four compiled result-gate cases cover those receipt checks. Main exported one
existing continuation label for linking the test. Final build forced script
reassembly after worker edits; earlier intermediate build was not used as evidence.

All11 campaign-gift groups pass. Evidence work/ability-audit-20260920/daily-berry-*.
Full Sootopolis NPC/progression/core/battle/wild objective remains unfinished.

Strict release build/gates pass, stamp504b1bd90887.

## Size-contest calculation cleanup and formatting leak fix

Read pokemon_size_record.c and both Sootopolis contest scripts. Replaced opaque
size-table fields and negative u16 initializer spellings with explicit range names
and thresholds. Range search now includes all16ranges and uses bounded32-bit
arithmetic, removing the separate index helper, redundant wide locals and float
round-trip. Exhaustive comparison of all65536hashes against the original unsigned
64-bit arithmetic is identical (the old skipped penultimate range had the same
slope, so it was not itself an observed sizing bug). Maximum u16species height
multiplication stays within32bits; underlying height field verifiedu16.

Native contest test then exposed an actual existing leak: six16-byte height-string
allocations were not freed. Combined the formatter/trim helper, copy from the first
digit and Free the original allocation base. Related read-only scan confirms all
other Pokedex height/weight callers already free their strings. No reward policy,
record hash, species eligibility, measurement display units or save schema changed.

All12 campaign-gift groups pass with native Seedot/Lotad record updates, equality,
Egg/cancel rejection and actual compiled failed-Elixir rollback/retry. Full Bag gift
UI was source-traced, not replayed. Evidence work/ability-audit-20260920/
pokemon-size-before.c, pokemon-size-parity.log and size-record-*; initial native
run reported six16-byte leaks before the formatter fix. Source removes30net lines.
Full Sootopolis/core/campaign/battle/wild audit remains unfinished.

Strict release build/gates pass, stamp1755207df69a.

## Juan reward retry and ice-stair consolidation

Source review confirms victory awards RainBadge/progression before Feraligite, and
unreceived-stone conversation retries independently of battle. Badge+Waterfall
license text matches field rules; missingFortreeBadge reminder retained for the
supported out-of-order badge path. No new gift/battle or progression policy change.

Retry now calls the existing gift subroutine, whose full-Bag path returns to the
caller; both success and failure then release as before. Three byte-for-byte
identical ice-stair scripts share adjacent public labels and one command sequence.
Original8/28/67 triggers, increment/delay/sound/stair update and drawing unchanged.

Native compiled retry test executes actual AddBagItem and gift routing: fullMega
pocket keeps no Feraligite, receiptfalse and RainBadge true; freeing one slot then
adds Feraligite. Success stops before fanfare/UI, so receipt commit after messages
is source-traced, not replayed. All13 gift groups pass. Evidence under
work/ability-audit-20260920/juan-gift-*. Ice movement/falling visuals and complete
Gym/campaign/core/battle/wild acceptance remain outside this scoped pass.

Strict release build/gates pass, stamp07fde5dbfc60.

## Explicit first League admission requirement

Native predicate baseline accepts a Winona-only badge mask because donor guards
assumed the other7badges were enforced by prior story. First admission now checks
all8 explicitly, matching its NPC text. Classified as latent robustness: no normal
campaign path with missing mandatory badges was demonstrated. Existing ENTERED
shortcut precedes the check and is retained; HallOfFame resets don't clear it,
so repeat League access/finale flow remain unchanged. No authored team/AI changes.

Native compiled gate enumerates all256badge masks; only255 is admitted. Combined
campaign/finale suite passes17groups, including earned finale steps/ship wins and
protected AI-information flags. Read-only caller scan found no competing active
all-badges admission gate. Removed an unreferenced alternate guard-movement prefix
while retaining its shared Champion continuation, and an unused legendary-team
prompt. Evidence work/ability-audit-20260920/league-badges-* includes failing native
baseline. Guard walking/UI and full League/expedition battles were not replayed.
Full core/campaign/battle/wild objective remains unfinished.

Strict release build/gates pass, stamp071b3b7c5d1d.

## Campaign story native-module consolidation

Combined emerald_champions_opening.c and emerald_champions_finale.c into
src/emerald_champions_story.c, removing one engine source file. Opening/rescue/
regional-rival and finale function/data bodies retained byte-for-byte; only the
combined include set and filename changed. Public opening header/function names
remain stable. Updated catalogue exporter source paths and current code inventory/
progress list. Original49core audit modules now occupy48files; top-level engine
C count394. Historical evidence references remain historical, not rewritten.

All4 native opening/finale groups pass, including every region's distinct starter
pairs and earned finale milestones/AI information flags. Trainer-reference host
test passes; actual catalogue opening/rival/rescue extraction resolves new source.
Makefile wildcard discovery links the new module without duplicate old objects.
Source trace also confirms first-clear invitation checks SYS_GAME_CLEAR before
GameClear sets it; earlier setup sets IS_CHAMPION, a separate flag. No new scene,
reward, battle, AI or save-layout behavior. Evidence work/ability-audit-20260920/
story-module-*; parity log stores both unchanged body hashes.

Strict release build/gates pass, stampeeff68834885. Full core/campaign/battle/wild objective remains
unfinished; consolidation is not comprehensive behavioral acceptance of the module.


## Ferry menu current-ticket integration repair

Strict release build/gates pass, stampe3e8558cc0aa.

Native baseline reproduced Steven's delivered AuroraTicket + received/enabled flags
missing from the dynamic ferry menu because it required an obsolete EC_EARNED flag
with no live producer. Same mismatch affected current Eon/OldSeaMap grants. Important
scope: single-ticket first-time scripts can board directly, masking the bug; repeat
and multiple-ticket menus lose the destinations. New builder requires enabled flag
plus actual Bag ticket OR the existing legacy earned flag. Mystic remains physical
Bag-ticket-only. BirthIsland's finale-stage gate remains in boarding scripts.

Separated list construction from rendering and consolidated four repeated ticket
branches in one ordered table. Preserved Slateport/Frontier conditions, first-time
shown-flag writes, enum order, Exit/cancel and full-list scroll handoff. TESTING-only
wrapper exposes the actual list for native tests without creating UI windows.
Removed unused harbor VAR_TEMP_A mask/5helpers and identical NoEventTickets branch;
remaining first-time dialogue state computation unchanged. Removed duplicated menu construction and obsolete script computation.

All19 campaign/finale groups pass: real reward-state regression,128ticket-mode/
ownership/enabled/shown/legacy combinations, destination result mapping/cancel,
full7-option order and existing progression/AI flags. No visual ferry UI or boarding
travel replay. Evidence work/ability-audit-20260920/ferry-ticket-* includes baseline.
Related-scan correction: OldSeaMap pickup already persists its object flag through
RemoveObjectEventByLocalIdAndMap; no duplicate-receipt fix needed. Dad's both-full
SSTicket path continues with earned registration and later Center delivery; it does
not block the scene. Full campaign/core/battle/wild objective remains unfinished.


## S.S.Tidal campaign cabin cleanup and cruise boundary checks

Strict release build/gates pass, stamp47028fe141ec.

Read cabin/lower-deck/corridor battle and healing flow. Active rooms contain
Colton/Micah/Thomas/Naomi and shared Lea+Jed battle ID, matching the5victories used
by finale progress. Bed calls standard outside-Center healing and advances the
existing voyage state; landing does not remove the ship's rooms/battle flags.
BigPearl formerTM gift retains delivery check before receipt. No team/AI/reward or
healing rule change. Three retired trainer scripts have no object/caller references:
removed Garret and Phillip/Leonard remnants and their unused text. LowerDeck keeps
its required empty map-script table; no map, object or item placement was removed.

Native test covers inactive/active/reset cruise counting across all205steps plus
12east/west route-boundary coordinate cases. Existing finale/ferry cases retained;
all5finale groups pass. These exercise native counters/location conversion, not
visible porthole scenery, cabin navigation, healing animation or tactical battles.
Evidence work/ability-audit-20260920/voyage-cleanup-*. No new functional defect was
established in this pass. Full core/campaign/battle/wild objective remains open.


## BirthIsland puzzle step saturation and outcome routing

Native baseline reproduced step99->0 on the100th step. Because puzzle interactions
compare the counter against3..8-step maxima, extra walking could wrap an invalid
route into an accepted one. Counter now saturates99 with a widened increment;
interaction still resets0 and all authored path limits/rock coordinates remain.
Native checks cover205steps on both Hoenn/FRLG map IDs, invalid65535 normalization
and no off-island increment. Related scan found no analogous required fix: other
puzzles use bounded geometry/bitsets or deliberately cyclic non-success counters.

Added compiled encounter outcome routing/reentry checks for outcomes1..10: only
win/capture set finale resolution; caught and defeated remain distinct; other
outcomes reopen/reset the triangle on map reentry, including clearing COMPLETE.
Loss routing is a controlled script test, not proof the loss callback reaches this
branch (normal battle loss whiteouts). Consolidated three identical puzzle-release
entrypoints under adjacent labels; no actor movement or battle-team changes.
All7finale groups pass. Strict release build/gates pass, stamp6ab4ca8bee23.
Evidence work/ability-audit-20260920/deoxys-steps-baseline-* and deoxys-puzzle-*.
No physical triangle walking/visual awakening/battle replay; full core/campaign/
battle/wild objective remains unfinished.


## Buffel final-trial dialogue and gate check

Strict release build/gates pass, stampd5bd37b3baee.

Reviewed the motel's finale interaction. Updated Buffel's introduction to recognize
the completed League/Wally/voyage/Steven/BirthIsland path and present the final
DoubleBattle, replacing casual creator-cameo text. Removed the unreferenced old
come-back-tomorrow rematch promise; corrected battle-intro capitalization. Actual
battle/team/AI, decline behavior, progression derivation and completion text remain
unchanged. The intro is shown only before the first eligible Buffel battle.

Native compiled gate checks both unresolved/resolved Deoxys and undefeated/defeated
Buffel states with prior expedition victories earned. Incomplete expedition routes
to shared directions; eligible undefeated player reaches invitation; already-won
state reaches completion dialogue without a repeat battle. All8finale groups pass.
Evidence work/ability-audit-20260920/buffel-gate-*. Text rendering is covered by
release width checks, not a visual motel replay; no tactical boss playtest here.
Full core/campaign/battle/wild objective remains unfinished.


## Sept 22 evening — whole-campaign route-by-route dialogue audit (Claude)

Four read-only segment audits (opening→Slateport, Mauville→Norman,
Fortree→Mossdeep, Seafloor→postgame incl. Frontier text; reports in
`work/claude-20260922/dialogue-audit/`). Applied, then full suite green
(1635 tests, 1626 pass, 9 expected-failing, 0 failing):
- Script logic: Roxanne re-armed the Rustboro rival triggers after Route 104
  (invisible-rival replay); Lavaridge goggles full-Bag path left the rival
  theme as map music; Groudon/Kyogre/Jirachi knockouts removed them for good
  (only a capture now resolves them, like every other static legendary);
  Magma Hideout left Courtney visible after the fade; Fossil Maniac now
  recognizes every fossil; Wally's Match-Call-style PokéNav call removed
  (script, step trigger and function); Route 109 printed Briney's question
  twice; Southern Island stone full-Bag path explained and releases all.
- Frontier: Exchange Corner clerks sold other items than listed (Quick Ball
  row gave Protein; Linking Cord row charged 48 BP for Leftovers) — rebuilt
  both clerks on one confirm path; Frontier Brains could only appear in
  Singles (now Doubles too, never Multis); Scott's shields read the closed
  Singles streak and his symbol Berries required the closed Arena; Maniac
  tips remap closed topics and read Doubles streaks; "inelegible ineligible"
  (22 lines); Frontier Pass descriptions advertised the Circuit; Tent party
  size, Center-preset and closed-Tent texts; Factory counter lock/face.
- Wrong guidance: Groudon/Kyogre/Mewtwo/Poipole hints gave the wrong unlock
  (Hall of Fame); island-ticket and Diancite hints pointed to nothing; Lottery
  rule; Altarianite rematch promise; Maxie's line (wrong and three lines in a
  two-line box); Stern's Dive line; Dream Ball Hidden Ability claim; Max Repel
  vs Repel Spray; Secret Power; Mega Ring before owning one; Route 111 Trainer
  Hill sign; Fallarbor Tent NPCs/sign; Moomoo Milk price; Aerodactylite loss
  warning; Knuckle Badge now names Flash; Rustboro boy's doubles tip.
- Text: ALL-CAPS regression from restored vanilla text (trainer text file,
  TV/Apprentice/Tent/cable club/tutors/Birch intro and more) normalized to
  Inclement's exact casing where Inclement has the line, else title case with
  acronyms kept; typos and old item names; Diancite/Birch-lab boxes that pushed
  a third line out of the window; genuinely over-wide lines reflowed.
- Consistency: Mossdeep's former-TM Rare Candy gift → Big Pearl (ledger policy).
- Dead content: 1,377 unreferenced script/text/movement blocks removed
  (`work/claude-20260922/rmdeadlabels.py`: only data blocks or code after an
  unconditional end/return/goto, to a fixpoint; build, lock check and suite
  confirm nothing referenced them).
Held for the user: remaining PokéNav story calls (Dad on Briney's first sail,
Roxanne, Scott ×2, rival Rayquaza); Gabby & Ty's endlessly re-arming battle;
~21 former-TM Rare Candy gifts (economy); Norman's fixed Hoenn starter stones;
Ranking Hall still shows Singles records.
- Legendary outcomes: Regirock/Regice/Registeel, Latis, Lugia, Ho-Oh, Heatran
  and Moltres treated any unexpected battle end (e.g. a wild Roar) as a
  capture. All eight now resolve only on `B_OUTCOME_CAUGHT`; everything else
  retreats for a retry. Each Regi capture now runs the three-ruins progress
  (next ruin, or the completion and Regigigas lore once all three are caught),
  which previously ran only after puzzles and so could never complete.
- Sootopolis: Steven's Cave of Origin walk scripted only the east and south
  approaches; the reachable north and west ones skipped the lead-in and played
  the long walk from the wrong tiles. Added both (player steps clear, then
  trails Steven one tile behind). Native: all four approaches end on (16,8)
  with the player behind Steven (`work/claude-20260922/sooto/`).
- Finale/League segment: S.S. Tidal cabin trainers Micah, Jed and Lea had
  sight ranges; their finale-stage check stops the engine's trainer dry-run,
  so they approached on every sighting even after defeat (now range 0 like
  Colton/Thomas/Naomi). Rayquaza's Sky Pillar knockout now retreats like every
  other legendary. Scott's boat speech no longer assumes Wally is beaten;
  post-League Wally has his own line; ferry/ticket texts, Thomas, Wallace's
  Diancie-room lines fixed; stray `waitfanfare` removed. Suite and release
  gates pass (stamp 00f7f22f94f1).
Open design questions are collected in `docs/OPEN_DECISIONS.md`.

## September 23 — HM-system cross-cutting audit

Traced the eight license gifts against their matching Badges, central species
selector, obstacle/terrain scripts, Fly Beacon party/PC lookup, Flash map entry,
and the separate Dig/Regi requirements. Native scenes exercised each HM action
type and the blocked Cut/Surf/Dive/Waterfall branches; full results and the
remaining map-by-map and follower coverage boundary are in
`docs/FIELD_MOVE_AUDIT.md`. Corrected stale gift/Gym/route dialogue, Steven's
surface instruction, the Strength visual fixture, restored Cut-on-grass and
its Faraway Island response, and the Fly-map allocation
failure. License flags are gifts rather than Bag items, so full-Bag retry does
not apply to these eight unlocks. This cross-cutting pass does not replace the
gate-by-gate earned campaign traversal in the table above.
