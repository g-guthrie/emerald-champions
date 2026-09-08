# MauvilleCity_PokemonCenter_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../03-mauville.md) · [Map source](../../baseline/source/data/maps/MauvilleCity_PokemonCenter_1F/map.json) · [Scripts](../../baseline/source/data/maps/MauvilleCity_PokemonCenter_1F/scripts.inc)

## Current map contract

`MAP_MAUVILLE_CITY_POKEMON_CENTER_1F` · `LAYOUT_POKEMON_CENTER_1F` · `WEATHER_NONE` · `MUS_POKE_CENTER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MauvilleCity_PokemonCenter_1F:object_events:001` | 8,2 | [MauvilleCity_PokemonCenter_1F_EventScript_Nurse](../../baseline/source/data/maps/MauvilleCity_PokemonCenter_1F/scripts.inc#L16) — MauvilleCity_PokemonCenter_1F_EventScript_Nurse at (8,2); shared behavior CENTER | **REPAIR** · [W-C-CENTER](../common-contracts.md#w-c-center) · W-CENTER-FIRST-VISIT |
| `MauvilleCity_PokemonCenter_1F:object_events:002` | 5,4 | [MauvilleCity_PokemonCenter_1F_EventScript_MauvilleOldMan](../../baseline/source/data/scripts/mauville_man.inc#L1) — MauvilleCity_PokemonCenter_1F_EventScript_MauvilleOldMan at (5,4); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MauvilleCity_PokemonCenter_1F:object_events:003` | 12,7 | [MauvilleCity_PokemonCenter_1F_EventScript_Woman1](../../baseline/source/data/maps/MauvilleCity_PokemonCenter_1F/scripts.inc#L24) — MauvilleCity_PokemonCenter_1F_EventScript_Woman1 at (12,7); That man over there, he says weird /  things! //  He's funny in a weird way. /  I doubt I'll forget about him! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MauvilleCity_PokemonCenter_1F:object_events:004` | 10,6 | [MauvilleCity_PokemonCenter_1F_EventScript_Woman2](../../baseline/source/data/maps/MauvilleCity_PokemonCenter_1F/scripts.inc#L28) — MauvilleCity_PokemonCenter_1F_EventScript_Woman2 at (10,6); When I accessed the RECORD CORNER, /  the data for what's hot in DEWFORD /  got updated. //  Now that bit of data is the same /  as my friend's! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MauvilleCity_PokemonCenter_1F:object_events:005` | 3,5 | [MauvilleCity_PokemonCenter_1F_EventScript_Youngster](../../baseline/source/data/maps/MauvilleCity_PokemonCenter_1F/scripts.inc#L32) — MauvilleCity_PokemonCenter_1F_EventScript_Youngster at (3,5); A RECORD CORNER opened upstairs in /  the POKéMON CENTER. //  I don't know what it's about, but it /  sounds fun. I'll go check it out! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MauvilleCity_PokemonCenter_1F:object_events:006` | 2,2 | [Common_EventScript_EmeraldChampionsBattleVendor](../../baseline/source/data/scripts/emerald_champions.inc#L384) — Common_EventScript_EmeraldChampionsBattleVendor at (2,2); shared behavior VENDOR | **KEEP** · [W-C-VENDOR](../common-contracts.md#w-c-vendor) |
| `MauvilleCity_PokemonCenter_1F:object_events:007` | 13,2 | [Common_EventScript_EmeraldChampionsMoveTutor](../../baseline/source/data/scripts/emerald_champions.inc#L1) — Common_EventScript_EmeraldChampionsMoveTutor at (13,2); shared behavior TUTOR | **KEEP** · [W-C-TUTOR](../common-contracts.md#w-c-tutor) |
| `MauvilleCity_PokemonCenter_1F:warp_events:001` | 8,8 | Warp from (8,8, elevation 3) to MAP_MAUVILLE_CITY warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MauvilleCity_PokemonCenter_1F:warp_events:002` | 7,8 | Warp from (7,8, elevation 3) to MAP_MAUVILLE_CITY warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MauvilleCity_PokemonCenter_1F:warp_events:003` | 1,6 | Warp from (1,6, elevation 4) to MAP_MAUVILLE_CITY_POKEMON_CENTER_2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MauvilleCity_PokemonCenter_1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [MauvilleCity_PokemonCenter_1F_OnTransition](../../baseline/source/data/maps/MauvilleCity_PokemonCenter_1F/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls MauvilleCity_PokemonCenter_1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `MauvilleCity_PokemonCenter_1F:map_scripts:002` | MAP_SCRIPT_ON_RESUME | [CableClub_OnResume](../../baseline/source/data/scripts/cable_club.inc#L1226) — MAP_SCRIPT_ON_RESUME calls CableClub_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
