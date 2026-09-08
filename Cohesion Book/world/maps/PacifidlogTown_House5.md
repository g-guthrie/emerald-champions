# PacifidlogTown_House5

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/PacifidlogTown_House5/map.json) · [Scripts](../../baseline/source/data/maps/PacifidlogTown_House5/scripts.inc)

## Current map contract

`MAP_PACIFIDLOG_TOWN_HOUSE5` · `LAYOUT_PACIFIDLOG_TOWN_HOUSE1` · `WEATHER_NONE` · `MUS_LILYCOVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `PacifidlogTown_House5:object_events:001` | 9,4 | [PacifidlogTown_House5_EventScript_MirageIslandWatcher](../../baseline/source/data/maps/PacifidlogTown_House5/scripts.inc#L4) — PacifidlogTown_House5_EventScript_MirageIslandWatcher at (9,4); I can't see MIRAGE ISLAND today… / Oh! Oh my! /  I can see MIRAGE ISLAND today! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `PacifidlogTown_House5:object_events:002` | 3,4 | [PacifidlogTown_House5_EventScript_Gentleman](../../baseline/source/data/maps/PacifidlogTown_House5/scripts.inc#L18) — PacifidlogTown_House5_EventScript_Gentleman at (3,4); MIRAGE ISLAND… //  It must become visible and invisible /  depending on the weather conditions /  that make mirages appear. //  Or is it really appearing and /  disappearing? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PacifidlogTown_House5:warp_events:001` | 4,8 | Warp from (4,8, elevation 0) to MAP_PACIFIDLOG_TOWN warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PacifidlogTown_House5:warp_events:002` | 5,8 | Warp from (5,8, elevation 0) to MAP_PACIFIDLOG_TOWN warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
