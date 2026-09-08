# Emerald Champions — Cohesion and Rearchitecture Book

**Complete proposed implementation specification · September 8, 2026.**

This reading edition combines the decision index and all ten main chapters. The [full index](README.md) links every battle, map, encounter table, species/form and Mega Stone review. The live game has not been reimplemented or playtested as part of writing this book.

## Contents

- [Decision index](DECISIONS.md)
- [00 — Governing design and authoring contract](chapters/00-design-charter.md)
- [01 — Two partners, one complete opening](chapters/01-opening-and-starters.md)
- [02 — Competition rules, party sizes, levels and Champions Points](chapters/02-competition-rules.md)
- [03 — Wild distribution, biomes and usable choice](chapters/03-wild-distribution-and-biomes.md)
- [04 — Obtaining every functional Pokémon and form](chapters/04-acquisition-evolutions-forms.md)
- [05 — Mega Stones and legendary rewards](chapters/05-mega-stones-and-legendary-rewards.md)
- [06 — Shared expert AI and tactical execution](chapters/06-shared-ai.md)
- [07 — Preparation, move access and evolution convenience](chapters/07-preparation-and-evolution-convenience.md)
- [08 — Implementation sequence and global reconciliation](chapters/08-implementation-sequence.md)
- [09 — Tests, gates and honest evidence](chapters/09-tests-and-evidence.md)

---

## Decision index

This is the proposed revision to review before implementation. The user's final instructions govern the book: expert trainer doubles throughout, two permanent starters and a doubles Birch rescue, ordinary wild/legendary captures remaining singles, at least four opponents in a standard team, and a Medium floor of the live cap minus two including gyms. Existing multi-party ownership is preserved. Champions Points use the actual flat-stat calculation.

### The resulting game

The player chooses two distinct starters from one generation and keeps both. Their first rescue is doubles. Catching, healing and free competitive preparation are available before the four-member Route103 rival, whose starter is the unchosen member of that generation. Every subsequent trainer has the same expert tactical capabilities; species, composition, levels and party size create the difficulty hierarchy.

Roxanne regains Nosepass, Phoebe regains Dusclops, and Glacia regains Walrein with fully specified competitive roles. Other established signatures remain. The review preserves 501 of 516 complete native trainer loadouts. The 15 changed branches include six opening aliases, these three signature restorations, and six precise timing, ability, form or move repairs. Changes to shared levels and AI still affect preserved rosters.

Exploration keeps the current map topology and habitat identities. Missing ordinary families and useful functional forms receive specific routes, while rare personality forms also have direct alternatives to repeated catching. Collection means obtaining and using supported species/forms within one save over time; the existing 14 boxes are retained. Mega rewards remain discoveries and earned entitlements. Existing world interactions are corrected where their advice, state flow or reward retry no longer matches the game.

### Authoritative contracts

|Decision|Final instruction|Implementation authority|
|---|---|---|
|INTRO-01|Two selected permanent starters; doubles rescue; four-member rival; early supplies/preparation; exact regional rivals and legacy-save handling.|[Opening](chapters/01-opening-and-starters.md)|
|DIFF-01|Set normal trainer level to `max(authored level, live cap)` before the single Easy/Medium/Hard reduction of 4/2/0. Include gyms; preserve higher authored levels.|[Competition rules](chapters/02-competition-rules.md)|
|TEAM-01|Standard opposing teams contain at least four. Preserve explicit three-plus-three half multis and full six-plus-six story multis. Ordinary captures remain singles.|[Competition rules](chapters/02-competition-rules.md)|
|POINTS-01|Use effective IV31, 66 total flat Stat Points and 32 per stat; evaluate actual early-level thresholds and native integer rounding.|[Competition rules](chapters/02-competition-rules.md) and [early benchmarks](battles/early/points-benchmarks.md)|
|SAI-01|One explicit expert campaign profile across trainer classes and difficulties, with a defined information boundary.|[Shared AI](chapters/06-shared-ai.md)|
|SAI-02–12|Repair damage/critical forecasts, simulated-state contamination, entry effects, switch/ace/Mega evaluation and pair coordination. Use a bounded one-turn action-pair search with cached expensive calculations.|[Final AI contracts](review/shared-ai-final.json)|
|Authored trainer revisions|Apply only the complete final teams and stated shared obligations. Retain the reviewed successful strategies.|[Early](battles/early/README.md), [late](battles/late/README.md), [all branches](review/all-trainers.json)|
|Wild slots and form additions|Apply 29 exact slots across 23 maps; preserve other tables, species-family alternatives, ordinary encounter floors and habitat roles.|[Wild volume](chapters/03-wild-distribution-and-biomes.md)|
|WATER-AUTHOR-01|Use one optional `fishing_species_overrides` object after all fishing slots are generated, for Route117 Paldean Wooper and Route118 blue-striped Basculin.|[Wild volume](chapters/03-wild-distribution-and-biomes.md)|
|GUIDE-01|Show reachable ordinary encounter methods. Suppress the hidden-method advertisement while DexNav is disabled.|[Wild volume](chapters/03-wild-distribution-and-biomes.md)|
|WILD-ENGINE-01|Check encounter-generation success before starting the ordinary Sweet Scent battle, including the caught-Pheromosa rejection path.|[Wild volume](chapters/03-wild-distribution-and-biomes.md)|
|FORM-01–12|Maintain explicit conditional routes for every active configured form; distinguish cosmetic services, costumes, evolutions, transformations and excluded legacy gimmicks.|[Acquisition/forms](chapters/04-acquisition-evolutions-forms.md)|
|New form resources|Seven prepared gifts, eight supplemental wild slots already included in the 29 above, and 14 nonprotected form tools in the current evolution archive.|[Acquisition/forms](chapters/04-acquisition-evolutions-forms.md)|
|Galarian birds|Append Sign IDs 82–84, preserving earlier IDs and receipts. Use exact species/form lookup and ownership rather than shared base-species caught status.|[Acquisition/forms](chapters/04-acquisition-evolutions-forms.md)|
|Mega rewards|Preserve all reviewed world placements and genuine Ring/form prerequisites. Extend starter receipts coherently for the second starter; preserve retry and protected relic capacity.|[Mega rewards](chapters/05-mega-stones-and-legendary-rewards.md)|
|PREP-02|Allow the Leveler to perform an already-ready normal evolution at the cap using existing conditions, cancellation and scenes; grant no extra EXP or levels.|[Preparation](chapters/07-preparation-and-evolution-convenience.md)|
|PREP-02-B|Allow ready battle-only evolution after an eligible successful battle when EXP is disabled; filter out ordinary evolutions and temporary facility normalization.|[Preparation](chapters/07-preparation-and-evolution-convenience.md)|
|FORM-11-NATURE|Toxel follows the edited Nature used by the builder and stats. Preserve unrelated personality-dependent form rules.|[Acquisition/forms](chapters/04-acquisition-evolutions-forms.md)|
|PREP-03|Preserve intentional native expanded move access. Resolve reference mismatches explicitly; do not purge existing sets because a narrower reference lacks a move.|[Preparation](chapters/07-preparation-and-evolution-convenience.md)|
|PREP-04/05|Expose the specified nonprotected form tools; make the Surf-forgetting guard follow actual unlocked party capability.|[Preparation](chapters/07-preparation-and-evolution-convenience.md)|
|PREP-06|Offer optional friendship adjustment through the existing specialist and rater, with preview, confirmation, Egg rejection and unchanged evolution conditions.|[Preparation](chapters/07-preparation-and-evolution-convenience.md)|
|World repairs|Apply the 27 proposals' exact targets: truthful story/Sign advice, defined NPC choreography, Spiritomb access, Trick House delivery, current facilities and optional Wallace restoration.|[World changes](world/changes.md)|
|Gym dialogue|Replace 40 named texts whose unconditional strategy claims or species references no longer fit actual play.|[Gym dialogue](battles/early/gym-dialogue.json)|
|FAC-01|Make all three Battle Tents local six-member competitive doubles exhibitions using the existing generator, cap normalization and their existing prizes.|[Competition rules](chapters/02-competition-rules.md)|
|FAC-02|Keep Trainer Hill geography, time attack and rewards; generate one coordinated six-member opposing side per floor with correct paired ownership.|[Competition rules](chapters/02-competition-rules.md)|
|FAC-03|Keep the shared Frontier Circuit route and truthful current records. Record a new observed best; never invent a historical maximum.|[Competition rules](chapters/02-competition-rules.md)|
|LINK-01|Offer Double/Multi/Info/Exit with unchanged protocol meanings. Retire native imported Secret Base battles while preserving profiles, records and decoration.|[Competition rules](chapters/02-competition-rules.md)|
|Save version 4|One migration owns all reclaimed variables, gift/Trick receipts, opening phases, second-starter stone bits, new Sign bits and recorded Circuit best.|[State allocation](appendices/state-allocation.md) and [opening](chapters/01-opening-and-starters.md)|
|Evidence policy|Keep checks for real behavior and data integrity; update changed requirements; retire arbitrary showcase, duplication and strategy quotas. Native tests and rendered scenes follow implementation.|[Tests and evidence](chapters/09-tests-and-evidence.md)|

### Preservation and rejected changes

The book does not add a general player Item Clause, change Champions' stat formula, enlarge storage/save structures, require every Mega or legendary to appear on an NPC team, or distribute novel strategies by badge. Ordinary wild captures remain singles. Prepared sets remain starting suggestions within the native expanded move policy.

Current source fixes for Meltan, Ursaluna, Kingambit, Runerigus and regional evolution paths are retained. High-tide Shoal layout names do not justify extra encounter headers: the current map/header routing already supplies them. Heatran's physical hide flag is not proof of a capture-ownership bug. Sitrus-to-flavor-berry replacement was rejected where the actual HP parity prevents the proposed activation. Unbound legacy scripts are identified as inactive rather than assumed to be live broken NPCs.

Implementation starts by comparing relevant current files to the frozen manifest. Apply the [nine integration units](chapters/08-implementation-sequence.md), with each shared concern retaining its named owner. The book defines a complete design proposal; it does not certify that a future implementation is balanced, traversable or ready to release.

---

## 00 — Governing design and authoring contract

### Authority and scope

The user requested a separate, exhaustive implementation book before changes to the game. This book's files are the only output being authored. `baseline/source/` is an evidentiary snapshot, not a second implementation checkout. Never edit it or the live source to make a proposal appear verified.

The current game is the authored baseline. Later user instructions override snapshot AGENTS.md where they conflict, specifically the abolished opening singles exception and the universal Medium level floor. Existing teams are substantial finished content. Inspect every trainer branch individually and preserve its composition, moves, investment, items, abilities, levels, lead order and personality unless a concrete defect or an explicit cohesion objective warrants a documented revision. A new idea alone is not evidence that the existing authoring is inferior.

The final specification must distinguish source facts, design judgments, exact proposed changes, and acceptance evidence still required after implementation.

### Non-negotiable experience

1. Every campaign trainer encounter is an expert-level puzzle from the opening onward. Do not design beginner stages, delayed access to strategic concepts, or deliberately weak AI for ordinary trainers.
2. All tactical capabilities—priority, sleep, redirection, Fake Out, speed control, field control, switching and partner coordination—belong throughout the game. Every trainer's composition, moves, investment and AI should pursue the strongest coherent, clever strategy for its selected pieces. Do not simplify strategy by trainer rank, confuse prose repairs with adequate tactical review, or claim mathematical optimality from source inspection.
3. Difficulty is primarily calibrated with effective species strength, levels, and party size. Effective strength includes ability, item, form, investment and role rather than raw base-stat total alone.
4. **There are no singles trainer competitions in the proposed game.** All six opening rival branches become four-Pokémon doubles. Birch's preceding rescue is a specifically requested scripted doubles encounter in which the player selects and keeps two distinct starters. Ordinary wild and one-off legendary captures remain singles. Standard trainer teams must never have fewer than four Pokémon. Preserve explicitly authored multi-party sizes: Half layouts contribute3+3=6; Full two-trainer layouts can contribute6+6=12 (E0127). Judge the combined opponent side rather than inflating each half tofour or trimming fullparties tosix. Native legacy singles challenge paths must be converted or retired explicitly.
5. Easy/Normal/Hard retain the shared-team/shared-AI model and their existing global level calibration. **On Medium, every normal campaign opponent Pokémon must be at least the live player level cap minus two, including gym trainers.** Central contract DIFF-01 will implement this consistently; existing gym exemptions are not the proposed behavior. Preserve authored offsets and stronger levels rather than mass-editing them to simulate a shared runtime floor. This book does not silently adopt the illustrative new level bands from earlier discussion.
6. Player choice and convenient preparation are central. Preserve free nature, ability, Stat Point, legal move and held-item preparation, cap-respecting Leveler access, healing, Retry and separate Reload Save. A clever solution is allowed to win cleanly.
7. There is no rule that each Pokémon needs its own chapter, that early exotic Pokémon are inherently undesirable, or that the game should progress from weak species to strong species. Preserve good early options; justify changes against actual battle and habitat needs.
8. Preserve broad access to babies and useful unevolved/early-evolving Pokémon. Evolution requirements must be practical and clearly explained. Do not revive egg-only baby restrictions or breeding-stat grind.
9. Habitat identity and nostalgia matter. Retain recognizable residents, meaningful regional discoveries and iconic trainer Pokémon. In particular, Glacia's Walrein must return with a substantive competitive role. Audit other principal trainers for missing signature species; document exact team consequences.
10. Mega Stones remain exploration, NPC and modest berry-exchange rewards. Preserve the Ring's existing acquisition and the free evolution-item archive. No universal free Mega archive.
11. Ordinary species encounter chance remains at least 5% per method, with the existing Feebas exception separately evaluated. Geographic discovery should not become repetitive random grinding.
12. Legendary guidance at Devon is optional. Retain deliberate field landmarks, appropriate existing story/badge gates, sparse meaningful species prerequisites, retry after revisiting an area, and permanent caught/reward state. Older mandatory research and permanent failure rules are superseded.
13. Magnemite retains New Mauville's technological association, Duskull retains Mt. Pyre's identity, and Dondozo/Tatsugiri retain their underwater/submarine relationship. Habitat improvements should preserve player options and account for displaced families globally.
14. Treat every advertised supported battle-relevant species/form as an acquisition obligation. Distinguish a family appearing somewhere from the actual requested form being obtainable in a single save.

### Required entry format

Every reviewed object or system needs a traceable disposition. Use **KEEP**, **REVISE**, **REPAIR**, or **INERT/EXCLUDED**. A preserve decision must identify what was examined and why it fits; it must not be an unexamined default used to inflate completion.

- Source: exact snapshot file and label/identifier, with useful line references.
- Current: the actual existing behavior or authored loadout.
- Assessment: coherence, access, identity, strategic purpose and failure conditions.
- Final specification: the entire proposed result or an explicit KEEP of the reproduced baseline.
- Dependencies: other entries or shared rules affected.
- Acceptance: the smallest meaningful source, runtime, traversal or battle evidence required.

### Parallel ownership and integration

- Early battle reviewer owns trainer groups through encounter E0250.
- Late battle reviewer owns trainer groups after E0250, including League/postgame and explicit iconic restoration proposals.
- World reviewer owns maps, NPCs, objects, warps, connections, coordinate/background events, story handoffs and interaction prose.
- Lead editor owns wild availability, forms/evolutions, Mega/legendary rewards, shared AI, global reconciliation, source binding, final index and book validation.

Battle and world reviewers may recommend shared changes, but shared AI and acquisition rules receive one final specification under lead-editor ownership. No reviewer may modify another reviewer's output without coordination. No agent may change live game files, baseline source, ROMs, saves, installed apps, git history or external services.

### Evidence boundaries

Catalog coverage, textual analysis and graph checks are not empirical playtesting. Historical test results retain their recorded source boundary. A table of intended strategies is not proof the live AI executes them. Geometry alone does not solve all stateful movement and story reachability. Record concrete source conclusions and exact downstream verification requirements without inventing results.

Keep implementation tests focused on observable behavior and demonstrated failure modes. Do not install prose locks, fixed strategy quotas, universal species-count quotas or historical encounter-count floors in the game.

### Completion contract

The book is ready for implementation review when all in-scope inventory entries have traceable dispositions; every trainer branch has been individually examined; every proposed roster has a complete final loadout; every source-backed repair and approved nostalgia objective has an exact specification; cross-chapter conflicts are resolved; source links and coverage reconcile; and remaining implementation-time evidence is clearly listed. Completion does not claim the proposed game has been built or all battles have been won.

---

## 01 — Two partners, one complete opening

**Disposition: REVISE.** This chapter implements the user's final opening decisions in instructions only. Choose two different starters from one regional trio, rescue Birch in a genuine two-against-two scripted wild battle, keep both Pokémon, and meet a four-Pokémon doubles rival whose starter is the third, unchosen member. Ordinary wild encounters, legendary captures and Wally's scripted capture tutorial remain singles. All trainer competitions remain doubles, with at least four opposing Pokémon in an independent standard team and the shared live-cap level floor.

The rival's first pair is the unchosen regional starter plus Pikachu. Shroomish and Taillow are its two reserves. Complete Hoenn branches are in [the early battle volume](battles/early/0001-0020.md), and the exact regional starter slot for all 27 possibilities appears below. This chapter supersedes every historical single-starter opening and single-rival exception elsewhere in the baseline.

### What the current source actually does

| Source | Current behavior | Required change |
|---|---|---|
| [Regional choice](baseline/source/data/scripts/emerald_champions.inc:616) | A nine-region menu chooses the bag's trio; dialogue promises a rival with the type advantage against one choice. | Retain the region menu and the three familiar bag balls, but promise two partners and the remaining rival starter. |
| [Starter chooser](baseline/source/src/starter_choose.c:620) | The first YES returns one index immediately. | Store the first provisional index, choose a distinct second, then confirm the pair. |
| [Grant callback](baseline/source/src/battle_setup.c:1006) | Writes `VAR_STARTER_MON`, gives one level-5 Pokémon, and applies raw doubles preset 0 to party slot 0. | Prepare and commit two permanent Pokémon before any battle controller reads the party. |
| [First-battle flag](baseline/source/src/battle_setup.c:1020) | Sets only `BATTLE_TYPE_FIRST_BATTLE`. | Set `FIRST_BATTLE | DOUBLE` for this Hoenn scene. Do not add `TRAINER`, `MULTI`, or a global wild-doubles rule. |
| [Enemy creation](baseline/source/src/battle_controllers.c:154) | Overwrites the first encounter with one level-2 Zigzagoon. | Build the two exact prepared rescue foes below. |
| [First-battle AI](baseline/source/src/battle_ai_main.c:6848) | The special beginner routine flees when the player reaches 20% HP. | Use the shared expert doubles evaluator; remove this mercy behavior from the Hoenn rescue only. |
| [Bag script](baseline/source/data/maps/Route101/scripts.inc:218) | Sets rescued/Pokémon flags before battle, then always proceeds to success and the lab. | Commit ownership after successful pair creation and rescue progress only after actual victory. |
| [Lab receipt](baseline/source/data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc:133) | Acknowledges/nicknames only party slot 0. Dex and five Balls come after the rival. | Acknowledge both existing partners, provide Dex and 20 Balls before Route103, and preserve the later story debrief without duplicate gifts. |
| [Rival dispatch](baseline/source/data/maps/Route103/scripts.inc:38) | Switches on the single player's choice and calls singles trainer battles. | Dispatch by the unchosen third starter and use four-Pokémon doubles. |
| [Regional replacement](baseline/source/src/battle_setup.c:2387) | Computes `(first + 1) % 3`, with a singles-preset exception and first-compatible doubles fallback. | Compute `3 − first − second`; retain exact starter stage and a coherent legal doubles loadout. |
| [Archive exclusion](baseline/source/src/field_specials.c:257) | Excludes only the first original starter. | Exclude both immutable initial choices. |
| [Starter stones](baseline/source/src/field_specials.c:555) | Supports up to two stones for one family, including Charizard X/Y. | Use four stable family/stone receipt slots covering both families, with at most three actual stones for current two-starter regional pairs. |

These are source findings. No opening scene or proposed rescue matchup has been run in an emulator during this documentation pass.

### Selection and permanent ownership

Keep the existing region selection before the bag. `VAR_STARTER_GEN` retains its current semantics: 1–9 are Kanto through Paldea; 0 remains the explicit old-save Hoenn fallback. Use `[Grass, Fire, Water] = [0, 1, 2]` within every trio.

The native three-ball chooser needs one extra selection phase, not a replacement selection framework:

1. Show the selected regional trio and “Choose your first partner.”
2. Selecting a ball shows its Pokémon and confirmation using the current sprite/window behavior. YES marks it as first without exiting the chooser.
3. Return to the three balls with the first marked unavailable and “Choose your second partner.” A second selection must differ. Pressing B at this phase returns to the first choice; it does not grant anything.
4. Show both species names and “Take these two partners?” Declining returns to selection with no party mutation. Confirming validates both indices and enters the grant callback.
5. Build both Pokémon in temporary `struct Pokemon` values, apply the exact initial sets below, validate them, check capacity, and commit both together. Update the first/second values and opening state in the same non-yielding operation. A failing constructor/preset/capacity check commits neither.
6. Present a short bag menu: **Battle / Prepare pair**. The pair is now permanently owned, and its family choices are fixed. Preparation can alter legal moves, ability, nature, Stat Points and ordinary held items without giving another Pokémon. Use the existing preparation functions and menu implementations through a return-to-caller entry point; do not duplicate their legality, Stat Point or item tables.

