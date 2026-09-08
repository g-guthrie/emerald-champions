# NavelRock_Up2

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/NavelRock_Up2/map.json) · [Scripts](../../baseline/source/data/maps/NavelRock_Up2/scripts.inc)

## Current map contract

`MAP_NAVEL_ROCK_UP2` · `LAYOUT_NAVEL_ROCK_LADDER_ROOM2` · `WEATHER_NONE` · `MUS_RG_SEVII_CAVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `NavelRock_Up2:warp_events:001` | 3,3 | Warp from (3,3, elevation 3) to MAP_NAVEL_ROCK_UP1 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `NavelRock_Up2:warp_events:002` | 5,5 | Warp from (5,5, elevation 3) to MAP_NAVEL_ROCK_UP3 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
