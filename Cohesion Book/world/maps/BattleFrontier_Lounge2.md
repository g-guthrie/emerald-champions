# BattleFrontier_Lounge2

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_Lounge2/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_Lounge2/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_LOUNGE2` · `LAYOUT_BATTLE_FRONTIER_LOUNGE1` · `WEATHER_NONE` · `MUS_B_TOWER_RS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_Lounge2:object_events:001` | 8,4 | [BattleFrontier_Lounge2_EventScript_FrontierManiac](../../baseline/source/data/maps/BattleFrontier_Lounge2/scripts.inc#L10) — BattleFrontier_Lounge2_EventScript_FrontierManiac at (8,4); Howdy! When it comes to news about /  the BATTLE FRONTIER, I'm no. 1. //  You can think of me as /  the FRONTIER MANIAC. //  Just checking, but you are a TRAINER, /  isn't that right? //  I'll happily share the hottest news /  I gathered about the BATTLE FRONTIER. / Howdy! Did you swing by to grill me /  about the latest word? Oh, all right! | **REPAIR** · [W-C-STORY](../common-contracts.md#w-c-story) · W-FRONTIER-ANCILLARY |
| `BattleFrontier_Lounge2:object_events:002` | 10,4 | [BattleFrontier_Lounge2_EventScript_Maniac1](../../baseline/source/data/maps/BattleFrontier_Lounge2/scripts.inc#L93) — BattleFrontier_Lounge2_EventScript_Maniac1 at (10,4); What amazing news-gathering power! /  My mentor's like none other! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge2:object_events:003` | 10,3 | [BattleFrontier_Lounge2_EventScript_Maniac2](../../baseline/source/data/maps/BattleFrontier_Lounge2/scripts.inc#L99) — BattleFrontier_Lounge2_EventScript_Maniac2 at (10,3); What amazing powers of observation! /  My mentor's like none other! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge2:object_events:004` | 4,6 | [BattleFrontier_Lounge2_EventScript_TriathleteF](../../baseline/source/data/maps/BattleFrontier_Lounge2/scripts.inc#L111) — BattleFrontier_Lounge2_EventScript_TriathleteF at (4,6); …What is this place? /  It's scaring me… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge2:object_events:005` | 10,5 | [BattleFrontier_Lounge2_EventScript_Maniac3](../../baseline/source/data/maps/BattleFrontier_Lounge2/scripts.inc#L105) — BattleFrontier_Lounge2_EventScript_Maniac3 at (10,5); What amazing power of persuasion! /  My mentor's like none other! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge2:warp_events:001` | 1,7 | Warp from (1,7, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_Lounge2:warp_events:002` | 2,7 | Warp from (2,7, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
