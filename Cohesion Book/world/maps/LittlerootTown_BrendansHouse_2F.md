# LittlerootTown_BrendansHouse_2F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/LittlerootTown_BrendansHouse_2F/map.json) · [Scripts](../../baseline/source/data/maps/LittlerootTown_BrendansHouse_2F/scripts.inc)

## Current map contract

`MAP_LITTLEROOT_TOWN_BRENDANS_HOUSE_2F` · `LAYOUT_LITTLEROOT_TOWN_BRENDANS_HOUSE_2F` · `WEATHER_NONE` · `MUS_LITTLEROOT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LittlerootTown_BrendansHouse_2F:object_events:001` | 7,1 | [RivalsHouse_2F_EventScript_Rival](../../baseline/source/data/maps/LittlerootTown_MaysHouse_2F/scripts.inc#L244) — RivalsHouse_2F_EventScript_Rival at (7,1); shared behavior STORY | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:object_events:002` | 0,0 | Passive/staged OBJ_EVENT_GFX_VAR_0 at (0,0); visibility flag FLAG_DECORATION_1; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:object_events:003` | 0,1 | Passive/staged OBJ_EVENT_GFX_VAR_1 at (0,1); visibility flag FLAG_DECORATION_2; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:object_events:004` | 0,2 | Passive/staged OBJ_EVENT_GFX_VAR_2 at (0,2); visibility flag FLAG_DECORATION_3; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:object_events:005` | 0,3 | Passive/staged OBJ_EVENT_GFX_VAR_3 at (0,3); visibility flag FLAG_DECORATION_4; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:object_events:006` | 0,4 | Passive/staged OBJ_EVENT_GFX_VAR_4 at (0,4); visibility flag FLAG_DECORATION_5; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:object_events:007` | 0,5 | Passive/staged OBJ_EVENT_GFX_VAR_5 at (0,5); visibility flag FLAG_DECORATION_6; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:object_events:008` | 1,0 | Passive/staged OBJ_EVENT_GFX_VAR_6 at (1,0); visibility flag FLAG_DECORATION_7; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:object_events:009` | 1,1 | Passive/staged OBJ_EVENT_GFX_VAR_7 at (1,1); visibility flag FLAG_DECORATION_8; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:object_events:010` | 1,2 | Passive/staged OBJ_EVENT_GFX_VAR_8 at (1,2); visibility flag FLAG_DECORATION_9; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:object_events:011` | 1,3 | Passive/staged OBJ_EVENT_GFX_VAR_9 at (1,3); visibility flag FLAG_DECORATION_10; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:object_events:012` | 1,4 | Passive/staged OBJ_EVENT_GFX_VAR_A at (1,4); visibility flag FLAG_DECORATION_11; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:object_events:013` | 1,5 | Passive/staged OBJ_EVENT_GFX_VAR_B at (1,5); visibility flag FLAG_DECORATION_12; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:object_events:014` | 7,1 | Passive/staged OBJ_EVENT_GFX_MOM at (7,1); visibility flag FLAG_HIDE_LITTLEROOT_TOWN_PLAYERS_BEDROOM_MOM; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:object_events:015` | 3,4 | [LittlerootTown_BrendansHouse_2F_EventScript_RivalsPokeBall](../../baseline/source/data/maps/LittlerootTown_BrendansHouse_2F/scripts.inc#L49) — LittlerootTown_BrendansHouse_2F_EventScript_RivalsPokeBall at (3,4); shared behavior STORY | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:object_events:016` | 5,5 | Passive/staged OBJ_EVENT_GFX_SWABLU_DOLL at (5,5); visibility flag FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_2F_SWABLU_DOLL; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:bg_events:001` | 0,1 | [LittlerootTown_BrendansHouse_2F_EventScript_PC](../../baseline/source/data/maps/LittlerootTown_BrendansHouse_2F/scripts.inc#L244) — LittlerootTown_BrendansHouse_2F_EventScript_PC at (0,1); shared behavior BACKGROUND | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:bg_events:002` | 1,1 | [PlayersHouse_2F_EventScript_Notebook](../../baseline/source/data/maps/LittlerootTown_BrendansHouse_2F/scripts.inc#L272) — PlayersHouse_2F_EventScript_Notebook at (1,1); {PLAYER} flipped open the notebook. //  ADVENTURE RULE NO. 1 /  Open the MENU with START. //  ADVENTURE RULE NO. 2 /  Record your progress with SAVE. //  The remaining pages are blank… | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:bg_events:003` | 5,1 | [LittlerootTown_BrendansHouse_2F_EventScript_WallClock](../../baseline/source/data/scripts/players_house.inc#L43) — LittlerootTown_BrendansHouse_2F_EventScript_WallClock at (5,1); shared behavior BACKGROUND | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:bg_events:004` | 3,1 | [PlayersHouse_2F_EventScript_GameCube](../../baseline/source/data/maps/LittlerootTown_BrendansHouse_2F/scripts.inc#L276) — PlayersHouse_2F_EventScript_GameCube at (3,1); It's a Nintendo GameCube. //  A Game Boy Advance is connected to /  serve as the Controller. | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:warp_events:001` | 7,1 | Warp from (7,1, elevation 0) to MAP_LITTLEROOT_TOWN_BRENDANS_HOUSE_1F warp 2. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [LittlerootTown_BrendansHouse_2F_OnTransition](../../baseline/source/data/maps/LittlerootTown_BrendansHouse_2F/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls LittlerootTown_BrendansHouse_2F_OnTransition. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · INTRO-01 |
| `LittlerootTown_BrendansHouse_2F:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [LittlerootTown_BrendansHouse_2F_OnWarp](../../baseline/source/data/maps/LittlerootTown_BrendansHouse_2F/scripts.inc#L40) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls LittlerootTown_BrendansHouse_2F_OnWarp. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · INTRO-01 |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
