# Seaspray_Cave_B1F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/Seaspray_Cave_B1F/map.json) · [Scripts](../../baseline/source/data/maps/Seaspray_Cave_B1F/scripts.inc)

## Current map contract

`MAP_SEASPRAY_CAVE_B1F` · `LAYOUT_SEASPRAY_CAVE_B1F` · `WEATHER_NONE` · `MUS_MT_PYRE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Seaspray_Cave_B1F:object_events:001` | 46,15 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_KINGS_ROCK; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_SEASPRAY_KINGS_ROCK. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Seaspray_Cave_B1F:object_events:002` | 40,7 | [Seaspray_Cave_B1F_EventScript_QuickBallPack](../../baseline/source/data/maps/Seaspray_Cave_B1F/scripts.inc#L4) — Pickup ITEM_QUICK_BALL; root Seaspray_Cave_B1F_EventScript_QuickBallPack; flag FLAG_EC_ITEM_SEASPRAY_CAVE_B1F_QUICK_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Seaspray_Cave_B1F:object_events:003` | 11,12 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_SLOWBRONITE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_SEASPRAY_SLOWBRONITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Seaspray_Cave_B1F:object_events:004` | 20,24 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_CRABOMINITE; root Common_EventScript_FindItem; flag FLAG_EC_MEGA_REWARD_CRABOMINITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Seaspray_Cave_B1F:object_events:005` | 25,21 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ABOMASITE; root Common_EventScript_FindItem; flag FLAG_ITEM_SHOAL_CAVE_ICE_ROOM_ABOMASITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Seaspray_Cave_B1F:object_events:006` | 15,5 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_FROSLASSITE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_SHOAL_CAVE_ICE_ROOM_FROSLASSITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Seaspray_Cave_B1F:object_events:007` | 32,27 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_GLALITITE; root Common_EventScript_FindItem; flag FLAG_ITEM_ABANDONED_SHIP_ROOMS_B1F_GLALITITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Seaspray_Cave_B1F:bg_events:001` | 25,20 | Hidden ITEM_ICE_STONE at (25,20); persistent flag FLAG_EC_HIDDEN_ITEM_SEASPRAY_B1F_ICE_STONE. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Seaspray_Cave_B1F:warp_events:001` | 11,3 | Warp from (11,3, elevation 0) to MAP_SEASPRAY_CAVE warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Seaspray_Cave_B1F:warp_events:002` | 33,22 | Warp from (33,22, elevation 0) to MAP_SEASPRAY_CAVE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
