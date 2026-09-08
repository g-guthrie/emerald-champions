# AbandonedShip_Rooms2_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/AbandonedShip_Rooms2_1F/map.json) · [Scripts](../../baseline/source/data/maps/AbandonedShip_Rooms2_1F/scripts.inc)

## Current map contract

`MAP_ABANDONED_SHIP_ROOMS2_1F` · `LAYOUT_ABANDONED_SHIP_ROOMS2_1F` · `WEATHER_SHADE` · `MUS_ABANDONED_SHIP`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `AbandonedShip_Rooms2_1F:object_events:001` | 7,13 | [AbandonedShip_Rooms2_1F_EventScript_Dan](../../baseline/source/data/maps/AbandonedShip_Rooms2_1F/scripts.inc#L4) — AbandonedShip_Rooms2_1F_EventScript_Dan at (7,13); DAN: While searching for treasures, /  we discovered a TRAINER! / DAN: We couldn't win even though /  we worked together… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AbandonedShip_Rooms2_1F:object_events:002` | 6,13 | [AbandonedShip_Rooms2_1F_EventScript_Kira](../../baseline/source/data/maps/AbandonedShip_Rooms2_1F/scripts.inc#L23) — AbandonedShip_Rooms2_1F_EventScript_Kira at (6,13); KIRA: Oh? /  We were searching for treasures. /  But we discovered a TRAINER instead! / KIRA: Ooh, so strong! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AbandonedShip_Rooms2_1F:object_events:003` | 4,4 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_REVIVE; root Common_EventScript_FindItem; flag FLAG_ITEM_ABANDONED_SHIP_ROOMS_2_1F_REVIVE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AbandonedShip_Rooms2_1F:object_events:004` | 3,2 | [AbandonedShip_Rooms2_1F_EventScript_Garrison](../../baseline/source/data/maps/AbandonedShip_Rooms2_1F/scripts.inc#L47) — AbandonedShip_Rooms2_1F_EventScript_Garrison at (3,2); Strength and compassion… /  Those are a TRAINER's treasures! / Ah, there is something about you /  that sparkles. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AbandonedShip_Rooms2_1F:object_events:005` | 7,2 | [AbandonedShip_Rooms2_1F_EventScript_Jani](../../baseline/source/data/maps/AbandonedShip_Rooms2_1F/scripts.inc#L42) — AbandonedShip_Rooms2_1F_EventScript_Jani at (7,2); I'm not good at swimming, /  but I am good at battles! / Oops. /  That didn't go very well. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AbandonedShip_Rooms2_1F:warp_events:001` | 4,16 | Warp from (4,16, elevation 3) to MAP_ABANDONED_SHIP_CORRIDORS_1F warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Rooms2_1F:warp_events:002` | 5,16 | Warp from (5,16, elevation 3) to MAP_ABANDONED_SHIP_CORRIDORS_1F warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Rooms2_1F:warp_events:003` | 4,1 | Warp from (4,1, elevation 3) to MAP_ABANDONED_SHIP_CORRIDORS_1F warp 11. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
