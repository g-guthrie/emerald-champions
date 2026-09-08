# ShoalCave_LowTideLowerRoom

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/ShoalCave_LowTideLowerRoom/map.json) · [Scripts](../../baseline/source/data/maps/ShoalCave_LowTideLowerRoom/scripts.inc)

## Current map contract

`MAP_SHOAL_CAVE_LOW_TIDE_LOWER_ROOM` · `LAYOUT_SHOAL_CAVE_LOW_TIDE_LOWER_ROOM` · `WEATHER_NONE` · `MUS_MT_PYRE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `ShoalCave_LowTideLowerRoom:object_events:001` | 25,3 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (25,3); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `ShoalCave_LowTideLowerRoom:object_events:002` | 11,4 | [ShoalCave_LowTideLowerRoom_EventScript_BlackBelt](../../baseline/source/data/maps/ShoalCave_LowTideLowerRoom/scripts.inc#L33) — ShoalCave_LowTideLowerRoom_EventScript_BlackBelt at (11,4); Your focus overcame the cold! //  Take this Deep Sea Scale. / Everything starts with focus! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `ShoalCave_LowTideLowerRoom:bg_events:001` | 18,2 | [ShoalCave_LowTideLowerRoom_EventScript_ShoalSalt4](../../baseline/source/data/maps/ShoalCave_LowTideLowerRoom/scripts.inc#L17) — ShoalCave_LowTideLowerRoom_EventScript_ShoalSalt4 at (18,2); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `ShoalCave_LowTideLowerRoom:warp_events:001` | 7,2 | Warp from (7,2, elevation 3) to MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideLowerRoom:warp_events:002` | 2,6 | Warp from (2,6, elevation 3) to MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideLowerRoom:warp_events:003` | 19,11 | Warp from (19,11, elevation 3) to MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideLowerRoom:warp_events:004` | 28,11 | Warp from (28,11, elevation 3) to MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideLowerRoom:map_scripts:001` | MAP_SCRIPT_ON_LOAD | [ShoalCave_LowTideLowerRoom_OnLoad](../../baseline/source/data/maps/ShoalCave_LowTideLowerRoom/scripts.inc#L5) — MAP_SCRIPT_ON_LOAD calls ShoalCave_LowTideLowerRoom_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
