# Legendary pipeline source audit — September 10, 2026

Authority: current executable C, included event scripts, map events, graphics registrations and generated encounter tables. No design documents were used as proof of behavior.

## Source coverage

- All 85 acquisition definitions: 39 repeatable native residents, 15 gated wild discoveries, seven ordinary Ultra Beast table residents, seven visible/scripted discoveries, and 17 reward/breeding definitions.
- Sixteen additional original encounter roots: Moltres, Jirachi, Diancie, Heatran, the three original Regis, Groudon, Kyogre, Rayquaza, Latias, Latios, Lugia, Ho-Oh, Mew and Deoxys. Evolution/form continuations were traced through breeding, item/form tables and fusion storage.
- All 31 included map-script files that invoke the legendary encounter or local-discovery owners, including seven imported Kanto scenes. Kanto scenes retain their imported rules; Hoenn maps have no warp/connection/script-warp links to those imported maps. The active Hoenn scenes use the current capture/retry pipeline.
- 120 source dialogue blocks selected by legendary names/terms, including unrelated lore/trainer uses, plus the C-generated requirement pages and all 47 Center leads.
- Main owners inspected: legendary_signs.c; wild_encounter.c; roamer.c; battle_setup.c; capture delivery in battle_script_commands.c, battle_main.c, pokemon.c, item_use.c and script_pokemon_util.c; champions_circuit.c rewards; daycare.c and egg_hatch.c; field_specials.c; event_data.c and script.c; NPC graphics registration and object removal/respawn; faraway_island.c; braille_puzzles.c; Deoxys field_effect.c; party_menu.c fusion/unfusion transactions and animations; legendary form/evolution data and field weather/music handoffs.

## Fixes

1. Critical: all 20 new roster signs referenced graphics registered only under IS_FRLG. Native Articuno-room rendering reproduced a null graphics pointer followed by a reset. Moved the existing sign picture, frame table, graphics definition, registration and white NPC palette into the shared build. No fallback masking of missing graphics was added.
2. Center guide: caught Galarian Moltres could complete the original Moltres lead through their shared national Dex entry. Original Moltres now uses its actual Ember Path capture flag.
3. Regigigas: moved its cry after eligibility checks; failed requirements now produce the live requirement text. The shared C discovery check preserves the existing requirement that all three Regis be present on retries, matching the statue and Center guide.
4. Southern Island: replaced hardcoded level-100 Latias/Latios creation with the shared live-cap static encounter creator, retaining Soul Dew and the fateful encounter marker.
5. Deoxys: skip palette restoration when its destroyed/hidden triangle has no registered palette; previously index 255 could write outside the palette buffers after returning to the field.
6. Removed six stale dialogue references to Devon's deleted global guide. Corrected Type: Null and Ogerpon NPC dialogue that still claimed badge requirements.
7. Route rosters identify Galarian Articuno/Zapdos/Moltres explicitly.

## Evidence

- Compiled native regressions pass for scene prerequisites and Lati generation, Center guide progression/regional distinction, and 3% wild / shared 25% Sweet Scent selection before and after capture.
- Existing Circuit prefix: 14 native checks pass, including delivery, full-PC retries and mastery ordering. Existing pending-relic full-storage/replay regression passes.
- Source census: all 54 overlay wild definitions have their required land/water encounter headers; none duplicates its species in its ordinary local table.
- Emulator: eight custom legendary overworld fixtures render and satisfy their native sprite checks after the sign fix. Twenty sign-area map fixtures pass map-load/render checks. Their temporary ROM changes only fixture camera data; it is not the delivered game. Screenshots were inspected as contact sheets. These checks do not establish that every interaction, camera path or capture outcome was played.
- Production release builds; release gates and source/artifact stamp pass; production ROM boots in native mGBA. See the adjacent logs and screenshots.

## Limits

This is a source audit with focused compiled and emulator evidence, not a complete fresh-save playthrough or runtime certification of every legendary animation. The Deoxys palette correction is source-verified; no full Deoxys puzzle/capture replay was claimed. Imported Kanto content is not part of the reachable Hoenn campaign and was not runtime-certified.

Delivered ROM: pokeemerald-legendary-audit-20260910-release.gba. It also contains the earlier approved professor gift, native encounters, quest preservation, menu removal, Center guides and single-clerk Marts.
