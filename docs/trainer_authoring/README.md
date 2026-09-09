# Complete trainer authoring pass

User-authorized September 8, 2026. Scope: every currently authored campaign
trainer branch, individually reviewed and optimized where justified. Starting
catalogue: 516 branches across 468 encounter records, including rival aliases and
multi-trainer groups. Endless generated Circuit teams are a separate system,
not 516 hand-authored campaign battles. Excluded/retired encounters must not be
silently restored to increase coverage.

## Completion standard

**Latest user override: restart at Battle 1 and fix executable AI as each battle
is handled.** Later authoring batches are paused. Previous source reviews and
chat ratings remain useful work, but are not encounter-AI completion. For each
battle, run relevant native decision scenarios using the actual compiled team,
fix demonstrated decision failures, rerun, and report observed behavior plus
remaining limits in the main chat before advancing. Do not equate human-readable
plan/crack prose with executable instructions or demand a bad scripted move.
Current restarted encounter-AI review completion: **23 / 516 branches, 18 / 468
encounter records**; E0001 through E0015 and E0017–E0019 repaired with actual-team native scenarios. This is
bounded scenario verification, not exhaustive matchups or a full playthrough.
Evidence and limitations: ai_restart_0001.md through ai_restart_0015.md and ai_restart_0017.md through ai_restart_0019.md.
E0009 adds native screen/Struggle Bug repairs and a selectively authored
pressure plan that corrects its sampled healthy opening waits. E0010 adds
action-time HP-powered damage, spread-target scoring repairs, and native-tested
Speed/bulk/item upgrades. E0011 adds offensive-drop, Life Orb, stolen-Sitrus and
contact-damage AI repairs, plus native-tested defensive investment and a rain
setter preservation pivot. E0012 adds timely Cotton Guard and damage-based recoil
forecasts plus native-tested Eviolite Gimmighoul reserve pressure. E0013 adds
delayed Wish valuation, deterministic Trace switch-entry parity, and reliable
Fire Punch coverage; Helping Hand/Thunder Wave and recovery execute natively.
E0014 adds primary Charm, non-Flying Roost, Nuzzle secondary speed control and
action-time Plus/Minus survival forecasts plus an individually tested pressure
preference; actual Tail Glow reserve pressure and mixed-pair support execute
natively. E0015 adds action-time Deep Sea Tooth/Life Orb loss forecasts and an
individually tested pressure preference; native Acid Spray, Ice Beam and
Chinchou follow-through produce sampled knockouts. Its intention rating is
7.5/10, with unobserved Shell Smash payoff and reserve double guards explicitly
remaining limits. E0017 adds native Feint partner guard-clearing, accurate
noncontact Psycho Cut, Eviolite Clobbopus survival and an individually tested
pressure preference; Coaching and four-turn reserve buildup execute natively.
Its intention rating is8.5/10. E0018 adds native Glare/Body Slam partner-order
forecasts and Helping Hand Spinda supporting Toxic Boost Facade;31 actual-party
parameters and31 clean shared groups pass. Its intention rating is8.5/10.
E0019 adds mixed-bulk Spoink, Eviolite Baltoy and a native-tested pressure
preference alongside Room. Actual Yawn, Regenerator and Competitive sequences
execute; 30 battle parameters plus 3 field-state queries and 31 clean shared
groups pass. Its intention rating is 8/10. E0020 is next; E0016 is not a current encounter record.

Each trainer record needs its actual team reviewed: lead and partner payoff,
reserve transitions, legal moves/abilities/items, native Stat Points and speed
logic, useful counterplay at that stage, and species/strategy repetition nearby
and campaign-wide. Explicit reasons may support retaining good choices; cosmetic
edits are not required. Fix clear defects rather than merely rating them lower.

An intention rating evaluates cohesion, creativity, counterplay and progression
fit, not raw strength or a measured win rate. Every completed record is announced
in chat with its 1–10 rating and a concrete improvement path below 10. Native
mechanical evidence and remaining approximations are recorded separately.

Source review/authoring, materialization checks and native battle execution are
separate statuses. Do not claim every fight was played because shared tests pass.

## Work ledger

