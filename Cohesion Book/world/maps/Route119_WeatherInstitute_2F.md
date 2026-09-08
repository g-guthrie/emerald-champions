# Route119_WeatherInstitute_2F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/Route119_WeatherInstitute_2F/map.json) · [Scripts](../../baseline/source/data/maps/Route119_WeatherInstitute_2F/scripts.inc)

## Current map contract

`MAP_ROUTE119_WEATHER_INSTITUTE_2F` · `LAYOUT_ROUTE119_WEATHER_INSTITUTE_2F` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route119_WeatherInstitute_2F:object_events:001` | 15,6 | [Route119_WeatherInstitute_2F_EventScript_Grunt2](../../baseline/source/data/maps/Route119_WeatherInstitute_2F/scripts.inc#L31) — Route119_WeatherInstitute_2F_EventScript_Grunt2 at (15,6); CASTFORM reads a battlefield before the /  first drop falls. AQUA needs that gift. //  You will not reach the research wing. / You changed the forecast around me… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route119_WeatherInstitute_2F:object_events:002` | 10,8 | [Route119_WeatherInstitute_2F_EventScript_Grunt3](../../baseline/source/data/maps/Route119_WeatherInstitute_2F/scripts.inc#L36) — Route119_WeatherInstitute_2F_EventScript_Grunt3 at (10,8); Rare data, rare POKéMON, hidden currents. /  AQUA follows what the world conceals. / You exposed every hidden line… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route119_WeatherInstitute_2F:object_events:003` | 4,6 | [Route119_WeatherInstitute_2F_EventScript_Shelly](../../baseline/source/data/maps/Route119_WeatherInstitute_2F/scripts.inc#L41) — Route119_WeatherInstitute_2F_EventScript_Shelly at (4,6); SHELLY: We came for weather records, /  but they chart the SIGNS as well. //  AQUA wins by changing the board before /  the opponent knows what it became. //  Let's see how quickly you read a storm. / You found the calm inside my storm… | **REPAIR** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01, W-SIGN-OPTIONAL |
| `Route119_WeatherInstitute_2F:object_events:004` | 0,6 | Passive/staged OBJ_EVENT_GFX_MAN_4 at (0,6); visibility flag FLAG_HIDE_WEATHER_INSTITUTE_2F_WORKERS; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route119_WeatherInstitute_2F:object_events:005` | 18,6 | [Route119_WeatherInstitute_2F_EventScript_WeatherScientist](../../baseline/source/data/maps/Route119_WeatherInstitute_2F/scripts.inc#L217) — Route119_WeatherInstitute_2F_EventScript_WeatherScientist at (18,6); You saved our staff and our data. Take /  this CASTFORM as a research partner. //  It reads weather directly, and old desert /  records say the land itself knows it. / {PLAYER} received CASTFORM! | **REPAIR** · [W-C-STORY](../common-contracts.md#w-c-story) · W-SIGN-OPTIONAL |
| `Route119_WeatherInstitute_2F:object_events:006` | 1,7 | Passive/staged OBJ_EVENT_GFX_MAN_4 at (1,7); visibility flag FLAG_HIDE_WEATHER_INSTITUTE_2F_WORKERS; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route119_WeatherInstitute_2F:object_events:007` | 16,6 | Passive/staged OBJ_EVENT_GFX_AQUA_MEMBER_M at (16,6); visibility flag FLAG_HIDE_WEATHER_INSTITUTE_2F_AQUA_GRUNT_M; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route119_WeatherInstitute_2F:object_events:008` | 19,6 | [Route119_WeatherInstitute_2F_EventScript_Grunt5](../../baseline/source/data/maps/Route119_WeatherInstitute_2F/scripts.inc#L26) — Route119_WeatherInstitute_2F_EventScript_Grunt5 at (19,6); You came for the weather POKéMON, too? //  Then you know a changing form can answer /  more fields than a rigid one. Show me. / Your answer changed one turn sooner… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route119_WeatherInstitute_2F:warp_events:001` | 17,1 | Warp from (17,1, elevation 0) to MAP_ROUTE119_WEATHER_INSTITUTE_1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route119_WeatherInstitute_2F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route119_WeatherInstitute_2F_OnTransition](../../baseline/source/data/maps/Route119_WeatherInstitute_2F/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route119_WeatherInstitute_2F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
