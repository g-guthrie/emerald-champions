# BattleFrontier_Lounge8

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_Lounge8/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_Lounge8/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_LOUNGE8` · `LAYOUT_BATTLE_FRONTIER_LOUNGE2` · `WEATHER_NONE` · `MUS_B_TOWER_RS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_Lounge8:object_events:001` | 4,5 | [BattleFrontier_Lounge8_EventScript_NinjaBoy](../../baseline/source/data/maps/BattleFrontier_Lounge8/scripts.inc#L12) — BattleFrontier_Lounge8_EventScript_NinjaBoy at (4,5); At the BATTLE TOWER, an older girl /  told me that I have a lot of talent /  for battling! //  I like POKéMON CONTESTS more! //  But I'm no good at CONTESTS! //  I guess having talent and liking /  something aren't the same. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge8:object_events:002` | 8,7 | [BattleFrontier_Lounge8_EventScript_Man](../../baseline/source/data/maps/BattleFrontier_Lounge8/scripts.inc#L4) — BattleFrontier_Lounge8_EventScript_Man at (8,7); What a TRAINER needs… //  Knowledge… /  Strategy… /  Luck… /  Guts… /  Spirit… /  Bravery… /  And ability… //  Well, I'm all set in every way! /  I'll be unstoppable at every building! //  Huh? POKéMON? /  What's that? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge8:object_events:003` | 8,4 | [BattleFrontier_Lounge8_EventScript_Woman](../../baseline/source/data/maps/BattleFrontier_Lounge8/scripts.inc#L8) — BattleFrontier_Lounge8_EventScript_Woman at (8,4); Do you know about the FRONTIER /  BRAINS? //  That's what SCOTT calls the seven /  special TRAINERS that run the seven /  facilities in the BATTLE FRONTIER. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge8:warp_events:001` | 4,9 | Warp from (4,9, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_EAST warp 10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
