# AlteringCave_B1F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/AlteringCave_B1F/map.json) · [Scripts](../../baseline/source/data/maps/AlteringCave_B1F/scripts.inc)

## Current map contract

`MAP_ALTERING_CAVE_B1F` · `LAYOUT_ALTERING_CAVE_B1F` · `WEATHER_NONE` · `MUS_RG_SEVII_CAVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `AlteringCave_B1F:object_events:001` | 2,2 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (2,2); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_B1F:object_events:002` | 3,4 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (3,4); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_B1F:object_events:003` | 4,1 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (4,1); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_B1F:object_events:004` | 6,1 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (6,1); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_B1F:object_events:005` | 35,1 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (35,1); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_B1F:object_events:006` | 37,1 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (37,1); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_B1F:object_events:007` | 38,2 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (38,2); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_B1F:object_events:008` | 37,4 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (37,4); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_B1F:object_events:009` | 35,5 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (35,5); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `AlteringCave_B1F:object_events:010` | 7,13 | [AlteringCave_B1F_EventScript_Mewtwo](../../baseline/source/data/maps/AlteringCave_B1F/scripts.inc#L108) — AlteringCave_B1F_EventScript_Mewtwo at (7,13); A CHAMPION'S SIGN is carved here. //  Its light is dormant. Hoenn's story /  has not yet reached this place. / The CHAMPION'S SIGN erupts with light! /  MEWTWO answers the challenge! | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `AlteringCave_B1F:object_events:011` | 21,16 | Passive/staged OBJ_EVENT_GFX_LEAF at (21,16); visibility flag FLAG_EC_DEFEATED_LEAF_ALTERING_CAVE; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `AlteringCave_B1F:object_events:012` | 35,21 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MEWTWONITE_Y; root Common_EventScript_FindItem; flag FLAG_EC_MEGA_REWARD_MEWTWONITE_Y. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AlteringCave_B1F:coord_events:001` | 21,19 | [AlteringCave_B1F_EventScript_Leaf](../../baseline/source/data/maps/AlteringCave_B1F/scripts.inc#L4) — Coordinate trigger at (21,19); VAR_TEMP_0 == 0 invokes AlteringCave_B1F_EventScript_Leaf. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `AlteringCave_B1F:warp_events:001` | 5,7 | Warp from (5,7, elevation 0) to MAP_ALTERING_CAVE warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
