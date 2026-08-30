# Emerald Champions battle-system source audit

> **Historical diagnosis, not current status.** This document records the
> `e7df95386` pre-repair snapshot that supplied the first-pass review queue.
> Its illegal Abilities, nature contradictions, dead charge moves, broken
> speed modes, Imposter/Helping Hand uncertainty, and named trainer defects
> were subsequently addressed in executable source and re-audited across all
> 513 encounters. Current closure evidence lives in the three
> `TRAINER_QUALITY_*` ledgers, the fresh campaign second pass, and the release
> gates; do not quote a “still-open” statement below as present truth.

Audit snapshot: commit `e7df953860be1a250f6720fa4579aebe30436a04` on
`audit/story-progression-closure`.

Input hashes at audit time:

- `src/data/trainers.party`:
  `5d9b753aa28c1d736f0c7d1b87b0e9d9f8c2363c7977871a4366ca79c7a6884c`
- `docs/emerald_champions_master_battle_design.txt`:
  `e4c016cae325cdf2c115cd4637dbcd0bcae7cec1fdbabead6b4e4d6b7bf47b1b`

This audit uses the master only to identify and order the 561 runtime campaign
branches and associate each with its cap milestone. Every team, level, item,
Ability, nature, Stat Point spread, move, party order, and AI profile below was
read from `src/data/trainers.party`. Difficulty prose and `difficulty_target`
were not treated as evidence.

## Executive verdict

The external audit is directionally right about battle-data quality, but several
of its numerical interpretations are too broad or use the wrong model.

Confirmed:

- The campaign materializes 561 branches containing 2,147 authored Pokemon.
- Twelve branches contain both Trick Room and Tailwind.
- Protect appears on 1,440 of 2,147 Pokemon (67.1%). It is move four on 1,273
  of the 2,141 four-move sets, and all members carry it on 180 of 561 teams.
- Nature errors are real, including Wallace's Jolly Ferrothorn and multiple
  fully physical or fully special sets whose nature lowers their only attacking
  stat.
- AI quality is tiered but not bespoke. No campaign script calls the existing
  dynamic-AI hooks. There are only six repeated generic profiles.
- Hard-mode authored levels soften materially after the first quarter.
- Pledge, Decorate, Commander sleep/Yawn, and simultaneous voluntary switch
  order have source fixes and focused tests. Imposter remains unproven.

Corrections:

- Trick Room plus Tailwind is not automatically a defect. Several teams put the
  two modes in successive party phases. Others do contradict themselves.
- Protect is foundational in doubles. The defect is unexplained saturation and
  copied fourth slots, especially in singles, not the presence of Protect.
- The Stat Points are not theatrical. Under the active Champions formula, 32
  stored points become 63 points of stat investment (`2 * points - 1`), the
  analogue of a maximized traditional investment. See `src/pokemon.c:1417-1428`
  and `src/pokemon.c:1447-1458`.
- The AI does not simply decline. The final third has more full Smart profiles
  and more Smart Switching than the first third, but it also retains 55 bare
  Basic branches. Progression is noisy, not monotonically worse.

The highest-value repairs are: make Drake a coherent speed-control boss; repair
the attack/nature families; fix Wallace's Ferrothorn; repair Wayne's weather
contradiction; prove or contain Imposter; fix the known Helping Hand scoring
failure; and run the per-branch review described at the end of this document.

## Corpus and runtime facts

### Formats

| Format | Branches | Pokemon |
|---|---:|---:|
| Single | 87 | 200 |
| Double | 460 | 1,905 |
| Multi | 14 | 42 |
| **Total** | **561** | **2,147** |

The doubles/multi share is 84.49%, matching the intended 85% campaign.

### Levels are authored, not dynamically cap-relative

`GenerateMonFromTrainerMon` passes `trainerMon->lvl` directly to `CreateMon`
(`src/trainer_util.c:124-140`). The badge cap table is independently fixed at
14, 20, 30, 40, 45, 55, 60, 70, and 80
(`src/caps.c:8-38`). No runtime function converts a trainer offset into
`current cap + offset`.

