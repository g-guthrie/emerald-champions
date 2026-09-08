# SootopolisCity_Mart

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SootopolisCity_Mart/map.json) · [Scripts](../../baseline/source/data/maps/SootopolisCity_Mart/scripts.inc)

## Current map contract

`MAP_SOOTOPOLIS_CITY_MART` · `LAYOUT_MART` · `WEATHER_NONE` · `MUS_POKE_MART`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SootopolisCity_Mart:object_events:001` | 1,3 | [SootopolisCity_Mart_EventScript_Clerk](../../baseline/source/data/maps/SootopolisCity_Mart/scripts.inc#L4) — SootopolisCity_Mart_EventScript_Clerk at (1,3); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SootopolisCity_Mart:object_events:002` | 5,5 | [SootopolisCity_Mart_EventScript_FatMan](../../baseline/source/data/maps/SootopolisCity_Mart/scripts.inc#L27) — SootopolisCity_Mart_EventScript_FatMan at (5,5); PP UP is great! //  It raises the POWER POINTS, the PP, /  of a POKéMON move. / What… /  What is happening? //  I really want to know, but it's too /  scary to go outside. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `SootopolisCity_Mart:object_events:003` | 9,5 | [SootopolisCity_Mart_EventScript_Gentleman](../../baseline/source/data/maps/SootopolisCity_Mart/scripts.inc#L41) — SootopolisCity_Mart_EventScript_Gentleman at (9,5); Do you know FULL RESTORE? //  Full restoration of HP! /  Eradication of all status problems! //  It's truly an item of your dreams! / This weather… /  Did something awaken? | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `SootopolisCity_Mart:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_SOOTOPOLIS_CITY warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SootopolisCity_Mart:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_SOOTOPOLIS_CITY warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
