# PetalburgCity

**KEEP.** Preserve Norman/Wally story and lake-town identity, including party save/restore in the capture demonstration. Expert battle capability is present immediately; young-character narrative is not a difficulty curriculum.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/PetalburgCity/map.json) · [Scripts](../../baseline/source/data/maps/PetalburgCity/scripts.inc)

## Current map contract

`MAP_PETALBURG_CITY` · `LAYOUT_PETALBURG_CITY` · `WEATHER_SUNNY` · `MUS_PETALBURG`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `PetalburgCity:object_events:001` | 16,18 | [PetalburgCity_EventScript_WallysMom](../../baseline/source/data/maps/PetalburgCity/scripts.inc#L92) — PetalburgCity_EventScript_WallysMom at (16,18); Where has our WALLY gone? //  We have to leave for VERDANTURF TOWN /  very soon… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PetalburgCity:object_events:002` | 15,10 | Passive/staged OBJ_EVENT_GFX_WALLY at (15,10); visibility flag FLAG_HIDE_PETALBURG_CITY_WALLY; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `PetalburgCity:object_events:003` | 8,22 | [PetalburgCity_EventScript_Boy](../../baseline/source/data/maps/PetalburgCity/scripts.inc#L82) — PetalburgCity_EventScript_Boy at (8,22); My face is reflected in the water. //  It's a shining grin full of hope… //  Or it could be a look of somber silence /  struggling with fear… //  What do you see reflected in your face? | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `PetalburgCity:object_events:004` | 20,10 | [PetalburgCity_EventScript_Gentleman](../../baseline/source/data/maps/PetalburgCity/scripts.inc#L228) — PetalburgCity_EventScript_Gentleman at (20,10); Let's say you have six POKéMON. /  If you catch another one… //  It is automatically sent to a STORAGE /  BOX over a PC connection. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PetalburgCity:object_events:005` | 15,10 | Passive/staged OBJ_EVENT_GFX_POKEFAN_M at (15,10); visibility flag FLAG_HIDE_PETALBURG_CITY_WALLYS_DAD; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `PetalburgCity:object_events:006` | 19,2 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MAX_REVIVE; root Common_EventScript_FindItem; flag FLAG_ITEM_PETALBURG_CITY_MAX_REVIVE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `PetalburgCity:object_events:007` | 3,28 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ETHER; root Common_EventScript_FindItem; flag FLAG_ITEM_PETALBURG_CITY_ETHER. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `PetalburgCity:object_events:008` | 12,15 | [PetalburgCity_EventScript_GymBoy](../../baseline/source/data/maps/PetalburgCity/scripts.inc#L696) — PetalburgCity_EventScript_GymBoy at (12,15); Hiya! Are you maybe… /  A rookie TRAINER? //  Do you know what POKéMON TRAINERS /  do when they reach a new town? //  They first check what kind of GYM /  is in the town. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PetalburgCity:object_events:009` | 13,12 | Passive/staged OBJ_EVENT_GFX_SCOTT at (13,12); visibility flag FLAG_HIDE_PETALBURG_CITY_SCOTT; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `PetalburgCity:coord_events:001` | 8,10 | [PetalburgCity_EventScript_ShowGymToPlayer0](../../baseline/source/data/maps/PetalburgCity/scripts.inc#L236) — Coordinate trigger at (8,10); VAR_PETALBURG_CITY_STATE == 0 invokes PetalburgCity_EventScript_ShowGymToPlayer0. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `PetalburgCity:coord_events:002` | 8,11 | [PetalburgCity_EventScript_ShowGymToPlayer1](../../baseline/source/data/maps/PetalburgCity/scripts.inc#L242) — Coordinate trigger at (8,11); VAR_PETALBURG_CITY_STATE == 0 invokes PetalburgCity_EventScript_ShowGymToPlayer1. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `PetalburgCity:coord_events:003` | 8,12 | [PetalburgCity_EventScript_ShowGymToPlayer2](../../baseline/source/data/maps/PetalburgCity/scripts.inc#L248) — Coordinate trigger at (8,12); VAR_PETALBURG_CITY_STATE == 0 invokes PetalburgCity_EventScript_ShowGymToPlayer2. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `PetalburgCity:coord_events:004` | 8,13 | [PetalburgCity_EventScript_ShowGymToPlayer3](../../baseline/source/data/maps/PetalburgCity/scripts.inc#L254) — Coordinate trigger at (8,13); VAR_PETALBURG_CITY_STATE == 0 invokes PetalburgCity_EventScript_ShowGymToPlayer3. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `PetalburgCity:coord_events:005` | 4,10 | [PetalburgCity_EventScript_Scott0](../../baseline/source/data/maps/PetalburgCity/scripts.inc#L498) — Coordinate trigger at (4,10); VAR_SCOTT_PETALBURG_ENCOUNTER == 0 invokes PetalburgCity_EventScript_Scott0. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `PetalburgCity:coord_events:006` | 4,11 | [PetalburgCity_EventScript_Scott1](../../baseline/source/data/maps/PetalburgCity/scripts.inc#L506) — Coordinate trigger at (4,11); VAR_SCOTT_PETALBURG_ENCOUNTER == 0 invokes PetalburgCity_EventScript_Scott1. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `PetalburgCity:coord_events:007` | 4,12 | [PetalburgCity_EventScript_Scott2](../../baseline/source/data/maps/PetalburgCity/scripts.inc#L514) — Coordinate trigger at (4,12); VAR_SCOTT_PETALBURG_ENCOUNTER == 0 invokes PetalburgCity_EventScript_Scott2. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `PetalburgCity:coord_events:008` | 4,13 | [PetalburgCity_EventScript_Scott3](../../baseline/source/data/maps/PetalburgCity/scripts.inc#L522) — Coordinate trigger at (4,13); VAR_SCOTT_PETALBURG_ENCOUNTER == 0 invokes PetalburgCity_EventScript_Scott3. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `PetalburgCity:bg_events:001` | 17,10 | [PetalburgCity_EventScript_GymSign](../../baseline/source/data/maps/PetalburgCity/scripts.inc#L220) — PetalburgCity_EventScript_GymSign at (17,10); PETALBURG CITY POKéMON GYM /  LEADER: NORMAN /  “A man in pursuit of power!” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `PetalburgCity:bg_events:002` | 26,12 | [Common_EventScript_ShowPokemartSign](../../baseline/source/data/event_scripts.s#L1173) — Common_EventScript_ShowPokemartSign at (26,12); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `PetalburgCity:bg_events:003` | 21,16 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (21,16); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `PetalburgCity:bg_events:004` | 17,16 | [PetalburgCity_EventScript_CitySign](../../baseline/source/data/maps/PetalburgCity/scripts.inc#L224) — PetalburgCity_EventScript_CitySign at (17,16); PETALBURG CITY /  “Where people mingle with nature.” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `PetalburgCity:bg_events:005` | 22,16 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (22,16); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `PetalburgCity:bg_events:006` | 27,12 | [Common_EventScript_ShowPokemartSign](../../baseline/source/data/event_scripts.s#L1173) — Common_EventScript_ShowPokemartSign at (27,12); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `PetalburgCity:bg_events:007` | 8,9 | [PetalburgCity_EventScript_WallyHouseSign](../../baseline/source/data/maps/PetalburgCity/scripts.inc#L232) — PetalburgCity_EventScript_WallyHouseSign at (8,9); WALLY'S HOUSE | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `PetalburgCity:bg_events:008` | 11,29 | Hidden ITEM_ULTRA_BALL at (11,29); persistent flag FLAG_HIDDEN_ITEM_PETALBURG_CITY_ULTRA_BALL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `PetalburgCity:warp_events:001` | 10,19 | Warp from (10,19, elevation 0) to MAP_PETALBURG_CITY_HOUSE1 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgCity:warp_events:002` | 7,5 | Warp from (7,5, elevation 0) to MAP_PETALBURG_CITY_WALLYS_HOUSE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgCity:warp_events:003` | 15,8 | Warp from (15,8, elevation 0) to MAP_PETALBURG_CITY_GYM warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgCity:warp_events:004` | 20,16 | Warp from (20,16, elevation 0) to MAP_PETALBURG_CITY_POKEMON_CENTER_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgCity:warp_events:005` | 20,24 | Warp from (20,24, elevation 0) to MAP_PETALBURG_CITY_HOUSE2 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgCity:warp_events:006` | 25,12 | Warp from (25,12, elevation 0) to MAP_PETALBURG_CITY_MART warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgCity:connections:001` | left | left connection to MAP_ROUTE104, offset -50. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgCity:connections:002` | right | right connection to MAP_ROUTE102, offset 10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgCity:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [PetalburgCity_OnTransition](../../baseline/source/data/maps/PetalburgCity/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls PetalburgCity_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `PetalburgCity:map_scripts:002` | MAP_SCRIPT_ON_FRAME_TABLE | [PetalburgCity_OnFrame](../../baseline/source/data/maps/PetalburgCity/scripts.inc#L27) — MAP_SCRIPT_ON_FRAME_TABLE calls PetalburgCity_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
