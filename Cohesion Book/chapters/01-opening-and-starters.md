# 01 — Two partners, one complete opening

**Disposition: REVISE.** This chapter implements the user's final opening decisions in instructions only. Choose two different starters from one regional trio, rescue Birch in a genuine two-against-two scripted wild battle, keep both Pokémon, and meet a four-Pokémon doubles rival whose starter is the third, unchosen member. Ordinary wild encounters, legendary captures and Wally's scripted capture tutorial remain singles. All trainer competitions remain doubles, with at least four opposing Pokémon in an independent standard team and the shared live-cap level floor.

The rival's first pair is the unchosen regional starter plus Pikachu. Shroomish and Taillow are its two reserves. Complete Hoenn branches are in [the early battle volume](../battles/early/0001-0020.md), and the exact regional starter slot for all 27 possibilities appears below. This chapter supersedes every historical single-starter opening and single-rival exception elsewhere in the baseline.

## What the current source actually does

| Source | Current behavior | Required change |
|---|---|---|
| [Regional choice](../baseline/source/data/scripts/emerald_champions.inc:616) | A nine-region menu chooses the bag's trio; dialogue promises a rival with the type advantage against one choice. | Retain the region menu and the three familiar bag balls, but promise two partners and the remaining rival starter. |
| [Starter chooser](../baseline/source/src/starter_choose.c:620) | The first YES returns one index immediately. | Store the first provisional index, choose a distinct second, then confirm the pair. |
| [Grant callback](../baseline/source/src/battle_setup.c:1006) | Writes `VAR_STARTER_MON`, gives one level-5 Pokémon, and applies raw doubles preset 0 to party slot 0. | Prepare and commit two permanent Pokémon before any battle controller reads the party. |
| [First-battle flag](../baseline/source/src/battle_setup.c:1020) | Sets only `BATTLE_TYPE_FIRST_BATTLE`. | Set `FIRST_BATTLE | DOUBLE` for this Hoenn scene. Do not add `TRAINER`, `MULTI`, or a global wild-doubles rule. |
| [Enemy creation](../baseline/source/src/battle_controllers.c:154) | Overwrites the first encounter with one level-2 Zigzagoon. | Build the two exact prepared rescue foes below. |
| [First-battle AI](../baseline/source/src/battle_ai_main.c:6848) | The special beginner routine flees when the player reaches 20% HP. | Use the shared expert doubles evaluator; remove this mercy behavior from the Hoenn rescue only. |
| [Bag script](../baseline/source/data/maps/Route101/scripts.inc:218) | Sets rescued/Pokémon flags before battle, then always proceeds to success and the lab. | Commit ownership after successful pair creation and rescue progress only after actual victory. |
| [Lab receipt](../baseline/source/data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc:133) | Acknowledges/nicknames only party slot 0. Dex and five Balls come after the rival. | Acknowledge both existing partners, provide Dex and 20 Balls before Route103, and preserve the later story debrief without duplicate gifts. |
| [Rival dispatch](../baseline/source/data/maps/Route103/scripts.inc:38) | Switches on the single player's choice and calls singles trainer battles. | Dispatch by the unchosen third starter and use four-Pokémon doubles. |
| [Regional replacement](../baseline/source/src/battle_setup.c:2387) | Computes `(first + 1) % 3`, with a singles-preset exception and first-compatible doubles fallback. | Compute `3 − first − second`; retain exact starter stage and a coherent legal doubles loadout. |
| [Archive exclusion](../baseline/source/src/field_specials.c:257) | Excludes only the first original starter. | Exclude both immutable initial choices. |
| [Starter stones](../baseline/source/src/field_specials.c:555) | Supports up to two stones for one family, including Charizard X/Y. | Use four stable family/stone receipt slots covering both families, with at most three actual stones for current two-starter regional pairs. |

These are source findings. No opening scene or proposed rescue matchup has been run in an emulator during this documentation pass.

## Selection and permanent ownership

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

## Save state without expanding the save layout

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

Advance the existing core save version from 3 to 4. The current [migration](../baseline/source/src/overworld.c:2409) returns early for version 3; modifying the version constant alone would also stop recognizing those saves and can send them into the ambiguous-state reset path. Explicitly preserve v1/v2/v3 migration behavior, then initialize the two new vars and stamp v4 through a common completion path. Never stamp v4 before its state conversion is done.

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

## The rescue battle

Use a scoped `IsEmeraldChampionsBirchRescueBattle` predicate combining the Hoenn first-battle context with the new pending opening state. This predicate selects enemy creation, expert AI, capture/bag restrictions, restart reconstruction and outcome routing. Reuse `FIRST_BATTLE` for the existing narrative presentation and no-running behavior, while adding `DOUBLE`; do not introduce a global rule converting wild encounters into doubles.

