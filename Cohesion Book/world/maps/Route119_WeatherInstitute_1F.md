# Route119_WeatherInstitute_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/Route119_WeatherInstitute_1F/map.json) · [Scripts](../../baseline/source/data/maps/Route119_WeatherInstitute_1F/scripts.inc)

## Current map contract

`MAP_ROUTE119_WEATHER_INSTITUTE_1F` · `LAYOUT_ROUTE119_WEATHER_INSTITUTE_1F` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route119_WeatherInstitute_1F:object_events:001` | 15,3 | [Route119_WeatherInstitute_1F_EventScript_Grunt1](../../baseline/source/data/maps/Route119_WeatherInstitute_1F/scripts.inc#L61) — Route119_WeatherInstitute_1F_EventScript_Grunt1 at (15,3); The INSTITUTE mapped every pressure /  shift before the storm arrived. //  ARCHIE needs those maps. You are not /  interrupting this current again. / You read the pressure change first… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route119_WeatherInstitute_1F:object_events:002` | 10,5 | [Route119_WeatherInstitute_1F_EventScript_Grunt4](../../baseline/source/data/maps/Route119_WeatherInstitute_1F/scripts.inc#L66) — Route119_WeatherInstitute_1F_EventScript_Grunt4 at (10,5); You crossed the flooded grass alone? /  Then this indoor front will not stop you. //  I will change the forecast myself. / My forecast failed completely… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route119_WeatherInstitute_1F:object_events:003` | 5,4 | [Route119_WeatherInstitute_1F_EventScript_InstituteWorker2](../../baseline/source/data/maps/Route119_WeatherInstitute_1F/scripts.inc#L49) — Route119_WeatherInstitute_1F_EventScript_InstituteWorker2 at (5,4); You saved our staff and our records. //  AQUA was tracing the same anomaly that /  links our weather stations to the SIGNS. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route119_WeatherInstitute_1F:object_events:004` | 2,11 | [Route119_WeatherInstitute_1F_EventScript_InstituteWorker1](../../baseline/source/data/maps/Route119_WeatherInstitute_1F/scripts.inc#L28) — Route119_WeatherInstitute_1F_EventScript_InstituteWorker1 at (2,11); The PROFESSOR loves rain. /  That's a fact. //  But if it keeps raining, people will be in /  trouble. That's another fact. //  And thus, the PROFESSOR is studying /  if the rain can be put to good use. / On the 2nd floor of the INSTITUTE, /  we study the weather patterns over /  the HOENN region. //  We've been noticing temporary and /  isolated cases of droughts and /  heavy rain lately… | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route119_WeatherInstitute_1F:object_events:005` | 14,11 | [Route119_WeatherInstitute_1F_EventScript_LittleBoy](../../baseline/source/data/maps/Route119_WeatherInstitute_1F/scripts.inc#L14) — Route119_WeatherInstitute_1F_EventScript_LittleBoy at (14,11); Those AQUA people took the researchers /  upstairs while I was asleep! //  Can you go up and help them? / Wow, you're really strong! //  I wish I could be a POKéMON TRAINER /  like you! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route119_WeatherInstitute_1F:bg_events:001` | 1,2 | [Route119_WeatherInstitute_1F_EventScript_Bed](../../baseline/source/data/maps/Route119_WeatherInstitute_1F/scripts.inc#L53) — Route119_WeatherInstitute_1F_EventScript_Bed at (1,2); There's a bed… /  Let's take a rest. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route119_WeatherInstitute_1F:bg_events:002` | 1,3 | [Route119_WeatherInstitute_1F_EventScript_Bed](../../baseline/source/data/maps/Route119_WeatherInstitute_1F/scripts.inc#L53) — Route119_WeatherInstitute_1F_EventScript_Bed at (1,3); There's a bed… /  Let's take a rest. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route119_WeatherInstitute_1F:bg_events:003` | 0,2 | [Route119_WeatherInstitute_1F_EventScript_Bed](../../baseline/source/data/maps/Route119_WeatherInstitute_1F/scripts.inc#L53) — Route119_WeatherInstitute_1F_EventScript_Bed at (0,2); There's a bed… /  Let's take a rest. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route119_WeatherInstitute_1F:bg_events:004` | 0,3 | [Route119_WeatherInstitute_1F_EventScript_Bed](../../baseline/source/data/maps/Route119_WeatherInstitute_1F/scripts.inc#L53) — Route119_WeatherInstitute_1F_EventScript_Bed at (0,3); There's a bed… /  Let's take a rest. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route119_WeatherInstitute_1F:warp_events:001` | 9,12 | Warp from (9,12, elevation 0) to MAP_ROUTE119 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route119_WeatherInstitute_1F:warp_events:002` | 10,12 | Warp from (10,12, elevation 0) to MAP_ROUTE119 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route119_WeatherInstitute_1F:warp_events:003` | 17,1 | Warp from (17,1, elevation 0) to MAP_ROUTE119_WEATHER_INSTITUTE_2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route119_WeatherInstitute_1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route119_WeatherInstitute_1F_OnTransition](../../baseline/source/data/maps/Route119_WeatherInstitute_1F/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route119_WeatherInstitute_1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
