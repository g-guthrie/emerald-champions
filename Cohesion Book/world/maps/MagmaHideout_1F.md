# MagmaHideout_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/MagmaHideout_1F/map.json) · [Scripts](../../baseline/source/data/maps/MagmaHideout_1F/scripts.inc)

## Current map contract

`MAP_MAGMA_HIDEOUT_1F` · `LAYOUT_MAGMA_HIDEOUT_1F` · `WEATHER_NONE` · `MUS_AQUA_MAGMA_HIDEOUT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MagmaHideout_1F:object_events:001` | 4,5 | [MagmaHideout_1F_EventScript_Grunt1](../../baseline/source/data/maps/MagmaHideout_1F/scripts.inc#L9) — MagmaHideout_1F_EventScript_Grunt1 at (4,5); Every guard has one fixed position. /  Mine is this corner and this approach. //  MAXIE says order fails when one person /  abandons their place. I will hold mine. / I held the position, but not the battle… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_1F:object_events:002` | 3,20 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ULTRA_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_MAGMA_HIDEOUT_1F_ULTRA_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MagmaHideout_1F:object_events:003` | 30,20 | [MagmaHideout_1F_EventScript_Grunt2](../../baseline/source/data/maps/MagmaHideout_1F/scripts.inc#L14) — MagmaHideout_1F_EventScript_Grunt2 at (30,20); Our leader told us to dig into /  MT. CHIMNEY, so we dug and dug. //  We found a chamber tied to the same /  network as the CHAMPION'S SIGNS. //  Beat my formation and see it yourself. / The entrance line has fallen… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_1F:object_events:004` | 5,22 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (5,22); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `MagmaHideout_1F:object_events:005` | 7,22 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (7,22); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `MagmaHideout_1F:object_events:006` | 6,23 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (6,23); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `MagmaHideout_1F:warp_events:001` | 10,34 | Warp from (10,34, elevation 3) to MAP_JAGGED_PASS warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MagmaHideout_1F:warp_events:002` | 25,34 | Warp from (25,34, elevation 3) to MAP_MAGMA_HIDEOUT_2F_1R warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MagmaHideout_1F:warp_events:003` | 31,3 | Warp from (31,3, elevation 0) to MAP_MAGMA_HIDEOUT_2F_2R warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MagmaHideout_1F:warp_events:004` | 20,22 | Warp from (20,22, elevation 0) to MAP_MAGMA_HIDEOUT_2F_3R warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MagmaHideout_1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [MagmaHideout_1F_OnTransition](../../baseline/source/data/maps/MagmaHideout_1F/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls MagmaHideout_1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
