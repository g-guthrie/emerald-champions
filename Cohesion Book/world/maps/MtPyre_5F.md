# MtPyre_5F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/MtPyre_5F/map.json) · [Scripts](../../baseline/source/data/maps/MtPyre_5F/scripts.inc)

## Current map contract

`MAP_MT_PYRE_5F` · `LAYOUT_MT_PYRE_5F` · `WEATHER_NONE` · `MUS_MT_PYRE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MtPyre_5F:object_events:001` | 3,7 | [MtPyre_5F_EventScript_Atsushi](../../baseline/source/data/maps/MtPyre_4F/scripts.inc#L6) — MtPyre_5F_EventScript_Atsushi at (3,7); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MtPyre_5F:object_events:002` | 6,11 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_LAX_INCENSE; root Common_EventScript_FindItem; flag FLAG_ITEM_MT_PYRE_5F_LAX_INCENSE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MtPyre_5F:warp_events:001` | 2,1 | Warp from (2,1, elevation 3) to MAP_MT_PYRE_6F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_5F:warp_events:002` | 10,5 | Warp from (10,5, elevation 3) to MAP_MT_PYRE_4F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_5F:warp_events:003` | 1,10 | Warp from (1,10, elevation 3) to MAP_MT_PYRE_6F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_5F:warp_events:004` | 12,10 | Warp from (12,10, elevation 3) to MAP_MT_PYRE_4F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_5F:warp_events:005` | 12,12 | Warp from (12,12, elevation 3) to MAP_MT_PYRE_4F warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
