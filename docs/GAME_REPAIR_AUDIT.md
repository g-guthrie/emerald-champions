# September 8 game repair audit

This audit starts from published 5.1 commit `635ae1f22`. The user reported
opening choreography, unnecessary preparation, opponent levels, repeated nurse
dialogue, battle stalls and ineffective AI. The Cohesion Book is historical
input, not a correctness oracle. This report separates source findings from
runtime evidence and does not certify an entire played campaign.

## Confirmed opening and dialogue defects

- `CreateEmeraldChampionsBirchRescueParty` created both opponents at level 5.
  It now creates them at level 3; the two starters remain level 5.
- The added Poochyena field actor used `MOVEMENT_TYPE_FACE_UP`, then a single
  `face_left`. Unlike the original chasing Zigzagoon, it had no walking idle.
  It now uses the existing jog-in-place movement and its native animated sprite.
  The source actor is Poochyena, not Mightyena; its species is unchanged.
- Selecting the pair previously entered a nested moves/stats/items preparation
  menu during Birch's emergency. It now starts the rescue immediately. A loss
  offers another attempt or a return to the bag. Starter-selection cancellation
  and the separate in-battle Retry/Reload controls remain available.
- After delivering tools, the nurse restarted the full welcome conversation.
  She now asks, “Well, now that you're here, would you also like to heal your
  Pokémon?” and follows the existing yes/no healing paths.
- The first tool explanation claimed Repel Spray lasted until toggled off.
  `UpdateRepelCounter` actually expires it after 500 steps. The dialogue now
  describes the implemented behavior; no encounter mechanic changed.
- `CanPlayerForfeitNormalTrainerBattle` rejected the rescue's `FIRST_BATTLE`
  flag before reaching its rescue exception, making the intended Retry menu
  unreachable. The repair permits that one flag for an identified Birch rescue
  while retaining the other battle-format exclusions. An extracted
  production-function C check passed 25 eligibility cases; native menu testing
  is recorded separately.

## Battle execution and trainer quality

5.1's own handoff recorded a full-party AI probe at 515 frames (about 8.6
seconds), and a warm decision at 374 frames. Those were earlier measurements,
not fresh measurements from this audit. The paired AI enumerated joint actions,
switch pairs and Mega alternatives, replaying turn simulation and further
responses synchronously. This is a concrete cause of long periods without
responsive gameplay; it does not prove every reported freeze has that cause.

The repair replaces that exhaustive path with bounded cached tactical scoring
and executable authored battle plans. See `TRAINER_STRATEGY_AUDIT.md` for the
full trainer-data appraisal and remaining matchup-specific questions. A legal
loadout and an authored instruction do not alone establish a strong opponent.

### User-approved strategy upgrade

After the initial audit, the user approved improving the actual encounters.
This is a focused portfolio, not a claim that every trainer now has a bespoke
script. Existing gym identities, party sizes and level settings remain intact.

- Maura gains Perish Song Jynx beside Shadow Tag Gothitelle, replacing Wobbuffet
  while preserving her other five species and Mega Slowbro. Her plan must leave
  the singer early, retain the trap, and escape the trapper's own countdown.
  Ghost types, Soundproof and successful pivot moves provide counterplay.
- Parker leads Lickilicky beside Telepathy Oranguru. Earthquake replaces
  Stomping Tantrum; Trick Room permits the slower Lickilicky to attack before
  Oranguru repeats it with Instruct. Nonimmune reserve partners must remain a
  real constraint, not collateral damage the AI ignores.
- Connie retains both Storm Drain/Surf partners; coordinated scoring recognizes
  the actual absorption boosts and subsequent damage instead of treating Surf
  solely as ally damage.
- Kate & Joy and Quincy gain explicit Neutralizing Gas/Slaking partnership
  roles. Quincy's Durant/Entrainment phase must not blindly preserve Gas;
  Zygarde's Power Construct is explicitly unsuppressible in the native engine.

These changes supersede the initial audit's exact book/loadout parity. Existing
Flannery After You/Eruption and Tabitha Steam Engine/Weakness Policy teams are
also exercised against native opponents, without gratuitous roster changes.
Runtime results below are distinct from these design intentions.

## Wild encounters

See `WILD_DISTRIBUTION_AUDIT.md`. Rarity, habitat identity, physical access,
early variety and evolution timing are separate questions. The audit does not
redistribute species simply to satisfy a numeric quota. Corrections to misleading
clues and stale generators must remain distinct from proposed balance changes.

