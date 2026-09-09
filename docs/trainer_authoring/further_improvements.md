# Further trainer improvements — main-chat backlog

This separate document preserves the suggestions attached to battle-completion messages. Update it after **every main-chat completion** so later opportunities are not lost. It is not a new test suite, an instruction to force particular moves, or a claim that every listed weakness is a demonstrated engine bug.

## Scope and provenance

The current cutoff is **Clark's completion, E0023, on 2026-09-09 at 13:04:37.801 UTC: 26/516 trainer branches, 20/468 fully completed encounter records, plus partial E0023 (Clark only)**. The original backfill covered all twenty records through Jose; Clark is the first incremental addition. E0016/E0022 are not invented missing completions. Johnson is a separate battle and remains unfinished, so E0023 is **not** counted as a completed grouped encounter record.

The requested thread is **Download newest Astra app version**, ID `01a08344-2f19-75c2-8b19-a05811d82e92`.

- `read_thread` was paginated to exhaustion: three pages, 27 turns, final `hasMore=false`. The first two pages returned ten turns each; the final page returned seven. All 26 later turns had empty `items`, including after requesting outputs and the maximum supported 20,000-character item limit. Only the original app-download turn contained message text. That API result alone could not establish the completion suggestions.
- Actual main-chat text was then recovered read-only from this thread's exact [local rollout](</Users/ericguthrie/.codex/sessions/2026/09/08/rollout-2026-09-08T18-04-38-01a08344-2f19-75c2-8b19-a05811d82e92.jsonl>). **P** below means that source; numbers are its one-based JSONL lines. Source items are `response_item` messages with `payload.role == "assistant"`; subagent reports, reasoning and tool outputs were not substituted for completion messages.
- The full source was searched for completion/rating/improvement messages, then the newly posted Clark completion was retrieved from the same exact rollout. Restart completion sources now span P:5280–19212, 02:56:06.365–13:04:37.801 UTC on September 9. Earlier source-only suggestions are preserved separately below. There is **no document-only fallback** in the current backlog and no missing completed restart branch within this cutoff. This does not claim recovery of content deleted before it entered the rollout or of later completions.
- Quotation marks identify **verbatim excerpts**; surrounding headings, indexing and status explanations are editorial. Markdown emphasis and surrounding bullet markers are omitted from excerpts. Trainer-ID mappings were checked against the current canonical catalogue; that catalogue supplies identity, not the quoted judgment.
- Ratings are the **actual main-chat intention ratings at completion**, not confidence scores, measured win rates, or current re-ratings.

**Open** means the completion message left the opportunity unresolved and no closure is recorded here. It does not independently certify that a later shared repair has not helped. Before closing one, record the actual change/evidence and a dated completion or follow-up source; do not silently delete the original suggestion.

## Restarted completions — open opportunities

All source times in this section are **2026-09-09 UTC**. Rows through Jose each represent one completed encounter record, with E0001's three rated starter branches grouped in one row and its six aliases mapped below. Clark's appended row represents **one completed branch of partial E0023**, not another completed encounter record.

