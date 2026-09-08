# SeafloorCavern_Room4

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SeafloorCavern_Room4/map.json) · [Scripts](../../baseline/source/data/maps/SeafloorCavern_Room4/scripts.inc)

## Current map contract

`MAP_SEAFLOOR_CAVERN_ROOM4` · `LAYOUT_SEAFLOOR_CAVERN_ROOM4` · `WEATHER_NONE` · `MUS_MT_CHIMNEY`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SeafloorCavern_Room4:object_events:001` | 5,8 | [SeafloorCavern_Room4_EventScript_Grunt3](../../baseline/source/data/maps/SeafloorCavern_Room4/scripts.inc#L4) — SeafloorCavern_Room4_EventScript_Grunt3 at (5,8); Who are you? How did you even get /  down here without the sub? / Lost it… and I'm lost. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `SeafloorCavern_Room4:object_events:002` | 5,12 | [SeafloorCavern_Room4_EventScript_Grunt4](../../baseline/source/data/maps/SeafloorCavern_Room4/scripts.inc#L9) — SeafloorCavern_Room4_EventScript_Grunt4 at (5,12); Who are you? Where do you think /  you're going? Not past TOXAPEX. / I failed to win. Twice today. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `SeafloorCavern_Room4:warp_events:001` | 13,1 | Warp from (13,1, elevation 3) to MAP_SEAFLOOR_CAVERN_ROOM2 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SeafloorCavern_Room4:warp_events:002` | 4,1 | Warp from (4,1, elevation 3) to MAP_SEAFLOOR_CAVERN_ROOM5 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SeafloorCavern_Room4:warp_events:003` | 9,10 | Warp from (9,10, elevation 3) to MAP_SEAFLOOR_CAVERN_ROOM5 warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SeafloorCavern_Room4:warp_events:004` | 10,15 | Warp from (10,15, elevation 3) to MAP_SEAFLOOR_CAVERN_ENTRANCE warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
