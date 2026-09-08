# SeafloorCavern_Room9

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SeafloorCavern_Room9/map.json) · [Scripts](../../baseline/source/data/maps/SeafloorCavern_Room9/scripts.inc)

## Current map contract

`MAP_SEAFLOOR_CAVERN_ROOM9` · `LAYOUT_SEAFLOOR_CAVERN_ROOM9` · `WEATHER_FOG_HORIZONTAL` · `MUS_MT_CHIMNEY`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SeafloorCavern_Room9:object_events:001` | 17,38 | Passive/staged OBJ_EVENT_GFX_KYOGRE_FRONT at (17,38); visibility flag FLAG_HIDE_SEAFLOOR_CAVERN_ROOM_9_KYOGRE; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `SeafloorCavern_Room9:object_events:002` | 9,42 | Passive/staged OBJ_EVENT_GFX_ARCHIE at (9,42); visibility flag FLAG_HIDE_SEAFLOOR_CAVERN_ROOM_9_ARCHIE; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `SeafloorCavern_Room9:object_events:003` | 9,42 | Passive/staged OBJ_EVENT_GFX_MAXIE at (9,42); visibility flag FLAG_HIDE_SEAFLOOR_CAVERN_ROOM_9_MAXIE; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `SeafloorCavern_Room9:object_events:004` | 8,41 | Passive/staged OBJ_EVENT_GFX_MAGMA_MEMBER_M at (8,41); visibility flag FLAG_HIDE_SEAFLOOR_CAVERN_ROOM_9_MAGMA_GRUNTS; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `SeafloorCavern_Room9:object_events:005` | 8,42 | Passive/staged OBJ_EVENT_GFX_MAGMA_MEMBER_F at (8,42); visibility flag FLAG_HIDE_SEAFLOOR_CAVERN_ROOM_9_MAGMA_GRUNTS; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `SeafloorCavern_Room9:object_events:006` | 14,5 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MASTER_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_SEAFLOOR_CAVERN_ROOM_9_MASTER_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SeafloorCavern_Room9:object_events:007` | 17,38 | Passive/staged OBJ_EVENT_GFX_KYOGRE_ASLEEP at (17,38); visibility flag FLAG_HIDE_SEAFLOOR_CAVERN_ROOM_9_KYOGRE_ASLEEP; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `SeafloorCavern_Room9:coord_events:001` | 17,42 | [SeafloorCavern_Room9_EventScript_ArchieAwakenKyogre](../../baseline/source/data/maps/SeafloorCavern_Room9/scripts.inc#L4) — Coordinate trigger at (17,42); VAR_SEAFLOOR_CAVERN_STATE == 0 invokes SeafloorCavern_Room9_EventScript_ArchieAwakenKyogre. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `SeafloorCavern_Room9:warp_events:001` | 5,4 | Warp from (5,4, elevation 3) to MAP_SEAFLOOR_CAVERN_ROOM8 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
