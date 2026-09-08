# LilycoveCity_CoveLilyMotel_1F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/LilycoveCity_CoveLilyMotel_1F/map.json) · [Scripts](../../baseline/source/data/maps/LilycoveCity_CoveLilyMotel_1F/scripts.inc)

## Current map contract

`MAP_LILYCOVE_CITY_COVE_LILY_MOTEL_1F` · `LAYOUT_LILYCOVE_CITY_COVE_LILY_MOTEL_1F` · `WEATHER_NONE` · `MUS_LILYCOVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LilycoveCity_CoveLilyMotel_1F:object_events:001` | 10,3 | [LilycoveCity_CoveLilyMotel_1F_EventScript_MotelOwner](../../baseline/source/data/maps/LilycoveCity_CoveLilyMotel_1F/scripts.inc#L4) — LilycoveCity_CoveLilyMotel_1F_EventScript_MotelOwner at (10,3); Hm, so they doubled the guests by /  using POKéMON as attractions? //  Hm, well, maybe I should make a cute /  POKéMON our inn's mascot. //  I wonder if that will attract more /  guests to stay with us? / Oh, sorry, sorry! /  I was too involved in watching TV! //  Since that TEAM AQUA came to town, /  the tourists have been staying away. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `LilycoveCity_CoveLilyMotel_1F:coord_events:001` | 10,2 | [LilycoveCity_CoveLilyMotel_1F_EventScript_BlockingTV](../../baseline/source/data/maps/LilycoveCity_CoveLilyMotel_1F/scripts.inc#L40) — Coordinate trigger at (10,2); VAR_TEMP_1 == 0 invokes LilycoveCity_CoveLilyMotel_1F_EventScript_BlockingTV. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `LilycoveCity_CoveLilyMotel_1F:warp_events:001` | 5,8 | Warp from (5,8, elevation 0) to MAP_LILYCOVE_CITY warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_CoveLilyMotel_1F:warp_events:002` | 6,8 | Warp from (6,8, elevation 0) to MAP_LILYCOVE_CITY warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_CoveLilyMotel_1F:warp_events:003` | 2,1 | Warp from (2,1, elevation 0) to MAP_LILYCOVE_CITY_COVE_LILY_MOTEL_2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
