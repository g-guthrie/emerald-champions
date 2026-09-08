# Underwater_Route124

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Underwater_Route124/map.json) · [Scripts](../../baseline/source/data/maps/Underwater_Route124/scripts.inc)

## Current map contract

`MAP_UNDERWATER_ROUTE124` · `LAYOUT_UNDERWATER_ROUTE124` · `WEATHER_UNDERWATER_BUBBLES` · `MUS_UNDERWATER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Underwater_Route124:bg_events:001` | 42,51 | Hidden ITEM_PEARL_STRING at (42,51); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_124_PEARL_STRING. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route124:bg_events:002` | 14,40 | Hidden ITEM_GREEN_SHARD at (14,40); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_124_GREEN_SHARD. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route124:bg_events:003` | 66,34 | Hidden ITEM_PEARL at (66,34); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_124_PEARL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route124:bg_events:004` | 64,54 | Hidden ITEM_BIG_PEARL at (64,54); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_124_BIG_PEARL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route124:bg_events:005` | 70,64 | Hidden ITEM_HEART_SCALE at (70,64); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_124_HEART_SCALE_1. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route124:bg_events:006` | 42,5 | Hidden ITEM_BIG_PEARL at (42,5); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_124_BIG_PEARL_2. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route124:bg_events:007` | 45,36 | Hidden ITEM_HEART_SCALE at (45,36); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_124_HEART_SCALE_2. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route124:connections:001` | down | down connection to MAP_UNDERWATER_ROUTE126, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Underwater_Route124:connections:002` | emerge | emerge connection to MAP_ROUTE124, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
