# VictoryRoad_B1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../07-league.md) · [Map source](../../baseline/source/data/maps/VictoryRoad_B1F/map.json) · [Scripts](../../baseline/source/data/maps/VictoryRoad_B1F/scripts.inc)

## Current map contract

`MAP_VICTORY_ROAD_B1F` · `LAYOUT_VICTORY_ROAD_B1F` · `WEATHER_NONE` · `MUS_VICTORY_ROAD`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `VictoryRoad_B1F:object_events:001` | 20,5 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (20,5); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `VictoryRoad_B1F:object_events:002` | 21,4 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (21,4); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `VictoryRoad_B1F:object_events:003` | 4,7 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (4,7); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `VictoryRoad_B1F:object_events:004` | 9,10 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (9,10); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `VictoryRoad_B1F:object_events:005` | 20,26 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (20,26); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `VictoryRoad_B1F:object_events:006` | 21,25 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (21,25); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `VictoryRoad_B1F:object_events:007` | 35,6 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (35,6); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `VictoryRoad_B1F:object_events:008` | 19,5 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (19,5); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `VictoryRoad_B1F:object_events:009` | 20,4 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (20,4); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `VictoryRoad_B1F:object_events:010` | 18,12 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (18,12); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `VictoryRoad_B1F:object_events:011` | 20,25 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (20,25); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `VictoryRoad_B1F:object_events:012` | 21,26 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (21,26); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `VictoryRoad_B1F:object_events:013` | 34,4 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (34,4); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `VictoryRoad_B1F:object_events:014` | 37,12 | [VictoryRoad_B1F_EventScript_Samuel](../../baseline/source/data/maps/VictoryRoad_B1F/scripts.inc#L4) — VictoryRoad_B1F_EventScript_Samuel at (37,12); The thought that I'm getting closer to /  the POKéMON LEAGUE… //  I'm getting stage fright… / I couldn't do a thing… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `VictoryRoad_B1F:object_events:015` | 26,16 | [VictoryRoad_B1F_EventScript_Shannon](../../baseline/source/data/maps/VictoryRoad_B1F/scripts.inc#L9) — VictoryRoad_B1F_EventScript_Shannon at (26,16); To win your way through the POKéMON /  LEAGUE, you need the trust of your /  POKéMON. / Your relationship is based on /  solid trust. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `VictoryRoad_B1F:object_events:016` | 5,21 | [VictoryRoad_B1F_EventScript_Michelle](../../baseline/source/data/maps/VictoryRoad_B1F/scripts.inc#L14) — VictoryRoad_B1F_EventScript_Michelle at (5,21); This isn't the goal. It's only a place /  on the way to the POKéMON LEAGUE. / That's the way! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `VictoryRoad_B1F:object_events:017` | 34,3 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (34,3); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `VictoryRoad_B1F:object_events:018` | 42,8 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ULTRA_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_VICTORY_ROAD_B1F_ULTRA_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `VictoryRoad_B1F:object_events:019` | 32,3 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_FULL_RESTORE; root Common_EventScript_FindItem; flag FLAG_ITEM_VICTORY_ROAD_B1F_FULL_RESTORE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `VictoryRoad_B1F:object_events:020` | 14,16 | [VictoryRoad_B1F_EventScript_Mitchell](../../baseline/source/data/maps/VictoryRoad_B1F/scripts.inc#L19) — VictoryRoad_B1F_EventScript_Mitchell at (14,16); My POKéMON are cosmically /  awe inspiring! / I've never met anyone like you before. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `VictoryRoad_B1F:object_events:021` | 14,20 | [VictoryRoad_B1F_EventScript_Halle](../../baseline/source/data/maps/VictoryRoad_B1F/scripts.inc#L24) — VictoryRoad_B1F_EventScript_Halle at (14,20); Okay, no need to get your back up! /  Relax, let's take it easy! / Whoa! /  Wonderful! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `VictoryRoad_B1F:object_events:022` | 13,17 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (13,17); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `VictoryRoad_B1F:warp_events:001` | 30,25 | Warp from (30,25, elevation 3) to MAP_VICTORY_ROAD_B2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VictoryRoad_B1F:warp_events:002` | 17,16 | Warp from (17,16, elevation 3) to MAP_VICTORY_ROAD_B2F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VictoryRoad_B1F:warp_events:003` | 42,25 | Warp from (42,25, elevation 3) to MAP_VICTORY_ROAD_1F warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VictoryRoad_B1F:warp_events:004` | 42,2 | Warp from (42,2, elevation 4) to MAP_VICTORY_ROAD_B2F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VictoryRoad_B1F:warp_events:005` | 8,3 | Warp from (8,3, elevation 3) to MAP_VICTORY_ROAD_1F warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VictoryRoad_B1F:warp_events:006` | 20,21 | Warp from (20,21, elevation 3) to MAP_VICTORY_ROAD_1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VictoryRoad_B1F:warp_events:007` | 5,26 | Warp from (5,26, elevation 3) to MAP_VICTORY_ROAD_B2F warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
