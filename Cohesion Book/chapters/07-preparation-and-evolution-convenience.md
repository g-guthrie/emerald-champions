# 07 — Preparation, move access and evolution convenience

This chapter preserves the existing expanded-game preparation freedom. It repairs broken or misleading connections between the builder, evolution, field access and generated presets without imposing a new ladder-legality purge or a progression curriculum.

## PREP-02 — Let the Leveler finish ready evolutions at the cap

### Current source

`src/item_use.c:CanLevelPartyToCap` and `src/party_menu.c:FindNextLevelerSlot` only consider non-Egg Pokémon below the current cap. By contrast, `ItemUseCB_RareCandy` already checks `GetEvolutionTargetSpecies(...EVO_MODE_NORMAL...CHECK_EVO)` when leveling has no effect, and can perform a ready evolution without increasing level. Thus an appropriate Rare Candy can evolve a ready level 100 Pokémon; the convenient Leveler entry path can still skip it.

This is a convenience/routing defect, not proof that every capped evolution is impossible. It affects practical use of newly learned evolution moves, friendship readiness, held evolution items and newly reached map conditions. The snapshot's Meltan, Bisharp, regional starters and Type:Null are relevant examples.

### Final specification

Introduce one shared predicate for Leveler eligibility: nonempty, non-Egg Pokémon that is either below the cap or has a ready **normal** evolution under the current map/time/item/move/friendship conditions. The ready check is side-effect-free CHECK_EVO, including Everstone and all current table conditions. Use it from both bag/registered-item entry and party-sequence scanning; do not leave two divergent definitions.

For a ready already-capped Pokémon, invoke the existing Rare-Candy-style normal evolution branch as a Leveler action. Do not increase EXP, exceed the cap, consume the Leveler, fabricate friendship or waive a required item. Preserve native evolution scenes, cancellation and legal branch selection.

After an evolution, continue the existing party sequence. Guard against an unchanged species or a cancelled evolution being selected indefinitely. Each attempted slot/evolution must either change species or advance the sequence. If a legitimate next stage is immediately ready, the existing continuation may present it; cancellation must not be silently reversed.

Change the no-effect message to `Your party is at the current cap.\nNo Pokémon is ready to evolve.$` only when both leveling and ready evolution are unavailable. Keep the ordinary completion message accurate if the action evolved Pokémon without raising levels.

Acceptance: Munchlax reaching friendship at cap 14, Mime Jr./Bonsly learning Mimic at the cap, Type:Null at 100, Meltan arriving at New Mauville at its current level, Bisharp holding Leader's Crest, a blocked condition, an Egg, Everstone, cancellation, an evolution chain and a party with mixed ready/unready slots. Compare actual EXP/level before and after; no extra level is granted.

## PREP-02-B — Battle-only evolution with battle EXP disabled

Tandemaus has a native `EVO_LEVEL_BATTLE_ONLY` evolution. In `src/battle_main.c:TryEvolvePokemon`, the existing call to `EVO_MODE_BATTLE_ONLY` is gated by `gLeveledUpInBattle`; battle EXP is globally disabled in the configured Champions game. Raising Tandemaus with the Leveler does not establish an in-battle level gain.

Preserve the existing callback rather than add a second evolution scene pipeline. Add a narrow ready-battle-only evaluation after the existing special evolution check when a normal, non-linked/non-recorded/non-facility battle has legitimately ended in victory or capture. Require a conscious owned non-Egg Pokémon and actual species-table readiness. Retain the current level 25 threshold, PID branch conditions and Everstone behavior. Do not turn this into a general post-loss, post-flee or tutorial evolution trigger.

Because `EVO_MODE_BATTLE_ONLY` also accepts ordinary level evolutions, the no-level-gain path needs an explicit filter/mode that considers **only** `EVO_LEVEL_BATTLE_ONLY` entries. Reuse the same additional-condition evaluator and CHECK_EVO/DO_EVO handling. The normal leveled-up path remains unchanged. No EXP or level is added.

