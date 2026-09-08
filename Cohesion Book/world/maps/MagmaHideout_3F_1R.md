# MagmaHideout_3F_1R

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/MagmaHideout_3F_1R/map.json) · [Scripts](../../baseline/source/data/maps/MagmaHideout_3F_1R/scripts.inc)

## Current map contract

`MAP_MAGMA_HIDEOUT_3F_1R` · `LAYOUT_MAGMA_HIDEOUT_3F_1R` · `WEATHER_NONE` · `MUS_AQUA_MAGMA_HIDEOUT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MagmaHideout_3F_1R:object_events:001` | 2,7 | [MagmaHideout_3F_1R_EventScript_Grunt9](../../baseline/source/data/maps/MagmaHideout_3F_1R/scripts.inc#L4) — MagmaHideout_3F_1R_EventScript_Grunt9 at (2,7); Guard posting next to the lava. My left /  ear's been cooking for an hour. //  At least fighting you means I can move. / Heat exhaustion. Definitely the heat. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_3F_1R:object_events:002` | 21,21 | [MagmaHideout_3F_1R_EventScript_Grunt16](../../baseline/source/data/maps/MagmaHideout_3F_1R/scripts.inc#L9) — MagmaHideout_3F_1R_EventScript_Grunt16 at (21,21); MAXIE offers a world people can predict, /  build on, and pass to their children. //  AQUA rejects it. You question it. /  I will defend it. / Your uncertainty outplayed my order… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_3F_1R:object_events:003` | 9,16 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_NUGGET; root Common_EventScript_FindItem; flag FLAG_ITEM_MAGMA_HIDEOUT_3F_1R_NUGGET. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MagmaHideout_3F_1R:warp_events:001` | 7,21 | Warp from (7,21, elevation 0) to MAP_MAGMA_HIDEOUT_4F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MagmaHideout_3F_1R:warp_events:002` | 21,9 | Warp from (21,9, elevation 0) to MAP_MAGMA_HIDEOUT_3F_2R warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MagmaHideout_3F_1R:warp_events:003` | 23,3 | Warp from (23,3, elevation 0) to MAP_MAGMA_HIDEOUT_2F_1R warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
