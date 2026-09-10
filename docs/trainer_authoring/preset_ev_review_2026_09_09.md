# Shared preset EV review — September 9, 2026

**Latest override:** [all HP-triggered berries now get one HP of tolerance](berry_thresholds_2026_09_09.md).
The parity normalizer and both normalization tests were removed; exact authored
EVs are retained. The Snorlax ±40 fix, Circuit parity exception and Sitrus-only
threshold evidence below describe the superseded intermediate implementation,
not current requirements or permission to change EVs. The six catalogue edits
remain current. Level-dependent Substitute timing is a design consideration,
not a reason to restore hidden HP-EV adjustments.

Status: **catalogue-wide EV compatibility pass completed; individual trainer
benchmarking remains next**. This
report is derived from current code/data, not a new preset source. No trainer
branch receives completion credit from this inventory or its native stat probes.

## Complete inventory and scope

The canonical JSON resolves through the actual generator to 6,923 presets:
4,199 doubles and 2,724 singles, covering 1,309 species including 105 inherited
mechanical aliases. There are 87 distinct EV spreads; every set totals 508
effective EVs within 252/stat. The remaining two legal EVs cannot change EV/4.
This establishes allocation consistency, not optimal battle performance.

Sol/medium produced the bounded inventory and traced actual consumers; the main
reviewer adjudicated the changes below. Reproduce the scratch inventory with
`python3 work/preset-ev-audit/audit.py`. Its generated `inventory.json` contains
every stable species/format/choice/name identity, moves and native damage axes,
configured base stats, and review arithmetic at levels 10/12/14/50/100.
The reviewed canonical hash after these six changes is
`542986228c572c9a3a5fae91f3057cf95ec979c3edbf06cb08e76032dc5707d7`.

The scratch arithmetic is NOT a second runtime formula or a release gate.
Actual verification below invokes native preset application and stat generation.
Ordinary presets serve levels 2–100; some Circuit references serve opponents
above 100. A level-50 rounding improvement is not automatically safe everywhere.
See [consumer trace](preset_consumers_2026_09_09.md) for live overrides, conditional
rival stages, level ranges, dormant legacy paths and acquisition exceptions.

## Individual changes made

| Species / format / set | Change | Reason and tradeoff |
|---|---|---|
| Wigglytuff / doubles / Perish Control | 0/0/252/0/252/4 → 4/0/252/0/252/0 | Preserve dual-defense investment on a high-base-HP support; remove spare Speed from its deliberately slow mode. Lower Speed is intentional, not a universal advantage. |
| Applin / singles / Sash Mixed Attacker | Quiet → Rash; retain 4/0/0/252/0/252 | Preserve Draco Meteor and physical priority power without undermining the invested Speed/Pounce role. Costs Special Defense; Focus Sash does not erase every defensive consequence. |
| Rolycoly / doubles / Steam Engine Policy | Gyro Ball → Rock Blast; retain Attack/Speed spread | Gyro Ball's power conflicts with the intended Speed activation. Rock Blast is in the current teachable pool and gives single-target multi-hit pressure alongside spread Rock Slide. Steel coverage is lost. |
| Beedrill / doubles / Sash Tailwind Endeavor | Jolly → Timid; 4/252/0/0/0/252 → 4/0/0/252/0/252 | Endeavor uses HP difference, not Attack. Electroweb is the sole stat-scaled attack. Preserve Speed/Sash HP and invest in the stat actually used. |
| Tangela / doubles / Double Powder | 252/0/252/4/0/0 → 252/0/252/0/4/0 | No move uses Special Attack. Preserve Knock Off damage and physical bulk; put the spare investment in Special Defense. |
| Hisuian Avalugg / doubles / Bulky Control | 252/4/252/0/0/0 → 252/0/252/0/4/0 | Body Press uses Defense, Icy Wind uses Special Attack. Spare Attack EVs did not support either move; Special Defense does. |

Materialization used only the canonical preset generator and then the Circuit
reference projection. Catalogue counts and struct sizes are unchanged; no
per-preset runtime branches or copied team tables were added.

## Judged retentions and remaining review

- Retain the 24 Speed-invested Trick Room sets: inspection identifies fast
  Imprison, reversal or anti-Room intentions. A blanket zero-Speed rewrite would
  contradict those roles. This is preset design review, not proof that a trainer's
  AI executes Imprison/reversal correctly.
- Retain the six Shedinja spreads' maximum Attack/Speed; each has only four
  residual Defense EVs and no HP investment. Those four EVs have no ordinary
  survival benefit at 1 HP, but there is no unused Attack/Speed capacity to buy.
- After the six edits, the inventory has no slow-nature/Speed or ordinary
  offensive-axis mismatch flags and no unused effective EV budget. These flags
  are deliberately incomplete; mixed moves, abilities and support intent still
  require judgment.
- **Historical HP-item issue inventory, now covered by the override above:**
  549 threshold-item sets, including 49 with
  Substitute and/or Belly Drum. Current Sitrus uses ceil-half activation, so
  odd HP is not itself a Sitrus defect. Gluttony/flavor berries, Berry Juice,
  Salac/Substitute, re-leveling and Circuit level changes need separate evidence.
  The existing retained Drum check covers only four species at player caps,
  not every preset or difficulty-adjusted level.
- Shared preset EVs are not substitutes for actual trainer-specific Speed,
  damage, survival and ability-activation benchmarks. E0001 remains open.

## Executed native evidence

`work/preset_ev_review.inc` is temporary diagnostic input, not a permanent new
per-preset test suite. Two native groups passed using current compiled presets:

