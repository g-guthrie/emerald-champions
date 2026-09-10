# E0052 Archie at the Museum — Gen 9 native review

Reviewed `TRAINER_ARCHIE_SLATEPORT`, first-access Normal, one badge and native
cap20. Actual levels are19/20/20/21/21/22, retaining the authored boss offsets.
This is the third separate Museum double, after both Grunts without scripted
healing. The script permits one usable player Pokémon and heals after victory.
No encounter/progression/level changes or full-gauntlet balance certification.

## Accepted changes

- Lombre: Life Orb → Sitrus Berry. Preserve Swift Swim, Modest and maximum
  SpA/Speed. Losing the Orb multiplier buys actual extra attacking turns;
  Giga Drain then sustains the established rain attacker instead of recoil
  consuming its narrow survival margin.
- Tentacool: Knock Off → Rain Dance. Preserve Black Sludge, Clear Body,
  Timid and maximum SpA/Speed. The team gains a fast backup weather setter
  that immediately supports Water attacks and can restore Lombre's Speed.
  Corphish retains Knock Off. This is a real moveslot trade, not free coverage.
- All six species, other moves/items, abilities, EVs, offsets, shared stat and
  berry rules remain unchanged. Existing bounded RAIN/SETUP policy is retained;
  no shared-AI changes, new branches, search or allocation. Updated prose
  describes conditional options and no longer promises an unobserved Dance.

| Pokémon | EVs HP/Atk/Def/SpA/SpD/Spe | Native HP/Atk/Def/SpA/SpD/Spe |
|---|---|---|
| Wingull,19 |252/0/4/0/252/0|62/19/24/31/34/43|
| Lombre,20 |4/0/0/252/0/252|60/27/31/51/39/43|
| Corphish,20 |4/252/0/0/0/252|53/60/37/27/25/37|
| Tentacool,21 |4/0/0/252/0/252|54/25/26/45/53/59|
| Golbat,21 |4/252/0/0/0/252|69/58/40/34/43/68|
| Carvanha,22 |4/252/0/0/0/252|58/71/20/36/20/54|

All exact508-EV spreads and IV31 remain. Lombre43→86 in rain outruns Natu56,
Pachirisu49 and Mienfoo53, but not opposing Tailwind112/98. Tentacool59 can
set rain ahead of ordinary foes and Lombre; Wingull43 needs support. Corphish37
would reach55 after one Dragon Dance, still below Natu56; no claim that the
optional boost solves all Speed matchups. Carvanha54→81 after Speed Boost
supplies a separate speed engine. Golbat68 already outruns the ordinary
references. Lombre60HP has a31HP Sitrus threshold and heals15HP; no EV-parity
adjustment or berry-rule change.

Distribution: preserve the sole Wingull, first of three Lombre, third Tentacool,
first of two Golbat, final early Carvanha and fourth Corphish. The other two
Lombre use Own Tempo/Eviolite; this one is the rain attacker. Other Tentacool
use defensive Liquid Ooze/Acid Spray, while this fast Clear Body setter is
distinct. Corphish remains Sash with optional setup rather than the earlier
Choice Band/Eviolite variants. No wild/tutor or habitat changes.

Coaching card: Fake Out buys Wingull's long rain. Spend the resulting Speed
advantage on Hydro Pump, using Giga Drain to sustain Lombre. Tentacool can
replace lost weather, then attack; use Icy Wind only for useful position or
damage. Corphish can attack immediately, use priority or seek an optional
setup window. Golbat/Carvanha provide weatherless pressure when weather is
denied. Fast Electric attacks, opposing Tailwind, redirection, priority and
setter disruption remain meaningful counters.

## Native evidence

`work/archie-museum-gen9-final-motifs.log`:7 groups/40 parameter cases PASS.
Opponents are the current compiled, natively generated trainer party. Player
references use native level20, IV31, exact EVs and preparation-legal moves.
All NPC decisions and normal replacements are unforced. Promoted middle/end
reserves and explicitly selected mixed survivors are identified below; these
are not proof that a natural battle always produces those exact pairings.
Observed secondary-effect rolls and outcomes are not guarantees or win rates.

- Physical leads: Fake Out/Rain Dance establishes rain turn1. Sitrus keeps
  Lombre42HP rather than the original21HP; it then Giga Drains and remains
  active instead of falling on turn2. Wingull eventually falls and Corphish
  enters naturally. Turn4 finishes Eevee/Timburr with Corphish53HP/Lombre56HP.
  Wingull's survival is not solved: it drops to7HP turn2 and falls turn3.
- Psychic/Electric leads: rain/Fake Out executes, Sitrus leaves Lombre44HP
  rather than23HP. Hydro Pump removes Natu turn2; Wingull falls and Carvanha
  enters, then falls to Electric pressure and Tentacool enters. Giga Drain
  finishes Pachirisu turn4, Tentacool54HP/Lombre60HP. This trades away other
  members; it is not a no-loss victory. Initial rain, berry consumption and
  Lombre's86 effective Speed are asserted in both lead references.
