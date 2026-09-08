# LavaridgeTown_Gym_B1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/LavaridgeTown_Gym_B1F/map.json) · [Scripts](../../baseline/source/data/maps/LavaridgeTown_Gym_B1F/scripts.inc)

## Current map contract

`MAP_LAVARIDGE_TOWN_GYM_B1F` · `LAYOUT_LAVARIDGE_TOWN_GYM_B1F` · `WEATHER_FOG_HORIZONTAL` · `MUS_GYM`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LavaridgeTown_Gym_B1F:object_events:001` | 4,18 | [LavaridgeTown_Gym_B1F_EventScript_Jace](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc#L134) — LavaridgeTown_Gym_B1F_EventScript_Jace at (4,18); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `LavaridgeTown_Gym_B1F:object_events:002` | 3,6 | [LavaridgeTown_Gym_B1F_EventScript_Keegan](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc#L119) — LavaridgeTown_Gym_B1F_EventScript_Keegan at (3,6); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `LavaridgeTown_Gym_B1F:object_events:003` | 13,17 | [LavaridgeTown_Gym_B1F_EventScript_Jeff](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc#L139) — LavaridgeTown_Gym_B1F_EventScript_Jeff at (13,17); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `LavaridgeTown_Gym_B1F:object_events:004` | 4,16 | [LavaridgeTown_Gym_B1F_EventScript_Eli](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc#L144) — LavaridgeTown_Gym_B1F_EventScript_Eli at (4,16); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `LavaridgeTown_Gym_B1F:warp_events:001` | 10,18 | Warp from (10,18, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:002` | 0,17 | Warp from (0,17, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:003` | 8,9 | Warp from (8,9, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:004` | 5,14 | Warp from (5,14, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:005` | 4,18 | Warp from (4,18, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:006` | 5,9 | Warp from (5,9, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:007` | 2,15 | Warp from (2,15, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:008` | 3,14 | Warp from (3,14, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 9. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:009` | 1,14 | Warp from (1,14, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:010` | 0,10 | Warp from (0,10, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 11. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:011` | 3,10 | Warp from (3,10, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 12. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:012` | 0,6 | Warp from (0,6, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 13. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:013` | 3,6 | Warp from (3,6, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 14. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:014` | 5,6 | Warp from (5,6, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 15. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:015` | 2,3 | Warp from (2,3, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 16. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:016` | 5,2 | Warp from (5,2, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 17. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:017` | 7,2 | Warp from (7,2, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 18. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:018` | 8,6 | Warp from (8,6, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 19. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:019` | 10,6 | Warp from (10,6, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 20. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:020` | 12,3 | Warp from (12,3, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 22. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:021` | 4,16 | Warp from (4,16, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 21. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:022` | 14,6 | Warp from (14,6, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 23. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:023` | 13,17 | Warp from (13,17, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 24. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:warp_events:024` | 12,12 | Warp from (12,12, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_1F warp 25. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_B1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [LavaridgeTown_Gym_B1F_OnTransition](../../baseline/source/data/maps/LavaridgeTown_Gym_B1F/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls LavaridgeTown_Gym_B1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
