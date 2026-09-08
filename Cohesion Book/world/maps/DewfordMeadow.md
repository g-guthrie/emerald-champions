# DewfordMeadow

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/DewfordMeadow/map.json) · [Scripts](../../baseline/source/data/maps/DewfordMeadow/scripts.inc)

## Current map contract

`MAP_DEWFORD_MEADOW` · `LAYOUT_DEWFORD_MEADOW` · `WEATHER_NONE` · `MUS_DEWFORD`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `DewfordMeadow:object_events:001` | 27,1 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_SHINY_STONE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_MEADOW_SHINY_STONE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `DewfordMeadow:object_events:002` | 4,13 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_HAWLUCHANITE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_MEADOW_HAWLUCHANITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `DewfordMeadow:object_events:003` | 21,14 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MAWILITE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_MEADOW_MAWILITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `DewfordMeadow:object_events:004` | 10,12 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (10,12); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `DewfordMeadow:bg_events:001` | 11,18 | Hidden ITEM_YELLOW_NECTAR at (11,18); persistent flag FLAG_EC_HIDDEN_ITEM_DEWFORD_MEADOW_YELLOW_NECTAR. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `DewfordMeadow:bg_events:002` | 21,5 | Hidden ITEM_RED_NECTAR at (21,5); persistent flag FLAG_EC_HIDDEN_ITEM_DEWFORD_MEADOW_RED_NECTAR. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `DewfordMeadow:bg_events:003` | 8,8 | [DewfordMeadow_EventScript_ManorSign](../../baseline/source/data/maps/DewfordMeadow/scripts.inc#L19) — DewfordMeadow_EventScript_ManorSign at (8,8); DEWFORD MANOR: Once a sea captain's /  escape, now home only to POKéMON. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `DewfordMeadow:warp_events:001` | 10,7 | Warp from (10,7, elevation 0) to MAP_DEWFORD_MANOR_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DewfordMeadow:connections:001` | right | right connection to MAP_DEWFORD_TOWN, offset -10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DewfordMeadow:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [DewfordMeadow_OnTransition](../../baseline/source/data/maps/DewfordMeadow/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls DewfordMeadow_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
