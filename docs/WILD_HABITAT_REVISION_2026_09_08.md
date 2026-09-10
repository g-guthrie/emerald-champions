# September 8 habitat and opening-choice revision

Earlier checkpoint: the subsequent [approved area revision](WILD_AREA_REVISION_2026_09_08.md) supersedes the current-placement claims below. This document records the preceding build and its evidence.

The user approved stronger habitat identity and discovery pacing, while
preserving powerful early catches and a demanding opening. They then explicitly
authorized changing early choices to provide useful Fighting options, including
Timburr. This revision changes encounter species, not species stats, trainer
teams, level caps, encounter levels, encounter frequency or method access.

## Opening choices

Percentages below are ordinary slot chances. Sweet Scent and Lures can reverse
slot selection; every ordinary species still meets the 5% minimum.

| Area | Current change | Immediate use with existing free preparation |
| --- | --- | --- |
| Route101 | Mienfoo replaces Pidgey at 10% | Regenerator, Fake Out, Fighting offense; Eviolite, Drain Punch and Taunt are available. |
| Route102 | Timburr replaces Bidoof at 11% | Guts/Flame Orb with Drain Punch and Mach Punch, or Eviolite with Bulk Up and recovery. |
| Route102 | Pachirisu replaces Kricketot at 8% | Follow Me, Nuzzle, Super Fang and Protect; Helping Hand is also available. |
| Route103 | Bidoof replaces Patrat at 13% | Keeps the Simple setup option in the opening. |
| Route104 | Pidgey replaces Mienfoo at 11% | Mienfoo is now earlier; Pidgey stays before Roxanne. |
| Petalburg Woods | Kricketot replaces Wurmple at 11% | Wurmple remains on Route101 at 11%, with Caterpie still in the woods at 10%. |
| Route123 | Patrat replaces Tropius at 8% | Preserves the displaced family; Tropius remains on Routes119/120. |

The seven core opening land maps now contain 84 direct species identifiers,
versus 83 before the revision. Pachirisu and Timburr are newly present in those
maps; Patrat moves later. Mienfoo was already before Roxanne, but now arrives
before the Route103 rival. Route102 opens after that rival and before Calvin
and the other first-route trainers.

Oldale's Center already supplies preparation and the Leveler before the rival.
The compiled Mienfoo, Timburr and Pachirisu presets and All Legal Moves supply
the actions described above without a move-level restriction. Timburr itself
is the intended opening option: Gurdurr evolves at 25, beyond the pre-Roxanne
cap of 14. The free evolution-item archive still requires the Mega Ring after
Brawly; early Growlithe/Eevee access is not automatic access to stone evolutions.

Riolu and Throh on Route116, Machop in Rusturf, the babies, and existing powerful
forest/meadow discoveries remain. Makuhita retains its Dewford/Granite Cave
identity rather than duplicating the two new earliest Fighting roles.

## Habitat changes

- Artisan Cave: Smeargle is 56% on 1F and 54% in B1F. The floors contain eight
  and five distinct residents respectively, instead of identical twelve-species
  lists. The former birds and beetles remain elsewhere in Hoenn.
- Diancie's room: Carbink is 57%, accompanied by Sableye, Mawile and Glimmora.
  Mirage Island's grass is a Wynaut colony (43%) with birds and forest visitors.
- Mt. Pyre: 1F emphasizes Shuppet/Duskull, 3F candles, 4F evolved ghosts and 6F
  final forms. Antique Sinistea on 2F and Artisan Poltchageist on 5F are untouched.
- Mirage Tower's top and Sandstrewn B1F gain recurring relic residents. Desert
  entrance rosters, Great Tusk, Unown, Stakataka, Iron Treads, Gimmighoul Chest,
  and the existing Darmanitan/Krookodile slots remain. B1F precedes Sandstrewn's
  numbered upper floors in the actual warp route.
- Seafloor Cavern: selected rooms emphasize Dragalge/Golisopod, a Dhelmise/relic
  chamber, Kingdra, and the deeper marine roster. Clobbopus remains in Room8.
- Water: ship interiors favor wreck residents; eastern reefs, cold currents,
  open whale routes and deep shell beds have explicit populations. Meteor Falls
  gains ready dragon partners in its deep pool. Victory Road's stronger rod
  offers Crawdaunt and Dragonair. Petalburg's Old Rod now has Marill at 40%,
  alongside Magikarp, rather than two Magikarp slots.

Dhelmise occurs on eight ordinary encounter maps, down from 24: both flooded
Abandoned Ship maps, Route108, Route122, Slateport, Seafloor Room3, Underwater
Route124 and the submerged submarine chamber. It becomes the leading Super Rod
catch in the ship's flooded corridors. Inland replacements include Whiscash,
Ludicolo, Jellicent, Azumarill and Relicanth, according to habitat.

All three Dive rosters, Dondozo/Tatsugiri, Pheromosa in Dewford Meadow, Kartana
in the deeper forest, the early babies, Dewford Old Rod Mantyke and New Mauville
are unchanged. This pass preserves every previously present ordinary Hoenn
direct species/form identifier: 640 before, 641 now (Ludicolo newly appears
directly rather than only through evolution). These are not evolution-family
counts or proof that every physical acquisition route has been traversed.

## Authoring and regeneration

Land and Rock Smash remain authored in `src/data/wild_encounters.json`.
`data/emerald_champions/wild_route_sheet.json` now contains exact Surf, Old Rod,
Good Rod and Super Rod species in native slot order for all 58 registered Hoenn
water maps. The generator copies those species and preserves native levels and
encounter rates. Its historical generic land filler, forced deduplication,
evolution promotion and rotating water replacement rules were removed.

The migration alone produced zero native encounter changes before the intended
edits. Three non-Hoenn Safari authoring rows were removed; their native tables
were not changed. The existing distribution check now compares exact authored
water species with the compiled-source tables, rather than reapplying the old
rotation algorithm. No new permanent tests or design quotas were added.

## Build and evidence

Local ROM: `pokeemerald-habitats-20260908-release.gba`.
SHA-256: `55a1471823240783ddd36e1c97afdb2571007da88ad95932ba33b1f75ef9296b`.
The adjacent ELF and input stamp bind 28,977 inputs. The earlier repair ROM and
published 5.1 ROM are preserved. Nothing was committed, pushed or uploaded.

Verified:

- Named production build and all existing release checks passed, including
  authored trainer/preset materialization, wild tables, rewards, input stamp,
  ROM/ELF correspondence, header/checksum and memory bounds.
- All 264 active encounter tables pass the existing slot, level and probability
  check, including normal and reversed selection. Regeneration is idempotent.
- The existing host C Dive/land selector regression passed all 100 rolls in both
  map modes and both selection orders. This exercises the extracted production
  selector, not an emulator encounter.
- A one-off source comparison verified 121 species-slot changes in 50 method
  tables across 44 maps. Levels, encounter rates, other fields, inactive groups,
  and the protected rosters listed above are unchanged.
- Every changed method table was read back from the production ROM using its
  linked symbol address and compared with the native min/max/species entries.
  All 50 matched. Route signs already deduplicate repeated residents.

The source review establishes preparation availability and intended roles.
No new capture traversal or native battle trials were run, so first-route and
Roxanne win rates remain unmeasured. Historical Cohesion Book rosters and the
pre-revision wild audit are not current authoring inputs.
