# SlateportCity_NameRatersHouse

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/SlateportCity_NameRatersHouse/map.json) · [Scripts](../../baseline/source/data/maps/SlateportCity_NameRatersHouse/scripts.inc)

## Current map contract

`MAP_SLATEPORT_CITY_NAME_RATERS_HOUSE` · `LAYOUT_HOUSE_WITH_BED` · `WEATHER_NONE` · `MUS_SLATEPORT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SlateportCity_NameRatersHouse:object_events:001` | 5,4 | [SlateportCity_NameRatersHouse_EventScript_MegaGift_MALAMARITE](../../baseline/source/data/maps/SlateportCity_NameRatersHouse/scripts.inc#L113) — SlateportCity_NameRatersHouse_EventScript_MegaGift_MALAMARITE at (5,4); Hi, hi! I'm the NAME RATER! /  I'm the fortune-teller of names! //  I shall be pleased to rate your /  POKéMON's nickname. / Which POKéMON's nickname should /  I critique? | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `SlateportCity_NameRatersHouse:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_SLATEPORT_CITY warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SlateportCity_NameRatersHouse:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_SLATEPORT_CITY warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
