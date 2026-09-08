# Coordination audit through Victory Road and the League

September 5, 2026. This continues the coordinated-gym and route-pacing work. The campaign remains intentionally difficult. No species, individual Pokémon level, global difficulty offset, level cap, or retired route battle was changed by the coordination pass.

## Scope and outcome

The structural scan covers all 516 retained trainer branches. Detailed review focused on Victory Road, the Elite Four, Wallace, and flagged late-campaign partnerships and field interactions. It does not claim that every possible board state has been simulated or that a full competitive playthrough is complete.

**51 trainer branches across 46 physical encounters were revised.** The same 782 showcased species and all Mega/legendary showcase requirements remain. Existing roster sizes and each Pokémon's level are preserved; some leads and reserves were reordered intentionally.

The audit checks actual moves, abilities, items, lead order, reserve roles, and field effects. Existing authoring prose was revised where it described a plan the moves did not perform. Static legality is useful evidence, but does not establish good coordination on its own.

## Victory Road and League highlights

| Encounter | Coordination change |
|---|---|
| Wally | Togekiss and a Swords Dance Garchomp open together; Ground coverage is partner-safe, and Mega Gallade occupies the reserved ace slot. |
| Hope | Lilligant offers Helping Hand instead of an After You move with little payoff for the lead. |
| Quincy | Slaking starts beside Neutralizing Gas Weezing and uses High Horsepower rather than damaging its own enabler. Durant's Truant-transfer role remains a later phase. |
| Katelynn | Victini's spread Fire attack no longer damages or burns its own partners, including Fluffy Bewear. |
| Shannon | Ribombee supplies immediate Tailwind beside Mega Emboar instead of keeping delayed switch-in speed control in reserve. |
| Michelle | Triage Mega Meganium starts beside an attacking Machamp rather than another passive healer. |
| Mitchell | Marowak uses Bonemerang without damaging the grounded Trick Room core. |
| Vito | Flygon retains mixed Dragon/Ground pressure and Tailwind without friendly Ground damage. |
| Caroline | The sand setter starts beside Sand Rush Stoutland; Dugtrio's Ground coverage is safe beside Tyranitar and Eternatus. |
| Felix | Jynx's Fake Out and Xerneas's Power Herb setup are paired in the opening. |
| Dianne | Snow Warning and Aurora Veil are paired from the start, with Mega Barbaracle correctly reserved as the finisher. |
| Sidney | Yveltal trades Assault Vest for an executable Tailwind support role and a Dark-boosting item. Incineroar helps create that window. |
| Phoebe | Giratina's burns and Icy Wind support Lunala's Calm Mind and Spectrier's Hex. The Ghost team retains an attrition/speed-reduction identity rather than becoming another Tailwind team. |
| Glacia | Articuno can restore finite snow after it expires or is overwritten. The original strong Veil/Kyurem opening remains. |
| Drake | Mega Garchomp can use Ground coverage alongside the grounded legendary partners without hitting them. |
| Wallace | Milotic supports attacks instead of attempting Hypnosis under its own Misty Terrain; Ferrothorn's Steel damage no longer declines with allied Icy Wind. |

Other changes fix late gym, rival, route, and villain-team interactions. The broadcast finale now gives Noivern and Gardevoir Telepathy so they can stand beside Exploud's Boomburst. Exploud retains a safe sound alternative beside Sylveon. The twins' competing weather setters no longer cancel each other immediately in the opening. Several defensive cores replace friendly-fire Water attacks or ally-resetting Haze with attacks/support that work beside their partners.

Not every spread attack was removed. All-airborne Earthquake teams remain, as do deliberate Water Absorb/Storm Drain lines and Cramorant's Gulp Missile access. A conditional tradeoff is not automatically a defect. Unchanged teams were not given cosmetic edits just to make the audit look larger.

## Native tests and their boundaries

