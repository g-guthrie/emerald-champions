# AquaHideout_B1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/AquaHideout_B1F/map.json) · [Scripts](../../baseline/source/data/maps/AquaHideout_B1F/scripts.inc)

## Current map contract

`MAP_AQUA_HIDEOUT_B1F` · `LAYOUT_AQUA_HIDEOUT_B1F` · `WEATHER_NONE` · `MUS_AQUA_MAGMA_HIDEOUT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `AquaHideout_B1F:object_events:001` | 28,16 | [AquaHideout_B1F_EventScript_Grunt2](../../baseline/source/data/maps/AquaHideout_B1F/scripts.inc#L79) — AquaHideout_B1F_EventScript_Grunt2 at (28,16); Want the HIDEOUT's secret? Then you go /  through me and my SHARPEDO first! / I can't win at all lately… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AquaHideout_B1F:object_events:002` | 6,6 | [AquaHideout_B1F_EventScript_Grunt3](../../baseline/source/data/maps/AquaHideout_B1F/scripts.inc#L91) — AquaHideout_B1F_EventScript_Grunt3 at (6,6); Fuel loaded A-OK! Snacks loaded A-OK! //  Only thing left on my checklist is /  KO one meddler. That's you. / That's… not how the checklist went. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AquaHideout_B1F:object_events:003` | 29,12 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MAX_ELIXIR; root Common_EventScript_FindItem; flag FLAG_ITEM_AQUA_HIDEOUT_B1F_MAX_ELIXIR. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AquaHideout_B1F:object_events:004` | 20,18 | [AquaHideout_B1F_EventScript_Grunt5](../../baseline/source/data/maps/AquaHideout_B1F/scripts.inc#L101) — AquaHideout_B1F_EventScript_Grunt5 at (20,18); Yawn… Guard duty in a cave with no sea /  view. I'll take you on just to wake up. / Yawn… oh. I lost. Figures. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AquaHideout_B1F:object_events:005` | 15,9 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MASTER_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_AQUA_HIDEOUT_B1F_MASTER_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AquaHideout_B1F:object_events:006` | 16,9 | [AquaHideout_B1F_EventScript_Electrode1](../../baseline/source/data/maps/AquaHideout_B1F/scripts.inc#L29) — AquaHideout_B1F_EventScript_Electrode1 at (16,9); shared behavior STORY | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `AquaHideout_B1F:object_events:007` | 16,10 | [AquaHideout_B1F_EventScript_Electrode2](../../baseline/source/data/maps/AquaHideout_B1F/scripts.inc#L54) — AquaHideout_B1F_EventScript_Electrode2 at (16,10); shared behavior STORY | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `AquaHideout_B1F:object_events:008` | 15,10 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_NUGGET; root Common_EventScript_FindItem; flag FLAG_ITEM_AQUA_HIDEOUT_B1F_NUGGET. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AquaHideout_B1F:object_events:009` | 28,21 | [AquaHideout_B1F_EventScript_Grunt7](../../baseline/source/data/maps/AquaHideout_B1F/scripts.inc#L106) — AquaHideout_B1F_EventScript_Grunt7 at (28,21); Hey! Honest question. Whose uniform is /  cooler, ours or MAGMA's? //  …Right answer. Now battle me anyway. / I lost in a cool way, at least… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AquaHideout_B1F:object_events:010` | 45,18 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MASTER_BALL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_AQUA_HIDEOUT_B1F_MASTER_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AquaHideout_B1F:warp_events:001` | 29,1 | Warp from (29,1, elevation 3) to MAP_AQUA_HIDEOUT_1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:002` | 18,1 | Warp from (18,1, elevation 3) to MAP_AQUA_HIDEOUT_B2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:003` | 12,1 | Warp from (12,1, elevation 3) to MAP_AQUA_HIDEOUT_B2F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:004` | 3,3 | Warp from (3,3, elevation 3) to MAP_AQUA_HIDEOUT_B2F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:005` | 31,4 | Warp from (31,4, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:006` | 27,4 | Warp from (27,4, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:007` | 20,4 | Warp from (20,4, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:008` | 27,12 | Warp from (27,12, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:009` | 3,15 | Warp from (3,15, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:010` | 3,20 | Warp from (3,20, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 12. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:011` | 32,19 | Warp from (32,19, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:012` | 23,10 | Warp from (23,10, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 22. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:013` | 45,3 | Warp from (45,3, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 9. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:014` | 42,5 | Warp from (42,5, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 18. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:015` | 45,5 | Warp from (45,5, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 12. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:016` | 48,5 | Warp from (48,5, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 16. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:017` | 42,9 | Warp from (42,9, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 15. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:018` | 45,9 | Warp from (45,9, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 20. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:019` | 48,9 | Warp from (48,9, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 13. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:020` | 42,13 | Warp from (42,13, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 24. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:021` | 45,13 | Warp from (45,13, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 17. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:022` | 48,13 | Warp from (48,13, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 12. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:023` | 42,17 | Warp from (42,17, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 11. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:024` | 45,17 | Warp from (45,17, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 17. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:warp_events:025` | 48,17 | Warp from (48,17, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 19. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B1F:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [AquaHideout_B1F_OnResume](../../baseline/source/data/maps/AquaHideout_B1F/scripts.inc#L6) — MAP_SCRIPT_ON_RESUME calls AquaHideout_B1F_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `AquaHideout_B1F:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [AquaHideout_B1F_OnTransition](../../baseline/source/data/maps/AquaHideout_B1F/scripts.inc#L16) — MAP_SCRIPT_ON_TRANSITION calls AquaHideout_B1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