- Opposing Tailwind: rain still establishes, but Lombre86 remains slower
  than112/98 foes. Wingull falls; Sash Corphish joins, attacks and survives
  at1HP. Turn3 Crabhammer/Giga Drain finishes both foes, Lombre55HP versus
  original Life Orb11HP. No redundant Tailwind move was added.
- Fake Out/Taunt denial: Wingull is flinched turn1 and switches to Golbat
  turn2. The team attacks weatherlessly instead of repeatedly failing Rain
  Dance. At turn3 Mienfoo is down, Pachirisu55HP remains, Golbat12HP/Lombre60HP.
  This is a bounded fallback, not a finished win or proof that rain always
  survives disruption.
- Promoted Corphish/Tentacool versus physical foes: Tentacool sets rain
  before Corphish's immediate Crabhammer knockout on Eevee, asserted.
  Crabhammer/Hydro Pump finish Timburr turn2, Corphish53HP/Tentacool38HP.
  The original non-rain pair had Corphish15HP and Timburr77HP at turn2.
- Same middle pair versus Psychic/Electric: Tentacool's rain supports a
  turn1 Natu knockout, Corphish consumes Sash and Tentacool survives at9HP.
  Corphish falls turn2, Carvanha enters, and Icy Wind lowers Pachirisu49→32.
  Hydro Pump finishes turn3, Carvanha58HP/Tentacool12HP. Only one Icy Wind
  is used; later offense resumes.
- Promoted Golbat/Carvanha, physical: Brave Bird with a guard, then Crunch
  attacks finish both foes by turn3. Golbat falls; Carvanha remains full58HP.
  Special reference: Poison Fang/Crunch, Brave Bird/Protect, then Brave
  Bird/Liquidation finish turn3 with Golbat11HP/Carvanha58HP. The guard is
  conservative but does not repeat consecutively; this is weatherless payoff.
- Actual opposing Prankster Sunny Day on turn2: Wingull restores rain that
  turn, Rain Dance PP4→3 asserted, and switches to Tentacool on turn3 while
  Lombre continues Hydro Pump. No same-weather repetition is claimed as useful.
- Explicit Tentacool/Lombre mixed pair: Tentacool establishes rain turn1;
  Sun interrupts it turn2, and it restores rain turn3, Rain Dance PP3 and
  Lombre86 Speed asserted. Hydro Pump then removes the redirecting Pachirisu;
  Tentacool13HP/Lombre60HP remain against Sableye76HP. This proves backup
  restoration and payoff, not a completed win or an automatic same-turn reset.
- Explicit Lombre/Corphish pairing: Fake Out is followed by direct Crabhammer,
  not Dragon Dance. Corphish falls turn2; Lombre survives to attack turn3.
  A separate fast-foe/redirection reference with Corphish/Tentacool also attacks:
  Crabhammer while Tentacool is flinched, Aqua Jet/Rain Dance turn2, then
  Crabhammer/Hydro Pump finishes turn3 at53HP/16HP. Priority, weather support
  and attacking payoff execute; optional Dragon Dance does not.
- One injured survivor: Eevee or Timburr starts at75%HP with a fainted second
  slot. Native absence is asserted. Rain Dance/Fake Out establishes the plan
  turn1; Hurricane/Hydro Pump finish turn2, both leads full. No dead-slot
  command loop or requirement for a second player target. These constructed
  adverse entries do not certify the actual three-fight attrition playthrough.

Dragon Dance, Roost, Golbat Taunt/U-turn and a long recovery loop are not
demonstrated. The main rain engine, backup restoration, sustain, priority and
weatherless closing plan are demonstrated. No broad AI exception was added
to force every optional move into these bounded references.

Evidence trail: `work/archie-museum-gen9-baseline.log`,
`work/archie-museum-gen9-rain-support.log`,
`work/archie-museum-gen9-followthrough.log`, and final motifs log. Scratch
`work/archie_museum_gen9_probe.inc` reuses prior Museum native player helpers;
only its own prefix is executed. Its include is removed from the retained
build, with files preserved for reproducibility, not permanent trainer tests.

Sampled complete cold decision:50 GBA frames (~0.83s),7 setup+43 both-actor
frames, six-member player and opponent parties. Original55; warm43 excludes
setup. Under the1.2s ceiling for this sample, not campaign-wide worst-case
proof. Canonical checks match2339 slots/516 branches and6923 shared sets.
Clean shared regression outcome is recorded in the ledger. No release build
or replacement of the frozen Downloads ROM.

## Rating

8.5/10, intention-based. More reliable Dragon Dance opportunities and safer
weather-setter preservation would improve Archie. The core is now a supported,
sustained rain offense with a second setter and a credible weatherless fallback;
optional setup and the full Museum attrition balance remain evidence boundaries.
