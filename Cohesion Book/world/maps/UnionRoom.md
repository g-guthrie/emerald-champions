# UnionRoom

**REVISE.** Shared player-owned or external-link support. Preserve geography, trades, records and decoration data. Native trainer challenge entry is governed by the all-doubles external-entry contract, never silently accepted as a singles exception.

[Regional experience](../10-support.md) · [Map source](../../baseline/source/data/maps/UnionRoom/map.json) · [Scripts](../../baseline/source/data/maps/UnionRoom/scripts.inc)

## Current map contract

`MAP_UNION_ROOM` · `LAYOUT_UNION_ROOM` · `WEATHER_NONE` · `MUS_EVER_GRANDE`

Shared player-owned or external-link support. Preserve geography, trades, records and decoration data. Native trainer challenge entry is governed by the all-doubles external-entry contract, never silently accepted as a singles exception.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `UnionRoom:object_events:001` | 3,2 | [UnionRoom_EventScript_Attendant](../../baseline/source/data/maps/UnionRoom/scripts.inc#L94) — UnionRoom_EventScript_Attendant at (3,2); shared behavior STORY | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story)  · LINK-01 |
| `UnionRoom:object_events:002` | 1,8 | [UnionRoom_EventScript_Player4](../../baseline/source/data/maps/UnionRoom/scripts.inc#L54) — UnionRoom_EventScript_Player4 at (1,8); shared behavior STORY | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story)  · LINK-01 |
| `UnionRoom:object_events:003` | 7,8 | [UnionRoom_EventScript_Player8](../../baseline/source/data/maps/UnionRoom/scripts.inc#L86) — UnionRoom_EventScript_Player8 at (7,8); shared behavior STORY | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story)  · LINK-01 |
| `UnionRoom:object_events:004` | 1,4 | [UnionRoom_EventScript_Player7](../../baseline/source/data/maps/UnionRoom/scripts.inc#L78) — UnionRoom_EventScript_Player7 at (1,4); shared behavior STORY | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story)  · LINK-01 |
| `UnionRoom:object_events:005` | 7,4 | [UnionRoom_EventScript_Player6](../../baseline/source/data/maps/UnionRoom/scripts.inc#L70) — UnionRoom_EventScript_Player6 at (7,4); shared behavior STORY | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story)  · LINK-01 |
| `UnionRoom:object_events:006` | 13,4 | [UnionRoom_EventScript_Player5](../../baseline/source/data/maps/UnionRoom/scripts.inc#L62) — UnionRoom_EventScript_Player5 at (13,4); shared behavior STORY | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story)  · LINK-01 |
| `UnionRoom:object_events:007` | 10,6 | [UnionRoom_EventScript_Player3](../../baseline/source/data/maps/UnionRoom/scripts.inc#L46) — UnionRoom_EventScript_Player3 at (10,6); shared behavior STORY | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story)  · LINK-01 |
| `UnionRoom:object_events:008` | 13,8 | [UnionRoom_EventScript_Player2](../../baseline/source/data/maps/UnionRoom/scripts.inc#L38) — UnionRoom_EventScript_Player2 at (13,8); shared behavior STORY | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story)  · LINK-01 |
| `UnionRoom:object_events:009` | 4,6 | [UnionRoom_EventScript_Player1](../../baseline/source/data/maps/UnionRoom/scripts.inc#L30) — UnionRoom_EventScript_Player1 at (4,6); shared behavior STORY | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story)  · LINK-01 |
| `UnionRoom:warp_events:001` | 7,11 | Warp from (7,11, elevation 3) to MAP_DYNAMIC warp WARP_ID_DYNAMIC. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp)  · LINK-01 |
| `UnionRoom:warp_events:002` | 8,11 | Warp from (8,11, elevation 3) to MAP_DYNAMIC warp WARP_ID_DYNAMIC. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp)  · LINK-01 |
| `UnionRoom:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [UnionRoom_OnResume](../../baseline/source/data/maps/UnionRoom/scripts.inc#L6) — MAP_SCRIPT_ON_RESUME calls UnionRoom_OnResume. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback)  · LINK-01 |
| `UnionRoom:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [UnionRoom_OnTransition](../../baseline/source/data/maps/UnionRoom/scripts.inc#L26) — MAP_SCRIPT_ON_TRANSITION calls UnionRoom_OnTransition. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback)  · LINK-01 |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.

Shared entry/format reconciliation: **LINK-01**. Preserve imported teams, trading and records; use its exact doubles-only native entry and recovery rules.