Fresh games have an empty party and available capacity. The ordinary successful path installs the two selected Pokémon in slots 0 and 1. Never silently overwrite a nonempty party or send one rescue partner to the PC while starting with only one battler. An unexpected capacity failure returns a clear message and no partial grant; the developer fixture must expose rather than mask that invalid startup state. PC fullness cannot block the normal fresh two-party-slot transaction because the PC is not needed.

Both gifts retain species, personality, OT, held item and preparation through rescue loss, Retry, lab acknowledgment and saving. `ScriptGiveMon` currently mutates inventory/party immediately and its result must never be ignored; do not call it twice and assume both succeeded. Use the existing prepared-mon construction/insertion primitives with an atomic two-slot precondition. Lab scripts acknowledge ownership; they do not generate the Pokémon again.

The current default catalog contains all-support Chikorita and Torchic choices. For this two-member opening, the exact initial sets below add a direct damaging option to both. Preserve the rest of the existing expanded native move access. Squirtle Follow Me, Cyndaquil Nature Power, Piplup Scald and Scorbunny Blaze Kick differ from the narrower pinned reference, but the current preparation system intentionally admits existing preset moves. That reference difference is not proof of a gameplay defect and is not a reason to delete them. This chapter changes initial-grant choices only; the shared preparation chapter owns the explicit source/reference policy.

### Save state without expanding the save layout

The lead editor has reserved two declaration-only retired Hoenn nursery vars. A whole-source search found no current readers or writers of their old names; their historical values must still be treated as untrusted during migration.

| Final name | Address | Encoding |
|---|---:|---|
| `VAR_EC_SECOND_STARTER` | `0x40E1` | 0 = unset; 1 = Grass index 0; 2 = Fire index 1; 3 = Water index 2. Replaces `VAR_EC_RETIRED_HATCH_SPECIES_1`. |
| `VAR_EC_OPENING_STATE` | `0x40E2` | Explicit state enum below. Replaces `VAR_EC_RETIRED_HATCH_SPECIES_2`. |
| `VAR_STARTER_MON` | existing `0x4023` | First index 0–2; preserve for compatibility and first-partner cosmetic consumers. |
| `VAR_STEVEN_STARTER_STONE_DELIVERY` | existing `0x40E0` | Existing bits 0/1 stay first-family receipts; bits 2/3 become second-family receipts. |

`0x40E3` is centrally allocated by the same v4 migration to `VAR_EC_CIRCUIT_BEST_WINS` (the former third retired nursery var). The shared migration must initialize all three reclaimed slots before any v3 early return; the Circuit chapter owns its record semantics. Do not use `VAR_GIFT_UNUSED_7`: `ClearMysteryGiftVars` writes it, so it is not an unowned slot. Do not expand `VARS_END`, `SaveBlock1`, `SaveBlock3`, the serialization layout, or the checksum contract for this feature.

| Value | State | Durable meaning |
|---:|---|---|
| 0 | UNSELECTED | No pair transaction committed. |
| 1 | PAIR_GRANTED_RESCUE_PENDING | Both permanent gifts exist; rescue/preparation may resume without granting again. |
| 2 | RESCUE_WON_LAB_PENDING | Actual victory committed; lab acknowledgment and early supplies pending. |
| 3 | PRE_RIVAL_READY | Dex and initial Ball packet delivered; preparation/capture works; rival or its post-win debrief may still be pending according to existing story vars. |
| 4 | OPENING_COMPLETE | The existing post-rival story wrap-up is complete; no introductory grants repeat. |
| 5 | LEGACY_SECOND_OWED | Existing single-starter save needs a once-only supplemental second partner, without rewinding its story. |

The existing `VAR_ROUTE101_STATE`, `VAR_BIRCH_LAB_STATE`, rival-defeated flag and hide flags continue to own their original scene/chronology functions. The new state owns the pair transaction and gift receipt boundary. Do not reinterpret `VAR_BIRCH_LAB_STATE >= 5` merely because the Dex is available earlier: that would prematurely release unrelated story gates.

Advance the existing core save version from 3 to 4. The current [migration](baseline/source/src/overworld.c:2409) returns early for version 3; modifying the version constant alone would also stop recognizing those saves and can send them into the ambiguous-state reset path. Explicitly preserve v1/v2/v3 migration behavior, then initialize the two new vars and stamp v4 through a common completion path. Never stamp v4 before its state conversion is done.

- A genuinely pre-rescue save with no starter becomes state 0 with second value 0. Clear only premature old rescue completion if its old scene state proves the rescue was still pending.
- An old save caught in the pre-lab rescue sequence with one already-created starter must preserve that Pokémon, deterministically create the remaining non-rival starter when capacity is available, then resume state 1. Do not overwrite it with a freshly rolled first starter.
- An old save past the rescue keeps every story flag, party, box, item, difficulty and reward receipt. Set its second index to `(first + 2) % 3`, so the rival's existing `(first + 1) % 3` family stays unchanged. Mark state 5 and offer that second starter through Birch or his always-present lab aide exactly once. Put the aide's state-5 branch before ordinary dialogue, so Birch's daily fieldwork location never requires waiting for the gift. This compatibility gift is fixed to preserve the established rival, while fresh games retain all 54 ordered choices.
- The legacy supplement may use a free party slot or PC slot; prepare the Pokémon before insertion, preserve the returned destination for optional naming, and advance the receipt state only after successful insertion. Full party plus full PC leaves the gift owed, with no story rollback. Existing ownership of that species is not proof that this distinct new gift was delivered; the state is the receipt.
- For legacy old lab states **2, 3 or 4**, handle the owed second Pokémon before any acknowledgment that both partners are present. Set the old lab state to **3** while the supplement is pending, preventing the original state-2 or state-4 OnFrame scene from running prematurely. After successful supplemental insertion, set the **new opening state to 2**, then run or resume the same early Pokédex/20-Ball transaction used by fresh games. Do not jump directly from new state 5 to 3: old pre-rival saves have neither the Dex nor the early Ball grant. Only after both supplies succeed set new state 3. If `FLAG_DEFEATED_RIVAL_ROUTE103` is already set, queue the old lab-state-4 debrief; otherwise leave old lab state 3 for the still-pending rivalry battle. Preserve the defeated-rival flag throughout.
- Only a fully wrapped legacy **old lab state 5 with an existing Dex receipt** advances directly to new state 4 after successful supplementation. It receives no duplicate Dex or Ball packet and repeats neither the rescue nor rivalry battle.
- Corrupt/out-of-range indices never produce a negative index, out-of-bounds species or duplicate pair. Before any grant, return to selection. For a progressed legacy save, use its valid first choice plus the documented Hoenn fallback/remaining-index compatibility rule, and preserve the story rather than resetting unrelated game state.

Birch and the always-present aide use one shared entitlement handler and the same receipt state. Their dispatch order is: **owed legacy second starter (new state 5) → pending early supplies (new state 2) → optional ResearchPartners menu from the forms volume → ordinary dialogue**. The optional second Kubfu/Cosmog and postgame form gifts do not create another starter entitlement, bypass its pending transaction, or require waiting for Birch to return.

Birch is outside the lab on `VAR_BIRCH_STATE` values 2–5, and that state advances daily. The aide's pending-supplement branch must therefore work with Birch on Route101 or Route103, without changing his location flags or waiting for another day.

Legacy migration acceptance is table-driven: old lab states 2/3/4 must pass through new state 2 after the supplement, remain freely escapable at old lab state 3 during full storage, and only become ready after early supplies. Test both defeated-rival flag values, Pokémon-storage failure, Ball-storage failure, naming cancellation, save/load and lab exit/re-entry at each receipt boundary. Old lab state 5 plus Dex must avoid duplicate supplies.

Ordinary save/load and redundant-slot recovery are the persistence boundary. An external emulator savestate captured inside an old binary's active UI is not a supported cross-build migration format. The book does not require automatic disk saves during the opening.

### The rescue battle

Use a scoped `IsEmeraldChampionsBirchRescueBattle` predicate combining the Hoenn first-battle context with the new pending opening state. This predicate selects enemy creation, expert AI, capture/bag restrictions, restart reconstruction and outcome routing. Reuse `FIRST_BATTLE` for the existing narrative presentation and no-running behavior, while adding `DOUBLE`; do not introduce a global rule converting wild encounters into doubles.

| Foe | Level | Item | Ability | Nature | Points HP/Atk/Def/SpA/SpD/Spe | Moves |
|---|---:|---|---|---|---|---|
| POOCHYENA | 5 | FOCUS_SASH | RATTLED | JOLLY | 2/32/0/0/0/32 | CRUNCH, PLAY_ROUGH, SUCKER_PUNCH, HELPING_HAND |
| ZIGZAGOON | 5 | SITRUS_BERRY | GLUTTONY | ADAMANT | 2/32/0/0/0/32 | BELLY_DRUM, EXTREME_SPEED, SEED_BOMB, PROTECT |

These two opponents have no trainer reserves: this is the specifically authorized wild rescue, not an exception to standard trainer party sizes. Their level 5 is explicit scene calibration, not an application of the trainer cap floor. The player also begins with two fully prepared level-5 Pokémon, and can inspect/change their preparation through the bag menu before committing to combat.

The rescue enemy constructor zeros the enemy party once, fills slots0and1 with the complete prepared values, and updates party counts before controller initialization. Do not call the old one-mon constructor twice and overwrite the first foe. The pair has a real support/setup threat with immediate attacking alternatives. The AI must evaluate Belly Drum survival, immediate Sitrus recovery, Helping Hand timing, Sucker Punch eligibility, priority blocking and available targets. It must not always set up on turn one or always attack the first starter. Remove the current `AI_FirstBattle` automatic flee at low player HP; do not replace it with another hidden mercy rule.

Both wild Pokémon appear in the field scene. World proposal `W-INTRO-GEOMETRY` appends Poochyena at (6,14), elevation 3, facing up, with the existing rescue hide flag. After the current chase completes, it steps to (6,13) and faces left; Birch ends at (4,13), Zigzagoon at (5,13). Remove both foe objects during the bag's existing fade before relocating the player to (6,13). This is temporary scene removal, not a permanent success flag. On a field return after loss, while faded/locked, place the player at the verified clear standby (7,15), facing the bag at (7,14), then restore Birch/Zigzagoon/Poochyena to (4,13)/(5,13)/(6,13). Keep route state 2 and the bag visible before fading/releasing. Only victory commits permanent success hides and the lab warp. The world volume owns trajectory evidence and geometry validation.

Capture and ordinary Bag actions are unavailable in this scripted rescue, including with externally injected Balls. `WE_FLAG_NO_CATCHING` is configured as 0 in the snapshot; do not set flag zero or change the global wild config. Add the scoped rescue predicate to the appropriate Bag/ball eligibility check. This restriction also makes the existing party-only Retry snapshot sufficient: there is no Bag transaction to roll back.

Extend [the current Retry snapshot](baseline/source/src/battle_main.c:484) from eligible trainer fights to this exact rescue context. It snapshots both permanent partners after preparation, including their held items, PP and HP. At retry teardown, use the rescue enemy constructor instead of `EmeraldChampions_RebuildTrainerBattleParties`, restore the player snapshot, and re-enter battle initialization. Never pass through `CB2_GiveStarter` or the gift transaction on Retry. No trainer reward, defeat flag, new Ball or duplicate starter is produced by a failed attempt.

On loss, the ending callback must not fall into the old unconditional success script. Return to the pending rescue menu with restored partners: **Retry / Prepare pair**. A player may change their build, then take a new battle snapshot. The scene remains pending. Only a true victory commits state 2 and the rescue success flags, heals both partners, hides both foes/bag and warps to the lab.

The proposed rescue's empirical difficulty is not yet established. Acceptance requires winning lines for all 54 ordered choices, including the support-heavy combinations, with the documented preparation options and actual points formula. If a pairing fails, audit fixture preparation, legality, action rules and AI behavior before changing levels or weakening the puzzle. Do not claim a story-critical forced pair is viable merely because a different starter pair wins.

### Before the first rival: choice must already be practical

At the first lab receipt, Birch acknowledges both existing partners. Offer naming for each through the existing naming service using its actual stored slot/location; declining one name does not skip the other. Neither is regenerated. Then deliver the Pokédex and a packet of **20 Poké Balls** before sending the player toward Route103.

After acknowledging and offering names for both partners, set the **old `VAR_BIRCH_LAB_STATE` to 3 before any supply failure can release control**. The old lab-state-2 OnFrame trigger must no longer fire. Keep the **new `VAR_EC_OPENING_STATE` at 2** until the early supplies are complete; these variables own different duties. Add an opening-state-2 check before the ordinary Birch and aide dialogue so either interaction resumes the pending supplies. A full Bag/PC must allow the player to leave, free storage and return without another automatic lock. Route103 continues to require new opening state 3.

Call the existing `ReceivePokedex` functionality once, preserving its National/Dex flags and setup. Handle the Ball grant as a resumable transaction: if all 20 fit in the Bag, deliver once; otherwise try the PC item store. If neither store can receive the packet, keep state 2 and explain that the supplies are waiting. The player can exit and free storage, then resume with Birch or the aide. Do not mark the opening ready, repeat a successful packet, or permanently lose it because a fanfare/menu was interrupted. Set state 3 immediately after both receipt conditions succeed, before presentation that yields control.

Move the existing five-Ball post-rival reward into this earlier supply packet rather than stacking it unnoticed. The later state-4 lab event becomes a short debrief that retains the existing `FLAG_ADVENTURE_STARTED`, Oldale/Littleroot state changes and rival relocation. It must not give another Dex or Ball packet.

Keep ordinary catching available on accessible opening grass before the rival. The Route101 and Route103 encounter tables remain owned by the global distribution chapter. Route102 access still follows the existing story blocker; do not open the rest of Hoenn by advancing `FLAG_ADVENTURE_STARTED` early. Existing unlocked grass must already provide enough catchable options to build four to six Pokémon.

Oldale's Center already has a battle vendor at `(2,2)` and move tutor at `(13,2)`, both without hide flags. Retain them. The move tutor already defaults to doubles preparation; the vendor already provides ordinary held items before the Ring. The nurse already provides the Leveler, Poké Vial, Repel Spray and Flight Beacon. Verify all these menus are reachable after the early Dex grant and before the rival, without adding redundant NPCs.

Add a small opening-only Ball replenishment branch to Birch's aide or the existing Oldale Mart employee: while the Route103 rival is undefeated, if the player has fewer than 10 Poké Balls across Bag and PC, top up the Bag toward 20 with a clear “field supplies” message. Do not take money, create other item types or advance story. This is a deliberate convenience supply, not a once-only valuable-item reward. After the rival win, retain normal acquisition/economy. The player can therefore recover from failed captures instead of being stranded with two starters and no way to experiment.

Route103 first requires opening state 3 (or an already-completed compatible legacy state); partial Dex/supply delivery cannot start the challenge. Then it presents **Battle / Keep preparing** before starting. The battle requires the engine's existing two usable battlers; the player is allowed to attempt it with two excellent starters or build a larger team. The opposing team always has four. The shared Normal/Medium floor applies even if the authored cap offset is negative. Retry and Reload Save remain separate actions with separate meanings.

### Rival identity through the whole campaign

Validate `first ∈ {0,1,2}`, `second ∈ {0,1,2}` and `first != second`, then compute `unchosen = 3 - first - second`. Resolve species through `GetStarterPokemonForGeneration(unchosen, generation)`. This formula works for all six ordered choices in each region and never selects either owned starter.

Historical trainer identifiers remain stable. Their suffixes describe the old player's choice, so derive the existing dispatch selector as `(unchosen + 2) % 3`:

| Unchosen family | Existing selector | Existing suffix | Actual Hoenn starter |
|---|---:|---|---|
| Grass | 2 | `*_MUDKIP` | TREECKO line |
| Fire | 0 | `*_TREECKO` | TORCHIC line |
| Water | 1 | `*_TORCHIC` | MUDKIP line |

Replace the single-choice switches in Route103, Route104, RustboroCity, Route110, Route119 and LilycoveCity with a call to this one shared selector. Do not independently invent six different arithmetic expressions. Both genders use the same result. The later branch's existing reserve variant follows the unchosen starter, not merely the player's first selection.

`ApplyRegionalRivalStarter` must identify the unique authored Hoenn starter slot before mutation, retain its stage, level/offset and position, then replace that slot only. Opening slot 0 uses the exact regional table below. Mid-stage encounters use `GetMiddleEvolutionForStarter`; late encounters use `GetFinalEvolutionForStarter`. Validate exactly one replacement target in each rival template; unrelated Hoenn partners and Mega holders remain untouched.

The Lilycove audit specifically retains **Mega Metagross** for the Grass-starter branch, **Mega Salamence** for Water, and **Mega Tyranitar** for Fire, with the ordinary regional starter in slot 6 at offset +3. A regional starter preset must not steal the fixed partner's Mega allocation. Exclude required-item/required-move Mega/form presets from this replacement path. Use the exact [54 stage profiles and 270resolved variants](battles/early/regional-rival-profiles.md), including their specified held items. Opponent item uniqueness is an authoring convention, not a general player Item Clause. These rows resolve current conflicts while protecting strategy-dependent items, rather than stripping an item or taking an arbitrary first-compatible preset at runtime. Their [complete machine-readable variants](review/opening-regional-rival-profiles.json) preserve every other party member and the authored starter slot/offset.

The six Hoenn source branches remain complete authored teams; the runtime regional substitution matrix is an additional obligation. Its acceptance must include all regions and stages, not only the Hoenn templates. The broader battle book owns any later exact roster improvement; this chapter owns the mapping and invariants and rejects undefined, illegal or itemless error fallbacks.

### Both starter families receive their actual rewards

The archive's `IsEmeraldChampionsInitialStarter` check compares against both original base species using the immutable choices. It must not inspect only the current lead: evolution, boxing, trading or release must not reopen either initial gift as an unclaimed archive prize. The third regional starter remains normally obtainable from the archive, along with other noninitial starters. Preserve all existing per-species prize receipts and cost/gate decisions.

Steven's receipt ledger extends naturally:

| Bit/slot | Meaning |
|---:|---|
| 0 | First selected family's first stone |
| 1 | First selected family's second stone, currently Charizard Y when applicable |
| 2 | Second selected family's first stone |
| 3 | Second selected family's second stone |

Never sort this list: stable positions preserve old bits 0/1. `ITEM_NONE` is a satisfied nonreward slot. Deduplicate identical item IDs if a future mapping produces them, without reassigning receipt meanings. A current Kanto pair containing Charmander can deliver three distinct stones; a pair with no supported Mega receives an accurate no-applicable-stones explanation and the Ring/other progression continues.

Reuse `OwnsStevenStarterStone`: Bag, PC items, held party items and held boxed items all count. A delivered bit remains delivered after disposal, preventing duplication. Extend the delivery function to all four slots and iterate one receipt at a time in the map script, so its obtained-item scratch vars cannot overwrite a later receipt. A small indexed special can return item/destination/success for one slot, using an explicit script loop index; this is safer than passing four live receipt values through a helper that reuses them.

If any unfulfilled stone cannot fit in Bag or PC, Steven remains available, the pending bits remain clear, and the next interaction resumes only those slots. Do not execute `FLAG_HIDE_GRANITE_CAVE_STEVEN` or the departure choreography until all applicable starter stones are fulfilled. First-family legacy bits survive the v4 migration; second-family bits start clear. If a legacy second starter gift is still owed (new state 5), its selected family records the future mapping but does **not** yet create owed slots 2/3. Those slots become earned only when the actual second Pokémon is successfully delivered. An unused slot is not owed and does not set a receipt bit merely for being empty.

A v3 save may already have `FLAG_HIDE_GRANITE_CAVE_STEVEN` set after its old one-starter handoff. Merely preventing a future departure cannot make that absent actor reachable. At v4 migration completion and again immediately after a successful legacy second-Pokémon insertion, reconcile the real earned stone slots. If the old letter, Knuckle Badge and Ring handoff are already complete and at least one populated newly earned second-family slot remains unreceipted, clear **only** `FLAG_HIDE_GRANITE_CAVE_STEVEN`. Preserve the letter flag, Ring, badge/story state and existing first-family receipts. His return enters the pending-stones path before the old departure logic; it does not replay the letter, Ring or prior gifts. Keep him visible through full Bag/PC failures and depart only when all actually owed slots have receipts. Do not unhide him for a still-owed second Pokémon, a family without an applicable Mega Stone, already-fulfilled slots or a fresh pre-badge pair.

