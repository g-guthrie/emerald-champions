# E0005 Tiana — executable-AI review completed

Historical flat-stat record. Current completion and evidence:
[final Gen9 practical pass](e0005_gen9_2026_09_09.md).

September 9, 2026. Intention rating: **9/10**, not a measured win rate.
Current compiled TRAINER_TIANA, native party
generation and Normal zero-badge difficulty produce four level12 members:

| Pokemon | HP | Atk | Def | SpA | SpD | Speed |
| --- | --- | --- | --- | --- | --- | --- |
| Skitty | 69 | 19 | 21 | 15 | 17 | 57 |
| Swirlix | 42 | 18 | 24 | 59 | 22 | 52 |
| Eevee | 70 | 58 | 22 | 17 | 24 | 21 |
| Buneary | 40 | 56 | 19 | 17 | 22 | 67 |

All sixteen compiled moves and four configured abilities were checked against
current game data. Species, items, flat points and level offsets are unchanged.
Wish's native35HP heal belongs to its field position
at the following end turn, not immediate recovery. Klutz suppresses Buneary's
own Orb, but its recipient can burn at end turn; it does not weaken that turn's
attack. Wonder Skin protects Skitty from incoming status accuracy; it does not
raise Sing's native55 accuracy.

## Baseline observations

- Bulky native Pachirisu/Eevee: Skitty Fake Out into Pachirisu plus Swirlix
  Protect on turn1; turn2 Skitty Protect and Swirlix switches to Eevee, which
  takes the repeated player attacks and ends9HP. This is not yet rated good.
- Native Mienfoo64Speed and Natu60 both precede Swirlix52. Turn1 Skitty protects
  while Swirlix Thunderbolts Natu; the supplied Strength leaves Swirlix22HP,
  just above Sitrus's threshold. Turn2 Strength activates Sitrus/Unburden;
  Swirlix then removes Natu before its Psychic, while Skitty's later Sing
  sleeps Mienfoo. Swirlix12HP/Skitty69HP survive. This sampled sleep hit is not
  a guarantee, and Sing did not deny Mienfoo's already-executed action.
- Later-board exact reserves, with defeated lead records retained: actual
  Eevee/Buneary use Protect/Fake Out then Double-Edge/Drain Punch under modest
  physical pressure. Healthy Iron Fist, Guts, Fire and already-poisoned controls
  do not produce an Orb transfer in these two sampled turns. Actual Protect
  bait also does not induce Encore on turn2. Third-turn selection may already
  appear in THEN; it is not an executed turn.
- Forced native comparison: actual Swirlix starts22/42HP, above Sitrus threshold,
  and attacks before native Timburr's Ice Punch. Thunderbolt leaves Timburr61HP
  and Swirlix faints; Draining Kiss leaves Timburr49HP and Swirlix20HP with
  consumed Sitrus/active Unburden. The same native board therefore demonstrates
  meaningful drain recovery, not that the unrestricted AI chose wrongly there.

## Completed repairs and authoring

The paired scorer had no same-turn denial for newly inflicted Sing, although it
had the raw future-sleep opinion and the previous Spore repair. Sing now shares
the bounded effect/no-effect pass with native55%-accuracy weighting. The cache
is keyed by actor, move slot and recipient, so simultaneous Sing/Spore menus
cannot overwrite distinct immunity results. Sing retains native Substitute
bypass and Soundproof protection; only Spore uses powder immunity. No extra
search branch or damage call. Native controls and shared safety reruns pass.

Dazzling Gleam over Energy Ball is now materialized for E0005 only: legal native
80BP spread Fairy damage gives Helping Hand a dual-target payoff, retains Kiss
healing and Thunderbolt coverage, and trades Grass coverage of Ground/Rock.
The final Mienfoo/Natu board chooses Gleam turn1 (foes26/53HP), then switches
Skitty to Eevee while Swirlix protects: Eevee55/Swirlix22HP, Skitty preserved
at69HP in reserve. This differs from the intermediate Sing/Protect result
before drain valuation. The spread option is not automatically better on every
supplied line. Against two native Inner Focus Mienfoo, actual AI chooses
Helping Hand/Gleam and removes both, ending Skitty1/Swirlix24HP. A forced
Helping Hand/Kiss comparison removes only one and leaves the other at42HP.
That is the concrete new coordinated payoff, not a general win-rate claim.

