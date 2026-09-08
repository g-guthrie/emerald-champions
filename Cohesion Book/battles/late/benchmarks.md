# Native Points arithmetic and narrow revision validation

These calculations use the snapshot's configured **fixed IV31 and flat Stat Points**, with integer Nature rounding. They are source arithmetic, not measured emulator damage or proof of an optimal spread. All point strings are HP/Attack/Defense/Special Attack/Special Defense/Speed, which differs from the engine enum order.

## Formula

[Native stat calculation](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/pokemon.c:1424>) applies `floor((2 × base + 31) × level / 100) + 5 + Points` before the integer Nature multiplier for non-HP stats. HP uses `floor((2 × baseHP + 31) × level / 100) + level + 10 + HP Points`. Zero stored IV does not produce a zero-IV stat under this configuration.

The benchmark cap is80 for the three League changes. At these positive offsets, the common floor does not bind: Hard uses cap+offset, Medium uses cap+offset−2, Easy cap+offset−4. Future revisits must use the live cap and the central floor rule. No proposed loadout encodes a fixed level or quietly changes its Points by difficulty.

## Proposed signature and Mega builds

| Trainer / Pokémon | Setting | Level | HP | Atk | Def | SpA | SpD | Spe | Additional effect |
|---|---|---:|---:|---:|---:|---:|---:|---:|---|
| GLACIA / WALREIN | Hard | 83 | 333 | 146 | 196 | 224 | 182 | 138 | Tailwind Speed 276 |
| GLACIA / WALREIN | Medium | 81 | 326 | 143 | 191 | 220 | 177 | 135 | Tailwind Speed 270 |
| GLACIA / WALREIN | Easy | 79 | 319 | 139 | 187 | 214 | 173 | 132 | Tailwind Speed 264 |
| PHOEBE / DUSCLOPS | Hard | 82 | 215 | 130 | 259 | 128 | 287 | 71 | Eviolite applies in battle; shown stats exclude item modifier |
| PHOEBE / DUSCLOPS | Medium | 80 | 210 | 126 | 253 | 125 | 280 | 69 | Eviolite applies in battle; shown stats exclude item modifier |
| PHOEBE / DUSCLOPS | Easy | 78 | 206 | 124 | 247 | 122 | 273 | 68 | Eviolite applies in battle; shown stats exclude item modifier |
| WALLACE / STARMIE_MEGA | Hard | 88 | 232 | 240 | 217 | 234 | 217 | 302 | Huge Power Attack 480 |
| WALLACE / STARMIE_MEGA | Medium | 86 | 227 | 235 | 212 | 229 | 212 | 297 | Huge Power Attack 470 |
| WALLACE / STARMIE_MEGA | Easy | 84 | 222 | 231 | 207 | 224 | 207 | 290 | Huge Power Attack 462 |

Walrein at Medium level81 has **326HP, 191Defense, 220Special Attack and 135Speed**; Articuno's Tailwind gives270 Speed before other modifiers. Its bulk/coverage/control role is intentional. These numbers do not claim it outdamages the displaced Glastrier. Snow, Veil, Thick Fat and Chien-Pao's allied Defense effect belong in actual damage calculations, not in an unsupported promise of survivability.

Mega Starmie at Medium level86 retains **297Speed** with the new Jolly spread, matching the baseline Timid spread. Its Attack becomes235 before Huge Power and **470 after Huge Power**, compared with the baseline's287 Special Attack for its all-special set. This is a useful structural reason for the physical revision, not a direct damage ratio: Liquidation/Psycho Cut/Aqua Jet have different power, defenses, priority, accuracy and coverage from Hydro Pump/Thunderbolt/Ice Beam.

Dusclops's Eviolite modifies its defenses in battle and Pressure changes PP use only for applicable targets. Its proposed Helping Hand/Pain Split role must be validated against actual partner attacks and HP states. No unsupported Trick Room or zero-IV adjustment is added.

## Existing Sitrus choice retained after checking the actual contract

[Native Berry threshold](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/battle_util.c:5529>) gives Sitrus a rounded-up half-HP threshold. Gluttony's flavor-Berry condition uses floor-half. This makes a tempting Figy substitution unreliable at odd HP, despite its larger healing fraction. Keep Jack's existing Sitrus, unchanged Points and current moves.

| Linoone level | Max HP | After full-HP Drum | Sitrus threshold | Figy + Gluttony threshold | Sitrus activates | Figy activates |
|---:|---:|---:|---:|---:|---|---|
| 68 | 207 | 104 | 104 | 103 | True | False |
| 70 | 212 | 106 | 106 | 106 | True | True |
| 78 | 235 | 118 | 118 | 117 | True | False |
| 80 | 241 | 121 | 121 | 120 | True | False |
| 98 | 293 | 147 | 147 | 146 | True | False |
| 100 | 299 | 150 | 150 | 149 | True | False |

At the important Medium level68 case, maxHP207 becomes104 after Drum. Sitrus activates at104; Gluttony Figy would require103 or lower. No new implicit HP normalization is proposed to rescue that item change. This is why a source-grounded review preserves a seemingly suboptimal existing item.

## Exact changed-set checks

The volume renderer verified all proposed moves for Ronald's Raichu, Phoebe's Dusclops, Glacia's Walrein, Wallace's Starmie, Quincy's Power Construct Zygarde and Colton's Eternal Floette against the snapshot's pinned `showdown_champions_learnsets.json`, and all proposed base abilities against the configured species inventory. Each final team retains legal66-total/32-per-stat Points, distinct non-NONE held items, its authored party size and unchanged offsets.

Ronald's Thunder is present in the pinned Raichu learnset. Mega Raichu Y has No Guard in the configured species data; Thunder therefore has stronger immediate power/status pressure after Mega at lower PP. The native battle check must still cover pre-Mega rain dependence, reciprocal No Guard accuracy and whether spending a setup turn is actually profitable.

## Evidence still required

- Materialize the six exact revised teams through the existing authoring pipeline and compare full source/native loadouts.

- Verify the proposed roles with actual damage, speed, item, weather, terrain and AI decisions under each effective difficulty level.

- For restored Walrein/Dusclops, play meaningful favorable and unfavorable boards and more than one player solution; preserve the encounter's intended severity without assuming a nostalgia change is automatically balanced.

- Verify Mega Starmie receives Huge Power and uses physical attacks/priority correctly, and that the shared Mega AI handles form tradeoffs and Steven's two eligible candidates.

- [Machine-readable arithmetic](../../review/late-battles-benchmarks.json). No ROM or test ELF was built for this appendix.
