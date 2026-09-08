# BattleFrontier_Lounge5

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_Lounge5/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_Lounge5/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_LOUNGE5` · `LAYOUT_BATTLE_FRONTIER_LOUNGE1` · `WEATHER_NONE` · `MUS_B_TOWER_RS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_Lounge5:object_events:001` | 12,4 | [BattleFrontier_Lounge5_EventScript_NatureGirl](../../baseline/source/data/maps/BattleFrontier_Lounge5/scripts.inc#L4) — BattleFrontier_Lounge5_EventScript_NatureGirl at (12,4); Ehehe! /  I can tell what POKéMON are thinking! //  Please! /  Can I see your POKéMON? / Boo! /  Cheapie! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `BattleFrontier_Lounge5:object_events:002` | 0,4 | [BattleFrontier_Lounge5_EventScript_Gentleman](../../baseline/source/data/maps/BattleFrontier_Lounge5/scripts.inc#L32) — BattleFrontier_Lounge5_EventScript_Gentleman at (0,4); How charming! /  That little lady claims she can /  understand POKéMON! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge5:object_events:003` | 6,5 | [BattleFrontier_Lounge5_EventScript_BlackBelt](../../baseline/source/data/maps/BattleFrontier_Lounge5/scripts.inc#L36) — BattleFrontier_Lounge5_EventScript_BlackBelt at (6,5); I have this feeling that the little girl /  is saying something profound. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge5:object_events:004` | 11,7 | [BattleFrontier_Lounge5_EventScript_LittleBoy](../../baseline/source/data/maps/BattleFrontier_Lounge5/scripts.inc#L40) — BattleFrontier_Lounge5_EventScript_LittleBoy at (11,7); I know something! //  That little girl plays at the red house /  a lot! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge5:warp_events:001` | 1,7 | Warp from (1,7, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_EAST warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_Lounge5:warp_events:002` | 2,7 | Warp from (2,7, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_EAST warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
