# SlateportCity_SternsShipyard_2F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/SlateportCity_SternsShipyard_2F/map.json) · [Scripts](../../baseline/source/data/maps/SlateportCity_SternsShipyard_2F/scripts.inc)

## Current map contract

`MAP_SLATEPORT_CITY_STERNS_SHIPYARD_2F` · `LAYOUT_SLATEPORT_CITY_STERNS_SHIPYARD_2F` · `WEATHER_NONE` · `MUS_SLATEPORT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SlateportCity_SternsShipyard_2F:object_events:001` | 10,7 | Passive/staged OBJ_EVENT_GFX_SCIENTIST_1 at (10,7); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `SlateportCity_SternsShipyard_2F:object_events:002` | 8,4 | [SlateportCity_SternsShipyard_2F_EventScript_Scientist1](../../baseline/source/data/maps/SlateportCity_SternsShipyard_2F/scripts.inc#L4) — SlateportCity_SternsShipyard_2F_EventScript_Scientist1 at (8,4); Designing a large ship is more like /  making a big building than putting /  together a transportation vehicle. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SlateportCity_SternsShipyard_2F:object_events:003` | 0,9 | [SlateportCity_SternsShipyard_2F_EventScript_Scientist2](../../baseline/source/data/maps/SlateportCity_SternsShipyard_2F/scripts.inc#L8) — SlateportCity_SternsShipyard_2F_EventScript_Scientist2 at (0,9); Don't you think it's strange that /  a ship made of heavy iron floats? //  It floats because of a principle /  called buoyancy. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SlateportCity_SternsShipyard_2F:warp_events:001` | 3,1 | Warp from (3,1, elevation 0) to MAP_SLATEPORT_CITY_STERNS_SHIPYARD_1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
