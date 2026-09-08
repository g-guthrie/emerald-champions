# MtPyre_6F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/MtPyre_6F/map.json) · [Scripts](../../baseline/source/data/maps/MtPyre_6F/scripts.inc)

## Current map contract

`MAP_MT_PYRE_6F` · `LAYOUT_MT_PYRE_6F` · `WEATHER_NONE` · `MUS_MT_PYRE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MtPyre_6F:object_events:001` | 6,3 | [MtPyre_6F_EventScript_Valerie](../../baseline/source/data/maps/MtPyre_6F/scripts.inc#L4) — MtPyre_6F_EventScript_Valerie at (6,3); When I'm here… /  A curious power flows into me… / The power is ebbing away… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MtPyre_6F:object_events:002` | 6,9 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_BLUK_BERRY; root Common_EventScript_FindItem; flag FLAG_EC_GARDEN_BUNDLE_MTPYRE_6F_BLUK_BERRY. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MtPyre_6F:object_events:003` | 10,3 | [MtPyre_6F_EventScript_Cedric](../../baseline/source/data/maps/MtPyre_6F/scripts.inc#L25) — MtPyre_6F_EventScript_Cedric at (10,3); Have you lost your bearings? /  Have no fear for I am here! / Weren't you lost? | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MtPyre_6F:object_events:004` | 5,10 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (5,10); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `MtPyre_6F:bg_events:001` | 8,8 | [MtPyre_6F_EventScript_Pecharunt](../../baseline/source/data/maps/MtPyre_6F/scripts.inc#L73) — MtPyre_6F_EventScript_Pecharunt at (8,8); A violet SIGN shows three masks: a dog, /  a monkey, and a brilliant bird. //  Catch all three challengers before /  returning to this poisoned shrine. / The three masks belong to OKIDOGI, /  MUNKIDORI and FEZANDIPITI. //  Catch all three, then return here. /  They do not need to travel with you. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `MtPyre_6F:warp_events:001` | 2,1 | Warp from (2,1, elevation 3) to MAP_MT_PYRE_5F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_6F:warp_events:002` | 1,10 | Warp from (1,10, elevation 3) to MAP_MT_PYRE_5F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
