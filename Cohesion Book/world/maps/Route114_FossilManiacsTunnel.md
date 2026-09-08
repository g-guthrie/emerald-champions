# Route114_FossilManiacsTunnel

**REVISE.** Keep the underpass restoration after Mirage Tower loss and current complete-fossil support. Correct the stale Landorus silhouette below into an accurate ruins connection clue.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/Route114_FossilManiacsTunnel/map.json) · [Scripts](../../baseline/source/data/maps/Route114_FossilManiacsTunnel/scripts.inc)

## Current map contract

`MAP_ROUTE114_FOSSIL_MANIACS_TUNNEL` · `LAYOUT_ROUTE114_FOSSIL_MANIACS_TUNNEL` · `WEATHER_NONE` · `MUS_FALLARBOR`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route114_FossilManiacsTunnel:object_events:001` | 5,3 | [Route114_FossilManiacsTunnel_EventScript_FossilManiac](../../baseline/source/data/maps/Route114_FossilManiacsTunnel/scripts.inc#L37) — Route114_FossilManiacsTunnel_EventScript_FossilManiac at (5,3); I'm the FOSSIL MANIAC… /  I'm a nice guy who loves FOSSILS… //  Do you want a FOSSIL? //  But the FOSSILS around these parts all /  belong to me… None for you… //  If you can't bear to go without /  a FOSSIL, look in a desert where there /  are boulders and sand that may hide /  FOSSILS… / You found a FOSSIL, didn't you? /  That's so nice… It's so dreamy… //  DEVON's researcher can revive complete /  fossils from every ancient era. //  The machine handles one at a time, /  but you may return with every kind. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route114_FossilManiacsTunnel:coord_events:001` | 5,4 | [Route114_FossilManiacsTunnel_EventScript_ManiacMentionCaveIn](../../baseline/source/data/maps/Route114_FossilManiacsTunnel/scripts.inc#L27) — Coordinate trigger at (5,4); VAR_FOSSIL_MANIAC_STATE == 1 invokes Route114_FossilManiacsTunnel_EventScript_ManiacMentionCaveIn. | **REPAIR** · [W-C-COORD](../common-contracts.md#w-c-coord) · W-SIGN-LOCAL |
| `Route114_FossilManiacsTunnel:coord_events:002` | 6,4 | [Route114_FossilManiacsTunnel_EventScript_ManiacMentionCaveIn](../../baseline/source/data/maps/Route114_FossilManiacsTunnel/scripts.inc#L27) — Coordinate trigger at (6,4); VAR_FOSSIL_MANIAC_STATE == 1 invokes Route114_FossilManiacsTunnel_EventScript_ManiacMentionCaveIn. | **REPAIR** · [W-C-COORD](../common-contracts.md#w-c-coord) · W-SIGN-LOCAL |
| `Route114_FossilManiacsTunnel:warp_events:001` | 6,25 | Warp from (6,25, elevation 3) to MAP_ROUTE114_FOSSIL_MANIACS_HOUSE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route114_FossilManiacsTunnel:warp_events:002` | 7,25 | Warp from (7,25, elevation 3) to MAP_ROUTE114_FOSSIL_MANIACS_HOUSE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route114_FossilManiacsTunnel:warp_events:003` | 6,2 | Warp from (6,2, elevation 0) to MAP_DESERT_UNDERPASS warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route114_FossilManiacsTunnel:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route114_FossilManiacsTunnel_OnTransition](../../baseline/source/data/maps/Route114_FossilManiacsTunnel/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls Route114_FossilManiacsTunnel_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route114_FossilManiacsTunnel:map_scripts:002` | MAP_SCRIPT_ON_LOAD | [Route114_FossilManiacsTunnel_OnLoad](../../baseline/source/data/maps/Route114_FossilManiacsTunnel/scripts.inc#L15) — MAP_SCRIPT_ON_LOAD calls Route114_FossilManiacsTunnel_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
