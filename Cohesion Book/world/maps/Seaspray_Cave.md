# Seaspray_Cave

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/Seaspray_Cave/map.json) · [Scripts](../../baseline/source/data/maps/Seaspray_Cave/scripts.inc)

## Current map contract

`MAP_SEASPRAY_CAVE` · `LAYOUT_SEASPRAY_CAVE` · `WEATHER_NONE` · `MUS_MT_PYRE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Seaspray_Cave:object_events:001` | 5,30 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (5,30); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Seaspray_Cave:object_events:002` | 25,31 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (25,31); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Seaspray_Cave:object_events:003` | 7,27 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (7,27); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Seaspray_Cave:object_events:004` | 5,5 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_DAWN_STONE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_SEASPRAY_DAWN_STONE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Seaspray_Cave:object_events:005` | 10,24 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_LURE_BALL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_SEASPRAY_LURE_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Seaspray_Cave:object_events:006` | 6,25 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_BLASTOISINITE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_SEASPRAY_BLASTOISINITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Seaspray_Cave:object_events:007` | 25,34 | [Seaspray_Cave_EventScript_DuskBallPack](../../baseline/source/data/maps/Seaspray_Cave/scripts.inc#L19) — Pickup ITEM_DUSK_BALL; root Seaspray_Cave_EventScript_DuskBallPack; flag FLAG_EC_ITEM_SEASPRAY_CAVE_DUSK_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Seaspray_Cave:object_events:008` | 46,18 | [Seaspray_Cave_EventScript_TimerBallPack](../../baseline/source/data/maps/Seaspray_Cave/scripts.inc#L23) — Pickup ITEM_TIMER_BALL; root Seaspray_Cave_EventScript_TimerBallPack; flag FLAG_EC_ITEM_SEASPRAY_TIMER_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Seaspray_Cave:bg_events:001` | 36,22 | Hidden ITEM_DAWN_STONE at (36,22); persistent flag FLAG_EC_HIDDEN_ITEM_SEASPRAY_DAWN_STONE. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Seaspray_Cave:warp_events:001` | 9,34 | Warp from (9,34, elevation 3) to MAP_ROUTE115 warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Seaspray_Cave:warp_events:002` | 10,10 | Warp from (10,10, elevation 3) to MAP_SEASPRAY_CAVE_B1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Seaspray_Cave:warp_events:003` | 31,29 | Warp from (31,29, elevation 0) to MAP_SEASPRAY_CAVE_B1F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Seaspray_Cave:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Seaspray_Cave_OnTransition](../../baseline/source/data/maps/Seaspray_Cave/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Seaspray_Cave_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
