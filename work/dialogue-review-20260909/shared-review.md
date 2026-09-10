# Shared dialogue review — September 9, 2026

Completed a sequential reading of all **3,468 extracted blocks across 68 assigned files**, plus **four conditional berry-growth blocks and one debug font-test block omitted by extraction**. All conditional PC-transfer continuations were also read. Total labeled coverage: **3,473 blocks**. This is source review, not an emulator playthrough.

**90 blocks changed in 14 files.** No mechanics, scripts, trainer parties, rewards, or gates were altered. Comparing each owned file with its before snapshot after removing text directives and comments produced no non-text differences. The only non-dialogue edit is the comment above Birch’s introduction.

## Substantive revisions

- Birch now welcomes the player into HOENN’s changing habitats and a journey with two partners. Free tutoring, EV editing, Battle Sets and the Leveler remain clear, while the intimidating mechanics lecture and Reload/retry conflation are removed. Detailed tools remain explained at the first Center.
- The first Center explanation now calls out battle held-item supplies accurately, keeps free healing/refill, reusable Leveler and 500-step Repel behavior, and preserves the existing separate optional healing offer.
- Scott’s late-campaign Match Call now directs the player to the six-badge Lilycove ferry/Circuit instead of promising a facility only after the Hall of Fame. Drake recognizes Circuit and League as distinct current challenges. May/Brendan treat Mt. Pyre’s ghosts as part of its memorial rather than an infestation.
- All sixteen active apprentice level-choice personalities now describe their own practice plans, rather than inviting the player into retired Tower Level50/Open rooms. The karate apprentice no longer assumes the player is League Champion.
- Canonical party review found obsolete tactical claims: Donald had no redirection or sleep; Kent no longer had the implied Shedinja; Yasu lacked the promised type changes and stat resets; Takashi had no sand setter; Fabian had no Lightning Rod; Jennifer lacked Shadow Tag/Bad Dreams. Those hints now fit their actual party roles and personalities. Brooke’s actual lead is Raikou beside Infernape, so its incorrect fixed lead/finisher narration became flexible partner-role language. Marley’s slow Escavalier is no longer described as lightning-fast.
- Removed unsupported claims about precisely how the player won (never touching Cofagrigus, making no wasted turns, exploiting every reflection, etc.). Defeated trainers acknowledge losing without inventing the player’s actions. Braxton no longer assumes badges remain to earn on a revisit.
- Leonel’s Route120 clue no longer requires Tropius or activated grass marks; Ogerpon is a five-badge rare wild discovery and Sweet Scent can help. Existing Route105/120 Regi-ruin hints and literal island inscriptions were retained.
- Birch’s cancel reply now honors a declined review. His completed rating explicitly means the HOENN Pokédex, and the proportional rating row no longer asserts an exact100 count. Encouragement points toward revisiting habitats rather than requiring a trading partner.
- Tutor explanations distinguish prepared builds from separately acquired transformation items. A legacy move tutor offers a one-time lesson without claiming the move itself can never be relearned. Gift capacity wording correctly allows space in either party or PC.
- Secret Power clues cover compatible partners and all supported trees, bushes and cave indents. Wayne’s Waterfall comments describe field capability and the necessary Sootopolis badge. Safari’s northeast Pikachu seeker now gives the actual southern habitat.
- TV’s battle seminar now identifies its evidence as a damage comparison; a harder hit can be wrong for a capture or another tactical goal. The Secret Base TV narrator no longer awards Champion status based solely on an average-level threshold. Three contest commentary blocks give constructive feedback instead of declaring a loss worthless or an error unforgivable.
- Replaced a Japanese-only bookshelf fragment and the follower’s “ERROR404” fallback with ordinary exploration observations; the Southern Island sailor no longer calls the unique discovery island forgettable.

## Retained intentionally after consumer review

