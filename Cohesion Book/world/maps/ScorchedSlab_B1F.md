# ScorchedSlab_B1F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/ScorchedSlab_B1F/map.json) · [Scripts](../../baseline/source/data/maps/ScorchedSlab_B1F/scripts.inc)

## Current map contract

`MAP_SCORCHED_SLAB_B1F` · `LAYOUT_SCORCHED_SLAB_B1F` · `WEATHER_NONE` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `ScorchedSlab_B1F:warp_events:001` | 3,25 | Warp from (3,25, elevation 0) to MAP_SCORCHED_SLAB warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ScorchedSlab_B1F:warp_events:002` | 23,26 | Warp from (23,26, elevation 0) to MAP_SCORCHED_SLAB_B2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