| Battle / actual trainer ID | Main-chat rating | Remaining improvement or countertradeoff — verbatim | Provenance | Status |
|---|---:|---|---|---|
| E0001 — Route103 rival, six aliases below | Treecko 8.5; Mudkip 8; Torchic 8.5; regional alternatives collectively 8 | “More branch-specific reserve support would improve them; the shared reserve lineup still limits their distinctiveness.” | P:5280, 02:56:06.365 | Open |
| E0002 — Calvin, `TRAINER_CALVIN_1` | 9/10 | “the defensive transition between the setup lead and reserves could be richer; the birds still share Rock/Electric weaknesses.” | P:6033, 03:27:00.938 | Open |
| E0003 — Rick, `TRAINER_RICK` | 9/10 | “the reserves could have a stronger way to regain Speed control after Spinarak falls, though that would cost another useful move.” | P:6503, 03:44:01.749 | Open |
| E0004 — Allen, `TRAINER_ALLEN` | 8.5/10 | “the reserves need a more distinctive support interaction beyond hard-hitting attacks and Truant counterplay.” | P:7323, 04:31:22.133 | Open |
| E0005 — Tiana, `TRAINER_TIANA` | 9/10 | “Eevee’s Wish and Buneary’s Orb transfer still need deeper multi-turn positioning to reach their full potential.” | P:8057, 05:05:49.006 | Open |
| E0006 — Billy, `TRAINER_BILLY` | 9/10 | “reserve positioning and next-turn planning still need improvement.” | P:9012, 05:45:56.405 | Open |
| E0007 — Darian, `TRAINER_DARIAN` | 8.5/10 | “stronger reserve positioning and better answers to Grass pressure would improve him.” | P:9559, 06:08:19.473 | Open |
| E0008 — Cindy, `TRAINER_CINDY_1` | 8.5/10 | “Slakoth’s narrow attack window needs stronger support, and the team could benefit from another pivot.” | P:10445, 06:48:30.067 | Open |
| E0009 — Lyle, `TRAINER_LYLE` | 8.5/10 | “better support Speed/bulk tuning or a genuinely evaluated pivot cycle.” | P:11304, 07:26:11.408 | Open |
| E0010 — Petalburg Woods Aqua grunt, `TRAINER_GRUNT_PETALBURG_WOODS` | 8.5/10 | “reserve play against fast physical pressure and longer-term switching could be stronger.” | P:12248, 08:04:23.736 | Open |
| E0011 — James, `TRAINER_JAMES_1` | 8.5/10 | “his late-game reserves need more distinctive coordination beyond the rain lead.” | P:13429, 09:09:12.050 | Open |
| E0012 — Winston, `TRAINER_WINSTON_1` | 8/10 | “smarter reserve selection against Fire and more selective setup decisions.” | P:14233, 09:40:15.711 | Open |
| E0013 — Haley, `TRAINER_HALEY_1` | 8/10 | “cleaner reserve tempo and better handling of mixed Trace outcomes.” | P:15125, 10:13:24.272 | Open |
| E0014 — Gina & Mia, `TRAINER_GINA_AND_MIA_1` | 8/10 | “improve remaining double-Protect openings, defensive-matchup pacing, and roughly one-second decision latency.” | P:15882, 10:49:42.879 | Open |
| E0015 — Ivan, `TRAINER_IVAN` | 7.5/10 | “fewer reserve double-Protects, better demonstrated Shell Smash opportunities, and lower decision latency—currently about 0.65–0.77 seconds in these samples.” | P:16473, 11:14:44.519 | Open |
| E0017 — Cyndy, `TRAINER_CYNDY_1` | 8.5/10 | “better adaptation to sustained Fairy pressure and burns, plus lower latency.” | P:16897, 11:32:30.637 | Open |
| E0018 — Hector, `TRAINER_HECTOR` | 8.5/10 | “better Seviper preservation, fuller flinch forecasting and lower worst-case latency.” | P:17346, 11:48:04.015 | Open |
| E0019 — Marlene, `TRAINER_MARLENE` | 8/10 | “fewer wasted defensive turns and more distinction from Sarah’s nearby Room team.” | P:17627, 12:01:37.082 | Open |
| E0020 — Joey, `TRAINER_JOEY` | 7.5/10 | “occasional wasted double Protect; Belly Drum’s full setup-to-sweep sequence isn’t demonstrated yet. Improving those would raise his rating.” | P:18202, 12:21:57.480 | Open |
| E0021 — Jose, `TRAINER_JOSE` | 8.5/10 | “eliminate unnecessary Tailwind/guard turns against slower opponents and differentiate Kricketune’s repeated moveset.” | P:18618, 12:40:50.803 | Open |
| E0023 — Clark, `TRAINER_CLARK` (branch complete; grouped record partial) | 8.5/10 | “better switching against single-target Water/Grass pressure, and fewer repeated Protect attempts when Archen is already weakened.” | P:19212, 13:04:37.801 | Open |

### E0001 alias/rating mapping

The trainer constant's starter suffix is not the opponent's actual Hoenn lead. Preserve the actual party mapping rather than assigning the rating from the suffix:

| Actual Hoenn lead | Actual trainer aliases | Completion rating |
|---|---|---:|
| Treecko | `TRAINER_BRENDAN_ROUTE_103_MUDKIP`, `TRAINER_MAY_ROUTE_103_MUDKIP` | 8.5/10 each |
| Mudkip | `TRAINER_BRENDAN_ROUTE_103_TORCHIC`, `TRAINER_MAY_ROUTE_103_TORCHIC` | 8/10 each |
| Torchic | `TRAINER_BRENDAN_ROUTE_103_TREECKO`, `TRAINER_MAY_ROUTE_103_TREECKO` | 8.5/10 each |

The same completion gave regional alternatives **8/10 collectively**. It did not give individual ratings to every regional starter, and those alternatives must not be counted as additional authored trainer branches here. The shared reserve-support suggestion applies to this grouped completion.

### Repeated completion summaries — deduplicated, not discarded

These actual later chat messages repeat an existing completion and add no distinct opportunity. They remain traceable without inflating the backlog:

| Battle | Later source | Verbatim repeated suggestion |
|---|---|---|
| E0010 | P:12419, 08:11:17.860 | “stronger multi-turn switching and reserve play.” |
| E0011 | P:13834, 09:22:01.106 | “Better reserve coordination would raise his score.” |
| E0012 | P:14473, 09:47:00.333 | “smarter Fire-matchup reserves and setup choices.” |
| E0013 | P:15129, 10:13:35.451 | “reserve tempo and mixed Trace outcomes.” |

## Historical source-only suggestions — not restart completion credit

The chat reset the ledger to **0 battles AI-complete** at 02:13:54.619 UTC. The following fifteen earlier rated messages cover thirteen encounter records, including three rival lead judgments. Their ratings and suggestions are preserved as history, **not substituted for the native-restart ratings above**.

