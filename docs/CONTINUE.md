## September 21 — item-effect parameter lookup

Replaced GetItemEffectParamOffset's impossible NULL guard and redundant pointer
with direct selection of normal or per-battler Enigma data, returning zero when
absent. Native combined-effect fixture then reproduced another defect: HP EV
before HP healing returned offset6 instead of7. The scan tested the entire
remaining flag mask rather than its current bit; corrected all four parameter
match checks to the shifted requested bit. Normal Potion/Full Restore offsets
remain unchanged; no AI strategy or authored item list changed. Current battle
item callers already skip NULL effect data.

Ten Bag groups pass. Additional focused checks cover combined HP/Attack EV,
healing, PP, Defense and Speed parameter positions across all four battlers.
Evidence: work/item-effects-20260921/{before-offset-tests,after-tests,packed-tests}.log.
This covers decoding, not a broad new trainer-item battle benchmark.

## September 21 — evolution condition costs

DoesMonMeetAdditionalConditions committed held/Bag costs inside its condition
loop, before checking later failures, and repeated an earlier Bag cost on each
subsequent condition. Moved consumption after successful validation, once only;
failed conditions also no longer disable cancellation. CHECK_EVO remains free of
item mutations while reporting cancellation eligibility on success. Native test
combines held item, one Bag cost and a later friendship requirement in four
preview/commit and pass/fail cases. Existing configured Bag-cost evolution is
Shedinja with one condition; this fixes the general condition contract without
adding/changing any evolution recipe or authored battle behavior.
Evidence: work/evolution-costs-20260921/tests.log includes campaign and Ultra Beast
coverage. Next concrete source issue: GetItemEffectParamOffset has an impossible
NULL guard (`temp != NULL && !temp`); trace callers and item-effect contracts.

## September 21 — regional lookup table consistency

Corrected the previous Kanto audit's physical-bounds claim: its table had one
unused zero-filled trailing entry, so the old count-sentinel lookup reached
padding rather than outside the allocation. The sentinel is still not a valid
Dex entry. Removed that padding and made both reverse regional lookups use
ARRAY_COUNT with a direct bounded search. No valid conversion changed.
Native tests round-trip every Kanto/Hoenn entry and reject NONE, a nonmember
and an oversized National Dex number. Six native groups pass:
work/dex-conversion-20260921/tests.log.

## September 21 — regional conversion and Birch rating bounds

KantoToNationalOrder accepted KANTO_DEX_COUNT, one beyond its final real entry
(the table had zero-filled padding); corrected its logical upper bound. Birch
rating now iterates only real regional entries,
keeps National Dex IDs separate from species IDs for caught-flag lookup, prevents
count subtraction underflow and clamps before scaling into its 21-text table.
Normal regional/mythical policy is unchanged. Five native groups pass, including
conversion endpoints/sentinel, zero/stale counts with a caught mythical, complete
regional collection, flag bounds and credits selection. Evidence:
work/pokedex-rating-20260921/tests.log.

Traced rating script through regional counts and optional national count display;
trainer stars use regional completion, while the diploma checks national
completion separately. Missing legendary acquisition sources remain unresolved;
no completion exemption was added to conceal them.

## September 21 — Pokédex flag number bounds

GetSetPokedexFlag previously decremented/indexed its argument without validating
National Dex range. Zero/oversized input could read or write outside dexSeen or
dexCaught. ReturnFALSE before indexing for NONE or values aboveNATIONAL_DEX_COUNT;
valid behavior and completion policy unchanged. Four native new_game_state groups
pass, including invalid0/count+1/0xFFFF get/set operations, independent seen/caught
flags at both valid endpoints, and the credits selection boundary cases.
Evidence: work/pokedex-bounds-20260921/tests.log. Pokédex source review is partial;
completion-policy/acquisition coherence is not established by this bounds test.

## September 21 — active credits selection boundary

Overworld scrolling credits are FRLG-only; traced current HoF exit into Emerald
credits.c and its eventual soft reset. DeterminePokemonToShow used a do/while
and Random()%numCaughtMon even when no catches were recorded. Seed its temporary
selection list with the chosen starter only when empty; ordinary caught-list
sampling remains unchanged. This also prevents zero-entry repeat indexing.

Native tests cover0,1,100 caught entries: all71 slides are valid national IDs,
belong to the caught list or chosen starter, and end with the starter. All three
new_game_state groups pass (including preexisting input/new-game checks).
Evidence: work/credits-selection-20260921/tests.log. No actual user save modified.
Full rendered credits and HoF-to-Continue playback remain unverified; source
selection coverage does not establish visual acceptance.

## September 21 — overworld music/ambient review

Fixed UpdateAmbientCry's Swarm loop: it tested Egg status at slot i but repeatedly
read ability from slot0. Now reads the same slot's ability. Native fixture covers
all six slots, Egg exclusion and no-Swarm control: repeat delay600-1199 versus
1200-2399 frames. Existing object movement fixtures also pass; evidence under
work/overworld-audio-20260921/tests.log. Encounter odds/teams/AI unchanged.

Removed the empty sNightMusicTable and identity remap helper. It indexed track
minus START_MUS without bounds, despite MUS_NONE/special values reaching callers.
There were no configured substitutions to preserve. GetLocationMusic's separate
map-header nightMusic selection remains supported; current map JSON defines no
explicit night tracks. Weather-crisis, invasion, Surf/underwater and Route118
track choices remain unchanged. Source/build/release checked, not a new audible
playthrough. Source coverage extended through Flash defaults and music/cry paths.

## September 21 — local callbacks and palette comparison

Reviewed local input lock dispatch, follower standing animation, minute-based
time updates, palette filtering and field callback completion. Follower respawn
failure paths clear in-progress state before normal input uses the follower ID;
no new lock-state defect confirmed. Simplified RunFieldCallback nesting while
retaining FALSE/pending behavior, secondary callback priority and clearing on
completion. Replaced three u32 pointer-cast comparisons of TimeBlendSettings with
memcmp over its complete current12-byte structure (two32-bit BlendSettings and
two16-bit weights); same bytes compared, no type-punning or hardcoded word count.
Source/build/release validation, not a new physical palette-transition session.
Evidence: work/overworld-callbacks-20260921/.

## September 21 — overworld template/camera source review

Reviewed template loading and Continue script-pointer refresh. Current static map
data tops out at40 objects (capacity64); cloned-object definitions are confined
to retained FRLG maps. Continue refreshes scripts while retaining saved positions
and movement, and dispatches Pyramid/Hill floors to their own generated-template
refresh functions. No additional Hoenn NPC loading defect confirmed by this pass.
Full save-version/object-layout migration compatibility is not established.

Removed identical SetCameraToTrackGuestPlayer_2 and routed its return-to-field
caller to SetCameraToTrackGuestPlayer. Removed two duplicate forward declarations.
Exact same camera initialization remains. Source/build/release checked; no new
two-console link session claimed. Continued reading local/link map-load sequencing;
the complete4006-line overworld review is still unfinished.
Evidence: work/overworld-camera-20260921/.

## September 21 — retained item-description window safety

Configuration qualification: OW_SHOW_ITEM_DESCRIPTIONS is OFF in the current
production build. The prior icon helpers were exercised directly by native tests;
they are not a presently enabled player-facing popup feature. Reviewed remaining
window branch and guarded AddWindow's WINDOW_NONE result. Failure uses the small
icon path and does not mark the unseen description as shown. Cleanup uses explicit
one-based window ownership rather than inferring allocation from the item-seen
flag, and clears ownership after removal. No feature was enabled. Source/build
and release verification only for this dormant window branch; no runtime popup
allocation-failure acceptance claimed. Evidence: work/overworld-item-window-20260921/.

## September 21 — overworld item icon ownership

ShowItemIconSprite wrote copyToObjWin into gSprites[MAX_SPRITES] on creation
failure; DestroyItemIconSprite unconditionally destroyed its stored slot and did
not invalidate it. gSprites includes a sentinel slot, so this was sentinel-state
mutation, not an out-of-array write. Repeated cleanup could destroy a new sprite
reusing a successfully freed slot. Gate all sprite access on creation success;
use private zero-initialized one-based ownership (zero=absent) and clear it after
destruction. Cleanup before creation/after failure/repeated cleanup is now inert.

Native item_icon suite passes, including sprite-slot exhaustion with unchanged
sentinel and live slots, successful flashing icon creation, and reallocated slot
surviving repeat cleanup. Evidence: work/overworld-item-icon-20260921/tests.log.
No new rendered popup capture; window allocation/failure paths remain to audit.

## September 21 — Sealed Chamber scripted Dive integration

Added native coverage executing Underwater_SealedChamber's actual ON_DIVE_WARP
script through SetDiveWarpEmerge. Its map header has no static connections.
All nine coordinates in the3x3 neighborhood were checked in sequence:12,44
selects SealedChamber_OuterRoom; its eight neighbors select Route134, including
calls after the special destination was selected (no stale fixed-warp routing).
The fixture sets/restores saveblock coordinates because getplayerxy reads those,
not object-event coordinates. Script is unchanged from original Inclement.
All five fly_destinations groups pass: work/sealed-dive-20260921/tests.log.
No production change or redundant release rebuild in this pass. Native destination
selection is verified, not the rendered animation or the complete Regi puzzles.

## September 21 — connectionless Dive map lookup

GetMapConnection dereferenced gMapHeader.connections before checking its inner
list. mapjson legitimately writes NULL headers for maps without static connections,
including AbandonedShip_Underwater1/2 and Underwater_SealedChamber. Those maps use
fixed/resume or ON_DIVE_WARP scripts; normal lookup must returnNULL to reach them.
Guard both header and list before scanning; retain first direction match and
count bounds. fieldmap.c's analogous lookup already guards the outer header.

Four native fly_destinations groups pass, including NULL header/list, missing
direction, shortened/empty list, and actual Abandoned Ship map header using its
fixed destination then correctly returningFALSE when no destination exists.
Evidence: work/map-connections-20260921/fallback-tests.log. This verifies function
and scripted-fallback integration, not a full rendered Dive animation. No badge,
license, map placement or legendary puzzle condition changed.

## September 21 — overworld reset/initialization consolidation

Four identical travel-state blocks (Fly, Teleport, Dig/Escape Rope, whiteout)
now share ResetFieldTravelState. Teleport still resets Briney afterward; whiteout
still clears battle configuration, resolves abnormal-weather expiry and removes
the appropriate follower afterward. Initial avatar and all flags/vars retain
their order. InitOverworldBgs_NoResetHeap reuses InitOverworldBgs after its explicit
BG reset, retaining text/printer/message initialization afterward. No additional
heap reset introduced. Net29 source lines removed in this pass.

Source traced warp-ID/coordinate/center fallback, field-return state machines,
callback state reset through SetMainCallback2, heap relocation before rebuilding
view resources, and nulling tilemap cleanup. No new defect confirmed in those
paths. Verification is source-equivalence/build/release, not a newly recorded
Fly/Teleport/whiteout or link traversal. Overworld's larger source pass remains
incomplete. Evidence: work/overworld-cleanup-20260921/.

## September 21 — shared generator source pass completed

Read remaining move-pool culling, forced selections, ability/item/EV selection,
type/weakness checks, team dependencies and lead ordering in champions_circuit.c.
No additional confirmed defect. AddMove enforces four slots; validated pools
exclude duplicate/empty moves and cap at9 entries/3 abilities. Each selected Dex
family becomes exhausted before candidate rejection, bounding each team attempt;
GenerateCompetitionOpponent stops after64 attempts. Lead swaps account for the
first swap moving the second selected index, then generation reverses the order.
Trainer Hill preserves both leads while splitting reserves into two owner parties.

generate_showdown_champions_circuit.py --check passes756 variants/1322 templates.
Its checks enforce pool uniqueness/bounds, template spans, authored EV budgets and
legal abilities. The prior unchanged 12-group native pass includes a2048-seed
assembly fixture and family-contiguity/ability checks. No redundant native rerun
or production edit this pass. This completes a source pass, not proof of ideal
team balance, every RNG outcome, native facility traversal or player access to
the dormant Circuit/Tent modes. Pending legendary distribution decision remains
separate from generator implementation.

## September 21 — Circuit reward consolidation and reachability qualification

Mastery Eternatus is now final (40-win) entry in the existing ordered reward
table, removing duplicate delivery/full-PC/unavailable branches. Earlier eligible
unclaimed rewards still return first; result codes and caught flags unchanged.
Corrected stale difficulty comment (Medium -1/Easy -3 relative to Hard).
All12 native Circuit groups pass: work/circuit-cleanup-20260921/tests.log.

Important current-source limitation: no map scripts call Circuit begin/reward;
only the headless bridge starts Circuit directly. Champions Tent's retained
attendant script is included but also has no map entry caller. Generator remains
used by Trainer Hill through CreateChampionsExhibitionParty. Do not count dormant
Circuit legendary rewards as obtainable campaign sources or tests as proof of
player access. No unused modes were re-enabled. Broader legendary availability
audit must reconcile those entries with actual wild/scripted sources.
Generator source pass so far covers exhaustion/attempt bounds, coherence filters,
party normalization and reward lifecycle, not every move/item heuristic.

## September 21 — Aisha progression and pressure fixture

Aisha is Route117, reachable from Mauville before Wattson. Authored plan explicitly
specifies Medium cap30; both native fixtures incorrectly generated three-badge
cap40 opponents against level30 players. Changed to two badges, retaining all
Frost Breath ally-target, Anger Point and no-Protect expectations; both pass.
Then corrected pressure targets: Tackle into Ghost Froslass was immune, so target
Tauros instead. Added original party-slot/survival checks to prevent replacement
false positives. Both fixtures still pass with actual pressure on the recipient.
Evidence: work/aisha-fixture-20260921/{tests,pressure-tests}.log. No AI, authored
roster or production changes. This validates the intended first-access encounter,
not a requirement to force the same activation at every later cap. The former
cap40 board selected Shadow Ball; that tactical choice was not changed.
Cristian remains the unresolved tactical test from the 62-group suite, in addition
to the separately tracked broader Protect/guard benchmark debt.

## September 21 — Takao knockout versus switch

Resolved Takao's test failure without changing AI. Native action expectations
confirm the original lead chooses a move in both power120/400 variants. With
power120, original slot0 retains68HP and stays active. With power400 it faints
(originalHP0) and Hitmonlee from slot2 replaces it. The old final-species assertion
misclassified that replacement as a voluntary switch. Keep explicit EXPECT_MOVES
over all four authored moves to reject an actual voluntary switch; assert original
party HP for the knockout branch and retained species for the surviving branch.
Both variants pass. Evidence: work/takao-fixture-20260921/{diagnostic,tests}.log.
No production edits. Two tactical baseline cases remain: Cristian and Aisha.

## September 21 — Cristian survival fixture ownership

Native diagnostics resolve a misleading apparent survival: both alternate Beat
Up recipients faint under the existing Psychic-pressure fixture. AI replaces
the fainted original slot1 with slot2, so opponentRight->hp was reading a fresh
reserve. identity.log records original species1370/448 both atHP0; current battler
slot2 at65/92HP. Corrected the permanent survival assertion to read original
opponent party slot1. Corrected the Dark-weakness comment; no tactical expectation
was relaxed and no AI/roster/production code changed. The original negative
BeatUp expectation still fails, and Lucario's claimed survival is also disproven.
Do not treat this as proof that the shared planner needs retuning: determine
whether this pressure fixture expresses an authored requirement and compare the
pre-integration AI before changing decisions.

Evidence in work/cristian-fixture-20260921: identity.log and archived diagnostic
source; forced diagnostic requested a replacement (record type4), consistent
with a faint, and was not a successful completed test. Diagnostic copies were
removed from active tests. The unchanged no-pressure six-hit Rage Fist control
is the meaningful mechanics check; it is distinct from tactical acceptance.

## September 21 — authored level fixture correction

Verified current caps (Badge3=40, Badge8=80), Normal level reduction and compiled
trainer offsets +2/+1. Corrected stale Flannery expected levels36/35 to41/40,
and Wallace96/95 to81/80. All substantive terrain/weather/seed/Mega permissions,
abilities, activation and budget assertions retained. Both native groups pass,
including both weather parameters and all five League owners:
work/authored-fixtures-20260921/tests.log. No production code changed. Current
test ELF uses consolidated plan source and is stamped; it supersedes the prior
baseline diagnostic ELF. Three tactical baseline failures remain unresolved:
Cristian Beat Up, Takao switch and Aisha activation. Also independently confirmed
Annihilape's native Fighting/Ghost typing; do not treat its Dark-neutral matchup
as the Dark weakness incorrectly claimed by the old Cristian fixture comment.

## September 21 — Perish plan consolidation and baseline test debt

Moved emerald_champions_perish.c function bodies byte-for-byte into
emerald_champions_battle_plan.c; moved declarations to its existing header and
removed the two redundant production files. Updated three include sites.
Archived originals and consolidated source under work/battle-plans-20260921.
No tactic weights, AI branches, teams or Mega permissions changed.

Native suite of plans/Perish/party_knowledge: 57/62 groups pass. Restored original
translation-unit layout and reran the identical suite: same five failures.
Baseline and consolidation logs are baseline-tests.log and tests.log there.
All six knowledge groups and Perish policy/deadline cases pass. Outstanding:
Cristian Beat Up rejection; Takao turn-one switch; Aisha ally activation;
Flannery level expects36 but gets41; Wallace level expects96 but gets81.
The latter two conflict with current cap calibration; audit fixture contracts
before changing tests. Cristian's comment incorrectly calls Fighting/Ghost weak
to Dark (combined typing is neutral); verify actual damage before any AI change.
Do not mark these known-failing merely to turn gates green. Test ELF currently
contains the baseline layout from the diagnostic comparison; build/stamp a fresh
test ELF before further tests. Release uses consolidated source.

## September 21 — battle setup transition cleanup

Removed empty RegisterTrainerInMatchCall and its callers, unused malloc include,
and redundant rematch defeated-flag update (HandleRematchVarsOnBattleEnd already
performs it). Consolidated GetSpecialBattleTransition's duplicated level branches:
only Hill/Secret Base/e-Reader choose by level; Pyramid/Dome use their unchanged
random tables; other Frontier modes retain random selection except link multis,
which retain their synchronized trainer-ID sum. RNG call count and selected tables
are unchanged. Build/release verification, no new rendered transition acceptance.

Reviewed remaining opponentB prize consumers: ordinary partner-versus-one entry
does not set TWO_OPPONENTS; campaign prize setup already excludes the sentinel
via BATTLE_TWO_VS_ONE_OPPONENT. No additional reachable reward defect confirmed.
Source coverage extended through wild/legendary entries, battle environments,
rematch lookups and regional rival replacement; full facility behavior remains
unverified. No trainer/AI/reward-value edits.

## September 21 — trade menu text sprite consolidation

Ten local-template CreateSprite calls across link-trade menu initialization and
summary return now share CreateTradeMenuTextSprite. Helper owns runtime template
and preserves tag offsets, positions, priority and fatal allocation semantics.
Removed duplicated template setup and two stack template locals. Data/trade.h
shows nested OAM/anims are static; sheets loaded before creation. NPC trade
transaction/handoff untouched. Source/build/release validation; no new link trade
visual session or transaction behavior test claimed for this UI-only refactor.

## September 21 — condition/Pokeblock sprite templates

Migrated four pokenav_conditions_gfx.c and four use_pokeblock.c local-template
creation sites to existing owned-template helper. Covered occupied/empty party
markers, Cancel and condition-mon picture. Template-derived resources are static
or copied into loaded sprite sheets; no stack image descriptors introduced.
Prior fatal-on-slot-exhaustion policy retained. No callbacks, coordinates,
selection logic or gameplay rewards changed. Helper ownership has native coverage
from prior icon tests; these eight full-screen paths are source/build checked,
not newly driven through a rendered UI test.

## September 21 — recovered-save menu consumers traced

Main-menu SAVE_STATUS_ERROR shows recovery warning and HAS_SAVED_GAME (with
Mystery Gift variation), while CORRUPT/EMPTY select no-save menu. Continue enters
normal map/party/object restoration and resets facility streaks on warning. Save
confirmation treats recovered valid state through its existing default branch;
legacy migration change does not alter warning or admission. No additional
source-proven consumer defect; no production edits or redundant suite rerun.
Physical UI recovery warning/Continue interaction remains unrecorded visually.

## September 21 — legacy Bag migration after fallback recovery

LoadGameSave migrated only SAVE_STATUS_OK, but GetSaveValidStatus returns
SAVE_STATUS_ERROR when it successfully selects one intact slot beside a damaged
slot. Migrate both success outcomes; preserve warning and exclude EMPTY/CORRUPT.
Native disposable-flash fixture writes two legacy-layout saves, erases newest
sector, and reproduces missing usable Leftovers (0 vs3) before fix. After fix,
restores older counter/quantity and valid migration markers; repeated migration
keeps quantity3. Sixteen Bag/recovery groups pass:
work/save-relocation-20260921/migration-tests.log. No user saves modified or save
format/recovery selection changed. Special-sector staging callers also traced:
they allocate whole-sector buffers, so bulk special-sector copy alone was not an
out-of-bounds defect; no speculative size changes made there.

## September 21 — campaign suite85/85 and Circuit mastery authority

Remaining Circuit failure was fixture incompleteness: Celesteela/Xurkitree have
native-wild source metadata but remain entries in finite Circuit reward list.
Test now demonstrates their pending deliveries before Eternatus after all
Circuit-classified species are marked caught. Removed redundant source-category
mastery scan: at40wins every finite milestone(max24) is eligible and the prior
reward loop cannot fall through until all are caught. Existing reward entries,
thresholds, teams, global caught ledger and availability checks unchanged.
Full85 campaign fixture groups now pass: work/save-relocation-20260921/campaign-final-tests.log.
This supersedes the prior six/two failure reports, not the wider incomplete goal.

## September 21 — broader campaign fixture reconciliation