Mapping actual party levels to the canonical encounter's earliest cap gives:

- mean hard-mode offset: **-0.755**;
- median: **-1**;
- 1,136 Pokemon below cap, 703 at cap, 308 above cap;
- quarter means: **+0.10, -0.75, -1.23, -1.05**.

This confirms a real pressure drop in the middle half. It does not prove that
those battles are easy; team composition and player tools still matter. It does
prove that the Leveler puts the player above most authored opponents in the
middle and late campaign.

The live slider itself is correct. New games default to Hard
(`src/new_game.c:225-234`). Hard/Medium/Easy subtract 0/2/4 levels, respectively,
then recompute EXP, stats, max HP, and current HP (`src/difficulty.c:27-66`). The
reduction is applied after creating both enemy parties
(`src/battle_setup.c:491-499`). Species and evolution stage remain unchanged, as
intended.

### Route density

The physical sequence still has meaningful fatigue spikes:

- Route 111: 21 encounters (`data/maps/Route111/scripts.inc:287-682`);
- Route 119 plus Weather Institute: 24 encounters
  (`data/maps/Route119/scripts.inc:79-448` and the two
  `Route119_WeatherInstitute_*` maps);
- Mt. Pyre: 16 encounters;
- Magma Hideout: 19 encounters.

These are not automatically too many. They require a pacing pass that marks
mandatory versus optional, healing opportunity, spatial distance, and expected
concentration. Team power alone cannot answer fatigue.

## AI progression and executability

### Actual campaign profiles

| Actual `AI:` line | Branches |
|---|---:|
| Basic Trainer | 155 |
| Basic + HP Aware + Smart Mon Choices + assumptions | 277 |
| Basic + Smart Switching/prediction/PP-stall prevention + assumptions | 104 |
| Full Smart + Prediction + omniscience + Powerful Status | 22 |
| Smart-Mon profile plus Attacks Partner | 2 |
| Basic plus Attacks Partner | 1 |

By canonical branch order:

| Third | Bare Basic | Full Smart | Smart Switching | Smart Mon Choices only | Mean written profile tokens |
|---|---:|---:|---:|---:|---:|
| First | 44/187 | 6 | 23 | 114 | 4.53 |
| Middle | 56/187 | 2 | 33 | 95 | 4.35 |
| Final | 55/187 | 14 | 48 | 70 | 4.82 |

So the report's claim that AI simply gets dumber is not supported. The middle
third is the weakest by profile, and the late game still contains too much bare
Basic AI, but the final third increases the two strongest profiles.

More importantly, the campaign has **zero bespoke dynamic-AI script calls**.
The engine exposes `ScriptSetDynamicAiFunc` and `ScriptSetDynamicAiSwitchFunc`
(`src/battle_ai_main.c:6807-6826`), but no map or event script invokes them.
Emerald Champions currently has bespoke teams run by generic AI, not bespoke AI
for almost every trainer.

That is not a demand for 561 custom functions. It is a demand to reserve custom
AI for teams whose central puzzle cannot be expressed by generic scoring and
party order, and to regression-test those few functions.

### Doubles mechanics already repaired

- Pledge: `ShouldUsePledgeMove` models each complementary pair and active side
  condition (`src/battle_ai_util.c:6684-6745`); the regression test checks both
  AI evaluation orders (`test/battle/ai/ai_doubles.c:5-27`). The old blanket
  claim that the Route 110 Pledge team cannot coordinate is stale for this tree.
- Decorate: foe targets are rejected before scoring
  (`src/battle_ai_main.c:1129-1138`) and a focused ally-only test exists
  (`test/battle/ai/ai_doubles.c:29-44`).
