# FortreeCity_Gym

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/FortreeCity_Gym/map.json) · [Scripts](../../baseline/source/data/maps/FortreeCity_Gym/scripts.inc)

## Current map contract

`MAP_FORTREE_CITY_GYM` · `LAYOUT_FORTREE_CITY_GYM` · `WEATHER_NONE` · `MUS_GYM`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `FortreeCity_Gym:object_events:001` | 15,2 | [FortreeCity_Gym_EventScript_Winona](../../baseline/source/data/maps/FortreeCity_Gym/scripts.inc#L20) — FortreeCity_Gym_EventScript_Winona at (15,2); I am WINONA, FORTREE's GYM LEADER. //  The sky controls position: wind decides /  speed, wings evade Ground, snow hides /  the final horizon. //  Preserve the answer you will need after /  the weather changes. / You denied the wind without losing sight /  of the snow. Accept the FEATHER BADGE. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `FortreeCity_Gym:object_events:002` | 4,14 | [FortreeCity_Gym_EventScript_Jared](../../baseline/source/data/maps/FortreeCity_Gym/scripts.inc#L76) — FortreeCity_Gym_EventScript_Jared at (4,14); SWOOBAT's SIMPLE doubles every CALM /  MIND. NOCTOWL puts you to sleep first. //  Wake up before STORED POWER lands! / You broke the boost before it grew! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `FortreeCity_Gym:object_events:003` | 10,10 | [FortreeCity_Gym_EventScript_Flint](../../baseline/source/data/maps/FortreeCity_Gym/scripts.inc#L86) — FortreeCity_Gym_EventScript_Flint at (10,10); MULTISCALE DRAGONITE, SPEED BOOST /  YANMEGA, and a TAILWIND behind them. //  WINONA needn't see you if I win here! / WINONA, I… The wind broke! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `FortreeCity_Gym:object_events:004` | 5,17 | [FortreeCity_Gym_EventScript_Ashley](../../baseline/source/data/maps/FortreeCity_Gym/scripts.inc#L91) — FortreeCity_Gym_EventScript_Ashley at (5,17); PIDGEOT's HURRICANE never misses under /  NO GUARD. SWANNA brings the rain. //  Position, then power! / You changed the weather on me! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `FortreeCity_Gym:object_events:005` | 9,8 | [FortreeCity_Gym_EventScript_Edwardo](../../baseline/source/data/maps/FortreeCity_Gym/scripts.inc#L81) — FortreeCity_Gym_EventScript_Edwardo at (9,8); JUMPLUFF puts you to sleep, and /  HONCHKROW grows with every fall. //  Give it nothing to feed on! / You outlasted my wind… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `FortreeCity_Gym:object_events:006` | 15,21 | [FortreeCity_Gym_EventScript_GymGuide](../../baseline/source/data/maps/FortreeCity_Gym/scripts.inc#L106) — FortreeCity_Gym_EventScript_GymGuide at (15,21); Yo, how's it going, CHAMPION- /  bound {PLAYER}? //  FORTREE GYM LEADER WINONA is /  a master of FLYING-type POKéMON. //  She's waiting at the back of this GYM, /  behind the rotating doors. //  She's waiting for new challengers /  who are trying to take wing! //  Okay, go for it! / You did it! /  You've achieved liftoff! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `FortreeCity_Gym:object_events:007` | 4,23 | [FortreeCity_Gym_EventScript_Humberto](../../baseline/source/data/maps/FortreeCity_Gym/scripts.inc#L96) — FortreeCity_Gym_EventScript_Humberto at (4,23); GLISCOR heals through its own poison, /  and CORVIKNIGHT returns what you send. //  Lower our stats. I dare you! / You hit MINIOR before the shell came off! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `FortreeCity_Gym:object_events:008` | 1,10 | [FortreeCity_Gym_EventScript_Darius](../../baseline/source/data/maps/FortreeCity_Gym/scripts.inc#L101) — FortreeCity_Gym_EventScript_Darius at (1,10); TORNADUS raises the TAILWIND before /  you have taken a single turn. //  Four turns. Outlast them! / You aimed at the rod itself! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `FortreeCity_Gym:bg_events:001` | 14,21 | [FortreeCity_Gym_EventScript_LeftGymStatue](../../baseline/source/data/maps/FortreeCity_Gym/scripts.inc#L119) — FortreeCity_Gym_EventScript_LeftGymStatue at (14,21); FORTREE CITY POKéMON GYM / FORTREE CITY POKéMON GYM //  WINONA'S CERTIFIED TRAINERS: /  {PLAYER} | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `FortreeCity_Gym:bg_events:002` | 17,21 | [FortreeCity_Gym_EventScript_RightGymStatue](../../baseline/source/data/maps/FortreeCity_Gym/scripts.inc#L125) — FortreeCity_Gym_EventScript_RightGymStatue at (17,21); FORTREE CITY POKéMON GYM / FORTREE CITY POKéMON GYM //  WINONA'S CERTIFIED TRAINERS: /  {PLAYER} | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `FortreeCity_Gym:warp_events:001` | 15,24 | Warp from (15,24, elevation 0) to MAP_FORTREE_CITY warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity_Gym:warp_events:002` | 16,24 | Warp from (16,24, elevation 0) to MAP_FORTREE_CITY warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity_Gym:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [FortreeCity_Gym_OnTransition](../../baseline/source/data/maps/FortreeCity_Gym/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls FortreeCity_Gym_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `FortreeCity_Gym:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [FortreeCity_Gym_OnWarp](../../baseline/source/data/maps/FortreeCity_Gym/scripts.inc#L11) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls FortreeCity_Gym_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
