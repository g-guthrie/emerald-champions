# EverGrandeCity_PokemonLeague_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../07-league.md) · [Map source](../../baseline/source/data/maps/EverGrandeCity_PokemonLeague_1F/map.json) · [Scripts](../../baseline/source/data/maps/EverGrandeCity_PokemonLeague_1F/scripts.inc)

## Current map contract

`MAP_EVER_GRANDE_CITY_POKEMON_LEAGUE_1F` · `LAYOUT_EVER_GRANDE_CITY_POKEMON_LEAGUE_1F` · `WEATHER_NONE` · `MUS_POKE_CENTER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `EverGrandeCity_PokemonLeague_1F:object_events:001` | 3,2 | [EverGrandeCity_PokemonLeague_1F_EventScript_Nurse](../../baseline/source/data/maps/EverGrandeCity_PokemonLeague_1F/scripts.inc#L17) — EverGrandeCity_PokemonLeague_1F_EventScript_Nurse at (3,2); shared behavior CENTER | **REPAIR** · [W-C-CENTER](../common-contracts.md#w-c-center) · W-CENTER-FIRST-VISIT |
| `EverGrandeCity_PokemonLeague_1F:object_events:002` | 15,2 | [Common_EventScript_EmeraldChampionsBattleVendor](../../baseline/source/data/scripts/emerald_champions.inc#L384) — Common_EventScript_EmeraldChampionsBattleVendor at (15,2); shared behavior VENDOR | **KEEP** · [W-C-VENDOR](../common-contracts.md#w-c-vendor) |
| `EverGrandeCity_PokemonLeague_1F:object_events:003` | 9,2 | [EverGrandeCity_PokemonLeague_1F_EventScript_DoorGuard](../../baseline/source/data/maps/EverGrandeCity_PokemonLeague_1F/scripts.inc#L48) — EverGrandeCity_PokemonLeague_1F_EventScript_DoorGuard at (9,2); Beyond this point, only those TRAINERS /  who have collected all the GYM BADGES /  are permitted to enter. //  TRAINER, let us confirm that you have /  all the GYM BADGES. / You need all eight GYM BADGES. //  If you're bound for the POKéMON /  LEAGUE, you must return with them. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `EverGrandeCity_PokemonLeague_1F:object_events:004` | 10,2 | [EverGrandeCity_PokemonLeague_1F_EventScript_DoorGuard](../../baseline/source/data/maps/EverGrandeCity_PokemonLeague_1F/scripts.inc#L48) — EverGrandeCity_PokemonLeague_1F_EventScript_DoorGuard at (10,2); Beyond this point, only those TRAINERS /  who have collected all the GYM BADGES /  are permitted to enter. //  TRAINER, let us confirm that you have /  all the GYM BADGES. / You need all eight GYM BADGES. //  If you're bound for the POKéMON /  LEAGUE, you must return with them. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `EverGrandeCity_PokemonLeague_1F:object_events:005` | 17,2 | [Common_EventScript_EmeraldChampionsMoveTutor](../../baseline/source/data/scripts/emerald_champions.inc#L1) — Common_EventScript_EmeraldChampionsMoveTutor at (17,2); shared behavior TUTOR | **KEEP** · [W-C-TUTOR](../common-contracts.md#w-c-tutor) |
| `EverGrandeCity_PokemonLeague_1F:warp_events:001` | 9,11 | Warp from (9,11, elevation 3) to MAP_EVER_GRANDE_CITY warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_PokemonLeague_1F:warp_events:002` | 10,11 | Warp from (10,11, elevation 3) to MAP_EVER_GRANDE_CITY warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_PokemonLeague_1F:warp_events:003` | 9,1 | Warp from (9,1, elevation 3) to MAP_EVER_GRANDE_CITY_HALL5 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_PokemonLeague_1F:warp_events:004` | 10,1 | Warp from (10,1, elevation 3) to MAP_EVER_GRANDE_CITY_HALL5 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_PokemonLeague_1F:warp_events:005` | 1,7 | Warp from (1,7, elevation 4) to MAP_EVER_GRANDE_CITY_POKEMON_LEAGUE_2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_PokemonLeague_1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [EverGrandeCity_PokemonLeague_1F_OnTransition](../../baseline/source/data/maps/EverGrandeCity_PokemonLeague_1F/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls EverGrandeCity_PokemonLeague_1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `EverGrandeCity_PokemonLeague_1F:map_scripts:002` | MAP_SCRIPT_ON_RESUME | [CableClub_OnResume](../../baseline/source/data/scripts/cable_club.inc#L1226) — MAP_SCRIPT_ON_RESUME calls CableClub_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
