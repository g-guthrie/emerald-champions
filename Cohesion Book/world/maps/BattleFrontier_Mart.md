# BattleFrontier_Mart

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_Mart/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_Mart/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_MART` · `LAYOUT_MART` · `WEATHER_NONE` · `MUS_POKE_MART`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_Mart:object_events:001` | 1,3 | [BattleFrontier_Mart_EventScript_Clerk](../../baseline/source/data/maps/BattleFrontier_Mart/scripts.inc#L4) — BattleFrontier_Mart_EventScript_Clerk at (1,3); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_Mart:object_events:002` | 5,4 | [BattleFrontier_Mart_EventScript_OldWoman](../../baseline/source/data/maps/BattleFrontier_Mart/scripts.inc#L35) — BattleFrontier_Mart_EventScript_OldWoman at (5,4); Dear, what do you think of this? /  Wouldn't these BALLS make a nice gift? //  Our grandson is always searching for /  one more teammate. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `BattleFrontier_Mart:object_events:003` | 5,5 | [Common_EventScript_EmeraldChampionsBattleVendor](../../baseline/source/data/scripts/emerald_champions.inc#L384) — Common_EventScript_EmeraldChampionsBattleVendor at (5,5); shared behavior VENDOR | **KEEP** · [W-C-VENDOR](../common-contracts.md#w-c-vendor) |
| `BattleFrontier_Mart:object_events:004` | 8,4 | [BattleFrontier_Mart_EventScript_Boy](../../baseline/source/data/maps/BattleFrontier_Mart/scripts.inc#L43) — BattleFrontier_Mart_EventScript_Boy at (8,4); A lot of the BATTLE FRONTIER's /  facilities don't allow the use of items /  during battles. //  That rule makes things tougher than /  they already are! | **REPAIR** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · W-FRONTIER-RESIDENTS |
| `BattleFrontier_Mart:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_Mart:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
