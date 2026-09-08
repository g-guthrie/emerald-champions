# BattleFrontier_Lounge9

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_Lounge9/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_Lounge9/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_LOUNGE9` · `LAYOUT_BATTLE_FRONTIER_LOUNGE2` · `WEATHER_NONE` · `MUS_B_TOWER_RS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_Lounge9:object_events:001` | 6,3 | Passive/staged OBJ_EVENT_GFX_GIRL_2 at (6,3); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_Lounge9:warp_events:001` | 1,9 | Warp from (1,9, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_EAST warp 11. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_Lounge9:warp_events:002` | 2,9 | Warp from (2,9, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_EAST warp 11. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