Initial full emerald_champions.c run:79/85 passed,6 failed. Five fixtures encoded
retired assumptions, verified against current source and approved design: prepared
uncapped ordinary wild sets, always-slot4 Sing, prepared held-item starter gifts,
HM compatibility and pre-Inclement prices/no resale. Updated tests to assert
natural moves/current cap, Sing wherever already known (otherwise slot4) without
changing other moves, natural capped transactional starter, badge+license any
party, baseline buy prices plus current modern quarter resale. No gameplay edits.

Rerun83/85 passed, leaving Circuit and an initially incorrect half-resale test
expectation. Verified ITEM_SELL_FACTOR uses GEN9 quarter, corrected fixture and
focused economy test passes. Thus only Circuit failure remains identified, but
latest full85-group green run has not occurred. Circuit fixture marks all
LEGENDARY_SOURCE_CIRCUIT species; explicit finite reward array also contains
species now tagged other acquisition sources. Need reconcile actual reward/
mastery contract before declaring the remaining failure obsolete or changing it.
Evidence: work/save-relocation-20260921/{campaign-suite,campaign-updated-tests,economy-tests}.log.

## September 21 — party restoration and obsolete restock fixtures

Added native three-mon save/zero/load roundtrip: injured, fainted and Egg states;
species/personality, status, held items, moves/PP and trailing empty slots preserved.
Existing stale-stat refresh regression also passes. Evidence:
work/save-relocation-20260921/party-tests.log (two focused groups).

Compiling emerald_champions.c exposed stale RestockEmeraldChampionsBasicBalls
references. Commit43ac8d634e removed that mechanic, declaration and nurse calls.
Removed the dedicated refill test and refill portion of mixed refill/economy test;
retained price/sale assertions under renamed economy test (not run or declared
passing here). No production refill feature restored and no gameplay changes.
Broader campaign suite now compiles but remains unvalidated as a whole.

## September 21 — save relocation copy cleanup

Read remaining load_save.c copy/rekey paths. Removed redundant SaveBlock1
pointer-to-pointer, replaced two Mail array loops with sizeof-based memcpy and
used IntrCallback for saved interrupt callbacks. Compile-time assertion binds
three temporary save copies to available heap capacity. Existing legacy Bag
snapshot rekey behavior preserved; extension pockets are not rekeyed twice.
Fifteen native Bag/save-recovery groups pass, using disposable emulator flash:
work/save-relocation-20260921/tests.log. No save format, RNG calls, key generation,
user-save edits or recovery policy changes.

## September 21 — main-loop source pass completed, scoped verification

Read main.c startup, callbacks, raw input/reset, link double-callback path, play
clock/music/VBlank ordering, timer/RTC seed, interrupt dispatch and soft reset.
Traced Trainer Hill counter through load_save/save: relocation nulls callbacks
and timer before moving blocks; save temporarily nulls and restores timer pointer.
No source-proven stale interrupt pointer in those paths. Retained hardware ordering
and existing RNG/audio cadence. This extends main.c to a source pass; native L=A
input tests remain its scoped behavior evidence. No emulator/hardware timing,
wireless/link frame-pacing or interrupt stress acceptance claimed. No production
changes in this pass; no repeat build necessary.

## September 21 — main input L=A repeat repair

ReadKeys compared raw input to mapped heldKeys, preventing L=A repeat after the
initial frame. Compare heldKeysRaw instead and propagate L into A for repeated
keys as already done for pressed/held keys. Hardware sampling remains once per
main loop; function takes sampled bits for native test injection. Normal-mode
behavior and raw inputs/soft-reset detection preserved. Native2-frame initial/
repeat delays, fresh press, held interval and release checked in both modes;
two input/new-game groups pass: work/main-input-20260921/tests.log. No timing,
speedup, audio or VBlank policy change. Main-loop audit remains partial.

## September 21 — encoded battle text output bounds completed

ExpandBattleTextBuffPlaceholders now takes dstSize; all three formatter and three
Z-move callers pass actual capacity (including prefix offsets). A bounded shared
writer preserves append versus replace semantics and EOS; overflow returns empty
text, zero capacity writes nothing. Number/nickname/item producers use bounded-
size local scratch before copying. Consolidated duplicate nickname-prefix cases.
Native concatenated move-record tests cover every capacity0..exact fit with
canaries; full42 text groups pass: work/battle-text-20260921/encoded-output-tests.log.
This closes decoder output-capacity finding for its fixed-size validated inputs;
not a claim of all text renderer/markup behaviors being complete.

## September 21 — encoded battle text input validation

Added preflight for the fixed TEXT_BUFF_ARRAY_COUNT input contract used by all
six callers. Reject unknown tokens (formerly infinite loop), truncated payloads,
missing EOS, number widths outside1/2/4 and digit counts outside1..10, and invalid
string/move/type/stat/species/party/flavor/ability/item indices before expansion.
Malformed output is EOS. Valid encoded records unchanged. Native negative fixtures
and positive Surf token pass; full41 text groups pass:
work/battle-text-20260921/encoded-tests.log. This validates input, not concatenated
output capacity inside this legacy decoder; that remaining seam is explicit.

## September 21 — line wrapper encoded-token boundaries

Shared token-length stepping now protects extended controls, keypad/extra glyphs,
and placeholder parameter bytes in line stripping/counting, manual-break/page
scans and word parsing. Rendering controls are not counted as ordinary glyphs;
keypad/extra symbols use their glyph widths. Native sound/color parameters with
space/newline/page-break values remain byte-identical while the real separator
wraps. Full40 text groups pass: work/battle-text-20260921/controls-tests.log.
Assumes valid encoded input, as other text APIs do. Runtime font changes,
Japanese/min-spacing state and repeated-gap width accounting are not yet fully
modeled by this wrapper; do not call typography audit complete.

## September 21 — line-wrapper metric truncation fixed

StringWord stored length/width in8-bit fields. Native60-character wide word
wrapped its pixel count and incorrectly kept following word on the same line;
300-character case also covers byte-length overflow. Use u16 startIndex/length/
width (6-byte transient word record vs4); no persistent memory/save changes.
Before failure captured; after all39 native text groups pass with canary/word
preservation checks. Evidence: work/battle-text-20260921/metrics-tests.log.
This handles current bounded game strings; no unbounded arbitrary-string claim.

## September 21 — automatic wrapping whitespace records

Count only real nonseparator runs; leading/trailing whitespace no longer creates
uninitialized empty StringWord records. Simplified filling from stateful length
tracking to explicit start/end runs. BuildNewString uses next line's recorded
startIndex-1 instead of summing word lengths plus assumed single spaces, preventing
break placement over letters after repeated spaces. Native leading/repeated/
trailing/all-space cases preserve word bytes and terminator canary. All38 native
text groups pass: work/battle-text-20260921/spaces-tests.log. Repeated-gap pixel
width estimation and embedded control parameters remain separate audit concerns;
this fixes parsing/index safety rather than claiming all custom markup layouts.

## September 21 — automatic wrapping populated-line bounds

BreakSubStringAutomatic's estimated totalLines can exceed populated line records
when words exceed the window width. BuildNewString then dereferenced unused
words pointers. Set final totalLines to actual currLineIndex+1 after a successful
layout; one unsplittable word returns unchanged without allocations. Native cases
cover oversized one/two/three-word strings, separator-to-newline/scroll behavior
and trailing canary. Full37 text groups pass: work/battle-text-20260921/wrap-tests.log.
No promise that oversized indivisible words visually fit: preserve them safely;
font fitting belongs to the caller. Whitespace/control-code parsing and8-bit word
metrics remain separate audit concerns.

## September 21 — full native text suite and Regenerator description

Ran all36 groups in test/text.c after battle formatter changes.35 passed;
Regenerator description exceeded109px item description width (117px). Shortened
to "Used Berries return / after battle. Except / Knock Off and theft." preserving
policy. First wording revision still118px; final wording passes unchanged width
gate. All36 groups now pass, including parameterized battle strings, names,
descriptions and map names. Evidence: work/battle-text-20260921/corpus-tests.log
(before) and corpus-fixed-tests.log (after). This is native font/layout and
placeholder coverage, not screenshots or all NPC-dialogue/story coherence.

## September 21 — battle status text byte comparison

TryGetStatusString now compares fixed8-byte legacy status tokens with memcmp,
removing u32 casts/dereferences of byte strings and redundant pointer/cached-word
locals. Preserves EOS padding and all seven mappings. Native tests cover all
seven tokens from unaligned storage, actual B_BUFF1 expansion and unknown/empty
rejection. Three battle-message groups pass: work/battle-text-20260921/status-tests.log.
No claim that alignment caused a reproduced player-visible failure; this is a
safe equivalent implementation with scoped behavior proof.

## September 21 — battle message destination bounds

BattleStringExpandPlaceholders now honors dstSize on literal, nickname-prefix,
placeholder and appended pause-control writes, reserving EOS. Capacity0 returns0
without writing. Overflow clears destination to a valid empty string and returns1
rather than exposing half a control sequence. Valid fitting messages unchanged.
Line-break source review confirms it replaces separator bytes, not appends bytes.
Native tests enumerate0 through exact-fit+1 capacities for literal/color-control/
expanded-buffer inputs, check both canaries and unchanged fitting output; trainer
name regression also passes. Two focused groups: work/battle-text-20260921/bounds-tests.log.
This bounds destination writes; scratch placeholder producers and whole authored
message corpus still need separate review. Full text suite not claimed passed.

## September 21 — battle message placeholder scratch lifetime

BattleStringExpandPlaceholders advanced textStart while composing class/name
placeholders and retained that offset for subsequent placeholders. Two-name
native comparison passes before/after because each toCpy starts at the current
offset; initial suspicion of immediately wrong text was not reproduced. However
repeated placeholders cumulatively consume the32-byte scratch array. Reset
textStart inside each placeholder block, so every name composition starts at
base. Native two-trainer combined-vs-independent output comparison passes.
Evidence: work/battle-text-20260921. No claim of full battle-string coverage.
Remaining finding: dstSize clears output but does not bound subsequent expansion
writes; automatic line wrapping must be audited before changing that contract.

## September 21 — picture decompression contract cleanup

DecompressPic was a local bool helper that unconditionally returned FALSE after
all decompression branches. Converted to void; removed two unreachable error
paths and simplified trainer-card caller. Allocation checks remain unchanged.
ResetAllPicSprites callers include sprite/heap reset transitions, so do NOT add
unconditional frees of old stored pointers without lifecycle proof; metadata reset
is distinct from destroying live owned pictures. No reset behavior changed.

## September 21 — shared picture allocator ownership

LoadPicPaletteByTagOrSlot now actually loads under the requested custom palette
tag for both Pokemon and trainers (formerly loaded species/trainer-derived tag
while sprite expected caller tag). Both picture constructors copy the mutable
shared template per sprite. Cleanup matches active records only, frees affine
matrix, destroys sprite before frame/image buffers, and retains shared palette
until unused. Removed duplicate field-effect matrix release; picture allocator
owns it. Native tests cover custom tag, shared palette, inactive-record cleanup
after sprite reuse, affine matrix return, and eight-slot exhaustion.12 field/icon
groups pass: work/summary-tiles-20260921/picture-resources-tests.log. Existing
allocation fatal policy unchanged; unreachable decompression-error branches and
ResetAllPicSprites lifecycle still require review.

## September 21 — portrait failure caller propagation

Extended picture-exhaustion repair through callers: InitFieldMoveMonSprite
returns MAX_SPRITES without writing dummy sprite; FldEff_FieldMoveShowMon creates
no animation task and clears its active-effect marker when portrait unavailable,
letting the parent field action continue. PicBox wrapper checks failure before
palette preservation; ScriptMenu_ShowPokemonPic returns false before task/window
creation. Hide-pic script already handles NULL close callback without waiting.
Native eight-picture exhaustion test verifies actual field-effect marker cleared,
no task-count growth and picture menu failure. Both field-effect groups pass:
work/summary-tiles-20260921/portrait-caller-tests.log. Ordinary successful visual
choreography unchanged; no new screen capture claimed.

## September 21 — field portrait/gate template and failure bounds

CreateTrainerSprite and RotatingGate_CreateGate now own dynamic templates using
the established helper; prior fatal sprite-slot exhaustion preserved. Field-move
portrait wrapper previously read gSprites[0xFFFF].oam before checking picture-
allocation failure. Check sentinel first, then preserve weather palette only for
valid sprite. Native test exhausts all eight picture slots and verifies safe
MAX_SPRITES result from actual wrapper, then releases allocations. Combined field
palette/icon results: work/summary-tiles-20260921/field-factory-tests.log.
No gate geometry, HM access, trainer artwork or palette policy changes. Rotating
gate visual motion and battle transition portraits not newly screenshot-tested.

## September 21 — battle popup sprite factory consolidated

CreateAbilityPopUp/CreateItemPopUp now share CreateBattlePopUpSprites; removed
duplicated sheet/palette/coordinate/two-sprite/task setup. Shared helper owns
runtime templates with prior fatal allocation policy. Ability overwrite and
headless recording hooks remain in their respective public wrappers, as does
ability/item text printing. Native four-battler construction checks distinct
persistent templates, per-battler tags and second-half tile offset32. Existing
animation lifetime and switch-in ability scene suites pass,20 groups:
work/summary-tiles-20260921/popups-tests.log. No rendered text/popup timing visual
claim beyond source-preserved callback/coordinates; mechanics/AI unchanged.

## September 21 — Pokemon icon template lifetimes

Tagged and raw icon factories now use the existing owned-template helper with
prior fatal-on-slot-exhaustion policy. Raw allocation's temporary frame-size
metadata is now static32x32/4bpp, matching both callers' fixed sMonIconOamData;
the special sprite.images raw-pixel pointer and FreeAndDestroyMonIconSprite
cleanup remain intact. Native mixed tagged/raw creation validates persistent tags,
size descriptor, raw image identity, freed raw tile reuse and tagged cleanup.
Evidence: work/summary-tiles-20260921/pokemon-icons-tests.log. No generic cleanup
substitution for raw icons; their image convention is intentionally different.

## September 21 — region map icon lifetime and tags

CreateRegionMapPlayerIcon never assigned playerIconTileTag/playerIconPaletteTag,
yet FreeRegionMapIconResources freed those fields. Store supplied tags; migrate
both cursor/player constructors to owned template copies with prior fatal-slot
policy. Clear sprite pointers after destruction in both full cleanup and cursor
zoom cleanup, preventing a repeated cleanup from touching a reused sprite slot.
Native test covers distinct tags/templates, complete tile/palette release and
repeat cleanup after slot reuse. Fly destination regressions and icon suite pass,
10 groups: work/summary-tiles-20260921/map-icons-tests.log. No destination changes;
this is lifecycle evidence, not a new Flight Beacon visual walkthrough.

## September 21 — Berry/cursor sprite ownership

Related stack-template scan found Berry dynamic factory and three list-menu
cursor/arrow factories. Migrated to owned-template helper, preserving original
fatal sprite-exhaustion policy. Berry cleanup also freed dynamic graphics/images
before DestroySprite used image size to release raw tiles; destroy now precedes
Free. Native two-Berry test checks distinct templates, owned image references,
other sprite survival and reuse of released tile range. Eight icon groups pass:
work/summary-tiles-20260921/berry-cursor-tests.log. Cursor layouts are source-
checked, not newly visually replayed. Remaining stack-template candidates include
region map, Pokemon icons (special image ownership), trade, slot machine and
other optional UI modules; scan results are not blanket proof of defects.

## September 21 — marking sprite template lifetime fixed

All five mon_markings.c dynamic sprite creation sites used stack templates;
marking editor also mutated a shared local template between sprite categories.
Use existing CreateSpriteWithTemplateCopy and preserve prior fatal-on-slot-
exhaustion behavior with explicit assertion. Covers combo/all-combo factories
used by summary, PC and Pokenav, plus editor window/mark/text/cursor creation.
Native factory test creates distinct tag pairs, checks independent persistent
templates, frees first without losing second graphics, and checks final tile/
palette release. Seven icon groups pass: work/summary-tiles-20260921/markings-tests.log.
No claim of complete editor/summary visual walkthrough.

## September 21 — summary Cancel result consistency

Task_HandleReplaceMoveInput A on row4 returned slot MAX_MON_MOVES but Result TRUE,
whereas B returned the same slot with Result FALSE. Set Result from slot validity.
Center/Rotom/map MoveDeleter scripts and party/evolution callers already guard the
slot sentinel; do not claim a demonstrated move-deletion bug. This repairs the
API's conflicting success result without changing valid-slot behavior. Source
trace found pokemon_center_move_tutor.inc contains obsolete Surf-blocking text,
but its missing special and absent event-script inclusion mean it is not proof of
an active Surf deletion gate; keep dormant data separate from active campaign.
Validation: source branches/callers plus release build/gates; no UI replay claimed.

## September 21 — summary multi Egg navigation fixed

IsValidToViewInMulti tested curMonIndex!=0 instead of the current page. Eggs
could appear on move pages when departing slot1–5, and be skipped on info when
departing slot0. Correct condition is currPageIndex==PSS_PAGE_INFO or non-Egg,
matching ordinary summary navigation. Native test reproduced invalid acceptance
before fix; now checks both pages, both Egg states, all six current slots and
empty-mon rejection.11 party groups pass; work/summary-tiles-20260921/egg-tests.log.
This invokes the real predicate via a test-only summary-state wrapper; no full
rendered input navigation claim. Replacement cancel handlers also source-traced;
slot sentinel propagation remains separate from the generic Result boolean.

## September 21 — summary temporary buffers made local

DrawPagination's fixed4*PSS_PAGE_COUNT u16 tilemap is now local; BG copy consumes
it synchronously before only the persistent destination is queued for VRAM.
BufferMonTrainerMemo's two32-byte temporary strings are local; expansion into
gStringVar4 is synchronous and the corresponding dynamic placeholder pointers
are cleared before returning. Removed three short-lived allocations/frees without
changing sizes, text generation, tile contents or callback flow. Source lifetime
review and release compilation/gates are validation for this low-impact change;
no new visual equivalence claim beyond the earlier sliding-tile parity test.

## September 21 — summary sliding tilemaps without heap churn

CopyNColumnsToTilemap draws directly into destination rows instead of allocating,
filling/copying/freeing an intermediate tilemap every animation update. Static
source graphics do not alias destinations; destination32-column stride preserved.
Compiled old/new functions compared across four authored layouts, all11 column
states, both directions and16 graphic patterns:1408 complete2048-tile buffer
comparisons identical. Plain and UBSan runs pass; combined ASan/UBSan process
hung before output and was terminated (no ASan success claimed). Artifacts:
work/summary-tiles-20260921. Source trace also covers summary allocation/close and
move selection, but full screen/visual audit remains incomplete.

## September 21 — trainer nature generation boundary fixed

Separate trainer helper also wraps on reachable generated personalities (e.g.
0x1000 -> requested13 becomes9). Preserved its gender-byte invariant: when signed
256-multiple delta would overflow, adjust by the equivalent25*256 period in the
other direction. All nonwrapping outputs unchanged. Same buggy expression exists
in authored508775fad8; this corrects execution of authored nature, not team design.
Native38400 generated-boundary/nature/gender cases pass;13 lifecycle groups.
Host250075 arithmetic cases verify modulo25, low-byte preservation and identical
nonwrapping outputs. Evidence: work/nature-boundary-20260921/trainer-tests.log.
No AI scoring or trainer-data changes. Full generated team equivalence across
all seed inputs remains larger than this bounded proof.

## September 21 — Frontier nature personality boundary fix

ModifyPersonalityForNature in battle_main used wrapping subtraction/addition;
u32 wrap changes modulo25 and can generate the wrong requested nature. Native
boundary test reproduced expected13/actual9. Keep nearest adjustment except
when it would wrap; then use equivalent nonwrapping adjustment by25-diff.
All1250 boundary/nature combinations pass,12 lifecycle groups total; evidence:
work/nature-boundary-20260921. Caller is battle_frontier.c. Distinct trainer_util
helper preserves low personality bits using256 increments and was NOT consolidated
or changed. No authored trainer nature/AI changes. That separate helper's boundary
behavior remains to audit rather than assume identical semantics.

## September 21 — battle party-order copies simplified

SwitchTwoBattlersInParty and SwitchPartyOrder now copy typed party-order rows
with sizeof-based memcpy instead of repeated loops/cast pointer arithmetic with
hardcoded stride3. Partner-row update policy and originalBattlerPartyId swap
unchanged.22 native party-item/switch-in groups pass (work/party-order-20260921),
covering normal/multi battle switches and party menu reorder/cancellation seams.
No new behavior or team ownership rules introduced.

## September 21 — replay Mimic slot ownership repaired

Recorded move permutations previously reordered moves/PP/PP Ups but rebuilt
mimickedMoves from original destination index j, leaving Mimic on the wrong slot.
Map each bit from moveSlots[j] instead; removed redundant temporary flag array.
Shared small mapping helper is used by playback and native tests.24 permutations
x16 masks plus explicit swap/legacy duplicate examples pass;11 lifecycle groups.
Evidence: work/record-cancellation-20260921/mimic-tests.log. Initial whole-playback
function fixture did not compile because test-runner enablement is const; removed
that fixture rather than weakening the runner. No full replay test claimed.

## September 21 — recorded packet bounds repaired

BufferNewBattlerData now receives destination capacity, caps byte-sized output,
and publishes only copied bytes; excess stays pending. Updated both controller
emitters with actual transfer-buffer remainder. Receive takes bounded command
capacity, validates duplicate length headers, complete record framing, battler IDs
and aggregate per-battler388-byte capacity before applying any packet. Removed
unchecked byte-consuming helper. Invalid receive disables recording, not combat.
All six player/link-partner/link-opponent consumers pass their command bounds.

Native packet test batches all four388-byte streams through29-byte payloads,
checks output canary and accumulated receive lengths; rejects overflow, bad IDs,
truncated bodies, inconsistent lengths and oversized declaration atomically.
Ten lifecycle groups pass: work/record-cancellation-20260921/packet-tests.log.
No saved-record or on-wire framing change. Not an actual two-console link test;
transport integrity and full replay fidelity remain outside this scoped proof.

## September 21 — recording saturation cancellation repaired

