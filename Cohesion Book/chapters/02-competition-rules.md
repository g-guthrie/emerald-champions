# 02 — Competition rules, party sizes, levels and Champions Points

**Disposition: REVISE the shared floor and opening format; preserve the configured battle calculation and authored competitive intent.** This chapter is authoritative wherever an individual battle page reproduces an older authored level or AI profile. Native source values in those pages are evidence, not exceptions to this contract.

## DIFF-01 — Universal campaign level floor

The user's final requirement is exact: on Medium, no opponent Pokémon in a normal campaign trainer battle may be below the player's **live current cap minus two**. Gym trainers are included. An earlier route revisited after another badge uses the new live cap. The maximum authored encounter ID is unrelated to this calculation.

Current owner: [`src/difficulty.c:ApplyTrainerLevelDifficulty`](../baseline/source/src/difficulty.c). The snapshot only applies its badge floor to field doubles and excludes maps using the gym battle scene. Gym Pokémon authored below their chapter cap therefore currently fall below the requested Medium floor after the additional two-level reduction. The old opening singleton is also outside the field-double condition; INTRO-01 supersedes that format.

### Exact final calculation

For an eligible normal trainer Pokémon:

```
C = min(100, GetCurrentLevelCap())
A = the authored level produced from this encounter's strict cap + slot offset
D = 4 on Easy, 2 on Normal/Medium, 0 on Hard
L = max(1, min(100, max(A, C)) - D)
```

Use the existing difficulty reducer. Remove the gym-scene exemption for campaign trainer construction. Preserve higher authored levels. Do not add another reduction in a generator, controller or script. Recalculate experience, level, stats and full starting HP exactly as the current function does after choosing `L`.

Eligibility is normal authored trainer competition, including gyms, rival battles, story multis and the new opening rival. Link/recorded battles, Circuit/exhibition opponents and Trainer Hill must use their explicitly owned level constructors; those constructors must meet the same Medium floor through FAC-01/FAC-02, rather than receiving this function's reduction twice. Wild captures are outside DIFF-01. The scripted Birch rescue has the explicit initial levels in INTRO-01.

|Live cap|Authored level|Easy|Medium|Hard|Reason|
|---:|---:|---:|---:|---:|---|
|14|12|10|12|14|Under-cap gym or route slot is lifted before the reduction.|
|14|20|16|18|20|Roxanne-strength authored ace remains stronger.|
|20|13|16|18|20|An unbeaten opening-route trainer scales on a return visit.|
|55|60|56|58|60|A stronger story slot is preserved.|
|100|80|96|98|100|Postgame revisit uses the live ceiling without indexing EXP above 100.|

The current cap sequence remains 14,20,30,40,45,55,60,70,80,100. These are numeric constraints on team construction; no competitive technique is withheld until a later badge.

### Implementation boundaries and acceptance

1. Modify the single floor predicate in `ApplyTrainerLevelDifficulty`; retain the independent Circuit/Hill exclusions and normal type guards.
2. Confirm every authored slot's original level is1–100. Do not change hundreds of authoring offsets to emulate a shared rule.
3. Run a production-function host test with explicit trainer/double/gym flags and live caps 14,20,55,100. Verify the table above, especially a gym slot authored at 12 and a returned field trainer authored at 13.
4. Test the actual opening rival, a Rustboro gym trainer, Roxanne, a two-trainer story battle, and a postgame revisit. Inspect both live party level and displayed level, full starting HP and actual Speed. Source math alone is not proof the correct constructor ran.
5. Demonstrate that wild levels and player leveling do not receive the opponent reducer, and Circuit/exhibition/Hill do not receive it twice.

## TEAM-01 — Doubles competition and minimum opposing team size

All normal trainer competitions are doubles. Every independently controlled authored opponent team has at least four Pokémon. The opening Route103 rival is explicitly four, replacing the six historical singleton variants. These variants remain six dispatch aliases into the new three unchosen-starter identities, not six different stories.

Existing multi battles can combine two three-Pokémon contributions into a six-Pokémon opposing side when their explicit Multi Party Half authoring applies. Full two-trainer battles retain both six-Pokémon parties, for twelve total opponents across two active slots (E0127). Preserve the exact authored layout: a three-slot component is not an independent three-Pokémon competition, and a full two-trainer battle must not be trimmed to six total. Audit the combined side, its two lead slots, reserve ownership, one-owner-per-party budget and independent trainer defeat callbacks. Do not accidentally build an eight-Pokémon side by applying a four-slot minimum independently to the two halves.

