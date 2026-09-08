# BattleFrontier_Lounge3

**REVISE.** Retire new wagers on unavailable modes and settle existing obligations exactly once with BP-cap protection. Keep the social scene and no new reward economy.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_Lounge3/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_Lounge3/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_LOUNGE3` · `LAYOUT_BATTLE_FRONTIER_LOUNGE2` · `WEATHER_NONE` · `MUS_B_TOWER_RS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_Lounge3:object_events:001` | 4,6 | [BattleFrontier_Lounge3_EventScript_Gambler](../../baseline/source/data/maps/BattleFrontier_Lounge3/scripts.inc#L8) — BattleFrontier_Lounge3_EventScript_Gambler at (4,6); …What's that you want? //  Can't you see we're kind of busy here? /  Can't your business wait till later? / …Huh? /  You look to me like a tough TRAINER. //  Heheh… /  Listen, I have this proposition. //  We have a little group going here, /  and we play a little game with what /  goes on in the BATTLE FRONTIER. //  The rules are really simple. //  First, we pick one of the facilities /  in the BATTLE FRONTIER. //  Then, we each pick a different TRAINER /  who's taking that facility's challenge, /  and bet with our Battle Points. //  The guy who bet on the TRAINER with /  the best record takes all the Battle /  Points in the pool. //  Sounds simple, huh? /  So, anyway… | **REPAIR** · [W-C-STORY](../common-contracts.md#w-c-story) · W-GAMBLER-RETIRE |
| `BattleFrontier_Lounge3:object_events:002` | 4,4 | [BattleFrontier_Lounge3_EventScript_FatMan](../../baseline/source/data/maps/BattleFrontier_Lounge3/scripts.inc#L186) — BattleFrontier_Lounge3_EventScript_FatMan at (4,4); That TRAINER… //  He's good, but he gets rattled too /  easily to survive the BATTLE DOME… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge3:object_events:003` | 3,5 | [BattleFrontier_Lounge3_EventScript_Woman](../../baseline/source/data/maps/BattleFrontier_Lounge3/scripts.inc#L172) — BattleFrontier_Lounge3_EventScript_Woman at (3,5); I backed the wrong TRAINER again! //  Maybe I should be battling normally /  like everyone else… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge3:object_events:004` | 5,5 | [BattleFrontier_Lounge3_EventScript_PokefanF](../../baseline/source/data/maps/BattleFrontier_Lounge3/scripts.inc#L179) — BattleFrontier_Lounge3_EventScript_PokefanF at (5,5); Giggle! /  I know a winner when I see one! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge3:object_events:005` | 0,6 | [BattleFrontier_Lounge3_EventScript_Man](../../baseline/source/data/maps/BattleFrontier_Lounge3/scripts.inc#L168) — BattleFrontier_Lounge3_EventScript_Man at (0,6); Those TRAINERS… /  What are they doing? /  They should be taking challenges. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Lounge3:warp_events:001` | 4,9 | Warp from (4,9, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_EAST warp 9. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