Track dropped bytes per battler when388-byte buffer is full. Cancellation first
consumes those discarded bytes, then only removes unpublished stored bytes.
Published-size floor prevents negative outgoing deltas. Counters reset with
RecordedBattle_Init; playback append behavior unchanged. Uses16 additional EWRAM
bytes, no saved-record layout change. Native tests publish300 bytes, append85,
then a six-byte choice that straddles capacity; exercise0–6 cancellation sizes,
verify exact preserved payload and repeat over-clear after publication. Nine
lifecycle groups pass: work/record-cancellation-20260921/saturation-tests.log.
Link receive framing/capacity and outgoing packet capacity remain to audit/fix.

## September 21 — recorded move playback bounds repaired

CheckMovesetChanges checks cursor before peeking and validates complete five-byte
move-change record plus all four indices before applying it. Invalid records use
the same existing finish/fade/quit behavior as GetBattlerAction, consolidated in
FinishInvalidRecordedBattle. Legacy duplicate indices retained: old writer maps
identical/empty moves to the first matching slot, so duplicate rejection would
break otherwise safe old records. Native validation checks every truncated size
and all252 invalid byte values in each of four index positions; eight lifecycle
groups pass (work/record-cancellation-20260921/playback-tests.log).
Remaining: full-buffer cancellation/publication and link receive bounds. This
pass does not fix those separately recorded findings or prove full replay fidelity.

## September 21 — recording saturation findings, unresolved

Traced all record-size readers/writers. SetBattlerAction silently stops appending
at388; ClearBattlerAction then removes stored bytes even if the latest selected
action was discarded. Example:388 recorded bytes +3 ignored choice bytes -3
cancelled bytes incorrectly leaves385. BufferNewBattlerData assumes size never
falls below published size, so saturation/cancellation can also invalidate its
unsigned delta protocol. Do not patch only the delta and claim recording fixed.

Playback CheckMovesetChanges directly peeks records[size] before the bounded
GetBattlerAction, and consumes four move-slot indices without validating their
range/permutation. A truncated/reordered record can therefore bypass the normal
end-of-record protection. Link receive also appends based on packet counts with
no destination capacity guard. These are source-proven unchecked boundaries;
no native exploit/crash or normal campaign reproduction claimed. Repair needs
consistent saturation/cancellation and validated playback/receive boundaries,
with focused native before/after tests. No production changes in this pass.

## September 21 — recorded action cancellation bounds

RecordedBattle_ClearBattlerAction decremented a zero u16 size before checking
it, then wrote at65535. Guard size before decrement; clear at most existing
bytes. Native tests cover empty/repeated empty cancellation and0/1/2/3/4/255
byte clears from a three-byte record, preserving prefix, another battler and
output canary. Seven lifecycle groups pass: work/record-cancellation-20260921.
Confirmed unsafe API edge, not claimed a reproduced normal-play menu sequence.
BufferNewBattlerData's previous-size delta assumes cancellation precedes confirmed
standby publication; caller lifecycle and long-record saturation still need audit.
No AI or game-balance changes.

## September 21 — action selection cancellation source trace

Traced HandleTurnActionSelectionState cancellation, pending-switch exclusions,
confirmation, restart question and recording rollback. PARTY_SIZE cancellation
returns to selection before UpdateBattlerPartyOrdersOnSwitch; before-action
state resets monToSwitchIntoId. Partner cancellation rewinds the recorded action
by its encoded length and clears the partner's pending gimmick bit. Retry clears
restartQuestionPending, routes accepted retry through run/forfeit teardown, and
otherwise presents ordinary forfeit confirmation. Switch ordering for in-game
partners commits only after all actions confirmed. No new source-proven defect
or gameplay edits in this pass. Native input-cancellation/Retry menu walkthrough
still required before marking this section behavior-verified.

## September 21 — switch transfer native behavior

Added parameterized normal-switch/Baton Pass fixture:400HP Substitute plus
Swords Dance, then switch to a new mon. Normal switch clears both; Baton Pass
retains100 Substitute HP and+2 Attack. Separate Shed Tail fixture preserves100
Substitute HP while clearing the Attack boost. All20 Mega/form/transfer groups
pass: work/power-trick-20260921/switch-transfer-tests.log. No production edits.
This covers those transfer rules, not every volatile or generation configuration.

## September 21 — duplicate Baton Pass state cleanup removed

SwitchInClearSetData cleared source-owned escapePrevention twice for normal
switches; retained the shared normal/Gen5+ Baton Pass loop and removed the
redundant first write. Embargo/healBlock timers already carry V_BATON_PASSABLE
in VOLATILE_DEFINITIONS; removed redundant manual copies. Special substituteHP,
Perish Song and trap-owner data copies remain; Shed Tail branch unchanged.
Thirty native Mega/PowerTrick/Disguise groups pass, including suppressed ability
Baton Pass into Disguise. Evidence: work/power-trick-20260921/transfer-tests.log.
These tests are supporting regression coverage, not exhaustive status-transfer
proof; duplicate-write equivalence also checked against the definition macros.

## September 21 — Baton Pass Power Trick stat truncation

Confirmed native counterexample: passing Power Trick to Attack400/Defense300
recipient yielded Attack300/Defense144. SwitchInClearSetData used packed u8
BattlerId as SWAP temporary; changed to u16. Same defect exists in authored
508775fad8, so it predates current integration. No AI retuning. Other direct
battle-stat SWAP sites use u16/u32 and need no same-type repair.
Before failure and after18 passing Mega/form groups recorded under
work/power-trick-20260921. Regression remains in existing mega_evolution.c next
to the prior Power Trick/form-refresh coverage. Broader Baton Pass volatile
transfer review remains incomplete.

## September 21 — reserve availability consolidation

HasNoMonsToSwitch now has one shared-party scan instead of three duplicated
loops. Preserved independent trainer-party handling and the two-versus-one
simultaneous-KO/final-reserve exception. Replaced unusual index[pointer] syntax
with ordinary monToSwitchIntoId[right]. Native shared-party test covers64 live
masks x49 pending-slot pairs x2 player positions =6272 checks. Existing switch-in
single/double/multi/1v2/2v1 and lone-party tests pass;20 total groups.
Evidence: work/turn-order-20260921/reserves-tests.log. This is a behavior-preserving
refactor of reserve availability, not a change to authored teams or AI scoring.

## September 21 — absent battlers and Commander

Traced UseMove's IsBattlerAlive/commandingDondozo early exit; IsBattlerAlive
rejects out-of-count, zeroHP and absent flags. Ran campaign_one_mon.c plus
Commander suite after action-order changes.45 groups pass, including lone-mon
wins/losses against two opponents, Commander target immunity/residual effects,
forced-switch prevention and attacker identity on either allied slot. Evidence:
work/absent-battlers-20260921/tests.log. No new battle mechanic changes. This
covers selected absent/attached-battler behavior, not every faint/reserve sequence.

## September 21 — After You insertion consolidation

Replaced three hand-written array shuffle cases in ChangeOrderTargetAfterAttacker
with one bounded insertion, preserving target/action pairing and relative order
of all other battlers. Self/already-acted/missing targets return false; adjacent
valid target retains configured Gen8+ success. Native permutation test covers24
orders x16 attacker/target pairs (384 cases), plus actual After You/Quash/dynamic
Speed scenes. All12 focused groups pass (work/turn-order-20260921/insertion-tests.log).
Related next/previous turn-order scan found pursuit loops bounded and Round's
previous-action access zero-guarded. No AI scoring changes.

## September 21 — action-end bounds and forced order

Native After You/Quash scene tests pass: forced target order survives dynamic
Speed recalculation. Source trace exposed real array bounds defects in
HandleAction_ActionFinished (next-battler After You lookup and next-action load)
and HandleAction_NothingIsFainted (next-action load). At final doubles action the
next index is4 but both arrays contain4 elements. Bound reads by gBattlersCount;
a completed turn now explicitly sets B_ACTION_FINISHED. Singles no longer reads
an inactive leftover slot either. No speed scoring/AI changes.

Native boundary test covers both handlers for singles/doubles, alongside Tailwind,
Icy Wind, After You, Quash, Speed Swap and cleanup/initialization tests. All11
groups pass: work/turn-order-20260921/bounds-tests.log. Full turn-order and battle
main audits remain incomplete.

## September 21 — dynamic Speed behavior

Added doubles scene regressions to existing speed_swap.c: Tailwind raises an
unacted ally from40 to80 ahead of a60-Speed opponent; Icy Wind lowers both
opponents60/55 below a50-Speed ally. Assert the actual move animation order in
the same turn. Both pass with existing production logic; all five focused groups
pass (work/turn-order-20260921/dynamic-tests.log). No AI/mechanics edits. Quash,
After You and tie-specific dynamic cases remain outside this narrow coverage.

## September 21 — turn order native coverage

Traced action sorting, manual switch Speed/Trick Room comparator, move-only
mid-turn reordering, and Quick Claw/Quick Draw/Custap announcement dispatch.
Compared suspicious Booster Energy condition with authored508775fad8: unchanged;
no AI or speed-modifier retuning based on that expression alone.
Ran switch_in_abilities.c and speed_swap.c against current source. Evidence:
work/turn-order-20260921/tests.log. Scope includes initial/post-KO ability order,
manual switches in normal/Trick Room order, ignoring move-only Custap/Stall/
Lagging Tail for switches, and Speed Swap's base-stat versus modifier behavior.
No production changes. This does not exhaust move priority, Quash/After You,
speed ties, dynamic reordering or shared planner behavior.

## September 21 — battle initialization native dirty-state evidence

Added native test invoking actual BeginBattleIntro with stale weather, field/side
state, outcome, level-up/absent/controller masks, payday, move locks/history and
held Berry input. Checks zeroed transient values, PARTY_SIZE/0xFF sentinels,
new original held item and owner-origin mapping. Three lifecycle groups pass:
work/battle-cleanup-20260921/init-native-tests.log. This covers selected state
owned by intro initialization; not every struct field, controller setup, link
mode or visual lifecycle. Production ROM unchanged by this test-only pass.

## September 21 — battle initialization source pass

Traced BattleStartClearSetData through turn/side/field resets, histories,
controller state, rewards, held-item provenance and sleep-clause sentinels.
BattleStruct is zero-allocated at entry; initialization assigns original items
and origins per trainer/slot. Moved player-party critical-hit reset outside the
trainer loop; same value, once per slot instead of four times. No gameplay or AI
behavior changes. Checked sprite allocation concern: CreateSprite is fatal-on-
exhaustion in this engine, so missing caller sentinel checks alone do not prove
an out-of-bounds write. gTriedEvolving is owned/reset by completed evolution loops;
no confirmed stale-mask defect from this source trace. Full initialization dirty-
state behavior coverage still pending; do not count source trace as native proof.

## September 21 — battle outcome dispatch review

Traced win/loss/run/mon-flee dispatch through battle scripts and trainer return
callback. Removed unreachable nested earlyRival else branch; outer condition
already guarantees true. Chooser remains1 and rival win-text/whiteout callback
behavior unchanged. Ordinary forfeit script currently prints forfeit and ends;
its historical GaveMoney symbol is not evidence of a payout. Restart uses its
separate no-penalty script. No trainer AI, team or difficulty changes.

## September 21 — completed-fade cleanup fix

battle_main.c FreeResetData_ReturnToOvOrDoEvolutions skipped all resource cleanup
when fade was already inactive and evolution was excluded. Removed the early
return after selecting ReturnFromBattleToOverworld: cleanup no longer depends on
an earlier fade-active frame. Existing fade-active and link ownership policies
unchanged. Native lifecycle test allocates battle resources, enters the completed-
fade first-battle branch, verifies resources and BattleStruct freed, then repeats
cleanup safely. Two scoped groups pass; work/battle-cleanup-20260921/tests.log.
This tests resource lifetime, not visual fade rendering or all battle endings.

## September 21 — battle_main battle-end source pass

Traced EcSnapshotForRestart/EcRestoreForRestart, restart forfeit dispatch,
FreeResetData_ReturnToOvOrDoEvolutions, TryEvolvePokemon and ReturnFromBattleToOverworld.
Restart preserves allied parties and skips evolution/forfeit payout before rebuilt
trainer parties. EVO_MODE_BATTLE_ONLY's evolutionItemArg receives the remaining
level-up mask, but pokemon.c does not consume that argument for this mode; not a
proven mechanic defect. EVO_MODE_BATTLE_SPECIAL correctly receives the party slot.
Resource cleanup is mostly null-safe, but fade-active cleanup/transition ordering
needs focused native lifecycle verification before refactoring. No battle-main
production edits in this pass. Do not label the6125-line file fully reviewed.

## Party-summary shared blend lifetime

Both summary-hide completion paths now share FinishPartyStatusSummary. Clear the
finishing tray's shown flag, but retain BLDCNT/BLDALPHA while another tray remains;
clear blend registers only at final tray completion, then destroy the task.
Native baseline reproduced first tray clearing BLDCNT0 while the other still
needed16192. Four intro/midbattle and single/overlapping completion cases now pass,
including final register release and task destruction.23 combined native animation/
controller/Regenerator groups PASS; strict release/gates PASS stamp0c6a608270b4.
Evidence work/ability-audit-20260920/summary-blend-* includes failing baseline.

Tests execute actual completion phases and GPU-register state, not a rendered
full fade sequence. No timing curves, AI, teams or gameplay mechanics changed.
No cross-task messages; shared build lock released locally. Continue controller/
interface lifecycle source audit; whole-game objective remains unfinished.

## Party-summary hide task ownership

Battle interface now owns HidePartyStatusSummary(battler), guarding shown state,
task bounds/activity, idle callback and battler owner before scheduling its private
hide task. Repeated requests cannot reset an in-progress fade or restart the task
after its sprites were destroyed but before its final two cleanup frames. Replaced
direct task rewrites in common controller and legacy Oak caller. Draw now sets
shown flag after successful creation rather than before its two-frame delay.

Native baseline proved repeated hide clobbered an active cleanup callback. Six
scheduling cases now pass (in-progress, visible, wrong owner, inactive, invalid ID,
hidden), including immediate repeat and controller completion.22 combined native
animation/controller/Berry groups PASS; strict release/gates PASS stampc5455033dfe6.
Evidence work/ability-audit-20260920/summary-hide-* including baseline. Tests prove
scheduling/resource-state contract, not rendered tray fade. Initial compile caught
legacy Oak caller; converted it before validation. No AI/team changes.

Next inspect both summary cleanup branches resetting shared BLDCNT/BLDALPHA while
another tray may still be visible/fading; source clears unconditionally, but no
native counterexample tested yet. Whole controller/game audit remains incomplete.
No cross-task messages. Build lock released locally after checks.

## Shared trainer portrait palette cleanup

Consolidated trainer sprite teardown into FreeTrainerSprite in battle_controllers.c.
It releases the affine matrix, destroys the sprite, then uses the existing
FieldEffectFreePaletteIfUnused check. Both intro callbacks and slide-back completion
now share it. Removed unused public FreeTrainerFrontPicPalette helper/declaration.
Native baseline using two opponent portraits with the same trainer palette proved
first destruction freed the shared tag while the second sprite remained alive.
Now shared palette survives until the final portrait is destroyed. Previous matrix
ownership/orientation tests remain passing. No AI, teams or script changes.

21 focused native animation/controller/Regenerator groups PASS; strict release/gates
PASS stamp2886be26b66e. Evidence work/ability-audit-20260920/trainer-palette-* includes
baseline. This validates resource ownership, not a full rendered staggered trainer
intro. No cross-task messages; shared build lock released locally. Full controller
and game audit remain incomplete.

## Player-side front trainer portrait affine matrix leak fixed

BtlController_HandleDrawTrainerPic's player/front branch creates an affine sprite
then used to assign affineMode=OFF without freeing the allocated OAM matrix. It now
uses existing FreeSpriteOamMatrix, which both releases ownership and turns affine
off. Front-facing orientation/offset/callback behavior unchanged.
Native constructor baseline proved bitmap1 instead of0 after initialization.
Control with another matrix already allocated verifies only portrait's own matrix
is released; hFlip and y2=48 remain unchanged.20 focused animation/controller/Berry
groups PASS. Strict release/gates PASS stampce4c58ac1856.
Evidence work/ability-audit-20260920/trainer-matrix-* includes failing baseline.
This is native resource/state evidence, not a full rendered trainer-intro capture.

Local analogous scan: battle_anim_flying.c DestroyAnimSpriteAfterTimer,
battle_anim_ice.c translation cleanup and pokemon_animation.c summary reset all
explicitly free matrix before disabling affine; no equivalent defect there.
No cross-task messages. Shared lock released locally after successful checks.
Controller lifecycle and full game objective remain unfinished.

## Mon animation completion owns its battle wait

Task_HandleMonAnimation previously dereferenced gBattleStruct unconditionally at
completion and decremented any nonzero battlerKOAnimsRunning counter. That task
also serves evolution/hatching/HallOfFame/visualizer paths. Native controlled
ordinary-completion test reproduced stealing another animation's counter (1->0).

LaunchAnimationTaskForFrontSprite/BackSprite now return their task ID; existing
ordinary callers can ignore it. LaunchKOAnimation registers the returned task
through TrackMonAnimationForBattle, which marks ownership and increments once.
Completion decrements only a marked task with live battle state. Uses task data6,
previously unused; no new persistent memory. Outside-battle completion avoids
battle-state dereference. Optional after-KO animation remains disabled; failed-ball
animation still uses its existing path. No AI/team or battle-rule changes.

19 animation/controller/Regenerator native groups PASS. Tests drive the real task's
completion phase, covering ordinary/tracked/idempotent registration and NULL battle
state; not a full animated hatching/evolution/capture visual walkthrough. Strict
release/gates PASS stampb580268a47bc. Evidence work/ability-audit-20260920/
mon-animation-* includes failing baseline. Local lock released, no cross-task
messages. Controller lifecycle review and entire game objective remain unfinished.

## Optional party-order response bytes initialized

BtlController_EmitChosenMonReturnValue now writes all three optional order bytes:
provided orders copied unchanged, NULL payload becomes zero instead of prior
transferBuffer contents. Party ID, command ID and packet length unchanged. Native
baseline demonstrated stale bytes after a real supplied-order response followed
by NULL. New player-cancel and NPC-choice parameters pass with supplied-order
control. Current cancellation consumers ignore these bytes; this is deterministic
packet cleanup, not evidence of a reproduced visible gameplay bug or AI change.

18 combined controller/Regenerator native groups PASS; strict release/gates PASS
stampd5c6ab8fb8f1. Evidence work/ability-audit-20260920/controller-response-*.
No cross-task messages; build lock released locally. Continue substantial controller
lifecycle/animation/party-data review; current progress is partial, not a full file
signoff. Link receive/ring-buffer code was read but not changed or link-play tested.
Full game objective remains unfinished.

## Controller variable-transfer boundaries

battle_controllers.c now bounds DATA/SETMON emit payloads to transferBuffer capacity
before writing; uses memcpy rather than byte loops. GetMonData aggregates through
one-record scratch into a252-byte reply, checking each actual serialized length
before append. Active party slot validated before bit shift; current gameplay
callers request one full record only. BattlePokemon records are zero-initialized,
so unfilled fields/padding do not serialize stack contents (receivers still set
battle types/ability/stages/volatiles as before). No AI/team changes.

Raw read no longer copies through an unrelated BattlePokemon temporary at an
arbitrary Pokemon offset. Read/write check offset+length against Pokemon size and
copy directly. No current raw-request emit caller found; guards cover legacy
handler endpoints, not a claimed reproduced campaign crash. Oversized unsupported
replies fail explicitly; no truncation/new fragmentation protocol. Current full
BattlePokemon is140bytes, so two full records cannot fit252-byte transfer payload,
despite old local-buffer comment claiming two Pokemon. Scalar six-slot requests fit.

17 focused controller/Regenerator native groups PASS: maximum252-byte response,
maximum253-byte update with allocation canaries, zero length, raw-record final2
bytes and completion, active slot/fixed mask/all-six species order, and existing
native Berry battle tests exercising normal full-record responses. Strict release/
gates PASS stamp581c4c1bf93e. Evidence work/ability-audit-20260920/controller-transfer-*.
Not a full multiplayer/ring-buffer or every controller command audit. Link buffering,
fixed-format payload capacities and other controller lifecycle paths remain pending.
No cross-task messages. Local build lock released. Whole-game objective remains open.

## Battle message packet consolidation

battle_controllers.c ordinary/selection message emitters now share EmitBattleString.
Selection previously omitted hpScale/itemEffectBattler/moveType despite the common
BufferStringBattle decoder unconditionally reading those fields into live state.
Same omission exists in preintegration508775fad8; no AI/teams/scoring changed.
New helper populates all fields and clears padding, preserving command headers,
string ID bytes, packet size and ordinary snapshot semantics.18net lines removed.

Native baseline emits ordinary message then changes metadata and emits selection:
hpScale remained1 instead of3. New two-parameter ordinary/selection regression
passes, checking move/item/ability/script-slot/HP-scale/effect-battler/type/text data
and distinct header arguments. This is native packet evidence, not rendered battle
text walkthrough. Strict release/gates PASS stamp452425396e8f. Evidence
work/ability-audit-20260920/controller-message-* including failing baseline.

No cross-task messages. Shared build lock released locally. Continue controller
transfer sizes/response consumers and lifecycle audit; entire3315-line module not
yet fully audited. Full game objective and unresolved authored-AI guard case remain
open. Preserve latest Regenerator KnockOff/theft exception.

## Latest Regenerator rule: Knock Off and theft are permanent Berry losses

User clarified: tool restores held Berries after consumption, except Knock Off or
stealing. Previous code restored knocked-off Berries; corrected now. Uses existing
PartyState padding bit as originalBerryRemoved (no struct-size/save-layout growth).
Record removal at Knock Off, successful Thief/Covet/Magician/Pickpocket, delayed
Pickpocket original owner, BugBite/Pluck, and opposing Trick/Switcheroo target item.
Voluntarily giving own Berry during Trick/Bestow is not a second theft event.
SetHeldItemOrigin clears the loss flag if the original item genuinely returns to
its owner (including theft back/Trick return); Recycle recovery still works.

