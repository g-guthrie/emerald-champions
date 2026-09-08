# MauvilleCity_House2

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../03-mauville.md) · [Map source](../../baseline/source/data/maps/MauvilleCity_House2/map.json) · [Scripts](../../baseline/source/data/maps/MauvilleCity_House2/scripts.inc)

## Current map contract

`MAP_MAUVILLE_CITY_HOUSE2` · `LAYOUT_HOUSE1` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MauvilleCity_House2:object_events:001` | 4,5 | [MauvilleCity_House2_EventScript_Woman](../../baseline/source/data/maps/MauvilleCity_House2/scripts.inc#L14) — MauvilleCity_House2_EventScript_Woman at (4,5); If I had a BIKE, it'd be easy to cycle to /  SLATEPORT for some shopping. //  I'd be able to buy HARBOR MAIL at the /  POKéMON MART in SLATEPORT… / Oh! You have HARBOR MAIL? /  Will you trade it for a COIN CASE? | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `MauvilleCity_House2:object_events:002` | 7,4 | [MauvilleCity_House2_EventScript_MegaGift_AMPHAROSITE](../../baseline/source/data/maps/MauvilleCity_House2/scripts.inc#L77) — MauvilleCity_House2_EventScript_MegaGift_AMPHAROSITE at (7,4); I studied the PLEDGE moves and the /  three ultimate starter techniques. //  The POKéMON CENTER specialist can /  teach all six now, free of charge. / An Ampharos can light a road for /  everyone following behind it. //  Take this stone. A strong partner /  can help others find their way. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `MauvilleCity_House2:warp_events:001` | 3,8 | Warp from (3,8, elevation 0) to MAP_MAUVILLE_CITY warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MauvilleCity_House2:warp_events:002` | 4,8 | Warp from (4,8, elevation 0) to MAP_MAUVILLE_CITY warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