- The66 legacy Battle Tent blocks contain historic singles/rental/Nature rules, but all three public attendants and boards jump directly to the current six-member exhibition. Their strings were read and retained as inactive source; they are not proof of the live tent rules.
- Gym and ordinary Match Call rematch preparation/readiness strings remain as disabled variants. OW_TRAINER_REMATCHES is FALSE, and the active leader call selector explicitly chooses the retrospective fourth row. This was verified before deciding not to rewrite each obsolete branch.
- Three-member apprentice team-planning references describe the apprentice’s own practice and do not assert the player’s Circuit entry requirements. The Circuit and Tents require six; ordinary route doubles require two usable partners; link doubles require four. Those distinct numbers were preserved.
- Conditional BILL/LANETTE transfer messages, berry growth variants, NPC berry gifts, TV generated statistics, timed lotteries/news, contests, registered Secret Base legacy texts, event gifts, shop transactions, debugging labels and imported FRLG-only strings were all read. A debug IV label is a diagnostic of stored data, not a promised player progression feature.
- Broad NPC boasts, silliness, shyness, fishing frustrations, sibling dynamics, and “training/raising” as care or practice were preserved. They do not by themselves impose an IV/EV grind. Story-based legendary gates were not replaced by generic availability claims.

## Verification and limits

Checked text terminators, compared non-text source against the start snapshot, traced the consumers listed in shared-reviewed.json, and ran git diff --check only. No builds, emulator scenarios, battle tests, or new regression tests were run. Changed strings use explicit narrow lines and ordinary dialogue controls; this is not rendered visual verification.

Outstanding integration concern: the archive still contains inactive old-facility dialogue and logic. Removing that code is outside this text-only ownership pass; do not interpret its presence as live gameplay or silently revive it. Actual scene timing, screen wrapping with dynamic names, interactions after losing, and mixed-record TV deserve in-game checks when the parent task reaches playtesting.

## Per-file coverage

