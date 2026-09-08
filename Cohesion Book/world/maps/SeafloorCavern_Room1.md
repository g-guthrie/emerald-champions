# SeafloorCavern_Room1

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SeafloorCavern_Room1/map.json) · [Scripts](../../baseline/source/data/maps/SeafloorCavern_Room1/scripts.inc)

## Current map contract

`MAP_SEAFLOOR_CAVERN_ROOM1` · `LAYOUT_SEAFLOOR_CAVERN_ROOM1` · `WEATHER_NONE` · `MUS_MT_CHIMNEY`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SeafloorCavern_Room1:object_events:001` | 5,11 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (5,11); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SeafloorCavern_Room1:object_events:002` | 12,11 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (12,11); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SeafloorCavern_Room1:object_events:003` | 5,10 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (5,10); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SeafloorCavern_Room1:object_events:004` | 8,6 | [SeafloorCavern_Room1_EventScript_Grunt1](../../baseline/source/data/maps/SeafloorCavern_Room1/scripts.inc#L4) — SeafloorCavern_Room1_EventScript_Grunt1 at (8,6); This cavern changes direction with every /  current. Turn back while you still can. / You made the shifting floor look fixed… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `SeafloorCavern_Room1:object_events:005` | 15,10 | [SeafloorCavern_Room1_EventScript_Grunt2](../../baseline/source/data/maps/SeafloorCavern_Room1/scripts.inc#L9) — SeafloorCavern_Room1_EventScript_Grunt2 at (15,10); The submarine found one narrow route in. /  My formation seals it behind us. / You opened the route again… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `SeafloorCavern_Room1:warp_events:001` | 5,18 | Warp from (5,18, elevation 3) to MAP_SEAFLOOR_CAVERN_ENTRANCE warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SeafloorCavern_Room1:warp_events:002` | 17,13 | Warp from (17,13, elevation 3) to MAP_SEAFLOOR_CAVERN_ROOM5 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SeafloorCavern_Room1:warp_events:003` | 6,2 | Warp from (6,2, elevation 3) to MAP_SEAFLOOR_CAVERN_ROOM2 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