Temporary Circuit/Tent normalization must never cause a permanently underleveled Pokémon to evolve. Keep these modes excluded from the no-level-gain path. Honor the existing attempted-slot flags and cancellation, so a Pokémon receives one coherent evolution presentation.

Acceptance: Tandemaus at 25 and 100 can evolve after a legitimate normal battle without EXP; level 24 cannot; both PID branches remain possible; fainted/lost/fled/tutorial/recorded/linked/temporary-facility cases do not introduce new side effects. Confirm that no existing callback already satisfies a tested case before adding redundant work.

## FORM-11-NATURE — Edited Nature governs Nature-based evolution

The acquisition volume specifies the exact source repair for Toxel: the builder edits `MON_DATA_HIDDEN_NATURE`, which the battle stat function uses, while current evolution condition logic reads PID-derived `GetNature`. Use the displayed/edited hidden Nature for Nature-based evolution conditions, preserving personality, shininess, gender and unrelated PID-based branches.

This is not permission to replace Wurmple, Dunsparce or Tandemaus's deliberate personality conditions with Nature. Their conditions remain separate. Verify all 25 Nature selections against Amped/Low Key outcomes and the displayed/battle Nature.

## PREP-03 — One explicit expanded move-access policy

### Facts from the snapshot

The current authored preset catalog contains 6,923 sets across doubles defaults/alternatives and retained singles-oriented preparation buckets. The native preparation list deliberately unions species preparation moves with preset-provided moves. The game is an expanded Champions-style ruleset with project-specific data; a move absent from a separate pinned reference is not automatically rejected by the native game.

The book's read-only comparison found861 sets with at least one difference from the mapped pinned Champions/latest-mainline-plus-reviewed-extension reference and 262 sets whose forms that resolver could not identify. See [the complete mismatch inventory](../inventory/preset-reference-mismatches.json) and [unmapped forms](../inventory/preset-reference-unmapped.json). Examples include older event/transfer moves, expanded role moves and special form mappings. These totals are an audit result, not a production failure quota.

The reference generator's own policy is specific: Champions-mod learnsets where supplied, otherwise each species's latest mainline generation, with form/pre-evolution inheritance and selected reviewed extensions. That policy is narrower than simply retaining every implemented expanded-game move. The reference resolver also lacks some project form names. Neither issue can be resolved by blindly labeling every difference illegal.

### Final policy and authoring

Preserve the current native expanded move access and deliberate existing extensions. The preset catalog remains authored content. Newly proposed sets must use configured real moves, valid species/form ability slots, legal native item interactions and actual preparation access. A reference mismatch receives an explicit explanation or a deliberate design proposal; it does not automatically remove a move or invalidate a team.

INTRO-01 consequently preserves the four initial presets that differed only from the pinned reference. It changes Chikorita/Torchic where the new two-member rescue requires direct offense, and supplies exact newly resolved regional rival loadouts. Those are role-based changes, not a blanket learnset purge.

Retain the underlying singles-oriented preparation data while wild captures remain singles and the player may freely choose an orientation. No trainer competition launches a singles battle. Rename misleading preparation help if necessary: an orientation's name describes a build, not an available competition format. The default recommended campaign orientation remains doubles.

Do not claim this expanded availability is identical to every current official Champions ladder rule. The book specifies the actual game being authored. A future strict external-format mode would be a separate design decision, not an implicit consequence of running an old gate.

### Meaningful integrity checks

Keep structural checks for existing moves, no duplicate moves in a set, correct stat ordering/budget, valid base/form ability resolution, actual protected-item handling and complete generated materialization. Preserve intentional ability-changing Mega roles by checking both the starting form and the actual transformed form. Do not demand that a pre-Mega ability remain active after transformation.

Treat broad quality heuristics as review prompts. A low-power move can have a specific job; an unusual investment can create an exact low-cap threshold; a retained older move can be an intended game extension. Fix concrete incoherence, invalid data or an inferior choice supported by the full team context.

