# Underwater_Route127

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Underwater_Route127/map.json) · [Scripts](../../baseline/source/data/maps/Underwater_Route127/scripts.inc)

## Current map contract

`MAP_UNDERWATER_ROUTE127` · `LAYOUT_UNDERWATER_ROUTE127` · `WEATHER_UNDERWATER_BUBBLES` · `MUS_UNDERWATER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Underwater_Route127:bg_events:001` | 12,42 | Hidden ITEM_STAR_PIECE at (12,42); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_127_STAR_PIECE. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route127:bg_events:002` | 50,36 | Hidden ITEM_PEARL_STRING at (50,36); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_127_PEARL_STRING. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route127:bg_events:003` | 34,72 | Hidden ITEM_HEART_SCALE at (34,72); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_127_HEART_SCALE. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route127:bg_events:004` | 72,20 | Hidden ITEM_RED_SHARD at (72,20); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_127_RED_SHARD. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route127:warp_events:001` | 57,5 | Warp from (57,5, elevation 0) to MAP_UNDERWATER_MARINE_CAVE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Underwater_Route127:warp_events:002` | 67,38 | Warp from (67,38, elevation 0) to MAP_UNDERWATER_MARINE_CAVE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Underwater_Route127:connections:001` | emerge | emerge connection to MAP_ROUTE127, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Underwater_Route127:connections:002` | left | left connection to MAP_UNDERWATER_ROUTE126, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Underwater_Route127:connections:003` | down | down connection to MAP_UNDERWATER_ROUTE128, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Underwater_Route127:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [Underwater_Route127_OnResume](../../baseline/source/data/maps/Underwater_Route127/scripts.inc#L5) — MAP_SCRIPT_ON_RESUME calls Underwater_Route127_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
