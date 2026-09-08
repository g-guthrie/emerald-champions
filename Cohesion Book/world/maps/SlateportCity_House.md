# SlateportCity_House

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/SlateportCity_House/map.json) · [Scripts](../../baseline/source/data/maps/SlateportCity_House/scripts.inc)

## Current map contract

`MAP_SLATEPORT_CITY_HOUSE` · `LAYOUT_HOUSE2` · `WEATHER_NONE` · `MUS_SLATEPORT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SlateportCity_House:object_events:001` | 4,4 | [SlateportCity_House_EventScript_PokefanM](../../baseline/source/data/maps/SlateportCity_House/scripts.inc#L4) — SlateportCity_House_EventScript_PokefanM at (4,4); My POKéMON has a HASTY nature. //  It has higher SPEED compared to /  my other POKéMON. //  Maybe their nature has something to /  do with the stat gains of POKéMON. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SlateportCity_House:object_events:002` | 7,4 | [SlateportCity_House_EventScript_Girl](../../baseline/source/data/maps/SlateportCity_House/scripts.inc#L8) — SlateportCity_House_EventScript_Girl at (7,4); You're a TRAINER, aren't you? //  Since you came to SLATEPORT CITY, /  you must be going to the BATTLE TENT. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SlateportCity_House:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_SLATEPORT_CITY warp 10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SlateportCity_House:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_SLATEPORT_CITY warp 10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