Other consumers remain deliberately scoped. Credits and the single starter-doll gift may use the first choice as a cosmetic representative; the doll dialogue should offer the matching Grass/Fire/Water Hoenn souvenir without falsely claiming it is a literal model of a non-Hoenn partner. Petalburg's Center starter-presence/type dialogue must recognize either selected family and its evolutions or use generation-neutral wording. Do not alter unrelated FRLG-only starter scripts merely because they use the same symbol in another configuration.

### Exact story text to implement

These are complete semantic paragraphs; implementation wraps them with the real game font and preserves existing sound/portrait/flag choreography unless a state change above requires relocation. They supersede conflicting one-starter wording in the world volume.

| Label/scene | Final text |
|---|---|
| `EmeraldChampions_Text_StarterRegionChosen` | “{STR_VAR_1}! Dad packed that trio in his field bag. You can choose two partners. I will train the one that stays behind.” |
| `Route101_Text_PleaseHelp` | “You there! Please help! There are two wild Pokémon after me! Choose two Pokémon from my BAG!” |
| Bag first/second prompts | “Choose your first partner.” / “Choose your second partner.” |
| Bag duplicate choice | “That Pokémon is already your first choice. Choose one of the other two.” |
| Pair confirmation | “Take {STR_VAR_1} and {STR_VAR_2} as your partners?” |
| Bag preparation menu | “Your partners are ready. Battle now, or prepare their moves and stats?” |
| Rescue defeat/pending | “Your partners are safe. Prepare your next attempt, and help BIRCH!” |
| First lab ownership text | “You and both Pokémon saved me. I would like them both to stay with you.” |
| Naming | “Would you like to nickname {STR_VAR_1}?” — ask independently for each partner. |
| Early Dex/supply handoff | “Take this POKéDEX and these POKé BALLS. There are many more partners out there. Oldale's Center can help you prepare any team you want.” |
| Rival introduction, both genders | “I took the partner you left with Dad. PIKACHU and the rest of my team are ready too. Bring the team you want, and show me your best!” |
| Rival challenge choice | “Ready for our Double Battle?” — Battle / Keep preparing. |
| Rival victory acknowledgment | “That was a serious battle! Let us tell Dad how our teams did.” |
| Post-rival lab debrief | “Welcome back! I am glad you both gave that battle everything. Your adventure is already underway. Keep finding partners and making the team your own.” |
| Full opening supplies storage | “Your field supplies are waiting here. Make room in your Bag or PC, then speak to me again.” |
| Legacy supplement | “I have another partner from your original regional trio for you. The Pokémon your rival chose will stay with them.” |
| Legacy full Pokémon storage | “Your second partner is safe here. Make room in your party or Boxes and return.” |
| Steven family-stone explanation | “I brought the available Mega Stones for both of your original partners. If there is no room, I will keep the remaining stones here.” |

A first-rival win need not involve any named tactic. Dialogue never claims the player used redirection, changed nature, denied Fake Out or beat a specific immunity unless the engine actually records that event and the text is intentionally conditional. No such recording subsystem is required for this opening.

### Acceptance and implementation order

1. **Source/state contract:** reserve the two approved vars, implement v4 migration safely, and keep saved-structure sizes/checksum boundaries unchanged. Test clean new game, v3 pre-rescue, v3 post-rescue/pre-rival, v3 post-rival, invalid retired var contents, full party/PC and a recovered redundant save slot. For full item storage after pair acknowledgment, assert old lab state3/new opening state2, free player movement for several frames, successful lab exit/re-entry, and one resumable supply handoff through either Birch or the aide. Re-running migration must change nothing after the first successful pass.
2. **Chooser and gift:** exercise every one of the 54 ordered choices, duplicate rejection, B/back, both confirmation refusals, failed preparation, unexpected capacity failure and re-entry after state 1. Assert exactly two permanent gifts, distinct families and stable IDs across retry/load. Verify no invisible extra third Pokémon or unauthorized held-item replacement.
3. **Rescue mechanics:** two active starters versus the exact two foes, expert AI with no low-HP flee, unavailable capture/Bag even with injected items, normal ordinary-wild formats afterward, both possible first faintings, loss, Retry, preparation after loss and victory. Snapshot restoration must return PP/items/HP without duplicating Pokémon or skipping the rescue.
4. **Actual puzzle viability:** test all 27 unordered starter pairs with both orderings, the exact points formula and legal preparation. Record at least one reproducible winning line for each, including support-heavy pairs; test alternate credible lines on representative pairs. This is runtime evidence still required, not a claim from the 54-row arithmetic table.
5. **Pre-rival access:** before Route103, capture additional Pokémon, use the Dex, obtain the Leveler, prepare nature/ability/points/moves/items, heal, use the PC and recover from failed catches through the Ball supply. Verify none of the later story blockers are accidentally lifted by the early Dex flags.
6. **Rival matrix:** both identities ×54 choices ×difficulty levels. Check four opponents, doubles format, unchosen species, exact companions, no item duplicates and the live Normal/Medium floor. Exercise Retry and a deliberate return to preparation without marking victory. Later milestone fixtures verify the proper evolved unchosen starter and preserve fixed Mega partners.
7. **Rewards:** all two-starter Mega combinations including Charmander first and second, families without Mega Stones, Bag-full/PC-free, both full, one already-held stone, an old delivered bit with the item discarded, and one pending second-family stone. Verify stable receipts and no premature Steven departure. Also test a previously hidden v3 Steven after a real supplemental second gift, no wake-up while that Pokémon is still owed, no-Mega/fully-receipted second families, and no repetition of letter/Ring/first-family gifts on the resumed handoff.
8. **Narrative/traversal:** render the two-choice screen, four-battler rescue, both visible wild actors, both name prompts, early Dex/balls, Oldale preparation, rival exit and post-rival lab scene. Confirm object positions, path blocking, locks/releases, fades/music and map state after save/load at every released boundary.

Implement this chapter before applying the six E0001 team changes in isolation: switching the rival to four Pokémon while withholding the player's second starter or capture tools would create an incoherent intermediate release. Merge the opening transaction, relevant world choreography, supplies, regional resolver and first-rival materialization as one reviewable feature slice. The rest of the book can then rely on the new immutable two-choice state.

The accompanying [machine-readable specification](review/opening.json) contains all 54 exact pair/foe mappings and the initial/rival loadouts. Its static checks pass index distinctness, rival exclusion and exact initial held-item preservation. Narrower pinned-reference move differences are listed and deliberately preserved under the expanded native policy. It does not certify runtime battle difficulty or scene execution. The [Stat Point appendix](battles/early/points-benchmarks.md) supplies exact opening and gym speed/point calculations, clearly separated from runtime battle proof.


### Complete initial player loadouts

All are permanent level 5 gifts. Only Chikorita and Torchic differ from the global default, to retain direct offense in a two-member rescue. Existing expanded move access is preserved; Squirtle Follow Me, Cyndaquil Nature Power, Piplup Scald and Scorbunny Blaze Kick remain despite differing from the narrower pinned reference. Both selected starters retain their exact listed items, including duplicate Eviolites where applicable. The source does not impose a general player Item Clause. The player can change preparation freely.

|Species|Level|Item|Ability|Nature|Points HP/Atk/Def/SpA/SpD/Spe|Moves|
|---|---:|---|---|---|---|---|
|BULBASAUR|5|EVIOLITE|CHLOROPHYLL|TIMID|2/0/0/32/0/32|GROWTH, SLEEP_POWDER, GIGA_DRAIN, SLUDGE_BOMB|
|CHARMANDER|5|LIFE_ORB|SOLAR_POWER|TIMID|2/0/0/32/0/32|HEAT_WAVE, WEATHER_BALL, DRAGON_PULSE, PROTECT|
|SQUIRTLE|5|EVIOLITE|TORRENT|BOLD|32/0/32/0/2/0|FOLLOW_ME, ICY_WIND, HELPING_HAND, PROTECT|
|CHIKORITA|5|EVIOLITE|OVERGROW|CALM|32/0/2/0/32/0|GIGA_DRAIN, HELPING_HAND, LIGHT_SCREEN, PROTECT|
|CYNDAQUIL|5|CHOICE_SCARF|FLASH_FIRE|TIMID|2/0/0/32/0/32|ERUPTION, HEAT_WAVE, EXTRASENSORY, NATURE_POWER|
|TOTODILE|5|LIFE_ORB|SHEER_FORCE|ADAMANT|2/32/0/0/0/32|LIQUIDATION, ICE_PUNCH, CRUNCH, PROTECT|
|TREECKO|5|BERRY_JUICE|UNBURDEN|ADAMANT|32/32/2/0/0/0|SWORDS_DANCE, BULLET_SEED, DRAIN_PUNCH, ACROBATICS|
|TORCHIC|5|EVIOLITE|SPEED_BOOST|TIMID|2/0/0/32/0/32|FLAMETHROWER, HELPING_HAND, WILL_O_WISP, PROTECT|
|MUDKIP|5|EVIOLITE|DAMP|CAREFUL|32/0/2/0/32/0|WIDE_GUARD, LIQUIDATION, ICY_WIND, PROTECT|
|TURTWIG|5|WHITE_HERB|SHELL_ARMOR|JOLLY|2/32/0/0/0/32|SHELL_SMASH, SEED_BOMB, SUPERPOWER, PROTECT|
|CHIMCHAR|5|EVIOLITE|IRON_FIST|ADAMANT|32/32/2/0/0/0|FAKE_OUT, FIRE_PUNCH, DRAIN_PUNCH, HELPING_HAND|
|PIPLUP|5|EVIOLITE|COMPETITIVE|BOLD|32/0/32/0/2/0|SCALD, ICY_WIND, YAWN, PROTECT|
|SNIVY|5|EVIOLITE|CONTRARY|TIMID|2/0/0/32/0/32|LEAF_STORM, GLARE, HELPING_HAND, PROTECT|
|TEPIG|5|EVIOLITE|THICK_FAT|JOLLY|2/32/0/0/0/32|FLARE_BLITZ, HEAD_SMASH, SUCKER_PUNCH, PROTECT|
|OSHAWOTT|5|EVIOLITE|SHELL_ARMOR|TIMID|32/0/2/0/0/32|ICY_WIND, CHILLING_WATER, HELPING_HAND, PROTECT|
|CHESPIN|5|EVIOLITE|BULLETPROOF|IMPISH|32/0/32/0/2/0|BULLET_SEED, SUPER_FANG, HELPING_HAND, PROTECT|
|FENNEKIN|5|EVIOLITE|BLAZE|TIMID|2/0/0/32/0/32|HEAT_WAVE, PSYCHIC, WILL_O_WISP, PROTECT|
|FROAKIE|5|FOCUS_SASH|PROTEAN|TIMID|2/0/0/32/0/32|HYDRO_PUMP, ICE_BEAM, U_TURN, PROTECT|
|ROWLET|5|LIFE_ORB|OVERGROW|JOLLY|2/32/0/0/0/32|SWORDS_DANCE, BRAVE_BIRD, LEAF_BLADE, SUCKER_PUNCH|
|LITTEN|5|EVIOLITE|INTIMIDATE|JOLLY|2/32/0/0/0/32|U_TURN, FLARE_BLITZ, WILL_O_WISP, FAKE_OUT|
|POPPLIO|5|EVIOLITE|LIQUID_VOICE|MODEST|2/0/0/32/0/32|HYPER_VOICE, MOONBLAST, ICY_WIND, PROTECT|
|GROOKEY|5|EVIOLITE|GRASSY_SURGE|ADAMANT|32/32/2/0/0/0|FAKE_OUT, GRASSY_GLIDE, KNOCK_OFF, PROTECT|
|SCORBUNNY|5|EVIOLITE|LIBERO|JOLLY|2/32/0/0/0/32|BLAZE_KICK, HIGH_JUMP_KICK, U_TURN, PROTECT|
|SOBBLE|5|SCOPE_LENS|SNIPER|TIMID|2/0/0/32/0/32|FOCUS_ENERGY, WATER_PLEDGE, WATER_PULSE, PROTECT|
|SPRIGATITO|5|LIFE_ORB|PROTEAN|JOLLY|2/32/0/0/0/32|SEED_BOMB, U_TURN, PLAY_ROUGH, PROTECT|
|FUECOCO|5|EVIOLITE|UNAWARE|CALM|32/0/2/0/32/0|FIRE_BLAST, WILL_O_WISP, SLACK_OFF, PROTECT|
|QUAXLY|5|EVIOLITE|MOXIE|ADAMANT|2/32/0/0/0/32|LIQUIDATION, AQUA_JET, RAPID_SPIN, PROTECT|

### Complete regional Route103 starter slot

The following exact slot0 loadout is combined with the three fixed companions in the early battle volume. The Hoenn rows preserve its six authored branch specifications. Other regional starters reuse their complete initial sets with a Sitrus replacement for Eviolite to avoid duplicating Shroomish's item. These are four-member teams, not single-starter battles.

|Species|Cap offset|Item|Ability|Nature|Points HP/Atk/Def/SpA/SpD/Spe|Moves|
|---|---:|---|---|---|---|---|
|BULBASAUR|-1|SITRUS_BERRY|CHLOROPHYLL|TIMID|2/0/0/32/0/32|GROWTH, SLEEP_POWDER, GIGA_DRAIN, SLUDGE_BOMB|
|CHARMANDER|-1|LIFE_ORB|SOLAR_POWER|TIMID|2/0/0/32/0/32|HEAT_WAVE, WEATHER_BALL, DRAGON_PULSE, PROTECT|
|SQUIRTLE|-1|SITRUS_BERRY|TORRENT|BOLD|32/0/32/0/2/0|FOLLOW_ME, ICY_WIND, HELPING_HAND, PROTECT|
|CHIKORITA|-1|SITRUS_BERRY|OVERGROW|CALM|32/0/2/0/32/0|GIGA_DRAIN, HELPING_HAND, LIGHT_SCREEN, PROTECT|
|CYNDAQUIL|-1|CHOICE_SCARF|FLASH_FIRE|TIMID|2/0/0/32/0/32|ERUPTION, HEAT_WAVE, EXTRASENSORY, NATURE_POWER|
|TOTODILE|-1|LIFE_ORB|SHEER_FORCE|ADAMANT|2/32/0/0/0/32|LIQUIDATION, ICE_PUNCH, CRUNCH, PROTECT|
|TREECKO|-1|LIFE_ORB|OVERGROW|JOLLY|2/32/0/0/0/32|SEED_BOMB, DRAIN_PUNCH, SWORDS_DANCE, QUICK_ATTACK|
|TORCHIC|-1|LIFE_ORB|BLAZE|JOLLY|2/32/0/0/0/32|FLARE_BLITZ, ROCK_SLIDE, SWORDS_DANCE, LOW_KICK|
|MUDKIP|-1|LIFE_ORB|TORRENT|ADAMANT|2/32/0/0/0/32|LOW_KICK, DOUBLE_EDGE, WATERFALL, ROCK_SLIDE|
|TURTWIG|-1|WHITE_HERB|SHELL_ARMOR|JOLLY|2/32/0/0/0/32|SHELL_SMASH, SEED_BOMB, SUPERPOWER, PROTECT|
|CHIMCHAR|-1|SITRUS_BERRY|IRON_FIST|ADAMANT|32/32/2/0/0/0|FAKE_OUT, FIRE_PUNCH, DRAIN_PUNCH, HELPING_HAND|
|PIPLUP|-1|SITRUS_BERRY|COMPETITIVE|BOLD|32/0/32/0/2/0|SCALD, ICY_WIND, YAWN, PROTECT|
|SNIVY|-1|SITRUS_BERRY|CONTRARY|TIMID|2/0/0/32/0/32|LEAF_STORM, GLARE, HELPING_HAND, PROTECT|
|TEPIG|-1|SITRUS_BERRY|THICK_FAT|JOLLY|2/32/0/0/0/32|FLARE_BLITZ, HEAD_SMASH, SUCKER_PUNCH, PROTECT|
|OSHAWOTT|-1|SITRUS_BERRY|SHELL_ARMOR|TIMID|32/0/2/0/0/32|ICY_WIND, CHILLING_WATER, HELPING_HAND, PROTECT|
|CHESPIN|-1|SITRUS_BERRY|BULLETPROOF|IMPISH|32/0/32/0/2/0|BULLET_SEED, SUPER_FANG, HELPING_HAND, PROTECT|
|FENNEKIN|-1|SITRUS_BERRY|BLAZE|TIMID|2/0/0/32/0/32|HEAT_WAVE, PSYCHIC, WILL_O_WISP, PROTECT|
|FROAKIE|-1|FOCUS_SASH|PROTEAN|TIMID|2/0/0/32/0/32|HYDRO_PUMP, ICE_BEAM, U_TURN, PROTECT|
|ROWLET|-1|LIFE_ORB|OVERGROW|JOLLY|2/32/0/0/0/32|SWORDS_DANCE, BRAVE_BIRD, LEAF_BLADE, SUCKER_PUNCH|
|LITTEN|-1|SITRUS_BERRY|INTIMIDATE|JOLLY|2/32/0/0/0/32|U_TURN, FLARE_BLITZ, WILL_O_WISP, FAKE_OUT|
|POPPLIO|-1|SITRUS_BERRY|LIQUID_VOICE|MODEST|2/0/0/32/0/32|HYPER_VOICE, MOONBLAST, ICY_WIND, PROTECT|
|GROOKEY|-1|SITRUS_BERRY|GRASSY_SURGE|ADAMANT|32/32/2/0/0/0|FAKE_OUT, GRASSY_GLIDE, KNOCK_OFF, PROTECT|
|SCORBUNNY|-1|SITRUS_BERRY|LIBERO|JOLLY|2/32/0/0/0/32|BLAZE_KICK, HIGH_JUMP_KICK, U_TURN, PROTECT|
|SOBBLE|-1|SCOPE_LENS|SNIPER|TIMID|2/0/0/32/0/32|FOCUS_ENERGY, WATER_PLEDGE, WATER_PULSE, PROTECT|
|SPRIGATITO|-1|LIFE_ORB|PROTEAN|JOLLY|2/32/0/0/0/32|SEED_BOMB, U_TURN, PLAY_ROUGH, PROTECT|
|FUECOCO|-1|SITRUS_BERRY|UNAWARE|CALM|32/0/2/0/32/0|FIRE_BLAST, WILL_O_WISP, SLACK_OFF, PROTECT|
|QUAXLY|-1|SITRUS_BERRY|MOXIE|ADAMANT|2/32/0/0/0/32|LIQUIDATION, AQUA_JET, RAPID_SPIN, PROTECT|

### Ordered choice truth table

Indices are Grass0,Fire1,Water2; stored second value is index+1. Each row applies to both rival identities and each difficulty.

|Gen|First|Second|Rival|Legacy branch selector|
|---:|---|---|---|---:|
|1|BULBASAUR|CHARMANDER|SQUIRTLE|1|
|1|BULBASAUR|SQUIRTLE|CHARMANDER|0|
|1|CHARMANDER|BULBASAUR|SQUIRTLE|1|
|1|CHARMANDER|SQUIRTLE|BULBASAUR|2|
|1|SQUIRTLE|BULBASAUR|CHARMANDER|0|
|1|SQUIRTLE|CHARMANDER|BULBASAUR|2|
|2|CHIKORITA|CYNDAQUIL|TOTODILE|1|
|2|CHIKORITA|TOTODILE|CYNDAQUIL|0|
|2|CYNDAQUIL|CHIKORITA|TOTODILE|1|
|2|CYNDAQUIL|TOTODILE|CHIKORITA|2|
|2|TOTODILE|CHIKORITA|CYNDAQUIL|0|
|2|TOTODILE|CYNDAQUIL|CHIKORITA|2|
|3|TREECKO|TORCHIC|MUDKIP|1|
|3|TREECKO|MUDKIP|TORCHIC|0|
|3|TORCHIC|TREECKO|MUDKIP|1|
|3|TORCHIC|MUDKIP|TREECKO|2|
|3|MUDKIP|TREECKO|TORCHIC|0|
|3|MUDKIP|TORCHIC|TREECKO|2|
|4|TURTWIG|CHIMCHAR|PIPLUP|1|
|4|TURTWIG|PIPLUP|CHIMCHAR|0|
|4|CHIMCHAR|TURTWIG|PIPLUP|1|
|4|CHIMCHAR|PIPLUP|TURTWIG|2|
|4|PIPLUP|TURTWIG|CHIMCHAR|0|
|4|PIPLUP|CHIMCHAR|TURTWIG|2|
|5|SNIVY|TEPIG|OSHAWOTT|1|
|5|SNIVY|OSHAWOTT|TEPIG|0|
|5|TEPIG|SNIVY|OSHAWOTT|1|
|5|TEPIG|OSHAWOTT|SNIVY|2|
|5|OSHAWOTT|SNIVY|TEPIG|0|
|5|OSHAWOTT|TEPIG|SNIVY|2|
|6|CHESPIN|FENNEKIN|FROAKIE|1|
|6|CHESPIN|FROAKIE|FENNEKIN|0|
|6|FENNEKIN|CHESPIN|FROAKIE|1|
|6|FENNEKIN|FROAKIE|CHESPIN|2|
|6|FROAKIE|CHESPIN|FENNEKIN|0|
|6|FROAKIE|FENNEKIN|CHESPIN|2|
|7|ROWLET|LITTEN|POPPLIO|1|
|7|ROWLET|POPPLIO|LITTEN|0|
|7|LITTEN|ROWLET|POPPLIO|1|
|7|LITTEN|POPPLIO|ROWLET|2|
|7|POPPLIO|ROWLET|LITTEN|0|
|7|POPPLIO|LITTEN|ROWLET|2|
|8|GROOKEY|SCORBUNNY|SOBBLE|1|
|8|GROOKEY|SOBBLE|SCORBUNNY|0|
|8|SCORBUNNY|GROOKEY|SOBBLE|1|
|8|SCORBUNNY|SOBBLE|GROOKEY|2|
|8|SOBBLE|GROOKEY|SCORBUNNY|0|
|8|SOBBLE|SCORBUNNY|GROOKEY|2|
|9|SPRIGATITO|FUECOCO|QUAXLY|1|
|9|SPRIGATITO|QUAXLY|FUECOCO|0|
|9|FUECOCO|SPRIGATITO|QUAXLY|1|
|9|FUECOCO|QUAXLY|SPRIGATITO|2|
|9|QUAXLY|SPRIGATITO|FUECOCO|0|
|9|QUAXLY|FUECOCO|SPRIGATITO|2|

