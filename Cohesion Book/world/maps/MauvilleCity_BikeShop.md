# MauvilleCity_BikeShop

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../03-mauville.md) · [Map source](../../baseline/source/data/maps/MauvilleCity_BikeShop/map.json) · [Scripts](../../baseline/source/data/maps/MauvilleCity_BikeShop/scripts.inc)

## Current map contract

`MAP_MAUVILLE_CITY_BIKE_SHOP` · `LAYOUT_MAUVILLE_CITY_BIKE_SHOP` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MauvilleCity_BikeShop:object_events:001` | 2,5 | [MauvilleCity_BikeShop_EventScript_Rydel](../../baseline/source/data/maps/MauvilleCity_BikeShop/scripts.inc#L4) — MauvilleCity_BikeShop_EventScript_Rydel at (2,5); Well, well, what have we here? /  A most energetic customer! //  Me? You may call me RYDEL. /  I'm the owner of this cycle shop. / RYDEL: Your RUNNING SHOES… /  They're awfully filthy. //  Did you come from far away? | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `MauvilleCity_BikeShop:object_events:002` | 7,6 | [MauvilleCity_BikeShop_EventScript_Assistant](../../baseline/source/data/maps/MauvilleCity_BikeShop/scripts.inc#L100) — MauvilleCity_BikeShop_EventScript_Assistant at (7,6); I'm learning about BIKES while /  I work here. //  If you need advice on how to ride your /  BIKE, there're a couple handbooks in /  the back. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MauvilleCity_BikeShop:bg_events:001` | 8,1 | [MauvilleCity_BikeShop_EventScript_MachBikeHandbook](../../baseline/source/data/maps/MauvilleCity_BikeShop/scripts.inc#L104) — MauvilleCity_BikeShop_EventScript_MachBikeHandbook at (8,1); It's a handbook on the MACH BIKE. //  Which page do you want to read? / A BIKE moves in the direction that /  the + Control Pad is pressed. //  It will speed up once it gets rolling. //  To stop, release the + Control Pad. /  The BIKE will slow to a stop. //  Want to read a different page? | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `MauvilleCity_BikeShop:bg_events:002` | 11,1 | [MauvilleCity_BikeShop_EventScript_AcroBikeHandbook](../../baseline/source/data/maps/MauvilleCity_BikeShop/scripts.inc#L142) — MauvilleCity_BikeShop_EventScript_AcroBikeHandbook at (11,1); It's a handbook on the ACRO BIKE. //  Which page do you want to read? / Press the B Button while riding, /  and the front wheel lifts up. //  You can zip around with the front /  wheel up using the + Control Pad. //  This technique is called a wheelie. //  Want to read a different page? | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `MauvilleCity_BikeShop:warp_events:001` | 3,8 | Warp from (3,8, elevation 0) to MAP_MAUVILLE_CITY warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MauvilleCity_BikeShop:warp_events:002` | 4,8 | Warp from (4,8, elevation 0) to MAP_MAUVILLE_CITY warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
