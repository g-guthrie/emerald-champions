# MtPyre_3F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/MtPyre_3F/map.json) · [Scripts](../../baseline/source/data/maps/MtPyre_3F/scripts.inc)

## Current map contract

`MAP_MT_PYRE_3F` · `LAYOUT_MT_PYRE_3F` · `WEATHER_NONE` · `MUS_MT_PYRE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MtPyre_3F:object_events:001` | 1,4 | [MtPyre_3F_EventScript_William](../../baseline/source/data/maps/MtPyre_3F/scripts.inc#L4) — MtPyre_3F_EventScript_William at (1,4); The rich atmosphere of the mountain /  has elevated my psychic power! //  A mere child like you… /  You dream of winning? / I drown in self-pity… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MtPyre_3F:object_events:002` | 11,4 | [MtPyre_3F_EventScript_Kayla](../../baseline/source/data/maps/MtPyre_3F/scripts.inc#L9) — MtPyre_3F_EventScript_Kayla at (11,4); Ahahahaha! //  This is no place for children, least /  of all you! / I lost that cleanly… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MtPyre_3F:object_events:003` | 0,7 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_SUPER_REPEL; root Common_EventScript_FindItem; flag FLAG_ITEM_MT_PYRE_3F_SUPER_REPEL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MtPyre_3F:object_events:004` | 6,4 | [MtPyre_3F_EventScript_Gabrielle](../../baseline/source/data/maps/MtPyre_3F/scripts.inc#L14) — MtPyre_3F_EventScript_Gabrielle at (6,4); Why have you come here? / That was amazing! /  You're a very special TRAINER. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MtPyre_3F:warp_events:001` | 10,1 | Warp from (10,1, elevation 3) to MAP_MT_PYRE_2F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_3F:warp_events:002` | 2,1 | Warp from (2,1, elevation 3) to MAP_MT_PYRE_4F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_3F:warp_events:003` | 9,10 | Warp from (9,10, elevation 3) to MAP_MT_PYRE_4F warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_3F:warp_events:004` | 1,12 | Warp from (1,12, elevation 3) to MAP_MT_PYRE_4F warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_3F:warp_events:005` | 10,12 | Warp from (10,12, elevation 3) to MAP_MT_PYRE_2F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_3F:warp_events:006` | 6,12 | Warp from (6,12, elevation 3) to MAP_MT_PYRE_2F warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
