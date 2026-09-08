# LilycoveCity_PokemonCenter_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/LilycoveCity_PokemonCenter_1F/map.json) · [Scripts](../../baseline/source/data/maps/LilycoveCity_PokemonCenter_1F/scripts.inc)

## Current map contract

`MAP_LILYCOVE_CITY_POKEMON_CENTER_1F` · `LAYOUT_POKEMON_CENTER_1F` · `WEATHER_NONE` · `MUS_POKE_CENTER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LilycoveCity_PokemonCenter_1F:object_events:001` | 8,2 | [LilycoveCity_PokemonCenter_1F_EventScript_Nurse](../../baseline/source/data/maps/LilycoveCity_PokemonCenter_1F/scripts.inc#L26) — LilycoveCity_PokemonCenter_1F_EventScript_Nurse at (8,2); shared behavior CENTER | **REPAIR** · [W-C-CENTER](../common-contracts.md#w-c-center) · W-CENTER-FIRST-VISIT |
| `LilycoveCity_PokemonCenter_1F:object_events:002` | 12,7 | [LilycoveCity_PokemonCenter_1F_EventScript_Boy](../../baseline/source/data/maps/LilycoveCity_PokemonCenter_1F/scripts.inc#L34) — LilycoveCity_PokemonCenter_1F_EventScript_Boy at (12,7); I wonder how many kinds of POKéMON /  there are in the world. //  It'd be great to cross seas and /  trade POKéMON with people far away. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_PokemonCenter_1F:object_events:003` | 13,8 | [LilycoveCity_PokemonCenter_1F_EventScript_Maniac](../../baseline/source/data/maps/LilycoveCity_PokemonCenter_1F/scripts.inc#L38) — LilycoveCity_PokemonCenter_1F_EventScript_Maniac at (13,8); I've been hearing about some rotten /  scoundrels who steal POKéMON and rip /  off METEORITES. / Those rotten scoundrels who steal /  POKéMON and rip off METEORITES… //  I haven't seen them around recently. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `LilycoveCity_PokemonCenter_1F:object_events:004` | 4,4 | [LilycoveCity_PokemonCenter_1F_EventScript_LilycoveLady](../../baseline/source/data/scripts/lilycove_lady.inc#L1) — LilycoveCity_PokemonCenter_1F_EventScript_LilycoveLady at (4,4); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_PokemonCenter_1F:object_events:005` | 5,4 | [LilycoveCity_PokemonCenter_1F_EventScript_ContestLadyMon](../../baseline/source/data/scripts/lilycove_lady.inc#L462) — LilycoveCity_PokemonCenter_1F_EventScript_ContestLadyMon at (5,4); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_PokemonCenter_1F:object_events:006` | 2,2 | [Common_EventScript_EmeraldChampionsBattleVendor](../../baseline/source/data/scripts/emerald_champions.inc#L384) — Common_EventScript_EmeraldChampionsBattleVendor at (2,2); shared behavior VENDOR | **KEEP** · [W-C-VENDOR](../common-contracts.md#w-c-vendor) |
| `LilycoveCity_PokemonCenter_1F:object_events:007` | 13,2 | [Common_EventScript_EmeraldChampionsMoveTutor](../../baseline/source/data/scripts/emerald_champions.inc#L1) — Common_EventScript_EmeraldChampionsMoveTutor at (13,2); shared behavior TUTOR | **KEEP** · [W-C-TUTOR](../common-contracts.md#w-c-tutor) |
| `LilycoveCity_PokemonCenter_1F:warp_events:001` | 8,8 | Warp from (8,8, elevation 3) to MAP_LILYCOVE_CITY warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_PokemonCenter_1F:warp_events:002` | 7,8 | Warp from (7,8, elevation 3) to MAP_LILYCOVE_CITY warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_PokemonCenter_1F:warp_events:003` | 1,6 | Warp from (1,6, elevation 4) to MAP_LILYCOVE_CITY_POKEMON_CENTER_2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_PokemonCenter_1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [LilycoveCity_PokemonCenter_1F_OnTransition](../../baseline/source/data/maps/LilycoveCity_PokemonCenter_1F/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls LilycoveCity_PokemonCenter_1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `LilycoveCity_PokemonCenter_1F:map_scripts:002` | MAP_SCRIPT_ON_RESUME | [CableClub_OnResume](../../baseline/source/data/scripts/cable_club.inc#L1226) — MAP_SCRIPT_ON_RESUME calls CableClub_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