- Commander: swallowed Tatsugiri sleep and Yawn progress without revealing it
  (`src/battle_end_turn.c:925-970`), with tests at
  `test/battle/ability/commander.c:928-974`. Commander switch prevention also has
  focused AI coverage (`test/battle/ai/ai_switching.c:2351-2372`).
- Simultaneous voluntary switches now use effective Speed and Trick Room
  (`src/battle_main.c:4621-4636`, `src/battle_main.c:4752-4759`).

### Still-open doubles risks

1. **Imposter is not proven safe.** The generic Imposter test file is still a
   TODO (`test/battle/ability/imposter.c:4`), and no focused AI regression proves
   that a transformed opponent cannot target itself incorrectly. Two campaign
   branches use Imposter, including early `TRAINER_BILLY`
   (`src/data/trainers.party:12872-12923`). Calling the upstream report a test
   harness artifact may be correct, but it is not release proof. Add a cartridge
   scenario test or remove Imposter from Billy until it is proven.
2. **Helping Hand has a known failing AI test.** The campaign uses Helping Hand
   152 times on 130 teams, but the test that should prefer it when the ally deals
   notably more damage remains `KNOWN_FAILING`
   (`test/battle/ai/ai_doubles.c:1239-1251`). This is a much larger practical
   AI exposure than Perish Song, which appears only once.
3. **Fainted replacements still iterate battler slots.** `HandleFaintedMonActions`
   walks battler IDs from zero upward (`src/battle_util.c:1821-1901`). The manual
   switch fix does not establish modern Speed ordering for simultaneous faint
   replacements. This can reorder Intimidate, weather, Trace, and similar entry
   effects.
4. **Full spread-end interleaving remains unproven.** This matters because the
   campaign contains 206 Rock Slides, 117 Earthquakes, 104 Heat Waves, 60 Hyper
   Voices, 49 Dazzling Gleams, 35 Muddy Waters, 22 Surfs, 21 Blizzards, and 7
   Discharges. Treat it as a differential-test target, not a speculative rewrite.
5. **Mid-turn AI cache invalidation remains unproven.** Exposure is high: 81
   U-turns, 50 Volt Switches, 16 Parting Shots, and 4 Flip Turns. Reproduce the
   upstream stale-data case before changing architecture.
6. **Three Solar Beam sets have no sun or Power Herb.** Irene's Exeggutor,
   Cameron's Alolan Exeggutor, and Tabitha's Coalossal spend two turns on the
   move. The other 15 Solar Beam uses have a team sun source. These three should
   receive immediate coverage unless a tested charge-turn puzzle is intentional.

## All twelve Trick Room plus Tailwind teams

The normal AI has a benefit check for Trick Room, but `AI_FLAG_POWERFUL_STATUS`
skips that negative check (`src/battle_ai_main.c:2713-2731`). Powerful Status
also directly rewards setting Trick Room whenever it is absent
(`src/battle_ai_main.c:6464-6479`). This distinction makes Drake worse than an
ordinary dual-mode team.

