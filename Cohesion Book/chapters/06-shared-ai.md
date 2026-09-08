# 06 — Shared expert AI and tactical execution

**Authority:** these are the final shared AI specifications used by every authored trainer, including the new opening rival and explicitly scripted rescue. Individual battle chapters name required scenarios; they do not each install another trainer-specific scoring patch. Difficulty settings change levels, not access to these tactical capabilities.

The current engine already contains substantial damage, status, switching, partner and field logic. Preserve successful behavior. The source review identified specific defects and limitations below; the book has not executed a new native battle build or proven mathematical optimality. Every repair needs a reproducing fixture and relevant negative control before implementation is called verified.

## Common information and decision contract

Use one explicit expert campaign profile. Opponents may know team species, moves, items and abilities through that profile, but must not read the player's unexecuted chosen-command buffer as foreknowledge. Current transformed/suppressed abilities, consumed items, PP, status and field state override stale assumptions. Keep the observation contract consistent across trainer classes and difficulty modes. Document this game rule in the optional battle guide rather than claim exact equivalence to another live ladder's information rules.

Champions Points use the formula and fixed effective IV31 in chapter 02. Compare priority before Speed: Spore already precedes Trick Room by priority, while Speed investment can determine whether the sleeper acts before the setter's attacking partner. Ability/item/field changes can alter that order. Never substitute a conventional EV or zero-Speed-IV calculation.

### Integration sequence

1. SAI-03 and SAI-11: trustworthy candidate state and current-hit damage.
2. SAI-02: truthful roll/crit forecasts.
3. SAI-01 and SAI-09: shared capabilities and one owner per mechanic.
4. SAI-06: real base/Mega form evaluation and owner budgets.
5. SAI-04/05/07/08/10: reserve, paired-action, ally-activation and field decisions on those reliable inputs.
6. SAI-12: preserve existing working cases and validate the actual authored matchups.

Do not introduce a monolithic replacement AI or a parallel external battle simulator. Reuse native move, stat, ability, form and field primitives with compact scratch state. Do not copy entire save blocks or allocate a new heap object for every candidate.

## SAI-01 — One expert campaign profile

**Disposition: REPAIR.**

**Source evidence**

