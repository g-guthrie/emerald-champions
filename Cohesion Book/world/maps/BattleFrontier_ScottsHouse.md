# BattleFrontier_ScottsHouse

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_ScottsHouse/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_ScottsHouse/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_SCOTTS_HOUSE` · `LAYOUT_BATTLE_FRONTIER_SCOTTS_HOUSE` · `WEATHER_NONE` · `MUS_B_TOWER_RS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_ScottsHouse:object_events:001` | 2,3 | [BattleFrontier_ScottsHouse_EventScript_Scott](../../baseline/source/data/maps/BattleFrontier_ScottsHouse/scripts.inc#L4) — BattleFrontier_ScottsHouse_EventScript_Scott at (2,3); SCOTT: Well, hello and welcome! /  Heheh… Sorry about the cramped space. //  Anyway, {PLAYER}{KUN}, let me formally /  welcome you to the BATTLE FRONTIER. //  This is my dream come true. /  It took me years and years, but I've /  finally given shape to my dream. / On reflection, it was a terribly long /  journey… //  I left home alone on a quest to find /  strong TRAINERS. //  No one can imagine how much effort /  or time it took to make this real. | **REPAIR** · [W-C-STORY](../common-contracts.md#w-c-story) · W-FRONTIER-GUIDES |
| `BattleFrontier_ScottsHouse:warp_events:001` | 2,7 | Warp from (2,7, elevation 3) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_ScottsHouse:warp_events:002` | 3,7 | Warp from (3,7, elevation 3) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