| Encounter / trainer | Source | Classification | Source-derived judgment |
|---|---|---|---|
| 19 / Marlene | `src/data/trainers.party:30918` | **Repair** | Meditite and Spoink are fast leads; reserve Tropius has Tailwind and reserve Litwick has Trick Room. The two opposing setters can arrive together. Tropius is Sassy while spending 32 points on Speed. Choose one reserve mode or enforce a real phase. |
| 43 / Edmond | `src/data/trainers.party:19196` | **Keep, verify** | Slowpoke/Koffing are a coherent slow Trick Room opening; Staravia is the only reserve and supplies Tailwind after the opening breaks. This is a credible phase transition, not an automatic contradiction. |
| 259 / Chip | `src/data/trainers.party:2286` | **Keep, verify** | Brave Crustle plus Quiet Claydol use Trick Room; Aerodactyl is the only reserve and can establish Tailwind later. Verify the one-turn handoff when it enters before Room expires. |
| 270 / Cameron | `src/data/trainers.party:9801` | **Keep architecture; fix Solar Beam** | Solrock and Alolan Exeggutor are slow Room leads; Delphox and Shaymin form the fast Tailwind reserve. This is the clearest dual-phase team, but Alolan Exeggutor's ungated Solar Beam is a dead two-turn slot. |
| 326 / Magma Grunt 5 | `src/data/trainers.party:29408` | **Repair** | Talonflame/Bisharp open Tailwind, but Dusknoir is the only slow Room payoff and its Shadow Sneak ignores speed anyway. Venomoth remains fast. Replace Trick Room or add an actual slow closing core. |
| 353 / Chad | `src/data/trainers.party:7571` | **Repair** | Jellicent sets Room beside max-Speed Stoutland; the only reserve is max-Speed Lumineon with Tailwind. Only Jellicent wants Room. This is direct self-interference. |
| 376 / Henry | `src/data/trainers.party:27114` | **Repair** | Jellicent sets Room beside fast Tentacruel, followed by fast Chatot and Fletchinder, both carrying Tailwind. One slow setter does not make a dual-mode team. |
| 392 / Samantha | `src/data/trainers.party:10026` | **Repair or explicitly sequence** | Fast Xatu/Gardevoir lead with Tailwind; Magearna alone brings Room, while fast Glimmora is the other reserve. Smart Switching can interleave the phases. Either remove Room or pair Magearna with a slow payoff and deterministic order. |
| 466 / Bethany | `src/data/trainers.party:12397` | **Keep architecture; fix investments/order** | Slowking, Brave Azumarill, and Brave Relicanth can use Room before Staraptor supplies late Tailwind. But Azumarill and Relicanth each waste 32 points on Speed despite Brave natures, and Smart Switching can send Staraptor early. |
| 483 / Shannon | `src/data/trainers.party:4673` | **Repair** | The lead already mixes fast Blaziken with slow Escavalier. Ribombee and Orbeetle, the final two slots, bring opposing modes together; almost every other member is speed-positive. No stable phase exists. |
| 496 / Drake | `src/data/trainers.party:10797` | **Critical repair** | Every member is Timid or Jolly. Dialga's Room has no slow payoff; Reshiram's Tailwind matches the rest. Drake's Powerful Status flag bypasses the normal Room-benefit rejection and rewards setting it anyway. This directly weakens an Elite Four boss. Remove Room or rebuild a real slow dragon mode. |
| 501 / Leroy | `src/data/trainers.party:3528` | **Plausible, but not closed** | Cresselia can open Room and Mega Mawile can profit later; Salamence can supply late Tailwind. However Volcanion and Mawile spend 32 points on Speed, Smart Switching can scramble the intended order, and Kartana/Slurpuff are fast. Preserve the concept only with corrected investments and an executable handoff. |

Result: four are credible phased concepts (Edmond, Chip, Cameron, Bethany), two
can be rescued by explicit sequencing (Samantha, Leroy), and six should lose or
rebuild one speed mode (Marlene, Magma Grunt 5, Chad, Henry, Shannon, Drake).

## Nature and attack defects

The external claim of exactly ten individual contradictions is not reproducible
as an exhaustive count. Definitions matter because Foul Play, Body Press,
fixed-damage moves, utility Fake Out, and mixed attackers can rationally use a
nature that lowers one nominal damage category.

A strict source test found 29 entries that simultaneously:

1. invest 32 Stat Points in Attack or Sp. Atk;
2. use damaging moves only from that category; and
3. lower that same category with their nature.

Those collapse into 22 exact set fingerprints. The following ten repeated or
high-impact families should be repaired first; they account for 21 of the 29
entries and include both examples from the external report.

