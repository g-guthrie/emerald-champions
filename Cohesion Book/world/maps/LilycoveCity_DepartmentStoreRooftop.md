# LilycoveCity_DepartmentStoreRooftop

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/LilycoveCity_DepartmentStoreRooftop/map.json) · [Scripts](../../baseline/source/data/maps/LilycoveCity_DepartmentStoreRooftop/scripts.inc)

## Current map contract

`MAP_LILYCOVE_CITY_DEPARTMENT_STORE_ROOFTOP` · `LAYOUT_LILYCOVE_CITY_DEPARTMENT_STORE_ROOFTOP` · `WEATHER_NONE` · `MUS_LILYCOVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LilycoveCity_DepartmentStoreRooftop:object_events:001` | 4,4 | [LilycoveCity_DepartmentStoreRooftop_EventScript_Man](../../baseline/source/data/maps/LilycoveCity_DepartmentStoreRooftop/scripts.inc#L49) — LilycoveCity_DepartmentStoreRooftop_EventScript_Man at (4,4); Don't they have set dates for their /  clear-out sales? //  I watch TV, but they never show any /  commercials. / Yes! I've been waiting a long time for /  this clear-out sale. //  They have unusual decorations today. /  I'm going to load up, that I am! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStoreRooftop:object_events:002` | 7,5 | [LilycoveCity_DepartmentStoreRooftop_EventScript_ThirstyMan](../../baseline/source/data/maps/LilycoveCity_DepartmentStoreRooftop/scripts.inc#L63) — LilycoveCity_DepartmentStoreRooftop_EventScript_ThirstyMan at (7,5); Ohh… I'm bone-dry thirsty! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStoreRooftop:object_events:003` | 6,1 | [LilycoveCity_DepartmentStoreRooftop_EventScript_SaleWoman](../../baseline/source/data/maps/LilycoveCity_DepartmentStoreRooftop/scripts.inc#L19) — LilycoveCity_DepartmentStoreRooftop_EventScript_SaleWoman at (6,1); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStoreRooftop:object_events:004` | 15,5 | [LilycoveCity_DepartmentStoreRooftop_EventScript_SubstituteTutor](../../baseline/source/data/scripts/move_tutors.inc#L99) — LilycoveCity_DepartmentStoreRooftop_EventScript_SubstituteTutor at (15,5); shared behavior TUTOR | **KEEP** · [W-C-TUTOR](../common-contracts.md#w-c-tutor) |
| `LilycoveCity_DepartmentStoreRooftop:bg_events:001` | 9,1 | [LilycoveCity_DepartmentStoreRooftop_EventScript_VendingMachine](../../baseline/source/data/maps/LilycoveCity_DepartmentStoreRooftop/scripts.inc#L67) — LilycoveCity_DepartmentStoreRooftop_EventScript_VendingMachine at (9,1); It's a VENDING MACHINE. /  Which drink would you like? / Clang! //  A can of {STR_VAR_1} dropped down. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `LilycoveCity_DepartmentStoreRooftop:bg_events:002` | 10,1 | [LilycoveCity_DepartmentStoreRooftop_EventScript_VendingMachine](../../baseline/source/data/maps/LilycoveCity_DepartmentStoreRooftop/scripts.inc#L67) — LilycoveCity_DepartmentStoreRooftop_EventScript_VendingMachine at (10,1); It's a VENDING MACHINE. /  Which drink would you like? / Clang! //  A can of {STR_VAR_1} dropped down. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `LilycoveCity_DepartmentStoreRooftop:warp_events:001` | 13,3 | Warp from (13,3, elevation 0) to MAP_LILYCOVE_CITY_DEPARTMENT_STORE_5F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_DepartmentStoreRooftop:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [LilycoveCity_DepartmentStoreRooftop_OnTransition](../../baseline/source/data/maps/LilycoveCity_DepartmentStoreRooftop/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls LilycoveCity_DepartmentStoreRooftop_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