| Foe | Level | Item | Ability | Nature | Points HP/Atk/Def/SpA/SpD/Spe | Moves |
|---|---:|---|---|---|---|---|
| POOCHYENA | 5 | FOCUS_SASH | RATTLED | JOLLY | 2/32/0/0/0/32 | CRUNCH, PLAY_ROUGH, SUCKER_PUNCH, HELPING_HAND |
| ZIGZAGOON | 5 | SITRUS_BERRY | GLUTTONY | ADAMANT | 2/32/0/0/0/32 | BELLY_DRUM, EXTREME_SPEED, SEED_BOMB, PROTECT |

These two opponents have no trainer reserves: this is the specifically authorized wild rescue, not an exception to standard trainer party sizes. Their level 5 is explicit scene calibration, not an application of the trainer cap floor. The player also begins with two fully prepared level-5 Pokémon, and can inspect/change their preparation through the bag menu before committing to combat.

The rescue enemy constructor zeros the enemy party once, fills slots0and1 with the complete prepared values, and updates party counts before controller initialization. Do not call the old one-mon constructor twice and overwrite the first foe. The pair has a real support/setup threat with immediate attacking alternatives. The AI must evaluate Belly Drum survival, immediate Sitrus recovery, Helping Hand timing, Sucker Punch eligibility, priority blocking and available targets. It must not always set up on turn one or always attack the first starter. Remove the current `AI_FirstBattle` automatic flee at low player HP; do not replace it with another hidden mercy rule.

Both wild Pokémon appear in the field scene. World proposal `W-INTRO-GEOMETRY` appends Poochyena at (6,14), elevation 3, facing up, with the existing rescue hide flag. After the current chase completes, it steps to (6,13) and faces left; Birch ends at (4,13), Zigzagoon at (5,13). Remove both foe objects during the bag's existing fade before relocating the player to (6,13). This is temporary scene removal, not a permanent success flag. On a field return after loss, while faded/locked, place the player at the verified clear standby (7,15), facing the bag at (7,14), then restore Birch/Zigzagoon/Poochyena to (4,13)/(5,13)/(6,13). Keep route state 2 and the bag visible before fading/releasing. Only victory commits permanent success hides and the lab warp. The world volume owns trajectory evidence and geometry validation.

Capture and ordinary Bag actions are unavailable in this scripted rescue, including with externally injected Balls. `WE_FLAG_NO_CATCHING` is configured as 0 in the snapshot; do not set flag zero or change the global wild config. Add the scoped rescue predicate to the appropriate Bag/ball eligibility check. This restriction also makes the existing party-only Retry snapshot sufficient: there is no Bag transaction to roll back.

Extend [the current Retry snapshot](../baseline/source/src/battle_main.c:484) from eligible trainer fights to this exact rescue context. It snapshots both permanent partners after preparation, including their held items, PP and HP. At retry teardown, use the rescue enemy constructor instead of `EmeraldChampions_RebuildTrainerBattleParties`, restore the player snapshot, and re-enter battle initialization. Never pass through `CB2_GiveStarter` or the gift transaction on Retry. No trainer reward, defeat flag, new Ball or duplicate starter is produced by a failed attempt.

On loss, the ending callback must not fall into the old unconditional success script. Return to the pending rescue menu with restored partners: **Retry / Prepare pair**. A player may change their build, then take a new battle snapshot. The scene remains pending. Only a true victory commits state 2 and the rescue success flags, heals both partners, hides both foes/bag and warps to the lab.

The proposed rescue's empirical difficulty is not yet established. Acceptance requires winning lines for all 54 ordered choices, including the support-heavy combinations, with the documented preparation options and actual points formula. If a pairing fails, audit fixture preparation, legality, action rules and AI behavior before changing levels or weakening the puzzle. Do not claim a story-critical forced pair is viable merely because a different starter pair wins.

## Before the first rival: choice must already be practical

At the first lab receipt, Birch acknowledges both existing partners. Offer naming for each through the existing naming service using its actual stored slot/location; declining one name does not skip the other. Neither is regenerated. Then deliver the Pokédex and a packet of **20 Poké Balls** before sending the player toward Route103.

After acknowledging and offering names for both partners, set the **old `VAR_BIRCH_LAB_STATE` to 3 before any supply failure can release control**. The old lab-state-2 OnFrame trigger must no longer fire. Keep the **new `VAR_EC_OPENING_STATE` at 2** until the early supplies are complete; these variables own different duties. Add an opening-state-2 check before the ordinary Birch and aide dialogue so either interaction resumes the pending supplies. A full Bag/PC must allow the player to leave, free storage and return without another automatic lock. Route103 continues to require new opening state 3.