- The dynamic-AI test file imports the real generated campaign table because the standard battle harness substitutes mock trainers. Campaign-mon setup now includes the authored stat points as well as species, level, ability, nature, item and moves.
- The Quincy opening regression checks that the real lead pair permits Slaking to attack on consecutive turns.
- A Wally probe exposed a shared AI defect: CheckBadMove deducted 20 points from Follow Me whenever the partner chose any status move. It now permits covering setup/recovery while retaining the penalty beside single-Pokémon protection. A controlled native regression fails on the old rule (80 versus the expected 100) and passes on the corrected rule, including the Protect countercase. It does not force an entire team to follow one scripted opening.
- Wallace's now-unused Hypnosis-specific scoring override was removed. A generic Misty Terrain regression remains.
- The formerly empty Telepathy test now has positive and negative controls: allied Boomburst is blocked only with Telepathy, and enemy Boomburst still damages the target.
- Native template validation checks the Chansey graphics used by five actual compiled map objects. Original references fail; corrected references pass.
- Exact authoring/materialization, ability legality, runtime-coherence, encounter-quality, route-retirement and release checks remain in use. They are evidence for their stated contracts, not proof of perfect difficulty.

The complete before/after loadout ledger is `work/coordination-through-league-2026-09-05/team-changes.json`.

## Playthrough visual supervision

The separate Astra Medium playthrough task now captures and actually inspects screenshots at map transitions, important interactions and before/during/after choreography. A 15-minute heartbeat checks its status and visual-review evidence and samples new images. It stays quiet when there is nothing actionable and should stop monitoring when campaign validation is complete.

Independent review opened the Mauville house exit and Route111 trainer approach frames, then confirmed a genuine white-screen failure near the Route111 nurse. The playthrough task correctly stopped instead of counting it as a passing transition.

The defect was an FRLG-only Chansey graphics slot referenced by Emerald maps. Route111, Route112, JaggedPass, Route133 and AshenWoods now use the existing Emerald species-graphics mechanism. Positions, movement, flags and scene scripts remain unchanged. The restoration sources were corrected too, preventing them from reinstalling the invalid references. Reviewed visual baselines preserve their original-source records and accept only the deliberate current graphics changes.

The corrected Route111 replay shows Chansey rendering, jumping, walking away, the nurse dialogue and Heal Ball receipt, then a clear controllable field. The parent task independently opened the before, motion and final screenshots. No white screen, missing actor or clipping was visible in those samples. Other maps have native graphics-reference coverage; they have not all been visually replayed yet.

The playthrough also found an input-driver issue: facing a direction already held could send another movement input and trigger a scene early. The idempotent face command and automatic intermediate screenshot capture are now in the canonical pipeline. Four focused observation tests pass, including rejecting blank final frames and corrupt PNGs while accepting legitimate intermediate fade frames. Actual visual review remains separate from image validation.

Visual evidence and the continuing review log live under `work/campaign-playthrough/resume-2026-09-05/tuned/`. Local copies of the confirmed before/corrected images are alongside the team-change ledger.

This is ongoing world validation. Traversal autowin does not measure combat difficulty, and sampled frames do not prove unobserved choreography.

## Final validation

The isolated native release build and every configured release gate pass. **86 selected native tests pass**, including the 67-test doubles-AI group, focused coordination/ability tests, five AI-knowledge tests in the built snapshot, the five-map Chansey graphics regression, and exhaustive preset application checks. This is a selected suite, not a claim that the entire upstream test corpus was run.

The parent independently reviewed the corrected Chansey motion/final frames and Fallarbor entry. The playthrough continues beyond Fallarbor with recurring supervision. Its latest details remain in the playthrough task's visual-review log.

Validated ROM/ELF, content stamp and logs are exported under `release/2026-09-05-league-coordination/`. The build snapshot is `/private/tmp/ec-coordination-through-league-build-0905`; shared work can continue independently without changing this artifact's identity.

## Required imported/QoL scene revisits

The user's final steering explicitly prioritizes every Inclement-imported or QoL-modified scene. Forward traversal alone does not close these checks:

- Pokémon Centers: actual exits and return warps, including Lavaridge's hot-springs door; full nurse movement, Poké Ball placement, machine animation and return to control. Inspect party-size cases where the animation differs.
- Dewford Gym: blocker position, visibility and doorway access at the relevant story states, plus dialogue explaining the next destination.
- Steven, Briney, Dewford and Slateport handoffs: compare source-defined order with what the player is told and what becomes accessible before/after each event.
- Imported/restored locations and QoL services: compare pinned Inclement source/history with actual before/during/after frames and post-event collision/access.
- Revisit earlier high-risk scenes that only have final screenshots or semantic/autowin evidence. Do not mark unobserved choreography passed.

The playthrough task and recurring monitor both carry this priority. Lavaridge's hot-springs warp exists in current map data, but its visible doorway and traversal still require the explicit visual/runtime check; data presence alone is not approval.
