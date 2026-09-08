# LilycoveCity_DepartmentStore_1F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_1F/map.json) · [Scripts](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_1F/scripts.inc)

## Current map contract

`MAP_LILYCOVE_CITY_DEPARTMENT_STORE_1F` · `LAYOUT_LILYCOVE_CITY_DEPARTMENT_STORE_1F` · `WEATHER_NONE` · `MUS_LILYCOVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LilycoveCity_DepartmentStore_1F:object_events:001` | 8,2 | [LilycoveCity_DepartmentStore_1F_EventScript_Greeter](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_1F/scripts.inc#L4) — LilycoveCity_DepartmentStore_1F_EventScript_Greeter at (8,2); Welcome to LILYCOVE DEPARTMENT STORE. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_1F:object_events:002` | 10,2 | [LilycoveCity_DepartmentStore_1F_EventScript_LotteryClerk](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_1F/scripts.inc#L8) — LilycoveCity_DepartmentStore_1F_EventScript_LotteryClerk at (10,2); shared behavior STORY | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `LilycoveCity_DepartmentStore_1F:object_events:003` | 14,5 | [LilycoveCity_DepartmentStore_1F_EventScript_PokefanF](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_1F/scripts.inc#L115) — LilycoveCity_DepartmentStore_1F_EventScript_PokefanF at (14,5); Whenever I come to the DEPARTMENT /  STORE, I always end up buying all sorts /  of things because it's so fun. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_1F:object_events:004` | 4,4 | [LilycoveCity_DepartmentStore_1F_EventScript_LittleGirl](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_1F/scripts.inc#L119) — LilycoveCity_DepartmentStore_1F_EventScript_LittleGirl at (4,4); Today, my mom is going to buy me some /  nice furniture. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_1F:object_events:005` | 3,6 | [LilycoveCity_DepartmentStore_1F_EventScript_PokefanM](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_1F/scripts.inc#L123) — LilycoveCity_DepartmentStore_1F_EventScript_PokefanM at (3,6); I'm buying something for my AZUMARILL /  as a reward for winning a CONTEST. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_1F:object_events:006` | 2,6 | [LilycoveCity_DepartmentStore_1F_EventScript_Azumarill](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_1F/scripts.inc#L127) — LilycoveCity_DepartmentStore_1F_EventScript_Azumarill at (2,6); AZUMARILL: Maririroo! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_1F:bg_events:001` | 0,8 | [LilycoveCity_DepartmentStore_1F_EventScript_FloorNamesSign](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_1F/scripts.inc#L137) — LilycoveCity_DepartmentStore_1F_EventScript_FloorNamesSign at (0,8); 1F: SERVICE COUNTER /      LOTTERY CORNER //  2F: TRAINER'S ZONE //  3F: BATTLE COLLECTION //  4F: MOVE STUDY //  5F: POKé DOLL FLOOR //  ROOFTOP: ROOFTOP PLAZA | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `LilycoveCity_DepartmentStore_1F:warp_events:001` | 8,7 | Warp from (8,7, elevation 0) to MAP_LILYCOVE_CITY warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_DepartmentStore_1F:warp_events:002` | 9,7 | Warp from (9,7, elevation 0) to MAP_LILYCOVE_CITY warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_DepartmentStore_1F:warp_events:003` | 16,1 | Warp from (16,1, elevation 0) to MAP_LILYCOVE_CITY_DEPARTMENT_STORE_2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_DepartmentStore_1F:warp_events:004` | 2,1 | Warp from (2,1, elevation 0) to MAP_LILYCOVE_CITY_DEPARTMENT_STORE_ELEVATOR warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
