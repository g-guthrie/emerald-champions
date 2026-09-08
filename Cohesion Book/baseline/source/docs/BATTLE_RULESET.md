# Battle rules and Mega Kangaskhan

This ROM is a custom Emerald Champions campaign, not a faithful recreation of one mainline generation or an official VGC regulation. `GEN_LATEST` resolves to `GEN_CHAMPIONS`, which sits above Gen9. Most configurable mechanics consequently use their modern branches, with explicitly implemented Champions overrides.

The setup combines modern types/abilities/move effects, Gen9-oriented level-up tables, retained older teachable moves and curated presets, plus selectively imported Inclement base-stat changes and custom Megas. Perfect IVs and 66 total Stat Points replace traditional IV/EV training. The campaign adds its own level caps, trainer teams and doubles format; those are separate from an official competitive ruleset. A configuration named Champions does not imply that every roster or move restriction from the official game is imposed here.

## Mega Kangaskhan in this ROM

- Base stats in HP/Attack/Defense/Sp.Atk/Sp.Def/Speed order: 105/125/100/60/100/100. No special Kangaskhan stat rebalance is applied.
- Parental Bond uses the Gen7+ second-hit modifier of 0.25. The ordinary two-hit total is roughly 1.25 times one hit, subject to rounding, damage rolls and move effects. Spread attacks in doubles and already-multihit moves have their own exclusions.
- Sucker Punch has 70 base power, the Gen7+ value.
- Return, Power-Up Punch and Seismic Toss remain in the authored Kangaskhan presets. Power-Up Punch is not listed in the raw ordinary teachable table, so the preset layer matters when assessing the actual available tools.
- The Mega form's Speed applies on the Mega Evolution turn under the modern turn-order setting.

## Historical changes

The major direct nerf in Sun/Moon was Parental Bond's second hit dropping from 50% to 25%. That halves the additional hit, not the entire attack: a normal two-hit total goes from roughly 150% to 125%, about a 16.7% reduction. Sucker Punch also fell from 80 to 70 base power in Gen7, a general move change that affected Kangaskhan. Later game/format move availability can further change its strength, but this ROM retains the legacy presets listed above.

Sources checked: local `include/config/general.h`, `include/config/battle.h`, `include/config/pokemon.h`, `src/battle_util.c`, `src/data/moves_info.h`, `src/data/pokemon/species_info/gen_1_families.h`, and `data/emerald_champions/emerald_champions_battle_sets.json`.

Historical simulator reference: [Parental Bond generation modifier](https://github.com/smogon/pokemon-showdown/blob/master/sim/battle-actions.ts), [Gen6 Sucker Punch override](https://github.com/smogon/pokemon-showdown/blob/master/data/mods/gen6/moves.ts), and [current Sucker Punch data](https://github.com/smogon/pokemon-showdown/blob/master/data/moves.ts).

This task changes availability and nursery rewards; it does not change these battle rules.