GetBattleRestoredHeldItem suppresses removed original Berries regardless of tool.
Consumption still returns only with tool; non-Berry restoration and temporary
battle policies unchanged. Existing Incinerate destruction policy left unchanged;
no new rule beyond requested KnockOff/theft exceptions. CudChew/Harvest/Recycle
in-battle execution untouched. Updated test expectations that encoded old KnockOff
restoration, not the move mechanics. Initial broader Trick experiment marked both
items lost; corrected to target-only, preserving voluntary transfer semantics.

23 focused native Regenerator/theft groups PASS, including tool/no-tool CudChew,
KnockOff/Thief/Covet/Trick/Switcheroo/BugBite/Pluck removal, actual stolen-item return,
Recycle then KnockOff, identical Berry ownership and existing theft/inventory seams.
Strict release/gates PASS stampb161be615af9. Evidence
work/ability-audit-20260920/regenerator-loss-*. Norman explanation and item description
now mention KnockOff/theft exceptions; removed wording implying in-battle recovery
abilities were disabled. Dialogue source/gates checked, no new visual capture.
No AI weights/teams changed. Shared lock released locally, no cross-task message.

Resume battle_controllers.c message-payload audit (selection packets omit fields
that formatter reads). Full game objective remains unfinished.

## User clarification: native Berry mechanics remain active before Regenerator

User explicitly reaffirmed Cud Chew and moves/abilities must function normally
without the Regenerator. Verified current Cud Chew end-turn path has no tool gate;
Regenerator check is only in GetBattleRestoredHeldItem postbattle policy. Recycle/
Harvest restoration paths likewise do not require the tool. No mechanics changed.

Renamed misleading CudChew test and strengthened it with tool present/absent battle
parameters: Farigiraf120/200HP takes40DragonRage, heals50fromSitrus and50fromCudChew,
ends180HP with empty held slot. Both parameters pass. Existing postbattle assertions
still distinguish missing tool versus owned tool.10 Regenerator native groups PASS.
Evidence work/ability-audit-20260920/cud-chew-clarification-*; test-only change, no
release rebuild needed. The ability repeats consumption/effect, not a permanent
uneaten held Berry. Recycle/Harvest can genuinely recover a held Berry; their
recovery clears consumption provenance when applicable.
Shared lock released locally; no messages to other task.

Resume battle_controllers.c audit: EmitPrintSelectionString omits hpScale,
itemEffectBattler and moveType although BufferStringBattle copies those fields
unconditionally. Preintegration508775fad8 has same omission. Candidate shared
message-payload builder could remove duplication and stale fields; investigate
consumer behavior and native regression before changing. No controller edit yet.
Whole-game objective remains unfinished.

## Pyramid held-item bundle transfer simplified

No cross-task messages sent; local build lock only.
TryStoreHeldItemsInPyramidBag now snapshots the small PyramidBag on the stack,
uses AddPyramidBagItem directly, restores the snapshot on any failed addition,
and clears party held items only after all three fit. Invalid difficulty returns
failure before indexing. Removed two heap allocations, frees and configuration
branches;12net lines removed. The real lobby sets the routing flag, so its normal
behavior is preserved; direct invocation no longer risks writing the ordinary Bag.

Native exhausted-heap baseline crashed on20-byte snapshot allocation. New8-case
fixture covers0/1/2free-slot rollback and3-slot success with routing flag off/on,
checks all three holders, ordinary Bag unchanged and other difficulty intact.
All37 Bag/PC/gift/storage groups PASS; strict release/gates PASS stamp04c5dd874070.
Evidence work/ability-audit-20260920/pyramid-transfer-* including baseline.
No changes to facility battles, campaign placements or authored AI. Shared lock
released locally after checks. Whole-game objective remains unfinished.

This closes the currently traced ordinary/Pyramid item-insertion allocation family.
Resume the large core-file audit (battle_controllers.c is still pending a full
pass) rather than rerunning unchanged inventory suites. Keep known authored-AI
guard-scoring failure and campaign/visual integration gaps explicit.

## Pyramid item arithmetic and atomic transactions

Keep user direction: no cross-task messages. This pass used local build lock only.

AddPyramidBagItem formerly narrowed addition into u8 quantities before splitting;
standalone compiled actual-source probe proved Add(256) returnedTRUE storing0.
Now capacity preflight precedes in-place commit with widened arithmetic; preserves
Pyramid-specific matching-stacks-before-empty ordering. RemovePyramidBagItem now
preflights ownership, bounds the preferred cursor index, and otherwise consumes
matching stacks in original index order. A Cancel cursor at10 formerly reached
the other difficulty row. All four Pyramid quantity/space/mutation helpers reject
out-of-range difficulty before indexing. No heap snapshots remain in Add/Remove.

36 focused native Bag/PC/storage/gift groups PASS, including both difficulties,
256-item addition, cross-difficulty Cancel cursor regression, matching-stack and
selected-slot priority, atomic failure, invalid mode and exhausted-heap operations.
Strict release/gates PASS stamp622a119d6ede. Evidence
work/ability-audit-20260920/pyramid-items-* and pyramid-overflow-probe.{c,log}.
Pyramid battle/facility design untouched; no new campaign content.

Related remaining seam found locally: battle_pyramid_bag.c
TryStoreHeldItemsInPyramidBag still allocates two snapshots for a three-mon bundle.
Could replace with one bounded PyramidBag snapshot and verify partial-failure
rollback; it currently calls AddBagItem and depends on the storing-items flag.
Do not claim all Pyramid/UI paths allocation-free. Full game audit remains open.
Shared lock released after checks; no message sent to other task.

## User direction: stop cross-task messaging

User explicitly said to stop messaging the other task because it wastes its time.
Do not send further coordination/status prompts to it. Keep using the local shared
build lock and checking live build processes; no notification is needed on release.

## Gift bundle preview no longer allocates

CheckBagHasSpaceForItemBundle uses one bounded stack preview sized from the maximum
of all seven configured pocket capacities (currently180slots/720bytes), reused
per pocket. Existing transactional insertion and pocket ordering rules unchanged;
real Bag and discovery flags remain untouched. Removed fatal allocation/dead NULL
branch/free, with a capacity guard before copying into the local array.

Native baseline with exhausted heap crashed trying to allocate404bytes. Updated
exhausted-heap bundle test now passes success and late Berry refusal across four
pockets, duplicate Berry entries and final-slot use in the largest pocket, while
verifying no items are actually inserted. All32 focused Bag/PC/storage/campaign-gift
groups PASS; strict release/gates PASS stamp19312f973f3e. Evidence
work/ability-audit-20260920/bundle-heap-* includes failing baseline.
Shared lock released locally after completed checks; no cross-task message sent.
Next related seam: AddPyramidBagItem snapshot allocations. Full game goal remains
unfinished; no blanket release-ready claim.

## Ordinary Bag/PC insertion no longer allocates

Corrected an earlier recovery assumption: AddBagItem/AddPCItem reached
BagPocket_AddItem's heap scratch allocation. Strengthening the real storage-cursor
item test to exhaust heap reproduced CRASH at item.c426 (360-byte allocation).
Replaced scratch array and single-use CheckSlotAndUpdateCount with allocation-free
capacity preflight then deterministic commit. Preserves index order, segmented
pockets, and first eligible-slot Berry rule. Capacity subtracts clamped quantities
so malformed oversized stacks contribute no free space and are preserved.

All31 Storage/PC/Bag/campaign-gift native groups PASS; tests include all three
storage-cursor item sinks with exhausted heap,24split-stack/Berry capacity cases,
atomic insufficient-space refusal and oversized-stack preservation. Strict release
and gates PASS stamp490d7c8e33af. Evidence work/ability-audit-20260920/item-delivery-*,
including actual failing baseline. Worker confirmed preflight/insertion equivalence.

Remaining related dependencies: CheckBagHasSpaceForItemBundle still allocates a
private pocket using fatal Alloc despite a NULL return branch. AddPyramidBagItem
still allocates snapshots; do not claim AddBagItem universally allocation-free.
Ordinary PC cursor recovery cannot route through Pyramid. Other low-memory and
whole-game audit scope remain unfinished.

Coordination: overworld lost the lock race but its batch continued editing five
registration ceremonies and cycling-triathlete graphics metadata. Preserved these;
forced event_scripts and graphics metadata rebuilds before final tests/stamp.
That task reports dialogue rewrite unfinished, so do not call this a finished
player release. Current lock is released and task notified to finish its dialogue
pass. Core did not modify those map/metadata files. Full goal remains active.

## Optional party-menu setup recovery verified

InitPartyMenu returns bool and uses unchecked allocation. On optional failure it
sets cancellation outputs before callback, preserving successful keepCursorPos.
Battle entry wrappers reorder only on success; failed return from battle Summary
restores the already-reordered party. Background allocation uses unchecked storage;
its failure cancels, then Task_ExitPartyMenu restores field order before callback.
Both paths use the preceding allocation-free permutation. Removed the empty
ReshowBattleScreenDummy call from these two wrappers; other callers remain.

IMPORTANT correction to earlier worker advice: mandatory SEND_OUT and
CHOOSE_FAINTED_MON cannot safely returnPARTY_SIZE; Cmd_switchhandleorder accepts
that byte as the next index. Preserve existing fatal allocation-failure behavior
for these two mandatory battle actions. This is not full OOM recovery everywhere.
Other later graphics/window allocations also remain outside this pass.

19 Party-items/Bag native groups PASS; new exhausted-heap cases cover script/
daycare selection, optional battle switch, item selection, Summary return, and
field/battle background failure. Both order and cancellation outputs checked.
Strict release/gates PASS stampc1285cf73ceb. Evidence work/ability-audit-20260920/
party-recovery-*. Worker independently reviewed active callers, committed item/mail
mutations, mode sentinels and order restoration; no further defect found.

Coordination incident: overworld Lucy lock won a race; my first script incorrectly
continued after mkdir failure and edited party_menu.c. Immediately notified owner,
stopped mutations, and owner rebuilt/restamped current headless inputs before
releasing. Subsequent acquisition explicitly checked tool exit status and live
make before editing. Current core build validated after that handoff; shared lock
released and overworld notified. Preserve Lucy arrival callback edits.

Next core pass: remaining party lifecycle/recipient handling or large-file audit;
do not rerun unchanged recovery tests without new changes. Full objective remains
open (campaign task owns overworld, authored AI unresolved guard case still open,
battle/wild audit and full integrated acceptance incomplete).

## Party drawing and reordering allocations removed

Applied both prepared patches after overworld task released its lock. Party-panel
rasterization directly blits8x8tiles; no temporary bitmap allocation remains.
Host probe using actual blitter and authored layouts proved6158 rectangle outputs
identical, including transparent/untouched pixels. No new visual scene captured.

Forward/inverse party ordering now share ReorderPartyForMenu, moving validated
permutation cycles with one Pokemon temporary and six slot pointers. No heap or
new persistent RAM. Invalid/duplicate indices return before mutation. Uses memcpy
for byte preservation (including padding). Host comparison against original
functions passed23040 permutation/layout/link/flank cases across both parties.
Native test covers16layout/flank cases with every free heap block occupied,
proves a real change then byte-exact restoration of both parties and rejects
invalid/duplicate mappings. Compiled function reserves148local stack bytes plus
36saved-register bytes. Worker independently checked ownership/transparency.

All17 Party-items/Bag native groups PASS; strict release/gates PASS stamp
e2deff3e9de7. Evidence work/ability-audit-20260920/party-no-allocation-*;
party-render-parity.*, party-order-parity.* retain host proof. Source refactor is
applied; prepared patches now archival, do not reapply them.

Next: enable party entry/BG allocation recovery coherently: failure must cancel
selection, battle wrappers must not reorder after failed initial allocation,
later BG failure must restore field order. These recovery fixes are NOT yet made.
Now ordering itself is allocation-free, that blocking dependency is resolved.
Preserve successful keepCursorPos behavior. Full game objective remains open.
Shared build lock released after completed checks; overworld task notified.

## Party recovery caller audit completed; build slot still reserved

No production input changed while overworld task owns lock. Its latest explicit
message: fixed initial-visibility native build followed by incremental headless
build, then release. wait_threads cursor a657ded4-74ca-4c01-91a9-118eae051895:4
confirmed the task active/inProgress; do not claim a live make from the lock alone.
Prepared party-panel patch and6158-rectangle parity evidence remain unapplied.

Main/worker independently traced additional recovery hazards: both battle-entry
wrappers reorder party AFTER InitPartyMenu even if its allocation fails. Later
AllocPartyMenuBg failure uses Task_ExitPartyMenu, which unlike normal close never
restores field order. Battle controllers also consume gPartyMenuUseExitCallback;
stale TRUE could return stale gSelectedMonPartyId/order as a real choice. Script,
contest/daycare/relearner callbacks consume gPartyMenu.slotId directly.

Coherent next recovery change: InitPartyMenu returns success; battle wrappers only
reorder on success. Failure outputs must mirror cancellation (slot>=PARTY_SIZE,
gPartyMenuUseExitCallback FALSE, mode-appropriate selectedMon/script result) before
callback. Preserve successful keepCursorPos behavior: do not clear the old cursor
unconditionally before a successful Init. Later BG failure restores party order
before callback, with no dependency on another heap allocation. Existing forward
and inverse order functions allocate an entire party buffer; consider bounded
stack storage after checking call depth/stack use or another allocation-free
approach. Test field selection, both battle-entry wrappers, inverse permutation
and mandatory-send-out no-selection response. Do not merely swap allocators.

ReshowBattleScreenDummy is actually empty. Init failure callback can bypass it.
Task_ExitPartyMenu runs solely from BG-allocation failure; normal close separately
resets sprites/restores party ordering. Panel raster change can be applied/tested
independently first; deferred recovery remains a distinct verified pending seam.
Full core/campaign/battle/wild goal remains active and unfinished.

## Party-menu read-only audit during overworld build slot

Production files unchanged this turn: promised build slot remains owned by the
separate overworld task, verified active through wait_threads; lock owner says
initial visibility regression/headless validation. Do not steal it merely because
there is a gap between make invocations. Continue source review or wait for release.

Concrete prepared simplification: replace BlitBitmapToPartyWindow temporary
height*width*32 allocation/copy/free with direct8x8 tile blits. Created unapplied
work/ability-audit-20260920/party-panel-no-allocation.patch. Host probe compiles the
actual BlitBitmapRect4Bit body against all five authored slot*.bin layouts, with
deterministic mixed/transparent pixels and initialized destination. All6158 valid
subrectangles produce identical entire buffers; transparency/untouched areas
preserved. Evidence party-render-parity.{c,log}. This is algorithm parity evidence,
not applied production code or native visual acceptance. Apply/review when lock
available, then focused native validation and release build.

Allocation recovery constraints: InitPartyMenu and AllocPartyMenuBg use fatal
allocators despite explicit NULL paths. Return callbacks BufferMonSelection,
CB2_ChooseContestMon and CB2_ChooseMonForMoveRelearner read gPartyMenu.slotId directly;
enabling recovery must set cancelled selection (PARTY_SIZE+1) first and preserve
mode-specific callback/battle order semantics. Bg allocation failure precedes
party boxes/windows and goes through ExitPartyMenu/Task_ExitPartyMenu; pointer
cleanup is NULL-safe. Do not simply change allocators and preserve stale selection.
Panel direct blitting eliminates an allocation instead of adding recovery branches.
Whole core/campaign/battle/wild objective remains unfinished.

## Overworld graphics factory template lifetime

CreateObjectGraphicsSpriteWithTag now uses a stack template plus the persistent
sprite-slot template helper. Removed its template allocation/free, checks a free
sprite slot before resource loading, handles regular palette exhaustion (dynamic
already did), and rolls back only newly introduced palettes/compressed sheets on
creation failure. Shared tags are preserved. Final subsprite setup unchanged.
Worker verified palette-return semantics and ownership against actual loaders.

22 focused Bag/icon/object-palette groups PASS; strict release/gates PASS stamp
05ea44340889. New native checks cover full sprite pool with static/dynamic graphics,
regular palette exhaustion, raw tile exhaustion with new/shared palette, and
Pikachu-doll generic cleanup through this factory. Doll fixture initially assumed
all tiles were tagged; corrected to check tagged allocations only and sprite
release for raw graphics. Evidence work/ability-audit-20260920/object-template-*.
No native visual scene rendered this turn. Compressed decoder heap failure is
still fatal; not a claim of universal OOM recovery.

Potential next simplification: paletteTag argument remains unused as before.
Only WithTag callers are decoration.c and debug.c plus wrapper/header; could
consolidate with CreateObjectGraphicsSprite after verifying caller cleanup intent.
Party allocation recovery and remaining whole-game scope remain open.

BUILD HANDOFF: next shared build slot promised to overworld task
01a0bf54-07d2-7372-b344-3a524e3786fd for confirmed new-game hide-flag/map fixes
(Heatran/Lucy/Cynthia/RustboroHiker/Roxanne) and Rustboro guide fixture variable.
Core build completed; lock released and task notified. Hold core build-input edits
while that task's builds are active. Do read-only audit meanwhile; verify actual
live process/task state before treating it as a wait. Preserve its map/fixture edits.

## Copied sprite template ownership

Fixed the prerequisite for using persistent templates in cloneable overworld
sprites. CopySpriteToSlot now copies a dynamic template into the destination
slot's storage; ordinary static-template pointers retain identity. CopySprite and
CreateCopySpriteAt share this primitive. Native baseline reproduced original-slot
reuse changing the clone's tile tag from0x7100 to0x7101. Both directions now preserve
own tags/coordinates/priority and generic cleanup leaves replacement resources
intact; static-template controls pass.19 combined Bag/icon/object-palette groups
PASS; strict release/gates PASS stamp947887cec9c3. Evidence
work/ability-audit-20260920/sprite-clone-* (failing baseline retained).

Worker audited other whole-Sprite assignments: only remaining two copy battler
sprites, which cannot originate from current copied-template icon constructors.
No conversions needed there. CreateObjectGraphicsSpriteWithTag still owns/frees a
heap template; migrate that constructor next now that cloning is safe, checking
palette/tile ownership and allocation failure. Also preserve dynamic palette
exhaustion tests. Full core/campaign/battle/wild objective remains unfinished.
Shared /tmp/emerald-champions-build.lock held throughout edits/build/tests/stamps;
release completed and lock released at handoff. Separate overworld task notified.

## Persistent icon templates and resource failure cleanup

Added CreateSpriteWithTemplateCopy in sprite.c/header: CreateSpriteUnchecked then
copy/repoint the finalized template into persistent storage indexed by returned
sprite slot. No callback escapes during creation. Adds1536bytes EWRAM (232268/
262144,88.60percent), removes item/decoration temporary template heap allocations.
Item icons and decoration icon-table/metatile constructors use it. Native generic
DestroySpriteAndFreeResources now sees valid own tags even with simultaneous icons.
Items return MAX_SPRITES on exhausted slots and free only newly loaded resources,
retaining preexisting shared tags. Decoration own-resource failure paths also free
their tags. No static-template identity semantics changed for other sprite callers.

All14 Bag/icon groups PASS; includes both decoration constructor paths (HeavyDesk
icon and SmallDesk metatiles), normal/custom icon lifetime, scratch failure, bounded
successful allocation, and full sprite pool with/without shared graphics. Strict
release/gates PASS stamp026a97b98efb. Evidence work/ability-audit-20260920/icon-template-*.
An initial test allowing only800scratch bytes exposed the existing SMOL decoder's
additional256-byte allocation, so success fixture now reserves1280; zero-scratch
failure coverage retained. Decoder allocation failure itself remains fatal; this
pass does not claim arbitrary low-memory icon creation is fully recoverable.

Remaining lifetime seams: decoration DECORPERM_SPRITE delegates to
CreateObjectGraphicsSpriteWithTag, which still frees its temporary template.
Do not extend copied-template helper to cloneable sprites without preserving
copies in CopySprite/CreateCopySpriteAt: those memcpy Sprite and would alias the
source slot's template. Worker found no such cloning among current icon users.
Party allocation recovery and full-game objective remain unfinished.

BUILD COORDINATION: user has a separate overworld task01a0bf54-07d2-7372-b344-3a524e3786fd,
which owns map scripts/dialogue/choreography. Preserve its Rustboro/AshenWoods/
Route111/JaggedPass edits. Both tasks agreed to serialize ALL shared builds/tests/
stamping with atomic mkdir /tmp/emerald-champions-build.lock and rmdir after done;
check live make processes before claiming. Do not treat a stale lock as a live
process. Source mutations affecting a shared build must also wait. Overworld uses
immutable staged ROM/ELF for scene workers. My build finished and no live make was
observed at this handoff. No lock acquired by this already-completed build.

## Item icon ownership and Bag cleanup

Consolidated AddItemIconSprite into AddCustomItemIconSprite, combining the two
scratch buffers into one zeroed unchecked allocation with one owner and idempotent
cleanup. Template allocation is now unchecked before loading graphics, so either
allocation failure returns MAX_SPRITES without partial resources. This restores
the advertised failure contract for these allocations; CreateSprite itself remains
fatal on sprite exhaustion and needs separate resource-ownership review.

Worker found reachable freed-template read: item constructors CreateSprite with a
heap template then free it; shipped BUGFIX RemoveBagItemIconSprite called generic
DestroySpriteAndFreeResources, which dereferences that dangling template for tags.
Native regression simulates reused template metadata pointing to the other icon:
baseline frees the wrong resources. Bag cleanup now reuses existing RemoveBagSprite
with stable slot-owned tags; preserves BUGFIX other-icon hiding, affine cleanup,
and SPRITE_NONE reset. No new persistent template array.44net production lines
removed across item_icon.c/item_menu_icons.c.

All12 Bag/icon native groups PASS: zero scratch memory, scratch-only memory leaving
no template room, standard/custom variants, repeated cleanup/reallocation, normal
simultaneous icon creation and independent tags, stale-template Bag cleanup.
Strict release/gates PASS stamp6c6ef92f2c23. Evidence work/ability-audit-20260920/
item-icon-* (template-baseline failure retained). Initial test stamp detected a
concurrent RustboroCity script edit; preserved it and rebuilt with -W before
stamping current tree. No stale artifact accepted.

