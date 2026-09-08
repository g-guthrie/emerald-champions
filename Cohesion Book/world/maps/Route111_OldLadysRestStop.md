# Route111_OldLadysRestStop

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/Route111_OldLadysRestStop/map.json) · [Scripts](../../baseline/source/data/maps/Route111_OldLadysRestStop/scripts.inc)

## Current map contract

`MAP_ROUTE111_OLD_LADYS_REST_STOP` · `LAYOUT_HOUSE3` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route111_OldLadysRestStop:object_events:001` | 6,3 | [Route111_OldLadysRestStop_EventScript_OldLady](../../baseline/source/data/maps/Route111_OldLadysRestStop/scripts.inc#L9) — Route111_OldLadysRestStop_EventScript_OldLady at (6,3); Oh, dear, dear. /  Aren't your POKéMON exhausted? //  If you'd like, rest up here. /  That's a fine idea! You should do that. / That's right. /  Take your time and rest up! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route111_OldLadysRestStop:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_ROUTE111 warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route111_OldLadysRestStop:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_ROUTE111 warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route111_OldLadysRestStop:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route111_OldLadysRestStop_OnTransition](../../baseline/source/data/maps/Route111_OldLadysRestStop/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route111_OldLadysRestStop_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
