# Verdant early evolution-stage policy

Verdant's opening should use the short part of the campaign in which young
Pokémon are still credible opponents. Difficulty comes from bespoke moves,
items, abilities, synergy, AI, and small cap-relative level advantages—not from
placing ordinary final evolutions below the levels at which their families can
actually reach them.

`scripts/verdant_evolution_stage_audit.py` reads the checked-in evolution table,
trainer parties, strict cap table, map battle scripts, and Mega Bracelet grant
scripts. It enforces these rules for every closed bespoke encounter before the
Stone Badge and before the player can obtain the Mega Bracelet:

1. At least 75% of each party must be either a first-stage Pokémon or a truly
   single-stage Pokémon. For a four-Pokémon party, that means at least three.
2. Any species with an incoming non-battle evolution edge is evolved. Item,
   trade, friendship, move, location, beauty, and other non-level methods all
   count; they are not mislabeled as single-stage.
3. An evolved Pokémon cannot appear below the earliest real level required by
   its complete family path. Non-level edges add no invented threshold, but do
   not erase a level threshold earlier in the same path.
4. No Mega Stone or Mega form may appear before the player's first reachable
   Mega Bracelet grant.
5. Rare single-stage and legendary/mythical showcases are allowed, but they
   must be named below. A showcase note records intent; it never waives the
   75% rule or legal-level rule.

## Registered opening showcases

| Encounter | Species | Kind | Design rationale |
|---|---|---|---|
| `BATTLE_002_ROUTE_102_CALVIN` | `SPECIES_MEW` | `legendary-single-stage` | Calvin's single mythical reveal is the deliberate early signal that Verdant's roster will be surprising. |
| `BATTLE_005_ROUTE_102_TIANA` | `SPECIES_ARCTOZOLT` | `rare-single-stage` | The fossil is the naturally single-stage centerpiece of Tiana's otherwise first-stage snow team. |
| `BATTLE_006_ROUTE_104_BILLY` | `SPECIES_DITTO` | `rare-single-stage` | Imposter Ditto is the deliberate strange-footprint reveal in Billy's young shoreline singles puzzle. |
| `BATTLE_011_PETALBURG_WOODS_JAMES` | `SPECIES_CELEBI` | `legendary-single-stage` | Celebi is James's deliberate forest-spirit reveal and the mythical center of Verdant's first Perish-trap lesson. |
| `BATTLE_012_ROUTE_104_WINSTON` | `SPECIES_STONJOURNER` | `rare-single-stage` | Stonjourner is Winston's expensive display piece and introduces the doubles-only Power Spot ability. |
| `BATTLE_014_ROUTE_104_GINA_MIA` | `SPECIES_ORICORIO` | `rare-single-stage` | Oricorio is the twins' recital centerpiece; three young partners reveal Dancer through distinct legal dance moves. |
| `BATTLE_015_ROUTE_104_IVAN` | `SPECIES_WISHIWASHI` | `rare-single-stage` | Ivan's level-20 ace is the first legal Schooling reveal; its extraordinary form has the visible counterplay of collapsing at one-quarter HP. |
| `BATTLE_018_RUSTBORO_GYM_MARC` | `SPECIES_AERODACTYL` | `rare-single-stage` | Aerodactyl is Marc's fast fossil closer after Shieldon, Woobat, and Corsola turn switching itself into the puzzle. |
| `BATTLE_019_RUSTBORO_GYM_ROXANNE` | `SPECIES_REGIROCK` | `legendary-single-stage` | Regirock is the first Gym Leader's lone legendary mineral ace; five common weaknesses and no Protect keep the level-19 finish broadly answerable. |

The rebuilt opening encounters now pass this gate. Any future failure is a
real progression regression: either the party must be rewritten or the mature
concept must be tabled for a later encounter.

## Campaign stage targets

The legal-level rule applies everywhere. The roster-composition targets become
less restrictive as the caps make mature teams natural:

| Chapter start | Strict cap | Default opponent presentation |
|---|---:|---|
| Before Stone Badge | 14 | At least 75% first-stage or genuinely single-stage; no Megas. |
| After Stone Badge | 20 | Young and middle stages still lead; only final forms actually attainable by level 20; first player Mega access. |
| After Knuckle Badge | 30 | Middle stages and early final evolutions mix; important trainers may use one Mega. |
| After Dynamo Badge | 40 | Fully evolved teams become normal; younger forms remain only for Eviolite, Little Cup, or a deliberate theme. |
| After Heat Badge | 45 | Competitive final-form teams are the default and rare species become more frequent. |
| Balance Badge onward | 55+ | Full competitive rosters, legendaries, unusual forms, and Megas can be used freely when their individual encounter remains unique. |

An offset never excuses an impossible evolution. A level-16 opponent cannot
use a species whose checked-in family path first reaches that form at level 24.
Stone, trade, friendship, beauty, move, and location evolutions have no invented
minimum level, but they still count as mature forms for the chapter ratio.

## Mega progression

- The first player Mega Bracelet remains Steven's Granite Cave reward after the
  Stone Badge, at strict cap 20. Norman is only a fallback if Steven was missed.
- No opponent may Mega Evolve before that first reachable player grant. Roxanne
  and every cap-14 encounter therefore use no Mega.
- Mega access must be useful immediately. After the Stone Badge, the Rustboro
  Pokémon Center shop unlocks Honey; Petalburg Woods 2 Honey encounters now supply Weedle and
  Caterpie rather than impossible level-2 final forms, and the connected
  Cut-accessible Petalburg Woods 3 holds Beedrillite. A caught Weedle can use
  frictionless cap leveling, evolve legally at levels 7 and 10, and become the
  player's first real cap-20 Mega instead of leaving only future starter stones.
- Brawly is the first boss eligible to showcase an opposing Mega. Its base
  species must itself fit the cap-20 stage, such as a single-stage Heracross;
  an impossible level-20 Machamp is not acceptable.
- From Wattson onward, major bosses normally receive one Mega. Ordinary
  trainers use them selectively as memorable reveals, never as filler.
- A reachable trainer carries at most one Mega Stone. A Mega-capable family is
  made obtainable before or near its stone, and stones remain exploration
  rewards rather than generic shop stock.

## Wild encounter progression

- Early routes favor base forms of high-ceiling families so evolution is a
  payoff rather than a grind: examples already include Dreepy, Larvesta, Axew,
  Beldum, Aron, and Timburr.
- Genuinely single-stage species such as Rotom, Mawile, and Sableye provide
  immediate tactical flexibility without pretending to be premature final
  evolutions.
- Ordinary wild evolved forms do not precede their real family threshold unless
  a specific location and rarity make the exception meaningful.
- Each chapter exposes several different answers to its next major battle—type
  pressure, immunity, speed control, weather interaction, disruption, or
  redirection—rather than one mandatory catch.
- Useful or exciting species cannot require low-percentage grinding. A 1–4%
  encounter may be a bonus discovery, never the only practical source of an
  important counter or Mega family.

Mature designs removed by this policy are retained in
`docs/verdant_tabled_mature_battle_concepts.json`. They must be reassigned by
hand at a fitting later trainer; they are not automatically allocated.
