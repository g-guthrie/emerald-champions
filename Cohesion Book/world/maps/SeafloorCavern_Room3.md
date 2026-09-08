# SeafloorCavern_Room3

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SeafloorCavern_Room3/map.json) · [Scripts](../../baseline/source/data/maps/SeafloorCavern_Room3/scripts.inc)

## Current map contract

`MAP_SEAFLOOR_CAVERN_ROOM3` · `LAYOUT_SEAFLOOR_CAVERN_ROOM3` · `WEATHER_NONE` · `MUS_MT_CHIMNEY`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SeafloorCavern_Room3:object_events:001` | 13,10 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (13,10); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SeafloorCavern_Room3:object_events:002` | 11,10 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (11,10); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SeafloorCavern_Room3:object_events:003` | 12,9 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (12,9); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SeafloorCavern_Room3:object_events:004` | 12,7 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (12,7); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SeafloorCavern_Room3:object_events:005` | 11,8 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (11,8); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SeafloorCavern_Room3:object_events:006` | 12,11 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (12,11); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SeafloorCavern_Room3:object_events:007` | 13,8 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (13,8); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SeafloorCavern_Room3:object_events:008` | 5,5 | [SeafloorCavern_Room3_EventScript_Shelly](../../baseline/source/data/maps/SeafloorCavern_Room3/scripts.inc#L4) — SeafloorCavern_Room3_EventScript_Shelly at (5,5); SHELLY: You reached the seafloor without /  our submarine. I expected no less. //  At the INSTITUTE you read the weather. /  Here the cavern itself changes the field. //  Read the current before it closes. / You mapped the current while inside it… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `SeafloorCavern_Room3:object_events:009` | 7,8 | [SeafloorCavern_Room3_EventScript_Grunt5](../../baseline/source/data/maps/SeafloorCavern_Room3/scripts.inc#L9) — SeafloorCavern_Room3_EventScript_Grunt5 at (7,8); AQUA needs POKéMON to reach a current /  human tools cannot follow. //  Your team carried you just as far. /  Only one formation advances. / Your formation held deeper… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `SeafloorCavern_Room3:warp_events:001` | 8,1 | Warp from (8,1, elevation 3) to MAP_SEAFLOOR_CAVERN_ROOM8 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SeafloorCavern_Room3:warp_events:002` | 9,13 | Warp from (9,13, elevation 3) to MAP_SEAFLOOR_CAVERN_ROOM7 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SeafloorCavern_Room3:warp_events:003` | 4,15 | Warp from (4,15, elevation 3) to MAP_SEAFLOOR_CAVERN_ROOM6 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
