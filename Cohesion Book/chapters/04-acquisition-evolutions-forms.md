# 04 — Obtaining every functional Pokémon and form

The proposed game preserves its generous existing collection and makes every supported battle option practical in one save. This means a player can capture, prepare, use, evolve, transform and later try another supported option. It does not require simultaneously storing a living dex: retain the existing 14 boxes/420 ordinary slots and native fusion storage.

This volume accounts for every configured species/form entry. The appendix distinguishes ordinary acquisition, evolution, persistent forms, cosmetic choices, temporary battle states, Megas and explicitly inactive legacy/gimmick templates. A potential graph is an inventory aid, not proof: it cannot by itself establish reachable NPCs, valid breeding, evolution conditions, two-component fusion or enough copies of a nonbreedable branching Pokémon.

The current snapshot already fixes several earlier findings. Meltan evolves in New Mauville; Ursaring uses Peat Block at night without a Hisui requirement; Bisharp uses a held Leader's Crest; several Hisuian evolutions have Hoenn location conditions; Burmy and Deerling have roots; and Runerigus has a live carved-stone interaction on Route111. Preserve these improvements. Do not reimplement an obsolete gap.

The final specification below adds eight exact wild-form substitutions to the lead editor's 21 roster additions, three independent Galarian bird Signs, seven prepared gifts, an explicit cosmetic/costume service, missing form tools in the existing archive, and a postgame Deoxys service. It also closes precise evolution-readiness and Mega-base requirements without reducing existing preparation move access.

## FORM-01

**Disposition: KEEP.** Existing successful native behavior is preserved except for the exact changes specified here.

**Source and coverage contract.** The snapshot configures 1,579 entries including SPECIES_NONE: 1,578 actual species/forms spanning 1,025 National Dex identities. The full [species/form index](../appendices/species-and-forms.md) and [machine ledger](../review/acquisition-forms.json) identify each disposition and its immediate source or required transition. The ledger retains complete root records and condition-bearing evolution/form steps.

Do not use a shared National Dex number, a form-enumeration table or a reversed evolution arrow as acquisition proof. The proposed closure uses actual active form-change tables, explicit fusion conjunctions, native breeding exceptions and a resource ledger for unique branching Pokémon. It deliberately excludes inactive G-Max, Ultra Burst, Stellar/Tera-only states, legacy Totems and promotional templates without an additional supported competitive capability. Normal Terapagos→Terastal through Tera Shift remains usable; its name does not make that automatic ability transformation a banned selectable gimmick.

The Appendix's access paths are source-backed implementation specifications. They are not claims that every path was newly traversed in an emulator. WILD-ENGINE-01 must ensure failed/gated ordinary encounter construction, including Sweet Scent, does not launch a stale opposing party. Actual final capture formats follow the lead editor's global format decision; this volume does not quietly decide the remaining wild-format question.

**Acceptance:** reconcile the configured species set dynamically, not against a historical species-count quota. Every advertised functional entry needs one documented path whose actual prerequisites can be met in the same save. Verify each new interaction and representative native transition classes; do not manufacture thousands of redundant tests that merely echo rows.

## FORM-02

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**Extend the existing free evolution archive with 14 explicit form tools.** Its current 45 evolution items remain. Preserve free acquisition and existing archive access; adding a tool does not bypass the species' own acquisition requirement.

| Item | Functional access |
|---|---|
| ITEM_ROTOM_CATALOG | Rotom's five appliances and return to ordinary Rotom |
| ITEM_GRACIDEA | Shaymin Sky under native day/status conditions |
| ITEM_PRISON_BOTTLE | Hoopa Unbound, with native return/reapplication behavior |
| ITEM_RED_NECTAR | Oricorio Baile |
| ITEM_YELLOW_NECTAR | Oricorio Pom-Pom |
| ITEM_PINK_NECTAR | Oricorio Pa'u |
| ITEM_PURPLE_NECTAR | Oricorio Sensu |
| ITEM_ADAMANT_CRYSTAL | Origin Dialga while equipped |
| ITEM_LUSTROUS_GLOBE | Origin Palkia while equipped |
| ITEM_GRISEOUS_CORE | Origin Giratina under the configured modern item rule |
| ITEM_DOUSE_DRIVE | Douse Genesect/Water Techno Blast |
| ITEM_SHOCK_DRIVE | Shock Genesect/Electric Techno Blast |
| ITEM_BURN_DRIVE | Burn Genesect/Fire Techno Blast |
| ITEM_CHILL_DRIVE | Chill Genesect/Ice Techno Blast |

The existing generator [scripts/generate_emerald_champions_mega_archive.py:29](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/scripts/generate_emerald_champions_mega_archive.py:29>) derives the evolution-item list. Add a small explicit supplemental form-tool set and union it with the current derived evolution items; keep the generated output as the archive's single authoritative input. Do not edit only generated output or add separate shops for individual tools. The old generator's broad name and historical count floors do not define the design; revise checks to validate actual required entries and protected exclusions.