---

## 02 — Competition rules, party sizes, levels and Champions Points

**Disposition: REVISE the shared floor and opening format; preserve the configured battle calculation and authored competitive intent.** This chapter is authoritative wherever an individual battle page reproduces an older authored level or AI profile. Native source values in those pages are evidence, not exceptions to this contract.

### DIFF-01 — Universal campaign level floor

The user's final requirement is exact: on Medium, no opponent Pokémon in a normal campaign trainer battle may be below the player's **live current cap minus two**. Gym trainers are included. An earlier route revisited after another badge uses the new live cap. The maximum authored encounter ID is unrelated to this calculation.

Current owner: [`src/difficulty.c:ApplyTrainerLevelDifficulty`](baseline/source/src/difficulty.c). The snapshot only applies its badge floor to field doubles and excludes maps using the gym battle scene. Gym Pokémon authored below their chapter cap therefore currently fall below the requested Medium floor after the additional two-level reduction. The old opening singleton is also outside the field-double condition; INTRO-01 supersedes that format.

#### Exact final calculation

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

#### Implementation boundaries and acceptance

1. Modify the single floor predicate in `ApplyTrainerLevelDifficulty`; retain the independent Circuit/Hill exclusions and normal type guards.
2. Confirm every authored slot's original level is1–100. Do not change hundreds of authoring offsets to emulate a shared rule.
3. Run a production-function host test with explicit trainer/double/gym flags and live caps 14,20,55,100. Verify the table above, especially a gym slot authored at 12 and a returned field trainer authored at 13.
4. Test the actual opening rival, a Rustboro gym trainer, Roxanne, a two-trainer story battle, and a postgame revisit. Inspect both live party level and displayed level, full starting HP and actual Speed. Source math alone is not proof the correct constructor ran.
5. Demonstrate that wild levels and player leveling do not receive the opponent reducer, and Circuit/exhibition/Hill do not receive it twice.

### TEAM-01 — Doubles competition and minimum opposing team size

All normal trainer competitions are doubles. Every independently controlled authored opponent team has at least four Pokémon. The opening Route103 rival is explicitly four, replacing the six historical singleton variants. These variants remain six dispatch aliases into the new three unchosen-starter identities, not six different stories.

Existing multi battles can combine two three-Pokémon contributions into a six-Pokémon opposing side when their explicit Multi Party Half authoring applies. Full two-trainer battles retain both six-Pokémon parties, for twelve total opponents across two active slots (E0127). Preserve the exact authored layout: a three-slot component is not an independent three-Pokémon competition, and a full two-trainer battle must not be trimmed to six total. Audit the combined side, its two lead slots, reserve ownership, one-owner-per-party budget and independent trainer defeat callbacks. Do not accidentally build an eight-Pokémon side by applying a four-slot minimum independently to the two halves.

The player may win with fewer survivors. Do not conflate the opponent's authored minimum with an obligation for the player to still have four healthy Pokémon after attrition. Entry to optional six-member exhibitions/Circuit uses its own preparation rule; ordinary story battle entry needs two usable battlers, with source-compatible party/partner exceptions examined under INTRO-01 and world handoffs.

Ordinary wild and legendary captures remain singles. Wally's catching demonstration is a capture sequence, not a trainer competition. The first Birch rescue is the explicitly requested scripted doubles exception.

Native e-Reader, obsolete facility and imported Secret Base challenge paths cannot silently expose singles or three-Pokémon competitions. The world volume specifies retirement of unsupported native entrypoints while preserving harmless maps, archived records and save identities. Do not edit arbitrary external link/recorded battle data as though it were authored campaign content.

### POINTS-01 — The actual Champions calculation

Sources: [`include/constants/emerald_champions.h`](baseline/source/include/constants/emerald_champions.h), [`src/pokemon.c:CalculateMonStatsCont`](baseline/source/src/pokemon.c), [`src/field_specials.c`](baseline/source/src/field_specials.c), and the native preparation API.

The builder permits 66 total Stat Points and 32 per stat. Under the configured Champions calculation, the stat function **forces effective IV31**, including Speed. Stored zero-IV flags therefore do not create a slower Trick Room Pokémon in this ruleset. Do not recommend a conventional zero-Speed-IV build or silently change the stat rules to make a borrowed set behave as it would in another game.

For base stat `B`, level `L`, points `P`, and nature multiplier `N`:

```
ordinary HP = floor((2*B + 31)*L/100) + L + 10 + P
other stat = floor((floor((2*B + 31)*L/100) + 5 + P) * N)
```

Nature uses the native integer modification, normally 0.9/1.0/1.1. Shedinja keeps its explicit one-HP handling. Ability, held-item, stage, weather and other battle modifiers are then applied through the normal battle calculation. Raw summary Speed does not already include all of those modifiers.

Authored spread notation is **HP/Atk/Def/SpA/SpD/Spe**. The native engine stores **HP/Atk/Def/Spe/SpA/SpD**. Use the existing materializer's field mapping; never copy a displayed array into an engine-order array without conversion. Legacy `MAX_TOTAL_EVS=510` and 252-EV constants elsewhere are not this builder's point budget.

These flat additions are especially consequential at level 5 and 14. Early loadouts must be evaluated at actual effective levels after DIFF-01, not by familiar level 50 or100 expectations. Preserve 32/32/2 spreads only when they serve the actual plan; precision spreads may buy a required turn order or survival threshold.

#### Required authoring evidence

For a timing-dependent strategy, state both final Speed values at the relevant difficulty and include priority/field modifiers. For survival-dependent partner activation, use exact HP/defense and actual move damage—including allied spread reduction and potential opposing follow-up attacks. For berry setup, use the native activation inequality and HP parity.

The early reviewer provides [early Points benchmarks](battles/early/points-benchmarks.md); the late volume supplies [its own measured benchmarks](battles/late/benchmarks.md). They are reproducible calculations and source checks, not empirical battle wins.

Sitrus in this snapshot activates at half HP rounded **up**. Gluttony plus a flavor berry uses a different threshold; replacing Sitrus solely because the alternative heals more can prevent Belly Drum's berry from triggering at odd HP. The late volume explicitly preserves Jack's relevant Sitrus case after checking this issue. Do not introduce automatic silent investment changes merely to preserve a proposed berry swap.

### PREP-01 — Expert preparation from the opening

INTRO-01 supplies two permanent prepared starters, a doubles rescue and pre-rival catching/preparation access. The first required trainer battle must permit the player to assemble a real team before committing. Preserve free nature, ability, legal move, held-item and Stat Point services, the cap-respecting Leveler, storage, healing, Retry and separate Reload Save.

The preparation UI must show actual resulting stats and remaining points. A selected nature or point change must immediately recalculate the displayed Pokémon through the same native function used in battle. Do not describe points as conventional EV fractions. Preserve the native budget and avoid silently adding a general player Item Clause: trainer authoring uses unique held items, but the snapshot does not universally enforce that rule on ordinary player parties.

A valid default preset is a convenient starting orientation. It does not restrict manual legal moves or force a player to use one catalog strategy. The preset consistency audit and exact exceptions are supplied in the preparation chapter; source/pinned-reference disagreements must be resolved deliberately rather than treated as automatic proof that one dataset is correct.

### FAC-01 — Competitive Battle Tent exhibitions

All three live Tent attendants become six-Pokémon doubles exhibition entrypoints. Reuse the existing competitive Circuit team generator and legal set construction; do not revive the inherited rental, autonomous Palace or Arena singles rules.

Sources include the three `{SlateportCity,VerdanturfTown,FallarborTown}_BattleTentLobby_EventScript_Attendant` roots, `src/battle_tent.c`, `src/champions_circuit.c` and the shared generated-battle constructor. World entries contain exact clerk and rule-board replacements.

#### Final mode and state contract

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

### FAC-02 — Trainer Hill

Retain the authored time-attack geography, floor progression and existing earned-prize storage. Replace inherited generated competition rosters with a coherent six-Pokémon opposing doubles side on each floor, using the shared competitive generator at DIFF-01 levels.

If the current paired NPC presentation remains, generate **one** complete six-Pokémon team, choose its coordinated two leads, and split its reserves between the existing three-slot owners. Do not independently generate two unrelated three-Pokémon parties. Native owner semantics still govern switches and Mega entitlement.

Own the change in `src/trainer_hill.c`, the `TrainerHill_EventScript_TrainerBattle` root and entrance/roof contracts. Preserve timer pause/resume and legitimate floor completion. Trace the actual `trainerhill_settrainerflags`/`trainerhill_lost` consumers before changing their apparent names; a cleanup call is not proof of a victory flag bug.

Hill runs do not affect Circuit counters or rewards. A defeated/withdrawn run restores the correct party/location state and cannot award roof completion. Test entry, every floor pair, a loss mid-run, withdrawal, timer behavior, generation failure and full-bag prize resumption. The world volume gives exact roots and map contexts.

### FAC-03 — Frontier records and presentation

All current Frontier challenge desks continue to admit the postgame Circuit. Their text must explicitly describe the shared central Tower competition. Historical records may be preserved as labeled archives; current boards must not advertise inaccessible singles, rentals, judging rules or Symbols.

Add a read-only `ChampionsCircuitBufferRecord` helper: current comes from `VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS` (0x40DB), lifetime from `VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS` (0x40DC), and the new `VAR_EC_CIRCUIT_BEST_WINS` reuses the declaration-only retired nursery variable at 0x40E3. INTRO-01's version 4 migration owns the reclamation alongside0x40E1/E2. Initialize best from an observed active current run only; otherwise0 means no best has yet been recorded. Never synthesize best from lifetime or retired facility records. On a real Circuit win, set best to max(previous best,new current); Tent/Hill wins cannot update it.

Buffer current in STR_VAR_1, best in STR_VAR_2, lifetime in STR_VAR_3 using five-digit-safe formatting. Show `--` for unrecorded best and label it `Best recorded`. The buffer writes no progress. World boards use it where they claim to show current Circuit performance; historical facility data remains untouched. Existing pending Circuit rewards remain claimable from the attendant after party/run restoration.

### LINK-01 — Native link menus and imported Secret Base challenges

This is an explicit final disposition for competition entrypoints outside the516 authored branches. Preserve trading, communications cleanup, imported records and harmless base decoration. Do not invent valid modern teams from arbitrary imported legacy data.

#### Wired and wireless battle choices

Owner: `src/data/script_menu.h:MultichoiceList_BattleMode`; `data/scripts/cable_club.inc:CableClub_EventScript_SelectBattleMode` and `WirelessBattleSelect`.

Final shared menu order is **Double Battle, Multi Battle, Info, Exit**. Both dispatchers must use indices 0/1/2/3 respectively; B cancels. The Double branch still sends the existing `USING_DOUBLE_BATTLE` / `LINK_GROUP_DOUBLE_BATTLE` protocol values, and Multi keeps its existing protocol values. UI indices must not be substituted for protocol constants.

Remove new-entry access to `CableClub_EventScript_SingleBattleMode` and `WirelessSingleBattle`. Retain the labels as guarded obsolete entrypoints if needed for safe recovery: show `This desk hosts DOUBLE BATTLES. Please choose Double or Multi.` and return to a safe selection/exit path without starting negotiation for singles. Retain existing single-session return/cleanup labels so a saved or interrupted old session can exit cleanly; their existence is not permission to begin a new singles competition.

Update `ExplainBattleModes` and `WirelessBattleInfo` together with the menu. Both players' selected rosters must meet a scoped wired/wireless admission requirement (at least four on an independent side); Multi contributions use their explicit combined-side semantics. Do not redefine the shared `HasEnoughMonsForDoubleBattle` guard to four: normal story entry and the scripted rescue still need its two-usable-battler semantics. Reject an incompatible peer request before battle entry with a clear explanation and normal communication cleanup. Do not mutate remote Pokémon, rewrite imported saves or weaken communications integrity to force compatibility.

Acceptance: enumerate every displayed choice in both wired and wireless menus, verify exact protocol IDs, reject a peer requesting an unsupported format/party size, cancel at every selection step, and recover an old singles session without starting a new battle. Trading and record exchange still work.

#### Imported Secret Base owner challenges

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

---

## 03 — Wild distribution, biomes and usable choice

**Final design:** preserve the great majority of the existing broad roster, fill concrete missing family/form routes, maintain recognizable habitats and make every advertised acquisition truthful. There is no weak-to-strong curriculum and no rule that a Pokémon must wait for its own chapter.

Read [all 138map reviews and exact tables](appendices/wild-encounter-catalog.md). Each map has an individually written assessment and full ordinary method inventory; changed methods include complete final slot tables. The exact machine specifications are in [wild-distribution.json](review/wild-distribution.json) and [wild-proposals.json](review/wild-proposals.json).

### What stays

The opening retains its babies, quick evolutions, Mienfoo, Ferroseed and exciting long-term choices. Dreepy, Bagon and Beldum are not moved merely because they evolve later. Pheromosa and Kartana are not delayed merely because they are strong. Their usefulness and the nearby authored battles must be judged together.

Magnemite remains a New Mauville discovery. Duskull remains a Mt.Pyre family. The volcanic, desert, rainforest, haunted, cold-water and underwater populations retain their recognizable cores. Dondozo and all Tatsugiri forms remain an underwater/submarine-focused discovery.

The user corrected the earlier scarcity-oriented proposal. Consequently this book does not turn Rusturf, Artisan Cave or Mirage Island back into nearly monotypic areas at the expense of existing choices. Their iconic residents, locations and interactions should carry identity while their broader useful rosters remain. The map reviews explicitly discuss this tradeoff rather than pretending broad variety is itself a defect.

Ordinary wild and legendary captures remain singles. `WE_DOUBLE_WILD_CHANCE` stays0 and the generic force-double flag remains disabled. INTRO-01's first Birch rescue is a specifically scripted doubles encounter; it must not enable double random encounters globally.

### Exact new family and form placements

The snapshot already contains Burmy in Petalburg Woods and Deerling in Verdanturf Meadow. It also already supplies local evolution routes for several previously missing forms. Those are preserved.

The slot revisions in the linked catalogue introduce missing regional or exceptional choices mostly by replacing a repeated evolved family that has another practical route. Examples include Galarian Zigzagoon/Squawkabilly on Route118, Galarian Meowth in the Safari Zone, Alolan Grimer on Route121, cold regional forms in Shoal, and the previously absent Paradox species in matching habitats. New availability does not require removing that family's existing rival or trainer showcase.

These are exact species replacements at existing indices. Keep the current minimum/maximum levels and weights. Every record names the displaced family and its remaining route. Additional Antique/Artisan/Tatsugiri/other form-specific entries from the acquisition chapter are integrated into the final catalogue; use that complete manifest when implementing.

The Paldean Wooper route is a deliberate exception to the generator's hardcoded Old Rod Magikarp rule: Route117's Old Rod becomes Wooper-Paldea60% and Tympole40%. Earlier Magikarp access remains in Dewford and many other locations. This adds Clodsire access without removing Tympole or requiring another map/encounter engine.

#### WATER-AUTHOR-01 — Small authoring extension

Add one optional `fishing_species_overrides` object to a row in `data/emerald_champions/wild_route_sheet.json`, mapping zero-based native fishing-slot indices to configured species names without the `SPECIES_` prefix. Route117 supplies `{"0":"WOOPER_PALDEA"}`; Route118 supplies `{"8":"BASCULIN_BLUE_STRIPED"}` for the functionally distinct Blue Basculin route.

In `scripts/emerald_champions_rebuild_wild_water.py:build_fishing`, build the existing ten slots first, then validate and apply only each specified species replacement before returning the table. Require integer indices 0–9, no duplicate normalized index, and a configured enabled species. Keep the existing min/max levels. Applying the override after ordinary generation leaves the internal `used` pool and every other slot unchanged.

This single small authoring extension serves both new fishing routes. Do not add separate Old Rod and Super Rod override systems, change shared encounter-rate arrays, invent another water authoring file, or reorder maps and thereby rotate unrelated generated rosters. Compare all 10slots and all unaffected maps with the snapshot after materializing. The complete final tables in the catalogue are the intended result.

### Method access and physical identity

The catalogue separates walking, Surf, Rock Smash, Old Rod, Good Rod and Super Rod. A route's grass access does not imply that its Surf or rod catches are already obtainable. Route115's northern grass and southern Seaspray access are separate physical cases. Riding past Route105 on Briney's boat does not prove access to its grass tiles. The ruined and underwater rooms require their actual path, puzzle and story prerequisites.

The existing timing helper contains broad map-prefix fallbacks and cannot serve as an exact progression oracle. The world volume owns physical routes; the Mega volume separately examines pickup access and usable evolution timing.

A suspected Shoal defect was rejected: high tide changes layouts while retaining the low-tide saved map IDs, so the existing low-tide water/fishing headers still serve the flooded rooms. Do not add redundant high-tide tables or shift generator ordering based on layout names alone.

The nine Altering Cave selector rows are conditional alternatives, not nine simultaneous populations. Keep their compatibility, but do not inflate ordinary coverage from their union. The dedicated expanded cave rooms remain separate real maps.

### GUIDE-01 — Advertise usable methods

`BufferCurrentMapRouteSignSpecies` currently appends Hidden entries even though `DEXNAV_ENABLED` isFALSE. The hidden tables also retain historical encounter-rate values20, whereas DexNav's detector switch interprets0/1 as terrain modes. Those rows are not ordinary encounter methods.

Final change: suppress the Hidden section when its actual encounter feature is disabled. Keep the ordinary route sign derived from active native tables, including the three rod groups, and retain the separate Route119 Feebas clue. Do not enable DexNav, repurpose its rate field or make a new detector merely to justify obsolete display rows.

Acceptance: a route with inactive Hidden data lists only its usable methods; route entries still match the final tables; no dormant-only species is used to close a roster gap. A future explicitly approved detector feature would need its own semantics and access review.

### WILD-ENGINE-01 — Respect failed Sweet Scent generation

Source evidence: normal land/water branches of `SweetScentWildEncounterInner` call `TryGenerateWildMon` and then unconditionally start a battle. `TryGenerateWildMon` can returnFALSE before creating a Pokémon, including when an ordinary Sign species was already caught or is unavailable. Pike/Pyramid branches already check its result.

Final change: on normal land, if there is no successful outbreak and `TryGenerateWildMon` returnsFALSE, returnFALSE before starting the battle. On normal water, similarly require successful generation before starting. Preserve existing roamer/outbreak handling, wrapper restoration of the Sweet Scent selection state, ordinary single-capture format, repel behavior and other generation guards. Do not add an unbounded reroll loop or silently bypass capture eligibility.

Acceptance: seed an old enemy party, force a caught Pheromosa draw, and prove that no battle starts and no stale species is exposed. Also test a valid ordinary draw, a valid uncaught Sign draw, successful outbreak, roamer, land/water paths and wrapper cleanup after failure. This is a source-backed control-flow repair; the book does not claim a reproduced live exploit.

### Probability and convenience

Keep the ordinary5%minimum per species per method, aggregating repeated slots. Check both normal and Sweet Scent reversed selection. Encounter frequency, conditional rejected Sign draws and capture success are separate quantities; a5%slot is not a5%chance on every walking step.

Specialness comes from location, species, story and strategy as well as probability. Preserve generous preparation and the cap-respecting Leveler. PREP-02 ensures a Pokémon already at its cap can still receive a ready normal evolution through that same convenient tool.

Implementation order is WILD-ENGINE-01/GUIDE-01, the exact slot manifest plus WATER-AUTHOR-01, acquisition/form dependencies, then local capture/path fixtures. The battle volumes remain the authored challenge baseline against which these new player options are exercised.

---

## 04 — Obtaining every functional Pokémon and form

