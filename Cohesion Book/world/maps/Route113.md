# Route113

**REVISE.** Preserve ash collection and current Kingambit clue. The newer held-Leader’s-Crest evolution is already in snapshot and is not an acquisition defect. The Glass Workshop is optional regional craft, not a battle-stat grind.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/Route113/map.json) · [Scripts](../../baseline/source/data/maps/Route113/scripts.inc)

## Current map contract

`MAP_ROUTE113` · `LAYOUT_ROUTE113` · `WEATHER_SUNNY` · `MUS_ROUTE113`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route113:object_events:001` | 66,12 | [Route113_EventScript_NinjaBoy](../../baseline/source/data/maps/Route113/scripts.inc#L29) — Route113_EventScript_NinjaBoy at (66,12); PAWNIARD leave little tracks in the ash. /  They become BISHARP at Lv. 52. //  Let BISHARP hold a LEADER'S CREST, /  then level it up for KINGAMBIT. /  The crest is used up when it evolves. //  The free Evolution Items archive has /  crests. Show a battle supplier your /  MEGA RING to open the archive. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route113:object_events:002` | 36,10 | [Route113_EventScript_Gentleman](../../baseline/source/data/maps/Route113/scripts.inc#L25) — Route113_EventScript_Gentleman at (36,10); Wahahaha! Today's technology is a /  wondrous thing! //  Take this volcanic ash here. /  It can be fashioned into glass. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route113:object_events:003` | 62,8 | [Route113_EventScript_Jaylen](../../baseline/source/data/maps/Route113/scripts.inc#L49) — Route113_EventScript_Jaylen at (62,8); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route113:object_events:004` | 21,11 | [Route113_EventScript_Dillon](../../baseline/source/data/maps/Route113/scripts.inc#L54) — Route113_EventScript_Dillon at (21,11); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route113:object_events:005` | 51,11 | [Route113_EventScript_Madeline](../../baseline/source/data/maps/Route113/scripts.inc#L59) — Route113_EventScript_Madeline at (51,11); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route113:object_events:006` | 53,7 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_FALINKSITE; root Common_EventScript_FindItem; flag FLAG_EC_MEGA_REWARD_FALINKSITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route113:object_events:007` | 79,5 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_EXCADRITE; root Common_EventScript_FindItem; flag FLAG_EC_MEGA_REWARD_EXCADRITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route113:object_events:008` | 29,6 | [Route113_EventScript_Lao](../../baseline/source/data/maps/Route113/scripts.inc#L80) — Route113_EventScript_Lao at (29,6); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route113:object_events:009` | 71,2 | [Route113_EventScript_Lung](../../baseline/source/data/maps/Route113/scripts.inc#L101) — Route113_EventScript_Lung at (71,2); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route113:object_events:010` | 45,6 | [Route113_EventScript_Tori](../../baseline/source/data/maps/Route113/scripts.inc#L106) — Route113_EventScript_Tori at (45,6); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route113:object_events:011` | 46,6 | [Route113_EventScript_Tia](../../baseline/source/data/maps/Route113/scripts.inc#L111) — Route113_EventScript_Tia at (46,6); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route113:object_events:012` | 15,15 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_HYPER_POTION; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_113_HYPER_POTION. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route113:object_events:013` | 75,3 | [Route113_EventScript_Wyatt](../../baseline/source/data/maps/Route113/scripts.inc#L131) — Route113_EventScript_Wyatt at (75,3); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route113:object_events:014` | 71,4 | [Route113_EventScript_Lawrence](../../baseline/source/data/maps/Route113/scripts.inc#L126) — Route113_EventScript_Lawrence at (71,4); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route113:object_events:015` | 7,6 | [Route113_EventScript_Sophie](../../baseline/source/data/maps/Route113/scripts.inc#L116) — Route113_EventScript_Sophie at (7,6); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route113:object_events:016` | 7,13 | [Route113_EventScript_Coby](../../baseline/source/data/maps/Route113/scripts.inc#L121) — Route113_EventScript_Coby at (7,13); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route113:coord_events:001` | 19,11 | Coordinate weather at (19,11); weather COORD_EVENT_WEATHER_VOLCANIC_ASH. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:002` | 19,10 | Coordinate weather at (19,10); weather COORD_EVENT_WEATHER_VOLCANIC_ASH. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:003` | 19,12 | Coordinate weather at (19,12); weather COORD_EVENT_WEATHER_VOLCANIC_ASH. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:004` | 19,13 | Coordinate weather at (19,13); weather COORD_EVENT_WEATHER_VOLCANIC_ASH. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:005` | 86,9 | Coordinate weather at (86,9); weather COORD_EVENT_WEATHER_VOLCANIC_ASH. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:006` | 85,10 | Coordinate weather at (85,10); weather COORD_EVENT_WEATHER_VOLCANIC_ASH. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:007` | 85,11 | Coordinate weather at (85,11); weather COORD_EVENT_WEATHER_VOLCANIC_ASH. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:008` | 14,10 | Coordinate weather at (14,10); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:009` | 14,11 | Coordinate weather at (14,11); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:010` | 14,12 | Coordinate weather at (14,12); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:011` | 14,13 | Coordinate weather at (14,13); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:012` | 94,8 | Coordinate weather at (94,8); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:013` | 94,9 | Coordinate weather at (94,9); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:014` | 94,10 | Coordinate weather at (94,10); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:015` | 94,11 | Coordinate weather at (94,11); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:016` | 19,14 | Coordinate weather at (19,14); weather COORD_EVENT_WEATHER_VOLCANIC_ASH. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:017` | 87,8 | Coordinate weather at (87,8); weather COORD_EVENT_WEATHER_VOLCANIC_ASH. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:018` | 87,6 | Coordinate weather at (87,6); weather COORD_EVENT_WEATHER_VOLCANIC_ASH. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:coord_events:019` | 87,7 | Coordinate weather at (87,7); weather COORD_EVENT_WEATHER_VOLCANIC_ASH. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route113:bg_events:001` | 85,6 | [Route113_EventScript_RouteSign111](../../baseline/source/data/maps/Route113/scripts.inc#L33) — Route113_EventScript_RouteSign111 at (85,6); ROUTE 113 /  {RIGHT_ARROW} ROUTE 111 | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route113:bg_events:002` | 12,9 | [Route113_EventScript_RouteSignFallarbor](../../baseline/source/data/maps/Route113/scripts.inc#L37) — Route113_EventScript_RouteSignFallarbor at (12,9); ROUTE 113 /  {LEFT_ARROW} FALLARBOR TOWN | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route113:bg_events:003` | 58,4 | [Route113_EventScript_TrainerTipsRegisterKeyItems](../../baseline/source/data/maps/Route113/scripts.inc#L45) — Route113_EventScript_TrainerTipsRegisterKeyItems at (58,4); TRAINER TIPS //  You may register one of the KEY ITEMS /  in your BAG as SELECT. //  Simply press SELECT to use /  the registered item conveniently. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route113:bg_events:004` | 31,5 | [Route113_EventScript_GlassWorkshopSign](../../baseline/source/data/maps/Route113/scripts.inc#L41) — Route113_EventScript_GlassWorkshopSign at (31,5); GLASS WORKSHOP /  “Turning Volcanic Ash into Glass Items” | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route113:bg_events:005` | 49,8 | None at (49,8); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route113:bg_events:006` | 66,3 | Hidden ITEM_ETHER at (66,3); persistent flag FLAG_HIDDEN_ITEM_ROUTE_113_ETHER. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route113:bg_events:007` | 22,5 | Hidden ITEM_ULTRA_BALL at (22,5); persistent flag FLAG_HIDDEN_ITEM_ROUTE_113_ULTRA_BALL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route113:bg_events:008` | 73,3 | Hidden ITEM_NUGGET at (73,3); persistent flag FLAG_HIDDEN_ITEM_ROUTE_113_NUGGET. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route113:warp_events:001` | 33,5 | Warp from (33,5, elevation 0) to MAP_ROUTE113_GLASS_WORKSHOP warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route113:warp_events:002` | 41,12 | Warp from (41,12, elevation 0) to MAP_TERRA_CAVE_ENTRANCE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route113:warp_events:003` | 88,5 | Warp from (88,5, elevation 0) to MAP_TERRA_CAVE_ENTRANCE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route113:connections:001` | down | down connection to MAP_ROUTE112, offset 60. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route113:connections:002` | left | left connection to MAP_FALLARBOR_TOWN, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route113:connections:003` | right | right connection to MAP_ROUTE111, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route113:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [Route113_OnResume](../../baseline/source/data/maps/Route113/scripts.inc#L6) — MAP_SCRIPT_ON_RESUME calls Route113_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route113:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [Route113_OnTransition](../../baseline/source/data/maps/Route113/scripts.inc#L10) — MAP_SCRIPT_ON_TRANSITION calls Route113_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