| Family | Affected runtime trainers | Contradiction |
|---|---|---|
| Quagsire | Vivi, Gwen, Brianna (`src/data/trainers.party:24405`, `:2862`, `:5496`) | Bold lowers Attack on fully physical, 32-Attack sets. |
| Gourgeist forms | Weather Institute Grunts 1, 2, 4 (`src/data/trainers.party:912`, `:965`, `:1389`) | Adamant/Jolly lower Sp. Atk on Shadow Ball + Energy Ball, 32-SpA sets. |
| Swellow template | All six Route 119 Brendan/May starter branches (begins `src/data/trainers.party:20426`) | Timid lowers Attack on Quick Attack + Facade + Brave Bird with 32 Attack. |
| Gigalith | Tabitha in Magma Hideout and Mossdeep (`src/data/trainers.party:30044`, `:20113`) | Modest lowers Attack on Rock Slide + Heavy Slam + Earthquake with 32 Attack. |
| Seismitoad | Matt at Mt. Pyre and Seafloor (`src/data/trainers.party:32349`, `:1590`) | Adamant lowers Sp. Atk on Muddy Water + Earth Power + Grass Knot with 32 SpA. |
| Dragapult | Tabitha in Magma Hideout (`src/data/trainers.party:30044`) | Jolly lowers Sp. Atk on Draco Meteor + Shadow Ball with 32 SpA. |
| Giratina | Phoebe (`src/data/trainers.party:10645`) | Adamant lowers Sp. Atk on Draco Meteor + Shadow Ball with 32 SpA. |
| Nidoqueen | Lorenzo (`src/data/trainers.party:22019`) | Adamant lowers Sp. Atk on Earth Power + Sludge Bomb + Ice Beam with 32 SpA. |
| Nidoking | Jenna (`src/data/trainers.party:22238`) | Adamant lowers Sp. Atk on the same three-special-attack set with 32 SpA. |
| Golduck | Annika (`src/data/trainers.party:19553`) | Jolly lowers Sp. Atk on Hydro Pump + Ice Beam + Psychic with 32 SpA. |

The remaining eight exact fingerprints include Lapras, Lickilicky, Emolga,
Skuntank, Cherrim, Crobat, Tropius, and Seviper and belong in the same repair
batch. A release gate should detect the strict contradiction while allowing an
explicit exemption for deliberate mixed or alternate-stat sets.

Separately, 49 source entries have base Speed at most 50 and a positive-Speed
nature. This is a review queue, not 49 bugs: early unevolved attackers and
speed-creep targets can rationally use Jolly/Timid. The unquestionably bad
marquee examples are Wallace's base-20 Jolly Ferrothorn, Glacia's base-30 Jolly
Glastrier, and Phoebe's base-40 Jolly Shedinja.

## Protect saturation and repair policy

Exact distribution:

| Format | Pokemon with Protect | All-Protect teams |
|---|---:|---:|
| Single | 95/200 (47.5%) | 36/87 (41.4%) |
| Double | 1,321/1,905 (69.3%) | 138/460 (30.0%) |
| Multi | 24/42 (57.1%) | 6/14 (42.9%) |

Do not bulk-delete Protect. Review it per slot.

Keep it when it creates a concrete turn: scouting a Choice lock, stalling one
specific weather/Room/Tailwind/Perish turn, activating an Orb, preserving a
setup or redirection piece, avoiding an ally spread attack, or creating a clear
double-target mind game.

Flag it for replacement when it is copied into move four without a named turn,
appears across an entire singles team, occupies every member of a six-Pokemon
doubles team, or displaces the coverage/support move needed to make the team's
central plan executable. The audit should emit a review queue, not fail on a
quota. The reviewer must write one short justification for every retained
Protect on an all-Protect team.

## Marquee and named examples

### Roxanne (`src/data/trainers.party:10873`)

**Verdict: powerful, thematic, not yet clean.** At cap 14 her levels are 17-19.
Carbink supplies Sturdy + Mental Herb Trick Room, Lileep's Storm Drain punishes
the obvious Water answer, and the slow reserve supports Room. The weaknesses are
the Jolly max-Speed Rockruff lead beside the Room setter and Protect on all six
members. The battle is likely brutally hard, but some of that difficulty is
stat/level pressure rather than a perfectly executable puzzle. Preserve the
Rock identity and Water trap; give the lead one coherent speed plan and justify
or replace at least two Protect slots.

