# Wild-area follow-up — September 8, 2026

The user authorized discretionary refinements after the full area pass. This follow-up addresses the two remaining placement decisions discussed: Route123's weak orchard returns and an optional grown Bug/Rock catch for revisiting Petalburg Woods. It changes **11 species slots in two native tables**. All other maps, field-method access, wild levels, frequencies, slot weights, trainer teams, species stats and difficulty settings are untouched by this pass.

**Route123: a mature orchard**

| Pokémon | Ordinary chance | Why I chose it |
|---|---:|---|
| Hydrapple | 13% | A memorable grown apple-dragon among the fruit trees; a strong special attacker worth visiting for. |
| Arboliva | 12% | The mature olive tree supplies Seed Sower and special damage; Smoliv remains available on early Route104. |
| Sunflora | 11% | A familiar garden resident that fits the Berry Master's cultivated landscape. |
| Oranguru | 10% | More prominent doubles support among the orchard's canopy dwellers. |
| Escavalier | 10% | A sturdy physical Bug/Steel option develops the old Karrablast population. |
| Accelgor | 8% | Its speed and utility complement Escavalier while retaining both sides of the paired evolution. |
| Flamigo | 8% | A useful Fighting/Flying visitor around the orchard's wet margins. |
| Bellossom | 7% | A grown flowering resident replaces the late Oddish catch; Gloom remains on Route120 for either evolution branch. |
| Greedent | 6% | A mature berry-hoarding mammal fits better than finding another low-stage squirrel this late. |
| Kricketune | 5% | Keeps the cricket family as a smaller garden encounter with usable support moves. |
| Spidops | 5% | A grown ambush spider offers Sticky Web, First Impression and Circle Throw through existing preparation. |
| Carnivine | 5% | Retains the unusual wetland plant among the cultivated fruit and flowers. |

Applin remains in Woods2, Smoliv remains on Route104, and Karrablast/Shelmet remain in the unchanged Safari southwest habitat. The four basic forms that lose direct encounters—Kricketot, Tarountula, Oddish and Skwovet—remain obtainable through breeding the ordinary evolved catches. Their configured egg groups and evolution relations were checked against the Daycare's reverse-evolution egg-species logic. This is source review of acquisition, not an executed breeding session. No early baby placement changes.

There are now **654 direct ordinary Hoenn species/form identifiers**, up from 652 in the preceding build. The six newly direct species are Hydrapple, Arboliva, Bellossom, Greedent, Kricketune and Spidops; the four basic forms above become breeding acquisitions. All previously represented evolutionary families remain catchable.

**Petalburg Woods: optional Rock Smash return**

| Pokémon | Ordinary Rock Smash chance |
|---|---:|
| Pineco | 35% |
| Crustle | 25% |
| Ferroseed | 18% |
| Dwebble | 12% |
| Heracross | 10% |

Only the former 25% Dwebble slot changes to Crustle. Dwebble's family still has earlier Route106 grass access. Existing preparation supplies Crustle with Sturdy, White Herb and Shell Smash; no new item or move access was introduced.

**First-visit grass is unchanged:** Heracross 11%, Pineco 8%, Ferroseed 5%, alongside the existing Shroomish, Slakoth, Pichu and other forest residents. Mienfoo, Timburr, Pachirisu, Kartana, Pheromosa and all three Dive rosters retain their placements. Rock Smash supplies a bonus; it does not gate those early forest choices.

**Evidence**

- Exact structural comparison isolates the changes to these 11 species slots. Every other native field and area is identical to the pre-follow-up snapshot.
- The existing wild-distribution check passes all 264 active tables, including configured species, numeric fields, probabilities, reversed selection and exact water-authoring agreement.
- The configured evolution-family review retains every formerly represented family; breeding access for the four basic forms is described above.
- Production ROM: [pokeemerald-wild-polish-20260908-release.gba](../pokeemerald-wild-polish-20260908-release.gba), with adjacent ELF and input stamp. SHA-256: `1a4a366695dc6eefea9d92c232796ee632479867fff9f1b9836168fb473d56d5`.
- A shared AI source changed during the first compilation, so that candidate was rebuilt. The final rebuild used a stable set of 28,977 declared inputs, digest `55002336679edaf73febcdb9a9c3d8e49c39a245fca027cfb6a966916029bcaa`.
- All existing release checks passed for the final ROM, including map/tile integrity, authored trainer/preset/Circuit agreement, wild tables, patch integrity, ROM/ELF/header/checksum/memory validation and the source/artifact stamp.
- All 256 default native method tables / 2,361 encounter slots across 138 areas matched the final production ROM bytes.
- No capture traversal, breeding session or campaign battle-balance playtest was performed for this follow-up. Preserve the existing fresh-save requirement. Previous ROMs remain available; nothing was committed, pushed or uploaded.

Source: [native encounter tables](../src/data/wild_encounters.json), [all 138 final rosters](../work/wild-polish-20260908/after_area_rosters.json), [the 11 exact changes](../work/wild-polish-20260908/slot_changes.json), and [the previous 108-area revision](WILD_AREA_REVISION_2026_09_08.md).

These are distribution decisions for the next playtest build. They do not establish measured early battle difficulty, and no speculative Heracross nerf was made.