The proposed game preserves its generous existing collection and makes every supported battle option practical in one save. This means a player can capture, prepare, use, evolve, transform and later try another supported option. It does not require simultaneously storing a living dex: retain the existing 14 boxes/420 ordinary slots and native fusion storage.

This volume accounts for every configured species/form entry. The appendix distinguishes ordinary acquisition, evolution, persistent forms, cosmetic choices, temporary battle states, Megas and explicitly inactive legacy/gimmick templates. A potential graph is an inventory aid, not proof: it cannot by itself establish reachable NPCs, valid breeding, evolution conditions, two-component fusion or enough copies of a nonbreedable branching Pokémon.

The current snapshot already fixes several earlier findings. Meltan evolves in New Mauville; Ursaring uses Peat Block at night without a Hisui requirement; Bisharp uses a held Leader's Crest; several Hisuian evolutions have Hoenn location conditions; Burmy and Deerling have roots; and Runerigus has a live carved-stone interaction on Route111. Preserve these improvements. Do not reimplement an obsolete gap.

The final specification below adds eight exact wild-form substitutions to the lead editor's 21 roster additions, three independent Galarian bird Signs, seven prepared gifts, an explicit cosmetic/costume service, missing form tools in the existing archive, and a postgame Deoxys service. It also closes precise evolution-readiness and Mega-base requirements without reducing existing preparation move access.

### FORM-01

**Disposition: KEEP.** Existing successful native behavior is preserved except for the exact changes specified here.

**Source and coverage contract.** The snapshot configures 1,579 entries including SPECIES_NONE: 1,578 actual species/forms spanning 1,025 National Dex identities. The full [species/form index](appendices/species-and-forms.md) and [machine ledger](review/acquisition-forms.json) identify each disposition and its immediate source or required transition. The ledger retains complete root records and condition-bearing evolution/form steps.

Do not use a shared National Dex number, a form-enumeration table or a reversed evolution arrow as acquisition proof. The proposed closure uses actual active form-change tables, explicit fusion conjunctions, native breeding exceptions and a resource ledger for unique branching Pokémon. It deliberately excludes inactive G-Max, Ultra Burst, Stellar/Tera-only states, legacy Totems and promotional templates without an additional supported competitive capability. Normal Terapagos→Terastal through Tera Shift remains usable; its name does not make that automatic ability transformation a banned selectable gimmick.

The Appendix's access paths are source-backed implementation specifications. They are not claims that every path was newly traversed in an emulator. WILD-ENGINE-01 must ensure failed/gated ordinary encounter construction, including Sweet Scent, does not launch a stale opposing party. Actual final capture formats follow the lead editor's global format decision; this volume does not quietly decide the remaining wild-format question.

**Acceptance:** reconcile the configured species set dynamically, not against a historical species-count quota. Every advertised functional entry needs one documented path whose actual prerequisites can be met in the same save. Verify each new interaction and representative native transition classes; do not manufacture thousands of redundant tests that merely echo rows.

### FORM-02

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**Extend the existing free evolution archive with 14 explicit form tools.** Its current 45 evolution items remain. Preserve free acquisition and existing archive access; adding a tool does not bypass the species' own acquisition requirement.

| Item | Functional access |
|---|---|
| ITEM_ROTOM_CATALOG | Rotom's five appliances and return to ordinary Rotom |
| ITEM_GRACIDEA | Shaymin Sky under native day/status conditions |
| ITEM_PRISON_BOTTLE | Hoopa Unbound, with native return/reapplication behavior |
| ITEM_RED_NECTAR | Oricorio Baile |
| ITEM_YELLOW_NECTAR | Oricorio Pom-Pom |
| ITEM_PINK_NECTAR | Oricorio Pa'u |
| ITEM_PURPLE_NECTAR | Oricorio Sensu |
| ITEM_ADAMANT_CRYSTAL | Origin Dialga while equipped |
| ITEM_LUSTROUS_GLOBE | Origin Palkia while equipped |
| ITEM_GRISEOUS_CORE | Origin Giratina under the configured modern item rule |
| ITEM_DOUSE_DRIVE | Douse Genesect/Water Techno Blast |
| ITEM_SHOCK_DRIVE | Shock Genesect/Electric Techno Blast |
| ITEM_BURN_DRIVE | Burn Genesect/Fire Techno Blast |
| ITEM_CHILL_DRIVE | Chill Genesect/Ice Techno Blast |

The existing generator [scripts/generate_emerald_champions_mega_archive.py:29](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/scripts/generate_emerald_champions_mega_archive.py:29>) derives the evolution-item list. Add a small explicit supplemental form-tool set and union it with the current derived evolution items; keep the generated output as the archive's single authoritative input. Do not edit only generated output or add separate shops for individual tools. The old generator's broad name and historical count floors do not define the design; revise checks to validate actual required entries and protected exclusions.

Retain the separation between ordinary preparation and earned progression items. Do not add Mega Stones, the Mega Ring, Red/Blue Orbs, Rusted Sword/Shield, Ogerpon masks, Arceus plates, fusion tools or Meteorite to this new free-tool set. The native relic system already uses 24 item-pending bits and six earned-group bits in its 32-bit state: these 14 archive additions need no relic bits and must not overflow that ledger.

Silvally's17 Memories are already in the free Species held-item category ([src/field_specials.c:394](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/field_specials.c:394>)); preserve that route. Their protected-preset classification prevents silent replacement and does not mean they are unavailable. Birch already gives DNA Splicers, Zygarde Cube, N-Solarizer, N-Lunarizer and Reins of Unity through his postgame research reward. Reveal Glass already belongs to the Weather Institute. Preserve those world rewards.

**Acceptance:** obtain and use each added tool through its native party menu; cancellation preserves items/Pokémon, full Bag does not consume a one-time entitlement, and finite/earned relics remain excluded. Verify Rotom's actual appliance ability/move changes, all four Oricorio types, all four Genesect drives and exact modern Origin items. A pinned-source move comparison is advisory, never permission to remove existing expanded moves.

### FORM-03

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**A prepared female Indeedee from Wally's aunt.** The current Verdanturf Meadow table provides the male form. [src/daycare.c:913](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/daycare.c:913>) chooses a fixed egg species, with specific exceptions for Nidoran, Volbeat, Manaphy and certain forms; it has no male-Indeedee→female-species branch. `SetInitialEggData` preserves that species, and male Indeedee is male-only. A random personality does not repair the missing female form.

Use the existing aunt object at (2,4), elevation 3, in `VerdanturfTown_WandasHouse`. It currently routes through `VerdanturfTown_WandasHouse_EventScript_MegaGift_GARDEVOIRITE` (`scripts.inc:170`). Preserve that wrapper, its successful Bag receipt check and its flag. A full Bag must retain the current failure path; Indeedee acceptance must never gate Gardevoirite.

After the aunt's appropriate existing story text, offer: “This Indeedee loves looking after a partner. Would you like her to join your team?” Convert only the endings of the aunt's mutually exclusive text branches into one shared gift continuation. Do not call an end-terminated script expecting it to return. Declining leaves the offer available; successful party or PC delivery sets FLAG_EC_GIFT_INDEEDEE_F. No room/visibility state changes are needed.

| Species | Level | Item | Ability | Nature | Points HP/Atk/Def/SpA/SpD/Spe | Moves |
|---|---:|---|---|---|---|---|
| INDEEDEE_F | min(current cap,25) | PSYCHIC_SEED | PSYCHIC_SURGE | CALM | 32/0/2/0/32/0 | FOLLOW_ME / HELPING_HAND / PSYCHIC / PROTECT |

This is the existing female Indeedee default preparation build ([src/data/pokemon/emerald_champions_battle_sets.h:12764](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/data/pokemon/emerald_champions_battle_sets.h:12764>)), with a valid ability and useful immediate doubles role. Other builds remain freely selectable.

**Acceptance:** the received species is actually INDEEDEE_F with its female stat/ability data; the existing stone and story dialogue remain available in every branch. Full party/full PC failure leaves the gift flag unset. Decline, later receipt, reload and repeat interaction cannot duplicate or lose the entitlement.

### FORM-04

**Disposition: REVISE.** Existing successful native behavior is preserved except for the exact changes specified here.

**Eight precise wild-form additions.** These preserve existing slot weights and levels. They supplement the lead editor's 21 changes; they do not replace that global plan.

| ID | Map/method | Native zero-based slot | Replace | Final species | Chance | Levels |
|---|---|---:|---|---|---:|---|
| FORM-WILD-01 | MtPyre_2F land | 9 | SINISTEA (Phony) | SINISTEA_ANTIQUE | 5% | 24 |
| FORM-WILD-02 | MtPyre_5F land | 9 | POLTCHAGEIST (Counterfeit) | POLTCHAGEIST_ARTISAN | 5% | 24 |
| FORM-WILD-03 | Underwater_SeafloorCavern land | 5 | KINGDRA | TATSUGIRI_DROOPY | 8% | 40–44 |
| FORM-WILD-04 | Underwater_SeafloorCavern land | 8 | WISHIWASHI | TATSUGIRI_STRETCHY | 6% | 40–44 |
| FORM-WILD-05 | Route118 Super Rod | 8 | BASCULIN (Red) | BASCULIN_BLUE_STRIPED | 15% | 55–60 |
| FORM-WILD-06 | SafariZone_Northwest land | 11 | DITTO | SQUAWKABILLY_YELLOW | 5% | 29 |
| FORM-WILD-07 | Route121 land | 5 | KOMALA | MAUSHOLD_THREE | 8% | 26 |
| FORM-WILD-08 | DesertUnderpass land | 11 | ESPATHRA | DUDUNSPARCE_THREE_SEGMENT | 5% | 45 |

The antique/artisan tea discoveries belong on different Mt. Pyre floors. Their ordinary counterparts remain on other floors, and the free archive already provides Chipped Pot and Masterpiece Teacup. Breeding deliberately returns Phony/Counterfeit offspring, so it is not an alternate route to the special tea forms.

At the submerged submarine approach, Curly Tatsugiri and Dondozo keep their existing 10% slots. Droopy/Stretchy add genuinely different Commander/Order Up options; they are not cosmetic conversions. Kingdra and Wishiwashi retain numerous other aquatic sources.

Route118 keeps Red Basculin at 18% Surf while Blue becomes a fishing discovery. Blue's Rock Head differs from Red's Reckless, so a generic color menu would bypass a real ability distinction. Yellow Squawkabilly supplies the Sheer Force group; Green from the lead editor's Route118 change supplies the Guts group. Ditto remains readily available on Route117. Styling is permitted only within Green↔Blue and Yellow↔White groups.

Keep the native rare-PID Dudunsparce/Maushold evolutions as optional surprises, but do not make players repeat approximately 100 acquisitions to access those forms. The direct rare-form discoveries above give a practical alternative. Their different weights prevent declaring them mechanically interchangeable cosmetics. Komala remains on Route118; Desert Underpass still has Flittle at 10%, which evolves into Espathra. All four Pumpkaboo sizes already occur on Route120 at 10/6/5/5%; preserve them and their corresponding Gourgeist evolutions.

For fishing, use the lead editor's one optional `fishing_species_overrides` map keyed by native slots0–9, applied after existing ten-slot generation: Route117 `{"0":"WOOPER_PALDEA"}` and Route118 `{"8":"BASCULIN_BLUE_STRIPED"}`. Do not add separate Old Rod and Super Rod override systems or perturb unrelated generated slots/used-pool behavior.

**Acceptance:** source/native slot agreement, no overlapping contradictory proposals, every displaced family retains a practical source, and ordinary probabilities remain at least5%. Confirm exact captured forms survive wild-set preparation and saving. For tea forms, evolve the intended authenticity with the correct item; for Tatsugiri, verify each native Order Up effect rather than only the picture.

### FORM-05

**Disposition: REVISE.** Existing successful native behavior is preserved except for the exact changes specified here.

**A strictly limited cosmetic stylist.** The existing Lilycove Contest Lobby Girl at (4,9), script `LilycoveCity_ContestLobby_EventScript_Girl` (`scripts.inc:458`), has a single ordinary conversation and no reward gate. Preserve that text, then offer optional “Appearance” and the separate “Cosplay partner” service from FORM-09. Use one controlled lock/faceplayer interaction; do not continue menus after an auto-releasing MSGBOX_NPC without reacquiring control.

The exact whitelisted groups are reproduced in the appendix and machine ledger: Furfrou's10 trims; the two Shellos; the two Gastrodon; 20 Scatterbug patterns, 20 Spewpa patterns and 20 Vivillon patterns; four Deerling and four Sawsbuck seasons; the five ordinary Flabébé, five ordinary Floette and five Florges colors; seven Minior meteor colors; 63 ordinary Alcremie combinations; Magearna/Original paint; Zarude/Dada scarf; and the two separate Squawkabilly pairs. There are 17 explicit groups and 175 entries in this snapshot; these counts are descriptive, not future design quotas.

Current source comparison confirms equal base stats, types, ability slots, weight, gender rules, egg groups, native level/TM learnset pointers and equivalent evolution conditions within each group. Evolution targets may differ only by the corresponding cosmetic child form. Current expanded preparation access remains intact: if final producer review finds differing current tutor additions within a purely cosmetic group, preserve their union rather than silently removing moves. Pinned learnsets do not override authored expanded access.

The service must require both source and destination to belong to the **same** explicit group. It cannot turn one whitelisted family into another. Reject Eggs, bad Eggs, unowned/cancelled selections and all unlisted targets. Do not cross evolution stages or convert regional forms, genders, tea authenticity, Tatsugiri styles, Pumpkaboo sizes, rare mouse/segment forms, Floette Eternal, legendary power forms, Megas, fusions or inactive gimmicks.

Change only the exact species/form ID and native form-duration metadata where applicable. Preserve PID, shiny status, gender, OT, nickname, level/EXP, Nature, Points, ability slot, held item, friendship, current HP/status, moves, PP and PP Ups. Recalculate stats with the native helper and verify the equal-property conversion has not healed or altered combat stats. Furfrou retains its native five-day/withdrawal return behavior; initialize its native timer with `TrySetDayLimitToFormChange` ([src/pokemon.c:6144](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/pokemon.c:6144>)) and explain that a fresh trim is free. Minior styling changes meteor color only; the matching core appears through Shields Down in battle.

Magearna and Zarude styling requires already owning that same legendary/mythical Pokémon. It supplies no Pokémon, item, caught flag or power form. Unown's 28 appearances already derive from personality through native sprite/render paths ([src/pokemon.c:1831](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/pokemon.c:1831>)); do not rewrite PID to offer a letter menu or mislabel the 27 rendering entries as missing species.

**Acceptance:** one success and one rejected cross-group case for every distinct group mechanism; preserve identity/HP/moves/items exactly. Verify Furfrou timing and Minior's matching core. These checks enforce the specific cosmetic-service promise; they must be updated if a future intentional design changes a group's battle properties.

### FORM-06

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**Independent Galarian bird discoveries.** Append these IDs immediately before LEGENDARY_SIGN_COUNT; do not renumber existing entries.

| Numeric ID | Symbol | Species | Existing landmark map | Badge/flag gate | Offset |
|---:|---|---|---|---|---:|
| 82 | LEGENDARY_SIGN_ARTICUNO_GALAR | ARTICUNO_GALAR | ROUTE120 | 6 / FLAG_BADGE06_GET | +2 |
| 83 | LEGENDARY_SIGN_ZAPDOS_GALAR | ZAPDOS_GALAR | ROUTE112 | 3 / FLAG_BADGE03_GET | +2 |
| 84 | LEGENDARY_SIGN_MOLTRES_GALAR | MOLTRES_GALAR | MT_PYRE_EXTERIOR | 6 / FLAG_BADGE06_GET | +2 |

Use LANDMARK_SIGN definitions with required species NONE and the existing deliberate local landmark menu. No new physical bird object, mandatory Devon research task or permanent failed-encounter flag is required. The Psychic bird belongs among Route120's mysterious highlands, the Fighting runner on Route112's rough foothills, and the Dark bird beside Mt. Pyre. Keep original Articuno, Zapdos and Moltres acquisitions independently available.

This needs an exact identity repair. `GetLegendarySignIdBySpecies` currently strips forms before lookup ([src/legendary_signs.c:303](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/legendary_signs.c:303>)), and `MarkLegendarySignCaughtBySpecies` strips them again (`:575`). First sanitize and match the exact species against the registry. Only if no exact registered encounter exists may an approved interchangeable battle/persistent form fall back to its base Sign. Marking capture must preserve the incoming exact form until that lookup. A Galarian bird must never mark the original bird's caught state or vice versa.

The appended IDs use unlocked/caught word 5 bits 2/3/4; count 85 remains within the existing96-bit capacity. No relic-pending bits are added. Preserve prior caught/unlocked states and previously earned Arceus rewards. New saves' mastery checks can include the newly appended Signs through the existing count-based loop; do not revoke an already received reward on an older save.

Devon's optional guide must show three distinct names/locations/statuses. Add an exact-species display-name helper returning Galarian Articuno, Galarian Zapdos and Galarian Moltres, and use it in both menu entries and lead text; the ordinary GetSpeciesName strings alone are identical to the original birds. Preserve dynamic allocation by actual display length and verify the native menu's text width. Original Moltres is a native encounter whose guide currently relies on the shared National Dex caught bit; use its actual FLAG_EC_CAUGHT_MOLTRES to distinguish it from Galarian Moltres. Keep resting/revisit retry and permanent successful capture behavior. Do not infer regional-form capture from a species-wide Pokédex bit during migration.

**Acceptance:** capture original then regional, and regional then original, for each bird; each combination remains independently obtainable and correctly marked after reload. Old saves with original birds caught start the new regional bits unset. Failed encounters rest only for the visit; revisiting retries. Verify IDs 82–84 at the actual word/bit boundary and guide/mastery behavior without adding a historical fixed-count gate.

### FORM-07

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**Enough nonbreedable parents for both irreversible branches.** A single Cosmog cannot produce both Solgaleo and Lunala, and a single Kubfu cannot produce both Urshifu styles. A graph that marks both branches reachable independently misses the one-save resource constraint.

Offer one supplemental Cosmog and one supplemental Kubfu through a shared Birch/Aide ResearchPartners menu after their respective original Sign is caught. Each uses its own successful-receipt flag, not another Sign or a reset of the original caught bit. The generic prepared-gift path must not call the ordinary already-caught Sign rejector. Cosmog's original source is already postgame; Kubfu's supplemental partner is available after the original badge-three acquisition.

Birch can leave the lab during his daily routine. The existing Lab Aide must call the **same** entitlement handler, with the same receipt flags. Preserve the opening volume's priorities: legacy state5 second-starter completion, then state2 early supplies, then optional ResearchPartners, then ordinary dialogue. Do not add Birch visibility flags, change his daily schedule or make the player wait for a particular day.

| Species | Flag | Level | Item | Ability | Nature | Points | Moves |
|---|---|---:|---|---|---|---|---|
| KUBFU | FLAG_EC_GIFT_SECOND_KUBFU | min(cap,25) | EVIOLITE | INNER_FOCUS | JOLLY | 2/32/0/0/0/32 | CLOSE_COMBAT / ICE_PUNCH / U_TURN / PROTECT |
| COSMOG | FLAG_EC_GIFT_SECOND_COSMOG | min(cap,25) | EVIOLITE | UNAWARE | ADAMANT | 32/32/2/0/0/0 | DOUBLE_EDGE / TELEPORT / MIMIC / NONE |

Kubfu's build exists in the current default catalog (`emerald_champions_battle_sets.h:10684`). Cosmog retains the current catalog's expanded usable Double-Edge/Teleport/Mimic access while omitting empty Splash and investing for a durable slow role. This is not a reason to remove its expanded moves from the game.

Explain that Dark/Water Scrolls produce the two Urshifu styles, and that Cosmoem evolves by day into Solgaleo or by night into Lunala. The two source copies permit both choices in one save. PREP-02 handles ready ordinary evolution at the cap, including 100, without forcing a level above the cap. Do not add a generic legendary conversion/devolution menu.

**Acceptance:** each original caught prerequisite is required; each successful supplemental delivery occurs once from either NPC; full party/PC leaves the entitlement available. Demonstrate both Urshifu styles and both celestial branches in one save, then their respective fusion choices. Preserve original Sign and reward state throughout.

### FORM-08

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**Keep native form and fusion rules, with working tools and clear requirements.** The final routes are:

