# MagmaHideout_2F_2R

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/MagmaHideout_2F_2R/map.json) · [Scripts](../../baseline/source/data/maps/MagmaHideout_2F_2R/scripts.inc)

## Current map contract

`MAP_MAGMA_HIDEOUT_2F_2R` · `LAYOUT_MAGMA_HIDEOUT_2F_2R` · `WEATHER_NONE` · `MUS_AQUA_MAGMA_HIDEOUT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MagmaHideout_2F_2R:object_events:001` | 29,8 | [MagmaHideout_2F_2R_EventScript_Grunt8](../../baseline/source/data/maps/MagmaHideout_2F_2R/scripts.inc#L19) — MagmaHideout_2F_2R_EventScript_Grunt8 at (29,8); Wait. One of ours dropped his MAGMA /  EMBLEM outside. Was it you who found it? //  Doesn't matter. You're still not /  getting past FLYGON. / I'm having trouble believing this… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_2F_2R:object_events:002` | 25,11 | [MagmaHideout_2F_2R_EventScript_Grunt7](../../baseline/source/data/maps/MagmaHideout_2F_2R/scripts.inc#L14) — MagmaHideout_2F_2R_EventScript_Grunt7 at (25,11); You feel those tremors? People say it's /  the volcano. Some of us know better. //  Whoops! Never mind. Fight me! / You hit like a volcano yourself… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_2F_2R:object_events:003` | 21,7 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MAX_ELIXIR; root Common_EventScript_FindItem; flag FLAG_ITEM_MAGMA_HIDEOUT_2F_2R_MAX_ELIXIR. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MagmaHideout_2F_2R:object_events:004` | 8,9 | [MagmaHideout_2F_2R_EventScript_Grunt6](../../baseline/source/data/maps/MagmaHideout_2F_2R/scripts.inc#L9) — MagmaHideout_2F_2R_EventScript_Grunt6 at (8,9); I used to hate this heat. Then MAGMORTAR /  taught me to wear it like armor. / Even armor buckles under pressure… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_2F_2R:object_events:005` | 7,13 | [MagmaHideout_2F_2R_EventScript_Grunt15](../../baseline/source/data/maps/MagmaHideout_2F_2R/scripts.inc#L4) — MagmaHideout_2F_2R_EventScript_Grunt15 at (7,13); Nothing personal. I'm following orders, /  and my orders are: nobody passes. / Orders, meet reality… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_2F_2R:object_events:006` | 14,6 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_FULL_RESTORE; root Common_EventScript_FindItem; flag FLAG_ITEM_MAGMA_HIDEOUT_2F_2R_FULL_RESTORE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MagmaHideout_2F_2R:object_events:007` | 26,4 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MAGMA_STONE; root Common_EventScript_FindItem; flag FLAG_ITEM_MAGMA_HIDEOUT_2F_2R_MAGMA_STONE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MagmaHideout_2F_2R:warp_events:001` | 10,22 | Warp from (10,22, elevation 3) to MAP_MAGMA_HIDEOUT_2F_1R warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MagmaHideout_2F_2R:warp_events:002` | 36,4 | Warp from (36,4, elevation 0) to MAP_MAGMA_HIDEOUT_1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
