# BattleFrontier_Lounge6

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_Lounge6/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_Lounge6/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_LOUNGE6` · `LAYOUT_BATTLE_FRONTIER_LOUNGE2` · `WEATHER_NONE` · `MUS_B_TOWER_RS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_Lounge6:object_events:001` | 2,4 | [BattleFrontier_Lounge6_EventScript_Trader](../../baseline/source/data/maps/BattleFrontier_Lounge6/scripts.inc#L4) — BattleFrontier_Lounge6_EventScript_Trader at (2,4); My POKéMON is a {STR_VAR_2}. /  Do you know it? /  It was built to become anything. //  It deserves a Champion's team, and /  I could trade it with pride. //  Would you like to trade me a {STR_VAR_1} /  for my {STR_VAR_2}? / Oh, it's adorable! /  Thank you! /  I promise I'll be good to it! //  {STR_VAR_2} comes with a complete /  battle set. Give it a memory disc /  and it will become SILVALLY. | **REPAIR** · [W-C-STORY](../common-contracts.md#w-c-story) · W-FIELD-CLUES |
| `BattleFrontier_Lounge6:warp_events:001` | 4,9 | Warp from (4,9, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_EAST warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
