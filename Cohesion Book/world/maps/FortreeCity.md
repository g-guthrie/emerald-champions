# FortreeCity

**KEEP.** Preserve treetop paths, Kecleon/Scope access, east-road Feather gate and clear directions to Steven’s bridge. Distinctive canopy residents and free services coexist; no encounter scarcity is imposed to protect a type chapter.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/FortreeCity/map.json) · [Scripts](../../baseline/source/data/maps/FortreeCity/scripts.inc)

## Current map contract

`MAP_FORTREE_CITY` · `LAYOUT_FORTREE_CITY` · `WEATHER_SUNNY` · `MUS_FORTREE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `FortreeCity:object_events:001` | 31,3 | [FortreeCity_EventScript_Man](../../baseline/source/data/maps/FortreeCity/scripts.inc#L14) — FortreeCity_EventScript_Man at (31,3); No one believes me, but I saw this /  gigantic POKéMON in the sky. //  It seemed to squirm as it flew toward /  ROUTE 131. //  By the way… Sniff… /  Um… You, uh…smell singed. //  Were you at a volcano or something? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity:object_events:002` | 32,16 | [FortreeCity_EventScript_Girl](../../baseline/source/data/maps/FortreeCity/scripts.inc#L31) — FortreeCity_EventScript_Girl at (32,16); The ground absorbs rainwater, and /  trees grow by drinking that water… //  Our FORTREE CITY exists because /  there's both water and soil. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity:object_events:003` | 32,10 | [FortreeCity_EventScript_Woman](../../baseline/source/data/maps/FortreeCity/scripts.inc#L18) — FortreeCity_EventScript_Woman at (32,10); I want to go to the POKéMON GYM, /  but something's blocking the way. //  After all the bother I went through /  training on ROUTE 120… //  STEVEN is on the bridge east of town. /  He was studying something invisible. //  Maybe he can help us clear the GYM path! / I've got my pride-and-joy POKéMON /  with me. This time, I'll beat WINONA. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `FortreeCity:object_events:004` | 11,14 | [FortreeCity_EventScript_Boy](../../baseline/source/data/maps/FortreeCity/scripts.inc#L39) — FortreeCity_EventScript_Boy at (11,14); Living on top of trees is okay. //  But sometimes BUG POKéMON come in /  through windows. /  It can be really startling. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity:object_events:005` | 8,10 | [FortreeCity_EventScript_OldMan](../../baseline/source/data/maps/FortreeCity/scripts.inc#L35) — FortreeCity_EventScript_OldMan at (8,10); The CITY consists of homes built on /  trees. //  Perhaps because of that lifestyle, /  everyone is healthy and lively. //  Why, even myself--I feel as if I've /  grown thirty years younger. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity:object_events:006` | 9,16 | [FortreeCity_EventScript_GameboyKid](../../baseline/source/data/maps/FortreeCity/scripts.inc#L43) — FortreeCity_EventScript_GameboyKid at (9,16); There are POKéMON that evolve when /  you trade them! That's what I heard. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity:object_events:007` | 25,8 | [FortreeCity_EventScript_Kecleon](../../baseline/source/data/maps/FortreeCity/scripts.inc#L55) — FortreeCity_EventScript_Kecleon at (25,8); Something unseeable is in the way. / Something unseeable is in the way. //  Want to use the DEVON SCOPE? | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `FortreeCity:bg_events:001` | 6,9 | [FortreeCity_EventScript_CitySign](../../baseline/source/data/maps/FortreeCity/scripts.inc#L47) — FortreeCity_EventScript_CitySign at (6,9); FORTREE CITY /  “The treetop city that frolics with /  nature.” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `FortreeCity:bg_events:002` | 7,6 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (7,6); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `FortreeCity:bg_events:003` | 5,14 | [Common_EventScript_ShowPokemartSign](../../baseline/source/data/event_scripts.s#L1173) — Common_EventScript_ShowPokemartSign at (5,14); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `FortreeCity:bg_events:004` | 26,10 | [FortreeCity_EventScript_GymSign](../../baseline/source/data/maps/FortreeCity/scripts.inc#L51) — FortreeCity_EventScript_GymSign at (26,10); FORTREE CITY POKéMON GYM /  LEADER: WINONA //  “The bird user taking flight into /  the world.” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `FortreeCity:bg_events:005` | 6,6 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (6,6); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `FortreeCity:bg_events:006` | 6,14 | [Common_EventScript_ShowPokemartSign](../../baseline/source/data/event_scripts.s#L1173) — Common_EventScript_ShowPokemartSign at (6,14); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `FortreeCity:warp_events:001` | 5,6 | Warp from (5,6, elevation 0) to MAP_FORTREE_CITY_POKEMON_CENTER_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity:warp_events:002` | 10,3 | Warp from (10,3, elevation 0) to MAP_FORTREE_CITY_HOUSE1 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity:warp_events:003` | 22,11 | Warp from (22,11, elevation 0) to MAP_FORTREE_CITY_GYM warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity:warp_events:004` | 4,14 | Warp from (4,14, elevation 0) to MAP_FORTREE_CITY_MART warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity:warp_events:005` | 17,3 | Warp from (17,3, elevation 0) to MAP_FORTREE_CITY_HOUSE2 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity:warp_events:006` | 25,3 | Warp from (25,3, elevation 0) to MAP_FORTREE_CITY_HOUSE3 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity:warp_events:007` | 32,2 | Warp from (32,2, elevation 0) to MAP_FORTREE_CITY_HOUSE4 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity:warp_events:008` | 12,13 | Warp from (12,13, elevation 0) to MAP_FORTREE_CITY_HOUSE5 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity:warp_events:009` | 37,13 | Warp from (37,13, elevation 0) to MAP_FORTREE_CITY_DECORATION_SHOP warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity:connections:001` | left | left connection to MAP_ROUTE119, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity:connections:002` | right | right connection to MAP_ROUTE120, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [FortreeCity_OnTransition](../../baseline/source/data/maps/FortreeCity/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls FortreeCity_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `FortreeCity:map_scripts:002` | MAP_SCRIPT_ON_RESUME | [FortreeCity_OnResume](../../baseline/source/data/maps/FortreeCity/scripts.inc#L10) — MAP_SCRIPT_ON_RESUME calls FortreeCity_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