| Family | Required pieces and route |
|---|---|
| Kyurem White / Black | Owned Kyurem + Reshiram or Zekrom + DNA Splicers; unfuse before selecting the other branch |
| Necrozma Dusk Mane / Dawn Wings | Owned Necrozma + Solgaleo or Lunala + N-Solarizer or N-Lunarizer |
| Calyrex Ice / Shadow | Owned Calyrex + Glastrier or Spectrier + Reins of Unity |
| Zygarde10% / 50%, Aura Break / Power Construct | Owned Zygarde + Birch's Cube; use the actual four-way native transitions |
| Zygarde Complete | A Power Construct form reaches the native at-most-half-HP end-turn condition while its ability functions |
| Arceus types | Owned Arceus + the appropriate earned plate; preserve current relic grant/retry state |
| Silvally types | Evolve owned Type: Null with friendship and use the existing free Species-category Memories |
| Crowned Zacian / Zamazenta | Owned Pokémon + its earned Rusted Sword/Shield; native battle-form handling |
| Ogerpon masks | Owned Ogerpon + its earned mask; preserve the ordinary four forms, not banned Tera forms |
| Castform / Cherrim / Darmanitan / Wishiwashi / Aegislash / Minior / Mimikyu / Eiscue / Cramorant / Palafin / Morpeko | Their native ability, weather, HP, move or switching state; no direct gift of a transient battle form |
| Keldeo Resolute | Learn Secret Sword; existing preparation explicitly runs move-driven form normalization |
| Meloetta Pirouette | Relic Song's native battle transition; retain its normal return behavior |
| Shaymin Sky / Hoopa Unbound | FORM-02 tools with native day/status, duration and return conditions |
| Terapagos Terastal | Native Tera Shift on entering battle; no selectable Terastallization is required |

The fusion definitions are in [src/data/pokemon/form_change_tables.h:1189](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/data/pokemon/form_change_tables.h:1189>), `:1658` and `:2042`; party interaction begins in [src/party_menu.c:6472](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/party_menu.c:6472>). A fusion needs both actual owned components, not just their Dex flags. Native unfusion requires a free party slot (`party_menu.c:6847`); communicate this before attempting it and preserve the component when space is unavailable. Sequential use needs no storage-layout expansion.

Phione specifically requires breeding an owned Manaphy with Ditto at the Day Care, then hatching the resulting Phione egg; its Sign entry is bookkeeping, not a standalone gift. Circuit, Game Corner and mastery entries similarly require their actual delivery pathways documented in the Mega/legendary reward volume. An OTHER_SIGN definition alone does not place a Pokémon in the world.

The free Move Tutor remains the convenient route to condition moves such as Ancient Power, Mimic, Psyshield Bash, Rage Fist, Hyper Drill, Twin Beam, Dragon Cheer and Secret Sword. Preserve the game's existing expanded preparation access. The current source/pinned comparison is useful for understanding differences, not a mandate to delete hundreds of authored move slots or narrow creativity.

**FORM-08-UNFUSE-MOVES:** preserve the current expanded move-access contract during unfusion. [src/party_menu.c:6623](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/party_menu.c:6623>) currently filters only native level/TM/egg lists, which can disagree with intentionally expanded preparation access; it also deletes from a shifting move array during a forward scan. Build the proposed resulting four slots from an immutable copy, using the same effective current move-access policy as preparation. Preserve every move allowed by that policy. Preview and confirm only genuinely form-specific/unsupported changes before committing; do not erase a move merely because a pinned reference omits it. Apply the resulting list once, preserving unrelated PP/PP Ups and mandatory field capability. Keep native explicit fusion signature swaps when applicable. Cancellation preserves both Pokémon and their moves. A focused case with two adjacent candidates for removal must not skip one through slot shifting.

**Acceptance:** actual fusion/unfusion preserves both identities, held items and storage state through cancellation/reload/full-party failure; no component is cloned or lost. Verify form changes use the selected ability/item and restore correctly. Native inactive gimmicks remain unavailable; do not count an enum-table entry as a functional player mode.

### FORM-09

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**Functional research partners and a separate Cosplay mechanic.** After game clear, the shared Birch/Aide ResearchPartners menu offers three distinct non-evolving/ability variants. Give each once with successful-receipt state. These are existing configured battle options, not ordinary cosmetics. Numeric receipt-flag assignments and one-time v4 initialization are owned by the [state allocation specification](appendices/state-allocation.md); do not allocate competing bits here.

| Species | Flag | Level | Item | Ability | Nature | Points | Moves |
|---|---|---:|---|---|---|---|---|
| GRENINJA_BATTLE_BOND | FLAG_EC_GIFT_BATTLE_BOND_GRENINJA | min(cap,25) | LIFE_ORB | BATTLE_BOND | TIMID | 2/0/0/32/0/32 | HYDRO_PUMP / DARK_PULSE / WATER_SHURIKEN / PROTECT |
| PIKACHU_STARTER | FLAG_EC_GIFT_PARTNER_PIKACHU | min(cap,25) | LIGHT_BALL | LIGHTNING_ROD | TIMID | 2/0/0/32/0/32 | FAKE_OUT / THUNDERBOLT / GRASS_KNOT / PROTECT |
| EEVEE_STARTER | FLAG_EC_GIFT_PARTNER_EEVEE | min(cap,25) | SITRUS_BERRY | ADAPTABILITY | ADAMANT | 32/32/2/0/0/0 | DOUBLE_EDGE / YAWN / HELPING_HAND / PROTECT |

Battle Bond Greninja has an existing valid default preset (`emerald_champions_battle_sets.h:11804`). The configured Gen9+ Battle Bond boosts Attack, Special Attack and Speed once rather than becoming Ash-Greninja ([include/config/battle.h:223](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/include/config/battle.h:223>), [src/battle_util.c:4600](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/battle_util.c:4600>)); mark Ash form as inactive legacy data. The enhanced Pikachu/Eevee have distinct higher base stats and cannot evolve. Explain that tradeoff. Do not reuse ordinary Eevee's Eviolite support preset for its non-evolving partner variant. Light Ball checks the Pikachu base family (`battle_util.c:7104`) and applies to the enhanced partner.

At the Contest Lobby Girl, offer one prepared female PIKACHU_COSPLAY with FLAG_EC_GIFT_COSPLAY_PIKACHU: level min(cap,25), Light Ball, Lightning Rod, Timid,2/0/0/32/0/32, Thunderbolt/Grass Knot/Knock Off/Protect. This is the current Cosplay default (`emerald_champions_battle_sets.h:11154`). No contest victory or mandatory contest participation gate is needed.

Her distinct “Costume lessons” submenu accepts only an owned member of this exact six-form family:

| Costume species | Costume signature move |
|---|---|
| PIKACHU_COSPLAY | None |
| PIKACHU_ROCK_STAR | METEOR_MASH |
| PIKACHU_BELLE | ICICLE_CRASH |
| PIKACHU_POP_STAR | DRAINING_KISS |
| PIKACHU_PHD | ELECTRIC_TERRAIN |
| PIKACHU_LIBRE | FLYING_PRESS |

These signature additions are explicitly recorded in `data/emerald_champions/emerald_champions_preparation_form_learnsets.json`. They make costume changes mechanically meaningful, so do not send them through FORM-05's cosmetic-only action. Never convert ordinary, enhanced-partner or cap Pikachu into this family.

Preview the requested costume and exact move exchange. If the outgoing costume's signature is present, offer that slot as the replacement; otherwise use the existing move-learning slot-selection interface. Confirm before replacing anything. Cancel leaves species and moves unchanged. Returning to plain Cosplay explicitly confirms removal of the outgoing signature, if present; it may leave three moves. Never erase unrelated moves, silently remove a necessary field capability, reset the rest of the build, or scan/delete every unusual move merely because it is absent from a pinned reference. Preserve all other identity, stats, ability/item and move/PP state. Only the confirmed new move receives its normal learning behavior.

Pikachu caps and Spiky-eared Pichu remain explicitly excluded legacy promotional appearances: they add no distinct positive competitive stat/ability capability, while their gender/breeding/evolution restrictions make unrestricted styling misleading. This is not an exclusion of Battle Bond, enhanced partners or Cosplay signatures.

**Acceptance:** each seven-gift entitlement across FORM-03/07/09 is independent and successful-receipt-only. Confirm valid abilities and complete ready builds; new gifts remain their exact variant through preparation/storage. Cosplay cancellation and failed slot choice preserve the entire Pokémon; each of five signature exchanges has a positive/negative case and no unrelated move loss. Verify modern Battle Bond as a stat boost, not an Ash transformation.

### FORM-10

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**Postgame Deoxys research at Cozmo's house.** Deoxys's four native forms cycle through Meteorite use (`form_change_tables.h:780`). Meteorite is also a story handoff item, so it must not appear in the free archive or be reissued early just to supply form access.

Append an optional Deoxys research continuation to Professor Cozmo after his existing applicable dialogue/handoff branch, gated by FLAG_SYS_GAME_CLEAR. Preserve Dawn Stone receipt, Meteorite handoff, Bag failure and refusal behavior. The lab can lend its research equipment for an owned Deoxys; no Meteorite is granted, consumed or removed by this service.

Offer Normal, Attack, Defense, Speed and Cancel for an owned non-Egg Deoxys. Use exactly those four approved target species and the native form/stat/move-change semantics, preserving PID/shiny/OT/level/Points/Nature/item and unrelated moves. Do not manipulate the story item to simulate a menu click. A fainted owned Deoxys may be studied but remains at zero HP; this service does not heal. The same explicit form-update primitive can be reused by the bounded cosmetic service only with its separate whitelist and stricter equal-property rules.

**Acceptance:** all four forms can be selected repeatedly after game clear, before or after the old Meteorite handoff, with no duplicate Dawn Stone or changed quest flags. Cancellation, fainted selection policy and stat/HP recalculation follow the stated native rules; no early game access is introduced.

### FORM-11

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**Evolution requirements remain real, visible and practical.** Preserve these current conveniences:

- Meltan: ready level evolution in the New Mauville map section.
- Ursaluna: Peat Block at night; Bloodmoon is a separate direct encounter from the global wild plan.
- Kingambit: ready level evolution while holding Leader's Crest, already in the free archive.
- Gholdengo: level 45; do not restore a999-coin grind.
- Palafin: Finizen level 38; no link-session requirement.
- Alcremie: Milcery level 30; use FORM-05 for ordinary cosmetic combinations rather than inventing spinning/rare-sweet grind.
- Trade evolutions: preserve existing local Linking Cord/item/party-partner alternatives. A native trade row alone is not proof of solo access.
- Babies: preserve current no-incense breeding and short hatch-cycle behavior. Regional offspring may need an Everstone on the relevant regional parent; native overrides make tea offspring ordinary and Scatterbug offspring Fancy.

The current Hoenn regional-evolution locations are deliberate: Quilava/Rufflet in Mt. Pyre; Dewott in Shoal Cave; Dartrix in Petalburg Woods; Petilil in Verdanturf Meadow; Goomy in Meteor Falls1F1R; Bergmite in Shoal's ice room; Koffing in Fiery Path. The exact levels/conditions remain those printed in the appendix. Alolan Raichu/Exeggutor/Marowak and Galarian Mr. Mime already have direct Hoenn roots, so do not pretend unavailable Alola/Galar region predicates are usable local evolution methods.

Runerigus uses the live Route111 carved-stone trigger with a conscious Galarian Yamask missing at least 49 HP and no Everstone. Sirfetch'd uses three critical hits in one qualifying battle. Wyrdeer/Annihilape use their actual 20-use counters; Basculegion uses 294 accumulated recoil and gender; Pawmo/Bramblin/Rellor need the first live party slot and 1,000 follower steps. Publish these exact requirements through appropriate evolution guidance. Do not claim that merely possessing the family completes its evolution chain.

**FORM-11-NATURE:** make Toxel's form selection follow the Nature chosen in preparation. The free editor writes MON_DATA_HIDDEN_NATURE (`field_specials.c:5551`), but evolution currently reads `GetNature(mon)`, which is PID modulo25 (`pokemon.c:4068`,`:4101`). Use MON_DATA_HIDDEN_NATURE as the input to the evolution nature predicates. Preserve PID, shiny status, gender and all other evolution conditions. This makes Amped/Low Key an understandable preparation choice without breeding another Toxel.

**PREP-06 dependency:** optional Bonding preparation is available through the shared Center Specialist from Oldale and through the existing Verdanturf Friendship Rater. It offers 0, the configured evolution threshold, or 255 friendship with preview/confirmation; preserve PID and do not auto-evolve or alter battle-affection configuration. This keeps both evolution readiness and retained Return/Frustration roles convenient without menu-grind teaching loops. The lead editor owns the one shared implementation.

**PREP-02 / PREP-02-B dependencies:** the Leveler offers already-ready ordinary evolutions at the current cap, including 100, without increasing level. Tandemaus remains battle-only: after a legitimate won/caught ordinary battle, the existing end-of-battle evolution callback evaluates ready EVO_LEVEL_BATTLE_ONLY entries without requiring a level gain. Preserve level 25, PID branch, Everstone, consciousness and cancellation. Do not evolve an underleveled Pokémon permanently from temporary Circuit normalization, or add duplicate prompts if a current callback already handles the case. The lead editor owns that exact callback repair.

Retain PID surprise evolution rules for Wurmple, Dudunsparce and Maushold. The two rarer forms receive practical direct discoveries in FORM-04; the stylist never rewrites PID or treats a weight difference as irrelevant. Gender-dependent evolution remains meaningful; the existing broad wild roster and breeding routes supply the necessary genders without a generic gender-changing service.

**Acceptance:** confirm one real example for each distinct requirement class, with negative controls that actually violate the condition. Check equality/threshold boundaries using native integer Points/Nature stats, actual time/map/weather and persistent counters. Tests should validate these intended behaviors and be revised or retired when their contract becomes obsolete; do not freeze historical encounter counts, level-stage aesthetics or strategy quotas.

### FORM-12

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**Exact Mega bases, not family-wide assumptions.** The Mega/reward volume owns the99 stones and their locations. The configured source has 103 item-triggered Mega forms plus move-triggered Rayquaza. Form access requires the actual eligible base, its stone/move and the Mega Ring under native rules.

Important exact prerequisites:

- Floettite: Eternal Flower Floette only, already obtainable in Verdanturf Meadow. Ordinary colors are not substitutes. Repair Colton's final team to use FLOETTE_ETERNAL while preserving its other fields.
- Zygardite: Complete Zygarde only. The Cube supplies a Power Construct form; the ability must be active at its half-HP trigger, then a subsequent eligible turn can Mega Evolve. Repair Quincy's Zygarde to ZYGARDE_50_POWER_CONSTRUCT/POWER_CONSTRUCT; Neutralizing Gas must no longer suppress that ability when transformation is needed.
- Raichunite X/Y: ordinary Kantonian Raichu; Slowbronite: ordinary Slowbro; Greninjite: ordinary Greninja, not the separate Battle Bond species.
- Magearnite: both ordinary/Original Magearna; Meowsticite: both sexes; Tatsugirinite: all three native Tatsugiri forms.
- Dragon Ascent enables Rayquaza's stone-free Mega eligibility, which still requires the Ring and shares its trainer's actual activation budget.

The Mega chapter owns item-description clarity under MEGA-FORM-LABELS; do not duplicate those edits here. The shared AI chapter owns real base/form evaluation and owner-scoped Mega decisions. Do not downgrade Steven's Dragon Ascent to satisfy a stone-count assumption: both Rayquaza and Metagross can be eligible while only one actually activates.

**Acceptance:** for each distinct eligibility family, demonstrate an eligible base and a closely related ineligible form. Do not apply a universal “same National Dex” rule or count the stone alone as proof of the transformation. Reconcile the six final late-battle loadout revisions with the battle volume, including Quincy and Colton.

### Implementation and evidence boundary

Implement the root wild plan and these eight substitutions together; add exact Sign identity before the Galarian rows; extend the existing archive; add the shared receipt-safe gifts and bounded services; then integrate preparation readiness and exact Mega eligibility. The appendix accounts for every configured entry and records conditions rather than claiming all potential edges have been played.

Validate the real new routes and transitions, including old saves, full Bag/party/PC, cancellation, counterpart ownership, fusion recovery and failed encounter construction. Preserve the successful authored game while correcting concrete gaps. Pinned-source comparisons, existing tests and verifiers are code/evidence to audit; they are not a mandate to remove creative moves, impose old counts or rewrite working teams.

---

## 05 — Mega Stones and legendary rewards

The current world reward architecture is worth preserving. The book keeps every existing Mega Stone position, the meaningful NPC gifts, the three Berry Master exchanges and the Ring’s acquisition. The repairs concern exact receipt behavior, accurate information and actual form availability—not a new policy rationing strong Pokémon by chapter.

### The complete native roster

The frozen source contains **99 Mega Stone items with 103 item-triggered native Mega bindings**. The extra bindings come from male/female Meowstic, ordinary/Original Magearna and the three functional Tatsugiri forms. Rayquaza supplies the additional stone-free Mega binding, bringing the compiled `.isMegaEvolution` total to 104. This is a snapshot inventory, not a permanent game gate or a claim about an external competitive service’s current roster.

[The complete per-stone catalogue](appendices/mega-stone-catalog.md) individually specifies all 99 items, native base/Mega forms, source locations, flags, NPC roots, world context, evolution/acquisition dependencies, final disposition and acceptance. [Its JSON companion](review/mega-rewards.json) keeps all raw source routes and complete ancestry/root data.

The current routes reconcile to 73 physical sparkles,41 literal scripted grant sites and 3 berry exchanges. First/retry grant sites are not separate prizes: after grouping their real receipts, these are109 distinct finite world entitlements. Ten stones have two deliberate world sources; the other89 have one. The conditional original-starter gifts are additional pathways, fulfilled by the same inventory/held ownership and receipt rules described below.

No physical Mega receipt aliases another physical or hidden-item reward numerically in this snapshot, and no different Mega Stones share a numeric receipt accidentally. All 73 sparkles have a matching fixed-layout elevation and at least one passable adjacent tile. Those useful structural results **do not establish each stone’s complete earliest approach**: routes contain ledges, currents, doors, boulders, Cut branches and dynamic layouts that must be followed in the actual story state.

### Ring and stone eligibility

Keep Steven in Granite Cave as the Ring owner. Deliver Mr.Stone’s letter and earn Brawly’s Knuckle Badge; successful Ring delivery then opens the existing evolution-item archive. The Ring enables Mega use but never dispenses the entire Mega Stone collection. Mr.Stone’s old free-Mega-archive claim is corrected under W-MEGA-GUIDE.

Production `CanMegaEvolve` checks **Mega Ring possession in the Bag**, the exact native form table, held stone, existing gimmick usage and other battle eligibility conditions. A related Pokémon family is insufficient. Importantly, the Ring check is explicitly bypassed under `TESTING`; a test-build Mega battle cannot prove that production gate. Verify it through an appropriate production path when changing it or when no valid evidence exists.

Rayquaza uses its native Dragon Ascent Mega condition rather than a stone, while still following production Ring/battle eligibility. Primal Orbs, fusion tools, masks, memories and similar form items belong to the form/relic systems. Do not count them as missing Mega Stones or create a second archive for them.

### Two original starters, one existing receipt ledger

Apply **INTRO-STONES** exactly as specified in [the opening chapter](chapters/01-opening-and-starters.md) and `review/opening.json`. The immutable first starter uses receipt slots0/1; the actually delivered second starter uses slots2/3 in the existing16-bit `VAR_STEVEN_STARTER_STONE_DELIVERY` at 0x40E0. Skip `ITEM_NONE` slots. Four slots are reserved; with the present same-region pair rule and stone table, at most three are populated at once (Charizard X/Y plus the other chosen Kanto starter).

The current table recognizes 12 starter families and 13 stones: Bulbasaur; Charmander X/Y; Squirtle; Chikorita; Totodile; Treecko; Torchic; Mudkip; Tepig; Chespin; Fennekin; Froakie. Do not invent Mega Stones for starter families without a native binding. All original choices remain supported and powerful team options regardless of whether their family has a Mega.

Before granting a slot, retain the existing check for an owned stone in Bag, item PC, party held items or PC Pokémon held items. An already owned stone fulfills that slot. Otherwise deliver to Bag, then item PC; persist only a successful receipt. Pending slots survive re-entry. Completed bits survive later disposal, so revisiting Steven does not regenerate a discarded gift.

Legacy saves require one additional visibility reconciliation already owned by INTRO-STONES: **an encoded but still-owed second Pokémon creates no second-family stone entitlement**. Only after actual second-Pokémon delivery, if letter/badge/Ring handoff is complete and a populated second slot is still pending, clear Steven’s Granite Cave hide flag. He resumes the pending stone handoff without repeating the letter, Ring or first-family gifts. No pending slot may be lost by making him depart; a no-Mega, already fulfilled or not-yet-delivered second family does not wake him.

### Preserve deliberate finite alternatives

The two-world-source stones are Absolite, Alakazite, Altarianite, Cameruptite, Gengarite, Gyaradosite, Lopunnite, Lucarionite, Metagrossite and Staraptite. These are explicit authored alternatives, with at most one award from each distinct receipt. Keep that finite generosity. Do not create a99-item global ownership ledger simply to prevent a player collecting a second legitimately authored copy.

Distinguish alternatives from retries. Winona’s first/repeat gift functions and Lilycove’s Altarianite keeper share one `FLAG_RECEIVED_WINONA_ALTARIANITE` entitlement; the separate Route111 pickup has its own flag. The other leader first/retry functions likewise share their leader receipt. Trick House entrance retrieval is the same stage prize as the rear-room grant.

