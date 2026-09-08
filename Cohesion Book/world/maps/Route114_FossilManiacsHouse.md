# Route114_FossilManiacsHouse

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/Route114_FossilManiacsHouse/map.json) · [Scripts](../../baseline/source/data/maps/Route114_FossilManiacsHouse/scripts.inc)

## Current map contract

`MAP_ROUTE114_FOSSIL_MANIACS_HOUSE` · `LAYOUT_ROUTE114_FOSSIL_MANIACS_HOUSE` · `WEATHER_NONE` · `MUS_FALLARBOR`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route114_FossilManiacsHouse:object_events:001` | 3,2 | [Route114_FossilManiacsHouse_EventScript_FossilManiacsBrother](../../baseline/source/data/maps/Route114_FossilManiacsHouse/scripts.inc#L9) — Route114_FossilManiacsHouse_EventScript_FossilManiacsBrother at (3,2); My brother dug up this Metal Coat. //  It evolves Onix or Scyther when used. / If you make a POKéMON DIG inside a /  cave, you're returned to the entrance… //  Old peat has another use: use a PEAT /  BLOCK on URSARING at night to evolve /  it into URSALUNA, anywhere in HOENN. //  The free Evolution Items archive /  supplies PEAT BLOCKS once you have /  the MEGA RING. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route114_FossilManiacsHouse:bg_events:001` | 5,3 | [Route114_FossilManiacsHouse_EventScript_RockDisplay](../../baseline/source/data/maps/Route114_FossilManiacsHouse/scripts.inc#L25) — Route114_FossilManiacsHouse_EventScript_RockDisplay at (5,3); Rocks in peculiar shapes fill /  the display case… | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route114_FossilManiacsHouse:bg_events:002` | 6,3 | [Route114_FossilManiacsHouse_EventScript_RockDisplay](../../baseline/source/data/maps/Route114_FossilManiacsHouse/scripts.inc#L25) — Route114_FossilManiacsHouse_EventScript_RockDisplay at (6,3); Rocks in peculiar shapes fill /  the display case… | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route114_FossilManiacsHouse:bg_events:003` | 7,2 | [Route114_FossilManiacsHouse_EventScript_Bookshelf](../../baseline/source/data/maps/Route114_FossilManiacsHouse/scripts.inc#L29) — Route114_FossilManiacsHouse_EventScript_Bookshelf at (7,2); THE COMPOSITION OF STRATA… /  HOW RAIN SHAPES THE LAND… /  STONES, SOIL, AND ROCK… //  It's crammed with books. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route114_FossilManiacsHouse:bg_events:004` | 8,2 | [Route114_FossilManiacsHouse_EventScript_Bookshelf](../../baseline/source/data/maps/Route114_FossilManiacsHouse/scripts.inc#L29) — Route114_FossilManiacsHouse_EventScript_Bookshelf at (8,2); THE COMPOSITION OF STRATA… /  HOW RAIN SHAPES THE LAND… /  STONES, SOIL, AND ROCK… //  It's crammed with books. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route114_FossilManiacsHouse:warp_events:001` | 4,7 | Warp from (4,7, elevation 0) to MAP_ROUTE114 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route114_FossilManiacsHouse:warp_events:002` | 5,7 | Warp from (5,7, elevation 0) to MAP_ROUTE114 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route114_FossilManiacsHouse:warp_events:003` | 4,1 | Warp from (4,1, elevation 0) to MAP_ROUTE114_FOSSIL_MANIACS_TUNNEL warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route114_FossilManiacsHouse:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route114_FossilManiacsHouse_OnTransition](../../baseline/source/data/maps/Route114_FossilManiacsHouse/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route114_FossilManiacsHouse_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