Status **Historical / unreconciled** means retain the original idea for reference, but re-evaluate it against current native teams before opening implementation work. Some premises changed during the restart; a superseded rating is not evidence that its suggestion was either implemented or rejected. These are all recovered chat excerpts, not document-derived reconstructions.

| Battle / branch | Historical rating | Verbatim suggestion | Provenance, 2026-09-09 UTC | Status |
|---|---:|---|---|---|
| E0001 Treecko, same two aliases mapped above | 8.5/10 | “A more distinct defensive reserve transition would improve it; Treecko and Shroomish overlap.” | P:3734, 01:56:49.472 | Historical / unreconciled |
| E0001 Mudkip, same two aliases mapped above | 8/10 | “Better pivot support would make recovering from Superpower’s stat drops more interesting.” | P:3750, 01:57:34.201 | Historical / unreconciled |
| E0001 Torchic, same two aliases mapped above | 8.5/10 | “The transition into the physical reserves could be more coordinated.” | P:3759, 01:57:47.238 | Historical / unreconciled |
| E0002 Calvin | 8.5/10 | “The two bird reserves still overlap; a different defensive or special-attacking reserve could make the second phase richer.” | P:3786, 01:58:49.348 | Historical / unreconciled |
| E0003 Rick | 8/10 | “A second source of support would make the reserves more coordinated after Spinarak falls.” | P:3795, 01:59:12.449 | Historical / unreconciled |
| E0004 Allen | 8/10 | “A stronger pivot-to-sleep transition would improve cooperation between the opening pair and reserves.” | P:3810, 02:00:34.004 | Historical / unreconciled |
| E0005 Tiana | 8.5/10 | “More direct cooperation between the Orb disruption and the opening pair would improve it.” | P:3819, 02:00:59.220 | Historical / unreconciled |
| E0006 Billy | 9/10 | “make the reserves interact more directly with rain.” | P:3912, 02:06:40.311 | Historical / unreconciled |
| E0007 Darian | 8/10 | “a non-Water support reserve would broaden the defensive options.” | P:3947, 02:07:21.428 | Historical / unreconciled |
| E0008 Cindy | 8.5/10 | “add another useful pivot to strengthen that defensive cycle.” | P:3957, 02:07:33.790 | Historical / unreconciled |
| E0009 Lyle | 8.5/10 | “a useful switching option could let Nymble reuse First Impression.” | P:3967, 02:07:46.374 | Historical / unreconciled |
| E0010 Woods grunt | 8/10 | “give the reserves a more distinctive partnership.” | P:3987, 02:08:28.891 | Historical / unreconciled |
| E0011 James | 8/10 | “give the reserves a stronger rain interaction without repeating more Water bugs.” | P:3998, 02:09:05.856 | Historical / unreconciled |
| E0012 Winston | 8/10 | “connect Wooloo’s late entry more reliably to a surviving Room setter.” | P:4019, 02:10:08.458 | Historical / unreconciled |
| E0013 Haley | 8/10 | “differentiate Marill and Snubbull’s reserve roles more sharply.” | P:4057, 02:10:56.784 | Historical / unreconciled |

### Earlier campaign-wide design advice

At **2026-09-08 23:53:28.386 UTC, P:1691**, the provisional campaign design rating was **7/10**, not an individual restart score. The message said:

> What keeps the overall rating below 9 is unevenness: some encounters have a distinctive interconnected plan; others mostly assemble individually strong Pokémon. And some clever plans still lack reliable execution or a convincing fallback when disrupted.

> To reach your target, I’d prioritize **memorable combinations with multiple ways to operate**, not simply more gimmicks. A proper Perish Trap encounter would be a worthwhile addition—but it needs trapping, survival, switching and endgame instructions that actually work.

The same chat later announced Maura's coordinated Perish Trap repair at **2026-09-09 00:26:55.530 UTC, P:2605**. Record that narrow proposal as **addressed in the earlier repair pass**, not as a missing feature to re-add, and not as a later encounter's completed sequential AI review. The broader consistency/fallback advice remains an open campaign-wide design direction.

## Updating after each completion

1. Read the actual main-chat completion message. Append the battle ID, exact trainer branch or aliases, stated rating, remaining suggestion/countertradeoff, source timestamp and source location. Preserve the wording as a quotation, or explicitly label a paraphrase.
2. Update the cutoff and completed branch/encounter counts from the announcement. Do not count a pending battle, diagnostic pass, source review, subagent report or regional variant as another completion.
3. Keep genuinely new follow-up opportunities; deduplicate repeated summaries while retaining their provenance. If a completion has no improvement suggestion, say **not stated in completion** rather than inventing one.
4. Keep original suggestions when closing or superseding them. Add the dated resolution and native evidence or design decision separately. A changed shared engine may justify review, but does not silently close every related encounter entry.
5. If chat text cannot be recovered, mark any fallback explicitly **document-only / chat not recovered**, cite that document, and leave the main-chat rating unverified. Never relabel a source-only historical rating as a restart completion rating.

No entries for the remaining hundreds of encounters have been fabricated. They will be appended as the actual completion messages are posted.