**W-TRICK-REWARDS** fixes the real exceptions. Puzzle 4’s retry must give King’s Rock, matching the immediate reward. The final chosen tent and Alakazite need independent successful receipts, with a one-time initialization marker for conservative old-state handling; claim only the still-owed part at the entrance. Keep Granite Cave’s Alakazite pickup as its deliberate alternate. The three receipt/initialization flags are reserved at 0x2B2–0x2B4 in the [central state allocation](appendices/state-allocation.md); initialize them only in the agreed version 4 migration. Do not force players to replay puzzle 8 or grant extra tents to compensate for a failed stone insertion.

Diancie’s chamber keeps its physical Diancite. The restored authored Wallace exhibition is optional and independent under W-WALLACE-ROOT; its old unbound duplicate item handoff is not reactivated. Gardevoirite remains an unconditional one-time world gift from Wally’s aunt; the new optional Indeedee-F gift must not become its prerequisite or consume its receipt.

### Preserve berry exchanges as local exploration rewards

The Berry Master exchanges exactly 20 total garden berries for one Baxcalibrite, Dragoninite or Tyranitarite, once per stone. Eligible currency is Razz, Bluk, Nanab, Wepear, Pinap, Pomeg, Kelpsy, Qualot, Hondew, Grepa and Tamato, in any mixture. Keep the visible count, price, eligibility list and affirmative confirmation.

The existing source validates the selected stone, its one-time flag, enough berries and successful stone insertion before removing currency. Preserve that ordering. Invalid selection, refusal, already claimed, insufficient berries and full Bag do not debit or claim the reward. Keep the real garden sources and planting; free vendors and competitive presets must not produce this currency as a back door around the exchange.

This is a modest regional reward. It must not become a farming requirement for ordinary battle preparation, and the book adds neither a free Mega archive nor a new economic subsystem.

### Exact forms deserve exact information

Six item descriptions are repaired under **MEGA-FORM-LABELS**, with exact three-line replacements in their catalogue entries: Floettite names Eternal Floette; Zygardite names Complete Zygarde; Raichunite X/Y name Kantonian Raichu and the corresponding Mega; Slowbronite and Greninjite name their ordinary base forms. Preserve their item IDs, prices, icons and bindings.

Follow [chapter 04’s form authority](chapters/04-acquisition-evolutions-forms.md):

- **FORM-12:** Eternal Floette is already a real7% Verdanturf Meadow encounter; ordinary flower-color Floette cannot use Floettite. Zygardite needs Complete Forme.
- **FORM-08/12:** obtain Zygarde, use the Cube for Power Construct, and reach≤50% HP at turn end to become Complete. Validate the actual subsequent Mega opportunity rather than pretending the Cube grants persistent Complete/Mega directly.
- **FORM-04:** Droopy and Stretchy Tatsugiri join Curly at the submarine habitat. Their Order Up behavior differs; these are functional forms, not merely cosmetic recolors. Tatsugirinite supports all three.
- **FORM-05:** the verified Original Magearna color can be selected only after actual Magearna ownership. Magearnite supports both native colors. The prototype’s Diancie prerequisite is separate from the eight-badge stone gift.
- Meowsticite supports both sexes through distinct form tables. Raichunite does not activate Alolan Raichu; Slowbronite does not activate Galarian Slowbro; Greninjite’s base is ordinary Greninja.

Stone discovery can precede evolution or a later legendary. That anticipation is desirable when the player understands what is needed. It is not a reason to restrict the broad early roster, weaken the opening battles or assign each Pokémon a protected chapter.

### Legendary acquisition and relic rewards

The baseline has 82 Sign entries:51 deliberate landmarks,7 visible field entries,7 ordinary-wild entries and 17 other-provider entries. Those17 comprise 12 Circuit rewards,2 mastery rewards,2 Game Corner rewards and 1 breeding route. An `OTHER_SIGN`’s zero badge field is not evidence of opening availability; its real provider owns the gate.

Devon remains an optional guide, including after a discovery is completed. The actual field/badge/story/species conditions remain authoritative. Failed encounters can be retried after leaving and returning; captures and earned reward entitlements remain permanent. W-SIGN-OPTIONAL and W-SIGN-LOCAL remove stale mandatory research and retired partner silhouettes from live world text. Do not restore the old compulsory researcher visit or permanent failure rule.

FORM-06 appends the three distinct Galarian bird Signs at IDs82–84, leaving the original 82 identities and existing native one-off indexes stable. Exact-form acquisition is resolved before any permitted family fallback. Its detailed locations, conditions and capture identity are owned by chapter 04; this reward chapter does not duplicate that definition.

The 16 native one-off species below remain separate from the Sign array. Their reserved encounter indexes are96–111 in source order. The apparent `CAUGHT`/`DEFEATED` names of physical hide flags do not independently prove capture—Heatran’s reveal is a useful example. Preserve actual caught ownership and each event’s current retry behavior.


| Native species | Current principal scene | Actual access context |
|---|---|---|
| Groudon | [TerraCave_End](world/maps/TerraCave_End.md) | Abnormal-weather postgame cave; actual event location and arrival state |
| Kyogre | [MarineCave_End](world/maps/MarineCave_End.md) | Abnormal-weather postgame cave reached through its current Dive approach |
| Rayquaza | [SkyPillar_Top](world/maps/SkyPillar_Top.md) | After the story awakening and Sootopolis resolution; actual return/bike-floor approach |
| Regirock | [DesertRuins](world/maps/DesertRuins.md) | Sealed Chamber opened plus this ruin’s local puzzle |
| Regice | [IslandCave](world/maps/IslandCave.md) | Sealed Chamber opened plus this cave’s local puzzle |
| Registeel | [AncientTomb](world/maps/AncientTomb.md) | Sealed Chamber opened plus this tomb’s local puzzle |
| Latias | [SouthernIsland_Interior](world/maps/SouthernIsland_Interior.md) | Selected native roaming/shrine identity and the complementary island route; verify actual choice state |
| Latios | [SouthernIsland_Interior](world/maps/SouthernIsland_Interior.md) | Selected native roaming/shrine identity and the complementary island route; verify actual choice state |
| Lugia | [NavelRock_Bottom](world/maps/NavelRock_Bottom.md) | Champion-issued Mystic Ticket route and the long descent |
| Ho Oh | [NavelRock_Top](world/maps/NavelRock_Top.md) | Champion-issued Mystic Ticket route and the upper ascent |
| Mew | [FarawayIsland_Interior](world/maps/FarawayIsland_Interior.md) | Champion-issued Old Sea Map route and hide-and-seek |
| Deoxys | [BirthIsland_Exterior](world/maps/BirthIsland_Exterior.md) | Champion-issued Aurora Ticket route and triangle puzzle |
| Jirachi | [MeteorFalls_JirachisRoom](world/maps/MeteorFalls_JirachisRoom.md) | Actual deep Meteor Falls stair/Waterfall route to the chamber |
| Diancie | [CaveOfOrigin_DianciesRoom](world/maps/CaveOfOrigin_DianciesRoom.md) | Eight-badge ladder and expanded Origin cave route |
| Heatran | [ScorchedSlab_HeatransRoom](world/maps/ScorchedSlab_HeatransRoom.md) | Actual deep cave path and Magma Stone initial reveal |
| Moltres | [EmberPath](world/maps/EmberPath.md) | Heat Badge/Strength branch through Ember Path |

Six capture reward groups grant 24 relic items: Groudon→Red Orb; Kyogre→Blue Orb; Zacian→Rusted Sword; Zamazenta→Rusted Shield; Ogerpon→three masks; Arceus→seventeen type plates. Keep the existing earned-group and pending-item bits. An actual acquisition whose insertion fails creates debt; later nurse/service retries settle it. Migration must not fabricate a pending reward merely from old Pokédex ownership or recreate a relic discarded after a fulfilled grant. Fusion tools and Zygarde Cube follow FORM-08 and the actual Birch research reward path.

### Implementation and verification

First reconcile the exact source and final acquisition/form definitions. Preserve all current stone placements while applying the named receipt, clue and description repairs. Extend the original-starter helper through INTRO-STONES’s single receipt variable; do not implement another parallel gift ledger. Apply the independent Trick House receipts together with entrance recovery and central flag allocation.

Then verify behavior in proportion to the changes. Useful distinct cases are: ordinary pickup with persistent ownership; a leader first/retry gift; the shared Winona/keeper entitlement; conditional two-starter delivery with partial storage; final tent/stone split; each materially different berry transaction outcome; the special exact-form bindings; and a production Ring check. Inspect unchanged data bindings once and reuse valid existing evidence rather than creating99 identical tests.

Field access checks must start from a legitimate story entry and reach the exact tile or NPC, including any real Cut, Strength, Surf, Dive, Waterfall, bike, tide, ledge or scripted gate. New Mauville requires the actual Surf/Key route; Dive access requires Mind Badge/Steven’s handoff; Southern Island uses the Champion ferry pass; desert approaches require their actual Goggles/underpass route. A fallback cap attached to a whole map is not acceptable proof. Record any unresolved earliest-access question explicitly instead of inventing a value.

Audit every existing gate’s contract, fixture, configuration and artifact freshness before interpreting a failure. Update or delete obsolete prose/count expectations that constrain deliberate design. Keep meaningful item/receipt, save and battle failure protections. This chapter’s99 count and quoted text are review scope and implementation instructions, not new production invariants. No ROM was built or gameplay changed in producing this book; traversal and battle acceptance remains implementation-time evidence.

---

## 06 — Shared expert AI and tactical execution

**Authority:** these are the final shared AI specifications used by every authored trainer, including the new opening rival and explicitly scripted rescue. Individual battle chapters name required scenarios; they do not each install another trainer-specific scoring patch. Difficulty settings change levels, not access to these tactical capabilities.

The current engine already contains substantial damage, status, switching, partner and field logic. Preserve successful behavior. The source review identified specific defects and limitations below; the book has not executed a new native battle build or proven mathematical optimality. Every repair needs a reproducing fixture and relevant negative control before implementation is called verified.

### Common information and decision contract

Use one explicit expert campaign profile. Opponents may know team species, moves, items and abilities through that profile, but must not read the player's unexecuted chosen-command buffer as foreknowledge. Current transformed/suppressed abilities, consumed items, PP, status and field state override stale assumptions. Keep the observation contract consistent across trainer classes and difficulty modes. Document this game rule in the optional battle guide rather than claim exact equivalence to another live ladder's information rules.

Champions Points use the formula and fixed effective IV31 in chapter 02. Compare priority before Speed: Spore already precedes Trick Room by priority, while Speed investment can determine whether the sleeper acts before the setter's attacking partner. Ability/item/field changes can alter that order. Never substitute a conventional EV or zero-Speed-IV calculation.

#### Integration sequence

1. SAI-03 and SAI-11: trustworthy candidate state and current-hit damage.
2. SAI-02: truthful roll/crit forecasts.
3. SAI-01 and SAI-09: shared capabilities and one owner per mechanic.
4. SAI-06: real base/Mega form evaluation and owner budgets.
5. SAI-04/05/07/08/10: reserve, paired-action, ally-activation and field decisions on those reliable inputs.
6. SAI-12: preserve existing working cases and validate the actual authored matchups.

Do not introduce a monolithic replacement AI or a parallel external battle simulator. Reuse native move, stat, ability, form and field primitives with compact scratch state. Do not copy entire save blocks or allocate a new heap object for every candidate.

### SAI-01 — One expert campaign profile

**Disposition: REPAIR.**

**Source evidence**