Retain the separation between ordinary preparation and earned progression items. Do not add Mega Stones, the Mega Ring, Red/Blue Orbs, Rusted Sword/Shield, Ogerpon masks, Arceus plates, fusion tools or Meteorite to this new free-tool set. The native relic system already uses 24 item-pending bits and six earned-group bits in its 32-bit state: these 14 archive additions need no relic bits and must not overflow that ledger.

Silvally's17 Memories are already in the free Species held-item category ([src/field_specials.c:394](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/field_specials.c:394>)); preserve that route. Their protected-preset classification prevents silent replacement and does not mean they are unavailable. Birch already gives DNA Splicers, Zygarde Cube, N-Solarizer, N-Lunarizer and Reins of Unity through his postgame research reward. Reveal Glass already belongs to the Weather Institute. Preserve those world rewards.

**Acceptance:** obtain and use each added tool through its native party menu; cancellation preserves items/Pokémon, full Bag does not consume a one-time entitlement, and finite/earned relics remain excluded. Verify Rotom's actual appliance ability/move changes, all four Oricorio types, all four Genesect drives and exact modern Origin items. A pinned-source move comparison is advisory, never permission to remove existing expanded moves.

## FORM-03

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**A prepared female Indeedee from Wally's aunt.** The current Verdanturf Meadow table provides the male form. [src/daycare.c:913](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/daycare.c:913>) chooses a fixed egg species, with specific exceptions for Nidoran, Volbeat, Manaphy and certain forms; it has no male-Indeedee→female-species branch. `SetInitialEggData` preserves that species, and male Indeedee is male-only. A random personality does not repair the missing female form.

Use the existing aunt object at (2,4), elevation 3, in `VerdanturfTown_WandasHouse`. It currently routes through `VerdanturfTown_WandasHouse_EventScript_MegaGift_GARDEVOIRITE` (`scripts.inc:170`). Preserve that wrapper, its successful Bag receipt check and its flag. A full Bag must retain the current failure path; Indeedee acceptance must never gate Gardevoirite.

After the aunt's appropriate existing story text, offer: “This Indeedee loves looking after a partner. Would you like her to join your team?” Convert only the endings of the aunt's mutually exclusive text branches into one shared gift continuation. Do not call an end-terminated script expecting it to return. Declining leaves the offer available; successful party or PC delivery sets FLAG_EC_GIFT_INDEEDEE_F. No room/visibility state changes are needed.

| Species | Level | Item | Ability | Nature | Points HP/Atk/Def/SpA/SpD/Spe | Moves |
|---|---:|---|---|---|---|---|
| INDEEDEE_F | min(current cap,25) | PSYCHIC_SEED | PSYCHIC_SURGE | CALM | 32/0/2/0/32/0 | FOLLOW_ME / HELPING_HAND / PSYCHIC / PROTECT |

This is the existing female Indeedee default preparation build ([src/data/pokemon/emerald_champions_battle_sets.h:12764](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/data/pokemon/emerald_champions_battle_sets.h:12764>)), with a valid ability and useful immediate doubles role. Other builds remain freely selectable.

**Acceptance:** the received species is actually INDEEDEE_F with its female stat/ability data; the existing stone and story dialogue remain available in every branch. Full party/full PC failure leaves the gift flag unset. Decline, later receipt, reload and repeat interaction cannot duplicate or lose the entitlement.

## FORM-04

**Disposition: REVISE.** Existing successful native behavior is preserved except for the exact changes specified here.

**Eight precise wild-form additions.** These preserve existing slot weights and levels. They supplement the lead editor's 21 changes; they do not replace that global plan.

| ID | Map/method | Native zero-based slot | Replace | Final species | Chance | Levels |
|---|---|---:|---|---|---:|---|
| FORM-WILD-01 | MtPyre_2F land | 9 | SINISTEA (Phony) | SINISTEA_ANTIQUE | 5% | 24 |
| FORM-WILD-02 | MtPyre_5F land | 9 | POLTCHAGEIST (Counterfeit) | POLTCHAGEIST_ARTISAN | 5% | 24 |
| FORM-WILD-03 | Underwater_SeafloorCavern land | 5 | KINGDRA | TATSUGIRI_DROOPY | 8% | 40–44 |
| FORM-WILD-04 | Underwater_SeafloorCavern land | 8 | WISHIWASHI | TATSUGIRI_STRETCHY | 6% | 40–44 |
| FORM-WILD-05 | Route118 Super Rod | 8 | BASCULIN (Red) | BASCULIN_BLUE_STRIPED | 15% | 55–60 |
| FORM-WILD-06 | SafariZone_Northwest land | 11 | DITTO | SQUAWKABILLY_YELLOW | 5% | 29 |
| FORM-WILD-07 | Route121 land | 5 | KOMALA | MAUSHOLD_THREE | 8% | 26 |
| FORM-WILD-08 | DesertUnderpass land | 11 | ESPATHRA | DUDUNSPARCE_THREE_SEGMENT | 5% | 45 |

