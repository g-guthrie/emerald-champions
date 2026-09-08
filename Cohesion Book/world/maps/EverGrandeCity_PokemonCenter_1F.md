# EverGrandeCity_PokemonCenter_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../07-league.md) · [Map source](../../baseline/source/data/maps/EverGrandeCity_PokemonCenter_1F/map.json) · [Scripts](../../baseline/source/data/maps/EverGrandeCity_PokemonCenter_1F/scripts.inc)

## Current map contract

`MAP_EVER_GRANDE_CITY_POKEMON_CENTER_1F` · `LAYOUT_POKEMON_CENTER_1F` · `WEATHER_NONE` · `MUS_POKE_CENTER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `EverGrandeCity_PokemonCenter_1F:object_events:001` | 8,2 | [EverGrandeCity_PokemonCenter_1F_EventScript_Nurse](../../baseline/source/data/maps/EverGrandeCity_PokemonCenter_1F/scripts.inc#L16) — EverGrandeCity_PokemonCenter_1F_EventScript_Nurse at (8,2); shared behavior CENTER | **REPAIR** · [W-C-CENTER](../common-contracts.md#w-c-center) · W-CENTER-FIRST-VISIT |
| `EverGrandeCity_PokemonCenter_1F:object_events:002` | 5,5 | [EverGrandeCity_PokemonCenter_1F_EventScript_Woman](../../baseline/source/data/maps/EverGrandeCity_PokemonCenter_1F/scripts.inc#L24) — EverGrandeCity_PokemonCenter_1F_EventScript_Woman at (5,5); The POKéMON LEAGUE is only a short /  distance after the VICTORY ROAD. //  If you've come this far, what choice /  do you have but to keep going? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `EverGrandeCity_PokemonCenter_1F:object_events:003` | 12,7 | [EverGrandeCity_PokemonCenter_1F_EventScript_ExpertM](../../baseline/source/data/maps/EverGrandeCity_PokemonCenter_1F/scripts.inc#L28) — EverGrandeCity_PokemonCenter_1F_EventScript_ExpertM at (12,7); The long and harrowing VICTORY ROAD… //  It's like reliving the path one has /  traveled in life… //  Believe in your POKéMON and give it /  your very best! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `EverGrandeCity_PokemonCenter_1F:object_events:004` | 9,4 | [EverGrandeCity_PokemonCenter_1F_EventScript_Scott](../../baseline/source/data/maps/EverGrandeCity_PokemonCenter_1F/scripts.inc#L32) — EverGrandeCity_PokemonCenter_1F_EventScript_Scott at (9,4); SCOTT: {PLAYER}{KUN}, you've clawed your /  way up to face the POKéMON LEAGUE! //  I'm happy for you! /  You made my cheering worthwhile! //  {PLAYER}{KUN}, if you were to become /  the POKéMON LEAGUE CHAMPION… //  I'll get in touch with you then. //  Okay, {PLAYER}{KUN}. /  Go for greatness! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `EverGrandeCity_PokemonCenter_1F:object_events:005` | 2,2 | [Common_EventScript_EmeraldChampionsBattleVendor](../../baseline/source/data/scripts/emerald_champions.inc#L384) — Common_EventScript_EmeraldChampionsBattleVendor at (2,2); shared behavior VENDOR | **KEEP** · [W-C-VENDOR](../common-contracts.md#w-c-vendor) |
| `EverGrandeCity_PokemonCenter_1F:object_events:006` | 13,2 | [Common_EventScript_EmeraldChampionsMoveTutor](../../baseline/source/data/scripts/emerald_champions.inc#L1) — Common_EventScript_EmeraldChampionsMoveTutor at (13,2); shared behavior TUTOR | **KEEP** · [W-C-TUTOR](../common-contracts.md#w-c-tutor) |
| `EverGrandeCity_PokemonCenter_1F:warp_events:001` | 8,8 | Warp from (8,8, elevation 3) to MAP_EVER_GRANDE_CITY warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_PokemonCenter_1F:warp_events:002` | 7,8 | Warp from (7,8, elevation 3) to MAP_EVER_GRANDE_CITY warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_PokemonCenter_1F:warp_events:003` | 1,6 | Warp from (1,6, elevation 4) to MAP_EVER_GRANDE_CITY_POKEMON_CENTER_2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_PokemonCenter_1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [EverGrandeCity_PokemonCenter_1F_OnTransition](../../baseline/source/data/maps/EverGrandeCity_PokemonCenter_1F/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls EverGrandeCity_PokemonCenter_1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `EverGrandeCity_PokemonCenter_1F:map_scripts:002` | MAP_SCRIPT_ON_RESUME | [CableClub_OnResume](../../baseline/source/data/scripts/cable_club.inc#L1226) — MAP_SCRIPT_ON_RESUME calls CableClub_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
