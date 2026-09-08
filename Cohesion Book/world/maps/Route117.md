# Route117

**REVISE.** Keep meadow, Daycare access and independent Audinite gift. The Daycare’s egg objective is an exploration reward with one clear qualifying hatch, not a stat-preparation requirement.

[Regional experience](../03-mauville.md) · [Map source](../../baseline/source/data/maps/Route117/map.json) · [Scripts](../../baseline/source/data/maps/Route117/scripts.inc)

## Current map contract

`MAP_ROUTE117` · `LAYOUT_ROUTE117` · `WEATHER_SUNNY` · `MUS_ROUTE110`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route117:object_events:001` | 13,13 | [Route117_EventScript_Woman](../../baseline/source/data/maps/Route117/scripts.inc#L52) — Route117_EventScript_Woman at (13,13); AUDINO often wander through these /  flowers. They're wonderful partners. //  Take this AUDINITE. With a MEGA RING, /  it helps an AUDINO Mega Evolve. / What do you think? /  Aren't these flowers pretty? //  I planted them all! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route117:object_events:002` | 25,5 | [Route117_EventScript_LittleBoy](../../baseline/source/data/maps/Route117/scripts.inc#L74) — Route117_EventScript_LittleBoy at (25,5); The air is tasty here! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route117:object_events:003` | 47,4 | [Route117_EventScript_DaycareMan](../../baseline/source/data/scripts/day_care.inc#L1) — Route117_EventScript_DaycareMan at (47,4); shared behavior DAYCARE | **KEEP** · [W-C-DAYCARE](../common-contracts.md#w-c-daycare) |
| `Route117:object_events:004` | 33,3 | Passive/staged OBJ_EVENT_GFX_ZIGZAGOON_2 at (33,3); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route117:object_events:005` | 39,4 | Passive/staged OBJ_EVENT_GFX_KECLEON at (39,4); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route117:object_events:006` | 42,2 | Passive/staged OBJ_EVENT_GFX_AZUMARILL at (42,2); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route117:object_events:007` | 49,2 | Passive/staged OBJ_EVENT_GFX_PIKACHU at (49,2); visibility flag FLAG_TEMP_2; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route117:object_events:008` | 38,16 | [Route117_EventScript_Dylan](../../baseline/source/data/maps/Route117/scripts.inc#L136) — Route117_EventScript_Dylan at (38,16); Training tip: take a breath. /  That is also my swimming plan. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route117:object_events:009` | 8,10 | [Route117_EventScript_Lydia](../../baseline/source/data/maps/Route117/scripts.inc#L115) — Route117_EventScript_Lydia at (8,10); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route117:object_events:010` | 33,11 | [Route117_EventScript_Isaac](../../baseline/source/data/maps/Route117/scripts.inc#L94) — Route117_EventScript_Isaac at (33,11); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route117:object_events:011` | 41,13 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (41,13); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route117:object_events:012` | 42,13 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (42,13); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route117:object_events:013` | 43,13 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (43,13); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route117:object_events:014` | 16,18 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_CHESNAUGHTITE; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_117_CHESNAUGHTITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route117:object_events:015` | 15,2 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (15,2); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route117:object_events:016` | 9,1 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_GRENINJITE; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_117_GRENINJITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route117:object_events:017` | 26,13 | [Route117_EventScript_Maria](../../baseline/source/data/maps/Route117/scripts.inc#L152) — Route117_EventScript_Maria at (26,13); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route117:object_events:018` | 17,12 | [Route117_EventScript_Derek](../../baseline/source/data/maps/Route117/scripts.inc#L173) — Route117_EventScript_Derek at (17,12); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route117:object_events:019` | 43,6 | [Route117_EventScript_Meg](../../baseline/source/data/maps/Route117/scripts.inc#L197) — Route117_EventScript_Meg at (43,6); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route117:object_events:020` | 42,6 | [Route117_EventScript_Anna](../../baseline/source/data/maps/Route117/scripts.inc#L178) — Route117_EventScript_Anna at (42,6); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route117:object_events:021` | 48,10 | [Route117_EventScript_Girl](../../baseline/source/data/maps/Route117/scripts.inc#L78) — Route117_EventScript_Girl at (48,10); I left two compatible POKéMON at the /  DAY CARE, and they produced an EGG! //  Center tutors handle moves. The DAY CARE /  is where new partners inherit traits. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route117:object_events:022` | 15,4 | [Route117_EventScript_Brandi](../../baseline/source/data/maps/Route117/scripts.inc#L221) — Route117_EventScript_Brandi at (15,4); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route117:object_events:023` | 21,4 | [Route117_EventScript_Aisha](../../baseline/source/data/maps/Route117/scripts.inc#L226) — Route117_EventScript_Aisha at (21,4); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route117:object_events:024` | 16,4 | [Route117_EventScript_Melina](../../baseline/source/data/maps/Route117/scripts.inc#L216) — Route117_EventScript_Melina at (16,4); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route117:object_events:025` | 4,18 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_QUICK_BALL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_ROUTE117_QUICK_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route117:object_events:026` | 3,18 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_CHARIZARDITE_Y; root Common_EventScript_FindItem; flag FLAG_HIDDEN_ITEM_ROUTE_117_CHARIZARDITE_Y. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route117:object_events:027` | 13,8 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (13,8); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `Route117:bg_events:001` | 16,6 | [Route117_EventScript_RouteSignVerdanturf](../../baseline/source/data/maps/Route117/scripts.inc#L82) — Route117_EventScript_RouteSignVerdanturf at (16,6); ROUTE 117 /  {LEFT_ARROW} VERDANTURF TOWN | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route117:bg_events:002` | 49,12 | [Route117_EventScript_RouteSignMauville](../../baseline/source/data/maps/Route117/scripts.inc#L86) — Route117_EventScript_RouteSignMauville at (49,12); ROUTE 117 /  {RIGHT_ARROW} MAUVILLE CITY | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route117:bg_events:003` | 49,5 | [Route117_EventScript_DayCareSign](../../baseline/source/data/maps/Route117/scripts.inc#L90) — Route117_EventScript_DayCareSign at (49,5); POKéMON DAY CARE /  “Let us raise your POKéMON.” | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route117:warp_events:001` | 51,5 | Warp from (51,5, elevation 0) to MAP_ROUTE117_POKEMON_DAY_CARE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route117:connections:001` | left | left connection to MAP_VERDANTURF_TOWN, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route117:connections:002` | right | right connection to MAP_MAUVILLE_CITY, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route117:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route117_OnTransition](../../baseline/source/data/maps/Route117/scripts.inc#L8) — MAP_SCRIPT_ON_TRANSITION calls Route117_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
