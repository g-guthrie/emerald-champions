# FortreeCity_House5

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/FortreeCity_House5/map.json) · [Scripts](../../baseline/source/data/maps/FortreeCity_House5/scripts.inc)

## Current map contract

`MAP_FORTREE_CITY_HOUSE5` · `LAYOUT_FORTREE_CITY_HOUSE1` · `WEATHER_NONE` · `MUS_FORTREE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `FortreeCity_House5:object_events:001` | 6,4 | [FortreeCity_House5_EventScript_MegaGift_CHIMECHITE](../../baseline/source/data/maps/FortreeCity_House5/scripts.inc#L37) — FortreeCity_House5_EventScript_MegaGift_CHIMECHITE at (6,4); The tree houses of FORTREE are great! //  I think it's the number one town for /  living together with POKéMON. / Listen to the chimes in the trees. /  No two breezes sound quite alike. //  I kept this for a Chimecho. /  Perhaps yours will give it a voice. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `FortreeCity_House5:object_events:002` | 6,3 | [FortreeCity_House5_EventScript_Zigzagoon](../../baseline/source/data/maps/FortreeCity_House5/scripts.inc#L12) — FortreeCity_House5_EventScript_Zigzagoon at (6,3); ZIGZAGOON: Bufuu! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity_House5:object_events:003` | 2,4 | [FortreeCity_House5_EventScript_Man](../../baseline/source/data/maps/FortreeCity_House5/scripts.inc#L8) — FortreeCity_House5_EventScript_Man at (2,4); POKéMON and people have adapted to /  nature for survival. //  There's no need to make nature /  conform to the way we want to live. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity_House5:warp_events:001` | 3,5 | Warp from (3,5, elevation 0) to MAP_FORTREE_CITY warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity_House5:warp_events:002` | 4,5 | Warp from (4,5, elevation 0) to MAP_FORTREE_CITY warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