Call the existing `ReceivePokedex` functionality once, preserving its National/Dex flags and setup. Handle the Ball grant as a resumable transaction: if all 20 fit in the Bag, deliver once; otherwise try the PC item store. If neither store can receive the packet, keep state 2 and explain that the supplies are waiting. The player can exit and free storage, then resume with Birch or the aide. Do not mark the opening ready, repeat a successful packet, or permanently lose it because a fanfare/menu was interrupted. Set state 3 immediately after both receipt conditions succeed, before presentation that yields control.

Move the existing five-Ball post-rival reward into this earlier supply packet rather than stacking it unnoticed. The later state-4 lab event becomes a short debrief that retains the existing `FLAG_ADVENTURE_STARTED`, Oldale/Littleroot state changes and rival relocation. It must not give another Dex or Ball packet.

Keep ordinary catching available on accessible opening grass before the rival. The Route101 and Route103 encounter tables remain owned by the global distribution chapter. Route102 access still follows the existing story blocker; do not open the rest of Hoenn by advancing `FLAG_ADVENTURE_STARTED` early. Existing unlocked grass must already provide enough catchable options to build four to six Pokémon.

Oldale's Center already has a battle vendor at `(2,2)` and move tutor at `(13,2)`, both without hide flags. Retain them. The move tutor already defaults to doubles preparation; the vendor already provides ordinary held items before the Ring. The nurse already provides the Leveler, Poké Vial, Repel Spray and Flight Beacon. Verify all these menus are reachable after the early Dex grant and before the rival, without adding redundant NPCs.

Add a small opening-only Ball replenishment branch to Birch's aide or the existing Oldale Mart employee: while the Route103 rival is undefeated, if the player has fewer than 10 Poké Balls across Bag and PC, top up the Bag toward 20 with a clear “field supplies” message. Do not take money, create other item types or advance story. This is a deliberate convenience supply, not a once-only valuable-item reward. After the rival win, retain normal acquisition/economy. The player can therefore recover from failed captures instead of being stranded with two starters and no way to experiment.

Route103 first requires opening state 3 (or an already-completed compatible legacy state); partial Dex/supply delivery cannot start the challenge. Then it presents **Battle / Keep preparing** before starting. The battle requires the engine's existing two usable battlers; the player is allowed to attempt it with two excellent starters or build a larger team. The opposing team always has four. The shared Normal/Medium floor applies even if the authored cap offset is negative. Retry and Reload Save remain separate actions with separate meanings.

## Rival identity through the whole campaign

Validate `first ∈ {0,1,2}`, `second ∈ {0,1,2}` and `first != second`, then compute `unchosen = 3 - first - second`. Resolve species through `GetStarterPokemonForGeneration(unchosen, generation)`. This formula works for all six ordered choices in each region and never selects either owned starter.

Historical trainer identifiers remain stable. Their suffixes describe the old player's choice, so derive the existing dispatch selector as `(unchosen + 2) % 3`:

| Unchosen family | Existing selector | Existing suffix | Actual Hoenn starter |
|---|---:|---|---|
| Grass | 2 | `*_MUDKIP` | TREECKO line |
| Fire | 0 | `*_TREECKO` | TORCHIC line |
| Water | 1 | `*_TORCHIC` | MUDKIP line |

Replace the single-choice switches in Route103, Route104, RustboroCity, Route110, Route119 and LilycoveCity with a call to this one shared selector. Do not independently invent six different arithmetic expressions. Both genders use the same result. The later branch's existing reserve variant follows the unchosen starter, not merely the player's first selection.

`ApplyRegionalRivalStarter` must identify the unique authored Hoenn starter slot before mutation, retain its stage, level/offset and position, then replace that slot only. Opening slot 0 uses the exact regional table below. Mid-stage encounters use `GetMiddleEvolutionForStarter`; late encounters use `GetFinalEvolutionForStarter`. Validate exactly one replacement target in each rival template; unrelated Hoenn partners and Mega holders remain untouched.