## PREP-04 — Form tools and protected rewards

The acquisition volume owns the exact additions for practical form access. Reuse the existing free evolution-item archive for the selected missing nonprotected form tools. Keep Silvally Memories in their existing free Species category; retain the Weather Institute's Reveal Glass and Birch's fusion-tool reward routes.

Do not add Mega Stones, earned Primal Orbs, crowned relics, Ogerpon masks or Arceus plates to that free archive. Existing protected-relic delivery has finite bit capacity and must not be extended by blindly appending items. INTRO-01 owns the new two-starter stone receipts; the Mega volume owns ordinary stone rewards.

Meteorite is also a story key: an early free replacement could bypass story conditions. Use the acquisition chapter's gated Cozmo/Deoxys recovery or interaction path rather than adding the quest item to an unrestricted early menu.

## PREP-05 — Field convenience follows the actual resolver

The current field-move resolver accepts a compatible party species once the relevant HM/badge is owned without requiring the move to occupy a battle slot. The old last-learned-Surf deletion guard can therefore block a safe preparation change.

Apply the world volume's W-FIELD-SURF repair: decide whether the resulting party retains actual unlocked Surf capability through the current resolver, rather than counting only Pokémon with Surf in their moveset. Preserve genuine traversal safety and the independent HM/badge requirements. Do not globally toggle unrelated HM-forgetting settings or remove valid progression gates.

## Collection and storage meaning

The existing14 boxes hold420 boxed Pokémon. Preserve that layout and save compatibility. Comprehensive availability in this book means the ability to obtain and use supported species/forms within one save over time; it does not promise a simultaneous living dex of every form. One-off release choices and fusion-parent ownership must be accurately explained. Cosmetic conveniences and repeatable normal encounters do not create duplicate one-off legendary entitlements.

Validation of these changes is proportional: reproduce the Leveler readiness and edited-Nature failures, check the exact resulting party state, verify required items and source materialization, and exercise the relevant UI. No 6,923-test suite mirroring every preset record is proposed.

## PREP-06 — Optional bonding preparation

Add a deliberate friendship adjustment to the existing shared Center specialist, available from Oldale, and let Verdanturf's Friendship Rater call the same service after its personal dialogue. This makes early friendship-dependent Pokémon practical without repeatedly teaching/forgetting moves or changing their evolution requirements.

Offer three explicit values: **Low (0)**, **Evolution threshold (the configured `FRIENDSHIP_EVO_THRESHOLD`)**, and **Maximum (255)**. Display the selected Pokémon and requested value before confirmation. Low/maximum remain useful for the expanded game's retained Frustration/Return choices. Do not automatically set every Pokémon to maximum or present the action as a compulsory tutorial.

Use `MON_DATA_FRIENDSHIP` on the selected owned Pokémon, preserving personality, shininess, gender, Nature, ability, moves, points, held item, experience and ownership. Reject Eggs: the same field stores hatch-cycle information for them and must not be edited as friendship. Reject empty/invalid slots and make cancel a complete no-op. Recalculate through the existing native stat function as appropriate; do not change the configured battle affection/crit behavior.

The action does not automatically evolve the Pokémon. The player can use the Leveler through PREP-02 when the existing level/map/time/item/move conditions are met, and Everstone/cancellation still work. No new global time-change feature or bypass of night/day conditions is introduced.

Implementation owners are `data/scripts/emerald_champions.inc` for the optional menu, `src/field_specials.c` for one validated friendship setter/preview helper, and `data/specials.inc` for the script binding. Verdanturf's rater references that helper instead of maintaining a second implementation.

Acceptance: each of the three values on a normal owned Pokémon, before/after comparison of unrelated data, Egg/empty/cancel rejection, current Return/Frustration calculation, a ready friendship evolution at the cap, an unmet night/item condition, and an Everstone holder. This is a small convenience proposal, not proof that the previous friendship paths were unobtainable.
