# Lung review completed — 2026-09-10

SUPERSEDED NOTES BELOW. Current completion is documented in
docs/trainer_authoring/e0110_lung_gen9_2026_09_10.md and
docs/trainer_authoring/redundant_status_repair_2026_09_10.md.
Main-chat P48232 at20:47:27.715 UTC,8.5/10; exact backlog updated.
Ledger130/516 branches,101/468 records. Wyatt is next.
Shared filter repaired spent immediate status against already-afflicted foes;
59/59 retained groups pass, including frozen before/after burn regression and
damage/forced-fallback controls. Lung2groups36cases pass on final inputs.
WeezingPayapa, ShedinjaProtect, VenomothCoba228HP252SpA28Speed are accepted;
Ninjask unchanged. Native Venomoth134HP104Speed156afterQuiver. No release build.
The old in-progress record is retained below only as investigation history.

Lawrence is complete, documented and reported in main chat P47947 at
2026-09-10 20:31:21.562 UTC; further_improvements has the exact quotation.
Ledger129/516 branches,101/468 records. E0110 Lung and Wyatt remain.
No live build: Lung status-review session60603 ended exit0.

Current scratch include at end of test/battle/ai/emerald_champions_plans.c:
../../../work/lung_gen9_probe.inc. Remove after completing review/shared check.
No Lung canonical/team/engine change has been made yet.

## Actual source and baseline

Route113 scripts102 is its own double; Lawrence doc contains dispatch proof.
Native all38, three-badge cap40, universalIV31:
- Weezing Black Sludge Levitate Bold WD:133/76/144/81/70/62.
- Ninjask Sharp Beak Speed Boost Jolly PS:106/116/50/48/54/178.
- Shedinja Safety Goggles Wonder Guard Jolly PS:1/109/50/35/39/78.
- Venomoth Black Sludge Tinted Lens Timid SS:113/59/62/116/73/127.
Keep species context: earlier Shedinja Sash; later Ninjask Sash/Baton Pass,
other Weezing Gas/Slaking or Levitate defense, later Venomoth Sash dual STAB.
Current Venomoth replaces historically incoherent special Dancer beside SD.

work/lung-gen9-baseline.log:2 groups22cases (20 two-turn prefixes +2 timing).
work/lung-gen9-status-review.log repeats same inputs with player-status logging;
both pass but tactical review remains unfinished. Complete timing58 full/29
isolated, initial51/26. Full six-bench sample voluntarily switches Weezing to
Shedinja1 while Ninjask attacks, not Mega. Speed Boost178→267→356 observed.

Core full after2:
- Physical EE DoubleEdge/Gu IcePunch:Weez81/Ninj106,EE109/Gu0.
  NinjProtect1 thenDualWingbeat2;WeezSludgeBomb, not demonstrated burn support.
- Xatu Psychic left/Pachi SuperFang right:Weez dies1;Shed enters;after2
  Shed1/Ninj53,Xatu3/Pachi135. Next3 Sneak/Protect planned, not executed yet.
- FO/Scald:finish2 atWeez130/Ninj39; Ninj attacks both turns, no SD.
- Dug Earthquake/Xatu AirSlash right:Weez voluntarily→Shed1, NinjProtect;
  Ninj attacksDug2 and falls toAirSlash, Venomoth forced entry.
  ShedWisp2 targets Dug after partner damage; need distinguish target death.
  End2Shed1/Veno113,Dug0/Xatu114.
- Taunt atWeez/GigaDrain:NinjWingbeat andWeezSludge finish2 at133/106.

Isolated reserves after2:
- Physical uses EE **Bite** atShed, not uselessDoubleEdge. Venomoth sleepsEE1
  beforeBite, ShedXScissor1/2, VenoProtect2. EndShed1/Veno75,EE41/Gu155.
- Special currentlyXatu**Psychic atShed** (immune),SuperFang atVeno:
  Sneak1/2+BugBuzz2, Xatu dies2;Shed1/Veno64,Pachi135. This is NOT meaningful
  anti-Shed pressure. Change reserve Xatu toAirSlash atShed (full may retain
  Psychic atWeez); run corrected fixture before claims of coverage.
- FO/Scald:finish2Shed1/Veno69; Ghost immunity preventsFakeOut atShed.
- Ground/AirSlash:ShedWisp1, VenoProtect1; Veno choosesQuiverDance2 but dies
  BEFORE executing it,PP remains20. Shed repeatsWisp2 despite already-burnedDug.
  End2Shed1/Veno0,Dug80/Xatu114.
- Taunt/GigaDrain:finish2Shed1/Veno108, attacking fallback.

## Confirmed next investigation: redundant burn

In isolated Ground/AirSlash, status-review log around310–344 proves:
turn1 Wisp PP15→14, DugHP90→85,status1=16 (burn).
turn2 Wisp PP14→13 again at same Dug,HP85→80,status1 still16.
This is actual repeated attempt, not just a printed future decision or inferred
burn. Diagnose scoring before authoring around it; no fix implemented yet.

Source checked:
- battle_ai_main.c3278 nonvolatile burn branch only deducts10 if !AI_CanBurn
  and5 if !ShouldBurn; does not hard-reject useless pure status.
- battle_ai_util.c4380 AI_CanBurn delegates CanBeBurned and substitute/partner.
- IncreaseBurnScore5610 only adds if AI_CanBurn.
Need inspect pair candidate/bonus scoring and actual scores; don't assume cause
or expand to a new unbounded evaluator. A compact existing-status rejection for
pure status moves may be appropriate if evidence supports it; damaging moves
with burn as secondary must remain legal. Keep useful burn and setup behavior.

Potential team improvement to evaluate (NOT applied): Payapa on physically
bulky Weezing could save it from opening Psychic; preserve high Defense and
Levitate. Ninjask already178 before Boost, so consider whether some excessSpeed
can support bulk while retaining actual useful benchmarks. Venomoth needs real
sleep/Quiver attacking payoff and meaningful Flying/rock counterplay sampled.
Do not demand a win against every counter or solve this by forcing SD/Quiver.

Lawrence final accepted evidence:
work/lawrence-gen9-speed-candidate.log3groups39cases. Crad244/0/4/252/0/8 gives
148HP/50Speed and wins isolated physical5 at60HP (old49Speed lost5).
Shared57/57 inwork/ai-shared-through-lawrence-final.log with scratch removed.
Docs and canonical checks complete. Frozen release ROM remains unchanged.
