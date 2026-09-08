# ScorchedSlab_HeatransRoom

**KEEP.** Keep Magma Stone reveal. FLAG_EC_CAUGHT_HEATRAN is a physical-presence flag also set at new game; clearing it for the initial reveal is not proof of recapture. Preserve permanent capture through the actual legendary/Pokédex ledger.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/ScorchedSlab_HeatransRoom/map.json) · [Scripts](../../baseline/source/data/maps/ScorchedSlab_HeatransRoom/scripts.inc)

## Current map contract

`MAP_SCORCHED_SLAB_HEATRANS_ROOM` · `LAYOUT_SCORCHED_SLAB_HEATRANS_ROOM` · `WEATHER_NONE` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `ScorchedSlab_HeatransRoom:object_events:001` | 10,12 | [ScorchedSlab_HeatransRoom_EventScript_Heatran](../../baseline/source/data/maps/ScorchedSlab_HeatransRoom/scripts.inc#L33) — ScorchedSlab_HeatransRoom_EventScript_Heatran at (10,12); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `ScorchedSlab_HeatransRoom:object_events:002` | 8,16 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_HEATRANITE; root Common_EventScript_FindItem; flag FLAG_EC_MEGA_REWARD_HEATRANITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `ScorchedSlab_HeatransRoom:coord_events:001` | 10,14 | [ScorchedSlab_HeatransRoom_EventScript_MagmaStone](../../baseline/source/data/maps/ScorchedSlab_HeatransRoom/scripts.inc#L4) — Coordinate trigger at (10,14); VAR_TEMP_0 == 0 invokes ScorchedSlab_HeatransRoom_EventScript_MagmaStone. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `ScorchedSlab_HeatransRoom:warp_events:001` | 10,18 | Warp from (10,18, elevation 0) to MAP_SCORCHED_SLAB_B2F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
