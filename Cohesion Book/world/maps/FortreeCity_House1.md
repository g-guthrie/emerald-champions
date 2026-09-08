# FortreeCity_House1

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/FortreeCity_House1/map.json) · [Scripts](../../baseline/source/data/maps/FortreeCity_House1/scripts.inc)

## Current map contract

`MAP_FORTREE_CITY_HOUSE1` · `LAYOUT_FORTREE_CITY_HOUSE1` · `WEATHER_NONE` · `MUS_FORTREE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `FortreeCity_House1:object_events:001` | 1,3 | [FortreeCity_House1_EventScript_Trader](../../baseline/source/data/maps/FortreeCity_House1/scripts.inc#L4) — FortreeCity_House1_EventScript_Trader at (1,3); Wrooooaaar! I need it! /  I have to get me a {STR_VAR_1}! /  I'll do anything for it! //  …Uh… Did you hear that? /  My shout from the bottom of my heart? //  Having heard that, you will trade /  your {STR_VAR_1} for my {STR_VAR_2}, /  won't you? / Oh, yeah, right on! //  {STR_VAR_1}, welcome! /  {STR_VAR_2}, you take care! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `FortreeCity_House1:object_events:002` | 2,3 | [FortreeCity_House1_EventScript_Zigzagoon](../../baseline/source/data/maps/FortreeCity_House1/scripts.inc#L33) — FortreeCity_House1_EventScript_Zigzagoon at (2,3); ZIGZAGOON: Gumomoh? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity_House1:object_events:003` | 7,4 | [FortreeCity_House1_EventScript_MegaGift_MILOTICITE](../../baseline/source/data/maps/FortreeCity_House1/scripts.inc#L81) — FortreeCity_House1_EventScript_MegaGift_MILOTICITE at (7,4); Trading POKéMON with others… //  It's as if you're trading your own /  memories with other people. / People admire a Milotic, but few /  remember the patient work of raising it. //  Take this stone if you find one. /  Beauty deserves a little adventure. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `FortreeCity_House1:warp_events:001` | 3,5 | Warp from (3,5, elevation 0) to MAP_FORTREE_CITY warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity_House1:warp_events:002` | 4,5 | Warp from (4,5, elevation 0) to MAP_FORTREE_CITY warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