- [scripts/implement_emerald_champions_master_battles.py:39](baseline/source/scripts/implement_emerald_champions_master_battles.py#L39)
- [include/constants/battle_ai.h:49](baseline/source/include/constants/battle_ai.h#L49)
- [src/battle_ai_util.c:201](baseline/source/src/battle_ai_util.c#L201)
- [src/battle_ai_main.c:321](baseline/source/src/battle_ai_main.c#L321)

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

### SAI-02 — Truthful damage and critical probabilities

**Disposition: REPAIR.**

**Source evidence**

- [src/battle_ai_util.c:112](baseline/source/src/battle_ai_util.c#L112)
- [include/config/ai.h:109](baseline/source/include/config/ai.h#L109)
- [src/battle_ai_util.c:860](baseline/source/src/battle_ai_util.c#L860)
- [src/battle_ai_main.c:6206](baseline/source/src/battle_ai_main.c#L6206)

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

### SAI-03 — Transactional switch-in state

**Disposition: REPAIR.**

**Source evidence**

- [src/battle_ai_switch.c:74](baseline/source/src/battle_ai_switch.c#L74)
- [src/battle_ai_switch.c:2708](baseline/source/src/battle_ai_switch.c#L2708)
- [src/battle_ai_switch.c:2861](baseline/source/src/battle_ai_switch.c#L2861)
- [src/battle_ai_switch.c:2468](baseline/source/src/battle_ai_switch.c#L2468)

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

### SAI-04 — Actual doubles reserve selection

**Disposition: OPTIMIZATION.**

**Source evidence**

- [src/battle_ai_switch.c:2621](baseline/source/src/battle_ai_switch.c#L2621)
- [src/battle_ai_switch.c:2468](baseline/source/src/battle_ai_switch.c#L2468)
- [src/battle_ai_util.c:6334](baseline/source/src/battle_ai_util.c#L6334)
- [src/battle_ai_switch.c:1073](baseline/source/src/battle_ai_switch.c#L1073)

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

### SAI-05 — Soft ace preservation

**Disposition: REPAIR.**

**Source evidence**

- [src/battle_ai_switch.c:240](baseline/source/src/battle_ai_switch.c#L240)
- [src/battle_ai_switch.c:1398](baseline/source/src/battle_ai_switch.c#L1398)
- [src/battle_ai_switch.c:2498](baseline/source/src/battle_ai_switch.c#L2498)
- [src/battle_ai_main.c:550](baseline/source/src/battle_ai_main.c#L550)

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

### SAI-06 — Real base-versus-Mega candidates and budget

**Disposition: REPAIR.**

**Source evidence**

- [src/battle_ai_main.c:597](baseline/source/src/battle_ai_main.c#L597)
- [src/battle_ai_main.c:458](baseline/source/src/battle_ai_main.c#L458)
- [src/battle_ai_util.c:944](baseline/source/src/battle_ai_util.c#L944)
- [src/battle_gimmick.c:67](baseline/source/src/battle_gimmick.c#L67)
- [src/battle_ai_main.c:747](baseline/source/src/battle_ai_main.c#L747)
- [src/battle_gimmick.c:103](baseline/source/src/battle_gimmick.c#L103)
- [src/battle_util.c:8569](baseline/source/src/battle_util.c#L8569)
- [src/battle_controllers.c:3299](baseline/source/src/battle_controllers.c#L3299)

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

### SAI-07 — Concrete paired actions

**Disposition: OPTIMIZATION.**

**Source evidence**

- [src/battle_ai_main.c:482](baseline/source/src/battle_ai_main.c#L482)
- [src/battle_ai_main.c:1029](baseline/source/src/battle_ai_main.c#L1029)
- [src/battle_ai_main.c:3290](baseline/source/src/battle_ai_main.c#L3290)

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

### SAI-08 — Deliberate ally activation

**Disposition: OPTIMIZATION.**

**Source evidence**

- [src/battle_ai_main.c:3228](baseline/source/src/battle_ai_main.c#L3228)
- [src/battle_ai_util.c:5775](baseline/source/src/battle_ai_util.c#L5775)
- [src/battle_ai_main.c:3470](baseline/source/src/battle_ai_main.c#L3470)
- [src/battle_ai_main.c:3670](baseline/source/src/battle_ai_main.c#L3670)

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

### SAI-09 — Shared mechanics instead of trainer-ID privilege

**Disposition: REPAIR.**

**Source evidence**

- [src/emerald_champions_ai.c:141](baseline/source/src/emerald_champions_ai.c#L141)
- [src/battle_ai_main.c:326](baseline/source/src/battle_ai_main.c#L326)
- [src/battle_ai_main.c:3916](baseline/source/src/battle_ai_main.c#L3916)
- [src/battle_ai_main.c:2191](baseline/source/src/battle_ai_main.c#L2191)

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

### SAI-10 — Consistent field and screen evaluation

**Disposition: REPAIR.**

**Source evidence**

- [src/battle_ai_field_statuses.c:66](baseline/source/src/battle_ai_field_statuses.c#L66)
- [src/battle_ai_field_statuses.c:102](baseline/source/src/battle_ai_field_statuses.c#L102)
- [src/battle_ai_field_statuses.c:456](baseline/source/src/battle_ai_field_statuses.c#L456)
- [src/battle_ai_util.c:4122](baseline/source/src/battle_ai_util.c#L4122)
- [src/battle_ai_field_statuses.c:506](baseline/source/src/battle_ai_field_statuses.c#L506)

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

### SAI-11 — Separate first-hit damage from two-hit Berry planning

**Disposition: REPAIR.**

**Source evidence**

- [src/battle_ai_util.c:885](baseline/source/src/battle_ai_util.c#L885)
- [src/battle_ai_util.c:1366](baseline/source/src/battle_ai_util.c#L1366)
- [src/battle_ai_main.c:4115](baseline/source/src/battle_ai_main.c#L4115)
- [src/battle_ai_main.c:4129](baseline/source/src/battle_ai_main.c#L4129)

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

### SAI-12 — Preserve existing good work and bound verification

**Disposition: KEEP.**

**Source evidence**

- [src/battle_ai_switch.c:830](baseline/source/src/battle_ai_switch.c#L830)
- [src/battle_ai_switch.c:1047](baseline/source/src/battle_ai_switch.c#L1047)
- [src/battle_ai_main.c:1300](baseline/source/src/battle_ai_main.c#L1300)
- [src/battle_ai_main.c:827](baseline/source/src/battle_ai_main.c#L827)
- [src/battle_ai_util.c:2293](baseline/source/src/battle_ai_util.c#L2293)
- [src/battle_util.c:5529](baseline/source/src/battle_util.c#L5529)

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

### Authored teams remain the consumers

The battle volumes reproduce full baseline and final teams. Their specific examples include Falinks/Gallade ordering, Fake Out plus setup, Room setters, Instruct, ally Surf/Steam Engine, snow screens, Palafin activation, Choice-locked friendly spread damage and actual Mega abilities. Test the intended board and an interrupted board; do not prove a combo only by forcing the desired commands.

The newly restored Walrein, Dusclops and Nosepass need purposeful decisions with their partners. The revised Huge Power Mega Starmie requires actual transformed Attack in AI forecasts. Rayquaza's move-based Mega eligibility must be considered alongside Steven's Metagross; one stone is not proof that only one Pokémon can Mega. Zygarde's Complete transition and Eternal Floette's specific Mega eligibility are also part of the final acquisition/team reconciliation.

### Verification discipline

Audit any existing test that disagrees: its current contract, configured forms, exact Points/natures/levels, fixture Speed/HP, branch setup, and source/artifact freshness. Preserve real invariants; update or retire obsolete assumptions. A score-number difference alone does not establish worse play. Conversely, a source comparison or scripted forced move is not proof the AI independently chooses the right action.

Use a focused set of production-function and native action-selection tests covering the distinct failures above, followed by actual representative authored battles. Measure decision latency and peak scratch memory at doubles, full-party, dual-owner and multiple-Mega-candidate boundaries. Re-run only when a change or unresolved concern justifies it; no historical test-count floor is added.

---

## 07 — Preparation, move access and evolution convenience

This chapter preserves the existing expanded-game preparation freedom. It repairs broken or misleading connections between the builder, evolution, field access and generated presets without imposing a new ladder-legality purge or a progression curriculum.

### PREP-02 — Let the Leveler finish ready evolutions at the cap

#### Current source

`src/item_use.c:CanLevelPartyToCap` and `src/party_menu.c:FindNextLevelerSlot` only consider non-Egg Pokémon below the current cap. By contrast, `ItemUseCB_RareCandy` already checks `GetEvolutionTargetSpecies(...EVO_MODE_NORMAL...CHECK_EVO)` when leveling has no effect, and can perform a ready evolution without increasing level. Thus an appropriate Rare Candy can evolve a ready level 100 Pokémon; the convenient Leveler entry path can still skip it.

This is a convenience/routing defect, not proof that every capped evolution is impossible. It affects practical use of newly learned evolution moves, friendship readiness, held evolution items and newly reached map conditions. The snapshot's Meltan, Bisharp, regional starters and Type:Null are relevant examples.

#### Final specification

Introduce one shared predicate for Leveler eligibility: nonempty, non-Egg Pokémon that is either below the cap or has a ready **normal** evolution under the current map/time/item/move/friendship conditions. The ready check is side-effect-free CHECK_EVO, including Everstone and all current table conditions. Use it from both bag/registered-item entry and party-sequence scanning; do not leave two divergent definitions.

For a ready already-capped Pokémon, invoke the existing Rare-Candy-style normal evolution branch as a Leveler action. Do not increase EXP, exceed the cap, consume the Leveler, fabricate friendship or waive a required item. Preserve native evolution scenes, cancellation and legal branch selection.

After an evolution, continue the existing party sequence. Guard against an unchanged species or a cancelled evolution being selected indefinitely. Each attempted slot/evolution must either change species or advance the sequence. If a legitimate next stage is immediately ready, the existing continuation may present it; cancellation must not be silently reversed.

Change the no-effect message to `Your party is at the current cap.\nNo Pokémon is ready to evolve.$` only when both leveling and ready evolution are unavailable. Keep the ordinary completion message accurate if the action evolved Pokémon without raising levels.

Acceptance: Munchlax reaching friendship at cap 14, Mime Jr./Bonsly learning Mimic at the cap, Type:Null at 100, Meltan arriving at New Mauville at its current level, Bisharp holding Leader's Crest, a blocked condition, an Egg, Everstone, cancellation, an evolution chain and a party with mixed ready/unready slots. Compare actual EXP/level before and after; no extra level is granted.

### PREP-02-B — Battle-only evolution with battle EXP disabled

Tandemaus has a native `EVO_LEVEL_BATTLE_ONLY` evolution. In `src/battle_main.c:TryEvolvePokemon`, the existing call to `EVO_MODE_BATTLE_ONLY` is gated by `gLeveledUpInBattle`; battle EXP is globally disabled in the configured Champions game. Raising Tandemaus with the Leveler does not establish an in-battle level gain.

Preserve the existing callback rather than add a second evolution scene pipeline. Add a narrow ready-battle-only evaluation after the existing special evolution check when a normal, non-linked/non-recorded/non-facility battle has legitimately ended in victory or capture. Require a conscious owned non-Egg Pokémon and actual species-table readiness. Retain the current level 25 threshold, PID branch conditions and Everstone behavior. Do not turn this into a general post-loss, post-flee or tutorial evolution trigger.

Because `EVO_MODE_BATTLE_ONLY` also accepts ordinary level evolutions, the no-level-gain path needs an explicit filter/mode that considers **only** `EVO_LEVEL_BATTLE_ONLY` entries. Reuse the same additional-condition evaluator and CHECK_EVO/DO_EVO handling. The normal leveled-up path remains unchanged. No EXP or level is added.

Temporary Circuit/Tent normalization must never cause a permanently underleveled Pokémon to evolve. Keep these modes excluded from the no-level-gain path. Honor the existing attempted-slot flags and cancellation, so a Pokémon receives one coherent evolution presentation.

Acceptance: Tandemaus at 25 and 100 can evolve after a legitimate normal battle without EXP; level 24 cannot; both PID branches remain possible; fainted/lost/fled/tutorial/recorded/linked/temporary-facility cases do not introduce new side effects. Confirm that no existing callback already satisfies a tested case before adding redundant work.

### FORM-11-NATURE — Edited Nature governs Nature-based evolution

The acquisition volume specifies the exact source repair for Toxel: the builder edits `MON_DATA_HIDDEN_NATURE`, which the battle stat function uses, while current evolution condition logic reads PID-derived `GetNature`. Use the displayed/edited hidden Nature for Nature-based evolution conditions, preserving personality, shininess, gender and unrelated PID-based branches.

This is not permission to replace Wurmple, Dunsparce or Tandemaus's deliberate personality conditions with Nature. Their conditions remain separate. Verify all 25 Nature selections against Amped/Low Key outcomes and the displayed/battle Nature.

### PREP-03 — One explicit expanded move-access policy

#### Facts from the snapshot

The current authored preset catalog contains 6,923 sets across doubles defaults/alternatives and retained singles-oriented preparation buckets. The native preparation list deliberately unions species preparation moves with preset-provided moves. The game is an expanded Champions-style ruleset with project-specific data; a move absent from a separate pinned reference is not automatically rejected by the native game.

The book's read-only comparison found861 sets with at least one difference from the mapped pinned Champions/latest-mainline-plus-reviewed-extension reference and 262 sets whose forms that resolver could not identify. See [the complete mismatch inventory](inventory/preset-reference-mismatches.json) and [unmapped forms](inventory/preset-reference-unmapped.json). Examples include older event/transfer moves, expanded role moves and special form mappings. These totals are an audit result, not a production failure quota.

The reference generator's own policy is specific: Champions-mod learnsets where supplied, otherwise each species's latest mainline generation, with form/pre-evolution inheritance and selected reviewed extensions. That policy is narrower than simply retaining every implemented expanded-game move. The reference resolver also lacks some project form names. Neither issue can be resolved by blindly labeling every difference illegal.

#### Final policy and authoring

Preserve the current native expanded move access and deliberate existing extensions. The preset catalog remains authored content. Newly proposed sets must use configured real moves, valid species/form ability slots, legal native item interactions and actual preparation access. A reference mismatch receives an explicit explanation or a deliberate design proposal; it does not automatically remove a move or invalidate a team.

INTRO-01 consequently preserves the four initial presets that differed only from the pinned reference. It changes Chikorita/Torchic where the new two-member rescue requires direct offense, and supplies exact newly resolved regional rival loadouts. Those are role-based changes, not a blanket learnset purge.

Retain the underlying singles-oriented preparation data while wild captures remain singles and the player may freely choose an orientation. No trainer competition launches a singles battle. Rename misleading preparation help if necessary: an orientation's name describes a build, not an available competition format. The default recommended campaign orientation remains doubles.

Do not claim this expanded availability is identical to every current official Champions ladder rule. The book specifies the actual game being authored. A future strict external-format mode would be a separate design decision, not an implicit consequence of running an old gate.

#### Meaningful integrity checks

Keep structural checks for existing moves, no duplicate moves in a set, correct stat ordering/budget, valid base/form ability resolution, actual protected-item handling and complete generated materialization. Preserve intentional ability-changing Mega roles by checking both the starting form and the actual transformed form. Do not demand that a pre-Mega ability remain active after transformation.

Treat broad quality heuristics as review prompts. A low-power move can have a specific job; an unusual investment can create an exact low-cap threshold; a retained older move can be an intended game extension. Fix concrete incoherence, invalid data or an inferior choice supported by the full team context.

### PREP-04 — Form tools and protected rewards

The acquisition volume owns the exact additions for practical form access. Reuse the existing free evolution-item archive for the selected missing nonprotected form tools. Keep Silvally Memories in their existing free Species category; retain the Weather Institute's Reveal Glass and Birch's fusion-tool reward routes.

Do not add Mega Stones, earned Primal Orbs, crowned relics, Ogerpon masks or Arceus plates to that free archive. Existing protected-relic delivery has finite bit capacity and must not be extended by blindly appending items. INTRO-01 owns the new two-starter stone receipts; the Mega volume owns ordinary stone rewards.

Meteorite is also a story key: an early free replacement could bypass story conditions. Use the acquisition chapter's gated Cozmo/Deoxys recovery or interaction path rather than adding the quest item to an unrestricted early menu.

### PREP-05 — Field convenience follows the actual resolver

The current field-move resolver accepts a compatible party species once the relevant HM/badge is owned without requiring the move to occupy a battle slot. The old last-learned-Surf deletion guard can therefore block a safe preparation change.

Apply the world volume's W-FIELD-SURF repair: decide whether the resulting party retains actual unlocked Surf capability through the current resolver, rather than counting only Pokémon with Surf in their moveset. Preserve genuine traversal safety and the independent HM/badge requirements. Do not globally toggle unrelated HM-forgetting settings or remove valid progression gates.

### Collection and storage meaning

The existing14 boxes hold420 boxed Pokémon. Preserve that layout and save compatibility. Comprehensive availability in this book means the ability to obtain and use supported species/forms within one save over time; it does not promise a simultaneous living dex of every form. One-off release choices and fusion-parent ownership must be accurately explained. Cosmetic conveniences and repeatable normal encounters do not create duplicate one-off legendary entitlements.

Validation of these changes is proportional: reproduce the Leveler readiness and edited-Nature failures, check the exact resulting party state, verify required items and source materialization, and exercise the relevant UI. No 6,923-test suite mirroring every preset record is proposed.

### PREP-06 — Optional bonding preparation

Add a deliberate friendship adjustment to the existing shared Center specialist, available from Oldale, and let Verdanturf's Friendship Rater call the same service after its personal dialogue. This makes early friendship-dependent Pokémon practical without repeatedly teaching/forgetting moves or changing their evolution requirements.

Offer three explicit values: **Low (0)**, **Evolution threshold (the configured `FRIENDSHIP_EVO_THRESHOLD`)**, and **Maximum (255)**. Display the selected Pokémon and requested value before confirmation. Low/maximum remain useful for the expanded game's retained Frustration/Return choices. Do not automatically set every Pokémon to maximum or present the action as a compulsory tutorial.

Use `MON_DATA_FRIENDSHIP` on the selected owned Pokémon, preserving personality, shininess, gender, Nature, ability, moves, points, held item, experience and ownership. Reject Eggs: the same field stores hatch-cycle information for them and must not be edited as friendship. Reject empty/invalid slots and make cancel a complete no-op. Recalculate through the existing native stat function as appropriate; do not change the configured battle affection/crit behavior.

The action does not automatically evolve the Pokémon. The player can use the Leveler through PREP-02 when the existing level/map/time/item/move conditions are met, and Everstone/cancellation still work. No new global time-change feature or bypass of night/day conditions is introduced.

Implementation owners are `data/scripts/emerald_champions.inc` for the optional menu, `src/field_specials.c` for one validated friendship setter/preview helper, and `data/specials.inc` for the script binding. Verdanturf's rater references that helper instead of maintaining a second implementation.

Acceptance: each of the three values on a normal owned Pokémon, before/after comparison of unrelated data, Egg/empty/cancel rejection, current Return/Frustration calculation, a ready friendship evolution at the cap, an unmet night/item condition, and an Everstone holder. This is a small convenience proposal, not proof that the previous friendship paths were unobtainable.

---

## 08 — Implementation sequence and global reconciliation

This book is a specification, not an implemented revision. The original game remains the baseline. Implement through coherent changes that preserve successful authored content, with each subsystem still having one owner.

### Rebase the specification before touching the game

Use `baseline/manifest.json` to identify the exact source inputs reviewed. The snapshot includes uncommitted work at the recorded capture time; git HEAD alone does not identify it. Compare relevant live files with these hashes and inspect intervening changes before applying any instruction. Never replace newer work with a frozen copy.

`baseline/source/` is an evidentiary content snapshot, not a standalone build checkout with every toolchain and visual asset. Build and test from the canonical project or an appropriately isolated complete checkout. Do not build inside the book and then claim the working game was updated.

### Integration units

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

### Keep the existing authoring

The original trainer identities, encounter-group identifiers, maps and save layout remain. The battle volumes supply full baseline and final loadouts. Most Pokémon fields are unchanged; many REPAIR dispositions correct false explanations or require shared AI behavior rather than replacing the team.

The new opening is deliberately broader: six legacy aliases now dispatch four-member doubles teams, and two selected starters determine the unchosen rival family. All later regional substitutions use the explicit supplement. The aliases are compatibility identifiers, not evidence that the rival owns the player's selected Pokémon.

Roxanne, Phoebe and Glacia regain their primary iconic species with defined competitive roles. Wally retains the Ralts-family/Gallade identity; existing major signatures such as Hariyama, Manectric, Torkoal, Slaking, Altaria, Solrock/Lunatone, Kingdra, Absol, Salamence, Milotic and Metagross remain. Legendary-heavy teams are preserved where coherent. Displacing a unique Glastrier or Shedinja showcase is an explicit local decision, not a reason to force an unrelated replacement elsewhere; those species remain available and supported.

### One owner for each cross-volume change

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

### Source changes implied by preserved content

KEEP on a roster means its Pokémon fields remain intact. The battle still consumes the new universal floor and expert AI where applicable. KEEP on a world map means its layout and unlisted interactions remain intact; its shared nurse/vendor/guide contract may receive an explicitly owned global improvement. This is why global contracts must be integrated before interpreting each page's disposition in isolation.

No blanket behavior change should be hidden behind a preserved label. The unified decision index records the local and shared obligations. A source claim that turned out false—such as treating high-tide layout names as missing map encounters, or interpreting Heatran's physical hide flag as capture ownership—is marked as rejected rather than turned into a repair.

### Play and traversal acceptance

A fresh-save route should exercise the two-starter rescue, pre-rival catching/preparation, the first four-member rival, Roxanne and core story handoffs through the League. Include optional habitats and rewards at the actual first access conditions, rather than warping a fully equipped party into every room and calling it progression proof.

Separately, battle testing must disable traversal auto-win and exercise the intended AI decisions. Demonstrate credible solutions with different team structures, including early Nature/Point reallocations, without installing a fixed number-of-solutions gate or demanding a prescribed six-Pokémon answer. The player is allowed to solve a hard puzzle cleanly.

For late collection, test at least the distinct mechanism classes: direct wild regional form, edited-Nature evolution, normal evolution at cap 100, battle-only evolution with EXP disabled, cosmetic/functional form services, irreversible nonbreedable branch alternatives, exact Galar legendary identity, and fusion/unfusion move preservation. Shared mechanism coverage is more valuable than hundreds of tests that merely mirror table rows.

For save/reward integrity, test fresh version 4 state and representative old states at every new transaction boundary. Ordinary reload must not rerun migration or duplicate either starter, stones, research partners, final Trick prizes or Circuit entitlements.

After a real build, bind the actual ROM/ELF to its source inputs through the existing artifact workflow. A book source hash, an old passing ELF or an automated traversal result cannot certify the new competition's quality. Report exactly what was exercised, following chapter 09.

### Definition of a completed implementation

All final authoring is materialized; new entrypoints and retired entrypoints behave as specified; each advertised functional acquisition has its stated route; relevant source/transaction/AI failures are repaired; changed scenes and menus render correctly; and actual battle/traversal evidence is attached to the exact new build. The book itself reaches only the preceding milestone: a complete, internally reconciled implementation specification ready for the user's review.

---

## 09 — Tests, gates and honest evidence

The user explicitly warned that many existing tests and gates are outdated, unnecessary or hostile to creative changes. Treat them as code and claims to examine. A failure is evidence to investigate, not authority to restore old design.

### TEST-01 — Decide the contract before changing the game

For any failing or inherited check:

1. Identify the current user requirement it is supposed to protect.
2. Read the actual assertion and its fixture/configuration, not only the test name.
3. Confirm the source/artifact versions, battle format, actual form/ability, effective levels and Champions Points calculation.
4. Determine whether it fails for a real behavior defect, stale assumptions, broken setup or an obsolete preference.
5. Keep a meaningful check, update a superseded contract, or retire a redundant/invalid check with its reason recorded.

Do not weaken a valid save/transaction/battle invariant merely to get green output. Equally, do not change good teams, restore singles, remove intended move access or preserve arbitrary species placement to satisfy a historical gate.

### Disposition of relevant existing checks

|Check or family|Final disposition|Reason and exact treatment|
|---|---|---|
|Authored teams → master → native trainer source agreement|KEEP/UPDATE|Protect the current authoring pipeline. Regenerate from the new intended authoring; do not compare the new game to the old snapshot as a permanent requirement. Update opening format and regional-pair handling.|
|Configured species/ability validity|KEEP|A requested ability that falls back to another slot changes observable battle behavior. Validate starting and transformed forms correctly.|
|66 total/32 per-stat preparation budget and display/engine order|KEEP|These are actual current builder rules. Use the native flat-Points calculation and effective IV31, not old EV/IV conventions.|
|New Medium cap-minus-two floor|UPDATE|Existing gym exemptions and the old singleton opening are superseded. Test actual runtime levels at entry and revisit, without double-reducing facility opponents.|
|Old opening singles/minimum party exceptions|RETIRE/UPDATE|INTRO-01 is explicitly doubles with two starters; Route103 has four opponents. Preserve full 6+6 and half 3+3 multi semantics instead of forcing one old count.|
|Map/label/warp/geometry integrity|KEEP narrowly|Dangling references, invalid coordinates and wrong source assets can break the game. A static graph does not prove stateful traversal. Do not freeze every historic object position or dialogue sentence.|
|Wild table shape, configured species, level bounds and current 5%policy|KEEP/UPDATE|These protect usable distribution. No fixed twelve-unique-species quota, old slot weight requirement, or fixed roster count. Reflect explicit new fishing overrides.|
|Mega reward transaction and coverage of actually enabled stones|KEEP/UPDATE|Protect real rewards, flags and bag-full retries. Derive enabled items from current data. Do not require exactly 99 forever or one particular past location.|
|Legendary caught/unlocked identity and state capacity|KEEP/UPDATE|Exact forms, permanent captures and finite bit capacity are real requirements. Append the proposed Galar bird IDs safely; retire old mandatory-Devon/permanent-loss expectations.|
|Source/ROM/ELF stamp and save integrity checks|KEEP|These protect against testing stale artifacts or corrupting player data. They do not certify game quality.|
|Pinned external learnset comparison|ADVISORY under the expanded-native policy|The current game deliberately supplies expanded preset moves and has special form mappings. Record differences; do not mass-remove moves solely because of this reference.|
|Repeated team fingerprints or mandatory every-Mega/every-legendary showcases|RETIRE as blockers|`audit_emerald_champions_master_battles.py` includes these checks around 968–982. Intentional thematic repetition or a targeted nostalgia substitution must not be forbidden by an old showcase quota.|
|Five consecutive strategies / repeated species across neighboring teams|ADVISORY, not blockers|The same audit includes such quotas around 1029–1046. Judge actual battles and their context. Identical labels can hide different puzzles; different labels do not prove variety.|
|Broad competitive-quality heuristics|ADVISORY|`audit_battle_set_quality.py` already describes optional heuristics. Lower power, specialized investment, unusual moves or an intentional risk cannot be rejected without team context.|
|Exact old prose, NPC totals, screenshot hashes and historical corpus floors|RETIRE unless they protect a distinct current failure|Render new text and inspect changed scenes for actual layout/access problems. Do not require old wording or revive a deleted test corpus.|
|Existing working Room, Instruct, Sitrus, Palafin, item and party regressions|KEEP relevant cases|Audit fixture quality first, then preserve observable behaviors that are still required. Avoid duplicating equivalent tests around every trainer ID.|

The snapshot release path has already been reduced. Its current deterministic gates focus on source/materialization/structure; the existence of broader standalone scripts does not mean their every assertion should be reinstated as a release condition.

### TEST-02 — Small evidence sets with independent value

Use shared cases for shared behavior, with the actual affected trainer scenarios as integration evidence. The following groups each protect a distinct failure mode:

- **Opening and persistent transactions:** two different starters inserted atomically;54 selection outcomes; pre-rival supplies; partial/full PC; legacy migration; second-starter Mega receipts and Steven reappearance. The 54-case matrix covers the actual current selection domain, not a permanent future test-count floor.
- **Levels and Points:** Medium floor at gyms/routes/revisits; source stat formula; nature/priority/Speed; a precise timing repair and actual berry thresholds. Fixtures specify their stats and do not accidentally bypass native calculation through testing-only fixed-stat flags.
- **AI state correctness:** one switch-entry hazard/self-effect application, candidate-order independence, truthful first-hit Berry damage, actual base/Mega simulation and independent owner budgets.
- **AI action selection:** a useful support pair, a disrupted setup, deliberate ally activation, an ace needed early, Palafin before its last switch opportunity, and a replacement that must survive both opponents. Do not count forced scripted moves as autonomous AI success.
- **Acquisition and reward state:** a rejected Sweet Scent draw cannot launch a stale enemy party; exact Galar bird identities; one-save irreversible evolution alternatives; independent gift/stone/prize delivery after full-bag/PC failure; no free protected-relic bypass.
- **World and format:** mandatory story handoffs, two-starter rescue choreography, no native trainer singles entry, local Tent returns, Hill timer/party cleanup, and reachable pending reward NPCs.
- **UI and source binding:** readable two-name starter confirmation and relevant menus, correct displayed stats/methods, and the specific ROM/ELF/input stamp used for any runtime result.

A useful implementation test should be able to fail for the behavior it protects. Prefer production functions and real battle/field paths over a second implementation that simply reproduces the proposed answer. Test-build bypasses are explicit: for example `CanMegaEvolve` skips the player's Ring check under TESTING, so that environment cannot prove the production no-Ring gate by itself.

### TEST-03 — What book validation does and does not prove

The book's inventory, source-link, no-overlap, shape and coverage checks establish that the document accounts for its frozen input and that its proposed records are internally consistent. They are not new game gates.

The source snapshot contains 516 authored branches,540 Hoenn maps,138 wild-header map IDs and 99 stone items. Those counts are useful for detecting an omitted page in this book. A future creative revision can legitimately change any inventory size. The document must update with that revision rather than constrain it.

The book was written without a ROM build or source implementation. Mathematical Points benchmarks are calculations; historical tests retain their own source boundary; source analysis is not a campaign playthrough. Actual implementation still needs the stated new evidence. Do not describe a catalogue export, a static pass or a traversal harness that auto-wins battles as proof that every fight is challenging, optimal or fun.

### Implementation reporting

For each patch, report the intended behavior, exact changed scope, what evidence was obtained, what failed and why, and what the next check establishes. Once relevant checks pass, move forward; do not repeat unchanged suites without a new reason. Design review, runtime battle quality, fresh-save traversal and release/artifact integrity remain distinct evidence.
