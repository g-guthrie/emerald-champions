# Underwater_Route128

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Underwater_Route128/map.json) · [Scripts](../../baseline/source/data/maps/Underwater_Route128/scripts.inc)

## Current map contract

`MAP_UNDERWATER_ROUTE128` · `LAYOUT_UNDERWATER_ROUTE128` · `WEATHER_UNDERWATER_BUBBLES` · `MUS_UNDERWATER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Underwater_Route128:bg_events:001` | 38,19 | Hidden ITEM_BIG_PEARL at (38,19); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_128_BIG_PEARL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route128:bg_events:002` | 69,18 | Hidden ITEM_PEARL at (69,18); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_128_PEARL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route128:warp_events:001` | 38,26 | Warp from (38,26, elevation 3) to MAP_UNDERWATER_SEAFLOOR_CAVERN warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Underwater_Route128:connections:001` | up | up connection to MAP_UNDERWATER_ROUTE127, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Underwater_Route128:connections:002` | emerge | emerge connection to MAP_ROUTE128, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
