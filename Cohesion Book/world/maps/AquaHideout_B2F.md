# AquaHideout_B2F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/AquaHideout_B2F/map.json) · [Scripts](../../baseline/source/data/maps/AquaHideout_B2F/scripts.inc)

## Current map contract

`MAP_AQUA_HIDEOUT_B2F` · `LAYOUT_AQUA_HIDEOUT_B2F` · `WEATHER_NONE` · `MUS_AQUA_MAGMA_HIDEOUT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `AquaHideout_B2F:object_events:001` | 23,19 | [AquaHideout_B2F_EventScript_Matt](../../baseline/source/data/maps/AquaHideout_B2F/scripts.inc#L25) — AquaHideout_B2F_EventScript_Matt at (23,19); MATT: You crossed the entire hideout /  before the tide changed. Impressive. //  ARCHIE needs one final current of time. /  My job is to make every turn cost you. / You refused every delay I offered… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AquaHideout_B2F:object_events:002` | 23,10 | [AquaHideout_B2F_EventScript_Grunt4](../../baseline/source/data/maps/AquaHideout_B2F/scripts.inc#L69) — AquaHideout_B2F_EventScript_Grunt4 at (23,10); Waiting is part of an ambush. You finally /  entered the current I was assigned. / You turned my ambush back on me… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AquaHideout_B2F:object_events:003` | 3,13 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_NEST_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_AQUA_HIDEOUT_B2F_NEST_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AquaHideout_B2F:object_events:004` | 19,20 | Passive/staged OBJ_EVENT_GFX_SUBMARINE_SHADOW at (19,20); visibility flag FLAG_HIDE_AQUA_HIDEOUT_B2F_SUBMARINE_SHADOW; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `AquaHideout_B2F:object_events:005` | 7,5 | [AquaHideout_B2F_EventScript_Grunt6](../../baseline/source/data/maps/AquaHideout_B2F/scripts.inc#L79) — AquaHideout_B2F_EventScript_Grunt6 at (7,5); Warp panels, the HIDEOUT's pride /  and joy! //  Every panel changes your position and /  every battle changes your resources. //  AQUA wins by making the field move. / What's wrong with you? /  You're not tired at all! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AquaHideout_B2F:object_events:006` | 13,5 | [AquaHideout_B2F_EventScript_Grunt8](../../baseline/source/data/maps/AquaHideout_B2F/scripts.inc#L84) — AquaHideout_B2F_EventScript_Grunt8 at (13,5); First thing they taught me here was how /  the warp panels connect. //  Second thing was doubles. I'm better /  at the panels. / Too busy thinking about panels… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AquaHideout_B2F:coord_events:001` | 28,17 | [AquaHideout_B2F_EventScript_MattNoticePlayer](../../baseline/source/data/maps/AquaHideout_B2F/scripts.inc#L13) — Coordinate trigger at (28,17); VAR_TEMP_1 == 0 invokes AquaHideout_B2F_EventScript_MattNoticePlayer. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `AquaHideout_B2F:coord_events:002` | 28,16 | [AquaHideout_B2F_EventScript_MattNoticePlayer](../../baseline/source/data/maps/AquaHideout_B2F/scripts.inc#L13) — Coordinate trigger at (28,16); VAR_TEMP_1 == 0 invokes AquaHideout_B2F_EventScript_MattNoticePlayer. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `AquaHideout_B2F:warp_events:001` | 18,1 | Warp from (18,1, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B2F:warp_events:002` | 12,1 | Warp from (12,1, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B2F:warp_events:003` | 3,3 | Warp from (3,3, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B2F:warp_events:004` | 31,8 | Warp from (31,8, elevation 3) to MAP_AQUA_HIDEOUT_B2F warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B2F:warp_events:005` | 8,8 | Warp from (8,8, elevation 3) to MAP_AQUA_HIDEOUT_B2F warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B2F:warp_events:006` | 5,8 | Warp from (5,8, elevation 3) to MAP_AQUA_HIDEOUT_B2F warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B2F:warp_events:007` | 18,13 | Warp from (18,13, elevation 3) to MAP_AQUA_HIDEOUT_B2F warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B2F:warp_events:008` | 12,13 | Warp from (12,13, elevation 3) to MAP_AQUA_HIDEOUT_B2F warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B2F:warp_events:009` | 31,17 | Warp from (31,17, elevation 3) to MAP_AQUA_HIDEOUT_B2F warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B2F:warp_events:010` | 32,20 | Warp from (32,20, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_B2F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [AquaHideout_B2F_OnTransition](../../baseline/source/data/maps/AquaHideout_B2F/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls AquaHideout_B2F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