Remaining: constructors still leave a freed template pointer; other audited callers
mostly free explicit tags or use FieldEffectFreeGraphicsResources runtime metadata,
but full producer contract cleanup is unfinished. Decoration duplicates that
producer; avoid stack-template substitution without auditing consumers. Party-menu
allocation-recovery family remains open. No full UI capture for these icon changes.
Full core/campaign/battle/wild objective remains active.

## Lossless PC initialization failure recovery

Implemented dedicated Task_ExitStorageAfterInitFailure for states1/5 instead of
normal screen-transition logic. It never reads uninitialized item icons. Persistent
handoff recovery uses sSavedMovingMon and sMovingItemId only on reentry, before
cleanup. Empty original mon slot is preferred; after swaps with occupied origin,
find a free party slot or box slot, copy the mon and apply normal placement
form/stat/cap updates. No occupied slot is overwritten. Carried items try Bag,
then PC item storage, then an existing non-Egg Pokemon's empty held-item slot
(with item form update). Pickup/swap invariants guarantee a hole; only broken
invariants invoke fatal assertions rather than silently lose state.

Enabled existing allocation recovery contracts with AllocUnchecked for storage
entry/reentry and MultiMove_Init. Reentry failure uses saved sCurrentBoxOption,
never the NULL sStorage. Shared failure callback clears scripted selection result
and index exactly like cancellation. MultiMove cleanup remains idempotent.
Worker independently checked Name/Summary handoff ownership, swap occupancy,
item/mailer restrictions, cleanup ordering and cancellation; no defect found.
Exceptional recovery can place a formerly boxed mon in the party or a carried
item onto another empty holder when inventory sinks are full; this is lossless.

All14 Storage/Bag native groups PASS, strict release/gates PASS stamp68c99edd2294.
New cases: occupied original with only party or final-box hole, exactly one copy
of recovered species, Bag/PC/full-inventory held-item sinks, stale scripted success
cleared, and actual AddWindow8Bit failure selecting dedicated handler. The last
checks dispatch only; no full UI OOM scene or every complete init-state teardown
was played. Baseline wrong-destination evidence is in storage-window-baseline-*.
Current evidence work/ability-audit-20260920/storage-recovery-*.

Remaining allocation-family work: party entry/background/optional panel,
item-icon scratch buffers; TilemapUtil_Init currently fatal despite NULL fallback.
Other later PC allocations remain fatal and do not all have recovery contracts.
Full-game objective (including battle mechanics, AI unresolved guard case,
remaining campaign/NPC/wild audit and integrated visual acceptance) remains open.

## PC setup failure dispatch and repeated cleanup

Fixed Task_InitPokeStorage cases1/5 to explicitly set SCREEN_CHANGE_EXIT_BOX
before scheduling Task_ChangeScreen. Native real AddWindow8Bit exhaustion at
state5 reproduced a wrong destination with seededprior1; five destination seeds
now select EXIT. Test wrapper tests dispatch only, not actual final callback/UI.
MultiMove_Free now clears its global pointer after freeing; the native wrapper
also verifies NULL and repeats cleanup safely. This prevents case1 reentry from
freeing the previous session's dangling sMultiMove before state5 replaces it.
All11 compiled Storage/Bag groups pass; strict release/gates PASS stamp93a1e5c386ff.
Evidence work/ability-audit-20260920/storage-window-*; baseline preserved.

CRITICAL REMAINING PC recovery work: setting EXIT does NOT finish recovery.
Main traced sSavedMovingMon outside its box during summary reentry; latest origin
slot can be occupied after a shift, so do not simply restore to origin or enable
unchecked allocations everywhere. Worker confirmed OPTION_MOVE_ITEMS case1/5
failure precedes CreateItemIconSprites(state9); Task_ChangeScreen reads uninitialized
itemIcons[].active/area and movingItemId, potentially clobbering persistent carried
item. Need dedicated initialization failure cleanup with lossless disposition of
carried mon/item, before enabling currently fatal entry/reentry/multimove allocators.
CB2_ReturnToPokeStorage's dead NULL branch still dereferences sStorage; do not
expose it without fixing the ownership return contract. No allocator changed here.
TilemapUtil_Free also leaves pointer dangling; state0 currently replaces it before
these failures, unlike MultiMove. Keep state-boundary evidence precise.
Full core/campaign/battle/wild objective remains incomplete.

## Bag allocation recovery repaired

GoToBagMenu's explicit failure branch was unreachable because AllocZeroed is fatal
on allocation failure in both release and tests. It also passed NULL to
SetMainCallback2 when reopening the last pocket (the normal callback-retention
API). Use AllocZeroedUnchecked and explicit-or-remembered exit callback, then
return early; successful initialization unchanged and unnested.

New native test in test/bag.c consumes existing free heap blocks without replacing
the harness heap, then opens the Bag with explicit and NULL callbacks. Baseline
CRASH at src/malloc.c:200 proves failure; both callback parameters now pass.
All11 combined Bag/shop groups and strict release gates PASS, stampc2f814603c2d.
Evidence work/ability-audit-20260920/bag-allocation-*.
This fixes first Bag allocation only; later setup allocations remain fatal.

Reusable worker identified same dead-recovery family in InitPartyMenu,
AllocPartyMenuBg, optional party-panel rendering, item_icon.c scratch buffers,
EnterPokeStorage/CB2_ReturnToPokeStorage, MultiMove_Init, TilemapUtil_Init.
These are NOT fixed yet. Main verified item_icon allocations and PC multi-move
failure source. PRIORITIZE PC case: Task_InitPokeStorage case5 calls
Task_ChangeScreen after MultiMove_Init fails without initializing screenChangeType;
that state selects summary/name/Bag callbacks with uninitialized fields. Set
explicit EXIT_BOX when fixing and test real failure path. PC reopen NULL branch
also dereferences sStorage->boxOption; worker says saved sCurrentBoxOption is the
correct source (reverify before edit). Do not blindly change every allocator;
prove cleanup/callback ownership first. Worker remained read-only.
Full core/campaign/battle/wild objective remains unfinished.

## Bag removal refresh consolidation

src/item_menu.c now shares RefreshBagListAfterRemoval across sale, slot-based
toss, and generic removal after deposit. Extracted exactly DestroyListMenuTask,
UpdatePocketItemList, UpdatePocketListPosition, LoadBagItemListBuffers, ListMenuInit;
callers retain their original money, cursor color, screen-copy and wait timing.
Toss retains its original selected-slot lookup before refresh. A source expansion
comparison against work/ability-audit-20260920/item-menu-before-list-cleanup.c
proved all three operation sequences unchanged. Initial compile caught removed
cursor locals still needed by toss; restored them before successful verification.
InitOldManBag now delegates to identical DoWallyTutorialBagMenu implementation.

All10 compiled Bag/shop native groups pass, strict release/gates PASS, stamp
23cd3411a3e0. Existing Bag tests cover pockets/migration/link restore/sorting/key
registration, not interactive sale/toss/deposit callbacks. Do not claim full UI
acceptance from them. Source review: sell rejects zero-price/protected/free items,
limits transaction value to MAX_MONEY, and wallet saturates on payment. Existing
policy can discard proceeds above wallet headroom; unchanged baseline behavior.
No reachable failed RemoveBagItem sale demonstrated. Deposit only removes after
successful AddPCItem and user acknowledgment; no concurrent Bag mutation found.
Evidence work/ability-audit-20260920/bag-list-cleanup-*.
Next: remaining item_menu routing/use/give paths and callers. Full objective open.

## Shop exit visual check

Current headless ROM aba344cb606f5dfd (source stampa5ffd6e314ee) was built from
current source and exercised in an independent synthetic OldaleTown_Mart scene.
Observed Welcome -> Buy/Sell/Quit -> B -> Please come again -> A -> cleared field.
Inspected contact sheet shop-exit-before3/sheet-01.png: menu disappears during
farewell and all dialogue clears when controls return. Thus the numeric2 passed
to ClearStdWindowAndFrameToTransparent is false under its ==TRUE contract, but
subsequent script dialogue refreshes the tilemap; no visible stuck-menu bug was
reproduced. No production change made. Do not blindly replace2 withTRUE: that
would queue pixel-buffer DMA immediately before RemoveWindow frees the buffer.

First two recipes pressed buttons before greeting/menu printing finished and did
NOT test exit. Third recipe explicitly waits300frames for menu/farewell, actually
executes exit, and passes final readiness. Keep this distinction when reusing logs.
Evidence work/ability-audit-20260920/shop-exit-before3/{result.json,recording.json,
sheet-01.png}; exact inputs in shop-exit-recipe.json. This is synthetic native
UI evidence, not earned campaign progress or every shop type. Full shop buy/sell,
full-Bag UI, decoration and free-catalog visuals remain unverified here.
Next source target: item_menu.c sell transaction flow and quantity/money seams.
Overall core/campaign/battle/wild goal remains active and unfinished.

## Shop purchase-history cleanup

Native2groups PASS (five bulk-boundary parameters plus history capacity/reset),
strict release/gates PASS, stampa5ffd6e314ee. Baseline failed at256-versus255.
RecordItemPurchase now accepts item/quantity directly instead of task storage,
uses the first empty ITEM_NONE record instead of a redundant cursor, and applies
the existing255 TV saturation to initial as well as repeated purchases. Previously
999 then1 produced history999 then255. Actual delivered quantity is unchanged.
TV consumer uses>=255 for its bulk branch; no save layout or dialogue change.
Worker verified sole consumer/append-only history and no analogous accumulator.

Shop object sprite-failure suspicion is NOT an out-of-bounds defect: gSprites has
MAX_SPRITES+1 entries and StartSpriteAnim only writes the supplied sprite fields.
BuyMenuDrawObjectEvents can write the allocated sentinel; normal item-icon paths
already guard allocation failure. No sprite behavior changed this pass.
Source-read remaining concern: Task_HandleShopMenuQuit passes2 to a bool8 argument
whose implementation copies VRAM only for==TRUE. Existing comment calls this
incorrect; verify visible menu cleanup before changing. Full shop visuals and
runtime transaction UI are not yet tested. Overall objective remains unfinished.
Evidence: work/ability-audit-20260920/shop-history-*.

## Shop transaction review — quantity arithmetic

Strict release build and gates PASS; stamp ed84504c1fd3.
In src/shop.c, keep affordable quantity as u32 until clamping to999, and
replace the equivalent four-line cap branch with min. Current lowest positive
literal item price is20, and wallet maximum999999 yields49999, so the previous
u16 temporary has no demonstrated reachable overflow under current pricing.
This is defensive simplification, not a claimed player-facing bug fix.

Read purchase/confirmation/payment/bonus/exit flow: normal Bag and decoration
adds precede the charge callback; failed delivery returns without charging;
free catalog skips charging; zero price avoids division; Premier quantity is
limited to available Bag space. These are source findings, not native UI proof.
Purchase-history handling is inconsistent: new entries may exceed255 while a
repeat purchase clamps total to255; TV stores u16 but uses255 as a special
narrative threshold. Investigate intended semantics before changing that seam.
BuyMenuDrawObjectEvents also uses CreateObjectGraphicsSprite's result without
an explicit failure check; establish actual failure contract/reachability next.
Palette copy is NOT an overflow: PLTT_BUFFER_SIZE is512 and copy length is bytes,
so copying from palette index256 correctly copies the remaining256 u16 entries.
Evidence: work/ability-audit-20260920/shop-quantity-release-{build,gates}.log.
The overall source/campaign/battle/wild audit remains unfinished.

## All rare/native resident selector coverage

Strict release build/gates pass, stamp018f5ce99f86.

Added native parameterized coverage for every RARE_WILD/NATIVE_WILD definition
(currently65residents): resolve its actual map header, require a nonzero runtime
habitat table, satisfy its declared progression/discovery requirements, enumerate
all100normal percentile rolls and verify authored3percent native/1percent quest
share, reject the other habitat, then mark captured and verify all100rolls exclude
that resident. 13000before/after selector rolls plus65wrong-habitat checks. This
proves configured selector availability once eligible, not physical map reachability
or that every discovery NPC can establish those requirements in a full playthrough.

The initial single-batch fixture reached the harness timeout; split into one
parameter per resident without dropping any checks. Parameterized baseline passed.
Selector now rejects non-wild source classes before expensive family/discovery
queries; those read-only results were previously ignored. Pool ordering, RNG draws,
probabilities and acquisition rules unchanged. All26combined wild/sign groups pass
after this cleanup, including existing ordinary-slot, habitat and lifecycle checks.
Evidence work/ability-audit-20260920/resident-distribution-*.
Full core/campaign/battle/wild objective remains unfinished.

## Rare wild habitat integration

Strict release build/gates pass, stampb8741a8e095f.

Time-of-day audit confirms shipped OW_TIME_OF_DAY_ENCOUNTERS isFALSE and both Dex
and live encounters select default tables; no active fallback mismatch established.
Native baseline instead proved unlockedMarshadow selectable onRoute113 but absent
from the Dex area search. Added only RARE_WILD/NATIVE_WILD sign locations when the
same CanAcquireLegendarySignSpecies predicate used by encounter selection allows
them, with current-region filtering and existing landmark/moving-area rules.
Consolidated regular/special/expansion map routing into AddMapToAreaScreen; original
Feebas routing retained. Fixed/gift/breeding/quest-battle sources remain excluded.
No encounter odds, species, levels, discovery requirements or capture rules changed.

All25 combined wild/sign native groups pass. New checks compare Marshadow runtime
selection with area presence before unlock/after unlock/after capture, reject its
Hoenn habitat while viewing Kanto, and verify Enamorus/Fezandipiti meadow glows
remove only the captured resident. Test wrapper queries actual area construction
without rendering. Related source audit verifies65rare/native definitions across
36maps each has the correct nonzero default habitat table: water on125/126/127,
land elsewhere, including land-routed UnderwaterSeafloor. This is structural/runtime
routing evidence, not proof of physical reachability or every discovery quest.
Evidence work/ability-audit-20260920/rare-habitat-* includes failing baseline.
Full habitat rendering and broader core/campaign/battle/wild objective unfinished.

## AlteringCave habitat-display integration repair

Strict release build/gates pass, stampfda653d679f7.

Native baseline reproduced Noivern missing from the discovered AlteringCave area.
MapHasSpecies used the shared MAPSEC_ALTERING_CAVE label to filter nine original
rotating tables, then incorrectly discarded restored1F/B1F as variants10/11.
Filter now checks the exact original map group/number; new floors retain their
independent ordinary tables. Encounter species/levels/odds are unchanged.

Native full habitat-search tests (TESTING-only allocated screen-state wrapper,
no rendering) cover Noivern1F/BasculegionB1F under all9variant choices plus invalid
choice fallback; original-map Zoroark still appears only for set0/fallback. Hidden
landmark behavior remains. Initial missing-landmark hypothesis was disproved:
Route103 enters the original cave, whose transition already sets the flag before
newfloors (and B1F can branch directly from that original cave). No redundant map
hook added. Other expansion landmark metadata has no current visibility consumers.

Removed optional broken fishing-length branch: always scan the10fishing slots,
not12land slots. Synthetic neighboring Mew/Celebi entries are excluded while valid
Magikarp is found; empty table remains absent. Did not independently establish
whether the old BUGFIX-off branch was active in the shipped build. All13wild groups
pass, including prior odds/caps/outbreak/roamer checks. Evidence under
work/ability-audit-20260920/habitat-cave-*. Full habitat rendering/time-of-day/rare
resident overlays and all-species campaign availability remain unverified.
Full core/campaign/battle/wild objective remains unfinished.

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

## Explicit first League admission requirement

Strict release build/gates pass, stamp071b3b7c5d1d.

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

## Juan reward retry and ice-stair consolidation

Strict release build/gates pass, stamp07fde5dbfc60.

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

## Size-contest calculation cleanup and formatting leak fix

Strict release build/gates pass, stamp1755207df69a.

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

## Kiri daily Berry bundle delivery repair

Strict release build/gates pass, stamp504b1bd90887.

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

## Sootopolis crisis-state branch consolidation

Strict release build/gates pass, stampf013d6d6f648.

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

## Seafloor awakening / Sootopolis authorization pass

Strict release build/gates pass, stampb192ab1e6eaa.

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

## EerieSpell / Spite PP-drain consolidation

Strict release build/gates pass, stamp83220ef1c96e.

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

## Shared move-driven held-item destruction

Strict release build/gates pass, stamp893610da0375.

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

## Sleep-cure Nightmare cleanup and shared status clearing

Strict release build/gates pass, stamp947411215489.

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

## Set-effect flinch cleanup and status controls

Strict release build/gates pass, stamp5ed1ce9dc3e5.

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

## Stat queue reset consolidation and FlowerVeil lifecycle check

Strict release build/gates pass, stamp20edf92c7ade.

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

## Intimidate stat-preview side-effect repair

Strict release build/gates pass, stampea5b431616f3.

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

## Partial-save expanded Bag persistence repair

Strict release build/gates pass, stamp4f3a216047c6.

Native baseline: addSlowbronite after a full save, perform SAVE_LINK, reload;
the stone disappears because the old partial writer only persisted logical0..4
while expanded Bag data occupies SaveBlock3 chunks beside PC sectors5..13.
Synchronous and task-driven partial saves now update all extension chunks. For
PC sectors, read and validate the existing signature/id/counter/checksum, retain
its full saved PC payload, replace the extension chunk, recalculate the versioned
checksum and use the shared commit-last writer. Invalid source-sector validation
fails before erasing it. Existing saved PC snapshot and save counter stay intact.

Extracted WritePreparedSaveSector so full/incremental preparation and extension-
only preparation share the existing flash/commit path. Async pipeline now stages
through13 and commits13 on the completion call. Source-audited StartMenu, record
mixing and BerryBlender callers all wait forTRUE without fixed iteration limits.
Synchronous partials perform14writes instead of5; longer pause is expected. Older
peer BerryBlender link timing is not verified (additional standby rounds); do not
claim cross-version link support. This does not alter authored battle/AI content.

All6 native save groups pass, including both partial APIs retaining new MegaStone,
keeping old boxedPikachu despite RAM-only Eevee edit, and unchanged counters. Legacy
partial-save test now correctly expects all14sectors upgraded; manual mixed-version
compatibility remains covered by host tests. Four host save-integrity tests pass.
Evidence work/ability-audit-20260920/partial-bag-*. Full power-loss/failure injection,
link UI and complete game/campaign/battle/wild audit remain unfinished.

## Versioned SaveBlock3 checksum protection

Strict release build/gates pass, stampc753e372acd2. Four host save-integrity tests
also pass after extractor repair and added extension-boundary/mixed-format cases.

Resolved the earlier extension-checksum gap for newly written sectors. New normal
save-slot signature0x08022025 checksums all4084payload bytes (3968data+116extension),
including deterministic zero padding. Legacy0x08012025 retains its original logical
payload checksum and remains readable. Shared PrepareSaveSector replaces duplicate
normal/incremental preparation. Footer/layout/sector sizes are unchanged; low commit
byte0x25 is compile-time checked equal, and HOF/special-sector formats stay legacy.
Counter coherence is still enforced, while legacy/new signatures may coexist in a
valid partial-upgrade slot. Existing legacy extension corruption cannot be detected
retroactively. New sectors are not readable by older ROM builds; after both slots
are rewritten, old-ROM downgrade cannot load this save. No user save was modified.

Native baseline proves a flipped extension byte was accepted as statusOK. All5
native recovery groups now pass: extension corruption falls back with both logical
and extension state restored; independent reconstruction of14legacy sectors loads;
SAVE_LINK yields5new+9legacy sectors and loads; next full save upgrades all14; full
incremental writer retains erased0xFF commit byte until final0x25 write and loads.
Prior mixed-counter and erased-sector fallbacks remain passing. Evidence under
work/ability-audit-20260920/save-extension-*. These are disposable-flash targeted
checks, not exhaustive electrical fault/power-cut or every UI save-path coverage.
Full core/campaign/battle/wild objective remains unfinished.

## Save-slot counter coherence repair

Strict release build/gates pass, stampc8b567c6ede0.

Native disposable-flash baseline proved ValidateSaveSlot accepted all14 valid
logical IDs/checksums even when one sector carried a different save counter.
It returned SAVE_STATUS_OK instead of recovering the older complete state.
Validation now rejects counter disagreement among checksum-valid sectors. Corrupt
or unrecognized sectors do not establish the reference counter. No format,
checksum formula, sector placement or counter-wrap policy changed.

Writer scan confirms full/incremental saves use one new counter across14sectors;
partial link/record-mixing saves retain the existing counter alongside untouched
PC sectors. The recovery fixture alters only the first physical sector footer,
checks older-state fallback, repairs via full save and verifies a subsequent
SAVE_LINK reload retains its counter and updated state. Both native save-recovery
groups pass, including erased-sector fallback. Initial test stalled from a4KiB
stack-local sector exceeding harness headroom; fixed by using gSaveDataBuffer.
That stalled fixture is not production evidence; the corrected baseline fails
specifically at acceptance status1 vs255. Evidence save-counter-* under
work/ability-audit-20260920. No user save files were edited.

The separate SaveBlock3 extension-checksum gap remains open and requires a
versioned compatibility plan; this patch does not claim full save corruption or
power-interruption coverage. Full core/campaign/battle/wild objective unfinished.

## Roamer lifecycle and movement cleanup

Strict release build/gates pass, stampebd75cbbc2cc.

Read roamer.c and traced active post-League TV initialization, encounter creation,
Repel handling and battle_main return/capture retirement. No new production defect
was established. Current roamers retain their authored level40 (post-League cap
permits it), identity and moves. Native tests cover Latias/Latios, single-slot
capacity, IV/personality/shiny preservation, half-HP and paralysis/frostbite through
reencounter, 64other-set moves each, defeated-roamer full-HP/status-clear retry and
explicit deactivation. Battle-end capture-vs-defeat dispatch is source-traced,
not a replayed capture battle.

