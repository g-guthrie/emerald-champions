# Emerald Champions Overworld Static Encounter Fresh Pass

Date: 2026-08-29

## Verdict

The executable map data contains 25 visible Legendary Sign quests and 32 unique physical one-off Pokémon encounters across 25 maps. Every physical one-off has a unique persistence flag, a stable local object ID, a fixed authored position, a live script, a legendary-battle transition, capture cleanup, and a complete indexed-color overworld sheet. The five Chansey healers and the decorative Carbink are intentionally excluded from that count.

The visible collection is visually coherent with Emerald. Twenty-six species use 32x32 frames; Dialga, Zygarde, Reshiram, Regigigas, Palkia, and Enamorus use intentional 64x64 frames. Regigigas uses the complete 384x64 six-frame giant sheet requested for its Sealed Chamber encounter. A fresh contact-sheet inspection found no missing direction, truncated frame, placeholder, or style outlier.

Landorus, Thundurus, and Tornadus now form a real campaign progression rather than dossier-only promises:

- Thundurus is visible on Route 110 after Dynamo Badge with a Manectric-family party key.
- Landorus is visible at the Route 111 Ruins after Balance Badge with a Castform-family party key.
- Tornadus is visible on Route 119 after Balance Badge with a Castform-family party key.

They retain distinct locations, real sprites, independent flags, native cries and battle setup, capture-only completion, and repeat-safe defeat behavior.

## Source proof

Ground truth inspected:

- `data/maps/*/map.json`
- each corresponding live `scripts.inc`
- `src/data/pokemon/legendary_signs.h`
- `src/legendary_signs.c`
- `graphics/pokemon/*/overworld.png`
- `src/data/pokemon/species_info/*.h`
- `include/constants/flags.h`
- `data/layouts/layouts.json` and map block data

`scripts/verify_legendary_signs_and_circuit.py` now fails closed if a physical encounter:

- duplicates a supposedly one-off species or persistence flag;
- points outside its map layout;
- can wander away from the authored position;
- lacks its map object, script, local ID, flag, battle transition, or cleanup path;
- lacks a valid PNG signature, indexed palette, native 32px/64px frame height, or complete six/eight-frame sheet;
- loses the giant Regigigas sheet;
- loses the deliberately accessible Route 110 or Route 119 weather-trio placement.

Current result:

```text
Legendary Signs: 82 complete acquisition definitions
Visible Legendary Sign quests: 25
Visible overworld encounters: 32 physical one-off Pokémon
Showdown Circuit: 311 variants, 444 templates
PASS
```

## Human-only boundary

Static source proves wiring, persistence, assets, and placement bounds. It cannot prove the final emotional read of scale against every surrounding tileset, camera behavior during every interaction direction, or the subjective pacing of discovering all 32. Those remain part of the fresh-save playthrough rather than being mislabeled as code proof.