The player may win with fewer survivors. Do not conflate the opponent's authored minimum with an obligation for the player to still have four healthy Pokémon after attrition. Entry to optional six-member exhibitions/Circuit uses its own preparation rule; ordinary story battle entry needs two usable battlers, with source-compatible party/partner exceptions examined under INTRO-01 and world handoffs.

Ordinary wild and legendary captures remain singles. Wally's catching demonstration is a capture sequence, not a trainer competition. The first Birch rescue is the explicitly requested scripted doubles exception.

Native e-Reader, obsolete facility and imported Secret Base challenge paths cannot silently expose singles or three-Pokémon competitions. The world volume specifies retirement of unsupported native entrypoints while preserving harmless maps, archived records and save identities. Do not edit arbitrary external link/recorded battle data as though it were authored campaign content.

## POINTS-01 — The actual Champions calculation

Sources: [`include/constants/emerald_champions.h`](../baseline/source/include/constants/emerald_champions.h), [`src/pokemon.c:CalculateMonStatsCont`](../baseline/source/src/pokemon.c), [`src/field_specials.c`](../baseline/source/src/field_specials.c), and the native preparation API.

The builder permits 66 total Stat Points and 32 per stat. Under the configured Champions calculation, the stat function **forces effective IV31**, including Speed. Stored zero-IV flags therefore do not create a slower Trick Room Pokémon in this ruleset. Do not recommend a conventional zero-Speed-IV build or silently change the stat rules to make a borrowed set behave as it would in another game.

For base stat `B`, level `L`, points `P`, and nature multiplier `N`:

```
ordinary HP = floor((2*B + 31)*L/100) + L + 10 + P
other stat = floor((floor((2*B + 31)*L/100) + 5 + P) * N)
```

Nature uses the native integer modification, normally 0.9/1.0/1.1. Shedinja keeps its explicit one-HP handling. Ability, held-item, stage, weather and other battle modifiers are then applied through the normal battle calculation. Raw summary Speed does not already include all of those modifiers.

Authored spread notation is **HP/Atk/Def/SpA/SpD/Spe**. The native engine stores **HP/Atk/Def/Spe/SpA/SpD**. Use the existing materializer's field mapping; never copy a displayed array into an engine-order array without conversion. Legacy `MAX_TOTAL_EVS=510` and 252-EV constants elsewhere are not this builder's point budget.

These flat additions are especially consequential at level 5 and 14. Early loadouts must be evaluated at actual effective levels after DIFF-01, not by familiar level 50 or100 expectations. Preserve 32/32/2 spreads only when they serve the actual plan; precision spreads may buy a required turn order or survival threshold.

### Required authoring evidence

For a timing-dependent strategy, state both final Speed values at the relevant difficulty and include priority/field modifiers. For survival-dependent partner activation, use exact HP/defense and actual move damage—including allied spread reduction and potential opposing follow-up attacks. For berry setup, use the native activation inequality and HP parity.

The early reviewer provides [early Points benchmarks](../battles/early/points-benchmarks.md); the late volume supplies [its own measured benchmarks](../battles/late/benchmarks.md). They are reproducible calculations and source checks, not empirical battle wins.

Sitrus in this snapshot activates at half HP rounded **up**. Gluttony plus a flavor berry uses a different threshold; replacing Sitrus solely because the alternative heals more can prevent Belly Drum's berry from triggering at odd HP. The late volume explicitly preserves Jack's relevant Sitrus case after checking this issue. Do not introduce automatic silent investment changes merely to preserve a proposed berry swap.

## PREP-01 — Expert preparation from the opening

INTRO-01 supplies two permanent prepared starters, a doubles rescue and pre-rival catching/preparation access. The first required trainer battle must permit the player to assemble a real team before committing. Preserve free nature, ability, legal move, held-item and Stat Point services, the cap-respecting Leveler, storage, healing, Retry and separate Reload Save.

The preparation UI must show actual resulting stats and remaining points. A selected nature or point change must immediately recalculate the displayed Pokémon through the same native function used in battle. Do not describe points as conventional EV fractions. Preserve the native budget and avoid silently adding a general player Item Clause: trainer authoring uses unique held items, but the snapshot does not universally enforce that rule on ordinary player parties.

