# E0008 Cindy — executable-AI review complete, 8.5/10 intention

Historical flat-stat review. Current completion and evidence:
[final Gen9 practical pass](e0008_gen9_2026_09_09.md).

September 9, 2026. Actual compiled TRAINER_CINDY_1, Normal, zero badges:
all four Pokemon are level 12 after native difficulty processing. This is
bounded scenario verification, not a complete playthrough or measured win rate.

## Final team

Stats are native HP / Atk / Def / SpA / SpD / Speed, before held-item and ability
damage modifiers. All assigned moves and abilities were checked against current
preparation arrays and species data, not the Cohesion Book.

| Pokemon | Native stats | Role and changes |
| --- | --- | --- |
| Snubbull | 72 / 64 / 22 / 16 / 18 / 15 | Lum, Intimidate, Adamant. Fire Punch replaces Fire Fang; Play Rough, Thunder Wave and Protect retained. |
| Milcery | 68 / 16 / 22 / 36 / 39 / 16 | Eviolite, Aroma Veil, Bold. Points become 32/0/2/16/16/0; Draining Kiss, Dazzling Gleam, Helping Hand and Recover retained. |
| Furfrou | 75 / 27 / 25 / 21 / 68 / 33 | Sitrus, Fur Coat, Careful. Double-Edge, U-turn, Sucker Punch and Snarl retained. |
| Slakoth | 72 / 60 / 25 / 15 / 17 / 15 | Choice Band, Truant, Adamant. Retaliate replaces Night Slash; Body Slam, Hammer Arm and Rock Slide retained. |

Milcery trades special bulk for meaningful attacks: SpA 20→36, SpD 55→39
(Eviolite approximately 82→58). Physical bulk and Speed stay unchanged. Snubbull
trades Fang's rare order-dependent flinch for Punch's 75 power/100 accuracy
instead of 65/95. Both retain the native burn chance. Slakoth no longer exactly
duplicates Allen's set; its revenge option costs Dark coverage and only pays
on the turn after an ally faints. Body Slam remains the normal STAB option.
Truant and Choice lock are not bypassed. The other three species are their first
trainer appearances in this campaign sequence. No species, level, party-size or
progression change. Conservative remains; strategy NONE adds no setup bias and
does not disable executable AI. The plan/crack text is explanatory only.

## Executable repairs and observed evidence

1. **Recover in the paired turn forecast.** Previously the raw single-move
   opinion existed, but the paired forecast skipped its HP restoration. Against
   native slower Munchlax Body Slam plus faster Pachirisu Round, actual Milcery
   starts at an explicitly initialized 20 HP. With both reserves fainted, old AI
   chose Draining Kiss and lost Milcery. The repair chooses Recover and survives:
   29 HP with the old spread, 28 with the final spread. With living reserves the
   old Furfrou switch was already valid; that was not labeled a failure.
   Native forced controls show Recover heals 34 before Body Slam; Kiss heals
   only 3 initially or 5 with the new spread and still loses this position.
   The forecast uses action-time HP, native floor-half non-Dynamax max HP,
   Heal Block, a max-HP cap and modeled action probability without applying
   survival twice. Initially full Milcery can heal after a faster hit: native
   Round does 4 and Recover restores 4. Aroma Veil blocks native Heal Block;
   after real Skill Swap removes Veil, Heal Block cancels the selected Recover.
   That Elgyem/Natu control is mechanics coverage, not an opening-access claim.
   Earlier potential Snatch withholds new healing credit; redirection is not
   simulated. Rest, Roost and weather healing have different effect contracts.

2. **Snarl's same-turn Special Attack reduction.** The bounded stat forecast
   now has a SpA column, cached native eligibility, sound/secondary-effect
   blockers, Simple/Contrary adjustment, Defiant/Competitive and White Herb
   order. Competitive acts before Herb, so its net positive stage retains Herb.
   Net stage changes are applied once to avoid false 100→66→99 restoration.
   Unaware and guaranteed-critical stage bypass are respected. Furfrou actually
   chooses Snarl against two slower native Lotad: both fall to -1 SpA on turn 1
   and -2 on turn 2. Forced native Jigglypuff controls deliver Ice Beam damage
   26 without Snarl, 17 after Snarl, 26 after Herb restoration, and 39 after
   Competitive with Herb retained. Soundproof Whismur takes no Snarl damage/drop.

3. **A second plausible opponent targeting pattern.** Native decision logs
   exposed a fixed-target trap: both enemy attacks were forecast into Snubbull,
   then Protect made the unprotected Slakoth appear safe. Under actual focused
   Super Fang + Adaptability Eevee Double-Edge, Milcery fainted, Slakoth entered,
   selected Retaliate and fainted before acting. This was not missing native
   Speed, dynamic Super Fang or survival calculation; it was the single target
   assumption. The existing opponent-action enumeration now retains its original
   best forecast plus the best different nonempty damaging-target mask. Each
   proposed response is scored against the worse of those two; candidates that
   cannot beat the incumbent skip the second calculation. No pending human
   command is read, and no additional native damage calls occur inside the
   pair walk. Explicit forecast logs confirmed both opposing focused patterns
   for the exposed lead/Slakoth positions. Repeating the same two-turn pressure
   now pivots Milcery to Furfrou, preserving Milcery 68 HP and Slakoth 72 HP;
   only Furfrou falls, rather than both Milcery and Slakoth. Reversing the human
   focus onto Snubbull instead leaves Milcery 68, Furfrou 71 and Slakoth 72 after
   Snubbull falls. This is not immunity to focused attacks or a guaranteed win.