### Brawly (`src/data/trainers.party:10948`)

**Verdict: current quality benchmark.** At cap 20 his levels are 22-25.
Pachirisu redirects and controls speed while Falinks uses No Retreat; Hitmontop
brings Fake Out, Intimidate, Wide Guard, Helping Hand, and an Eject Button;
Kirlia supplies special/support texture; Toxic Orb Poison Heal Breloom turns
Facade into a real payoff; White Herb Hawlucha consumes the Herb after Close
Combat and triggers Unburden. Only Hawlucha carries Protect. This is difficult
because six roles reinforce one another, not because every slot copies a module.

### Drake (`src/data/trainers.party:10797`)

**Verdict: critical redesign.** The legendary dragon roster is spectacular, but
its speed plan is self-defeating for the AI reason documented above. Dialga is
Timid and the other five are Jolly/Timid; Room helps many player counters more
than Drake. Koraidon's sun, Rayquaza's Air Lock, and partner-unsafe Earthquake
pairings also need explicit lead/order testing. Keep the iconic roster if
desired, but choose Tailwind offense or create a genuine slow dragon phase.

### Wallace (`src/data/trainers.party:13581`)

**Verdict: strong rain shell with one hard defect and too much copied Protect.**
Kyogre + Zapdos is a credible Water Spout/Thunder/Hurricane/Tailwind lead;
Manaphy uses Hydration and Tail Glow; Palkia and Competitive Milotic reinforce
rain pressure. Ferrothorn is Jolly, has no Speed investment, lowers the Sp. Atk
used by Giga Drain, and boosts a base-20 stat that cannot rescue it. All six
members carry Protect. Fix Ferrothorn to Relaxed/Sassy and give it a move aligned
with its investment; then replace enough Protect slots to add Haze, Leech Seed,
redirection, or other actual texture.

### Wayne (`src/data/trainers.party:27423`)

**Verdict: broken until both data and concept are repaired.** Authored Drizzle
on Wailord was illegal at audit time and therefore fell back to ability slot 0;
the separate Ability audit owns that correction. Even after Drizzle becomes
legal, Lickilicky's Cloud Nine suppresses the rain that powers Wailord's Water
Spout and Hydro Pump. The four-Pokemon team is run by bare Basic AI. Remove Cloud
Nine or make the weather cancellation an explicit second phase with moves that
benefit from it.

### Kirk (`src/data/trainers.party:8074`)

**Verdict: good simple engine once Electric Surge is truly legal.** Terrain
boosts Toxtricity's Throat Spray Overdrive and Electrode/Heliolisk pressure;
Heliolisk adds Hyper Voice and Glare. Its modest three-Pokemon size and mid-tier
AI are appropriate for a Gym trainer. If Electrode still cannot legally carry
Electric Surge, the runtime gets Soundproof and the whole engine disappears;
the Ability gate must prove the intended result.

### Leroy (`src/data/trainers.party:3528`)

**Verdict: better than the external audit implies, but unfinished.** Cresselia
and Mega Mawile provide two genuine slow pieces, and Salamence can establish a
late Tailwind phase. This is not the same failure as Drake. It still lacks a
guaranteed transition because Smart Switching can scramble reserve order,
Volcanion and Mawile spend 32 points on Speed, and Slurpuff uses Calm Mind with
only physical attacks. Correct the investments and either sequence the phase or
commit to one speed mode.

## Per-branch review schema

Apply this to all 561 runtime branches. The output is evidence, not a score
generated from dossier labels.

1. **Bind the physical encounter.** Record the actual map script line, trainer
   ID branch, format, mandatory/optional status, partner configuration, and
   earliest and typical campaign state.
