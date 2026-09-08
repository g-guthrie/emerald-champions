# MossdeepCity_House3

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/MossdeepCity_House3/map.json) · [Scripts](../../baseline/source/data/maps/MossdeepCity_House3/scripts.inc)

## Current map contract

`MAP_MOSSDEEP_CITY_HOUSE3` · `LAYOUT_HOUSE2` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MossdeepCity_House3:object_events:001` | 4,4 | [MossdeepCity_House3_EventScript_SuperRodFisherman](../../baseline/source/data/maps/MossdeepCity_House3/scripts.inc#L4) — MossdeepCity_House3_EventScript_SuperRodFisherman at (4,4); Hey there, TRAINER! /  A SUPER ROD really is super! //  Say all you want, but this baby can /  catch POKéMON off the seafloor! //  What do you think? /  You want it, don't you? / You bet, you bet! /  After all, a SUPER ROD is really super! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `MossdeepCity_House3:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_MOSSDEEP_CITY warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MossdeepCity_House3:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_MOSSDEEP_CITY warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
