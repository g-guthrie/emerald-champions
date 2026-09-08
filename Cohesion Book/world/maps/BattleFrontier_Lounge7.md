# BattleFrontier_Lounge7

**REVISE.** Keep the veteran pair and their personality; both use the existing free specialist. No BP toll for an otherwise free legal move.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_Lounge7/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_Lounge7/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_LOUNGE7` · `LAYOUT_BATTLE_FRONTIER_LOUNGE2` · `WEATHER_NONE` · `MUS_B_TOWER_RS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_Lounge7:object_events:001` | 0,7 | [BattleFrontier_Lounge7_EventScript_Sailor](../../baseline/source/data/maps/BattleFrontier_Lounge7/scripts.inc#L296) — BattleFrontier_Lounge7_EventScript_Sailor at (0,7); Those ladies, the way they bad-mouth /  each other, you probably think that /  they don't get along. //  But if that were true, they wouldn't /  stay out here together, would they? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge7:object_events:002` | 2,5 | [BattleFrontier_Lounge7_EventScript_LeftMoveTutor](../../baseline/source/data/maps/BattleFrontier_Lounge7/scripts.inc#L5) — BattleFrontier_Lounge7_EventScript_LeftMoveTutor at (2,5); Buhahaha! //  You couldn't tell it from looking now, /  but I used to be one tough TRAINER. //  I had a reputation as the toughest /  BEAUTY around, I tell you! //  … … … … … … //  What is it now? /  You don't believe me. //  I'm not like that blowhard woman over /  there. I'm actually talented! //  Let me prove it to you. /  I can teach your POKéMON special and /  yet cute moves. //  But my lessons don't come free. /  How about paying for the moves I teach /  with a wee bit of Battle Points? / Buhahaha! //  Are you back to learn special and /  yet cute POKéMON moves? | **REPAIR** · [W-C-STORY](../common-contracts.md#w-c-story) · W-FREE-SERVICE-COHESION |
| `BattleFrontier_Lounge7:object_events:003` | 6,5 | [BattleFrontier_Lounge7_EventScript_RightMoveTutor](../../baseline/source/data/maps/BattleFrontier_Lounge7/scripts.inc#L127) — BattleFrontier_Lounge7_EventScript_RightMoveTutor at (6,5); Fine, fine, look here! /  Which move should I teach? / The move {STR_VAR_1}, is it? /  That will be {STR_VAR_2} Battle Points, okay? | **REPAIR** · [W-C-STORY](../common-contracts.md#w-c-story) · W-FREE-SERVICE-COHESION |
| `BattleFrontier_Lounge7:object_events:004` | 8,3 | [BattleFrontier_Lounge7_EventScript_Gentleman](../../baseline/source/data/maps/BattleFrontier_Lounge7/scripts.inc#L300) — BattleFrontier_Lounge7_EventScript_Gentleman at (8,3); When I was just a wee YOUNGSTER, /  those ladies were strong and beautiful. //  They were idols among us TRAINERS. //  Even now, age hasn't dulled their /  abilities. //  In fact, their POKéMON moves have /  grown even more polished. //  But… For some reason, I can't help /  but feel this… //  Time is so cruel… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge7:warp_events:001` | 4,9 | Warp from (4,9, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
