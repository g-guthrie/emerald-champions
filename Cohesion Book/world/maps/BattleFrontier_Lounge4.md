# BattleFrontier_Lounge4

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_Lounge4/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_Lounge4/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_LOUNGE4` · `LAYOUT_BATTLE_FRONTIER_LOUNGE2` · `WEATHER_NONE` · `MUS_B_TOWER_RS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_Lounge4:object_events:001` | 4,4 | [BattleFrontier_Lounge4_EventScript_Woman](../../baseline/source/data/maps/BattleFrontier_Lounge4/scripts.inc#L4) — BattleFrontier_Lounge4_EventScript_Woman at (4,4); I wonder if they'll be airing interviews /  with tough TRAINERS today? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge4:object_events:002` | 6,6 | [BattleFrontier_Lounge4_EventScript_Cook](../../baseline/source/data/maps/BattleFrontier_Lounge4/scripts.inc#L8) — BattleFrontier_Lounge4_EventScript_Cook at (6,6); If I opened a restaurant here, /  it'd make money for sure. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge4:object_events:003` | 0,5 | [BattleFrontier_Lounge4_EventScript_Man](../../baseline/source/data/maps/BattleFrontier_Lounge4/scripts.inc#L12) — BattleFrontier_Lounge4_EventScript_Man at (0,5); Whew… //  I need to take a breather after /  some intense battles… //  But even now, I never take a break /  from plotting strategy and combos. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge4:warp_events:001` | 4,9 | Warp from (4,9, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
