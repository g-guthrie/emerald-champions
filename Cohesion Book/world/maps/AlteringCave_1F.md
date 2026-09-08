# AlteringCave_1F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/AlteringCave_1F/map.json) · [Scripts](../../baseline/source/data/maps/AlteringCave_1F/scripts.inc)

## Current map contract

`MAP_ALTERING_CAVE_1F` · `LAYOUT_ALTERING_CAVE_1F` · `WEATHER_NONE` · `MUS_RG_SEVII_CAVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `AlteringCave_1F:object_events:001` | 4,12 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (4,12); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_1F:object_events:002` | 9,13 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (9,13); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_1F:object_events:003` | 10,20 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (10,20); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_1F:object_events:004` | 23,16 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (23,16); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_1F:object_events:005` | 28,20 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (28,20); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_1F:object_events:006` | 30,20 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (30,20); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_1F:object_events:007` | 33,10 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (33,10); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_1F:object_events:008` | 33,9 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (33,9); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_1F:object_events:009` | 25,11 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (25,11); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_1F:object_events:010` | 13,6 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (13,6); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_1F:object_events:011` | 33,12 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_PRISON_BOTTLE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_PRISON_BOTTLE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AlteringCave_1F:object_events:012` | 29,16 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MASTER_BALL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_MASTER_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AlteringCave_1F:object_events:013` | 9,18 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_BEAST_BALL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_ALTERING_BEAST_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AlteringCave_1F:object_events:014` | 9,5 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (9,5); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `AlteringCave_1F:warp_events:001` | 5,6 | Warp from (5,6, elevation 0) to MAP_ALTERING_CAVE warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AlteringCave_1F:warp_events:002` | 13,4 | Warp from (13,4, elevation 0) to MAP_ALTERING_CAVE warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AlteringCave_1F:warp_events:003` | 7,14 | Warp from (7,14, elevation 0) to MAP_ALTERING_CAVE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AlteringCave_1F:warp_events:004` | 23,10 | Warp from (23,10, elevation 0) to MAP_ALTERING_CAVE warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AlteringCave_1F:warp_events:005` | 26,9 | Warp from (26,9, elevation 0) to MAP_ALTERING_CAVE warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AlteringCave_1F:warp_events:006` | 33,4 | Warp from (33,4, elevation 0) to MAP_ALTERING_CAVE warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
