# Trainer strategy audit — September 8, 2026

The authored campaign has 468 encounter records, 516 trainer branches and 2,339
Pokémon. The 502 doubles branches and 14 multi branches include alternative
rival teams and independent trainers sharing an encounter record; these are
not 516 battles in a single playthrough. Party sizes are 362 four-member,
137 six-member, nine five-member and eight three-member multi parties.

This pass read the teams source, its materialized master and `trainers.party`,
the configured species/ability tables, and the actual AI consumers. It did not
play 468 encounters or measure campaign win rates. Legality and diverse species
are necessary, but do not establish a strong battle.

## Findings and changes

The central failure was that `plan:` and `crack:` were prose. They did not tell
the AI to execute the battle's strategy. All trainers also received generic
Prediction, which could recursively evaluate hypothetical switch positions
during AI initialization. Meanwhile 128 branches had Risky, 96 had Prefer
Highest Damage Move and 37 had Force Setup First Turn. Those blanket biases
could oppose support, accuracy or a sensible decision to attack instead of
setting up. They have been removed; knowledge, ordinary move evaluation and
switching remain. Ace/Double Ace and Conservative traits remain authored.

Every branch now has an explicit `strategy:` directive. The existing teams
compiler emits a trainer-indexed `u16` table (1,710 bytes for 855 trainer IDs)
and `EmeraldChampions_GetBattlePlan` resolves the actual trainer owner. It does
not read prose or classify teams during a turn. Unlisted trainers and unrelated
wild, link (including recorded link) and facility namespaces receive no campaign
plan. Single-player recordings preserve their original trainer IDs and plans. The
compiler's existing `--check` now checks the generated directives too.

Current branch counts are 31 Trick Room, 41 rain, 24 sun, 20 sand, 16 snow,
38 redirection, 87 Tailwind, 212 setup, five deliberate ally combinations and
one Perish Trap.
Categories overlap; 163 branches use the ordinary evaluator without an extra
plan. These are inventories, not quality targets. Initial directives were
derived from actual loadouts and reviewed against the intended field/partner
roles. Susie's Mega Froslass and Drake's Koraidon need snow and sun directives
despite not having the corresponding ordinary weather ability in their base
loadout. Mega Meganium's Mega Sol is personal effective weather, not team sun.

The static loadout audit found no invalid configured ability, unknown or
duplicate move, Assault Vest/status contradiction, or incoherent Choice setup
remaining in the campaign source. The configured ability check covered all
2,339 Pokémon. No Eviolite holder was a fully evolved species, and every
authored Swift Swim/Chlorophyll/Sand Rush/Slush Rush user had a compatible
team weather source. This did not justify replacing rosters at random.

The old audit itself rejected six valid four-member doubles opening rival
branches because it still required singles with one Pokémon. Those obsolete
exceptions were removed. The full static master audit now has zero errors;
that result is not combat evidence.

## What the book changed, and what it did not prove

Before the subsequently approved creativity upgrades, an exact comparison
against `Cohesion Book/review/all-trainers.json` found all 516 Pokémon loadouts
matched the book's final arrays. The book preserved
501 baseline loadouts and revised 15, including six opening rival aliases.
Preserving those teams was not itself the cause of freezing. Initial repair
changed how plans reach the AI without changing loadouts. The user then approved
the five-encounter portfolio below; Parker and Maura now intentionally differ
from the book. The other three improvements concern execution and phase guidance.

Some book changes are concretely useful: Cristian's faster Jolly Falinks can
activate Justified before Gallade attacks; Wallace's physical Starmie exploits
its custom Huge Power Mega; Colton's Eternal Floette and Quincy's Power
Construct Zygarde use the forms required by their Mega paths. Phoebe's Dusclops
and Glacia's Walrein are deliberate roster choices, not proven improvements
in difficulty. Keeping or revisiting them should depend on actual battles.

The problematic architecture prescription was to enumerate the complete legal
one-turn action-pair set across form/switch scenarios while also enabling
generic Prediction everywhere. The final SAI-07 bound of 32 choices per actor
and 1,024 pairs per form describes finite work, not acceptable GBA latency.
It did not supply a measured total turn budget for the combined loops.
The preference against trainer-specific handlers also removed an easy place
to express authored intent; compact per-battle directives now restore that
intent while mechanics and legality remain shared.

However, the book did **not** claim the game was playtested. Its README explicitly
disclaimed builds and native play. Chapter 06 said to preserve successful
behavior and not introduce a monolithic replacement AI; SAI-09 said to migrate
useful handler behavior before deleting it. SAI-12 called for actual authored
battles and cycle/frame measurements. Shipping the broad replacement without
those acceptance results was an implementation/verification failure as well
as an overambitious design. Inventory completeness and pages of battle prose
did not certify execution quality.