- Wigglytuff and Applin compared before/after application at every level 3–100.
  Wigglytuff gained HP at 52 levels, never lost HP, and preserved all non-Speed
  combat stats. Applin gained Speed at all 98 levels while preserving HP,
  Attack, Defense and Special Attack; Special Defense decreases as intended.
- Beedrill, Tangela and Hisuian Avalugg compared at every level 2–100. Beedrill
  gained Special Attack at all 99 levels; Tangela and Avalugg gained Special
  Defense at 52 levels each. HP, Defense and Speed remained unchanged. Tangela's
  Attack and Avalugg's Special Attack remained unchanged.
- Rolycoly's compiled preset exposes Rock Blast in the revised slot. This is
  not a claim of a completed Steam Engine partner battle.

Generator comparison reproduced all 6,923 sets and 756 Circuit variants / 1,322
templates. Native output is in `work/preset-ev-review-native.log`.

After the user's explicit Sitrus clarification, all three existing `Sitrus Berry`
native groups passed (seven parameter cases). They exercise Belly Drum with
96/97 maximum HP, the adjacent half-HP activation boundaries, and full-HP
Shedinja non-consumption. At 97 max HP, Drum leaves 49 and Sitrus heals to 73;
50/97 does not trigger it. `HasEnoughHpToEatBerry`, the joint pair forecast,
hit-context berry forecast and switch-entry estimate all use the intended
ceil-half threshold. Log: `work/sitrus-convenience-review.log`.

### Non-Sitrus normalization repair

The temporary native berry probe applied eight non-Sitrus Belly Drum presets
at nine levels, plus Eiscue and Hawlucha Substitute presets at five levels:
82 applications in two groups. Before the repair, level-14 Snorlax remained
at 81 maximum HP because its Gluttony/Iapapa preset needs a 28-EV HP reduction,
outside the old 24-EV search. The bounded search and preset matcher now share
one 40-EV tolerance constant. Native application now yields 80 HP with 224 HP
EVs; other EVs are unchanged (480 total, not redistributed). Sitrus behavior
is unchanged. After the repair both diagnostic groups pass, and both existing
retained Belly Drum groups pass with Snorlax added to their cap coverage.
Logs: `work/preset-berry-review-native.log`,
`work/belly-drum-bound-review.log`.

This is not an all-level parity guarantee: at sampled levels 2/5, some Snorlax,
Zigzagoon, Linoone and Munchlax sets still have odd HP within the bounded search.
All sampled non-Sitrus Drum applications at levels 10/12/14/20/30/50/100 were
even. Re-leveling and Circuit construction remain separate paths to review.
Substitute/Drum HP arithmetic is labeled non-battle evidence: Eiscue's Salac
does not necessarily activate after exactly one Substitute plus Drum, nor does
Hawlucha's Sitrus necessarily activate after exactly two Substitutes. Their
roles must be judged against actual intended timing, not a universal modulo
rewrite that might break a different level.

The scratch checker itself was then corrected before relying on its item
flags. Its old berry-only inventory omitted eleven Berry Juice presets and
misclassified the unused Oran case. It now preprocesses current item hold
effects, parameters and pockets; Oran is half-HP/fixed10, Berry Juice is
half-HP/fixed20 and not a berry-pocket item. Sitrus remains ceil-half/floor25%.
The inventory is now 549 threshold-item sets, including 49 Substitute/Drum
sets (formerly 538/46). Gluttony eligibility and actual threshold changes are
separate fields; repeated Substitute arithmetic models consuming the item only
once, with restoration and other battle gates explicitly excluded.

The three newly included setup sets are the two Berry Juice Poliwag Drum
presets and Gastly's singles Hex/Substitute set. Poliwag's raw level-14 spread
has43HP, but both native application probes already demonstrate42HP after
normalization (HP_EV96, total488): the raw arithmetic flag is **not** a newly
demonstrated application bug. Gastly's finite Juice recovery and all Salac/
Substitute timing remain level-dependent; automatic universal HP-modulo changes
are not justified. The shared threshold report describes raw authored EVs,
whereas the native application log includes the deliberate HP adjustment.

After the concurrent legendary-acquisition work settled, the consumer report
was refreshed against current code: 54 rare-wild replacements, seven tracked
ordinary-table Ultra Beasts, six visible encounters, 16 other native statics,
and distinct gift providers are now separated with their actual level and
preset-application paths. Its earlier blanket landmark-route account is
superseded, not gameplay authority.

## Next bounded work

The universal cutoff fixes the non-Sitrus Drum threshold without application or
re-leveling exceptions. Native complete-catalogue application now passes with
exact authored EVs; the raw HP inventory is regenerated under the new policy.
Retain the Substitute sets rather than imposing a universal HP modulo: Salac
Eiscue, Unburden/Sitrus Hawlucha and Berry Juice Gastly retain their different
setup/recovery roles, whose precise timing depends on level and prior chip.
This is compatibility/disposition, not a claim of optimal play at every level.
Resume E0001 against the current stat owner and actual starter-dependent teams:
the corrected counterfactual in the Gen 9 report rules out blindly restoring
the old Seed Bomb/Web line.
The 50-theme source inventory is planning evidence for subsequent individual
reviews, not permission to skip their native execution or main-chat ratings.

The current canonical and generated E0495 Glacia party already contain Walrein
(Thick Fat, Leftovers; Hydro Pump/Blizzard/Encore/Protect). The user's requested
signature species is preserved as a requirement, not marked individually battle
verified. Trainer sets may be bespoke and need not copy shared presets.

An unrelated recent Sootopolis dialogue used a C-only LegendarySignId constant,
blocking assembly. The enum was moved unchanged into a shared constants header
and included by both C and event scripts; no numeric IDs or dialogue were altered.
The rebuilt native diagnostic passed. Birch and nurse explanations now describe
EVs rather than the superseded 66/32 system.
