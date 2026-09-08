# NavelRock_B1F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/NavelRock_B1F/map.json) · [Scripts](../../baseline/source/data/maps/NavelRock_B1F/scripts.inc)

## Current map contract

`MAP_NAVEL_ROCK_B1F` · `LAYOUT_NAVEL_ROCK_B1F` · `WEATHER_NONE` · `MUS_RG_SEVII_CAVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `NavelRock_B1F:warp_events:001` | 4,3 | Warp from (4,3, elevation 3) to MAP_NAVEL_ROCK_ENTRANCE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `NavelRock_B1F:warp_events:002` | 18,9 | Warp from (18,9, elevation 3) to MAP_NAVEL_ROCK_FORK warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
