# SlateportCity_BattleTentLobby

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/SlateportCity_BattleTentLobby/map.json) · [Scripts](../../baseline/source/data/maps/SlateportCity_BattleTentLobby/scripts.inc)

## Current map contract

`MAP_SLATEPORT_CITY_BATTLE_TENT_LOBBY` · `LAYOUT_BATTLE_TENT_LOBBY` · `WEATHER_NONE` · `MUS_B_TOWER_RS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SlateportCity_BattleTentLobby:object_events:001` | 6,5 | [SlateportCity_BattleTentLobby_EventScript_Attendant](../../baseline/source/data/maps/SlateportCity_BattleTentLobby/scripts.inc#L90) — SlateportCity_BattleTentLobby_EventScript_Attendant at (6,5); shared behavior FACILITY | **REVISE** · [W-C-FACILITY](../common-contracts.md#w-c-facility) · FAC-01, W-TENT-SLATEPORTCITY |
| `SlateportCity_BattleTentLobby:object_events:002` | 1,5 | [SlateportCity_BattleTentLobby_EventScript_PrismScaleGiver](../../baseline/source/data/maps/SlateportCity_BattleTentLobby/scripts.inc#L197) — SlateportCity_BattleTentLobby_EventScript_PrismScaleGiver at (1,5); So, like, I couldn't find myself any /  POKéMON that were, like, for me. //  So, I figured, like, hey, I should file /  a complaint to the guy there? //  And he wouldn't hear me out, like, hey! /  So, like, total bummer, man! //  Hey, like, you! Zip it, you know? /  Just, you know, take this! / A Prism Scale evolves Feebas. /  Use it from the Bag when ready. | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story) · FAC-01, W-TENT-FLAVOR |
| `SlateportCity_BattleTentLobby:object_events:003` | 3,7 | [SlateportCity_BattleTentLobby_EventScript_Man](../../baseline/source/data/maps/SlateportCity_BattleTentLobby/scripts.inc#L214) — SlateportCity_BattleTentLobby_EventScript_Man at (3,7); I don't really like BUG POKéMON, /  but maybe I'll try using some for /  a change of pace. //  Who knows, I might even get to like /  them! | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · FAC-01 |
| `SlateportCity_BattleTentLobby:object_events:004` | 1,8 | [SlateportCity_BattleTentLobby_EventScript_Girl](../../baseline/source/data/maps/SlateportCity_BattleTentLobby/scripts.inc#L218) — SlateportCity_BattleTentLobby_EventScript_Girl at (1,8); You can battle all you want here even /  if you don't have any tough POKéMON. | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · FAC-01, W-TENT-FLAVOR |
| `SlateportCity_BattleTentLobby:object_events:005` | 11,8 | [SlateportCity_BattleTentLobby_EventScript_Woman](../../baseline/source/data/maps/SlateportCity_BattleTentLobby/scripts.inc#L222) — SlateportCity_BattleTentLobby_EventScript_Woman at (11,8); Wouldn't it be nice if they had more of /  a selection? | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · FAC-01, W-TENT-FLAVOR |
| `SlateportCity_BattleTentLobby:bg_events:001` | 4,5 | [SlateportCity_BattleTentLobby_EventScript_RulesBoard](../../baseline/source/data/maps/SlateportCity_BattleTentLobby/scripts.inc#L226) — SlateportCity_BattleTentLobby_EventScript_RulesBoard at (4,5); shared behavior BACKGROUND | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · FAC-01, W-TENT-SLATEPORTCITY |
| `SlateportCity_BattleTentLobby:warp_events:001` | 6,9 | Warp from (6,9, elevation 0) to MAP_SLATEPORT_CITY warp 3. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · FAC-01 |
| `SlateportCity_BattleTentLobby:warp_events:002` | 7,9 | Warp from (7,9, elevation 0) to MAP_SLATEPORT_CITY warp 3. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · FAC-01 |
| `SlateportCity_BattleTentLobby:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [SlateportCity_BattleTentLobby_OnFrame](../../baseline/source/data/maps/SlateportCity_BattleTentLobby/scripts.inc#L15) — MAP_SCRIPT_ON_FRAME_TABLE calls SlateportCity_BattleTentLobby_OnFrame. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · FAC-01 |
| `SlateportCity_BattleTentLobby:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [SlateportCity_BattleTentLobby_OnWarp](../../baseline/source/data/maps/SlateportCity_BattleTentLobby/scripts.inc#L6) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls SlateportCity_BattleTentLobby_OnWarp. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · FAC-01 |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
