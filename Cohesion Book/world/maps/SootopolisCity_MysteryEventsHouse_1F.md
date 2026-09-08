# SootopolisCity_MysteryEventsHouse_1F

**REVISE.** Preserve the home, harmless residents, minigame records and imported data. Retire the native e-Reader trainer challenge under the all-doubles policy; externally stored data is not an authored native team.

[Regional experience](../10-support.md) · [Map source](../../baseline/source/data/maps/SootopolisCity_MysteryEventsHouse_1F/map.json) · [Scripts](../../baseline/source/data/maps/SootopolisCity_MysteryEventsHouse_1F/scripts.inc)

## Current map contract

`MAP_SOOTOPOLIS_CITY_MYSTERY_EVENTS_HOUSE_1F` · `LAYOUT_SOOTOPOLIS_CITY_MYSTERY_EVENTS_HOUSE_1F` · `WEATHER_NONE` · `MUS_SOOTOPOLIS`

Preserve the home, harmless residents, minigame records and imported data. Retire the native e-Reader trainer challenge under the all-doubles policy; externally stored data is not an authored native team.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SootopolisCity_MysteryEventsHouse_1F:object_events:001` | 6,4 | [SootopolisCity_MysteryEventsHouse_1F_EventScript_OldMan](../../baseline/source/data/maps/SootopolisCity_MysteryEventsHouse_1F/scripts.inc#L68) — SootopolisCity_MysteryEventsHouse_1F_EventScript_OldMan at (6,4); When I was young, I traveled the world /  as a POKéMON TRAINER. //  Now that I've become an old buzzard, /  my only amusement is watching young /  TRAINERS battle. / A TRAINER named {STR_VAR_1} /  is visiting my home. //  Would you like to challenge /  {STR_VAR_1}? | **REPAIR** · [W-C-STORY](../common-contracts.md#w-c-story) · W-EREADER-NATIVE-RETIRE |
| `SootopolisCity_MysteryEventsHouse_1F:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_SOOTOPOLIS_CITY warp 12. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SootopolisCity_MysteryEventsHouse_1F:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_SOOTOPOLIS_CITY warp 12. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SootopolisCity_MysteryEventsHouse_1F:warp_events:003` | 3,1 | Warp from (3,1, elevation 3) to MAP_SOOTOPOLIS_CITY_MYSTERY_EVENTS_HOUSE_B1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SootopolisCity_MysteryEventsHouse_1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [SootopolisCity_MysteryEventsHouse_1F_OnTransition](../../baseline/source/data/maps/SootopolisCity_MysteryEventsHouse_1F/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls SootopolisCity_MysteryEventsHouse_1F_OnTransition. | **REPAIR** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · W-EREADER-NATIVE-RETIRE |
| `SootopolisCity_MysteryEventsHouse_1F:map_scripts:002` | MAP_SCRIPT_ON_FRAME_TABLE | [SootopolisCity_MysteryEventsHouse_1F_OnFrame](../../baseline/source/data/maps/SootopolisCity_MysteryEventsHouse_1F/scripts.inc#L24) — MAP_SCRIPT_ON_FRAME_TABLE calls SootopolisCity_MysteryEventsHouse_1F_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
