# LilycoveCity_House1

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/LilycoveCity_House1/map.json) · [Scripts](../../baseline/source/data/maps/LilycoveCity_House1/scripts.inc)

## Current map contract

`MAP_LILYCOVE_CITY_HOUSE1` · `LAYOUT_HOUSE1` · `WEATHER_NONE` · `MUS_LILYCOVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LilycoveCity_House1:object_events:001` | 4,5 | [LilycoveCity_House1_EventScript_ExpertM](../../baseline/source/data/maps/LilycoveCity_House1/scripts.inc#L4) — LilycoveCity_House1_EventScript_ExpertM at (4,5); POKéMON are partners to people. /  They aren't our tools. //  Unfortunately, there are some people /  who fail to understand that… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_House1:object_events:002` | 4,4 | [LilycoveCity_House1_EventScript_Kecleon](../../baseline/source/data/maps/LilycoveCity_House1/scripts.inc#L8) — LilycoveCity_House1_EventScript_Kecleon at (4,4); KECLEON: Ruroro? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_House1:warp_events:001` | 3,8 | Warp from (3,8, elevation 0) to MAP_LILYCOVE_CITY warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_House1:warp_events:002` | 4,8 | Warp from (4,8, elevation 0) to MAP_LILYCOVE_CITY warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
