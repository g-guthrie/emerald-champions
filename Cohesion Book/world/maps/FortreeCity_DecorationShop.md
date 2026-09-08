# FortreeCity_DecorationShop

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/FortreeCity_DecorationShop/map.json) · [Scripts](../../baseline/source/data/maps/FortreeCity_DecorationShop/scripts.inc)

## Current map contract

`MAP_FORTREE_CITY_DECORATION_SHOP` · `LAYOUT_FORTREE_CITY_DECORATION_SHOP` · `WEATHER_NONE` · `MUS_FORTREE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `FortreeCity_DecorationShop:object_events:001` | 6,5 | [FortreeCity_DecorationShop_EventScript_PokefanM](../../baseline/source/data/maps/FortreeCity_DecorationShop/scripts.inc#L4) — FortreeCity_DecorationShop_EventScript_PokefanM at (6,5); Merchandise you buy here is sent to /  your own PC. //  That's fantastic! I wish they could /  also deliver me home like that. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity_DecorationShop:object_events:002` | 0,4 | [FortreeCity_DecorationShop_EventScript_Girl](../../baseline/source/data/maps/FortreeCity_DecorationShop/scripts.inc#L8) — FortreeCity_DecorationShop_EventScript_Girl at (0,4); I'm buying a pretty desk and I'm /  putting my cute DOLLS on it. //  If I don't, when I decorate my /  SECRET BASE, my DOLLS will get /  dirty or poked with splinters. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity_DecorationShop:object_events:003` | 1,2 | [FortreeCity_DecorationShop_EventScript_ClerkDesks](../../baseline/source/data/maps/FortreeCity_DecorationShop/scripts.inc#L12) — FortreeCity_DecorationShop_EventScript_ClerkDesks at (1,2); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity_DecorationShop:object_events:004` | 6,2 | [FortreeCity_DecorationShop_EventScript_ClerkChairs](../../baseline/source/data/maps/FortreeCity_DecorationShop/scripts.inc#L34) — FortreeCity_DecorationShop_EventScript_ClerkChairs at (6,2); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity_DecorationShop:warp_events:001` | 3,5 | Warp from (3,5, elevation 0) to MAP_FORTREE_CITY warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity_DecorationShop:warp_events:002` | 4,5 | Warp from (4,5, elevation 0) to MAP_FORTREE_CITY warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