The Lilycove audit specifically retains **Mega Metagross** for the Grass-starter branch, **Mega Salamence** for Water, and **Mega Tyranitar** for Fire, with the ordinary regional starter in slot 6 at offset +3. A regional starter preset must not steal the fixed partner's Mega allocation. Exclude required-item/required-move Mega/form presets from this replacement path. Use the exact [54 stage profiles and 270resolved variants](../battles/early/regional-rival-profiles.md), including their specified held items. Opponent item uniqueness is an authoring convention, not a general player Item Clause. These rows resolve current conflicts while protecting strategy-dependent items, rather than stripping an item or taking an arbitrary first-compatible preset at runtime. Their [complete machine-readable variants](../review/opening-regional-rival-profiles.json) preserve every other party member and the authored starter slot/offset.

The six Hoenn source branches remain complete authored teams; the runtime regional substitution matrix is an additional obligation. Its acceptance must include all regions and stages, not only the Hoenn templates. The broader battle book owns any later exact roster improvement; this chapter owns the mapping and invariants and rejects undefined, illegal or itemless error fallbacks.

## Both starter families receive their actual rewards

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

## Exact story text to implement

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

## Acceptance and implementation order

1. **Source/state contract:** reserve the two approved vars, implement v4 migration safely, and keep saved-structure sizes/checksum boundaries unchanged. Test clean new game, v3 pre-rescue, v3 post-rescue/pre-rival, v3 post-rival, invalid retired var contents, full party/PC and a recovered redundant save slot. For full item storage after pair acknowledgment, assert old lab state3/new opening state2, free player movement for several frames, successful lab exit/re-entry, and one resumable supply handoff through either Birch or the aide. Re-running migration must change nothing after the first successful pass.
2. **Chooser and gift:** exercise every one of the 54 ordered choices, duplicate rejection, B/back, both confirmation refusals, failed preparation, unexpected capacity failure and re-entry after state 1. Assert exactly two permanent gifts, distinct families and stable IDs across retry/load. Verify no invisible extra third Pokémon or unauthorized held-item replacement.
3. **Rescue mechanics:** two active starters versus the exact two foes, expert AI with no low-HP flee, unavailable capture/Bag even with injected items, normal ordinary-wild formats afterward, both possible first faintings, loss, Retry, preparation after loss and victory. Snapshot restoration must return PP/items/HP without duplicating Pokémon or skipping the rescue.
4. **Actual puzzle viability:** test all 27 unordered starter pairs with both orderings, the exact points formula and legal preparation. Record at least one reproducible winning line for each, including support-heavy pairs; test alternate credible lines on representative pairs. This is runtime evidence still required, not a claim from the 54-row arithmetic table.
5. **Pre-rival access:** before Route103, capture additional Pokémon, use the Dex, obtain the Leveler, prepare nature/ability/points/moves/items, heal, use the PC and recover from failed catches through the Ball supply. Verify none of the later story blockers are accidentally lifted by the early Dex flags.
6. **Rival matrix:** both identities ×54 choices ×difficulty levels. Check four opponents, doubles format, unchosen species, exact companions, no item duplicates and the live Normal/Medium floor. Exercise Retry and a deliberate return to preparation without marking victory. Later milestone fixtures verify the proper evolved unchosen starter and preserve fixed Mega partners.
7. **Rewards:** all two-starter Mega combinations including Charmander first and second, families without Mega Stones, Bag-full/PC-free, both full, one already-held stone, an old delivered bit with the item discarded, and one pending second-family stone. Verify stable receipts and no premature Steven departure. Also test a previously hidden v3 Steven after a real supplemental second gift, no wake-up while that Pokémon is still owed, no-Mega/fully-receipted second families, and no repetition of letter/Ring/first-family gifts on the resumed handoff.
8. **Narrative/traversal:** render the two-choice screen, four-battler rescue, both visible wild actors, both name prompts, early Dex/balls, Oldale preparation, rival exit and post-rival lab scene. Confirm object positions, path blocking, locks/releases, fades/music and map state after save/load at every released boundary.

Implement this chapter before applying the six E0001 team changes in isolation: switching the rival to four Pokémon while withholding the player's second starter or capture tools would create an incoherent intermediate release. Merge the opening transaction, relevant world choreography, supplies, regional resolver and first-rival materialization as one reviewable feature slice. The rest of the book can then rely on the new immutable two-choice state.

The accompanying [machine-readable specification](../review/opening.json) contains all 54 exact pair/foe mappings and the initial/rival loadouts. Its static checks pass index distinctness, rival exclusion and exact initial held-item preservation. Narrower pinned-reference move differences are listed and deliberately preserved under the expanded native policy. It does not certify runtime battle difficulty or scene execution. The [Stat Point appendix](../battles/early/points-benchmarks.md) supplies exact opening and gym speed/point calculations, clearly separated from runtime battle proof.


## Complete initial player loadouts

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

## Complete regional Route103 starter slot

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

## Ordered choice truth table

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
