# LilycoveCity_DepartmentStore_3F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_3F/map.json) · [Scripts](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_3F/scripts.inc)

## Current map contract

`MAP_LILYCOVE_CITY_DEPARTMENT_STORE_3F` · `LAYOUT_LILYCOVE_CITY_DEPARTMENT_STORE_3F` · `WEATHER_NONE` · `MUS_POKE_MART`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LilycoveCity_DepartmentStore_3F:object_events:001` | 0,5 | [LilycoveCity_DepartmentStore_3F_EventScript_TriathleteM](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_3F/scripts.inc#L45) — LilycoveCity_DepartmentStore_3F_EventScript_TriathleteM at (0,5); For building a new team quickly, /  the right kind of BALL is best. //  QUICK BALLS reward a fast throw, /  while DUSK BALLS shine in caves. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_3F:object_events:002` | 7,7 | [LilycoveCity_DepartmentStore_3F_EventScript_PokefanM](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_3F/scripts.inc#L49) — LilycoveCity_DepartmentStore_3F_EventScript_PokefanM at (7,7); Some rare POKéMON take patience. //  A TIMER BALL grows stronger as the /  battle lasts, so I always carry a few. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_3F:object_events:003` | 13,5 | [LilycoveCity_DepartmentStore_3F_EventScript_Woman](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_3F/scripts.inc#L53) — LilycoveCity_DepartmentStore_3F_EventScript_Woman at (13,5); I caught a new partner with a /  QUICK BALL on the very first turn! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_3F:object_events:004` | 8,2 | [LilycoveCity_DepartmentStore_3F_EventScript_ClerkLeft](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_3F/scripts.inc#L4) — LilycoveCity_DepartmentStore_3F_EventScript_ClerkLeft at (8,2); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_3F:object_events:005` | 10,2 | [LilycoveCity_DepartmentStore_3F_EventScript_ClerkRight](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_3F/scripts.inc#L24) — LilycoveCity_DepartmentStore_3F_EventScript_ClerkRight at (10,2); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_3F:warp_events:001` | 13,1 | Warp from (13,1, elevation 0) to MAP_LILYCOVE_CITY_DEPARTMENT_STORE_2F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_DepartmentStore_3F:warp_events:002` | 16,1 | Warp from (16,1, elevation 0) to MAP_LILYCOVE_CITY_DEPARTMENT_STORE_4F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_DepartmentStore_3F:warp_events:003` | 2,1 | Warp from (2,1, elevation 0) to MAP_LILYCOVE_CITY_DEPARTMENT_STORE_ELEVATOR warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
