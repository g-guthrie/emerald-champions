# Live Frontier preview

This local ROM builds the shared Champions doubles Circuit. Every opponent is
assembled by code running inside the ROM; there is no precomputed list of teams.
The pool contains 756 species/forms and 1,322 individual role/set templates,
using pinned Pokémon Showdown Champions and Gen 9 doubles sources plus legacy
individual doubles supplements.

Bring six healthy Pokémon after becoming Champion. Your team temporarily
battles at level 100. Opponents start at 100 on Hard, 98 on Normal, or 96 on
Easy, preserving the game's global difficulty offsets. Each win adds one total
opposing level across six slots. Opponents saturate at 255; your original party
is restored when the run ends. Reward acquisition levels remain 80.

The seven Frontier facilities currently have 13 challenge desks, all entering
this same Circuit. Separate tournament, branching-route, and exploration
adventures are proposals, not implemented features of this preview.

## Verified

- Release ROM builds; its header, checksum, input identity and memory limits pass.
- The actual ROM boots under the native mGBA runner using no user save.
- 15 Frontier runtime tests and three campaign-level/stat regressions pass.
- The runtime sample generates 2,048 teams (12,288 Pokémon), checking set
  completeness, duplicates, item conflicts, speed control and investment bounds.
- Actual doubles battles verify level-255 damage and above-100 Mega Evolution.
- Ten focused host regressions pass; the original pre-League roster check passes.

## Release status

This is a local preview, not a published or fully certified release. Four
project-wide checks remain blocked by the separate addition of seven Mega
forms: visual inventory, preset archive counts, species-rebalance inventory,
and campaign Mega showcases. The initial and follow-up logs are included.
Competitive difficulty still needs independent playtesting; structural
coherence and runtime correctness do not establish expert-level balance.

See FRONTIER_LEVELS_AND_GENERATION.md for the level audit and design details,
validation.json for the results, and THIRD_PARTY_NOTICES.md for source credits.

ROM SHA-256:
195924b9a97e23b0cc68eeb1c35a8284147f67042604ae8556432c21083ca255
