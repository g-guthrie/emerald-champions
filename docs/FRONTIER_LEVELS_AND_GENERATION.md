# Frontier level and generator audit

## Campaign progression

`src/caps.c:GetCurrentLevelCap` returns the cap for the first unearned
milestone: 14, 20, 30, 40, 45, 55, 60, 70 for the eight badges, then 80
until `FLAG_IS_CHAMPION`, then `MAX_LEVEL` (100). The Hall of Fame sets
the Champion flag. Frontier entry separately requires the game-clear flag.
There is no remaining postgame story cap to preserve. The existing milestone
runtime test exercises all nine transitions and the final 100 unlock.

Hard EXP limits, candies/Levelers, daycare, obedience, wild scaling and
legendary scaling consume the existing cap. None needs a new progression
rule. Battle EXP is currently disabled globally; the Leveler is the primary
leveling path and already follows this cap. Legendary encounters clamp to 100. Ordinary saved Pokémon must continue
to stop at 100: experience tables only have entries 0–100 and the boxed
experience field explicitly assumes that ceiling.

## Arena levels and consequences

The player brings six healthy non-Egg Pokémon. The Circuit saves their party,
temporarily normalizes it to 100, and restores it after the run. This avoids
requiring postgame grinding and preserves the moves, investments and items
the player prepared. Rewards retain their separate authored level of 80.

For wins `w` and party slot `s`, the opponent level is
`min(255, 100 - difficultyReduction + w / 6 + (s < w % 6))`.
The global campaign offsets remain Easy −4, Normal −2, Hard 0. Thus a Hard
run starts at six 100s; one win produces 101/100/100/100/100/100; six wins
produce six 101s. This makes the difficulty setting material to any comparison
of streaks. The opponent roster and AI do not become weaker on Easy.

Enemy experience is created at no more than 100. Only an active Circuit's
transient opposing party can retain a level above 100 during stat
recalculation. Mega/form changes therefore preserve the handicap. No Pokémon
or save structure is widened. Player, wild, PC, reward and ordinary trainer
level semantics remain unchanged.

Both live party levels and battle-controller levels are bytes. Level 255 is
an explicit saturation boundary, reached after 930 Hard wins (942 Easy).
Generation and the streak continue there, but level escalation cannot honestly
be called unbounded. Extending beyond 255 would require a separate audit of
controller payloads, battle data, level-based moves, arithmetic and UI.

At 255, ordinary base stats and Champions investment fit the existing 16-bit
stats and HP storage; three-digit level displays already accommodate 255.
Battle Tower flags prohibit capture and EXP rewards; the doubles healthbox
does not index an opponent EXP bar. Fixed-level damage and speed thresholds
increase with the handicap as intended. Relative team strengths are different
from Showdown's species-level-balanced random ladder because this arena uses
equal starting levels; empirical combat tuning remains necessary.

The current win counter is 16-bit and saturates instead of wrapping. Match
text needs five digits. Lifetime rewards remain independent of the current
run; saturating lifetime counters must not overflow before computing BP.

## Live generator

The ROM chooses a species family, a form, a competitive role and moves, then
an Ability, item, nature and Stat Points. It repeats this for each opponent
and validates the assembled team. There is no precomputed roster of teams.

Champions doubles templates take precedence. Pinned Gen 9 doubles templates
fill missing species and forms. Tera-dependent roles are omitted; form-required
items are retained and do not consume the one-Mega allowance. Remaining legacy
species use this project's authored individual doubles sets. Supplement
investments are converted from display order to engine stat order.

The pool includes the upstream competitive unevolved choices plus fully evolved
legacy species, rather than padding teams with every early evolution. Source
identities, omitted roles and exact generated-table provenance are recorded
in `data/emerald_champions/showdown_champions_random_doubles.json`.

Runtime checks cover duplicate species, type/weakness concentration, selected
weather conflicts, required field support, Commander partners, speed control,
physical and special pressure, and a limit on low-stat filler. Lead selection
rewards support and partner combinations without inspecting the player's team.
Bounded retries must fail cleanly rather than hang or start an incomplete battle.

The expanded port includes Showdown's bidirectional status/Choice-utility
incompatibility and redundant-attack rules. A culled move cannot be reinserted
to fill a short set. Required-item arrays (such as Arceus plates) are resolved,
not just singular item fields. Belly Drum/Sitrus HP is optimized for the actual
arena level by redistributing the same 66 Stat Points; this matters at level
100 because this engine's fixed-IV investment formula differs from mainline EVs.

All live Frontier challenge desks already route to this Circuit. Existing
competitive doubles AI is reused; no new fixed boss teams are needed. Generator
legality and structural cohesion are testable contracts. Claims of expert-level
combat difficulty require independent battle play, not merely passing those
contracts.

## September 6 battle-review follow-up

The generator now records the exact available terrain types, including manually set terrain, and accepts manual Sandstorm as sand support. Surge Surfer, Expanding Force and Grassy Glide require their matching terrain; Screen Cleaner cannot accompany allied screens. Field evaluation uses the actual Mega Ability. Lead selection rewards field setters beside their beneficiaries and excludes a pair with no repeatable attack, including zero-score ties.

Focused host tests execute the production field/coherence/lead functions with controlled move and species fixtures. They cover valid and invalid terrain, manual field support, Mega weather, allied-screen conflicts, passive openings, and weather/terrain pair ordering. These tests do not simulate whole generated battles. No new ROM was built during the final source review.
