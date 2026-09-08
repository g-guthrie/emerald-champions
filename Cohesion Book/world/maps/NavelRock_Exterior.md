# NavelRock_Exterior

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/NavelRock_Exterior/map.json) · [Scripts](../../baseline/source/data/maps/NavelRock_Exterior/scripts.inc)

## Current map contract

`MAP_NAVEL_ROCK_EXTERIOR` · `LAYOUT_NAVEL_ROCK_EXTERIOR` · `WEATHER_NONE` · `MUS_RG_SEVII_ROUTE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `NavelRock_Exterior:warp_events:001` | 10,18 | Warp from (10,18, elevation 0) to MAP_NAVEL_ROCK_HARBOR warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `NavelRock_Exterior:warp_events:002` | 10,10 | Warp from (10,10, elevation 0) to MAP_NAVEL_ROCK_ENTRANCE warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `NavelRock_Exterior:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [NavelRock_Exterior_OnTransition](../../baseline/source/data/maps/NavelRock_Exterior/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls NavelRock_Exterior_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