The antique/artisan tea discoveries belong on different Mt. Pyre floors. Their ordinary counterparts remain on other floors, and the free archive already provides Chipped Pot and Masterpiece Teacup. Breeding deliberately returns Phony/Counterfeit offspring, so it is not an alternate route to the special tea forms.

At the submerged submarine approach, Curly Tatsugiri and Dondozo keep their existing 10% slots. Droopy/Stretchy add genuinely different Commander/Order Up options; they are not cosmetic conversions. Kingdra and Wishiwashi retain numerous other aquatic sources.

Route118 keeps Red Basculin at 18% Surf while Blue becomes a fishing discovery. Blue's Rock Head differs from Red's Reckless, so a generic color menu would bypass a real ability distinction. Yellow Squawkabilly supplies the Sheer Force group; Green from the lead editor's Route118 change supplies the Guts group. Ditto remains readily available on Route117. Styling is permitted only within Green↔Blue and Yellow↔White groups.

Keep the native rare-PID Dudunsparce/Maushold evolutions as optional surprises, but do not make players repeat approximately 100 acquisitions to access those forms. The direct rare-form discoveries above give a practical alternative. Their different weights prevent declaring them mechanically interchangeable cosmetics. Komala remains on Route118; Desert Underpass still has Flittle at 10%, which evolves into Espathra. All four Pumpkaboo sizes already occur on Route120 at 10/6/5/5%; preserve them and their corresponding Gourgeist evolutions.

For fishing, use the lead editor's one optional `fishing_species_overrides` map keyed by native slots0–9, applied after existing ten-slot generation: Route117 `{"0":"WOOPER_PALDEA"}` and Route118 `{"8":"BASCULIN_BLUE_STRIPED"}`. Do not add separate Old Rod and Super Rod override systems or perturb unrelated generated slots/used-pool behavior.

**Acceptance:** source/native slot agreement, no overlapping contradictory proposals, every displaced family retains a practical source, and ordinary probabilities remain at least5%. Confirm exact captured forms survive wild-set preparation and saving. For tea forms, evolve the intended authenticity with the correct item; for Tatsugiri, verify each native Order Up effect rather than only the picture.

## FORM-05

**Disposition: REVISE.** Existing successful native behavior is preserved except for the exact changes specified here.

**A strictly limited cosmetic stylist.** The existing Lilycove Contest Lobby Girl at (4,9), script `LilycoveCity_ContestLobby_EventScript_Girl` (`scripts.inc:458`), has a single ordinary conversation and no reward gate. Preserve that text, then offer optional “Appearance” and the separate “Cosplay partner” service from FORM-09. Use one controlled lock/faceplayer interaction; do not continue menus after an auto-releasing MSGBOX_NPC without reacquiring control.

The exact whitelisted groups are reproduced in the appendix and machine ledger: Furfrou's10 trims; the two Shellos; the two Gastrodon; 20 Scatterbug patterns, 20 Spewpa patterns and 20 Vivillon patterns; four Deerling and four Sawsbuck seasons; the five ordinary Flabébé, five ordinary Floette and five Florges colors; seven Minior meteor colors; 63 ordinary Alcremie combinations; Magearna/Original paint; Zarude/Dada scarf; and the two separate Squawkabilly pairs. There are 17 explicit groups and 175 entries in this snapshot; these counts are descriptive, not future design quotas.

Current source comparison confirms equal base stats, types, ability slots, weight, gender rules, egg groups, native level/TM learnset pointers and equivalent evolution conditions within each group. Evolution targets may differ only by the corresponding cosmetic child form. Current expanded preparation access remains intact: if final producer review finds differing current tutor additions within a purely cosmetic group, preserve their union rather than silently removing moves. Pinned learnsets do not override authored expanded access.

The service must require both source and destination to belong to the **same** explicit group. It cannot turn one whitelisted family into another. Reject Eggs, bad Eggs, unowned/cancelled selections and all unlisted targets. Do not cross evolution stages or convert regional forms, genders, tea authenticity, Tatsugiri styles, Pumpkaboo sizes, rare mouse/segment forms, Floette Eternal, legendary power forms, Megas, fusions or inactive gimmicks.

