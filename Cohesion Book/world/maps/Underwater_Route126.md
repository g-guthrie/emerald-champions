# Underwater_Route126

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Underwater_Route126/map.json) · [Scripts](../../baseline/source/data/maps/Underwater_Route126/scripts.inc)

## Current map contract

`MAP_UNDERWATER_ROUTE126` · `LAYOUT_UNDERWATER_ROUTE126` · `WEATHER_UNDERWATER_BUBBLES` · `MUS_UNDERWATER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Underwater_Route126:bg_events:001` | 30,17 | Hidden ITEM_HEART_SCALE at (30,17); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_126_HEART_SCALE. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route126:bg_events:002` | 41,19 | Hidden ITEM_ULTRA_BALL at (41,19); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_126_ULTRA_BALL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route126:bg_events:003` | 63,19 | Hidden ITEM_STARDUST at (63,19); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_126_STARDUST. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route126:bg_events:004` | 10,36 | Hidden ITEM_PEARL at (10,36); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_126_PEARL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route126:bg_events:005` | 11,39 | Hidden ITEM_PEARL_STRING at (11,39); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_126_PEARL_STRING. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route126:bg_events:006` | 12,35 | Hidden ITEM_YELLOW_SHARD at (12,35); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_126_YELLOW_SHARD. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route126:bg_events:007` | 65,60 | Hidden ITEM_BIG_PEARL at (65,60); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_126_BIG_PEARL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route126:bg_events:008` | 9,77 | Hidden ITEM_BLUE_SHARD at (9,77); persistent flag FLAG_HIDDEN_ITEM_UNDERWATER_126_BLUE_SHARD. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Underwater_Route126:warp_events:001` | 45,65 | Warp from (45,65, elevation 0) to MAP_UNDERWATER_SOOTOPOLIS_CITY warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Underwater_Route126:connections:001` | up | up connection to MAP_UNDERWATER_ROUTE124, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Underwater_Route126:connections:002` | right | right connection to MAP_UNDERWATER_ROUTE127, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Underwater_Route126:connections:003` | emerge | emerge connection to MAP_ROUTE126, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