## Battles where execution matters

| Battle | Actual dependency and review result |
| --- | --- |
| Roxanne; Tate & Liza | Slow attackers need Room; Tate & Liza also have two setters. Coordinated selection must choose at most one setter, preserve useful active Room, and re-establish it after expiry. |
| Chip; Samantha; Nate; Virgil | Mixed fast/slow phases mean a Room directive cannot blindly forbid clearing unfavorable Room. The current active matchup matters more than the roster label. |
| Cristian, E0038 | Jolly Falinks leads beside Justified Gallade; Lucario is an alternate recipient. Beat Up must target a surviving physical attacker before it acts, and must not spend both actions activating one another. |
| Nolan, E0117 | Dondozo/Tatsugiri lead together. Native Commander activation exists. A forecast must neither give hidden Tatsugiri an attack nor treat it as a legal target. |
| Tabitha, Magma Hideout, E0337 | Low-investment Dragapult Surf activates Coalossal's Steam Engine and Weakness Policy. Charging only friendly-fire damage undervalues the entire authored opening. Volcanion is a compatible Water Absorb reserve. |
| Sylvia, E0394 | Commander is a reserve phase in slots five/six. The native switch-candidate code can activate it when the pair is assembled; it must not invent actions for the hidden fish. There is no guarantee both reserves will survive or enter together. |
| Parker; Cedric | Oranguru's Instruct needs an actual repeatable partner attack. A scorer that skips all status actions after assigning no independent support score loses this strategy. |
| Miu & Yuki; Gabrielle | These have deliberately separate weather phases. Their weather bits are permissions, not an instruction to overwrite a currently useful partner's weather. |

Independent review of the replacement scorer flagged missing Wide/Quick Guard
distinction, Instruct/After You value, Coalossal activation, hidden Commander
participants and overly absolute Room preservation. These were passed to the
AI implementation owner for correction. The native game effects and the
lightweight forecast are separate: an implemented mechanic can still be chosen
badly when the forecast omits its payoff.

## Evidence and remaining boundary

Executed: teams compiler/materialization, exact master-to-party comparison,
configured ability legality and full static master audit. A focused native
lookup regression in `test/battle/ai/emerald_champions_plans.c` passed with actual
trainer ownership, reciprocal partner roles, Mega/base-form matching, foreign
namespace rejection and single-player recording support. The test runner now
includes the production trainer catalog while retaining its three synthetic
fixture IDs; this permits actual trainer names, identities and plans in battle.

The final integrated native run passed **20/20 test declarations**, including
parameterized cases (`work/ai-repair-final-native.log`). The authored scenarios
used complete production-generated parties, actual points, abilities, moves,
items, Normal-level calibration and active compiled plans:

| Trainer | Observed native result |
| --- | --- |
| Flannery | After You advanced Eruption before the opposing Rock Slides; Torkoal remained at full HP. |
| Tabitha | Surf activated surviving Coalossal's Steam Engine and Weakness Policy before Heat Wave. A 1-HP Coalossal survived via a safe partner decision. Against weaker targets already in unboosted Heat Wave's native minimum-damage KO range, the AI avoided unnecessary Surf. |
| Connie | Both partners selected Surf and gained one Special Attack stage through Storm Drain; the fixture's paralyzed opponents removed any useful Tailwind speed crossing. |
| Parker | Native Speed 62/54 and Room produced Earthquake followed by Instruct, safely through Telepathy. A separate nonimmune, injured Girafarig replacement was preserved rather than sacrificed to friendly fire. |
| Maura | The singer had escaped by turn three while Gothitelle retained the trap. By turn four Gothitelle had also escaped, both original partners survived, and the original trapped opponents fainted with reserves still available. |

These are controlled tactical situations, not complete trainer-battle wins or
campaign difficulty measurements. The run also retained generic regression
coverage for accurate KOs, Room expiration/re-establishment, useful redirection,
healing, Helping Hand, Justified, Mega ownership, simultaneous Perish deadlines
and candidate-state/RNG restoration. Unconditional Justified/Weakness Policy/
Steam Engine bonuses were removed: the actual stage and speed payoff must earn
the action, rather than a bonus for merely activating a gimmick.

Existing `EC expert pair` native scenarios exercise the real expert profile,
including Room expiration, Justified, Helping Hand, Mega ownership and state/RNG
restoration. Many older tests named “AI understands …” omit Smart Mon Choices
and therefore do not exercise the new pair planner. Passing those alone cannot
establish the revised campaign AI works.

