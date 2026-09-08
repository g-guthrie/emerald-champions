# LilycoveCity_DepartmentStore_5F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_5F/map.json) · [Scripts](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_5F/scripts.inc)

## Current map contract

`MAP_LILYCOVE_CITY_DEPARTMENT_STORE_5F` · `LAYOUT_LILYCOVE_CITY_DEPARTMENT_STORE_5F` · `WEATHER_NONE` · `MUS_POKE_MART`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LilycoveCity_DepartmentStore_5F:object_events:001` | 1,6 | [LilycoveCity_DepartmentStore_5F_EventScript_LittleGirl](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_5F/scripts.inc#L141) — LilycoveCity_DepartmentStore_5F_EventScript_LittleGirl at (1,6); I'm not big enough to raise POKéMON, /  so I'm getting a cute DOLL instead. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_5F:object_events:002` | 7,7 | [LilycoveCity_DepartmentStore_5F_EventScript_PokefanF](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_5F/scripts.inc#L114) — LilycoveCity_DepartmentStore_5F_EventScript_PokefanF at (7,7); This place is full of cute DOLLS. //  I should buy some for me, instead of /  just for my children. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_5F:object_events:003` | 7,2 | [LilycoveCity_DepartmentStore_5F_EventScript_ClerkFarLeft](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_5F/scripts.inc#L18) — LilycoveCity_DepartmentStore_5F_EventScript_ClerkFarLeft at (7,2); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_5F:object_events:004` | 9,2 | [LilycoveCity_DepartmentStore_5F_EventScript_ClerkMidLeft](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_5F/scripts.inc#L44) — LilycoveCity_DepartmentStore_5F_EventScript_ClerkMidLeft at (9,2); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_5F:object_events:005` | 15,6 | [LilycoveCity_DepartmentStore_5F_EventScript_ClerkMidRight](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_5F/scripts.inc#L67) — LilycoveCity_DepartmentStore_5F_EventScript_ClerkMidRight at (15,6); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_5F:object_events:006` | 17,6 | [LilycoveCity_DepartmentStore_5F_EventScript_ClerkFarRight](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_5F/scripts.inc#L90) — LilycoveCity_DepartmentStore_5F_EventScript_ClerkFarRight at (17,6); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_5F:object_events:007` | 9,5 | [LilycoveCity_DepartmentStore_5F_EventScript_Woman](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_5F/scripts.inc#L118) — LilycoveCity_DepartmentStore_5F_EventScript_Woman at (9,5); They sell many cute MATS here. //  I wonder which one I should get? /  Maybe I'll buy them all… / I think they closed the rooftop /  because the weather is wild today. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `LilycoveCity_DepartmentStore_5F:warp_events:001` | 13,1 | Warp from (13,1, elevation 0) to MAP_LILYCOVE_CITY_DEPARTMENT_STORE_4F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_DepartmentStore_5F:warp_events:002` | 2,1 | Warp from (2,1, elevation 0) to MAP_LILYCOVE_CITY_DEPARTMENT_STORE_ELEVATOR warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_DepartmentStore_5F:warp_events:003` | 16,1 | Warp from (16,1, elevation 0) to MAP_LILYCOVE_CITY_DEPARTMENT_STORE_ROOFTOP warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_DepartmentStore_5F:map_scripts:001` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [LilycoveCity_DepartmentStore_5F_OnWarp](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_5F/scripts.inc#L7) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls LilycoveCity_DepartmentStore_5F_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
