# MeteorFalls_B1F_1R

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/MeteorFalls_B1F_1R/map.json) · [Scripts](../../baseline/source/data/maps/MeteorFalls_B1F_1R/scripts.inc)

## Current map contract

`MAP_METEOR_FALLS_B1F_1R` · `LAYOUT_METEOR_FALLS_B1F_1R` · `WEATHER_NONE` · `MUS_CAVE_OF_ORIGIN`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MeteorFalls_B1F_1R:object_events:001` | 3,9 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (3,9); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `MeteorFalls_B1F_1R:warp_events:001` | 5,6 | Warp from (5,6, elevation 4) to MAP_METEOR_FALLS_1F_2R warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MeteorFalls_B1F_1R:warp_events:002` | 7,11 | Warp from (7,11, elevation 5) to MAP_METEOR_FALLS_1F_2R warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MeteorFalls_B1F_1R:warp_events:003` | 18,15 | Warp from (18,15, elevation 4) to MAP_METEOR_FALLS_1F_2R warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MeteorFalls_B1F_1R:warp_events:004` | 17,3 | Warp from (17,3, elevation 3) to MAP_METEOR_FALLS_B1F_2R warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MeteorFalls_B1F_1R:warp_events:005` | 3,23 | Warp from (3,23, elevation 5) to MAP_METEOR_FALLS_1F_1R warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MeteorFalls_B1F_1R:warp_events:006` | 20,36 | Warp from (20,36, elevation 3) to MAP_METEOR_FALLS_1F_1R warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
