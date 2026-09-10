# Exhausted-bench pivot cleanup — native repair

Found during E0091 Angelo's actual reserve review, 2026-09-10. This is a
shared native move-resolution defect, not a trainer-specific reward change.

## Reproduction and cause

Rotom/Kilowattrel versus Mienfoo/Lombre, no healthy NPC reserves: Kilowattrel
uses Volt Switch on Mienfoo, Rotom finishes Mienfoo, then on the next turn
Kilowattrel spends Volt Switch PP against Lombre without damaging it. Rotom
heals Kilowattrel with Thunderbolt, but the supposed partner attack is skipped.
The original autonomous sample ends3 with Kilowattrel fainted and Lombre64HP.
This is not Volt Absorb redirecting an enemy attack or a miss: native message/
HP traces show the selected target2 and no HP event on that second attack.

The same two-turn sequence with all moves manually specified leaves Lombre
at104/104 and fails its damage expectation. Thus it reproduces without AI
decisions. Source: `MoveEndHitEscape` queued an escape script without checking
whether the attacker had a healthy replacement. The script's cannot-switch
branch exits via `end`, skipping the remaining move-end cleanup. Per-target
state can then suppress a later move. Parting Shot has the same early-exit
path after stat drops, so a subsequent use can fail to lower the new target.

## Small shared correction

In `src/battle_move_resolution.c`, require the existing native
`CanBattlerSwitch` check before starting the hit-and-run escape sequence or
Parting Shot's escape sequence. With no reserve, normal damage/stat drops
still happen and move-end cleanup continues; no replacement is queued.
The existing native helper respects party ownership and excludes active,
fainted, empty and egg slots. It does not treat trapping as an exhausted bench.

An initial experiment changing the script's blocked return was withdrawn:
it resumed cleanup but left an already queued replacement, wrongly removing
Kilowattrel from the field. No changes from that script experiment remain in
`data/battle_scripts_1.s`. The accepted repair prevents the invalid escape
sequence before it starts. No AI search, new reward, recursive evaluation or
per-trainer exception was added.

## Evidence

- `work/angelo-gen9-native-heal-hp.log` and
  `work/angelo-gen9-native-heal-targets.log`: original actual message, target
  and HP traces. Volt Switch spends PP without an HP event on turn2.
- `work/angelo-gen9-no-ai-control.log`: manually commanded reproduction
  fails, Lombre104/104. The initial trace-only fixture accidentally omitted a
  dead actor's later command; the harness tried to supply a fifth move. That
  fixture issue was corrected before obtaining this reproduction.
- `work/angelo-gen9-native-no-reserve-fixed.log`: the unchanged commanded
  reproduction now damages Lombre104→64. Autonomous follow-through finishes3
  at Rotom74/Kilowattrel18, versus the old Kilowattrel faint/Lombre64 endpoint.
- Retained `EC pivot cleanup: exhausted bench does not skip the next target`
  is one compact four-case mechanic regression, independent of authored team
  data: Volt Switch, U-turn, Flip Turn and Parting Shot. It checks the first
  foe actually fainted, the second foe receives damage/stat drops, targeting
  cleanup is clear and the attacker was not incorrectly removed from play.
  Its stats are frozen diagnostic inputs, not trainer-rating evidence.
- `work/angelo-pivot-shared-parting-control.log`: with only the damaging-pivot
  correction, the Parting Shot case still fails: the second target stays at
  neutral Attack instead of-1. An earlier version incorrectly gave that first
  target a Sash when Parting Shot dealt no damage; removing that invalid setup
  lets the first target actually faint and exposes the meaningful failure.
- `work/angelo-pivot-shared-fixed.log`: all four exhausted-bench cases pass
  after both guards. The original real-party Volt Switch before/after control
  and the frozen Parting Shot negative control establish separate failures;
  do not claim all four cases were independently reverted and rerun.
- `work/angelo-gen9-accepted.log`: four temporary available-bench controls
  still switch to Luxray with all four moves, even against Shadow Tag. These
  ensure the helper does not accidentally block legitimate escape moves.
  The actual Angelo review also exercises full parties and natural entries.

Proactive source review: Eject Button, Eject Pack and trainer Emergency Exit
already test replacement availability; no speculative changes to those paths.
Arena/Commander and every possible forced-switch interaction were not given
new exhaustive coverage. The repair is shared, but does not retroactively
certify every previously reviewed battle's outcome or timing.

Temporary message/HP logging and encounter includes are removed. The single
retained regression protects a demonstrated serious native failure; no giant
per-trainer test corpus. Canonical trainer/preset checks still match. Angelo's
sampled complete AI decision remains54frames (~0.90s); the repair is in move
resolution and adds no AI search. No release ROM, ROM-size claim or Downloads
replacement: the frozen user ROM does not include this repair.

`work/ai-shared-through-angelo-final.log`:57/57 retained shared native groups
pass, including the new compact pivot regression, with encounter scratch cases
removed. This is focused shared coverage, not whole-campaign revalidation.