Change only the exact species/form ID and native form-duration metadata where applicable. Preserve PID, shiny status, gender, OT, nickname, level/EXP, Nature, Points, ability slot, held item, friendship, current HP/status, moves, PP and PP Ups. Recalculate stats with the native helper and verify the equal-property conversion has not healed or altered combat stats. Furfrou retains its native five-day/withdrawal return behavior; initialize its native timer with `TrySetDayLimitToFormChange` ([src/pokemon.c:6144](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/pokemon.c:6144>)) and explain that a fresh trim is free. Minior styling changes meteor color only; the matching core appears through Shields Down in battle.

Magearna and Zarude styling requires already owning that same legendary/mythical Pokémon. It supplies no Pokémon, item, caught flag or power form. Unown's 28 appearances already derive from personality through native sprite/render paths ([src/pokemon.c:1831](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/pokemon.c:1831>)); do not rewrite PID to offer a letter menu or mislabel the 27 rendering entries as missing species.

**Acceptance:** one success and one rejected cross-group case for every distinct group mechanism; preserve identity/HP/moves/items exactly. Verify Furfrou timing and Minior's matching core. These checks enforce the specific cosmetic-service promise; they must be updated if a future intentional design changes a group's battle properties.

## FORM-06

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**Independent Galarian bird discoveries.** Append these IDs immediately before LEGENDARY_SIGN_COUNT; do not renumber existing entries.

| Numeric ID | Symbol | Species | Existing landmark map | Badge/flag gate | Offset |
|---:|---|---|---|---|---:|
| 82 | LEGENDARY_SIGN_ARTICUNO_GALAR | ARTICUNO_GALAR | ROUTE120 | 6 / FLAG_BADGE06_GET | +2 |
| 83 | LEGENDARY_SIGN_ZAPDOS_GALAR | ZAPDOS_GALAR | ROUTE112 | 3 / FLAG_BADGE03_GET | +2 |
| 84 | LEGENDARY_SIGN_MOLTRES_GALAR | MOLTRES_GALAR | MT_PYRE_EXTERIOR | 6 / FLAG_BADGE06_GET | +2 |

Use LANDMARK_SIGN definitions with required species NONE and the existing deliberate local landmark menu. No new physical bird object, mandatory Devon research task or permanent failed-encounter flag is required. The Psychic bird belongs among Route120's mysterious highlands, the Fighting runner on Route112's rough foothills, and the Dark bird beside Mt. Pyre. Keep original Articuno, Zapdos and Moltres acquisitions independently available.

This needs an exact identity repair. `GetLegendarySignIdBySpecies` currently strips forms before lookup ([src/legendary_signs.c:303](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/legendary_signs.c:303>)), and `MarkLegendarySignCaughtBySpecies` strips them again (`:575`). First sanitize and match the exact species against the registry. Only if no exact registered encounter exists may an approved interchangeable battle/persistent form fall back to its base Sign. Marking capture must preserve the incoming exact form until that lookup. A Galarian bird must never mark the original bird's caught state or vice versa.

The appended IDs use unlocked/caught word 5 bits 2/3/4; count 85 remains within the existing96-bit capacity. No relic-pending bits are added. Preserve prior caught/unlocked states and previously earned Arceus rewards. New saves' mastery checks can include the newly appended Signs through the existing count-based loop; do not revoke an already received reward on an older save.

Devon's optional guide must show three distinct names/locations/statuses. Add an exact-species display-name helper returning Galarian Articuno, Galarian Zapdos and Galarian Moltres, and use it in both menu entries and lead text; the ordinary GetSpeciesName strings alone are identical to the original birds. Preserve dynamic allocation by actual display length and verify the native menu's text width. Original Moltres is a native encounter whose guide currently relies on the shared National Dex caught bit; use its actual FLAG_EC_CAUGHT_MOLTRES to distinguish it from Galarian Moltres. Keep resting/revisit retry and permanent successful capture behavior. Do not infer regional-form capture from a species-wide Pokédex bit during migration.

**Acceptance:** capture original then regional, and regional then original, for each bird; each combination remains independently obtainable and correctly marked after reload. Old saves with original birds caught start the new regional bits unset. Failed encounters rest only for the visit; revisiting retries. Verify IDs 82–84 at the actual word/bit boundary and guide/mastery behavior without adding a historical fixed-count gate.

## FORM-07

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**Enough nonbreedable parents for both irreversible branches.** A single Cosmog cannot produce both Solgaleo and Lunala, and a single Kubfu cannot produce both Urshifu styles. A graph that marks both branches reachable independently misses the one-save resource constraint.

Offer one supplemental Cosmog and one supplemental Kubfu through a shared Birch/Aide ResearchPartners menu after their respective original Sign is caught. Each uses its own successful-receipt flag, not another Sign or a reset of the original caught bit. The generic prepared-gift path must not call the ordinary already-caught Sign rejector. Cosmog's original source is already postgame; Kubfu's supplemental partner is available after the original badge-three acquisition.

