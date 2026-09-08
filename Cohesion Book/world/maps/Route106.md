# Route106

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/Route106/map.json) · [Scripts](../../baseline/source/data/maps/Route106/scripts.inc)

## Current map contract

`MAP_ROUTE106` · `LAYOUT_ROUTE106` · `WEATHER_SUNNY` · `MUS_ROUTE104`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route106:object_events:001` | 18,5 | [Route106_EventScript_Douglas](../../baseline/source/data/maps/Route106/scripts.inc#L8) — Route106_EventScript_Douglas at (18,5); A LAPRAS ferry sounds relaxing. /  I should have booked one. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route106:object_events:002` | 29,10 | [Route106_EventScript_Kyla](../../baseline/source/data/maps/Route106/scripts.inc#L12) — Route106_EventScript_Kyla at (29,10); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route106:object_events:003` | 51,14 | [Route106_EventScript_Elliot](../../baseline/source/data/maps/Route106/scripts.inc#L17) — Route106_EventScript_Elliot at (51,14); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route106:object_events:004` | 65,14 | [Route106_EventScript_Ned](../../baseline/source/data/maps/Route106/scripts.inc#L38) — Route106_EventScript_Ned at (65,14); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route106:object_events:005` | 29,14 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_GREAT_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_106_GREAT_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route106:object_events:006` | 63,13 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (63,13); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route106:object_events:007` | 52,16 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (52,16); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route106:object_events:008` | 58,12 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (58,12); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route106:object_events:009` | 54,11 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (54,11); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route106:object_events:010` | 51,17 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (51,17); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route106:bg_events:001` | 41,11 | Hidden ITEM_POKE_BALL at (41,11); persistent flag FLAG_HIDDEN_ITEM_ROUTE_106_POKE_BALL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route106:bg_events:002` | 53,12 | Hidden ITEM_PRISM_SCALE at (53,12); persistent flag FLAG_EC_HIDDEN_ITEM_ROUTE_106_PRISM_SCALE. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route106:bg_events:003` | 68,15 | Hidden ITEM_HEART_SCALE at (68,15); persistent flag FLAG_HIDDEN_ITEM_ROUTE_106_HEART_SCALE. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route106:bg_events:004` | 59,13 | [Route106_EventScript_TrainerTipsSign](../../baseline/source/data/maps/Route106/scripts.inc#L4) — Route106_EventScript_TrainerTipsSign at (59,13); TRAINER TIPS //  Advice on catching POKéMON with a ROD: /  Press the A Button if you get a bite. | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route106:warp_events:001` | 48,16 | Warp from (48,16, elevation 0) to MAP_GRANITE_CAVE_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route106:connections:001` | up | up connection to MAP_ROUTE105, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route106:connections:002` | down | down connection to MAP_DEWFORD_TOWN, offset 60. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
