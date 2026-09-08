# SootopolisCity_PokemonCenter_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SootopolisCity_PokemonCenter_1F/map.json) · [Scripts](../../baseline/source/data/maps/SootopolisCity_PokemonCenter_1F/scripts.inc)

## Current map contract

`MAP_SOOTOPOLIS_CITY_POKEMON_CENTER_1F` · `LAYOUT_POKEMON_CENTER_1F` · `WEATHER_NONE` · `MUS_POKE_CENTER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SootopolisCity_PokemonCenter_1F:object_events:001` | 8,2 | [SootopolisCity_PokemonCenter_1F_EventScript_Nurse](../../baseline/source/data/maps/SootopolisCity_PokemonCenter_1F/scripts.inc#L10) — SootopolisCity_PokemonCenter_1F_EventScript_Nurse at (8,2); shared behavior CENTER | **REPAIR** · [W-C-CENTER](../common-contracts.md#w-c-center) · W-CENTER-FIRST-VISIT |
| `SootopolisCity_PokemonCenter_1F:object_events:002` | 10,6 | [SootopolisCity_PokemonCenter_1F_EventScript_Gentleman](../../baseline/source/data/maps/SootopolisCity_PokemonCenter_1F/scripts.inc#L18) — SootopolisCity_PokemonCenter_1F_EventScript_Gentleman at (10,6); WALLACE is rumored to be the toughest /  TRAINER in the whole HOENN region. //  This town's GYM is led by the TRAINER /  who taught WALLACE. //  But the ELITE FOUR… They're said to be /  even stronger than WALLACE's mentor. //  How strong could they be? / Everyone in town has taken refuge /  and won't come out of their homes. //  Even I would rather not venture /  outside. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `SootopolisCity_PokemonCenter_1F:object_events:003` | 4,7 | [SootopolisCity_PokemonCenter_1F_EventScript_Woman](../../baseline/source/data/maps/SootopolisCity_PokemonCenter_1F/scripts.inc#L32) — SootopolisCity_PokemonCenter_1F_EventScript_Woman at (4,7); Whenever, wherever, and whatever /  happens, I will always be friends with /  POKéMON. //  Because it's fun to be with POKéMON! / Aren't POKéMON our friends? //  Why are they going wild this way? | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `SootopolisCity_PokemonCenter_1F:object_events:004` | 1,2 | [SootopolisCity_PokemonCenter_1F_EventScript_Clerk2](../../baseline/source/data/maps/SootopolisCity_PokemonCenter_1F/scripts.inc#L71) — SootopolisCity_PokemonCenter_1F_EventScript_Clerk2 at (1,2); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SootopolisCity_PokemonCenter_1F:object_events:005` | 2,2 | [Common_EventScript_EmeraldChampionsBattleVendor](../../baseline/source/data/scripts/emerald_champions.inc#L384) — Common_EventScript_EmeraldChampionsBattleVendor at (2,2); shared behavior VENDOR | **KEEP** · [W-C-VENDOR](../common-contracts.md#w-c-vendor) |
| `SootopolisCity_PokemonCenter_1F:object_events:006` | 13,2 | [Common_EventScript_EmeraldChampionsMoveTutor](../../baseline/source/data/scripts/emerald_champions.inc#L1) — Common_EventScript_EmeraldChampionsMoveTutor at (13,2); shared behavior TUTOR | **KEEP** · [W-C-TUTOR](../common-contracts.md#w-c-tutor) |
| `SootopolisCity_PokemonCenter_1F:warp_events:001` | 8,8 | Warp from (8,8, elevation 3) to MAP_SOOTOPOLIS_CITY warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SootopolisCity_PokemonCenter_1F:warp_events:002` | 7,8 | Warp from (7,8, elevation 3) to MAP_SOOTOPOLIS_CITY warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SootopolisCity_PokemonCenter_1F:warp_events:003` | 1,6 | Warp from (1,6, elevation 4) to MAP_SOOTOPOLIS_CITY_POKEMON_CENTER_2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SootopolisCity_PokemonCenter_1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [SootopolisCity_PokemonCenter_1F_OnTransition](../../baseline/source/data/maps/SootopolisCity_PokemonCenter_1F/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls SootopolisCity_PokemonCenter_1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `SootopolisCity_PokemonCenter_1F:map_scripts:002` | MAP_SCRIPT_ON_RESUME | [CableClub_OnResume](../../baseline/source/data/scripts/cable_club.inc#L1226) — MAP_SCRIPT_ON_RESUME calls CableClub_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