Birch can leave the lab during his daily routine. The existing Lab Aide must call the **same** entitlement handler, with the same receipt flags. Preserve the opening volume's priorities: legacy state5 second-starter completion, then state2 early supplies, then optional ResearchPartners, then ordinary dialogue. Do not add Birch visibility flags, change his daily schedule or make the player wait for a particular day.

| Species | Flag | Level | Item | Ability | Nature | Points | Moves |
|---|---|---:|---|---|---|---|---|
| KUBFU | FLAG_EC_GIFT_SECOND_KUBFU | min(cap,25) | EVIOLITE | INNER_FOCUS | JOLLY | 2/32/0/0/0/32 | CLOSE_COMBAT / ICE_PUNCH / U_TURN / PROTECT |
| COSMOG | FLAG_EC_GIFT_SECOND_COSMOG | min(cap,25) | EVIOLITE | UNAWARE | ADAMANT | 32/32/2/0/0/0 | DOUBLE_EDGE / TELEPORT / MIMIC / NONE |

Kubfu's build exists in the current default catalog (`emerald_champions_battle_sets.h:10684`). Cosmog retains the current catalog's expanded usable Double-Edge/Teleport/Mimic access while omitting empty Splash and investing for a durable slow role. This is not a reason to remove its expanded moves from the game.

Explain that Dark/Water Scrolls produce the two Urshifu styles, and that Cosmoem evolves by day into Solgaleo or by night into Lunala. The two source copies permit both choices in one save. PREP-02 handles ready ordinary evolution at the cap, including 100, without forcing a level above the cap. Do not add a generic legendary conversion/devolution menu.

**Acceptance:** each original caught prerequisite is required; each successful supplemental delivery occurs once from either NPC; full party/PC leaves the entitlement available. Demonstrate both Urshifu styles and both celestial branches in one save, then their respective fusion choices. Preserve original Sign and reward state throughout.

## FORM-08

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**Keep native form and fusion rules, with working tools and clear requirements.** The final routes are:

| Family | Required pieces and route |
|---|---|
| Kyurem White / Black | Owned Kyurem + Reshiram or Zekrom + DNA Splicers; unfuse before selecting the other branch |
| Necrozma Dusk Mane / Dawn Wings | Owned Necrozma + Solgaleo or Lunala + N-Solarizer or N-Lunarizer |
| Calyrex Ice / Shadow | Owned Calyrex + Glastrier or Spectrier + Reins of Unity |
| Zygarde10% / 50%, Aura Break / Power Construct | Owned Zygarde + Birch's Cube; use the actual four-way native transitions |
| Zygarde Complete | A Power Construct form reaches the native at-most-half-HP end-turn condition while its ability functions |
| Arceus types | Owned Arceus + the appropriate earned plate; preserve current relic grant/retry state |
| Silvally types | Evolve owned Type: Null with friendship and use the existing free Species-category Memories |
| Crowned Zacian / Zamazenta | Owned Pokémon + its earned Rusted Sword/Shield; native battle-form handling |
| Ogerpon masks | Owned Ogerpon + its earned mask; preserve the ordinary four forms, not banned Tera forms |
| Castform / Cherrim / Darmanitan / Wishiwashi / Aegislash / Minior / Mimikyu / Eiscue / Cramorant / Palafin / Morpeko | Their native ability, weather, HP, move or switching state; no direct gift of a transient battle form |
| Keldeo Resolute | Learn Secret Sword; existing preparation explicitly runs move-driven form normalization |
| Meloetta Pirouette | Relic Song's native battle transition; retain its normal return behavior |
| Shaymin Sky / Hoopa Unbound | FORM-02 tools with native day/status, duration and return conditions |
| Terapagos Terastal | Native Tera Shift on entering battle; no selectable Terastallization is required |

The fusion definitions are in [src/data/pokemon/form_change_tables.h:1189](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/data/pokemon/form_change_tables.h:1189>), `:1658` and `:2042`; party interaction begins in [src/party_menu.c:6472](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/party_menu.c:6472>). A fusion needs both actual owned components, not just their Dex flags. Native unfusion requires a free party slot (`party_menu.c:6847`); communicate this before attempting it and preserve the component when space is unavailable. Sequential use needs no storage-layout expansion.

Phione specifically requires breeding an owned Manaphy with Ditto at the Day Care, then hatching the resulting Phione egg; its Sign entry is bookkeeping, not a standalone gift. Circuit, Game Corner and mastery entries similarly require their actual delivery pathways documented in the Mega/legendary reward volume. An OTHER_SIGN definition alone does not place a Pokémon in the world.