A valid default preset is a convenient starting orientation. It does not restrict manual legal moves or force a player to use one catalog strategy. The preset consistency audit and exact exceptions are supplied in the preparation chapter; source/pinned-reference disagreements must be resolved deliberately rather than treated as automatic proof that one dataset is correct.

## FAC-01 — Competitive Battle Tent exhibitions

All three live Tent attendants become six-Pokémon doubles exhibition entrypoints. Reuse the existing competitive Circuit team generator and legal set construction; do not revive the inherited rental, autonomous Palace or Arena singles rules.

Sources include the three `{SlateportCity,VerdanturfTown,FallarborTown}_BattleTentLobby_EventScript_Attendant` roots, `src/battle_tent.c`, `src/champions_circuit.c` and the shared generated-battle constructor. World entries contain exact clerk and rule-board replacements.

### Final mode and state contract

- Entry requires six healthy non-Egg Pokémon. The clerk offers preparation/exit without consuming money, items or progress on refusal.
- Snapshot the player's full party once. Capture entry cap and originating Tent identity. Temporarily normalize the exhibition party to that cap and heal, preserving the actual chosen moves, natures, abilities, points and items.
- Create six legal, coherent opponents at `entryCap - difficultyReduction` (minimum1). There is no win-based level escalation in exhibitions. The cached cap cannot change mid-run.
- Start battles **locally** from the Tent. Reuse competitive generation and battle mechanics without entering the Tower corridor/room warp scripts. Return to the same attendant after the run.
- A run ends on loss, withdrawal or three wins. Track its win count and active mode transiently; do not allocate a new persistent streak merely for this optional mode. Saving within an active run remains unavailable. Retain an origin record for abort cleanup.
- Three wins earn one reward from that Tent's existing prize pool. Write the existing pending prize field before attempting delivery; clear it only after successful insertion. Existing pending prizes must remain claimable.
- Exhibition wins never increment Circuit current/best/lifetime counters, award its legendary milestones, or write classic facility Symbols. They cannot unlock postgame Frontier access.
- Restore the exact pre-entry party on every terminal path. An error during generation must end safely without an incomplete battle, lost Pokémon or an altered saved team.

Introduce a small explicit mode parameter/state around shared competition functions: `POSTGAME_CIRCUIT` versus `TENT_EXHIBITION`. Keep reward/accounting callbacks separate. Reuse generation and party construction, not postgame admission and reward logic. No second random-team generator is needed.

The current prize pools remain: Verdanturf's ordinary evolution stones; Fallarbor's PP Max/specialty Balls; Slateport's evolution items. They are small rewards for optional competition, not a replacement for free preparation.

Acceptance includes all three entry/exit locations, a full party/PC, bag-full prize retry, loss after one/two wins, retirement, generation failure, unchanged Circuit counters, and exact player-party restoration. Verify an actual doubles battle with all six opponents—not only the clerk text.

## FAC-02 — Trainer Hill

Retain the authored time-attack geography, floor progression and existing earned-prize storage. Replace inherited generated competition rosters with a coherent six-Pokémon opposing doubles side on each floor, using the shared competitive generator at DIFF-01 levels.

If the current paired NPC presentation remains, generate **one** complete six-Pokémon team, choose its coordinated two leads, and split its reserves between the existing three-slot owners. Do not independently generate two unrelated three-Pokémon parties. Native owner semantics still govern switches and Mega entitlement.

Own the change in `src/trainer_hill.c`, the `TrainerHill_EventScript_TrainerBattle` root and entrance/roof contracts. Preserve timer pause/resume and legitimate floor completion. Trace the actual `trainerhill_settrainerflags`/`trainerhill_lost` consumers before changing their apparent names; a cleanup call is not proof of a victory flag bug.

Hill runs do not affect Circuit counters or rewards. A defeated/withdrawn run restores the correct party/location state and cannot award roof completion. Test entry, every floor pair, a loss mid-run, withdrawal, timer behavior, generation failure and full-bag prize resumption. The world volume gives exact roots and map contexts.

## FAC-03 — Frontier records and presentation

All current Frontier challenge desks continue to admit the postgame Circuit. Their text must explicitly describe the shared central Tower competition. Historical records may be preserved as labeled archives; current boards must not advertise inaccessible singles, rentals, judging rules or Symbols.

