# FortreeCity_House3

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/FortreeCity_House3/map.json) · [Scripts](../../baseline/source/data/maps/FortreeCity_House3/scripts.inc)

## Current map contract

`MAP_FORTREE_CITY_HOUSE3` · `LAYOUT_FORTREE_CITY_HOUSE1` · `WEATHER_NONE` · `MUS_FORTREE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `FortreeCity_House3:object_events:001` | 0,3 | [FortreeCity_House3_EventScript_Maniac](../../baseline/source/data/maps/FortreeCity_House3/scripts.inc#L4) — FortreeCity_House3_EventScript_Maniac at (0,3); While speaking about POKéDEXES, /  I remembered something. //  I met this TRAINER, STEVEN, when /  I was searching for rare stones. //  Hoo, boy, he had some amazing POKéMON /  with him. //  They weren't just rare, they were /  trained to terrifying extremes! //  He might even be stronger than the /  GYM LEADER in this town… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity_House3:object_events:002` | 5,4 | [FortreeCity_House3_EventScript_SchoolKidM](../../baseline/source/data/maps/FortreeCity_House3/scripts.inc#L8) — FortreeCity_House3_EventScript_SchoolKidM at (5,4); What's that thing you have there? //  … … … … … … //  Oh, it's called a POKéDEX? /  It's really awesome! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity_House3:warp_events:001` | 3,5 | Warp from (3,5, elevation 0) to MAP_FORTREE_CITY warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity_House3:warp_events:002` | 4,5 | Warp from (4,5, elevation 0) to MAP_FORTREE_CITY warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