## Design-book and verification problems

The book prescribed both the level-5 rescue and preparation interruption now
rejected by the user. Its world placement also explicitly prescribed the static
Poochyena facing behavior. Implementing those details exactly did not make them
good gameplay. The live handoff, meanwhile, still pointed to the book as a frozen
specification. That authority claim is now superseded in the repository guidance.

Other stale claims found in the supporting material include an opening-rival
single-Pokémon exception in the trainer audit, obsolete save-migration functions
in `KNOWN_ISSUES.md`, and availability calculations that count disabled or
not-yet-accessible encounter methods. These are documentation/tool defects,
not evidence that old behavior should be restored.

### What to keep from the book, and what to reject

Keep the coherent doubles identity, two permanent starters, free and accurate
team preparation, separate Retry/Reload actions, deliberate revisitable
legendary encounters, and reliable item/reward delivery. Its analysis of native
Stat Points, truthful damage estimates, ally coordination, and restoration of
scratch battle state addresses real requirements. Preserving an existing good
team is reasonable; rewriting teams merely to produce a larger change is not.

Reject the opening interruption and level-5 rescue prescriptions now explicitly
overruled by the user. Replace the AI chapter's action-count bound with measured
responsiveness: up to 1,024 pair checks per form scenario can still be far too
expensive. Retain working trainer intent through executable plans while sharing
the mechanics; the book's removal of trainer-ID handlers depended on first
proving the generic replacement, not just deleting specialized behavior.

There is a traceable scope expansion within the book itself. The shared-AI
review's SAI-07 proposed two shortlisted actions per actor and four pair checks
per form scenario. The final chapter's lead-editor reconciliation explicitly
rejected that cap and raised the bound to 32 actions per actor and 1,024 pairs
per form scenario to avoid losing enabling moves. That is a 256-fold increase
in the specified maximum pair count, not a measured 256-fold runtime increase.
Protecting support moves was a valid concern; accepting the expanded search
without an enforced frame budget was not a demonstrated solution.

Do not attribute every implementation problem to the document. Chapter 06
explicitly prohibited a monolithic replacement simulator and recursive
multi-turn trees, required shared caches and target profiling, and acknowledged
useful existing tactics. Chapter 09 explicitly warned that static catalogues and
auto-won traversal do not establish battle quality. Shipping with multi-second
AI measurements and incomplete tactical evidence was an implementation and
release decision despite those warnings.

Reassess opening encounter density, repeated area rosters and trainer matchup
quality through play. Neither broad availability nor retaining 501 of 516
loadouts proves a defect by itself. The problem is treating inventories and
descriptions as sufficient evidence for an experiential redesign.

The existing campaign traversal harness automatically resolves battles. Its
passes cannot establish AI strength, latency or fair difficulty. The historical
normal-key opening trace also includes the removed preparation menu and must
not be replayed to completion and called a pass without fresh visual review.

## Evidence collected

- Final clean named production build and release checks pass. ROM SHA-256:
  `226bd3e025e096254f710c42dec339a00eb6d744bfc8b5b3cdbe12022513ab7c`.
  The adjacent input stamp binds 28,977 inputs and both ROM/ELF artifacts.
  The original tracked 5.1 ROM remains unchanged. Gate log:
  `work/repair-runtime/final-release-gates.log`.
- Final production blank-flash/normal-key UI check confirms two level-3 foes,
  level-5 starters, immediate rescue after pair selection, animated Poochyena,
  and accepted in-battle Retry. Continuing manual combat reached a natural loss;
  declining its retry offer returned to the bag with the actors restored.
  Exact outcomes and input caveats: `work/repair-runtime/opening-final-2/README.md`.
- Final headless nurse fixture, ROM `e710cb6233294685b31529207a79ef6123ad02bc2e488124af808d57a620dc12`:
  optional prompt visually reviewed; Yes heals/clears poison and No preserves
  1/31 HP/poison. Both deliver all four tools and release control. Evidence:
  `work/audits/nurse-flow-final-2026-09-08/README.md`. This uses a native scenario
  setup, not a production fresh-save journey to Oldale.