**Audit prerequisite completed; authoring resumed.** The static-check audit and
test reduction finished with 156 retained host checks passing. No later batch
edits were started during the pause.
Earlier changes through E0045 were materialized before this pause, but they are
not counted as completed/announced encounters. The audit corrected earlier
legality claims: Grimer Knock Off and Slowpoke Scald are natively available.
See `docs/static_check_audit/` before using any former gate as evidence.

Before the executable-AI restart, source-reviewed, materialized and announced:
**18 / 516 branches, 13 / 468 encounter records**.
This count is not native encounter verification. Route103's six aliases were
announced as three actual Hoenn team variants (Brendan and May each): Treecko
8.5, Mudkip 8, Torchic 8.5, with individual changes/improvement paths in chat.
Regional starter substitutions are recorded separately in the batch review.

| Assigned encounter IDs | Owner | Status | Individual records |
| --- | --- | --- | --- |
| E0001–E0009 | root | E0001–9 repaired with native scenarios and announced | batch_0001_0009.md; ai_restart_0001.md through ai_restart_0009.md |
| E0010 | root | Executable AI and native team upgrades complete | ai_restart_0010.md |
| E0011 | root | Executable AI and native team upgrades complete | ai_restart_0011.md |
| E0012 | root with subagent review | Bounded executable AI and native item upgrade complete; 8/10 | ai_restart_0012.md |
| E0013 | root with subagent review | Bounded executable AI and native coverage upgrade complete; 8/10 | ai_restart_0013.md |
| E0014 | root with subagent review | Bounded executable AI repairs and actual-team native scenarios complete; 8/10 | ai_restart_0014.md |
| E0015 | root with subagent review | Bounded executable item-loss repairs and native pressure improvement complete; 7.5/10 | ai_restart_0015.md |
| E0017 | root with subagent review | Bounded executable Feint repair and native team/pressure improvements complete; 8.5/10 | ai_restart_0017.md |
| E0018 | root with subagent review | Bounded executable paralysis repairs and native Helping Hand upgrade complete; 8.5/10 | ai_restart_0018.md |
| E0019 | root with subagent review | Native defensive investments and executable Room/pressure preference complete; 8/10 | ai_restart_0019.md |
| E0020–E0021 | trainer_strategy | Earlier source review only; native restart pending | batch_0010_0021.md |
| E0022–E0033 | battle_ai | Source complete; root integration/announcements pending | batch_0022_0033.md |
| E0034–E0045 | wild_distribution | Source complete; root integration/announcements pending | batch_0034_0045.md |
| E0046–E0075 | root | Paused: E0046–49 source changes; E0050–75 untouched in this batch | batch_0046_0075.md |
| E0076–E0105 | battle_ai | Paused: through E0088 source-reviewed; E0089–92 partial | batch_0076_0105.md |
| E0106–E0135 | wild_distribution | Paused: through E0116 source-reviewed; E0117–135 untouched | batch_0106_0135.md |
| All remaining active IDs | unassigned | pending | — |

Gaps in encounter IDs are intentional. A range assignment includes only current
records and every branch within them. Root owns shared materialization and the
completion count. Parallel authors edit only assigned canonical team blocks and
their own review documents. Current wild habitat edits and prior repairs are
preserved. No publishing or whole-game completion claim before full integration.

## Distribution baseline

Native authoring trap found while reviewing E0004: current materialization emits
no Happiness field, so trainerMon friendship defaults to0 and trainer_util.c
copies that into the Pokemon. Native Return then has1 base power after its
minimum clamp, while Frustration has102. Do not assume a Showdown-style255
friendship default or prescribe Return as a Strength upgrade. Review actual
friendship on each relevant branch; this finding does not mark later encounters
reviewed or authorize counting a bulk replacement as individual completion.

At pass start, the branch-weighted most frequent species are Manectric 22,
Breloom 21, Swellow 19, Taillow 15, and Camerupt/Crobat/Sharpedo/Mimikyu 13 each.
These include duplicated rival branches: raw frequency alone is not a veto on a
species. Check unique encounter frequency, adjacent fights, family repetition,
team role and regional identity before changing a roster. Preserve meaningful
returning rival identities and villain/gym themes without turning every reserve
into the same generic competitive pick. No species-coverage quota.
