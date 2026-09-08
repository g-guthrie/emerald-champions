# FortreeCity_House4

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/FortreeCity_House4/map.json) · [Scripts](../../baseline/source/data/maps/FortreeCity_House4/scripts.inc)

## Current map contract

`MAP_FORTREE_CITY_HOUSE4` · `LAYOUT_FORTREE_CITY_HOUSE2` · `WEATHER_NONE` · `MUS_FORTREE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `FortreeCity_House4:object_events:001` | 6,4 | [FortreeCity_House4_EventScript_Woman](../../baseline/source/data/maps/FortreeCity_House4/scripts.inc#L4) — FortreeCity_House4_EventScript_Woman at (6,4); By being together with POKéMON, /  people make more and more friends. //  And that brings the world closer /  together. I think it's wonderful! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity_House4:object_events:002` | 1,3 | [FortreeCity_House4_EventScript_Boy](../../baseline/source/data/maps/FortreeCity_House4/scripts.inc#L8) — FortreeCity_House4_EventScript_Boy at (1,3); There! /  Go, BIRD POKéMON! / Heheh, I asked my WINGULL to run /  an errand for me. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `FortreeCity_House4:object_events:003` | 2,3 | [FortreeCity_House4_EventScript_Wingull](../../baseline/source/data/maps/FortreeCity_House4/scripts.inc#L55) — FortreeCity_House4_EventScript_Wingull at (2,3); WINGULL: Pihyoh! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity_House4:warp_events:001` | 3,5 | Warp from (3,5, elevation 0) to MAP_FORTREE_CITY warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity_House4:warp_events:002` | 4,5 | Warp from (4,5, elevation 0) to MAP_FORTREE_CITY warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
