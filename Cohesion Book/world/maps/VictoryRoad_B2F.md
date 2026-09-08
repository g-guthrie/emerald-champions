# VictoryRoad_B2F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../07-league.md) · [Map source](../../baseline/source/data/maps/VictoryRoad_B2F/map.json) · [Scripts](../../baseline/source/data/maps/VictoryRoad_B2F/scripts.inc)

## Current map contract

`MAP_VICTORY_ROAD_B2F` · `LAYOUT_VICTORY_ROAD_B2F` · `WEATHER_NONE` · `MUS_VICTORY_ROAD`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `VictoryRoad_B2F:object_events:001` | 15,6 | [VictoryRoad_B2F_EventScript_Vito](../../baseline/source/data/maps/VictoryRoad_B2F/scripts.inc#L4) — VictoryRoad_B2F_EventScript_Vito at (15,6); I trained together with my whole family, /  every one of us! /  I'm not losing to anyone! / Better than my family?! /  Is that possible?! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `VictoryRoad_B2F:object_events:002` | 43,14 | [VictoryRoad_B2F_EventScript_Owen](../../baseline/source/data/maps/VictoryRoad_B2F/scripts.inc#L9) — VictoryRoad_B2F_EventScript_Owen at (43,14); I'd heard that there was a tough /  little kid around. Do they mean you? / The little shrimp is tough! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `VictoryRoad_B2F:object_events:003` | 2,17 | [VictoryRoad_B2F_EventScript_Caroline](../../baseline/source/data/maps/VictoryRoad_B2F/scripts.inc#L14) — VictoryRoad_B2F_EventScript_Caroline at (2,17); You must be getting a little tired. / No signs of tiring at all! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `VictoryRoad_B2F:object_events:004` | 35,22 | [VictoryRoad_B2F_EventScript_Julie](../../baseline/source/data/maps/VictoryRoad_B2F/scripts.inc#L19) — VictoryRoad_B2F_EventScript_Julie at (35,22); You shouldn't get complacent just /  because you have a lot of GYM BADGES. //  There's always going to be someone /  who's better than you! / You're better than me! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `VictoryRoad_B2F:object_events:005` | 13,8 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_FULL_HEAL; root Common_EventScript_FindItem; flag FLAG_ITEM_VICTORY_ROAD_B2F_FULL_HEAL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `VictoryRoad_B2F:object_events:006` | 25,18 | [VictoryRoad_B2F_EventScript_Dianne](../../baseline/source/data/maps/VictoryRoad_B2F/scripts.inc#L29) — VictoryRoad_B2F_EventScript_Dianne at (25,18); The elite among the elite gather in /  this cave. //  How are you finding it? / Not rattled in the least bit! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `VictoryRoad_B2F:object_events:007` | 25,21 | [VictoryRoad_B2F_EventScript_Felix](../../baseline/source/data/maps/VictoryRoad_B2F/scripts.inc#L24) — VictoryRoad_B2F_EventScript_Felix at (25,21); I've come this far, but the tension's /  giving me awful stomach pain… / Ooh… /  It hurts… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `VictoryRoad_B2F:bg_events:001` | 28,5 | Hidden ITEM_ELIXIR at (28,5); persistent flag FLAG_HIDDEN_ITEM_VICTORY_ROAD_B2F_ELIXIR. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `VictoryRoad_B2F:bg_events:002` | 37,1 | Hidden ITEM_MAX_REPEL at (37,1); persistent flag FLAG_HIDDEN_ITEM_VICTORY_ROAD_B2F_MAX_REPEL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `VictoryRoad_B2F:warp_events:001` | 30,25 | Warp from (30,25, elevation 3) to MAP_VICTORY_ROAD_B1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VictoryRoad_B2F:warp_events:002` | 43,2 | Warp from (43,2, elevation 3) to MAP_VICTORY_ROAD_B1F warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VictoryRoad_B2F:warp_events:003` | 19,12 | Warp from (19,12, elevation 3) to MAP_VICTORY_ROAD_B1F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VictoryRoad_B2F:warp_events:004` | 5,26 | Warp from (5,26, elevation 3) to MAP_VICTORY_ROAD_B1F warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
