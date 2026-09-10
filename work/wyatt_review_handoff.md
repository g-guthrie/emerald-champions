# Historical Wyatt baseline notes — superseded

The accepted final native review is now in
`docs/trainer_authoring/e0110_wyatt_gen9_2026_09_10.md` and
`work/wyatt-gen9-final.log`. The following baseline notes are historical, not
current build handles, party inputs or frontier instructions.

Previous goal turn made progress: Lung completed and shared repeated-status
filter repaired. Current ledger130/516 branches,101/468 records. Lung main-chat
P48232 at2026-09-10 20:47:27.715 UTC; exact further-improvement quote saved.
Lung and repair docs are current, not the older superseded investigation notes.
Wyatt is the last separate battle in E0110; completion would be131/516,102/468.
Next record E0111 Jaylen, not yet reviewed.

No live build: Wyatt baseline session30414 completed exit0.
Current scratch include at end of test/battle/ai/emerald_champions_plans.c is
../../../work/wyatt_gen9_probe.inc. Remove after review/shared checks.
No Wyatt canonical or AI changes have been made yet.

## Current source/native baseline

Route113 scripts133: own trainerbattle_double TRAINER_WYATT. Lawrence's E0110
doc contains approach/dispatch proof; no merge with Lawrence or Lung.
Native all38, threebadge cap40, universalIV31:
- Gabite Eviolite Rough Skin Adamant PS:111/119/66/48/58/103.
- Pupitar Eviolite Shed Skin Adamant PB:136/114/70/59/69/55.
- Shelgon Lum Overcoat Jolly PS:109/112/92/55/54/85.
- Turtonator White Herb Shell Armor Modest SS:105/68/119/137/81/68.
See canonical block1310 onward. No Mega. These four species have few nearby
duplicates; later Turtonator has another WhiteHerbSmash or bulky BodyPress.

work/wyatt-gen9-baseline.log:2groups22cases pass (20two-turn core prefixes and
2complete timing). Complete46full/19isolated, initial43/18.

Full after2:
- Physical EE DoubleEdge atPupitar / Gurd IcePunch atGabite: GabiteProtect1,
  attacks2 and falls; Turtonator enters. EndTurt105/Pup92,EE0/Gurd136.
- XatuPsychic atGab / PachiSuperFang atPup: Gab47/Pup68,Xatu0/Pachi11.
- FO atGab/ScaldPup: bothProtect1, bothattack2; Pupfalls,Turtenters.
  EndGab71/Turt105,Mien33/Lom135.
- DugEarthquake/XatuAirSlash atPup: GabProtect1/PupRockSlide1,attack/guard2;
  endGab69/Pup33,Dug0/Xatu36.
- Taunt atGab/GigaDrainPup: bothProtect1, attacks2; endGab71/Pup4,Mien0/Lom135.

Isolated Shelgon/Turtonator after2:
- Physical ShelgProtect1/TurtHeatWave1, thenDragonClaw+DragonPulse focusGurd;
  endShelg5/Turt65,EE76/Gurd0.
- Special: endShelg6/Turt27,Xatu0/Pachi46.
- FO/Scald: endShelg67/Turt13,Mien0/Lom135.
- Ground/AirSlash: Turtfalls1, Shelgonfalls2; foes90/114.
- Taunt/Giga: endShelg67/Turt83,Mien0/Lom135.
Need actual full logs before describing setup/HelpingHand; not yet verified.

The full opening doubleProtect against Mienfoo is permitted by the existing
PairWaitingHasPayoff first-turn FakeOut condition, not necessarily a new bug.
Both actors attack2 in the observed cases. The sixbench benchmark also includes
Mienfoo with a known FakeOut, regardless of controlled actual Protect command.
Do not claim AI reads player commands or classify this as repeated Protect spam.

Potential authoring to evaluate, NOT APPLIED:
- Turtonator Modest68Speed→136 afterShellSmash would remain belowXatu138 and
  Dug151. Timid may supply74→148 (must measure), tradingSpA for passingXatu;
  still not a Dug outrun. PreserveWhiteHerb defensive restoration and meaningful
  actual setup/boosted attack, not printed planned moves.
- Shelgon85→127 afterDragonDance cannot passXatu138, but passesMien103/Pachi93.
  Lum→Eviolite may make its setup realistic; genuine status-cure tradeoff.
- Check a meaningful Fairy counter in addition to existing Ice/Water/Ground.
  Keep party identity and don't overfit every counter to force wins.

## Shared fix accepted this turn

src/battle_ai_pair.c BuildPairActions existing known-failure filter now also
excludes pure immediate single-target EFFECT_NON_VOLATILE_STATUS versus an
already-statused foe when another candidate remains. Damaging secondaries,
allied actions, spread/random/smart moves and delayedYawn unchanged. All-invalid
fallback retains original legal actions; no inventedStruggle. No new buffers,
allocation or recursive search. No releaseROM/Downloads replacement.

Frozen native pre-authoring Shed/Veno two-turn regression failsWispPP13≠14
before, passes after; separate2controls preserveFlamethrower onburnedfoe and
onlyWisp forcedfallback. Two retainedgroups inemerald_champions_plans.c,
3cases total. work/status-legality-before.log, status-legality-after.log.
59/59retainedgroups pass afterLungscratchremoved in
work/ai-shared-through-lung-final.log. FullLung+shared61groups pass in
work/ai-and-lung-final.log. Lung2groups36cases final, complete60/25initial58.
All docs, inventory, ledger, canonical comparisons and provenance updated.
