# E0018 — Hector: bounded executable-AI repair complete

Intention rating: **8.5/10**, not a measured win rate. Route115's native script
invokes `trainerbattle_double TRAINER_HECTOR`; this is one current branch and
one encounter. No progression, party size, cap, species or investment changed.
The compiled party and native battle engine are the authority.

## Accepted team and AI

Normal zero-badge native generation gives all four level12:

| Pokémon | HP/Atk/Def/SpA/SpD/Spe | Item and ability | Moves |
| --- | --- | --- | --- |
| Spinda | 72/60/25/20/23/23 | Sitrus, Own Tempo | Fake Out, Helping Hand, Sucker Punch, Rock Slide |
| Zangoose | 45/68/23/20/23/68 | Toxic Orb, Toxic Boost | Facade, Close Combat, Knock Off, Protect |
| Seviper | 45/28/23/70/23/56 | Life Orb, Infiltrator | Sludge Bomb, Flamethrower, Giga Drain, Glare |
| Dunsparce | 63/62/27/21/24/37 | Leftovers, Serene Grace | Body Slam, Rock Slide, Roost, Protect |

All sixteen moves are natively preparation-legal; abilities and all66-point
allocations were checked. Raw displayed stats exclude item/ability modifiers.
Spinda's low Speed does not prevent priority Fake Out or Helping Hand from
supporting fast Zangoose. Seviper can slow a target before Dunsparce acts;
native45-Speed Timburr becomes22 after Glare, below Dunsparce's37.

- **Encore → Helping Hand on Spinda.** The same native Pachirisu/Timburr board
  previously used Rock Slide/Facade on turn2 and left Timburr15HP. Helping
  Hand/Facade instead removes it on turn2, leaving Hector's same46/21HP.
  Pachirisu keeps5 more HP because spread chip was traded for the knockout.
  The Grass board still uses Rock Slide/Knock Off when support is unnecessary.
  This sacrifices Encore's future denial; Rock Slide remains repeatable damage
  so a surviving Spinda is not left with only conditional Fake Out/Sucker Punch.
- **Glare in the paired forecast.** Previously only Thunder Wave/Nuzzle
  continued into local paralysis and changed partner action order. Glare now
  receives native status, Substitute, reflection and priority checks without
  incorrectly inheriting Electric absorber/Lightning Rod rules. Immediate
  cures and unmodeled beneficial status interactions earn no false slowdown.
- **Body Slam in the paired forecast.** Uses native secondary-effect chance:
  ordinary30%, Serene Grace60%, including the native rainbow calculation.
  Covert Cloak, Shield Dust, Sheer Force, damage immunity, status immunity and
  a blocking Substitute prevent unsupported secondary credit. The existing
  two-pass forecast weights the change; it does not assume every hit paralyzes.
- The extension adds16 cached bytes, no extra evaluation pass, recursive search
  or damage calculation. The NONE plan and Conservative flag remain: no
  hardcoded opening or pending human command is used. Already-poisoned native
  Toxic Boost/Facade damage, Helping Hand and non-Flying Roost needed no rewrite.

## Native observations

Final accepted actual-party run: **31 parameters across11 groups, all executed**.
These comprise6 lead,4 reserve,4 timing/calibration,4 Orb,3 transition,
4 paralysis-control and6 flinch/follow-through parameters. Mechanical controls
explicitly force moves/RNG or remove Orb; other opponent decisions use the full
authored menus. Promoted-reserve boards keep the original leads fainted, not
replaced by stronger inventions. A PASS alone is not a tactical judgment.

- Fake Out/Protect gives Zangoose an activation window against bulky pressure.
  Accepted turn2 Helping Hand/Facade removes the prepared81HP Timburr and also
  removes72HP Mienfoo in the denial board. Against Pachirisu/Munchlax, Hector
  attacks rather than always protecting; turn2 Helping Hand/Facade removes
  Munchlax. This does not claim the AI predicts the player's chosen Protect.
