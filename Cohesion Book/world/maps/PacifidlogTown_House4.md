# PacifidlogTown_House4

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/PacifidlogTown_House4/map.json) · [Scripts](../../baseline/source/data/maps/PacifidlogTown_House4/scripts.inc)

## Current map contract

`MAP_PACIFIDLOG_TOWN_HOUSE4` · `LAYOUT_PACIFIDLOG_TOWN_HOUSE2` · `WEATHER_NONE` · `MUS_LILYCOVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `PacifidlogTown_House4:object_events:001` | 3,4 | [PacifidlogTown_House4_EventScript_Woman](../../baseline/source/data/maps/PacifidlogTown_House4/scripts.inc#L8) — PacifidlogTown_House4_EventScript_Woman at (3,4); People were saying they saw a POKéMON /  flying high above HOENN. //  Is it flying around all the time? /  Doesn't it need to rest somewhere? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PacifidlogTown_House4:object_events:002` | 7,5 | [PacifidlogTown_House4_EventScript_LittleGirl](../../baseline/source/data/maps/PacifidlogTown_House4/scripts.inc#L4) — PacifidlogTown_House4_EventScript_LittleGirl at (7,5); A sky POKéMON! /  A sky POKéMON! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PacifidlogTown_House4:object_events:003` | 7,4 | [PacifidlogTown_House4_EventScript_Boy](../../baseline/source/data/maps/PacifidlogTown_House4/scripts.inc#L12) — PacifidlogTown_House4_EventScript_Boy at (7,4); Where did you come from? / Yes? /  YES TOWN? //  I've never heard of a place like that. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `PacifidlogTown_House4:warp_events:001` | 4,8 | Warp from (4,8, elevation 0) to MAP_PACIFIDLOG_TOWN warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PacifidlogTown_House4:warp_events:002` | 5,8 | Warp from (5,8, elevation 0) to MAP_PACIFIDLOG_TOWN warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
