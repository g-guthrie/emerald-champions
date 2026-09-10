# E0002 Calvin — reviewed and repaired

Historical flat-stat record. Current completion and evidence:
[final Gen9 practical pass](e0002_gen9_2026_09_09.md).

E0001 and Calvin have completed bounded actual-team scenario reviews, not
exhaustive matchups or uninterrupted player-driven playthroughs. Actual compiled Normal
party at zero badges is level12 throughout, generated natively with IV31 and
flat Stat Points. The previous Taillow/Starly baseline was run before changing
the reserve; all six baseline parameters executed through the native engine.

## Completion report

Final native run: Calvin's seven groups /13 parameters passed after the repairs,
alongside all49 E0001 parameters to check the shared changes. All16 retained
expert groups then passed, including board/cache/RNG restoration. The warmed
full-party decision measurement is21 GBA frames (previously20), not proof of
every possible position's latency or full-game freeze freedom.

New executable repairs account for Belly Drum's native HP cost/failure and
single Sitrus consumption/removal/suppression. The damage forecast now preserves
partial survival and remaining HP when a knockout depends on a damage roll,
instead of treating median damage as a guarantee. Native unrestricted rerun
selects Tailwind/Quick Attack, then Brave Bird/Hyper Voice: Natu faints and both
reserves survive (Hoothoot8HP, Starly15HP). The tested Drum opportunity, danger
protection, denial protection and immediate double-attack removal remain useful.

Intention rating:9/10. Conditional priority setup and a genuinely different
special/Tailwind reserve phase are now stronger than the duplicated physical
bird lineup. Below10: the defensive handoff between Poochyena's conditional
disruption, the Drum user and the reserves could be richer; the two birds still
share exploitable Rock/Electric weaknesses. No measured win-rate claim.

Temporary encounter includes and the specific damage-cache trace were removed
from the normal test build after execution. Scratch work/ probes remain evidence,
not a new permanent roster-lock suite. Generic decision-time tracing is still
temporary for the continuing pass and must be removed at final handoff.

## Observed baseline

- Against native level14 defensive Pachirisu/Lotad, Calvin selected Crunch plus
  Belly Drum. Zigzagoon ended at49/66HP, +6Attack, Sitrus consumed. The next
  decision log showed Helping Hand plus Extreme Speed (selection, not a second
  executed turn in this opening fixture).
- Against Mienfoo Drain Punch and Timburr Mach Punch, Calvin selected Crunch
  plus Protect and preserved Zigzagoon at66HP. Against faster Taunt plus Fake
  Out it also selected Protect rather than attempting Drum.
- A separate forced two-turn comparison executed Drum, immediate Sitrus, then
  Helping Hand plus Extreme Speed. Faster Pachirisu Follow Me redirected the
  attack away from Lotad. Damage was58 after Drum versus16 without Drum; actual
  Zigzagoon HP49/66 and consumed item confirmed the native mechanic.
- With both leads fainted, original Taillow/Starly both selected Brave Bird
  toward Lotad; Starly was knocked out and Taillow remained29HP with its Orb
  poison. This snapshot does not prove a complete reserve policy.

## Authored changes and native verification

Replaced the repeated rival-style Toxic Orb Taillow with native-legal Eviolite
Tinted Lens Modest Hoothoot, Air Slash/Hyper Voice/Tailwind/Protect.
Kept Reckless Starly and U-turn. Hoothoot adds special pressure and a conditional
later-turn speed mode, without claiming slow Hoothoot always enables its partner
on the setup turn. Hoothoot exists in current Route103 grass at8%; no earlier
authored Hoothoot duplicates. Tinted Lens doubles resisted damage, not immunity.

The first32HP/32SpA/2Def version was too slow to benefit from its own Tailwind.
The revised21HP/2Def/26SpA/17Speed investment natively produces61HP,17Def,47SpA,
37Speed at Normal12; Tailwind reaches74, above common fully Speed-invested
early Mienfoo64, Natu66, Taillow71 and Pachirisu73. It costs some bulk and damage.
In the physical-bulk case this let Hoothoot execute Air Slash before Thunderbolt
paralyzed it, helping remove Lotad while both reserves survived. In the genuine
fast-pressure case Hoothoot selected Tailwind and survived the setup turn15HP.

The two-turn continuation exposed a separate forecast defect. Before turn2,
native decision-time cache reports Starly Brave Bird51/55/60 damage against
Natu55HP at100 accuracy. The paired scorer treats the55 median as a certain KO.
Actual damage51 leaves Natu4HP; Hoothoot selects Air Slash at the other foe,
then Natu's Psychic knocks it out. Native forced comparison confirms the better
coverage line: identical Quick Attack16 and Brave Bird51 in both parameters;
Air Slash deals16 to Pachirisu, leaves Natu4HP and loses Hoothoot. Hyper Voice
deals15 to Pachirisu and the remaining4 to Natu, removes Psychic and preserves
Hoothoot8/61HP. The roll-uncertain knockout correction now causes the unrestricted
AI to choose that coverage line itself. Faster Hoothoot acts in time and survives;
no hardcoded Hyper Voice selection was added.

Only E0002 was synchronized into the master using the existing compile_master
output; the existing implementation generator then materialized the master.
E0002's compiled plan is SETUP|TAILWIND. Known unmaterialized later-batch
Vivian/Ben/Wattson authoring remains paused, not silently integrated here. The
whole canonical plan comparison still detects those known later differences;
it must not be reported as a Calvin failure or a whole-catalogue pass.

## Repair evidence and remaining limits

The combined-pressure counterfactual now executed: faster Round26 plus Water
Gun25 consume Sitrus and leave Zigzagoon31/66HP, so forced Drum fails with no
Attack increase. The unrestricted AI already chose Crunch plus Extreme Speed
into Lotad, removing it before Water Gun and retaining Zigzagoon40/66HP. This
is a successful observed decision, not an observed suicidal Drum choice.
The missing local HP/berry forecast has been corrected narrowly;
no additional worst-case targeting or access to pending human commands.

Actual Hoothoot reserve decisions and the paired Drum/berry correction have
now been rerun as recorded above. The baseline's sensible decisions did not
establish that the omitted state was harmless.
Current native Sitrus deliberately uses a ceiling half-HP threshold, so old
parity-only design explanations are stale; no berry mechanic change was made
in this Calvin review.

The scorer still compresses damage outcomes into conditional-mean surviving HP
and an interpolated cached min/max survival chance, not an exact native roll
histogram. Berry/support correlations are unbranched. General berry transfers,
Cheek Pouch, Unnerve-faint activation sweeps and Contrary/White Herb interactions
are not certified by Calvin's Gluttony case. No dynamic spread-damage correction
or general multi-turn search was added. These limits must not be hidden behind
passing diagnostics or generalized into whole-game certification.

Diagnostic corrections: the one-case damage capture required an explicit
PARAMETRIZE declaration; that harness error was fixed before the successful
26/25 native damage run. An initial reserve fixture used Night Shade against
Normal-type birds, which supplies no damage pressure. It was replaced with
native-legal Psychic; no claim of dangerous Night Shade/Tailwind testing remains.
