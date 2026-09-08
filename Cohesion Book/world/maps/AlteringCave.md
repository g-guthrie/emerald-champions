# AlteringCave

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/AlteringCave/map.json) · [Scripts](../../baseline/source/data/maps/AlteringCave/scripts.inc)

## Current map contract

`MAP_ALTERING_CAVE` · `LAYOUT_ALTERING_CAVE` · `WEATHER_NONE` · `MUS_RG_SEVII_CAVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `AlteringCave:object_events:001` | 5,20 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (5,20); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave:object_events:002` | 7,21 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (7,21); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave:object_events:003` | 9,18 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (9,18); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave:object_events:004` | 11,21 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (11,21); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave:object_events:005` | 13,21 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (13,21); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave:object_events:006` | 14,20 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (14,20); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave:warp_events:001` | 33,21 | Warp from (33,21, elevation 0) to MAP_ROUTE103 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AlteringCave:warp_events:002` | 2,4 | Warp from (2,4, elevation 0) to MAP_ALTERING_CAVE_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AlteringCave:warp_events:003` | 5,15 | Warp from (5,15, elevation 0) to MAP_ALTERING_CAVE_1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AlteringCave:warp_events:004` | 10,2 | Warp from (10,2, elevation 0) to MAP_ALTERING_CAVE_1F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AlteringCave:warp_events:005` | 24,11 | Warp from (24,11, elevation 0) to MAP_ALTERING_CAVE_1F warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AlteringCave:warp_events:006` | 30,10 | Warp from (30,10, elevation 0) to MAP_ALTERING_CAVE_1F warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AlteringCave:warp_events:007` | 34,2 | Warp from (34,2, elevation 0) to MAP_ALTERING_CAVE_1F warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AlteringCave:warp_events:008` | 1,8 | Warp from (1,8, elevation 0) to MAP_ALTERING_CAVE_B1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AlteringCave:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [AlteringCave_OnTransition](../../baseline/source/data/maps/AlteringCave/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls AlteringCave_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
