# MossdeepCity_GameCorner_1F

**REVISE.** Preserve the home, harmless residents, minigame records and imported data. Retire the native e-Reader trainer challenge under the all-doubles policy; externally stored data is not an authored native team.

[Regional experience](../10-support.md) · [Map source](../../baseline/source/data/maps/MossdeepCity_GameCorner_1F/map.json) · [Scripts](../../baseline/source/data/maps/MossdeepCity_GameCorner_1F/scripts.inc)

## Current map contract

`MAP_MOSSDEEP_CITY_GAME_CORNER_1F` · `LAYOUT_MOSSDEEP_CITY_GAME_CORNER_1F` · `WEATHER_NONE` · `MUS_RUSTBORO`

Preserve the home, harmless residents, minigame records and imported data. Retire the native e-Reader trainer challenge under the all-doubles policy; externally stored data is not an authored native team.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MossdeepCity_GameCorner_1F:object_events:001` | 6,2 | [MossdeepCity_GameCorner_1F_EventScript_OldMan](../../baseline/source/data/maps/MossdeepCity_GameCorner_1F/scripts.inc#L24) — MossdeepCity_GameCorner_1F_EventScript_OldMan at (6,2); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MossdeepCity_GameCorner_1F:object_events:002` | 4,2 | [MossdeepCity_GameCorner_1F_EventScript_InfoMan](../../baseline/source/data/maps/MossdeepCity_GameCorner_1F/scripts.inc#L16) — MossdeepCity_GameCorner_1F_EventScript_InfoMan at (4,2); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MossdeepCity_GameCorner_1F:bg_events:001` | 3,0 | [RS_MysteryEventsHouse_EventScript_Door](../../baseline/source/data/maps/MossdeepCity_GameCorner_1F/scripts.inc#L37) — RS_MysteryEventsHouse_EventScript_Door at (3,0); The door appears to be locked. | **REPAIR** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · W-EREADER-NATIVE-RETIRE |
| `MossdeepCity_GameCorner_1F:bg_events:002` | 0,1 | [MossdeepCity_GameCorner_1F_EventScript_DodrioBerryPickingRecords](../../baseline/source/data/scripts/cable_club.inc#L1385) — MossdeepCity_GameCorner_1F_EventScript_DodrioBerryPickingRecords at (0,1); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `MossdeepCity_GameCorner_1F:bg_events:003` | 1,1 | [MossdeepCity_GameCorner_1F_EventScript_PokemonJumpRecords](../../baseline/source/data/scripts/cable_club.inc#L1379) — MossdeepCity_GameCorner_1F_EventScript_PokemonJumpRecords at (1,1); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `MossdeepCity_GameCorner_1F:warp_events:001` | 5,9 | Warp from (5,9, elevation 0) to MAP_MOSSDEEP_CITY warp 9. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MossdeepCity_GameCorner_1F:warp_events:002` | 6,9 | Warp from (6,9, elevation 0) to MAP_MOSSDEEP_CITY warp 9. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MossdeepCity_GameCorner_1F:warp_events:003` | 2,0 | Warp from (2,0, elevation 0) to MAP_MOSSDEEP_CITY_GAME_CORNER_B1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MossdeepCity_GameCorner_1F:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [MossdeepCity_GameCorner_1F_OnFrame](../../baseline/source/data/maps/MossdeepCity_GameCorner_1F/scripts.inc#L11) — MAP_SCRIPT_ON_FRAME_TABLE calls MossdeepCity_GameCorner_1F_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `MossdeepCity_GameCorner_1F:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [MossdeepCity_GameCorner_1F_OnWarp](../../baseline/source/data/maps/MossdeepCity_GameCorner_1F/scripts.inc#L7) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls MossdeepCity_GameCorner_1F_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `MossdeepCity_GameCorner_1F:map_scripts:003` | MAP_SCRIPT_ON_LOAD | [CableClub_OnLoad](../../baseline/source/data/scripts/cable_club.inc#L68) — MAP_SCRIPT_ON_LOAD calls CableClub_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