- Final focused native battle run: 20/20 declarations pass in
  `work/ai-repair-final-native.log`. These include production-generated trainer
  parties with their authored plans enabled: Maura's three/four-turn trap and
  safe escape, Parker's Trick Room/Instruct Earthquake and nonimmune-partner
  control, Connie's mutual Storm Drain Surf, Flannery's After You/Eruption, and
  Tabitha's useful activation, injured-partner safety and unnecessary-boost
  controls. They are selected legal matchups, not wins against every player team.
- The same run checks coordinated support, weather, Trick Room expiry without
  self-cancellation, healing, Mega ownership, accuracy-aware KOs, scratch/RNG
  restoration and Perish deadlines. Its warmed full-party decision probe takes
  18 frames (about 0.30 seconds at 60 Hz), not including initialization. This is
  one native test position, not a worst-case campaign latency guarantee.
- The tests exposed two distinct issues during this upgrade: ordinary recorded
  campaign battles were wrongly treated as a foreign trainer-ID namespace,
  disabling plans in the native harness; and sequential forecast bookkeeping
  could spare the last actor from Perish when other actors had just been counted
  as fainted. The fixes preserve the real trainer namespace and evaluate the
  countdown from a shared pre-countdown board.
- Static map/script audit: 540 maps, 4,195 physical events, 1,402 warps and
  18,360 script/dialogue/movement references resolve after the opening changes.
  This does not establish state-dependent reachability.
- Prerequisite inventory: 930 recorded conditions, no structural failures.
  This describes inventory coverage, not 930 played interactions.
- Mega reward check: all 99 stones have declared world rewards, including
  three exclusive Berry Master exchanges. Runtime acquisition remains distinct.
- Interim production ROM `ffb243e4771f9a7e7d115337e31e7ea0ce1083f8b169338cdf875b2e8727562a`:
  blank-flash normal-key opening, continued manually after the historical trace
  diverged. Both chosen starters appeared at level 5 and both rescue opponents
  at level 3. The second confirmation started battle without a preparation menu.
  Run reached the restart confirmation; accepting it returned to the rescue
  with both starters and battle flags `0x15`, no outcome set. Screenshots and
  exact artifact snapshots: `work/repair-runtime/opening-interim-2/`.
  This is an interim opening check, not evidence for later AI changes.
  Continuing native combat with ordinary buttons subsequently won the rescue
  (`gBattleOutcome == 1`) and returned to Birch's overworld conversation. No
  battle automation or outcome writes were used; one starter fainted during
  the battle. This is one played starter pair, not opening balance coverage.
- Poochyena animation on the same interim production ROM: after dismissing
  Birch's help dialogue, screenshots eight frames apart show alternating
  walking poses while it remains in place. The sprite-region hashes alternate
  A/B/A/B at frames 59/67/75/83. Evidence:
  `work/repair-runtime/field-animation/free{59,67,75,83}.png`.
- Interim headless nurse fixture: the first tools handout reached the new
  optional-heal question. Yes healed once and cleared poison; No retained
  1/31 HP and poison. Both branches delivered all four tools and released script
  control. Exact artifact identity and evidence: `work/audits/nurse-flow-2026-09-08/`.

## Trainer dialogue as strategy guidance

Cross-checking Fortree and Mossdeep against their actual authored parties found
demonstrably stale advice, not merely subjective writing problems. Humberto
described Minior/Shell Smash, Sylvia described Torkoal/Oranguru, Samantha named
Xatu, and Tate/Liza promised an ice rider; those Pokémon are absent from the
corresponding current parties. Jared's Noctowl had no sleep move, Ashley's
Swanna did not set rain, and Macey's Blaze Delphox did not steal items.
Several other posts described different teams' tactics. The corrections use
current rosters and mechanics, without changing loadouts or encounter difficulty.

Static defeat lines also asserted unobserved actions such as removing weather,
stopping setup or breaking Trick Room. They now acknowledge defeat without
inventing how the player won. Advice no longer claims that Protect prevents
Trick Room or that merely using a spread attack guarantees stopping a setter.
All added dialogue lines in Fortree, Mossdeep and Buffel's corrected defeat
text measure at most 208 pixels using the native FONT_NORMAL glyph widths.
Static script references pass; these later-gym conversations still need visual
checks on the final rebuilt ROM.

Fresh build, focused tactical runtime results and visual opening evidence are
recorded above. Full-campaign balance, every trainer matchup, multiplayer and
all facilities remain outside any completion claim until actually played.
