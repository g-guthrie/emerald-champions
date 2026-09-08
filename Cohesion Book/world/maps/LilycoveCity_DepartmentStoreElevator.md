# LilycoveCity_DepartmentStoreElevator

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/LilycoveCity_DepartmentStoreElevator/map.json) · [Scripts](../../baseline/source/data/maps/LilycoveCity_DepartmentStoreElevator/scripts.inc)

## Current map contract

`MAP_LILYCOVE_CITY_DEPARTMENT_STORE_ELEVATOR` · `LAYOUT_LILYCOVE_CITY_DEPARTMENT_STORE_ELEVATOR` · `WEATHER_NONE` · `MUS_LILYCOVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LilycoveCity_DepartmentStoreElevator:object_events:001` | 0,5 | [LilycoveCity_DepartmentStoreElevator_EventScript_Attendant](../../baseline/source/data/maps/LilycoveCity_DepartmentStoreElevator/scripts.inc#L4) — LilycoveCity_DepartmentStoreElevator_EventScript_Attendant at (0,5); shared behavior STORY | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `LilycoveCity_DepartmentStoreElevator:warp_events:001` | 1,5 | Warp from (1,5, elevation 3) to MAP_DYNAMIC warp WARP_ID_DYNAMIC. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_DepartmentStoreElevator:warp_events:002` | 2,5 | Warp from (2,5, elevation 3) to MAP_DYNAMIC warp WARP_ID_DYNAMIC. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
