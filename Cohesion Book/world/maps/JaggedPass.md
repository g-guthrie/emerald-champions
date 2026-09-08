# JaggedPass

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/JaggedPass/map.json) · [Scripts](../../baseline/source/data/maps/JaggedPass/scripts.inc)

## Current map contract

`MAP_JAGGED_PASS` · `LAYOUT_JAGGED_PASS` · `WEATHER_NONE` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `JaggedPass:object_events:001` | 10,8 | [JaggedPass_EventScript_Eric](../../baseline/source/data/maps/JaggedPass/scripts.inc#L184) — JaggedPass_EventScript_Eric at (10,8); MT. CHIMNEY's JAGGED PASS… //  Now this is what I've always wanted /  in a mountain. //  This jagged bumpiness… /  It rocks my soul! / Losing left me bitter! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `JaggedPass:object_events:002` | 16,35 | [JaggedPass_EventScript_Ethan](../../baseline/source/data/maps/JaggedPass/scripts.inc#L209) — JaggedPass_EventScript_Ethan at (16,35); JAGGED PASS is hard to walk on. /  It's a good place for training. / It was all over while we were still /  trying to find a good footing… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `JaggedPass:object_events:003` | 23,24 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ABSOLITE; root Common_EventScript_FindItem; flag FLAG_ITEM_JAGGED_PASS_ABSOLITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `JaggedPass:object_events:004` | 10,21 | [JaggedPass_EventScript_Diana](../../baseline/source/data/maps/JaggedPass/scripts.inc#L189) — JaggedPass_EventScript_Diana at (10,21); This place isn't your casual hike. /  It's not suited for a picnic. / Ohhh, no! /  The ground is too bumpy… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `JaggedPass:object_events:005` | 16,19 | [JaggedPass_EventScript_MagmaHideoutGuard](../../baseline/source/data/maps/JaggedPass/scripts.inc#L73) — JaggedPass_EventScript_MagmaHideoutGuard at (16,19); Wah! What are you doing up here? //  What am I doing up here? Guarding a /  path. Badly, apparently. Fight me! / Urrrgh… Should've ducked into the /  HIDEOUT while I could. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `JaggedPass:object_events:006` | 14,25 | [JaggedPass_EventScript_Autumn](../../baseline/source/data/maps/JaggedPass/scripts.inc#L234) — JaggedPass_EventScript_Autumn at (14,25); I climb this hill every day. /  I have confidence in my strength! / Hmm… /  What went wrong? | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `JaggedPass:object_events:007` | 18,25 | [JaggedPass_EventScript_Julio](../../baseline/source/data/maps/JaggedPass/scripts.inc#L229) — JaggedPass_EventScript_Julio at (18,25); Aiyeeh! It's awfully scary to shoot /  down the mountain in one go! / I feel like I'm falling apart… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `JaggedPass:object_events:008` | 12,29 | [JaggedPass_EventScript_VialChansey](../../baseline/source/data/maps/JaggedPass/scripts.inc#L104) — JaggedPass_EventScript_VialChansey at (12,29); Blob vanished into ASHEN WOODS. /  The HEAL BALL is still warm. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `JaggedPass:object_events:009` | 8,10 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_SCRAFTINITE; root Common_EventScript_FindItem; flag FLAG_HIDDEN_ITEM_JAGGED_PASS_SCRAFTINITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `JaggedPass:object_events:010` | 7,29 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_SCOVILLAINITE; root Common_EventScript_FindItem; flag FLAG_HIDDEN_ITEM_JAGGED_PASS_SCOVILLAINITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `JaggedPass:coord_events:001` | 13,15 | Coordinate weather at (13,15); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `JaggedPass:coord_events:002` | 21,12 | Coordinate weather at (21,12); weather COORD_EVENT_WEATHER_VOLCANIC_ASH. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `JaggedPass:coord_events:003` | 14,15 | Coordinate weather at (14,15); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `JaggedPass:coord_events:004` | 18,17 | Coordinate weather at (18,17); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `JaggedPass:coord_events:005` | 22,19 | Coordinate weather at (22,19); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `JaggedPass:coord_events:006` | 21,15 | [JaggedPass_EventScript_OpenMagmaHideout](../../baseline/source/data/maps/JaggedPass/scripts.inc#L47) — Coordinate trigger at (21,15); VAR_JAGGED_PASS_STATE == 1 invokes JaggedPass_EventScript_OpenMagmaHideout. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `JaggedPass:coord_events:007` | 22,20 | [JaggedPass_EventScript_OpenMagmaHideout](../../baseline/source/data/maps/JaggedPass/scripts.inc#L47) — Coordinate trigger at (22,20); VAR_JAGGED_PASS_STATE == 1 invokes JaggedPass_EventScript_OpenMagmaHideout. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `JaggedPass:coord_events:008` | 21,20 | [JaggedPass_EventScript_OpenMagmaHideout](../../baseline/source/data/maps/JaggedPass/scripts.inc#L47) — Coordinate trigger at (21,20); VAR_JAGGED_PASS_STATE == 1 invokes JaggedPass_EventScript_OpenMagmaHideout. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `JaggedPass:coord_events:009` | 14,15 | [JaggedPass_EventScript_OpenMagmaHideout](../../baseline/source/data/maps/JaggedPass/scripts.inc#L47) — Coordinate trigger at (14,15); VAR_JAGGED_PASS_STATE == 1 invokes JaggedPass_EventScript_OpenMagmaHideout. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `JaggedPass:coord_events:010` | 13,15 | [JaggedPass_EventScript_OpenMagmaHideout](../../baseline/source/data/maps/JaggedPass/scripts.inc#L47) — Coordinate trigger at (13,15); VAR_JAGGED_PASS_STATE == 1 invokes JaggedPass_EventScript_OpenMagmaHideout. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `JaggedPass:warp_events:001` | 14,40 | Warp from (14,40, elevation 3) to MAP_ROUTE112 warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `JaggedPass:warp_events:002` | 15,40 | Warp from (15,40, elevation 3) to MAP_ROUTE112 warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `JaggedPass:warp_events:003` | 13,5 | Warp from (13,5, elevation 3) to MAP_MT_CHIMNEY warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `JaggedPass:warp_events:004` | 14,5 | Warp from (14,5, elevation 3) to MAP_MT_CHIMNEY warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `JaggedPass:warp_events:005` | 16,18 | Warp from (16,18, elevation 0) to MAP_MAGMA_HIDEOUT_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `JaggedPass:warp_events:006` | 7,28 | Warp from (7,28, elevation 3) to MAP_EMBER_PATH warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `JaggedPass:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [JaggedPass_OnResume](../../baseline/source/data/maps/JaggedPass/scripts.inc#L7) — MAP_SCRIPT_ON_RESUME calls JaggedPass_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `JaggedPass:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [JaggedPass_OnTransition](../../baseline/source/data/maps/JaggedPass/scripts.inc#L21) — MAP_SCRIPT_ON_TRANSITION calls JaggedPass_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `JaggedPass:map_scripts:003` | MAP_SCRIPT_ON_LOAD | [JaggedPass_OnLoad](../../baseline/source/data/maps/JaggedPass/scripts.inc#L38) — MAP_SCRIPT_ON_LOAD calls JaggedPass_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