The compact directives are contextual preferences, not fully scripted solutions
or a search of every future turn. Battle difficulty, long-term switching,
weather transitions, repeated support use and campaign pacing still require
actual opponent play. No complete-campaign or “every gimmick proven” claim is
made here.

In particular, `REDIRECTION` and `ALLY_COMBO` currently classify authored intent;
the runtime executes those combinations through shared conditional move logic,
not by reading those two bits. Calling the table “516 custom battle scripts”
would be inaccurate. Room, weather, Tailwind and setup bits influence bounded
preferences after validity checks. The initial unconditional Tailwind/setup and
redirection bonuses were reviewed: Tailwind now requires useful speed crossings
outside Room, setup requires a positive native evaluation, and redirection has
no separate bonus merely for standing beside a setter.

An additional sparse table now names 17 concrete actor/move/recipient tactics
across ten trainers, including Cristian, Nolan, Flannery, Parker, Cedric, Tabitha,
Sylvia, Connie, Kate & Joy and Quincy. The existing
authoring compiler verifies the actors, recipients and moves exist in those
loadouts (Commander and Gas name their actual ability holders). The records add 170 bytes.
The runtime lookup uses the actual trainer owner and base forms; a matching
reserve partnership opens a candidate for evaluation even without immediate
damage pressure. It does not force a switch or add a synthetic score bonus.
This is useful phase guidance, unlike duplicating mechanics in seven handlers.

The preconditions remain observable shared mechanics: Tabitha's Surf must leave Coalossal alive and
improve its subsequent attack; Cristian's Beat Up needs a physical Justified
recipient acting afterward; Flannery's After You must advance a useful Torkoal
action; Instruct needs a legal, repeatable attack. Commander is a native joint
entry effect, not an order to give Tatsugiri a hidden turn. The focused authored
scenarios use the production party generator, exact compiled points/loadouts
and Normal difficulty at the appropriate badge count, including the reserves.

## Approved focused creativity upgrades

- **Parker, E0165:** Lickilicky now leads beside Oranguru, replacing Girafarig in
  the lead position without removing any species. Its native-legal Earthquake
  replaces Stomping Tantrum. At Normal's live cap floor both leads are level 43;
  their generated Speed is Oranguru 62, Lickilicky 54. Inside Room, Lickilicky can
  attack before Instruct, and Telepathy protects Oranguru from repeated spread
  damage. Nonimmune replacement partners do not inherit that protection.
- **Maura, E0393:** the Psychic gym encounter now leads Jynx's Perish Song beside
  Shadow Tag Gothitelle, replacing Wobbuffet while retaining the other five
  species and Mega Slowbro. Both leads have Protect and useful attacks/support.
  The dedicated countdown policy evacuates the singer before the deadline,
  retains the trap temporarily, and then evacuates the trapper. Ghosts,
  Soundproof, Taunt, removing the trapper and successful pivot attacks remain
  counterplay. The native internal deadline is zero, not one.
- **Connie, E0462:** retains her roster and gains an explicit Lumineon Surf →
  Gastrodon Storm Drain role. Mutual Surf is not friendly-fire damage: the
  absorbing partner gains Special Attack. The revised explanation distinguishes
  absorbing a spread hit from redirecting an entire spread move.
- **Kate & Joy, E0294:** the Weezing/Slaking ability-suppression partnership now
  supplies a concrete reserve-entry reason. Bringing back Gas can restore
  consecutive Slaking attacks instead of blindly abandoning the position.
- **Quincy, E0480:** receives the same suppression-role guidance, while retaining
  his later gas-free Durant/Entrainment phase. Power Construct is explicitly
  unsuppressible in the configured ability table, so Zygarde's transformation
  does not require Gas to leave. A narrow reserve rule also considers ending
  authored Gas beside Truant/Entrainment when a legal recipient exists and no
  second Gas source keeps the ability suppressed. The resulting board still
  needs to justify the switch; this is not a forced scripted transition.

Party counts, encounter caps and the regional availability design are unchanged.
These are five targeted improvements, not a claim that all 516 branches have
received bespoke scripts or been played through.

One concrete remaining forecast limitation is same-turn manual terrain: the
native move evaluator sees terrain preferences, and switching into a terrain
ability is evaluated on the resulting board, but the fast pair forecast does
not yet update a local terrain state after a manual setter. Thus Rising Voltage,
Expanding Force, Grassy Glide and Psychic Terrain's priority blocking cannot be
claimed fully coordinated merely because their moves and abilities are legal.
