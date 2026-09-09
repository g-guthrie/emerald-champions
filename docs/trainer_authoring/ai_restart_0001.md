# E0001 executable-AI restart — reviewed and repaired

Latest user instruction: restart from Battle 1, fix executable decisions while
handling each encounter, and report each completed battle separately in chat.
E0001's six trainer aliases have completed the bounded encounter review below.
This means actual-team scenarios and demonstrated AI repairs, not exhaustive
matchup certification or a full player-driven playthrough. Prior improvements
to later teams remain preserved and do not count as their AI verification.

## Completion report

The final current-code run executed12 diagnostic groups /49 parameters: six
Hoenn alias openings,24 regional substitutions, two Mienfoo-pressure boards,
two forced pressure lines, one reserve-immunity board, five Magic Bounce cases,
three Chespin cases, two final-KO positions and four Cyndaquil cases. The
unrestricted choices and forced mechanic comparisons are distinguished below.
All16 retained shared expert groups then passed, including state/RNG restoration;
the warmed full-party decision probe remained20 GBA frames.

Executable fixes: same-turn Electroweb/Icy Wind Speed forecasting; joint opposing
target forecasting without duplicate knockout credit; target-specific partner
Magic Bounce handling; remaining-HP percentage damage; deterministic Nature
Power called-attack/category/accuracy handling. The repaired Nature Power branch
now selects Energy Ball on Grass for18 damage instead of Extrasensory8; Plain
retains Extrasensory. Full-health Cyndaquil still selects Eruption with Electroweb.

Team changes from the prior authoring pass are retained, with no extra stat
inflation to compensate for AI failures. Intention ratings remain Treecko8.5,
Mudkip8 and Torchic8.5 for both genders. The regional ensemble is8: it has useful
different roles, but a shared support/reserve core does not make every variant
a bespoke10. Better branch-specific reserve transitions would deepen the team;
Treecko also overlaps Shroomish's Grass typing. These are design judgments,
not measured win rates. See batch_0001_0009.md for the individual loadouts.

Temporary E1 includes were removed from the normal test build after execution;
scratch probes remain in work/ as investigation artifacts, not a new permanent
49-case roster-lock suite. Decision-time logging remains temporary during the
ongoing sequential pass and must be removed before the final handoff.

## Native evidence so far

Temporary investigations use `work/rival_ai_probe.inc`, included by the existing
native battle-plan file. `AuthoredOpponent` reads the compiled trainer table,
calls production party/stat generation and Normal difficulty at zero badges:
all four actual rival members are present at level12. Player members are actual
level14 native-calculated legal sets, not hand-entered derived stats.

- Six Hoenn alias scenarios executed (three branches and two attack/disruption
  variants). Actual decision-time logs show Treecko Seed Bomb plus Pikachu
  Thunderbolt removing opposing Torchic; Mudkip Protect plus Pikachu Thunderbolt;
  Torchic Flamethrower plus Pikachu Fake Out on threatening Mudkip. These are
  scenario observations, not exhaustive matchup proof or regional-variant proof.
- Eviolite Inner Focus Mienfoo pressure: actual Speed64 sits between Pikachu68
  and Treecko62. AI chose Protect plus Thunderbolt, removed one Mienfoo and lost
  Pikachu; native replacement selected Taillow. This trade is not inherently bad.
- Sash Mienfoo counterfactual: forced Protect/Thunderbolt leaves foes1/42HP and
  loses Pikachu. Forced Seed Bomb/Electroweb removes one foe, leaves the other32HP,
  retains Treecko34HP and also loses Pikachu. The forced experiment demonstrates
  what those lines do against the specified attacks, not what an AI should know
  about unexecuted human commands.
- The revised speed forecast chooses Electroweb but Treecko still Protects,
  leaving both foes alive. Removing Fake Out from the legal opposing movesets
  does not change that. Investigating the actual forecast and heuristic scores
  before changing them. Diagnostic logs now confirm both independent opposing
  forecasts select Drain Punch into Treecko for180 points each, although native
  cached median damage58/54 can KO either Treecko37HP or Pikachu36HP. This
  duplicated knockout credit inflates Protect/Web to520 versus448 for
  Seed Bomb/Web. Solo heuristic difference is only4 points, not the cause.
- Native reserve scenario retains original party order with both leads fainted.
  Against Grass Treecko and Magic Bounce Natu, Shroomish uses Giga Drain instead
  of an ineffective/reflected Spore. Taillow uses Brave Bird immediately and its
  Toxic Orb activates afterward; following-turn selection is Facade. This is a
  specific reserve decision check, not a played full encounter.

## Executable changes under verification

Paired forecasting now includes Electroweb/Icy Wind's same-turn Speed effects
through a bounded additional four-action pass. Hit/no-drop outcomes are blended;
independent spread-hit combinations are compressed, not fully simulated. Native
immunity/Contrary/caps/White Herb and retaliation checks are included. This has
compiled and run the scenarios above but still needs narrow negative controls,
state-restoration and latency verification before acceptance.

