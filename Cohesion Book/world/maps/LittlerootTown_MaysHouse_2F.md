# LittlerootTown_MaysHouse_2F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/LittlerootTown_MaysHouse_2F/map.json) · [Scripts](../../baseline/source/data/maps/LittlerootTown_MaysHouse_2F/scripts.inc)

## Current map contract

`MAP_LITTLEROOT_TOWN_MAYS_HOUSE_2F` · `LAYOUT_LITTLEROOT_TOWN_MAYS_HOUSE_2F` · `WEATHER_NONE` · `MUS_LITTLEROOT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LittlerootTown_MaysHouse_2F:object_events:001` | 1,1 | [RivalsHouse_2F_EventScript_Rival](../../baseline/source/data/maps/LittlerootTown_MaysHouse_2F/scripts.inc#L244) — RivalsHouse_2F_EventScript_Rival at (1,1); POKéMON fully restored! /  Items ready, and… / POKéMON fully restored… /  Items all packed, and… | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:object_events:002` | 0,6 | Passive/staged OBJ_EVENT_GFX_VAR_0 at (0,6); visibility flag FLAG_DECORATION_1; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:object_events:003` | 1,6 | Passive/staged OBJ_EVENT_GFX_VAR_1 at (1,6); visibility flag FLAG_DECORATION_2; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:object_events:004` | 2,6 | Passive/staged OBJ_EVENT_GFX_VAR_2 at (2,6); visibility flag FLAG_DECORATION_3; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:object_events:005` | 3,6 | Passive/staged OBJ_EVENT_GFX_VAR_3 at (3,6); visibility flag FLAG_DECORATION_4; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:object_events:006` | 4,6 | Passive/staged OBJ_EVENT_GFX_VAR_4 at (4,6); visibility flag FLAG_DECORATION_5; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:object_events:007` | 5,6 | Passive/staged OBJ_EVENT_GFX_VAR_5 at (5,6); visibility flag FLAG_DECORATION_6; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:object_events:008` | 0,7 | Passive/staged OBJ_EVENT_GFX_VAR_6 at (0,7); visibility flag FLAG_DECORATION_7; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:object_events:009` | 1,7 | Passive/staged OBJ_EVENT_GFX_VAR_7 at (1,7); visibility flag FLAG_DECORATION_8; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:object_events:010` | 2,7 | Passive/staged OBJ_EVENT_GFX_VAR_8 at (2,7); visibility flag FLAG_DECORATION_9; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:object_events:011` | 3,7 | Passive/staged OBJ_EVENT_GFX_VAR_9 at (3,7); visibility flag FLAG_DECORATION_10; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:object_events:012` | 4,7 | Passive/staged OBJ_EVENT_GFX_VAR_A at (4,7); visibility flag FLAG_DECORATION_11; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:object_events:013` | 5,7 | Passive/staged OBJ_EVENT_GFX_VAR_B at (5,7); visibility flag FLAG_DECORATION_12; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:object_events:014` | 1,1 | Passive/staged OBJ_EVENT_GFX_MOM at (1,1); visibility flag FLAG_HIDE_LITTLEROOT_TOWN_PLAYERS_BEDROOM_MOM; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:object_events:015` | 3,4 | Passive/staged OBJ_EVENT_GFX_PICHU_DOLL at (3,4); visibility flag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_2F_PICHU_DOLL; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:object_events:016` | 5,4 | [LittlerootTown_MaysHouse_2F_EventScript_RivalsPokeBall](../../baseline/source/data/maps/LittlerootTown_MaysHouse_2F/scripts.inc#L48) — LittlerootTown_MaysHouse_2F_EventScript_RivalsPokeBall at (5,4); You're {PLAYER}, right? /  I'm MAY. Welcome to LITTLEROOT! //  Dad said a GYM LEADER's kid was /  moving next door. I hoped we'd battle. //  He studies how Trainers from different /  regions build around their partners. //  For today's fieldwork, he packed all /  nine regional starter trios. //  He went to ROUTE 101, north of town. /  Go introduce yourself before he leaves! / It's {RIVAL}'s POKé BALL! //  Better leave it right where it is. | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:bg_events:001` | 5,1 | [PlayersHouse_2F_EventScript_GameCube](../../baseline/source/data/maps/LittlerootTown_BrendansHouse_2F/scripts.inc#L276) — PlayersHouse_2F_EventScript_GameCube at (5,1); shared behavior BACKGROUND | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:bg_events:002` | 7,1 | [PlayersHouse_2F_EventScript_Notebook](../../baseline/source/data/maps/LittlerootTown_BrendansHouse_2F/scripts.inc#L272) — PlayersHouse_2F_EventScript_Notebook at (7,1); shared behavior BACKGROUND | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:bg_events:003` | 3,1 | [LittlerootTown_MaysHouse_2F_EventScript_WallClock](../../baseline/source/data/scripts/players_house.inc#L49) — LittlerootTown_MaysHouse_2F_EventScript_WallClock at (3,1); shared behavior BACKGROUND | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:bg_events:004` | 8,1 | [LittlerootTown_MaysHouse_2F_EventScript_PC](../../baseline/source/data/maps/LittlerootTown_MaysHouse_2F/scripts.inc#L290) — LittlerootTown_MaysHouse_2F_EventScript_PC at (8,1); shared behavior BACKGROUND | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:warp_events:001` | 1,1 | Warp from (1,1, elevation 0) to MAP_LITTLEROOT_TOWN_MAYS_HOUSE_1F warp 2. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [LittlerootTown_MaysHouse_2F_OnTransition](../../baseline/source/data/maps/LittlerootTown_MaysHouse_2F/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls LittlerootTown_MaysHouse_2F_OnTransition. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · INTRO-01 |
| `LittlerootTown_MaysHouse_2F:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [LittlerootTown_MaysHouse_2F_OnWarp](../../baseline/source/data/maps/LittlerootTown_MaysHouse_2F/scripts.inc#L39) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls LittlerootTown_MaysHouse_2F_OnWarp. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · INTRO-01 |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
