# 08 — Implementation sequence and global reconciliation

This book is a specification, not an implemented revision. The original game remains the baseline. Implement through coherent changes that preserve successful authored content, with each subsystem still having one owner.

## Rebase the specification before touching the game

Use `baseline/manifest.json` to identify the exact source inputs reviewed. The snapshot includes uncommitted work at the recorded capture time; git HEAD alone does not identify it. Compare relevant live files with these hashes and inspect intervening changes before applying any instruction. Never replace newer work with a frozen copy.

`baseline/source/` is an evidentiary content snapshot, not a standalone build checkout with every toolchain and visual asset. Build and test from the canonical project or an appropriately isolated complete checkout. Do not build inside the book and then claim the working game was updated.

## Integration units

|Unit|Authoritative specification|Concrete result|Evidence before acceptance|
|---|---|---|---|
|1. Persistent state and opening|INTRO-01, state-allocation appendix|Two permanent starters, explicit rescue/rival doubles, safe early supplies and migration, correct regional rival selection and second-starter stone receipts.|All54 ordered choices; current/legacy story states; party/PC/Ball failure; Retry; source/variable/flag binding.|
|2. Levels, Points and shared eligibility|DIFF-01, TEAM-01, POINTS-01|Medium cap-minus-two floor including gyms; correct full/half multi sizes; actual flat-Points calculations and truthful UI.|Production level constructor cases and representative actual battles; source stats vs displayed stats.|
|3. Reliable AI state/forecasts|SAI-03, SAI-11, SAI-02|Single application of switch-entry effects, isolated candidates, truthful current-hit damage and probabilities.|Reproduce the actual failures; candidate-order independence; Berry/current-hit/crit controls.|
|4. Shared expert decisions|SAI-01/04/05/06/07/08/09/10/12|Common capabilities, genuine form/Mega comparison, owner-aware budgets, two-foe reserves and coherent paired actions.|Actual chosen actions in affected authored scenarios, interrupted alternatives, target performance and scratch-memory measurements.|
|5. Narrow authored battle revisions|Early/late battle volumes and regional-rival supplement|Iconic Nosepass/Dusclops/Walrein, precise timing/ability/form/loadout repairs, all unmodified teams preserved.|Exact generated materialization; configured starting/transformed forms; relevant Points benchmarks and actual fights.|
|6. Collection and preparation|Wild catalogue, FORM specifications, PREP-02/02-B/03/04/05/06|Missing functional roots, one-save branch resources, practical form/evolution tools, ready-at-cap behavior and edited Nature consistency.|Actual capture/gift/evolution paths, no duplicate one-offs, item/PC failure, complete form identity and retained expanded move access.|
|7. World/NPC/reward cohesion|World changes, Mega volume, GUIDE-01, WILD-ENGINE-01|Accurate current handoffs, usable method guide, safe Sweet Scent, finite reward retry and restored optional Wallace interaction.|Relevant source roots, field paths, bag/PC retry, actor choreography and rendered changed dialogue.|
|8. Other competition entrypoints|FAC-01/02/03, LINK-01 and world facility pages|Competitive Tent/Hill paths, truthful Frontier desks/records, no native singles/imported unsupported challenge entry.|Local return/party restore, format/party validation, timer and prize handling, no Circuit entitlement contamination.|
|9. Whole-game integration|This chapter and tests/evidence chapter|A consistent revision with all final sources/flags/UI and no stale materialization.|Focused runtime suite plus representative fresh-save traversal and separate battle play; exact release source/artifact identity.|

These units can be separate reviewable commits, but do not publish a partially migrated game. Version4 initialization, its consumers and the new receipts must ship coherently. Likewise exact Galar form lookup must precede or accompany new Sign rows; a new reward must not be exposed before its retry and ownership paths exist.

## Keep the existing authoring

The original trainer identities, encounter-group identifiers, maps and save layout remain. The battle volumes supply full baseline and final loadouts. Most Pokémon fields are unchanged; many REPAIR dispositions correct false explanations or require shared AI behavior rather than replacing the team.

The new opening is deliberately broader: six legacy aliases now dispatch four-member doubles teams, and two selected starters determine the unchosen rival family. All later regional substitutions use the explicit supplement. The aliases are compatibility identifiers, not evidence that the rival owns the player's selected Pokémon.