4. **Retaliate's replacement timing.** Post-KO candidate evaluation previously
   refreshed damage before the upcoming native timer decrement. Ordinary
   completed-turn replacement trials now forecast 2→1 (active revenge), 1→0
   (expired), and 0 unchanged, restoring both timers afterward through the
   existing snapshot. Initial-entry hazard and mid-turn replacements are
   excluded because they do not have that same pending tick. Five initialized
   native boundary probes observed those trial timers and checked party/HP,
   slot, both timers and RNG restoration. Native loaded Slakoth damage against
   the reference Eevee is 30 expired versus 58 in the window. Separately, a
   genuine forced Snubbull KO followed by Slakoth's next-turn attack produces
   18 Body Slam damage versus 30 Retaliate damage against bulky Pachirisu.
   That comparison proves the window, not unrestricted AI execution or a reason
   to sacrifice Snubbull. The unrestricted vulnerable Retaliate attempt above
   was explicitly not counted as a successful revenge attack.

## Other native decisions and verification

- Against physical Timburr/Eevee, the final leads use Protect/Gleam, then Play
  Rough/Gleam; own HP is 18/58 and opposing HP 35/19 after two turns.
- Against Charmander/Electrike, Snubbull pivots to Furfrou and Milcery Recovers,
  ending Furfrou 59/Milcery 68. Old-spread comparison instead drains and ends
  Milcery 50, with slightly more enemy damage. This is a bulk/offense/action
  tradeoff, not a universal improvement or a measured win-rate assertion.
- Mienfoo's Inner Focus correctly prevents Intimidate; Aroma Veil prevents its
  Taunt. The final first turn combines Play Rough and Gleam to remove Mienfoo
  while damaging Pachirisu. Milcery is not generic status-immune.
- Against native Woods Ferroseed/Sewaddle, the actual second turn selects
  Helping Hand plus fire coverage. Fang leaves Sewaddle at 1 HP in the sample;
  Punch removes it. Both leave Cindy at 22/59 HP. The sampled Fang burn is not
  guaranteed. Thunder Wave still does not make these slow leads universally
  faster: a 64-Speed foe becomes 32, above 15/16 but below Furfrou's 33.
- Actual reserve continuations preserve Choice lock and Truant's lost turns.
  After Protect denies its active attack, Slakoth does not receive a fictional
  extra attack on its loafing turn. Furfrou uses attacks versus physical pressure
  and Snarl versus special pressure. U-turn's immediate replacement Intimidate
  is still not simulated by the paired scorer; no tested pivot-cycle claim.

Final checkpoint: **42 parameters / 13 native groups pass**, including five
explicitly initialized timer-boundary probes. All use native records/calculation;
forced mechanics controls are distinguished above from unrestricted decisions.
The shared 16 coordination/restoration regressions also pass. An 11-parameter
Billy lead/reserve/timing rerun retains the Helmet-safe Super Fang/Protect and
native Imposter behavior. Actual six-player Cindy timing: **25 GBA frames warm,
32 including cache rebuild (~0.54 seconds)**. Billy's heavier board is 36/43
frames (~0.72 seconds including rebuild), versus 34/41 at its prior checkpoint.
Neither is a campaign-wide worst-case latency guarantee. Temporary source
logging was removed before the final Cindy and Billy runs.

Only E0008 was newly materialized; the master/party consistency check passes for
all 516 branches, which is not 516 battles of AI evidence. Temporary probes are
removed from the normal include list after this checkpoint, not installed as a
permanent per-trainer suite. No new release ROM, commit or push in this restart.

## Remaining limits and rating

**8.5/10 intention.** Cindy now has reliable support/coverage, complementary
defensive abilities, actual recovery decisions, a special-attack suppression
reserve and a distinct revenge option. A richer second pivot or better support
for Slakoth's narrow active window would improve her tactical ceiling. Protect,
Ghosts, focus pressure and stat-drop-punishing abilities remain real counterplay.

The two-forecast model omits the third target mask and alternative moves within
a retained mask; zero/negative-value alternate masks are omitted. It is not a
full adversarial search or an opponent status-plan simulator. Snarl still uses
cached damage rescaling, not native recalculation after every hypothetical
stage change; Mirror Armor reflection and capped Weakness Policy ordering are
not fully modeled. Independent effect chances, critical distributions, item
histories and retargeting retain the existing bounded approximations. Those
limits are not proof of exhaustive correctness, nor a reason to script a
preferred move regardless of survival.
