# LilycoveCity_UnusedMart

**INERT/EXCLUDED.** Registered historical or prototype map; no native adventure entry is authored. Preserve numeric identity and assets; do not populate it to inflate exploration coverage.

[Regional experience](../10-support.md) · [Map source](../../baseline/source/data/maps/LilycoveCity_UnusedMart/map.json) · [Scripts](../../baseline/source/data/maps/LilycoveCity_UnusedMart/scripts.inc)

## Current map contract

`MAP_LILYCOVE_CITY_UNUSED_MART` · `LAYOUT_MART` · `WEATHER_NONE` · `MUS_POKE_MART`

Registered historical or prototype map; no native adventure entry is authored. Preserve numeric identity and assets; do not populate it to inflate exploration coverage.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LilycoveCity_UnusedMart:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_LILYCOVE_CITY warp 0. | **INERT/EXCLUDED** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_UnusedMart:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_LILYCOVE_CITY warp 0. | **INERT/EXCLUDED** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