The free Move Tutor remains the convenient route to condition moves such as Ancient Power, Mimic, Psyshield Bash, Rage Fist, Hyper Drill, Twin Beam, Dragon Cheer and Secret Sword. Preserve the game's existing expanded preparation access. The current source/pinned comparison is useful for understanding differences, not a mandate to delete hundreds of authored move slots or narrow creativity.

**FORM-08-UNFUSE-MOVES:** preserve the current expanded move-access contract during unfusion. [src/party_menu.c:6623](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/party_menu.c:6623>) currently filters only native level/TM/egg lists, which can disagree with intentionally expanded preparation access; it also deletes from a shifting move array during a forward scan. Build the proposed resulting four slots from an immutable copy, using the same effective current move-access policy as preparation. Preserve every move allowed by that policy. Preview and confirm only genuinely form-specific/unsupported changes before committing; do not erase a move merely because a pinned reference omits it. Apply the resulting list once, preserving unrelated PP/PP Ups and mandatory field capability. Keep native explicit fusion signature swaps when applicable. Cancellation preserves both Pokémon and their moves. A focused case with two adjacent candidates for removal must not skip one through slot shifting.

**Acceptance:** actual fusion/unfusion preserves both identities, held items and storage state through cancellation/reload/full-party failure; no component is cloned or lost. Verify form changes use the selected ability/item and restore correctly. Native inactive gimmicks remain unavailable; do not count an enum-table entry as a functional player mode.

## FORM-09

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**Functional research partners and a separate Cosplay mechanic.** After game clear, the shared Birch/Aide ResearchPartners menu offers three distinct non-evolving/ability variants. Give each once with successful-receipt state. These are existing configured battle options, not ordinary cosmetics. Numeric receipt-flag assignments and one-time v4 initialization are owned by the [state allocation specification](../appendices/state-allocation.md); do not allocate competing bits here.

| Species | Flag | Level | Item | Ability | Nature | Points | Moves |
|---|---|---:|---|---|---|---|---|
| GRENINJA_BATTLE_BOND | FLAG_EC_GIFT_BATTLE_BOND_GRENINJA | min(cap,25) | LIFE_ORB | BATTLE_BOND | TIMID | 2/0/0/32/0/32 | HYDRO_PUMP / DARK_PULSE / WATER_SHURIKEN / PROTECT |
| PIKACHU_STARTER | FLAG_EC_GIFT_PARTNER_PIKACHU | min(cap,25) | LIGHT_BALL | LIGHTNING_ROD | TIMID | 2/0/0/32/0/32 | FAKE_OUT / THUNDERBOLT / GRASS_KNOT / PROTECT |
| EEVEE_STARTER | FLAG_EC_GIFT_PARTNER_EEVEE | min(cap,25) | SITRUS_BERRY | ADAPTABILITY | ADAMANT | 32/32/2/0/0/0 | DOUBLE_EDGE / YAWN / HELPING_HAND / PROTECT |

Battle Bond Greninja has an existing valid default preset (`emerald_champions_battle_sets.h:11804`). The configured Gen9+ Battle Bond boosts Attack, Special Attack and Speed once rather than becoming Ash-Greninja ([include/config/battle.h:223](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/include/config/battle.h:223>), [src/battle_util.c:4600](</Users/gguthrie/Desktop/Pokémon Emerald Champions/Cohesion Book/baseline/source/src/battle_util.c:4600>)); mark Ash form as inactive legacy data. The enhanced Pikachu/Eevee have distinct higher base stats and cannot evolve. Explain that tradeoff. Do not reuse ordinary Eevee's Eviolite support preset for its non-evolving partner variant. Light Ball checks the Pikachu base family (`battle_util.c:7104`) and applies to the enhanced partner.

At the Contest Lobby Girl, offer one prepared female PIKACHU_COSPLAY with FLAG_EC_GIFT_COSPLAY_PIKACHU: level min(cap,25), Light Ball, Lightning Rod, Timid,2/0/0/32/0/32, Thunderbolt/Grass Knot/Knock Off/Protect. This is the current Cosplay default (`emerald_champions_battle_sets.h:11154`). No contest victory or mandatory contest participation gate is needed.

Her distinct “Costume lessons” submenu accepts only an owned member of this exact six-form family:

| Costume species | Costume signature move |
|---|---|
| PIKACHU_COSPLAY | None |
| PIKACHU_ROCK_STAR | METEOR_MASH |
| PIKACHU_BELLE | ICICLE_CRASH |
| PIKACHU_POP_STAR | DRAINING_KISS |
| PIKACHU_PHD | ELECTRIC_TERRAIN |
| PIKACHU_LIBRE | FLYING_PRESS |

These signature additions are explicitly recorded in `data/emerald_champions/emerald_champions_preparation_form_learnsets.json`. They make costume changes mechanically meaningful, so do not send them through FORM-05's cosmetic-only action. Never convert ordinary, enhanced-partner or cap Pikachu into this family.

