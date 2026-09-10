> Historical flat-stat review. Current Gen9 evidence and the approved Gothita
> move exception are in [the final-stat review](e0019_gen9_2026_09_09.md).
> Old stats, thresholds and completion counts below are not current evidence.

# E0019 — Marlene: bounded executable-AI review complete

Intention rating: **8/10**, not a measured win rate. The native Route115 script
calls `trainerbattle_double TRAINER_MARLENE`: one current branch/encounter.
Progression, party size, level calibration and species are unchanged. This
record supersedes the earlier source-only review, not the executable code.

## Accepted loadout and instructions

Normal zero-badge generation gives all four level 12:

| Pokémon | HP/Atk/Def/SpA/SpD/Spe | Item and ability | Moves |
| --- | --- | --- | --- |
| Spoink | 72/14/33/25/49/20 | Eviolite, Thick Fat | Trick Room, Psychic, Power Gem, Protect |
| Slowpoke | 79/24/26/55/18/10 | Sitrus, Regenerator | Psychic, Hydro Pump, Yawn, Protect |
| Baltoy | 67/18/23/55/25/18 | Eviolite, Levitate | Earth Power, Psychic, Ice Beam, Protect |
| Gothita | 68/15/22/58/24/17 | Focus Sash, Competitive | Psychic, Energy Ball, Helping Hand, Protect |

All 16 moves, selected abilities and 66-point totals were checked against
current native preparation/generation. Displayed stats precede item modifiers.
Slowpoke, Gothita and Baltoy are slower than the tested 19-Speed Timburr; Spoink
is not. Room therefore benefits the team unevenly, not automatically every ally.
Priority Helping Hand remains useful independently of ordinary Speed order.

- **Spoink: 32 HP/2 Def/32 SpD → 32 HP/16 Def/18 SpD.** Native Def rises from
  19 to 33, SpD falls from 64 to 49; HP, Speed, nature and all other attributes
  remain. Under simultaneous native Poochyena Crunch/Pachirisu Thunderbolt,
  the old spread double-protects first, establishes Room on turn 2 and ends at
  2 HP while Slowpoke switches out. The mixed spread establishes Room turn 1,
  then protects while Slowpoke attacks: Spoink ends at 30 HP, Poochyena at 31.
  A full physical alternative (32 Def/2 SpD) ends at 6 HP after attacking on
  turn 2. Mixed investment retains more special bulk and a useful safe line;
  these are observed choices, not a claim of universal spread superiority.
- **Baltoy: Leftovers → Eviolite.** On the native Dark reserve board, old
  Leftovers Baltoy faints to Crunch before attacking; Eviolite leaves 13 HP
  and permits Earth Power. It also starts Helping Hand/Earth Power one turn
  earlier on the physical board before the pressure change. Its new item
  sacrifices 4 HP of end-turn recovery for immediate defense.
- **Executable plan: TRICK_ROOM → TRICK_ROOM|PRESSURE.** This changes the
  paired evaluator's HP-trade preference, not just prose. Slowpoke now attacks
  alongside a safe Room setup on the physical board, and Yawns immediately
  instead of joining an empty double guard on the Dark/Storm Drain board.
  Necessary Protect and switches remain available; the Dark reserve opening
  still double-protects. No fixed move script or pending player command is read.

Existing Room, Regenerator and Helping Hand logic was inspected and exercised,
not rewritten for a change quota. No new shared evaluator code, allocation or
permanent regression was needed for these individually authored improvements.

## Native evidence

Final run: **30 battle parameters plus 3 initialized field-state queries across
8 groups, all executed**. Battle parameters comprise 12 lead/transition,
4 timing/calibration, 7 Yawn, 4 reserve and 3 Competitive observations. Yawn's
three mechanical controls prescribe moves; all other opponent decisions use
the full compiled menus. Reserve snapshots explicitly promote the real bench
with original leads fainted. A diagnostic PASS alone is not tactical proof.

