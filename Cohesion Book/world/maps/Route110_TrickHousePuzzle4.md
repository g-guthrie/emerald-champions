# Route110_TrickHousePuzzle4

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/Route110_TrickHousePuzzle4/map.json) · [Scripts](../../baseline/source/data/maps/Route110_TrickHousePuzzle4/scripts.inc)

## Current map contract

`MAP_ROUTE110_TRICK_HOUSE_PUZZLE4` · `LAYOUT_ROUTE110_TRICK_HOUSE_PUZZLE4` · `WEATHER_NONE` · `MUS_TRICK_HOUSE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route110_TrickHousePuzzle4:object_events:001` | 2,2 | [Route110_TrickHousePuzzle4_EventScript_Cora](../../baseline/source/data/maps/Route110_TrickHousePuzzle4/scripts.inc#L15) — Route110_TrickHousePuzzle4_EventScript_Cora at (2,2); It's too much bother to think this out. /  I only wanted to battle! / Even though I lost, I still like battling /  the best! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle4:object_events:002` | 14,7 | [Route110_TrickHousePuzzle4_EventScript_Paula](../../baseline/source/data/maps/Route110_TrickHousePuzzle4/scripts.inc#L25) — Route110_TrickHousePuzzle4_EventScript_Paula at (14,7); The TRICK HOUSE is getting trickier, /  isn't it? / Aaak! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle4:object_events:003` | 2,14 | [Route110_TrickHousePuzzle4_EventScript_Yuji](../../baseline/source/data/maps/Route110_TrickHousePuzzle4/scripts.inc#L20) — Route110_TrickHousePuzzle4_EventScript_Yuji at (2,14); Heh! Boulders like this, I can brush /  aside with one finger! / I can push boulders, but I can't solve /  the puzzle… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle4:object_events:004` | 2,5 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MECH_MAIL; root Common_EventScript_FindItem; flag FLAG_ITEM_TRICK_HOUSE_PUZZLE_4_MECH_MAIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route110_TrickHousePuzzle4:object_events:005` | 13,3 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (13,3); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle4:object_events:006` | 12,5 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (12,5); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle4:object_events:007` | 5,16 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (5,16); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle4:object_events:008` | 4,6 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (4,6); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle4:object_events:009` | 12,2 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (12,2); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle4:object_events:010` | 5,7 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (5,7); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle4:object_events:011` | 9,3 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (9,3); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle4:object_events:012` | 10,12 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (10,12); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle4:object_events:013` | 14,2 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (14,2); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle4:object_events:014` | 10,15 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (10,15); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle4:bg_events:001` | 14,13 | [Route110_TrickHousePuzzle4_EventScript_Scroll](../../baseline/source/data/maps/Route110_TrickHousePuzzle4/scripts.inc#L4) — Route110_TrickHousePuzzle4_EventScript_Scroll at (14,13); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route110_TrickHousePuzzle4:warp_events:001` | 0,21 | Warp from (0,21, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_ENTRANCE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle4:warp_events:002` | 1,21 | Warp from (1,21, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_ENTRANCE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle4:warp_events:003` | 13,1 | Warp from (13,1, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_END warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
