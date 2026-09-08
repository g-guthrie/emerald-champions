# MeteorFalls_JirachisRoom

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/MeteorFalls_JirachisRoom/map.json) · [Scripts](../../baseline/source/data/maps/MeteorFalls_JirachisRoom/scripts.inc)

## Current map contract

`MAP_METEOR_FALLS_JIRACHIS_ROOM` · `LAYOUT_METEOR_FALLS_JIRACHIS_ROOM` · `WEATHER_NONE` · `MUS_CAVE_OF_ORIGIN`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MeteorFalls_JirachisRoom:object_events:001` | 7,6 | [MeteorFalls_JirachisRoom_EventScript_Jirachi](../../baseline/source/data/maps/MeteorFalls_JirachisRoom/scripts.inc#L4) — MeteorFalls_JirachisRoom_EventScript_Jirachi at (7,6); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `MeteorFalls_JirachisRoom:warp_events:001` | 7,10 | Warp from (7,10, elevation 0) to MAP_METEOR_FALLS_B1F_2R warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