Simplified the redundant early-return branch in other-set movement, history zeroing
and location predicate. Random draws, rejection condition and route table unchanged.
All11 focused wild groups pass. Structural check confirms all20roaming sets have
>=3distinct maps, more than one starting map, every destination has a starting row,
and each route has a land/water encounter table. This checks documented loop
preconditions; it does not prove map collision reachability or full campaign balance.
Evidence: work/ability-audit-20260920/roamer-lifecycle-* and roamer-route-table.log.
Full core/campaign/battle/wild objective remains unfinished.

## Outbreak TV index bounds repair

Strict release build/gates pass, stampb1ce40c9c9f8.

PrepareTvShowForRandomOutbreak used RandomUniform(0, ARRAY_COUNT(table)) despite
inclusive upper-bound semantics, selecting index5 past a five-entry authored table.
Corrected to count-1. Native real-RNG baseline reproduced outbreakIndex>5; corrected
512draw regression covers every valid outbreak and checks species/index consistency.
Function-test RNG_NONE defaults were masking the selection: the final test copies
the runner locally, disables its forced uniform callback only around the production
TV call, and restores it before assertions. Earlier seen-mask failures came from
the harness forcing0 and are not the out-of-bounds reproduction.

StartStaticMassOutbreak now rejects invalid indices before any read/write, protecting
saved TV records, and absorbs its single-use by-value writer to remove that extra
function/copy. Native invalid indices5/255/65535 leave all active outbreak fields
unchanged; expiry still works. All10 wild-slot groups pass, including prior ordinary
slot/RNG parity, capped Repel, scripted generation and outbreak moves/identity tests.
Read-only analogous scan found no other production count-as-inclusive-bound defect.
Evidence: work/ability-audit-20260920/outbreak-index-*. Existing table species, moves,
locations, probabilities and durations unchanged. This does not prove all campaign
species availability, reachability or distribution balance. Full game audit remains
unfinished, including saved-data validation beyond this index boundary.

## Native stat-service selection boundary repair

Strict release build/gates pass, stamp5af64d759a0b.

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

## Route124 shard exchange review and consolidation

Strict release build/gates pass, stamp69fd5ae37d4d.

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

## Latest obsolete soot reward removal

Removed the uncalled Cord/Marshadow/Mega tier dispatcher and tier-buffer helper
from field_specials, their declaration, obsolete native test and verifier entry.
Special-table slots remain unused/reserved so subsequent IDs do not move; the
legacy soot receipt bit remains reserved and preserved. Live ash purchases and
local Marshadow discovery are unchanged. All99 Mega Stones still have exactly
one finite world reward source. Strict release build/gates pass, stamp528da81400a6;
soot-retirement-release-* logs under work/ability-audit-20260920. No new native
behavior test for deletion of unreachable functions. Headless research scene13
still models retired Cord prerequisites and should be updated before using that
fixture as current workshop evidence. Full game audit remains unfinished.

## Latest Lilycove gift / vending cleanup

FormerTM gifts at House2/rooftop now Pearl/Nugget with updated text and unchanged
NPCs/receipt flags/Bag retries. Rooftop vending shares variable-cost money helpers
instead of six check/debit branches; prices, capacity checks and bonus rolls stay
unchanged. Nine compiled purchase cases verify no charge on failure and exact
200/300/350 debits. All17 gift/gate groups and strict release gates pass,
stampfd8c2d2b31e9; lilycove-vending-* logs in work/ability-audit-20260920.
No visual dispensing/bonus replay. Full core/campaign/battle/wild objective and
known guard-scoring issue remain unfinished. Continue broader source audit.

## Latest Lilycove lottery repair

Restored donor lottery prizes PPUp/BottleCap/MaxRevive/MasterBall and VAR8005
assignment. Native baseline proved a two-digit win retained stalePotion instead
of PPUp. Prize/location outputs initialize on every draw; digit comparison no
longer uses global scratch. Removed stale Circuit-ticket producer, retaining its
save bit as reserved; current clerk remains daily and pending-prize retries survive
rollover. All25 Inclement integration groups and strict release gates pass,
stampdc80274580cb; lottery-* logs in work/ability-audit-20260920.
Tests construct valid preset-OT mons and cover all tiers, PC, Eggs and leading
zeros. No full clerk/overnight UI replay. Remaining Lilycove former-TM RareCandy
gifts are still candidates for coherence cleanup; full core/campaign/battle/wild
goal and known guard-scoring issue remain unfinished.

## Latest Route123 source cleanup

FormerTM19 gift is now StarPiece instead of redundant RareCandy; same Grass-party
condition, receipt flag, NPC and retry. Updated reward text. Removed seven retired
NPC entrypoints with no map/script/tool references, retaining referenced entries
and all authored battles/objects. Grass check already rejects Eggs; Mt.Pyre elder
already retries failed MagmaEmblem delivery before progression flags commit.
Strict release gates pass, stamp266f1bcf7ea5; route123-cleanup-release-* logs under
work/ability-audit-20260920. No new tests for dialogue/dead-entrypoint changes and
no visual traversal. Full core/campaign/battle/wild scope and guard-scoring issue
remain unfinished. Continue the broader audit.

## Latest Fortree / Devon Scope coherence repairs

Steven now checks DevonScope capacity before Kecleon choreography, preserving
retry if KeyItems pocket is full instead of leaving without delivering the Scope.
Native full/open pocket gate checks and all16 gift/gate groups pass. Source scan
confirms no intervening battle KeyItems insertion and consistent item/flag gates.
Fortree House2 formerTM gifts now Nugget/BigPearl; removed false SleepTalk-teaching
promise and obsolete RareCandy prize text. Same NPCs, receipt flags and placements.
Strict release gates pass, stampfa6af90990d8; fortree-coherence-* logs in
work/ability-audit-20260920. No visual bridge/Gym replay. Continue full core,
campaign, battle and wild audit; known guard-scoring issue remains open.

## Latest Route118 Magikarp reward repair

Gyaradosite receipt flag now follows successful delivery. Winners with a pending
gift can retry directly via the trainer defeated flag, without another fight or
six-Magikarp party. Unbeaten players retain the authored challenge requirements;
completed gifts remain one-time. Native defeated/received and delivery-result
branches pass alongside all16 gift/gate/Weather Institute groups. Strict release
gates pass, stampcb8d651b349c; magikarp-reward-* logs in
work/ability-audit-20260920. Adjacent Weather Institute gifts already guard receipt.
No team/placement/reward changes or visual playback. Continue the full core,
campaign, battle and wild audit; guard-scoring issue remains separately open.

## Latest New Mauville campaign pass

Wattson's former-TM24 RareCandy reward is now a Nugget, at the same NPC with the
same receipt flag/full-Bag retry. Text updated; paid-service economy gets a useful
reward alongside Leveler. Rotom defeat/capture share completion code, preserving
different messages and state6. Native outcome branches cover every outcome through
FORFEITED; other outcomes retain retry. All14 gift/gate groups and strict release
gates pass, stampbdc858731f71; new-mauville-* logs in work/ability-audit-20260920.
Source-traced key/Surf/entrance and unchanged legendary report-back prerequisites.
No roster/AI/placement changes or visual traversal. Full core/campaign/battle/wild
scope and the previously documented guard-scoring issue remain unfinished.

## Latest wild-item theft transaction repair

Native wild Thief baseline added HyperPotion to Bag but capture restoration would
recreate it. Successful extraction now retires its exact original baseline through
item provenance. StealTargetItem returns success; failed Bag insertion leaves donor
item/origin untouched and suppresses success scripts in steal/Magician/StickyBarb
callers. Pickpocket cannot enter that Bag branch. Removed redundant attacker write.
All43 focused item groups and strict release gates pass, stamp334caebf9ed2.
Logs: work/ability-audit-20260920/wild-theft-*. Full-capacity native test uses the
supported GIVE_PLAYER_ITEM inventory API; manually filled Bag was reset by harness.
Capture endpoint is directly verified after native theft; modal capture/field UI
was source-traced, not fully replayed. This closes the recorded extraction seam;
full core/campaign/battle/wild scope and guard-scoring issue remain unfinished.

## Latest held-item origin tracking repair

Native Trick→foe eats original Sitrus baseline proved restoration reminted the
player's spent Berry without Regenerator. PartyState now tracks current/used item
origin by trainer+slot. Transfer, destruction and recovery update the original
owner's durable flags; identical Berries remain distinguishable. Removed old item-
value heuristics and the unused restoration argument. No saved-data schema change.
All42 focused native groups pass: Trick/Bestow, identical swaps, recovery chains,
transferred destruction, Thief/Covet, CudChew and delayed Pickpocket origin checks.
One stale theft fixtureHP101 ate Oran before attacking under the existing+1
threshold; changed to102 with stronger item assertions, not mechanics changes.
Strict release gates pass, stamp67591383bea6; berry-origin-* logs under
work/ability-audit-20260920; failing baseline is berry-transfer-baseline-*.
Next separate candidate: wild Thief/Covet sends an item to Bag, then capture may
restore it again; native reproduction and full-Bag behavior review needed before
fixing. Full core/campaign/battle/wild goal and guard-scoring issue remain open.

## Latest original-Berry consumption tracking repair

Native CudChew baseline restored spent Sitrus without Regenerator because the
ability cleared usedHeldItem. Added a transient consumed bit in PartyState padding;
ordinary/NaturalGift consumption records it, holder recovery clears it, and new
catch-slot restoration baselines clear old Berry history. Destruction stays
separate. Native CudChew and Recycle→KnockOff controls plus second-consumption/
catch-reset function cases pass (4 groups). Strict release gates pass, stamp
8de1001a436a; berry-history-* logs in work/ability-audit-20260920.
Related worker's claim that KnockOff uses removeitem was disproved and retracted:
it directly removes/marks stolen and does not touch consumption/recycle history.
Do not reintroduce that false finding or remove the fallback on that basis.
Norman's retry/Surf license flows were source-reviewed and coherent; no script
changes there. No save-size/AI/scoring changes. Full core/campaign/battle/wild
scope and separate guard-scoring issue remain unfinished; continue the audit.

## Latest fossil revival consolidation

One fossil/species mapping now drives Devon acceptance, Bag detection and revival
for the same11 fossils. Removed duplicated switch/list and an unused early
conversion with uninitialized input. Bag-picker cancel now uses polite decline.
Native all-item-ID acceptance/conversion checks and per-fossil Bag insert/remove
checks pass with all13 combined gift/gate groups. Source traced tower choice,
opposite underpass fossil, saved revival item and full-capacity retry.
Strict release gates pass, stampf9f9ae936b6b; fossil-mapping-* logs under
work/ability-audit-20260920. No visual tower/Bag/PC replay and no reward changes.
Full core/campaign/battle/wild audit and separate guard-scoring issue remain open.

## Latest Lavaridge reward cleanup

Flannery's retry path now shares the first-time Heatranite delivery routine.
Wynaut is delivered successfully before setting its one-time flag or showing
receipt narration. Existing party-capacity/decline/repeat behavior retained;
seven native party-size cases pass, alongside all12 gift/gate groups.
Source-traced Badge4/Petalburg readiness/Goggles pending flow and Strength text.
Strict release gates pass, stamp78b89494f26f; lavaridge-receipts-* logs under
work/ability-audit-20260920. No team, placement or reward identity changes.
No visual Gym/springs replay. Continue full core/campaign/battle/wild objective;
prior guard-scoring failure and remaining audit scope are still unfinished.

## Latest Meteorite / summit campaign cleanup

Removed15 unreferenced Mt.Chimney movement streams (203 lines); no live movement
or battle edits. Meteorite removal and Cozmo thank-you messages now follow verified
receipt/handover rather than falsely announcing success with a full Bag. Existing
retry flags and reward identities remain. Source-traced MeteorFalls victory opens
cable-car route and summit win returns Cozmo home/enables vendor. No visual replay.
Strict release gates pass, stamp a6e926d23709; chimney-cleanup-release-* logs under
work/ability-audit-20260920. No new native tests for unused-code/dialogue cleanup.
Continue full core/campaign/battle/wild audit; prior guard-scoring issue and many
remaining files/interactions are still unresolved. Goal remains active.

## Latest transformed Mega eligibility repair

Native Ditto→Medicham proved eligibility and AI preview allowed a Mega that runtime
rejected. Shared target query now honors runtime's GEN5+ transformed-form rule.
Four native cases cover stone-based Medicham and move-based Rayquaza, eligibility
and preview, with unchanged party/battle records on rejection. Ordinary Mega and
PowerTrick controls still pass; all25 combined Mega/EXP groups and release gates
pass, stamp766676bf259f. Logs: work/ability-audit-20260920/transform-mega-*.
All callers use current species as no-change; preintegration has the same omission.
No scoring weights, team data or future-command access changed. This closes the
recent query/runtime candidate; full core/campaign/battle/wild goal and separate
guard-scoring failure remain unfinished. Continue broader audit.

## Latest shared battle-stat refresh cleanup

PowerTrick preservation now belongs to CopyMonLevelAndBaseStatsToBattleMon,
whose only two callers are normal level-up and form refresh. Removed duplicated
caller swaps and the level-up temporary. Transformed level-up bypasses this helper
and keeps copied stats. All24 existing Mega/EXP native groups and strict release
gates pass, stamp23c71e691bcc; stat-refresh-shared-* logs under
work/ability-audit-20260920. No behavioral retuning or new API.
Next candidate: transformed mon holding a copied species' Mega Stone may pass
CanMegaEvolve/AI_ApplyMegaForm despite runtime CanBattlerFormChange rejecting it.
Prove in a native test before changing any eligibility/forecast path. Full core,
campaign, battle and wild goal remains active and incomplete; guard-scoring
failure is still a separate unresolved issue.

## Latest Power Trick Mega/form refresh repair

Native Medicham baseline proved PowerTrick stats were lost on Mega Evolution.
RecalcBattlerStats now reapplies the swap after base-stat copying, with a shared
SpeedSwap preservation predicate. Four native cases cover normal/swapped stats
in live Mega evolution and AI_ApplyMegaForm previews; preview leaves the real
party unchanged. Source review confirms no caller double-swap and snapshot restore;
preintegration508775fad8 has the same omission. No AI code/weights/teams changed.
All24 combined Mega/EXP groups and strict release gates pass, stamp839a5e053524.
Logs: work/ability-audit-20260920/power-trick-mega-*.
Continue broader core/campaign/battle/wild audit. This closes the recently found
form-refresh issue, not the separate guard-scoring failure or whole-game scope.

## Latest Speed Swap level-up repair

Native baseline: swapped battle Speed1 reset to normal200 upon level-up with the
volatile still active. Level-up refresh now preserves swapped Speed while party
stats and other battle stats update. Normal control and all32 combined native
groups pass. Strict release gates pass, stamp850a4d3abe20; evidence under
work/ability-audit-20260920/speed-swap-level-*.
Next: reproduce PowerTrick→Mega Medicham natively. Related scan found central
RecalcBattlerStats overwrites swapped Attack/Defense without reapplying PowerTrick;
this helper also feeds form forecasts, so verify both before accepting a fix.
No AI scoring/teams changed in this pass. Full core/campaign/battle/wild goal and
the separate previously recorded guard-scoring issue remain unfinished.

## Latest Transform / Power Trick level-up repairs

Native Mew→Shuckle→PowerTrick→KO→level-up proved two defects: copied attack400
reverted to100, and battle maxHP stayed53 while party maxHP became56. PowerTrick
swap now reapplies only when non-transformed base stats are refreshed; transformed
level/currentHP/maxHP update together, preserving copied combat stats. Normal
control passes. All31 combined native groups and strict release gates pass,
stamp9663d3feafc7. Evidence: work/ability-audit-20260920/transform-level-* plus
power-trick-level-baseline-* and transform-hp-baseline-*.
Next concrete candidate from related scan: normal level-up overwrites Speed Swap
speed while retaining its volatile flag. Reproduce with a native battle, then
preserve swapped speed during refresh. No AI/teams changed. Full core/campaign/
battle/wild audit and previously documented guard-scoring issue remain incomplete.

## Latest maximum-level recipient bug fix

Cmd_getexp now validates living/non-Egg recipients before its max-level branch.
Native trainer battle baseline proved a fainted LV100 former participant earned
EVs after replacement won, while LV13 did not. Fixed; living capped14/100 still
get EVs without EXP bars/messages. Reused recipient pointer and removed redundant
checks/zero stores. All30 convenience-EXP/Inclement groups and release gates pass,
stamp 5c68a4bdad1e. Logs: exp-recipient-* in work/ability-audit-20260920.
Initial wild replacement fixture timed out; the successful trainer reproduction
replaced it, so do not classify that harness timeout as a game defect. No AI change.
Full core/campaign/battle/wild goal remains active and incomplete. Remaining broad
battle lifecycle and known guard-scoring issue are not resolved by this pass.

## Latest battle EV calculation simplification

MonGainEVs now shares yield/held-item/Pokerus math across all stats and uses explicit
capped new values instead of mixed signed/unsigned adjustments. Preserves normal
bonuses, stat order, caps and existing legacy255-to252 normalization below total
cap. Thirteen native cases pass before/after; all24 Inclement integration groups
pass afterward. Strict release gates pass, stamp e569d1f89145. Evidence:
work/ability-audit-20260920/battle-ev-*. No authored AI, teams or yield data changed.
The related compatibility arithmetic candidate is now handled without retuning.
Battle EXP recipient/share lifecycle and the remaining core/campaign/wild audit
remain unfinished; known guard-scoring issue and stat selection fallback remain
open. Continue full goal rather than treating this helper pass as completion.

## Latest native EV arithmetic repair

IncreaseChosenMonEVs baseline turned HP EV255 plus4 into3 through unsigned
underflow. Guard current/total caps before subtraction; rejected records remain
byte-identical, while valid gains retain clamping and existing report variables.
The menu precheck already rejects this input: latent API/legacy-record repair,
not a demonstrated normal-menu bug. All23 Inclement integration groups and strict
release gates pass, stamp dbd47dd7652d. Logs: ev-boundary-* under
work/ability-audit-20260920. No AI/teams/prices changed.
Related MonGainEVs legacy255-to252 normalization uses mixed arithmetic and needs
separate compatibility review; not changed. ChosenMon fallback is another pending
boundary candidate. Entire core/campaign/battle/wild goal remains active and
unfinished, including the documented guard-scoring issue.

## Latest Fallarbor Hidden Power service cleanup

Replaced sixteen redundant type-selection branches with a bounds check and direct
copy of the chosen index. The 16 spreads, three-Cap price, saved party selection
and both cancellation paths are unchanged. All22 Inclement integration groups
pass, including new compiled menu-dispatch coverage and existing actual Hidden
Power type/recipient checks. Strict release gates pass, stamp f15d15e88c45.
Evidence: work/ability-audit-20260920/stat-menu-*. No visual menu/intro replay.
Native stat services were read; fallback-to-lead and over-cap EV boundaries remain
candidates for subsequent review. Full core/campaign/battle/wild scope is active
and incomplete; the known separate guard-scoring issue remains unresolved.

## Latest glass workshop consolidation

Seven purchase branches share formatting, price check, confirmation and debit.
First delivery reuses the existing retry block. All original prices, items and
pending receipt states 10..16 are preserved. Native tests exercise 21 selection/
affordability/confirmation cases and seven retry reconstructions with scratch
variables cleared. Correct single debits; no retry debit or lifetime-soot change.
All14 combined gift/legendary groups and strict release gates pass, stamp
 a0124d156b60. Evidence: work/ability-audit-20260920/glass-orders-*.
No modal UI or visual replay. Full core/campaign/battle/wild goal remains active
and incomplete, including the previously recorded guard-scoring failure.

## Latest Route 113 soot integration repair

Restored spendable ash accumulation alongside lifetime soot; the active glass
shop had no increasing currency counter. Reused local discovery API to wire
Marshadow's existing 250-lifetime-soot quest into the glassmaker; no new item
rewards. Failed Soot Sack delivery now preserves initial receipt state. Both
counters saturate independently; spending never reduces lifetime progress.
All 18 native encounter groups pass, including the actual Route113 1% Marshadow
selector after discovery. Strict release gates pass, stamp 54aeec4340da.
Evidence: work/ability-audit-20260920/soot-integration-*. No visual ash/shop replay.
Dormant ClaimEmeraldChampionsSootMilestone/BufferEmeraldChampionsSootProgress
still contain obsolete reward tiers with no production caller; cleanup candidate,
not a reason to activate free Cord/Mega gifts. Full core/campaign/battle/wild goal
remains unfinished; known guard-scoring issue remains open.

## Latest Route 111 Heal Ball repair

The Chansey intro now checks Ball-pocket capacity before moving any actor.
A full pocket shows a retry message and preserves quest state 0 and visibility.
Successful choreography and gift are unchanged; no saved flag or placement added.
Native full/empty/partly-filled pocket cases and all ten combined campaign/gift
check groups pass. Strict release gates pass, stamp 1fd2b81e25a5; evidence is in
work/ability-audit-20260920/heal-ball-*. This fixes the previously noted lost-gift
case, not a proven quest softlock. Adjacent gift scan found no additional defect.
No visual chase replay. Continue the full core/campaign/battle/wild audit; the
broader goal and previously documented guard-scoring issue remain unfinished.

## Latest Poke Vial chain and Chansey dialogue pass

Route133 text incorrectlycalled nativeChanseyactor Audino; corrected3pages and
internalGuideChansey labels. Chase Route111/112/Jagged/Ashen speciesconsistent.
Suspected3to2downgrade disproved byRoute133 transitioncapacity2gate; experimental
helper/test removed. Newnativeassembledtransitiontest verifies1hidden/2visible/
completed3hidden; all7campaigngategroups pass. Strictrelease/gates pass stamp
57672f75d24e; work/ability-audit-20260920/vial-gate-*. Earlier vial-order-baseline
experiment isnot an unresolved playerbug: it bypassed actualstorygate.
Nextcandidate: Route111 initialHealBall gift has uncheckedBagreceipt yet advances
quest; anyHealBallworks later, soinspectlostgiftretry withoutassumingsoftlock.
Entirecore/campaign/battle/wildscope remainsactive/incomplete; guard-scoring issue
unchanged. No visualchaseplayback thispass.

## Latest Verdanturf Meadow distribution verification