Preview the requested costume and exact move exchange. If the outgoing costume's signature is present, offer that slot as the replacement; otherwise use the existing move-learning slot-selection interface. Confirm before replacing anything. Cancel leaves species and moves unchanged. Returning to plain Cosplay explicitly confirms removal of the outgoing signature, if present; it may leave three moves. Never erase unrelated moves, silently remove a necessary field capability, reset the rest of the build, or scan/delete every unusual move merely because it is absent from a pinned reference. Preserve all other identity, stats, ability/item and move/PP state. Only the confirmed new move receives its normal learning behavior.

Pikachu caps and Spiky-eared Pichu remain explicitly excluded legacy promotional appearances: they add no distinct positive competitive stat/ability capability, while their gender/breeding/evolution restrictions make unrestricted styling misleading. This is not an exclusion of Battle Bond, enhanced partners or Cosplay signatures.

**Acceptance:** each seven-gift entitlement across FORM-03/07/09 is independent and successful-receipt-only. Confirm valid abilities and complete ready builds; new gifts remain their exact variant through preparation/storage. Cosplay cancellation and failed slot choice preserve the entire Pokémon; each of five signature exchanges has a positive/negative case and no unrelated move loss. Verify modern Battle Bond as a stat boost, not an Ash transformation.

## FORM-10

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**Postgame Deoxys research at Cozmo's house.** Deoxys's four native forms cycle through Meteorite use (`form_change_tables.h:780`). Meteorite is also a story handoff item, so it must not appear in the free archive or be reissued early just to supply form access.

Append an optional Deoxys research continuation to Professor Cozmo after his existing applicable dialogue/handoff branch, gated by FLAG_SYS_GAME_CLEAR. Preserve Dawn Stone receipt, Meteorite handoff, Bag failure and refusal behavior. The lab can lend its research equipment for an owned Deoxys; no Meteorite is granted, consumed or removed by this service.

Offer Normal, Attack, Defense, Speed and Cancel for an owned non-Egg Deoxys. Use exactly those four approved target species and the native form/stat/move-change semantics, preserving PID/shiny/OT/level/Points/Nature/item and unrelated moves. Do not manipulate the story item to simulate a menu click. A fainted owned Deoxys may be studied but remains at zero HP; this service does not heal. The same explicit form-update primitive can be reused by the bounded cosmetic service only with its separate whitelist and stricter equal-property rules.

**Acceptance:** all four forms can be selected repeatedly after game clear, before or after the old Meteorite handoff, with no duplicate Dawn Stone or changed quest flags. Cancellation, fainted selection policy and stat/HP recalculation follow the stated native rules; no early game access is introduced.

## FORM-11

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**Evolution requirements remain real, visible and practical.** Preserve these current conveniences:

- Meltan: ready level evolution in the New Mauville map section.
- Ursaluna: Peat Block at night; Bloodmoon is a separate direct encounter from the global wild plan.
- Kingambit: ready level evolution while holding Leader's Crest, already in the free archive.
- Gholdengo: level 45; do not restore a999-coin grind.
- Palafin: Finizen level 38; no link-session requirement.
- Alcremie: Milcery level 30; use FORM-05 for ordinary cosmetic combinations rather than inventing spinning/rare-sweet grind.
- Trade evolutions: preserve existing local Linking Cord/item/party-partner alternatives. A native trade row alone is not proof of solo access.
- Babies: preserve current no-incense breeding and short hatch-cycle behavior. Regional offspring may need an Everstone on the relevant regional parent; native overrides make tea offspring ordinary and Scatterbug offspring Fancy.

The current Hoenn regional-evolution locations are deliberate: Quilava/Rufflet in Mt. Pyre; Dewott in Shoal Cave; Dartrix in Petalburg Woods; Petilil in Verdanturf Meadow; Goomy in Meteor Falls1F1R; Bergmite in Shoal's ice room; Koffing in Fiery Path. The exact levels/conditions remain those printed in the appendix. Alolan Raichu/Exeggutor/Marowak and Galarian Mr. Mime already have direct Hoenn roots, so do not pretend unavailable Alola/Galar region predicates are usable local evolution methods.

Runerigus uses the live Route111 carved-stone trigger with a conscious Galarian Yamask missing at least 49 HP and no Everstone. Sirfetch'd uses three critical hits in one qualifying battle. Wyrdeer/Annihilape use their actual 20-use counters; Basculegion uses 294 accumulated recoil and gender; Pawmo/Bramblin/Rellor need the first live party slot and 1,000 follower steps. Publish these exact requirements through appropriate evolution guidance. Do not claim that merely possessing the family completes its evolution chain.

