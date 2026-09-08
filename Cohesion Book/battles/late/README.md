# Late battle volume — complete branch-by-branch revision specification

This volume covers **250 branches, 244 retained encounters and 1175 Pokémon slots** from E0251 through E0513. Each branch received an individual source review of its actual loadout, lead/support interactions, reserve roles, competitive counterplay and governing AI dependencies. The 250 distinct assessments are manually authored in the review notes and rendered beside complete native loadouts; these are not automatic KEEP labels.

Disposition totals: {'KEEP': 127, 'REPAIR': 117, 'REVISE': 6}. **Six branches have exact loadout revisions**; other REPAIR entries correct misleading authored explanations, restore the missing Wallace interaction or depend on shared state-aware AI work. Existing successful compositions remain the baseline.

## Governing rules

The latest user decisions supersede the original opening exception: trainer competition has no singles, standard teams never have fewer than four, and every team receives the same expert tactical standard. Medium uses a runtime floor of current player cap minus two, including gyms. Three-member E0409 trainer contributions are one opposing six-member multi side. Preserve authored offsets and Points; the common runtime level rule determines effective levels. No IV-zero recommendation is valid under the configured fixed-IV31 Champions stat formula.

Lilycove E0302 entries reproduce the six Hoenn authored templates. The early volume owns the regional resolver: the unchosen starter line from the player's two choices replaces only slot6, at the authored stage and +3 offset. The fixed Mega partner remains Metagross, Salamence or Tyranitar; the regional starter does not gain a second Mega entitlement.

Target the most coherent, capable and clever use of each roster. These source judgments do not establish mathematical global optimality or prove AI execution. Risky/Conservative may not hallucinate maximum/minimum damage rolls; the central AI specification owns probability-aware evaluation and can reinterpret historical trait labels.

## Read the catalog

| Chapter | Encounter groups | Branches |
|---|---:|---:|
| [01-E0251-E0275.md](01-E0251-E0275.md) | 20 | 20 |
| [02-E0276-E0297.md](02-E0276-E0297.md) | 20 | 20 |
| [03-E0298-E0317.md](03-E0298-E0317.md) | 20 | 25 |
| [04-E0318-E0337.md](04-E0318-E0337.md) | 20 | 20 |
| [05-E0338-E0362.md](05-E0338-E0362.md) | 20 | 20 |
| [06-E0363-E0387.md](06-E0363-E0387.md) | 20 | 20 |
| [07-E0388-E0407.md](07-E0388-E0407.md) | 20 | 20 |
| [08-E0408-E0427.md](08-E0408-E0427.md) | 20 | 21 |
| [09-E0428-E0449.md](09-E0428-E0449.md) | 20 | 20 |
| [10-E0450-E0469.md](10-E0450-E0469.md) | 20 | 20 |
| [11-E0470-E0489.md](11-E0470-E0489.md) | 20 | 20 |
| [12-E0490-E0509.md](12-E0490-E0509.md) | 20 | 20 |
| [13-E0510-E0513.md](13-E0510-E0513.md) | 4 | 4 |

## Exact loadout revisions

| Encounter | Change | Purpose |
|---|---|---|
| E0430 Ronald | Mega Raichu Y: Thunderbolt → Thunder | Exploit actual No Guard and Kingdra rain; retain all other data. |
| E0480 Quincy | Aura Break Zygarde50 → Power Construct50 | Enable Complete, the required prerequisite for the existing Zygardite Mega. |
| E0504 Colton | ordinary Floette → Eternal Flower Floette | Repair the exact base-species prerequisite for Floettite. |
| E0494 Phoebe | Shedinja → Eviolite Dusclops | Restore the signature Pokémon as burn, fixed-damage, Helping Hand and Pain Split support. |
| E0495 Glacia | Glastrier → Thick Fat Walrein | Restore the signature Pokémon with Water coverage, Encore and meaningful snow/Veil durability. |
| E0497 Wallace | Mega Starmie: special → Huge Power physical set | Give the real Mega ability a role and add physical rain pressure plus Aqua Jet. |

Each affected branch reproduces its entire final team. Legality of the changed Pokémon's moves and abilities was checked against the snapshot's pinned learnset input and configured species inventory. This does not replace materialization, build or live battle validation.

## Signature species decisions