No productionchange thispass. New nativepercentile tests verify Enamorus/Fez3%
each, sharedSweetScent25%, species-specific capture removal andland-only habitat
for0/1/2caughtstates. All16combinedencountergroups pass, logs meadow-distribution-*
inwork/ability-audit-20260920. Audino gift/luckyegg/PCfailure andSkarmorite replacement
source-traced. Hidden/Honey donor tables not treatedas enabledacquisition. Collision
reachability andgiftvisual/runtime notverified. Existingrelease stamp9382eadafbdb
stillmatches production. Fullcore/campaign/battle/wildgoal remainsincomplete and
active; nextcontinue regionalstory/coreinterfaces ratherthan changing validtables.

## Latest Rusturf/Verdanturf coherence cleanup

Both entrance signs now share oldpubliclabels/text and branch on tunnelopenflag.
ResthouseTunneler2 reuses existing completed-tunnel interaction afteropening.
Identical boyfriendapproach2/3 andWandaexit1/exit streams merged vialabelaliases;
commandbycommand comparison preserveschoreography. No map/actor/gift/AI changes.
Strength license+HeatBadge+anyparty dialogue sourceconfirmed; completion moves
couple tohouse andsetsstate6/openflag. Strictrelease/gates pass stamp9382eadafbdb;
work/ability-audit-20260920/rusturf-sign-release-*. No visualreplay/newnative tests
for identical-command/dialogue change. Fullgoal remainsactive/incomplete; broader
route/campaign/core/battle/wild audits andguard-scoring failure stilloutstanding.

## Latest Day Care service cleanup

Freewithdrawal reuses existing levelgain/nickname formatter; removed2costwrapper
helpers and retained zeroVAR8005. Removed3unreferenced scriptblocks and unused
text; active last-conscious-mon guard retained. Dialogue now plainly sayscareis
free instead of invented Championssettlement. Four native DayCare/giftgroups pass,
including step extremes/Hoopa/zerocost/textparity and14giftcapacity cases. Strict
release/gates pass stamp a9a4d1f518e8, work/ability-audit-20260920/daycare-cleanup-*.
Deposit/withdrawal/egg menus source-traced, not natively replayed. Core/campaign/
battle/wilddistribution scope still incomplete. Continue broader audit; no claim
of fullDayCare orRoute117 acceptance, andguard-scoring failure remains open.

## Latest Route117 daily Egg transaction pass

FirstTogepi/Extrasensory and later randomized Egg paths share one delivery block.
Capacity check remains beforegeneration; flags follow confirmed partyreceipt and
movegrant. Repeatdialogue now clearly saysreturntomorrow. Samegiftpool/location/
dailyflag andrequirement. Native14cases(first/later xparty0..6)verify species/move,
Eggstatus, receiptflags andfullparty retry; all9combinedgift/party groups pass.
Strictrelease/gates pass stamp ef63d2dadb80; work/ability-audit-20260920/daycare-gift-*.
Girl's daycare learnedmove claim agreeswith native auto-learning. No fullRoute117
NPC/visual/RTC acceptance claim. Continue route/progression and corecode audit;
allgame/battle/wildscope remains active, existingguard-scoring failure unresolved.

## Latest script move-update boundary repair

ScriptSetMonMoveSlot native baseline proves slot4 corrupts party data. Guard
invalid slot/move, explicit empty slots and empty-party last-member fallback;
retain Eggs, MOVE_NONE and >=PARTY_SIZE last-member convention. Recalculate
count only for fallback. Removed legacy BUGFIX split and updated macro docs.
Known callers are valid MysteryPichu scripts; latent API defect, not proven
campaign crash. All15 party/wild groups and strict release gates pass,
stampd545a10cb914; work/ability-audit-20260920/script-move-*.
Broader core/campaign/battle/wild scope remains unfinished. Continue source
interfaces/campaign progression; script_pokemon_util reviewed in sections but
not full native acceptance. Known guard-scoring issue remains unresolved.

## Latest scripted creation and nickname-storage cleanup

Shared single/double scripted wild mon initialization removes23duplicated lines,
preserving per-mon RNG order and clearing both opposing parties once. Native
parity exposed uninitialized stack bytes after short names in CreateBoxMon;
fixed EOS initialization there and equivalent Tower/hatch/Pyramid/partner/debug
local buffers. Names/teams/AI/rates/layouts unchanged. All15 native wild/legendary
groups pass, incl64seeded creation cases and explicit nickname tails for both
Tower constructors. Strict release/gates pass, stampbd59dc716124; logs under
work/ability-audit-20260920/scripted-wild-*. Optional facility/hatch paths source-
reviewed/built, not each played. Full goal remains active and incomplete.
Next candidate: ScriptSetMonMoveSlot fallback with emptyparty/out-of-range move
slot needs actual caller tracing before changes. Existing guard-scoring failure
and broader campaign/distribution gaps remain documented, not fixed by this pass.

## Latest visible legendary retry integration

Regigigas/Articuno/Zapdos/Mewtwo native objects used permanent legacy defeat
flags despite having separate Sign capture flags. Objects now use EC_CAUGHT
flags; four duplicate result blocks share one clear-guard/caught-or-rest handler.
Uncaught wins/escapes retreat locally and can return on reentry; captured objects
stay hidden through existing capture ledger and resume callback. Setup/levels/
held items/placements and trio requirement unchanged. Baseline Regigigas branch
regression failed; final compiled opcode and map-flag checks for4encounters x4
outcomes pass alongside party tests (13groups). Strict release gates pass,
stamp3518e59cde35; work/ability-audit-20260920/visible-retry-*.
No visual map replay. Larger visible-vs-Sign progression/level consistency still
needs review; no claim of complete legendary or whole-game acceptance. All core,
battle mechanics and wild-distribution scope remains active. Guard experiment
remains reverted with its known separate failure recorded.

## Latest Regi gate and encounter-test validation

Fixed CheckSpeciesInParty counting repeated party members instead of requested
species. Correct trio plus duplicates now succeeds; duplicates cannot replace
missing Regis, and Eggs/empty slots fail. Preserve ordered Relicanth/Wailord
puzzle. Sweet Scent test used obsolete table weights and invalid tie RNG; fixture
now enumerates actual water/land distributions and valid ties without changing
encounter rates. All18 native party/legendary/wild groups and strict release
gates pass (stamp340f364292e6). Logs: work/ability-audit-20260920/regi-scent-*.
Broader core/campaign/wild-distribution audit remains unfinished; guard-scoring
experiment remains reverted and its separately documented failure unresolved.

## Latest Cozmo/party-form repair

Native ChangeMonSpecies slotPARTY_SIZE overwrote adjacent party inbaseline.
Now bounds/target/source guards prevent invalid writes; ScriptGetPartyMonSpecies
returnsNONE for invalidslot. Cozmo checks slot beforeEgg/species reads and shares
one form-apply/message/exit block across4forms. Cancel/unknown choices exitcleanly.
Five nativeparty/form groups pass inclinvalidslots/species/Egg/empty and valid
Deoxys stats/HP/personality/EXP. No options/unlocks/cost/dialogue changes. Strict
release evidence work/ability-audit-20260920/party-form-*. ConversationUI source-
traced/compiled only, notvisuallyreplayed. Full core/campaign/wild goal remains
active; prior guard experiments stillreverted and unresolved issue documented.

## Latest Route114/Fallarbor coherence cleanup

Route114 formerTM05 gift nowBigPearl, FallarborMart formerTM60 nowNugget, Cozmo
Meteorite exchange nowStarPiece. Same existinglocations/receiptflags/fullBag
retries; redundantCozmo secondBagcheck removed. DailyRoute114Berry dialogue now
explains growing/daily supply/Regenerator. Seven no-reference retiredRoute114
script+text pairs removed; map20objects and5activebattles retained. Nancy historical
metadata reference preserved. No team/AI edits. Strictrelease andref/order checks
pass; evidence work/wild-audit-20260919/route114-rewards-release-* and storyaudit.
Source/compiled review only; nativevisual/campaign traversal still pending.
Full core/battle/campaign/wild-distribution objective remains incomplete.

## Latest rod/NPC coherence pass

GoodRod is given by Route114 object20, but code/text lived in Route118. Moved the
implementation and five texts toRoute114, keeping publicmap entrypoint and flags;
folded redundant yes branch. Good/SuperRod full-Bag retry andreceipt flow checked.
Route118 girl's Surf text nowmatches BalanceBadge+authorization/no teaching.
Dewford spareEviolite dialogue explains its actual benefit toanother team member.
No item/placement/team/AI changes. Nearby HMdialogue scan found no additional
activecontradictions. Strict release passes; rod-dialogue-release-* logs in
work/wild-audit-20260919/. Source-only interaction review, not fullmap traversal
orvisualproof. Entire core/campaign/wild-distribution goal remains incomplete.

## Latest distribution-tool cleanup and honest source inventory

Removed unused verify_wild_open_sources.py: donor/Honey references and hardcoded
sealed-map assumptions were not availability proof. No release gate depended on
it. Existing verify_wild_distribution retains its checks, gains --source-report,
and no longer crashes after reporting a missing species. Current report:633
canonical ordinary-table species references on147Hoenn headers; explicitly not
map/method access or gift/evolution/legend-overlay coverage. Structural294tables
pass; independent report equality and malformed species/level/weight checks pass.
Evidence work/wild-audit-20260919/ordinary-source-report.json,
source-validator-checks.log, source-report-release-*. One fewer support file;
no runtime/data changes. Next progression audit can use scoped source locations
rather than old690-open-species claim. Full goal and guard issue remain open.

## Latest encounter-selector simplification

Consolidated land/water/RockSmash Lure reversal and weighted selection; independent
authored threshold arrays retained. Removed unused Honey selector (actual item
uses SweetScent; source call chain verified). Net49source lines removed, no new
API/files or species/rate changes. Native3072seed odds/RNG parity passes before/
after; all9wildgroups pass. Evidence work/wild-audit-20260919/lure-*.log.
Fishing2/3/5slot groups still agree with current data/roster; picker unchanged.
Full core/campaign/wild-distribution objective remains active and incomplete;
prior guard experiments stay reverted with unresolved evidence documented.

## Latest route-roster and local-query audit

Compiled roster generation passes294 checks (147headers × caught/uncaught), max
633/1000bytes, no RNG changes or method-array excess. This is not donor-map or
rendered/progression acceptance. GetLocalWildMon supportsNULL habitat output;
two disabled follower-copy callers now useNULL explicitly. Enabled ambient cry
path unchanged. Fishing query now guardsHEADER_NONE before timed table lookup.
Native512seed parity pluspriorwildtests:9groupspass. Strictrelease evidence under
work/wild-audit-20260919/roster-*. No species/rate/placement/unlock changes.
Full core/campaign/wild-distribution goal remains active; guard experiments remain
reverted and their unresolved policy/accounting issue is still documented.

## Latest wild-level and outbreak consistency repair

Returned to wild audit while unresolved guard issue stays documented. Central
CreateWildMon now clamps to live campaign cap before stats/moves; outbreak checks
Repel against that capped level. DexNav preview cap agrees (feature disabled).
OW wild encounters also disabled: collision now preserves its already-created
visible outbreak mon and uses shared ApplyMassOutbreakMoves, avoiding raw-level
assert mismatch and identity reroll. No encounter tables/rates/placements changed.
Seven native groups pass including cap, Repel, identity/RNG, odds, Shoal andUB.
Evidence work/wild-audit-20260919/level-cap-*.log and WILD_ENCOUNTER_AUDIT.md.
Earlier guard experiments remain reverted; authored cadence unresolved, not
silently retuned. Full source/campaign/species-distribution goal remains active.

## Guard experiment rejected; verified production restored

Native denied520/banked45 trace confirmed repeated lethal-hit credit for one50HP
body. Per-recipient cap fixed originalProtect case but regressed2/8 authored
cadence tests; old source passesall8. Broader empty-cost/switch-payoff experiment
regressed3 groups and conflated noActionMask (also already-acted mons) with actual
switches. BOTH experiments reverted, all test expectations preserved. Pair source
restored byte-identically; current release stamp/artifact check PASSES.

Evidence and archived candidates: work/ability-audit-20260920/guard-accounting-*,
guard-cadence-baseline-*, pair-guard-cap-only.c, pair-guard-policy-experiment.c.
Test ELF is experimental/stale: rebuild before use. OriginalProtect/switch issue
remains. A coherent fix must separate prevented loss/pressure/partner progress,
carry actual switching state separately if needed, and preserve all8 cadence
boundaries. No production guard changes accepted this turn. Other core/campaign/
mechanics/wild-distribution work and documented seams remain unfinished.

## Latest Fake Out partner cache fix and remaining guard investigation

Fake Out had five stale chosen-slot reads in ally priority comparisons. Native
regression showed107vs101 for identical predicted action. All now use existing
aiData->partnerMove (already committed/simulated; NONE stays unknown). Full123
run:122pass/1fail, no other new failure. Normal release evidence:
work/ability-audit-20260920/fakeout-slot-release-*. Details in ability audit.

Original Protect/switch case also chooses Tackle without Weakness Policy. The
retained fixture now switches to Golurk, whose Ghost immunity makes Pikachu the
only effective Scratch target; ALL switch/Protect/HP/item assertions remain.
It still fails (protect-switch-isolated-tests.log). Do not dismiss as target
uncertainty or force green. NEXT trace guardDenied repeated credit for the same protected body. Worker
source review found each individually capped lethal hit adds180HP+80KO; two
hits count520 and subtract286 at55% unbanked share, exceeding body value180.
Need native score proof and per-target handling; do not globally cap side guards.
No Protect weight changed. Full core/campaign/mechanics/wild objective remains
active and incomplete; existing other documented seams still need coverage.

## Latest Protect partner-slot repair

battle_ai_util.c WeaknessPolicy lookup now uses already-resolved partnerMoveIndex
instead of stalechosen index during partnerprediction. Corrected native regression
passes fixedsource andfails onlyoldlookup; validates predicted/committed slots,
real item/type caches and switch exemption. Initialtest lackedcacheinitialization;
those earlier failures weren't evidence. OriginalProtect+partnerSwitch decision
remains open; no cleanaggregate122run claimed. TestELF currentlyoldlookupprobe,
so rebuild beforeusing. Normalrelease built restoredfixedsource; evidence under
work/ability-audit-20260920/protect-slot-{focused,old-lookup,release}-*.
NEXT relatedsource mismatch: four FakeOut partner-order checks in
battle_ai_main.c read chosen slot while partnerMove mayonlybesimulated. Reproduce
nativebeforeediting. Full core/campaign/mechanics/wild-distribution goal active.

## Latest strict ally-target scoring repair

HelpingHand safeHP problem was real invalidtargetscoring: foe100 vsally90, planned
foe0 thencontrollerremappedally3. ShouldConsider nowrejectsfoes forTARGET_ALLY
alongside existingUSER_OR_ALLY. Existingprocessingzerosrejectedscore; nohelper
needed. OriginalHPchoices+bothfoezeroasserts pass. Helpercommit fixture corrected
tohelperfirst/onlyHH; all271statuscases pass. Weather/terrain fixtures now specify
Tackle actions so CloseTurn cannot injectCelebrate/Taunt payoff; all16cases pass.
Full121groups120pass/1fail: remainingProtect+partnerSwitch only. Next diagnose
that fixture/decision without forcingknowledge of human target. Logs/evidence in
work/ability-audit-20260920/ally-target-final-tests.log and ally-target-release-*.
No team/weight/informationpolicy changes. Full core/campaign/mechanics/wild goal
and earlier documented untestedseams remain open; not fullgame acceptance.

## Latest Harvest/AfterYou fixture corrections

No production change. Harvest checks bonus withOran and neutral scores without
berry (Leftovers/None), instead of requiringScratch against a useful legal trade.
AfterYou now tests actual order: TrickRoom and slowMoonblast ally positive, already-
fasterMoonblast ally negative. Three cases each pass, logs harvest-fixture-tests.log
and after-you-fixture-tests.log under work/ability-audit-20260920/.
Four prior failed groups remain: HelpingHand, weather, terrain, Protect+partner
switch. No new aggregate suite run claimed. Read-only compatibility worker was
asked to inspect HelpingHand fixture assumptions. Full core/campaign/mechanics/
wild-distribution goal and documented production seams remain unfinished.

## Latest Contrary mixed-stat support correction

Target-aware EXPECT_MOVE failure diagnosed SpicyExtract on physicalContrary ally.
Generic Contrary earlybonus bypassed per-effect attacking-stat loss. Mixed target
raises+drops now consult existing ally scorer first; pure lowering unchanged.
Originalnegative and new special-only/pureCharm controls pass. Full121groups now
115pass/6fail, no newfailures; contrary-comparison.json listsremaining6. Strict
release evidence under work/ability-audit-20260920/contrary-release-*.
Related source-only nextseam: blanket stat-drop rejection against Contraryfoe may
misvalue mixed changes; reproduce beforeediting. Self-lowering effects excluded
by MoveHasAdditionalEffect, so Superpower/HammerArm aren't analogous bugs.
Full core/campaign/battle-mechanics/wild-distribution goal remains unfinished.

## Latest status-orb trade correction

OfferedFlame/ToxicOrb scoring now checks both prospective holders using existing
self-status-benefit helpers: avoid empowering foeGuts/PoisonHeal and retain own
useful orb untilstatus active; preserve harmful-orb shedding andKlutzFlame logic.
Native baseline newcase failed102vs100; new8case matrix plusold6cases nowpass.
Full120groups113pass/7fail, no newfailures; orb-comparison.json listsremaining7.
Native/release evidence in work/ability-audit-20260920/orb-*. Teams, scoring
constants and knowledgepermissions unchanged. Next remaining groups include
Harvest trade ties, HelpingHand/AfterYou, terrain/weather andSpicyExtract.
Full core/campaign/battle-mechanics/wild-distribution goal remains active and
unfinished; ABILITY_MECHANICS_AUDIT.md records exact coverage and remainingseams.

## Latest Umbrella recipient-cost follow-through

Two added score regression groups reproduced harmful gift100/100 and harmful
theft101/100 vsScratch, then passed after existing-WEAK costs were applied to
opposingDrySkin sun protection / restored opposingChlorophyll orSwiftSwim.
AirLock neutral control passes; old20Umbrella cases unchanged. Full119groups:
111pass/8fail, same8remaining failures (umbrella-seams-comparison.json). Native
and strictrelease evidence in work/ability-audit-20260920/umbrella-seams-*.
NEXT analogous source finding: offeredFlame/ToxicOrb shedding reward omits
recipient status benefits. Reproduce Guts/PoisonHeal case before changing it.
No authoredteam/knowledge/scoringconstant changes. Full game goal still active;
core/campaign/wild-distribution audit and documented other mechanics seams open.

## Latest Utility Umbrella scoring correction

battle_ai_main.c now charges existing-WEAK_EFFECT for surrendering DrySkin sun
protection or receiving Umbrella that suppresses one's own active weather ability.
Same-Umbrella exchanges retain protection; positive trade rewards unchanged.
Existing9gift/11receive cases pass unchanged. Full117 groups now109pass/8fail,
no new failed groups; umbrella-comparison.json lists remaining8. Strict release
logs umbrella-release-build.log/umbrella-release-gates.log record integration.
NEXT related cases: gifting Umbrella protects opposingDrySkin in sun; stealing
one can restore opponent weather-benefit ability. Source review only so far.
RemainingHarvest/orb neutral-score ties and6 other groups need triage, not forced
green. ABILITY_MECHANICS_AUDIT.md records details. Full core, battle mechanics,
campaign and wild-distribution goal remains active and incomplete.

## Latest stat-drop/recoil fixture corrections

No production changes. Stat-drop six cases now pass after synthetic WonderGuard
isolates the effective Tackle/IceBeam target; late drop may retain next-turn value,
so allow Strength/drop while retaining threatened allyProtect/fullHP and correct
recipient damage assertions. Recoil three cases pass: explicit5..6 recoil range,
mandatoryProtect at5HP, attack at7, either risk choice at6 with exact native HP/
item/faint receipts. Temporary traces removed. Logs drop-final-tests.log and
recoil-final-tests.log in work/ability-audit-20260920/; explanation in
ABILITY_MECHANICS_AUDIT.md. Two of prior12 groups resolved; remaining10 need
triage, no new aggregate suite claim. Full core/campaign/wild-distribution audit
and documented production probability/other-flank seams remain unfinished.

## Latest priority-order correction

Defeatist inversion traced exactly to PairPlanScore's-45 redundant-priority
penalty based only on Speed. Native opinions were equal112; prediction correctly
kept60HP withQuickAttack vs48Acrobatics, but penalty outweighed12HP benefit.
Exempt only known usable damaging priority responses from the target whose
priority can be beaten; retain-45 otherwise and all downstream plan scoring.
No committed-command reads, weights or teams changed. Four Defeatist cases pass,
including ordinaryTackle control. Full117 groups now105pass/12fail, no new failed
groups; PlusMinus also passes. priority-comparison.json lists remaining12.
Temporary traces removed. ABILITY_MECHANICS_AUDIT.md details evidence/limits.
NEXT: investigate remaining12 failures and target-other-flank priority case;
sleep probability seams and full core/campaign/wild-distribution goal unfinished.

## Latest target-uncertainty diagnosis