- [scripts/implement_emerald_champions_master_battles.py:39](../baseline/source/scripts/implement_emerald_champions_master_battles.py#L39)
- [include/constants/battle_ai.h:49](../baseline/source/include/constants/battle_ai.h#L49)
- [src/battle_ai_util.c:201](../baseline/source/src/battle_ai_util.c#L201)
- [src/battle_ai_main.c:321](../baseline/source/src/battle_ai_main.c#L321)

`scripts/implement_emerald_champions_master_battles.py:39` defines different sharp/master profiles. `include/constants/battle_ai.h:49` makes Smart Trainer include omniscience, smart switching/choices and randomized switch selection. `src/battle_ai_util.c:201` makes awareness global when an omniscient flag is present. `src/battle_ai_main.c:321` assigns per-trainer and prediction flags.

**Assessment**

Sharp already has substantial doubles intelligence. The difference is partly knowledge and composite side effects, not a simple poor/good AI distinction. Class-dependent access no longer fits the user's common expert standard.

**Final specification**

Keep sharp/master names as compatibility aliases if useful, but generate the same explicit campaign list: Basic Trainer, Omniscient, Smart Switching, Smart Mon Choices, Prediction, PP Stall Prevention, HP Aware, Try To 2HKO, Powerful Status and Know Opponent Party. Continue automatic Double Battle. Omit Smart Tera in the Mega-only game and do not inherit a category-wide randomized switch lottery accidentally. Full known sets do not imply knowledge of committed actions or immunity to stale state: current items, transformed/suppressed abilities, PP and volatiles remain authoritative. Difficulty modes change levels, not tactical capabilities or knowledge.

**Implementation bound**

One generated flag expression; no new inference engine.

**Acceptance evidence**

Identical boards/loadouts with ordinary and leader IDs produce identical base scores/knowledge. Changing only the unrevealed player action buffer does not change the observation snapshot. Preserve separate multi-owner parties and common tactical capabilities across difficulty modes.

**Limits**

This is requirement-driven unification, not proof sharp was primitive. This chapter owns the final information contract and its player-facing description.

## SAI-02 — Truthful damage and critical probabilities

**Disposition: REPAIR.**

**Source evidence**

- [src/battle_ai_util.c:112](../baseline/source/src/battle_ai_util.c#L112)
- [include/config/ai.h:109](../baseline/source/include/config/ai.h#L109)
- [src/battle_ai_util.c:860](../baseline/source/src/battle_ai_util.c#L860)
- [src/battle_ai_main.c:6206](../baseline/source/src/battle_ai_main.c#L6206)

`src/battle_ai_util.c:112` makes Risky assume maximum outgoing/minimum incoming damage and Conservative the reverse in attack, defense and switch contexts. `include/config/ai.h:109` defaults ordinary attack forecasts to maximum damage. `src/battle_ai_util.c:860` lets Risky treat a sufficiently high but non-guaranteed critical stage as certain. `src/battle_ai_main.c:6206` adds independent risk bonuses.

**Assessment**

Risk appetite currently changes the forecast itself. A possible high-roll knockout can be treated as certain, and critical uncertainty can disappear from the cache. That is different from rationally choosing risk when safer lines lose.

**Final specification**

Remove personality-dependent damage/crit certainty. Cache honest current-hit minimum, median and maximum outcomes. Guaranteed damage KOs require minimum damage meeting actual HP and valid hit/ability conditions; maximum-only KOs are possibilities. Rank close choices using real accuracy and critical probability. Derive the 16 standard rolls from one base damage context plus native post-roll modifiers when a threshold needs exact probability. Always-crit moves and true guaranteed stages remain guaranteed, subject to native prevention. Remove unconditional bonuses that double-count already evaluated power/risk; retain preferences only between effectively equivalent lines.

**Implementation bound**

Exact-roll expansion only for threshold-sensitive shortlisted moves; cache results. Counterfactual evaluation must not consume gameplay RNG.

**Acceptance evidence**

High-roll-only KO versus guaranteed accurate KO; non-guaranteed high crit versus guaranteed Wicked Blow; changed Risky/Conservative flags leaving identical cached ranges; recoil, multihit, Sash/Sturdy, fixed-Points order and resist-Berry controls.

**Limits**

The biased branches are proven source facts; not every current risky choice is wrong. Preserve useful diversity through genuine evaluated tradeoffs.

## SAI-03 — Transactional switch-in state

**Disposition: REPAIR.**

**Source evidence**

- [src/battle_ai_switch.c:74](../baseline/source/src/battle_ai_switch.c#L74)
- [src/battle_ai_switch.c:2708](../baseline/source/src/battle_ai_switch.c#L2708)
- [src/battle_ai_switch.c:2861](../baseline/source/src/battle_ai_switch.c#L2861)
- [src/battle_ai_switch.c:2468](../baseline/source/src/battle_ai_switch.c#L2468)

`src/battle_ai_switch.c:74` calls `SetBattlerStatStagesForSwitchin` and `SetBattlerHPChangeForSwitch` inside the loop over every other living battler. `src/battle_ai_switch.c:2708` applies self Intrepid Sword/Dauntless Shield/Download, seeds and Web changes. `src/battle_ai_switch.c:2861` subtracts hazards and applies item healing. `src/battle_ai_switch.c:2468` saves BattleMons outside the complete candidate loop and restores after it.

**Assessment**

Doubles can apply self entry effects and hazards up to three times. Targeted entry-effect helpers are also called with an ally as the nominal opponent. Opponent stat changes from one candidate can persist into later candidate trials. Candidate order can therefore influence the simulated board.

**Final specification**

Capture one immutable baseline before choosing. Restore it before and after every candidate, including early returns. Split entry into native-ordered phases: form/types/ability and candidate field; hazards once; self/item entry effects once with consumption and once-per-battle state; opponent effects once per actual foe; then damage/accuracy/order against each actor. Do not apply entry work inside the per-target damage loop. Use the supplied candidate terrain consistently. Clamp stages with native helpers. Restore BattleMons, AI caches, party indices, field proposals, item state and gimmick markers between candidates.

**Implementation bound**

One reusable scratch snapshot allocated outside at-most-six candidate trials. This removes repeated entry work rather than adding search.

**Acceptance evidence**

Same incoming mon receives one hazard application and one self boost regardless of other actors alive. Reverse candidate order without changing any candidate result. Intimidate affects valid foes, never its ally. Check Intrepid Sword activation history, Download, Room Service, terrain seeds, Web, White Herb, Defiant/Competitive/Contrary and item healing.

**Limits**

The loop placement and accumulation are direct code facts. First reproduce exact native observable failures with positive/negative fixtures; this audit did not execute them.

## SAI-04 — Actual doubles reserve selection

**Disposition: OPTIMIZATION.**

**Source evidence**

- [src/battle_ai_switch.c:2621](../baseline/source/src/battle_ai_switch.c#L2621)
- [src/battle_ai_switch.c:2468](../baseline/source/src/battle_ai_switch.c#L2468)
- [src/battle_ai_util.c:6334](../baseline/source/src/battle_ai_util.c#L6334)
- [src/battle_ai_switch.c:1073](../baseline/source/src/battle_ai_switch.c#L1073)

`src/battle_ai_switch.c:2621` explicitly gates integrated Smart Mon Choices with `!IsDoubleBattle()`. `src/battle_ai_switch.c:2468` selects from a vanilla path evaluating one opposite foe and preferring any Baton Pass holder before matchup/damage. `src/battle_ai_util.c:6334` selects one opposite living foe. `src/battle_ai_switch.c:1073` disables Wish passing in doubles.

**Assessment**

Flags alone do not give campaign doubles the integrated selector. A reserve can be good against one foe while losing to the other or harming its partner. Simply removing the singles guard would transplant an inappropriate one-on-one evaluator.

**Final specification**

After SAI-03, add a bounded doubles branch in the current selector. Evaluate every legal reserve against both foes and retained ally; include the current-turn incoming attacks for voluntary switches and the next actionable board for forced/free switches. Include hazards, field replacement, Choice-locked ally spread damage, healing, useful support and speed order. Retain a stay-in candidate. Reject illegal candidates; prioritize actual winning outcomes, then survival and joint knockout/preservation value before secondary damage/support and tie preferences. A necessary winning sacrifice must not be rejected merely for losing HP. If both slots need replacements, compare distinct owner-valid pairs. Value Commander pairing, Palafin activation opportunity and Wish recipient timing as actual board effects; do not reward merely knowing Baton Pass.

**Implementation bound**

At most six single candidates or bounded distinct owner-valid replacement pairs, with shared damage caches. No multi-turn tree.

**Acceptance evidence**

Boomburst-locked Exploud chooses a viable Telepathy partner over avoidable Sylveon damage; Palafin activates while a viable reserve remains; Commander pair is deployed coherently; a one-foe resistance loses to a candidate that survives both; Wish heals the actual entering occupant only if it survives until resolution. Swapping left/right opponents does not change equivalent tactical conclusions.

**Limits**

This improves a confirmed limited selector; it does not establish every current replacement is bad. Preserve authored leads and existing valid switch rules.

## SAI-05 — Soft ace preservation

**Disposition: REPAIR.**

**Source evidence**

- [src/battle_ai_switch.c:240](../baseline/source/src/battle_ai_switch.c#L240)
- [src/battle_ai_switch.c:1398](../baseline/source/src/battle_ai_switch.c#L1398)
- [src/battle_ai_switch.c:2498](../baseline/source/src/battle_ai_switch.c#L2498)
- [src/battle_ai_main.c:550](../baseline/source/src/battle_ai_main.c#L550)

`src/battle_ai_switch.c:240` identifies the last slot or last two slots as hard aces. They are excluded from eligibility at `src/battle_ai_switch.c:1398`, skipped in vanilla selection at `src/battle_ai_switch.c:2498`, and skipped by fallback voluntary switching at `src/battle_ai_main.c:550`.

**Assessment**

The named traits prevent selection before tactical evaluation. They can hide the sole winning reserve, delay a needed support/activation partner or preserve a final-slot Pokémon different from the narrated Mega ace. E0488 Owen's final slot is Victreebel, while Mega Lucario Z is slot 4.

**Final specification**

Keep ace classification only as preference metadata. Remove hard ace exclusions from actual campaign eligibility and fallback. After substantive board evaluation, prefer retaining a healthy signature reserve on equivalent lines. Permit immediate ace entry when it is the only viable defense, completes a stronger combination or secures victory. Preserve authored leads. Audit all `IsAceMon` consumers, including integrated selection and revival; a small revival preference may remain if it cannot override a losing result. Mega allocation is separate from positional ace labeling.

**Implementation bound**

No extra candidate search beyond SAI-04; remove exclusion filters and add a final tie preference.

**Acceptance evidence**

Sole-surviving-answer ace enters despite ordinary reserves; equivalent safe choices preserve the intended dramatic finish; Double Ace does not block required support; forced switches and multi ownership remain valid.

**Limits**

This follows the user's optimal expert standard. It does not mean saving an ace is always wrong.

## SAI-06 — Real base-versus-Mega candidates and budget

**Disposition: REPAIR.**

**Source evidence**

- [src/battle_ai_main.c:597](../baseline/source/src/battle_ai_main.c#L597)
- [src/battle_ai_main.c:458](../baseline/source/src/battle_ai_main.c#L458)
- [src/battle_ai_util.c:944](../baseline/source/src/battle_ai_util.c#L944)
- [src/battle_gimmick.c:67](../baseline/source/src/battle_gimmick.c#L67)
- [src/battle_ai_main.c:747](../baseline/source/src/battle_ai_main.c#L747)
- [src/battle_gimmick.c:103](../baseline/source/src/battle_gimmick.c#L103)
- [src/battle_util.c:8569](../baseline/source/src/battle_util.c#L8569)
- [src/battle_controllers.c:3299](../baseline/source/src/battle_controllers.c#L3299)

`src/battle_ai_main.c:597` starts move selection with USE_GIMMICK. `src/battle_ai_main.c:458` only reconsiders Z/Tera, not Mega. `src/battle_ai_util.c:944` toggles a gimmick marker, while `src/battle_gimmick.c:67` only writes an enum and does not transform species/stats/ability. `src/battle_ai_main.c:747` caches base ability/Speed. `src/battle_gimmick.c:103` scopes actual consumption by trainer ownership. `src/battle_util.c:8569` permits item- and move-based Mega eligibility. `src/battle_controllers.c:3299` commits RET_GIMMICK.

**Assessment**

The inspected path does not compare base versus Mega and marker-only toggling does not create the transformed actor. This misses benefits such as Huge Power/No Guard and losses such as Sand Rush/Triage/Prankster. Steven's Dragon Ascent Rayquaza is genuinely Mega-eligible despite holding Focus Sash and competes with Metagross.

**Final specification**

Trial a base actor and actual native Mega form transactionally. Resolve species, ability slot, types, fixed-IV31 flat-Points/Nature stats, HP handling and on-transformation ability/field effects. Recalculate damage, accuracy, priority, Speed and partner effects in the same candidate state. Use native form/stat primitives on scratch data and restore their touched globals; do not run visuals, consume real items or commit a budget during simulation. For one owner compare legal no-Mega/left-Mega/right-Mega choices; multi owners retain separate native budgets. Include bounded reserve opportunity cost. Prefer Steven's Metagross on equivalent lines, but allow a necessary winning Rayquaza Mega. Commit only after a coherent move/target pair is selected; declining Mega must invalidate any form-dependent cache.

**Implementation bound**

At most three active form scenarios for a shared owner, with reusable cached base/Mega actors and bounded reserve comparison. No external optimizer.

**Acceptance evidence**

Excadrill retains Sand Rush when needed and uses Piercing Drill when better; Meganium/Meowstic lose Triage/Prankster after Mega; Raichu Y gains reciprocal No Guard; physical Starmie gains Huge Power; Victreebel/Clefable/Scolipede lose Chlorophyll/Unaware/Speed Boost correctly; Steven and E0409 obey one actual activation per native owner. All simulation state is restored.

**Limits**

Source establishes the missing comparison and marker-only path. Native controller/form integration still needs focused fixtures before claiming an observed failure or successful repair.

## SAI-07 — Concrete paired actions

**Disposition: OPTIMIZATION.**

**Source evidence**

- [src/battle_ai_main.c:482](../baseline/source/src/battle_ai_main.c#L482)
- [src/battle_ai_main.c:1029](../baseline/source/src/battle_ai_main.c#L1029)
- [src/battle_ai_main.c:3290](../baseline/source/src/battle_ai_main.c#L3290)

`src/battle_ai_main.c:482` simulates a partner but stores only its move name, discarding the selected target. `src/battle_ai_main.c:1029` chooses per-actor move/target scores. `src/battle_ai_main.c:3290` evaluates Helping Hand using best available partner damage rather than a finalized paired outcome.

**Assessment**

The existing partner simulation is valuable but lacks a complete joint action and final consistency pass. Support can be valued against a move or target that changes when the second actor finalizes. Speculative Protect requires different treatment from committed protection.

**Final specification**

Carry an explicit candidate tuple: action kind, move, target, switch recipient, Mega choice and speculative/committed status. Enumerate the legal one-turn move/target choices and owner-valid switches for each active actor, then compare their valid pairs under the form scenarios from SAI-06. Reuse native per-action damage/effect caches; recompute partner payoff for the actual pair and order. Before controller output reject duplicate reserve claims, wasted support with no payoff, incompatible field changes, avoidable ally knockouts and duplicate same-owner Mega activation. Terminal winning lines outrank preserving HP or dramatic aces; assess uncertain outcomes honestly. Preserve recognized support/enabling moves even when their independent score is low. Counterfactual evaluation does not consume gameplay RNG; final genuine ties may use the existing decision seed. No fixed two-candidate cap is adopted.

**Implementation bound**

One turn only. The current four-move/four-actor/at-most-six-party model has a small finite action set; use a32-entry bound per actor with checked construction, at most1024pair checks per legal form scenario, and cache expensive calculations. Actual legal counts are normally lower. No recursive multi-turn tree or external simulator. Profile on target; any pruning needs measured justification and regression coverage for enabling actions.

**Acceptance evidence**

Helping Hand crosses a real KO threshold but earns no fake damage beside Protect/Room; Follow Me supports setup but gains no protection credit against two spread-only or bypassing attacks; Beat Up/Justified and Instruct work in either internal actor order; Surf/Protect choices remain coherent after both finalizations.

**Limits**

This is a bounded optimization to an incomplete representation, not proof every existing paired action is wrong. Preserve the useful current support heuristics and their negative controls.

**Lead-editor reconciliation:** Final book avoids a fixed two-action shortlist that could omit the enabling move. Evaluate the bounded full one-turn legal action-pair set using shared caches; profile before considering any pruning.

## SAI-08 — Deliberate ally activation

**Disposition: OPTIMIZATION.**

**Source evidence**

- [src/battle_ai_main.c:3228](../baseline/source/src/battle_ai_main.c#L3228)
- [src/battle_ai_util.c:5775](../baseline/source/src/battle_ai_util.c#L5775)
- [src/battle_ai_main.c:3470](../baseline/source/src/battle_ai_main.c#L3470)
- [src/battle_ai_main.c:3670](../baseline/source/src/battle_ai_main.c#L3670)

`src/battle_ai_main.c:3228` defines `isFriendlyFireOK` through survival and repeated-hit thresholds. `src/battle_ai_util.c:5775` changes those thresholds with personality. Policy bonuses at `src/battle_ai_main.c:3470` and Steam Engine scoring near `src/battle_ai_main.c:3670` depend on this condition.

**Assessment**

Generic protection against careless ally damage is good. A repeated-hit threshold cannot fully evaluate a one-time activation that spends HP to gain a winning same-turn action. Merely possessing an activation item also does not justify fatal opposing follow-up damage.

**Final specification**

Keep generic friendly-fire penalties. For a recognized useful activation, trial exact HP loss, protection/absorption, item consumption, stage caps, ability activation and changed same-turn order. Include incoming actions before the beneficiary can exploit it. Under Room, additional Speed can be harmful. Permit the activation only when its paired resulting board beats available safe lines. Reject repeated/capped activations without useful state change. Preserve a genuine winning ally-sacrifice line only when the actual outcome warrants it. Use shared effect logic for Policy/Steam Engine/Justified/Anger Point/healing absorption; never a Tabitha-only score exception.

**Implementation bound**

Reuse one shortlisted paired post-activation state and targeted recalculation; do not relax all ally-damage checks.

**Acceptance evidence**

E0337 actual Dragapult/Coalossal at real levels/Points supports a safe winning Surf line and rejects one where Coalossal dies before acting. Rain, current HP, consumed Policy, capped boosts, Protect and one-foe spread modifiers alter the result correctly. Volcanion is compatible; arbitrary grounded reserves are not immune. Preserve existing positive/negative Beat Up controls.

**Limits**

No new Tabitha battle was played. Native evidence must decide whether Surf actually wins a particular board.

## SAI-09 — Shared mechanics instead of trainer-ID privilege

**Disposition: REPAIR.**

**Source evidence**

- [src/emerald_champions_ai.c:141](../baseline/source/src/emerald_champions_ai.c#L141)
- [src/battle_ai_main.c:326](../baseline/source/src/battle_ai_main.c#L326)
- [src/battle_ai_main.c:3916](../baseline/source/src/battle_ai_main.c#L3916)
- [src/battle_ai_main.c:2191](../baseline/source/src/battle_ai_main.c#L2191)

`src/emerald_champions_ai.c:141` maps selected trainer IDs to five tactic handlers. `src/battle_ai_main.c:326` chooses the global automatic callback from opponentA. Shared After You and Follow Me logic already exists at `src/battle_ai_main.c:3916` and `src/battle_ai_main.c:2191`. Consumer search finds these AI_EC functions in the dispatcher, header and native tests, not current map-script calls.

**Assessment**

Extra Room discipline, Eruption/After You support, Truant targeting, snow screens and redirection are general competitive mechanics. The user requires their full capability for ordinary trainers too.

**Final specification**

Migrate useful behavior into the existing shared effect cases, then remove the trainer-ID dispatch and redundant helpers once consumers/tests are updated. Never score both paths. Room discipline joins shared field-transition logic. After You compares actual order improvement rather than requiring Eruption or an arbitrary HP threshold. Truant/Entrainment considers legal targets, Gas, Substitute and actual current threat rather than only raw Attack/SpA. Snow screens/redirection consider actual usable actions and committed partner payoff. Retain generic script-set dynamic hook infrastructure where it remains a supported engine API; this migration does not require removing unrelated scripting capability.

**Implementation bound**

Consolidate into current effect dispatch; remove duplicate bonuses. No handler per trainer or new tactical catalog.

**Acceptance evidence**

The same Lilligant/Torkoal board works for Flannery, Darcy and an ordinary trainer. Equivalent redirection pairs receive equivalent mechanics. Two-setter Room refresh still requires two living usable setters with compatible final actions. Existing Quincy/snow/redirection/After You tests become full shared-AI behavior tests without losing negative controls.

**Limits**

The current handlers are targeted and often sound. Their restricted distribution is the issue; retain their good behavior rather than wholesale replacing the engine.

## SAI-10 — Consistent field and screen evaluation

**Disposition: REPAIR.**

**Source evidence**

- [src/battle_ai_field_statuses.c:66](../baseline/source/src/battle_ai_field_statuses.c#L66)
- [src/battle_ai_field_statuses.c:102](../baseline/source/src/battle_ai_field_statuses.c#L102)
- [src/battle_ai_field_statuses.c:456](../baseline/source/src/battle_ai_field_statuses.c#L456)
- [src/battle_ai_util.c:4122](../baseline/source/src/battle_ai_util.c#L4122)
- [src/battle_ai_field_statuses.c:506](../baseline/source/src/battle_ai_field_statuses.c#L506)

`src/battle_ai_field_statuses.c:66` returns weather-positive or weather-negative when either ally has that result, so a mixed pair can satisfy both. Terrain evaluation at `src/battle_ai_field_statuses.c:102` identifies actor/partner explicitly. Psychic Terrain heuristics at `src/battle_ai_field_statuses.c:456` use ability families. `src/battle_ai_util.c:4122` rejects screens merely because a foe knows a screen-breaking move. Room's live-Speed/usable-priority handling at `src/battle_ai_field_statuses.c:506` is worth preserving.

**Assessment**

Preserve the specifically identified working behaviors while implementing isolated changes around them.

**Final specification**

Compute one FieldEffectOutcome per candidate transition; ShouldSet/ShouldClear inspect that single result. Identify actor by ID. The minimal consistency fix follows documented actor-first, neutral-partner fallback; shortlisted paired outcomes can refine qualitative ties using actual joint value. Compare both allies and foes under the proposed field: Speed crossings, move targets, grounded effects, weather accuracy, sleep compatibility, healing and residual damage. Do not treat Prankster Tailwind or allied Triage healing as blocked by Psychic Terrain. Do not treat Electric Terrain as curing existing sleep. Screen-breaking knowledge is a risk term: only usable, relevant, correctly timed breaking actions should remove the forecast screen benefit. Preserve finite timers and existing accurate Room checks.

**Implementation bound**

One compact transition context inside shortlisted pair/Mega evaluation. Do not enumerate every possible future field state.

**Acceptance evidence**

Conflicting sun/rain allies produce one order-invariant outcome. Psychic Terrain leaves self-side Tailwind and valid allied healing usable. Cynthia's Miraidon weighs remaining Roserade sleep value. Zero-PP or Choice-locked Brick Break does not veto Reflect, while a real later breaker matters. Glacia's Veil requires snow, respects finite duration and is not redundantly stacked.

**Limits**

Contradictory weather results are a direct defect; additional action-awareness changes are improvements requiring focused native cases.

## SAI-11 — Separate first-hit damage from two-hit Berry planning

**Disposition: REPAIR.**

**Source evidence**

- [src/battle_ai_util.c:885](../baseline/source/src/battle_ai_util.c#L885)
- [src/battle_ai_util.c:1366](../baseline/source/src/battle_ai_util.c#L1366)
- [src/battle_ai_main.c:4115](../baseline/source/src/battle_ai_main.c#L4115)
- [src/battle_ai_main.c:4129](../baseline/source/src/battle_ai_main.c#L4129)

`src/battle_ai_util.c:885` returns unmitigated damage when the resisted first hit plus future unmitigated hits would KO. `src/battle_ai_util.c:1366` feeds that shared cache into hit-count/KO consumers. `src/battle_ai_main.c:4115` reads cached minimum as guaranteed damage. `src/battle_ai_main.c:4129` adds a resist-Berry comparison preference but does not restore honest damage for other consumers.

**Assessment**

A 100 HP target taking60 through a resist Berry, with120 unmitigated damage, can satisfy the two-hit condition and receive120 in its current-hit cache. Generic consumers can then see a nonexistent immediate KO. The comparison flag only helps some relative move choices.

**Final specification**

Always retain the actual Berry-mitigated current-hit range. Evaluate explicit two-hit planning separately: native first-hit reduction/consumption, followed by a second hit under the resulting item state. Only the sequence scorer uses that total. Immediate KO, survival, partner activation and Mega choice must read truthful current damage. Handle Ripen, Unnerve, item suppression and multihit consumption timing through native rules, not a blanket doubling shortcut.

**Implementation bound**

Store or derive second-hit data only for Berry-affected shortlisted actions; avoid a new permanent full distribution matrix.

**Acceptance evidence**

The 60/120 versus100HP case is not an immediate KO but remains a two-hit sequence; a truly lethal first hit still gets credit. Include Ripen, multihit and suppressed-item controls. Original meaningful move-comparison tests retain their contracts.

**Limits**

The representation issue and arithmetic counterexample are source-backed. Build actual species/move fixtures that reproduce the behavior before implementing the repair.

## SAI-12 — Preserve existing good work and bound verification

**Disposition: KEEP.**

**Source evidence**

- [src/battle_ai_switch.c:830](../baseline/source/src/battle_ai_switch.c#L830)
- [src/battle_ai_switch.c:1047](../baseline/source/src/battle_ai_switch.c#L1047)
- [src/battle_ai_main.c:1300](../baseline/source/src/battle_ai_main.c#L1300)
- [src/battle_ai_main.c:827](../baseline/source/src/battle_ai_main.c#L827)
- [src/battle_ai_util.c:2293](../baseline/source/src/battle_ai_util.c#L2293)
- [src/battle_util.c:5529](../baseline/source/src/battle_util.c#L5529)

`src/battle_ai_switch.c:830` checks Palafin pivot targets, weather, absorbers and order; `src/battle_ai_switch.c:1047` already chooses a hard switch when safe pivoting is unavailable. `src/battle_ai_main.c:1300` handles current-versus-previous Instruct timing. `src/battle_ai_main.c:827` clears every cached move slot before skipping unusable moves. `src/battle_ai_util.c:2293` validates two-setter Room refresh. `src/battle_util.c:5529` deliberately rounds Sitrus's half-HP trigger up.

**Assessment**

Preserve the specifically identified working behaviors while implementing isolated changes around them.

**Final specification**

Preserve these behaviors and meaningful existing tests. The Sitrus rule is why Jack's existing item remains: Medium 68 Linoone has 207 HP, Drum leaves 104, Sitrus activates at 104, while Figy/Gluttony requires 103. Do not normalize authored Points implicitly to make an unnecessary item swap work. Keep actual activation/eligibility checks for Palafin, Instruct, Room and ally protection. Build on the engine rather than replace its successful teams or duplicate its native mechanics in an external simulator.

**Implementation bound**

Measure the current native baseline with AIDebugTimerStart/End before adding loops, then establish a concrete cycle/frame envelope. Every algorithm above is bounded; compare worst-case multi-owner, two-Mega-candidate and two-replacement scenarios, not just easy turns.

**Acceptance evidence**

Run audited targeted native mechanisms first, then representative authored battles and worst-case paired/Mega turns. Record source stamp, ELF hash, effective levels, exact Points, outcomes and cycle/frame cost. Compile-only and autowin traversal remain distinct from AI execution and competitive battle evidence. Counterfactual evaluation must restore real state and avoid gameplay-RNG changes.

**Limits**

Historical tests are not fresh proof for a revised engine. This documentation phase produced no native battle results or build artifacts.

## Authored teams remain the consumers

The battle volumes reproduce full baseline and final teams. Their specific examples include Falinks/Gallade ordering, Fake Out plus setup, Room setters, Instruct, ally Surf/Steam Engine, snow screens, Palafin activation, Choice-locked friendly spread damage and actual Mega abilities. Test the intended board and an interrupted board; do not prove a combo only by forcing the desired commands.

The newly restored Walrein, Dusclops and Nosepass need purposeful decisions with their partners. The revised Huge Power Mega Starmie requires actual transformed Attack in AI forecasts. Rayquaza's move-based Mega eligibility must be considered alongside Steven's Metagross; one stone is not proof that only one Pokémon can Mega. Zygarde's Complete transition and Eternal Floette's specific Mega eligibility are also part of the final acquisition/team reconciliation.

## Verification discipline

Audit any existing test that disagrees: its current contract, configured forms, exact Points/natures/levels, fixture Speed/HP, branch setup, and source/artifact freshness. Preserve real invariants; update or retire obsolete assumptions. A score-number difference alone does not establish worse play. Conversely, a source comparison or scripted forced move is not proof the AI independently chooses the right action.

Use a focused set of production-function and native action-selection tests covering the distinct failures above, followed by actual representative authored battles. Measure decision latency and peak scratch memory at doubles, full-party, dual-owner and multiple-Mega-candidate boundaries. Re-run only when a change or unresolved concern justifies it; no historical test-count floor is added.

