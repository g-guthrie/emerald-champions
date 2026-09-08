# VerdanturfTown_BattleTentLobby

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/VerdanturfTown_BattleTentLobby/map.json) · [Scripts](../../baseline/source/data/maps/VerdanturfTown_BattleTentLobby/scripts.inc)

## Current map contract

`MAP_VERDANTURF_TOWN_BATTLE_TENT_LOBBY` · `LAYOUT_BATTLE_TENT_LOBBY` · `WEATHER_NONE` · `MUS_B_TOWER_RS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `VerdanturfTown_BattleTentLobby:object_events:001` | 6,5 | [VerdanturfTown_BattleTentLobby_EventScript_Attendant](../../baseline/source/data/maps/VerdanturfTown_BattleTentLobby/scripts.inc#L104) — VerdanturfTown_BattleTentLobby_EventScript_Attendant at (6,5); shared behavior FACILITY | **REVISE** · [W-C-FACILITY](../common-contracts.md#w-c-facility) · FAC-01, W-TENT-VERDANTURFTOWN |
| `VerdanturfTown_BattleTentLobby:object_events:002` | 0,5 | [VerdanturfTown_BattleTentLobby_EventScript_AttractGiver](../../baseline/source/data/maps/VerdanturfTown_BattleTentLobby/scripts.inc#L222) — VerdanturfTown_BattleTentLobby_EventScript_AttractGiver at (0,5); My feelings toward my POKéMON… /  The attraction runs deep… //  Oh, hi! A bond like yours deserves /  this SHINY STONE. / My feelings toward my POKéMON… /  I'm sure the attraction is mutual! //  They battle exactly the way I want /  them to! | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story) · FAC-01 |
| `VerdanturfTown_BattleTentLobby:object_events:003` | 9,7 | [VerdanturfTown_BattleTentLobby_EventScript_Boy1](../../baseline/source/data/maps/VerdanturfTown_BattleTentLobby/scripts.inc#L239) — VerdanturfTown_BattleTentLobby_EventScript_Boy1 at (9,7); What kind of moves have you taught /  your POKéMON? //  I think you would give yourself /  an advantage if they knew how to /  heal or protect themselves. | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · FAC-01 |
| `VerdanturfTown_BattleTentLobby:object_events:004` | 1,8 | [VerdanturfTown_BattleTentLobby_EventScript_Boy2](../../baseline/source/data/maps/VerdanturfTown_BattleTentLobby/scripts.inc#L243) — VerdanturfTown_BattleTentLobby_EventScript_Boy2 at (1,8); If it doesn't like a certain move, /  a POKéMON will be reluctant to use it. //  It doesn't matter how strong it is, /  either. //  For example, a POKéMON with a GENTLE /  nature probably won't enjoy hurting /  its opponents. //  If it can't seem to live up to its /  potential, it's probably failing at /  using a disliked move against its will. | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · FAC-01, W-TENT-FLAVOR |
| `VerdanturfTown_BattleTentLobby:object_events:005` | 12,6 | [VerdanturfTown_BattleTentLobby_EventScript_Scott](../../baseline/source/data/maps/VerdanturfTown_BattleTentLobby/scripts.inc#L249) — VerdanturfTown_BattleTentLobby_EventScript_Scott at (12,6); SCOTT: Hey there, {PLAYER}{KUN}! /  I thought I might see you here. //  A BATTLE TENT's a place where /  you can meet tough TRAINERS. //  It doesn't matter what the rules are, /  or how battles are waged, either. //  {PLAYER}{KUN}, I expect you to do /  the best you can! / SCOTT: I visit here regularly in hopes /  of seeing tough TRAINERS in action /  in whatever the situation. | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story) · FAC-01 |
| `VerdanturfTown_BattleTentLobby:object_events:006` | 2,8 | [VerdanturfTown_BattleTentLobby_EventScript_LittleBoy](../../baseline/source/data/maps/VerdanturfTown_BattleTentLobby/scripts.inc#L264) — VerdanturfTown_BattleTentLobby_EventScript_LittleBoy at (2,8); My big sister is gentle usually. /  But when she gets angry, /  she's really, really scary! //  I bet a gentle POKéMON will be scary /  if it gets angry! | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · FAC-01 |
| `VerdanturfTown_BattleTentLobby:bg_events:001` | 4,5 | [VerdanturfTown_BattleTentLobby_EventScript_RulesBoard](../../baseline/source/data/maps/VerdanturfTown_BattleTentLobby/scripts.inc#L270) — VerdanturfTown_BattleTentLobby_EventScript_RulesBoard at (4,5); shared behavior BACKGROUND | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · FAC-01, W-TENT-VERDANTURFTOWN |
| `VerdanturfTown_BattleTentLobby:warp_events:001` | 6,9 | Warp from (6,9, elevation 0) to MAP_VERDANTURF_TOWN warp 0. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · FAC-01 |
| `VerdanturfTown_BattleTentLobby:warp_events:002` | 7,9 | Warp from (7,9, elevation 0) to MAP_VERDANTURF_TOWN warp 0. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · FAC-01 |
| `VerdanturfTown_BattleTentLobby:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [VerdanturfTown_BattleTentLobby_OnFrame](../../baseline/source/data/maps/VerdanturfTown_BattleTentLobby/scripts.inc#L15) — MAP_SCRIPT_ON_FRAME_TABLE calls VerdanturfTown_BattleTentLobby_OnFrame. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · FAC-01 |
| `VerdanturfTown_BattleTentLobby:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [VerdanturfTown_BattleTentLobby_OnWarp](../../baseline/source/data/maps/VerdanturfTown_BattleTentLobby/scripts.inc#L6) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls VerdanturfTown_BattleTentLobby_OnWarp. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · FAC-01 |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
