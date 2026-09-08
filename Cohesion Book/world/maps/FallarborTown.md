# FallarborTown

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/FallarborTown/map.json) · [Scripts](../../baseline/source/data/maps/FallarborTown/scripts.inc)

## Current map contract

`MAP_FALLARBOR_TOWN` · `LAYOUT_FALLARBOR_TOWN` · `WEATHER_SUNNY` · `MUS_FALLARBOR`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `FallarborTown:object_events:001` | 8,11 | [FallarborTown_EventScript_Girl](../../baseline/source/data/maps/FallarborTown/scripts.inc#L40) — FallarborTown_EventScript_Girl at (8,11); See! Take a look! /  This is my precious AZURILL! //  It's slick and smooth and plushy, too! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FallarborTown:object_events:002` | 11,9 | [FallarborTown_EventScript_ExpertM](../../baseline/source/data/maps/FallarborTown/scripts.inc#L27) — FallarborTown_EventScript_ExpertM at (11,9); Something's happening, /  and I don't like it! //  I've seen shady characters wandering /  in and out of PROF. COZMO's home… / This region's been known for meteors /  since the olden days. //  They say METEOR FALLS was gouged out /  by a falling meteorite long ago. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `FallarborTown:object_events:003` | 11,15 | [FallarborTown_EventScript_Gentleman](../../baseline/source/data/maps/FallarborTown/scripts.inc#L44) — FallarborTown_EventScript_Gentleman at (11,15); Have you already challenged FLANNERY, /  the LEADER of LAVARIDGE GYM? //  The girl's grandfather was famous. /  He was one of the ELITE FOUR in the /  POKéMON LEAGUE at one point. //  It wouldn't surprise me to see FLANNERY /  become a great TRAINER in her own /  right. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FallarborTown:object_events:004` | 8,12 | [FallarborTown_EventScript_Azurill](../../baseline/source/data/maps/FallarborTown/scripts.inc#L48) — FallarborTown_EventScript_Azurill at (8,12); AZURILL: Rooreelooo. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FallarborTown:object_events:005` | 6,10 | Passive/staged OBJ_EVENT_GFX_VAR_0 at (6,10); visibility flag FLAG_HIDE_FALLARBOR_RIVAL; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `FallarborTown:object_events:006` | 6,10 | Passive/staged OBJ_EVENT_GFX_VAR_3 at (6,10); visibility flag FLAG_HIDE_FALLARBOR_RIVAL_ON_BIKE; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `FallarborTown:coord_events:001` | 13,8 | [FallarborTown_EventScript_RivalTrigger1](../../baseline/source/data/maps/FallarborTown/scripts.inc#L70) — Coordinate trigger at (13,8); VAR_FALLARBOR_TOWN_STATE == 0 invokes FallarborTown_EventScript_RivalTrigger1. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `FallarborTown:coord_events:002` | 13,9 | [FallarborTown_EventScript_RivalTrigger2](../../baseline/source/data/maps/FallarborTown/scripts.inc#L75) — Coordinate trigger at (13,9); VAR_FALLARBOR_TOWN_STATE == 0 invokes FallarborTown_EventScript_RivalTrigger2. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `FallarborTown:coord_events:003` | 13,10 | [FallarborTown_EventScript_RivalTrigger3](../../baseline/source/data/maps/FallarborTown/scripts.inc#L80) — Coordinate trigger at (13,10); VAR_FALLARBOR_TOWN_STATE == 0 invokes FallarborTown_EventScript_RivalTrigger3. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `FallarborTown:coord_events:004` | 13,11 | [FallarborTown_EventScript_RivalTrigger4](../../baseline/source/data/maps/FallarborTown/scripts.inc#L85) — Coordinate trigger at (13,11); VAR_FALLARBOR_TOWN_STATE == 0 invokes FallarborTown_EventScript_RivalTrigger4. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `FallarborTown:coord_events:005` | 13,12 | [FallarborTown_EventScript_RivalTrigger5](../../baseline/source/data/maps/FallarborTown/scripts.inc#L90) — Coordinate trigger at (13,12); VAR_FALLARBOR_TOWN_STATE == 0 invokes FallarborTown_EventScript_RivalTrigger5. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `FallarborTown:bg_events:001` | 16,15 | [Common_EventScript_ShowPokemartSign](../../baseline/source/data/event_scripts.s#L1173) — Common_EventScript_ShowPokemartSign at (16,15); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `FallarborTown:bg_events:002` | 15,7 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (15,7); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `FallarborTown:bg_events:003` | 6,8 | [FallarborTown_EventScript_BattleTentSign](../../baseline/source/data/maps/FallarborTown/scripts.inc#L58) — FallarborTown_EventScript_BattleTentSign at (6,8); BATTLE TENT FALLARBOR SITE /  “May the Greatest Teams Gather!” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `FallarborTown:bg_events:004` | 16,7 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (16,7); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `FallarborTown:bg_events:005` | 10,11 | [FallarborTown_EventScript_TownSign](../../baseline/source/data/maps/FallarborTown/scripts.inc#L62) — FallarborTown_EventScript_TownSign at (10,11); FALLARBOR TOWN /  “A farm community with small gardens.” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `FallarborTown:bg_events:006` | 17,15 | [Common_EventScript_ShowPokemartSign](../../baseline/source/data/event_scripts.s#L1173) — Common_EventScript_ShowPokemartSign at (17,15); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `FallarborTown:bg_events:007` | 3,7 | [FallarborTown_EventScript_MoveTutorSign](../../baseline/source/data/maps/FallarborTown/scripts.inc#L66) — FallarborTown_EventScript_MoveTutorSign at (3,7); HEART SCALE WORKSHOP /  “Scales polished into PP UPS.” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `FallarborTown:bg_events:008` | 2,15 | Hidden ITEM_NUGGET at (2,15); persistent flag FLAG_HIDDEN_ITEM_FALLARBOR_TOWN_NUGGET. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `FallarborTown:warp_events:001` | 15,15 | Warp from (15,15, elevation 0) to MAP_FALLARBOR_TOWN_MART warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FallarborTown:warp_events:002` | 8,7 | Warp from (8,7, elevation 0) to MAP_FALLARBOR_TOWN_BATTLE_TENT_LOBBY warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FallarborTown:warp_events:003` | 14,7 | Warp from (14,7, elevation 0) to MAP_FALLARBOR_TOWN_POKEMON_CENTER_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FallarborTown:warp_events:004` | 6,17 | Warp from (6,17, elevation 0) to MAP_FALLARBOR_TOWN_COZMOS_HOUSE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FallarborTown:warp_events:005` | 1,6 | Warp from (1,6, elevation 0) to MAP_FALLARBOR_TOWN_MOVE_RELEARNERS_HOUSE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FallarborTown:connections:001` | left | left connection to MAP_ROUTE114, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FallarborTown:connections:002` | right | right connection to MAP_ROUTE113, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FallarborTown:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [FallarborTown_OnTransition](../../baseline/source/data/maps/FallarborTown/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls FallarborTown_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
