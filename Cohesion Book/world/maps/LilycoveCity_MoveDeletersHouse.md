# LilycoveCity_MoveDeletersHouse

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/LilycoveCity_MoveDeletersHouse/map.json) · [Scripts](../../baseline/source/data/maps/LilycoveCity_MoveDeletersHouse/scripts.inc)

## Current map contract

`MAP_LILYCOVE_CITY_MOVE_DELETERS_HOUSE` · `LAYOUT_HOUSE2` · `WEATHER_NONE` · `MUS_LILYCOVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LilycoveCity_MoveDeletersHouse:object_events:001` | 4,4 | [LilycoveCity_MoveDeletersHouse_EventScript_MoveDeleter](../../baseline/source/data/maps/LilycoveCity_MoveDeletersHouse/scripts.inc#L4) — LilycoveCity_MoveDeletersHouse_EventScript_MoveDeleter at (4,4); Uh… /  Oh, yes, I'm the MOVE DELETER. //  I can make POKéMON forget their moves. //  Would you like me to do that? / Which POKéMON should forget a move? | **REPAIR** · [W-C-STORY](../common-contracts.md#w-c-story) · W-FIELD-SURF |
| `LilycoveCity_MoveDeletersHouse:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_LILYCOVE_CITY warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_MoveDeletersHouse:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_LILYCOVE_CITY warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