Add a read-only `ChampionsCircuitBufferRecord` helper: current comes from `VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS` (0x40DB), lifetime from `VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS` (0x40DC), and the new `VAR_EC_CIRCUIT_BEST_WINS` reuses the declaration-only retired nursery variable at 0x40E3. INTRO-01's version 4 migration owns the reclamation alongside0x40E1/E2. Initialize best from an observed active current run only; otherwise0 means no best has yet been recorded. Never synthesize best from lifetime or retired facility records. On a real Circuit win, set best to max(previous best,new current); Tent/Hill wins cannot update it.

Buffer current in STR_VAR_1, best in STR_VAR_2, lifetime in STR_VAR_3 using five-digit-safe formatting. Show `--` for unrecorded best and label it `Best recorded`. The buffer writes no progress. World boards use it where they claim to show current Circuit performance; historical facility data remains untouched. Existing pending Circuit rewards remain claimable from the attendant after party/run restoration.

## LINK-01 — Native link menus and imported Secret Base challenges

This is an explicit final disposition for competition entrypoints outside the516 authored branches. Preserve trading, communications cleanup, imported records and harmless base decoration. Do not invent valid modern teams from arbitrary imported legacy data.

### Wired and wireless battle choices

Owner: `src/data/script_menu.h:MultichoiceList_BattleMode`; `data/scripts/cable_club.inc:CableClub_EventScript_SelectBattleMode` and `WirelessBattleSelect`.

Final shared menu order is **Double Battle, Multi Battle, Info, Exit**. Both dispatchers must use indices 0/1/2/3 respectively; B cancels. The Double branch still sends the existing `USING_DOUBLE_BATTLE` / `LINK_GROUP_DOUBLE_BATTLE` protocol values, and Multi keeps its existing protocol values. UI indices must not be substituted for protocol constants.

Remove new-entry access to `CableClub_EventScript_SingleBattleMode` and `WirelessSingleBattle`. Retain the labels as guarded obsolete entrypoints if needed for safe recovery: show `This desk hosts DOUBLE BATTLES. Please choose Double or Multi.` and return to a safe selection/exit path without starting negotiation for singles. Retain existing single-session return/cleanup labels so a saved or interrupted old session can exit cleanly; their existence is not permission to begin a new singles competition.

Update `ExplainBattleModes` and `WirelessBattleInfo` together with the menu. Both players' selected rosters must meet a scoped wired/wireless admission requirement (at least four on an independent side); Multi contributions use their explicit combined-side semantics. Do not redefine the shared `HasEnoughMonsForDoubleBattle` guard to four: normal story entry and the scripted rescue still need its two-usable-battler semantics. Reject an incompatible peer request before battle entry with a clear explanation and normal communication cleanup. Do not mutate remote Pokémon, rewrite imported saves or weaken communications integrity to force compatibility.

Acceptance: enumerate every displayed choice in both wired and wireless menus, verify exact protocol IDs, reject a peer requesting an unsupported format/party size, cancel at every selection step, and recover an old singles session without starting a new battle. Trading and record exchange still work.

### Imported Secret Base owner challenges

All24base map templates route through `data/scripts/secret_base.inc:SecretBase_EventScript_RecordMixTrainer`. The legacy persona paths eventually reach `SecretBase_EventScript_BattleTrainer`, which invokes `PrepSecretBaseBattleFlags` and `SPECIAL_BATTLE_SECRET_BASE`. The current C flags are singles and the imported team is not part of the reviewed native catalog.

Final disposition is **retire the native challenge interaction**, preserving the owner's presence, imported profile and base contents. At `RecordMixTrainer`, route to a new harmless owner dialogue before any persona challenge path, save prompt, daily battled-owner write or battle-state preparation. Suggested final text:

```
Welcome to my SECRET BASE!\n
Make yourself at home.\p
For a DOUBLE BATTLE, visit a BATTLE\n
TENT or the CHAMPIONS CIRCUIT.$
```

Retain appropriate existing owner-name/flavor buffering where it is side-effect free. `SecretBase_EventScript_BattleTrainer` also receives a defensive retired-entry return: explain the supported doubles destinations and return with the calling script's correct return semantics, without calling either battle special. Do not reset daily state, delete imported Pokémon data, remove map IDs or silently convert arbitrary imported teams.

Acceptance: interact with all persona selectors through the shared root, confirming no battle, save prompt or battled-owner mutation. Exercise the defensive old label directly in an isolated script fixture. Base entry/exit, decoration, trading and record exchange remain intact. The world event ledger's W-C-LINK references this exact contract.