Roxanne, Phoebe and Glacia regain their primary iconic species with defined competitive roles. Wally retains the Ralts-family/Gallade identity; existing major signatures such as Hariyama, Manectric, Torkoal, Slaking, Altaria, Solrock/Lunatone, Kingdra, Absol, Salamence, Milotic and Metagross remain. Legendary-heavy teams are preserved where coherent. Displacing a unique Glastrier or Shedinja showcase is an explicit local decision, not a reason to force an unrelated replacement elsewhere; those species remain available and supported.

## One owner for each cross-volume change

|Shared concern|Owner|Do not duplicate|
|---|---|---|
|Two-starter state, regional aliases, early supplies and starter-stone receipt bits|INTRO-01|A second gift ledger, a second rival-type resolver, or a separate Steven receipt schema.|
|Persistent addresses/version 4 initialization|State-allocation appendix + INTRO migration|Independent agents allocating the same unused flag/variable or reinitializing receipts on every load.|
|Trainer level floor|DIFF-01|Per-map copied reducers or mass offset rewrites.|
|Native Points calculation|POINTS-01/current stat function|A conventional EV formula in benchmarks or a separate GUI approximation.|
|Shared decision logic|SAI specifications|Trainer-ID-specific copies of generic Room, redirection, field, activation or Mega behavior.|
|Wild slots and water authoring overrides|Wild catalogue/WATER-AUTHOR-01|Another encounter database or generator-order changes affecting unrelated maps.|
|Functional form routes and identities|Acquisition/forms volume|Cosmetic conversion that bypasses meaningful form conditions, new gift flags outside the allocation ledger, or base normalization erasing Galar identity.|
|Mega/legendary item and receipt delivery|Mega volume + referenced INTRO/world/form contracts|A free Mega archive, overflowing the existing relic bitfield, or multiple independent claim flags for the same intended entitlement.|
|World dialogue and physical event changes|World change targets|Contradictory alternate versions of the same text or accidentally activating every unbound legacy script.|
|Tent/Hill/Circuit generation|FAC specifications/current competitive generator|Another random-team engine or shared lifetime counters that award postgame legends for early exhibitions.|

## Source changes implied by preserved content

KEEP on a roster means its Pokémon fields remain intact. The battle still consumes the new universal floor and expert AI where applicable. KEEP on a world map means its layout and unlisted interactions remain intact; its shared nurse/vendor/guide contract may receive an explicitly owned global improvement. This is why global contracts must be integrated before interpreting each page's disposition in isolation.

No blanket behavior change should be hidden behind a preserved label. The unified decision index records the local and shared obligations. A source claim that turned out false—such as treating high-tide layout names as missing map encounters, or interpreting Heatran's physical hide flag as capture ownership—is marked as rejected rather than turned into a repair.

## Play and traversal acceptance

A fresh-save route should exercise the two-starter rescue, pre-rival catching/preparation, the first four-member rival, Roxanne and core story handoffs through the League. Include optional habitats and rewards at the actual first access conditions, rather than warping a fully equipped party into every room and calling it progression proof.

Separately, battle testing must disable traversal auto-win and exercise the intended AI decisions. Demonstrate credible solutions with different team structures, including early Nature/Point reallocations, without installing a fixed number-of-solutions gate or demanding a prescribed six-Pokémon answer. The player is allowed to solve a hard puzzle cleanly.

For late collection, test at least the distinct mechanism classes: direct wild regional form, edited-Nature evolution, normal evolution at cap 100, battle-only evolution with EXP disabled, cosmetic/functional form services, irreversible nonbreedable branch alternatives, exact Galar legendary identity, and fusion/unfusion move preservation. Shared mechanism coverage is more valuable than hundreds of tests that merely mirror table rows.

For save/reward integrity, test fresh version 4 state and representative old states at every new transaction boundary. Ordinary reload must not rerun migration or duplicate either starter, stones, research partners, final Trick prizes or Circuit entitlements.

After a real build, bind the actual ROM/ELF to its source inputs through the existing artifact workflow. A book source hash, an old passing ELF or an automated traversal result cannot certify the new competition's quality. Report exactly what was exercised, following chapter 09.

## Definition of a completed implementation

All final authoring is materialized; new entrypoints and retired entrypoints behave as specified; each advertised functional acquisition has its stated route; relevant source/transaction/AI failures are repaired; changed scenes and menus render correctly; and actual battle/traversal evidence is attached to the exact new build. The book itself reaches only the preceding milestone: a complete, internally reconciled implementation specification ready for the user's review.