A bounded joint opposing target forecast has now been implemented to prevent
duplicate overkill credit while retaining focus fire when multiple hits are
needed. Native rerun selects Seed Bomb/Electroweb in the Sash case and retains
the different Protect/Thunderbolt Eviolite trade. No access to the player's
unexecuted command/target is used. All16 retained `EC expert pair:` groups passed
after this change, including candidate state/RNG restoration. The existing
full-party warmed decision probe measured20 GBA frames; this is not a full
gameplay latency measurement or proof all battle positions are equally fast.

All24 non-Hoenn opening substitutions were then sampled against native level14
Mienfoo/Natu using the real public regional set function. The scratch fixture
mirrors only the field resolver's species/EXP transition; it does not exercise
the actual field script/starter-choice routing. Charmander used Heat Wave with
Pikachu Electroweb; Chimchar/Litten/Grookey used Fake Out on Natu while Pikachu
attacked Mienfoo; other branches used a mixture of attacks, protection and
Light Screen/Icy Wind. One sampled board per variant is not full AI certification.

The follow-up investigations identified Chespin's percentage-damage forecast
using stale HP and Cyndaquil Nature Power being classified as nondamaging status.
Magic Bounce on a target's partner no longer incorrectly suppresses a selected
Spore/Leech Seed. Four native execution parameters confirm both moves reach
Mienfoo beside either Magic Bounce or Early Bird Natu. Same-board score probes
retain Mienfoo scores102/105 with either partner ability, while directly targeting
Magic Bounce Natu lowers those scores to82/85. This tests target-specific scoring,
not a claim the AI must always select Spore.

The Chespin native counterfactual confirms the percentage-damage defect: actual
Thunderbolt deals40 to a72HP target, then Super Fang deals16 to the remaining32HP.
Using the cached starting-HP half would incorrectly predict the remaining32HP
as damage and invent a knockout. The actual unrestricted AI on this board used
Super Fang with Pikachu Protect; this is a proven forecast defect, not evidence
that this particular decision relied on the false knockout. Both paired forecast
paths now recalculate percentage damage using simulated remaining HP and native
inverse-Dynamax rounding, retaining zero for immunity/failure. All nine current
E1 diagnostic groups reran after the correction; the unrestricted Chespin choice
remains Super Fang/Protect. All16 shared expert groups passed again and the
warmed full-party measurement remains20 GBA frames.

The final-reserve probe ran two reachable positions using the actual Taillow
and Shroomish, with both leads fainted and native Natu the last opponent at1HP.
Taillow used Facade immediately and survived at37HP whether initially healthy
or already poisoned. No redundant Protect/Orb-activation turn was chosen. This
is a remaining-position check, not an uninterrupted full rival playthrough.

Source inspection confirms the private regional resolver uses the unchosen
starter index, replaces the Hoenn starter's species/EXP, applies the regional
set and recalculates stats. Same-species Hoenn branches return without replacing
the canonical set. The temporary substitution probes mirror those operations;
they still do not certify the field script or starter-choice UI end to end.

Native Cyndaquil investigation: the current map permits Grass and Plain battle
environments depending on approach; native Nature Power resolves to Energy Ball
or Tri Attack respectively. At a reachable9/37HP injured re-entry, actual Scarf
Cyndaquil chose Extrasensory with Pikachu Fake Out against native Fire-resistant
Marill/Bonsly. The fixed-line Grass comparison measured Extrasensory8 versus
Nature Power/Energy Ball18 damage against the same79HP/60SpDef Marill. Both
attacks executed through the native engine; the paired evaluator currently
skipped Nature Power's cached damage as if it were nondamaging status. The fix
retains original command priority, slot and status restrictions while resolving
the executed attack and native accuracy once per board. This fixture controls
the native environment; it is not a UI walk. Same-turn terrain replacement and
Prankster/Dark interactions were not exercised by this Flash Fire Cyndaquil case.

Earlier shared Coaching/Acid Spray checks passed five native parameters; Motor
Drive passed two. They are not E0001 or Wattson encounter-completion claims.

## Diagnostic corrections

The native harness selects next-turn commands before stopping at THEN. Reading
`gChosenMoveByBattler` or targets there mislabels the next turn as the previous
one; a fainted/replaced actor also resets last-move data. Temporary decision-time
logging in the existing runner now records turn, actor, move and target.
`AI_LOG` overflows its ten-line score buffer on real parties, so it is not used
for these diagnostic runs. The native printf formatter also does not support
`%u`; numeric logs use `%d` so arguments are consumed correctly. A diagnostic
PASS means execution, not correct AI.

Remaining limits: compressed spread hit/miss outcomes, not a complete reaction
tree; sampled regional/remaining-party boards, not all matchups; no field/UI
playthrough or release-ROM verification here. Later encounter reviews must
continue testing their actual executable mechanics rather than assuming these
shared fixes prove every team works.
