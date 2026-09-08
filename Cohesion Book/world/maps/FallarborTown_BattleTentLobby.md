# FallarborTown_BattleTentLobby

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/FallarborTown_BattleTentLobby/map.json) · [Scripts](../../baseline/source/data/maps/FallarborTown_BattleTentLobby/scripts.inc)

## Current map contract

`MAP_FALLARBOR_TOWN_BATTLE_TENT_LOBBY` · `LAYOUT_BATTLE_TENT_LOBBY` · `WEATHER_NONE` · `MUS_B_TOWER_RS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `FallarborTown_BattleTentLobby:object_events:001` | 6,5 | [FallarborTown_BattleTentLobby_EventScript_Attendant](../../baseline/source/data/maps/FallarborTown_BattleTentLobby/scripts.inc#L103) — FallarborTown_BattleTentLobby_EventScript_Attendant at (6,5); shared behavior FACILITY | **REVISE** · [W-C-FACILITY](../common-contracts.md#w-c-facility) · FAC-01, W-TENT-FALLARBORTOWN |
| `FallarborTown_BattleTentLobby:object_events:002` | 1,5 | [FallarborTown_BattleTentLobby_EventScript_Hiker](../../baseline/source/data/maps/FallarborTown_BattleTentLobby/scripts.inc#L226) — FallarborTown_BattleTentLobby_EventScript_Hiker at (1,5); I heard something about some tent, /  so I came to camp out. //  I didn't know that tents these days /  are so luxurious! //  Since I'm here, I may as well try /  my hand at battling! | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · FAC-01 |
| `FallarborTown_BattleTentLobby:object_events:003` | 12,6 | [FallarborTown_BattleTentLobby_EventScript_LittleBoy](../../baseline/source/data/maps/FallarborTown_BattleTentLobby/scripts.inc#L230) — FallarborTown_BattleTentLobby_EventScript_LittleBoy at (12,6); Fufufufufu. //  I'm going to make everyone think /  I'm just a kid and let them play down. //  Then, I'll shock them and grab /  the title! | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · FAC-01 |
| `FallarborTown_BattleTentLobby:object_events:004` | 10,9 | [FallarborTown_BattleTentLobby_EventScript_Lass](../../baseline/source/data/maps/FallarborTown_BattleTentLobby/scripts.inc#L234) — FallarborTown_BattleTentLobby_EventScript_Lass at (10,9); You know how BATTLE TENTS offer /  different events in different towns? //  My favorite is definitely the BATTLE /  TENT in FALLARBOR TOWN. //  I think it's fantastic how TRAINERS /  try to win with all their faith in /  their POKéMON. | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · FAC-01, W-TENT-FLAVOR |
| `FallarborTown_BattleTentLobby:object_events:005` | 0,7 | [FallarborTown_BattleTentLobby_EventScript_Scott](../../baseline/source/data/maps/FallarborTown_BattleTentLobby/scripts.inc#L238) — FallarborTown_BattleTentLobby_EventScript_Scott at (0,7); SCOTT: Hi, {PLAYER}{KUN}! /  So you came out to this BATTLE TENT! //  The people in these parts tend to be /  easygoing and laid-back. //  But, you see, what I'm looking for are /  people with… //  How should I say this? //  Someone bursting with the desire /  and the drive to win. //  If there were a TRAINER like that, /  I'd immediately… //  Whoops! Never mind! /  Keep working at it! / SCOTT: Instead of wasting your /  time with the likes of me, why not /  make a challenge? | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story) · FAC-01 |
| `FallarborTown_BattleTentLobby:bg_events:001` | 4,5 | [FallarborTown_BattleTentLobby_EventScript_RulesBoard](../../baseline/source/data/maps/FallarborTown_BattleTentLobby/scripts.inc#L253) — FallarborTown_BattleTentLobby_EventScript_RulesBoard at (4,5); shared behavior BACKGROUND | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · FAC-01, W-TENT-FALLARBORTOWN |
| `FallarborTown_BattleTentLobby:warp_events:001` | 6,9 | Warp from (6,9, elevation 0) to MAP_FALLARBOR_TOWN warp 1. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · FAC-01 |
| `FallarborTown_BattleTentLobby:warp_events:002` | 7,9 | Warp from (7,9, elevation 0) to MAP_FALLARBOR_TOWN warp 1. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · FAC-01 |
| `FallarborTown_BattleTentLobby:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [FallarborTown_BattleTentLobby_OnFrame](../../baseline/source/data/maps/FallarborTown_BattleTentLobby/scripts.inc#L15) — MAP_SCRIPT_ON_FRAME_TABLE calls FallarborTown_BattleTentLobby_OnFrame. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · FAC-01 |
| `FallarborTown_BattleTentLobby:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [FallarborTown_BattleTentLobby_OnWarp](../../baseline/source/data/maps/FallarborTown_BattleTentLobby/scripts.inc#L6) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls FallarborTown_BattleTentLobby_OnWarp. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · FAC-01 |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