- Native Orb-vs-no-item mechanical control measures Facade33 versus12 damage
  into the same Pachirisu; no Helping Hand is supplied in that control.
- Under Fighting/Psychic pressure, Spinda uses Rock Slide, then voluntarily
  switches to Dunsparce while Zangoose removes Natu with Knock Off. Zangoose
  survives at1HP after turn2, contributes Facade, and faints by turn3 while
  Dunsparce finishes Mienfoo. This is an observed native transition, not merely
  starting a test with reserves on the field.
- Actual Seviper/Dunsparce remove Shellos with Giga Drain by turn2. On turn3
  they choose Glare/Roost; Dunsparce reaches53HP. Turn4 Sludge Bomb/Body Slam
  leaves Timburr16HP, own37/43HP. This diagnostic suppresses player full
  paralysis to observe continued pressure, not to manufacture trainer choices.
- Separate forced Glare/Rock Slide control demonstrates native order reversal
  and flinch denial; successful secondary RNG is explicit. The alternative
  Sludge Bomb control lets Timburr act first. Native metadata separately
  establishes60% Serene Grace flinch, not a measured frequency. An attempted
  forced miss on100%-accuracy Glare was rejected as an invalid diagnostic.
- Actual Glare paralyzes Ground Wooper but not Electric Pachirisu. Actual
  Serene Grace Body Slam paralyzes the uncured control, not Covert Cloak.
  Ghost Glare eligibility is source-reviewed, not separately played here.
- Counterplay remains real: prepared Natu's Psychic removes exposed Seviper
  immediately on the reserve board. Three Normal members remain Fighting-weak.

Two distinct small generic regressions were retained, not a permanent Hector
suite. Disabling Glare's cache changes Glare/Body Slam to an attack into Protect
and loses Seviper; the chosen-move assertion fails. A Cheri control rejects the
fictitious slowdown. Separately disabling Body Slam's cache changes its chosen
move to Rock Slide and fails its decision assertion;30/60/Cloak parameters do
not require a random proc. Both mutations were restored. These synthetic boards
prove evaluator behavior, not Hector's difficulty or native Stat Point balance.

Clean shared run after removing every Hector scratch include: **31 groups PASS**,
including transaction/RNG restoration, weather, Room, partner effects and both
new regressions. Master projection verifies516 branches; diff check passes.
That projection does not count later encounters as completed.

Final four/six-player Hector decisions measure40 warm frames and46 including
cache rebuild, versus48/53 with the original Encore menu. These are bounded
native samples, not a general performance benchmark or a claim of instant AI.
The existing full-party shared timing sample remains30 frames. Easy/Hard use
the production fallback accessor and produce all10/all14 respectively; these
are calibration observations, not full difficulty playthroughs.

## Variety, limits and path upward

All four species debut here in the current campaign order, without repeating
Cyndy's team. Branch-weighted counts are Spinda2, Zangoose4, Seviper4 and
Dunsparce3. The collector/rival quartet is retained: later Spinda uses different
Room/Instruct support, and later Dunsparce roles are not identical.
Early Mienfoo/Timburr, Intimidate Growlithe, Reflect Natu and Electric Pachirisu
offer answers. Burning Zangoose is not simple shutdown: Facade benefits from
burn too. Removing Orb before activation differs from removing it after poison.
Infiltrator Seviper can bypass screens and changes the answer to physical bulk.

To improve beyond8.5: better preservation of exposed Seviper, broader matchup
and endgame coverage, and lower worst-case decision latency. Future Orb payoff
is not explicitly forecast before activation. Rock Slide has native flinch
heuristics but no newly modeled probabilistic paired stop event; the forced
combo proves its mechanics, not that the AI always chooses it. The existing
two-pass minimum-chance approximation is not a full joint probability tree.
No fresh release ROM, full campaign integration or exhaustive playthrough is
claimed. Restart progress is **22/516 branches,17/468 encounters**; E0019 next.