| Principal trainer | Baseline identity and final decision |
|---|---|
| Spenser | Crobat, Slaking and Lapras already identify the Palace Maven; preserve the six. |
| Winona | Altaria has a substantial Tailwind/Helping Hand role. Preserve Zapdos and other premium partners. |
| Maxie | Mightyena, Crobat and Camerupt remain in his hideout team; Crobat/Camerupt remain in the multi climax. Preserve. |
| Matt/Shelly | Keep their deliberate rain/faction identities and premium species; no invented single nostalgia species replaces their strategies. |
| Tate and Liza | Solrock/Lunatone lead a coherent low-Speed plan. Preserve. |
| Archie | Sharpedo is the actual Mega ace, with Tentacruel supporting rain. Correct obsolete Mega Eelektross prose. |
| Juan | Kingdra retains a strong Specs/Swift Swim role. Preserve. |
| Wally | Gallade preserves the defining Ralts-family bond in both late encounters; Roserade/Magnezone reinforce campaign familiarity. Preserve. |
| Sidney | Mega Absol Z is the signature closer. Preserve the legendary-heavy composition. |
| Phoebe | Restore Dusclops for Shedinja. Banette is a secondary nostalgic association considered but not added: two replacements would unnecessarily displace successful current roles. The restored primary signature is substantive. |
| Glacia | Restore Walrein for Glastrier. Glalie is a secondary association considered but not added: retain the established Ninetales/Kyurem engine, Articuno field support, Chien-Pao and Mega Baxcalibur. There is no arbitrary legendary cap. |
| Drake | Salamence already leads with Intimidate/Tailwind. Preserve. |
| Wallace | Milotic already supports the Champion team. Preserve it and optimize the separate Mega Starmie ace. The postgame legends exhibition is explicitly a different collection showcase. |
| Leaf | Kanto ensemble of Ninetales, Kangaskhan, Gengar, Tangrowth and Mewtwo supplies identity without inventing a mandatory starter. Preserve. |
| Steven | Skarmory, Cradily and Metagross form a strong iconic core. Preserve Rayquaza too; allocate its competing Mega eligibility through the shared decision policy, with Metagross the signature tie-break. |
| Cynthia | Roserade, Togekiss, Spiritomb, Lucario and Garchomp already create strong recognition. Preserve and correct Mega Garchomp Z typing. |

Glastrier's displaced campaign showcase is intentional. Its obtainability and Circuit eligibility remain global inventory responsibilities; no historical showcased-species count should force an unrelated swap. Shedinja remains elsewhere in the authored campaign.

## Cross-cutting findings and implementation order

1. Fix form-aware AI forecasting and owner-scoped Mega allocation before interpreting a rewritten loadout as a solution. Mega Starmie, Excadrill, Meganium, Meowstic, Victreebel, Glimmora, Clefable, Scolipede, Raichu X/Y, Floette and Garchomp Z demonstrate why base species assumptions are insufficient.

2. Implement the six precise loadout changes and the optional Wallace interaction restoration. Regenerate native parties from the existing authoring pipeline, preserving unrelated teams.

3. Replace erroneous plan/counterplay fields from REPAIR entries; dialogue must describe tactics or outcomes actually established, not assert that every player used one particular solution. Coordinate source script wording with the world volume.

4. Validate the shared decision dependencies against the actual revised teams: Choice-lock/Telepathy reserves, Palafin activation, Commander pairing, Surf-trigger engines, loss of speed/priority abilities on Mega, target immunities, weather renewal and probability-aware damage.

5. Conduct battle validation at actual Hard/Medium/Easy effective levels with fixed-IV31 flat Points. Restored iconic species must carry their specified roles in credible winning lines; documentation coverage alone is not proof of difficulty.

## Evidence and reproducibility

- [Machine-readable full review](review.json) contains every trainer ID, baseline and final roster, unique assessment, exact deltas, AI dependencies and acceptance criteria.

- [Volume coverage summary](../../review/late-battles.json) reconciles all 250 trainer IDs and 244 encounter groups.

- [Source-derived arithmetic and revision checks](benchmarks.md) records Points calculations and narrow legality checks.

- `render_volume.py`, `baseline.jsonl` and `notes-*.txt` are book-authoring helpers/data only. They do not replace game source or introduce new gameplay machinery.

No ROM, game source, native build artifact, save or installed game was modified. No battle was newly played during this documentation volume. Historical review records are context only; this volume's source snapshot and explicit future acceptance criteria define its evidence boundary.
