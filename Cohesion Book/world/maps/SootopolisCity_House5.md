# SootopolisCity_House5

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SootopolisCity_House5/map.json) · [Scripts](../../baseline/source/data/maps/SootopolisCity_House5/scripts.inc)

## Current map contract

`MAP_SOOTOPOLIS_CITY_HOUSE5` · `LAYOUT_SOOTOPOLIS_CITY_HOUSE2` · `WEATHER_NONE` · `MUS_SOOTOPOLIS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SootopolisCity_House5:object_events:001` | 3,3 | [SootopolisCity_House5_EventScript_Maniac](../../baseline/source/data/maps/SootopolisCity_House5/scripts.inc#L4) — SootopolisCity_House5_EventScript_Maniac at (3,3); There appears to be some connection /  between SOOTOPOLIS and MT. PYRE. //  My friends and I did some research on /  it at the lab where I used to work. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SootopolisCity_House5:object_events:002` | 6,3 | [SootopolisCity_House5_EventScript_Girl](../../baseline/source/data/maps/SootopolisCity_House5/scripts.inc#L8) — SootopolisCity_House5_EventScript_Girl at (6,3); My big brother used to study the sea. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SootopolisCity_House5:warp_events:001` | 3,6 | Warp from (3,6, elevation 0) to MAP_SOOTOPOLIS_CITY warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SootopolisCity_House5:warp_events:002` | 4,6 | Warp from (4,6, elevation 0) to MAP_SOOTOPOLIS_CITY warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
