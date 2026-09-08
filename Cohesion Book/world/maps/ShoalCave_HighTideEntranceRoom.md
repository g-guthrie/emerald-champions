# ShoalCave_HighTideEntranceRoom

**INERT/EXCLUDED.** This registered header has no normal entrance. Its layout is actively selected by the corresponding LowTide map when the tide flag is set; preserve layout assets and IDs, and test that actual shared-header transition.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/ShoalCave_HighTideEntranceRoom/map.json) · [Scripts](../../baseline/source/data/maps/ShoalCave_HighTideEntranceRoom/scripts.inc)

## Current map contract

`MAP_SHOAL_CAVE_HIGH_TIDE_ENTRANCE_ROOM` · `LAYOUT_SHOAL_CAVE_HIGH_TIDE_ENTRANCE_ROOM` · `WEATHER_NONE` · `MUS_MT_PYRE`

This registered header has no normal entrance. Its layout is actively selected by the corresponding LowTide map when the tide flag is set; preserve layout assets and IDs, and test that actual shared-header transition.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