The current native Sing controls confirm Grass plus Safety Goggles does not
block Sing, a genuinely created Substitute does not block it, Soundproof does,
and a miss leaves the target's action intact. Whismur is explicitly a later-
access mechanic control, not claimed available before this trainer. Those
forced branches do not measure a55% hit frequency.

The exact reserve counterfactuals now establish:

- Encore after genuine Protect prevents the requested Ice Punch: Eevee63HP
  instead of46, Buneary10 instead of12, Pachirisu55 instead of48. The native
  lock is Protect with timer2; whether repeated Protect succeeds is separate
  from whether Ice Punch is denied. After the repair, actual AI now chooses
  Encore here and reproduces that denied attack. The bounded scorer replaces
  an unacted foe's command only for supported known guards, updates its native
  priority, and handles repeated guard success separately from Encore success.
  Native absent-history and Mental Herb negative controls allow the attack.
  Unsupported Encore targets retain the existing heuristic; this is not a
  complete arbitrary-move lock simulator.
- Switcheroo versus Drain Punch with the same next-turn forced Wish/Drain
  continuation: the trade does not reduce turn2 Ice Punch (17 in both), but
  turn3 falls from17 to8 after burn. Final Eevee38/Buneary7 versus29/8; the
  player ends69/48 versus74/41. Buneary receives Eviolite but Klutz suppresses
  its benefit. This is a real future trade, not instant damage prevention or
  proof the AI must choose it in the original two-turn board.

The unrestricted injured-Swirlix scenario exposed a real failure: before drain
valuation, it selected Gleam and fainted. The scorer now accounts for native
primary drain percentage, remaining target HP, effective Big Root, healing
cap before accuracy weighting, Heal Block and sanitized Liquid Ooze/Magic Guard.
Healing occurs before subsequent actions and can alter berry/Unburden timing.
It uses cached metadata, not more damage calls or search branches. Native
controls distinguish ordinary healing from Ooze damage followed by Sitrus;
the low-damage Big Root control rounds to the same3HP and does not demonstrate
a numerically larger drain. Independent source review caught and corrected
the missing-HP/accuracy operation order before the final rerun.

Actual AI after repair: injured Swirlix22/42 uses Kiss, survives at20HP with
Sitrus consumed and Unburden active; next turn Skitty uses Helping Hand and
Swirlix uses Kiss, removing Timburr and finishing42/42HP. Skitty remains69HP.

## Completion evidence and limits

Final Tiana run: 11 native groups / 29 parameterized scenarios pass. Repeated
with all prior E0001–4 diagnostics: 44 groups / 135 parameters pass. Key earlier
speed-control, spread, reserve and immunity decisions were inspected, not just
the PASS labels. Sixteen shared paired-AI groups also pass, including Klutz Orb,
weather, Trick Room, redirection, cache/RNG restoration and a warmed full-party
decision at23 GBA frames (previous checkpoint21). This single probe is not a
campaign-wide maximum-latency guarantee. E0005-only canonical materialization
is preserved; no later paused authoring was regenerated from canonical prose.

Why9 rather than10: the lead now has a concrete spread-versus-sustain choice,
with Unburden timing and fast Encore punishment; Buneary brings distinct Klutz
disruption rather than another nearby Guts bird. The reserve Wish/Orb phase
would be stronger with deeper, bounded multi-turn recovery/switch planning.
Wish and future Orb burn retain existing heuristics, not a full delayed-turn
simulation. No two-turn actual reserve sample chose Switcheroo; its forced
three-turn comparison proves a useful tradeoff, not that the AI must choose it
under every pressure line. Sing remains55% and drain into Substitute is
conservatively undercredited because the scorer has no separate Substitute HP
pool. Other existing conditional-survival approximations also remain.

Temporary work/ scenarios are investigations, not a permanent per-trainer suite.
No full playthrough, release ROM, measured win rate or whole-game completion claim.