**FORM-11-NATURE:** make Toxel's form selection follow the Nature chosen in preparation. The free editor writes MON_DATA_HIDDEN_NATURE (`field_specials.c:5551`), but evolution currently reads `GetNature(mon)`, which is PID modulo25 (`pokemon.c:4068`,`:4101`). Use MON_DATA_HIDDEN_NATURE as the input to the evolution nature predicates. Preserve PID, shiny status, gender and all other evolution conditions. This makes Amped/Low Key an understandable preparation choice without breeding another Toxel.

**PREP-06 dependency:** optional Bonding preparation is available through the shared Center Specialist from Oldale and through the existing Verdanturf Friendship Rater. It offers 0, the configured evolution threshold, or 255 friendship with preview/confirmation; preserve PID and do not auto-evolve or alter battle-affection configuration. This keeps both evolution readiness and retained Return/Frustration roles convenient without menu-grind teaching loops. The lead editor owns the one shared implementation.

**PREP-02 / PREP-02-B dependencies:** the Leveler offers already-ready ordinary evolutions at the current cap, including 100, without increasing level. Tandemaus remains battle-only: after a legitimate won/caught ordinary battle, the existing end-of-battle evolution callback evaluates ready EVO_LEVEL_BATTLE_ONLY entries without requiring a level gain. Preserve level 25, PID branch, Everstone, consciousness and cancellation. Do not evolve an underleveled Pokémon permanently from temporary Circuit normalization, or add duplicate prompts if a current callback already handles the case. The lead editor owns that exact callback repair.

Retain PID surprise evolution rules for Wurmple, Dudunsparce and Maushold. The two rarer forms receive practical direct discoveries in FORM-04; the stylist never rewrites PID or treats a weight difference as irrelevant. Gender-dependent evolution remains meaningful; the existing broad wild roster and breeding routes supply the necessary genders without a generic gender-changing service.

**Acceptance:** confirm one real example for each distinct requirement class, with negative controls that actually violate the condition. Check equality/threshold boundaries using native integer Points/Nature stats, actual time/map/weather and persistent counters. Tests should validate these intended behaviors and be revised or retired when their contract becomes obsolete; do not freeze historical encounter counts, level-stage aesthetics or strategy quotas.

## FORM-12

**Disposition: REPAIR.** Existing successful native behavior is preserved except for the exact changes specified here.

**Exact Mega bases, not family-wide assumptions.** The Mega/reward volume owns the99 stones and their locations. The configured source has 103 item-triggered Mega forms plus move-triggered Rayquaza. Form access requires the actual eligible base, its stone/move and the Mega Ring under native rules.

Important exact prerequisites:

- Floettite: Eternal Flower Floette only, already obtainable in Verdanturf Meadow. Ordinary colors are not substitutes. Repair Colton's final team to use FLOETTE_ETERNAL while preserving its other fields.
- Zygardite: Complete Zygarde only. The Cube supplies a Power Construct form; the ability must be active at its half-HP trigger, then a subsequent eligible turn can Mega Evolve. Repair Quincy's Zygarde to ZYGARDE_50_POWER_CONSTRUCT/POWER_CONSTRUCT; Neutralizing Gas must no longer suppress that ability when transformation is needed.
- Raichunite X/Y: ordinary Kantonian Raichu; Slowbronite: ordinary Slowbro; Greninjite: ordinary Greninja, not the separate Battle Bond species.
- Magearnite: both ordinary/Original Magearna; Meowsticite: both sexes; Tatsugirinite: all three native Tatsugiri forms.
- Dragon Ascent enables Rayquaza's stone-free Mega eligibility, which still requires the Ring and shares its trainer's actual activation budget.

The Mega chapter owns item-description clarity under MEGA-FORM-LABELS; do not duplicate those edits here. The shared AI chapter owns real base/form evaluation and owner-scoped Mega decisions. Do not downgrade Steven's Dragon Ascent to satisfy a stone-count assumption: both Rayquaza and Metagross can be eligible while only one actually activates.

**Acceptance:** for each distinct eligibility family, demonstrate an eligible base and a closely related ineligible form. Do not apply a universal “same National Dex” rule or count the stone alone as proof of the transformation. Reconcile the six final late-battle loadout revisions with the battle volume, including Quincy and Colton.

## Implementation and evidence boundary

Implement the root wild plan and these eight substitutions together; add exact Sign identity before the Galarian rows; extend the existing archive; add the shared receipt-safe gifts and bounded services; then integrate preparation readiness and exact Mega eligibility. The appendix accounts for every configured entry and records conditions rather than claiming all potential edges have been played.

Validate the real new routes and transitions, including old saves, full Bag/party/PC, cancellation, counterpart ownership, fusion recovery and failed encounter construction. Preserve the successful authored game while correcting concrete gaps. Pinned-source comparisons, existing tests and verifiers are code/evidence to audit; they are not a mandate to remove creative moves, impose old counts or rewrite working teams.