- Against Pachirisu/Timburr, Room/Psychic starts on turn 1; concentrated attacks
  remove Pachirisu on turn 3, and leave Timburr 24 HP on turn 4. Room remains
  active at timer 1, own HP 48/30. No refresh cancellation occurs in this line.
- Against Pachirisu/Lotad, Slowpoke switches to Baltoy on turn 2. Eviolite
  Baltoy takes Giga Drain and retains 35 HP rather than old Leftovers' 23.
  Protect preserves it on turn 3, then Earth Power removes Pachirisu on turn 4
  while Baltoy survives at 3 HP. The player could choose different targeting.
- Fake Out followed by Taunt on Spoink genuinely denies Room. The team attacks
  instead: Mienfoo falls on turn 3. On turn 4 Slowpoke switches at 22 HP and
  Regenerator restores its party HP to 48; Baltoy receives Thunderbolt without
  damage. This is a native voluntary pivot, not a forced regeneration control.
- Actual Dark/Storm Drain pressure produces Yawn on Poochyena turn 1, Yawn on
  Shellos turn 2, then Psychic pressure. Poochyena sleeps at the end of turn 2,
  Shellos at the end of turn 3. Both targets still get their pre-sleep actions.
  Separate mechanical controls confirm this delay and immediate Chesto cure.
- **Sleep Clause is disabled in current production and these observations.**
  `B_SLEEP_CLAUSE` is false and `B_FLAG_SLEEP_CLAUSE` is 0; native enabled/active
  logging confirms false. Consecutive Yawns are not a demonstrated clause bug.
  No gate was added based on an assumed rule. This does not change sleep rules.
- Under Snarl/Scald, Gothita chooses Energy Ball twice while Competitive raises
  its net SpA stage to +1, then +2; it reaches +3 on turn 3 and finishes Shellos
  with Psychic. Baltoy survives at 11 HP; Gothita remains at 44 HP. Thus common
  special drops are counterplay with a real downside, not a blanket answer.
- Three explicit Room-state queries use the generated team after native AI
  initialization: absent Room selects Trick Room/Psychic; active timer 2 and
  expiring timer 1 select Psychic/Psychic. Field/RNG are restored. These are
  decision queries, not a natural six-turn expiry/restart playthrough.

Final four/six-player timing is 28 warm frames and 34 with cache rebuild, versus
40/46 originally. The changed loadout alters candidate work; this is a local
sample, not a global latency guarantee. Easy/Hard native fallback generation
gives level 10/14 for all four. All temporary Marlene includes were removed;
the clean **31-group shared run passes**, including transaction/RNG restoration,
Room expiry, weather and partner interactions. No permanent tests were added.
Master projection verifies 516 branches; diff check passes. Neither proves
individual completion for the still-unreviewed encounters.

## Variety and remaining limits

Spoink and Slowpoke appear only here in the current compiled catalogue. Baltoy
and Gothita each appear twice, also with nearby Sarah's Room team. All debut
here. Keep Marlene's unique Psychic/Regenerator identity; revisit Sarah's shared
reserves when her individual review arrives instead of editing later fights now.
Early Poochyena, Mienfoo, Nymble and Woods Heracross provide Dark/Bug/Taunt
pressure. Slow Ferroseed can contest Room. Storm Drain denies Hydro Pump but
does not deny Psychic or Gothita's super-effective Energy Ball on pure-Water
Shellos. Hydro Pump accuracy and the team's shared Dark/Ghost/Bug weaknesses
are retained, not removed to make the encounter unbeatable.

To move above 8: differentiate it further from Sarah's nearby Room roster,
improve remaining unproductive defensive turns under Dark pressure, and expand
endgame/alternative-target coverage. Yawn still has native raw preference but
no complete delayed paired-state forecast; the observed sequence works without
claiming that general solver exists. The single Room setter can be denied, and
this is a coherent competitive team rather than an unusually elaborate combo.
No fresh release ROM, full campaign integration or exhaustive playthrough is
claimed. Restart progress: **23/516 branches, 18/468 encounters**. E0020 next.