| File | Extracted blocks read | Extra conditional blocks | Changed labels |
|---|---:|---:|---:|
| data/text/abnormal_weather.inc | 2 | 0 | 0 |
| data/text/apprentice.inc | 288 | 0 | 17 |
| data/text/battle_tent.inc | 66 | 0 | 0 |
| data/text/berries.inc | 41 | 0 | 0 |
| data/text/birch_speech.inc | 9 | 0 | 1 |
| data/text/blend_master.inc | 15 | 0 | 0 |
| data/text/cable_club.inc | 94 | 0 | 0 |
| data/text/check_furniture.inc | 7 | 0 | 0 |
| data/text/event_ticket_1.inc | 8 | 0 | 1 |
| data/text/event_ticket_2.inc | 10 | 0 | 0 |
| data/text/lottery_corner.inc | 15 | 0 | 0 |
| data/text/match_call.inc | 317 | 0 | 7 |
| data/text/mauville_man.inc | 18 | 0 | 0 |
| data/text/move_relearner.inc | 18 | 0 | 0 |
| data/text/move_tutors.inc | 41 | 0 | 1 |
| data/text/obtain_item.inc | 16 | 0 | 0 |
| data/text/pc.inc | 7 | 0 | 0 |
| data/text/pc_transfer.inc | 8 | 0 | 0 |
| data/text/pkmn_center_nurse.inc | 22 | 0 | 1 |
| data/text/pokedex_rating.inc | 49 | 0 | 4 |
| data/text/pokemon_news.inc | 12 | 0 | 0 |
| data/text/questionnaire.inc | 8 | 0 | 0 |
| data/text/record_mix.inc | 2 | 0 | 0 |
| data/text/save.inc | 7 | 0 | 0 |
| data/text/secret_base_trainers.inc | 60 | 0 | 0 |
| data/text/shoal_cave.inc | 10 | 0 | 0 |
| data/text/surf.inc | 3 | 0 | 0 |
| data/text/trainers.inc | 1145 | 0 | 44 |
| data/text/tv.inc | 401 | 0 | 7 |
| data/scripts/aide.inc | 3 | 0 | 0 |
| data/scripts/apricorn_tree.inc | 7 | 0 | 0 |
| data/scripts/battle_frontier.inc | 1 | 0 | 0 |
| data/scripts/berry_blender.inc | 47 | 0 | 0 |
| data/scripts/berry_tree.inc | 27 | 4 | 0 |
| data/scripts/champions_tent.inc | 9 | 0 | 0 |
| data/scripts/config.inc | 1 | 0 | 0 |
| data/scripts/contest_hall.inc | 76 | 0 | 0 |
| data/scripts/day_care.inc | 32 | 0 | 0 |
| data/scripts/debug.inc | 70 | 1 | 0 |
| data/scripts/dexnav.inc | 5 | 0 | 0 |
| data/scripts/elite_four.inc | 3 | 0 | 0 |
| data/scripts/emerald_champions.inc | 95 | 0 | 2 |
| data/scripts/field_move_scripts.inc | 22 | 0 | 0 |
| data/scripts/flavor_text.inc | 30 | 0 | 1 |
| data/scripts/follower.inc | 4 | 0 | 1 |
| data/scripts/gift_altering_cave.inc | 1 | 0 | 0 |
| data/scripts/gift_aurora_ticket.inc | 4 | 0 | 0 |
| data/scripts/gift_battle_card.inc | 2 | 0 | 0 |
| data/scripts/gift_mystic_ticket.inc | 4 | 0 | 0 |
| data/scripts/gift_old_sea_map.inc | 4 | 0 | 0 |
| data/scripts/gift_pichu.inc | 2 | 0 | 0 |
| data/scripts/gift_stamp_card.inc | 1 | 0 | 0 |
| data/scripts/gift_trainer.inc | 2 | 0 | 0 |
| data/scripts/lilycove_lady.inc | 52 | 0 | 0 |
| data/scripts/mart_clerk.inc | 3 | 0 | 0 |
| data/scripts/mauville_man.inc | 148 | 0 | 0 |
| data/scripts/mystery_event_club.inc | 13 | 0 | 0 |
| data/scripts/pokemon_league.inc | 1 | 0 | 0 |
| data/scripts/profile_man.inc | 12 | 0 | 0 |
| data/scripts/repel.inc | 4 | 0 | 0 |
| data/scripts/route23.inc | 5 | 0 | 0 |
| data/scripts/safari_zone.inc | 44 | 0 | 1 |
| data/scripts/seagallop.inc | 1 | 0 | 0 |
| data/scripts/secret_base.inc | 22 | 0 | 0 |
| data/scripts/secret_power_tm.inc | 4 | 0 | 2 |
| data/scripts/shared_secret_base.inc | 4 | 0 | 0 |
| data/scripts/silphco_doors.inc | 3 | 0 | 0 |
| data/scripts/trainer_hill.inc | 1 | 0 | 0 |

The changed-label inventory and revised text are in shared-changes.json; exhaustive coverage and findings are in shared-reviewed.json. Original text is in shared-dialogue.json and source snapshots under before/.

## Final raw source reconciliation

Reconciled all four owner lists against their original reading corpora, then checked the current files for added or removed text labels. The exact corpus has 8,199 unique labeled blocks: main 1,755, npcs_a 1,382, npcs_b 1,589, and shared 3,473. No owner gained or lost labels during editing. The original corpora omitted zero raw `.string` lines for each map owner and 24 for shared: six LANETTE transfer continuation lines, nine berry-branch lines, and nine debug font-test lines. All 24 are now read. Two explicitly inactive `#if IS_FRLG` BILL-transfer lines are excluded from these raw reconciliation totals (they were also read). The debug pangram remains unchanged because its unusual language intentionally exercises glyphs. Exact paths, original line numbers, and full source lines are in raw-string-reconciliation.json.
