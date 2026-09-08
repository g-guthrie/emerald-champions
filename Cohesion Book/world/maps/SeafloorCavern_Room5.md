# SeafloorCavern_Room5

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SeafloorCavern_Room5/map.json) · [Scripts](../../baseline/source/data/maps/SeafloorCavern_Room5/scripts.inc)

## Current map contract

`MAP_SEAFLOOR_CAVERN_ROOM5` · `LAYOUT_SEAFLOOR_CAVERN_ROOM5` · `WEATHER_NONE` · `MUS_MT_CHIMNEY`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SeafloorCavern_Room5:object_events:001` | 3,11 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (3,11); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SeafloorCavern_Room5:object_events:002` | 4,11 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (4,11); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SeafloorCavern_Room5:object_events:003` | 11,7 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (11,7); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SeafloorCavern_Room5:object_events:004` | 3,13 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (3,13); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SeafloorCavern_Room5:object_events:005` | 13,8 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (13,8); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SeafloorCavern_Room5:object_events:006` | 12,8 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (12,8); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SeafloorCavern_Room5:warp_events:001` | 4,1 | Warp from (4,1, elevation 3) to MAP_SEAFLOOR_CAVERN_ROOM1 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SeafloorCavern_Room5:warp_events:002` | 15,12 | Warp from (15,12, elevation 3) to MAP_SEAFLOOR_CAVERN_ROOM4 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SeafloorCavern_Room5:warp_events:003` | 7,17 | Warp from (7,17, elevation 3) to MAP_SEAFLOOR_CAVERN_ROOM4 warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