RockTomb/Cloak test now passes both cases unchanged AI: synthetic Smeargle became
HisuianZorua, preserving StrengthSTAB and manual stats while Ghost immunity leaves
Oranguru sole effective Tackle target. Original move/HP/survival assertions retained.
Prior claim of proven mechanics defect retracted: traces show fair mixed target
forecasts caused the original tactical mismatch.
Defeatist passive partner now Duskull to similarly isolate QuickAttack target;
60% still fails. Native trace proves damage endpoints22healthy/12weakened and
incoming12 are modeled, but Acrobatics->Eevee scores136 vsQuickAttack103. NEXT:
trace isolated native move opinion/overkill weighting before retuning shared AI.
Temporary tracing removed; production AI unchanged this turn. Details/evidence in
ABILITY_MECHANICS_AUDIT.md and work/ability-audit-20260920/*scores.log,
defeatist-isolated-trace.log, rock-tomb-final-tests.log. Trace+RockTomb resolved
among16 groups; no new aggregate run. Entire goal remains active and incomplete.

## Latest remaining-AI-test triage

Trace candidate loader test now uses an explicit native turn before cache/snapshot
checks, so voluntary AI switching cannot prevent its actual assertions. All four
cases pass, including added mixed-foe Trace uncertainty. No production changes.
Defeatist40% fixture now allows either surviving KO;60% retains its exact threshold
assertion and FAILS (Acrobatics vsQuickAttack). Initial failure previously hid it.
NEXT trace60% Defeatist and RockTomb/Cloak chosen targets/pair values. Do not assume
later parameters passed. Only Trace is resolved among16 broader failures; full
suite not repeated for this fixture-only edit. Details ABILITY_MECHANICS_AUDIT.md,
work/ability-audit-20260920/trace-fixture-tests.log and defeatist-fixture-tests.log.
Full core/campaign/wild-distribution objective remains active and incomplete.

## Latest sleep-targeting repair and baseline comparison

Imposter Spore ally targeting is repaired with two narrow battle_ai_pair.c changes:
cache ally sleep effects and publish sleep probability even after target acted.
Existing horizon weights/teams/knowledge unchanged; source matched508775fad8
before the repair. Four-case native regression covers both flags and ordinary
Smeargle/Imposter. Broader117-group A/B: baseline100pass/17fail, patch101pass/16fail,
no new failing group. work/ability-audit-20260920/sleep-comparison.json lists all
remaining failures. Investigate those rather than accepting or weakening tests.
ABILITY_MECHANICS_AUDIT.md records root cause and remaining probability seams.
Full core/campaign/wild-distribution goal remains active and incomplete.

## Latest ability audit and open Imposter reproduction

Commander/Yawn attachment fix passes 43 groups; four stale fixture assumptions
corrected, with all 38 Berserk/Pickpocket/Unseen Fist groups passing. Strict normal
release and gates pass. ABILITY_MECHANICS_AUDIT.md records limits and evidence.
NEXT: isolated Imposter doubles Spore wrongly targets Wimpod ally with both old
and actual campaign expert flags. test/battle/ability/imposter.c retains both
profiles, current first; this is not Billy's actual lead lineup. Trace selection
versus controller/execution and compare pre-integration before touching AI.
No AI/team edits made. Full core/campaign/wild-distribution goal remains active.

## Latest move-resolution verification

Sixteen non-Dynamax Encore groups pass. Added six native doubles Pressure cases
covering opposing vs allied abilities, depletion floor and battle/party PP sync;
all pass. No production/AI changes. MOVE_RESOLUTION_AUDIT.md and
work/move-restrictions-20260920/ record scope/evidence. Full goal still incomplete.

## Latest pending-palette ownership fixes

Native removal test caught freeing unowned placeholder palette0. Teardown now
preserves pending through cleanup; dynamic/generic refresh skip borrowed frees
and clear pending only after successful binding. Static-graphic retry stays hidden
until its requested palette is available. Six palette test groups pass together;
work/palette-removal-20260920/ and FIELD_EFFECT_AUDIT.md record evidence. Full
visual/campaign traversal still unfinished; broader goal remains active.

## Latest restored-object palette recovery

Fixed the prior moved-object reset limitation: return-to-field palette exhaustion
keeps the active object and hidden sprite, tracks retry in a runtime16-bit mask,
and retains coordinates/identity. Shared movement continues logically; graphics
and ground effects wait for the palette. Clear/removal resets the pending bit.
Native moved-object failure/retry plus dynamic variant tests pass. Evidence:
work/palette-recovery-20260920/, details FIELD_EFFECT_AUDIT.md. Save layout and AI
unchanged; full campaign/core goal remains active and incomplete.

## Latest dynamic Pokémon palette repair

Fixed variant cache tags and normal/shiny/female pointer checks. Dynamic creation
rejects failed palettes before lookup/sprite creation; refresh shares one helper
and retains a valid shared old palette on allocation failure. Four native groups
pass (dynamic2/object1/reflection1), evidence work/dynamic-palette-20260920/.
IMPORTANT remaining exhaustion recovery issue: inactive map-object retry uses its
template, potentially resetting transient moved-object state. Preserve/recover
that state before claiming full-fidelity palette failure handling. Details in
FIELD_EFFECT_AUDIT.md. Full goal active; no AI/save-layout changes.

## Latest generic object palette repair

UpdateSpritePalette preserves existing OAM index and reports0xFF when allocation
fails; skips needless reload of the same palette and preserves shared ownership.
Native full-capacity/retry/reuse/release plus reflection regression pass. Evidence
work/object-palette-20260920/. NEXT still required: LoadDynamicFollowerPalette and
its creation/refresh callers' failure propagation. No claim all palette paths fixed.

## Latest reflection palette follow-through

Pond/ice and high-bridge palette loaders reject failed allocations, preserve valid
OAM index and report failure. Reflection init/update hides unavailable reflections
and retries; duplicate normal palette code removed. Native three-kind failure/
retry and generic field palette regressions pass. work/reflection-audit-20260920/
contains evidence. NEXT: dynamic follower and generic object palette callers in
event_object_movement.c still need failure propagation. Full game goal active.

## Latest field-effect palette repair

Failed faded-palette allocations no longer modify unrelated weather palettes;
script operand advancement remains correct. Shared weather updater rejects invalid
OBJ slots. Native exhausted/reused-slot tests pass; logs work/field-effect-audit-20260920/.
Next confirmed related work: reflection/follower/object palette callers can still
truncate0xFF to OAM slot15; fix caller fallback/hiding in field_effect_helpers.c
and event_object_movement.c. FIELD_EFFECT_AUDIT.md records scope. Full goal active.

## Latest avatar state coverage

Shared coordinate read and corrected Surf authorization comment. Three native
groups pass25 movement-mode combinations,18 gender/presentation mappings, and
four facing-coordinate directions. See PLAYER_AVATAR_AUDIT.md and
work/avatar-audit-20260920/. Full traversal and overall game audit remain incomplete.

## Latest field routing pass

Whole-structure input reset replaces per-field clearing; redundant forced-move
condition removed. Native compiled coordinate routing passes for Littleroot,
Rustboro and New Mauville shared tiles, including nonmatching states. See
FIELD_ROUTING_AUDIT.md and work/field-routing-20260920/. Story gates/AI unchanged;
full campaign and core audit remain incomplete.

## Latest indicator lifecycle repair

Shared indicator lookup consistently skips Safari's absent indicator0 and
non-live IDs. Native test reproduces/prevents sprite0 mutation and checks valid
priority/level-offset updates. Source confirms Safari reachability; no fresh UI
replay claimed. Evidence work/indicator-audit-20260920/, details appended to
MEGA_MECHANICS_AUDIT.md. Battle mechanics/AI and full-goal scope unchanged.

## Latest Mega mechanics coverage

All 18 declared native Mega groups pass, including104 added/form cases and
switch/faint/revive/order/permission checks. Two stale Speed expectations now
use explicit Level100/IV31 and236 Speed; production mechanics and AI unchanged.
docs/MEGA_MECHANICS_AUDIT.md separates fixture evidence from remaining release UI
and campaign tactical acceptance. Logs: work/mega-audit-20260920/.

## Latest Pokenav dispatch cleanup

Menu IDs are bounds-checked before callback-table access; retired Match Call
remains blocked. Removed two pass-through wrappers. Native invalid/retired-ID
checks pass; Start-menu reload cancel source-traced. See POKENAV_DISPATCH_AUDIT.md
and work/pokenav-audit-20260920/. Broader UI/native traversal remains incomplete.

## Latest event-state hardening

Saved/special/test flag and variable accessors now reject out-of-array IDs.
Valid boundaries/literal VarGet behavior remain intact. Two API and six campaign
gate groups pass; docs/EVENT_DATA_AUDIT.md records the malformed-ID scope and
work/event-data-audit-20260920/ contains build evidence. No active invalid script
operand found; AI/save layout unchanged. Full-game goal remains incomplete.

## Latest Flight Beacon pass

Refusal text now names both Feather Badge and Fly authorization. Native home/
Ever Grande destination checks pass; Bag/registered-item cancel callback and
rider override cleanup source-traced. docs/FLIGHT_BEACON_AUDIT.md distinguishes
that from unperformed UI traversal. Evidence: work/fly-audit-20260919/.
Full-game/core goal remains active and incomplete.

## Latest field-move integration consolidation

HM classification shares the existing license table; no duplicate eight-HM list.
Four native unlock/message/puzzle groups pass. See docs/FIELD_MOVE_AUDIT.md and
work/hm-audit-20260919/ for release evidence and unverified obstacle/choreography
coverage. Full-game audit remains active and incomplete; AI/access rules unchanged.

## Latest finale coherence pass

S.S. Tidal cabin dialogue now addresses the Champion's doubles challenge. The
existing former-TM49 gift is one Big Pearl instead of redundant Rare Candy, with
the same receipt flag/full-Bag retry. Removed unused retired battle text and
shared Cleanup Brothers response. Source coverage is in STORY_GATE_AUDIT.md;
release logs in work/finale-coherence-20260919/. No authored battle/AI changes.

## Latest trade handoff consolidation

Cable/wireless NPC animation paths share FinishInGameTrade using VAR_0x8004;
removed redundant byte-copy wrapper. Native selected-slot handoff tests pass.
See docs/TRADE_AUDIT.md for unverified PC-evolution/Mail/link-protocol seams and
work/trade-audit-20260919/ for release evidence. Full goal remains incomplete.

## Latest evolution scene pass

Field/trade scenes share CommitEvolution. Removed dangling canceled-state guard
around trade task destruction (latent: no current production trade-cancel entry).
Native commit and cleanup regressions pass; evidence and scope are in
EVOLUTION_SCENE_AUDIT.md and work/evolution-audit-20260919/. Full goal incomplete.

## Latest stat-change mechanics repair

Guard Dog now converts Intimidate into +1 even at minimum Attack and still
blocks the drop at maximum. Native minimum/neutral/maximum cases pass; doubles
Defiant/Competitive test and release evidence are under work/stat-audit-20260919/.
See docs/STAT_CHANGE_AUDIT.md. AI untouched; full-game/core goal remains incomplete.

## Latest new-game reset repair

NewGameInitData explicitly clears harvest credits and garden-Celebi progress,
including direct scene-preparation entry. Native dirty-state regression passes
while preserving name/options; duplicate includes/party clear removed. Evidence
in docs/NEW_GAME_AUDIT.md and work/new-game-audit-20260919/. Full goal active.

## Latest save/load pass

Two physical save-slot validation scans now share one helper, preserving format
and recovery policy. Native damaged-newest-slot fallback/repair and six-group
legendary state roundtrip pass. docs/SAVE_LOAD_AUDIT.md records the remaining
SaveBlock3 checksum-coverage concern; do not alter format without compatibility
planning. Evidence: work/save-audit-20260919/. Full core/game goal still incomplete.

## Latest freeze mechanics repair

Actual freeze infliction incorrectly initialized elapsed actions to2 and forced
immediate thaw. It now initializes0; native inflicted-freeze regression and all
18 existing/new Freeze groups pass. Lum resets the count like Aspear; separate
cure regression is in work/freeze-audit-20260919/cure.log. Release evidence shares
that directory. Other status cleanup paths need later lifecycle review, although
normal reinfliction now resets their dormant counters. AI is unchanged.

## Latest held-item mechanics repair

Leppa now restores active copied PP while preserving permanent Transform/Mimic
PP. Ordinary moves still synchronize to party state. Native before/after logs
and copied-move/Ripen/Regenerator cases are in work/leppa-audit-20260919/.
See docs/HELD_ITEM_MECHANICS_AUDIT.md; AI untouched, full mechanics goal incomplete.

## Latest Ultra Beast acquisition verification

All ten special native sources have compiled land-style tables and exactly 3%
ordinary selector allocation when eligible; captured species are excluded. Native
Poipole evolution requires Dragon Pulse, present in its level-one learnset. Two
native groups pass; docs/ULTRA_BEAST_ACCESS_AUDIT.md separates this from unfinished
map gate timing/balance review. No production changes or new release build in this
pass. Core and battle-mechanics audit remain unfinished; goal stays active.

## Latest guaranteed encounter repair

Shoal's charted sighting now commits only after encounter creation, preserving
it when Repel/lead ability rejects the selected resident. Native before/after
regression passes; work/shoal-sighting-20260919/ contains release logs and an
Ultra Beast acquisition-source inventory (ten sign sources, zero ordinary-table
slots). Next distribution work: map access and progression timing for these
sources, plus Poipole-to-Naganadel evolution. Full core/mechanics goal remains active.

## Latest wild encounter pass

Land/water selection and wildlife-report helpers share probability thresholds;
Sweet Scent reuses those arrays. 2,048 seeded native comparisons preserve slots
and RNG progression. All 7,001 table level ranges are valid; source export of early
routes is in work/wild-audit-20260919/early-route-tables.json. No distribution
changes yet; access, rare species/Ultra Beast timing and balance remain to audit.
See docs/WILD_ENCOUNTER_AUDIT.md; full goal remains active and incomplete.

## Latest Berry capacity pass and expanded goal

Battle mechanics and wild Pokemon distribution are explicitly in scope.
GetFreeSpaceForItemInBag now follows the same single-stack Berry rule as insertion;
normal multi-stack items remain unchanged. Evidence: work/berry-capacity-20260919/.
Harvest failure dispatch is now repaired; see docs/BERRY_HARVEST_AUDIT.md.

## Latest item transactions pass

PC quantity checks now aggregate split stacks through the shared Bag helper;
RemovePCItem compacts after depletion and rejects invalid removal inputs.
Removed duplicate campaign handoff compaction. Four native regression groups pass;
docs/ITEM_TRANSACTIONS_AUDIT.md and work/item-audit-20260919/ contain evidence.
Core/campaign goal remains active, with remaining coverage in the ledger.

## Latest script-engine pass

See docs/SCRIPT_ENGINE_AUDIT.md: complete context reset, all 20 return slots,
shared byte readers, and expanded Aurora Ticket receipt prerequisite coverage.
Three native script regressions pass; campaign and release logs are under
work/script-audit-20260919/. Core goal remains active and incomplete.

## Active full-core refactor goal

The app goal is active and now matches the user's full-core audit/refactor request.
Use docs/CORE_REFACTOR_PROGRESS.md for all 49 files and keep campaign/finale work
in scope. Latest Day Care pass consolidates preview/withdrawal normalization and
fixes overflow and Hoopa-form preview mismatch. Two new native tests plus five
Nursery groups pass; docs/DAYCARE_AUDIT.md records coverage and remaining work.
An event-only script test macro dependency was fixed with identical command bytes.
Release logs: work/daycare-audit-20260919/. Do not claim full goal completion.

## Latest storage pass and source inventory

See docs/STORAGE_SYSTEM_AUDIT.md for the release-count bug, minimal cleanup and
native before/after evidence. docs/GAME_SOURCE_INVENTORY.md lists 49 central C
files by system; docs/GAME_CODE_FILE_LIST.txt inventories broader text sources.
The core shortlist is not enough to rebuild without the rest of the repository.
Storage release checks are in work/storage-audit-20260919/; broad audit unfinished.

## Latest large-file bug hunt

Party-menu item/Mail/form/lifetime corrections and native regression evidence
are in docs/PARTY_MENU_AUDIT.md. Three native tests, strict release compilation and all deterministic release gates
pass; evidence is in work/party-audit-20260919/.
This is a scoped pass, not completion of the largest files or campaign audit.
AI sources remain unchanged. Preserve the concurrent move-learning-state fix.

## Latest AI clarification and finale status — September 19

The user accepts full opposing moveset knowledge and requires preserving the
shared authored AI that all 368 battle entries were tuned around. Do not confuse
loadout knowledge with reading a pending command. Restored the exact original
EXPERT_AI_PROFILE and special-format flags (OMNISCIENT + KNOW_OPPONENT_PARTY).
The public-forecast experiment is parked under work/finale-20260919/parked-public-forecast
and fully removed from active source. Main/pair/util/switch decision-path review
found no opponent-scoring reads of committed player moves, actions, targets or
switch destinations; shared command snapshots only preserve state, and partner
coordination reads concern the AI's own teammate. No full behavioral proof claimed.

Current local finale: League → exact original Wally team → five S.S. Tidal
cabin teams → Steven/Aurora Ticket receipt → Birth Island puzzle and Deoxys
(defeat or catch) → Buffel. New teams for four boat battles and upgraded Buffel
remain provisional. The Regenerator, ordinary rematch retirement, menu cleanup,
and earlier campaign fixes remain intact. All changes remain uncommitted on main
following b677fd82ea. After profile restoration, the native test build and all eight focused finale/AI
knowledge tests pass (work/finale-20260919/restored-ai-tests.log). All 368 generated
authored entries retain both original knowledge flags, with no move/switch/incoming
prediction flags. Roster/rematch/source-materialization checks pass. A fresh normal
release ROM and finale tactical acceptance still remain. Prior reduced-knowledge tactical runs are diagnostic,
not acceptance evidence for the restored profile. Full campaign audit is unfinished.

# Current handoff — gate-by-gate story audit

The user authorized merging the integration and current fixes into main. The
integration landed at51bb2b2f68; continue solo source review on main. The latest user retains one Sol Low worker for analogous-bug scans and
explicitly assigned straightforward fixes; main handles complex changes. Do not resume
broad campaign-playthrough workers. Verify the current GitHub
Actions build rather than treating the historical commit as a release guarantee.

A fresh CI checkout exposed missing recipes for two Japanese contest composite
sheets. graphics_file_rules.mk now rebuilds them from tracked PNG sources. The
regenerated sheets match the previous local graphics byte-for-byte.

The battle_util.c scoped pass is now recorded in docs/BATTLE_UTIL_AUDIT.md.
New local changes after b677fd82ea remove pre-battle restoration-state writes
from starter/field gifts, guard absent battle context, and simplify equivalent
hazard checks. Four native test groups and release gates PASS; these new changes
are not yet committed/pushed. Speed-sort cleanup is already on main.
The broader utility/core and map-by-map coherence review remains incomplete.

## Current design

Inclement Emerald is the world/story/progression baseline. Keep the authored
Champions doubles teams and AI, universal free legal-move tutor/no TMs, native
ability switcher and Inclement EV/IV services. Keep early Eviolite, Choice items
and Focus Sash, paid discovery stock, and postbattle non-berry item restoration.
New direction: spent held Berries return only after receiving the Regenerator
Key Item from Norman after Badge5. Repeat Norman dialogue supports existing
saves and failed gift delivery. Battle mechanics do not regenerate midfight.
HMs need only their story unlock and badge, with no party compatibility or move
slot requirement. Preserve location checks and legendary puzzle conditions.
Leveler raises the party to each current species cap. XP uses normal modern
full/half shares without custom flat/catch-up bonuses; capped recipients receive
no XP award, message or animation. No new overworld item placements: only replace
original Inclement items or TM gifts. No Game Book or player-facing guides.

## Workflow and completed passes

Current direction: audit the entire story gate by gate, including all NPC dialogue
in each reached area. Main owns review and fixes; confirmed bugs trigger bounded
Sol Low scans for related instances elsewhere. Refactor only where meaningful.
Previous size-ordered audits are retained evidence, not a completed story review.

- Title banner regenerated using vanilla references and inspected in the game.
- HM/Leveler/XP changes built and verified with focused engine checks.
- event_object_movement.c: see docs/OBJECT_MOVEMENT_AUDIT.md.
- battle_script_commands.c: see docs/BATTLE_SCRIPT_AUDIT.md.
- Current story pass: docs/STORY_GATE_AUDIT.md. Opening fixes and bounded
  native region/pair checks completed. Petalburg initial-visit source pass is
  recorded; Route104/Woods source and flower-shop gift pass is recorded; next area is
  Rustboro/Roxanne/Devon/Route116/Rusturf. Entire
  campaign, postgame and all-NPC review remains unfinished.

Authored AI/team files are unchanged against pre-integration508775fad8. The rejected
guard-forecast experiment was removed from active source and parked locally;
its experimental batch11 ROM must not be delivered. All current source is intended
for the authorized merge; old experimental/rewrite branches are not part of it.

## Evidence and artifacts

Latest build/evidence: work/regenerator-20260919.
Regenerator policy native tests pass; Norman gift/claim is source-reviewed.
Previous Route104/Woods batch: work/route104-woods-audit-20260919.
New native flower-shop bundle regression passed.
Earlier opening regression evidence: work/story-gate-audit-20260919.
New opening/dialogue fixes remain local and uncommitted.
Earlier convenience ROM remains on Desktop; it predates the two file audits.
Prior ROMs and earned saves remain preserved. Never cross-load raw savestates
between builds. Local work/ folders contain detailed before/after evidence.
Merge validation and remote-verification receipts: work/main-merge-20260919.
# Battle setup owner flags — September 21, 2026

Confirmed native regression: SetBattledTrainersFlags treated opponentB=0xFFFF
(partner versus one trainer) as a real trainer. u16 flag arithmetic wraps to
0x4FF, now FLAG_EC_GIFT_FORTREE_CITY_HOUSE2, suppressing its Big Pearl gift.
It also marked a stale opponentB in single-owner battles. Only actual two-owner
battles with a nonzero/non-sentinel B now set B's defeated flag. Added five
native cases; before logs reproduce both defects, after suite passes all 86
groups. Evidence: work/battle-setup-20260921/{before-tests,sentinel-before-tests,
after-tests}.log. PARAMETRIZE_LABEL is necessary for generic TEST cases here:
the included battle header overrides bare PARAMETRIZE for battle-test state.

Replaced the small zeroed transient TrainerGenerator allocation with equivalent
local storage; removed identity CampaignPrizeMultiplier and obsolete prize
history comments. No authored teams, prize values or AI changed. Existing saves
cannot distinguish an erroneously set Fortree flag from a genuinely claimed
gift, so this does not reset earned gift flags or claim to repair old saves.
Battle setup review remains partial, including facility/rematch edge cases.
