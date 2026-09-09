# E0004 Allen — actual-team review and repair

One completed trainer branch, TRAINER_ALLEN, under bounded native scenarios.
Current source and compiled party, not the historical batch document, are authoritative. All four
actual Normal/zero-badge opponents natively become level12 with IV31 and flat
Stat Points. Species, items, abilities, points, offsets and progression retained.

## Current authored changes

Paras trades Reflect for native-legal Wide Guard. It can protect both partners
from spread attacks while Pikachu attacks, rather than protecting only itself.
Spore, Leech Life and Protect remain. Single-target attacks, Taunt and Volt
Tackle recoil still bypass that team-wide protection. This trades persistent
physical mitigation for immediate, priority spread defense; it is not a universal
upgrade against every opponent.

Bunnelby trades Strength for legal Frustration. Native trainer generation copies
the compiled friendship field, currently0; the materializer emits no Happiness
override. Native Frustration is therefore102BP versus Strength80, whereas Return
would be1BP after its minimum clamp, not the usual assumed255-friendship move.
The actual generated Bunnelby's native comparison deals43 versus34 damage into
the same prepared Timburr. No friendship, stat, level or player rule changed.

Retain physical Light Ball Pikachu (different from E0001's special Pikachu),
attacking Eviolite/Dry Skin Paras, bulky Choice Band/Truant Slakoth, and faster
Huge Power Bunnelby. These are not automatically interchangeable physical
reserves: Slakoth has72HP/15Speed, Bunnelby36HP/54Speed at Normal. No species
quota or compulsory special attacker was imposed.

## Native baseline and comparisons

- Opening opportunity: old Fake Out into Eevee plus Reflect preserved both
  allies. After the Wide Guard change, the current opening selected Fake Out/
  Protect, then Protect/Leech Life; after two actual turns Eevee24HP, Paras23HP
  and Pikachu36HP. That is progress with damage taken, not an effortless win.
- Grass Lotad plus Safety Goggles Mienfoo correctly dissuades Spore. Current
  Volt Tackle removes Mienfoo and Leech Life hits Lotad, ending Pika13/Paras66.
- Scarf Mienfoo is natively faster at its matching Fake Out priority and
  flinches Pikachu. Scarf speed is present in the cache; the opposing-action
  forecast may choose a damaging action instead of Fake Out. The AI must not
  read the actual human command to avoid every prediction error.
- Two native Brave Timburr17Speed become slower than Paras14 after Web.
  Forced Web/Spore denies one Ice Punch and leaves Paras22HP; forced Web/Reflect
  takes29+29 instead of the44 unreflected hit and leaves Paras8HP. First-turn
  Fake Out/Protect preserves full HP, so the control is not automatically better.
- Forced two-turn Web/Spore then Volt Tackle/Leech Life, with Ice Punch freeze
  and Volt Tackle paralysis explicitly suppressed, still loses Paras after its
  Leech Life. It ends Pika20HP, one foe13HP and the other74HP/asleep. This proves
  the selected line's consequences, not a guaranteed superior strategy.
- Spread comparison with the same native Natu/Pachirisu: Volt Tackle/Protect
  leaves Pikachu knocked out by Heat Wave; Volt Tackle/Wide Guard keeps Pika11HP
  and Paras66HP. Single-target Psychic through Wide Guard leaves Paras50HP.
  Volt Tackle paralysis is disabled only in this forced comparison, ensuring
  the opposing attack actually executes. The original uncontrolled sample
  paralyzed Natu and therefore did not demonstrate Guard's protection.
- Reserve snapshot: exact generated Pokemon are permuted to start Slakoth/
  Bunnelby with or without the original leads available. Protect wastes the
  first Slakoth attack; native loafing occurs on the second turn. Current
  Bunnelby chooses Frustration and both stay in, ending Slakoth36/Bunnelby20.
  Prior Strength version used U-turn to Paras when that reserve was available.
  These are later-board snapshots, not proof the AI chose the previous switch
  sequence independently.

## Executable repairs and observed decisions

Spore's newly inflicted sleep was absent from the paired action forecast. The
new bounded cache uses native sleep/powder/Substitute/status eligibility and
preserves native scratch state. Its existing effect/no-effect pass accounts for
same-turn action denial, Early Bird's reduced denial chance, Sleep Talk/Snore,
active cure berries and local duplicate/Sleep Clause claims. It retains native
future-sleep opinion, not a full sleep-turn simulation. New Marvel Scale and
temporarily blocked/removed cure berries are conservative exclusions from added
denial credit. No forced Spore preference was introduced.

The actual two-Timburr continuation exposed a separate defect: after first-turn
Protect, Paras selected Protect again and it failed, losing Paras. The paired
model had treated repeated protection as certain. A shared pure denominator
helper now supplies the existing native probability table to the AI without
sampling RNG; native execution retains its exact original table and RNG call.
The cached forecast mirrors native counter resets, modern Wide/Quick Guard
exemptions, and last-living-move failure. Repeated protection uses the existing
effect/no-effect blend, not another search tree. Independent review found no
blocking mismatch with native execution.

The repaired AI independently chooses Fake Out/Protect on turn1, then
Electroweb/Spore on turn2 against the same two Timburr. Pikachu remains36HP and
Paras22HP rather than fainting; foes74/62HP, one asleep. The sampled Ice Punch
also freezes Paras: survival here is not a guaranteed winning continuation or
immunity to secondary effects. Separate native controls confirm Lum cures sleep
before the selected Drain Punch, Early Bird can wake immediately or lose the
action according to its native sleep counter, and Sleep Talk executes asleep.

The unrestricted Natu/Pachirisu board chooses Protect rather than Wide Guard.
Temporary forecast tracing explains why: it predicts Super Fang into Paras plus
Heat Wave (33+48 damage into Paras,14 into Pika), whereas the supplied player
command targets Pika with Fang. Protect has meaningful value against that
prediction; this is not evidence of a missing Wide Guard implementation.
Against two actual prepared Heat Wave Natu, the AI first protects both leads,
then independently chooses Volt Tackle/Wide Guard on turn2. Native execution
ends Pika21/Paras66HP, foes23/71HP, with both Heat Waves blocked. Modern Wide
Guard remains reliable after the previous Protect. Single-target Psychic is
still counterplay; no command-reading or compulsory Guard preference was added.
The temporary AI-side forecast logger was removed after diagnosis.

## Diagnostic integrity and limits

The first with-reserves fixture changed only DSL currentMonIndexes and actually
started the living original leads. It was corrected by permuting exact generated
Pokemon, with native moves/speeds and legal abilities intact, before any reserve
completion claim. Fainted-lead snapshots do naturally start later living slots.
Tied Timburr animation order was a fixture-only assumption; damage captures now
use chronological hits. Forced native KOs require explicit replacement choices;
missing those produced TURN-incomplete diagnostics, not demonstrated game stalls.
An exploratory wording assertion confirmed the native loafing message but was
removed rather than retaining a brittle text lock.

The final current-source Allen run passes nine groups/24 parameters after the
AI-side logger is removed. Earlier E0001–3 scenarios also reran successfully
(82 parameters), retaining Sash Seed Bomb/Web, environment-dependent Nature
Power, Calvin's Tailwind/Hyper Voice follow-through and Rick's injured-target
Thread/Knock Off timing. All16 retained shared groups pass, including board/RNG
restoration and shared tactics; the warmed full-party decision remains21GBA
frames. That timing probe is not a worst-case campaign latency guarantee.
The materialized master-prefix comparison passes for516branches; known paused
later canonical changes were not swept into this encounter's materialization.

Temporary encounter includes are removed from the normal build after review.
The work/ files remain investigations, not a permanent trainer-lock suite.
General decision tracing remains temporary for the continuing pass. No full
playthrough, win rate, release ROM or blanket AI optimality is claimed.

The bounded forecast still compresses independent immediate effects into one
probability blend. It does not model every later sleep turn, healing exchange,
item-removal consequence or opponent prediction. Guard selection must remain
conditional, not privileged knowledge of the player's next command.

Intention rating:8.5/10. Fake Out, Web-to-Spore, spread protection and distinct
physical reserves give this early encounter several useful decisions. Below10:
the reserves are more conventional attackers than a new ability-combination
payoff. A future reserve redesign could add a distinct support interaction, but
must retain the meaningful Truant/Choice-lock counterplay rather than simply
erase it or add another repeated fast sweeper.
