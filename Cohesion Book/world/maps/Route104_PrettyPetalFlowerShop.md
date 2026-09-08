# Route104_PrettyPetalFlowerShop

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/Route104_PrettyPetalFlowerShop/map.json) · [Scripts](../../baseline/source/data/maps/Route104_PrettyPetalFlowerShop/scripts.inc)

## Current map contract

`MAP_ROUTE104_PRETTY_PETAL_FLOWER_SHOP` · `LAYOUT_ROUTE104_PRETTY_PETAL_FLOWER_SHOP` · `WEATHER_NONE` · `MUS_PETALBURG`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route104_PrettyPetalFlowerShop:object_events:001` | 0,3 | [Route104_PrettyPetalFlowerShop_EventScript_ShopOwner](../../baseline/source/data/maps/Route104_PrettyPetalFlowerShop/scripts.inc#L16) — Route104_PrettyPetalFlowerShop_EventScript_ShopOwner at (0,3); shared behavior STORY | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route104_PrettyPetalFlowerShop:object_events:002` | 7,3 | [Route104_PrettyPetalFlowerShop_EventScript_WailmerPailGirl](../../baseline/source/data/maps/Route104_PrettyPetalFlowerShop/scripts.inc#L62) — Route104_PrettyPetalFlowerShop_EventScript_WailmerPailGirl at (7,3); shared behavior STORY | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route104_PrettyPetalFlowerShop:object_events:003` | 11,6 | [Route104_PrettyPetalFlowerShop_EventScript_MegaGift_VENUSAURITE](../../baseline/source/data/maps/Route104_PrettyPetalFlowerShop/scripts.inc#L97) — Route104_PrettyPetalFlowerShop_EventScript_MegaGift_VENUSAURITE at (11,6); I found this while tending flowers. /  It seems to answer a Venusaur. //  Keep gathering berries as you travel. /  The Berry Master trades stones, too. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route104_PrettyPetalFlowerShop:warp_events:001` | 2,8 | Warp from (2,8, elevation 0) to MAP_ROUTE104 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route104_PrettyPetalFlowerShop:warp_events:002` | 3,8 | Warp from (3,8, elevation 0) to MAP_ROUTE104 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route104_PrettyPetalFlowerShop:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route104_PrettyPetalFlowerShop_OnTransition](../../baseline/source/data/maps/Route104_PrettyPetalFlowerShop/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route104_PrettyPetalFlowerShop_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
