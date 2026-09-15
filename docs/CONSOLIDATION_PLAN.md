# Emerald Champions — consolidation and baseline plan

Status: in execution, September 15, 2026. Phases 0, 1 and 3 landed the same day; see docs/CONTINUE.md. Once the user approves the decisions in
section 1, Phase 0 folds this file into `docs/CONTINUE.md` and the book, and this
file is deleted. It is not a second handoff.

## 0. What this game is (the sentence every rule must serve)

Emerald Champions gives the player Pokémon Champions-style freedom to build any
legal team, then runs them through the hardest, most interesting competitive
doubles campaign ever built: every fight authored like a top VGC team, every
Mega represented on a trainer, six-member Gym rooms, and no grind between fights.

Everything that is not a battle exists to do exactly one of three jobs:
deliver new species/Megas to the player, tell the story that strings the fights
together, or get the player to the next fight without friction. Anything that
does none of those is cut. This is the test applied to every item below.

Protected (not up for reduction): the 341-encounter roster, doubles everywhere,
four-to-six member parties, six-member Gym personnel, all 99 stones on trainers,
two-Mega bosses, shared AI depth, levels-only difficulty with the existing
formulas, free build editing at the Leveler.

## 1. Decisions needed before Phase 2 (recommendation first)

| # | Decision | Recommendation |
| --- | --- | --- |
| D1 | Mandatory legendary set | Keep as mandatory only the encounter that is the climax of its own chapter: Moltres, Landorus, Southern Island Lati, Mew, Heatran, Diancie, Jirachi. Deoxys, the three Regis, Regigigas become optional with rewards. |
| D2 | Legendary encounter format | DECIDED: stay singles (user choice). Each legendary scene gets an authored set, item and spread at a cap offset instead of a random wild set. Trainers already field 77 distinct legendary species across 65 groups; that is untouched. |
| D3 | Cap schedule | Collapse 23 milestones to 15: one per badge, one per act climax. Draft: 14, 20, 24, 30, 36, 42, 48, 54, 60, 68, 76, 84, 90, 96, 100. Offsets are re-seeded anyway; calibration is pending everywhere. |
| D4 | Story order between Groudon and Kyogre | Tighten to: Mt. Pyre → Magma/Groudon → Heatran → submarine theft → wreck/Scanner (Stern's immediate response) → Aqua Hideout → Mossdeep Gym → Space Center → Seafloor → Kyogre → Sootopolis → Sky Pillar → Juan. Birth Island, Sealed Chamber and Regis move to the post-Juan "origins" act beside Diancie/Jirachi. |
| D5 | Sandstrewn collapse | Keep the collapse as drama. Move the unique progression content out of the sealed area: Zygarde, Stakataka, Garchompite, Golurkite, Flygonite, unique wild families (Trapinch, Honedge lines). Sealed area keeps only duplicated wild entries and fossils. |
| D6 | Money lane | Money stays, but small and narrow: it buys evolution items, Balls, cosmetics. Nothing else. Stipends scaled to that. |
| D7 | Coin species | Poipole and Genesect move from 130k/150k Coins to Circuit BP mastery rewards. Battle skill is the currency that fits the pillar. Coins keep starters and dolls only. |
| D8 | Real-time and chore gates | REVISED (user, September 15): keep the soot, berry-harvest and Shoal economies as they are. They are optional, run in parallel with the campaign, and their rewards are late-family Megas or a mythical discovery, so they are hobbies with a real payoff, not gates. Only the duplicate cash substitutes change (equal to the item's price). The book must describe them as optional collections, never as requirements. |
| D9 | Rematch payouts | Return fights pay zero money (code), so "cannot be farmed" is true in the binary, not just the book. |

All of D1 and D3–D9 are accepted as recommended (September 15). Additional user
directions added the same day:

| # | Direction | Where it lands |
| --- | --- | --- |
| D10 | Mom gives the Old Rod together with the Running Shoes. Dewford's fisherman keeps his tip but never issues a second rod. Implemented September 15 (Littleroot send-off, Mom-at-home full-Bag retry). | Done |
| D11 | Unify all NPC dialogue: one voice, one direction style, rewritten against the new spine. | Next: gate order is final as of September 15 |
| D12 | Rewrite the book and guide from the new build rather than patching the current text. | Phase 4, after Phases 1 and 3 land |
| D13 | Run and Bun quality of life: key items register to L, R and Select, with a visual indicator in the Bag and no save-block offset shifts. Implemented September 15 in the unused Dex-flag filler of SaveBlock1; L inactive under L=A; R falls back to DexNav. | Done, pending native visual check |
| D14 | Process: Sonnet subagents implement scoped code tasks; the main agent reviews every diff before it lands. | AGENTS.md |

Difficulty note (September 15): `src/difficulty.c` applies Easy −4, Medium −2,
Hard 0 relative to authored offsets since commit 51f2cdb687. The book text must
say the same; an uncommitted edit reverting it to −2/0/+2 was discarded because
code is ground truth. If the intent was to undo the level cut, change the code.

## 2. Phase 0 — one baseline, one truth (do first, 1–2 days)

Goal: every fact lives in exactly one place, and the build fails when copies drift.

1. **Single clone.** `Projects/Pokémon Emerald Champions/game-source` is the
   repo. Confirm `emerald-champions-restored-20260914.YWDScx` and both Desktop
   `web-source`/`legacy-workspace` clones have no unique commits, then delete
   them. README stops carrying hand-typed counts.
2. **Generated numbers only.** Encounter/group/variant counts, cap and stipend
   table (read from `src/caps.c`), shop SKUs and prices (read from item and shop
   data), the cut ledger, the Mega register and the acceptance ladder (section 4)
   are emitted by `scripts/sync_game_book.py` into the book's generated
   reference and into a README status block. Hand-typed copies are deleted.
3. **Consistency gate.** New `scripts/check_book_consistency.py`, wired into the
   release gate: README, CONTINUE, book header and trainer-review-index must
   agree; every price quoted in the guide must match `items.h`; every retired
   item must be absent from shop, prize, Pickup and gift tables; every Mega Stone
   must have exactly one world source and at least one trainer owner.
4. **Known contradictions to close in this phase.**
   - 343/325/370 (README) vs 341/324/368 (guide, index).
   - Linking Cord: 3000 in `items.h` and the rule; 10000 recorded in the
     Slateport native check. Find the actual Slateport price and fix one.
   - "A 20,000 Mega costs 40 stones" vs "Mega Stones have no shop shelf."
   - Soot Cord substitute 5,000 vs Cord price 3,000. Rule: any cash alternative
     equals the item's price, or the alternative is removed.
   - BP section duplicated verbatim in the guide.
   - "Optional return fights scale with the live cap" vs "payouts cannot be farmed" (D9).
   - Chapter numbering: C03 before C02; C06/C11/C15/C21/C33 are not chapters.
   Closed September 15: README/CONTINUE/guide census now 341/324/368 from
   source; Cord is 3000 everywhere including Slateport (the 10000 note was
   stale); "20,000 Mega" sentence, 5,000 soot substitute, duplicate BP and C13
   blocks removed; difficulty text matches `src/difficulty.c`; rematch payouts
   stated as zero and implemented. `scripts/check_book_consistency.py` runs in
   the release gate (advisory until `--strict-book`).
   Open (design decision, user): **Garchompite has no trainer holder** since
   Mega Garchomp left Drake; choose its sand-team showcase in the Gym/story audit.
5. **Book diet.** The hand-authored guide is current rules only. Move every
   dated evidence paragraph (C14 native evidence, Laura benchmarks, Slateport
   mart steps, "September 14 user direction") into `docs/VERIFICATION.md` or the
   generated acceptance ladder. Strip dates and "approved" language from rules;
   git history is the approval record. Target: guide under 15,000 words, one rule
   per topic, stated once. The 12 MB book remains the generated reference.

## 3. Phase 1 — campaign structure (decisions D1–D5, then scripts/flags)

1. **Acts.** Renumber chapters sequentially in story order and group them:
   - Act 1, Stone to Balance: Roxanne, Dewford/manor, museum, Wattson, Chimney,
     Flannery, Ashen Woods/Moltres, desert safeguards, Norman.
   - Act 2, The survey: New Mauville, Weather Institute/Landorus, Winona,
     Lilycove, the voyage (Lati, Mew).
   - Act 3, The crisis: the D4 order, ending at Juan.
   - Act 4, Origins and the League: Birth Island, Sealed Chamber/Regis (optional
     per D1), Diancie, Jirachi, Steven's conclusion, Wally, League.
   - Post-game: Circuit, exhibitions, remaining captures.
2. **Caps** per D3, implemented in `src/caps.c` and mirrored by the generator.
3. **Gates.** Story-gate enforcement stays; the flag chain is re-pointed to the
   new order. Each moved chapter's handoff dialogue is rewritten to name the new
   next destination. Native walk of the whole spine with the story-gate fixtures
   before any trainer work resumes.
4. **Legendary doubles bosses** per D2: authored in
   `emerald_champions_battle_teams.txt` like any trainer, with the same review
   questions (Stats, Could it be cooler, Overrepresented, Counterplay).
5. **Missables** per D5. The collapse warning text shrinks to fossils and wilds.
6. **Difficulty labels.** Keep levels-only ±2. Rename to reflect reality:
   Medium is the designed game; Easy is a two-level handicap, not an easy game.
   The new-game text says so.

## 4. Phase 2 — battles: keep everything, add the pipeline

The roster, formats and Mega coverage are protected. What is missing is the
machinery that turns 341 designs into 341 calibrated fights.

1. **Acceptance ladder**, one row per encounter, generated into the book and the
   index, replacing per-trainer "calibration remains pending" prose:
   - L0 authored in the teams file, materialized
   - L1 AI forecast fixtures pass for the team's engine
   - L2 native benchmark against the cap window's reference parties
   - L3 earned clear in the campaign save
2. **Reference parties.** For each cap window, define two or three strongest
   legal player teams from the available pool (the `c15_available_pool` method,
   extended to every window). Seeds are tuned against what a strong player
   with full freedom actually brings, which is the premise of the game.
3. **Mega register**, generated: for each of the 99 forms, its signature owner
   (the boss or ace whose fight is the showcase), secondary owners, stone world
   source, and whether the family is obtainable by the player at that point.
   The register closes the four unowned-form gaps and the Rayquaza/Steven
   question by making them visible rows, not prose.
4. **Order of work** stays major-battle-first: League, whole Gyms, Magma/Aqua,
   legendary bosses (new), then the rest. Each block advances to L2 before the
   next block starts; L3 comes from the resumed earned run.
5. **Earned run.** Resume the paused C15 save after Phase 1 lands, on the new
   spine. It is the only source of L3.

## 5. Phase 3 — economy (D6–D9)

1. **Income.** Starting cash plus milestone stipends plus first-clear prize
   money. Rematch prize money is zero (D9). Defeat still costs nothing.
2. **Sinks.** Evolution stones 500, tools 1,000, Cord 3,000 (one price
   everywhere), Balls at current prices, cosmetics. That is the whole shop.
3. **Stipend rescale.** Size the schedule so a player who buys every evolution
   item they could want by the League has a modest surplus, not six figures.
   Measured September 15: campaign prize money is `cap × prizeMultiplier` per
   first clear (`src/battle_setup.c`, `GetCampaignBattleMoneyReward`), with
   multipliers 20 (most), 40 (aces), 100 (leaders), 150/200 (League). Summed
   over the roster at typical caps that is roughly 600,000, plus 223,000 in
   stipends, against about 45,000 of purchasable evolution items. Working
   target: total campaign cash about 100,000 to 150,000 including prize money;
   the cleanest lever is the multipliers (20/40/100 → 5/10/25) plus dropping
   most stipends to 0, since the stipend table is already the cap table.
4. **Coins** keep starters and dolls; Poipole/Genesect move to BP (D7).
5. **Remove timers and counters** per D8. Berry growth, tides and soot no longer
   gate any Mega Stone or legendary.
6. **Duplicate handling.** Shops refuse owned reusable items and stones (already
   built). Gift duplicates give the item's price in cash, nothing more. Delete
   every other special-case substitute.
7. **Retired economies** become a generated register with a build check
   (Phase 0 item 3), replacing the prose list.

## 6. Phase 4 — docs steady state

- `Game Blueprint/Emerald_Champions_Game_Book.txt`: short guide plus generated
  reference. Guide sections: 0 the sentence, 1 experience and battle rules,
  2 player rules and caps, 3 the campaign by act, 4 rewards and economy,
  5 world and scenes, 6 keeping the book honest.
- `docs/CONTINUE.md`: current state and the next three blocks only.
- `docs/VERIFICATION.md`: commands and evidence.
- `docs/trainer-review-index.json`: the ladder.
- README: generated status block, links.

## 7. Order and effort

| Phase | Depends on | Effort |
| --- | --- | --- |
| 0 baseline | nothing | 1–2 days |
| 1 campaign | D1–D5 | 3–5 days of scripts, flags, dialogue, spine walk |
| 2 battles | Phase 1 for L3; L0–L2 can start now | the long pole, ongoing |
| 3 economy | D6–D9 | 2–3 days |
| 4 docs | Phase 0 | folded into each phase's change |
