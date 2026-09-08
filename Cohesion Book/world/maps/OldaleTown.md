# OldaleTown

**KEEP.** Preserve the connected first preparation hub. INTRO-01 changes the adjacent first battles; the Center remains available before trainer challenges. The footprint joke and optional Potion promotion can remain without an artificial learning phase.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/OldaleTown/map.json) · [Scripts](../../baseline/source/data/maps/OldaleTown/scripts.inc)

## Current map contract

`MAP_OLDALE_TOWN` · `LAYOUT_OLDALE_TOWN` · `WEATHER_SUNNY` · `MUS_OLDALE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `OldaleTown:object_events:001` | 16,11 | [OldaleTown_EventScript_Girl](../../baseline/source/data/maps/OldaleTown/scripts.inc#L32) — OldaleTown_EventScript_Girl at (16,11); I want to take a rest, so I'm saving my /  progress. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `OldaleTown:object_events:002` | 13,7 | [OldaleTown_EventScript_MartEmployee](../../baseline/source/data/maps/OldaleTown/scripts.inc#L36) — OldaleTown_EventScript_MartEmployee at (13,7); Hi! /  I work at a POKéMON MART. //  Can I get you to come with me? / This is a POKéMON MART. /  Just look for our blue roof. //  We sell a variety of goods including /  POKé BALLS for catching POKéMON. //  Here, I'd like you to have this as /  a promotional item. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `OldaleTown:object_events:003` | 8,9 | [OldaleTown_EventScript_FootprintsMan](../../baseline/source/data/maps/OldaleTown/scripts.inc#L190) — OldaleTown_EventScript_FootprintsMan at (8,9); I just discovered the footprints of /  a rare POKéMON! //  Wait until I finish sketching /  them, okay? / I finished sketching the footprints of /  a rare POKéMON. //  But it turns out they were only my /  own footprints… | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `OldaleTown:object_events:004` | 11,19 | [OldaleTown_EventScript_Rival](../../baseline/source/data/maps/OldaleTown/scripts.inc#L218) — OldaleTown_EventScript_Rival at (11,19); MAY: {PLAYER}{KUN}! /  Over here! /  Let's hurry home! / BRENDAN: I'm heading back to my dad's /  LAB now. /  {PLAYER}, you should hustle back, too. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `OldaleTown:coord_events:001` | 0,10 | [OldaleTown_EventScript_BlockedPath](../../baseline/source/data/maps/OldaleTown/scripts.inc#L201) — Coordinate trigger at (0,10); VAR_OLDALE_TOWN_STATE == 0 invokes OldaleTown_EventScript_BlockedPath. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `OldaleTown:coord_events:002` | 8,19 | [OldaleTown_EventScript_RivalTrigger1](../../baseline/source/data/maps/OldaleTown/scripts.inc#L226) — Coordinate trigger at (8,19); VAR_OLDALE_RIVAL_STATE == 1 invokes OldaleTown_EventScript_RivalTrigger1. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `OldaleTown:coord_events:003` | 9,19 | [OldaleTown_EventScript_RivalTrigger2](../../baseline/source/data/maps/OldaleTown/scripts.inc#L236) — Coordinate trigger at (9,19); VAR_OLDALE_RIVAL_STATE == 1 invokes OldaleTown_EventScript_RivalTrigger2. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `OldaleTown:coord_events:004` | 10,19 | [OldaleTown_EventScript_RivalTrigger3](../../baseline/source/data/maps/OldaleTown/scripts.inc#L246) — Coordinate trigger at (10,19); VAR_OLDALE_RIVAL_STATE == 1 invokes OldaleTown_EventScript_RivalTrigger3. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `OldaleTown:bg_events:001` | 11,9 | [OldaleTown_EventScript_TownSign](../../baseline/source/data/maps/OldaleTown/scripts.inc#L28) — OldaleTown_EventScript_TownSign at (11,9); OLDALE TOWN /  “Where things start off scarce.” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `OldaleTown:bg_events:002` | 7,16 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (7,16); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `OldaleTown:bg_events:003` | 15,6 | [Common_EventScript_ShowPokemartSign](../../baseline/source/data/event_scripts.s#L1173) — Common_EventScript_ShowPokemartSign at (15,6); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `OldaleTown:bg_events:004` | 8,16 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (8,16); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `OldaleTown:bg_events:005` | 16,6 | [Common_EventScript_ShowPokemartSign](../../baseline/source/data/event_scripts.s#L1173) — Common_EventScript_ShowPokemartSign at (16,6); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `OldaleTown:warp_events:001` | 5,7 | Warp from (5,7, elevation 0) to MAP_OLDALE_TOWN_HOUSE1 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `OldaleTown:warp_events:002` | 15,16 | Warp from (15,16, elevation 0) to MAP_OLDALE_TOWN_HOUSE2 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `OldaleTown:warp_events:003` | 6,16 | Warp from (6,16, elevation 0) to MAP_OLDALE_TOWN_POKEMON_CENTER_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `OldaleTown:warp_events:004` | 14,6 | Warp from (14,6, elevation 0) to MAP_OLDALE_TOWN_MART warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `OldaleTown:connections:001` | up | up connection to MAP_ROUTE103, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `OldaleTown:connections:002` | down | down connection to MAP_ROUTE101, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `OldaleTown:connections:003` | left | left connection to MAP_ROUTE102, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `OldaleTown:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [OldaleTown_OnTransition](../../baseline/source/data/maps/OldaleTown/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls OldaleTown_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
