# LilycoveCity_DepartmentStore_4F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_4F/map.json) · [Scripts](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_4F/scripts.inc)

## Current map contract

`MAP_LILYCOVE_CITY_DEPARTMENT_STORE_4F` · `LAYOUT_LILYCOVE_CITY_DEPARTMENT_STORE_4F` · `WEATHER_NONE` · `MUS_POKE_MART`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LilycoveCity_DepartmentStore_4F:object_events:001` | 0,2 | [LilycoveCity_DepartmentStore_4F_EventScript_Gentleman](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_4F/scripts.inc#L4) — LilycoveCity_DepartmentStore_4F_EventScript_Gentleman at (0,2); Hmm… //  Immediate pressure… /  Or a defensive answer… //  A good set needs four moves that solve /  one plan without becoming predictable. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_4F:object_events:002` | 6,2 | [LilycoveCity_DepartmentStore_4F_EventScript_Woman](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_4F/scripts.inc#L8) — LilycoveCity_DepartmentStore_4F_EventScript_Woman at (6,2); There are so many different kinds of /  moves. //  The POKéMON CENTER tutor can teach /  every move a species may legally learn. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_4F:object_events:003` | 13,4 | [LilycoveCity_DepartmentStore_4F_EventScript_Youngster](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_4F/scripts.inc#L12) — LilycoveCity_DepartmentStore_4F_EventScript_Youngster at (13,4); A POKéMON learns only four moves. //  Changing one can change the role of the /  whole team. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_4F:object_events:004` | 7,6 | [LilycoveCity_DepartmentStore_4F_EventScript_ClerkLeft](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_4F/scripts.inc#L16) — LilycoveCity_DepartmentStore_4F_EventScript_ClerkLeft at (7,6); Every POKéMON CENTER has a tutor who /  teaches legal moves without charging. //  Experiment freely. No TM purchase is /  needed. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_4F:object_events:005` | 9,6 | [LilycoveCity_DepartmentStore_4F_EventScript_ClerkRight](../../baseline/source/data/maps/LilycoveCity_DepartmentStore_4F/scripts.inc#L20) — LilycoveCity_DepartmentStore_4F_EventScript_ClerkRight at (9,6); The CENTER tutor also applies complete /  competitive sets: moves, Nature, /  Ability, Stat Points, and held item. //  Mega sets still require the matching /  stone from your adventure. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_DepartmentStore_4F:warp_events:001` | 16,1 | Warp from (16,1, elevation 0) to MAP_LILYCOVE_CITY_DEPARTMENT_STORE_3F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_DepartmentStore_4F:warp_events:002` | 13,1 | Warp from (13,1, elevation 0) to MAP_LILYCOVE_CITY_DEPARTMENT_STORE_5F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_DepartmentStore_4F:warp_events:003` | 2,1 | Warp from (2,1, elevation 0) to MAP_LILYCOVE_CITY_DEPARTMENT_STORE_ELEVATOR warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