2. **Resolve the actual runtime party.** Expand forms and aliases; record party
   order, hard/medium/easy levels, the Ability that actually resolves at
   runtime, item, nature, six Stat Points, and four move categories.
3. **Check legal execution.** Reject silent Ability fallback, illegal moves,
   dead items, wrong natures, wasted Stat Points, Choice/Assault Vest conflicts,
   unsupported charge moves, and evolution/Mega timing errors.
4. **Model the opening board.** Evaluate the first two active Pokemon together:
   speed order at the actual level, Fake Out/redirection, spread safety,
   weather/terrain/Room/Tailwind, immunities, target choices, and likely AI
   scores. Do not review six sets as six isolated singles sets.
5. **Model every replacement state.** Use actual AI switching flags and party
   order. A claimed phase transition is invalid if Smart Mon Choices can select
   the opposite phase first. Check switch-in Ability order and faint replacement
   parity risks.
6. **Prove the central mechanic.** If the team depends on Pledge, Commander,
   Helping Hand, Perish Song, ally targeting, self-activation, weather
   suppression, or Mega timing, attach a focused battle test or a traced generic
   AI behavior. Prose is not proof.
7. **Compare with player ammunition.** From actual wild/static/reward source,
   list at least several broad answer families available by that point: speed
   control, priority, redirection denial, weather/terrain control, defensive
   typing, status, Wide Guard, Haze/phazing, or direct offense. Avoid requiring
   one exact species or set.
8. **Judge difficulty without the dossier number.** State expected first-loss
   lesson, strongest line, easiest broad counterplay, and whether difficulty
   comes from synergy, information, raw level/BST, or AI knowledge. Keep
   `observed` as UNPLAYED until a real attempt records result, turns, casualties,
   team, mode, and retries.
9. **Check campaign pacing.** Compare the preceding and following ten physical
   encounters for repeated species, speed modes, weather, Protect structure,
   cognitive load, and reward. A good team can still be wrong in a 24-fight
   corridor.
10. **Close with a binary disposition.** `KEEP`, `REPAIR_DATA`,
    `REPAIR_TEAM`, `REPAIR_AI`, or `REPLACE`. Record exact source changes and the
    test needed before closure.

### Scalable output row

Each branch should emit one compact record:

```text
physical_id | trainer_id | source_line | format | mandatory | cap
actual_leads | reserve_order | hard/medium/easy levels
runtime_ability_check | nature/stat/move/item_check
speed_mode | weather_or_terrain | ally_interactions | AI_profile | AI_proof
available_counterplay | first_loss_lesson | fatigue_context
verdict | exact_repairs | verification_test | observed_result
```

Automate extraction and contradiction flags. Keep the opening-board judgment,
counterplay judgment, difficulty judgment, and final disposition human-authored.
That is the fastest way to inspect every battle without replacing design with a
misleading aggregate score.

## Prioritized repair queue

1. Drake speed architecture and a focused AI test.
2. Silent Ability fallback gate and the intended Inclement/Champions Ability
   decisions (owned by the separate Ability audit).
3. Strict attack/nature/Stat Point contradiction batch, beginning with the ten
   families above; Wallace's Ferrothorn in the same batch.
4. Wayne's Drizzle/Cloud Nine composition.
5. Helping Hand known failure, because 130 teams depend on the move.
6. Imposter cartridge-level regression or temporary removal from early Billy.
7. The six incoherent Trick Room/Tailwind teams; sequence or test the two
   conditional teams; preserve the four credible phase concepts.
8. The three unsupported Solar Beam sets.
9. Protect review queue, starting with all-Protect singles and marquee teams.
10. Per-branch source review in chronological batches, with route-level fatigue
    review after each map and real playtest observations kept separate from
    static intent.

This source supports a potentially excellent competitive campaign. It does not
yet support the claim that every battle is a bespoke, proven puzzle. The path to
that claim is not another global difficulty metric; it is the compact branch
closure loop above plus focused tests for the handful of mechanics generic AI
cannot reliably execute.
