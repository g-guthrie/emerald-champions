# CaveOfOrigin_B1F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/CaveOfOrigin_B1F/map.json) · [Scripts](../../baseline/source/data/maps/CaveOfOrigin_B1F/scripts.inc)

## Current map contract

`MAP_CAVE_OF_ORIGIN_B1F` · `LAYOUT_CAVE_OF_ORIGIN_B1F` · `WEATHER_FOG_HORIZONTAL` · `MUS_NONE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `CaveOfOrigin_B1F:object_events:001` | 9,13 | [CaveOfOrigin_B1F_EventScript_Wallace](../../baseline/source/data/maps/CaveOfOrigin_B1F/scripts.inc#L4) — CaveOfOrigin_B1F_EventScript_Wallace at (9,13); Ah, so you are {PLAYER}{KUN}. STEVEN /  told me how you read the SIGNS. //  My name is WALLACE. I led SOOTOPOLIS's /  GYM until this, and left it to JUAN. //  … … … … … … /  … … … … … … //  GROUDON and KYOGRE are not enemies. The /  network amplified both until neither /  could hear the other. //  The oldest tale says a third power once /  reminded them of the whole: RAYQUAZA. //  It is the one SIGN no one alive has /  read. I do not know where it sleeps… / WALLACE: {PLAYER}{KUN}, you have walked /  more of HOENN than any of us. //  Do you know where RAYQUAZA is now? | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `CaveOfOrigin_B1F:object_events:002` | 12,4 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (12,4); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `CaveOfOrigin_B1F:warp_events:001` | 9,3 | Warp from (9,3, elevation 3) to MAP_CAVE_OF_ORIGIN_1F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
