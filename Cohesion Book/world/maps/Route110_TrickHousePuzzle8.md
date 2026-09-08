# Route110_TrickHousePuzzle8

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/Route110_TrickHousePuzzle8/map.json) · [Scripts](../../baseline/source/data/maps/Route110_TrickHousePuzzle8/scripts.inc)

## Current map contract

`MAP_ROUTE110_TRICK_HOUSE_PUZZLE8` · `LAYOUT_ROUTE110_TRICK_HOUSE_PUZZLE8` · `WEATHER_NONE` · `MUS_TRICK_HOUSE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route110_TrickHousePuzzle8:object_events:001` | 1,10 | [Route110_TrickHousePuzzle8_EventScript_Vincent](../../baseline/source/data/maps/Route110_TrickHousePuzzle8/scripts.inc#L15) — Route110_TrickHousePuzzle8_EventScript_Vincent at (1,10); Not many TRAINERS have made it /  this far. / That must mean you're tough, too… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle8:object_events:002` | 4,2 | [Route110_TrickHousePuzzle8_EventScript_Leroy](../../baseline/source/data/maps/Route110_TrickHousePuzzle8/scripts.inc#L25) — Route110_TrickHousePuzzle8_EventScript_Leroy at (4,2); You've been slugging through the TRICK /  HOUSE challenge, too. / I see… /  You possess an extraordinary style. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle8:object_events:003` | 8,17 | [Route110_TrickHousePuzzle8_EventScript_Keira](../../baseline/source/data/maps/Route110_TrickHousePuzzle8/scripts.inc#L20) — Route110_TrickHousePuzzle8_EventScript_Keira at (8,17); Consider yourself lucky to be /  battling me! / This isn't right! /  I can't lose! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle8:object_events:004` | 2,2 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_BEAD_MAIL; root Common_EventScript_FindItem; flag FLAG_ITEM_TRICK_HOUSE_PUZZLE_8_BEAD_MAIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route110_TrickHousePuzzle8:bg_events:001` | 3,21 | [Route110_TrickHousePuzzle8_EventScript_Scroll](../../baseline/source/data/maps/Route110_TrickHousePuzzle8/scripts.inc#L4) — Route110_TrickHousePuzzle8_EventScript_Scroll at (3,21); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route110_TrickHousePuzzle8:warp_events:001` | 0,21 | Warp from (0,21, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_ENTRANCE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle8:warp_events:002` | 1,21 | Warp from (1,21, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_ENTRANCE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle8:warp_events:003` | 13,1 | Warp from (13,1, elevation 0) to MAP_ROUTE110_TRICK_HOUSE_END warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
