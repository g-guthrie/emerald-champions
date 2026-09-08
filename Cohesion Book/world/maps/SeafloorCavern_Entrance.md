# SeafloorCavern_Entrance

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SeafloorCavern_Entrance/map.json) · [Scripts](../../baseline/source/data/maps/SeafloorCavern_Entrance/scripts.inc)

## Current map contract

`MAP_SEAFLOOR_CAVERN_ENTRANCE` · `LAYOUT_SEAFLOOR_CAVERN_ENTRANCE` · `WEATHER_NONE` · `MUS_MT_CHIMNEY`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SeafloorCavern_Entrance:object_events:001` | 10,2 | [SeafloorCavern_Entrance_EventScript_Grunt](../../baseline/source/data/maps/SeafloorCavern_Entrance/scripts.inc#L10) — SeafloorCavern_Entrance_EventScript_Grunt at (10,2); Hey! /  I remember your face! //  If you're here, it must mean that /  you're about to mess with us again! //  A punk like you, do you really think /  you can take on TEAM AQUA? //  I'd say you're too early by about /  a trillion years! //  You're a perfect fit for the likes of /  TEAM MAGMA! //  Speaking of TEAM MAGMA, I hear they /  were spotted near MOSSDEEP. //  That bunch of goons, they sure don't /  look good near the sea! / A punk like you, do you really think /  you can take on TEAM AQUA? //  I'd say you're too early by about /  a trillion years! //  You're a perfect fit for the likes of /  TEAM MAGMA! //  Speaking of TEAM MAGMA, I hear they /  were spotted near MOSSDEEP. //  That bunch of goons, they sure don't /  look good near the sea! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `SeafloorCavern_Entrance:warp_events:001` | 10,18 | Warp from (10,18, elevation 3) to MAP_UNDERWATER_ROUTE128 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SeafloorCavern_Entrance:warp_events:002` | 10,1 | Warp from (10,1, elevation 3) to MAP_SEAFLOOR_CAVERN_ROOM1 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SeafloorCavern_Entrance:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [SeafloorCavern_Entrance_OnResume](../../baseline/source/data/maps/SeafloorCavern_Entrance/scripts.inc#L5) — MAP_SCRIPT_ON_RESUME calls SeafloorCavern_Entrance_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
