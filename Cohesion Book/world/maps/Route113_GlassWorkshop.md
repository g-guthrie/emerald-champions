# Route113_GlassWorkshop

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/Route113_GlassWorkshop/map.json) · [Scripts](../../baseline/source/data/maps/Route113_GlassWorkshop/scripts.inc)

## Current map contract

`MAP_ROUTE113_GLASS_WORKSHOP` · `LAYOUT_HOUSE4` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route113_GlassWorkshop:object_events:001` | 2,3 | [Route113_GlassWorkshop_EventScript_GlassWorker](../../baseline/source/data/maps/Route113_GlassWorkshop/scripts.inc#L23) — Route113_GlassWorkshop_EventScript_GlassWorker at (2,3); This area is covered in volcanic ash, /  huff-puff! //  I'm specially gifted, huff-puff. //  I make glass out of volcanic ash /  and make items, huff-puff. //  Go collect ashes with this, huff-puff. / Just take that SOOT SACK and walk /  through piles of ash, huff-puff. //  And it will fill up with the volcanic ash, /  huff-puff. //  Once you think you've collected a good /  amount, come see me, huff-puff. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route113_GlassWorkshop:object_events:002` | 5,4 | [Route113_GlassWorkshop_EventScript_NinjaBoy](../../baseline/source/data/maps/Route113_GlassWorkshop/scripts.inc#L290) — Route113_GlassWorkshop_EventScript_NinjaBoy at (5,4); It's fun to blow a glass flute while /  my boss is talking. //  Huff-huff! Puff-puff! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route113_GlassWorkshop:warp_events:001` | 3,8 | Warp from (3,8, elevation 0) to MAP_ROUTE113 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route113_GlassWorkshop:warp_events:002` | 4,8 | Warp from (4,8, elevation 0) to MAP_ROUTE113 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route113_GlassWorkshop:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route113_GlassWorkshop_OnTransition](../../baseline/source/data/maps/Route113_GlassWorkshop/scripts.inc#L14) — MAP_SCRIPT_ON_TRANSITION calls Route113_GlassWorkshop_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
