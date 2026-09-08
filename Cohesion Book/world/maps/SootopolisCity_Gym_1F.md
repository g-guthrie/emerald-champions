# SootopolisCity_Gym_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SootopolisCity_Gym_1F/map.json) · [Scripts](../../baseline/source/data/maps/SootopolisCity_Gym_1F/scripts.inc)

## Current map contract

`MAP_SOOTOPOLIS_CITY_GYM_1F` · `LAYOUT_SOOTOPOLIS_CITY_GYM_1F` · `WEATHER_NONE` · `MUS_GYM`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SootopolisCity_Gym_1F:object_events:001` | 8,2 | [SootopolisCity_Gym_1F_EventScript_Juan](../../baseline/source/data/maps/SootopolisCity_Gym_1F/scripts.inc#L82) — SootopolisCity_Gym_1F_EventScript_Juan at (8,2); I am JUAN, WALLACE's mentor, keeper of /  SOOTOPOLIS's final examination. //  You calmed one impossible rain. Now solve /  rain used deliberately: speed, trapping, /  reversed order, and protected Water. //  This is artistry only if every choice has /  purpose. Show me yours. / Excellent! You found space inside every /  current and denied my final rain turn. //  Take the RAIN BADGE. WALLACE will demand /  everything this Gym only introduced. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `SootopolisCity_Gym_1F:object_events:002` | 7,24 | [SootopolisCity_Gym_1F_EventScript_GymGuide](../../baseline/source/data/maps/SootopolisCity_Gym_1F/scripts.inc#L149) — SootopolisCity_Gym_1F_EventScript_GymGuide at (7,24); Yo! How's it going, CHAMPION- /  bound {PLAYER}? //  SOOTOPOLIS's GYM LEADER JUAN is /  a master of WATER-type POKéMON. //  And, to get to JUAN, an icy floor /  will hamper your progress… //  Listen, I'm sorry, but that's all the /  advice that I have for you. //  The rest of the way, you have to /  go for it yourself! / Yow! You've beaten even JUAN, who /  was supposedly the best in all HOENN! //  Okay! Check out your TRAINER CARD. //  If you've gotten all the BADGES, you're /  set for the POKéMON LEAGUE challenge! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `SootopolisCity_Gym_1F:bg_events:001` | 6,24 | [SootopolisCity_Gym_1F_EventScript_LeftGymStatue](../../baseline/source/data/maps/SootopolisCity_Gym_1F/scripts.inc#L162) — SootopolisCity_Gym_1F_EventScript_LeftGymStatue at (6,24); SOOTOPOLIS CITY POKéMON GYM / SOOTOPOLIS CITY POKéMON GYM //  JUAN'S CERTIFIED TRAINERS: /  {PLAYER} | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SootopolisCity_Gym_1F:bg_events:002` | 10,24 | [SootopolisCity_Gym_1F_EventScript_RightGymStatue](../../baseline/source/data/maps/SootopolisCity_Gym_1F/scripts.inc#L168) — SootopolisCity_Gym_1F_EventScript_RightGymStatue at (10,24); SOOTOPOLIS CITY POKéMON GYM / SOOTOPOLIS CITY POKéMON GYM //  JUAN'S CERTIFIED TRAINERS: /  {PLAYER} | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SootopolisCity_Gym_1F:warp_events:001` | 8,25 | Warp from (8,25, elevation 0) to MAP_SOOTOPOLIS_CITY warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SootopolisCity_Gym_1F:warp_events:002` | 9,25 | Warp from (9,25, elevation 0) to MAP_SOOTOPOLIS_CITY warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SootopolisCity_Gym_1F:warp_events:003` | 11,22 | Warp from (11,22, elevation 3) to MAP_SOOTOPOLIS_CITY_GYM_B1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SootopolisCity_Gym_1F:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [SootopolisCity_Gym_1F_OnFrame](../../baseline/source/data/maps/SootopolisCity_Gym_1F/scripts.inc#L36) — MAP_SCRIPT_ON_FRAME_TABLE calls SootopolisCity_Gym_1F_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `SootopolisCity_Gym_1F:map_scripts:002` | MAP_SCRIPT_ON_RESUME | [SootopolisCity_Gym_1F_OnResume](../../baseline/source/data/maps/SootopolisCity_Gym_1F/scripts.inc#L12) — MAP_SCRIPT_ON_RESUME calls SootopolisCity_Gym_1F_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `SootopolisCity_Gym_1F:map_scripts:003` | MAP_SCRIPT_ON_LOAD | [SootopolisCity_Gym_1F_OnLoad](../../baseline/source/data/maps/SootopolisCity_Gym_1F/scripts.inc#L16) — MAP_SCRIPT_ON_LOAD calls SootopolisCity_Gym_1F_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `SootopolisCity_Gym_1F:map_scripts:004` | MAP_SCRIPT_ON_TRANSITION | [SootopolisCity_Gym_1F_OnTransition](../../baseline/source/data/maps/SootopolisCity_Gym_1F/scripts.inc#L8) — MAP_SCRIPT_ON_TRANSITION calls SootopolisCity_Gym_1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
